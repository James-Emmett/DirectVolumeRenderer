#include "System/Hash32.h"

unsigned int Hash32::ComputeHash(const Byte* data, unsigned int size)
{
	// Computes a CRC32 of the structure
	unsigned int crc = 0xFFFFFFFF;

	auto ptr = reinterpret_cast<const uint8_t*>(data);
	for (unsigned int j = 0; j < size; ++j)
	{
		crc = (crc >> 8) ^ s_crc32[(crc & 0xff) ^ *ptr++];
	}

	return crc ^ 0xFFFFFFFF;
}