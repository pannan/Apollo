#include "Shader\PostEffectCommon.hlsl"

float INV_PI = 1.0 / 3.14159;

textureCUBE  colorTex;
samplerCUBE colorSampler =
sampler_state
{
	Texture = <colorTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

const float PI = 3.1415926f;
const float halfPI = 3.1415926f * 0.5f;
float cubeMapSize;
int ConvolutionSampleCount = 128;
int HammersleyTexSize;
int ConvolutionMaxSamples;
float ConvolutionRoughness;

float2 DiffuseHammersleyBuffer[128];

float3 importanceSampleGGX(float2 Xi, float roughness, float3 N)
{
	float a = roughness * roughness;

	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float4 sumSpecular(float3 hdrPixel, float NoL, float4 result)
{
	result.xyz += (hdrPixel * NoL);
	result.w += NoL;
	return result;
}

float specularD(float roughness, float NoH)
{
	float r2 = roughness * roughness;
	float NoH2 = NoH * NoH;
	float a = 1.0 / (3.14159*r2*pow(NoH, 4));
	float b = exp((NoH2 - 1) / r2 * NoH2);
	return  a * b;
}

float3 ImportanceSample(float3 R)
{
	float3 N = R;
	float3 V = R;
	float4 result = float4(0, 0, 0, 0);

	//直接for 0-128 会有临时寄存器不够的报错
	for (uint i = 0; i < 64; i++)
	{
		float2 Xi = DiffuseHammersleyBuffer[i];// Hammersley(sampleId, ConvolutionMaxSamples);
		float3 H = importanceSampleGGX(Xi, ConvolutionRoughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = max(dot(N, L), 0);
		float VoL = max(dot(V, L), 0);
		float NoH = max(dot(N, H), 0);
		float VoH = max(dot(V, H), 0);
		if (NoL > 0)
		{
			float Dh = specularD(ConvolutionRoughness, NoH);
			float pdf = Dh * NoH / (4 * VoH);
			float solidAngleTexel = 4 * PI / (6 * 128 * 128);
			float solidAngleSample = 1.0 / (128 * pdf);
			float lod = ConvolutionRoughness == 0 ? 0 : 0.5 * log2((float)(solidAngleSample / solidAngleTexel));

			float3 pixel = texCUBElod(colorSampler, float4(H, lod)).xyz;
			result = sumSpecular(pixel, NoL, result);
		}
	}

	for (uint i = 64; i < 128; i++)
	{
		float2 Xi = DiffuseHammersleyBuffer[i];// Hammersley(sampleId, ConvolutionMaxSamples);
			float3 H = importanceSampleGGX(Xi, ConvolutionRoughness, N);
			float3 L = 2 * dot(V, H) * H - V;
			float NoL = max(dot(N, L), 0);
		float VoL = max(dot(V, L), 0);
		float NoH = max(dot(N, H), 0);
		float VoH = max(dot(V, H), 0);
		if (NoL > 0)
		{
			float Dh = specularD(ConvolutionRoughness, NoH);
			float pdf = Dh * NoH / (4 * VoH);
			float solidAngleTexel = 4 * PI / (6 * 128 * 128);
			float solidAngleSample = 1.0 / (128 * pdf);
			float lod = ConvolutionRoughness == 0 ? 0 : 0.5 * log2((float)(solidAngleSample / solidAngleTexel));

			float3 pixel = texCUBElod(colorSampler, float4(H, lod)).xyz;
				result = sumSpecular(pixel, NoL, result);
		}
	}

	if (result.w == 0)
		return result.xyz;
	else
		return (result.xyz / result.w);
}

float4 px_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(1.0, -st.y, -st.x);

	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled,1);
}

float4 nx_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(-1.0, -st.y, st.x);
	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled, 1);
}

float4 py_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(st.x, 1, st.y);
	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled, 1);
}

float4 ny_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(st.x, -1, -st.y);
	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled, 1);
}

float4 pz_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(st.x, -st.y, 1);
	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled, 1);
}

float4 nz_ps(float2 texcoord: TEXCOORD0) : COLOR0
{
	//to [-1,+1]
	float2 st = texcoord * 2.0f + float2(-1, -1);
	float3 direction = float3(-st.x, -st.y, -1);
	float3 importanceSampled = ImportanceSample(direction);
	return float4(importanceSampled, 1);
}


technique px
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 px_ps();
	}
}

technique nx
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 nx_ps();
	}
}

technique py
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 py_ps();
	}
}

technique ny
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 ny_ps();
	}
}

technique pz
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 pz_ps();
	}
}

technique nz
{
	pass
	{
		VertexShader = compile vs_3_0 main_vs();
		PixelShader = compile ps_3_0 nz_ps();
	}
}