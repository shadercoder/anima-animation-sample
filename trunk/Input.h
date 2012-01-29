#pragma once

#include "stdafx.h"

class Input
{
	static const UINT MAX_KEYBOARD_EVENTS_PER_FRAME = 16;

	typedef std::map<USHORT, USHORT> KeyInputType;

	POINT m_MouseDelta;
	KeyInputType m_Keys;

	struct
	{
		POINT Mouse;

		UINT KeyboardEventCount;
		struct 
		{
			USHORT Key;
			USHORT Flags;
		}KeyboardEvents[MAX_KEYBOARD_EVENTS_PER_FRAME];

	} m_BufferedInput;

	std::vector<BYTE> m_InputBuffer;
public:
	Input();
	~Input();

	void OnRawInput( HRAWINPUT hRawInput );
	void Update( float dt );

	POINT GetMouse() const { return m_MouseDelta; }
	bool GetKey( USHORT VKey ) const;

};

