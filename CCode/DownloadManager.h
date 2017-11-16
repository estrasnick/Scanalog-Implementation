#pragma once

#include "Port.h"
#include "PortCheck.h"

class DownloadManager
{
public:
   static DownloadManager* Instance();
   ~DownloadManager();

   // Download an array of bytes to the active port
   //void InitializeChips(const BYTE* loadOrder, int chipCount);  Dave Lovell remmed this out
    void DownloadBits(const BYTE* data, int dataSize, bool isReconfig = true);

   // For multi-device scenarios
   void InitializeChips(const BYTE* loadOrder, int chipCount);
   void ResetChips();

   // Port functions
   void OpenPort();
   void ClosePort();
   bool ShowPortDlg();
   bool IsPortOpen();

   // Save \ restore port
   void LoadPortSettings(CString regKey);
   void SavePortSettings(CString regKey);

private:
   DownloadManager();

   void HandleError(CString error);

   Port m_port;
   bool m_throwOnError;

   static DownloadManager m_instance;
};
