#pragma once

#include "math.h"

class Input;

class Camera
{
	Math::Vector m_Position;
	float m_Azimuth;
	float m_Zenith;

	POINT m_LastCursorPos;
	Math::Matrix m_Projection;
	Math::Matrix m_View;

	const Input& m_Input;

public:
	Camera( const Input& input );
	~Camera();

	void update( float dt );
	Math::Matrix ViewMatrix() { return m_View; }
	Math::Matrix ProjectionMatrix() { return m_Projection; }
};

