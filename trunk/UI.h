#pragma once

#include "stdafx.h"
#include "DisplayList.h"

class RenderContext;
class CubeRendererInterface;
class FramerateCounter;

/* Simple class for managing the 'user interface'. Basically just renders some statistics to the screen	*/
class UserInterface : public DisplayList::Node
{
	CubeRendererInterface* m_pCubeRenderer;
	FramerateCounter* m_pFrameCounter;

#ifndef OPENGL
	LPD3DXFONT m_Font;
#else
#endif

	// Top left corner of text rendering (pixel coordinates)
	static const int Top = 10;
	static const int Left = 10;

	// Width and height of each line of text
	static const int LineHeight = 30;
	static const int LineWidth = 500;

	D3DCOLOR TextColor;
	bool m_RenderStatistics;

	// helper function to abstract opengl and dx9
	void RenderText( TCHAR* text, RECT& rect );

public:
	UserInterface( RenderContext* context, CubeRendererInterface* cubeRenderer, FramerateCounter* frameCounter );
	virtual ~UserInterface(void);

	void Render( RenderContext* context );
	void ReleaseResources( RenderContext* context );
	void AcquireResources( RenderContext* context );
	void ToggleStatistics() { m_RenderStatistics = !m_RenderStatistics; }

};

