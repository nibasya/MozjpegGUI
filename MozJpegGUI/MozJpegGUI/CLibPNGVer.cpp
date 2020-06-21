#include "pch.h"
#include <png.h>
#include <zlib.h>
#include "CLibPNGVer.h"

CString CLibPNGVer::GetLibPNGVer()
{
	return CString(_T(PNG_HEADER_VERSION_STRING));
}

CString CLibPNGVer::GetZLibVer()
{
	return CString(TEXT(" zlib version ")) + CString(CA2T(zlibVersion()));
}
