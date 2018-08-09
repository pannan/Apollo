cbuffer PerObject : register(b0)
{
	float4x4	worldMatrix;
	float4x4	MVPMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv0 : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

Texture2D DiffuseMap : register(t0);
SamplerState	TexSampler : register(s0);

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(MVPMatrix, float4(input.position, 1));
	output.uv0 = input.uv0;
	output.normal = mul(worldMatrix, float4(input.normal, 0)).xyz;
	return output;
}

struct PS_OUTPUT
{
	float4 adelbo : SV_Target0;
	float4 normal : SV_Target1;
};

PS_OUTPUT PSMAIN(in VS_OUTPUT input)
{
	PS_OUTPUT output;
	output.adelbo = DiffuseMap.Sample(TexSampler, input.uv0);
	output.normal = float4(normalize(input.normal),1.0f);
	
	return output;
}

