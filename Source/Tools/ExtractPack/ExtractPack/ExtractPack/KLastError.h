/*****************************************************************************************
//	FileName:   KErrorBeae.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2008/12/22-10:50
//  Comment :   用于记录错误信息
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _KERRORBASE_H__
#define _KERRORBASE_H__

class KLastError
{
public:
	KLastError(void);
	virtual ~KLastError(void);

public:
	static int GetErrorCode();
	static void SetErrorCode(int nErrorCode);
 
private:

	static int m_nErrorCode;

};
#endif // _KERRORBASE_H__