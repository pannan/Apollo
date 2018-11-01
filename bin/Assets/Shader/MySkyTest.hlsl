#define _IN(x) in x
#define _OUT(x) out x
#define TEMPLATE(x) 
#define TEMPLATE_ARGUMENT(x) 
#define _HLSL
#define assert(x)
#define mod(x,y) fmod(x,y)

SamplerState	TransmittanceSampler;
SamplerState	ScatteringTextureSampler;
SamplerState	IrradianceTextureSampler;
SamplerState	SingleMieScatteringTextureSampler;

Texture3D	scattering_texture;
Texture3D  single_mie_scattering_texture;
//Texture2D irradiance_texture;

#include"SkyDefinitions.hlsl"
#include"Functions.hlsl"

RadianceSpectrum getSkyScatting(in AtmosphereParameters atmosphere, Length r, Number mu, Number mu_s,
	Number nu, bool ray_r_mu_intersects_ground,in ReducedScatteringTexture scattering_texture, 
	in ReducedScatteringTexture single_mie_scattering_texture)
{
	IrradianceSpectrum single_mie_scattering;
	IrradianceSpectrum scattering = GetCombinedScattering(
		atmosphere, scattering_texture, single_mie_scattering_texture,
		r, mu, mu_s, nu, ray_r_mu_intersects_ground, single_mie_scattering);

	return scattering * RayleighPhaseFunction(nu) + single_mie_scattering * MiePhaseFunction(atmosphere.mie_phase_function_g, nu);
}

cbuffer GlobalParameters : register(b0)
{
	float4			eyeWorldSpacePosition;
	float4			eyeEarthSpacePosition;
	float4			sunDirection;
	float4x4		inverseViewProjMatrix;
	float4x4		inverseViewMatrix;
	float4			projMat[4];
}

cbuffer GlobalParameters2 : register(b1)
{
	AtmosphereParameters atmosphere_;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float3 eyeDirection : TEXCOORD0;
};

inline float3 UVToCameraRay(float2 uv)
{
	uv.y = 1.0f - uv.y;
	float4 worldSpacePos = float4(uv * 2.0 - 1.0, 1.0, 1.0);
	worldSpacePos = mul(worldSpacePos, inverseViewProjMatrix);
	worldSpacePos = worldSpacePos / worldSpacePos.w;

	float3 viewRay = normalize(worldSpacePos.xyz - eyeWorldSpacePosition.xyz);

	return viewRay;
}

inline float3 UVToCameraRay2(float2 uv)
{
	uv.y = 1.0f - uv.y;
	float4 clipPos = float4(uv * 2.0 - 1.0, 0.5, 1.0);

	float3 ray;
	ray.z = (clipPos.z - projMat[3].z) / projMat[2].z;
	ray.x = clipPos.x * ray.z * projMat[2].w;
	ray.y = clipPos.y * ray.z * projMat[2].w / projMat[1].y;
	ray = normalize(ray);
	ray = mul(inverseViewMatrix, float4(ray, 0)).xyz;
	return ray;
}

VS_OUTPUT VSMAIN(_IN(VS_INPUT) input)
{
	VS_OUTPUT output;

	output.position = float4(input.position.xyz, 1.0);
	output.eyeDirection = UVToCameraRay2(input.uv);
	return output;
}

/*
��ÿ���㣬��ɢ�䵽�����䷽���a�ȵĹ�ı�����ɢ��ϵ��Bs����λ����P�õ���

���ڿ������ӣ�ʹ��rayleighģ��
Bray_s(h,y) = (8*pow(PI,3) * pow(n*n - 1,2))/(3*N*pow(y,4)) * exp(-h/Hr)   //rayleighɢ��ϵ��
P = 3/(16*PI) * (1 + mu*mu)

h = r - bottom_radius
y = ����
n = ������index of refraction
N = �ں�ƽ��Ŀ����ܶ�
Hr = 8km������ܶȾ���ʱ�Ĵ�������

Bray_s(h,y)������������h��y
ԭʼ����ʹ��һ�����ⳣ����ʾ�ں���Ϊ0
*/

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float r = length(eyeEarthSpacePosition.xyz);	//eyeToEarthCenterDistance
	float3  earthCenterToEyeDirection = eyeEarthSpacePosition.xyz / r;
	input.eyeDirection = normalize(input.eyeDirection);
	float mu = dot(earthCenterToEyeDirection, input.eyeDirection);
	float mu_s = dot(earthCenterToEyeDirection, sunDirection.xyz);
	float nu = dot(input.eyeDirection, sunDirection.xyz);
//return float4(input.eyeDirection, 1);
/*
��ray(r,mu)�����Ͼ���d��һ����Զ���Ϊ:r_d(r + mu*d,d * sqrt(1 - mu*mu))
����r_d = R:�� x*x + y*y = R*R��
r*r + 2*r*mu*d + mu*mu*d*d + d*d - d*d*mu*mu = R*R ->
r*r + 2*r*mu*d + d*d = R*R ->
(r*mu + d) * (r*mu + d) - r*r*mu*mu + r*r = R*R ->
r*mu + d = sqrt(R*R + r*r*mu*mu - r*r) ->
d = sqrt(R*R + r*r*(mu*mu - 1)) - r*mu
���������ray(r,mu)�͵����ཻ��R = bottom_radius,����R*R + r*r*(mu*mu - 1) >= 0
*/
	bool ray_r_mu_intersects_ground = false;
	if (mu < 0 && atmosphere_.bottom_radius*atmosphere_.bottom_radius + r * r*(mu*mu - 1) >= 0)
		ray_r_mu_intersects_ground = true;

//�������ڴ������Ⲣ�Ҳ��ʹ������ཻ
//�����������ڴ������ray�϶��ʹ������ཻ����������߶ȱ��볬��������
//���mu�����ϵģ��ض����ʹ������ཻ
//������£��ж�R*R + r*r*(mu*mu - 1)�Ƿ��н�
	if ((r >= atmosphere_.top_radius && mu < 0 && atmosphere_.top_radius*atmosphere_.top_radius + r * r*(mu*mu - 1) < 0) || (r >= atmosphere_.top_radius && mu >= 0))
		return float4(0, 0, 0, 1);

	float3 skyColor = getSkyScatting(atmosphere_, r, mu, mu_s, nu, ray_r_mu_intersects_ground, scattering_texture, single_mie_scattering_texture);
	return float4(skyColor,1);
}
