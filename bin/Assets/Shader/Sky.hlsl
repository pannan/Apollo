

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv0 : TEXCOORD0;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	int3 screenspace = int3( input.position.x, input.position.y, 0 );
	float2 uv = input.uv0;// input.position.xy;
	//float4 vSample = ColorMap00.Load( screenspace );
	float4 vSample = ColorMap00.Sample(TexSampler, uv);
	return float4(pow(vSample.rgb,1/2.2), 1);
	//return( vSample );
}

