#pragma once

#define WINVER 0x0501

// Exclude rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		
#endif

// Some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	

// Turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>

#include <afxdtctl.h>
#include <afxcmn.h>

#include "ApiCode.h"
#include "CAMCode.h"
