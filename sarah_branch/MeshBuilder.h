#pragma once
#include "stdafx.h"
#include "SkeletalModel.h"
#include "MeshBuilderBase.h"

class SkeletonBuilder;
class DataConverter;

class MeshBuilder : public MeshBuilderBase
{
	const SkeletonBuilder& mSkeletonBuilder;

	virtual void CreateDataConverters( MeshBuilderBase::ConverterMap& result ) const;

public:
	MeshBuilder( const aiScene* scene, const SkeletonBuilder& skeletonBuilder );
	~MeshBuilder();

	virtual void BuildMeshes( std::vector<SkeletalModel::Mesh>& meshes );
};

