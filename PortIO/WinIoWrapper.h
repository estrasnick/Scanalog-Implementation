#pragma once

typedef bool (__stdcall pfnInitializeWinIo)();
typedef void (__stdcall pfnShutdownWinIo)();
typedef bool (_stdcall pfnGetPortVal)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
typedef bool (_stdcall pfnSetPortVal)(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);

class WinIoWrapper
{
public:
   ~WinIoWrapper(void);

   static WinIoWrapper* Instance();

   bool Initialize();
   void Shutdown();
   bool GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
   bool SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);

   bool IsInitialized();

private:
   WinIoWrapper();
   
   void EnsureWinIoExists();
   void LoadWinIoLibrary();
   void CreateWinIoFile(CString resource, CString path);

   bool m_initialized;

   pfnInitializeWinIo* m_pfnInitializeWinIo;
   pfnShutdownWinIo* m_pfnShutdownWinIo;
   pfnGetPortVal* m_pfnGetPortVal;
   pfnSetPortVal* m_pfnSetPortVal;

   HMODULE m_hWinIO;

   static WinIoWrapper m_instance;
};
