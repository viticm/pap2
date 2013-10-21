#include "StdAfx.h"
#include "KG3DAnimationTagID.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT KG3DAnimationTagID::GetID(UINT uFrameIndex, UINT uKeyIndex, UINT& uID)
{
    HRESULT hr = E_FAIL;
    std::map<UINT, std::vector<Frame2ID>>::iterator itFind = m_Key2ID.find(uFrameIndex);
    KG_PROCESS_ERROR(itFind != m_Key2ID.end());
    const std::vector<Frame2ID>& KeyFrames = itFind->second;
    for (size_t i = 0; i < KeyFrames.size(); i++)
    {
        if (KeyFrames[i].uIndex == uKeyIndex)
        {
            uID = KeyFrames[i].uID;
            hr = S_OK;
            break;
        }
    }
Exit0:
    return hr;
}

HRESULT KG3DAnimationTagID::GetFrame(UINT uID, UINT& uFrameIndex, UINT& uKeyIndex)
{
    HRESULT hr = E_FAIL;
    std::map<UINT, ID2KeyFrame>::iterator itFind = m_ID2Key.find(uID);
    KG_PROCESS_ERROR(itFind != m_ID2Key.end());
    uFrameIndex = itFind->second.uFrame;
    uKeyIndex = itFind->second.uIndex;
    hr = S_OK;
Exit0:
    return hr;
}


UINT KG3DAnimationTagID::InsertKey(UINT uFrameIndex, UINT uKeyIndex)
{
    UINT uID = GetNextID();
    Frame2ID F2ID = {uKeyIndex, uID};
    m_Key2ID[uFrameIndex].push_back(F2ID);
    ID2KeyFrame ID2Key = { uFrameIndex, uKeyIndex };
    m_ID2Key[uID] = ID2Key;
    return uID;
}

void KG3DAnimationTagID::RemoveKey(UINT uID)
{
    std::map<UINT, std::vector<Frame2ID>>::iterator itFind1;
    std::map<UINT, ID2KeyFrame>::iterator itFind = m_ID2Key.find(uID);
	std::map<UINT, ID2KeyFrame>::iterator itModify;
	UINT uIndexToDelete, uFrameToDelete;
	KG_PROCESS_ERROR(itFind != m_ID2Key.end());
    itFind1 = m_Key2ID.find(itFind->second.uFrame);
	KG_PROCESS_ERROR(itFind1 != m_Key2ID.end());

	uIndexToDelete = itFind->second.uIndex;
	uFrameToDelete = itFind->second.uFrame;
	std::vector<Frame2ID>& Keys = m_Key2ID[uFrameToDelete];
    for (size_t i = 0; i < Keys.size(); i++)
    {
        if (Keys[i].uIndex == uIndexToDelete)
        {
            std::vector<Frame2ID>::iterator itErase = Keys.begin();
            advance(itErase, i);
            Keys.erase(itErase);
            for (size_t j = 0; j < Keys.size(); j++)
            {
                if (Keys[j].uIndex > uIndexToDelete)
                {
                    Keys[j].uIndex--;
                }
            }
            break;
        }
    }

	m_ID2Key.erase(itFind);
	itModify = m_ID2Key.begin();
	while (itModify != m_ID2Key.end())
	{
		if (itModify->second.uFrame == uFrameToDelete &&
			itModify->second.uIndex > uIndexToDelete)
		{
			itModify->second.uIndex--;
		}
		itModify++;
	}
Exit0:
    ;
}

KG3DAnimationTagID::KG3DAnimationTagID()
{
    m_dwCurrentID = 0;	
}

KG3DAnimationTagID::~KG3DAnimationTagID()
{

}