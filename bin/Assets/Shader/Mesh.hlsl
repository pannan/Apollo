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

VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul(MVPMatrix, float4(input.position,1));
	output.uv0 = input.uv0;
	output.normal = mul(worldMatrix, float4(input.normal, 0)).xyz;
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float3 lightDir = normalize(float3(1,1,1));
	float3 normal = normalize(input.normal);
	float diffuseLighting = saturate(dot(normal, lightDir));
	float3 diffuseColor = pow(DiffuseMap.Sample(TexSampler, input.uv0).rgb,2.2f);
	//return float4(normal, 1);
	float3 srgb = pow(diffuseLighting * diffuseColor, 1.0f/2.2f);
	return float4(srgb,1);
	//return( vSample );
}

