#if !defined(__CUBE_RENDERER_0_H__)
#define __CUBE_RENDERER_0_H__

#include "stdafx.h"
#include <vector>

#include "math.h"
#include "DisplayList.h"
#include "CubeRendererInterface.h"

class RenderContext;

class CubeRenderer0 : public CubeRendererInterface
{
	static const int VERTEX_SIZE = 32;		// vertex size in bytes

	// per vertex data: position, normal, txcoords, instance index 
	struct VertexData
	{
		float X, Y, Z;
		float Nx, Ny, Nz;
		float u, v;
	};

	// per instance data that will be sent to the gpu
	std::vector<D3DXVECTOR4> InstancePosition;
	std::vector<D3DXVECTOR4> InstanceRotationTexture;

	// data for procedural cube
	static VertexData CubeData[];
	static WORD CubeIndices[];

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;		// vertex data for 'CUBES_PER_BATCH' cubes
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;			// indices for 'CUBES_PER_BATCH' cubes
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
	LPDIRECT3DTEXTURE9 m_Texture;

	// effect data
	LPD3DXEFFECT m_pEffect;
	D3DXHANDLE m_effWorldViewProjection;
	D3DXHANDLE m_effTexture;
	D3DXHANDLE m_effTechnique;
	D3DXHANDLE m_effTransparentTechnique;
	D3DXHANDLE m_effPosition;
	D3DXHANDLE m_effRotationTexture;
	D3DXHANDLE m_effRotationAngleScale;
	D3DXHANDLE m_effLightDirection;

	Math::Vector GridTopLeft, GridBottomRight;	// dimensions of display grid

	float m_Rotation;	// current cube rotation
	float m_Scale;		// current cube scale

	Math::Vector m_LightDirection, m_LightPosition;

	int m_NumCubes;		// current number of cubes
	int m_NumTransparentCubes;	// current number of transparent cubes

	HRESULT RenderInstances( RenderContext* context, int start, int count );
	void FillBuffers( VertexData* vertices, WORD* indices, int numVerticesPerCube, int numIndicesPerCube, int numCubes );

	float m_CubeRotationSpeed; // speed of cube rotations (in radians/second)
	float m_LightRotationSpeed; // speed of light rotation (in radians/second)

	bool m_DynamicUpdate;

public:
	CubeRenderer0( RenderContext* context );
	CubeRenderer0::~CubeRenderer0();

	void AddCubes( int count );			// Add 'count' cubes with random rotation axis
	void RemoveCubes( int count );		// remove 'count' cubes

	void Update( float dt );
	void UpdateGrid();		// rearranges cubes in a regular 3d grid
	void Render( RenderContext* context );

	void SetScale( float scale ) { m_Scale = max( 0.001f, scale ); }
	float GetScale() { return m_Scale; }

	void SetRotation( float rotation ) { m_Rotation = rotation; }

	void SetDynamicUpdate( bool dynamicUpdate ) { m_DynamicUpdate = dynamicUpdate; }

	void SetLightPosition( Math::Vector lightPosition );

	int GetNumCubes() { return m_NumCubes; }

	void ReleaseResources( RenderContext* context );
	void AcquireResources( RenderContext* context );

};


#endif