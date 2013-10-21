#include "stdafx.h"
#include "KAttribGroup.h"
#include "Global.h"

BOOL KAttribGroup::Init(const char cszFileName[])
{
	BOOL        bResult         = false;
	int         nRetCode        = false;
	ITabFile*   piFile          = NULL;
    int         nHeight         = 0;
    int         nWidth          = 0;
    int         nGroupColIndex  = 0;
    int         nGroupColCount  = 0;
    pair<ATTR_GROUP_MAP::iterator, bool> InsResult;

	assert(cszFileName);

	piFile = g_OpenTabFile(cszFileName);
	KGLOG_PROCESS_ERROR(piFile);

	nHeight = piFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);
    
    nGroupColIndex = piFile->FindColumn("Group1");
    KGLOG_PROCESS_ERROR(nGroupColIndex > 0);

    nWidth = piFile->GetWidth();
    KGLOG_PROCESS_ERROR(nWidth > 1);
    
    nGroupColCount = nWidth - nGroupColIndex + 1;
    KGLOG_PROCESS_ERROR(nGroupColIndex > 0 && nGroupColIndex < MAX_ATTRIB_GROUP_NUM);

	for (int i = 0; i < nGroupColCount; ++i)
	{
        ATTR_GROUP  AttrGroup;
		char	    szKey[32];

		sprintf(szKey, "Group%d", i + 1);

		for (int nIndex = 2; nIndex < nHeight + 1; ++nIndex)
		{
			KAttribRate	AR;
			int     nRate   = 0;
			DWORD	dwID    = 0;

			nRetCode = piFile->GetInteger(nIndex, "ID", 0, (int*)&dwID);
			KGLOG_PROCESS_ERROR(nRetCode != 0);

			nRetCode = piFile->GetInteger(nIndex, szKey, 0, &nRate);
			KGLOG_PROCESS_ERROR(nRate >= 0);

			if (nRate > 0)
			{
				AR.nAttribIndex		= dwID;
				AR.nRate			= nRate;

				AttrGroup.push_back(AR);
			}
		}
        
        InsResult = m_AttrGroupMap.insert(make_pair(i, AttrGroup));
        KGLOG_PROCESS_ERROR(InsResult.second);
	}

	bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "KAttribGroup load file \"%s\" failed !\n", cszFileName);
    }
	KG_COM_RELEASE(piFile);
	return bResult;
}

void KAttribGroup::UnInit()
{
    m_AttrGroupMap.clear();
}

int KAttribGroup::GetAttribIndex(int nGroup)
{
	int                         nResult             = ERROR_ID;
    int                         nRand               = 0;
    int                         nCurrentRate        = 0;
    int                         nSeclectAttribIndex = ERROR_ID;
    ATTR_GROUP_MAP::iterator    AttrMapIt;
    ATTR_GROUP::iterator        AttrGroupIt;
    ATTR_GROUP::iterator        AttrGroupItEnd;

	KGLOG_PROCESS_ERROR(nGroup > 0 && nGroup < MAX_ATTRIB_GROUP_NUM);

	nRand = g_Random(MILLION_NUM);

    AttrMapIt = m_AttrGroupMap.find(nGroup);
    KGLOG_PROCESS_ERROR(AttrMapIt != m_AttrGroupMap.end());
	
    AttrGroupIt     = AttrMapIt->second.begin();
    AttrGroupItEnd  = AttrMapIt->second.end();
	while (AttrGroupIt != AttrGroupItEnd)
	{
		nCurrentRate        += AttrGroupIt->nRate;
		
        if (nRand > nCurrentRate)
        {
            nSeclectAttribIndex = AttrGroupIt->nAttribIndex;
            goto Exit1;   
        }

		++AttrGroupIt;
	}

Exit1:
	nResult = nSeclectAttribIndex;
Exit0:
	return nResult;
}
