#include "StdAfx.h"
#include "UI.h"
#include "RenderContext.h"
#include "FramerateCounter.h"
#include <stdio.h>


UserInterface::UserInterface( RenderContext* context, CubeRendererInterface* cubeRenderer, FramerateCounter* frameCounter )
	: m_pCubeRenderer( cubeRenderer )
	, m_pFrameCounter( frameCounter )
	, m_RenderStatistics( true )
{
	TextColor = D3DCOLOR_RGBA(255, 255, 255,255 );
}

UserInterface::~UserInterface()
{
}

void UserInterface::RenderText( TCHAR* text, RECT& rect )
{
#ifndef OPENGL
	m_Font->DrawText( NULL, text, -1, &rect, 0, TextColor );
#else

#endif
}

void UserInterface::Render( RenderContext* context )
{
	if( m_RenderStatistics )
	{
		RECT rcLine =  { Left,Top, Left + LineWidth, Top + LineHeight };
	
		TCHAR textBuffer[512];  
		float avgFPS = m_pFrameCounter->GetAverage();

		// Framerate
		sprintf_s( textBuffer, "Frame time: %.2f ms", avgFPS * 1000.0f );
		RenderText( textBuffer, rcLine );

		// number of cubes
		rcLine.left += LineWidth;
		rcLine.right += LineWidth;
/*		sprintf_s( textBuffer, "Number of Cubes: %d", m_pCubeRenderer->GetNumCubes() );
		RenderText( textBuffer, rcLine );

		// cubes per second
		rcLine.top += LineHeight;
		rcLine.bottom += LineHeight;
		sprintf_s( textBuffer, "Average cubes per second: %.0f", m_pCubeRenderer->GetNumCubes() * avgFPS );
		RenderText( textBuffer, rcLine );
*/
	}
}

void UserInterface::ReleaseResources(  RenderContext* )
{
#ifndef OPENGL
	m_Font->Release();
	m_Font = 0;
#else

#endif
}

void UserInterface::AcquireResources( RenderContext* context )
{
#ifndef OPENGL
	D3DXCreateFont( 
		context->Device(), 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("Arial"), &m_Font 
	);
#else

#endif
}


