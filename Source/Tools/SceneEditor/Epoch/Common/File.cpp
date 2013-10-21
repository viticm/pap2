// File: File.cpp
// Desc:

#include"File.h"

namespace Common
{

File::~File()
{
	Close();
}

void File::SetFilename(const String& sName)
{
	Close();

	m_sFilename = sName;
}

void File::Open()
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::Open", "Should never call this!");
}

void File::Close()
{
	if(m_File)
	{
		fclose(m_File);
		m_File = 0;
	}
}

bool File::IsFileExist(const String& sName)
{
	FILE* f = fopen(sName, "rb");
	if(f)  // The file exists.
	{
		fclose(f);
		return true;
	}

	return false;
}

int File::TranslateSeekDir(SeekStart ss) const
{
	int ret;

	switch(ss)
	{
	case SS_BEG:
		ret = SEEK_SET;
		break;

	case SS_CUR:
		ret = SEEK_CUR;
		break;

	case SS_END:
		ret = SEEK_END;
		break;

	default:
		throw Exception(EL_BAD, "File::SetPosition", "Bad SeekDir parameter!");
	}

	return ret;
}

void File::ReadBuffer(void* pBuffer, unsigned int uiBufferSize)
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::ReadBuffer", "Should never call this!");
}

void File::WriteBuffer(const void* pBuffer, unsigned int uiBufferSize)
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::WriteBuffer", "Should never call this!");
}

unsigned int File::ReadLine(char* pBuffer, unsigned int uiBufferSize)
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::ReadLine", "Should never call this!");
}

String File::ReadLine()
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::ReadLine", "Should never call this!");
}

void File::WriteLine(const String& sLine)
{
	throw FileError(m_sFilename.c_str(), EL_BAD, "File::WriteLine", "Should never call this!");
}

}