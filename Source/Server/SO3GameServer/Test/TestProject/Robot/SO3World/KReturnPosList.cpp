#include "stdafx.h"
#include "KReturnPosList.h"

#define RETURN_POS_LIST_FILE_NAME	"ReturnPos.tab"

#define RETURN_POS_INDEX			"Index"
#define RETURN_POS_MAP_ID			"MapID"
#define RETURN_POS_X				"nX"
#define RETURN_POS_Y				"nY"
#define RETURN_POS_Z				"nZ"

KReturnPosList::KReturnPosList(void)
{
	memset(m_ReturnPosList, 0, sizeof(m_ReturnPosList));
}

KReturnPosList::~KReturnPosList(void)
{
}

BOOL KReturnPosList::Init(void)
{
	BOOL bRetCode = FALSE;

	memset(m_ReturnPosList, 0, sizeof(m_ReturnPosList));

	ITabFile* pTabFile = g_OpenTabFile(RETURN_POS_LIST_FILE_NAME);
	KGLOG_PROCESS_ERROR(pTabFile);

	for (int nIndex = 1; nIndex < pTabFile->GetHeight(); nIndex++)
	{
		int nPosIndex = 0;
		
		bRetCode = pTabFile->GetInteger(nIndex + 1, RETURN_POS_INDEX, 0, &nPosIndex);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (nIndex == 1)
			KGLOG_PROCESS_ERROR(nPosIndex == 0);

		bRetCode = pTabFile->GetInteger(nIndex + 1, RETURN_POS_MAP_ID, 0, 
			(int*)&(m_ReturnPosList[nPosIndex].dwMapID));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nIndex + 1, RETURN_POS_X, 0,
			&(m_ReturnPosList[nPosIndex].nX));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nIndex + 1, RETURN_POS_Y, 0,
			&(m_ReturnPosList[nPosIndex].nY));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pTabFile->GetInteger(nIndex + 1, RETURN_POS_Z, 0,
			&(m_ReturnPosList[nPosIndex].nZ));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	KG_COM_RELEASE(pTabFile);

	return TRUE;

Exit0:
	KG_COM_RELEASE(pTabFile);

	return FALSE;
}

KReturnPos* KReturnPosList::GetReturnPos(int nIndex)
{
	KGLOG_PROCESS_ERROR(nIndex > 0 && nIndex < MAX_RETURN_POS);

	return &(m_ReturnPosList[nIndex]);

Exit0:

	return NULL;
}
