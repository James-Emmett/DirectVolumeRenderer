#pragma once
//NOTE:
/*
*  Potentialy change this too streaming?
*/

#pragma once
#include "System/Types.h"
#include <cstdio>
#include <string>

enum class FileMode { Read, Write };
enum class FileType { Text, Binary };

//----------Base File----------
class BaseFile
{
protected:
	FILE* m_File = nullptr;
	std::string  m_FileName = "";
	FileMode    m_Mode = FileMode::Read;
	FileType    m_Type = FileType::Binary; //Internal only

public:
	BaseFile();
	BaseFile(const char* fileName, FileMode mode);
	~BaseFile();

public:
	bool Open(const char* fileName, FileMode mode);
	bool Close();
	bool IsOpen()const;
	bool IsFileEnd()const;
	static bool Exists(std::string fileName);
};

//----------Text File----------
class File : public BaseFile
{
public:
	File();
	File(const char* fileName, FileMode mode);
	~File();

public:
	bool Read(char* buffer, Uint32 bufferSize);
	bool ReadLine(char* buffer, Uint32 bufferSize);
	bool ReadLine(std::string& line, bool removeTralingSpace);
	bool Write(const char* buffer, Uint32 bufferSize);
	bool WriteLine(const std::string& line);

public:
	static bool Load(const char* fileName, char* buffer, Uint32 bufferSize);
	static bool Save(const char* fileName, char* buffer, Uint32 bufferSize);
	static bool Append(const char* fileName, char* buffer, Uint32 bufferSize);

	// Maybe i need a Directory class?
	static bool DirectoryExists(const char* path);

};

//----------Binary File----------
class BinaryFile : public BaseFile
{
public:
	BinaryFile();
	BinaryFile(const char* fileName, FileMode mode);
	~BinaryFile();

public:
	//--Write--
	bool WriteByte(const Byte& value);
	bool WriteWord(const Word& value);
	bool WriteDword(const Dword& value);
	bool WriteFloat(const float& value);

	//--Read--
	Byte  ReadByte();
	Word  ReadWord();
	Dword ReadDword();
	float ReadFloat();

	//--Write Buffer--
	bool WriteBuffer(const Byte* data, unsigned int byteCount);
	bool ReadBuffer(Byte* data, unsigned int byteCount);

	//--Write String--
	bool WriteString(const std::string& string);
	std::string ReadString();

public:
	static bool Load(const char* fileName, Byte* data, Uint32 bufferSize);
	static bool Save(const char* fileName, const Byte* data, Uint32 bufferSize);
	static bool Append(const char* fileName, const Byte* data, Uint32 bufferSize);
};