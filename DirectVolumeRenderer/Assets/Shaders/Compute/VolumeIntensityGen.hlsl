Texture3D<float4> _InputBuffer : register(t0);
RWTexture3D<float> _Result : register(u0);

cbuffer VolumeData : register(b0)
{
	float3 _VolumeDimensions;
	float3 _VoxelsPerCell;
};

float ReadPixel(uint x, uint y, uint z)
{
	return _InputBuffer.Load(int4(x,y,z, 0)).w;
}

void WritePixel(uint3 id, float data)
{
	_Result[id] = data;
}

[numthreads(8, 8, 8)]
void CSMain(uint3 id : SV_DispatchThreadID )
{
	// Get start of voxel cube to compute
	uint3 volumePos = _VoxelsPerCell * id;
	
	// Get min and max bounding points
	float3 minVolumePos = max(volumePos - 1, float3(0,0,0));
	float3 maxVolumePos = min(volumePos + _VoxelsPerCell + 1, _VolumeDimensions);
	
	// Find the maz iso value for this chunk of the volume!
	int Width = maxVolumePos.x, Height = maxVolumePos.y, Depth = maxVolumePos.z;
	float fmax = 0;
	for (int z = minVolumePos.z; z < Depth; z++)
	{
		for (int y = minVolumePos.y; y < Height; y++)
		{
			for (int x = minVolumePos.x; x < Width; x++)
			{	
				fmax = max(fmax, ReadPixel(x, y, z));
			}
		}
	}

	WritePixel(id, fmax);
}