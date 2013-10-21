////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : recoder.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-15 16:11:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_DECODER_H_
#define _INCLUDE_DECODER_H_

////////////////////////////////////////////////////////////////////////////////

#include "./itemhandle.h"
#include "./itemtreeleaf.h"
#include "../../common/tblock.h"


namespace UI
{
	enum DECODE_KEY
	{
		DECODE_KEY_BEGIN = 0,
		DECODE_KEY_NULL_BEGIN = DECODE_KEY_BEGIN,
		DECODE_KEY_NULL_END,
		DECODE_KEY_NAME,
		DECODE_KEY_POS_TYPE,
		DECODE_KEY_X,
		DECODE_KEY_Y,
		DECODE_KEY_W,
		DECODE_KEY_H,
		DECODE_KEY_EVENT_ID,
		DECODE_KEY_DISABLE_SCALE,
		DECODE_KEY_LOCK_SHOW_HIDE,
		DECODE_KEY_USER_DATA,
		DECODE_KEY_SCRIPT,
		DECODE_KEY_INT_POS,
        DECODE_KEY_LINK,

		DECODE_KEY_TEXT_BEGIN,
		DECODE_KEY_TEXT_END,
		DECODE_KEY_TEXT,
		DECODE_KEY_FONT,
		DECODE_KEY_MULTI_LINE,
		DECODE_KEY_H_ALIGN,
		DECODE_KEY_V_ALIGN,
		DECODE_KEY_CENTER_EACH_ROW,
		DECODE_KEY_ROW_SPACING,
		DECODE_KEY_FONT_SPACING,
		DECODE_KEY_RICH_TEXT,
		DECODE_KEY_AUTO_ETC,
		DECODE_KEY_SHOW_ALL,
		DECODE_KEY_AUTO_ADJ,
		DECODE_KEY_R,
		DECODE_KEY_G,
		DECODE_KEY_B,

		DECODE_KEY_IMAGE_BEGIN,
		DECODE_KEY_IMAGE_END,
		DECODE_KEY_PATH,
		DECODE_KEY_FRAME,
		DECODE_KEY_IMAGE_TYPE,

		DECODE_KEY_SHADOW_BEGIN,
		DECODE_KEY_SHADOW_END,
		DECODE_KEY_COLOR,
		DECODE_KEY_ALPHA,

		DECODE_KEY_ANIMATE_BEGIN,
		DECODE_KEY_ANIMATE_END,
		DECODE_KEY_GROUP,
		DECODE_KEY_LOOP_COUNT,

		DECODE_KEY_SCENE_BEGIN,
		DECODE_KEY_SCENE_END,
		DECODE_KEY_ENABLE_FRAMEMOVE,
		DECODE_KEY_RENDER_TERRAIN,
		DECODE_KEY_RENDER_SKY,
		DECODE_KEY_ENABLE_ALPHA,

		DECODE_KEY_BOX_BEGIN,
		DECODE_KEY_BOX_END,
		DECODE_KEY_INDEX,

		DECODE_KEY_HANDLE_BEGIN,
		DECODE_KEY_HANDLE_END,
		DECODE_KEY_HANDLE_TYPE,
		DECODE_KEY_ROW_HEIGHT,
		DECODE_KEY_MAX_ROW_HEIGHT,
		DECODE_KEY_FIRST_ITEM_POS_TYPE,
		DECODE_KEY_PIXEL_SCROLL,
		DECODE_KEY_CONTROL_SHOW,

		DECODE_KEY_TREELEAF_BEGIN,
		DECODE_KEY_TREELEAF_END,
		DECODE_KEY_INDENT,
		DECODE_KEY_SHOW_LINE,
		DECODE_KEY_ALWAYS_NODE,
		DECODE_KEY_ICON_IMAGE,
		DECODE_KEY_EXPAND_FRAME,
		DECODE_KEY_COLLAPSE_FRAME,
		DECODE_KEY_INDENT_WIDTH,
		DECODE_KEY_LINE_COLOR,
		DECODE_KEY_ICON_WIDTH,
		DECODE_KEY_ICON_HEIGHT,

		DECODE_KEY_INVALID,
	};

	class KUiComponentsDecoder
	{
	private:
        enum { MAX_DECODE_TEXT_LEN = 8192 };

        struct KCtrlTable
		{
			LPCWSTR		    szCtrl;
			int	const		nCtrlLen;
			int const	    nCtrl;
		};

		KUiComponentsDecoder();
		~KUiComponentsDecoder();

	public:
		static bool Init();
		static void Exit();
		static KUiComponentsDecoder &GetSelf();

		int AppendItem(KItemHandle *pOwner, LPCWSTR wszBuffer, int &nDecodedLen, KWndWindow *pEventOwner, float fScale);
		int InsertItem(KItemHandle *pOwner, LPCWSTR wszBuffer, KItemNull *pReferenceItem, int nBeforeOrBehind, int &nDecodedLen, KWndWindow *pEventOwner, float fScale);
		int CreateItemHandle(KItemHandle **ppItemHandle, LPCWSTR wszBuffer, int &nDecodedLen, KWndWindow *pEventOwner, float fScale);

		KItemNull* AppendItem(KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName = NULL);
		KItemNull* InsertItem(KItemHandle *pOwner, KItemNull *pReferenceItem, int nBeforeOrBehind, IIniFile *pIni, LPCSTR pcszSectionName, KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName = NULL);
		int CreateItemHandle(KItemHandle **ppItemHandle, IIniFile *pIni, LPCSTR pcszSectionName, KWndWindow *pEventOwner, float fScale, LPCSTR pcszNewName = NULL);

		int SetDecodedText(LPCWSTR wszText);
		LPCWSTR GetDecodedText();

		LPCWSTR GetDecodedScript();

		LPCWSTR EncodeString(LPCWSTR wszString);

		LPCWSTR GetPureText(LPCWSTR wszString);
	private:
		int GetItemBeginKey(LPCWSTR wszBuffer, int &nDecodedLen);
		int IsItemEnd(int nItemEndKey, LPCWSTR wszBuffer, int &nDecodedLen);
		int IsAppendHandleEnd(LPCWSTR wszBuffer, int &nDecodedlen);
		int GetItemKey(int nItemBeginKey, LPCWSTR wszBuffer, int nMaxKeyLen, int &nDecodedLen);
		int DecodeString(LPCWSTR wszBuffer, LPWSTR wszDest, int nDestCount, int &nDecodedLen);

		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemNullData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemTextData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemImageData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemShadowData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemAnimateData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemSceneData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemBoxData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemHandleData &OutData, int &nDecodedLen);
		int ProcessKey(int nItemKey, LPCWSTR wszBuffer, KItemTreeLeafData &OutData, int &nDecodedLen);

		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemNullData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemTextData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemImageData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemShadowData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemAnimateData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemSceneData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemBoxData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemHandleData &OutData, LPCSTR pcszNewName);
		int DecodeItem(IIniFile *pIni, LPCSTR pcszSectionName, KItemTreeLeafData &OutData, LPCSTR pcszNewName);

		template<typename OutDataType>	
		int DecodeItem(
			LPCWSTR wszBuffer, int nItemBeginKey, int nItemEndKey, 
			OutDataType &OutData, int &nDecodedlen
			);

		template<typename ItemDataType, typename ItemType>
		ItemType* Append(KItemHandle *pOwner, LPCWSTR wszBuffer, 
			int nBeginKey, int nEndKey, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
			);

		template<typename ItemDataType, typename ItemType>
		ItemType* Insert(KItemHandle *pOwner, LPCWSTR wszBuffer, KItemNull **ppReferenceItem, int &nBeforeOrBehind,
			int nBeginKey, int nEndKey, int &nDecodedLen, KWndWindow *pEventOwner, float fScale
			);

		template<typename ItemDataType, typename ItemType>
		ItemType* Append(
			KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, 
			LPCSTR pcszNewName, KWndWindow *pEventOwner, float fScale
			);

		template<typename ItemDataType, typename ItemType>
		ItemType* Insert(
			KItemHandle *pOwner, IIniFile *pIni, LPCSTR pcszSectionName, 
			KItemNull *pReferenceItem, int nBeforeOrBehind,
			LPCSTR pcszNewName, KWndWindow *pEventOwner, float fScale
			);

	private:
		static KUiComponentsDecoder* ms_pSelf;

		static TBlock<WCHAR> ms_DecodedTextBuffer;
		static TBlock<WCHAR> ms_EncodeStringBuffer;
		static TBlock<WCHAR> ms_DecodedScriptBuffer;
        static TBlock<CHAR>  ms_DecodedScriptSrcBuffer;
	};

} //namespace UI

#endif //_INCLUDE_RECODER_H_
