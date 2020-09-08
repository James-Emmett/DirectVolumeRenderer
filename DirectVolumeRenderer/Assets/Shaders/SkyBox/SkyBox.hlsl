#include "..\globals.hlsl"

cbuffer PerDraw : register(b3)
{
	float4x4 _SkyWorldViewProject;
}

struct AppData
{
	float3 pos      : POSITION;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float3 texCoords : TEXCOORD0;
};

v2f vert(AppData v)
{
	v2f o;
	// Use ww so it is always far away
	o.pos       = mul(_SkyWorldViewProject, float4(v.pos, 1)).xyww;
	o.texCoords = v.pos; // Cube vert acts like a ray!
	return o;
}

float4 frag(v2f i): SV_TARGET
{
	return _SkyMap.Sample(_SkySample, i.texCoords);
}