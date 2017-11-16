#pragma once

#include "resource.h"
#include "GeneralChip.h"
#include "BaseModule.h"
#include "clPlot.h"
#include "FPAA_Socket.h"
#include <string>
#include <afxsock.h>

// forward declarations to resolve cyclic dependencies
class FPAA_Socket;

class FPAA_ApplicationDlg : public CDialog
{
public:
	clPlot m_Plot;
	bool canSize = false;
	FPAA_ApplicationDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_FPAA_Application_DIALOG };

	void HandleCommand(std::string cmdString, bool fromCommandLine);
	void SetClientSocket(FPAA_Socket *s);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

   // This is used to handle the Enter key when the focus is on an edit control.
   virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

   afx_msg void OnPort();
   afx_msg void OnDownloadGain();
   afx_msg void OnDownloadHalfRectifier();
   afx_msg void OnDownloadTestConfig();
   afx_msg void OnAddGain();
   afx_msg void OnAddVrefComparator();
   afx_msg void OnAddGroundComparator();
   afx_msg void OnReset();
   afx_msg void OnAbout();
   afx_msg void OnCommandEntered();
   afx_msg void OnCommandHistoryIncremented();
   afx_msg void OnCommandHistoryDecremented();

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSampleEditChanged();

   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg LRESULT OnSocketReceive(WPARAM wParam, LPARAM lParam);


private:
   void ConfigureAboutMenu();
   void UpdateSampleControls();
   void Configure();
   void Reconfigure(uint8_t whichCab);
   void VisualizeCircuit();
   void VisualizeNextOutput(BaseModule *startingPoint, std::stringstream &s, int numSpaces);
   void InitScope();
   void Capture();
   void Acquire();
   void Exit();
   void CreatePlot();
   void DisplayCapture(double d[]);
   void SendToClient(std::string s);
   std::string GetParamInTokenArray(std::string paramName, std::vector<std::string> tokens);
   std::string GetHelpText();

   FPAA_Socket *serverSocket;
   FPAA_Socket *clientSocket = NULL;
   bool clientConnected = false;

   bool autocapture = false;
   bool autoscroll = false;
   double lastCaptureTime;
   int numSamples;
   int samplingRate;
   int prevNumSamples;
   int prevSamplingRate;

   std::vector<std::string> commandHistory;
   std::vector<std::string>::iterator commandHistoryIterator;
   bool inCommandHistory = false;

	HICON m_hIcon;
   GeneralChip m_GeneralChip;

   CEdit m_sampleEdit;
   CSliderCtrl m_sampleSlider;
   CStatic m_circuitVisualization;
   CEdit m_commandLine;
   CRect m_plotRect;

   double m_sampleValue;
   double m_sampleMin;
   double m_sampleMax;

	DECLARE_MESSAGE_MAP()
};
