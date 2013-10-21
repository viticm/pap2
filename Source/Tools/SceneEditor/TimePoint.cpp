#include "stdafx.h"
#include "TimePoint.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelSFX
{

//GetValue()得到某一时刻的值， 时间从0开始到1。0f结束。
float KTimePoint::GetValue(float fTime)
{
	if ( fTime < 0.0f || fTime > 1.0f )
		return -1.0f;
	if ( fTime == 0.0f)
	{
		return m_vecTP[0].fRightVal;
	}
	else
	{
		vector<TimePoint>::iterator i = GetTimeSocket(fTime);
		return (i-1)->fRightVal + (i->fLeftVal - (i-1)->fRightVal) / (i->fTime - (i-1)->fTime) * (fTime - (i-1)->fTime);
	}
}

KTimePoint::~KTimePoint()
{
	m_vecTP.clear();
	//fclose(fpDebug);
}

KTimePoint::KTimePoint()
{
	TimePoint tpStart = { 0.0f, 0.0f, 0.0f };
	TimePoint tpEnd = { 1.0f, 1.0f, 1.0f };
	m_vecTP.push_back(tpStart);
	m_vecTP.push_back(tpEnd);
	//fpDebug = fopen("d:\\aaa.txt", "w");
}

KTimePoint::KTimePoint(const TimePoint *pTP, int iTPCount)
{
	TimePoint tpStart = { 0.0f, 0.0f, 0.0f };
	TimePoint tpEnd = { 1.0f, 0.0f, 0.0f };
	m_vecTP.push_back(tpStart);
	m_vecTP.push_back(tpEnd);
	Insert(pTP, iTPCount);
}

KTimePoint::KTimePoint(float *fTP, int iTPCount)
{
	TimePoint tpStart = { 0.0f, 0.0f, 0.0f };
	TimePoint tpEnd = { 1.0f, 0.0f, 0.0f };
	m_vecTP.push_back(tpStart);
	m_vecTP.push_back(tpEnd);
	Insert(fTP, iTPCount);
}

void KTimePoint::Insert(const TimePoint *pTP, int iTPCount)
{
	int j;
	for ( j = 0; j < iTPCount; j ++, pTP++)
	{
		vector<TimePoint>::iterator i = GetTimeSocket(pTP->fTime);
		if ( i == m_vecTP.end() )
			m_vecTP.push_back(*pTP);
        else if ( i->fTime == pTP->fTime )
		{
			i->fLeftVal = pTP->fLeftVal;
			i->fRightVal = pTP->fRightVal;
		}
		else
			m_vecTP.insert(i, *pTP);
	}
}

void KTimePoint::Insert(float *fTP, int iTPCount)
{
	//数组fTP的结构:
	//fTp[3n] = fTime;
	//fTp[3n+1] = fLeftVal;
	//fTp[3n+2] = fRightVal;
	int i;
	TimePoint tp;
	memcpy(&tp, &fTP, sizeof(TimePoint));
	for ( i = 0; i < iTPCount; i++ )
	{
		tp.fTime = *(fTP++);
		vector<TimePoint>::iterator i = GetTimeSocket(tp.fTime);
		if ( i == m_vecTP.end() )
			m_vecTP.push_back(tp);
		else if ( i->fTime == tp.fTime )
		{
			i->fLeftVal = tp.fLeftVal;
			i->fRightVal = tp.fRightVal;
		}
		else
		{
			tp.fLeftVal = *(fTP++);
			tp.fRightVal = *(fTP++);
			m_vecTP.insert(i, tp);
		}
	}
}


inline vector<TimePoint>::iterator KTimePoint::GetTimeSocket(float fTime)
{
	vector<TimePoint>::iterator i = m_vecTP.begin();
	//int iIndex = 0;
	//int iSize = static_cast<int>(m_vecTP.size());
    /*TimePoint* tmp = new TimePoint[iSize];
	for ( ; iIndex < iSize; iIndex++ )
		*tmp = m_vecTP[iIndex];
	delete[] tmp;*/
	for ( ; i != m_vecTP.end() && ( i->fTime < fTime ) ; i++ )
	{
		//TimePoint tmp = *i;
	};
	return i;
}

inline void KTimePoint::Remove(int iIndex)
{
	vector<TimePoint>::iterator i = m_vecTP.begin();
	i += iIndex;
	m_vecTP.erase(i);
}

bool KTimePoint::SaveToFile(FILE* & fp)
{
	int i;
	for ( i = 0; i < static_cast<int>(m_vecTP.size()); i++ )
        fwrite(&m_vecTP[i], sizeof( TimePoint), 1, fp);
	return true;
}



void KTimePoint::Reset(void)
{
	m_vecTP.clear();
	KTimePoint();
}

void KTimePoint::Output(void)
{
	/*if (!fpDebug)
		return;
	int i;
	int iSize = m_vecTP.size();
	for ( i = 0; i < iSize; i++ )
	{
		fprintf(fpDebug, "TimePoint %d\nTime = %f\nLeft Value = %f\nRight Value = %f\n\n",
			i, m_vecTP[i].dwTime, m_vecTP[i].fLeftVal, m_vecTP[i].fRightVal);
	}*/
}
}