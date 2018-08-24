#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

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

Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);

sampler LinearClampSampler      : register(s1);

struct VS_INPUT
{
	float3 position : POSITION;	
	float2 uv0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float3 positionWS   : TEXCOORD0;    
	float2 uv0     : TEXCOORD1;    
	float4 position     : SV_POSITION;  
};

cbuffer ScreenToWorldParams
{
	float4x4 InverseViewProjection;
	float2 ScreenDimensions;
}

float4 ClipToWorld(float4 clip)
{
	// View space position.
	float4 world = mul(InverseViewProjection, clip);
	// Perspecitive projection.
	world = world / world.w;

	return world;
}

// Convert screen space coordinates to view space.
float4 ScreenToWorld(float4 screen)
{
	// Convert to normalized texture coordinates
	float2 texCoord = screen.xy / ScreenDimensions;

	// Convert to clip space
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToWorld(clip);
}

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = float4(input.position, 1);
	output.uv0 = input.uv0;
	return output;
}

float3 DoDirectionalLight(Light light,float3 normalWS,float3 diffuseColor)
{
	float diffuseLight = saturate(dot(normalWS,light.directionWS));
	return diffuseLight * diffuseColor;
}

float3 DoPointLight()
{
	return float3(0, 0, 0);
}

float3 DoSpotLight()
{
	return float3(0, 0, 0);
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float3 diffuseColor = DiffuseTexture.Sample(LinearClampSampler, input.uv0).rgb;
	float3 normal = NormalTexture.Sample(LinearClampSampler, input.uv0).rgb;

	float3 lightColor = float3(0, 0, 0);
	switch (currentLigh.Type)
	{
	case DIRECTIONAL_LIGHT:
		lightColor = DoDirectionalLight(light, mat, V, P, N);
		break;
	case POINT_LIGHT:
		lightColor = DoPointLight();
		break;
	case SPOT_LIGHT:
		lightColor = DoSpotLight();
		break;
	}

	return float4(lightColor, 1);
}