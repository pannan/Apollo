cbuffer PerObject : register(b0)
{
	float4x4 MVPMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv0 : TEXCOORD0;
};

Texture2D       HeightMap : register(t0);

SamplerState	TexSampler : register(s0);


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	//float height = HeightMap.Sample(TexSampler, input.uv0).x;
	float height = HeightMap.Load(float3(100,100,1)).x * 256;
	float4 terrainLocalPos = float4(input.position.x, height, input.position.z, 1);
	output.position = mul(MVPMatrix, terrainLocalPos);
	output.uv0 = input.uv0;
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	return float4(1,0,0,1);
	//return( vSample );
}

