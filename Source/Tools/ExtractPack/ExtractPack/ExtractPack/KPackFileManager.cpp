/*****************************************************************************************
//	包文件的生成、展开、修改、合并等操作
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
*****************************************************************************************/
#include "Precompile.h"
#include "CRC32.h"
#include "File.h"
#include "SmartPtr.h"
#include "KPackFileManager.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __linux
	#include <sys/time.h>
	#include <sys/types.h>
	#include <utime.h>
#else
	#include <time.h>
	#include <sys/utime.h>
#endif

//---------------------------------------------------------------------------
//	Spr图文件结构定义
//---------------------------------------------------------------------------
#define	SPR_COMMENT_FLAG				0x525053	//'SPR'
struct SPRHEAD	//Spr文件头结构
{
	BYTE	Comment[4];	// 注释文字(SPR\0)
	WORD	Width;		// 图片宽度
	WORD	Height;		// 图片高度
	WORD	CenterX;	// 重心的水平位移
	WORD	CenterY;	// 重心的垂直位移
	WORD	Frames;		// 总帧数
	WORD	Colors;		// 颜色数
	WORD	Directions;	// 方向数
	WORD	Interval;	// 每帧间隔（以游戏帧为单位）
	WORD	Reserved[6];// 保留字段（到以后使用）
};
struct SPROFFS	//Spr文件帧偏移信息结构
{
	DWORD	Offset;		// 每一帧的偏移
	DWORD	Length;		// 每一帧的长度
};
//---------------------------------------------------------------------------



KPackFileManager::KPackFileManager()
{
    memset(&m_PackItemList, 0, sizeof(m_PackItemList));
    m_ElemFileRootPath[0] = 0;
    m_nElemFileRootPathLen = m_nElemFileRootPathNotEnderLen = 0;
    m_FullFolderFileName[0] = 0;
    m_nFullFolderFileNameLen = 0;
    m_pCallback = NULL;
    m_uPackFileShellOptionSprSplitFrameBalance = IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_DEF;
}

//设置子文件的根目录
void KPackFileManager::SetElemFileRootPath(const char* pPath)
{
    if (pPath && pPath[0])
    {
        m_nElemFileRootPathLen = (int)strlen(pPath);
        if (m_nElemFileRootPathLen < MAX_PATH - 128)
        {          
            memcpy(m_ElemFileRootPath, pPath, m_nElemFileRootPathLen + 1);
            if (m_ElemFileRootPath[m_nElemFileRootPathLen - 1] != '\\' &&
                m_ElemFileRootPath[m_nElemFileRootPathLen - 1] != '/')
            {
                m_ElemFileRootPath[m_nElemFileRootPathLen++] = '\\';
                m_ElemFileRootPath[m_nElemFileRootPathLen] = 0;
            }
            m_nElemFileRootPathNotEnderLen = m_nElemFileRootPathLen - 1;
            return;
        }
    }
    m_ElemFileRootPath[0] = 0;
    m_nElemFileRootPathLen = m_nElemFileRootPathNotEnderLen = 0;
}

int	KPackFileManager::Initialize()
{
    if (!CD_LCU_I())
        return false;
    return true;
}

//销毁接口对象
void KPackFileManager::Release()
{
    for (int i = 0; i < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; i++)
    {
        ClosePack(i);
    }
    delete this;
}

//创建/打开的一个打包文件，返回打包文件索引，返回0值表示操作失败。
int KPackFileManager::CreatePack(const char* pszFile, int bOpenExist, int bExcludeOfCheckId)
{
    int nPakIndex;
    for (nPakIndex = 0; nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; nPakIndex++)
    {
        if (m_PackItemList[nPakIndex].pIOFile == NULL)
            break;
    }
    if (nPakIndex == PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
        return -1;

    PACK_ITEM& item = m_PackItemList[nPakIndex];
    bool bOk = false;
	g_GetFullPath(item.PackFileName, pszFile);
    while(true)
    {
        item.pIndexList = (XPackIndexInfo*)malloc(sizeof(XPackIndexInfo) * PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
        if (item.pIndexList == NULL)
            break;
        memset(item.pIndexList, 0, sizeof(XPackIndexInfo) * PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
        if (bOpenExist)
        {
			if (!g_IsFileExist(pszFile))
				break;
			item.pIOFile = g_OpenFile(pszFile, true, true);
			if (item.pIOFile == NULL)
				break;
			if (item.pIOFile->Read(&item.Header, sizeof(item.Header)) != sizeof(item.Header) ||
				(*(int*)(&(item.Header.cSignature)) != IPACK_FILE_SIGNATURE_FLAG))
			{
				break;
			}
			item.pIOFile->Seek(item.Header.uIndexTableOffset, SEEK_SET);
			if (item.pIOFile->Read(item.pIndexList, sizeof(XPackIndexInfo) * item.Header.uCount) != sizeof(XPackIndexInfo) * item.Header.uCount)
				break;
			item.nDataEndOffset = item.Header.uIndexTableOffset;
			item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);
			LoadPackPartner(nPakIndex);
        }
        else
        {
            item.pIOFile = g_CreateFile(pszFile);
            if (item.pIOFile == NULL)
                break;
            memset(&item.Header, 0, sizeof(item.Header));
            *(int*)(&(item.Header.cSignature)) = IPACK_FILE_SIGNATURE_FLAG;
            if (item.pIOFile->Write(&item.Header, sizeof(item.Header)) != sizeof(item.Header))
                break;
            item.Header.uDataOffset = sizeof(item.Header);
            item.nDataEndOffset = sizeof(item.Header);
            item.bModified = true;
			CreatePackPartner(nPakIndex);
        }
        item.bExcludeOfCheckId = (bExcludeOfCheckId != false);
        bOk = true;
        break;
    }

    if (!bOk)
    {
		SAFE_FREE(item.pIndexList);
		SAFE_RELEASE(item.pIOFile);
		item.PackFileName[0] = 0;
        return -1;
    }

    return nPakIndex;
}

//关闭打包文件
void KPackFileManager::ClosePack(int nPakIndex)
{
    if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
        return;
    PACK_ITEM& item = m_PackItemList[nPakIndex];
    if (item.pIOFile)
    {
        if (item.bModified && item.pIndexList)
        {
			int nLen = sizeof(XPackIndexInfo) * item.Header.uCount;
			item.Header.uCrc32 = Misc_CRC32(0, item.pIndexList, nLen);
			item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);
            item.pIOFile->Write(item.pIndexList, nLen);
            item.pIOFile->Seek(0, SEEK_SET);
            item.Header.uIndexTableOffset = item.nDataEndOffset;
			time_t	t;
			item.Header.uPakTime = (unsigned int)time(&t);
            item.pIOFile->Write(&item.Header, sizeof(item.Header));
			GetPackPartnerFileName(nPakIndex, m_FullFolderFileName);
			m_PackPartnerList[nPakIndex].Save(m_FullFolderFileName, item.Header.uPakTime, item.Header.uCrc32);
        }
        item.pIOFile->Release();
        item.pIOFile = NULL;
    }
    SAFE_FREE(item.pIndexList);
    memset(&item, 0, sizeof(item));
	m_PackPartnerList[nPakIndex].Clear();
}

//往打包文件中添加一个目录
bool KPackFileManager::AddFolderToPak(int nPakIndex, const char* pFolder, fnAddFolderToPakCallback pCallback)
{
    if (pFolder == NULL || pFolder[0] == 0)
        return false;
    m_nCurrentPakIndex = nPakIndex;
    m_pCallback = pCallback;

	int nFullFolderFileNameLen = (int)strlen(pFolder);
	if (strcmp(pFolder, ".") == 0)
	{	//====压缩根目录下全部目录====
		strcpy(m_FullFolderFileName, m_ElemFileRootPath);
		nFullFolderFileNameLen = m_nElemFileRootPathLen;
	}
	else
	{
	    memcpy(m_FullFolderFileName, m_ElemFileRootPath, m_nElemFileRootPathLen);
		memcpy(m_FullFolderFileName + m_nElemFileRootPathLen, pFolder, nFullFolderFileNameLen);
		nFullFolderFileNameLen += m_nElemFileRootPathLen;
	}

    if (m_FullFolderFileName[nFullFolderFileNameLen - 1] != '\\' &&
        m_FullFolderFileName[nFullFolderFileNameLen - 1] != '/')
    {
        m_FullFolderFileName[nFullFolderFileNameLen++] = '\\';
        m_FullFolderFileName[nFullFolderFileNameLen] = 0;
    }

    bool nRet = AddFolderToPak(nFullFolderFileNameLen);
    m_pCallback = NULL;
    return nRet;
}

//往打包文件中添加一个目录
bool KPackFileManager::AddFolderToPak(int nFullFolderFileNameLen)
{
    strcpy(m_FullFolderFileName + nFullFolderFileNameLen, "*.*");
    HANDLE			hFindHandle;	//win32查找文件句柄
    WIN32_FIND_DATA	FindData;
    hFindHandle = ::FindFirstFile(m_FullFolderFileName, &FindData);
    if (hFindHandle == INVALID_HANDLE_VALUE)
        return false;

    bool bOk = true;
    do
    {
        if (FindData.cFileName[0] == '.')
            continue;
        int nNewLen = (int)strlen(FindData.cFileName);
        memcpy(m_FullFolderFileName + nFullFolderFileNameLen, FindData.cFileName, nNewLen);
        nNewLen += nFullFolderFileNameLen;
		if (nNewLen - m_nElemFileRootPathNotEnderLen >= 128)
		{
			printf("Error: The length of [%S] is out of limit!\n", m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);
		}
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            m_FullFolderFileName[nNewLen++] = '\\';
            m_FullFolderFileName[nNewLen] = 0;
            if (!AddFolderToPak(nNewLen))
            {
                bOk = false;
                break;
            }
        }
        else
        {
            m_FullFolderFileName[nNewLen] = 0;
            if (m_pCallback && !m_pCallback(m_FullFolderFileName))
                continue;
            if (!AddElemToPak())
            {
                bOk = false;
                break;
            }
        }
    }while(::FindNextFile(hFindHandle, &FindData));

    ::FindClose(hFindHandle);
    hFindHandle = INVALID_HANDLE_VALUE ;
    return bOk;
}

//往打包文件中添加一个子文件
bool KPackFileManager::AddElemToPak(int nPakIndex, const char* pElemFile)
{
    if (pElemFile == NULL || pElemFile[0] == 0)
        return false;
    m_nCurrentPakIndex = nPakIndex;
    memcpy(m_FullFolderFileName, m_ElemFileRootPath, m_nElemFileRootPathLen);
    strcpy(m_FullFolderFileName + m_nElemFileRootPathLen, pElemFile);
    return AddElemToPak();
}

bool KPackFileManager::GenerateElemIndexAndHashId(unsigned int & uElemIndex, unsigned int & uHashId)
{
    PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];

	if (m_PackItemList[m_nCurrentPakIndex].Header.uCount == PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM)
	{
		printf("Error: The elem file count in a single pak has reach %d!\n", PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
		return false;
	}

	uHashId = g_FileNameHash(m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);

	if (item.bExcludeOfCheckId == false)
	{	//==检查是否已经在其他包中有同名id==
		for (unsigned int i = 0; i < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; i++)
		{
			if (m_PackItemList[i].pIOFile == NULL)
				continue;
			if (i != m_nCurrentPakIndex && m_PackItemList[i].bExcludeOfCheckId)
				continue;
			unsigned int uIndex;
			if (FindElem(uHashId, i, uIndex))
			{
				printf("Error: %s has the same id %X!\n", m_FullFolderFileName + m_nElemFileRootPathNotEnderLen, uHashId);
				return false;
			}
			if (i == m_nCurrentPakIndex)
				uElemIndex = uIndex;
		}
	}
    return true;
}

bool KPackFileManager::AddBufferToFile(unsigned char * pSrcBuffer, int nSrcSize, unsigned int uCompressType, 
                             unsigned int & uDestSize, unsigned int & uDestCompressType)
{
    PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];

	void*	pWriteBuffer = pSrcBuffer;
	uDestSize = nSrcSize;
	uDestCompressType = XPACK_METHOD_NONE;
	if (uCompressType == XPACK_METHOD_UCL)
    {
        if (CD_LCU_C(pSrcBuffer, nSrcSize, m_CompressBuffer, &uDestSize, 5))
        {
			if (nSrcSize > (int)uDestSize)
			{	// Compress
				if (uDestSize <= COMPRESS_BUFFER_SIZE)
				{
					pWriteBuffer = m_CompressBuffer;
					uDestCompressType = XPACK_METHOD_UCL;
				}
				else
				{
					printf("Warning : compressbuffer overflow!");
				}
			}
        }
		if (pWriteBuffer == pSrcBuffer)
			uDestSize = nSrcSize;
	}

	if (item.pIOFile->Write(pWriteBuffer, uDestSize) == uDestSize)
	{
		return true;
	}

	printf("Error: Cannot write XPackFileFragment\n");
	return false;
}

//往打包文件中分块方式添加子文件
bool KPackFileManager::AddElemToPakFragment(unsigned char* pSrcBuffer, int nNumFragment, int* pFragmentSizeList, unsigned int& uCompressSize)
{
	PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];
	//使用UCL压缩
	uCompressSize = 0;

	unsigned int nBufferCompressType;
	unsigned int uBufferCompressSize = 0;
	int			 nFragmentInfoListSize = sizeof(XPackFileFragmentInfo) * nNumFragment;
	XPackFileFragmentInfo	FragmentInfoList[100];
	XPackFileFragmentInfo*	pFragmentInfoList = NULL;
	if (nNumFragment <= 100)
		pFragmentInfoList = FragmentInfoList;
	else
		pFragmentInfoList = (XPackFileFragmentInfo*)malloc(nFragmentInfoListSize);
	if (!pFragmentInfoList)
		return false;

	// [XPackFileFragmentElemHeader] + [第一块数据] + [第二块数据] + .. + [第n块数据] + [n个XPackFileFragmentInfo的数组]
	long lItemFileBegin = item.pIOFile->Tell();

	XPackFileFragmentElemHeader fragmentHeader = { 0, 0 };

	// Step 1: [XPackFileFragmentElemHeader] 保留位置
	{
		if (item.pIOFile->Write(&fragmentHeader, sizeof(XPackFileFragmentElemHeader)) 
			!= sizeof(XPackFileFragmentElemHeader))
		{
			printf("Error: Cannot write XPackFileFragmentElemHeader [%s]\n", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
		fragmentHeader.nFragmentInfoOffest += sizeof(XPackFileFragmentElemHeader);
	}

	// Step 2: 存储各块数据
	int nSrcOffset = 0;
	for (int nFragment = 0; nFragment < nNumFragment; nFragment++)
	{
		if (!AddBufferToFile(pSrcBuffer + nSrcOffset, pFragmentSizeList[nFragment], XPACK_METHOD_UCL, uBufferCompressSize, nBufferCompressType))
		{
			printf("Error: Cannot add fragment of [%s]", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
		nSrcOffset += pFragmentSizeList[nFragment];
		pFragmentInfoList[nFragment].uSize		= pFragmentSizeList[nFragment];
		pFragmentInfoList[nFragment].uOffset	= fragmentHeader.nFragmentInfoOffest;
		pFragmentInfoList[nFragment].uCompressSizeFlag = uBufferCompressSize | nBufferCompressType;
		fragmentHeader.nFragmentInfoOffest += uBufferCompressSize;
		if (fragmentHeader.nFragmentInfoOffest + nFragmentInfoListSize > MAX_SUPPORTABLE_STORE_SIZE)
		{
			printf("Warning: file store size exceed limit [%s].", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
	}

	// Step 3: [nFragment个XPackFileFragmentInfo的数组]
	{
		if (item.pIOFile->Write(pFragmentInfoList, nFragmentInfoListSize) != nFragmentInfoListSize)
		{
			printf("Error: Cannot write XPackFileFragmentInfo [%s]\n", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
	}

	// Step 4: [XPackFileFragmentElemHeader] 重新写入
	item.pIOFile->Seek(lItemFileBegin, SEEK_SET);
	fragmentHeader.nNumFragment = nNumFragment;
	if (item.pIOFile->Write(&fragmentHeader, sizeof(XPackFileFragmentElemHeader)) 
		!= sizeof(XPackFileFragmentElemHeader))
	{
		printf("Error: Cannot write XPackFileFragmentElemHeader [%s]\n", m_FullFolderFileName);
		goto ERROR_EXIT;
	}
	uCompressSize = fragmentHeader.nFragmentInfoOffest + nFragmentInfoListSize;
	item.pIOFile->Seek(lItemFileBegin + uCompressSize, SEEK_SET);

	if (nNumFragment > 100)
		SAFE_FREE(pFragmentInfoList);
	return true;

ERROR_EXIT:
	if (nNumFragment > 100)
		SAFE_FREE(pFragmentInfoList);
	return false;
}

bool KPackFileManager::AddElemToPakCommon(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressType, unsigned int& uCompressSize)
{
	if (nSrcSize <= COMMON_FILE_SPLIT_SIZE)
		return AddBufferToFile(pSrcBuffer, nSrcSize, uCompressType, uCompressSize, uCompressType);
	int nNumFragment = (nSrcSize + COMMON_FILE_SPLIT_SIZE - 1) / COMMON_FILE_SPLIT_SIZE;
	int	nFragmentSizeList[20];
	int*	pFragmentSizeList = NULL;
	if (nNumFragment <= 20)
		pFragmentSizeList = nFragmentSizeList;
	else
		pFragmentSizeList = (int*)malloc(sizeof(int) * nNumFragment);
	if (!pFragmentSizeList)
		return false;
	for (int i = 0; i < nNumFragment; i++)
		pFragmentSizeList[i] = COMMON_FILE_SPLIT_SIZE;
	if (nSrcSize % COMMON_FILE_SPLIT_SIZE)
		pFragmentSizeList[nNumFragment - 1] = nSrcSize % COMMON_FILE_SPLIT_SIZE;
	uCompressType = XPACK_FLAG_FRAGMENT;
	bool bResult = AddElemToPakFragment(pSrcBuffer, nNumFragment, pFragmentSizeList, uCompressSize);
	if (nNumFragment > 20)
		SAFE_FREE(pFragmentSizeList);
	return bResult;
}

bool KPackFileManager::AddElemToPakFragmentSPR(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressSize)
{
	// 对于SPR文件，分块方式为：
	//[SPRHEAD + pallette] + [offsettable] + [SPRFRAME0] + .. + [SPRFRAMEn]

    //使用UCL压缩
	uCompressSize = 0;

	SPRHEAD* head = (SPRHEAD*)pSrcBuffer;
	unsigned int const uSprHeadSize = sizeof(SPRHEAD) + head->Colors * 3;
	unsigned int const uOffsetTableSize = sizeof(SPROFFS) * head->Frames;
	SPROFFS*    const pSprOffsTable     = (SPROFFS *)(pSrcBuffer + uSprHeadSize);

	int nNumFragment = head->Frames + 2;
	int	nFragmentSizeList[100];
	int*	pFragmentSizeList = NULL;
	if (nNumFragment <= 100)
		pFragmentSizeList = nFragmentSizeList;
	else
		pFragmentSizeList = (int*)malloc(sizeof(int) * nNumFragment);
	if (!pFragmentSizeList)
		return false;

	pFragmentSizeList[0] = uSprHeadSize;
	pFragmentSizeList[1] = uOffsetTableSize;
	for (int i = 0; i < head->Frames; i++)
	{
		SPROFFS * pSprOffs = pSprOffsTable + i;
		pFragmentSizeList[i + 2] = pSprOffs->Length;
	}

	bool bResult = AddElemToPakFragment(pSrcBuffer, nNumFragment, pFragmentSizeList, uCompressSize);
	if (nNumFragment > 100)
		SAFE_FREE(pFragmentSizeList);
	return bResult;
}

//往打包文件中添加一个子文件
bool KPackFileManager::AddElemToPak()
{
	unsigned int uElemIndex = 0;
	unsigned int uHashId    = 0;
	unsigned int uCRC = 0;
	if (!GenerateElemIndexAndHashId(uElemIndex, uHashId))
		return false;

	KSmartFile SrcFile;
	SrcFile = g_OpenFile(m_FullFolderFileName, true, false);
	if (!SrcFile)
		return false;

	int	nSrcSize = SrcFile->Size();
	if (nSrcSize == 0)
		return true;

	unsigned int uCompressType = XPACK_METHOD_UCL;
	unsigned int uCompressSize;

	unsigned char * pSrcBuffer = (unsigned char*)SrcFile->GetBuffer();
	if (pSrcBuffer == NULL)
		return false;

	const char* pExt = strrchr(m_FullFolderFileName, '.');
	if (pExt && !stricmp(pExt + 1, "spr"))    // 判断是否为SPR文件
	{
		if ((unsigned int)nSrcSize >= m_uPackFileShellOptionSprSplitFrameBalance)	//spr超过大小则分块压缩
		{
			SPRHEAD* pSpr = (SPRHEAD*)pSrcBuffer;
			if (*(int*)(&(pSpr->Comment)) == SPR_COMMENT_FLAG && pSpr->Frames > 1)
				uCompressType = XPACK_FLAG_FRAGMENT;
		}
	}

	PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];
	item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);

	bool bOk = false;
	if (uCompressType == XPACK_FLAG_FRAGMENT)	//分块存的spr
		bOk = AddElemToPakFragmentSPR(pSrcBuffer, nSrcSize, uCompressSize);
	else
		bOk = AddElemToPakCommon(pSrcBuffer, nSrcSize, uCompressType, uCompressSize);

	uCRC = Misc_CRC32(0, pSrcBuffer, nSrcSize);
	SrcFile.Release();
	if (bOk)
	{
		for (unsigned int i = item.Header.uCount; i > uElemIndex; i--)
			item.pIndexList[i] = item.pIndexList[i - 1];
		item.Header.uCount++;
		item.pIndexList[uElemIndex].uCompressSizeFlag = uCompressSize | uCompressType;
		item.pIndexList[uElemIndex].uSize = nSrcSize;
		item.pIndexList[uElemIndex].uId = uHashId;
		item.pIndexList[uElemIndex].uOffset = item.nDataEndOffset;
		item.nDataEndOffset += uCompressSize;
		item.bModified = true;

		KPackFilePartner::PACKPARTNER_ELEM_INFO	info;
		info.nElemIndex = uElemIndex;
		strcpy(info.szFileName, m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);
		info.uCRC = uCRC;
		info.uId = uHashId;
		info.uSize = nSrcSize;
		info.uStoreSizeAndCompressFlag = uCompressSize | uCompressType;
		time_t	t;
		struct _stat	s;
		if (!_stat(m_FullFolderFileName, &s))
			info.uTime = (unsigned int)s.st_mtime;
		else
			info.uTime = (unsigned int)time(&t);
		bOk = m_PackPartnerList[m_nCurrentPakIndex].AddElem(info);
		assert(bOk);
	}
	return bOk;
}

//查找子文件在包中的位置,如果找到返回true,uIndex为传回找到的位置；未找到则返回false,uIndex传回应当插入的位置
bool KPackFileManager::FindElem(unsigned int uElemId, int nPakIndex, unsigned int& uIndex)
{
    PACK_ITEM&		item = m_PackItemList[nPakIndex];

    int nBegin, nEnd, nMid;
    nBegin = 0;
    nEnd = item.Header.uCount - 1;
    while (nBegin <= nEnd)
    {
        nMid = (nBegin + nEnd) / 2;
        if (uElemId < item.pIndexList[nMid].uId)
        {
            nEnd = nMid - 1;
        }
        else if (uElemId > item.pIndexList[nMid].uId)
        {
            nBegin = nMid + 1;
        }
        else
        {
            uIndex = nMid;
            return true;
        }
    }

    if (nBegin == nEnd)
        uIndex = (uElemId < item.pIndexList[nMid].uId) ? nMid : (nMid + 1);
    else
        uIndex = nBegin;
    return false;
}

//删除打包文件中的一个子文件
int	KPackFileManager::DeleteElemInPak(int nPakIndex, const char* pElemFile)
{
    return false;
}

//设置接口操作参数
void KPackFileManager::SetOption(IPACK_FILE_SHELL_OPTION eOption, int nValue)
{
    switch (eOption)
    {
    case IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE:
		if (nValue > 0)
			m_uPackFileShellOptionSprSplitFrameBalance = nValue;
		else
			m_uPackFileShellOptionSprSplitFrameBalance = IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_DEF;
        break;
	case IPACK_FILE_SHELL_OPTION_RETRENCH:
		break;
    }
}

//得到包中子文件的数目
int KPackFileManager::GetElemCountInPak(int nPakIndex)
{
    int nRet = 0;
    if (nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
    {
        nRet = m_PackItemList[nPakIndex].Header.uCount;
    }
    return nRet;
}

//获取子文件信息
bool KPackFileManager::GetElemInfo(const char* pElemName, ELEM_FILE_INFO& info)
{
	assert(pElemName);
	char	szName[128];
	strcpy(szName, pElemName);
	if (GetElemInfo(g_FileNameHash(szName), info))
	{
		if (!info.szFileName[0])
			strcpy(info.szFileName, szName);
		return true;
	}
	return false;
}

//获取子文件信息
bool KPackFileManager::GetElemInfo(unsigned int uElemId, ELEM_FILE_INFO& info)
{
	memset(&info, 0, sizeof(info));
	info.uId = uElemId;
	for (info.nPakIndex = 0; info.nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; info.nPakIndex++)
	{
		unsigned int uElemIndex;
		if (!FindElem(info.uId, info.nPakIndex, uElemIndex))
			continue;
		KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
		info.nElemIndex = uElemIndex;

		XPackIndexInfo&		IndexInfo = (m_PackItemList[info.nPakIndex].pIndexList[uElemIndex]);
		info.uCompressFlag	= (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
		info.uStoreSize		= (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
		info.uSize			= IndexInfo.uSize;
		if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
		{
			assert(info.nElemIndex == PartnerInfo.nElemIndex);
			info.uCRC = PartnerInfo.uCRC;
			info.uTime = PartnerInfo.uTime;
			strcpy(info.szFileName, PartnerInfo.szFileName);
		}
		return true;
	}
	info.nPakIndex = -1;
	return false;
}

//从包中解出某个文件
bool KPackFileManager::UnpackElem(int nPakIndex, const char* pElemName, const char* pDestName)
{
	if (pElemName && pElemName[0])
	{
		strcpy(m_FullFolderFileName, pElemName);
		unsigned int uElemId = g_FileNameHash(m_FullFolderFileName);
		return UnpackElemByID(nPakIndex, uElemId, pDestName);
	}
	return false;
}

//分配缓冲区，并把字文件解开读取到该缓冲区，成功返回缓冲区指针，失败返回空指针，调用处要负责释放该处返回的缓冲区
unsigned char* KPackFileManager::AllocBufferAndReadElemFile(int nPakIndex, unsigned int uElemIndex, unsigned int& uElemSize)
{
	uElemSize = 0;
	//----参数合法性判断-----
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return NULL;
	PACK_ITEM&		item = m_PackItemList[nPakIndex];
	if (uElemIndex >= item.Header.uCount)
		return NULL;

	XPackIndexInfo& elem = item.pIndexList[uElemIndex];
	unsigned uSize = elem.uSize;
	unsigned char* pBuffer = (unsigned char*)malloc(uSize);
	if (pBuffer == NULL)
		return NULL;

	bool bOk = true;
	while(true)
	{
		if ((elem.uCompressSizeFlag & XPACK_FLAG_FRAGMENT) == 0)
		{
			bOk = ReadElemBufferFromPak(nPakIndex, elem.uOffset, (elem.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER),
				(elem.uCompressSizeFlag & XPACK_METHOD_FILTER), pBuffer, uSize);
			break;
		}
		XPackFileFragmentElemHeader header;
		if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset, sizeof(header), XPACK_METHOD_NONE, &header, sizeof(header)))
		{
			bOk = false;
			break;
		}
		uSize = 0;
		for (int i = 0; i < header.nNumFragment; i++)
		{
			XPackFileFragmentInfo	fragment;
			if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset + header.nFragmentInfoOffest + sizeof(fragment) * i,
				sizeof(fragment), XPACK_METHOD_NONE, &fragment, sizeof(fragment)))
			{
				bOk = false;
				break;
			}
			if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset + fragment.uOffset, (fragment.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER),
				(fragment.uCompressSizeFlag & XPACK_METHOD_FILTER), pBuffer + uSize, fragment.uSize))
			{
				bOk = false;
				break;
			}
			uSize += fragment.uSize;
		}
		break;
	};
	item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);

	if (bOk)
	{
		uElemSize = uSize;
	}
	else
	{
		SAFE_FREE(pBuffer);
	}
	return pBuffer;
}

//从包中解出某个文件
bool KPackFileManager::UnpackElemByID(int nPakIndex, unsigned int uElemId, const char* pDestName)
{
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return false;
	unsigned int	uElemIndex;
	if (FindElem(uElemId, nPakIndex, uElemIndex))
		return UnpackElemByIndex(nPakIndex, uElemIndex, pDestName);
	return false;
}

//从包中解出某个文件
bool KPackFileManager::UnpackElemByIndex(int nPakIndex, unsigned int uElemIndex, const char* pDestName)
{
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return false;
	KSmartFile		dest;
	dest = g_CreateFile(pDestName);
	if (dest)
	{
		unsigned uElemSize = 0;
		unsigned char* pBuffer = AllocBufferAndReadElemFile(nPakIndex, uElemIndex, uElemSize);
		if (pBuffer)
		{
			dest->Write(pBuffer, uElemSize);
			SAFE_FREE(pBuffer);
			return true;
		}
	}
	return false;
}

//从包中解出全部文件
bool KPackFileManager::UnpackAll(int nPakIndex, int& nUnpackCount, const char* pDestPath, const char* pFileNamePrefix)
{
	nUnpackCount = 0;
	if (pFileNamePrefix == NULL)
		pFileNamePrefix = "";

	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile ||
		pDestPath == NULL || pDestPath[0] == 0)
	{
		return false;
	}
	int		nFileNameStartOffset = strlen(pDestPath);
	if (nFileNameStartOffset + 128 > MAX_PATH)
		return false;

	g_CreatePath(pDestPath);
	char	szFullName[MAX_PATH] = "";
	char*	pElemName = NULL;
	strcpy(szFullName, pDestPath);
	if (szFullName[nFileNameStartOffset - 1] == '\\')
	{
		pElemName = szFullName + nFileNameStartOffset - 1;
	}
	else
	{
		pElemName = szFullName + nFileNameStartOffset;
	}

	ELEM_FILE_INFO	info;
	info.nPakIndex = nPakIndex;
	PACK_ITEM& item = m_PackItemList[nPakIndex];

	for (info.nElemIndex = 0; info.nElemIndex < (int)item.Header.uCount; info.nElemIndex++)
	{
		XPackIndexInfo& IndexInfo = item.pIndexList[info.nElemIndex];
		info.uId = IndexInfo.uId;
	
		KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
		if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
		{
			if (pFileNamePrefix[0])
			{
				if (strstr(PartnerInfo.szFileName, pFileNamePrefix) != PartnerInfo.szFileName)
					continue;	//排除前缀不符合的文件
			}
			strcpy(info.szFileName, PartnerInfo.szFileName);
			info.uCRC = PartnerInfo.uCRC;
			info.uTime = PartnerInfo.uTime;
		}
		else
		{
			if (pFileNamePrefix[0])
				continue;
			sprintf(info.szFileName, "\\_-ID-_%08x", info.uId);
			info.uCRC = 0;
			info.uTime = 0;
		}
//		info.uCompressFlag = (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
//		info.uSize = IndexInfo.uSize;
//		info.uStoreSize = (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);

		strcpy(pElemName, info.szFileName);
		char* pLastSplit = strrchr(pElemName, '\\');
		if (pLastSplit && pLastSplit != pElemName)
		{
			*pLastSplit = 0;
			g_CreatePath(szFullName);
			*pLastSplit = '\\';
		}
		//----解开存盘----
		UnpackElemByIndex(nPakIndex, info.nElemIndex, szFullName);

		//----修改文件时间----
		if (info.uTime)
		{
			_utimbuf time;
			time.actime = time.modtime = info.uTime;
			_utime(szFullName, &time);
		}

		nUnpackCount++;
	}
	return true;
}

//从包中读取一段数据
bool KPackFileManager::ReadElemBufferFromPak(int nPakIndex, unsigned int uOffset, unsigned int uStoreSize,
				unsigned int uPakMethod, void* pBuffer, unsigned int uSize)
{
	assert(nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM && m_PackItemList[nPakIndex].pIOFile);
	PACK_ITEM&		item = m_PackItemList[nPakIndex];
	item.pIOFile->Seek(uOffset, SEEK_SET);

	if (uPakMethod == XPACK_METHOD_NONE)
	{
		assert(uStoreSize == uSize);
		if (item.pIOFile->Read(pBuffer, uSize) == uSize)
			return true;
	}
	else if (uStoreSize <= COMPRESS_BUFFER_SIZE && uPakMethod == XPACK_METHOD_UCL)
	{
		if (item.pIOFile->Read(m_CompressBuffer, uStoreSize) == uStoreSize &&
			CD_LCU_D(m_CompressBuffer, uStoreSize, (unsigned char*)pBuffer, uSize))
		{
			return true;
		}
	}
	return false;
}

//获得打包信息文件的文件名
bool KPackFileManager::GetPackPartnerFileName(int nPakIndex, char* pFileName)
{
	if (nPakIndex >=  0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM && m_PackItemList[nPakIndex].PackFileName[0])
	{
		assert(pFileName);
		sprintf(pFileName, "%s.txt", m_PackItemList[nPakIndex].PackFileName);
		return true;
	}
	return false;
}

//扫描整个包文件集合，对于每个子文件给出进行一次回调函数操作
//参数pFileNamePrefix表示遍历到的子文件名的前缀必须与此给定的字符串完全一致（不区分大小写），前缀不符的子文件将被略过，传入空指针表示无前缀限制。
bool	KPackFileManager::ScanAllPack(fnScanPackCallback pCallback, void* pCallbackParam, int& nCount, const char* pFileNamePrefix)
{
	nCount = 0;
	if (pFileNamePrefix == NULL)
		pFileNamePrefix = "";
	ELEM_FILE_INFO	info;
	for (info.nPakIndex = 0; info.nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; info.nPakIndex++)
	{
		PACK_ITEM& item = m_PackItemList[info.nPakIndex];
		for (info.nElemIndex = 0; info.nElemIndex < (int)item.Header.uCount; info.nElemIndex++)
		{
			XPackIndexInfo& IndexInfo = item.pIndexList[info.nElemIndex];
			info.uId = IndexInfo.uId;
			//排除已经出现过同ID的文件
			{
				int nPrePak = 0;
				for (nPrePak = 0; nPrePak < info.nPakIndex; nPrePak++)
				{
					unsigned int uPreElemIndex;
					if (FindElem(info.uId, nPrePak, uPreElemIndex))
						break;
				}
				if (nPrePak < info.nPakIndex)
					continue;
			}

			KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
			if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
			{
				if (pFileNamePrefix[0])
				{
					if (strstr(PartnerInfo.szFileName, pFileNamePrefix) != PartnerInfo.szFileName)
						continue;	//排除前缀不符合的文件
				}
				strcpy(info.szFileName, PartnerInfo.szFileName);
				info.uCRC = PartnerInfo.uCRC;
				info.uTime = PartnerInfo.uTime;
			}
			else
			{
				if (pFileNamePrefix[0])
					continue;
				info.szFileName[0] = 0;
				info.uCRC = 0;
				info.uTime = 0;
			}
			info.uCompressFlag = (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
			info.uSize = IndexInfo.uSize;
			info.uStoreSize = (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
			nCount++;
			if (pCallback)
			{
				if (!pCallback(info, pCallbackParam))	//返回值为0则终止扫描
				{
					return false;
				}
			}
		}
	}
	return true;
}

//获得包内某个子文件的存储数据
unsigned int KPackFileManager::GetElemStoreDataInPak(int nPakIndex, int nElemIndex, void* pBuffer, unsigned int uBufferSize)
{
	if (pBuffer == NULL || nPakIndex <  0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || nElemIndex < 0)
		return 0;
	PACK_ITEM& item = m_PackItemList[nPakIndex];
	if (nElemIndex >= (int)item.Header.uCount || uBufferSize < (item.pIndexList[nElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER))
		return 0;
	unsigned int uStoreSize = (item.pIndexList[nElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
	if (ReadElemBufferFromPak(nPakIndex, item.pIndexList[nElemIndex].uOffset, uStoreSize, XPACK_METHOD_NONE, pBuffer, uStoreSize))
		return uStoreSize;
	return 0;
}

//往打包文件中添加一个子文件（已经压缩好的）
//传入参数ElemInfo::nPakIndex表示要加入哪个Pak文件
//传入参数ElemInfo::nElemIndex无意义被忽略
bool KPackFileManager::AddElemToPak(ELEM_FILE_INFO& ElemInfo, void* pBuffer)
{
	if (pBuffer == NULL || ElemInfo.nPakIndex < 0 || ElemInfo.nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM ||
		ElemInfo.uId == 0 || ElemInfo.uSize == 0 || ElemInfo.uStoreSize == 0)
	{
		return false;
	}

	PACK_ITEM& item = m_PackItemList[ElemInfo.nPakIndex];
	unsigned int uElemIndex;
	bool bExist = FindElem(ElemInfo.uId, ElemInfo.nPakIndex, uElemIndex);

	//如果原来存在同id文件，原存储大小大于等于新存储大小，则新内容存在原字文件的存储位置。
	//原存储大小小于心存储大小，则新内容存在打包文件末尾。
	long	lOffset = item.nDataEndOffset;
	bool	bAppend = true;
	if (bExist && (item.pIndexList[uElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER) >= ElemInfo.uStoreSize)
	{
		lOffset = item.pIndexList[uElemIndex].uOffset;
		bAppend = false;
	}

	unsigned int uType;
	item.pIOFile->Seek(lOffset, SEEK_SET);
	m_nCurrentPakIndex = ElemInfo.nPakIndex;
	if (AddBufferToFile((unsigned char*)pBuffer, ElemInfo.uStoreSize, XPACK_METHOD_NONE, ElemInfo.uStoreSize, uType))
	{
		KPackFilePartner::PACKPARTNER_ELEM_INFO	info;
		if (!bExist)
		{
			for (unsigned int i = item.Header.uCount; i > uElemIndex; i--)
				item.pIndexList[i] = item.pIndexList[i - 1];
			item.Header.uCount++;
		}
		else
		{
			m_PackPartnerList[m_nCurrentPakIndex].GetElemInfo(ElemInfo.uId, info);
			if (stricmp(ElemInfo.szFileName, info.szFileName))
			{
				m_PackPartnerList[m_nCurrentPakIndex].GetElemInfo(ElemInfo.uId, info);
				printf("WARNING : Elem file [%s]of ID [%x] has been replace with [%s].\n",
					ElemInfo.szFileName, ElemInfo.uId, info.szFileName);
			}
		}

		item.pIndexList[uElemIndex].uCompressSizeFlag = ElemInfo.uStoreSize | ElemInfo.uCompressFlag;
		item.pIndexList[uElemIndex].uSize = ElemInfo.uSize;
		item.pIndexList[uElemIndex].uId = ElemInfo.uId;
		if (bAppend)
		{
			item.pIndexList[uElemIndex].uOffset = item.nDataEndOffset;
			item.nDataEndOffset += ElemInfo.uStoreSize;
		}
		item.bModified = true;

		info.nElemIndex = uElemIndex;
		strcpy(info.szFileName, ElemInfo.szFileName);
		info.uCRC = ElemInfo.uCRC;
		info.uId = ElemInfo.uId;
		info.uSize = ElemInfo.uSize;
		info.uStoreSizeAndCompressFlag = ElemInfo.uStoreSize | ElemInfo.uCompressFlag;
		info.uTime = ElemInfo.uTime;
		m_PackPartnerList[m_nCurrentPakIndex].AddElem(info);
	}
	return true;
}

//载入打包信息
bool KPackFileManager::LoadPackPartner(int nPakIndex)
{
	char	szFileName[MAX_PATH];
	if (GetPackPartnerFileName(nPakIndex, szFileName))
	{
		PACK_ITEM& item = m_PackItemList[nPakIndex];
		m_PackPartnerList[nPakIndex].Load(szFileName);
		if (m_PackPartnerList[nPakIndex].IsPartnerMatch(
			item.Header.uCount,	item.Header.uPakTime, item.Header.uCrc32))
		{
			return true;
		}
		printf("ERROR : elem list is not match to the pak file [%s] .\n", item.PackFileName);
	}
	return false;
}

//新建打包信息文件
bool KPackFileManager::CreatePackPartner(int nPakIndex)
{
	assert(nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM);
	return m_PackPartnerList[nPakIndex].Init();
}
