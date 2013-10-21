#ifndef _KUSER_PREFERENCES_
#define _KUSER_PREFERENCES_

#include "Global.h"

class KPlayer;

class KUserPreferences
{
public:
	KUserPreferences(void);
	~KUserPreferences(void);

	BOOL  Init(KPlayer* pPlayer);
	void  UnInit();

    BOOL  Load(BYTE* pbyData, size_t uDataLen);
    BOOL  Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);

	BOOL  SetUserPreferences(int nOffset, int nLength, BYTE* pbyData);
	BOOL  GetUserPreferences(int nOffset, int nLength, BYTE* pbyData);
	BYTE* GetUserPreferences(int nOffset);
	BYTE* GetData();
private:
	BYTE        m_byUserPrefences[MAX_USER_PREFERENCES_LEN];
    KPlayer*    m_pPlayer;
};

#endif //_KUSER_PREFERENCES_
