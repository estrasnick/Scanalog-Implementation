/* IMPORTS */
var net = require('net');
var bs = require('./node_modules/bitscope-js/build/Debug/bitscope-js');
var remote = require('electron').remote;
var SerialPort = require('serialport');
var fs = require('fs');

/* DEFINES */
var probeId = 'io3';
var originalDefaultNewModulePosition = { x: 200, y: 150}; // Where newly added modules first appear
var defaultNewModulePosition = Object.assign({}, originalDefaultNewModulePosition);
var maxModulePosition = {x: 400, y: 350};
var incrementNewModulePosition = 50; // Newly added modules after the first will shift by this amount in both x and y
var initialCaptureDelay = 1000;
var betweenCapturesDelay = 500; // This value acts as a timeout for attempting to restart capture. Don't decrease it too much, or captures will fail
var beforeCaptureDelay = 150;

var defaultSamplingRate = 1000000; // Default oscilloscope parameters
var defaultSampleSize = 1024;
var numGeneratorSamples = 1024;

var maxVoltage = 2.55; // Set based on actual rail to rail voltage of board while running
var minVoltage = -2.55;
var noSignalSpread = .2;

var DEBUG_WITHOUT_HARDWARE = false; // Used to disable hardware calls during debugging. Will render system largely unusable.

/* GLOBALS */
var port = new SerialPort('COM5', { // generator COM port
  autoOpen: false,
  baudRate: 9600
});

var assertions = [];
var nameSuffix = 0;
var respondingToCommand = false;
var buf = [];
var plotlyModules = [];
var plotlyModulesIndex = 0;
var highlightedModule = null;
var moduleDict = {};

var samplingRate = defaultSamplingRate;
var sampleSize = defaultSampleSize;
var autoscan = true;
var pausecapture = false;
var savedProbe = null;
var ongoingCapture = false;
var replaying = false;

var probemodule = null;
var isValid = false;
var recordingForCheckers = false;
var recordingForUnitTests = false;
var manualRecordingSeries = null;
var pauseOscilloscope = false;

var captureTimeout;
var commandHistory = [];
var commandHistoryCounter = 0;
var commandHistoryIndex = 0;

var savedRecordings = [];
var currentRecording = 0;
var unitTestFailures = [];

var mainscopelayout = {
    autosize: false,
    width: 360,
    height: 142,
    showlegend: false,
    xaxis: {
      titlefont: {
          color: '#ffff00',
      },
      autorange: true,
      showgrid: false,
      zeroline: false,
      showline: false,
      autotick: false,
      ticks: '',
      showticklabels: false
    },
    yaxis: {
      title: 'V',
      titlefont: {
          color: '#ffff00',
      },
      autorange: false,
      range: [-4, 4],
      showgrid: true,
      zeroline: true,
      showline: true,
      linecolor: '#e0e000',
      zerolinecolor: 'rgba(150,150,0,.4)',
      autotick: false,
      ticks: 'outside',
      tickcolor: '#e0e000',
      tickwidth: 2,
        ticklen: 5,
        tickfont: {
          family: 'Arial',
          size: 12,
          color: 'rgb(224, 224, 0)'
      },
      tick0: -4,
      dtick: 1,
      showticklabels: true
      },
      margin: {
          l: 30,
          r: 10,
          b: 28,
          t: 10,
          pad: 0
      },
      plot_bgcolor: 'rgba(50,50,50,.4)',
      paper_bgcolor: 'rgba(50,50,50,.4)'
};

var layout = {
    autosize: false,
    width: 100,
    height:40,
    showlegend: false,
    xaxis: {
      autorange: true,
      showgrid: false,
      zeroline: false,
      showline: false,
      autotick: true,
      ticks: '',
      showticklabels: false
    },
    yaxis: {
      autorange: false,
      range: [-4, 4],
      showgrid: false,
      zeroline: true,
      showline: false,
      autotick: true,
      ticks: '',
      showticklabels: false
      },
      margin: {
          l: 0,
          r: 0,
          b: 0,
          t: 0,
          pad: 0
      },
      plot_bgcolor: '#000000',
      paper_bgcolor: '#000000'
  };

/* ------- */

if (!DEBUG_WITHOUT_HARDWARE)
{
    // create scope object
    var bitscope = new bs.Bitscope();

    // open generator
    port.open(function (err) {
      if (err) {
        return console.log('Error opening generator port: ', err.message);
      }
    });

    function setBitscopeOptions()
    {
        bitscope.setOptions({
          "device" : 0,
          "mode"   : 0, // BL_MODE_FAST
          "size"   : parseInt(sampleSize),
          "rate"   : parseInt(samplingRate),
          "range"  : 3,
          "offset": 0.0,

          "channels" : [0],

          "trace_timeout"   : 0.005,
          "trigger_channel" : 0,
          "trigger_token"   : 0,
          "trigger_level"   : 0.0,
          "trigger_enable"  : true,

          "intro": 0,
          "delay": 0
        });
    }
    setBitscopeOptions();

    function doNextCapture()
    {
        if (autoscan || recordingForCheckers || recordingForUnitTests)
        {
            readyCapture(); // perform autoscan capture
        }
        else
        {
            manualCapture(); // perform manual capture
        }
    }

    // perform (autoscan) capture and update the corresponding plot
    function capture(module)
    {
        clearTimeout(captureTimeout);
        isValid = true;
        ongoingCapture = true;
        bitscope.capture(function() {
            if (isValid)
            {
                var buffers = bitscope.getBuffers(); // retrieve the trace

                // create new plot
                var series = [];
                var times = [];
                for (var i = 0; i < buffers[0].length; i++)
                {
                    series.push(buffers[0][i]);
                    times.push(i / samplingRate);
                }

                var div = module.get('plotcanvas');

                if ((module.get('visualize') == 'true') && autoscan)
                {
                    div.data = [];
                    Plotly.addTraces(div, {x: times, y: series, mode: 'lines',
                      line: {
                        color: '#12d400',
                        width: 2}});
                }

                module.set('recordedData', series); // store latest recording as module output

                runTestsForModule(module, series); // run any assertions

                plotlyModulesIndex++;
                if (plotlyModulesIndex >= plotlyModules.length)
                {
                    plotlyModulesIndex = 0;
                    if (recordingForCheckers)
                    {
                        recordingForCheckers = false;
                        runCheckers();
                        if (savedProbe != null)
                        {
                            respondingToCommand = true;
                            writeSocket("probe " + savedProbe.get('modulename'));
                            handleCommand("probe " + savedProbe.get('modulename'));
                            respondingToCommand = false;
                            savedProbe = null;
                        }
                    }

                    if (recordingForUnitTests)
                    {
                        recordingForUnitTests = false;
                        runUnitTest();
                        if (savedProbe != null)
                        {
                            respondingToCommand = true;
                            writeSocket("probe " + savedProbe.get('modulename'));
                            handleCommand("probe " + savedProbe.get('modulename'));
                            respondingToCommand = false;
                            savedProbe = null;
                        }
                    }
                }
            }
            ongoingCapture = false;
            doNextCapture();
          });
    }

    // perform a manual capture and update the corresponding plot
    function manualCapture()
    {
        isValid = true;
        clearTimeout(captureTimeout);
        ongoingCapture = true;
        bitscope.capture(function() {
            if (isValid)
            {
                var buffers = bitscope.getBuffers(); // retrieve the trace

                // create new plot
                var series = [];
                var times = [];
                for (var i = 0; i < buffers[0].length; i++)
                {
                    series.push(buffers[0][i]);
                    times.push(i / samplingRate);
                }
                if (!autoscan)
                {
                    mainscope.data = [];

                    Plotly.addTraces(mainscope, {x: times, y: series, mode: 'lines',
                      line: {
                        color: '#12d400',
                        width: 2}});
                }
                manualRecordingSeries = series;

                // run unit tests on the module, if any
                var links = graph.getConnectedLinks(probemodule, {inbound: true});
                if (links.length > 0 && (links[0].getSourceElement() != null))
                {
                    runTestsForModule(links[0].getSourceElement(), series);
                }

            }
            ongoingCapture = false;
            if (!pauseOscilloscope)
            {
                doNextCapture();
            }
        });
    }

    // trigger a hardware reconfiguration prior to recording a trace
    function readyCapture() {
        if (autoscan || recordingForCheckers || recordingForUnitTests)
        {
            if (plotlyModules.length > 0)
            {
                writeSocket("probe " + plotlyModules[plotlyModulesIndex].get('modulename'));
            }
            captureTimeout = setTimeout(readyCapture, beforeCaptureDelay+betweenCapturesDelay);
        }
    }

    captureTimeout = setTimeout(readyCapture, initialCaptureDelay);
}

// Configure Joint.js interface
var graph = new joint.dia.Graph();
var paper = new joint.dia.Paper({

    el: $('#paper'),
    model: graph,
    width: 1316, height: 485, gridSize: 5,
    snapLinks: false,
    linkPinning: false,
    defaultLink: new joint.shapes.logic.Wire,
    markAvailable: true,

    validateConnection: function(vs, ms, vt, mt, e, vl) {

        if (e === 'target') {

            // target requires an input port to connect
            if (!mt || !mt.getAttribute('class') || mt.getAttribute('class').indexOf('input') < 0) return false;

            // check whether the port is being already used
            var portUsed = _.find(this.model.getLinks(), function(link) {

                return (link.id !== vl.model.id &&
                        link.get('target').id === vt.model.id &&
                        link.get('target').port === mt.getAttribute('port'));
            });

            return !portUsed;

        } else { // e === 'source'

            // source requires an output port to connect
            return ms && ms.getAttribute('class') && ms.getAttribute('class').indexOf('output') >= 0;
        }
    }
});

// zoom the viewport
paper.scale(1.0,1.0);

/*     SOCKET COMMUNICATION WITH HOST APPLICATION    */
var socket = new net.Socket();
socket.connect(1442, '127.0.0.1', function() {
    console.log('Socket connection open!');
    //writeSocket('reset');   -- don't do this immediately. the circuit isn't set up yet
});
socket.on('data', function(data) {
	//console.log('Server: ' + data);
    respondingToCommand = true;
    handleCommand(parseSocketData(data));
    respondingToCommand = false;
});
socket.on('close', function() {
	console.log('Connection closed');
});


// Improvement needed: We should enque multiple SET requests and only send the most recent
socket.on('drain', function() {
    // If there's more data in the buffer, send it
    if (buf.length > 0)
    {
        socket.write(buf.shift());
    }
});

socket.on('end', function () {
    console.log('fin');
});

var addassertionbutton = $("#addassertionbutton");
addassertionbutton.on('click', function(e) {
    showAssertionDialog();
});

var runcheckersbutton = $("#runcheckersbutton");
runcheckersbutton.on('click', function(e) {
    respondingToCommand = true;
    handleCommand("check");
    respondingToCommand = false;
});

var rununittestsbutton = $("#rununittestsbutton");
rununittestsbutton.on('click', function(e) {
    respondingToCommand = true;
    handleCommand("rununittests");
    respondingToCommand = false;
});

var recordbutton = $("#recordbutton");
recordbutton.on('click', function(e) {
    respondingToCommand = true;
    handleCommand("record");
    respondingToCommand = false;
    resumebutton.show();
    replaybutton.show();
    $(this).hide();
});
recordbutton.hide();

var resumebutton = $("#resumebutton");
resumebutton.on('click', function(e) {
    respondingToCommand = true;
    handleCommand("resume");
    respondingToCommand = false;
    recordbutton.show();
    $(this).hide();
});
resumebutton.hide();

var replaybutton = $("#replaybutton");
replaybutton.on('click', function(e) {
    showReplaySelectionDialog();
});
replaybutton.hide();

var autoscanbox = $("#autoscan_box");
autoscanbox.on('click', function(e) {
    if (this.checked)
    {
        // store previously probed module
        var links = graph.getConnectedLinks(probemodule, {inbound: true});
        if (links.length > 0)
        {
            savedProbe = links[0].getSourceElement();
        }

        autoscan = true;

        // dehighlight probe and manual scope
        Plotly.update (mainscope, {line: {color: 'rgba(18,212,0,.2)', width: 2}}, {plot_bgcolor: 'rgba(50,50,50,.4)', paper_bgcolor: 'rgba(50,50,50,.4)'});
        probemodule.attr({'.body': { fill: 'rgba(50,50,50,.2)', stroke: 'rgba(50,50,50,.4)'},
        '.wire': { stroke: 'rgba(50,50,50,.4)' },
        text: {fill: 'rgba(50,50,50,.4)'}});

        if (pauseOscilloscope)
        {
            respondingToCommand = true;
            handleCommand("resume");
            respondingToCommand = false;
        }
        recordbutton.hide();
        resumebutton.hide();
    }
    else
    {
        autoscan = false;
        for (var i = 0; i < plotlyModules.length; i++)
        {
            Plotly.restyle(plotlyModules[i].get('plotcanvas'), {opacity: .35});
        }

        if (savedProbe != null)
        {
            respondingToCommand = true;
            writeSocket("probe " + savedProbe.get('modulename'));
            handleCommand("probe " + savedProbe.get('modulename'));
            respondingToCommand = false;
            savedProbe = null;
        }
        else
        {
            writeSocket("unprobe");
        }

        Plotly.update(mainscope, {line: {color: '#12d400', width: 2}}, {plot_bgcolor: '#000000', paper_bgcolor: '#000000'});
        probemodule.attr({'.body': { fill: '#c1c1c1', stroke: 'black'},
        '.wire': { stroke: 'black' },
        text: {fill: 'black'}});

        calculateWindowSize();
        recordbutton.show();
    }
});

samplingRateInput = $("#samplingRate");
samplingRateInput.on('input change', function(e) {
    if (this.value > 0)
    {
        isValid = false;
        samplingRate = this.value;
        setBitscopeOptions();
        calculateWindowSize();
    }
});

sampleSizeInput = $("#sampleSize");
sampleSizeInput.on('input change', function(e) {
    if (this.value > 0)
    {
        isValid = false;
        sampleSize = this.value;
        setBitscopeOptions();
        calculateWindowSize();
    }
});

// update displayed window size
function calculateWindowSize()
{
    var windowSize = sampleSize / samplingRate;
    var unit = ' s';
    if (windowSize < 1)
    {
        windowSize *= 1000;
        unit = ' ms';
    }
    if (windowSize < 1)
    {
        windowSize *= 1000;
        unit = ' us';
    }
    Plotly.relayout(mainscope, {
        xaxis: {
            title: windowSize + unit,
            titlefont: {
                color: '#ffff00',
                size: 11
            },
            tickfont: {
              size: 10,
              color: '#999900'
            },
            autorange: true,
            showgrid: false,
            zeroline: false,
            showline: false,
            autotick: true,
        }
    });
}

// display menu for setting generator output
function showReplaySelectionDialog()
{
    var newdialog = "<div class='dialog' title='Generate Signal'><div class='savedsignaldiv' id='savedsignaldiv'>";

    for (var i = 0; i < savedRecordings.length; i++)
    {
        newdialog += '<div class="replaycanvas" id="plotcanvas_';
        newdialog += i;
        newdialog += '"></div>';
    }

    newdialog += "</div></div>";

    $(newdialog).dialog({
        dialogClass: "no-close createreplayselectiondialog",
        buttons: [
        {
          text: "Cancel",
          icons: {
            primary: "ui-icon-heart"
          },
          click: function() {
            //$( this ).dialog( "close" );
            $(".ui-dialog-content").dialog("destroy");
          }

        }
      ]
    });

    // render each recorded signal
    for (var i = 0; i < savedRecordings.length; i++)
    {
        var plotdiv = document.getElementById('plotcanvas_' + i);
        Plotly.newPlot(plotdiv, [{
            mode: 'lines',
              line: {
                color: '#00ff00',
                width: 2
            }}], layout, {staticPlot: true});

        var times = [];

        for (var j = 0; j < savedRecordings[i].length; j++)
        {
            times.push(j / samplingRate);
        }
        Plotly.addTraces(plotdiv, {x: times, y: savedRecordings[i], mode: 'lines',
          line: {
            color: '#12d400',
            width: 2}});
        $(plotdiv).on('click', {command: "replay " + i}, function(e) {
            respondingToCommand = true;
            handleCommand(e.data.command);
            respondingToCommand = false;
            $(".ui-dialog-content").dialog("destroy");
        });
    }
    $('#savedsignaldiv').jScrollPane();
}

function replaySignal(whichSignal)
{
    programGeneratorOptions(port, null, whichSignal);
}

function endReplay()
{
    port.write('[46]@[01]s[47]@[00]sZ'); // stop)
    port.flush();
}

function programGeneratorOptions(p, cb, whichSignal)
{
    var writeCommand = '';
    writeCommand += '[7c]@[40]sU'; // enable generator output

    writeCommand += '[46]@[00]s[47]@[01]s'; // command, mode (8-bit write, wavetable target)
    writeCommand += '[4a]@[01]s[4b]@[00]s'; // write operation size = 1 sample
    writeCommand += '[4e]@[00]s[4f]@[00]s'; // destination address = wavetable start

    var datapoints = [];
    var divisor = (savedRecordings[whichSignal].length + 0.0) / numGeneratorSamples;
    var length = 1024 // Improve: Math.min(numGeneratorSamples, recordedSignal.length);
    var index = 0;
    var scalefactor = 256 / (maxVoltage - minVoltage);
    var offset = 128;
    for (var i = 0; i < length; i++)
    {
        datapoints.push(Math.floor(scalefactor *  Math.min(maxVoltage, Math.max(minVoltage, savedRecordings[whichSignal][Math.floor(index)])) + offset));
        index += divisor;
    }

    for (var i = 0; i < datapoints.length; i++)
    {
        writeCommand += '[' + (datapoints[i]).toString('16') + ']W';
    }

    writeCommand += '[4a]@[00]s[4b]@[04]s'; // operation size = 1024
    writeCommand += '[4c]@[00]s[4d]@[00]s'; // operation index = table start
    writeCommand += '[4e]@[00]s[4f]@[00]s'; // destination address = buffer start
    writeCommand += '[54]@[ff]s[55]@[ff]s'; // output level = full scale
    writeCommand += '[56]@[f0]s[57]@[f0]s'; // offset = zero
    //writeCommand += '[5a]@[01]s[5b]@[01]s[5c]@[01]s[5d]@[01]s'; // phase ratio
    writeCommand += '[5a]@[93]s[5b]@[18]s[5c]@[04]s[5d]@[00]s';
    writeCommand += '[46]@[00]s[47]@[00]sX'; // translate

    // Our recording is sampleSize samples at samplingRate Frequency
    // We are scaling the recording to 1024 samples (often downsampling), so we
    //      need our frequency to be samplingRate * (1024 / sampleSize)

    // Say we have divisor = 1 (that is, sampleSize = 1024)
    // Then, for a samplingRate of 1 MHz, we want to output at a frequency of 1 MHz.
    var newFreq = samplingRate / divisor;
    // With 40 ticks per period, the entire buffer cycles at 4 KHz.
    // That is, we have an output "sampling rate" of 4096000 Hz.
    // With 64 ticks per period, the buffer cycles at 2.5 KHz.
    // That is, we have an output "sampling rate" of 2560000 Hz.
    // That is, we have an output "sampling rate" of
    // The value X that goes in register 50 should be chosen s.t.
    // newFreq = 4096000 * (40 / X)

    // In our example, we wanted 1 MHz, so X = ~164
    //var x = Math.round(Math.min(256, Math.max(40, (163840000 / newFreq))));
    var x = Math.round(Math.min(65535, Math.max(40, (163840000 / newFreq))));
    console.log("X:" + x);
    var xHexString = x.toString('16');
    while (xHexString.length < 4)
    {
        xHexString = '0' + xHexString;
    }

    // The master clock runs at 400 KHz with this phase ratio (40 ticks per period = 10 KHz)
    // but we cannot go faster than 10 KHz (generator limitation?).
    // for now, we'll output at 10 KHz
    writeCommand += '[48]@[04]s[49]@[80]s'; // control flags
    writeCommand += '[50]@[' + xHexString.substring(2, 4) +  ']s[51]@[' + xHexString.substring(0, 2) + ']s'; // sample clock ticks - 40 tick period = 10KHz (so x28 should be in [50])
    //writeCommand += '[50]@[ff]s[51]@[11]s';
    writeCommand += '[52]@[00]s[53]@[04]s'; // table modulo size = 1024
    writeCommand += '[5e]@[0a]s[5f]@[00]s'; // mark count/phase
    writeCommand += '[60]@[01]s[61]@[00]s'; // space count/phase
    writeCommand += '[78]@[00]s[79]@[7f]s'; // DAC level


    writeCommand += '[46]@[02]s[47]@[00]sZ'; // generate

    p.write(writeCommand);
    p.flush();
}

function programRecord(p, cb)
{

    var writeCommand = '';

    writeCommand += '[7b]@[80]s'; // KitchenSinkA (enable hardware comparators)
    writeCommand += '[7c]@[80]s'; // KitchenSinkB (enable analog filter)
    writeCommand += '[fb]@[00]s'; // LedLevelGRN (turn CHB LED off, VM10 only)
    writeCommand += '[fc]@[c0]s'; // LedLevelYEL (turn CHA LED on, VM10 only)
    writeCommand += '[37]@[01]s'; // AnalogAnable (enable CHA input circuits)
    writeCommand += '[64]@[28]s[65]@[1c]s'; // ConvertorLo (set convertor range, low side)
    writeCommand += '[66]@[c1]s[67]@[b5]s'; // ConvertorHi (set convertor range, high side)
    writeCommand += '[14]@[01]s[15]@[00]s'; // ClockScale (set clock prescaler)
    writeCommand += '[2e]@[28]s[2f]@[00]s'; // ClockTicks (set clock divider)
    writeCommand += '[31]@[00]s'; // BufferMode (choose the capture buffer mode)
    writeCommand += '[21]@[00]s'; // TraceMode (choose the trace mode)
    writeCommand += '[22]@[00]s[23]@[00]s[24]@[00]s[25]@[00]s'; // TraceDelay (set post trigger delay)
    writeCommand += '[26]@[80]s[27]@[00]s'; // TraceIntro (set pre-trigger capture count)
    writeCommand += '[2a]@[e0]s[2b]@[06]s'; // TraceOutro (set post-trigger capture count)
    writeCommand += '[06]@[7f]s'; // TriggerMask (set the trigger logic mask)
    writeCommand += '[05]@[80]s'; // TriggerLogic (program the trigger logic)
    writeCommand += '[32]@[04]s[33]@[00]s'; // TriggerIntro (set trigger hold-off filter duration)
    writeCommand += '[34]@[04]s[35]@[00]s'; // TriggerOutro (set trigger hold-on filter duration)
    writeCommand += '[44]@[00]s[45]@[00]s'; // TriggerValue (set digital trigger level, optional)
    writeCommand += '[68]@[f5]s[69]@[68]s'; // TriggerLevel (set analog trigger level)
    writeCommand += '[07]@[20]s'; // SpockOption (choose edge triggered comparator mode)
    writeCommand += '[2c]@[00]s[2d]@[00]s'; // Timeout (specify a timeout, “forever” in this case)
    writeCommand += '[3a]@[00]s[3b]@[00]s'; // Prelude (set the buffer default value; “zero”)
    writeCommand += '[08]@[00]s[09]@[00]s[0a]@[00]s'; // SampleAddress (assign the trace start address)
    writeCommand += '>'; // (program capture hardware registers)
    writeCommand += 'U'; // (programming other hardware registers)
    writeCommand += 'D'; // commence the trace!

    p.flush(cb);
}

// Use jScollPane to set scrolling behavior on test buttons
$('.testbuttons').jScrollPane();

/* Command Line Setup */
var commandline = $('#commandline');
commandline.on('keydown', function (e) {
    if (e.which == 13) // enter
    {
        var command = $(this).val();
        if (command != "")
        {
            commandHistory[commandHistoryCounter++] = command;
            commandHistoryIndex = commandHistoryCounter;
            respondingToCommand = true;
            writeSocket(command);
            handleCommand(command);
            respondingToCommand = false;
            $(this).val(''); // clear box
        }
    }
    else if (e.which == 38) // up
    {
        if (commandHistoryIndex > 0)
        {
            $(this).val(commandHistory[--commandHistoryIndex]);
        }
    }
    else if (e.which == 40) // down
    {
        if (commandHistoryIndex < commandHistoryCounter)
        {
            $(this).val(commandHistory[++commandHistoryIndex]);
        }
        else
        {
            $(this).val('');
        }
    }
});

var mainscope = document.getElementById('mainscope');
Plotly.newPlot(mainscope, [{
    mode: 'lines',
      line: {
        color: '#00ff00',
        width: 2
    }}], mainscopelayout, {staticPlot: true});

function writeSocket(data) {
    data = data.concat(" "); // make sure we add whitespace, just in case
    // To prevent flooding the server with messages, we allow the buffer to flush
    // after each command before sending the next
    if (socket.bufferSize > 0)
    {
        buf.push(data + " "); // this will drain when ready
    }
    socket.write(data + " "); // otherwise, if free, just write
}

// Create a new visualization window for an added module
function createPlot(module)
{
    module.set('visualize', 'true');

    var div = module.findView(paper).$box[0];
    Plotly.newPlot(div, [{
        mode: 'lines',
          line: {
            color: '#00ff00',
            width: 2
        }}], layout, {staticPlot: true});

    module.set('plotcanvas', div);

    plotlyModules.push(module);
}

function deletePlot(m)
{
    m.set('visualize', 'false');
    removeFromArray(plotlyModules, m);
    Plotly.newPlot(m.get('plotcanvas'), [], layout, {staticPlot: true});
}

function pausePlot(m)
{
    m.set('visualize', 'false');
    Plotly.restyle(m.get('plotcanvas'), {opacity: .35});
    removeFromArray(plotlyModules, m);
}

// Run assertions on the latest trace for a given module
function runTestsForModule(m, data)
{
    var tests = m.get('assertions');
    for (var i = 0; i < tests.length; i++)
    {
        if (tests[i].get('testfunction')(m, data))
        {
            tests[i].attr('.body/fill', '#e36f6f');
        }
        else
        {
            tests[i].attr('.body/fill', '#a2f089');
        }
    }
}

function setAssertionList()
{
    newlist = '';
    for (var i = 0; i < assertions.length; i++)
    {
        newlist += "<button class='chooseassertionbutton controlbutton' onclick=\"addAssertionToGraph(\'";
        newlist += assertions[i].testname;
        newlist += "\');\">";
        newlist += assertions[i].testname;
        newlist += "</button>";
    }

    newlist += "<button class='customassertionbutton controlbutton' onclick='createCustomAssertion();'>(Add custom assertion)</button>";
    var scrolldata = $('.testbuttons').data('jsp');
    $('#assertionlist').html(newlist);

	// Reinitialise the #pane2 scrollpane
	scrolldata.reinitialise();
}

function addAssertionToGraph(name)
{
    var whichtest = null;
    for (var i = 0; i < assertions.length; i++)
    {
        if (assertions[i].testname == name)
        {
            whichtest = assertions[i];
            break;
        }
    }

    if (whichtest == null)
    {
        console.log("Could not find an assertion named: " + name);
    }

    var newtest = new joint.shapes.logic.Assertion({ position: defaultNewModulePosition});

    newtest.attr({text: {text: whichtest.testname}});
    newtest.set('testname', whichtest.testname);
    newtest.set('testfunction', whichtest.testfunction);
    newtest.set('explanation', whichtest.explanation);
    newtest.addTo(graph);
    appendAssertionDelete(newtest);
    updateNewModulePosition();
}

function createCustomAssertion()
{
    var newdialog = "<div class='dialog' title='Create Custom Assertion'>";

    newdialog += "<form>";
    newdialog += "Enter a name for the test: <input type='text' id='newtestname' name='newtestname' class='customAssertionTextInput'><br><br>";
    newdialog += "Enter an explanation for the test, to be displayed when tests fail: <input type='text' id='newtestexplanation' name='newtestexplanation' class='customAssertionTextInput'><br><br>";
    newdialog += "Enter the function body (in Javascript/jQuery) for the test. The function will be called with two arguments: 'm' - the module being tested, and 'data' - an array of doubles representing the most recent recording. Return true to alert when the test fails. The example below reports failure when the data contains voltage exceeding the operating voltage of the hardware: <textarea id='newtestfunction' name='newtestfunction' class='customAssertionTextInput' style='height: 300px; width: 95%'>";
    var exampleString = check_VoltageExceeded.toString();
    newdialog += exampleString.slice(exampleString.indexOf("{") + 1, exampleString.lastIndexOf("}"));
    newdialog += "</textarea>";
    newdialog += "</form><br>";


    newdialog += "<br></div>";

    $(newdialog).dialog({
        dialogClass: "no-close createcustomassertiondialog",
        buttons: [
        {
          text: "Cancel",
          icons: {
            primary: "ui-icon-heart"
          },
          click: function() {
            $( this ).dialog( "close" );
          }
      },
      {
        text: "Create test",
        icons: {
          primary: "ui-icon-heart"
        },
        click: function() {
            assertions.push(
                {
                    testname: $('#newtestname').val(),
                    explanation: $('#newtestexplanation').val(),
                    testfunction: new Function('m', 'data', $('#newtestfunction').val())
                }
            );
            setAssertionList();
            addAssertionToGraph($('#newtestname').val());
          $(".ui-dialog-content").dialog("destroy");
        }
      }]
    });
}

// Iterates through all recorded signals, generates them,
// and monitors each active assertion on each module as the tests run.
// Then, reports the results
function runUnitTests()
{
    if (savedRecordings.length < 1)
    {
        alert("At least one recorded signal is required to run unit tests.");
        return;
    }

    $("<div class='dialog' title='Running unit tests'><p>Running unit tests. Please wait...</p></div>").dialog({
        dialogClass: "no-close testresultdialog",
        buttons: [
        {
          text: "Cancel",
          icons: {
            primary: "ui-icon-heart"
          },
          click: function() {
            $( this ).dialog( "close" );
          }
        }]
    });

    currentRecording = 0;

    // Generate the first signal
    respondingToCommand = true;
    handleCommand("replay 0");
    respondingToCommand = false;

    // Run checkers
    respondingToCommand = true;
    handleCommand("unittest");
    respondingToCommand = false;
}

function runUnitTest()
{
    for (var i = 0; i < plotlyModules.length; i++)
    {
        var tests = plotlyModules[i].get('assertions');
        for (var j = 0; j < tests.length; j++)
        {
            if (tests[j].get('testfunction')(plotlyModules[i], plotlyModules[i].get('recordedData')))
            {
                unitTestFailures.push({module: plotlyModules[i], test: tests[j], signal: currentRecording});
            }
        }
    }

    if (currentRecording < savedRecordings.length - 1)
    {
        currentRecording++;

        respondingToCommand = true;
        handleCommand("replay " + currentRecording);
        respondingToCommand = false;

        respondingToCommand = true;
        handleCommand("unittest");
        respondingToCommand = false;
    }
    else
    {
        // destroy the "waiting" dialog
        $(".ui-dialog-content").dialog("destroy");

        var resultMessage = "";
        if (unitTestFailures.length == 0)
        {
            resultMessage += "All tests passed! <br>";
        }
        else
        {
            resultMessage += "Test failures: <br><br>";

            for (var i = 0; i < unitTestFailures.length; i++)
            {
                resultMessage += "Test: ";
                resultMessage += unitTestFailures[i].test.get('testname');
                resultMessage += ", Module: ";
                resultMessage += unitTestFailures[i].module.get('modulename');
                resultMessage += ", Signal: ";
                resultMessage += unitTestFailures[i].signal;
                resultMessage += "<br><br>";
            }
        }

        unitTestFailures = [];

        // Create a dialog showing the results
        $("<div class='dialog' title='Results'><p>" + resultMessage + "</p></div>").dialog({
            dialogClass: "no-close testresultdialog",
            buttons: [
            {
              text: "Ok",
              icons: {
                primary: "ui-icon-heart"
              },
              click: function() {
                $( this ).dialog( "close" );
              }
            }]
        });
    }
}

// Iterates through each defined assertion, running them on the latest recording from each module
function runCheckers()
{
    // initialize the failed results arrays for each test
    var failedResults = {};
    for (var i = 0; i < assertions.length; i++)
    {
        failedResults[assertions[i].testname] = [];
    }

    for (var i = 0; i < plotlyModules.length; i++)
    {
        var m = plotlyModules[i];

        for (var t = 0; t < assertions.length; t++)
        {
            if (assertions[t].testfunction(m, m.get('recordedData')))
            {
                failedResults[assertions[t].testname].push(m);
            }
        }
    }

    var resultMessage = "Potential issues detected on visualized modules: <br><br>";

    for (var i = 0; i < assertions.length; i++)
    {
        var testResults = failedResults[assertions[i].testname];
        if (testResults.length > 0)
        {
            resultMessage += assertions[i].explanation;
            resultMessage += " <br>";
            for (var j = 0; j < testResults.length; j++)
            {
                resultMessage += "- ";
                resultMessage += testResults[j].attr('text/text');
                resultMessage += "<br>";
            }
            resultMessage += "<br>";
        }
    }

    resultMessage += "(Issues may be reported erroneously if the oscilloscope settings are not configured correctly. Try adjusting the sampling rate and sample size to get a better view of the signal, then running checkers again.)";

    $("<div class='dialog' title='Results'><p>" + resultMessage + "</p></div>").dialog({
        dialogClass: "no-close testresultdialog",
        buttons: [
        {
          text: "Ok",
          icons: {
            primary: "ui-icon-heart"
          },
          click: function() {
            $( this ).dialog( "close" );
          }
        }]
    });
}

// Init built-in assertions
assertions.push(
    {
        testname: 'Voltage exceeded',
        explanation: "The following modules output values at or exceeding the maximum/minimum voltage of the FPAA. If unintended, this may indicate that gain needs to be reduced, or that a module is otherwise misconfigured. Note that comparators are omitted from this list, as their intended behavior is to output max/min voltage:",
        testfunction: check_VoltageExceeded
    }
);
assertions.push(
    {
        testname: 'No signal',
        explanation: "The following modules appear to output little or no signal. If unintended, likely issues are that a valid input is not connected to the module, or that the signal needs a gain stage:",
        testfunction: check_NoSignal
    }
);
assertions.push(
    {
        testname: 'Unchanging comparator',
        explanation: "The following comparators appear not to change their output over the course of recording. If unintended, verify that the value/signal used for comparison falls within variations in the input signal:",
        testfunction: check_NoSignalComparator
    }
);
setAssertionList();

// This checker reports modules whose output reaches or exceeds the max/min voltage of the FPAA
function check_VoltageExceeded(m, data)
{
    if (m instanceof joint.shapes.logic.Comparator)
    {
        return false;
    }
    var len = data.length;
    for (var i = 0; i < len; i++)
    {
        if ((data[i] > maxVoltage) || (data[i] < minVoltage))
        {
            return true;
        }
    }
    return false;
}


// This checker reports modules whose output does not significantly vary of the course of the recording
function check_NoSignal(m, data)
{
    // don't check for comparators
    if (m instanceof joint.shapes.logic.Comparator)
    {
        return false;
    }
    var len = data.length;
    var maxVal = minVoltage;
    var minVal = maxVoltage;
    for (var i = 0; i < len; i++)
    {
        maxVal = Math.max(maxVal, data[i]);
        minVal = Math.min(minVal, data[i]);
    }

    if (Math.abs(maxVal - minVal) < noSignalSpread)
    {
        return true;
    }
    else {
        return false;
    }
}

function check_NoSignalComparator(m, data)
{
    // only check for comparators
    if (!(m instanceof joint.shapes.logic.Comparator))
    {
        return false;
    }
    var len = data.length;
    var maxVal = minVoltage;
    var minVal = maxVoltage;
    for (var i = 0; i < len; i++)
    {
        maxVal = Math.max(maxVal, data[i]);
        minVal = Math.min(minVal, data[i]);
    }

    if (Math.abs(maxVal - minVal) < noSignalSpread)
    {
        return true;
    }
    else {
        return false;
    }
}

/* Add module buttons */
$("#add_gaininv").on('click', function(e) {
    respondingToCommand = true;
    writeSocket("add gaininv");
    handleCommand("add gaininv");
    respondingToCommand = false;
});
$("#add_filter").on('click', function(e) {
    respondingToCommand = true;
    writeSocket("add filter");
    handleCommand("add filter");
    respondingToCommand = false;
});
$("#add_comparator").on('click', function(e) {
    respondingToCommand = true;
    writeSocket("add comparator");
    handleCommand("add comparator");
    respondingToCommand = false;
});
$("#add_sum").on('click', function(e) {
    respondingToCommand = true;
    writeSocket("add sum");
    handleCommand("add sum");
    respondingToCommand = false;
});
$("#add_difference").on('click', function(e) {
    respondingToCommand = true;
    writeSocket("add difference");
    handleCommand("add difference");
    respondingToCommand = false;
});

function unselectAll() {
    _.each(graph.getCells(), function(cell) {
        paper.findViewByModel(cell).unhighlight();
    });
}

function clearParams() {
    $('#parameter-list').html("");
}

function appendParam(m, modulename, paramname, displayname, paramtype, min, max, defaultvalue, radiolabels) {
    var newparam = "";

    newparam = newparam.concat("<div class=\"form-group\">");
    newparam = newparam.concat("<label class=\"paramlabel\" for=\"");
    newparam = newparam.concat(paramname);
    newparam = newparam.concat("\" title=\"");
    newparam = newparam.concat(displayname);
    newparam = newparam.concat("\">");
    newparam = newparam.concat(displayname);
    newparam = newparam.concat("</label>");

    if (paramtype == "range")
    {
        newparam = newparam.concat("<br><input class=\"rangeparam param\" id=\"");
        newparam = newparam.concat(paramname);
        newparam = newparam.concat("_range\" name=\"");
        newparam = newparam.concat(paramname);
        newparam = newparam.concat("\" for=\"");
        newparam = newparam.concat(modulename);
        newparam = newparam.concat("\" type=");
        newparam = newparam.concat("\"range\"");
        newparam = newparam.concat(" step=\".1\"");
        newparam = newparam.concat(" value=\"");
        newparam = newparam.concat(defaultvalue);
        newparam = newparam.concat("\" min=\"");
        newparam = newparam.concat(min);
        newparam = newparam.concat("\" max=\"");
        newparam = newparam.concat(max);
        newparam = newparam.concat("\" autocomplete=\"off\">");

        newparam = newparam.concat("<input type=\"number\" class=\"numparam param\" for=\"");
        newparam = newparam.concat(modulename);
        newparam = newparam.concat("\" name=\"");
        newparam = newparam.concat(paramname);
        newparam = newparam.concat("\" min=\"");
        newparam = newparam.concat(min);
        newparam = newparam.concat("\" max=\"");
        newparam = newparam.concat(max);
        newparam = newparam.concat("\" id=\"");
        newparam = newparam.concat(paramname);
        newparam = newparam.concat("_num\" value=\"");
        newparam = newparam.concat(defaultvalue);
        newparam = newparam.concat("\">");
    }
    else if (paramtype == "radio")
    {
        newparam = newparam.concat("<div id=\"");
        newparam = newparam.concat(paramname);
        newparam = newparam.concat("_containiner\">");
        for (var i = 0; i < radiolabels.length; i++)
        {
            newparam = newparam.concat("<label class=\"radiolabel\" for=\"");
            newparam = newparam.concat(radiolabels[i]);
            newparam = newparam.concat("\">");
            newparam = newparam.concat(radiolabels[i]);
            newparam = newparam.concat("</label>");
            newparam = newparam.concat("<input type=\"radio\" class=\"radioparam param\" for=\"");
            newparam = newparam.concat(modulename);
            newparam = newparam.concat("\" name=\"");
            newparam = newparam.concat(modulename + '_' + paramname);
            newparam = newparam.concat("\" id=\"");
            newparam = newparam.concat(paramname);
            newparam = newparam.concat("_radio");
            newparam = newparam.concat(i);
            newparam = newparam.concat("\" value=\"");
            newparam = newparam.concat(radiolabels[i]);
            if (i == defaultvalue)
            {
                newparam = newparam.concat("\" checked>");
            }
            else
            {
                newparam = newparam.concat("\">");
            }
            newparam = newparam.concat("<br>");
        }
        newparam = newparam.concat("</div>");
    }

    $(m.findView(paper).$box[2]).append(newparam);
}

function appendDelete(m)
{
    var button = "<input type=\"button\" id=\"deletebutton\" name=\"deletebutton\" value=\"X\" style=\"background-color: #e36f6f; font-weight: bold; font-variant: small-caps; border: none; display:block; margin: auto;\"><br>";
    var deletediv = m.findView(paper).$box[1]; // find deletediv
    $(deletediv).append(button);

    deletebutton = $(deletediv).find("#deletebutton");
    deletebutton.on('click', {module: m}, function(e) {
        respondingToCommand = true;
        writeSocket("remove " + e.data.module.get('modulename'));
        handleCommand("remove " + e.data.module.get('modulename'));
        respondingToCommand = false;
        clearParams();
    });
}

function appendAssertionDelete(t)
{
    var button = "<input type=\"button\" id=\"deletebutton\" name=\"deletebutton\" value=\"X\" style=\"background-color: #e36f6f; font-weight: bold; font-variant: small-caps; border: none; display:block; margin: auto;\"><br>";
    var deletediv = t.findView(paper).$box[0]; // find deletediv
    $(deletediv).append(button);

    deletebutton = $(deletediv).find("#deletebutton");
    deletebutton.on('click', {test: t}, function(e) {
        e.data.test.remove()
    });
}

function appendVisualizeCheckbox(m)
{
    var button = "Visualize: <input type=\"checkbox\" id=\"visualizecheckbox\" name=\"visualizecheckbox\" value=\"Visualize Module\" style=\"background-color: #e36f6f; border: none;\">";

    var checkboxdiv = m.findView(paper).$box[3]; // find checkbox div
    $(checkboxdiv).prepend(button);
    visualizecheckbox = $(checkboxdiv).find("#visualizecheckbox");

    if (m.get('visualize') == "true")
    {
        visualizecheckbox.prop('checked', true);
    }
    else {
        visualizecheckbox.prop('checked', false);
    }

    visualizecheckbox.on('click', {module: m}, function(e) {
        if (this.checked)
        {
            createPlot(e.data.module);
        }
        else
        {
            pausePlot(e.data.module);
        }
    });
}

// Default (show no params)
joint.shapes.logic.Gate.prototype.displayParams = function() {
}

/* Parameters for each type of gate */
joint.shapes.logic.Input.prototype.displayParams = function() {
    appendVisualizeCheckbox(this);
};

joint.shapes.logic.Gaininv.prototype.displayParams = function() {
    var gainparamname = "gain";
    var gaindisplayname = "Gain";
    appendParam(this, this.get('modulename'), gainparamname, gaindisplayname, "range", 0, 10.0, this.get(gainparamname), "");
    var gainrange = $("input[id=\"" + gainparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gainrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gainparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gainparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set('gain', this.value);
        $(this).next().val(this.value);
    });
    var gainnum = $("input[id=\"" + gainparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gainnum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gainparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gainparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set('gain', this.value);
        $(this).prev().val(this.value);
    });
    appendDelete(this);
    appendVisualizeCheckbox(this);
};

joint.shapes.logic.Filter.prototype.displayParams = function() {
    var filtertypeparamname = "filtertype";
    var filtertypedisplayname = "Filter Type";
    appendParam(this, this.get('modulename'), filtertypeparamname, filtertypedisplayname, "radio", 0, 0, this.get(filtertypeparamname), ["Low pass", "High pass"]);
    var filtertyperadio = $("input[name=\"" + this.get('modulename') + '_' + filtertypeparamname + "\"][for=\"" + this.get('modulename') + "\"]");
    filtertyperadio.on('input change', {module: this}, function(e) {
        var valIndex = (this.value == "Low pass") ? 0 : 1;
        if (e.data.module.get(filtertypeparamname) != valIndex)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(filtertypeparamname).concat(" ").concat(valIndex));
        }
        e.data.module.set(filtertypeparamname, valIndex);
    });

    var cornerfrequencyparamname = "cornerfrequency";
    var cornerfrequencydisplayname = "Corner Frequency (kHz)";
    appendParam(this, this.get('modulename'), cornerfrequencyparamname, cornerfrequencydisplayname, "range", 0, 1000.0, this.get(cornerfrequencyparamname), "");
    var cornerfrequencyrange = $("input[id=\"" + cornerfrequencyparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencyrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).next().val(this.value);
    });
    var cornerfrequencynum = $("input[id=\"" + cornerfrequencyparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencynum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).prev().val(this.value);
    });

    var qparamname = "q";
    var qdisplayname = "Q";
    appendParam(this, this.get('modulename'), qparamname, qdisplayname, "range", 0, 1, this.get(qparamname), "");
    var qrange = $("input[id=\"" + qparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    qrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(qparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(qparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(qparamname, this.value);
        $(this).next().val(this.value);
    });
    var qnum = $("input[id=\"" + qparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    qnum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(qparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(qparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(qparamname, this.value);
        $(this).prev().val(this.value);
    });

    var gainparamname = "gain";
    var gaindisplayname = "Gain";
    appendParam(this, this.get('modulename'), gainparamname, gaindisplayname, "range", 0, 10.0, this.get(gainparamname), "");
    var gainrange = $("input[id=\"" + gainparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gainrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gainparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gainparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gainparamname, this.value);
        $(this).next().val(this.value);
    });
    var gainnum = $("input[id=\"" + gainparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gainnum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gainparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gainparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gainparamname, this.value);
        $(this).prev().val(this.value);
    });

    var outputpolarityparamname = "outputpolarity";
    var outputpolaritydisplayname = "Output Polarity";
    appendParam(this, this.get('modulename'), outputpolarityparamname, outputpolaritydisplayname, "radio", 0, 0, this.get(outputpolarityparamname), ["Noninverting", "Inverting"]);
    var outputpolarityradio = $("input[name=\"" + this.get('modulename') + '_' + outputpolarityparamname + "\"][for=\"" + this.get('modulename') + "\"]");
    outputpolarityradio.on('input change', {module: this}, function(e) {
        var valIndex = (this.value == "Noninverting") ? 0 : 1;
        if (e.data.module.get(outputpolarityparamname) != valIndex)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(outputpolarityparamname).concat(" ").concat(valIndex));
        }
        e.data.module.set(outputpolarityparamname, valIndex);
    });

    appendDelete(this);
    appendVisualizeCheckbox(this);
};

joint.shapes.logic.Comparator.prototype.displayParams = function() {
    var modeparamname = "mode";
    var modedisplayname = "Mode";
    appendParam(this, this.get('modulename'), modeparamname, modedisplayname, "radio", 0, 0, 1, ["Ground reference", "Variable reference", "Dual input"]);
    var filtertyperadio = $("input[name=\"" + this.get('modulename') + '_' + modeparamname + "\"][for=\"" + this.get('modulename') + "\"][for=\"" + this.get('modulename') + "\"]");
    filtertyperadio.on('input change', {module: this}, function(e) {
        if (this.value == "Ground reference")
        {
            var valIndex = 1;
        }
        else if (this.value == "Variable reference")
        {
            var valIndex = 4;
        }
        else if (this.value == "Dual input")
        {
            var valIndex = 2;
        }

        if (e.data.module.get(modeparamname) != valIndex)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(modeparamname).concat(" ").concat(valIndex));
        }
        e.data.module.set(modeparamname, valIndex);
    });

    var vrefparamname = "vref";
    var vrefdisplayname = "Variable reference voltage";
    appendParam(this, this.get('modulename'), vrefparamname, vrefdisplayname, "range", -2, 2, this.get(vrefparamname), "");
    var vrefrange = $("input[id=\"" + vrefparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    vrefrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(vrefparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(vrefparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(vrefparamname, this.value);
        $(this).next().val(this.value);
    });
    var vrefnum = $("input[id=\"" + vrefparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    vrefnum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(vrefparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(vrefparamname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(vrefparamname, this.value);
        $(this).prev().val(this.value);
    });

    var outputpolarityparamname = "outputpolarity";
    var outputpolaritydisplayname = "Output Polarity";
    appendParam(this, this.get('modulename'), outputpolarityparamname, outputpolaritydisplayname, "radio", 0, 0, this.get(outputpolarityparamname), ["Noninverting", "Inverting"]);
    var outputpolarityradio = $("input[name=\"" + this.get('modulename') + '_' + outputpolarityparamname + "\"][for=\"" + this.get('modulename') + "\"]");
    outputpolarityradio.on('input change', {module: this}, function(e) {
        var valIndex = (this.value == "Noninverting") ? 0 : 1;
        if (e.data.module.get(outputpolarityparamname) != valIndex)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(outputpolarityparamname).concat(" ").concat(valIndex));
        }
        e.data.module.set(outputpolarityparamname, valIndex);
    });

    var hysteresisparamname = "hysteresis";
    var hysteresisdisplayname = "Hysteresis";
    appendParam(this, this.get('modulename'), hysteresisparamname, hysteresisdisplayname, "radio", 0, 0, this.get(hysteresisparamname), ["0 mV", "10 mV"]);
    var hysteresisradio = $("input[name=\"" + this.get('modulename') + '_' + hysteresisparamname + "\"][for=\"" + this.get('modulename') + "\"]");
    hysteresisradio.on('input change', {module: this}, function(e) {
        var valIndex = (this.value == "0 mV") ? 0 : 1;
        if (e.data.module.get(hysteresisparamname) != valIndex)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(hysteresisparamname).concat(" ").concat(valIndex));
        }
        e.data.module.set(hysteresisparamname, valIndex);
    });

    appendDelete(this);
    appendVisualizeCheckbox(this);
};

joint.shapes.logic.Sum.prototype.displayParams = function() {
    var cornerfrequencyparamname = "cornerfrequency";
    var cornerfrequencydisplayname = "Corner Frequency (kHz)";
    appendParam(this, this.get('modulename'), cornerfrequencyparamname, cornerfrequencydisplayname, "range", 0, 1000.0, this.get(cornerfrequencyparamname), "");
    var cornerfrequencyrange = $("input[id=\"" + cornerfrequencyparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencyrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).next().val(this.value);
    });
    var cornerfrequencynum = $("input[id=\"" + cornerfrequencyparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencynum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).prev().val(this.value);
    });

    var gain1paramname = "gain1";
    var gain1displayname = "Gain1";
    appendParam(this, this.get('modulename'), gain1paramname, gain1displayname, "range", 0, 10.0, this.get(gain1paramname), "");
    gain1range = $("input[id=\"" + gain1paramname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain1range.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain1paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain1paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain1paramname, this.value);
        $(this).next().val(this.value);
    });
    var gain1num = $("input[id=\"" + gain1paramname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain1num.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain1paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain1paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain1paramname, this.value);
        $(this).prev().val(this.value);
    });

    var gain2paramname = "gain2";
    var gain2displayname = "Gain2";
    appendParam(this, this.get('modulename'), gain2paramname, gain2displayname, "range", 0, 10.0, this.get(gain2paramname), "");
    var gain2range = $("input[id=\"" + gain2paramname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain2range.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain2paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain2paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain2paramname, this.value);
        $(this).next().val(this.value);
    });
    var gain2num = $("input[id=\"" + gain2paramname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain2num.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain2paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain2paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain2paramname, this.value);
        $(this).prev().val(this.value);
    });

    appendDelete(this);
    appendVisualizeCheckbox(this);
};

joint.shapes.logic.Difference.prototype.displayParams = function() {
    var cornerfrequencyparamname = "cornerfrequency";
    var cornerfrequencydisplayname = "Corner Frequency (kHz)";
    appendParam(this, this.get('modulename'), cornerfrequencyparamname, cornerfrequencydisplayname, "range", 0, 1000.0, this.get(cornerfrequencyparamname), "");
    var cornerfrequencyrange = $("input[id=\"" + cornerfrequencyparamname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencyrange.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).next().val(this.value);
    });
    var cornerfrequencynum = $("input[id=\"" + cornerfrequencyparamname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    cornerfrequencynum.on('input change', {module: this}, function(e) {
        if (e.data.module.get(cornerfrequencyparamname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(cornerfrequencyparamname).concat(" ").concat(this.value));
        }
        e.data.module.set(cornerfrequencyparamname, this.value);
        $(this).prev().val(this.value);
    });

    var gain1paramname = "gain1";
    var gain1displayname = "Gain1";
    appendParam(this, this.get('modulename'), gain1paramname, gain1displayname, "range", 0, 10.0, this.get(gain1paramname), "");
    var gain1range = $("input[id=\"" + gain1paramname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain1range.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain1paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain1paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain1paramname, this.value);
        $(this).next().val(this.value);
    });
    var gain1num = $("input[id=\"" + gain1paramname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain1num.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain1paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain1paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain1paramname, this.value);
        $(this).prev().val(this.value);
    });

    var gain2paramname = "gain2";
    var gain2displayname = "Gain2";
    appendParam(this, this.get('modulename'), gain2paramname, gain2displayname, "range", 0, 10.0, this.get(gain2paramname), "");
    var gain2range = $("input[id=\"" + gain2paramname.concat("_range") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain2range.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain2paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain2paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain2paramname, this.value);
        $(this).next().val(this.value);
    });
    var gain2num = $("input[id=\"" + gain2paramname.concat("_num") + "\"][for=\"" + this.get('modulename') + "\"]");
    gain2num.on('input change', {module: this}, function(e) {
        if (e.data.module.get(gain2paramname) != this.value)
        {
            writeSocket("set ".concat(e.data.module.get('modulename')).concat(" ").concat(gain2paramname).concat(" ").concat(this.value).concat(" noflash"));
        }
        e.data.module.set(gain2paramname, this.value);
        $(this).prev().val(this.value);
    });

    appendDelete(this);
    appendVisualizeCheckbox(this);
};


// Highlight cell on click (funtionality disabled)
paper.on('cell:pointerclick', function(cellview, evt, x, y) {
    // Select if element (don't highlight links in this way)
    if (!cellview.model.isLink())
    {
        // Unselect previous selection
        // unselectAll();
        // cellview.highlight();
        // highlightedModule = cellview.model;
        // clearParams();
        // cellview.model.displayParams();

        /* UNCOMMENT TO AUTO-PROBE ON CLICK */
        // if (!autoscan && (!(cellview.model instanceof joint.shapes.logic.Output)))
        // {
        //     respondingToCommand = true;
        //     writeSocket("probe " + cellview.model.get('modulename'));
        //     handleCommand("probe " + cellview.model.get('modulename'));
        //     respondingToCommand = false;
        // }
    }
});

// initial graph setup
var gates = {
    input1: new joint.shapes.logic.Input({ position: { x: 105, y: 50 }, attrs: {text: {text: 'input1'}}, modulename: 'io1', simplename: 'input1'}),
    input2: new joint.shapes.logic.Input({ position: { x: 55, y: 200 }, attrs: {text: {text: 'input2'}}, modulename: 'io2', simplename: 'input2'}),
    input3: new joint.shapes.logic.Input({ position: { x: 5, y: 350 }, attrs: {text: {text: 'input3'}}, modulename: 'io5', simplename: 'input3'}),
    output1: new joint.shapes.logic.Output({ position: { x: 1100, y: 50 }, attrs: {text: {text: 'output1'}}, modulename: 'io4', simplename: 'output1'}),
    output2: new joint.shapes.logic.Output({ position: { x: 1050, y: 150 }, attrs: {text: {text: 'output2'}}, modulename: 'io6', simplename: 'output2'}),
    probemodule: new joint.shapes.logic.Output({ position: { x: 1000, y: 250 }, attrs: {text: {text: 'probe'}}, modulename: 'io3', simplename: 'probe'})
};

// add initial connections here
var wires = [

];

// Create the initial setup
function init()
{
    // add gates and wires to the graph
    graph.addCells(_.toArray(gates));
    _.each(wires, function(attributes) {
        graph.addCell(paper.getDefaultLink().set(attributes));
    });

    // Add modules to dictionary, so we can find them by name
    _.each(gates, function(m) {
        moduleDict[m.get('modulename')] = m;
        // Add visualization canvases
        if (m instanceof joint.shapes.logic.Input)
        {
            m.set('visualize', "true");
            createPlot(m);
            appendVisualizeCheckbox(m);
        }
    });

    probemodule = gates['probemodule'];
    probemodule.attr({'.body': { fill: 'rgba(50,50,50,.2)', stroke: 'rgba(50,50,50,.4)'},
    '.wire': { stroke: 'rgba(50,50,50,.4)' },
    text: {fill: 'rgba(50,50,50,.4)'}});
    nameSuffix = 0;
    respondingToCommand = false;
}

// Initialize graph
init();

// behavior when deleting a module or connection
graph.on('remove', function(model) {
    // COnnection removed
    if (!respondingToCommand)
    {
        if (model.isLink())
        {
            var s = graph.getCell(model.previous('source').id);
            var t = graph.getCell(model.previous('target').id);
            if (t instanceof joint.shapes.logic.Assertion)
            {
                removeFromArray(s.get("assertions"), t);
                t.attr('.body/fill', '#ffffff');
            }
            else
            {
                var newcommand = "disconnect ";
                newcommand = newcommand.concat(s.previous('modulename'));
                newcommand = newcommand.concat(" ");
                newcommand = newcommand.concat(t.previous('modulename'));
                newcommand = newcommand.concat(" outputsite ");
                newcommand = newcommand.concat(model.previous('source').port);
                newcommand = newcommand.concat(" inputsite ");
                newcommand = newcommand.concat(model.previous('target').port);

                writeSocket(newcommand);
            }
        }
    }
});

// behavior when changing a connection from one site to another
graph.on('change:source change:target', function(model, end) {
    var e = 'target' in model.changed ? 'target' : 'source';

    // If a new link was added, send a connect command
    if ((!model.previous(e).id && model.get(e).id))
    {
        var s = graph.getCell(model.get('source').id);
        var t = graph.getCell(model.get('target').id);
        if (t instanceof joint.shapes.logic.Assertion)
        {
            s.get('assertions').push(t);
        }
        else
        {
            var newcommand = "connect ";
            newcommand = newcommand.concat(s.get('modulename'));
            newcommand = newcommand.concat(" ");
            newcommand = newcommand.concat(t.get('modulename'));
            newcommand = newcommand.concat(" outputsite ");
            newcommand = newcommand.concat(model.get('source').port);
            newcommand = newcommand.concat(" inputsite ");
            newcommand = newcommand.concat(model.get('target').port);

            writeSocket(newcommand);

            if (t == probemodule)
            {
                savedProbe = s;
            }
        }
    }
    // If a link was removed, send a disconnect command
    else if (model.previous(e).id && !model.get(e).id)
    {
        var s = graph.getCell(model.previous('source').id);
        var t = graph.getCell(model.previous('target').id);
        if (t instanceof joint.shapes.logic.Assertion)
        {
            removeFromArray(s.get("assertions"), t);
            t.attr('.body/fill', '#ffffff');
        }
        else
        {
            var newcommand = "disconnect ";
            newcommand = newcommand.concat(s.previous('modulename'));
            newcommand = newcommand.concat(" ");
            newcommand = newcommand.concat(t.previous('modulename'));
            newcommand = newcommand.concat(" outputsite ");
            newcommand = newcommand.concat(model.previous('source').port);
            newcommand = newcommand.concat(" inputsite ");
            newcommand = newcommand.concat(model.previous('target').port);

            writeSocket(newcommand);
        }
    }
});

// Searches cArray for a param corresponding to paramName.
// Returns defaultValue if not found.
function findParamInArray(cArray, paramName, defaultValue)
{
    var i = cArray.indexOf(paramName);
    if (i != -1)
    {
        return cArray[i+1];
    }
    else
    {
        return defaultValue;
    }
}

// add a new module to the graph with the specified initial parameter values
function addNewModule(module, params)
{
    moduleDict[module.get('modulename')] = module;
    newModule.addTo(graph);

    var after = findParamInArray(params, 'after', "");
    var before = findParamInArray(params, 'before', "");

    // if both before and after are named, then insert between
    if ((after != "") && (before != ""))
    {
        // disconnect old
        handleCommand("disconnect " + after + " " + before);
        handleCommand("connect " + after + " " + newModule.get('modulename'));
        handleCommand("connect " + newModule.get('modulename') + " " + before);
    }
    else if (after != "")
    {
        handleCommand("connect " + after + " " + newModule.get('modulename'));
    }
    else if (before != "")
    {
        handleCommand("connect " + newModule.get('modulename') + " " + before);
    }

    updateNewModulePosition();

    newModule.displayParams();
}

function updateNewModulePosition()
{
    defaultNewModulePosition.x += incrementNewModulePosition;
    defaultNewModulePosition.y += incrementNewModulePosition;
    if ((defaultNewModulePosition.x > maxModulePosition.x) || (defaultNewModulePosition.y > maxModulePosition.y))
    {
        defaultNewModulePosition = Object.assign({}, originalDefaultNewModulePosition);
    }
}

// Add module functions
function addGaininv(params)
{
    newModule = new joint.shapes.logic.Gaininv({ position: defaultNewModulePosition});

    newModule.set('modulename', findParamInArray(params, 'name', 'gaininv' + nameSuffix));
    newModule.set('visualize', "true");
    newModule.attr({text: {text: findParamInArray(params, 'name', 'inverting gain')}});
    nameSuffix++;

    addNewModule(newModule, params);
    createPlot(newModule);
}
function addFilter(params)
{
    newModule = new joint.shapes.logic.Filter({ position: defaultNewModulePosition});

    newModule.set('modulename', findParamInArray(params, 'name', 'filterbiquad' + nameSuffix));
    newModule.set('visualize', "true");
    newModule.attr({text: {text: findParamInArray(params, 'name', 'filter')}});
    nameSuffix++;

    addNewModule(newModule, params);
    createPlot(newModule);
}
function addComparator(params)
{
    newModule = new joint.shapes.logic.Comparator({ position: defaultNewModulePosition});

    newModule.set('modulename', findParamInArray(params, 'name', 'comparator' + nameSuffix));
    newModule.set('visualize', "true");
    newModule.attr({text: {text: findParamInArray(params, 'name', 'comparator')}});
    nameSuffix++;

    addNewModule(newModule, params);
    createPlot(newModule);
}
function addSum(params)
{
    newModule = new joint.shapes.logic.Sum({ position: defaultNewModulePosition});

    newModule.set('modulename', findParamInArray(params, 'name', 'sumfilter' + nameSuffix));
    newModule.set('visualize', "true");
    newModule.attr({text: {text: findParamInArray(params, 'name', 'sum')}});
    nameSuffix++;

    addNewModule(newModule, params);
    createPlot(newModule);
}
function addDifference(params)
{
    newModule = new joint.shapes.logic.Difference({ position: defaultNewModulePosition});

    newModule.set('modulename', findParamInArray(params, 'name', 'sumfilter' + nameSuffix));
    newModule.set('visualize', "true");
    newModule.attr({text: {text: findParamInArray(params, 'name', 'difference')}});
    nameSuffix++;

    addNewModule(newModule, params);
    createPlot(newModule);
}

// export the graph in a JSON format
function convertToStandardRepresentation()
{
    var elements = graph.getElements();
    var links = graph.getLinks();
    var standard = {"components" : [], "wires" : []};

    _.each(links, function(l) {
        standard.wires.push({"source" : l.getSourceElement().get('modulename'), "sourceport" : l.get('source').port, "target" : l.getTargetElement().get('modulename'), "targetport" : l.get('target').port,});
    });

    _.each(elements, function(e) {
        if (e instanceof joint.shapes.logic.Gaininv)
        {
            standard.components.push({
                "type" : "gaininv",
                "modulename" : e.get("modulename"),
                "params" : {
                    "gain" : e.get("gain")
                }});
        }
        else if (e instanceof joint.shapes.logic.Comparator)
        {
            standard.components.push({
                "type" : "comparator",
                "modulename" : e.get("modulename"),
                "params" : {
                    "mode" : e.get("mode"),
                    "vref" : e.get("vref"),
                    "outputpolarity" : e.get("outputpolarity"),
                    "hysteresis" : e.get("hysteresis"),
                }});
        }
        else if (e instanceof joint.shapes.logic.Filter)
        {
            standard.components.push({
                "type" : "filter",
                "modulename" : e.get("modulename"),
                "params" : {
                    "filtertype" : e.get("filtertype"),
                    "cornerfrequency" : e.get("cornerfrequency"),
                    "q" : e.get("q"),
                    "gain" : e.get("gain"),
                    "outputpolarity" : e.get("outputpolarity")
                }});
        }
        else if (e instanceof joint.shapes.logic.Sum)
        {
            standard.components.push({
                "type" : "sum",
                "modulename" : e.get("modulename"),
                "params" : {
                    "cornerfrequency" : e.get("cornerfrequency"),
                    "gain1" : e.get("gain1"),
                    "gain2" : e.get("gain2")
                }});
        }
        else if (e instanceof joint.shapes.logic.Difference)
        {
            standard.components.push({
                "type" : "difference",
                "modulename" : e.get("modulename"),
                "params" : {
                    "cornerfrequency" : e.get("cornerfrequency"),
                    "gain1" : e.get("gain1"),
                    "gain2" : e.get("gain2")
                }});
        }
        else if (e instanceof joint.shapes.logic.Input)
        {
            standard.components.push({
                "type" : "input",
                "modulename" : e.get("modulename"),
                "params" : {}});
        }
        else if (e instanceof joint.shapes.logic.Output)
        {
            standard.components.push({
                "type" : "output",
                "modulename" : e.get("modulename"),
                "params" : {}});
        }
        else {
            console.error("Unknown module type encountered when trying to export.");
        }
    });
    return standard;
}

// import a saved graph (currently does not program the FPAA)
function loadFromFile(fileName)
{
    fs.readFile(fileName, 'utf8', function (err,data) {
      if (err) {
        return console.log(err);
      }
      graph.fromJSON(JSON.retrocycle(JSON.parse(data)));
    });

    //Add additional logic here to create plots and paramdivs,
    // as well as to program the FPAA
}

// save a graph
function saveToFile(fileName)
{
    var jsonString = JSON.stringify(JSON.decycle(graph.toJSON()), function(key, value) {
        if (key == "plotcanvas") {
            return {};
        }
        else {
            return value;
        }
    } );
    fs.writeFile(fileName, jsonString, function(err) {
        if(err) {
            return console.log(err);
        }

        console.log("Saved graph to: " + fileName);
    });
}

// save graph in JSON format
function exportToFile(fileName)
{
    var jsonString = JSON.stringify(convertToStandardRepresentation());
    fs.writeFile(fileName, jsonString, function(err) {
        if(err) {
            return console.log(err);
        }

        console.log("Exported graph to: " + fileName);
    });
}

function parseSocketData(data)
{
    var command = "";
    for (var i = 0; i < data.length; i++)
    {
        command = command.concat(String.fromCharCode(data[i]));
    }
    return command;
}

function handleCommand(command)
{
    cArray = command.split(" ");
    switch (cArray[0])
    {
        case 'capture': // trigger a new capture
            if (!ongoingCapture)
            {
                if (autoscan || recordingForCheckers || recordingForUnitTests)
                {
                    setTimeout(capture.bind(null, moduleDict[cArray[1]]), beforeCaptureDelay);
                }
                else {
                    manualCapture();
                }
            }
            break;
        case 'add': // add a module
            switch (cArray[1])
            {
                case 'gaininv':
                    addGaininv(cArray);
                    break;
                case 'filter':
                    addFilter(cArray);
                    break;
                case 'comparator':
                    addComparator(cArray);
                    break;
                case 'sum':
                    addSum(cArray);
                    break;
                case 'difference':
                    addDifference(cArray);
                    break;
                default:
                    return;
                    break;
            }
            break;
        case 'remove': // delete a module
            // remove from the plotlyModules list, if it exists
            moduleDict[cArray[1]].remove();
            deletePlot(moduleDict[cArray[1]]);
            removeFromArray(plotlyModules, moduleDict[cArray[1]]);
            delete moduleDict[cArray[1]];
            break;
        case 'connect': // add a connection
            var newlink = paper.getDefaultLink().set(
                { source: { id: moduleDict[cArray[1]].id, port: findParamInArray(cArray,'outputsite','out') },
                  target: { id: moduleDict[cArray[2]].id, port: findParamInArray(cArray,'inputsite','in') }}
            );
            graph.addCell(newlink);
            break;
        case 'disconnect': // delete a connection
            var source = moduleDict[cArray[1]];
            var links = graph.getConnectedLinks(source, {outbound: true});
            _.each(links, function(l) {
                if ((l.getTargetElement().get('modulename') == cArray[2])
                        && (l.get('source').port == findParamInArray(cArray, 'outputsite', 'out'))
                        && (l.get('target').port == findParamInArray(cArray, 'inputsite', 'in')))
                {
                    l.remove();
                }
            });
            break;
        case 'set': // set a parameter to a new value
            // set the new param value in the module
            moduleDict[cArray[1]].set(cArray[2], cArray[3]);

            // and update its param handles, if any
            $("input[name=" + cArray[2] + "]").val(cArray[3]);
            break;
        case 'probe': // connect the manual probe to the output of a given module
            graph.removeLinks(moduleDict[probeId]); // sever any other connection to probe
            var newlink = paper.getDefaultLink().set(
                { source: { id: moduleDict[cArray[1]].id, port: findParamInArray(cArray,'outputsite','out') },
                  target: { id: moduleDict[probeId].id, port: findParamInArray(cArray,'inputsite','in') }}
            );
            graph.addCell(newlink);
            break;
        case 'check': // run debug tests
            // save current probe
            if (!autoscan)
            {
                var links = graph.getConnectedLinks(probemodule, {inbound: true});
                if (links.length > 0)
                {
                    savedProbe = links[0].getSourceElement();
                }

            }
            isValid = false;
            recordingForCheckers = true;
            plotlyModulesIndex = 0;
            break;
        case 'unittest': // prep unit tests
            if (!autoscan)
            {
                var links = graph.getConnectedLinks(probemodule, {inbound: true});
                if (links.length > 0)
                {
                    savedProbe = links[0].getSourceElement();
                }

            }
            isValid = false;
            recordingForUnitTests = true;
            plotlyModulesIndex = 0;
            break;
        case 'rununittests': // run unit tests
            runUnitTests();
            break;
        case 'rename': // rename a module
            moduleDict[cArray[1]].set('modulename', cArray[2]);
            moduleDict[cArray[1]].attr({text: {text: cArray[2]}});
            moduleDict[cArray[2]] = moduleDict[cArray[1]];
            delete moduleDict[cArray[1]];
            break;
        case 'record': // record a signal
            pauseOscilloscope = true;
            savedRecordings.push(manualRecordingSeries);
            break;
        case 'resume': // unpause the oscilloscope
            pauseOscilloscope = false;
            doNextCapture();
            break;
        case 'replay': // generate a recorded signal
            isValid = false;
            replaying = true;
            replaySignal(cArray[1]);
            break;
        case 'endreplay': // stop generating a signal
            isValid = false;
            replaying = false;
            endReplay()
            break;
        case 'export': // export the given graph in JSON format
            exportToFile(cArray[1]);
            break;
        case 'import': // import graph from file
            importFromFile(cArray[1]);
            break;
        case 'save': // save graph
            saveToFile(cArray[1]);
            break;
        case 'load': // load graph
            loadFromFile(cArray[1]);
            break;
        case 'reset': // reset everything to initial state (reprograms FPAA)
            if (replaying)
            {
                endReplay();
            }

            location.reload();
            break;
        case 'exit': // close the application
            if (replaying)
            {
                endReplay();
            }
            remote.getCurrentWindow().close();
        default:
            console.log("Unknown command: " + command);
    }
}

function removeFromArray(array, entry)
{
    var index = array.indexOf(entry);
    if (index > -1)
    {
        array.splice(index, 1);
    }
}
