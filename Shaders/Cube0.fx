

float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
float4 RotationAngleScale : ROTATIONANGLESCALE;
float4 LightDirection : LIGHTDIRECTION;

float4 InstancePosition : INSTANCE_POSITION;
float4 InstanceRotationTexture : INSTANCE_ROTATION;


texture AlbedoTexture : TEXTURE;	
sampler AlbedoSampler = sampler_state
{
    Texture = <AlbedoTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

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

float4 quaternionMultiply( float4 q1, float4 q2 )
{
	return float4( 
		q1.w * q2.xyz + q2.w * q1.xyz + cross( q1.xyz, q2.xyz ),
		q1.w * q2.w - dot( q1.xyz, q2.xyz )
	);
}

float3 quaternionRotate(  float3 position, float4 quaternion )
{
	float4 quaternion_c = float4( -quaternion.xyz, quaternion.w);
	float4 p = quaternionMultiply( float4(position,0), quaternion_c );
	return quaternionMultiply( quaternion, p ).xyz;
}

VertexShaderOutput Cube_VS( VertexShaderInput input )
{
	// combine rotation axis and rotation angle to form a rotation quaternion. 
	// note that cosine(angle/2) and sin(angle/2) are stored in RotationAngle.xy

	float4 q = float4( 
		InstanceRotationTexture.xyz * RotationAngleScale.y, 
		RotationAngleScale.x
	);

	VertexShaderOutput result;
	float3 pos = quaternionRotate( input.Position.xyz, q ) * RotationAngleScale.z;
	result.Position =  mul( float4(pos.xyz,1)  + InstancePosition, WorldViewProjection );
	result.Normal = quaternionRotate( input.Normal.xyz, q );
	result.TexCoord = input.TexCoord + float2(InstanceRotationTexture.w, 0);

	return result;
}


float4 Cube_PS(	VertexShaderOutput input ) : COLOR0
{
	const float ambient = 0.3f;
	float diffuse = saturate(dot(input.Normal, LightDirection.xyz ) );

	return tex2D(  AlbedoSampler, input.TexCoord.xy ) * (ambient + diffuse);
}

float4 CubeTransparent_PS( VertexShaderOutput input ) : COLOR0
{
	const float ambient = 0.3f;
	const float transparency = 0.5f;
	float diffuse = saturate(dot(input.Normal, LightDirection ) );

	float4 color = tex2D(  AlbedoSampler, input.TexCoord.xy ) * (ambient + diffuse);
	return float4( color.xyz, transparency );
}


technique Cube
{
    pass P0
    {
        VertexShader = compile vs_2_0 Cube_VS();
        PixelShader  = compile ps_2_0 Cube_PS();
    }
}

technique TransparentCube
{
    pass P0
    {
        VertexShader = compile vs_2_0 Cube_VS();
        PixelShader  = compile ps_2_0 CubeTransparent_PS();
    }
}

