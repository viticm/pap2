// ExtractPack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KExtractUpdatePack.h"
#include "KGLog.h"
#include "ReadConfig.h"
#include "KExtractPak.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = false;
	int nResult = false;
	int nKGLogInitFlag = false;
	int nFileCount = 0;
	char *pPakPath = "E:\\PakExtract\\Pak\\update_2.pak";
	char *pExtractPath = "E:\\PakExtract\\Extract";
	ReadConfig config;
	KExtractUpdatePack pakOper;

	KExtractPak doExtractpak;


	KGLOG_PARAM KGLogParm = {"logs", "GameUpdater", KGLOG_OPTION_FILE, 1000 * 10};




	nRetCode = KGLogInit(KGLogParm, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

	/*config.Init();
	config.ReadConfigFile(COFING_FINE_NAME);
	
	pPakPath = config.GetPakPath();
	KGLOG_PROCESS_ERROR(pPakPath);

	pExtractPath = config.GetExtractPath();
	KGLOG_PROCESS_ERROR(pExtractPath);*/

	/*nResult = pakOper.Init(pPakPath, pExtractPath);
	KGLOG_PROCESS_ERROR(nResult);

	nFileCount = pakOper.GetFileCount();

	while (true)
	{
		nRetCode = pakOper.ExtractNext();
		if (nRetCode == EXTRACT_FINISH)
			break;
		KGLOG_PROCESS_ERROR(nRetCode == EXTRACT_OK);
	}*/

	doExtractpak.Init(pPakPath, pExtractPath);

	nResult = doExtractpak.Run();
	KGLOG_PROCESS_ERROR(nResult);

	nResult = true;
Exit0:
	if (nKGLogInitFlag)
	{
		KGLogUnInit(NULL);
        nKGLogInitFlag = false;
	}

	config.UnInit();
	return nResult ? 0 : 1;
}

