cbuffer PerObject : register(b0)
{
	float4x4	MVPMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
};

VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	//output.position = mul(MVPMatrix, float4(input.position,1));
	output.position = mul(float4(input.position, 1),MVPMatrix);
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{	
	return float4(1,1,0,1);
	//return( vSample );
}

