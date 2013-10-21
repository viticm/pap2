#include "StdAfx.h"
#include "KG3DMemoryFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TKG3DResourcePool<KG3DMemoryFile>g_cMemoryFilePool;

KG3DMemoryFile::KG3DMemoryFile(void)
:m_Position(0)
,m_Length(0)
{
	
}

KG3DMemoryFile::~KG3DMemoryFile(void)
{
}

HRESULT KG3DMemoryFile::Reset()
{
	m_Position = 0;
	m_Length = 0;
	return S_OK;
}

int KG3DMemoryFile::write(const void * _Str,size_t _Size)
{
	_ASSERTE(m_Position + _Size < GetCapability());

	memcpy(&m_byteData[m_Position],_Str,_Size);
	m_Position += _Size;
	
	m_Length = max(m_Length, m_Position);

	return (int)_Size;
}

int KG3DMemoryFile::read(void* _Str,size_t _Size)const
{
	_ASSERTE(m_Position + _Size < GetCapability());
	memcpy(_Str,&m_byteData[m_Position],_Size);
	m_Position += _Size;

	return (int)_Size;
}

void* KG3DMemoryFile::GetBuffer()
{
	return m_byteData;
}

DWORD KG3DMemoryFile::GetLength()
{
	return static_cast<DWORD>(m_Length);	//如果返回Pos，会有人用错的，写的时候用GetLength来得到总长，读的时候用GetPos得到总长// Last modify by Chen TianHong:2008-5-7 10:52:18
}

VOID KG3DMemoryFile::SetPos( size_t Pos )
{
	_ASSERTE(Pos < GetCapability());
	m_Position = Pos;
}

HRESULT KG3DMemoryFile::UnInit()
{
	Reset();
	return S_OK;
}
