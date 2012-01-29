#pragma once

#include "stdafx.h"
#include "math.h"

namespace DisplayList
{
	class Node;
}

/* Context abstraction for rendering: Stores the current view/projection matrices and exposes the d3d device to the application
	RenderFrame() traverses a display list and calls 'render' for the objects in the list	*/
class RenderContext
{
	HWND                    m_hWnd;

	LPDIRECT3D9             m_pD3D;
	LPDIRECT3DDEVICE9       m_pDevice;
	D3DPRESENT_PARAMETERS	m_PresentParameters;	// needed when device is reset

	Math::Matrix m_ViewMatrix;
	Math::Matrix m_ProjectionMatrix;

public:
	RenderContext( HWND hWnd, int width, int height );
	~RenderContext(void);

	LPDIRECT3DDEVICE9 Device() { return m_pDevice; }

	void RenderFrame( DisplayList::Node* displayList );

	void SetViewMatrix( Math::Matrix& newViewMatrix ) { m_ViewMatrix = newViewMatrix; }
	Math::Matrix GetViewMatrix() { return m_ViewMatrix; }
	Math::Matrix GetProjectionMatrix() { return m_ProjectionMatrix; }
};

