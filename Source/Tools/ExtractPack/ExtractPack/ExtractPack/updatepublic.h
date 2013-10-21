//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdatePublic.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-17 11:14:17
//  Comment     :   Define the comment interface of update system
//
//////////////////////////////////////////////////////////////////////////////////////


#ifndef __UPDATE_PUBLIC_H__
#define __UPDATE_PUBLIC_H__ 

//------------------------------------------------------------------------------------

#define MAX_FILE					2004800			//最多40万个文件
///////////////////////////////////////////////////////////////////////////////
//更新包合并 Add By Fellow 2003.08.11
//一个Pack文件具有下面的结构:
//首先是四个字节的文件的头标志:字符串'PACK',然后是项的数目然后是索引开始的偏移量\数据开始的偏移量,然后是校验和,然后是保留的字节:
typedef struct {
	UCHAR signature[4];			//"PACK"
	DWORD count;				//数据的条目数
	DWORD index_offset;			//索引的偏移量
	DWORD data_offset;			//数据的偏移量
	DWORD crc32;
	UCHAR reserved[12];
} t_pack_header;



typedef struct {		//索引信息
	DWORD id;
	DWORD offset;
	INT size;
	INT compress_size;
} t_index_info;

typedef struct 
{
	t_index_info* pIndexInfo;
	int nComprassSize;
	//int nBlackNum;

} t_PakNode_Info;

#define MAX_FILE					2004800			//最多20万个文件
#define BUFFER_MAX                  1024 * 1024 * 64  //128MB的缓存空间

#ifndef MAX
	#define MAX(x, y)					((x) > (y) ? (x) : (y))
	#define MIN(x, y)					((x) < (y) ? (x) : (y))
#endif

VOID GetHeader(t_pack_header* aHeader, FILE* aFile);
VOID GetIndexInfo(t_index_info* aIndexInfo, CONST t_pack_header* aHeader, FILE* aFile);
bool PackComb(LPCSTR stdFilePath,LPCSTR addFilePath);
int CompressPack(const TCHAR* cpszPackFileName);
///////////////////////////////////////////////////////////////////////////

#endif  //__UPDATE_PUBLIC_H__

