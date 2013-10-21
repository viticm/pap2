#ifndef _INCLUDE_KDLLHOLDER_H_
#define _INCLUDE_KDLLHOLDER_H_

////////////////////////////////////////////////////////////////////////////////

class KDllHolder
{
public:
	
	BOOL Init(LPCSTR lpstrDllName);
	void UnInit();
	HMODULE GetDllHandle();
	
	KDllHolder();
	KDllHolder(LPCSTR lpstrDllName);
	virtual ~KDllHolder();
private:
	HMODULE m_hMod;
};

#endif //_INCLUDE_KDLLHOLDER_H_
