#include "stdafx.h"
#include "WinIoWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WinIoWrapper WinIoWrapper::m_instance;

WinIoWrapper::WinIoWrapper() : 
   m_initialized(false),
   m_hWinIO(NULL)
{
   m_pfnInitializeWinIo = NULL;
   m_pfnShutdownWinIo = NULL;
   m_pfnGetPortVal = NULL;
   m_pfnSetPortVal = NULL;
}

WinIoWrapper::~WinIoWrapper()
{

}

WinIoWrapper* WinIoWrapper::Instance()
{
   return &m_instance;
}

bool WinIoWrapper::Initialize()
{
   EnsureWinIoExists();
   LoadWinIoLibrary();

   if (m_hWinIO && !m_initialized)
   {
      m_initialized = m_pfnInitializeWinIo();

      if (!m_initialized)
      {
         Sleep(500);
         m_initialized = m_pfnInitializeWinIo();
      }
   }

   return m_initialized;
}

void WinIoWrapper::Shutdown()
{
   if (m_initialized)
   {
      m_pfnShutdownWinIo();
   }

   if (m_hWinIO)
   {
      FreeLibrary(m_hWinIO);

      m_hWinIO = NULL;
      m_pfnInitializeWinIo = NULL;
      m_pfnShutdownWinIo = NULL;
      m_pfnGetPortVal = NULL;
      m_pfnSetPortVal = NULL;
   }

   m_initialized = false;
}

bool WinIoWrapper::GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize)
{
   if (!m_initialized)
   {
      return false;
   }

   return m_pfnGetPortVal(wPortAddr, pdwPortVal, bSize);
}

bool WinIoWrapper::SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize)
{
   if (!m_initialized)
   {
      return false;
   }

   return m_pfnSetPortVal(wPortAddr, dwPortVal, bSize);
}

bool WinIoWrapper::IsInitialized()
{
   return m_initialized;
}

void WinIoWrapper::LoadWinIoLibrary()
{
   if (m_hWinIO != NULL)
   {
      return;
   }

   // Find the program directory
	char lpAppPath[MAX_PATH];
	GetModuleFileName(NULL, lpAppPath, sizeof(lpAppPath));
	CString appPath = lpAppPath;
	appPath = appPath.Left(appPath.ReverseFind('\\')) + "\\";

   m_hWinIO = ::LoadLibrary(appPath + "WinIO.dll");

   if (!m_hWinIO)
   {
      return;
   }

   m_pfnInitializeWinIo = (pfnInitializeWinIo*) ::GetProcAddress(m_hWinIO, "InitializeWinIo");
   m_pfnShutdownWinIo = (pfnShutdownWinIo*) ::GetProcAddress(m_hWinIO, "ShutdownWinIo");
   m_pfnGetPortVal = (pfnGetPortVal*) ::GetProcAddress(m_hWinIO, "GetPortVal");
   m_pfnSetPortVal = (pfnSetPortVal*) ::GetProcAddress(m_hWinIO, "SetPortVal");
}

void WinIoWrapper::EnsureWinIoExists()
{
	// Find the program directory
	char lpAppPath[MAX_PATH];
	GetModuleFileName(NULL, lpAppPath, sizeof(lpAppPath));
	CString appPath = lpAppPath;
	appPath = appPath.Left(appPath.ReverseFind('\\')) + "\\";

   CFileFind findWinIo;

   // WinIo.dll
   if (!findWinIo.FindFile(appPath + "WinIo.dll"))
   {
      CreateWinIoFile("WinIo.dll", appPath + "WinIo.dll");
   }

   // WinIo.sys
   if (!findWinIo.FindFile(appPath + "WinIo.sys"))
   {
      CreateWinIoFile("WinIo.sys", appPath + "WinIo.sys");
   }

   // WinIo.vxd
   if (!findWinIo.FindFile(appPath + "WinIo.vxd"))
   {
      CreateWinIoFile("WinIo.vxd", appPath + "WinIo.vxd");
   }
}

void WinIoWrapper::CreateWinIoFile(CString resource, CString path)
{
   HRSRC hRC = ::FindResource(NULL, resource, "WinIo");

   BYTE* bytes = NULL;
   int length = 0;

   if (hRC)
   {
      HGLOBAL hGlobal = ::LoadResource(NULL, hRC);

      if (hGlobal)
      {
         BYTE* resBytes = (BYTE*) ::LockResource(hGlobal);
         length = SizeofResource(NULL, hRC);

         bytes = new BYTE[length];
         CopyMemory(bytes, resBytes, length);

         try
         {
            CFile file(path, CFile::modeWrite | CFile::modeCreate);
            file.Write(bytes, length);
         }
         catch (CException* e)
         {
            char error[512] = {'\0'};
            e->GetErrorMessage(error, 512);
            e->Delete();
         }

         delete[] bytes;
      }
   }
}