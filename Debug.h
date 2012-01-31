#pragma once

#include <DxErr.h>

/* Debug functionality */
void DebugPrint( const TCHAR* szFormat, ...);

#define DX_CHECK(hr) __DxCheckAndReportError( hr, __FILE__, __LINE__ );

void __DxCheckAndReportError( HRESULT hr, const char* file, int line );