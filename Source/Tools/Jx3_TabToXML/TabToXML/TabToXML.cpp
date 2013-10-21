// TabToXML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileType.h"
#include "file.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include<conio.h>  
#include <direct.h>
using namespace std;
char g_szstr[2048] = {0};
int  g_nstrpos = 0;
char* ChangeForstr(char *pszstr)// <  将"<"替换成"&lt";
{
	char *pStart = NULL;
	int nLenstr = 0;
	pStart = (char*) memchr(pszstr,'<',strlen(pszstr));

	if (pStart)
	{
		memcpy(&g_szstr[g_nstrpos],pszstr,pStart - pszstr );
		g_nstrpos += (pStart - pszstr);
		g_szstr[g_nstrpos++] = '&';
		g_szstr[g_nstrpos++] = 'l';
		g_szstr[g_nstrpos++] = 't';
		g_szstr[g_nstrpos++] = ';';
		return ChangeForstr(++pStart);
	}
	else
	{
		nLenstr = strlen(pszstr);
		memcpy(&g_szstr[g_nstrpos],pszstr,nLenstr);
		g_nstrpos += nLenstr;
		g_szstr[g_nstrpos] = '\0';
		return g_szstr;
	}
	
}
char* ChangeForstr1(char *pszstr)// >  将"<"替换成"&gt";
{
	char *pStart = NULL;
	int nLenstr = 0;
	
	pStart = (char*) memchr(pszstr,'>',strlen(pszstr));

	if (pStart)
	{
		memcpy(&g_szstr[g_nstrpos],pszstr,pStart - pszstr );
		
		g_nstrpos += (pStart - pszstr);
		g_szstr[g_nstrpos++] = '&';
		g_szstr[g_nstrpos++] = 'g';
		g_szstr[g_nstrpos++] = 't';
		g_szstr[g_nstrpos++] = ';';
		return ChangeForstr1(++pStart);
	}
	else
	{
		nLenstr = strlen(pszstr);
		memcpy(&g_szstr[g_nstrpos],pszstr,nLenstr);
		g_nstrpos += nLenstr;
		g_szstr[g_nstrpos] = '\0';
		return g_szstr;
	}

}

char* ChangeForstr2(char *pszstr)// &  将"&"替换成"&amp;";
{
	char *pStart = NULL;
    int nLenstr = 0;

	pStart = (char*) memchr(pszstr,'&',strlen(pszstr));

	if (pStart)
	{
		memcpy(&g_szstr[g_nstrpos],pszstr,pStart - pszstr );

		g_nstrpos += (pStart - pszstr);
		g_szstr[g_nstrpos++] = '&';
		g_szstr[g_nstrpos++] = 'a';
		g_szstr[g_nstrpos++] = 'm';
		g_szstr[g_nstrpos++] = 'p';
		g_szstr[g_nstrpos++] = ';';
		return ChangeForstr2(++pStart);
	}
	else
	{
		nLenstr = strlen(pszstr);
		memcpy(&g_szstr[g_nstrpos],pszstr,nLenstr);
		g_nstrpos += nLenstr;
		g_szstr[g_nstrpos] = '\0';
		return g_szstr;
	}

}

char* ChangeForstr3(char *pszstr)// "  将"""替换成"&quot;";
{
	char *pStart = NULL;
    int nLenstr = 0;

	pStart = (char*) memchr(pszstr,'\"',strlen(pszstr));

	if (pStart)
	{
		memcpy(&g_szstr[g_nstrpos],pszstr,pStart - pszstr );

		g_nstrpos += (pStart - pszstr);
		g_szstr[g_nstrpos++] = '&';
		g_szstr[g_nstrpos++] = 'q';
		g_szstr[g_nstrpos++] = 'u';
		g_szstr[g_nstrpos++] = 'o';
		g_szstr[g_nstrpos++] = 't';
		g_szstr[g_nstrpos++] = ';';
		return ChangeForstr3(++pStart);
	}
	else
	{
		nLenstr = strlen(pszstr);
		memcpy(&g_szstr[g_nstrpos],pszstr,nLenstr);
		g_nstrpos += nLenstr;
		g_szstr[g_nstrpos] = '\0';
		return g_szstr;
	}

}

char* ChangeForstr4(char *pszstr)// '  将"'"替换成"&qpos;";
{
	char *pStart = NULL;
	int nLenstr = 0;

	pStart = (char*) memchr(pszstr, '\'', strlen(pszstr));

	if (pStart)
	{
		memcpy(&g_szstr[g_nstrpos], pszstr, pStart - pszstr );

		g_nstrpos += (pStart - pszstr);
		g_szstr[g_nstrpos++] = '&';
		g_szstr[g_nstrpos++] = 'a';
		g_szstr[g_nstrpos++] = 'p';
		g_szstr[g_nstrpos++] = 'o';
		g_szstr[g_nstrpos++] = 's';
		g_szstr[g_nstrpos++] = ';';
		return ChangeForstr4(++pStart);
	}
	else
	{
		nLenstr = strlen(pszstr);
		memcpy(&g_szstr[g_nstrpos], pszstr, nLenstr);
		g_nstrpos += nLenstr;
		g_szstr[g_nstrpos] = '\0';
		return g_szstr;
	}

}

int Clearglobal()
{
	memset(g_szstr,0,sizeof(g_szstr));
	g_nstrpos = 0;
	return 1;

}

char *ChangstrForXML(char *psztab)//最终生成五个字符的转换，“<”“>”“&”“"”“'”
{
	char *pszTem = NULL;
	char szTemstr[2048] = {0};
	int nstrLen = 0;
	Clearglobal();

	pszTem = ChangeForstr2(psztab);//先检查“&”;
	nstrLen = strlen(pszTem);
	memset(szTemstr, 0, sizeof(szTemstr));
	memcpy(szTemstr, pszTem, nstrLen);
	szTemstr[nstrLen] = '\0';
	Clearglobal();

	pszTem = ChangeForstr(szTemstr);
	nstrLen = strlen(pszTem);
	memcpy(szTemstr, pszTem, nstrLen);
	szTemstr[nstrLen] = '\0';
	Clearglobal();

	pszTem = ChangeForstr1(szTemstr);
	nstrLen = strlen(pszTem);
	memset(szTemstr, 0, sizeof(szTemstr));
	memcpy(szTemstr, pszTem, nstrLen);
	szTemstr[nstrLen] = '\0';
	Clearglobal();

	pszTem = ChangeForstr3(szTemstr);
	nstrLen = strlen(pszTem);
	memset(szTemstr, 0, sizeof(szTemstr));
	memcpy(szTemstr, pszTem, nstrLen);
	szTemstr[nstrLen] = '\0';
	Clearglobal();

	pszTem = ChangeForstr4(szTemstr);

	return pszTem;

}

char* FindDir(char *pszstr)// 查文件的目录，返回最后一个"\"的下一个位置
{
	char *pStart = NULL;
	if (pszstr == NULL)
	{
		return NULL;
	}
	pStart = (char*) memchr(pszstr, '\\', strlen(pszstr));

	if (pStart)
	{
		return FindDir(++pStart);
	}
	else
	{
		return pszstr;
	}

}
bool CheckIsAbsoluteDir(char *pszDir)
{
	char *pStart = NULL;
	if (pszDir == NULL)
	{
		return false;
	}
	pszDir++;
	pStart = (char*) memchr(pszDir, ':', strlen(pszDir));
	if (pStart)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int _tmain(int argc, char * argv[])
{
	
	if (argc != 3)
	{
		cout << "tab文件的输入,注意后缀名.tab的大小写且路径是绝对路径" << endl;
		cout << "xml文件的输入,注意后缀名.xml的大小写且路径是绝对路径" << endl;
		cout << "例子如下：" << endl;
		cout << "D:\src.tab D:\des.xml" << endl;
		cout << "参数个数不对，参数个数为3" << endl;
		return 1;
	}
	ITabFile*   piTabFile   = NULL;
    int nResult = 0;
	int nRecode = 0;
	bool bRecode = false;
	char szKeyName[1024] = {0};
	char szTabFileName[1024] = {0};
	char szXmlMainKey[1024] = {0};
	char szTemValue[2048] = {0};
	int nNameLen = 0;
	int nWidth = 0;
	int nHeight = 0;
	char szInTabFileName[1024] = {0};
	char szXmlOutFileName [1024] = {0};
	ofstream outfile;
	char *pszTem = NULL;
	char szXMLDir[1024] = {0};
	char *pszstrpos = NULL;
	char *pszXMLmainKey = NULL;
	char *pRelativeTabFileName = NULL;

	strcpy(szInTabFileName, argv[1]);

	bRecode = CheckIsAbsoluteDir(argv[1]);
	if (bRecode == false)
	{
		cout<< argv[1] << "不是绝对路径" << endl;
		KG_PROCESS_ERROR(bRecode);
	}

	bRecode = CheckIsAbsoluteDir(argv[2]);
	if (bRecode == false)
	{
		cout<< argv[2] << "不是绝对路径" << endl;
		KG_PROCESS_ERROR(bRecode);
	}

    nNameLen = strlen(szInTabFileName);
	if (nNameLen < 5)
	{
		cout << "tab,输入文件名错误\n" << endl;
		KG_PROCESS_ERROR(nRecode);
		
	}
	
	nResult = memcmp((const void*)&szInTabFileName[nNameLen-4], ".tab", 4);
	if (nResult != 0)
	{
		cout << "tab,输入文件名后缀错误,应该.tab" << endl;
		KG_PROCESS_ERROR(nRecode);
	}

	strcpy(szXmlOutFileName, argv[2]);
	nNameLen = strlen(szXmlOutFileName);
	if (nNameLen < 5)
	{
		cout << "Xml,输入文件名错误\n" << endl;
		KG_PROCESS_ERROR(nRecode);

	}

	nResult = memcmp((const void*)&szXmlOutFileName[nNameLen-4], ".xml", 4);

	if (nResult != 0)
	{
		cout << "Xml,输入文件名后缀错误，应该.xml" << endl;
		KG_PROCESS_ERROR(nRecode);
	}

	memcpy(szTabFileName, szInTabFileName, sizeof(szTabFileName));

	nNameLen = strlen(szTabFileName);
	memcpy(szXmlMainKey, szTabFileName, nNameLen - 4);
	szXmlMainKey[nNameLen - 4] = '\0';
	pszXMLmainKey = FindDir(szXmlMainKey);

//	g_SetRootPath();
	pRelativeTabFileName = FindDir(szInTabFileName);

	piTabFile = g_OpenTabFile(szTabFileName);
	if (piTabFile == NULL)
	{
		cout << "tab文件打开失败，请检查是否存在该文件" << szTabFileName << "或者是绝对路径写错" << endl;
		KG_PROCESS_ERROR(piTabFile);
	}
	
	pszstrpos = FindDir(szXmlOutFileName);
	if (pszstrpos == szXmlOutFileName)
	{
		memcpy(szXMLDir, szXmlOutFileName, strlen(szXmlOutFileName));
		nNameLen = strlen(szXmlOutFileName);
		szXMLDir[nNameLen] = '\0';
	}
	else
	{
		memcpy(szXMLDir, szXmlOutFileName, pszstrpos - szXmlOutFileName);
		nNameLen = strlen(szXmlOutFileName);
		szXMLDir[nNameLen] = '\0';

	}

	outfile.open(szXmlOutFileName);
	nResult = outfile.is_open();
	if (nResult == 0)
	{
		cout << "XML文件的目录文件夹" << szXMLDir << "不存在,请重新创建或者路径写错，请检查" <<  endl;
		KG_PROCESS_ERROR(nResult);
	}
	

	nWidth = piTabFile->GetWidth();
	nHeight = piTabFile->GetHeight();

	outfile << "<?xml version=\"1.0\" encoding=\"gbk\"?>" << '\n';
	outfile << '<' << strupr(pszXMLmainKey) << 'S' << '>' <<'\n';
	if (0 == strncmp("RelationForce.tab", pRelativeTabFileName, sizeof("RelationForce.tab")))
	{
		for ( int i = 2; i <= nHeight; i++)
		{
			outfile<< '\t' << '<' << strupr(pszXMLmainKey)  << '\n';
			for (int j = 1; j <= 2; j++)
			{
				piTabFile->GetString(1,j,"",szKeyName,sizeof(szKeyName));

				if (szKeyName[0] == NULL)
				{
					continue;
				}

				piTabFile->GetString(i,j,"",szTemValue,sizeof(szTemValue));

				pszTem = ChangstrForXML(szTemValue);

				if ( j == 1)
				{
					outfile << '\t' << '\t' <<  szKeyName << '='<< '\"' << pszTem << '\"'; //<< '\n';
				}
				else
				{
					outfile << ' ' <<  szKeyName << '='<< '\"' << pszTem << '\"';
				}
				Clearglobal();

			}
			outfile << "/>" << '\n';
		}
	}
	else
	{
		for ( int i = 2; i <= nHeight; i++)
		{
			outfile<< '\t' << '<' << strupr(pszXMLmainKey)  << '\n';
			for (int j = 1; j <= nWidth; j++)
			{
				piTabFile->GetString(1,j,"",szKeyName,sizeof(szKeyName));

				if (szKeyName[0] == NULL)
				{
					continue;
				}

				piTabFile->GetString(i,j,"",szTemValue,sizeof(szTemValue));

				pszTem = ChangstrForXML(szTemValue);

				if ( j == 1)
				{
					outfile << '\t' << '\t' <<  szKeyName << '='<< '\"' << pszTem << '\"'; //<< '\n';
				}
				else
				{
					outfile << ' ' <<  szKeyName << '='<< '\"' << pszTem << '\"';
				}
				Clearglobal();

			}
			outfile << "/>" << '\n';
		}
	}
	outfile << "</" << strupr(pszXMLmainKey) << 'S' << '>' <<'\n';

	nRecode = 1;

Exit0:
	if (nRecode == 0)
	{
		cout << "tab文件转换成XML失败" << endl;
	}
	else
	{
		outfile.close();
		cout << "转换成功" << endl;
	}
	
	return 0;
}

