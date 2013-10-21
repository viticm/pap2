/* 
 * File:     UiTaskDataFile.h
 * Desc:     任务记事文件操作
 * Author:   flying
 * Creation: 2003/7/19
 */
//-----------------------------------------------------------------------------
/*************************************************************************
  美丽的花朵，会凋零。灿烂的星光，会在天际默默消失。地球，太阳，宇宙，
  也会有结束的一天。相比之下，人生的短暂，就象一颗流星。短暂的一生中，
  我们会遇到很多匆匆过客，我们会爱和憎恨，为自己的梦想努力奋斗。很多
  朋友会离开我们，也会继续认识很多朋友。在这之后，是永恒的长眠。每次
  在轮船上的夜晚，仰望和海洋相映的夜空，夜空上那么多灿烂的或者不灿烂
  的星光。也许下次还能见到，也许其中的一些将永远消失。所以应该用微笑
  来面对离别，因为离别意味着重逢，或者重逢的希望。所以应该用所有的心
  来珍惜相爱的人，因为夜空中能有一点星光属于自己是一生的幸福。所以应
  该记得老朋友，因为他们曾经是旅途中的一部分。所以应该不管走的多远都
  牵挂自己的父母，因为这个不用多说。所以应该尘封珍藏曾经的初恋，因为
  在人生中最终分开的曾经的结伴而行，会是一份永远难忘的回忆，虽然也许
  现在已经分开在天的两边，虽然失去的往事不再回头，虽然大家都奔波在繁
  忙的城市，但是应该在心底的最深处保留一份默默的祝福和牵挂，因为在心
  底最深处的回忆，点点滴滴都不会消失，会在某个静谧的夜晚，多年前的微
  不足道的细节也许会清晰的浮现。春天草木发芽，夏天满眼绿色，秋天的黄
  叶和花朵在风中飘散，冬天的漫天飞雪和寒冷的夜。这些在不断的发生，和
  夜空中的故事一样。花谢花飞花漫天，红销香断有谁怜。我们不需要有这样
  的感慨，这些要用微笑去面对。即使有泪，也是在心里。病神瑛，只生活在
  满纸荒唐中。
  剑侠情缘网络版，不再是一个游戏，而是另外一个世界的人生。在这个梦幻
  般的人生中，我们也会遇到上面所有的事情。也许在将来的某一天，我们会
  想看到自己走过的路，遇到过的人，曾经的战斗和共同撒过的热血，踏过的
  山水草木。希望我可以让我们留住回忆，只是希望到时候，不要像庄周那样，
  不知道是自己梦到蝴蝶，还是蝴蝶梦到自己。
  烟雾缭绕中，送给我所有的朋友，好梦。
                                     -- flying
                                   2003/7/19  3:28  西山居
 *************************************************************************/
/*
 * File format specification
 * 1 file header: 
 *   this section specifies the number of
 *   system record count and the bytes of 
 *   personal record.
 * 2 personal record:
 *   this section starts with one byte data
 *   as a flag, and keeps nPersonalRecordBytes
 *   bytes data.
 * 3 system record:
 *   this section also starts with one byte
 *   data to flag the beginning of system record.
 *   and then followed by nSystemRecordCount
 *   elements of TASK_SYSTEM_RECORD type.
 * 4 the whole file end with a byte flag as EOF.
 *                        flying 
 */
#if !defined _TASKDATAFILE_H
#define _TASKDATAFILE_H

#include <time.h>
#define		TASK_FILE_FLAG				0x46445400	//"\0TDF"

// data type definition.
struct TASK_FILE_HEADER 
{
	char	cFlag[sizeof(int)];
	int		nPersonalRecordBytes;
	int		nSystemRecordCount;
	int		nReserved;
};

struct TASK_SYSTEM_RECORD
{
	time_t			tTime;
	unsigned int	uReserved;
	int				nContentLen;
	union
	{
		char		cBuffer[4];
		const char*	pBuffer;	
	};
};


// Classsssssssssssssssssssss
class KTaskDataFile
{
public:
	static void		LoadData();
	static bool		SaveData();

	static void		ClearAll();

	static int		GetPersonalRecord(char* pszBuffer, int nBufferSize);
	static bool		SetPersonalRecord(const char* pstrRecord, int nLen);

	static void		ClearSystemRecords();
	static int		GetSystemRecordCount();
	static bool		InsertSystemRecord(TASK_SYSTEM_RECORD* pRecord);
	static bool		RemoveSystemRecord(int nIndex);
	static const TASK_SYSTEM_RECORD*	GetSystemRecord(int nIndex);

private:
	struct KPersonalRecord
	{
		int			nLen;
		char		cBuffer[4];
	};
	struct KTaskSystemRecordNode
	{
		KTaskSystemRecordNode*	pNext;
		TASK_SYSTEM_RECORD		Record;
	};

private:
	static void		GetFileName(char* pszFileName, int nLen);
	static void		AppendSystemRecord(KTaskSystemRecordNode* pNode);
	static void		InsertSystemRecord(KTaskSystemRecordNode* pNode);

private:
	static KPersonalRecord*			ms_pPersonalRecord;
	static KTaskSystemRecordNode*	ms_pSystemRecordList;
	static	int						ms_nSystemRecordCount;
};
#endif