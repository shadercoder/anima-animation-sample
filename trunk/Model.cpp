#include "Model.h"
#include <iostream>
#include "RenderContext.h"
#include "DxErr.h"

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
		memset( &result, 0x0, sizeof(Mesh) );

		aiMesh* importedMesh = scene->mMeshes[m];

		// create data converters first
		std::vector<DataConverter*> dataConverters;
		result.m_VertexSize = CreateDataConverters( importedMesh, dataConverters );

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
			const int vertexBufferSize = importedMesh->mNumVertices * result.m_VertexSize;
			context->Device()->CreateVertexBuffer( vertexBufferSize, 0, 0, D3DPOOL_DEFAULT, &result.m_pVertexBuffer, NULL );

			BYTE* vertexData;
			result.m_pVertexBuffer->Lock( 0, 0, reinterpret_cast<void**>( &vertexData ), D3DLOCK_DISCARD );

			BYTE* curOffset = reinterpret_cast<BYTE*>( vertexData );

			for( unsigned int v=0; v<importedMesh->mNumVertices; ++v )
			{
				for( unsigned int i=0; i<dataConverters.size(); ++i )
					dataConverters[i]->CopyData( vertexData + v * result.m_VertexSize, v );
			}

			result.m_pVertexBuffer->Unlock();
			result.m_VertexCount = importedMesh->mNumVertices;
		}

		// build index buffer
		{
			const D3DFORMAT indexFormat = importedMesh->mNumVertices < 0xFFFF ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
			const int indexSize = indexFormat==D3DFMT_INDEX16 ?  sizeof(UINT16) : sizeof(UINT32);

			const int indexBufferSize = importedMesh->mNumFaces * 3 * indexSize;
			context->Device()->CreateIndexBuffer( indexBufferSize, 0, indexFormat, D3DPOOL_DEFAULT, &result.m_pIndexBuffer, NULL );

			BYTE* indexData;
			result.m_pIndexBuffer->Lock( 0, 0, reinterpret_cast<void**>( &indexData ), D3DLOCK_DISCARD );

			for( unsigned int f=0; f<importedMesh->mNumFaces; ++f )
			{
				const aiFace& face = importedMesh->mFaces[f];
				assert( face.mNumIndices == 3 ); // we only accept triangle meshes

				UINT32 indices[3];

				if( indexFormat == D3DFMT_INDEX16 )
				{
					assert( face.mIndices[0] < 0xFFFF && face.mIndices[1] < 0xFFFF && face.mIndices[2] < 0xFFFF );

					UINT16* indices16 = reinterpret_cast<UINT16*>( indices );
					indices16[0] = face.mIndices[0];
					indices16[1] = face.mIndices[1];
					indices16[2] = face.mIndices[2];
				}
				else
				{
					indices[0] = face.mIndices[0];
					indices[1] = face.mIndices[1];
					indices[2] = face.mIndices[2];
				}

				memcpy( indexData + f * 3 * indexSize, indices, 3*indexSize );
			}

			result.m_pIndexBuffer->Unlock();
			result.m_TriangleCount = importedMesh->mNumFaces;

		}

		// load shaders
		{
			LPD3DXBUFFER buf;
			HRESULT hr = D3DXCreateEffectFromFile( context->Device(), "Shaders/Model.fx", NULL, NULL, NULL, NULL, &result.m_pEffect, &buf );

			if( FAILED(hr) && buf)
			{
				const char* text = reinterpret_cast<char*>( buf->GetBufferPointer() );
				DXTRACE_ERR( text, hr );
				buf->Release();
			}
		}

		m_Meshes.push_back( result );
	}


	// scene->mMeshes[0]->mFaces

	return true;

}


void Model::Render( RenderContext* context )
{
	for( unsigned int m=0; m<m_Meshes.size(); ++m )
	{
		const Mesh& mesh = m_Meshes[m];
		D3DXHANDLE hTechnique = mesh.m_pEffect->GetTechniqueByName( "Model" );
		D3DXHANDLE hWorldViewProjection =mesh.m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEWPROJECTION" );
		D3DXHANDLE hLightDirection =mesh.m_pEffect->GetParameterBySemantic( NULL, "LIGHTDIRECTION" );

		context->Device()->SetVertexDeclaration( mesh.m_pVertexDeclaration );
		context->Device()->SetStreamSource( 0, mesh.m_pVertexBuffer, 0, mesh.m_VertexSize );
		context->Device()->SetIndices( mesh.m_pIndexBuffer );

		mesh.m_pEffect->SetMatrix( hWorldViewProjection, &(context->GetViewMatrix() * context->GetProjectionMatrix()).data );


		D3DXVECTOR4 lightDirection = D3DXVECTOR4( 0, 1, 0, 1 );
		mesh.m_pEffect->SetVector( hLightDirection, &lightDirection );

		mesh.m_pEffect->SetTechnique( hTechnique );

		UINT cPasses;
		mesh.m_pEffect->Begin( &cPasses, 0 );

		for( unsigned int iPass = 0; iPass < cPasses; iPass++ )
		{
			mesh.m_pEffect->BeginPass( iPass );

			HRESULT hr = context->Device()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mesh.m_VertexCount, 0, mesh.m_TriangleCount );

			mesh.m_pEffect->EndPass();
		}

		mesh.m_pEffect->End();
	}

	context->Device()->SetStreamSource( 0, NULL, 0, 0 );
	context->Device()->SetIndices( NULL );
}