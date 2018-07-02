//--------------------------------------------------------------------------------
// FinalPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term of a standard lighting equation.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D       ColorMap00 : register( t0 );         

SamplerState	TexSampler : register(s0);

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
	return float4(vSample.rgb, 1);
	//return( vSample );
}

