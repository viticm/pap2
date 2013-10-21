#ifndef _KBOOK_LIST_
#define _KBOOK_LIST_

#include "Global.h"

class KPlayer;

struct KBookDB 
{
    size_t  uDataLen;
    BYTE    byData[0];
};

class KBookList
{
public:
	BOOL    Init(KPlayer* pPlayer);
	void    UnInit();

    BOOL    Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    Load(BYTE* pbyData, size_t uDataLen);

	BYTE    GetBookState(DWORD dwBookID);
	BOOL    SetBookState(DWORD dwBookID, BYTE byState);
	BOOL    AddBook(DWORD dwBookID, DWORD dwSubID);
	BOOL    IsBookMemorized(DWORD dwBookID, DWORD dwSubID);
    BOOL    IsBookMemorized(int nRecipeID);
    void    Clear();

private:
	BYTE        m_byBookList[MAX_READ_BOOK_ID];
	KPlayer*    m_pSelf;
};

#endif //_KBOOK_LIST_
