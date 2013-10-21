// File: RWFile.h
// Desc:

#ifndef RWFILE_H
#define RWFILE_H

#include"File.h"

namespace Common
{

class DLLAPI RWFile: public File  // 一开始我想public继承ROFile和WOFile，重载一下Open，不过这样就形成了钻石形的类继承树，Effective C++一书不提倡这样，忘了为什么了
{
public:
	virtual void Open();

	virtual void ReadBuffer(void* pBuffer, unsigned int uiBufferSize);
	virtual void WriteBuffer(const void* pBuffer, unsigned int uiBufferSize);

	virtual unsigned int ReadLine(char* pBuffer, unsigned int uiBufferSize = 256);
	virtual String ReadLine();
	virtual void WriteLine(const String& sLine);

	void SwitchRW() const { fflush(m_File); }
};

}

#endif