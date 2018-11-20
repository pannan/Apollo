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

//texture2D  HammersleyTex;
//sampler2D HammersleySampler =
//sampler_state
//{
//	Texture = <HammersleyTex>;
//	MinFilter = Point;
//	MagFilter = Point;
//	MipFilter = Point;
//};

const float PI = 3.1415926f;
const float halfPI = 3.1415926f * 0.5f;
float cubeMapSize;
int ConvolutionSampleCount = 128;
int HammersleyTexSize;
int ConvolutionMaxSamples;

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

float3 importanceSampleDiffuse(float2 Xi, float3 N)
{
	float CosTheta = 1.0 - Xi.y;
	float SinTheta = sqrt(1.0 - CosTheta*CosTheta);
	float Phi = 2 * PI*Xi.x;

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float4 sumDiffuse(float3 diffuseSample, float NoV, float4 result)
{
	result.xyz += diffuseSample;
	result.w++;
	return result;
}

float3 debugIS(float3 N)
{
	//float2 Xi = DiffuseHammersleyBuffer[64];// tex2D(HammersleySampler, float2(0.5, 1)).xy;
	//return float3(Xi, 0);
	float3 V = N;

	float4 result = float4(0, 0, 0, 0);
	float SampleStep = 1.0f;
	uint sampleId = 0;
	float3 avgH = float3(0, 0, 0);

	for (uint i = 0; i < 64; i++)
	{
		//float2 hamm_uv = float2(sampleId / HammersleyTexSize, ConvolutionMaxSamples / HammersleyTexSize);
		float2 Xi = DiffuseHammersleyBuffer[i];// tex2D(HammersleySampler, hamm_uv).xy;
		float3 H = importanceSampleGGX(Xi, 0.9,N);
		float3 L = normalize(reflect(-V, H));// normalize(2 * dot(V, H) * H - V);
		float NoL = saturate(dot(N, L));
		avgH += H;
		//sampleId += SampleStep;
	}

	for (uint i = 64; i < 128; i++)
	{
		//float2 hamm_uv = float2(sampleId / HammersleyTexSize, ConvolutionMaxSamples / HammersleyTexSize);
		float2 Xi = DiffuseHammersleyBuffer[i];// tex2D(HammersleySampler, hamm_uv).xy;
		float3 H = importanceSampleGGX(Xi, 0.9, N);
		float3 L = normalize(reflect(-V, H));// normalize(2 * dot(V, H) * H - V);
		float NoL = saturate(dot(N, L));
		avgH += H;
		//sampleId += SampleStep;
	}

	return avgH / 128;
}

float3 ImportanceSample(float3 N)
{
	//return debugIS(N);
	//return texCUBElod(colorSampler, float4(N, 7)).xyz;
	float3 V = N;
	
	float4 result = float4(0, 0, 0, 0);
	float SampleStep = 1.0f;
	uint sampleId = 0;

	//直接for 0-128 会有临时寄存器不够的报错
	for (uint i = 0; i < 64; i++)
	{
		//float2 hamm_uv = float2(sampleId / HammersleyTexSize, ConvolutionMaxSamples / HammersleyTexSize);
		float2 Xi = DiffuseHammersleyBuffer[i];// tex2D(HammersleySampler, hamm_uv).xy;
		float3 H = importanceSampleDiffuse(Xi, N);
		float3 L = normalize(reflect(-V, H));// normalize(2 * dot(V, H) * H - V);
		float NoL = saturate(dot(N, L));
		{
			float pdf = max(0.0, dot(N, L) * INV_PI);

			float solidAngleTexel = 4 * PI / (6 * HammersleyTexSize * HammersleyTexSize);
			float solidAngleSample = 1.0 / (ConvolutionSampleCount * pdf);
			float lod = 0.5 * log2((float)(solidAngleSample / solidAngleTexel));

			float3 diffuseSample = texCUBElod(colorSampler, float4(H, lod)).xyz;
			result = sumDiffuse(diffuseSample, NoL, result);
		}
	}

	for (uint i = 64; i < 128; i++)
	{
		//float2 hamm_uv = float2(sampleId / HammersleyTexSize, ConvolutionMaxSamples / HammersleyTexSize);
		float2 Xi = DiffuseHammersleyBuffer[i];// tex2D(HammersleySampler, hamm_uv).xy;
			float3 H = importanceSampleDiffuse(Xi, N);
			float3 L = normalize(reflect(-V, H));// normalize(2 * dot(V, H) * H - V);
			float NoL = saturate(dot(N, L));
		{
			float pdf = max(0.0, dot(N, L) * INV_PI);

			float solidAngleTexel = 4 * PI / (6 * HammersleyTexSize * HammersleyTexSize);
			float solidAngleSample = 1.0 / (ConvolutionSampleCount * pdf);
			float lod = 0.5 * log2((float)(solidAngleSample / solidAngleTexel));

			float3 diffuseSample = texCUBElod(colorSampler, float4(H, lod)).xyz;
				result = sumDiffuse(diffuseSample, NoL, result);
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