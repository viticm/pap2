#include "stdafx.h"
#ifdef _UNIT_TEST
#include "KWndEdit_Test.h"
#include "../ui/elem/wndedit.h"

using namespace UI;

CPPUNIT_TEST_SUITE_REGISTRATION(KWndEdit_Test);

void KWndEdit_Test::setUp()
{
}
void KWndEdit_Test::tearDown()
{
}

//////////////////////////////////////////////////////////////////////////
//
// Pre Senario:
// (1)
// By Object(s):
// my_KWndWindow.Clone()
// Operation(s):
// Declare two UI::KWndWindow objects,One is pointer and 'New' out, another must be assigned. 
// CheckPoint(s):
// (1)check all the associated data member of the pointer object and compare to another one
//////////////////////////////////////////////////////////////////////////
void KWndEdit_Test::testClone()
{
	KWndEdit i_KWndEdit;
	KWndEdit *pKWndEdit = new KWndEdit;
	DWORD dwFlagArray[4] = {0, 1, 0x00FF, UINT_MAX};
	int nLimitLenArray[5] = {INT_MIN, -1, 0, 1, INT_MAX};
	DWORD dwBgColorArray[3] = {0, 1, UINT_MAX};

	for(int nLoopFlag = 0; nLoopFlag < 4; nLoopFlag++)
	{
		i_KWndEdit.SetFlag(dwFlagArray[nLoopFlag]);
		for(int nLoopLimitLen = 0; nLoopLimitLen < 5; nLoopLimitLen++)
		{
			i_KWndEdit.SetLimitLen(nLimitLenArray[nLoopLimitLen]);
			for(int nLoopBgKColor = 0; nLoopBgKColor < 3; nLoopBgKColor++)
			{
				i_KWndEdit.SetFocusBkColor(dwBgColorArray[nLoopBgKColor]);
				i_KWndEdit.Clone(pKWndEdit);
				CPPUNIT_ASSERT_EQUAL(pKWndEdit->m_FocusBgColor, dwBgColorArray[nLoopBgKColor]);
				CPPUNIT_ASSERT_EQUAL(pKWndEdit->m_nLimitText, nLimitLenArray[nLoopLimitLen]);
				DWORD dwRetFlag = dwFlagArray[nLoopFlag] & WNDEDIT_ES_FILTER;
				CPPUNIT_ASSERT_EQUAL(pKWndEdit->m_Flag, dwRetFlag);
			}
			nLoopBgKColor = 0;
		}
		nLoopLimitLen = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
//
// Pre Senario:
//(1)m_TextColor == 0;
// By Object(s):
// my_KWndWindow.SetColor();
// Operation(s):
//(1)pass the below valuations to SetColor function and check the result
// CheckPoint(s):
//(1)pass -1 to the SetColor function check the data member m_TextColor must >=0
//(2)pass 0 to the SetColor function check the data member m_TextColor must ==0
//(3)pass 0xFFFFFFFF to the SetColor function check the data member m_TextColor must ==FFFFFFFF
//Note:   0   ---->>>  0xFFFFFFFF
//        black -->>>  white
//////////////////////////////////////////////////////////////////////////
void KWndEdit_Test::testSetText()
{
	KWndEdit i_KWndEdit;
	int nRet		= 0;	
	char *szText[3] = {NULL, "Client", "SO3Client"};
	    
	i_KWndEdit.m_nLimitText = 8;
	for(int nLoop = 0; nLoop < 3; nLoop++)
	{
		if(nLoop ==0)
		{
			i_KWndEdit.SetText(szText[nLoop]);
            CPPUNIT_ASSERT(i_KWndEdit.m_pText);
		}
		else
		{
			nRet = strlen(szText[nLoop]);
			i_KWndEdit.m_pText = new char[nRet];
			i_KWndEdit.m_nTextLen = nRet;
			i_KWndEdit.SetText(szText[nLoop]);
			nRet = strcmp(szText[nLoop], i_KWndEdit.m_pText);
			CPPUNIT_ASSERT_EQUAL(nRet, 0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//
// Pre Senario:
//(1)
// By Object(s):
// iUIKWinEdit.SetText()
// Operation(s):
//(1)
// CheckPoint(s):
//(1)
//(2)
//////////////////////////////////////////////////////////////////////////
void KWndEdit_Test::testSetIntText()
{
	KWndEdit i_KWndEdit;
	int nRet		= 0;	
	int nTestNum[5] = {INT_MIN, -1, 0, 1, INT_MAX};
	for(int nLoop = 0; nLoop < 5; nLoop++)
	{
		i_KWndEdit.SetIntText(nTestNum[nLoop]);
		int nRet = atoi(i_KWndEdit.m_pText);
		CPPUNIT_ASSERT_EQUAL(nTestNum[nLoop], nRet);
	}
}

void KWndEdit_Test::testEnable()
{
	KWndEdit i_KWndEdit;
	DWORD dwRet		= i_KWndEdit.m_dwStyle;
	int nTestNum[5] = {INT_MIN, -1, 0, 1, INT_MAX};

	for(int nLoop = 0; nLoop < 5; nLoop++)
	{
		i_KWndEdit.Enable(nTestNum[nLoop]);
		if(nTestNum[nLoop])
		{
			dwRet &= ~WND_S_DISABLE;
			CPPUNIT_ASSERT_EQUAL(dwRet, i_KWndEdit.m_dwStyle);
		}
		else
		{
			if((i_KWndEdit.m_Flag & WNDEDIT_F_HAVE_FOCUS))
				;//to do ...
			else
			{
				dwRet |= WND_S_DISABLE;
				CPPUNIT_ASSERT_EQUAL(dwRet, i_KWndEdit.m_dwStyle);
			}
		}

	}
}
void KWndEdit_Test::testClearText()
{
	KWndEdit i_KWndEdit;
	int nRet		= 0;	
	char *szText[3] = {NULL, "Client", "SO3Client"};

	i_KWndEdit.m_nLimitText = 8;
	for(int nLoop = 0; nLoop < 3; nLoop++)
	{
		if(nLoop == 0)
		{
			i_KWndEdit.SetText(szText[nLoop]);
			CPPUNIT_ASSERT(!i_KWndEdit.m_pText);
			CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_nTextLen, 0);
			CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_nCaretPos, 0);
			CPPUNIT_ASSERT(i_KWndEdit.m_pText[0] == 0);
		}
		else
		{
			nRet = strlen(szText[nLoop]);
			i_KWndEdit.m_pText = new char[nRet];
			i_KWndEdit.m_nTextLen = nRet;
			i_KWndEdit.SetText(szText[nLoop]);
			nRet = strcmp(szText[nLoop], i_KWndEdit.m_pText);
			CPPUNIT_ASSERT_EQUAL(nRet, 0);
			i_KWndEdit.ClearText();
			CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_nTextLen, 0);
			CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_nCaretPos, 0);
			CPPUNIT_ASSERT(i_KWndEdit.m_pText[0] == 0);
		}
	}
}
void KWndEdit_Test::testInsertString()
{
}
void KWndEdit_Test::testSetFocusBkColor()
{
	KWndEdit i_KWndEdit;
	DWORD KClore[2] = {0, UINT_MAX};

	i_KWndEdit.SetFocusBkColor(KClore[0]);
	CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_FocusBgColor, KClore[0]);

	i_KWndEdit.SetFocusBkColor(KClore[1]);
	CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_FocusBgColor, KClore[1]);
}
void KWndEdit_Test::testSetLimitLen()
{
	KWndEdit i_KWndEdit;
	int nRet[5] = {INT_MIN, -1, 0, 1, INT_MAX};
	for(int nLoop = 0; nLoop < 5; nLoop++)
	{
		i_KWndEdit.SetLimitLen(nRet[nLoop]);
		CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_nLimitText, nRet[nLoop]);
	}
}
void KWndEdit_Test::testSetFlag()
{
	KWndEdit i_KWndEdit;
	DWORD nRet[2] = {0, UINT_MAX};
	for(int nLoop = 0; nLoop < 2; nLoop++)
	{
		i_KWndEdit.SetFlag(nRet[nLoop]);
		CPPUNIT_ASSERT_EQUAL(i_KWndEdit.m_Flag, nRet[nLoop]);
	}
}
void KWndEdit_Test::testSetType()
{
	KWndEdit i_KWndEdit;
	int nRet[7] = {INT_MIN, INT_MAX, -1, 3, 0, 1, 2};

	for (int nLoop = 0; nLoop < 7; nLoop++)
	{
		BOOL bRet = i_KWndEdit.SetType(nRet[nLoop]);
		if(nLoop < 4)
			CPPUNIT_ASSERT_EQUAL(bRet, FALSE);
		else
		{
			CPPUNIT_ASSERT_EQUAL(bRet, TRUE);
			DWORD dwRet = i_KWndEdit.m_Flag;
			if(nLoop == 4)
				CPPUNIT_ASSERT(dwRet == 0);
			if(nLoop == 5)
				CPPUNIT_ASSERT(dwRet == 1);
			if(nLoop == 6)
				CPPUNIT_ASSERT(dwRet == 2);
		}
	}
}
void KWndEdit_Test::testGetTextLength()
{
	KWndEdit i_KWndEdit;
	i_KWndEdit.m_nTextLen = INT_MIN;
	int nRet = i_KWndEdit.GetTextLength();
	CPPUNIT_ASSERT(nRet == INT_MIN);

	i_KWndEdit.m_nTextLen = INT_MAX;
	nRet = i_KWndEdit.GetTextLength();
	CPPUNIT_ASSERT(nRet == INT_MAX);
}
void KWndEdit_Test::testGetText()
{
	KWndEdit i_KWndEdit;
	int nRet		= 0;	
	char *szText[3] = {NULL, "Client", "SO3Client"};

	i_KWndEdit.m_nLimitText = 8;
	for(int nLoop = 0; nLoop < 3; nLoop++)
	{
		if(nLoop ==0)
		{
			i_KWndEdit.SetText(szText[nLoop]);
			CPPUNIT_ASSERT(i_KWndEdit.m_pText);
			char *pText = new char[i_KWndEdit.m_nTextLen + 1];
			int nRetByte = i_KWndEdit.GetText(pText, (int)strlen(pText), true);
			CPPUNIT_ASSERT_EQUAL(nRetByte, 0);
		}
		else
		{
			nRet = (int)strlen(szText[nLoop]);
			i_KWndEdit.m_pText = new char[nRet];
			i_KWndEdit.m_nTextLen = nRet;

			i_KWndEdit.SetText(szText[nLoop]);
			nRet = strcmp(szText[nLoop], i_KWndEdit.m_pText);
			CPPUNIT_ASSERT_EQUAL(nRet, 0);

			char *pText = new char[i_KWndEdit.m_nTextLen + 1];
			int nRetByte = i_KWndEdit.GetText(pText, (int)strlen(pText), true);
			CPPUNIT_ASSERT(!memcmp(i_KWndEdit.m_pText, szText[nLoop], nRetByte));
		}
	}
}
void KWndEdit_Test::testGetIntNumber()
{
	KWndEdit i_KWndEdit;
	char* szRet1 = "  -2147483648  ";
	i_KWndEdit.m_pText = szRet1;
	int nRet = i_KWndEdit.GetIntNumber();
	CPPUNIT_ASSERT(nRet == -2147483648);

	char* szRet2 = " -0 ";
	i_KWndEdit.m_pText = szRet2;
	nRet = i_KWndEdit.GetIntNumber();
	CPPUNIT_ASSERT(nRet == 0);

	char* szRet3 = "2147483647";
	i_KWndEdit.m_pText = szRet3;
	nRet = i_KWndEdit.GetIntNumber();
	CPPUNIT_ASSERT(nRet == 2147483647);

	char* szRet4 = "-2147483649";
	i_KWndEdit.m_pText = szRet4;
	nRet = i_KWndEdit.GetIntNumber();
	CPPUNIT_ASSERT(nRet == 2147483647);

	char* szRet5 = "2147483648";
	i_KWndEdit.m_pText = szRet5;
	nRet = i_KWndEdit.GetIntNumber();
	CPPUNIT_ASSERT(nRet == -2147483648);
}
void KWndEdit_Test::testGetCaretPosition()
{
	//²»¿É²â
}
void KWndEdit_Test::testGetFocusBgColor()
{
	KWndEdit i_KWndEdit;
	DWORD KClore[2] = {0, UINT_MAX};
	i_KWndEdit.SetFocusBkColor(KClore[0]);
	CPPUNIT_ASSERT_EQUAL(KClore[0], i_KWndEdit.GetFocusBgColor());
	i_KWndEdit.SetFocusBkColor(KClore[1]);
	CPPUNIT_ASSERT_EQUAL(KClore[1], i_KWndEdit.GetFocusBgColor());
}
void KWndEdit_Test::testGetFlag()
{
	KWndEdit i_KWndEdit;
	DWORD nRet[2] = {0, UINT_MAX};
	for(int nLoop = 0; nLoop < 2; nLoop++)
	{
		i_KWndEdit.SetFlag(nRet[nLoop]);
		CPPUNIT_ASSERT_EQUAL(i_KWndEdit.GetFlag(), nRet[nLoop]);
	}
}
void KWndEdit_Test::testGetFontSize()
{
}
void KWndEdit_Test::testGetFontColor()
{
}
void KWndEdit_Test::testGetFontBorderColor()
{
}
#endif