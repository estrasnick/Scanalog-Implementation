Scanalog is a design tool which leverages programmable analog electronics to aid in the design and debugging of analog circuits. For an introduction to the system, its components, and its uses, see the paper and auxiliary materials located at: https://estrasnick.github.io/Scanalog/

## Hardware Setup
Scanalog has three primary hardware components: an FPAA, a digital oscilloscope, and an arbitrary waveform generator.

Scanalog was built on the AN231E04 dpASP from Anadigm Inc. (http://www.anadigm.com/an231e04.asp) Other FPAA variants by Anadigm may be compatible but are not guaranteed and will likely not configure correctly. The developer edition of this board is recommended, as it contains a number of essential features, such as USB COM programming and I/O filters for converting single-ended to differential signals.

For both the oscilloscope and waveform generator, we use the Bitscope Micro (BS05). (http://www.bitscope.com/product/BS05/)

Channel A on the oscilloscope should be connected to filtered output 2. If the generator functionality is to be used, then an additional amplification circuit must be added to the board. This is because, unlike the FPAA, the generator is not capable of outputting negative voltages. As a result, for an observed voltage sample of V, Scanalog programs the generator to output (V / 2) + 1.67, and the custom amplifier adjust the signal back to the the -3.3 - 3.3 V range.

Both the oscilloscope and signal generator should be appropriately grounded with the FPAA. All three devices should be connected via USB to the host computer.

Upon initialization or reset, Scanalog configures the board in a blank state, with three inputs and two outputs (in addition to the probe output). Input1 and Input2 use the IO1 and IO2 Raunch filters as configured on the Development Board. Output1 and Output2 use the IO4 and IO3 output buffers. The output buffers can be configured according to the official documentation to implement gain and filtering, but can be configured to have a minimal impact on the signal using equally sized large resistors across all four open connections. All DIP switches are closed.

## Software Installation
Requirements:
* Windows 10 x64
* 3x USB (3.0) ports
* Visual Studio (2015 or later)
* Node.js
* Electron


The source code provided here has two primary components: A C++ backend and a graphical interface built using Electron and Node.js. These two components communicate via sockets.

To run Scanalog, the C++ application (FPAA_Application.sln) must first be launched in the background. This can be done by running the application (in Debug mode) in Visual Studio. In the dialog that appears, click the "Port" button and use the resulting menu to select the COM port which has been assigned to the FPAA.

Then, navigate to the JointApp folder in a Node.js terminal and execute `npm start` to launch the GUI. If the oscilloscope and generator do not function properly, then the COM ports are likely not set correctly in logic.js.

## Usage
The primary functionality of Scanalog can be controlled both by using GUI or with the built-in command line. In the GUI, modules are added using the panel at the bottom left, and connected by drawing lines from output to input. Oscilloscope settings are controlled in the bottom right, and disabling "Autoscan" will allow the user to manually probe a module by connecting its output to the "Probe" output that appears.

Command Line commands are as follows:
* help - Displays command list
* reset - Resets the configuration of the board and reloads the interface (same functionality as F5)
* add <module type> (name <name>) (param <param>...) -  Adds a new module, with optional name and parameters
* remove <module name> - Removes the specified module
* rename <module name> <new name> - Renames a module
* set <param name> <new value> - Sets the specified parameter to the given value
* connect <outputting module name> <inputting module name> (outputsite <output site name>) (inputsite <input site name>) - Connects the output module to the input module, optionally specifying a particular site for each
* disconnect <outputting module name> <inputting module name> (outputsite <output site name>) (inputsite <input site name>) - Severs the specified connection between modules
* probe <name of module to probe> (outputsite <output site name>) (probename <probe name>) - Shorthand for connecting the output of a specified module to the input of the probe
* pause - Stops playback of the manual oscilloscope window
* resume - Resumes playback of the manual oscilloscope window
* record - Triggers a capture of the current oscilloscope window (and pauses)
* export <filename> - Exports the circuit in JSON format
* import <filename> - Imports a circuit from JSON format (currently not implemented)
* exit - Closes the application
