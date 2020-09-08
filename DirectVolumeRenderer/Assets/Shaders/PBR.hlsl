#include "globals.hlsl"

//I will make the engine reuse smaplers in future...
Texture2D 	 _DiffuseMap 	: register(t4);
SamplerState _DiffuseSample : register(s4);

Texture2D 	 _NormalMap 	: register(t5);
SamplerState _NormalSample  : register(s5);

Texture2D 	 _MetalnessMap 	: register(t6);
SamplerState _MetalnessSample  : register(s6);

Texture2D 	 _RoughnessMap 	: register(t7);
SamplerState _RoughnessSample  : register(s7);

struct AppData
{
	float3 pos      : POSITION;
	float3 normal   : NORMAL;
	float4 tangent  : TANGENT;
	float4 color    : COLOR;
	float2 uv       : TEXCOORD0;
};

struct v2f
{
	float4 pos       : SV_POSITION;
	float4 worldPos  : POSITION;
	float4 color     : COLOR;
	float2 uv        : TEXCOORD0;
	float3 normal    : TEXCOORD1;
	float3 tangent   : TEXCOORD2;
	float3 binormal  : TEXCOORD3;
};

v2f vert(AppData v)
{
	v2f o;
	o.pos       = mul(_WorldViewProj, float4(v.pos, 1));
	o.worldPos  = mul(_World, float4(v.pos, 1));
	o.normal    = normalize(mul(_World, float4(v.normal, 0.0f)).xyz);
	o.tangent   = normalize(mul(_World, float4(v.tangent.xyz, 0.0f)).xyz);
	o.binormal  = normalize(cross(o.normal, o.tangent)) * v.tangent.w;
	o.color     = v.color;
	o.uv        = v.uv;
	return o;
}

float TrowbridgeReitzGGX(float NdotH, float roughness)
{
	float r2   = roughness * roughness;
	float NdotH2 = NdotH * NdotH;
	float denom = max(NdotH2 * r2 + (1 - NdotH2), 0.00390625);
	return r2 / (PI * denom * denom);
}
  
// Single term for separable Schlick-GGX below.
float SchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Geometric GGX attenuation function, Smith method.
float GeometrySchlickSmith(float NdotL, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	return SchlickG1(NdotL,k) * SchlickG1(NdotV,k);
}

float3 FresnelSchlick(float cosT, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosT, 5.0);
}

float3 CookTorranceBRDF(float NdotL, float NdotV, float D, float G, float3 F)
{
	return (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
}

float3 CalculateNormal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
	float3 m = normalize((2.0f * (_NormalMap.Sample(_NormalSample, uv).xyz)) - 1.0f);
	float3 n = normalize(normal);
	float3 t = normalize(tangent - (n * dot(tangent, n))); // Force orthogonal
	float3 b = normalize(binormal);
	return (t * m.x + b * m.y + n * m.z);
}

float4 frag(v2f i): SV_TARGET
{
	float3 albedo    = _DiffuseMap.Sample(_DiffuseSample, i.uv).rgb;
	float roughness  = _RoughnessMap.Sample(_RoughnessSample, i.uv).b;
	float metalness  = _MetalnessMap.Sample(_MetalnessSample, i.uv).b;

	float3 LightDir = normalize(float3(0,0,1)); // Pass this in via buffer?	
	float3 N  = CalculateNormal(i.normal, i.tangent, i.binormal, i.uv);
	
	float3 L  = -LightDir; 
	float3 LightColor = float3(1,1,1);
	float3 LR = reflect(L, N);
	float3 V  = normalize(_CameraPosWorld - i.worldPos.xyz);
	float3 VR = normalize(reflect(-V, N));
	float3 H  = normalize(L + V);
	float  NdotL = max(dot(N, L),0);
    float  NdotV = max(dot(N, V),0);
	float  NdotH = max(dot(N, H),0);
	float  VdotH = max(dot(V, H),0);
	float  VdotR = max(dot(LR,V),0);
	float  LdotH = max(dot(L, H),0);
	float  LdotV = max(dot(L, V),0);
	
	float3 F0 = lerp(Fdielectric , albedo, metalness);
	float  D  = TrowbridgeReitzGGX(NdotH, roughness);
	float  G  = GeometrySchlickSmith(NdotL, NdotV, roughness);
	// Fresnel based on refletion to view angle
	float3 F  = FresnelSchlick(VdotH, F0);
	
	// Metals are 100% reflective thus we need to scale diffsue by metalness
	// to be energy conservative
	float3 KD = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
	
	float3 diffuseBDRF = KD * albedo;
	float3 specularBRDF = CookTorranceBRDF(NdotL, NdotV, D, G, F);
	float3 directLighting = (diffuseBDRF + specularBRDF) * NdotL;
	
	// Add specular irradiance, need to adjust engine to make content processing
	// much easier.
	float3 ambientLighting;
	// Irradiance Cubemap prefiled with NdotL 
	float3 irradiance = _IrradianceMap.Sample(_IrradianceSample, N).rgb;
	// Adjust F to be from general light direction
	F = FresnelSchlick(NdotV, F0);
	KD = lerp(1.0 - F, 0.0, metalness);
	
	// Result of diffuse irradiance 
	float3 diffuseIBL = KD * albedo * irradiance;
		
	// Sample pre-filtered specular reflection environment at correct mipmap level.
	uint specularTextureLevels = GetSpecularMipLevels();
	float3 specularIrradiance = _SpecularEnvMap.SampleLevel(_SpecularEnvSample, VR, roughness * specularTextureLevels).rgb;
	
	// Split-sum approximation factors for Cook-Torrance specular BRDF.
	float2 ambientSpec = _SpecularBRDFLUT.Sample(_SpecularBRDFLUTSampe, float2(NdotV, roughness)).rg;
	
	// Total specular IBL contribution.
	float3 specularIBL = (F0 * ambientSpec.x + ambientSpec.y) * specularIrradiance;
		
	// Diffuse + Specullar IBL
	ambientLighting = diffuseIBL + specularIBL;
	
	return float4(directLighting + ambientLighting,1);
}