#pragma once

#include "stdafx.h"
#include "math.h"


class DataConverter
{
protected:
	D3DDECLUSAGE m_UsageType;
	int m_UsageIndex;
	int m_Offset;

public:
	DataConverter( D3DDECLUSAGE usageType, int usageIndex, int offsetInBytes )
		: m_UsageType( usageType )
		, m_UsageIndex( usageIndex )
		, m_Offset( offsetInBytes )
	{
	}

	virtual int Size() = 0;
	virtual void CopyType( std::vector<D3DVERTEXELEMENT9>& out_Type ) = 0;
	virtual void CopyData( BYTE* destination, int index ) = 0;
};

template< typename T >
class ArrayDataConverter : public DataConverter
{
	const T* m_SourceData;
	const int m_SourceSize;

public:
	ArrayDataConverter( D3DDECLUSAGE usageType, int usageIndex, int& offsetInBytes, const T* sourceData, int sourceSize )
	: DataConverter( usageType, usageIndex, offsetInBytes )
	, m_SourceData( sourceData )
	, m_SourceSize( sourceSize ) {} 

	const T& GetElement( int index )
	{
		assert( index >= 0 && index < m_SourceSize );
		return m_SourceData[index];
	}
};

struct aiVector3DConverter : public ArrayDataConverter<aiVector3D>
{
	aiVector3DConverter( D3DDECLUSAGE usageType, int usageIndex, int& offsetInBytes, const aiVector3D* sourceData, int sourceSize )
		: ArrayDataConverter<aiVector3D>( usageType, usageIndex, offsetInBytes, sourceData, sourceSize ) {}
	
	int Size() { return 3 * sizeof(float); }

	virtual void CopyType( std::vector<D3DVERTEXELEMENT9>& out_Type )
	{
		D3DVERTEXELEMENT9 _result = { 0, m_Offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, m_UsageType, m_UsageIndex };
		out_Type.push_back( _result );
	}

	void CopyData( BYTE* destination, int elementIndex  )
	{
		const aiVector3D& element = GetElement( elementIndex );

		float _data[] = { element[0], element[1], element[2] };
		memcpy( destination + m_Offset, _data, Size() );
	}

};

struct aiVector3DToFloat2Converter : public ArrayDataConverter<aiVector3D>
{
	aiVector3DToFloat2Converter( D3DDECLUSAGE usageType, int usageIndex, int& offsetInBytes, const aiVector3D* sourceData, int sourceSize )
		: ArrayDataConverter<aiVector3D>( usageType, usageIndex, offsetInBytes, sourceData, sourceSize ) {}
	
	int Size() { return 2 * sizeof(float); }

	virtual void CopyType( std::vector<D3DVERTEXELEMENT9>& out_Type )
	{
		D3DVERTEXELEMENT9 _result = { 0, m_Offset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, m_UsageType, m_UsageIndex };
		out_Type.push_back( _result );
	}

	void CopyData( BYTE* destination, int elementIndex  )
	{
		const aiVector3D& element = GetElement( elementIndex );

		float _data[] = { element[0], element[1] };
		memcpy( destination + m_Offset, _data, Size() );
	}

};

struct aiColor4DConverter : public ArrayDataConverter<aiColor4D>
{
	aiColor4DConverter( D3DDECLUSAGE usageType, int usageIndex, int& offsetInBytes, const aiColor4D* sourceData, int sourceSize )
	: ArrayDataConverter<aiColor4D>( usageType, usageIndex, offsetInBytes, sourceData, sourceSize ) {} 
	
	int Size() { return 4 * sizeof(BYTE); }

	virtual void CopyType( std::vector<D3DVERTEXELEMENT9>& out_Type )
	{
		D3DVERTEXELEMENT9 _result = { 0, m_Offset, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, m_UsageType, m_UsageIndex };
		out_Type.push_back( _result );
	}

	void CopyData( BYTE* destination, int elementIndex  )
	{
		const aiColor4D& element = GetElement( elementIndex );

		BYTE _data[] = 
		{
			Math::normalizedFloatToByte( element[0] ),
			Math::normalizedFloatToByte( element[1] ),
			Math::normalizedFloatToByte( element[2] ),
			Math::normalizedFloatToByte( element[3] ),
		};
			
		memcpy( destination + m_Offset, _data, Size() );
	}
};

struct aiSkinningConverter : public DataConverter
{
	static const int MAX_INFLUENCES_PER_VERTEX = 4; // WARNING: tied to D3DDECLTYPE of vertex declaration. See CopyType()

	struct VertexInfluence
	{
		int BoneIndex;
		float Weight;

	};

	struct BoneWeightCompare
	{
		bool operator()(const VertexInfluence& x, const VertexInfluence& y)
		{
			return x.Weight < y.Weight;
		}
	};

	struct BoneIndexCompare
	{
		bool operator()(const VertexInfluence& x, const VertexInfluence& y)
		{
			return x.BoneIndex < y.BoneIndex;
		}
	};

	struct BoneWeightAccumulate
	{
		float operator()( float value, const VertexInfluence& x )
		{
			return value + x.Weight;
		}
	};

	typedef std::map<int, std::vector<VertexInfluence> > VertexInfluenceMap;
	VertexInfluenceMap m_VertexInfluenceMap;

	aiSkinningConverter( int& offsetInBytes, aiBone** bones, unsigned int boneCount )
	: DataConverter( D3DDECLUSAGE_BLENDINDICES, 0, offsetInBytes ) 
	{
		// build map first
		for( unsigned int b=0; b<boneCount; ++b )
		{
			for( unsigned int w=0; w<bones[b]->mNumWeights; ++w )
			{
				const aiVertexWeight& aiVW = bones[b]->mWeights[w];
				VertexInfluence bi = { b, aiVW.mWeight };

				m_VertexInfluenceMap[aiVW.mVertexId].push_back( bi );
			}
		}

		// now filter weights (we only allow MAX_INFLUENCES_PER_VERTEX bone influences per vertex )
		for( VertexInfluenceMap::iterator it = m_VertexInfluenceMap.begin(); it != m_VertexInfluenceMap.end(); ++it )
		{
			const int vertexIndex = it->first;
			std::vector<VertexInfluence>& influences = it->second;

			// only keep most significant weights
			std::sort( influences.begin(), influences.end(), BoneWeightCompare() );
			influences.resize( MAX_INFLUENCES_PER_VERTEX );
			std::sort( influences.begin(), influences.end(), BoneIndexCompare() );

			// renormalize weights
			float totalWeight = std::accumulate( influences.begin(), influences.end(), 0.f,BoneWeightAccumulate() );
			for( int i=0; i<MAX_INFLUENCES_PER_VERTEX; ++i )
				influences[i].Weight /= totalWeight;
		}
	} 
	
	int IndicesSize() { return 4 * sizeof(BYTE); }
	int WeightsSize() { return 4 * sizeof(float); }
	virtual int Size() { return IndicesSize() + WeightsSize(); }

	virtual void CopyType( std::vector<D3DVERTEXELEMENT9>& out_Type )
	{
		D3DVERTEXELEMENT9 boneIndices = { 0, m_Offset, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 };
		D3DVERTEXELEMENT9 boneWeights = { 0, m_Offset + 4*sizeof(BYTE), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 };
		
		out_Type.push_back( boneIndices );
		out_Type.push_back( boneWeights );
	}

	void CopyData( BYTE* destination, int elementIndex  )
	{
		VertexInfluenceMap::const_iterator it = m_VertexInfluenceMap.find( elementIndex );
		assert( it != m_VertexInfluenceMap.end() );
		const std::vector<VertexInfluence>& influences = it->second;

		BYTE boneIndices[MAX_INFLUENCES_PER_VERTEX];
		float boneWeights[MAX_INFLUENCES_PER_VERTEX];

		for( int i=0; i<MAX_INFLUENCES_PER_VERTEX; ++i )
		{
			boneIndices[i] = influences[i].BoneIndex;
			boneWeights[i] = influences[i].Weight;
		}
			
		memcpy( destination + m_Offset, boneIndices, IndicesSize() );
		memcpy( destination + m_Offset + IndicesSize(), boneWeights, WeightsSize() );
	}
};