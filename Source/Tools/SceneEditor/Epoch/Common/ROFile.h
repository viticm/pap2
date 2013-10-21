// File: ROFile.h
// Desc: Ö»¶ÁÎÄ¼þ

#ifndef ROFILE_H
#define ROFILE_H

#include"File.h"

namespace Common
{

class DLLAPI ROFile: public File
{
public:
	virtual void Open();

	virtual void ReadBuffer(void* pBuffer, unsigned int uiBufferSize);

	virtual unsigned int ReadLine(char* pBuffer, unsigned int uiBufferSize = 256);
	virtual String ReadLine();
};

}  // namespace

#endif