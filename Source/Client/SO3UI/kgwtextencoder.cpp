#include "stdafx.h"
#include "./kgwtextencoder.h"
#include "SO3UI.h"
#include "SO3UI/IKGUI.h"

KGWTextEncoder::KGWTextEncoder()
	: m_fnReset(NULL)
{
}


KGWTextEncoder::~KGWTextEncoder()
{
}


KGWTextEncoder& KGWTextEncoder::GetSelf()
{
	static KGWTextEncoder s_self;
	return s_self;
}

KGWTextEncoder& KGWTextEncoder::GetTalkEncoder()
{
	static KGWTextEncoder s_TalkEncoder;
	return s_TalkEncoder;
}

void KGWTextEncoder::Reset()
{
	m_aEncoder.clear();
	m_fnReset = NULL;
}


bool KGWTextEncoder::RegisterPattern(LPCWSTR wszPatternBegin, LPCWSTR wszPatternEnd, FnEncoder fn)
{

	ASSERT(wszPatternBegin);
	ASSERT(wszPatternEnd);

	static LPCWSTR s_wszReserveds[] =
	{
		L"text",
        L"param",
	};

	BOOL bRetCode = FALSE;
	KEncoderItem item;

	item.nBegin = wcslen(wszPatternBegin);
	item.nEnd = wcslen(wszPatternEnd);

	KGLOG_PROCESS_ERROR(item.nBegin < PATTERN_LENGTH);
	KGLOG_PROCESS_ERROR(item.nEnd < PATTERN_LENGTH);
	KGLOG_PROCESS_ERROR(fn);

	for (size_t nIndex = 0; nIndex < _countof(s_wszReserveds); ++nIndex)
	{
		bRetCode = wcscmp(s_wszReserveds[nIndex], wszPatternBegin) != 0;
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = wcscmp(s_wszReserveds[nIndex], wszPatternEnd) != 0;
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = !IsExist(wszPatternBegin);
	KGLOG_PROCESS_ERROR(bRetCode);

	wcscpy(item.wszBegin, wszPatternBegin);
	wcscpy(item.wszEnd, wszPatternEnd);

	item.fnEncoder = fn;

	m_aEncoder.push_back(item);

	std::sort(m_aEncoder.begin(), m_aEncoder.end());

	return true;
Exit0:
	return false;
}


bool KGWTextEncoder::RegisterReset(FnReset fn)
{
	m_fnReset = fn;
	return true;
}


bool KGWTextEncoder::IsExist(LPCWSTR wszPatternBegin) const
{
	if (wszPatternBegin == NULL || wszPatternBegin[0] == L'\0')
		return false;

	KGWTexEncoderItemList::const_iterator it = m_aEncoder.begin();
	KGWTexEncoderItemList::const_iterator ie = m_aEncoder.end();
	for (; it != ie; ++it)
	{
		KEncoderItem const& item = *it;
		if (wcscmp(wszPatternBegin, item.wszBegin) == 0)
		{
			return true;
		}
	}

	return false;
}


bool KGWTextEncoder::Encode(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtomList& aEncodedAtom, size_t& nEncodedLength) const
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(wszStream);
	KGLOG_PROCESS_ERROR(wszStream[0] != L'\0');
	KGLOG_PROCESS_ERROR(nStreamLength > 0);

	if (m_fnReset)
	{
		bRetCode = (*m_fnReset)();
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	nEncodedLength = 0;

	while (nEncodedLength < nStreamLength)
	{
		KGWTextAtom atom;
		size_t nEncodedPatternLength = 0;

		bRetCode = EncodePattern(wszStream + nEncodedLength, nStreamLength - nEncodedLength, atom, nEncodedPatternLength);
		KGLOG_PROCESS_ERROR(bRetCode);

		nEncodedLength += nEncodedPatternLength;
		aEncodedAtom.push_back(atom);
	}

	return true;
Exit0:
	return false;
}


bool KGWTextEncoder::EncodePattern(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom, size_t& nEncodedLength) const
{
	// NOTE: 目前不处理嵌套或者不配对的情况

	BOOL bRetCode = FALSE;
    unsigned int uCodePage = 0;

	ASSERT(wszStream);
	ASSERT(nStreamLength);

    ASSERT(g_pSO3UI && g_pSO3UI->m_pUI);
    uCodePage = g_pSO3UI->m_pUI->GetCodePage();

	KGWTexEncoderItemList::const_iterator it = m_aEncoder.begin();
	KGWTexEncoderItemList::const_iterator ie = m_aEncoder.end();
	for (; it != ie; ++it)
	{
		KEncoderItem const& item = *it;
		if (wcsncmp(wszStream, item.wszBegin, item.nBegin) == 0)
		{
            LPCWSTR wszStreamEnd = NULL;
			
            wszStreamEnd = wcsstr(wszStream, item.wszEnd);
			KGLOG_PROCESS_ERROR(wszStreamEnd);

			nEncodedLength = wszStreamEnd - wszStream + item.nEnd;
			KGLOG_PROCESS_ERROR(nEncodedLength <= nStreamLength);
			
			if (nEncodedLength > 0)
			{
				if (!(*item.fnEncoder)(item.wszBegin, wszStream + item.nBegin, nEncodedLength - item.nBegin - item.nEnd, atom))
				{
					KGLogPrintf(KGLOG_ERR, "SO3UI KGWTextEncoder::EncodePattern(%s, %d) [%s, %s]\n", 
						ATL::CW2A(wszStream, uCodePage), nStreamLength, ATL::CW2A(item.wszBegin, uCodePage), ATL::CW2A(item.wszEnd, uCodePage));
				}
			}

			return true;
		}
	}

	return EncodePlainText(wszStream, nStreamLength, atom, nEncodedLength);

Exit0:
	return false;
}


LPCWSTR KGWTextEncoder::FindPattern(LPCWSTR wszStream, size_t nStreamLength) const
{
	ASSERT(wszStream);
	
	LPCWSTR wszStreamFindMin = wszStream + nStreamLength;

	KGWTexEncoderItemList::const_iterator it = m_aEncoder.begin();
	KGWTexEncoderItemList::const_iterator ie = m_aEncoder.end();
	for (; it != ie; ++it)
	{
		KEncoderItem const& item = *it;

		LPCWSTR wszStreamFind = wcsstr(wszStream, item.wszBegin);
		if (wszStreamFind)
		{
			if (wszStreamFind < wszStreamFindMin)
			{
				wszStreamFindMin = wszStreamFind;
			}
		}
	}

	return wszStreamFindMin;
}


bool KGWTextEncoder::EncodePlainText(LPCWSTR wszStream, size_t nStreamLength, KGWTextAtom& atom, size_t& nEncodedLength) const
{
	ASSERT(atom.IsEmpty());

	LPCWSTR wszStreamEnd = NULL;

	wszStreamEnd = FindPattern(wszStream, nStreamLength);
	if (wszStreamEnd == wszStream + nStreamLength)
	{
		atom.SetContext(wszStream);
		nEncodedLength = nStreamLength;
	}
	else
	{
        std::wstring strTemp(wszStream, wszStreamEnd);
		atom.SetContext(strTemp.c_str());
		nEncodedLength = wszStreamEnd - wszStream;
	}

	atom.SetName(L"text");
	return true;
}


