////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : recoder.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-15 16:11:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "decoder.h"
#include "../tipcenter.h"
#include "../../driver/kareatest.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////


#define STR_DECODE_NULL_BEGIN			L"<null>"
#define STR_DECODE_NULL_END				L"</null>"
#define STR_DECODE_NAME					L"name"
#define STR_DECODE_POS_TYPE				L"postype"
#define STR_DECODE_X					L"x"
#define STR_DECODE_Y					L"y"
#define STR_DECODE_W					L"w"
#define STR_DECODE_H					L"h"
#define STR_DECODE_EVENT_ID				L"eventid"
#define STR_DECODE_DISABLE_SCALE		L"disablescale"
#define STR_DECODE_LOCK_SHOW_HIDE		L"lockshowhide"
#define STR_DECODE_USER_DATA			L"userdata"
#define STR_DECODE_SCRIPT				L"script"
#define STR_DECODE_INT_POS				L"intpos"
#define STR_DECODE_LINK                 L"link"

#define STR_DECODE_TEXT_BEGIN			L"<text>"
#define STR_DECODE_TEXT_END				L"</text>"
#define STR_DECODE_TEXT					L"text"
#define STR_DECODE_FONT					L"font"
#define STR_DECODE_MULTI_LINE			L"multiline"
#define STR_DECODE_H_ALIGN				L"halign"
#define STR_DECODE_V_ALIGN				L"valign"
#define STR_DECODE_CENTER_EACH_ROW		L"centereachrow"
#define STR_DECODE_ROW_SPACING			L"rowspacing"
#define STR_DECODE_FONT_SPACING			L"fontspacing"
#define STR_DECODE_RICH_TEXT			L"richtext"
#define STR_DECODE_AUTO_ETC				L"autoetc"
#define STR_DECODE_SHOW_ALL				L"showall"
#define STR_DECODE_AUTO_ADJ				L"autoadj"
#define STR_DECODE_R					L"r"
#define STR_DECODE_G					L"g"
#define STR_DECODE_B					L"b"

#define STR_DECODE_IMAGE_BEGIN			L"<image>"
#define STR_DECODE_IMAGE_END			L"</image>"
#define STR_DECODE_PATH					L"path"
#define STR_DECODE_FRAME				L"frame"
#define STR_DECODE_IMAGE_TYPE			L"imagetype"

#define STR_DECODE_SHADOW_BEGIN			L"<shadow>"
#define STR_DECODE_SHADOW_END			L"</shadow>"
#define STR_DECODE_COLOR				L"color"
#define STR_DECODE_ALPHA				L"alpha"

#define STR_DECODE_ANIMATE_BEGIN		L"<animate>"
#define STR_DECODE_ANIMATE_END			L"</animate>"
#define STR_DECODE_GROUP				L"group"
#define STR_DECODE_LOOP_COUNT			L"loopcount"

#define STR_DECODE_BOX_BEGIN			L"<box>"
#define STR_DECODE_BOX_END				L"</box>"
#define STR_DECODE_INDEX				L"index"

#define STR_DECODE_SCENE_BEGIN			L"<scene>"
#define STR_DECODE_SCENE_END			L"</scene>"
#define STR_DECODE_ENABLE_FRAMEMOVE		L"enableframemove"
#define STR_DECODE_RENDER_TERRAIN		L"renderterrain"
#define STR_DECODE_RENDER_SKY			L"rendersky"
#define STR_DECODE_ENABLE_ALPHA			L"enablealpha"

#define STR_DECODE_HANDLE_BEGIN			L"<handle>"
#define STR_DECODE_HANDLE_END			L"</handle>"
#define STR_DECODE_HANDLE_TYPE			L"handletype"
#define STR_DECODE_ROW_HEIGHT           L"rowheight"
#define STR_DECODE_MAX_ROW_HEIGHT       L"maxrowheight"  
#define STR_DECODE_FIRST_ITEM_POS_TYPE  L"firstpostype"
#define STR_DECODE_PIXEL_SCROLL			L"pixelscroll"
#define STR_DECODE_CONTROL_SHOW			L"ControlShow"


#define STR_DECODE_TREELEAF_BEGIN		L"<treeLeaf>"
#define STR_DECODE_TREELEAF_END			L"</treeLeaf>"
#define STR_DECODE_INDENT				L"indent"
#define STR_DECODE_SHOW_LINE			L"showline"
#define STR_DECODE_ALWAYS_NODE			L"alwaysnode"
#define STR_DECODE_ICON_IMAGE			L"iconimage"
#define STR_DECODE_EXPAND_FRAME			L"expandframe"
#define STR_DECODE_COLLAPSE_FRAME		L"collapseframe"
#define STR_DECODE_INDENT_WIDTH			L"indentwidth"
#define STR_DECODE_LINE_COLOR			L"linecolor"
#define STR_DECODE_ICON_WIDTH			L"iconwidth"
#define STR_DECODE_ICON_HEIGHT			L"iconheight"


namespace UI
{

	KUiComponentsDecoder *KUiComponentsDecoder::ms_pSelf = NULL;

    TBlock<WCHAR> KUiComponentsDecoder::ms_DecodedTextBuffer;
    TBlock<WCHAR> KUiComponentsDecoder::ms_EncodeStringBuffer;
    TBlock<WCHAR> KUiComponentsDecoder::ms_DecodedScriptBuffer;
    TBlock<CHAR>  KUiComponentsDecoder::ms_DecodedScriptSrcBuffer;

	KUiComponentsDecoder::KUiComponentsDecoder()
	{
	}

	KUiComponentsDecoder::~KUiComponentsDecoder()
	{
	}

	bool KUiComponentsDecoder::Init()
	{
        int nRetCode = false;
        bool bResult = false;

		ASSERT(!ms_pSelf);

		ms_pSelf = new(std::nothrow) KUiComponentsDecoder;
        KGLOG_PROCESS_ERROR(ms_pSelf);
		
        nRetCode = ms_DecodedTextBuffer.Alloc(MAX_DECODE_TEXT_LEN);
        KGLOG_PROCESS_ERROR(nRetCode);

        ms_DecodedTextBuffer[0] = L'\0';

		nRetCode = ms_EncodeStringBuffer.Alloc(MAX_DECODE_TEXT_LEN);
        KGLOG_PROCESS_ERROR(nRetCode);

        ms_EncodeStringBuffer[0] = L'\0';

        nRetCode = ms_DecodedScriptBuffer.Alloc(MAX_DECODE_TEXT_LEN);
        KGLOG_PROCESS_ERROR(nRetCode);

        ms_DecodedScriptBuffer[0] = L'\0';

        nRetCode = ms_DecodedScriptSrcBuffer.Alloc(MAX_DECODE_TEXT_LEN);
        KGLOG_PROCESS_ERROR(nRetCode);

        ms_DecodedScriptSrcBuffer[0] = '\0';

		bResult = true;
Exit0:
        if (!bResult)
        {
            ms_DecodedScriptSrcBuffer.Free();
            ms_DecodedScriptBuffer.Free();
            ms_EncodeStringBuffer.Free();
            ms_DecodedTextBuffer.Free();

            SAFE_DELETE(ms_pSelf);
        }
        return bResult;
	}

	void KUiComponentsDecoder::Exit()
	{
        ms_DecodedScriptSrcBuffer.Free();
        ms_DecodedScriptBuffer.Free();
        ms_EncodeStringBuffer.Free();
        ms_DecodedTextBuffer.Free();

		delete ms_pSelf;
	}

	KUiComponentsDecoder &KUiComponentsDecoder::GetSelf()
	{
		ASSERT(ms_pSelf);
		return *ms_pSelf;
	}

	template<typename OutDataType> 
	int KUiComponentsDecoder::DecodeItem(
		LPCWSTR wszBuffer, int nItemBeginKey, int nItemEndKey, 
		OutDataType &OutData, int &nDecodedlen
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nValueLen = false;
		LPCWSTR wszNextDecodePos = wszBuffer;

		KG_PROCESS_ERROR(wszBuffer);

		while (true) 
		{
			switch (*wszNextDecodePos)
			{
			case L'<': //是不是到了结尾
				nRetCode = IsItemEnd(nItemEndKey, wszNextDecodePos, nValueLen);
				if (nRetCode)
				{
					wszNextDecodePos += nValueLen;
					goto Exit1;
				}
				++wszNextDecodePos;
				break;
			case L'=':	// 解析值
				nRetCode = GetItemKey(nItemBeginKey, wszNextDecodePos, (int)(wszNextDecodePos - wszBuffer), nValueLen);
				if (nRetCode != DECODE_KEY_INVALID)
				{
					++wszNextDecodePos; //跳过等于号
					ProcessKey(nRetCode, wszNextDecodePos, OutData, nValueLen);
					wszNextDecodePos += nValueLen;
				}
				else
					++wszNextDecodePos;
				break;
			case L'\0':
				goto Exit1;
				break;
			default:
				++wszNextDecodePos;
				break;
			}
		}

Exit1:
		nDecodedlen = (int)(wszNextDecodePos - wszBuffer);
		nResult = true;
Exit0:
		return nResult;
	}

	template<typename ItemDataType, typename ItemType>
	ItemType* KUiComponentsDecoder::Append(
		KItemHandle *pOwner, LPCWSTR wszBuffer, 
		int nBeginKey, int nEndKey, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;

		ItemDataType ItemData(pEventOwner);
		ItemType *pItem = NULL;

		KG_PROCESS_ERROR(pOwner);

		nRetCode = DecodeItem<ItemDataType>(wszBuffer, nBeginKey, nEndKey, ItemData, nDecodedLen);
		KG_PROCESS_ERROR(nRetCode);

		ItemData.fScale = fScale;
		pItem = ItemType::Create(ItemData);
		KG_PROCESS_ERROR(pItem);

		if(nBeginKey == DECODE_KEY_HANDLE_BEGIN || nBeginKey == DECODE_KEY_TREELEAF_BEGIN)
		{
			int nSubItemLen = 0;
			KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
			if (pHandle)
            {
				AppendItem(pHandle, wszBuffer + nDecodedLen, nSubItemLen, pEventOwner, fScale);
                pHandle->FormatAllItemPos();
            }
			nDecodedLen += nSubItemLen;
		}

		nRetCode = pOwner->AppendItem(pItem);
		KG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		if (!nResult)
			SAFE_RELEASE(pItem);
		return pItem;
	}

	template<typename ItemDataType, typename ItemType>
	ItemType* KUiComponentsDecoder::Insert(
		KItemHandle *pOwner, LPCWSTR wszBuffer, KItemNull **ppReferenceItem, int &nBeforeOrBehind,
		int nBeginKey, int nEndKey, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;

		ItemDataType ItemData(pEventOwner);
		ItemType *pItem = NULL;

		ASSERT(pOwner);
		ASSERT(ppReferenceItem);
		ASSERT(*ppReferenceItem);

		nRetCode = DecodeItem<ItemDataType>(wszBuffer, nBeginKey, nEndKey, ItemData, nDecodedLen);
		KG_PROCESS_ERROR(nRetCode);

		ItemData.fScale = fScale;
		pItem = ItemType::Create(ItemData);
		KG_PROCESS_ERROR(pItem);

		if(nBeginKey == DECODE_KEY_HANDLE_BEGIN || nBeginKey == DECODE_KEY_TREELEAF_BEGIN)
		{
			int nSubItemLen = 0;
			KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
			if (pHandle)
            {
				AppendItem(pHandle, wszBuffer + nDecodedLen, nSubItemLen, pEventOwner, fScale);
                pHandle->FormatAllItemPos();
            }
			nDecodedLen += nSubItemLen;
		}

		nRetCode = pOwner->InsertItem(pItem, *ppReferenceItem, nBeforeOrBehind);
		KG_PROCESS_ERROR(nRetCode);

		*ppReferenceItem = pItem;
		nBeforeOrBehind = INSERT_BEHIND;

		nResult = true;
Exit0:
		if (!nResult)
			SAFE_RELEASE(pItem);
		return pItem;
	}

	template<typename ItemDataType, typename ItemType>
	ItemType* KUiComponentsDecoder::Append(
		KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, 
		LPCSTR pcszNewName, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;
		KItemHandle *pHandle = NULL; 
		ItemDataType ItemData(pEventOwner);
		ItemType *pItem = NULL;

		KG_PROCESS_ERROR(pOwner);
		KG_PROCESS_ERROR(pIni);

		nRetCode = DecodeItem(pIni, pcszSectionName, ItemData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		ItemData.fScale = fScale;
		pItem = ItemType::Create(ItemData);
		KG_PROCESS_ERROR(pItem);

		pHandle = g_ItemCast<KItemHandle>(pItem);
		if (pHandle)
		{
			char szItemName[ITEM_NAME_MAX_SIZE];
			char szNext[ITEM_NAME_MAX_SIZE];

			szNext[0] = '\0';
			szItemName[0] = '\0';

			while (pIni->GetNextSection(szItemName, szNext))
			{
				strncpy(szItemName, szNext, _countof(szItemName));
				pIni->GetString(szItemName, "._Parent", "", szNext, _countof(szNext));
				if (_tcsicmp(szNext, pcszSectionName) == 0)
					AppendItem(pHandle, pIni, szItemName, pEventOwner, fScale);
			}
			pHandle->FormatAllItemPos();
		}

		nRetCode = pOwner->AppendItem(pItem);
		KG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		if (!nResult)
			SAFE_RELEASE(pItem);
		return pItem;
	}

	template<typename ItemDataType, typename ItemType>
	ItemType* KUiComponentsDecoder::Insert(
		KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, 
		KItemNull *pReferenceItem, int nBeforeOrBehind,
		LPCSTR pcszNewName, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;
		KItemHandle *pHandle = NULL; 
		ItemDataType ItemData(pEventOwner);
		ItemType *pItem = NULL;

		ASSERT(pOwner);
		ASSERT(pIni);
		ASSERT(pReferenceItem);

		nRetCode = DecodeItem(pIni, pcszSectionName, ItemData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		ItemData.fScale = fScale;
		pItem = ItemType::Create(ItemData);
		KG_PROCESS_ERROR(pItem);

		pHandle = g_ItemCast<KItemHandle>(pItem);
		if (pHandle)
		{
			char szItemName[ITEM_NAME_MAX_SIZE];
			char szNext[ITEM_NAME_MAX_SIZE];

			szNext[0] = '\0';
			szItemName[0] = '\0';

			while (pIni->GetNextSection(szItemName, szNext))
			{
				strncpy(szItemName, szNext, _countof(szItemName));
				pIni->GetString(szItemName, "._Parent", "", szNext, _countof(szNext));
				if (_tcsicmp(szNext, pcszSectionName) == 0)
					AppendItem(pHandle, pIni, szItemName, pEventOwner, fScale);
			}
			pHandle->FormatAllItemPos();
		}

		nRetCode = pOwner->InsertItem(pItem, pReferenceItem, nBeforeOrBehind);
		KG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		if (!nResult)
			SAFE_RELEASE(pItem);
		return pItem;
	}

	int KUiComponentsDecoder::GetItemBeginKey(LPCWSTR wszBuffer, int &nDecodedLen)
	{
		ASSERT(wszBuffer);

		static KCtrlTable const s_CtrlTable[] =
		{
			//注意顺序，长的string在前面
			{ STR_DECODE_TREELEAF_BEGIN, (int)wcslen(STR_DECODE_TREELEAF_BEGIN), DECODE_KEY_TREELEAF_BEGIN },
			{ STR_DECODE_ANIMATE_BEGIN, (int)wcslen(STR_DECODE_ANIMATE_BEGIN), DECODE_KEY_ANIMATE_BEGIN },
			{ STR_DECODE_SHADOW_BEGIN, (int)wcslen(STR_DECODE_SHADOW_BEGIN), DECODE_KEY_SHADOW_BEGIN },
			{ STR_DECODE_HANDLE_BEGIN, (int)wcslen(STR_DECODE_HANDLE_BEGIN), DECODE_KEY_HANDLE_BEGIN },
			{ STR_DECODE_IMAGE_BEGIN, (int)wcslen(STR_DECODE_IMAGE_BEGIN), DECODE_KEY_IMAGE_BEGIN },
			{ STR_DECODE_SCENE_BEGIN, (int)wcslen(STR_DECODE_SCENE_BEGIN), DECODE_KEY_SCENE_BEGIN },
			{ STR_DECODE_NULL_BEGIN, (int)wcslen(STR_DECODE_NULL_BEGIN), DECODE_KEY_NULL_BEGIN },
			{ STR_DECODE_TEXT_BEGIN, (int)wcslen(STR_DECODE_TEXT_BEGIN), DECODE_KEY_TEXT_BEGIN },
			{ STR_DECODE_BOX_BEGIN, (int)wcslen(STR_DECODE_BOX_BEGIN), DECODE_KEY_BOX_BEGIN },
		};

		for (int nIndex = 0; nIndex < _countof(s_CtrlTable); ++nIndex)
		{
			nDecodedLen = s_CtrlTable[nIndex].nCtrlLen;
			if (wcsnicmp(wszBuffer, s_CtrlTable[nIndex].szCtrl, nDecodedLen) == 0)
            {
				return s_CtrlTable[nIndex].nCtrl;
            }
		}

		nDecodedLen = 0;
		return DECODE_KEY_INVALID;
	}

	int KUiComponentsDecoder::IsItemEnd(int nItemEndKey, LPCWSTR wszBuffer, int &nDecodedLen)
	{
		ASSERT(wszBuffer);

		static KCtrlTable const s_CtrlTable[] = 
		{
			{ STR_DECODE_NULL_END, (int)wcslen(STR_DECODE_NULL_END), DECODE_KEY_NULL_END },
			{ STR_DECODE_TEXT_END, (int)wcslen(STR_DECODE_TEXT_END), DECODE_KEY_TEXT_END },
			{ STR_DECODE_IMAGE_END, (int)wcslen(STR_DECODE_IMAGE_END), DECODE_KEY_IMAGE_END },
			{ STR_DECODE_SHADOW_END, (int)wcslen(STR_DECODE_SHADOW_END), DECODE_KEY_SHADOW_END },
			{ STR_DECODE_ANIMATE_END, (int)wcslen(STR_DECODE_ANIMATE_END), DECODE_KEY_ANIMATE_END },
			{ STR_DECODE_SCENE_END, (int)wcslen(STR_DECODE_SCENE_END), DECODE_KEY_SCENE_END },
			{ STR_DECODE_BOX_END, (int)wcslen(STR_DECODE_BOX_END), DECODE_KEY_BOX_END },
			{ STR_DECODE_HANDLE_END, (int)wcslen(STR_DECODE_HANDLE_END), DECODE_KEY_HANDLE_END },
			{ STR_DECODE_TREELEAF_END, (int)wcslen(STR_DECODE_TREELEAF_END), DECODE_KEY_TREELEAF_END },
		};

		int nClearDecodedLen = false;

		if (nItemEndKey == DECODE_KEY_HANDLE_END || nItemEndKey == DECODE_KEY_TREELEAF_END)
		{
			if (GetItemBeginKey(wszBuffer, nDecodedLen) != DECODE_KEY_INVALID)
			{
				nDecodedLen = 0;
				return true;
			}
			nClearDecodedLen = true;
		}

		for (int nIndex = 0; nIndex < _countof(s_CtrlTable); ++nIndex)
		{
			nDecodedLen = s_CtrlTable[nIndex].nCtrlLen;
			if (nItemEndKey == s_CtrlTable[nIndex].nCtrl &&
				wcsnicmp(wszBuffer, s_CtrlTable[nIndex].szCtrl, nDecodedLen) == 0)
			{
				if (nClearDecodedLen)
					nDecodedLen = 0;
				return true;
			}
		}

		nDecodedLen = 0;
		return false;
	}

	int KUiComponentsDecoder::GetItemKey(int nItemBeginKey, LPCWSTR wszBuffer, int nMaxKeyLen, int &nDecodedLen)
	{
        ASSERT(wszBuffer);

        static KCtrlTable const s_NullCtrlTable[] = 
        {
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_TextCtrlTable[] = 
        {
            { STR_DECODE_CENTER_EACH_ROW, (int)wcslen(STR_DECODE_CENTER_EACH_ROW), DECODE_KEY_CENTER_EACH_ROW },
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_FONT_SPACING, (int)wcslen(STR_DECODE_FONT_SPACING), DECODE_KEY_FONT_SPACING },
            { STR_DECODE_ROW_SPACING, (int)wcslen(STR_DECODE_ROW_SPACING), DECODE_KEY_ROW_SPACING },
            { STR_DECODE_MULTI_LINE, (int)wcslen(STR_DECODE_MULTI_LINE), DECODE_KEY_MULTI_LINE },
            { STR_DECODE_RICH_TEXT, (int)wcslen(STR_DECODE_RICH_TEXT), DECODE_KEY_RICH_TEXT },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_AUTO_ETC, (int)wcslen(STR_DECODE_AUTO_ETC), DECODE_KEY_AUTO_ETC },
            { STR_DECODE_SHOW_ALL, (int)wcslen(STR_DECODE_SHOW_ALL), DECODE_KEY_SHOW_ALL },
            { STR_DECODE_AUTO_ADJ, (int)wcslen(STR_DECODE_AUTO_ADJ), DECODE_KEY_AUTO_ADJ },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
            { STR_DECODE_H_ALIGN, (int)wcslen(STR_DECODE_H_ALIGN), DECODE_KEY_H_ALIGN },
            { STR_DECODE_V_ALIGN, (int)wcslen(STR_DECODE_V_ALIGN), DECODE_KEY_V_ALIGN },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_TEXT, (int)wcslen(STR_DECODE_TEXT), DECODE_KEY_TEXT },
            { STR_DECODE_FONT, (int)wcslen(STR_DECODE_FONT), DECODE_KEY_FONT },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
            { STR_DECODE_R, (int)wcslen(STR_DECODE_R), DECODE_KEY_R },
            { STR_DECODE_G, (int)wcslen(STR_DECODE_G), DECODE_KEY_G },
            { STR_DECODE_B, (int)wcslen(STR_DECODE_B), DECODE_KEY_B },
        };	

        static KCtrlTable const s_ImageCtrlTable[] = 
        {
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_IMAGE_TYPE, (int)wcslen(STR_DECODE_IMAGE_TYPE), DECODE_KEY_IMAGE_TYPE },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_FRAME, (int)wcslen(STR_DECODE_FRAME), DECODE_KEY_FRAME },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_PATH, (int)wcslen(STR_DECODE_PATH), DECODE_KEY_PATH },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_ShadowCtrlTable[] = 
        {
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_COLOR, (int)wcslen(STR_DECODE_COLOR), DECODE_KEY_COLOR },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_AnimateCtrlTable[] = 
        {
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_LOOP_COUNT, (int)wcslen(STR_DECODE_LOOP_COUNT), DECODE_KEY_LOOP_COUNT },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_GROUP, (int)wcslen(STR_DECODE_GROUP), DECODE_KEY_GROUP },
            { STR_DECODE_PATH, (int)wcslen(STR_DECODE_PATH), DECODE_KEY_PATH },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_SceneCtrlTable[] = 
        {
            { STR_DECODE_ENABLE_FRAMEMOVE, (int)wcslen(STR_DECODE_ENABLE_FRAMEMOVE), DECODE_KEY_ENABLE_FRAMEMOVE },
			{ STR_DECODE_RENDER_TERRAIN, (int)wcslen(STR_DECODE_RENDER_TERRAIN), DECODE_KEY_RENDER_TERRAIN },
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_ENABLE_ALPHA, (int)wcslen(STR_DECODE_ENABLE_ALPHA), DECODE_KEY_ENABLE_ALPHA },
            { STR_DECODE_RENDER_SKY, (int)wcslen(STR_DECODE_RENDER_SKY), DECODE_KEY_RENDER_SKY },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_BoxCtrlTable[] = 
        {
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_INDEX, (int)wcslen(STR_DECODE_INDEX), DECODE_KEY_INDEX },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_HandleCtrlTable[] = 
        {
            { STR_DECODE_FIRST_ITEM_POS_TYPE, (int)wcslen(STR_DECODE_FIRST_ITEM_POS_TYPE), DECODE_KEY_FIRST_ITEM_POS_TYPE },
            { STR_DECODE_MAX_ROW_HEIGHT, (int)wcslen(STR_DECODE_MAX_ROW_HEIGHT), DECODE_KEY_MAX_ROW_HEIGHT },
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_PIXEL_SCROLL, (int)wcslen(STR_DECODE_PIXEL_SCROLL), DECODE_KEY_PIXEL_SCROLL },
			{ STR_DECODE_CONTROL_SHOW, (int)wcslen(STR_DECODE_CONTROL_SHOW), DECODE_KEY_CONTROL_SHOW },
            { STR_DECODE_ROW_SPACING, (int)wcslen(STR_DECODE_ROW_SPACING), DECODE_KEY_ROW_SPACING },
            { STR_DECODE_HANDLE_TYPE, (int)wcslen(STR_DECODE_HANDLE_TYPE), DECODE_KEY_HANDLE_TYPE },
            { STR_DECODE_ROW_HEIGHT, (int)wcslen(STR_DECODE_ROW_HEIGHT), DECODE_KEY_ROW_HEIGHT },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };	

        static KCtrlTable const s_TreeLeafCtrlTable[] = 
        {
            { STR_DECODE_FIRST_ITEM_POS_TYPE, (int)wcslen(STR_DECODE_FIRST_ITEM_POS_TYPE), DECODE_KEY_FIRST_ITEM_POS_TYPE },
            { STR_DECODE_COLLAPSE_FRAME, (int)wcslen(STR_DECODE_COLLAPSE_FRAME), DECODE_KEY_COLLAPSE_FRAME },
            { STR_DECODE_MAX_ROW_HEIGHT, (int)wcslen(STR_DECODE_MAX_ROW_HEIGHT), DECODE_KEY_MAX_ROW_HEIGHT },
            { STR_DECODE_LOCK_SHOW_HIDE, (int)wcslen(STR_DECODE_LOCK_SHOW_HIDE), DECODE_KEY_LOCK_SHOW_HIDE },
            { STR_DECODE_DISABLE_SCALE, (int)wcslen(STR_DECODE_DISABLE_SCALE), DECODE_KEY_DISABLE_SCALE },
            { STR_DECODE_EXPAND_FRAME, (int)wcslen(STR_DECODE_EXPAND_FRAME), DECODE_KEY_EXPAND_FRAME },
            { STR_DECODE_INDENT_WIDTH, (int)wcslen(STR_DECODE_INDENT_WIDTH), DECODE_KEY_INDENT_WIDTH },
            { STR_DECODE_PIXEL_SCROLL, (int)wcslen(STR_DECODE_PIXEL_SCROLL), DECODE_KEY_PIXEL_SCROLL },
			{ STR_DECODE_CONTROL_SHOW, (int)wcslen(STR_DECODE_CONTROL_SHOW), DECODE_KEY_CONTROL_SHOW },
            { STR_DECODE_ICON_HEIGHT, (int)wcslen(STR_DECODE_ICON_HEIGHT), DECODE_KEY_ICON_HEIGHT },
            { STR_DECODE_ROW_SPACING, (int)wcslen(STR_DECODE_ROW_SPACING), DECODE_KEY_ROW_SPACING },
            { STR_DECODE_HANDLE_TYPE, (int)wcslen(STR_DECODE_HANDLE_TYPE), DECODE_KEY_HANDLE_TYPE },
            { STR_DECODE_ALWAYS_NODE, (int)wcslen(STR_DECODE_ALWAYS_NODE), DECODE_KEY_ALWAYS_NODE },
            { STR_DECODE_ICON_IMAGE, (int)wcslen(STR_DECODE_ICON_IMAGE), DECODE_KEY_ICON_IMAGE },
            { STR_DECODE_LINE_COLOR, (int)wcslen(STR_DECODE_LINE_COLOR), DECODE_KEY_LINE_COLOR },
            { STR_DECODE_ICON_WIDTH, (int)wcslen(STR_DECODE_ICON_WIDTH), DECODE_KEY_ICON_WIDTH },
            { STR_DECODE_ROW_HEIGHT, (int)wcslen(STR_DECODE_ROW_HEIGHT), DECODE_KEY_ROW_HEIGHT },
            { STR_DECODE_SHOW_LINE, (int)wcslen(STR_DECODE_SHOW_LINE), DECODE_KEY_SHOW_LINE },
            { STR_DECODE_USER_DATA, (int)wcslen(STR_DECODE_USER_DATA), DECODE_KEY_USER_DATA },
            { STR_DECODE_EVENT_ID, (int)wcslen(STR_DECODE_EVENT_ID), DECODE_KEY_EVENT_ID },
            { STR_DECODE_POS_TYPE, (int)wcslen(STR_DECODE_POS_TYPE), DECODE_KEY_POS_TYPE },
			{ STR_DECODE_INT_POS, (int)wcslen(STR_DECODE_INT_POS), DECODE_KEY_INT_POS },
            { STR_DECODE_SCRIPT, (int)wcslen(STR_DECODE_SCRIPT), DECODE_KEY_SCRIPT },
            { STR_DECODE_INDENT, (int)wcslen(STR_DECODE_INDENT), DECODE_KEY_INDENT },
            { STR_DECODE_ALPHA, (int)wcslen(STR_DECODE_ALPHA), DECODE_KEY_ALPHA },
            { STR_DECODE_NAME, (int)wcslen(STR_DECODE_NAME), DECODE_KEY_NAME },
            { STR_DECODE_LINK, (int)wcslen(STR_DECODE_LINK), DECODE_KEY_LINK},
            { STR_DECODE_X, (int)wcslen(STR_DECODE_X), DECODE_KEY_X },
            { STR_DECODE_Y, (int)wcslen(STR_DECODE_Y), DECODE_KEY_Y },
            { STR_DECODE_W, (int)wcslen(STR_DECODE_W), DECODE_KEY_W },
            { STR_DECODE_H, (int)wcslen(STR_DECODE_H), DECODE_KEY_H },
        };

		static struct KCtrlTableList 
		{
			KCtrlTable const *pCtrlTable;
			int const nTableLen;
			int const nItemBeginKey;
		}

		const s_CtrlTableList[] = 
		{
			{ s_NullCtrlTable, _countof(s_NullCtrlTable), DECODE_KEY_NULL_BEGIN },
			{ s_TextCtrlTable, _countof(s_TextCtrlTable), DECODE_KEY_TEXT_BEGIN },
			{ s_ImageCtrlTable, _countof(s_ImageCtrlTable), DECODE_KEY_IMAGE_BEGIN },
			{ s_ShadowCtrlTable, _countof(s_ShadowCtrlTable), DECODE_KEY_SHADOW_BEGIN },
			{ s_AnimateCtrlTable, _countof(s_AnimateCtrlTable), DECODE_KEY_ANIMATE_BEGIN },
			{ s_SceneCtrlTable, _countof(s_SceneCtrlTable), DECODE_KEY_SCENE_BEGIN },
			{ s_BoxCtrlTable, _countof(s_BoxCtrlTable), DECODE_KEY_BOX_BEGIN },
			{ s_HandleCtrlTable, _countof(s_HandleCtrlTable), DECODE_KEY_HANDLE_BEGIN },
			{ s_TreeLeafCtrlTable, _countof(s_TreeLeafCtrlTable), DECODE_KEY_TREELEAF_BEGIN },
		};

		for (int i = 0; i < _countof(s_CtrlTableList); ++i)
		{
			if (s_CtrlTableList[i].nItemBeginKey == nItemBeginKey)
			{
				for (int j = 0; j < s_CtrlTableList[i].nTableLen; ++j)
				{
					nDecodedLen = min(s_CtrlTableList[i].pCtrlTable[j].nCtrlLen, nMaxKeyLen);
					if (wcsnicmp(wszBuffer - nDecodedLen, s_CtrlTableList[i].pCtrlTable[j].szCtrl, nDecodedLen) == 0)
                    {
						return s_CtrlTableList[i].pCtrlTable[j].nCtrl;
                    }
				}
			}
		}

		nDecodedLen = 0;
		return DECODE_KEY_INVALID;
	}

	int KUiComponentsDecoder::IsAppendHandleEnd(LPCWSTR wszBuffer, int &nDecodedlen)
	{
		ASSERT(wszBuffer);

		nDecodedlen = (int)wcslen(STR_DECODE_HANDLE_END);
		if (wcsnicmp(wszBuffer, STR_DECODE_HANDLE_END, nDecodedlen) == 0)
			return true;

		nDecodedlen = (int)wcslen(STR_DECODE_TREELEAF_END);
		if (wcsnicmp(wszBuffer, STR_DECODE_TREELEAF_END, nDecodedlen) == 0)
			return true;

		nDecodedlen = 0;
		return false;
	}

	int KUiComponentsDecoder::DecodeString(LPCWSTR wszBuffer, LPWSTR wszDest, int nDestCount, int &nDecodedLen)
	{
		int nResult = false;
		int nSpecial = false;
		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);
		KG_PROCESS_ERROR(wszBuffer[0] == L'\"');
		KG_PROCESS_ERROR(wszDest);
		KG_PROCESS_ERROR(nDestCount > 0);

		wszDest[0] = L'\0';

		LPCWSTR wszStartPos = wszBuffer;

		--nDestCount;
		++wszBuffer;	//第一个字符是引号跳过

		while (nDestCount)
		{
			switch (*wszBuffer)
			{
			case L'\"':
				++wszBuffer;
			case L'\0':
				goto Exit1;
				break;
			case L'\\':
				switch (*(wszBuffer + 1))
				{
				case L'\n':
				case L'\t':
				case L'\\':
				case L'\"':
					*wszDest = *(wszBuffer + 1);
					++wszDest;
					++wszBuffer;
					++wszBuffer;
					--nDestCount;
					break;
				case L'\0':
					*wszDest = *wszBuffer;
					++wszDest;
					++wszBuffer;
					goto Exit1;
					break;
				default:
					*wszDest = *wszBuffer;
					++wszDest;
					++wszBuffer;
					--nDestCount;
					break;
				}
				break;
			default:
				*wszDest = *wszBuffer;
				++wszDest;
				++wszBuffer;
				--nDestCount;
				break;
			}
		}

Exit1:
		nDecodedLen = (int)(wszBuffer - wszStartPos);
		*wszDest = L'\0';
		nResult = true;
Exit0:
		return nResult;
	}


	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemNullData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		switch (nItemKey)
		{
		case DECODE_KEY_NAME:
			DecodeString(wszBuffer, OutData.wszName, _countof(OutData.wszName), nDecodedLen);
			break;
		case DECODE_KEY_POS_TYPE:
			OutData.nPosType = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_X:
			OutData.nPosType = POSITION_BY_CUSTOM;
			OutData.fRelX = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_Y:
			OutData.nPosType = POSITION_BY_CUSTOM;
			OutData.fRelY = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_W:
			OutData.nAutoSize = false;
			OutData.fWidth = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_H:
			OutData.nAutoSize = false;
			OutData.fHeight= (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_EVENT_ID:
			OutData.dwEventID = OutData.dwEventID | (DWORD)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_DISABLE_SCALE:
			OutData.nDisableScale = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_LOCK_SHOW_HIDE:
			OutData.nLockShowAndHide = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ALPHA:
			OutData.nAlpha = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_USER_DATA:
			OutData.fUserData = _wtof(wszBuffer);
			break;
		case DECODE_KEY_SCRIPT:
			OutData.nScript = true;
			DecodeString(wszBuffer, &ms_DecodedScriptBuffer[0], ms_DecodedScriptBuffer.GetSize(), nDecodedLen);
			break;
		case DECODE_KEY_INT_POS:
			OutData.nIntPos = (int)_wtoi(wszBuffer);
			break;
        case DECODE_KEY_LINK:
            if (ms_DecodedTextBuffer[0] != L'\0')
            {
                WCHAR wszLinkText[MAX_DECODE_TEXT_LEN];

                nRetCode = _snwprintf(wszLinkText, sizeof(wszLinkText) / sizeof(WCHAR), L"%s", ms_DecodedTextBuffer.GetAt(0));
                KGLOG_PROCESS_ERROR(nRetCode != EOF);

                nRetCode = _snwprintf(ms_DecodedTextBuffer.GetAt(0), ms_DecodedTextBuffer.GetSize(), L"[%s]", wszLinkText);
                KGLOG_PROCESS_ERROR(nRetCode != EOF);
            }

            OutData.nLink = true;
            OutData.dwEventID = OutData.dwEventID | 0x00000110;
            DecodeString(wszBuffer, OutData.wszLinkInfo, _countof(OutData.wszLinkInfo), nDecodedLen);
            break;
		default:
			goto Exit0;
		}

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemTextData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_TEXT:
			OutData.nText = true;
            DecodeString(wszBuffer, &ms_DecodedTextBuffer[0], ms_DecodedTextBuffer.GetSize(), nDecodedLen);
			break;
		case DECODE_KEY_FONT:
			OutData.nFontScheme = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_MULTI_LINE:
			OutData.nMultiLine = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_H_ALIGN:
			OutData.nHAlign = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_V_ALIGN:
			OutData.nVAlign = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ROW_SPACING:
			OutData.fRowSpacing = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_FONT_SPACING:
			OutData.fFontSpacing = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_CENTER_EACH_ROW:
			OutData.nCenterEachLine = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_RICH_TEXT:
			OutData.nRichText = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_AUTO_ETC:
			OutData.nAutoEtc = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_SHOW_ALL:
			OutData.nShowAll = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_AUTO_ADJ:
			OutData.nAutoAdj = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_R:
			OutData.dwColor &= 0xFF00FFFF;
			OutData.dwColor |= ((((int)_wtoi(wszBuffer) & 0x0000FF) << 16 ) | 0xFF000000);
			break;
		case DECODE_KEY_G:
			OutData.dwColor &= 0xFFFF00FF;
			OutData.dwColor |= ((((int)_wtoi(wszBuffer) & 0x0000FF) << 8 ) | 0xFF000000);
			break;
		case DECODE_KEY_B:
			OutData.dwColor &= 0xFFFFFF00;
			OutData.dwColor |= (((int)_wtoi(wszBuffer) & 0x0000FF) | 0xFF000000);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemImageData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_PATH:
            {
                WCHAR wszImagePath[MAX_PATH];
                DecodeString(wszBuffer, wszImagePath, _countof(wszImagePath), nDecodedLen);
                ATL::CW2A szImagePath(wszImagePath, GetCodePage());
                strncpy(OutData.szImagePath, szImagePath, _countof(OutData.szImagePath));
                FormatFilePath(OutData.szImagePath);
            }
			break;
		case DECODE_KEY_FRAME:
			OutData.nFrame = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_IMAGE_TYPE:
			OutData.nPartImageType = (int)_wtoi(wszBuffer);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemShadowData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_COLOR:
			{
				WCHAR wszColor[32];
				wszColor[0] = L'\0';
                KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;

                DecodeString(wszBuffer, wszColor, _countof(wszColor), nDecodedLen);

                int nColorScheme = ColorSchemeMgr.FindScheme(wszColor);
                if (ColorSchemeMgr.IsExistColor(nColorScheme))
                    OutData.dwShadowColor = ColorSchemeMgr.GetColor(nColorScheme);
			}
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemAnimateData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_PATH:
            {
                WCHAR wszImagePath[MAX_PATH];
                DecodeString(wszBuffer, wszImagePath, _countof(wszImagePath), nDecodedLen);
                ATL::CW2A szImagePath(wszImagePath, GetCodePage());
                strncpy(OutData.szImagePath, szImagePath, _countof(OutData.szImagePath));
                FormatFilePath(OutData.szImagePath);
            }
			break;
		case DECODE_KEY_GROUP:
			OutData.nGroup = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_LOOP_COUNT:
			OutData.nLoopCount = (int)_wtoi(wszBuffer);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemSceneData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_ENABLE_FRAMEMOVE:
			OutData.nEnableFrameMove = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_RENDER_TERRAIN:
			OutData.nEnableRenderTerrain = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_RENDER_SKY:
			OutData.nEnableRenderSkyBox = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ENABLE_ALPHA:
			OutData.nEnableAlpha = (int)_wtoi(wszBuffer);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemBoxData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_INDEX:
			OutData.nIndex = (int)_wtoi(wszBuffer);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemHandleData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemNullData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_HANDLE_TYPE:
			OutData.nHandleType = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ROW_HEIGHT:
			OutData.fRowHeight = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_MAX_ROW_HEIGHT:
			OutData.fMaxRowHeight = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_ROW_SPACING:
			OutData.fRowSpacing = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_FIRST_ITEM_POS_TYPE:
			OutData.nFirstItemPosType = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_PIXEL_SCROLL:
			OutData.nPixelScroll = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_CONTROL_SHOW:
			OutData.nControlShow = (int)_wtoi(wszBuffer);
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemTreeLeafData &OutData, int &nDecodedLen)
	{
		int nResult = false;
		int nRetCode = false;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(wszBuffer);

		nRetCode = ProcessKey(nItemKey, wszBuffer, (KItemHandleData &)OutData, nDecodedLen);
		KG_PROCESS_SUCCESS(nRetCode);

		switch (nItemKey)
		{
		case DECODE_KEY_INDENT:
			OutData.nIndent = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_SHOW_LINE:
			OutData.nShowLine = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ALWAYS_NODE:
			OutData.nAlwaysNode = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_ICON_IMAGE:
            {
                WCHAR wszImageName[MAX_PATH];
                DecodeString(wszBuffer, wszImageName, _countof(wszImageName), nDecodedLen);
                ATL::CW2A szImageName(wszImageName, GetCodePage());
                strncpy(OutData.szImageName, szImageName, _countof(OutData.szImageName));
                FormatFilePath(OutData.szImageName);
            }
			break;
		case DECODE_KEY_EXPAND_FRAME:
			OutData.nExpandIconFrame = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_COLLAPSE_FRAME:
			OutData.nCollapseIconFrame = (int)_wtoi(wszBuffer);
			break;
		case DECODE_KEY_INDENT_WIDTH:
			OutData.fEachIndentWidth = (float)_wtof(wszBuffer);
			break;
		case DECODE_KEY_LINE_COLOR:
			{
				WCHAR wszColor[32];
                wszColor[0] = L'\0';

                KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;

                DecodeString(wszBuffer, wszColor, _countof(wszColor), nDecodedLen);

                int nColorScheme = ColorSchemeMgr.FindScheme(wszColor);
                if (ColorSchemeMgr.IsExistColor(nColorScheme))
                    OutData.dwLineColor = ColorSchemeMgr.GetColor(nColorScheme);
            }
			break;
		case DECODE_KEY_ICON_WIDTH:
			OutData.fIconWidth = (float)_wtof(wszBuffer);
			OutData.nAutoIconSize = false;
			break;
		case DECODE_KEY_ICON_HEIGHT:
			OutData.fIconHeight = (float)_wtof(wszBuffer);
			OutData.nAutoIconSize = false;
			break;
		default:
			goto Exit0;
		}

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::AppendItem(KItemHandle *pOwner, LPCWSTR wszBuffer, int &nDecodedLen, KWndWindow *pEventOwner, float fScale)
	{
		int nResult = false;
		int nRetCode = false;
		nDecodedLen = 0;
		LPCWSTR wszNextDecodePos = wszBuffer;

		KG_PROCESS_ERROR(pOwner);
		KG_PROCESS_ERROR(wszBuffer);

		while (true)
		{
			switch (*wszNextDecodePos)
			{
			case L'\0':
				goto Exit1;
				break;
			case L'<':
				{
					if (IsAppendHandleEnd(wszNextDecodePos, nDecodedLen))
					{
						wszNextDecodePos += nDecodedLen;
						goto Exit1;
					}

					nRetCode = GetItemBeginKey(wszNextDecodePos, nDecodedLen);
					if (nRetCode == DECODE_KEY_INVALID)
					{
						++wszNextDecodePos;
						break;
					}
					wszNextDecodePos += nDecodedLen;
					nDecodedLen = 0;
					switch (nRetCode)
					{
					case DECODE_KEY_NULL_BEGIN:
						Append<KItemNullData, KItemNull>(
							pOwner, wszNextDecodePos, DECODE_KEY_NULL_BEGIN, 
							DECODE_KEY_NULL_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_TEXT_BEGIN:
						Append<KItemTextData, KItemText>(
							pOwner, wszNextDecodePos, DECODE_KEY_TEXT_BEGIN, 
							DECODE_KEY_TEXT_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_IMAGE_BEGIN:
						Append<KItemImageData, KItemImage>(
							pOwner, wszNextDecodePos, DECODE_KEY_IMAGE_BEGIN, 
							DECODE_KEY_IMAGE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_SHADOW_BEGIN:
						Append<KItemShadowData, KItemShadow>(
							pOwner, wszNextDecodePos, DECODE_KEY_SHADOW_BEGIN, 
							DECODE_KEY_SHADOW_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_ANIMATE_BEGIN:
						Append<KItemAnimateData, KItemAnimate>(
							pOwner, wszNextDecodePos, DECODE_KEY_ANIMATE_BEGIN, 
							DECODE_KEY_ANIMATE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_SCENE_BEGIN:
						Append<KItemSceneData, KItemScene>(
							pOwner, wszNextDecodePos, DECODE_KEY_SCENE_BEGIN, 
							DECODE_KEY_SCENE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_BOX_BEGIN:
						Append<KItemBoxData, KItemBox>(
							pOwner, wszNextDecodePos, DECODE_KEY_BOX_BEGIN, 
							DECODE_KEY_BOX_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_HANDLE_BEGIN:
						Append<KItemHandleData, KItemHandle>(
							pOwner, wszNextDecodePos, DECODE_KEY_HANDLE_BEGIN, 
							DECODE_KEY_HANDLE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_TREELEAF_BEGIN:
						Append<KItemTreeLeafData, KItemTreeLeaf>(
							pOwner, wszNextDecodePos, DECODE_KEY_TREELEAF_BEGIN, 
							DECODE_KEY_TREELEAF_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					}
					wszNextDecodePos += nDecodedLen;
				}
				break;
			default:
				++wszNextDecodePos;
			}
		}

Exit1:
		nDecodedLen = (int)(wszNextDecodePos - wszBuffer);
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::InsertItem(
		KItemHandle *pOwner, LPCWSTR wszBuffer, KItemNull *pReferenceItem, 
		int nBeforeOrBehind, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nBeginKey = DECODE_KEY_INVALID;
		LPCWSTR wszNextDecodePos = wszBuffer;

		nDecodedLen = 0;

		KG_PROCESS_ERROR(pOwner);
		KG_PROCESS_ERROR(wszBuffer);
		KG_PROCESS_ERROR(pReferenceItem);

		while (true)
		{
			switch (*wszNextDecodePos)
			{
			case L'\0':
				goto Exit1;
				break;
			case L'<':
				{
					nBeginKey = GetItemBeginKey(wszNextDecodePos, nDecodedLen);
					if (nBeginKey == DECODE_KEY_INVALID)
					{
						++wszNextDecodePos;
						break;
					}
					wszNextDecodePos += nDecodedLen;
					nDecodedLen = 0;
					switch (nBeginKey)
					{
					case DECODE_KEY_NULL_BEGIN:
						Insert<KItemNullData, KItemNull>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_NULL_BEGIN, DECODE_KEY_NULL_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_TEXT_BEGIN:
						Insert<KItemTextData, KItemText>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_TEXT_BEGIN, DECODE_KEY_TEXT_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_IMAGE_BEGIN:
						Insert<KItemImageData, KItemImage>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_IMAGE_BEGIN, DECODE_KEY_IMAGE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_SHADOW_BEGIN:
						Insert<KItemShadowData, KItemShadow>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_SHADOW_BEGIN, DECODE_KEY_SHADOW_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_ANIMATE_BEGIN:
						Insert<KItemAnimateData, KItemAnimate>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_ANIMATE_BEGIN, DECODE_KEY_ANIMATE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_SCENE_BEGIN:
						Insert<KItemSceneData, KItemScene>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_SCENE_BEGIN, DECODE_KEY_SCENE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_BOX_BEGIN:
						Insert<KItemBoxData, KItemBox>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_BOX_BEGIN, DECODE_KEY_BOX_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_HANDLE_BEGIN:
						Insert<KItemHandleData, KItemHandle>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_HANDLE_BEGIN, DECODE_KEY_HANDLE_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					case DECODE_KEY_TREELEAF_BEGIN:
						Insert<KItemTreeLeafData, KItemTreeLeaf>(
							pOwner, wszNextDecodePos, &pReferenceItem, nBeforeOrBehind,
							DECODE_KEY_TREELEAF_BEGIN, DECODE_KEY_TREELEAF_END, nDecodedLen, pEventOwner, fScale
							);
						break;
					}
					wszNextDecodePos += nDecodedLen;
				}
				break;
			default:
				++wszNextDecodePos;
			}
		}

Exit1:
		nDecodedLen = (int)(wszNextDecodePos - wszBuffer);
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::CreateItemHandle(
		KItemHandle **ppItemHandle, LPCWSTR wszBuffer, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nGetFirstHandle = false; 
		KItemHandleData HandleData(pEventOwner);
		LPCWSTR wszNextDecodePos = wszBuffer;

		KG_PROCESS_ERROR(ppItemHandle);
		ASSERT(!(*ppItemHandle));
		KG_PROCESS_ERROR(wszBuffer);

		while (true)
		{
			switch (*wszNextDecodePos)
			{
			case L'<':
				{
					nRetCode = GetItemBeginKey(wszBuffer, nDecodedLen);
					if (nRetCode != DECODE_KEY_INVALID)
						wszNextDecodePos += nDecodedLen;
					else
						++wszNextDecodePos;
					if (nRetCode == DECODE_KEY_HANDLE_BEGIN)
						nGetFirstHandle = true;
				}
				break;
			case L'\0':
				goto Exit0;
				break;
			default:
				++wszNextDecodePos;
				break;
			}
			if (nGetFirstHandle)
				break;
		}
		nRetCode = DecodeItem<KItemHandleData>(
			wszNextDecodePos, DECODE_KEY_HANDLE_BEGIN, DECODE_KEY_HANDLE_END, HandleData, nDecodedLen
			);
		KG_PROCESS_ERROR(nRetCode);
		wszNextDecodePos += nDecodedLen;

		HandleData.fScale = fScale;
		*ppItemHandle = KItemHandle::Create(HandleData);
		KG_PROCESS_ERROR(*ppItemHandle);

		AppendItem(*ppItemHandle, wszNextDecodePos, nDecodedLen, pEventOwner, fScale);
		wszNextDecodePos += nDecodedLen;

		nDecodedLen = (int)(wszNextDecodePos - wszBuffer);
		nResult = true;
Exit0:
		if (!nResult && ppItemHandle)
			SAFE_RELEASE(*ppItemHandle);
		return nResult;
	}


	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemNullData &OutData, LPCSTR pcszNewName)
	{
		char szBuffer[512];
		int nValue = 0;
		int nValue1 = 0;
		static int nsTipIndex = 0;

		ASSERT(pIni);
		ASSERT(pcszSectionName);

        wcsncpy(OutData.wszName, ATL::CA2W(pcszNewName ? pcszNewName : pcszSectionName, GetCodePage()), _countof(OutData.wszName));
		OutData.wszName[_countof(OutData.wszName) - 1] = L'\0';

		if (pIni->GetInteger(pcszSectionName, "Left", INT_MIN, &nValue))
			OutData.fRelX = static_cast<float>(nValue);
		if (pIni->GetInteger(pcszSectionName, "Top", INT_MIN, &nValue))
			OutData.fRelY = static_cast<float>(nValue);
		if (pIni->GetInteger(pcszSectionName, "Width", INT_MIN, &nValue))
		{
			OutData.nAutoSize = false;
			OutData.fWidth = static_cast<float>(nValue);
		}
		if (pIni->GetInteger(pcszSectionName, "Height", INT_MIN, &nValue))
		{
			OutData.nAutoSize = false;
			OutData.fHeight = static_cast<float>(nValue);
		}
		if (pIni->GetInteger(pcszSectionName, "PosType", INT_MIN, &nValue))
			OutData.nPosType = nValue;
		if (pIni->GetInteger(pcszSectionName, "EventID", INT_MIN, &nValue))
			OutData.dwEventID = static_cast<DWORD>(nValue);
		if (pIni->GetInteger(pcszSectionName, "DisableScale", INT_MIN, &nValue))
			OutData.nDisableScale = nValue;
		if (pIni->GetInteger(pcszSectionName, "LockShowAndHide", INT_MIN, &nValue))
			OutData.nLockShowAndHide = nValue;
		if (pIni->GetString(pcszSectionName, "$Tip", "", szBuffer, _countof(szBuffer)))
		{
			if (szBuffer[0])
			{
				int nIndex = -1;
				nValue = 0;
				pIni->GetInteger(pcszSectionName, "OrgTip", 0, &nValue);
				if (nValue)
				{
					char szTipName[32];
					sprintf(szTipName, "$$$TipText_%d", nsTipIndex);
					++nsTipIndex;
					g_pUI->m_GlobalStrValuableMgr.RecodeValue(szBuffer);
					g_pUI->m_GlobalStrValuableMgr.Append(szTipName, szBuffer);
					nIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szTipName);
				}
				else
				{
					nIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szBuffer);
				}
				pIni->GetInteger(pcszSectionName, "ShowTipType", 0, &nValue);
				pIni->GetInteger(pcszSectionName, "TipRichText", 0, &nValue1);
				OutData.nTipIndex = KTipCenter::GetSelf().Append(nIndex, nValue, nValue1);
			}
		}
		if (pIni->GetInteger(pcszSectionName, "Alpha", 255, &nValue))
			OutData.nAlpha = nValue;

		if (pIni->GetString(pcszSectionName, "Script", "", &ms_DecodedScriptSrcBuffer[0], ms_DecodedScriptSrcBuffer.GetSize()))
        {
            wcsncpy(&ms_DecodedScriptBuffer[0], ATL::CA2W(&ms_DecodedScriptSrcBuffer[0], GetCodePage()), ms_DecodedScriptBuffer.GetSize() - 1);
            ms_DecodedScriptBuffer[ms_DecodedScriptBuffer.GetSize() - 1] = L'\0';

			OutData.nScript = true;
        }
		if (pIni->GetString(pcszSectionName, "AreaFile", "", szBuffer, _countof(szBuffer)))
		{
			FormatFilePath(szBuffer);
			OutData.nAreaTestIndex = g_AreaTester.GetID(szBuffer);
		}
		if (pIni->GetInteger(pcszSectionName, "IntPos", INT_MIN, &nValue))
			OutData.nIntPos = nValue;

        if (pIni->GetString(pcszSectionName, "Link", "", szBuffer, _countof(szBuffer)))
        {
            OutData.nLink = true;
            wcsncpy(OutData.wszLinkInfo, ATL::CA2W(szBuffer, GetCodePage()), _countof(OutData.wszLinkInfo));
            OutData.wszLinkInfo[_countof(OutData.wszLinkInfo) - 1] = L'\0';
        }
		return true;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemTextData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;
		char szText[512];
		szText[0] = '\0';

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetInteger(pcszSectionName, "MultiLine", 0, &nValue))
			OutData.nMultiLine = nValue;
		if (pIni->GetInteger(pcszSectionName, "RowSpacing", 0, &nValue))
			OutData.fRowSpacing = (float)nValue;
		if (pIni->GetInteger(pcszSectionName, "FontSpacing", 0, &nValue))
			OutData.fFontSpacing = (float)nValue;

		if (pIni->GetInteger(pcszSectionName, "HAlign", 0, &nValue))
			OutData.nHAlign = nValue;
		if (pIni->GetInteger(pcszSectionName, "VAlign", 0, &nValue))
			OutData.nVAlign = nValue;

		if (pIni->GetInteger(pcszSectionName, "CenterEachRow", 0, &nValue))
			OutData.nCenterEachLine = nValue;
		if (pIni->GetInteger(pcszSectionName, "RichText", 1, &nValue))
			OutData.nRichText = nValue;
		if (pIni->GetInteger(pcszSectionName, "AutoEtc", 0, &nValue))
			OutData.nAutoEtc = nValue;

		if (pIni->GetInteger(pcszSectionName, "FontScheme", 0, &nValue))
			OutData.nFontScheme = nValue;
		if (pIni->GetString(pcszSectionName, "$Text", "", szText, _countof(szText)))
		{
			OutData.nText = true;
			nValue = 0;
			pIni->GetInteger(pcszSectionName, "OrgText", 0, &nValue);
			if (nValue)
			{
				g_pUI->m_GlobalStrValuableMgr.RecodeValue(szText);
				SetDecodedText(ATL::CA2W(szText, GetCodePage()));
			}
			else
			{
				SetDecodedText(ATL::CA2W(g_pUI->m_GlobalStrValuableMgr.GetAt(szText), GetCodePage()));
			}
		}
		if (pIni->GetInteger(pcszSectionName, "ShowAll", 1, &nValue))
			OutData.nShowAll = nValue;
		if (pIni->GetInteger(pcszSectionName, "MlAutoAdj", 0, &nValue))
			OutData.nAutoAdj = nValue;

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemImageData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetString(pcszSectionName, "Image", "", OutData.szImagePath, _countof(OutData.szImagePath));
        FormatFilePath(OutData.szImagePath);

		if (pIni->GetInteger(pcszSectionName, "Frame", 0, &nValue))
			OutData.nFrame = nValue;

		if (pIni->GetInteger(pcszSectionName, "ImageType", 0, &nValue))
			OutData.nPartImageType = nValue;

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemShadowData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;
	    char szColor[32];

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetString(pcszSectionName, "ShadowColor", "", szColor, _countof(szColor)))
        {
            int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szColor, GetCodePage()));
            if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
                OutData.dwShadowColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
        }
		if (pIni->GetInteger(pcszSectionName, "Alpha", 0, &nValue))
			OutData.dwShadowColor = AddAlpha(OutData.dwShadowColor, nValue);
		else
			OutData.dwShadowColor = AddAlpha(OutData.dwShadowColor, 128);

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, UI::KItemAnimateData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;
        char szImagePath[MAX_PATH];

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetString(pcszSectionName, "Image", "", szImagePath, _countof(szImagePath));
        {
            strncpy(OutData.szImagePath, szImagePath, _countof(OutData.szImagePath));
            FormatFilePath(OutData.szImagePath);
        }

		if (pIni->GetInteger(pcszSectionName, "Group", 0, &nValue))
			OutData.nGroup = nValue;

		if (pIni->GetInteger(pcszSectionName, "LoopCount", 0, &nValue))
			OutData.nLoopCount = nValue;

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, UI::KItemSceneData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetInteger(pcszSectionName, "RenderTerrain", 0, &nValue))
			OutData.nEnableRenderTerrain = nValue;
		if (pIni->GetInteger(pcszSectionName, "RenderSky", 0, &nValue))
			OutData.nEnableRenderSkyBox = nValue;
		if (pIni->GetInteger(pcszSectionName, "EnableAlpha", 0, &nValue))
			OutData.nEnableAlpha = nValue;

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemBoxData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetInteger(pcszSectionName, "Index", -1, &nValue))
			OutData.nIndex = nValue;

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, UI::KItemHandleData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemNullData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		if (pIni->GetInteger(pcszSectionName, "HandleType", INT_MIN, &nValue))
			OutData.nHandleType = nValue;
		if (pIni->GetInteger(pcszSectionName, "MinRowHeight", INT_MIN, &nValue))
			OutData.fRowHeight = static_cast<float>(nValue);
		if (pIni->GetInteger(pcszSectionName, "MaxRowHeight", INT_MIN, &nValue))
			OutData.fMaxRowHeight = static_cast<float>(nValue);
		if (pIni->GetInteger(pcszSectionName, "RowSpacing", INT_MIN, &nValue))
			OutData.fRowSpacing = static_cast<float>(nValue);
		if (pIni->GetInteger(pcszSectionName, "FirstItemPosType", INT_MIN, &nValue))
			OutData.nFirstItemPosType = nValue;
		if (pIni->GetInteger(pcszSectionName, "PixelScroll", INT_MIN, &nValue))
			OutData.nPixelScroll = nValue;
		if (pIni->GetInteger(pcszSectionName, "ControlShow", INT_MIN, &nValue))
			OutData.nControlShow = nValue;
		
		nResult = true;
Exit0:
		return nResult;
	}

	int KUiComponentsDecoder::DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemTreeLeafData &OutData, LPCSTR pcszNewName)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = false;
		char szColor[32];

		ASSERT(pIni);
		ASSERT(pcszSectionName);

		nRetCode = DecodeItem(pIni, pcszSectionName, (KItemHandleData &)OutData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		pIni->GetString(pcszSectionName, "IconImage", "", OutData.szImageName, _countof(OutData.szImageName) - 1);
        FormatFilePath(OutData.szImageName);

		if (pIni->GetInteger(pcszSectionName, "ShowLine", 1, &nValue))
			OutData.nShowLine = nValue;
		if (pIni->GetInteger(pcszSectionName, "ExpandIconFrame", -1, &nValue))
			OutData.nExpandIconFrame = nValue;
		if (pIni->GetInteger(pcszSectionName, "CollapseIconFrame", -1, &nValue))
			OutData.nCollapseIconFrame = nValue;
		if (pIni->GetInteger(pcszSectionName, "Indent", 0, &nValue))
			OutData.nIndent = nValue;
		if (pIni->GetInteger(pcszSectionName, "IndentWidth", 0, &nValue))
			OutData.fEachIndentWidth = static_cast<float>(nValue);

		if (pIni->GetInteger(pcszSectionName, "IconWidth", 0, &nValue) && nValue)
		{
			OutData.fIconWidth = static_cast<float>(nValue);
			OutData.nAutoIconSize = false;
		}
		if (pIni->GetInteger(pcszSectionName, "IconHeight", 0, &nValue) && nValue)
		{
			OutData.fIconHeight = static_cast<float>(nValue);
			OutData.nAutoIconSize = false;
		}

		if (pIni->GetInteger(pcszSectionName, "AlwaysNode", 0, &nValue))
			OutData.nAlwaysNode = nValue;

		if (pIni->GetString(pcszSectionName, "LineColor", "", szColor, _countof(szColor)))
        {
            int nColorScheme = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szColor, GetCodePage()));
            if (g_pUI->m_ColorSchemeMgr.IsExistColor(nColorScheme))
			{
                OutData.dwLineColor = g_pUI->m_ColorSchemeMgr.GetColor(nColorScheme);
			}
        }

		nResult = true;
Exit0:
		return nResult;
	}

	KItemNull* KUiComponentsDecoder::AppendItem(
		KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, 
		KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName /*= NULL*/
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nItemType = ITEM_UNKNOWN;
		char szItemType[ITEM_TYPE_NAME_MAX_SIZE];
        KItemNull *pItem = NULL;

		ASSERT(pOwner);
		ASSERT(pIni);
		ASSERT(pcszSectionName);
		KGLOG_PROCESS_ERROR(pIni->IsSectionExist(pcszSectionName));

		pIni->GetString(pcszSectionName, "._WndType", "", szItemType, _countof(szItemType));

		nItemType = g_GetWndItemType(szItemType);
		switch (nItemType) 
		{
		case ITEM_NULL:
			pItem = Append<KItemNullData, KItemNull>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_TEXT:
			pItem = Append<KItemTextData, KItemText>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_IMAGE:
			pItem = Append<KItemImageData, KItemImage>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_SHADOW:
			pItem = Append<KItemShadowData, KItemShadow>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_ANIAMTE:
			pItem = Append<KItemAnimateData, KItemAnimate>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_SCENE:
			pItem = Append<KItemSceneData, KItemScene>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_BOX:
			pItem = Append<KItemBoxData, KItemBox>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;			
		case ITEM_HANDLE:
			pItem = Append<KItemHandleData, KItemHandle>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		case ITEM_TREE_LEAFE:
			pItem = Append<KItemTreeLeafData, KItemTreeLeaf>(pOwner, pIni, pcszSectionName, pcszNewName, pEventOwner, fScale);
			break;
		}
		KG_PROCESS_ERROR(pItem);

		nResult = true;
Exit0:
        if (!nResult)
        {
            KGLogPrintf(KGLOG_ERR, "KGUI KUiComponentsDecoder::AppendItem(%s, %.8x, %s, %s, %s)\n",
                pOwner->GetName(), pIni, pcszSectionName ? pcszSectionName : "", 
                g_pUI->m_WndStation.GetTreePath(pEventOwner), pcszNewName ? pcszNewName : "");
        }
		return pItem;
	}

	KItemNull* KUiComponentsDecoder::InsertItem(
		KItemHandle *pOwner, KItemNull *pReferenceItem, int nBeforeOrBehind, 
		IIniFile *pIni, LPCSTR pcszSectionName, KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName /* = NULL */
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nItemType = ITEM_UNKNOWN;
		char szItemType[ITEM_TYPE_NAME_MAX_SIZE];
        KItemNull *pItem = NULL;

		ASSERT(pOwner);
		ASSERT(pIni);
		ASSERT(pcszSectionName);
		ASSERT(pReferenceItem);

		KGLOG_PROCESS_ERROR(pIni->IsSectionExist(pcszSectionName));

		pIni->GetString(pcszSectionName, "._WndType", "", szItemType, _countof(szItemType));

		nItemType = g_GetWndItemType(szItemType);
		switch(nItemType) 
		{
		case ITEM_NULL:
			pItem = Insert<KItemNullData, KItemNull>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_TEXT:
			pItem = Insert<KItemTextData, KItemText>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_IMAGE:
			pItem = Insert<KItemImageData, KItemImage>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_SHADOW:
			pItem = Insert<KItemShadowData, KItemShadow>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_ANIAMTE:
			pItem = Insert<KItemAnimateData, KItemAnimate>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_SCENE:
			pItem = Insert<KItemSceneData, KItemScene>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_BOX:
			pItem = Insert<KItemBoxData, KItemBox>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_HANDLE:
			pItem = Insert<KItemHandleData, KItemHandle>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		case ITEM_TREE_LEAFE:
			pItem = Insert<KItemTreeLeafData, KItemTreeLeaf>(
				pOwner, pIni, pcszSectionName, pReferenceItem, nBeforeOrBehind, pcszNewName, pEventOwner, fScale
				);
			break;
		}
		KG_PROCESS_ERROR(pItem);

		nResult = true;
Exit0:
		return pItem;
	}

	int KUiComponentsDecoder::CreateItemHandle(
		KItemHandle **ppItemHandle, IIniFile *pIni, LPCSTR pcszSectionName, 
		KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName /*= NULL*/
		)
	{
		int nResult = false;
		int nRetCode = false;
		int nItemCount = false;
		int nItemType = ITEM_UNKNOWN;
		char szItemName[ITEM_NAME_MAX_SIZE];
		char szNext[ITEM_NAME_MAX_SIZE];
		KItemHandleData ItemHandleData(pEventOwner);

		szNext[0] = '\0';

		KG_PROCESS_ERROR(ppItemHandle);

		ASSERT(*ppItemHandle == NULL);

		KG_PROCESS_ERROR(pIni);
		KG_PROCESS_ERROR(pcszSectionName);

		nRetCode = pIni->IsSectionExist(pcszSectionName);
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = DecodeItem(pIni, pcszSectionName, ItemHandleData, pcszNewName);
		KG_PROCESS_ERROR(nRetCode);

		ItemHandleData.fScale = fScale;

		*ppItemHandle = KItemHandle::Create(ItemHandleData);
		KG_PROCESS_ERROR(*ppItemHandle);

		szItemName[0] = '\0';
		while (pIni->GetNextSection(szItemName, szNext))
		{
			strncpy(szItemName, szNext, _countof(szItemName));
			pIni->GetString(szItemName, "._Parent", "", szNext, _countof(szNext));
			if (_tcsicmp(szNext, pcszSectionName) == 0)
				AppendItem(*ppItemHandle, pIni, szItemName, pEventOwner, fScale);
		}
		(*ppItemHandle)->FormatAllItemPos();

		nResult = true;
Exit0:
		if (!nResult && ppItemHandle)
			SAFE_RELEASE(*ppItemHandle);
		return nResult;
	}


	int KUiComponentsDecoder::SetDecodedText(LPCWSTR wszText)
	{
		int nResult = false;

		ms_DecodedTextBuffer[0] = L'\0';

		KG_PROCESS_ERROR(wszText);

        wcsncpy(&ms_DecodedTextBuffer[0], wszText, ms_DecodedTextBuffer.GetSize() - 1);
        ms_DecodedTextBuffer[ms_DecodedTextBuffer.GetSize() - 1] = L'\0';

		nResult = true;
Exit0:
		return nResult;
	}

	LPCWSTR KUiComponentsDecoder::GetDecodedText()
	{
		return &ms_DecodedTextBuffer[0];
	}

	LPCWSTR KUiComponentsDecoder::GetDecodedScript()
	{
		return &ms_DecodedScriptBuffer[0];
	}


	LPCWSTR KUiComponentsDecoder::EncodeString(LPCWSTR wszString)
	{
		LPCWSTR wszResult = NULL;
		LPWSTR wszEncoded = NULL;

		KG_PROCESS_ERROR(wszString);

        if ((int)wcslen(wszString) >= ms_EncodeStringBuffer.GetSize())
        {
            ms_EncodeStringBuffer.Alloc((int)wcslen(wszString) * 2);
            ms_EncodeStringBuffer[0] = L'\0';
        }

        wszEncoded = &ms_EncodeStringBuffer[0];

		*wszEncoded = L'\"';	//前面加引号
		++wszEncoded;

		while (true)
		{
			switch (*wszString)
			{
			case L'\"':
			case L'\\':
			case L'\n':
			case L'\t':
				*wszEncoded = L'\\';
				++wszEncoded;
				*wszEncoded = *wszString;
				break;
			case L'\0':
				*wszEncoded = L'\"';
				++wszEncoded;
				*wszEncoded = L'\0';
				goto Exit1;
				break;
			default:
				*wszEncoded = *wszString;
				break;
			}
			++wszEncoded;
			++wszString;
		}

Exit1:
        ASSERT((int)wcslen(&ms_EncodeStringBuffer[0]) < ms_EncodeStringBuffer.GetSize());
		wszResult = &ms_EncodeStringBuffer[0];
Exit0:
		return wszResult;
	}

	LPCWSTR KUiComponentsDecoder::GetPureText(LPCWSTR wszString)
	{
		if (!wszString)
			return NULL;
		DWORD dwKeyLen = (DWORD)wcslen(STR_DECODE_TEXT);
		LPWSTR pwszBuffer = &ms_EncodeStringBuffer[0];
        int nBufferLen = (int)ms_EncodeStringBuffer.GetSize() - 1;
		while (*wszString)
		{
			if (wcsnicmp(wszString, STR_DECODE_TEXT, dwKeyLen) == 0)
			{
				wszString += dwKeyLen;
				if (*wszString == L'=' && *(wszString + 1) == L'\"')
				{
					++wszString;
					int nLen = 0;
					DecodeString(wszString, pwszBuffer, nBufferLen, nLen);
					wszString += nLen;
					nLen -= 2;
					if (nLen < 0)
						nLen = 0;
					pwszBuffer += nLen;
					nBufferLen -= nLen;
					
				}
				continue;
			}
			++wszString;
		}
		*pwszBuffer = L'\0';
		return &ms_EncodeStringBuffer[0];
	}

} //namespace UI