// File: RWFile.cpp
// Desc:

#include"RWFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Common
{

void RWFile::Open()
{
	if(m_sFilename.empty())
	{
		throw FileError("NULL Filename", EL_BAD, "RWFile::Open", "Filename is NULL!");
	}

	Close();
	const char* p = m_sFilename.c_str();
	m_File = fopen(m_sFilename.c_str(), "r+b");
	if(!m_File)
	{
		throw FileNotFound(m_sFilename.c_str());
	}
}

void RWFile::ReadBuffer(void* pBuffer, unsigned int uiBufferSize)
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

void RWFile::WriteBuffer(const void* pBuffer, unsigned int uiBufferSize)
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

unsigned int RWFile::ReadLine(char* pBuffer, unsigned int uiBufferSize)
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

String RWFile::ReadLine()
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

void RWFile::WriteLine(const String& sLine)
{
	if(!m_File)
	{
		throw FileError("NULL File pointer", EL_BAD, "RWFile::WriteLine", "File hasn't been opened!");
	}

	String sTemp = sLine + "\r\n";
	WriteBuffer((const void*)sTemp.c_str(), sTemp.GetSize());
}

}  // namespace