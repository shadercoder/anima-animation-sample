

#include "stdafx.h"
#include "Debug.h"

#include <stdio.h> 
#include <stdarg.h>


void DebugPrint( const TCHAR* szFormat, ...)
{
	TCHAR szBuff[8192];
    va_list arg;
    va_start(arg, szFormat);
    vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
    va_end(arg);

    OutputDebugString(szBuff);
}
