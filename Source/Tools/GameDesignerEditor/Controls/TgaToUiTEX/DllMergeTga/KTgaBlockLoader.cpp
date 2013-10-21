////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KTgaBlockLoader.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-19 15:34:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KTgaBlockLoader.h"
#include "UiTex.h"
#include "KTgaManager.h"
#include "KTga.h"
#include "KAnimateManager.h"
#include "KSaveManager.h"
#include "KWndFrameList.h"
#include "KIconManager.h"
#include "KPickBox.h"
#include "KShow.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////

static inline int g_IsTgaAnimateFrame(LPCTSTR pcszName)
{
    int nResult = false;
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    pcszClip = _tcsrchr(pcszName, _T('\\'));
    if (!pcszClip)
        pcszClip = pcszName;
    pcszTemp = _tcsrchr(pcszClip, _T('/'));
    if (pcszTemp)
        pcszClip = pcszTemp;

    pcszTemp = _tcsrchr(pcszClip, _T('_'));
    KG_PROCESS_ERROR(pcszTemp);

    pcszClip = _tcsrchr(pcszTemp, _T('.'));
    KG_PROCESS_ERROR(pcszClip);

    for (++pcszTemp; pcszTemp < pcszClip; ++pcszTemp) 
    {
        KG_PROCESS_ERROR(*pcszTemp >= _T('0'));
        KG_PROCESS_ERROR(*pcszTemp <= _T('9'));
    }

    nResult = true;
Exit0:
    return nResult;
}

KTgaBlockLoader *KTgaBlockLoader::ms_pSelf = NULL;

KTgaBlockLoader::KTgaBlockLoader()
{
    m_pcszUITex = NULL;
    m_pcszTga = NULL;
    m_pcszSTex = NULL;
    m_pcszText = NULL;
    m_pIcon = NULL;
    m_pszFileTitle[0] = _T("Farme");
    m_pszFileTitle[1] = _T("Left");
    m_pszFileTitle[2] = _T("Top");
    m_pszFileTitle[3] = _T("Width");
    m_pszFileTitle[4] = _T("High");
    m_pszFileTitle[5] = _T("File");
}

KTgaBlockLoader::~KTgaBlockLoader()
{
}

int KTgaBlockLoader::Init()
{
    int nResult = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KTgaBlockLoader;
    KG_PROCESS_ERROR(ms_pSelf);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(ms_pSelf);
    return nResult;
}

int KTgaBlockLoader::Exit()
{
    KG_DELETE(ms_pSelf);
    return true;
}

KTgaBlockLoader &KTgaBlockLoader::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KTgaBlockLoader::SetUITexName(LPCTSTR pcszUITex)
{
	m_pcszUITex = pcszUITex;
	return true;
}

int KTgaBlockLoader::SetTgaName(LPCTSTR pcszTga)
{
	m_pcszTga = pcszTga;
	return true;
}

int KTgaBlockLoader::SetSTexName(LPCTSTR pcszSTex)
{
	m_pcszSTex = pcszSTex;
	return true;
}

int KTgaBlockLoader::SetTextName(LPCTSTR pcszText)
{
	m_pcszText = pcszText;
	return true;
}

LPCTSTR KTgaBlockLoader::GetUITexName()
{
	return m_pcszUITex;
}

LPCTSTR KTgaBlockLoader::GetTgaName()
{
	return m_pcszTga;
}

LPCTSTR KTgaBlockLoader::GetSTexName()
{
	return m_pcszSTex;
}

LPCTSTR KTgaBlockLoader::GetTextName()
{
	return m_pcszText;
}

int KTgaBlockLoader::LoadTga(LPCTSTR pcszTgaFile)
{
    int nResult = false;
    int nRetCode = false;
    KTgaBlock *pBlock = NULL; 
    KAnimateBlock *pAnimate = NULL;
    KAnimateBlock *pParent = NULL;

    KG_PROCESS_ERROR(pcszTgaFile);
    KG_PROCESS_ERROR(!KTgaManager::GetSelf().IsTgaExist(pcszTgaFile));
    
    pBlock = KTgaBlock::CreateFromFile(pcszTgaFile);
    KG_PROCESS_ERROR(pBlock);
    
    nRetCode = KTgaManager::GetSelf().Append(pBlock);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = g_IsTgaAnimateFrame(pcszTgaFile);
    KG_PROCESS_SUCCESS(!nRetCode);

    //Todo: ²åÈë
    //pParent = KAnimateManager::GetSelf().GetParent(pcszTgaFile);
	pParent = KAnimateManager::GetSelf().GetParentJustFromName(pcszTgaFile);
    if (pParent)
    {
        nRetCode = pParent->InsertFrame(pcszTgaFile);
        KG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        LPTSTR pszClip = NULL;
        pszClip = _tcsrchr((LPTSTR)pcszTgaFile, _T('_'));
        ASSERT(pszClip);
        *pszClip = _T('\0');
        pAnimate = KAnimateBlock::Create(pcszTgaFile, true, -1);
        *pszClip = _T('_');
        KG_PROCESS_ERROR(pAnimate);
        nRetCode = pAnimate->InsertFrame(pcszTgaFile);
        KG_PROCESS_ERROR(nRetCode);
        nRetCode = KAnimateManager::GetSelf().Append(pAnimate);
        KG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KAnimateManager::GetSelf().Remove(pAnimate);
        KG_COM_RELEASE(pAnimate);
        KG_COM_RELEASE(pBlock);
    }
    return nResult;
}

int KTgaBlockLoader::LoadUITex(LPCTSTR pcszUITex, int nLoadStyle, 
							   int nReadKind, int nFindKind, int TgaInd, 
                               int nFrameNum, HWND hWnd)
{
	int nResult = false;
	int nRetCode = false; 

	size_t uReadIn;
	FILE *fpUiTex = NULL;
	UITEXFILEHEADER UiTexFileHeader;

	LPTSTR pszClip = NULL;
	LPTSTR pszTemp = NULL;
	LPTSTR pszText = NULL;

	TCHAR szTgaName[MAX_PATH];
	TCHAR szSTexName[MAX_PATH];
	TCHAR szTextName[MAX_PATH];

	szTgaName[0] = _T('\0');
	szSTexName[0] = _T('\0');
	szTextName[0] = _T('\0');

	KG_PROCESS_ERROR(pcszUITex);

	fpUiTex = fopen(pcszUITex, "rb");
	KG_PROCESS_ERROR(fpUiTex);

	uReadIn = fread(&UiTexFileHeader, sizeof(UiTexFileHeader), 1, fpUiTex);
	KG_PROCESS_ERROR(uReadIn == 1);
	fclose(fpUiTex);
	fpUiTex = NULL;
	_tcsncpy(szTgaName, pcszUITex, sizeof(szTgaName) / sizeof(TCHAR));
	szTgaName[sizeof(szTgaName) / sizeof(TCHAR) - 1] = _T('\0');
	pszClip = _tcsrchr(szTgaName, '\\');
	if (pszClip)
		++pszClip;
	else
		pszClip = szTgaName;
	pszTemp = _tcsrchr(pszClip, '/');
	if (pszTemp)
		pszClip = pszTemp + 1;
	_tcsncpy(pszClip, UiTexFileHeader.szTgaFileName, sizeof(szTgaName) / sizeof(TCHAR)- (pszClip - szTgaName));
	szTgaName[sizeof(szTgaName) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(szTextName,pcszUITex,sizeof(szTextName)/sizeof(TCHAR));
	pszText = _tcsrchr(szTextName, _T('.'));
	*pszText = _T('\0');
	_tcsncat(pszText, ".txt", sizeof(pszText) / sizeof(TCHAR));

	if (nLoadStyle == LOAD_AS_REPLACE)
	{
		nRetCode = KSaveManager::GetSelf().SetCurrentEditFile(pcszUITex, szTgaName, szSTexName, szTextName);
		KG_PROCESS_ERROR(nRetCode);        
	}

	nRetCode = SetUITexName(pcszUITex);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = SetTgaName(szTgaName);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = SetSTexName(szSTexName);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = SetTextName(szTextName);
	KG_PROCESS_ERROR(nRetCode);

//	if (nReadKind)
//	{
		nRetCode = ReadText(szTextName, nLoadStyle);
		KG_PROCESS_ERROR(nRetCode);
//	}
	nRetCode = ReadUITex(pcszUITex, nLoadStyle, nFindKind, TgaInd, nFrameNum);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (fpUiTex)
	{
		fclose(fpUiTex);
		fpUiTex = NULL;
	}
	return nResult;
}

int KTgaBlockLoader::ReadText(LPCTSTR pcszText, int nLoadStyle)
{
	int nResult = false;
	int nRetCode = false;

	FILE *fpTxt = NULL;
	TCHAR strTxtFileData[16];
    TCHAR strTxtFileTitle[16];
	TXTFILEDATA TxtFileData;
	
	KG_PROCESS_ERROR(pcszText);

	fpTxt = fopen(pcszText, "r");
	KG_PROCESS_ERROR(fpTxt);

	m_aNewTgaTxtVector.clear();
	for (int i = 0; i < 6; ++i)
	{
		fscanf(fpTxt,"%s",strTxtFileData);
        _tcsncpy(strTxtFileTitle,m_pszFileTitle[i],
            sizeof(strTxtFileTitle)/sizeof(strTxtFileTitle[0]));
        KG_PROCESS_ERROR(!strcmp(strTxtFileData,strTxtFileTitle));
	}
	while (1 == fscanf(fpTxt,"%d",&TxtFileData.nFrameNum))
	{
		TxtFileData.nTextNum = TxtFileData.nFrameNum;
		fscanf(fpTxt,"%d%d%d%d",&TxtFileData.nLeft, &TxtFileData.nTop,
			&TxtFileData.nWidth, &TxtFileData.nHeight);
        fgets(TxtFileData.szTxtFile, sizeof(TxtFileData.szTxtFile), fpTxt);
		if (TxtFileData.szTxtFile[_tcslen(TxtFileData.szTxtFile) - 1] == _T('\n'))
			TxtFileData.szTxtFile[_tcslen(TxtFileData.szTxtFile) - 1] = _T('\0');
		m_aNewTgaTxtVector.push_back(TxtFileData);
	}
	fclose(fpTxt);
	fpTxt = NULL;
	nResult = true;
Exit0:
	if (fpTxt)
	{
		fclose(fpTxt);
		fpTxt = NULL;
	}
	return nResult;
}

int KTgaBlockLoader::ReadIconText(LPCTSTR pcszText)
{
	int nResult = false;
	int nRetCode = false;
	FILE *fpText = NULL;
	TXTFILEDATA TxtFileData;
	TCHAR strTextData[16];
	LPCSTR pszCurrent = NULL;
	int i = 0;

	KG_PROCESS_ERROR(pcszText);

	fpText = fopen(pcszText, "r");
	KG_PROCESS_ERROR(fpText);

	m_aNewIconTxtVector.clear();
	for (i = 0; i < 3; ++i)
	{
		fscanf(fpText, "%s", strTextData);
	}

	while (1 == fscanf(fpText, "%d", &TxtFileData.nFrameNum))
	{
		fscanf(fpText, "%s%d", TxtFileData.szTxtFile, &TxtFileData.nTextNum);
		m_aNewIconTxtVector.push_back(TxtFileData);
	}
	fclose(fpText);
	fpText = NULL;

	nResult = true;
Exit0:
	if (fpText)
	{
		fclose(fpText);
		fpText = NULL;
	}
	return nResult;
}

int KTgaBlockLoader::ReadUITex(LPCTSTR pcszUITex, int nLoadStyle, int nFindKind, int TgaInd, int nFrameNum)
{
	int nResult = false;
	int nRetCode = false;
	int nPosed = 0;
	int nIndexCanchangeable = 0;

	KTga32 MyTga;
	LPCTSTR pcszTga;
	TCHAR szTgaName[MAX_PATH];
	szTgaName[0] = _T('\0');

	FILE *fpUiTex = NULL;
	size_t uReadIn;
	UITEXFILEHEADER UiTexFileHeader;

	KAnimateBlock *pAnimate = NULL;
	TCHAR szAnimate[MAX_PATH];
	szAnimate[0] = _T('\0');
	int nBufferSize = 0;

	KTgaBlock *pBlock = NULL;
	TCHAR szName[MAX_PATH];
    TCHAR szFrameName[MAX_PATH];
	szName[0] = _T('0');
    szFrameName[0] = _T('0');
	BYTE *pBuffer = NULL;

	UITEXFRAMEDATASTRUCTURE FrameInfo;
	std::vector<UITEXFRAMEDATASTRUCTURE> aFrameVector;

	int i = 0;
	int j = 0;
	int k = 0;

	switch(nLoadStyle) 
	{
	case LOAD_AS_APPEND:
		nPosed = false;
		nIndexCanchangeable = true;
		break;
	case LOAD_AS_REPLACE:
		nPosed = true;
		nIndexCanchangeable = false;
		break;
	default:
		ASSERT(0);
	}

	pcszTga = KTgaBlockLoader::GetSelf().GetTgaName();
	_tcsncpy(szTgaName, pcszTga, sizeof(szTgaName) / sizeof(TCHAR));
	nRetCode = MyTga.CreateFromFile((char *)szTgaName, TRUE);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = MyTga.ReadAllDataToSelfMemory();
	KG_PROCESS_ERROR(nRetCode);

	fpUiTex = fopen(pcszUITex, "rb");
	KG_PROCESS_ERROR(fpUiTex);

	uReadIn = fread(&UiTexFileHeader, sizeof(UiTexFileHeader), 1, fpUiTex);
	KG_PROCESS_ERROR(uReadIn == 1);

	aFrameVector.reserve(UiTexFileHeader.nFrameCount);
	for (i = 0; i < UiTexFileHeader.nFrameCount; ++i) 
	{
		uReadIn = fread(&FrameInfo, sizeof(FrameInfo), 1, fpUiTex);
		FrameInfo.dwInfo = 0;
		KG_PROCESS_ERROR(uReadIn == 1);
		aFrameVector.push_back(FrameInfo);
	}

	k = 0;
	for (i = 0; i < UiTexFileHeader.nAnimateCount; ++i) 
	{
		int nAnimateFrameCount = 0;
		int nThisAnimateFrame = 0;//¶¯»­Ö¡ÐòºÅ
		uReadIn = fread(&nAnimateFrameCount, sizeof(int), 1, fpUiTex);
		KG_PROCESS_ERROR(uReadIn == 1);

		LPTSTR pcszAnimate = NULL;
		if (k < 0 || k >= m_aNewTgaTxtVector.size())
			_sntprintf(szAnimate, sizeof(szAnimate) / sizeof(TCHAR), "%s:%s%d", pcszUITex, "Animate", i);
		else
		{
			_tcsncpy(szAnimate, m_aNewTgaTxtVector[k].szTxtFile, sizeof(szAnimate)/sizeof(TCHAR));
			pcszAnimate = _tcsrchr(szAnimate, _T('_'));
			if(pcszAnimate)
				*pcszAnimate = _T('\0');
		}
		pAnimate = KAnimateBlock::Create(szAnimate, nIndexCanchangeable, i);
		k += nAnimateFrameCount;
		KG_PROCESS_ERROR(pAnimate); 
		for (j = 0; j < nAnimateFrameCount; ++j)  
		{
			uReadIn = fread(&nThisAnimateFrame, sizeof(int), 1, fpUiTex);
			KG_PROCESS_ERROR(uReadIn == 1);
			KG_PROCESS_ERROR(nThisAnimateFrame < UiTexFileHeader.nFrameCount);
			aFrameVector[nThisAnimateFrame].dwInfo = 1;
			
			if (nThisAnimateFrame >= 0 && nThisAnimateFrame < (int)m_aNewTgaTxtVector.size() && m_aNewTgaTxtVector[nThisAnimateFrame].nTextNum == nThisAnimateFrame)
				_tcsncpy(szName, m_aNewTgaTxtVector[nThisAnimateFrame].szTxtFile, sizeof(szName)/sizeof(TCHAR));
			else				
				_sntprintf(szName, sizeof(szName) / sizeof(TCHAR), "%s_%d", szAnimate, j);
			szName[sizeof(szName) / sizeof(TCHAR) - 1] = _T('\0');
			if (KTgaManager::GetSelf().IsTgaExist(szName))  
				continue;
			nBufferSize = aFrameVector[nThisAnimateFrame].nWidth * aFrameVector[nThisAnimateFrame].nHeight * 4;
			KG_DELETE_ARRAY(pBuffer);
			pBuffer = new BYTE[nBufferSize];
			KG_PROCESS_ERROR(pBuffer);
			MyTga.CopyRectBufferTo(
				pBuffer, nBufferSize, 
				aFrameVector[nThisAnimateFrame].nLeft, aFrameVector[nThisAnimateFrame].nTop, 
				aFrameVector[nThisAnimateFrame].nWidth, aFrameVector[nThisAnimateFrame].nHeight
				);
			pBlock = KTgaBlock::CreateFromBuffer(
				pBuffer, nBufferSize, 
				aFrameVector[nThisAnimateFrame].nWidth, aFrameVector[nThisAnimateFrame].nHeight, 
				aFrameVector[nThisAnimateFrame].nLeft, aFrameVector[nThisAnimateFrame].nTop,
				nIndexCanchangeable, nThisAnimateFrame, nPosed, szName
				);
			KG_PROCESS_ERROR(pBlock);
			if (nFindKind == FIND_ICON_FILE)
			{
				if (nThisAnimateFrame == TgaInd)
				{
					KIconBlock *pIcon = NULL;
					LPCTSTR pcszFrameName = NULL;

                    _tcsncpy(szFrameName, pcszUITex, sizeof(szFrameName) / sizeof(szFrameName[0]));
					pcszFrameName = _tcsrchr(szName, _T('\\'));
					pcszFrameName++;
					m_pIcon = KIconBlock::CreateFromBuffer( 
						pBuffer, nBufferSize, nFrameNum, TgaInd,
						aFrameVector[i].nWidth, aFrameVector[i].nHeight,
                        szName, szFrameName
						);
					nRetCode = KIconManager::GetSelf().Append(m_pIcon);
					KG_PROCESS_ERROR(nRetCode);
					goto Exit1;
				}
			}
			else
			{
				nRetCode = KTgaManager::GetSelf().Append(pBlock);
				KG_PROCESS_ERROR(pBlock);
				nRetCode = pAnimate->AppendFrame(szName);
				KG_PROCESS_ERROR(nRetCode);
			}	
		}
		if (nFindKind == NOT_FIND_ICON_FILE)
		{	
			nRetCode = KAnimateManager::GetSelf().Append(pAnimate);
			KG_PROCESS_ERROR(nRetCode);
		}
	}

	j = 0;
	for (int i = 0; i < UiTexFileHeader.nFrameCount; ++i) 
	{
		if (aFrameVector[i].dwInfo)
			continue;
		aFrameVector[i].dwInfo = 1;

		if (i >= 0 && i < m_aNewTgaTxtVector.size() && m_aNewTgaTxtVector[i].nTextNum == i)
			_tcsncpy(szName, m_aNewTgaTxtVector[i].szTxtFile, sizeof(szName)/sizeof(TCHAR));
		else
			_sntprintf(szName, sizeof(szName) / sizeof(TCHAR), "%s:%s%d", pcszUITex, "Frame", j);
		szName[sizeof(szName) / sizeof(TCHAR) - 1] = _T('\0');
		++j;

		if (KTgaManager::GetSelf().IsTgaExist(szName))  
			continue;
		nBufferSize = aFrameVector[i].nWidth * aFrameVector[i].nHeight * 4;
		KG_DELETE_ARRAY(pBuffer);
		pBuffer = new BYTE[nBufferSize];
		KG_PROCESS_ERROR(pBuffer);
		MyTga.CopyRectBufferTo(
			pBuffer, nBufferSize, 
			aFrameVector[i].nLeft, aFrameVector[i].nTop, 
			aFrameVector[i].nWidth, aFrameVector[i].nHeight
			);
		pBlock = KTgaBlock::CreateFromBuffer(
			pBuffer, nBufferSize, 
			aFrameVector[i].nWidth, aFrameVector[i].nHeight, 
			aFrameVector[i].nLeft, aFrameVector[i].nTop,
			nIndexCanchangeable, i, nPosed, szName
			);
		KG_PROCESS_ERROR(pBlock);
		if (nFindKind == FIND_ICON_FILE)
		{
			if (i == TgaInd)
			{
                KIconBlock *pIcon = NULL;
                LPCTSTR pcszFrameName = NULL;

                _tcsncpy(szFrameName, pcszUITex, sizeof(szFrameName) / sizeof(szFrameName[0]));
                pcszFrameName = _tcsrchr(szName, _T('\\'));
                pcszFrameName++;
                m_pIcon = KIconBlock::CreateFromBuffer( 
                    pBuffer, nBufferSize, nFrameNum, TgaInd,
                    aFrameVector[i].nWidth, aFrameVector[i].nHeight,
                    szName, szFrameName
                    );
                nRetCode = KIconManager::GetSelf().Append(m_pIcon);
                KG_PROCESS_ERROR(nRetCode);
                goto Exit1;
			}
		}
		else
		{
			nRetCode = KTgaManager::GetSelf().Append(pBlock);
			KG_PROCESS_ERROR(pBlock);
		}
	}
Exit1:
	nResult = true;
Exit0:
	MyTga.Release();
	if (!nResult)
	{   
		if (nLoadStyle == LOAD_AS_REPLACE)
			KSaveManager::GetSelf().Clear();
		KTgaManager::GetSelf().Remove(pBlock);
		KAnimateManager::GetSelf().Remove(pAnimate);
		KG_COM_RELEASE(pBlock);
		KG_COM_RELEASE(pAnimate);

	}
	KG_DELETE_ARRAY(pBuffer);
	if (fpUiTex)
	{
		fclose(fpUiTex);
		fpUiTex = NULL;
	}
	return nResult;
}

int KTgaBlockLoader::ReadTextFromIcon(LPCTSTR pcszText, HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	std::vector<TXTFILEDATA> aTextCurrentVector;
	TCHAR szFile[MAX_PATH];
	TCHAR szCurrent[MAX_PATH];
	int i = 0;

	KG_PROCESS_ERROR(pcszText);

	nRetCode = KTgaManager::GetSelf().Clear();
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = KIconManager::GetSelf().Clear();
	KG_PROCESS_ERROR(nRetCode);

	aTextCurrentVector.clear();
	for (i = 0; i < (int)m_aNewIconTxtVector.size(); ++i)
	{
		_tcsncpy(szFile, pcszText, sizeof(szCurrent)/sizeof(TCHAR));
		_tcsncat(szFile, "\\",sizeof(szFile)/sizeof(TCHAR));
		_tcsncpy(szCurrent, m_aNewIconTxtVector[i].szTxtFile, sizeof(szCurrent)/sizeof(TCHAR));
		_tcsncat(szFile, szCurrent,sizeof(szFile)/sizeof(TCHAR));
		_tcsncpy(m_aNewIconTxtVector[i].szTxtFile, szFile, sizeof(m_aNewIconTxtVector[i].szTxtFile)/sizeof(TCHAR));
		aTextCurrentVector.push_back(m_aNewIconTxtVector[i]);
	}

	for (i = 0; i < (int)m_aNewIconTxtVector.size(); ++i)
	{
		nRetCode = KTgaBlockLoader::LoadUITex(
			aTextCurrentVector[i].szTxtFile,LOAD_AS_REPLACE, READ_TEXT, FIND_ICON_FILE,
			aTextCurrentVector[i].nTextNum,aTextCurrentVector[i].nFrameNum, hWnd);
		KG_PROCESS_ERROR(nRetCode);
	}
	nRetCode = KIconManager::GetSelf().UpDataListView();
    KG_PROCESS_ERROR(nRetCode);
	nResult	= true;
Exit0:
	return nResult;
}
