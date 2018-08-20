struct Light
{
	//16bit
	float4	positionWS;
	//16bit
	float4   directionWS;
	//16bit
	float4   color;
	//16bit
	float       spotlightAngle;
	float       range;
	float       intensity;
	uint		  type;
};

cbuffer Light currentLigh;

Texture2D DiffuseTextureVS : register(t0);
Texture2D NormalTextureVS : register(t1);

sampler LinearClampSampler      : register(s1);

struct VSInput
{
	float3 position : POSITION;	
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VSOutput
{
	float3 positionVS   : TEXCOORD0;    // View space position.
	float2 texCoord     : TEXCOORD1;    // Texture coordinate
	float3 tangentVS    : TANGENT;      // View space tangent.
	float3 binormalVS   : BINORMAL;     // View space binormal.
	float3 normalVS     : NORMAL;       // View space normal.
	float4 position     : SV_POSITION;  // Clip space position.
};