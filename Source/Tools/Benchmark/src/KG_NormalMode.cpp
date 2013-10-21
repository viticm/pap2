////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_NormalMode.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-12 17:50:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <list>
#include <string>
#include <map>
#include "KG_OptionDialog.h"
#include "KG_NormalMode.h"
#include "KG_RecPlayer.h"
#include "KG_InfoSender.h"
#include "IEKG3DRecPlayer.h"
#include "KG_Benchmark.h"
////////////////////////////////////////////////////////////////////////////////

class KG_NormalModeNotify : public IKG_RecPlayerNotify
{
public:
    KG_NormalModeNotify(void);
    ~KG_NormalModeNotify(void);

    virtual int OnPlayBegin(IKG3DEngineManager *piEngineManager, const char cszRecFileName[]);
    virtual int OnPlayEnd(IKG3DEngineManager *piEngineManager);
    virtual int OnFrameMove(IKG3DEngineManager *piEngineManager);

    //typedef std::vector<string> SAMPLE_FILE_LIST;

    //int GetSampleFileList(SAMPLE_FILE_LIST *pSampleFileList)  const;
    int GetScore(DWORD *pdwScore)  const;
	int GetMaxFps(DWORD *pdwMaxScore) const;
	int GetMinFps(DWORD *pdwMinScore) const;
private:
    //struct KG_SAMPLE_INFO
    //{
    //    DWORD dwFPS;
    //    DWORD dwCpuUsage;
    //    DWORD dwVGAMemUsed;
    //    DWORD dwVGAMemFree;
    //    DWORD dwAGPMemUsed;
    //    DWORD dwAGPMemFree;
    //    DWORD dwReadCount;
    //    DWORD dwWriteCount;
    //    DWORD dwReadKBytes;
    //    DWORD dwWriteKBytes;
    //};

   // std::string m_strRecFileName;

    //SAMPLE_FILE_LIST m_SampleFileList;

    //typedef std::list<KG_SAMPLE_INFO> SAMPLE_INFO_LIST;
   // SAMPLE_INFO_LIST m_SampleInfoList;

    ULARGE_INTEGER m_LastSystimeTime;
    ULARGE_INTEGER m_LastKernelTime;
    ULARGE_INTEGER m_LastUserTime;
    IO_COUNTERS m_LastIOCounters;

    DWORD m_dwRenderFrameCount;
    DWORD m_dwTotalFrameCount;
    DWORD m_dwStartTime;
    DWORD m_dwTotalCostTime;
	DWORD m_dwMaxFps;
	DWORD m_dwMinFps;
};
//////////////////////////////////////////////////////////////////////////////


KG_NormalModeNotify::KG_NormalModeNotify(void)
{
    m_dwTotalFrameCount = 0;
    m_dwTotalCostTime = 0;
	m_dwMaxFps = 0;
	m_dwMinFps = 0;
}

KG_NormalModeNotify::~KG_NormalModeNotify(void)
{
}
////////////////////////////////////////////////////////////////////////////////
int KG_NormalModeNotify::OnPlayBegin(IKG3DEngineManager *piEngineManager, const char cszRecFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    HANDLE  hProcessHandle;
    SYSTEMTIME SystemTime;
    FILETIME CreateTime;
    FILETIME ExitTime;

    KGLOG_PROCESS_ERROR(piEngineManager);

    //m_strRecFileName = cszRecFileName;

    m_dwRenderFrameCount = 0;
    //m_SampleInfoList.clear();

    GetSystemTime(&SystemTime);

    nRetCode = SystemTimeToFileTime(&SystemTime, (LPFILETIME)&m_LastSystimeTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    hProcessHandle = GetCurrentProcess();
    //KGLOG_PROCESS_ERROR(hProcessHandle != INVALID_HANDLE_VALUE);

    nRetCode = GetProcessTimes(hProcessHandle, &CreateTime, &ExitTime, (LPFILETIME)&m_LastKernelTime, (LPFILETIME)&m_LastUserTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetProcessIoCounters(hProcessHandle, &m_LastIOCounters);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_NormalModeNotify::OnPlayEnd(IKG3DEngineManager *piEngineManager)
{
    int nResult  = false;
    int nRetCode = false;
    //HANDLE hFile = INVALID_HANDLE_VALUE;
    //DWORD dwBytesWrite = 0;
    //string strSampleFileName;
    //string strSampleInfo;
    ////SAMPLE_INFO_LIST::iterator itIndex, itEnd;
    //char szLine[512] = {0};
    //char szRecFileMD5[33] = {0};
    //char szConfigFile[] = "config.ini";
    //DWORD dwFileSize = 0;
    //DWORD dwReadBytes = 0;
    //BYTE *pbyBuffer = NULL;
    //char szEngineVersion[33] = {0};
    DWORD dwEndTime = 0;
    //DWORD dwScore = 0;
    //char szScore[9] = {0};

    KGLOG_PROCESS_ERROR(piEngineManager);

    m_dwTotalFrameCount += m_dwRenderFrameCount;

    dwEndTime = ::GetTickCount();
    m_dwTotalCostTime += (dwEndTime - m_dwStartTime);

 //   KG_PROCESS_SUCCESS(m_SampleInfoList.empty());

 //   nRetCode = GetEngineVersion(szEngineVersion, sizeof(szEngineVersion));
 //   KGLOG_PROCESS_ERROR(nRetCode);
 //   szEngineVersion[sizeof(szEngineVersion) - 1] = '\0';

 //   nRetCode = (int)strlen(szEngineVersion);
 //   strSampleInfo.append(szEngineVersion);
 //   strSampleInfo.append(32 - nRetCode, ' ');

 //   nRetCode = GetFileMD5(szRecFileMD5, sizeof(szRecFileMD5), m_strRecFileName.c_str());
 //   KGLOG_PROCESS_ERROR(nRetCode);
 //   szRecFileMD5[sizeof(szRecFileMD5) - 1] = '\0';

 //   nRetCode = (int)strlen(szRecFileMD5);
 //   strSampleInfo.append(szRecFileMD5);
 //   strSampleInfo.append(32 - nRetCode, ' ');

 //   ASSERT(dwEndTime != m_dwStartTime);

 //   dwScore = (DWORD)(m_dwRenderFrameCount * 100.0f * 1000 / (dwEndTime - m_dwStartTime));
 //   
 //   nRetCode = sprintf(szScore, "%d", dwScore);
 //   KGLOG_PROCESS_ERROR(nRetCode > 0);
	//szScore[sizeof(szScore) - 1] = '\0';
	//
 //   nRetCode = (int)strlen(szScore);
 //   strSampleInfo.append(szScore);
 //   strSampleInfo.append(8 - nRetCode, ' ');

 //   hFile = ::CreateFile(
 //       szConfigFile,
 //       GENERIC_READ,
 //       FILE_SHARE_READ,
 //       NULL,
 //       OPEN_EXISTING,
 //       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
 //       NULL
 //   );
 //   KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

 //   dwFileSize = ::GetFileSize(hFile, NULL);

 //   pbyBuffer = new BYTE[dwFileSize];
 //   ZeroMemory(pbyBuffer, dwFileSize);
 //   
 //   nRetCode = ::ReadFile(hFile, pbyBuffer, dwFileSize, &dwReadBytes, NULL);
 //   KGLOG_PROCESS_ERROR(nRetCode);
 //   KGLOG_PROCESS_ERROR(dwFileSize == dwReadBytes);

 //   nRetCode = dwFileSize > (5 * 1024) ? (5 * 1024) : dwFileSize;

 //   strSampleInfo.append((const char *)pbyBuffer, nRetCode);

 //   strSampleInfo.append(5 * 1024 - nRetCode, ' ');

 //   CloseHandle(hFile);
 //   hFile = INVALID_HANDLE_VALUE;

 //   strSampleInfo.append(
 //       "FPS\tCpuUsage\t"
 //       "VGAMemoryUsed\tVGAMemoryFree\t"
 //       "AGPMemoryUsed\tAGPMemoryFree\t"
 //       "ReadCount\tWriteCount\t"
 //       "ReadKBytes\tWriteKBytes\r\n"
 //   );

 //   itEnd = m_SampleInfoList.end();
 //   for (itIndex = m_SampleInfoList.begin(); itIndex != itEnd; itIndex++)
 //   {
 //       nRetCode = sprintf(
 //           szLine,
 //           "%d\t%d\t"
 //           "%d\t%d\t"
 //           "%d\t%d\t"
 //           "%d\t%d\t"
 //           "%d\t%d\r\n",
 //           (*itIndex).dwFPS, (*itIndex).dwCpuUsage,
 //           (*itIndex).dwVGAMemUsed, (*itIndex).dwVGAMemFree,
 //           (*itIndex).dwAGPMemUsed, (*itIndex).dwAGPMemFree,
 //           (*itIndex).dwReadCount, (*itIndex).dwWriteCount,
 //           (*itIndex).dwReadKBytes, (*itIndex).dwWriteKBytes
 //       );
 //       KGLOG_PROCESS_ERROR(nRetCode > 0);

 //       strSampleInfo.append(szLine);
 //   }

 //   strSampleFileName = m_strRecFileName + "_123.sample";

 //   hFile = CreateFile(
 //       strSampleFileName.c_str(), 
 //       GENERIC_WRITE, 
 //       FILE_SHARE_WRITE, 
 //       NULL, 
 //       CREATE_ALWAYS, 
 //       FILE_ATTRIBUTE_NORMAL, 
 //       NULL
 //   );
 //   KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

 //   nRetCode = WriteFile(hFile, (LPCVOID)strSampleInfo.c_str(), (DWORD)strSampleInfo.length(), &dwBytesWrite, NULL);
 //   KGLOG_PROCESS_ERROR(nRetCode);

 //   CloseHandle(hFile);
 //   hFile = INVALID_HANDLE_VALUE;

 //   m_SampleFileList.push_back(strSampleFileName);

//Exit1:
    nResult = true;
Exit0:

    //KG_DELETE_ARRAY(pbyBuffer);

    //if (hFile != INVALID_HANDLE_VALUE)
    //{
    //    CloseHandle(hFile);
    //    hFile = INVALID_HANDLE_VALUE;
    //}

    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_NormalModeNotify::OnFrameMove(IKG3DEngineManager *piEngineManager)
{
    int nResult  = false;
    int nRetCode = false;
    HANDLE  hProcessHandle;
    SYSTEMTIME SystemTime;
    ULARGE_INTEGER CurrentSystemTime;
    FILETIME CreateTime;
    FILETIME ExitTime;
    ULARGE_INTEGER KernelTime;
    ULARGE_INTEGER UserTime;
    DWORD dwKernelTime = 0;
    DWORD dwUserTime = 0;
    IKG3DUI *pi3DUI = NULL;
    VideoMemoryInfo Info;
    //KG_SAMPLE_INFO SampleInfo;
    IO_COUNTERS CurrentIOCounters;
    float fSecond = 0;

	DWORD dwFPS = 0;

    KGLOG_PROCESS_ERROR(piEngineManager);

    if (!m_dwRenderFrameCount)
    {
        m_dwStartTime = ::GetTickCount();
    }

    m_dwRenderFrameCount ++;
    
    ZeroMemory(&Info, sizeof(Info));

    GetSystemTime(&SystemTime);

    nRetCode = SystemTimeToFileTime(&SystemTime, (LPFILETIME)&CurrentSystemTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    hProcessHandle = GetCurrentProcess();
    //KGLOG_PROCESS_ERROR(hProcessHandle != INVALID_HANDLE_VALUE);

    nRetCode = GetProcessTimes(
        hProcessHandle, 
        (LPFILETIME)&CreateTime, 
        (LPFILETIME)&ExitTime, 
        (LPFILETIME)&KernelTime, 
        (LPFILETIME)&UserTime
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    dwKernelTime = (DWORD)(KernelTime.QuadPart - m_LastKernelTime.QuadPart);
    dwUserTime = (DWORD)(UserTime.QuadPart - m_LastUserTime.QuadPart);

    fSecond = (float)(CurrentSystemTime.QuadPart - m_LastSystimeTime.QuadPart) / 10000000;

    //SampleInfo.dwFPS = (DWORD)(64 / fSecond);
    //SampleInfo.dwCpuUsage = (DWORD)((dwKernelTime + dwUserTime) * 100 / (CurrentSystemTime.QuadPart - m_LastSystimeTime.QuadPart));
	dwFPS = (DWORD)(1 / fSecond);
	if (dwFPS > m_dwMaxFps)
		m_dwMaxFps = dwFPS;
	if (dwFPS < m_dwMinFps || m_dwMinFps == 0)
		m_dwMinFps = dwFPS;

    m_LastSystimeTime = CurrentSystemTime;
    m_LastKernelTime = KernelTime;
    m_LastUserTime = UserTime;

    //pi3DUI =  piEngineManager->Get3DUI();
    //Info = pi3DUI->GetVideoMemeryInfo();

    //SampleInfo.dwVGAMemUsed = (Info.dwLocalTatal - Info.dwLocalFree) / 1024 / 1024;
    //SampleInfo.dwVGAMemFree = Info.dwLocalFree / 1024 / 1024;
    //SampleInfo.dwAGPMemUsed = (Info.dwNonLocalTatal - Info.dwNonLocalFree) / 1024 / 1024;
    //SampleInfo.dwAGPMemFree = Info.dwNonLocalFree / 1024 / 1024;

	if (!m_dwRenderFrameCount || (m_dwRenderFrameCount % 64))
		goto Exit1;

    nRetCode = GetProcessIoCounters(hProcessHandle, &CurrentIOCounters);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    //SampleInfo.dwReadCount = (DWORD)(CurrentIOCounters.ReadOperationCount - m_LastIOCounters.ReadOperationCount);
    //SampleInfo.dwWriteCount = (DWORD)(CurrentIOCounters.WriteOperationCount - m_LastIOCounters.WriteOperationCount);
    //SampleInfo.dwReadKBytes = 
    //    (DWORD)(CurrentIOCounters.ReadTransferCount - m_LastIOCounters.ReadTransferCount) / 1024;
    //SampleInfo.dwWriteKBytes = 
    //    (DWORD)(CurrentIOCounters.WriteTransferCount - m_LastIOCounters.WriteTransferCount) / 1024;

    m_LastIOCounters = CurrentIOCounters;

    //m_SampleInfoList.push_back(SampleInfo);
    
Exit1:
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
//int KG_NormalModeNotify::GetSampleFileList(SAMPLE_FILE_LIST *pSampleFileList) const
//{
//    *pSampleFileList = m_SampleFileList;
//
//    return true;
//}
//////////////////////////////////////////////////////////////////////////
int KG_NormalModeNotify::GetScore(DWORD *pdwScore) const
{
    *pdwScore = (DWORD)(m_dwTotalFrameCount * 1.0F / (m_dwTotalCostTime / 1000));

    return true;
}


int KG_NormalModeNotify::GetMaxFps(DWORD *pdwMaxScore) const
{
	*pdwMaxScore = m_dwMaxFps;
	return true;
}
int KG_NormalModeNotify::GetMinFps(DWORD *pdwMinScore) const
{
	*pdwMinScore = m_dwMinFps;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
int _ShowScore(const KG_NormalModeNotify &crNotify, const KG_RecPlayer &crPlayer)
{
    int nResult  = false;
    int nRetCode = false;
    char szMessage[256];
    const char *pcszFormat = NULL;
    DWORD dwScore = 0,dwMaxFps = 0,dwMinFps = 0;
    HWND hWnd = NULL;

 //   nRetCode = crNotify.GetScore(&dwScore);
 //   KGLOG_PROCESS_ERROR(nRetCode);

	//nRetCode = crNotify.GetMaxFps(&dwMaxFps);
 //   KGLOG_PROCESS_ERROR(nRetCode);

	//nRetCode = crNotify.GetMinFps(&dwMinFps);
	//KGLOG_PROCESS_ERROR(nRetCode);
	IERecPlayerInfo infoResult;
	infoResult = crPlayer.GetInfo();
	dwMaxFps = infoResult.nMaxFps;
	dwMinFps = infoResult.nMinFps;
	dwScore = infoResult.nAvgFPS;

    //if (dwScore < 1499)
    //{
    //    pcszFormat = 
    //        "您的得分为：%d分,   3000分以上才表示您能正常运行。\r\n"
    //        "在此设置下，您的体验可能很差。您可以在调整窗口大小或更改设置后，再次尝试运行。";
    //}
    //else if (dwScore < 2999)
    //{
    //    pcszFormat = 
    //        "您的得分为：%d分，  得分3000以上表示您能正常运行。\r\n"
    //        "在此设置下，您能基本运行。如果您想获得更好的体验，请尝试更改为较低的设置。";
    //}
    //else if (dwScore < 4999)
    //{
    //    pcszFormat = 
    //        "您的得分为：%d分\r\n"
    //        "恭喜您，在此设置下可以流畅运行，您可以获得舒适的体验。";
    //}
    //else
    //{
    //    pcszFormat = 
    //        "您的得分为：%d分\r\n"
    //        "非常棒，在此设置下您将获得最好的体验。";
    //}
	pcszFormat = "      FPS INFO:"
				 "    AVG: %d"
				 "    MAX: %d"
				 "    MIN: %d";


    nRetCode = snprintf(szMessage, sizeof(szMessage), pcszFormat, dwScore,dwMaxFps,dwMinFps);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szMessage));
    szMessage[sizeof(szMessage) - 1] = '\0';

    hWnd = crPlayer.GetWindow();
    //KGLOG_PROCESS_ERROR(hWnd);

    nRetCode = ::MessageBox(hWnd, szMessage, "Info", MB_ICONINFORMATION);
    //KGLOG_PROCESS_ERROR(nRetCode == IDYES);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int RunAsNormalMode()
{
    int nResult  = false;
    int nRet = false;

    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindData;
    KG_NormalModeNotify Notify;
    IIniFile* piFile = NULL;
    KG_RecPlayer Player;
    int nPlayerInitFlag = false;
    int nX = 0;
    int nY = 0;
    int nWidth  = 0;
    int nHeight = 0;
    int nFullScreen = 0;
	
 //   KG_NormalModeNotify::SAMPLE_FILE_LIST SampleFileList;
  
	const std::string strDir = "DebugFiles/BenchMark/";
	LPCTSTR strExt = "rec";

    nRet = ShowOptionDialog();
    KGLOG_PROCESS_ERROR(nRet);

    piFile = g_OpenIniFile("config.ini");
    KGLOG_PROCESS_ERROR(piFile);

    piFile->GetInteger("Main", "X", 0, &nX);
    piFile->GetInteger("Main", "Y", 0, &nY);
    piFile->GetInteger("Main", "CanvasWidth",  1024, &nWidth);
    piFile->GetInteger("Main", "CanvasHeight", 768,  &nHeight);
    piFile->GetInteger("Main", "FullScreen",   0,    &nFullScreen);

    nRet = Player.Init(nX, nY, nWidth, nHeight, nFullScreen);
    KGLOG_PROCESS_ERROR(nRet);
    nPlayerInitFlag = true;

    // ---------------- player all file in current directory ------------------
  
    if(g_benchMarkOption.bPlayAll)
    {
        TCHAR strRecordFilePath[MAX_PATH];
        {
            TCHAR strForFind[MAX_PATH] = ("");
            nRet = _stprintf_s(strForFind, _T("%s*.%s"), strDir.c_str(), strExt);
            KG_PROCESS_ERROR(nRet > 0);
            hFindFile = ::FindFirstFile(strForFind, &FindData);
        }
        if (hFindFile == INVALID_HANDLE_VALUE)
        {
            MessageBox(NULL, "Can't find any record file (*.rec)!", "Error", MB_OK | MB_ICONSTOP);
            KG_PROCESS_ERROR(false);
        }
        nRet = _stprintf_s(strRecordFilePath,"%s%s",strDir.c_str(), FindData.cFileName);
        KG_PROCESS_ERROR(nRet);

        nRet = Player.Play(strRecordFilePath, &Notify);
        while (FindNextFile(hFindFile, &FindData) != 0)
        {
            nRet = _stprintf_s(strRecordFilePath,_T("%s%s"),strDir.c_str(),FindData.cFileName);
            KG_PROCESS_ERROR(nRet);
            nRet = Player.Play(strRecordFilePath, &Notify);
        }
    }
    else 
	{
		TCHAR strRecordFilePath[MAX_PATH];
		if (g_benchMarkOption.bPlayNoramlTest)
		{
			nRet = _stprintf_s(strRecordFilePath, _T("%s%s"), strDir.c_str(), g_benchMarkOption.strNormalRecFile);
			KG_PROCESS_ERROR(nRet);
			nRet = Player.Play(strRecordFilePath, &Notify);
		}
		if (g_benchMarkOption.bPlayCudaTest)
		{		
			nRet = _stprintf_s(strRecordFilePath, _T("%s%s"), strDir.c_str(), g_benchMarkOption.strCudaRecFile);
			KG_PROCESS_ERROR(nRet);
			nRet = Player.Play(strRecordFilePath, &Notify);
		}
	}

  //  nRet = _ShowScore(Notify, Player);
    //KGLOG_PROCESS_ERROR(nRetCode);

    //nRetCode = Notify.GetSampleFileList(&SampleFileList);
    //KGLOG_PROCESS_ERROR(nRetCode);

    //for (unsigned uIndex = 0; uIndex < SampleFileList.size(); ++uIndex)
    //{
    //    nRetCode = SendSampleFile(SampleFileList[uIndex].c_str());
    //    //KGLOG_PROCESS_ERROR(nRetCode);
    //}

    nResult = true;
Exit0:

    //for (unsigned uIndex = 0; uIndex < SampleFileList.size(); ++uIndex)
    //{
    //    nRetCode = ::DeleteFile(SampleFileList[uIndex].c_str());
    //}

    if (hFindFile)
    {
        ::FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
    if (nPlayerInitFlag)
    {
        Player.UnInit();
        nPlayerInitFlag = false;
    }
    KG_COM_RELEASE(piFile);
    return nResult;
}
