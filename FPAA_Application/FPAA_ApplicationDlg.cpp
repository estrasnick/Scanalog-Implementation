#include "stdafx.h"
#include "FPAA_Application.h"
#include "FPAA_ApplicationDlg.h"
#include "AboutDlg.h"
#include "DownloadManager.h"
#include "GeneralChip.h"

#include "Helpers.h"
#include "BaseModule.h"
#include "IO.h"
#include "GainInv_Module.h"
#include "Comparator_Module.h"
#include "FilterBiquad_Module.h"
#include "SumFilter_Module.h"
#include "Configurator.h"
#include <Library\BitLib.h>
#include <sstream>
#include <iterator>
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//crs 10/4/2007
#define AUTO_EXEC	1
#define ZERO_CROSS	0
//crs 10/4/2007 end

// Dave Lovell added these defines
#define RESET_FIRST		0
#define NO_RESET		1


BEGIN_MESSAGE_MAP(FPAA_ApplicationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PORT, OnPort)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_DOWNLOAD_GAIN, OnDownloadGain)
	ON_BN_CLICKED(IDC_DOWNLOAD_HALF_RECTIFIER, OnDownloadHalfRectifier)
	ON_BN_CLICKED(IDC_DOWNLOAD_TEST_CONFIG, OnDownloadTestConfig)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_ADD_GAIN, OnAddGain)
	ON_BN_CLICKED(IDC_ADD_VREF_COMPARATOR, OnAddVrefComparator)
	ON_BN_CLICKED(IDC_ADD_GROUND_COMPARATOR, OnAddGroundComparator)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_SAMPLE_EDIT, OnSampleEditChanged)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SOCKET_RECEIVE_UPDATE, OnSocketReceive)
END_MESSAGE_MAP()

FPAA_ApplicationDlg::FPAA_ApplicationDlg(CWnd* pParent)
	: CDialog(FPAA_ApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void FPAA_ApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SAMPLE_SLIDER, m_sampleSlider);
   DDX_Control(pDX, IDC_SAMPLE_EDIT, m_sampleEdit);
   DDX_Control(pDX, IDC_COMMAND_LINE, m_commandLine);
}

BOOL FPAA_ApplicationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

   // Setup application icon
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

   // Add the "About..." menu item
   ConfigureAboutMenu();

   // Initialize the sample slider\edit controls
   m_sampleMin = .1;
   m_sampleMax = 3.0;
   m_sampleValue = 1.0;

   // Initialize default circuit text
   SetDlgItemText(IDC_CIRCUIT_VISUALIZATION, "circuit not yet configured");
   
   // The range [0, 100] will be percent.  The slider control cannot use
   // floating point values, so we map [0, 100] on to our desired scale
   // to achieve floating point results.
   m_sampleSlider.SetRange(0, 100);
   
   // Initial update of the sample controls
   UpdateSampleControls();

   // Open sockets
   serverSocket = new FPAA_Socket();
   serverSocket->Create(DEFAULT_PORT);
   serverSocket->SetParentDlg(this);
   HWND *objectHandle = new	HWND;
   *objectHandle = GetSafeHwnd();
   serverSocket->SetHWND(objectHandle);

   if (serverSocket->Listen() == FALSE)
   {
	   AfxMessageBox("Unable to Listen on that port, please try another port");
   }

   OnReset();
	
	// return TRUE  unless you set the focus to a control
   return TRUE;
}

void FPAA_ApplicationDlg::Exit()
{
	serverSocket->Close();
	BL_Close(); // close the scope
	OnReset();
	EndDialog(IDOK);
}

void FPAA_ApplicationDlg::InitScope()
{
	/*
	* Open and select the first channel on the first device.
	*/
	if (!BL_Open(MY_PROBE_FILE, MY_DEVICE)) {
		Message::ErrorMessage("Could not open/init scope.");
		return;
	}
	if (BL_Select(BL_SELECT_DEVICE, MY_DEVICE) != MY_DEVICE) {
		Message::ErrorMessage("Could not select scope.");
		return;
	}
	if (BL_Select(BL_SELECT_CHANNEL, MY_CHANNEL) != MY_CHANNEL) {
		Message::ErrorMessage("Could not select scope channel.");
		return;
	}

	/*
	* Capture settings
	*/
	if (BL_Mode(MY_MODE) != MY_MODE) {
		Message::ErrorMessage("Could not select scope mode.");
		return;
	}

	numSamples = MY_SIZE;
	samplingRate = MY_RATE;
	prevNumSamples = numSamples;
	prevSamplingRate = samplingRate;

	BL_Intro(BL_ZERO); /* optional, default BL_ZERO */
	BL_Delay(BL_ZERO); /* optional, default BL_ZERO */
	BL_Rate(samplingRate); /* optional, default BL_MAX_RATE */
	BL_Size(numSamples); /* optional, default BL_MAX_SIZE */
	BL_Select(BL_SELECT_CHANNEL, MY_CHANNEL); /* choose the channel */
	BL_Trigger(BL_ZERO, BL_TRIG_RISE); /* optional when untriggered */
	BL_Select(BL_SELECT_SOURCE, BL_SOURCE_POD); /* use the POD input */
	BL_Range(BL_Count(BL_COUNT_RANGE)); /* maximum range */
	BL_Offset(BL_ZERO); /* optional, default 0 */
	BL_Enable(TRUE); /* at least one channel must be initialised */

	// Create a relevant plot
	CreatePlot();
}

void FPAA_ApplicationDlg::Capture()
{
#ifndef DEBUG_WITHOUT_HARDWARE
	/*
	* Capture and acquire the data...
	*/
	//BL_Trace(TRIGGER_TIMEOUT_S, BL_ASYNCHRONOUS);  /* capture data (with a trigger and timeout) */
	BL_Trace(BL_TRACE_FORCED, BL_ASYNCHRONOUS);  /* capture data (untriggered) */
	//printf("Data acquisition complete. Dump Log...\n");
	//printf("%s\n", BL_Log());
	//LogStringData(BL_Log());
	SetTimer(POLL_TRACE_TIMER, POLL_TRACE_TIME_MS, NULL);

#endif // DEBUG_WITHOUT_HARDWARE
}

void FPAA_ApplicationDlg::Acquire()
{
	double *d = new double[numSamples]; /* let's get 5 samples */
	BL_Select(BL_SELECT_CHANNEL, MY_CHANNEL); /* optional if only one channel */
	if (BL_Acquire(numSamples, d) == numSamples) { /* acquire (i.e. dump) the capture data */

		DisplayCapture(d);
	}
}

void FPAA_ApplicationDlg::DisplayCapture(double d[])
{
	double x = 0;
	double period = 1.0 / samplingRate;
	static BOOL pros = { FALSE };
	if (!pros) {
		pros = TRUE;
		if (!autoscroll)
		{
			m_Plot.Reset();
			for (int i = 0; i < numSamples; i++)
			{
				m_Plot.AddPoint(MY_CHANNEL, x, d[i]);
				x += period;
			}
		}
		else
		{
			x = lastCaptureTime + period;
			for (int i = 0; i < numSamples; i++)
			{
				m_Plot.AddPoint(MY_CHANNEL, x, d[i]);
				x += period;
			}
			lastCaptureTime = x;
		}
		Invalidate();
		pros = FALSE;
	}
}

void FPAA_ApplicationDlg::CreatePlot()
{
	m_plotRect = CRect(PLOT_RECT_COORDS);
	//GetClientRect(Rect);
	m_Plot.Create(WS_CHILD | WS_VISIBLE | WS_EX_TOPMOST, m_plotRect, this, 12000);

	m_Plot.SetSerie(0, PS_SOLID, RGB(255, 0, 0), 0.0, 5.0, "Channel A");
	m_Plot.SetLegend(0, PS_SOLID, RGB(255, 0, 0), "Channel A");
	m_Plot.SetBXTitle("Time");
	m_Plot.SetLYTitle("Voltage");

	m_Plot.m_bAutoScrollX = FALSE;
	
	SetTimer(CAPTURE_TIMER, DELAY_BETWEEN_CAPTURES_MS, NULL);
	canSize = true;
}

void FPAA_ApplicationDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == POLL_TRACE_TIMER)
	{
		if (BL_State() == BL_STATE_DONE)
		{
			KillTimer(POLL_TRACE_TIMER);
			Acquire();
			if (autocapture)
			{
				SetTimer(CAPTURE_TIMER, DELAY_BETWEEN_CAPTURES_MS, NULL);
			}
		}
	}
	else if (nIDEvent == CAPTURE_TIMER)
	{
		KillTimer(CAPTURE_TIMER);
		Capture();
	}
	CDialog::OnTimer(nIDEvent);
}

void FPAA_ApplicationDlg::SetClientSocket(FPAA_Socket *s)
{
	clientSocket = s;

	clientSocket->SetParentDlg(this);
	HWND *objectHandle = new HWND;
	*objectHandle = GetSafeHwnd();
	clientSocket->SetHWND(objectHandle);

	clientConnected = true;
}

LRESULT FPAA_ApplicationDlg::OnSocketReceive(WPARAM wParam, LPARAM lParam)
{
	char *pBuf = new char[COMMAND_BUFFER_SIZE];
	int iLen;
	iLen = clientSocket->Receive(pBuf, COMMAND_BUFFER_SIZE - 1);
	if (iLen == SOCKET_ERROR)
	{
		Message::ErrorMessage("Could not receive command from socket.");
		return 1;
	}
	else
	{
		pBuf[iLen] = NULL;
		std::string compare = "connect io1 io3";
		std::string s = std::string(pBuf);
		int res = compare.compare(s);
		HandleCommand(s, false);

		delete pBuf;
	}
	return 0;
}

void FPAA_ApplicationDlg::SendToClient(std::string s)
{
	if (clientConnected)
	{
		clientSocket->Send(s.c_str(), s.length());
	}
}

void FPAA_ApplicationDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (canSize) {
		m_Plot.MoveWindow(m_plotRect, this);
	}
}

BOOL FPAA_ApplicationDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void FPAA_ApplicationDlg::ConfigureAboutMenu()
{
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
}

void FPAA_ApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		AboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void FPAA_ApplicationDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR FPAA_ApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void FPAA_ApplicationDlg::OnPort()
{
   DownloadManager::Instance()->ShowPortDlg();
}

void FPAA_ApplicationDlg::OnAbout()
{
   AboutDlg dlg;
   dlg.DoModal();
}

void FPAA_ApplicationDlg::OnDownloadTestConfig()
{
	Configurator::Reset();

	GainInv_Module *gaininv1 = new GainInv_Module("GainInv", "GainInv");
	gaininv1->SetParamValue("gain", DoubleToString(m_sampleValue));

	gaininv1->AddToEnd();

	Configure();
}

void FPAA_ApplicationDlg::OnDownloadGain()
{
   m_GeneralChip.ExecuteConfig(an_Gain_ID, RESET_FIRST);

   m_GeneralChip.Gain_GainInv1.setGainInv(m_sampleValue);
   m_GeneralChip.ExecuteReconfig(an_Gain_ID, AUTO_EXEC);
}

void FPAA_ApplicationDlg::OnDownloadHalfRectifier()
{
	m_GeneralChip.ExecuteConfig(an_HalfRectifier_ID, RESET_FIRST);

	m_GeneralChip.RectifierHold1.setGainRectHold(m_sampleValue);
	m_GeneralChip.ExecuteReconfig(an_HalfRectifier_ID, AUTO_EXEC);
}

void FPAA_ApplicationDlg::OnReset()
{
   m_GeneralChip.SetCurrentActiveConfig(an_CUSTOM_INITIAL);
   Configurator::Reset();
   Configure();
}

void FPAA_ApplicationDlg::OnAddGain()
{
	m_GeneralChip.SetCurrentActiveConfig(an_CUSTOM_GAININV);

	GainInv_Module *newGain = new GainInv_Module();
	newGain->SetParamValue("gain", DoubleToString(m_sampleValue));
	newGain->AddToEnd();

	Configure();
}

void FPAA_ApplicationDlg::OnAddVrefComparator()
{
	m_GeneralChip.SetCurrentActiveConfig(an_CUSTOM_VREFCOMPARATOR);

	Comparator_Module *newComp = new Comparator_Module();
	newComp->AddToEnd(newComp->GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)); // add to positive input
	newComp->SetParamValue("inputsamplingphase", IntToString(INPUT_SAMPLING_PHASE_1));
	newComp->SetParamValue("mode", IntToString(COMPARATOR_MODE_VARIABLE_REFERENCE));
	newComp->SetParamValue("hysteresis", IntToString(HYSTERESIS_0MV));
	newComp->SetParamValue("outputpolarity", IntToString(OUTPUT_POLARITY_NONINV));
	newComp->SetParamValue("vref", DoubleToString(m_sampleValue));

	Configure();
}

void FPAA_ApplicationDlg::OnAddGroundComparator()
{
	m_GeneralChip.SetCurrentActiveConfig(an_CUSTOM_VREFCOMPARATOR);

	Comparator_Module *newComp = new Comparator_Module();
	newComp->AddToEnd(newComp->GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)); // add to positive input
	newComp->SetParamValue("inputsamplingphase", IntToString(INPUT_SAMPLING_PHASE_1));
	newComp->SetParamValue("mode", IntToString(COMPARATOR_MODE_GROUND));
	newComp->SetParamValue("hysteresis", IntToString(HYSTERESIS_10MV));
	newComp->SetParamValue("outputpolarity", IntToString(OUTPUT_POLARITY_NONINV));
	newComp->SetParamValue("vref", DoubleToString(m_sampleValue));

	Configure();
}

void FPAA_ApplicationDlg::UpdateSampleControls()
{
   // Map the actual sample value to the range [0, 100] for the slider
   int percent = (int) (100.0 * (m_sampleValue - m_sampleMin) / (m_sampleMax - m_sampleMin));

   // Update the slider position
   m_sampleSlider.SetPos(percent);
   m_sampleSlider.Invalidate();

   // Format the text for the edit control
   CString value;
   value.Format("%.2f Units", m_sampleValue);

   // Update the edit control text
   m_sampleEdit.SetWindowText(value);

   if (m_GeneralChip.GetCurrentActiveConfig() == an_Gain_ID)
   {
	   m_GeneralChip.Gain_GainInv1.setGainInv(m_sampleValue);
	   m_GeneralChip.ExecuteReconfig(an_Gain_ID, AUTO_EXEC);
   }
   else if (m_GeneralChip.GetCurrentActiveConfig() == an_HalfRectifier_ID)
   {
	   m_GeneralChip.RectifierHold1.setGainRectHold(m_sampleValue);
	   m_GeneralChip.ExecuteReconfig(an_HalfRectifier_ID, AUTO_EXEC);
   }
   else if (m_GeneralChip.GetCurrentActiveConfig() == an_CUSTOM_INITIAL)
   {

   }
   else if (m_GeneralChip.GetCurrentActiveConfig() == an_CUSTOM_GAININV) // now outdated check, thanks to GetDerivedType()
   {
	   GainInv_Module *lastGain = dynamic_cast<GainInv_Module*>(Configurator::GetDefaultSystemOutput()->GetDefaultInputSite()->input->front());
	   if (lastGain)
	   {
		   lastGain->SetParamValue("gain", DoubleToString(m_sampleValue));
	   }

	   Reconfigure(lastGain->GetCurrentCab());
   }
   else if (m_GeneralChip.GetCurrentActiveConfig() == an_CUSTOM_VREFCOMPARATOR) // now outdated check, thanks to GetDerivedType()
   {
	   Comparator_Module *lastComp = dynamic_cast<Comparator_Module*>(Configurator::GetDefaultSystemOutput()->GetDefaultInputSite()->input->front());
	   if (lastComp)
	   {
		   lastComp->SetParamValue("vref", DoubleToString(m_sampleValue));
	   }

	   Reconfigure(lastComp->GetCurrentCab());
   }

}

// This function is used to handle the Enter key when the focus is on
// an edit control.
BOOL FPAA_ApplicationDlg::PreTranslateMessage(MSG* pMsg)
{
   bool eatMessage = true;

   if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
   {
      if (GetFocus() == &m_sampleEdit)
      {
         OnSampleEditChanged();
      }
	  else if (GetFocus() == &m_commandLine)
	  {
		  OnCommandEntered();
	  }
      else
      {
         eatMessage = false;
      }
   }
   // up key increments command history
   else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP)
   {
	   if (GetFocus() == &m_commandLine)
	   {
		   OnCommandHistoryIncremented();
	   }
	   else
	   {
		   eatMessage = false;
	   }
   }
   // down key decrements command history
   else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN)
   {
	   if (GetFocus() == &m_commandLine)
	   {
		   OnCommandHistoryDecremented();
	   }
	   else
	   {
		   eatMessage = false;
	   }
   }
   else
   {
      eatMessage = false;
   }

   if (eatMessage)
   {
      return true;
   }
   else
   {
      return CDialog::PreTranslateMessage(pMsg);
   }
}

// This function handles ALL horizontal sliders on the window.  If you need
// to handle vertical sliders, that should go in the OnVScroll message handler.
void FPAA_ApplicationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   // This switch determines which horizontal slider is changing
   switch (pScrollBar->GetDlgCtrlID())
   {
      case IDC_SAMPLE_SLIDER: 
      {
         // Get the current position of the slider
         int percent = ((CSliderCtrl*) pScrollBar)->GetPos();
         
         // Map the percent onto the actual sample value
         m_sampleValue = m_sampleMin + (percent / 100.0) * (m_sampleMax - m_sampleMin);

         // Synchronize the slider and the edit control
         UpdateSampleControls();

         break;
      }

      default:
         // Nothing to do
         break;
   }
   
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

// This function is called by MFC when the edit control loses focus.
void FPAA_ApplicationDlg::OnSampleEditChanged()
{
   // Get the value in the edit box
   CString text;
   m_sampleEdit.GetWindowText(text);

   // Convert to a double
   m_sampleValue = atof(text);

   // Ensure the value is within the allowable range
   m_sampleValue = max(min(m_sampleValue, m_sampleMax), m_sampleMin);

   // Synchronize the slider and the edit control
   UpdateSampleControls();
}

void FPAA_ApplicationDlg::OnCommandEntered()
{
	// Get the value in the edit box
	CString text;
	m_commandLine.GetWindowText(text);

	if (text.IsEmpty())
	{
		return;
	}

	// Clear edit box
	m_commandLine.SetWindowText("");

	// mark us as out of command history
	inCommandHistory = false;

	// Handle the command
	HandleCommand(text.GetString(), true);

	// Forward command to the client
	//std::string s = "Command: ";
	//s.append(text.GetString());
	//SendToClient(s);
	SendToClient(text.GetString());
}

void FPAA_ApplicationDlg::OnCommandHistoryIncremented()
{
	if (commandHistory.size() > 0)
	{
		if (inCommandHistory)
		{
			if (commandHistoryIterator != commandHistory.begin())
			{
				commandHistoryIterator--;
			}
		}
		else
		{
			inCommandHistory = true;
			commandHistoryIterator = commandHistory.end() - 1;
		}

		std::string newCommand = *commandHistoryIterator;

		// Set edit box
		m_commandLine.SetWindowText(newCommand.c_str());
	}
}

void FPAA_ApplicationDlg::OnCommandHistoryDecremented()
{
	if (commandHistory.size() > 0)
	{
		if (inCommandHistory)
		{
			commandHistoryIterator++;
		}

		std::string newCommand;
		if (commandHistoryIterator != commandHistory.end())
		{
			 newCommand = *commandHistoryIterator;
		}
		else
		{
			inCommandHistory = false;
			newCommand = "";
		}

		// Set edit box
		m_commandLine.SetWindowText(newCommand.c_str());
	}
}

void FPAA_ApplicationDlg::HandleCommand(std::string cmdString, bool fromCommandLine)
{
	// to lower case
	std::transform(cmdString.begin(), cmdString.end(), cmdString.begin(), ::tolower);

	// store the command in history
	if (fromCommandLine)
	{
		commandHistory.push_back(cmdString);
	}
	
	// split into tokens
	std::istringstream ss(cmdString);
	std::istream_iterator<std::string> begin(ss), end;
	std::vector<std::string> tokens(begin, end);

	std::string cmd = tokens[0];

	if (cmd.compare("reset") == 0)
	{
		OnReset();
	}
	else if (cmd.compare("capture") == 0)
	{
		Capture();
	}
	else if (cmd.compare("add") == 0)
	{
		if (tokens.size() < 2)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *newModule;
		BaseModule::ModuleInput *whichInputSiteOfThis = NULL;
		BaseModule::ModuleOutput *whichOutputSiteOfThis = NULL;
		BaseModule::ModuleOutput *whichOutputSiteFromIn = NULL;
		BaseModule::ModuleInput *whichInputSiteFromOut = NULL;
		std::string newModuleName = GetParamInTokenArray("name", tokens);

		if (tokens[1].compare("gaininv") == 0)
		{
			newModule = new GainInv_Module(newModuleName, newModuleName);
			GainInv_Module *gainInv = dynamic_cast<GainInv_Module*>(newModule);

			{
				// Set gain
				std::string gainParam = GetParamInTokenArray("gain", tokens);
				gainInv->SetParamValue("gain", (gainParam.compare("") != 0) ? gainParam : "1.0");
			}
		}
		else if (tokens[1].compare("comparator") == 0)
		{
			newModule = new Comparator_Module(newModuleName, newModuleName);
			Comparator_Module *comp = dynamic_cast<Comparator_Module*>(newModule);

			// Set mode
			std::string modeParam = GetParamInTokenArray("mode", tokens);
			comp->SetParamValue("mode", (modeParam.compare("") != 0) ? modeParam : IntToString(DEFAULT_COMPARATOR_MODE));

			// Set input sampling phase
			std::string phaseParam = GetParamInTokenArray("inputsamplingphase", tokens);
			comp->SetParamValue("inputsamplingphase", (phaseParam.compare("") != 0) ? phaseParam : IntToString(DEFAULT_INPUT_SAMPLING_PHASE));
	
			// Set output polarity
			std::string polarityParam = GetParamInTokenArray("outputpolarity", tokens);
			comp->SetParamValue("outputpolarity", (polarityParam.compare("") != 0) ? polarityParam : IntToString(DEFAULT_OUTPUT_POLARITY));

			// Set hysteresis
			std::string hysteresisParam = GetParamInTokenArray("hysteresis", tokens);
			comp->SetParamValue("hysteresis", (hysteresisParam.compare("") != 0) ? hysteresisParam : IntToString(DEFAULT_HYSTERESIS));

			// Set vref
			std::string vrefParam = GetParamInTokenArray("vref", tokens);
			comp->SetParamValue("vref", (vrefParam.compare("") != 0) ? vrefParam : "0");
		}
		else if (tokens[1].compare("filter") == 0)
		{
			newModule = new FilterBiquad_Module(newModuleName, newModuleName);
			FilterBiquad_Module *filter = dynamic_cast<FilterBiquad_Module*>(newModule);

			// Set filter type
			std::string filterTypeParam = GetParamInTokenArray("filtertype", tokens);
			filter->SetParamValue("filtertype", (filterTypeParam.compare("") != 0) ? filterTypeParam : IntToString(DEFAULT_FILTER_TYPE));

			// Set input sampling phase
			std::string phaseParam = GetParamInTokenArray("inputsamplingphase", tokens);
			filter->SetParamValue("inputsamplingphase", (phaseParam.compare("") != 0) ? phaseParam : IntToString(DEFAULT_INPUT_SAMPLING_PHASE));

			// Set output polarity
			std::string polarityParam = GetParamInTokenArray("outputpolarity", tokens);
			filter->SetParamValue("outputpolarity", (polarityParam.compare("") != 0) ? polarityParam : IntToString(DEFAULT_OUTPUT_POLARITY));

			// Set gain
			std::string gainParam = GetParamInTokenArray("gain", tokens);
			filter->SetParamValue("gain", (gainParam.compare("") != 0) ? gainParam : "1.0");

			// Set corner frequency
			std::string cornerFrequencyParam = GetParamInTokenArray("cornerfrequency", tokens);
			filter->SetParamValue("cornerfrequency", (cornerFrequencyParam.compare("") != 0) ? cornerFrequencyParam : DoubleToString(DEFAULT_FILTER_CORNER_FREQUENCY));

			// Set q
			std::string qParam = GetParamInTokenArray("q", tokens);
			filter->SetParamValue("q", (qParam.compare("") != 0) ? qParam : DoubleToString(DEFAULT_FILTER_Q));
		}
		else if (tokens[1].compare("sum") == 0)
		{
			newModule = new SumFilter_Module(newModuleName, newModuleName);
			SumFilter_Module *sum = dynamic_cast<SumFilter_Module*>(newModule);

			// Set input 1 sign
			std::string input1SignParam = GetParamInTokenArray("input1sign", tokens);
			sum->SetParamValue("input1sign", (input1SignParam.compare("") != 0) ? input1SignParam : IntToString(OUTPUT_POLARITY_NONINV));

			// Set input 2 sign
			std::string input2SignParam = GetParamInTokenArray("input2sign", tokens);
			sum->SetParamValue("input2sign", (input2SignParam.compare("") != 0) ? input2SignParam : IntToString(OUTPUT_POLARITY_NONINV));

			// Set output changes on
			std::string outputParam = GetParamInTokenArray("outputchangeson", tokens);
			sum->SetParamValue("outputchangeson", (outputParam.compare("") != 0) ? outputParam : IntToString(DEFAULT_INPUT_SAMPLING_PHASE));

			// Set gain1
			std::string gain1Param = GetParamInTokenArray("gain1", tokens);
			sum->SetParamValue("gain1", (gain1Param.compare("") != 0) ? gain1Param : "1.0");

			// Set gain2
			std::string gain2Param = GetParamInTokenArray("gain2", tokens);
			sum->SetParamValue("gain2", (gain2Param.compare("") != 0) ? gain2Param : "1.0");

			// Set corner frequency
			std::string cornerFrequencyParam = GetParamInTokenArray("cornerfrequency", tokens);
			sum->SetParamValue("cornerfrequency", (cornerFrequencyParam.compare("") != 0) ? cornerFrequencyParam : DoubleToString(DEFAULT_SUMDIFF_CORNER_FREQUENCY));
		}
		else if (tokens[1].compare("difference") == 0)
		{
			newModule = new SumFilter_Module(newModuleName, newModuleName);
			SumFilter_Module *diff = dynamic_cast<SumFilter_Module*>(newModule);

			// Set input 1 sign
			std::string input1SignParam = GetParamInTokenArray("input1sign", tokens);
			diff->SetParamValue("input1sign", (input1SignParam.compare("") != 0) ? input1SignParam : IntToString(OUTPUT_POLARITY_NONINV));

			// Set input 2 sign
			std::string input2SignParam = GetParamInTokenArray("input2sign", tokens);
			diff->SetParamValue("input2sign", (input2SignParam.compare("") != 0) ? input2SignParam : IntToString(OUTPUT_POLARITY_INV));

			// Set output changes on
			std::string outputParam = GetParamInTokenArray("outputchangeson", tokens);
			diff->SetParamValue("outputchangeson", (outputParam.compare("") != 0) ? outputParam : IntToString(DEFAULT_INPUT_SAMPLING_PHASE));

			// Set gain1
			std::string gain1Param = GetParamInTokenArray("gain1", tokens);
			diff->SetParamValue("gain1", (gain1Param.compare("") != 0) ? gain1Param : "1.0");

			// Set gain2
			std::string gain2Param = GetParamInTokenArray("gain2", tokens);
			diff->SetParamValue("gain2", (gain2Param.compare("") != 0) ? gain2Param : "1.0");

			// Set corner frequency
			std::string cornerFrequencyParam = GetParamInTokenArray("cornerfrequency", tokens);
			diff->SetParamValue("cornerfrequency", (cornerFrequencyParam.compare("") != 0) ? cornerFrequencyParam : DoubleToString(DEFAULT_SUMDIFF_CORNER_FREQUENCY));
		}
		else
		{
			Message::ErrorMessage("Unknown argument for command: " + cmd + ", arg: " + tokens[1]);
			return;
		}

		BaseModule *insertAfter = NULL;
		BaseModule *insertBefore = NULL;

		std::string insertAfterName = GetParamInTokenArray("after", tokens);
		std::string insertBeforeName = GetParamInTokenArray("before", tokens);

		// if both are named, insert between
		if ((insertAfterName.compare("") != 0) && (insertBeforeName.compare("") != 0))
		{
			insertAfter = Configurator::GetModuleWithName(insertAfterName);
			insertBefore = Configurator::GetModuleWithName(insertBeforeName);
			if ((insertAfter != NULL) && (insertBefore != NULL))
			{
				newModule->AddBetween(insertAfter, insertBefore, whichInputSiteOfThis, whichOutputSiteOfThis, whichOutputSiteFromIn, whichInputSiteFromOut); // add specific site selectors
			}
			else
			{
				Message::ErrorMessage("Could not find modules with names " + insertAfterName + " and " + insertBeforeName);
				return;
			}
		}
		else if (insertAfterName.compare("") != 0)
		{
			insertAfter = Configurator::GetModuleWithName(insertAfterName);
			if (insertAfter != NULL)
			{
				newModule->ConnectToOutput(insertAfter, whichInputSiteOfThis, whichOutputSiteFromIn);
			}
			else
			{
				Message::ErrorMessage("Could not find module with name " + insertAfterName);
				return;
			}
		}
		else if (insertBeforeName.compare("") != 0)
		{
			insertBefore = Configurator::GetModuleWithName(insertBeforeName);
			if (insertBefore != NULL)
			{
				insertBefore->ConnectToOutput(newModule, whichInputSiteFromOut, whichOutputSiteOfThis);
			}
			else
			{
				Message::ErrorMessage("Could not find module with name " + insertBeforeName);
				return;
			}
		}
		// if neither are named, add to the default end
		else
		{
			//newModule->AddToEnd(whichInputSiteOfThis, whichOutputSiteOfThis);
		}

		Configure();
	}
	// Remove a named module. The 1st argument should be the module's name.
	// Note that this will leave the inputs and outputs to that module unattached
	else if (cmd.compare("remove") == 0)
	{
		if (tokens.size() < 2)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *toRemove = Configurator::GetModuleWithName(tokens[1]);

		if (toRemove == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to remove.");
			return;
		}

		// Remove the module as the output of all of its inputs
		for (auto it = toRemove->GetInputs().begin(); it != toRemove->GetInputs().end(); it++)
		{
			for (auto it2 = (*it)->input->begin(); it2 != (*it)->input->end(); it2++)
			{
				if (*it2 != NULL)
				{
					(*it2)->RemoveOutput(toRemove);
				}
			}
		}
		// Remove the module as the input of all of its outputs
		for (auto it = toRemove->GetOutputs().begin(); it != toRemove->GetOutputs().end(); it++)
		{
			for (auto it2 = (*it)->output->begin(); it2 != (*it)->output->end(); it2++)
			{
				if (*it2 != NULL)
				{
					(*it2)->RemoveInput(toRemove);
				}
			}
		}

		// Remove the module from the configurator
		Configurator::RemoveModule(toRemove);

		Configure();
	}
	else if (cmd.compare("set") == 0)
	{
		if (tokens.size() < 4)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *toSet = Configurator::GetModuleWithName(tokens[1]);

		if (toSet == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to set.");
			return;
		}

		toSet->SetParamValue(tokens[2], tokens[3]);

		if ((tokens.size() >= 5) && (tokens[4].compare("noflash") == 0))
		{
			Reconfigure(toSet->GetCurrentCab());
		}
		else
		{
			Configure();
		}
	}
	else if (cmd.compare("connect") == 0)
	{
		if (tokens.size() < 3)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *outputModule = Configurator::GetModuleWithName(tokens[1]);
		BaseModule *inputModule = Configurator::GetModuleWithName(tokens[2]);

		if (outputModule == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to connect.");
			return;
		}
		if (inputModule == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[2] + " to connect.");
			return;
		}

		std::string inputSiteName = GetParamInTokenArray("inputsite", tokens);
		std::string outputSiteName = GetParamInTokenArray("outputsite", tokens);
		BaseModule::ModuleInput *inputSite = NULL;
		BaseModule::ModuleOutput *outputSite = NULL;

		if (inputSiteName.compare("") != 0)
		{
			inputSite = inputModule->GetInputSiteByName(inputSiteName);
		}
		if (outputSiteName.compare("") != 0)
		{
			outputSite = outputModule->GetOutputSiteByName(outputSiteName);
		}

		inputModule->ConnectToOutput(outputModule, inputSite, outputSite);

		Configure();
	}
	else if ((cmd.compare("disconnect") == 0) || (cmd.compare("unconnect") == 0))
	{
		if (tokens.size() < 3)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *outputModule = Configurator::GetModuleWithName(tokens[1]);
		BaseModule *inputModule = Configurator::GetModuleWithName(tokens[2]);

		if (outputModule == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to disconnect.");
			return;
		}
		if (inputModule == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[2] + " to disconnect.");
			return;
		}

		std::string inputSiteName = GetParamInTokenArray("inputsite", tokens);
		std::string outputSiteName = GetParamInTokenArray("outputsite", tokens);
		BaseModule::ModuleInput *inputSite = NULL;
		BaseModule::ModuleOutput *outputSite = NULL;

		if (inputSiteName.compare("") != 0)
		{
			inputSite = inputModule->GetInputSiteByName(inputSiteName);
		}
		if (outputSiteName.compare("") != 0)
		{
			outputSite = outputModule->GetOutputSiteByName(outputSiteName);
		}

		outputModule->RemoveOutput(inputModule);
		inputModule->RemoveInput(outputModule);

		Configure();
	}
	else if (cmd.compare("probe") == 0)
	{
		if (tokens.size() < 2)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *outputModule = Configurator::GetModuleWithName(tokens[1]);

		if (outputModule == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to probe.");
			return;
		}

		std::string outputSiteName = GetParamInTokenArray("outputsite", tokens);
		BaseModule::ModuleOutput *outputSite = NULL;

		if (outputSiteName.compare("") != 0)
		{
			outputSite = outputModule->GetOutputSiteByName(outputSiteName);
		}

		BaseModule *inputModule = Configurator::GetDefaultSystemProbe();
		std::string probeName = GetParamInTokenArray("probename", tokens);
		if (probeName.compare("") != 0)
		{
			inputModule = Configurator::GetModuleWithName(probeName);
		}

		// Remove the probe as the output of all of its inputs
		// then remove all inputs of the probe
		std::vector<BaseModule*> toRemove;
		for (auto it = inputModule->GetInputs().begin(); it != inputModule->GetInputs().end(); it++)
		{
			for (auto it2 = (*it)->input->begin(); it2 != (*it)->input->end(); it2++)
			{
				if (*it2 != NULL)
				{
					(*it2)->RemoveOutput(inputModule);
					toRemove.push_back(*it2);
				}
			}
		}
		for (auto it = toRemove.begin(); it != toRemove.end(); it++)
		{
			inputModule->RemoveInput(*it);
		}

		inputModule->ConnectToOutput(outputModule, inputModule->GetDefaultInputSite(), outputSite);

		Configure();

		// Tell the client that we've updated the circuit, and that we're ready to capture
		SendToClient("capture " + tokens[1]);
	}
	else if (cmd.compare("unprobe") == 0)
	{
		BaseModule *inputModule = Configurator::GetDefaultSystemProbe();
		std::string probeName = GetParamInTokenArray("probename", tokens);
		if (probeName.compare("") != 0)
		{
			inputModule = Configurator::GetModuleWithName(probeName);
		}

		// Remove the probe as the output of all of its inputs
		// then remove all inputs of the probe
		std::vector<BaseModule*> toRemove;
		for (auto it = inputModule->GetInputs().begin(); it != inputModule->GetInputs().end(); it++)
		{
			for (auto it2 = (*it)->input->begin(); it2 != (*it)->input->end(); it2++)
			{
				if (*it2 != NULL)
				{
					(*it2)->RemoveOutput(inputModule);
					toRemove.push_back(*it2);
				}
			}
		}
		for (auto it = toRemove.begin(); it != toRemove.end(); it++)
		{
			inputModule->RemoveInput(*it);
		}

		Configure();

		// Tell the client that we've updated the circuit, and that we're ready to capture
		SendToClient("capture");
	}
	else if (cmd.compare("rename") == 0)
	{
		if (tokens.size() < 3)
		{
			Message::ErrorMessage("Not enough arguments for command: " + cmd);
			return;
		}
		BaseModule *toRename = Configurator::GetModuleWithName(tokens[1]);

		if (toRename == NULL)
		{
			Message::ErrorMessage("Could not find a module named " + tokens[1] + " to rename.");
			return;
		}

		toRename->SetModuleName(tokens[2]);
		toRename->SetSimpleName(tokens[2]);

		Configure();
	}
	else if ((cmd.compare("help") == 0) || (cmd.compare("?") == 0))
	{
		Message::ErrorMessage(GetHelpText());
	}
	else if (cmd.compare("check") == 0)
	{

	}
	else if ((cmd.compare("exit") == 0) || (cmd.compare("close") == 0))
	{
		Exit();
	}
	else 
	{
		Message::ErrorMessage("Unknown command: " + cmd);
		return;
	}
}

std::string FPAA_ApplicationDlg::GetParamInTokenArray(std::string paramName, std::vector<std::string> tokens)
{
	std::string returnString = "";
	for (auto it = tokens.begin(); it != tokens.end(); it++)
	{
		if ((*it).compare(paramName) == 0)
		{
			it++;
			if (it != tokens.end())
			{
				returnString = *it;
			}
			break;
		}
	}

	return returnString;
}

void FPAA_ApplicationDlg::Configure()
{
	Configurator::ClearConfigurationData();
	an_Byte configData[CONFIG_DATA_SIZE] = { 0 };
	Configurator::GenerateConfigurationData(configData);
#ifndef DEBUG_WITHOUT_HARDWARE
	DownloadManager::Instance()->DownloadBits(configData, CONFIG_DATA_SIZE, RESET_FIRST);
#endif // DEBUG_WITHOUT_HARDWARE
	//VisualizeCircuit();

	//LogFormattedConfigData(configData);
}

void FPAA_ApplicationDlg::Reconfigure(uint8_t whichCab)
{
	Configurator::ClearConfigurationData();
	an_Byte reconfigData[RECONFIG_DATA_SIZE] = { 0 };
	Configurator::GenerateReconfigurationData(reconfigData, whichCab);
#ifndef DEBUG_WITHOUT_HARDWARE
	DownloadManager::Instance()->DownloadBits(reconfigData, RECONFIG_DATA_SIZE);
#endif // DEBUG_WITHOUT_HARDWARE
	//VisualizeCircuit();

	//LogHexData(reconfigData, RECONFIG_DATA_SIZE);
}

std::string FPAA_ApplicationDlg::GetHelpText()
{
	std::stringstream s;

	s << "Commands:\n\n";
	s << "- reset\n";
	s << "- add <module type> (name <name>) (param <param>...) (outputsite <output site name>) (inputsite <input site name>)\n";
	s << "- remove <module name>\n";
	s << "- rename <module name> <new name>\n";
	s << "- set <param name> <new value>\n";
	s << "- connect <outputting module name> <inputting module name> (outputsite <output site name>) (inputsite <input site name>)\n";
	s << "- disconnect <outputting module name> <inputting module name> (outputsite <output site name>) (inputsite <input site name>)\n";
	s << "- probe <name of module to probe> (outputsite <output site name>) (probename <probe name>)\n";
	s << "- pause\n";
	s << "- resume\n";
	s << "- record <number of samples>\n";
	s << "- exit\n";
	s << "- help\n";
	
	return s.str();
}

void FPAA_ApplicationDlg::VisualizeCircuit()
{
	std::stringstream s;
	std::vector<BaseModule*> orderedModules = Configurator::GetOrderedModules();
	
	for (auto it = orderedModules.begin(); it != orderedModules.end(); it++)
	{
		VisualizeNextOutput(*it, s, 0);
		s << "\n";
	}
	
	SetDlgItemText(IDC_CIRCUIT_VISUALIZATION, s.str().c_str());
}

void FPAA_ApplicationDlg::VisualizeNextOutput(BaseModule *startingPoint, std::stringstream &s, int numSpaces)
{
	s << startingPoint->GetModuleName();
	s << startingPoint->GetModuleInformation();
	s << " --> ";
	numSpaces += startingPoint->GetModuleName().length() + startingPoint->GetModuleInformation().length() + 5;
	if (startingPoint->GetDefaultOutputSite()->output != NULL)
	{
		for (auto it = startingPoint->GetOutputs().begin(); it != startingPoint->GetOutputs().end(); it++)
		{
			for (auto it2 = (*it)->output->begin(); it2 != (*it)->output->end(); it2++)
			{
				BaseModule *outputModule = *it2;
				if (outputModule != NULL)
				{
					// indent all but the first
					if (it2 != (*(startingPoint->GetOutputs().begin()))->output->begin())
					{
						s << "\n";
						for (int i = 0; i < numSpaces; i++)
						{
							s << " ";
						}
					}
					VisualizeNextOutput(outputModule, s, numSpaces);
				}
			}
		}
	}
}
