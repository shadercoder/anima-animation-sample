#include "Model.h"
#include <iostream>
#include "RenderContext.h"

using namespace std;
Model::Model( const std::string& fileName )
	: mFileName( fileName )
	, mIsLoaded( false )
	, mScene( NULL )
{
}


Model::~Model(void)
{
}

int Model::CreateDataConverters( aiMesh* mesh, std::vector<DataConverter*>& result )
{
	const int vertexCount = mesh->mNumVertices;
	int offset = 0;

	// positions, normals and tangents/bitangents
	if( mesh->HasPositions() )	
	{
		result.push_back( new aiVector3DConverter( D3DDECLUSAGE_POSITION, 0, offset, mesh->mVertices, vertexCount ) );
		offset += result.back()->Size();
	}

	if( mesh->HasNormals() ) 
	{
		result.push_back( new aiVector3DConverter( D3DDECLUSAGE_NORMAL, 0, offset, mesh->mNormals, vertexCount ) );
		offset += result.back()->Size();
	}

	if( mesh->HasTangentsAndBitangents() )
	{
		result.push_back( new aiVector3DConverter( D3DDECLUSAGE_TANGENT, 0, offset, mesh->mTangents, vertexCount ) );
		offset += result.back()->Size();

		result.push_back( new aiVector3DConverter( D3DDECLUSAGE_BINORMAL, 0, offset, mesh->mBitangents, vertexCount ) );
		offset += result.back()->Size();
	}

	for( unsigned int c=0; c < mesh->GetNumColorChannels(); ++c )
	{
		result.push_back( new aiColor4DConverter( D3DDECLUSAGE_COLOR, 0, offset, mesh->mColors[c], vertexCount ) );
		offset += result.back()->Size();

	}

	for( unsigned int t=0; t < mesh->GetNumUVChannels(); ++t )
	{
		if( mesh->mNumUVComponents[t] == 2 )
			result.push_back( new aiVector3DToFloat2Converter( D3DDECLUSAGE_TEXCOORD, t, offset, mesh->mTextureCoords[t], vertexCount ) );
		else
			result.push_back( new aiVector3DConverter( D3DDECLUSAGE_TEXCOORD, t, offset, mesh->mTextureCoords[t], vertexCount ) );	

		offset += result.back()->Size();
	}

	if( mesh->HasBones() )
	{
		result.push_back( new aiSkinningConverter( offset, mesh->mBones, mesh->mNumBones ) );
		offset += result.back()->Size();
	}

	return offset;
}

bool Model::load( RenderContext* context )
{
	  // Create an instance of the Importer class
  Assimp::Importer importer;

  // And have it read the given file with some example postprocessing
  // Usually - if speed is not the most important aspect for you - you'll 
  // propably to request more postprocessing than we do in this example.
  const aiScene* scene = importer.ReadFile( mFileName, 
        aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);
  
  // If the import failed, report it
  if( !scene)
  {
	  std::cout << importer.GetErrorString() << std::endl;
	  return false;
  }

  mSkeleton = SkeletonFactory::extractSkeleton( scene );

  for( unsigned int m=0; m<scene->mNumMeshes; ++m )
  {
		Mesh result;
		aiMesh* importedMesh = scene->mMeshes[m];

		// create data converters first
		std::vector<DataConverter*> dataConverters;
		int vertexSize = CreateDataConverters( importedMesh, dataConverters );

		// now build vertex declaration
		std::vector<D3DVERTEXELEMENT9> vertexElements;
		{
			for( unsigned int i=0; i<dataConverters.size(); ++i )
				dataConverters[i]->CopyType( vertexElements );
			
			D3DVERTEXELEMENT9 endElement = D3DDECL_END();
			vertexElements.push_back( endElement );

			context->Device()->CreateVertexDeclaration( &vertexElements[0], &result.m_pVertexDeclaration );
		}

		// now create vertex buffer
		{
			const int vertexBufferSize = importedMesh->mNumVertices * vertexSize;
			context->Device()->CreateVertexBuffer( vertexBufferSize, 0, 0, D3DPOOL_DEFAULT, &result.m_pVertexBuffer, NULL );

			BYTE* vertexData;
			result.m_pVertexBuffer->Lock( 0, vertexBufferSize, reinterpret_cast<void**>( &vertexData ), D3DLOCK_DISCARD );

			BYTE* curOffset = reinterpret_cast<BYTE*>( vertexData );

			for( unsigned int v=0; v<importedMesh->mNumVertices; ++v )
			{
				for( unsigned int i=0; i<dataConverters.size(); ++i )
					dataConverters[i]->CopyData( vertexData + v * vertexSize, v );
			}
		
		}

		// build index buffer
		{
		}
  }


 // scene->mMeshes[0]->mFaces

  return true;

}


void Model::Render( RenderContext* context )
 {



}