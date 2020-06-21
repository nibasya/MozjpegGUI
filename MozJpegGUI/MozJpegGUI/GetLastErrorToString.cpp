	#include "pch.h"	// enable for visual studio version >= 2019
	// #include "stdafx.h"	// enable for visual studio version < 2019
#include "GetLastErrorToString.h"

GetLastErrorToString::operator CString()
{
	CreateMsg();
	return m_msg;
}

GetLastErrorToString::operator LPCTSTR()
{
	CreateMsg();
	return (LPCTSTR)m_msg;
}

void GetLastErrorToString::CreateMsg()
{
	LPTSTR *buff;
	m_ID = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, m_ID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buff, 10, NULL);
	m_msg.Format(_T("%s"), (TCHAR*)buff);
	LocalFree(buff);
#ifdef UNICODE
	_RPTW2(_CRT_WARN, _T("Error ID: %d Msg: %s"), m_ID, m_msg);
#else
	_RPT2(_CRT_WARN, _T("Error ID: %d Msg: %s"), m_ID, m_msg);
#endif
}
