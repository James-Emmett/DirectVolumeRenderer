#include "System/File.h"
#include <sys/stat.h>

//-----------------------------------------------------------------------------
BaseFile::BaseFile()
{
}

//-----------------------------------------------------------------------------
BaseFile::BaseFile(const char* fileName, FileMode mode)
{
	Open(fileName, mode);
}

//-----------------------------------------------------------------------------
BaseFile::~BaseFile()
{
}

//-----------------------------------------------------------------------------
bool BaseFile::Open(const char* fileName, FileMode mode)
{
	m_FileName = fileName;
	m_Mode = mode;

	const char* fileMode;

	if (m_Mode == FileMode::Read)
	{
		if (m_Type == FileType::Text)
		{
			fileMode = "r";
		}
		else
		{
			fileMode = "rb";
		}
	}
	else
	{
		if (m_Type == FileType::Text)
		{
			fileMode = "w";
		}
		else
		{
			fileMode = "wb";
		}
	}

	fopen_s(&m_File, fileName, fileMode);
	if (m_File == nullptr)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool BaseFile::Close()
{
	if (m_File)
	{
		fclose(m_File);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool BaseFile::IsOpen() const
{
	if (m_File)
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool BaseFile::IsFileEnd() const
{
	if (m_File)
	{
		return feof(m_File);
	}
	return false;
}

bool BaseFile::Exists(std::string fileName)
{
	FILE* file;
	fopen_s(&file, fileName.c_str(), "r");

	if (file)
	{
		fclose(file);
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
File::File()
{
	m_Type = FileType::Text;
}

//-----------------------------------------------------------------------------
File::File(const char* fileName, FileMode mode)
{
	m_Type = FileType::Text;
	Open(fileName, mode);
}

//-----------------------------------------------------------------------------
File::~File()
{
}

//-----------------------------------------------------------------------------
bool File::Read(char* buffer, Uint32 bufferSize)
{
	if (buffer != nullptr && m_Mode == FileMode::Read)
	{
		return fread(buffer, sizeof(char), bufferSize, m_File) >= bufferSize;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::ReadLine(char* buffer, Uint32 bufferSize)
{
	if (buffer != nullptr && m_Mode == FileMode::Read)
	{
		return fgets(buffer, bufferSize, m_File) != NULL;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::ReadLine(std::string& line, bool removeTralingSpace)
{
	if (m_Mode == FileMode::Read)
	{
		char buffer[1024];
		if (fgets(buffer, 1024, m_File) != NULL)
		{
			line = std::string(buffer);

			if (removeTralingSpace)
			{
				while ((line.length() > 0) && (isspace(line.back())))
				{
					line.pop_back();
				}
			}

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::Write(const char* buffer, Uint32 bufferSize)
{
	if (buffer != nullptr && m_Mode == FileMode::Write)
	{
		return (fwrite(buffer, sizeof(char), bufferSize, m_File) >= bufferSize) ? true : false;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::WriteLine(const std::string& line)
{
	if (m_Mode == FileMode::Write)
	{
		std::string out = line + "\n";
		return (fwrite(out.c_str(), sizeof(char), out.length(), m_File) >= out.length()) ? true : false;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::Load(const char* fileName, char* buffer, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "r");

	if (file != nullptr)
	{
		fread(buffer, sizeof(char), bufferSize, file);
		fclose(file);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::Save(const char* fileName, char* buffer, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "w");

	if (file != nullptr)
	{
		fwrite(buffer, sizeof(char), bufferSize, file);
		fclose(file);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool File::Append(const char* fileName, char* buffer, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "a");

	if (file != nullptr)
	{
		//fwrite("\n", sizeof(char), 1, file); // It wasnt adding a new line in some cases???
		fwrite(buffer, sizeof(char), bufferSize, file);
		fclose(file);
		return true;
	}

	return false;
}

bool File::DirectoryExists(const char* path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	else if (info.st_mode & S_IFDIR)
	{
		return true;
	}

	// Not directory
	return false;
}

//-----------------------------------------------------------------------------
BinaryFile::BinaryFile()
{
	m_Type = FileType::Binary;
}

//-----------------------------------------------------------------------------
BinaryFile::BinaryFile(const char* fileName, FileMode mode)
{
	m_Type = FileType::Binary;
	Open(fileName, mode);
}

//-----------------------------------------------------------------------------
BinaryFile::~BinaryFile()
{
}

//-----------------------------------------------------------------------------
bool BinaryFile::WriteByte(const Byte& value)
{
	if (m_Mode != FileMode::Read)
	{
		fwrite(&value, 1, sizeof(Byte), m_File);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool BinaryFile::WriteWord(const Word& value)
{
	if (m_Mode != FileMode::Read)
	{
		Byte bytes[2];
		bytes[0] = value & 0xFF;
		bytes[1] = (value >> 8) & 0xFF;

		fwrite(bytes, 1, sizeof(bytes), m_File);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool BinaryFile::WriteDword(const Dword& value)
{
	if (m_Mode != FileMode::Read)
	{
		Byte bytes[4];
		bytes[0] = value & 0xFF;
		bytes[1] = (value >> 8) & 0xFF;
		bytes[2] = (value >> 16) & 0xFF;
		bytes[3] = (value >> 24) & 0xFF;
		fwrite(bytes, 1, sizeof(bytes), m_File);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// I had issues writing floats for some reason, so this hack was invented.
//-----------------------------------------------------------------------------
bool BinaryFile::WriteFloat(const float& value)
{
	if (m_File && m_Mode != FileMode::Write)
	{
		//Type pruning hack?
		union u
		{
			float f;
			Byte data[4];
		}bytes;
		bytes.f = value;

		fwrite(&bytes, sizeof(u), sizeof(bytes), m_File);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
Byte BinaryFile::ReadByte()
{
	if (m_File && m_Mode == FileMode::Read)
	{
		Byte byte;
		fread(&byte, 1, sizeof(byte), m_File);
		return byte;
	}

	return Byte();
}

//-----------------------------------------------------------------------------
Word BinaryFile::ReadWord()
{
	if (m_File && m_Mode == FileMode::Read)
	{
		Byte byte[2];
		fread(byte, 1, sizeof(byte), m_File);
		return (byte[0] | byte[1] << 8);
	}

	return Byte();
}

//-----------------------------------------------------------------------------
Dword BinaryFile::ReadDword()
{
	if (m_File && m_Mode == FileMode::Read)
	{
		Byte byte[4];
		fread(byte, 1, sizeof(byte), m_File);
		return (byte[0] | byte[1] << 8 | byte[2] << 16 | byte[3] << 24);
	}

	return Byte();
}

//-----------------------------------------------------------------------------
float BinaryFile::ReadFloat()
{
	if (m_File && m_Mode == FileMode::Read)
	{
		float value;
		fread(&value, 1, sizeof(value), m_File);
		return value;
	}

	return float();
}

//-----------------------------------------------------------------------------
bool BinaryFile::WriteBuffer(const Byte* data, unsigned int byteCount)
{
	if (m_File && m_Mode == FileMode::Write)
	{
		fwrite(data, 1, byteCount, m_File);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool BinaryFile::ReadBuffer(Byte* data, unsigned int byteCount)
{
	if (m_File && m_Mode == FileMode::Read)
	{
		if (fread(data, 1, byteCount, m_File) >= byteCount)
		{
			return true;
		}
	}

	perror("Error: ");
	return false;
}

bool BinaryFile::WriteString(const std::string& string)
{
	if (!string.empty())
	{
		WriteDword((Dword)string.size() + 1);
		WriteBuffer((Byte*)string.c_str(), (unsigned int)string.size());

		char n = '\0';
		WriteByte(n); // Make sure to write null terminator.

		return true;
	}
	return false;
}

std::string BinaryFile::ReadString()
{
	std::string string;
	int n = ReadDword();
	char* buffer = new char[n];
	ReadBuffer((Byte*)buffer, n);
	string = buffer;
	delete[] buffer;

	return string;
}

//-----------------------------------------------------------------------------
bool BinaryFile::Load(const char* fileName, Byte* data, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "rb");

	if (file != nullptr)
	{
		fread((char*)&data, sizeof(Byte), bufferSize, file);
		fclose(file);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
bool BinaryFile::Save(const char* fileName, const Byte* data, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "rb");

	if (file != nullptr)
	{
		if (fwrite((char*)&data, sizeof(Byte), bufferSize, file) >= bufferSize)
		{
			fclose(file);
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
bool BinaryFile::Append(const char* fileName, const Byte* data, Uint32 bufferSize)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "ab");

	if (file != nullptr)
	{
		if (fwrite(data, 1, bufferSize, file) >= bufferSize)
		{
			fclose(file);
			return true;
		}
	}

	return false;
}
