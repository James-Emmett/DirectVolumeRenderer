// Renders the RenderTarget to set render target
// basically just to blit to back buffer as the final step!

Texture2D _Scene : register(t0);
SamplerState _SceneSampler : register(s0);

struct AppData
{
	float3 pos      : POSITION;
	float2 uv       : TEXCOORD0;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

v2f vert(uint id : SV_VERTEXID)
{
	v2f o;
	o.uv = float2((id << 1) & 2, id & 2 );
	o.pos = float4(o.uv * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );
    return o;
}

float4 frag(v2f i) : SV_Target
{
	return _Scene.Sample(_SceneSampler, i.uv);
}