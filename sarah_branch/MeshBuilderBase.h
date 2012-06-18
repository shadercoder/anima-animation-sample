#pragma once
#include "stdafx.h"
#include "ModelBase.h"
#include <string>

class DataConverter;

class MeshBuilderBase
{
protected:
	const aiScene* mScene;

	typedef std::map<const aiMesh*, std::vector<DataConverter*> > ConverterMap;
	ConverterMap mDataConverters;

	virtual void CreateDataConverters( ConverterMap& result ) const;
	virtual int  GetVertexSize( const aiMesh* mesh ) const;
	virtual void ReadTexture( std::vector<BYTE>& result, const char* fileName );

public:
	MeshBuilderBase( const aiScene* scene);
	virtual ~MeshBuilderBase();

	virtual void BuildMeshes( std::vector<ModelBase::Mesh>& meshes, const std::string& textureFileName );
};

