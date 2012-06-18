#pragma once
#include "stdafx.h"
#include "Serialization.h"
#include "DisplayList.h"
#include <string>

class RenderContext;

/** ModelBase class to load any model.
*/
class ModelBase : public DisplayList::Node, Serialization::StreamSerializable
{
	friend class MeshBuilderBase;

	struct MeshData : public Serialization::StreamSerializable
	{
		int mVertexCount;
		int mVertexSize;

		int mTriangleCount;
		D3DFORMAT mIndexFormat;

		std::vector<BYTE> mVertexData;
		std::vector<BYTE> mIndexData;
		std::vector<D3DVERTEXELEMENT9> mVertexElements;

		//std::vector<BYTE> mAlbedoMap;
		std::vector<BYTE> mNormalMap;

		MeshData() : mVertexCount(0), mVertexSize(0), mTriangleCount(0), mIndexFormat(D3DFMT_UNKNOWN) {};

		virtual bool ToStream( std::ostream& stream );
		virtual bool FromStream( std::istream& stream );
	};

	struct Mesh
	{
		IDirect3DVertexBuffer9* mVertexBuffer;
		IDirect3DVertexDeclaration9* mVertexDeclaration;
		IDirect3DIndexBuffer9* mIndexBuffer;

		ID3DXEffect* mEffect;
		IDirect3DTexture9* mDiffuseMap;
		IDirect3DTexture9* mNormalMap;

		/*int mDiffuseMapSamplerIndex[SAM_COUNT];
		int mNormalMapSamplerIndex[SAM_COUNT];*/

		struct 
		{
			D3DXHANDLE mViewProjection;
			D3DXHANDLE mShaderTest;
			/*D3DXHANDLE mBoneTransforms;
			D3DXHANDLE mTechniques[SAM_COUNT];*/
		} EffectParameters;

		MeshData Data;

		Mesh() : mVertexBuffer(NULL), mVertexDeclaration(NULL), mIndexBuffer(NULL), mEffect(NULL), mDiffuseMap(NULL), mNormalMap(NULL) {}
	};

	std::vector<Mesh> mMeshes;

protected:
	int mShaderTest;

public:
	ModelBase( const std::string& fileName );
	virtual ~ModelBase(void);

	virtual void setShaderFile( const std::string& fileName );
	virtual void setTextureFile( const std::string& fileName );

	virtual bool Load( RenderContext* context );
	virtual void AcquireResources( RenderContext* context );
	virtual void ReleaseResources( RenderContext* context );

	virtual void loadTextures( RenderContext* context, Mesh& mesh );
	virtual void loadShaders( RenderContext* context, Mesh& mesh );

	virtual bool ToStream( std::ostream& stream );
	virtual bool FromStream( std::istream& stream );
	
	virtual void Render( RenderContext* context );

	bool mIsLoaded;
	std::string mFileName;
	std::string mShaderFileName;
	std::string mTextureFileName;
};