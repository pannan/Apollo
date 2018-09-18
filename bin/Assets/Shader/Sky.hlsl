
cbuffer GlobalParameters : register(b0)
{
	float3		eyeWorldSpacePosition;
	float3		eyeEarthSpacePosition;
	//float4x4	inverseViewMatrix;
	//float4x4	inverseProjMatrix;
	float4x4 inverseViewProjMatrix;
	float2		expand;
}

cbuffer AtmosphereParameters 
{
	//�����㶥���뾶
	float		top_radius;
	//����뾶
	float		bottom_radius;		
	float2	expand2;
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
	//cameraRay = mul(inverseProjMatrix, cameraRay);
	//cameraRay = cameraRay / cameraRay.w;
	//cameraRay = mul(inverseViewMatrix, cameraRay);
	//return mul((float3x3)inverseViewMatrix, cameraRay.xyz);
	worldSpacePos = mul(worldSpacePos, inverseViewProjMatrix);
	worldSpacePos = worldSpacePos / worldSpacePos.w;

	float3 viewRay = normalize(worldSpacePos.xyz - eyeWorldSpacePosition);

	return viewRay;
}

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = float4(input.position.xyz, 1.0);
	output.eyeDirection = UVToCameraRay(input.uv);
	return output;
}

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float r = length(eyeEarthSpacePosition);	//eyeToEarthCenterDistance
	float3  earthCenterToEyeDirection = eyeEarthSpacePosition / r;
	input.eyeDirection = normalize(input.eyeDirection);
	float mu = dot(earthCenterToEyeDirection, input.eyeDirection);
	return float4(input.eyeDirection, 1);
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
	if (mu < 0 && bottom_radius*bottom_radius + r*r*(mu*mu - 1) > 0)
		return float4(1, 0, 0, 1);

	return float4(0, 0, 1, 1);
}

