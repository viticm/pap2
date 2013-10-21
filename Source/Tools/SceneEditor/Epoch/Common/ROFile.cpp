// File: ROFile.cpp
// Desc:

#include"ROFile.h"

namespace Common
{

void ROFile::Open()
{
	if(m_sFilename.empty())
	{
		throw FileError("NULL Filename", EL_BAD, "ROFile::Open", "Filename is NULL!");
	}

	Close();
	
	m_File = fopen(m_sFilename.c_str(), "rb");
	if(!m_File)
	{
		throw FileNotFound(m_sFilename.c_str());
	}
}

void ROFile::ReadBuffer(void* pBuffer, unsigned int uiBufferSize)
{
	if(!m_File)
	{
		throw FileError("NULL File pointer", EL_BAD, "ROFile::ReadBuffer", "File hasn't been opened!");
	}

	unsigned int rc = fread(pBuffer, uiBufferSize, 1, m_File);

	if(feof(m_File))
	{
		throw FileEOF(m_sFilename.c_str(), rc);
	}
	else if(ferror(m_File))
	{
		throw FileReadError(m_sFilename.c_str(), EL_BAD, "ROFile::ReadBuffer", "Error while reading file:\r\n" + m_sFilename);
	}
}

unsigned int ROFile::ReadLine(char* pBuffer, unsigned int uiBufferSize)
{
	char c;

	unsigned int uiRead = 0;
	while(uiRead < (uiBufferSize - 1))
	{
		try
		{
			ReadBuffer((void*)&c, 1);
			++uiRead;
		}
		catch(FileEOF fe)
		{
			break;
		}

		if(c == '\0' || c == '\n')
		{
			--uiRead;
			break;
		}

		*pBuffer++ = c;
	}

	if(*(pBuffer - 1) != '\0')
	{
		*pBuffer = '\0';
	}

	return uiRead;
}

String ROFile::ReadLine()
{
	String sTemp;
	char c;

	while(true)
	{
		try
		{
			ReadBuffer((void*)&c, 1);
		}
		catch(FileEOF fe)
		{
			break;
		}

		if(c == '\0' || c == '\n')
		{
			break;
		}

		sTemp += c;
	}

	return sTemp;
}

}  // namespace