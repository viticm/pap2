#pragma once
/************************************************************************/
/*
GetLength是得到总长，GetPos是得到当前写的位置。如果是用g_cMemoryFile写的话
，用GetLength总是能得到正确的写的字节大小，而如果没有用过SetPos的话，GetPos的结果应该一致。
如果用g_cMemoryFile读的话，GetLength将是已经写的大小。
用Reset会连Length也清空，所以要SetPos(0)，read完，再用GetPos()，就是总读的大小
*/
/************************************************************************/
#include "KG3DTemplates.h"

class KG3DMemoryFile
{
private:
	mutable size_t m_Position;	//相当于指针
	size_t m_Length;	//已经写的长度

	enum
	{
		EM_BUFFER_SIZE = 1024*1024*4,
	};
	BYTE  m_byteData[EM_BUFFER_SIZE];

public:
	size_t GetCapability()const{return EM_BUFFER_SIZE;}
	void* GetBuffer();

	HRESULT Reset();

	int write(const void * _Str,size_t _Size);
	int read(void* _Str,size_t _Size)const;

	DWORD GetLength();
	DWORD GetMaxLength()	{return static_cast<DWORD>(GetCapability());}
	size_t GetPos()const	{return m_Position;}//可以用这个来控制读写位置
	VOID SetPos(size_t Pos);

	KG3DMemoryFile(void);
	~KG3DMemoryFile(void);

	HRESULT UnInit();
};

extern TKG3DResourcePool<KG3DMemoryFile>g_cMemoryFilePool;