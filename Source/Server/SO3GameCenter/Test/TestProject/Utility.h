#pragma  once
#include "Common/kg_memory.h"
#include "ScopedPtr.h"

inline void ComRelease(void* p)
{
	IUnknown* pCom = static_cast<IUnknown*>(p);
	if (pCom)
	{
		pCom->Release();
	}
}

typedef ScopedPtrMalloc<IUnknown, ComRelease> ComScopedPrt;
typedef ScopedPtrMalloc<IKG_Buffer, ComRelease> BufferScopedPrt;

template<class T>
class ResumeScopeValue
{
public:
	ResumeScopeValue(T* pValue, T NewValue)
	{
		m_pValue = pValue;
		m_OldValueCopy = *pValue;
		*pValue = NewValue;
	}
	~ResumeScopeValue()
	{
		*m_pValue = m_OldValueCopy;
	}
private:
	T m_OldValueCopy;
	T* m_pValue;
};