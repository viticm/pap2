#pragma once
#include <vector>
using namespace std;

namespace ModelSFX
{

//_Time_Point因为有跳变的可能所以使用了左值右值
struct TimePoint
{
	float fTime;
	float fLeftVal;
	float fRightVal;
};

class KTimePoint
{
public:
	//friend class KParticleSystem;
	KTimePoint();
	~KTimePoint();
	KTimePoint(float* fTP, int iTPCount = 1);
	KTimePoint(float fTime, float fLeftVal, float fRightVal);
	KTimePoint(const TimePoint *tp, int iTPCount = 1);
	//得到时间点的个数
	int Size() { return static_cast<int>(m_vecTP.size()); }
	
	//加入预先储存好的数组
	void Insert(float *fTP, int iTPCount = 1);
	
	void Insert( const TimePoint *pTP, int iTPCount = 1);

	//按位置移除TIMEPOINT, 从0开始
	inline void Remove( int iIndex);
	
	//得到在某一个时间的值
	float GetValue( float fTime);

	vector<TimePoint> m_vecTP;
private:
	vector<TimePoint>::iterator GetTimeSocket( float fTime);
	FILE* fpDebug;
public:
	bool SaveToFile(FILE* & fp);
	void Reset(void);
	void Output(void);
};
}