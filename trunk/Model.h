#pragma once
#include "stdafx.h"
#include <string>
#include "DisplayList.h"
#include "Skeleton.h"
#include "DataConverters.h"
#include "Animation.h"

class SkeletonBuilder;
class MeshBuilder;
struct aiScene;

class SkeletalModel : public DisplayList::Node
{
	friend class MeshBuilder;

	enum SkeletalAnimationMethod
	{
		SAM_LINEAR_BLEND_SKINNING,
		SAM_DUAL_QUATERNION,

		SAM_COUNT,
	};


	struct MeshData
	{
		int mVertexCount;
		int mVertexSize;

		int mTriangleCount;
		D3DFORMAT mIndexFormat;

		std::vector<BYTE> mVertexData;
		std::vector<BYTE> mIndexData;
		std::vector<D3DVERTEXELEMENT9> mVertexElements;

		MeshData() : mVertexCount(0), mVertexSize(0), mTriangleCount(0), mIndexFormat(D3DFMT_UNKNOWN) {};
	};

	struct Mesh
	{
		IDirect3DVertexBuffer9* mVertexBuffer;
		IDirect3DVertexDeclaration9* mVertexDeclaration;
		IDirect3DIndexBuffer9* mIndexBuffer;

		ID3DXEffect* mEffect;
		IDirect3DTexture9* mDiffuseMap;
		IDirect3DTexture9* mNormalMap;

		MeshData Data;

		Mesh() : mVertexBuffer(NULL), mVertexDeclaration(NULL), mIndexBuffer(NULL), mEffect(NULL) {}

	};

	SkeletonInterface* mSkeletons[SAM_COUNT];
	PoseBufferInterface* mPoseBuffers[SAM_COUNT];

	std::vector<Mesh> mMeshes;
	std::vector<Animation*> mAnimations;

	Animation* mCurrentAnimation;
	bool mAnimationPaused;

	int mShaderTest;
	SkeletalAnimationMethod mCurrentAnimationMethod;

	int CreateDataConverters( aiMesh* mesh, SkeletonBuilder* skeletonBuilder, std::vector<DataConverter*>& result );

public:
	SkeletalModel( const std::string& fileName );
	~SkeletalModel(void);

	bool Load( RenderContext* context );
	void AcquireResources( RenderContext* context );
	void ReleaseResources( RenderContext* context );


	void SetRoot( const aiVector3D& translation, const aiQuaternion& rotation );
	void PlayAnimation( unsigned int animationIndex, float playbackSpeed = 0.f );
	void PauseAnimation();
	bool ToggleAnimationPlayback();
	
	void ToggleShaderTest();
	int ToggleAnimationMethod();

	void Render( RenderContext* context );
	void Update( float dt );

	bool mIsLoaded;
	std::string mFileName;
};
