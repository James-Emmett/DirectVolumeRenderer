#include "..\globals.hlsl"

Texture3D _VolumeMap     : register(t4);
Texture3D _OccupancyMap     : register(t5);
Texture2D _NoiseText      : register(t6);

SamplerState _VolumeSampler  	: register(s4);
SamplerState _OccupancyMapSampler	: register(t5);
SamplerState _NoiseSampler   	: register(s6);

static const int MAX_SAMPLES = 800;	

cbuffer PerMaterial : register(b3)
{
	float3 _ScaleFactor;
	float  _Hounsfield;
	float3 _StepSize;
	float  _Iterations;
	float3 _OccupancyDims;
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
	float4 pos      : TEXCOORD1;
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
	o.pos      = float4(v.position,1);
	return o;
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
	
	ray.Start = 0.5f * (_CameraPosLocal + dir * points.x) + 0.5;
	ray.End	  = 0.5f * (_CameraPosLocal + dir * points.y) + 0.5;
	ray.Dir = ray.End - ray.Start;
	ray.Length  = length(ray.Dir);
	ray.Dir /= ray.Length;
	return ray;
}

float4 frag(v2f i) : SV_TARGET
{
	Ray ray = GetRay(_CameraPosLocal, normalize(i.pos.xyz - _CameraPosLocal), float3(-1,-1,-1), float3( 1, 1, 1));

    float3 stepDelta = _StepSize * ray.Dir;
    float3 p = ray.Start + stepDelta * _NoiseText.Sample(_NoiseSampler, i.pos.xy).r;

	float4 voxel;
	float mip;
	for(int j = 0; j < _Iterations; j++)
	{
        voxel = _VolumeMap.SampleLevel(_VolumeSampler, p, 0).rgba;
		
		if(voxel.w > _Hounsfield)
		{
			if(voxel.w > mip)
			{
				mip = voxel.w;
			}
		}
		
        p += stepDelta;
		
		if(p.x > 1.0f || p.y > 1.0f || p.z > 1.0f ||
		   p.x < 0.0f || p.y < 0.0f || p.z < 0.0f)
		{
			break;
		}
    }
	
	return float4(mip,mip,mip,mip);
}