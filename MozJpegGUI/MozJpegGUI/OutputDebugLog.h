#pragma once

/// <summary>
/// Initialize OutputDebugLog. If not initialized, OutputDebugLog() is disabled.
/// </summary>
/// <param name="filename"></param>
void OutputDebugLogInit(CString filename);

/// <summary>
/// Output debug log to a file.
/// </summary>
/// <param name="str">An text written to a file. \n is not automatically added.</param>
void OutputDebugLog(CString str);
