#pragma once

#ifndef _DEBUG
	#define _RPTT0(msg)
	#define _RPTTN(msg, ...)
	#define _RPTFT0(msg)
	#define _RPTFTN(msg, ...)
#else	// ifndef _DEBUG else
	#ifdef UNICODE
		#define _RPTT0(msg)      _RPT_BASE_W(_CRT_WARN, NULL, 0, NULL, L"%ls", msg)
		#define _RPTTN(msg, ...) _RPT_BASE_W(_CRT_WARN, NULL, 0, NULL, msg, __VA_ARGS__)
		#define _RPTFT0(msg)      _RPT_BASE_W(_CRT_WARN, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%ls", msg)
		#define _RPTFTN(msg, ...) _RPT_BASE_W(_CRT_WARN, _CRT_WIDE(__FILE__), __LINE__, NULL, msg, __VA_ARGS__)
	#else	// ifdef UNICODE else
		#define _RPTT0(msg)      _RPT_BASE(_CRT_WARN, NULL, 0, NULL, "%s", msg)
		#define _RPTTN(msg, ...) _RPT_BASE(_CRT_WARN, NULL, 0, NULL, msg, __VA_ARGS__)
		#define _RPTFT0(msg)      _RPT_BASE(_CRT_WARN, __FILE__, __LINE__, NULL, "%s", msg)
		#define _RPTFTN(msg, ...) _RPT_BASE(_CRT_WARN, __FILE__, __LINE__, NULL, msg, __VA_ARGS__)
	#endif	// ifdef UNICODE
#endif	// ifndef _DEBUG
