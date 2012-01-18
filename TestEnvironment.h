#pragma once
class TestEnvironment
{
	int m_NumCubes;
	float m_CubeRotation;
	float m_CubeScale;

	bool m_IsValid;

	int m_RendererVersion;
	int m_RandomSeed;

public:
	TestEnvironment( int argc, const LPWSTR* argv );
	~TestEnvironment(void);

	int GetNumCubes() const { return m_NumCubes; }
	float GetCubeRotation() const { return m_CubeRotation; }
	float GetCubeScale() const { return m_CubeScale; }
	int GetRendererVersion() const { return m_RendererVersion; }
	int GetRandomSeed() const { return m_RandomSeed; }

	bool IsValid() const { return m_IsValid; }
};

