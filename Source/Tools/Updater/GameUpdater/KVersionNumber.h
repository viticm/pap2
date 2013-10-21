///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KVersionNumber.h
// Creator  : chengzhaohui
// Date     : 2008-3-27
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KVERSIONNUMBER_H_
#define _KVERSIONNUMBER_H_

class KVersionNumber
{
public:
	KVersionNumber();
	KVersionNumber(const TCHAR szVersion[]);	
	~KVersionNumber(void);

	int operator==(const KVersionNumber &rVersion);
	int operator> (const KVersionNumber &rVersion);
	int operator= (const TCHAR szVersion[]);

	
private:
	enum{MAX_VERSION_LENGTH = 256};
	
	int Parse(const TCHAR szVersion[]);
	int IdentifyType(const TCHAR szVersion[]);

private:
	int m_nVersionSect1;
	int m_nVersionSect2;
	int m_nVersionSect3;
	int m_nVersionSect4;

	TCHAR m_cType;
};

#endif //_KVERSIONNUMBER_H_
