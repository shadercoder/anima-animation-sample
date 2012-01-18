#include "stdafx.h"
#include "math.h"

namespace Math
{
	char normalizedFloatToByte( float value )
	{
		int v = static_cast<int>( value * 255.0f );
		return max( min( v, 255 ), 0 );
	}

	int vectorToInt( D3DXVECTOR4 value, D3DXVECTOR4 min, D3DXVECTOR4 max )
	{
		return 
			normalizedFloatToByte( (value.x - min.x) / (max.x - min.x)  ) << 24 ||
			normalizedFloatToByte( (value.y - min.y) / (max.y - min.y) ) << 16 ||
			normalizedFloatToByte( (value.z - min.z) / (max.z - min.z) ) << 8 ||
			normalizedFloatToByte( (value.w - min.w) / (max.w - min.w) );

	}

	D3DXVECTOR4 Min( D3DXVECTOR4 a, D3DXVECTOR4 b )
	{
		D3DXVECTOR4 result;

		result.x = min( a.x, b.x );
		result.y = min( a.y, b.y );
		result.z = min( a.z, b.z );
		result.w = min( a.w, b.w );

		return result;
	}

	D3DXVECTOR4 Max( D3DXVECTOR4 a, D3DXVECTOR4 b )
	{
		D3DXVECTOR4 result;

		result.x = max( a.x, b.x );
		result.y = max( a.y, b.y );
		result.z = max( a.z, b.z );
		result.w = max( a.w, b.w );

		return result;
	}

}