#include "..\globals.hlsl"

struct AppData
{
	float3 position : POSITION;
};

struct v2f
{
	float4 position : SV_POSITION;	
};

v2f vert(AppData v)
{
	v2f o;
	o.position = mul(_WorldViewProj, float4(v.position, 1));
	return o;
}

float4 frag(v2f i): SV_TARGET
{
	return float4(1,1,1,1);
}