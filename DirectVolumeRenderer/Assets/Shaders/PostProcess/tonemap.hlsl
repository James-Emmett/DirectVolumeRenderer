Texture2D _Scene : register(t0);
SamplerState _SceneSampler : register(s0);

cbuffer material : register(b2)
{
	float _Gamma = 2.2;
	float _Exposure = 16.0;
	int   _ToneMapType = 0;
}

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

// Create fullscreen quad
v2f vert(uint id : SV_VERTEXID)
{
	v2f o;
	o.uv = float2((id << 1) & 2, id & 2 );
	o.pos = float4(o.uv * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );
    return o;
}

float3 UnchartedToneMap(float3 x) 
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

    return ((x*(A*x+C*B)+D*E) / (x*(A*x+B)+D*F))- E / F;
}

float3 ApplyUnchartedToneMap(float3 color) 
{
	color = color * _Exposure;
	float3 current = UnchartedToneMap(color * 2.0f);
	float3 whiteScale = 1.0f / UnchartedToneMap(11.2);
	color = current * whiteScale;
	return pow(color, 1.0f / _Gamma);
}

float3 ApplyReinhard(float3 color)
{
	color *= _Exposure;
	color = color / (1 + color);
	return pow(color, 1.0f / _Gamma);
}

float4 frag(v2f i) : SV_Target
{
	float3 color = _Scene.Sample(_SceneSampler, i.uv).rgb;
	
	// Yeah branches i know...
	if(_ToneMapType == 0)
	{
		color = ApplyReinhard(color.rgb);
	}
	else if (_ToneMapType == 1)
	{
		color = ApplyUnchartedToneMap(color.rgb);
	}

	return float4(color, 1.0f);
}
