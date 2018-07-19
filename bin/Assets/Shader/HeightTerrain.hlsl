cbuffer PerObject : register(b0)
{
	float4x4 MVPMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	uint vertexid	: SV_VertexID;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv0 : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

Texture2D       HeightMap : register(t0);

SamplerState	TexSampler : register(s0);

StructuredBuffer<float3>					VertexNormalBuffer;

VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	//float height = HeightMap.Sample(TexSampler, input.uv0).x;
	float height = HeightMap.Load(float3(input.uv0.x * 1024, input.uv0.y * 1024,0)).x * 50;
	float4 terrainLocalPos = float4(input.position.x * 4, height, input.position.z * 4, 1);
	output.position = mul(MVPMatrix, terrainLocalPos);
	output.uv0 = input.uv0;
	output.normal = VertexNormalBuffer[input.vertexid];
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float3 lightDir = normalize(float3(1,1,1));
	float diffuseLighting = dot(input.normal, lightDir);
	return float4(diffuseLighting, diffuseLighting, diffuseLighting,1);
	//return( vSample );
}

