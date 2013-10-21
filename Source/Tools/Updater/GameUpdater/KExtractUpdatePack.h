//////////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    :   KExtractUpdatePack.h
//  Creater     :   ZhongGuangZe
//  Date        :   2008-06-10
//  Comment     :   解压更新包并更新客户端
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KEXTRACT_PACKAGE_H_
#define _KEXTRACT_PACKAGE_H_ 

#include <vector>
#include "unzip.h"

#define PAK_LIST_LMIT 128

using std::vector;

enum EXTRACT_PACK_RESULT
{
	EXTRACT_OK,
	EXTRACT_FAILD,
	EXTRACT_FINISH
};

class KExtractUpdatePack
{
public:
	KExtractUpdatePack();
	~KExtractUpdatePack() {UnInit();}

public:
	int          Init(const TCHAR szPackFileName[], const TCHAR szExtractPath[]);
	void         UnInit();

	EXTRACT_PACK_RESULT  ExtractNext();
	int          ExtractSingleFile(const TCHAR szZipName[], const TCHAR szFileName[], const TCHAR szOutPath[]);

	const TCHAR* GetCurrentFileName() {return m_szCurrentFile;}
	int          GetFileCount() {return m_nZipTotalFiles;}
	void         SetPackSize(unsigned int uSize) {m_uPackSize = uSize;} //自动化测试用

private:
	int _ExtractFileToPathFile(int nItemIndex, const TCHAR cszFullFileName[]);
	int _DisposePackFile(int nItemIndex);
	int _IsPackFileSuffix(const TCHAR szFileName[]);
	int _ReadPackageini();
	int _SavePackageList();
	int _AddUpdatePack();
	int _CheckPack(const TCHAR szDesPack[], const TCHAR szAddPack[]);  //检查PACK包是否大于2G。或者2个PACK包内文件数加起来超过20W
	int _ExceptFile(const TCHAR szFileName[]);
	int _MargePackFile(const TCHAR szSrcPack[], const TCHAR szDesPack[]);

	int _GetFileNameLoaction(const TCHAR szFile[], int* nLocation);
	int _FileIsExist(const TCHAR szFilePath[]);
	int _GetFileSize(const TCHAR szFileName[], DWORD* dwRetSize);

    int _CreateEmptyFile(const TCHAR cszFullFileName[]);
    int _GetParentDirectory(const TCHAR cszFullName[], tstring *pstrRetParentDirectory);
    int _EnsureDirectory(const TCHAR cszDirectoryName[]);
private:	
	TCHAR*    m_pszPackageList[PAK_LIST_LMIT];
	int       m_nPackageCount;
	int		  m_nZipTotalFiles;				//文件总数
	int		  m_nZipItemIndex;				//当前正在解压缩的文件的索引
	int		  m_nVersionCfgIndex;
	HZIP	  m_pZip;
	unsigned  m_uPackSize;                  //分包的文件大小

	TCHAR	  m_szPackFileName[MAX_PATH];	//压缩包路径
	TCHAR	  m_szExtractPath[MAX_PATH];	//解压缩路径
	TCHAR     m_szCurrentFile[MAX_PATH];    //解压的当前文件名
	TCHAR     m_szPakPath[MAX_PATH];
};

#endif /* _KEXTRACT_PACKAGE_H_ */