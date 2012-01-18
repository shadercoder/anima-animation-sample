#include "StdAfx.h"
#include "TestEnvironment.h"
#include <stdio.h>

TestEnvironment::TestEnvironment( int argc, const LPWSTR* argv )
	: m_NumCubes( 1000 )
	, m_CubeRotation( 0.7f )
	, m_CubeScale( 1.0f )
	, m_IsValid( false )
	, m_RendererVersion( 0 )
	, m_RandomSeed( 27520 )
{
	bool hasNumCubes = false;
	bool hasCubeRotation = false;
	bool hasCubeScale = false;

	for( int i=1; i<argc; ++i )
	{
		if( wcscmp( argv[i], L"--NumCubes" ) == 0 )
		{
			++i;
			m_NumCubes = _wtoi( argv[i] );
			hasNumCubes = true;
		}

		else if(  wcscmp( argv[i], L"--CubeRotation" ) == 0 )
		{
			++i;
			m_CubeRotation = (float) _wtof( argv[i] );
			hasCubeRotation = true;
		}

		else if(  wcscmp( argv[i], L"--CubeScale" ) == 0 )
		{
			++i;
			m_CubeScale = (float) _wtof( argv[i] );
			hasCubeScale = true;
		}
		else if(  wcscmp( argv[i], L"--RendererVersion" ) == 0 )
		{
			++i;
			m_RendererVersion =  _wtoi( argv[i] );
		}
		else if(  wcscmp( argv[i], L"--RandomSeed" ) == 0 )
		{
			++i;
			m_RandomSeed =  _wtoi( argv[i] );
		}

	}

	m_IsValid = hasNumCubes && hasCubeRotation && hasCubeScale;
}


TestEnvironment::~TestEnvironment(void)
{
}
