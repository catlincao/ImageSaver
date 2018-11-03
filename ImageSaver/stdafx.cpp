// stdafx.cpp : source file that includes just the standard includes
// ImageSaver.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


char* str2char(CString str) //CString -> const char *
{
	char *ptr;
#ifdef _UNICODE
	LONG len;
	len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	ptr = new char[len + 1];
	memset(ptr, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, len + 1, NULL, NULL);
#else
	ptr = new char[str.GetAllocLength() + 1];
	sprintf(ptr, _T("%s"), str);
#endif
	return ptr;
}