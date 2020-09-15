#pragma once
#include <cstdint>

typedef unsigned char		Uint8;
typedef unsigned short		Uint16;
typedef unsigned long		Uint32;
typedef unsigned long long	Uint64;

typedef signed char			Int8;
typedef short				Int16;
typedef long				Int32;
typedef long long			Int64;

typedef float				F32;
typedef float				F64;

typedef unsigned char  Byte;  // 8 Bit
typedef unsigned short Word;  // 16 bit
typedef unsigned int   Dword; // 32 bit

typedef void* WindowHandle;

struct Handle
{
	Uint16 Index;
	Uint16 Generation = UINT16_MAX;

	Handle()
	{
		Index = 0;
		Generation = UINT16_MAX;
	}

	bool IsValid()const
	{
		return Generation != UINT16_MAX;
	}

	bool operator==(const Handle& handle)const
	{
		return (Index == handle.Index) && (Generation == handle.Generation);
	}
};

// Jsut a single ID without GENERATION!!!
struct HandlePlain
{
	Uint16 Index = UINT16_MAX;

	HandlePlain()
	{
		Index = UINT16_MAX;
	}

	HandlePlain(Uint16 index)
	{
		Index = index;
	}

	bool IsValid()const
	{
		return Index != UINT16_MAX;
	}
};