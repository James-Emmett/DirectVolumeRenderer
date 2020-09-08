#pragma pack_matrix(column_major)

// Sky box used in the scene
TextureCube  _SkyMap 	: register(t0);
SamplerState _SkySample  : register(s0);

// Prefileted irradiance NdotL around hemisphere
TextureCube  _IrradianceMap 	: register(t1);
SamplerState _IrradianceSample  : register(s1);

// Pre-Convulted Specular irradiance
TextureCube  _SpecularEnvMap 	: register(t2);
SamplerState _SpecularEnvSample  : register(s2);

// // LUT for reconstructing specular
Texture2D _SpecularBRDFLUT : register(t3);
SamplerState _SpecularBRDFLUTSampe  : register(s3);

static const float Epsilon = 0.00001;
static const float PI = 3.14159265f;
static const float SQRT2 = 1.41421356237309504880;
static const float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   
static const float DBL_MAX = 1.7976931348623158e+308;

// Normal incidence Fresnel factor.
static const float3 Fdielectric  = float3(0.04, 0.04, 0.04);

cbuffer FrameBuffer : register(b0)
{
	float _Width;
	float _Height;
	float _Time;
	float _SinTime;
	float _CostTime;
	float _DeltaTime;
};

cbuffer CameraBuffer : register(b1)
{
	float4x4 _View;
	float4x4 _Projection;
	float4x4 _ViewProjection;
			 
	float4x4 _InvView;
	float4x4 _InvProjection;
	float4x4 _InvViewProjection;
			 
	float3 	 _CameraPosWorld;
}

cbuffer ObjectBuffer : register(b2)
{
	float4x4 _World;
	float4x4 _WorldToObject;
	float4x4 _WorldViewProj;
	float4x4 _NormalWorld;
	float3   _CameraPosLocal;
}

uint GetSpecularMipLevels()
{
	uint width, height, levels;
	_SpecularEnvMap.GetDimensions(0, width, height, levels);
	return levels;
}

float gold_noise(float2 xy, float seed)
{
       return frac(tan(distance(xy*PHI, xy)*seed)*xy.x);
}
