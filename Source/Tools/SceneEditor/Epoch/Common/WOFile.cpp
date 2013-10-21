// File; WOFile.cpp
// Desc:

#include"WOFile.h"

namespace Common
{

void WOFile::Open()
{
	if(m_sFilename.empty())
	{
		throw FileError("NULL Filename", EL_BAD, "WOFile::Open", "Filename is NULL!");
	}

	Close();
	
	m_File = fopen(m_sFilename.c_str(), "wb");
	if(!m_File)
	{
		throw FileNotFound(m_sFilename.c_str());
	}
}

void WOFile::WriteBuffer(const void* pBuffer, unsigned int uiBufferSize)
{
	if(!m_File)
	{
		throw FileError("NULL File pointer", EL_BAD, "WOFile::WriteBuffer", "File hasn't been opened!");
	}

	fwrite(pBuffer, uiBufferSize, 1, m_File);
	if(ferror(m_File))
	{
		throw FileWriteError(m_sFilename, EL_GENERAL, "WOFile::WriteBuffer", "Error while writing file:\r\n" + m_sFilename);
	}
}

void WOFile::WriteLine(const String& sLine)
{
	if(!m_File)
	{
		throw FileError("NULL File pointer", EL_BAD, "WOFile::WriteLine", "File hasn't been opened!");
	}

	String sTemp = sLine + "\r\n";
	WriteBuffer((const void*)sTemp.c_str(), sTemp.GetSize());
}

}  // namespace