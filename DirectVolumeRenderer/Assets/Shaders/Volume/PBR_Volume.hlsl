#include "..\globals.hlsl"

Texture3D _VolumeMap      : register(t4);
Texture3D _OccupancyMap   : register(t5);
Texture2D _AlbedoTransfer : register(t6);
Texture2D _NoiseText      : register(t7);

SamplerState _VolumeSampler  	  : register(s4);
SamplerState _OccupancyMapSampler : register(s5);
SamplerState _AlbedoSampler 	  : register(s6);
SamplerState _NoiseSampler   	  : register(s7);

static const int MAX_SAMPLES = 800;	

cbuffer PerMaterial : register(b3)
{
	float  _Hounsfield;
	float3 _StepSize;
	float  _Iterations;
	float3 _VolumeSize;
	float3 _OccupancySize;
};

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
	float4 worldPos : POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float4 color    : COLOR;
	float2 uv       : TEXCOORD0;
	float4 vertex : TEXCOORD1;
};

v2f vert(AppData v)
{
	v2f o;
	o.position = mul(_WorldViewProj, float4(v.position, 1));
	o.worldPos = mul(_World, float4(v.position, 1));
	o.normal   = mul(_NormalWorld, float4(v.normal, 1)).xyz;
	o.tangent  = v.tangent;
	o.color    = v.color;
	o.uv       = v.uv;
	o.vertex   = float4(v.position,1);
	return o;
}

float DistributionGGX(float NdotH, float roughness)
{
	float a = NdotH * roughness;
    float k = roughness / (1.0 - NdotH * NdotH + a * a);
    return k * k * (1.0 / PI);
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

float2 RayBoxIntersection(float3 origin, float3 dir, const float3 boxMin, const float3 boxMax)
{
	float3 invDir = 1.0 / dir;
	
	float3 tA = (boxMin - origin) * invDir;
	float3 tB = (boxMax - origin) * invDir;
	float3 tMin = min(tA, tB);
	float3 tMax = max(tA, tB);
	float t0 = max(tMin.x, max(tMin.y, tMin.z));
	float t1 = min(tMax.x, min(tMax.y, tMax.z));
	
	return float2(t0, t1);
}

struct Ray
{
	float3 Start;
	float3 End;
	float3 Dir;
	float  Length;
};

Ray GetRay(float3 origin, float3 dir, const float3 boxMin, const float3 boxMax)
{
	Ray ray;
	
	float2 points = RayBoxIntersection(origin, dir, boxMin, boxMax);
	points.x = max(points.x, 0.0); // Dont sample behind camera.
	
	ray.Start 	= 0.5f * (origin + dir * points.x) + 0.5f;
	ray.End	  	= 0.5f * (origin + dir * points.y) + 0.5f;
	
	ray.Dir 	= dir;
	ray.Length  = length(ray.End - ray.Start);

	return ray;
}


/*
// Origional no space skipping 
float4 frag(v2f i) : SV_TARGET
{
	Ray ray = GetRay(_CameraPosLocal, normalize(i.vertex.xyz - _CameraPosLocal), float3(-1,-1,-1), float3(1,1,1));
	float4 color = float4(0,0,0,0);
	float3 L     = -normalize(float3(0,0,1)); 
	
	//--DDA--
	float3 rayStart  = ray.Start;
	float3 rayEnd  	 = ray.End;
	float3 rayDir 	 = ray.Dir;
	float3 stepDelta = ray.Dir 	 * _StepSize;

	// Kind of works???
	float3 p = rayStart;
	int iteractions = length((rayEnd - rayStart) * _VolumeSize) / _StepSize;
	// need a way to determine how many samples should be run.
	for(int j = 0; j < iteractions; ++j)
	{
		// Sample origional volume with p
		float4 voxel = _VolumeMap.SampleLevel(_VolumeSampler, p, 0).rgba;
			
		// Run shading if intensity is high enough.
		if(voxel.w > _Hounsfield)
		{		
			float4 microfacet = _AlbedoTransfer.SampleLevel(_AlbedoSampler, float2(voxel.w, 0.5), 0);
			float4 albedo 	  = _AlbedoTransfer.SampleLevel(_AlbedoSampler, float2(voxel.w, 0), 0);
			float metalness   = microfacet.r;
			float roughness   = microfacet.g;
			float3 N          = mul(_NormalWorld, float4((2.0 * voxel.rgb - 1.0f), 0.0f)).xyz;
			N *= -1;
			
			float3 V     = normalize(_CameraPosWorld - i.worldPos.xyz);
			float3 VR    = normalize(reflect(-V, N));
			float3 H     = normalize(L + V);
			float  NdotL = max(dot(N, L),0);
			float  NdotV = max(dot(N, V),0);
			float  NdotH = max(dot(N, H),0);
			float  VdotH = max(dot(V, H),0);
		
			float3 F0 = lerp(Fdielectric , albedo.xyz, metalness);
			float  D  = DistributionGGX(NdotH, roughness);
			float  G  = GeometrySchlickSmith(NdotL, NdotV, roughness);
			float3 F  = FresnelSchlick(VdotH, F0);	
		
			//--Diffuse and Specular--
			float3 KD = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);	
			float3 diffuseBDRF 	= KD * albedo.xyz;
			float3 specularBRDF = CookTorranceBRDF(NdotL, NdotV, D, G, F);
			float3 directLighting = (diffuseBDRF + specularBRDF) * NdotL;
			
			//--Enviromental Lighting--
			float3 irradiance = _IrradianceMap.SampleLevel(_IrradianceSample, N, 0).rgb;
			F = FresnelSchlick(NdotV, F0);
			KD = lerp(1.0 - F, 0.0, metalness);
			
			float3 diffuseIBL = KD * albedo.xyz * irradiance;
			float3 specularIrradiance = _SpecularEnvMap.SampleLevel(_SpecularEnvSample, VR, roughness * GetSpecularMipLevels()).rgb;
			float2 ambientSpec = _SpecularBRDFLUT.SampleLevel(_SpecularBRDFLUTSampe, float2(NdotV, roughness), 0).rg;
			float3 specularIBL = (F0 * ambientSpec.x + ambientSpec.y) * specularIrradiance;
			float3 ambientLighting = diffuseIBL + specularIBL;
		
			//--Finalize color--
			float4 src = float4(directLighting + ambientLighting, albedo.w);
			src.rgb *= src.a;
			color = (1.0f - color.a)*src + color;
			
			// Break as alpha is diminishing returns
			if(color.a > 0.95f)
			{
				// We have to return color here, as breaking would only break inner loop!
				return color;
			}
		}
		
		// Step position by some fixed amount
		p += stepDelta;
		
		// Check if we left the occupancy volume!
		if (p[0] < 0 || p[0] > 1.0f || 
			p[1] < 0 || p[1] > 1.0f ||
			p[2] < 0 || p[2] > 1.0f)
		{
			return color;
		}
		
	}
	
	return color;
}
*/



// 3D-DDA -Na Method Fast GPU 
float4 frag(v2f i) : SV_TARGET
{
	Ray ray = GetRay(_CameraPosLocal, normalize(i.vertex.xyz - _CameraPosLocal), float3(-1,-1,-1), float3(1,1,1));
	
	float4 src   = float4(0,0,0,0);
	float4 color = float4(0,0,0,0);
	float3 L     = -normalize(float3(0,0,1)); 
	
	//--DDA--
	float3 rayStart  = ray.Start * _OccupancySize;
	float3 rayEnd  	 = ray.End 	 * _OccupancySize;
	float3 rayDir 	 = ray.Dir	 * _OccupancySize;
	float3 stepDelta = ray.Dir 	 * _StepSize;
	float  previousT;
	
	float3 t = float3(4294967295,4294967295,4294967295);
	float3 deltaT = float3(4294967295,4294967295,4294967295);
	float3 voxelIncr = float3(0,0,0);
	float3 stepDir = float3(0,0,0);
	
	// Current voxel were starting on
	int3 currentV = int3(rayStart.x, rayStart.y, rayStart.z);
	int3 lastVoxel = int3(rayEnd.x, rayEnd.y, rayEnd.z);
	
	if(ray.Dir.x != 0)
	{
		stepDir.x = (rayDir.x < 0)? -1 : 1;
		deltaT.x = 1.0f /rayDir.x * stepDir.x;
		t.x = ((currentV.x + stepDir.x) - rayStart.x) / rayDir.x;
	}

	if(ray.Dir.y != 0)
	{
		stepDir.y = (rayDir.y < 0)? -1 : 1;
		deltaT.y = 1.0f /rayDir.y * stepDir.y;
		t.y = ((currentV.y + stepDir.y) - rayStart.y) / rayDir.y;
	}

	if(ray.Dir.z != 0)
	{
		stepDir.z = (rayDir.z < 0)? -1 : 1;
		deltaT.z = 1.0f /rayDir.z * stepDir.z;
		t.z = ((currentV.z + stepDir.z) - rayStart.z) / rayDir.z;
	}
	
	// This seems more reliable than checking if we left the volume for some reason?
	int iteractions = length((ray.End - ray.Start) * _OccupancySize);
	for(int j = 0; j <= iteractions; ++j)
	{	
		// Finds shortest axis its the next t intersection
		voxelIncr.x = (t.x <= t.y) && (t.x <= t.z);
		voxelIncr.y = (t.y <= t.x) && (t.y <= t.z);
		voxelIncr.z = (t.z <= t.x) && (t.z <= t.y);
		
		// cache previousT, probably a more effecint way?
		previousT = t.x * voxelIncr.x + t.y * voxelIncr.y + t.z * voxelIncr.z;
		// Increment T by delta on the axis with next intersect
		t += voxelIncr * deltaT;
		// Move to the next voxel
		currentV += voxelIncr * stepDir;

		// Get intensity of voxel at the current voxel, normalize currentV to do so!
		float intensity = _OccupancyMap.SampleLevel(_OccupancyMapSampler, currentV/_OccupancySize, 0).r;
		
		// Run shading if intensity is high enough.
		if(intensity >= _Hounsfield)
		{
			float3 start = (ray.Start + (previousT * ray.Dir));
			float3 end = (ray.Start + (t * ray.Dir));
			
			// This mathmatically should be the ideal steps for the inner voxel reigon!
			int innerVoxelSteps = length((start - end)/ _StepSize);
			for(int j = 0; j < 10; ++j)
			{
				// Sample origional volume with p
				float4 voxel = _VolumeMap.SampleLevel(_VolumeSampler, start, 0).rgba;
				
				if(voxel.w >= _Hounsfield)
				{
					float4 microfacet = _AlbedoTransfer.SampleLevel(_AlbedoSampler, float2(voxel.w, 0.5), 0);
					float4 albedo 	  = _AlbedoTransfer.SampleLevel(_AlbedoSampler, float2(voxel.w, 0), 0);
					float metalness   = microfacet.r;
					float roughness   = microfacet.g;
					float3 N          = mul(_NormalWorld, float4((2.0 * voxel.rgb - 1.0f), 0.0f)).xyz;
					N *= -1;
					
					float3 V     = normalize(_CameraPosWorld - i.worldPos.xyz);
					float3 VR    = normalize(reflect(-V, N));
					float3 H     = normalize(L + V);
					float  NdotL = max(dot(N, L),0);
					float  NdotV = max(dot(N, V),0);
					float  NdotH = max(dot(N, H),0);
					float  VdotH = max(dot(V, H),0);
				
					float3 F0 = lerp(Fdielectric , albedo.xyz, metalness);
					float  D  = DistributionGGX(NdotH, roughness);
					float  G  = GeometrySchlickSmith(NdotL, NdotV, roughness);
					float3 F  = FresnelSchlick(VdotH, F0);	
				
					//--Diffuse and Specular--
					float3 KD = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);	
					float3 diffuseBDRF 	= KD * albedo.xyz;
					float3 specularBRDF = CookTorranceBRDF(NdotL, NdotV, D, G, F);
					float3 directLighting = (diffuseBDRF + specularBRDF) * NdotL;
					
					//--Enviromental Lighting--
					float3 irradiance = _IrradianceMap.SampleLevel(_IrradianceSample, N, 0).rgb;
					F = FresnelSchlick(NdotV, F0);
					KD = lerp(1.0 - F, 0.0, metalness);
					
					float3 diffuseIBL = KD * albedo.xyz * irradiance;
					float3 specularIrradiance = _SpecularEnvMap.SampleLevel(_SpecularEnvSample, VR, roughness * GetSpecularMipLevels()).rgb;
					float2 ambientSpec = _SpecularBRDFLUT.SampleLevel(_SpecularBRDFLUTSampe, float2(NdotV, roughness), 0).rg;
					float3 specularIBL = (F0 * ambientSpec.x + ambientSpec.y) * specularIrradiance;
					float3 ambientLighting = diffuseIBL + specularIBL;
				
					//--Finalize color--
					src = float4(directLighting + ambientLighting, albedo.w);
					src.rgb *= src.a;
					color = (1.0f - color.a)*src + color;
					
					// Break as alpha is diminishing returns
					if(color.a > 0.95f)
					{
						// We have to return color here, as breaking would only break inner loop!
						return color;
					}
				}
				
				// Step position by some fixed amount
				start += stepDelta;
			}
		}
	}
	return color;
}