// ChatFilter.cpp: implementation of the CChatFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KEngine.h"
#include "ChatFilter.h"
#include "stdio.h"
#include "../../Engine/Src/Cryptography/EDOneTimePad.h"
#include "../ErrorCode.h"
#include "malloc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

const char file_chatflt[] = "\\settings\\chatsent.flt";

//////////////////////////////////////////////////////////////////////

const size_t EXP_MAXLEN = 1024;


extern CFilterTextLib g_libFilterText;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatFilter::CChatFilter()
	: m_pTextFilter(NULL)
{

}

CChatFilter::~CChatFilter()
{

}


BOOL CChatFilter::Initialize()
{
	Error_SetErrorString("CChatFilter::Initialize()");
	if (m_pTextFilter)
	{
		return TRUE;
	}

	KPakFile	FileChatFlt;

	if (FileChatFlt.Open((char*)file_chatflt) == FALSE)
	{
		Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
		Error_SetErrorString(file_chatflt);
		return FALSE;
	}

	if (SUCCEEDED(g_libFilterText.CreateTextFilter(&m_pTextFilter)))
	{
		char* pBuffer = NULL;
		int nSize = FileChatFlt.Size();
		if (nSize)
			pBuffer = (char*)_alloca(nSize + 1);
		if (pBuffer)
		{
			int nFinalSize = FileChatFlt.Read(pBuffer, nSize);
			if (nFinalSize >= nSize)
				pBuffer[nSize] = 0;
			else
				memset(pBuffer + nFinalSize, 0, nSize - nFinalSize + 1);

			char* pLineHeader = pBuffer;

			do
			{
				char* pLineEnd = strchr(pLineHeader, 0x0a);
				int nLineLen;
				if (pLineEnd)
				{
					*pLineEnd = 0;
					nLineLen = pLineEnd - pLineHeader;
				}
				else
				{
					nLineLen = nFinalSize - (pLineHeader - pBuffer);
				}

				if (pLineHeader[0] && nLineLen > 0)
				{
					if (pLineHeader[nLineLen - 1] == 0x0d)
						pLineHeader[--nLineLen] = 0;
					if (nLineLen)
					{
						EDOneTimePad_Decipher(pLineHeader, nLineLen);
						m_pTextFilter->AddExpression(pLineHeader);
					}
				}

				pLineHeader = pLineEnd ? pLineEnd + 1: NULL;
			}while(pLineHeader);
		}
	}

	FileChatFlt.Close();

	if(m_pTextFilter)
	{
		return TRUE;
	}
	else
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		return FALSE;
	}
}

BOOL CChatFilter::Uninitialize()
{
	if (m_pTextFilter)
	{
		m_pTextFilter->Release();
		m_pTextFilter = NULL;
	}

	return TRUE;
}

BOOL CChatFilter::IsTextPass(LPCTSTR text)
{
	if (!m_pTextFilter)
		return FALSE;

	return m_pTextFilter->IsTextPass(text);
}

