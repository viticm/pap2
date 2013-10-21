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

template<typename T, int nSize>
class KCircleList
{
public:
	KCircleList();
	~KCircleList();

	BOOL Push(T& Obj);

	BOOL Pop(T* pObj);

    BOOL Pick(T* pObj);

private:
    // 注意,这个表实际上只能存放nSize-1个元素,m_nHead指向的位置永远是空的
	T		    m_ObjList[nSize];	// 队列数组
	int         m_nHead;		    // 头指针
	int		    m_nTail;		    // 尾指针
};

template<typename T, int nSize>
KCircleList<T, nSize>::KCircleList()
{
    assert(nSize > 2);
	m_nHead    = 0;
    m_nTail    = 0;
}

template<typename T, int nSize>
KCircleList<T, nSize>::~KCircleList()
{
}


template<typename T, int nSize>
BOOL KCircleList<T, nSize>::Push(T& Obj)
{
	int nNextPos = (m_nHead + 1) % nSize;

	if (nNextPos == m_nTail)
		return false;

	m_ObjList[m_nHead]  = Obj;
	m_nHead             = nNextPos;

	return true;
}

template<typename T, int nSize>
BOOL KCircleList<T, nSize>::Pop(T* pObj)
{
	if (m_nTail == m_nHead)
		return false;

    if (pObj)
    {
	    *pObj = m_ObjList[m_nTail];
    }

	m_nTail = (m_nTail + 1) % nSize;

	return true;
}

template<typename T, int nSize>
BOOL KCircleList<T, nSize>::Pick(T* pObj)
{
	if (m_nTail == m_nHead)
		return false;

    if (pObj)
    {
	    *pObj = m_ObjList[m_nTail];
    }

	return true;
}

#endif	//_KCIRCLE_LIST_H_

