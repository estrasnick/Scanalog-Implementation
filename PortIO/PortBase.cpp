#include "stdafx.h"
#include "PortBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


PortBase::PortBase() : m_strLastError(_T("Success"))
{
}

PortBase::~PortBase()
{
}