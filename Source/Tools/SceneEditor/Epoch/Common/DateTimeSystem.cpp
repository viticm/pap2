// File: DateTimeSystem.cpp
// Desc:

#include"DateTimeSystem.h"

namespace Common
{

DateTimeSystem::DateTimeSystem(): m_bIsSupportHighResolutionCounter(false), m_Freq(0)
{
	if(QueryPerformanceFrequency((LARGE_INTEGER*)&m_Freq))
	{
		m_bIsSupportHighResolutionCounter = true;
	}
}

void DateTimeSystem::GetLocalDate(Date& d) const
{
	time_t ltime;
	time(&ltime);
	tm* local_DateTime = localtime(&ltime);  // 本地日期、时间

	d.m_Year = local_DateTime->tm_year + 1900;
	d.m_Month = local_DateTime->tm_mon + 1;
	d.m_Day = local_DateTime->tm_mday;
	d.m_WeekDay = (local_DateTime->tm_wday == 0)? 7: local_DateTime->tm_wday;  // Bible said, Sunday is the 7th day in a week. Why the west world makes it the first day?
}

void DateTimeSystem::GetLocalTime(Time& t) const
{
	time_t ltime;
	time(&ltime);
	tm* local_DateTime = localtime(&ltime);  // 本地日期、时间

	t.m_Hour = local_DateTime->tm_hour;
	t.m_Minute = local_DateTime->tm_min;
	t.m_Second = local_DateTime->tm_sec;
}

void DateTimeSystem::GetGMTDate(Date& d) const
{
	time_t ltime;
	time(&ltime);
	tm* gmt_DateTime = gmtime(&ltime);  // 格林尼治标准时间

	d.m_Year = gmt_DateTime->tm_year + 1900;
	d.m_Month = gmt_DateTime->tm_mon + 1;
	d.m_Day = gmt_DateTime->tm_mday;
	d.m_WeekDay = (gmt_DateTime->tm_wday == 0)? 7: gmt_DateTime->tm_wday;
}

void DateTimeSystem::GetGMTTime(Time& t) const
{
	time_t ltime;
	time(&ltime);
	tm* gmt_DateTime = gmtime(&ltime);  // 格林尼治标准时间

	t.m_Hour = gmt_DateTime->tm_hour;
	t.m_Minute = gmt_DateTime->tm_min;
	t.m_Second = gmt_DateTime->tm_sec;
}

}  // namespace