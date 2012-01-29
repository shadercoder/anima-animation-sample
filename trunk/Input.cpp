#include "Input.h"




Input::Input()
{
	memset( &m_BufferedInput, 0x0, sizeof(m_BufferedInput) ); 
}


Input::~Input(void)
{
}

void Input::Update( float dt )
{
	m_MouseDelta = m_BufferedInput.Mouse;
	
	m_Keys.clear();
	for( unsigned int i=0; i<m_BufferedInput.KeyboardEventCount; ++i )
	{
		m_Keys[m_BufferedInput.KeyboardEvents[i].Key] = m_BufferedInput.KeyboardEvents[i].Flags;
	}

	// reset buffered input
	memset( &m_BufferedInput, 0x0, sizeof(m_BufferedInput) ); 

}

void Input::OnRawInput( HRAWINPUT hRawInput )
{
	UINT requiredSize;
	GetRawInputData( hRawInput, RID_INPUT, NULL, &requiredSize, sizeof(RAWINPUTHEADER) );
	if( m_InputBuffer.size() < requiredSize ) m_InputBuffer.resize( requiredSize );

	GetRawInputData( hRawInput, RID_INPUT, &m_InputBuffer[0], &requiredSize, sizeof(RAWINPUTHEADER) ) ;
	RAWINPUT* rawInput = reinterpret_cast<RAWINPUT*>( &m_InputBuffer[0] );	

	if (rawInput->header.dwType == RIM_TYPEKEYBOARD && m_BufferedInput.KeyboardEventCount < MAX_KEYBOARD_EVENTS_PER_FRAME-1)
	{
		m_BufferedInput.KeyboardEvents[m_BufferedInput.KeyboardEventCount].Key = rawInput->data.keyboard.VKey;
		m_BufferedInput.KeyboardEvents[m_BufferedInput.KeyboardEventCount].Flags = rawInput->data.keyboard.Flags;
		m_BufferedInput.KeyboardEventCount++;
	}
	else if (rawInput->header.dwType == RIM_TYPEMOUSE)
	{
		m_BufferedInput.Mouse.x += rawInput->data.mouse.lLastX ;
		m_BufferedInput.Mouse.y += rawInput->data.mouse.lLastY;
	} 
}

bool Input::GetKey( USHORT VKey ) const
{
	return m_Keys.find( VKey ) != m_Keys.end();
	
}
