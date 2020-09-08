#include "..\..\globals.hlsl"

struct AppData
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float4 color    : COLOR;
	float2 uv       : TEXCOORD0;
};

struct v2f
{
	float4 position : SV_POSITION;
	float4 local 	: TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 frontPos : SV_TARGET0;
	float4 backPos  : SV_TARGET1;
};

v2f vert(AppData v)
{
	v2f o;
	o.position = mul(_WorldViewProj, float4(v.position, 1));
	o.local    = float4(v.position,1);
	return o;
}

PS_OUTPUT frag(v2f i, bool isFrontFacing : SV_IsFrontFace)
{

	float3 pos = float3(i.local.x, i.local.y, i.local.z);
	
	PS_OUTPUT o;
	
	if(isFrontFacing)
	{
		o.frontPos = float4((0.5f * pos.xyz + 0.5f), 1.0f);
		o.backPos  = float4(0,0,0,0);
	}
	else
	{
		o.frontPos = float4(0,0,0,0);
		o.backPos  = float4((0.5f * pos.xyz + 0.5f), 1.0f);
	}
	
	return o;
}