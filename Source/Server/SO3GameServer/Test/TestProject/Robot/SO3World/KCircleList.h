/************************************************************************/
/* 循环链表								                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.02	Create												*/
/* Comment	 :															*/
/*		对于一个线程只读，一个线程只写的操作，该类是安全的				*/
/*																		*/
/* P.S.		西江月一首													*/
/*		玉 骨 哪 愁 瘴 雾												*/
/*		冰 肌 自 有 仙 风												*/
/*		海 仙 时 遣 探 芳 丛											*/
/*		倒 挂 绿 毛 么 凤												*/
/*																		*/
/*		素 面 常 嫌 粉 宛												*/
/*		洗 妆 不 褪 唇 红												*/
/*		高 情 已 逐 晓 云 空											*/
/*		不 与 梨 花 同 梦												*/
/*																		*/
/*		该词传为苏轼悼侍妾朝云所作										*/
/************************************************************************/
#ifndef _KCIRCLE_LIST_H_
#define _KCIRCLE_LIST_H_

class KCircleList
{
public:
	KCircleList(void);
	~KCircleList(void);

	//初始化，指定循环队列长度
	BOOL Init(DWORD dwSize);

	//压入数据指针
	BOOL Push(void* pObj);
	//弹出数据指针
	void* Pop(void);
	//检测即将弹出的指针
	void* Check(void);

private:
	DWORD		m_dwSize;		//队列长度
	void**		m_ObjList;		//队列数组
	DWORD		m_dwHead;		//头指针
	DWORD		m_dwRear;		//尾指针
};
#endif	//_KCIRCLE_LIST_H_