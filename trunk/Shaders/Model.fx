

float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
float4 LightDirection : LIGHTDIRECTION;

struct VertexShaderInput
{
    float4 Position			: POSITION;
    float3 Normal			: NORMAL;
	float2 TexCoord			: TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position			: POSITION0;
	float3 Normal			: TEXCOORD1;
    float2 TexCoord			: TEXCOORD0;
};


VertexShaderOutput Model_VS( VertexShaderInput input )
{

	VertexShaderOutput result;
	result.Position =  mul( float4(input.Position.xyz,1), WorldViewProjection );
	result.Normal = input.Normal.xyz;
	result.TexCoord = input.TexCoord;

	return result;
}


float4 Model_PS( VertexShaderOutput input ) : COLOR0
{
	
	const float ambient = 0.3f;
	float diffuse = saturate(dot(input.Normal, LightDirection.xyz ) );

	return float4(float3(1,0,0) * (ambient + diffuse),1);
}

technique Model
{
    pass P0
    {
        VertexShader = compile vs_2_0 Model_VS();
        PixelShader  = compile ps_2_0 Model_PS();
    }
}



