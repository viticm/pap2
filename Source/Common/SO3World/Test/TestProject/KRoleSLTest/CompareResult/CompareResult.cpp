// CompareResult.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
int Compare(const char* szFile1, const char* szFile2)
{
    int                         nResult         = -1;
    int                         nRetCode        = 0;
    FILE                        *fp1            = NULL;
    FILE                        *fp2            = NULL;

    fp1 = fopen(szFile1, "rb");
    KG_PROCESS_ERROR(fp1);

    fp2 = fopen(szFile2, "rb");
    KG_PROCESS_ERROR(fp2);

    fseek(fp1, 0, SEEK_END);   
    fseek(fp2, 0, SEEK_END); 
    
    if(ftell(fp1) != ftell(fp2))
    {
        nResult = 1;
        KG_PROCESS_ERROR(false);
    }

    fseek(fp1, 0, SEEK_SET);   
    fseek(fp2, 0, SEEK_SET);   

    while(!feof(fp1) && !feof(fp2))   
    {   
        if(fgetc(fp1) != fgetc(fp2))
        {
            nResult = 1;
            break;
        }
    }   
    
    if(feof(fp1) && feof(fp2) && nResult != 1) 
    {
        nResult = 0;
    }
    else
    {
        nResult = 1;
    }

Exit0:

    if (fp1)
    {
        fclose(fp1);
        fp1 = NULL;
    }
    if (fp2)
    {
        fclose(fp2);
        fp2 = NULL;
    }

    return nResult;
}


int _tmain(int argc, _TCHAR* argv[])
{
    bool                        bResult      = false;
    char                        szLoadServer[100];
    char                        szSaveServer[100];
    char                        szLoadCenter[100];
    char                        szSaveCenter[100];
    int                         szTest[] = {16384, 32767, 32768, 32769, 49152, 65536, 262143, 262144, 262145};
    FILE*                       fpResult = NULL;
    
    fpResult = fopen("savelog/Result.txt", "w");
    KG_PROCESS_ERROR(fpResult);
    
    for(int nCounter = 0; nCounter < 9; nCounter++)
    {
        snprintf(szLoadServer, sizeof(szLoadServer), "savelog/Load_Server%d", szTest[nCounter]);
        szLoadServer[sizeof(szLoadServer) - 1] = '\0';

        snprintf(szSaveServer, sizeof(szSaveServer), "savelog/Save_Server%d", szTest[nCounter]);
        szSaveServer[sizeof(szSaveServer) - 1] = '\0';

        snprintf(szLoadCenter, sizeof(szLoadCenter), "savelog/Load_Center%d", szTest[nCounter]);
        szLoadCenter[sizeof(szLoadCenter) - 1] = '\0';

        snprintf(szSaveCenter, sizeof(szSaveCenter), "savelog/Save_Center%d", szTest[nCounter]);
        szSaveCenter[sizeof(szSaveCenter) - 1] = '\0';
        
        if (0 == Compare(szSaveCenter, szSaveServer))
        {
            fprintf(fpResult, "存储用例中%d字节用例测试通过。\n", szTest[nCounter]);
        }
        else
        {
            fprintf(fpResult, "存储用例中%d字节用例测试失败。\n", szTest[nCounter]);
        }

        if (0 == Compare(szLoadServer, szLoadCenter))
        {
            fprintf(fpResult, "读取用例中%d字节用例测试通过。\n", szTest[nCounter]);
        }
        else
        {
            fprintf(fpResult, "读取用例中%d字节用例测试失败。\n", szTest[nCounter]);
        }
        remove(szSaveCenter);
        remove(szSaveServer);
        remove(szLoadServer);
        remove(szLoadCenter);
    }

    bResult = true;
Exit0:
    if (fpResult)
    {
        fclose(fpResult);
        fpResult = NULL;
    }

	return 0;
}

