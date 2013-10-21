// File: Singleton.h
// Desc:

#ifndef SINGLETON_H
#define SINGLETON_H

#include"DLLAPI.h"

namespace Common
{

template<class T>
class DLLAPI Singleton
{
protected:
	static T* m_pInstance;

protected:
	Singleton() {}

private:
	Singleton(const Singleton<T>& sl) {}
	Singleton<T>& operator = (const Singleton<T>& sl) { return *this; }

public:
	static T* GetInstance();
};

template<class T>
T* Singleton<T>::m_pInstance = 0;

template<class T>
T* Singleton<T>::GetInstance()
{
	if(!m_pInstance)
	{
		m_pInstance = new T;
	}

	return m_pInstance;
}

}  // namespace

#endif