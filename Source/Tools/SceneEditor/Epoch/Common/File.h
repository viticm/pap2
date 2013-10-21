// File: File.h
// Desc:

#ifndef FILE_H
#define FILE_H

#include<stdio.h>

#include"String.h"
#include"FileError.h"

namespace Common
{

enum SeekStart
{
	SS_BEG,
	SS_CUR,
	SS_END
};

class DLLAPI File
{
protected:
	FILE* m_File;
	String m_sFilename;

private:
	File(const File& file) {}
	File& operator = (const File& file) { return *this; }

public:
	File(): m_File(0) {}
	virtual ~File();

	void SetFilename(const String& sName);
	void GetFilename(String& sName) const { sName = m_sFilename; }

	virtual void Open();
	void Close();

	static bool IsFileExist(const String& sName);
	bool IsFileExist() const { return IsFileExist(m_sFilename); }
	bool IsFileEOF() const { return feof(m_File); }
	bool IsFileOpen() const { return m_File; }

	int TranslateSeekDir(SeekStart ss) const;
	void SetPosition(long lPos, SeekStart ss = SS_BEG) { fseek(m_File, lPos, TranslateSeekDir(ss)); }
	unsigned int GetPosition() const { return ftell(m_File); }

	virtual void ReadBuffer(void* pBuffer, unsigned int uiBufferSize);
	virtual void WriteBuffer(const void* pBuffer, unsigned int uiBufferSize);

	virtual unsigned int ReadLine(char* pBuffer, unsigned int uiBufferSize = 256);
	virtual String ReadLine();
	virtual void WriteLine(const String& sLine);

	template<class T> void Read(T* pData);
	template<class T> void Write(const T& data);
};

template<class T>
void File::Read(T* pData)
{
	ReadBuffer((void*)pData, sizeof(T));
}

template<class T>
void File::Write(const T& data)
{
	WriteBuffer((void*)&data, sizeof(data));
}

}  // namespace

#endif