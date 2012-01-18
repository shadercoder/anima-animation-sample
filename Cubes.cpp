#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>

#include "resource.h"

#include "Cubes.h"
#include "RenderContext.h"
#include "FramerateCounter.h"
#include "UI.h"
#include <assert.h>
#include <Shellapi.h>
#include "TestEnvironment.h"



//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
	
	AnimaApplication::CreateInstance( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	AnimaApplication::Instance()->Run();
	AnimaApplication::DestroyInstance();  

	return 0;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	return AnimaApplication::Instance()->OnMessage( hWnd, msg, wParam, lParam );
}

AnimaApplication* AnimaApplication::m_pInstance = 0;

AnimaApplication::~AnimaApplication()
{
	if( m_pInstance )
	{
		UnregisterClass( "MY_WINDOWS_CLASS", m_WindowClass.hInstance );

		m_pUserInterface->ReleaseResources( m_pRenderContext );
		delete m_pUserInterface;

	//	m_pCubeRenderer->ReleaseResources( m_pRenderContext );
	//	delete m_pCubeRenderer;

		delete m_pFramerateCounter;
		delete m_pRenderContext;
	}
}

void GLUT_OnKeyUp( unsigned char key, int x, int y )
{
}

void GLUT_OnRenderFrame()
{
	AnimaApplication::Instance()->NextFrame();
}

HRESULT AnimaApplication::CreateInstance( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	assert( AnimaApplication::m_pInstance == 0 );

	HWND	   hWnd;
	WNDCLASSEX winClass;
	memset( &winClass, 0x0, sizeof(WNDCLASSEX) );
	
#ifndef OPENGL

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
                             "DX9 Smiling Cubes",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 1024, 768, NULL, NULL, hInstance, NULL );

	if( hWnd == NULL )
		return E_FAIL;

	ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );
#else

	// very hacky way to do this but i didn't want to invest more time in such an unimportant task
	char* argv[] =
	{
		GetCommandLine(),
		NULL
	};

	int argc = 1;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(1024, 768);
    hWnd = (HWND)glutCreateWindow("OpenGL DX9 Smiling Cubes");

	glutDisplayFunc(GLUT_OnRenderFrame);
	glutKeyboardUpFunc(GLUT_OnKeyUp);
  
	// register vbo extension functions; 
	initVBO();
#endif

	LPWSTR *szArglist;
	int nArgs;
	
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	TestEnvironment* testEnvironment = new TestEnvironment( nArgs, szArglist );
	Math::Random::Init( testEnvironment->GetRandomSeed() );

	AnimaApplication::m_pInstance = new AnimaApplication( winClass, hWnd );

	Instance()->m_pTestEnvironment = testEnvironment;
	Instance()->m_pRenderContext = new RenderContext( hWnd, DISPLAY_WIDTH, DISPLAY_HEIGHT );
	Instance()->m_pFramerateCounter = new FramerateCounter;
	/*
	switch( Instance()->m_pTestEnvironment->GetRendererVersion() )
	{
	case 0: 
		Instance()->m_pCubeRenderer = new CubeRenderer0( m_pInstance->m_pRenderContext );
		break;
	case 1:
		Instance()->m_pCubeRenderer = new CubeRenderer1( m_pInstance->m_pRenderContext );
		break;
	case 2:
		Instance()->m_pCubeRenderer = new CubeRenderer2( m_pInstance->m_pRenderContext );
		break;
	case 3:
		Instance()->m_pCubeRenderer = new CubeRenderer3( m_pInstance->m_pRenderContext );
		break;
	case 4:
		Instance()->m_pCubeRenderer = new CubeRenderer4( m_pInstance->m_pRenderContext );
		break;
	case 5:
		Instance()->m_pCubeRenderer = new CubeRenderer5( m_pInstance->m_pRenderContext );
		break;
	case 6:
		Instance()->m_pCubeRenderer = new CubeRenderer6( m_pInstance->m_pRenderContext );
		break;

	}

	Instance()->m_pCubeRenderer->AcquireResources( Instance()->m_pRenderContext );
	

	Instance()->m_pUserInterface = new UserInterface( m_pInstance->m_pRenderContext, m_pInstance->m_pCubeRenderer, m_pInstance->m_pFramerateCounter );
	Instance()->m_pUserInterface->AcquireResources( Instance()->m_pRenderContext );

	Instance()->m_pCubeRenderer->SetNext( m_pInstance->m_pUserInterface );
*/
	// set up renderer 
	Instance()->m_DeltaTime = 0;


	

	return S_OK;
}

void AnimaApplication::DestroyInstance()
{
	assert( AnimaApplication::m_pInstance );
	delete AnimaApplication::m_pInstance;
	AnimaApplication::m_pInstance = 0;
}


void AnimaApplication::Run()
{
#ifndef OPENGL
	MSG        uMsg;
    memset(&uMsg,0,sizeof(uMsg));

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
        else
        {
			NextFrame(); 
		}
	}
#else
	glutMainLoop();
#endif
}

void AnimaApplication::NextFrame()
{
	m_pFramerateCounter->FrameStart();

//	m_pCubeRenderer->Update( m_DeltaTime );
	m_pRenderContext->RenderFrame( NULL );

	m_pFramerateCounter->FrameEnd();
	m_DeltaTime = m_pFramerateCounter->GetDelta();
//	Sleep(10);
}

LRESULT AnimaApplication::OnMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{	
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
				case 0x51: // 'Q'
					PostQuitMessage(0);
					break;
/*
				case VK_LEFT:
					if( !m_pTestEnvironment->IsValid() )
						m_pCubeRenderer->RemoveCubes( 1000 );
					break;
				case VK_RIGHT:
					if( !m_pTestEnvironment->IsValid() )
						m_pCubeRenderer->AddCubes( 1000 );
					break;

				case VK_UP:
					if( !m_pTestEnvironment->IsValid() )
						m_pCubeRenderer->SetScale( m_pCubeRenderer->GetScale() + 0.5f );
					break;

				case VK_DOWN:
					if( !m_pTestEnvironment->IsValid() )
						m_pCubeRenderer->SetScale( m_pCubeRenderer->GetScale() - 0.5f );
					break;
					*/
				case 0x52: // 'R'
					TestDeviceLost();
					break;

				case 0x53:	// 'S'
					m_pUserInterface->ToggleStatistics();
					break;
			}
		}
        break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

void AnimaApplication::OnDeviceLost()
{
//	m_pCubeRenderer->ReleaseResources( m_pRenderContext ); 
	m_pUserInterface->ReleaseResources( m_pRenderContext );
}

void AnimaApplication::OnDeviceReset()
{
//	m_pCubeRenderer->AcquireResources( m_pRenderContext ); 
	m_pUserInterface->AcquireResources( m_pRenderContext );
}

void AnimaApplication::TestDeviceLost()
{
#ifndef OPENGL
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
					
	D3DDISPLAYMODE d3ddm;

	m_pRenderContext->Device()->GetDisplayMode( 0, &d3ddm );

	d3dpp.Windowed               = TRUE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_pRenderContext->Device()->Reset( &d3dpp );
#endif
}