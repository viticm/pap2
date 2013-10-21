/*****************************************************************************************
//	包文件的生成、展开、修改、合并等操作
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
------------------------------------------------------------------------------------------
   对打包文件进行操作的类，操作包括：生成、展开、修改、合并...等等
*****************************************************************************************/
#ifndef _ENGINE_PACK_FILE_MANAGER_H_
#define _ENGINE_PACK_FILE_MANAGER_H_

#include "PackDef.h"
#include "KPackFilePartner.h"
#include "File.h"

class KPackFileManager : public IPackFileShell
{
public:
	KPackFileManager();
	//销毁接口对象
	void	Release();
	//设置子文件的根目录
	void	SetElemFileRootPath(const char* pPath);
	//创建/打开的一个打包文件，返回打包文件索引，返回0值表示操作失败。
	int		CreatePack(const char* pszFile, int bOpenExist, int bExcludeOfCheckId);
	//关闭打包文件
	void	ClosePack(int nPakIndex);
	//往打包文件中添加一个目录
	bool	AddFolderToPak(int nPakIndex, const char* pFolder, fnAddFolderToPakCallback pCallback);
	//往打包文件中添加一个子文件
	bool	AddElemToPak(int nPakIndex, const char* pElemFile);
	//删除打包文件中的一个子文件
	int		DeleteElemInPak(int nPakIndex, const char* pElemFile);
	//获取子文件信息
	bool	GetElemInfo(const char* pElemName, ELEM_FILE_INFO& info);
	//获取子文件信息
	bool	GetElemInfo(unsigned int uElemId, ELEM_FILE_INFO& info);
	//从包中解出某个文件
	bool	UnpackElem(int nPakIndex, const char* pElemName, const char* pDestName);
	//从包中解出某个文件
	bool	UnpackElemByID(int nPakIndex, unsigned int uElemId, const char* pDestName);
	//从包中解出某个文件
	bool	UnpackElemByIndex(int nPakIndex, unsigned int uElemIndex, const char* pDestName);
	//从包中解出全部文件
	//参数nUnpackCount用于传出结果解了多少个文件
	//参数pFileNamePrefix表示子文件名的前缀必须与此给定的字符串完全一致（不区分大小写），传入空指针表示无前缀限制
	bool	UnpackAll(int nPakIndex, int& nUnpackCount, const char* pDestPath, const char* pFileNamePrefix = NULL);
	//得到包中子文件的数目
	int		GetElemCountInPak(int nPakIndex);
	//设置接口操作参数
	void	SetOption(IPACK_FILE_SHELL_OPTION eOption, int nValue);
	//扫描整个包文件集合，对于每个子文件给出进行一次回调函数操作
	//参数pFileNamePrefix表示遍历到的子文件名的前缀必须与此给定的字符串完全一致（不区分大小写），前缀不符的子文件将被略过，传入空指针表示无前缀限制。
	//返回值表示多少个满足条件的子文件被扫描到
	bool	ScanAllPack(fnScanPackCallback pCallback, void* pCallbackParam, int& nCount, const char* pFileNamePrefix = NULL);
	//获得包内某个子文件的存储数据
	unsigned int	GetElemStoreDataInPak(int nPakIndex, int nElemIndex, void* pBuffer, unsigned int uBufferSize);
	//往打包文件中添加一个子文件（已经压缩好的）
	//传入参数ElemInfo::nPakIndex表示要加入哪个Pak文件
	//传入参数ElemInfo::nElemIndex无意义被忽略
	bool	AddElemToPak(ELEM_FILE_INFO& ElemInfo, void* pBuffer);

	//初始化
	int		Initialize();
private:
	//往打包文件中添加一个目录
	bool	AddFolderToPak(int nFullFolderFileNameLen);
	//往打包文件中添加一个子文件
	bool	AddElemToPak();
	//查找子文件在包中的位置,如果找到返回true,uIndex为传回找到的位置；未找到则返回false,uIndex传回应当插入的位置
	bool	FindElem(unsigned int uElemId, int nPakIndex, unsigned int& uIndex);

    //生成用于打包用的ElemIndex以及HashId
    bool	GenerateElemIndexAndHashId(unsigned int & uElemIndex, unsigned int & uHashId);

	//往打包文件中分块方式添加子文件
	bool	AddElemToPakFragment(unsigned char* pSrcBuffer, int nNumFragment, int* pFragmentSizeList, unsigned int& uCompressSize);
	//往打包文件中添加一个常规子文件
    bool	AddElemToPakCommon(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressType, unsigned int& uCompressSize);
    //往打包文件中分块方式添加一个SPR子文件
    bool	AddElemToPakFragmentSPR(unsigned char* pSrcBuffer, int nSrcSize,unsigned int& uCompressSize);

    //把一块缓冲打包到文件里
    bool	AddBufferToFile(unsigned char * pSrcBuffer, int nSrcSize, unsigned int uCompressType, 
        unsigned int & uDestSize, unsigned int & uDestCompressType);
	//从包中读取一段数据
	bool	ReadElemBufferFromPak(int nPakIndex, unsigned int uOffset, unsigned int uStoreSize,
				unsigned int uPakMethod, void* pBuffer, unsigned int uSize);
	//获得打包信息文件的文件名
	bool	GetPackPartnerFileName(int nPakIndex, char* pFileName);
	//载入打包信息
	bool	LoadPackPartner(int nPakIndex);
	//新建打包信息文件
	bool	CreatePackPartner(int nPakIndex);
	//分配缓冲区，并把字文件解开读取到该缓冲区，成功返回缓冲区指针，失败返回空指针，调用处要负责释放该处返回的缓冲区
	unsigned char* AllocBufferAndReadElemFile(int nPakIndex, unsigned int uElemIndex, unsigned int& uElemSize);

private:
	struct PACK_ITEM
	{
		IFile*			pIOFile;			//读取写入操作的文件对象
		XPackFileHeader	Header;				//打包文件信息
		XPackIndexInfo*	pIndexList;			//子文件索引列表缓冲区
		int				nDataEndOffset;	    //当前打包文件数据结束位置的偏移位置（相对于文件头）
		bool			bModified;			//已经被修改
		bool			bExcludeOfCheckId;	//是否排出在检查相同id之外
		char			PackFileName[MAX_PATH];	//包文件名
	}		m_PackItemList[PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM];
	KPackFilePartner	m_PackPartnerList[PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM];		//打包文件伴侣

	char	m_ElemFileRootPath[MAX_PATH];	//子文件的根目录
	int		m_nElemFileRootPathLen;			//子文件的根目录字符串长度
	int		m_nElemFileRootPathNotEnderLen;	//子文件的根目录字符串长度(不包括最后那个'\')

	//--单次外部操作调用过程中的内部变量--
	int		m_nCurrentPakIndex;
	char	m_FullFolderFileName[MAX_PATH + MAX_PATH];	//这是个临时缓冲区，用于放置当前操作的文件/目录的名称，有时候是子文件相对于根目录的完成名称。
	int		m_nFullFolderFileNameLen;

	#define MAX_SUPPORTABLE_STORE_SIZE	XPACK_COMPRESS_SIZE_FILTER
 	#define COMPRESS_BUFFER_SIZE		4194304		//4M
	#define COMMON_FILE_SPLIT_SIZE		2097152		//2M
    unsigned char	m_CompressBuffer[COMPRESS_BUFFER_SIZE];
	fnAddFolderToPakCallback	m_pCallback;

    unsigned int  m_uPackFileShellOptionSprSplitFrameBalance; //spr文件大小分帧压缩阀值，大于此值才压缩，小于此值整个文件写入，
                                                              //spr文件不受m_bPackFileShellOptionSizeLimit限制
};

#endif //_ENGINE_PACK_FILE_MANAGER_H_

