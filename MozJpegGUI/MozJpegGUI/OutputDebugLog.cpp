#include "pch.h"
#include "OutputDebugLog.h"
#include "CAboutDlg.h"
#include "PathCch.h"

bool g_Init;
CStdioFile g_Log;

/// <summary>
/// Get version info of the file, and output in CString form.
/// </summary>
/// <param name="filename">target filename</param>
/// <returns>%filename% %version% (path is removed from %filename%</returns>
CString GetFileVersion(CString filename) {
	CString ret=_T("No version info");

	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(filename, &verHandle);

	if (verSize != NULL) {
		LPSTR verData = new char[verSize];
		if (GetFileVersionInfo(filename, verHandle, verSize, verData)) {
			if (VerQueryValue(verData, _T("\\"), (VOID FAR * FAR*) & lpBuffer, &size)) {
				if (size) {
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd) {
						ret.Format(_T("%s %d.%d.%d.%d"), filename,
							(verInfo->dwFileVersionMS >> 16) & 0xffff,
							(verInfo->dwFileVersionMS >> 0) & 0xffff,
							(verInfo->dwFileVersionLS >> 16) & 0xffff,
							(verInfo->dwFileVersionLS >> 0) & 0xffff
						);
					}
				}
			}
		}
		delete[] verData;
	}
	return ret;
}


void OutputDebugLogInit(CString filename)
{
	if (g_Init) {
		MessageBox(NULL, _T("Bug! OutputDebugLogInit is called multiple times. Ignoring current call."), _T("MozJpegGUI bug"), MB_OK);
		return;
	}

	if (g_Log.Open(filename, CFile::modeWrite | CFile::typeText | CFile::modeCreate | CFile::typeUnicode) == 0) {
		_RPTTN(_T("Failed to create log file: %s\n"), filename);
		MessageBox(NULL, _T("Failed to create log file. The software will not log its status."), _T("Error"), MB_OK);
		return;
	}
	g_Init = true;

	OutputDebugLog(_T("Start logging\n"));
	OutputDebugLog(CAboutDlg::GetAppVersion() + _T("\n"));

	const int BUFFSIZE = _MAX_PATH + 1;
	TCHAR buff[BUFFSIZE];
	GetModuleFileName(NULL, buff, BUFFSIZE);
	PathCchRemoveFileSpec(buff, BUFFSIZE);
	CString target = _T("MozJpegGUIENU.dll");
	PathCchAppend(buff, BUFFSIZE, target);
	if (PathFileExists(buff)) {
		OutputDebugLog(GetFileVersion(target) + _T("\n"));
	}
	PathCchRemoveFileSpec(buff, BUFFSIZE);
	target = _T("MozJpegGUIJPN.dll");
	PathCchAppend(buff, BUFFSIZE, target);
	if (PathFileExists(target)) {
		OutputDebugLog(GetFileVersion(target) + _T("\n"));
	}
}

void OutputDebugLog(CString str, ...)
{
	if (!g_Init)
		return;
	
	// generate output string
	CString buff;
	va_list list;
	va_start(list, str);
	buff.FormatV(str, list);
	va_end(list);
	
	// write out the string
	try {
		g_Log.WriteString(buff);
		g_Log.Flush();
	}
	catch (const CFileException& e) {
		const int BUFFSIZE = 1024;
		TCHAR buff[BUFFSIZE];
		BOOL ret = false;
		ret = e.GetErrorMessage(buff, BUFFSIZE);
		if (ret != TRUE) {
			MessageBox(NULL, _T("Failed to get error message in OutputDebugLog. Continue."), _T("Error"), MB_OK);
		}
		else {
			MessageBox(NULL, buff, _T("MozJpegGUI error"), MB_OK);
		}
		MessageBox(NULL, _T("Logging stopped."), _T("MozJpegGUI"), MB_OK);
		g_Init = false;
	}
}
