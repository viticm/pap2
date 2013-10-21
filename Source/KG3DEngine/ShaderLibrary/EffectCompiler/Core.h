////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Core.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-3-12 15:02:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_CORE_H_
#define _INCLUDE_CORE_H_

////////////////////////////////////////////////////////////////////////////////

struct AppCmdParams 
{
	enum
	{
		em_run_mode_dialog,
		em_run_mode_console,
		em_run_mode_vc,
	};
	//BOOL	bShowDialog;
	INT		nRunMode;
	BOOL	bHasCmdFilePathsToBeCompiled;
	BOOL	bForceCompile;
	BOOL	bShowOnlyError;
	BOOL	bShowHelp;

	CString	strOutputDir;	//如果是空，则不输出
	CString strCopyToDir;	//如果是空，则不拷贝

	AppCmdParams()
		:nRunMode(em_run_mode_dialog)
		,bHasCmdFilePathsToBeCompiled(FALSE)
		,bForceCompile(FALSE)
		,bShowOnlyError(FALSE)
		,bShowHelp(FALSE)
	{
	}
};

extern AppCmdParams g_AppCmdParams;

struct ICFileEnumer		//用作文件输入
{
	enum em_get_file_ret
	{
		em_no_next_file = 0,
		em_file_name_not_correct,
		em_file_correct,
	};
	virtual em_get_file_ret GetFirstFile(WIN32_FIND_DATA* pfileData) = 0;
	virtual em_get_file_ret GetNextFile(WIN32_FIND_DATA* pfileData) = 0;
	virtual LPCTSTR FindFileAndGetFullPath() = 0;
	virtual UINT GetFileCount() = 0;
	virtual ~ICFileEnumer() = 0{}
};

struct COutputItem	//输出消息的结构
{
	enum
	{
		em_out_file_result,
		em_out_statistic,	//统计信息和下面的notice等，只有strCompileResult成员有用
		em_out_notice,
		em_out_count,
	};

	CString	strResult[3];

	CString& GetFileName(){return strResult[0];}
	const CString& GetFileName()const {return strResult[0];}

	CString& GetCompileResult(){return strResult[1];}
	const CString& GetCompileResult()const {return strResult[1];}

	CString& GetFullPath(){return strResult[2];}
	const CString& GetFullPath()const {return strResult[2];}

	
	BOOL	bCompileSuccess;
	DWORD	dwOutputCategory;	//是上面的枚举值

	static VOID GetElementDesc(LPCTSTR** pItemDescArray, size_t* pDescCount);	//这个是指向字符串的数组的指针的意思（用指针的指针代替数组的指针的语义，不然更难看）;

	static VOID CreateInfo(COutputItem* pOutItem, DWORD dwCategory, LPCTSTR strString);
	static VOID CreateAppError(COutputItem* pOutItem, LPCTSTR strErrorResult);
};

struct ICCompileResultOutput	//输出消息的接口
{
	virtual VOID SetOnlyOutputError(BOOL bOnlyOutputError) = 0;
	virtual VOID Output(const COutputItem& outputItem) = 0;
	virtual VOID OutputPercent(FLOAT fPercent) = 0;	//这个可以写成空函数
	virtual ~ICCompileResultOutput() = 0{}
};

struct ICCompileFileController	//输出编译好的文件的接口
{
	virtual BOOL IsFileUnChanged(LPCTSTR fileName, const FILETIME& ftLastWriteTime) = 0;
	virtual BOOL SaveFileOutput(LPCTSTR strFileName, const FILETIME& ftLastWriteTime, LPD3DXEFFECT pEffect, BOOL bIsIncludedFile) = 0;
	virtual BOOL CopyFileToDes(LPCTSTR fileFullPath, BOOL* pSkipped) = 0;
	virtual VOID SetPaths(LPCTSTR strOutputDir, LPCTSTR strCopyToDir) = 0;	//可能为NULL的，如果为NULL就不启动对应的功能
	virtual VOID SetForceRebuild(BOOL bForceRebuild) = 0;

	virtual ~ICCompileFileController() = 0{}

	BOOL CopyFileToDesDefaultImp(LPCTSTR fileFullPath, LPCTSTR copyToFullPath);
};

class Core	//核心算法
{
public:
	enum
	{
		em_file_limit = 100,
		em_file_max_size = 1000000,//1M最大，大于这个的就不做头文件分析了
	};

	static LPCTSTR FindFileAndGetFullPath(LPCTSTR strFileName, CString* pRetString);	//返回pRetString
	static ICCompileResultOutput& GetDefaultOutput();
	static void CompileShaders(ICFileEnumer& fileEnumer, ICCompileResultOutput& outDevice, ICCompileFileController& fileController
		, LPCTSTR strOutputDir, LPCTSTR strCopytoDir, BOOL bForceRebuild, BOOL bShowOnlyError);
private:
	static HRESULT CreateDevice( LPDIRECT3D9 &pDeviceCore, HWND hWndOut, LPDIRECT3DDEVICE9& pDevice);
};


#endif //_INCLUDE_CORE_H_
