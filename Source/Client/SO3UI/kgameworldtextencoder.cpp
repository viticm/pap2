#include "stdafx.h"
#include "./kgameworldtextencoder.h"
#include "SO3UI.h"
#include "SO3UI/IKGUI.h"

int	KGameWorldTextEncoder::ms_nEncoded = 0;
int	KGameWorldTextEncoder::ms_nParam = 0;
int	KGameWorldTextEncoder::ms_nString = 0;
int	KGameWorldTextEncoder::ms_nPlayer = 0;
int	KGameWorldTextEncoder::ms_nItem = 0;
int	KGameWorldTextEncoder::ms_nQuest = 0;
int KGameWorldTextEncoder::ms_nOwner = 0;
int KGameWorldTextEncoder::ms_nShop = 0;
int KGameWorldTextEncoder::ms_nSelf = 0;
int KGameWorldTextEncoder::ms_nRoleType = 0;
int KGameWorldTextEncoder::ms_nMail = 0;
int KGameWorldTextEncoder::ms_nBank = 0;
int	KGameWorldTextEncoder::ms_GuildBank = 0;
int KGameWorldTextEncoder::ms_nFont = 0;
int KGameWorldTextEncoder::ms_nPic = 0;
int KGameWorldTextEncoder::ms_nAnimate = 0;
int	KGameWorldTextEncoder::ms_nHeightControl = 0;
int KGameWorldTextEncoder::ms_nBattleField = 0;
int	KGameWorldTextEncoder::ms_nSkillFormula = 0;
int	KGameWorldTextEncoder::ms_nCraftFormula = 0;
int	KGameWorldTextEncoder::ms_nSpace = 0;
int	KGameWorldTextEncoder::ms_nMoney = 0;
int	KGameWorldTextEncoder::ms_nTraffic = 0;
int	KGameWorldTextEncoder::ms_nAuction = 0;
int	KGameWorldTextEncoder::ms_nEdit = 0;
int	KGameWorldTextEncoder::ms_nSound = 0;
int	KGameWorldTextEncoder::ms_nAction = 0;
int	KGameWorldTextEncoder::ms_nWait = 0;
int	KGameWorldTextEncoder::ms_nPage = 0;
int KGameWorldTextEncoder::ms_nCardSell = 0;
int KGameWorldTextEncoder::ms_nCardBuy = 0;


KGameWorldTextEncoder::KGameWorldTextEncoder()
{
	Reset();
}


KGameWorldTextEncoder::~KGameWorldTextEncoder()
{
}


int KGameWorldTextEncoder::Init()
{
	struct KPatternItem
	{
		LPCWSTR						wszBegin;
		LPCWSTR						wszEnd;
		KGWTextEncoder::FnEncoder	fn;
	};

	static KPatternItem const s_aPatternItem[] =
	{
		{ L"<AT",			L">",		&EncodeAction },
        { L"<BF",           L">",       &EncodeBattleField },
		{ L"<GB",			L">",		&EncodeGuildBank },
		{ L"<PG",			L">",		&EncodePage },
		{ L"<SD",			L">",		&EncodeSound },
		{ L"<WT",			L">",		&EncodeWait },
		{ L"<CS",           L">",       &EncodeCardSell},
        { L"<CB",           L">",       &EncodeCardBuy},
		{ L"<A",			L">",		&EncodeAnimate },
		{ L"<B",			L">",		&EncodeBank },
		{ L"<C",			L">",		&EncodeRoleType },
		//{ L"<D",			L">",	}, 保留,游戏逻辑的FormatString函数用的
		{ L"<E",			L">",		&EncodeCraftFormula },
		{ L"<F",			L">",		&EncodeFont },
		{ L"<G",			L">",		&EncodeSpace },
		{ L"<H",			L">",		&EncodeHeightControl },
		{ L"<I",			L">",		&EncodeItem },
		{ L"<J",			L">",		&EncodeMoney },
		{ L"<K",			L">",		&EncodeSkillFormula },
		{ L"<L",			L">",		&EncodeMail },
		{ L"<M",			L">",		&EncodeShop },
		{ L"<N",			L">",		&EncodeSelf },
		{ L"<O",			L">",		&EncodeOwner },
		{ L"<P",			L">",		&EncodePlayer },
		{ L"<Q",			L">",		&EncodeQuest },
		//<R<D%d>%s<D%d>>, 保留,游戏逻辑的FormatString函数用的
		{ L"<S",			L">",		&EncodeString },
		{ L"<T",			L">",		&EncodePic },
		{ L"<U",			L">",		&EncodeTraffic },
		{ L"<W",			L">",		&EncodeSureParam },
		{ L"<X",			L">",		&EncodeEdit },
		{ L"<Y",			L">",		&EncodeAuction },

		{ L"<$",			L">",		&EncodeParam },
	};

	BOOL bRetCode = FALSE;
    unsigned int uCodePage = 0;

    ASSERT(g_pSO3UI && g_pSO3UI->m_pUI);
    uCodePage = g_pSO3UI->m_pUI->GetCodePage();

	KGWTextEncoder::GetSelf().Reset();

	bRetCode = KGWTextEncoder::GetSelf().RegisterReset(&Reset);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (size_t nIndex = 0; nIndex < _countof(s_aPatternItem); ++nIndex)
	{
		KPatternItem const& item = s_aPatternItem[nIndex];
		bRetCode = KGWTextEncoder::GetSelf().RegisterPattern(item.wszBegin, item.wszEnd, item.fn);
		if (!bRetCode)
            KGLogPrintf(KGLOG_ERR, "SO3UI KGameWorldTextEncoder: Register pattern error (%s %s)\n", 
            ATL::CW2A(item.wszBegin, uCodePage), ATL::CW2A(item.wszEnd,uCodePage));
		KGLOG_PROCESS_ERROR(bRetCode);
	}



	static KPatternItem const s_aTalkPatternItem[] =
	{
		{ L"<AT",			L">",		&EncodeAction },
		{ L"<SD",			L">",		&EncodeSound },
//		{ L"<WT",			L">",		&EncodeWait },
	};

	KGWTextEncoder::GetTalkEncoder().Reset();

	bRetCode = KGWTextEncoder::GetTalkEncoder().RegisterReset(&ResetTalk);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (size_t nIndex = 0; nIndex < _countof(s_aTalkPatternItem); ++nIndex)
	{
		KPatternItem const& item = s_aTalkPatternItem[nIndex];
		bRetCode = KGWTextEncoder::GetTalkEncoder().RegisterPattern(item.wszBegin, item.wszEnd, item.fn);
		if (!bRetCode)
			KGLogPrintf(KGLOG_ERR, "SO3UI KGameWorldTextEncoder: Register talk pattern error (%s %s)\n", 
			ATL::CW2A(item.wszBegin, uCodePage), ATL::CW2A(item.wszEnd, uCodePage));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	return true;
Exit0:
	return false;
}


void KGameWorldTextEncoder::Exit()
{
}

bool KGameWorldTextEncoder::ResetTalk()
{
	return true;
}

bool KGameWorldTextEncoder::Reset()
{
	ms_nEncoded = 0;
	ms_nParam = 0;
	ms_nString = 0;
	ms_nPlayer = 0;
	ms_nItem = 0;
	ms_nQuest = 0;
	ms_nOwner = 0;
	ms_nShop = 0;
	ms_nSelf = 0;
	ms_nRoleType = 0;

	ms_nMail = 0;
	ms_nBank = 0;

	ms_nFont = 0;
	ms_nPic = 0;
	ms_nAnimate = 0;
	ms_nHeightControl = 0;
    ms_nBattleField = 0;
	ms_nSkillFormula = 0;
	ms_nCraftFormula = 0;
	ms_nSpace = 0;
	ms_nMoney = 0;
	ms_nTraffic = 0;
	ms_nAuction = 0;
	ms_nEdit = 0;

	ms_nSound = 0;
	ms_nAction = 0;
	ms_nWait = 0;
	ms_nPage = 0;

    ms_nCardSell = 0;
    ms_nCardBuy = 0;

	return true;
}


bool KGameWorldTextEncoder::EncodeOwner(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{		
	BOOL bRetCode = FALSE;
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"O");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nOwner, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	wszStreamNext = wcschr(wszStream, L' ');
	KGLOG_PROCESS_ERROR(wszStreamNext);

	nAttributeLength = (size_t)(wszStreamNext - wszStream);
	KGLOG_PROCESS_ERROR(nAttributeLength < nStreamLength);
	KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nAttributeLength);
	wszKey[nAttributeLength] = L'\0';

	atom.PushAttribute(L"targettype", wszKey);

	++wszStreamNext;

	nStreamLength -= nAttributeLength + 1;
	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStreamNext, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"targetid", wszKey);

	++ms_nEncoded;
	++ms_nOwner;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeParam(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"$");

	if (wszStream[0] == L'C' && wszStream[1] == L' ')
	{
		atom.PushAttribute(L"close", L"true");

		++wszStream;
		++wszStream;

		ASSERT(nStreamLength > 0);
		--nStreamLength;

		ASSERT(nStreamLength > 0);
		--nStreamLength;
	}

	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nParam, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nParam;
	return true;
}

bool KGameWorldTextEncoder::EncodeSureParam(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);
	
	atom.SetName(L"W");

	if (wszStream[0] == L'C' && wszStream[1] == L' ')
	{
		atom.PushAttribute(L"close", L"true");

		++wszStream;
		++wszStream;

		ASSERT(nStreamLength > 0);
		--nStreamLength;

		ASSERT(nStreamLength > 0);
		--nStreamLength;
	}

	//参数用空格隔开
	wszStreamNext = wcschr(wszStream, L' ');
	KGLOG_PROCESS_ERROR(wszStreamNext);

	nAttributeLength = (size_t)(wszStreamNext - wszStream);
	KGLOG_PROCESS_ERROR(nAttributeLength < nStreamLength);
	KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nAttributeLength);
	wszKey[nAttributeLength] = L'\0';

	atom.PushAttribute(L"sureid", wszKey);

	++wszStreamNext;
	nStreamLength -= nAttributeLength + 1;
	atom.SetContext(wszStreamNext, nStreamLength);
 
	_itow(ms_nParam, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nParam;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeAuction(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"Y");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nAuction, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nAuction;
	return true;
}

bool KGameWorldTextEncoder::EncodeEdit(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];
	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);
	int nIndex = 0;
	size_t nStart = 0;

	atom.SetName(L"X");
	atom.SetContext(wszStream, nStreamLength);

	if (wszStream[0] == L'C' && wszStream[1] == L' ')
	{
		atom.PushAttribute(L"close", L"true");
		
		nStart = 2;
	}
	else if (wszStream[0] == L' ')
	{
		nStart = 1;
	}
	
	for (size_t i = nStart; i <= nStreamLength; ++i)
	{
		if ((i == nStreamLength || wszStream[i] == L' ') && i != nStart)
		{
			switch (nIndex)
			{
			case 0:
				atom.PushAttribute(L"len", wszStream + nStart, wcslen(L"len"), i - nStart);
				break;
			case 1:
				atom.PushAttribute(L"width", wszStream + nStart, wcslen(L"width"), i - nStart);
				break;
			case 2:
				atom.PushAttribute(L"type", wszStream + nStart, wcslen(L"type"), i - nStart);
				break;
			case 3:
				if (wszStream[nStart] == L'1')
					atom.PushAttribute(L"required", L"true");
				break;
			default:
				break;
			}
			++nIndex;
			nStart = i + 1;
		}
	}

	_itow(ms_nEdit, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nEdit;
	return true;
}

bool KGameWorldTextEncoder::EncodeSound(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"SD");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nSound, wszKey, 10);
	atom.PushAttribute(L"id", wszKey);

	while (*wszStream == L' ' && nStreamLength > 0)
	{
		++wszStream;
		--nStreamLength;
	}

	atom.PushAttribute(L"soundid", wszStream, wcslen(L"soundid"), nStreamLength);

	++ms_nEncoded;
	++ms_nSound;

	return true;
}

bool KGameWorldTextEncoder::EncodeAction(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"AT");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nAction, wszKey, 10);
	atom.PushAttribute(L"id", wszKey);

	while (*wszStream == L' ' && nStreamLength > 0)
	{
		++wszStream;
		--nStreamLength;
	}

	for (size_t i = 0; i < nStreamLength; ++i)
	{
		if (wszStream[i] == L' ')
		{
			atom.PushAttribute(L"actionid", wszStream, wcslen(L"actionid"), i);
			wszStream = wszStream + i + 1;
			nStreamLength = nStreamLength - i - 1;
			while (*wszStream == L' ' && nStreamLength > 0)
			{
				++wszStream;
				--nStreamLength;
			}
			if (nStreamLength > 0 && *wszStream == L'0')
				atom.PushAttribute(L"face", L"false");
			else
				atom.PushAttribute(L"face", L"true");
			goto Exit1;
		}
	}

	atom.PushAttribute(L"actionid", wszStream, wcslen(L"actionid"), nStreamLength);
	atom.PushAttribute(L"face", L"true");

Exit1:
	++ms_nEncoded;
	++ms_nAction;

	return true;
}

bool KGameWorldTextEncoder::EncodeWait(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);
	int nIndex = 0;
	size_t nStart = 0;

	atom.SetName(L"WT");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nWait, wszKey, 10);
	atom.PushAttribute(L"id", wszKey);

	while (*wszStream == L' ' && nStreamLength > 0)
	{
		++wszStream;
		--nStreamLength;
	}

	for (size_t i = 0; i <= nStreamLength; ++i)
	{
		if ((i == nStreamLength || wszStream[i] == L' ') && i != nStart)
		{
			switch (nIndex)
			{
			case 0:
				atom.PushAttribute(L"waittime", wszStream + nStart, wcslen(L"waittime"), i - nStart);
				break;
			case 1:
				if (wszStream[nStart] == L'1')
					atom.PushAttribute(L"clear", L"true");
				break;
			case 2:
				if (wszStream[nStart] == L'1')
					atom.PushAttribute(L"cannotskip", L"true");
				break;
			case 3:
				if (wszStream[nStart] == L'1')
					atom.PushAttribute(L"cannotgoback", L"true");
				break;
			default:
				break;
			}
			++nIndex;
			nStart = i + 1;
		}
	}

	++ms_nEncoded;
	++ms_nWait;

	return true;
}

bool KGameWorldTextEncoder::EncodePage(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"PG");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nPage, wszKey, 10);
	atom.PushAttribute(L"id", wszKey);

	while (*wszStream == L' ' && nStreamLength > 0)
	{
		++wszStream;
		--nStreamLength;
	}

	atom.PushAttribute(L"pageid", wszStream, wcslen(L"pageid"), nStreamLength);

	++ms_nEncoded;
	++ms_nPage;

	return true;
}

bool KGameWorldTextEncoder::EncodeString(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"S");
	atom.SetContext(wszStream, nStreamLength);
	
	_itow(ms_nString, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"stringid", wszKey);

	++ms_nEncoded;
	++ms_nString;
	return true;
Exit0:
	return false;
}


bool KGameWorldTextEncoder::EncodePlayer(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"P");
	atom.SetContext(wszStream, nStreamLength);
	
	_itow(ms_nPlayer, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"playerid", wszKey);

	++ms_nEncoded;
	++ms_nPlayer;
	return true;
Exit0:
	return false;
}


bool KGameWorldTextEncoder::EncodeItem(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"I");
	atom.SetContext(wszStream, nStreamLength);
	
	_itow(ms_nItem, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"itemid", wszKey);

	++ms_nEncoded;
	++ms_nItem;
	return true;
Exit0:
	return false;
}


bool KGameWorldTextEncoder::EncodeQuest(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"Q");
	atom.SetContext(wszStream, nStreamLength);
	
	_itow(ms_nQuest, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"questid", wszKey);

	++ms_nEncoded;
	++ms_nQuest;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeShop(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"M");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nShop, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	while (wszStream[0] == L' ' && nStreamLength != 0)
	{
		++wszStream;
		--nStreamLength;
	}

	wszStreamNext = wcschr(wszStream, L' ');

	if (wszStreamNext && ((nAttributeLength = (size_t)(wszStreamNext - wszStream)) < nStreamLength))
	{
		KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nAttributeLength);
		wszKey[nAttributeLength] = L'\0';

		atom.PushAttribute(L"shopid", wszKey);

		++wszStreamNext;

		nStreamLength -= nAttributeLength + 1;
		if (nStreamLength)
		{
			atom.PushAttribute(L"shopname", wszStreamNext, wcslen(L"shopname"), nStreamLength);
		}
	}
	else
	{
		KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nStreamLength);
		wszKey[nStreamLength] = L'\0';

		atom.PushAttribute(L"shopid", wszKey);
	}

	++ms_nEncoded;
	++ms_nShop;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeSelf(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);

	atom.SetName(L"N");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nSelf, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nSelf;
	return true;
}

bool KGameWorldTextEncoder::EncodeRoleType(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);

	atom.SetName(L"C");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nRoleType, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nRoleType;
	return true;
}

bool KGameWorldTextEncoder::EncodeMail(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"L");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nMail, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nMail;
	return true;
}

bool KGameWorldTextEncoder::EncodeBank(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"B");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nBank, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_nBank;
	return true;
}

bool KGameWorldTextEncoder::EncodeGuildBank(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"GB");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_GuildBank, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	++ms_nEncoded;
	++ms_GuildBank;
	return true;
}


bool KGameWorldTextEncoder::EncodeFont(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"F");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nFont, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	wszStreamNext = wcschr(wszStream, L' ');
	KGLOG_PROCESS_ERROR(wszStreamNext);

	nAttributeLength = (size_t)(wszStreamNext - wszStream);
	KGLOG_PROCESS_ERROR(nAttributeLength < nStreamLength);
	KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nAttributeLength);
	wszKey[nAttributeLength] = L'\0';

	atom.PushAttribute(L"fontid", wszKey);

	++wszStreamNext;

	nStreamLength -= nAttributeLength + 1;

	atom.PushAttribute(L"text", wszStreamNext, wcslen(L"text"), nStreamLength);

	++ms_nEncoded;
	++ms_nFont;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodePic(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"T");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nPic, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	if (*wszStream == L'$')
	{
		WCHAR wszParam[32];

		_itow(ms_nParam, wszParam, 10);

		atom.PushAttribute(L"paramid", wszParam);

		++ms_nParam;

		++wszStream;

		ASSERT(nStreamLength > 0);
		--nStreamLength;

		if (*wszStream == L'C')
		{
			atom.PushAttribute(L"close", L"true");

			++wszStream;

			ASSERT(nStreamLength > 0);
			--nStreamLength;
		}
		if (*wszStream == L' ')
		{
			++wszStream;

			ASSERT(nStreamLength > 0);
			--nStreamLength;
		}
	}

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	for (size_t i = 0; i < nStreamLength; ++i)
	{
		if (wszStream[i] == L' ')
		{
			wcsncpy(wszKey, wszStream + i + 1, nStreamLength - i - 1);
			wszKey[nStreamLength - i - 1] = L'\0';
			atom.PushAttribute(L"tipid", wszKey);
		}
	}

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"picid", wszKey);

	++ms_nEncoded;
	++ms_nPic;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeAnimate(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"A");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nAnimate, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"animateid", wszKey);

	++ms_nEncoded;
	++ms_nAnimate;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeHeightControl(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"H");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nHeightControl, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

	wcsncpy(wszKey, wszStream, nStreamLength);
	wszKey[nStreamLength] = L'\0';

	atom.PushAttribute(L"height", wszKey);

	++ms_nEncoded;
	++ms_nHeightControl;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeBattleField(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
    int nRetCode = false;
    bool bResult = false;

    WCHAR wszBuff[256];
    LPWSTR pwszBuff = NULL;
    int nParamIndex = 0;
    int nParamLength = 0;

    KGLOG_PROCESS_ERROR(wszPatternBegin);
    KGLOG_PROCESS_ERROR(wszPatternBegin[0]);
    KGLOG_PROCESS_ERROR(wszStream);
    KGLOG_PROCESS_ERROR(nStreamLength > 0);

    atom.SetName(L"BF");
    atom.SetContext(wszStream, nStreamLength);

    _itow(ms_nBattleField, wszBuff, _countof(wszBuff));
    atom.PushAttribute(L"id", wszBuff);

    KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszBuff));
    wcsncpy(wszBuff, wszStream, nStreamLength);
    wszBuff[nStreamLength] = '\0';

    pwszBuff = wszBuff;

    while ((unsigned)(pwszBuff - wszBuff) <= nStreamLength)
    {
        pwszBuff = wcstok(pwszBuff, L" ");
        KGLOG_PROCESS_ERROR(pwszBuff);
        
        switch (nParamIndex)
        {
        case 0:
            atom.PushAttribute(L"npcid", pwszBuff);
            break;
        case 1:
            atom.PushAttribute(L"mapid", pwszBuff);
            break;
        case 2:
            atom.PushAttribute(L"groupid", pwszBuff);
            break;
        case 3:
            atom.PushAttribute(L"context", pwszBuff);
            break;
        }

        pwszBuff += wcslen(pwszBuff) + 1;
        nParamIndex++;
    }

    ms_nEncoded++;
    ms_nBattleField++;
    bResult = true;
Exit0:
    return bResult;
}

bool KGameWorldTextEncoder::EncodeSkillFormula(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"K");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nSkillFormula, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	while (wszStream[0] == L' ' && nStreamLength != 0)
	{
		++wszStream;
		--nStreamLength;
	}

	wszStreamNext = wcschr(wszStream, L' ');
	if (wszStreamNext && ((nAttributeLength = (size_t)(wszStreamNext - wszStream)) < nStreamLength))
	{
		KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nAttributeLength);
		wszKey[nAttributeLength] = L'\0';

		atom.PushAttribute(L"masterid", wszKey);

		++wszStreamNext;

		nStreamLength -= nAttributeLength + 1;
		if (nStreamLength)
		{
			atom.PushAttribute(L"text", wszStreamNext, wcslen(L"text"), nStreamLength);
		}
	}
	else
	{
		KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nStreamLength);
		wszKey[nStreamLength] = L'\0';

		atom.PushAttribute(L"masterid", wszKey);
	}

	++ms_nEncoded;
	++ms_nSkillFormula;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeCraftFormula(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
    ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"E");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nCraftFormula, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	while (wszStream[0] == L' ' && nStreamLength != 0)
	{
		++wszStream;
		--nStreamLength;
	}
	wszStreamNext = wcschr(wszStream, L' ');
	if (wszStreamNext && ((nAttributeLength = (size_t)(wszStreamNext - wszStream)) < nStreamLength))
	{
		KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nAttributeLength);
		wszKey[nAttributeLength] = L'\0';

		atom.PushAttribute(L"masterid", wszKey);

		++wszStreamNext;

		nStreamLength -= nAttributeLength + 1;
		if (nStreamLength)
		{
			atom.PushAttribute(L"text", wszStreamNext, wcslen(L"text"), nStreamLength);
		}
	}
	else
	{
		KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nStreamLength);
		wszKey[nStreamLength] = L'\0';

		atom.PushAttribute(L"masterid", wszKey);
	}

	++ms_nEncoded;
	++ms_nCraftFormula;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeSpace(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);

	atom.SetName(L"G");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nSpace, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	if (*wszStream == L'E')
		atom.PushAttribute(L"english", L"true");

	++ms_nEncoded;
	++ms_nSpace;
	return true;
}

bool KGameWorldTextEncoder::EncodeMoney(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	WCHAR wszKey[32];

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"J");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nMoney, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	if (*wszStream == L'C')
	{
		atom.PushAttribute(L"compare", L"true");

		++wszStream;

		ASSERT(nStreamLength > 0);
		--nStreamLength;

		if (*wszStream == L' ')
		{
			++wszStream;

			ASSERT(nStreamLength > 0);
			--nStreamLength;
		}
	}

	atom.PushAttribute(L"money", wszStream, wcslen(L"money"), nStreamLength);

	++ms_nEncoded;
	++ms_nMoney;
	return true;
}

bool KGameWorldTextEncoder::EncodeTraffic(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom)
{
	LPCWSTR wszStreamNext = NULL;
	WCHAR wszKey[32];
	size_t nAttributeLength = 0;

	ASSERT(wszPatternBegin && wszPatternBegin[0] != L'\0');
	ASSERT(wszStream);
	ASSERT(nStreamLength > 0);

	atom.SetName(L"U");
	atom.SetContext(wszStream, nStreamLength);

	_itow(ms_nTraffic, wszKey, 10);

	atom.PushAttribute(L"id", wszKey);

	//参数用空格隔开
	while (wszStream[0] == L' ' && nStreamLength != 0)
	{
		++wszStream;
		--nStreamLength;
	}
	wszStreamNext = wcschr(wszStream, L' ');
	if (wszStreamNext && ((nAttributeLength = (size_t)(wszStreamNext - wszStream)) < nStreamLength))
	{
		KGLOG_PROCESS_ERROR(nAttributeLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nAttributeLength);
		wszKey[nAttributeLength] = L'\0';

		atom.PushAttribute(L"pointid", wszKey);

		++wszStreamNext;

		nStreamLength -= nAttributeLength + 1;
		if (nStreamLength)
		{
			atom.PushAttribute(L"text", wszStreamNext, wcslen(L"text"), nStreamLength);
		}
	}
	else
	{
		KGLOG_PROCESS_ERROR(nStreamLength < _countof(wszKey));

		wcsncpy(wszKey, wszStream, nStreamLength);
		wszKey[nStreamLength] = L'\0';

		atom.PushAttribute(L"pointid", wszKey);
	}

	++ms_nEncoded;
	++ms_nTraffic;
	return true;
Exit0:
	return false;
}

bool KGameWorldTextEncoder::EncodeCardSell(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom &atom)
{
    int nRetCode = false;
    bool bResult = false;

    WCHAR wszKey[32];

    KGLOG_PROCESS_ERROR(wszPatternBegin);
    KGLOG_PROCESS_ERROR(wszPatternBegin[0]);
    KGLOG_PROCESS_ERROR(wszStream);
    KGLOG_PROCESS_ERROR(nStreamLength > 0);

    atom.SetName(L"CS");
    atom.SetContext(wszStream, nStreamLength);
    
    _itow(ms_nBank, wszKey, _countof(wszKey));
    wszKey[_countof(wszKey) - 1] = L'\0';
    atom.PushAttribute(L"id", wszKey);

    ms_nEncoded++;
    ms_nCardSell++;
    bResult = true;
Exit0:
    return bResult;
}

bool KGameWorldTextEncoder::EncodeCardBuy(LPCWSTR wszPatternBegin, LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom &atom)
{
    int nRetCode = false;
    bool bResult = false;

    WCHAR wszKey[32];

    KGLOG_PROCESS_ERROR(wszPatternBegin);
    KGLOG_PROCESS_ERROR(wszPatternBegin[0]);
    KGLOG_PROCESS_ERROR(wszStream);
    KGLOG_PROCESS_ERROR(nStreamLength > 0);

    atom.SetName(L"CB");
    atom.SetContext(wszStream, nStreamLength);

    _itow(ms_nBank, wszKey, _countof(wszKey));
    wszKey[_countof(wszKey) - 1] = L'\0';
    atom.PushAttribute(L"id", wszKey);

    ms_nEncoded++;
    ms_nCardBuy++;
    bResult = true;
Exit0:
    return bResult;
}