
cbuffer GlobalParameters : register(b0)
{
	float4			eyeWorldSpacePosition;
	float4			eyeEarthSpacePosition;
	float4x4		inverseViewProjMatrix;
	float4x4		inverseViewMatrix;
	float4			projMat[4];
}

cbuffer AtmosphereParameters 
{
	//大气层顶部半径
	float		top_radius;
	//地球半径
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
	worldSpacePos = mul(worldSpacePos,inverseViewProjMatrix);
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
	ray = mul(inverseViewMatrix, float4(ray,0));
	return ray;
}

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = float4(input.position.xyz, 1.0);
	output.eyeDirection = UVToCameraRay2(input.uv);
	return output;
}

/*
在每个点，被散射到和入射方向成a度的光的比例由散射系数Bs和相位函数P得到。

对于空气分子，使用rayleigh模型
Bray_s(h,y) = (8*pow(PI,3) * pow(n*n - 1,2))/(3*N*pow(y,4)) * exp(-h/Hr)   //rayleigh散射系数
P = 3/(16*PI) * (1 + mu*mu)

h = r - bottom_radius
y = 波长
n = 空气的index of refraction
N = 在海平面的空气密度
Hr = 8km是如果密度均匀时的大气层厚度

Bray_s(h,y)变量有两个，h和y
原始论文使用一个特殊常数表示在海拔为0
*/

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float r = length(eyeEarthSpacePosition.xyz);	//eyeToEarthCenterDistance
	float3  earthCenterToEyeDirection = eyeEarthSpacePosition.xyz / r;
	input.eyeDirection = normalize(input.eyeDirection);
	float mu = dot(earthCenterToEyeDirection, input.eyeDirection);
	//return float4(input.eyeDirection, 1);
	/*
	在ray(r,mu)方向上距离d的一点可以定义为:r_d(r + mu*d,d * sqrt(1 - mu*mu))
	假设r_d = R:由 x*x + y*y = R*R得
	r*r + 2*r*mu*d + mu*mu*d*d + d*d - d*d*mu*mu = R*R ->
	r*r + 2*r*mu*d + d*d = R*R ->
	(r*mu + d) * (r*mu + d) - r*r*mu*mu + r*r = R*R ->
	r*mu + d = sqrt(R*R + r*r*mu*mu - r*r) ->
	d = sqrt(R*R + r*r*(mu*mu - 1)) - r*mu
	这样，如果ray(r,mu)和地面相交，R = bottom_radius,并且R*R + r*r*(mu*mu - 1) >= 0
	*/
	if (mu < 0 && bottom_radius*bottom_radius + r*r*(mu*mu - 1) > 0)
		return float4(1, 0, 0, 1);

	return float4(0, 0, 1, 1);
}

