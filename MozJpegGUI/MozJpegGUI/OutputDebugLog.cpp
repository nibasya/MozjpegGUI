#include "pch.h"
#include "OutputDebugLog.h"

bool g_Init;
CFile g_Log;

void OutputDebugLogInit(CString filename)
{
	if (g_Init) {
		MessageBox(NULL, _T("Bug! OutputDebugLogInit is called multiple times. Ignoring current call."), _T("MozJpegGUI bug"), MB_OK);
		return;
	}

	if (g_Log.Open(filename, CFile::modeWrite | CFile::typeText | CFile::modeCreate) == 0) {
		_RPTTN(_T("Failed to create log file: %s\n"), filename);
		MessageBox(NULL, _T("Failed to create log file. The software will not log its status."), _T("Error"), MB_OK);
		return;
	}
	g_Init = true;
}

void OutputDebugLog(CString str)
{
	if (!g_Init)
		return;

	try {
		g_Log.Write(str.GetBuffer(), str.GetLength());
	}
	catch (const CFileException& e) {
		const int BUFFSIZE = 1024;
		TCHAR buff[BUFFSIZE];
		BOOL ret = false;
		ret = e.GetErrorMessage(buff, BUFFSIZE);
		if (ret != TRUE) {
			MessageBox(NULL, _T("Failed to get error message in OutputDebugLog. Contine."), _T("Error"), MB_OK);
		}
		else {
			MessageBox(NULL, buff, _T("MozJpegGUI error"), MB_OK);
		}
		MessageBox(NULL, _T("Logging stopped."), _T("MozJpegGUI"), MB_OK);
		g_Init = false;
	}
}
