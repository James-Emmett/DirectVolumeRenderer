// For this to work with any format you need a data range to normalize agaist?

Texture3D<uint> _InputBuffer : register(t0);
RWTexture3D<uint4> _Result : register(u0);

cbuffer VolumeData : register(b0)
{
	float _MinValue;
	float _MaxValue;
	float _Range;
	float _InitialMax;
};

uint ReadPixel(uint x, uint y, uint z)
{
	return _InputBuffer.Load(int4(x,y,z, 0));
}

void WritePixel(uint3 id, float3 normal, uint w)
{
	_Result[id] = uint4(normal, w);
}

[numthreads(8, 8, 8)]
void CSMain(uint3 id : SV_DispatchThreadID )
{
	// Fetch neighbours and Normalize intensity values
	float3 s1; float3 s2;
	s1.x = ReadPixel(id.x - 1, id.y, id.z);
	s2.x = ReadPixel(id.x + 1, id.y, id.z);
	s1.y = ReadPixel(id.x, id.y - 1, id.z);
	s2.y = ReadPixel(id.x, id.y + 1, id.z);
	s1.z = ReadPixel(id.x, id.y, id.z - 1);
	s2.z = ReadPixel(id.x, id.y, id.z + 1);
	
	// Normalize vector and map back to image range (0 - 255)
	float3 normal = (normalize(s2 - s1) * 0.5 + 0.5) * 255;
	
	// Remap the data to be 0-255 using the data range clamp at either end
	WritePixel(id, normal, ((ReadPixel(id.x, id.y, id.z) - _MinValue) / _Range) * _InitialMax);
	//WritePixel(id, normal, ReadPixel(id.x, id.y, id.z) * 255);
}