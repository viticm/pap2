// File: Time.h
// Desc:

#ifndef MYTIME_H
#define MYTIME_H

#include"DLLAPI.h"

namespace Common
{

class DLLAPI Time
{
public:
	unsigned int m_Hour;  // 0 - 23
	unsigned int m_Minute;  // 0 - 59
	unsigned int m_Second;  // 0 - 59

private:
	void IncreaseSecond(unsigned int ui);  // So many functions, just for convenience.
	void DecreaseSecond(unsigned int ui);
	void IncreaseMinute(unsigned int ui);
	void DecreaseMinute(unsigned int ui);
	void IncreaseHour(unsigned int ui);
	void DecreaseHour(unsigned int ui);

public:
	Time(unsigned int Hour = 0, unsigned int Minute = 0, unsigned int Second = 0): m_Hour(Hour), m_Minute(Minute), m_Second(Second) {}
	Time(const Time& t): m_Hour(t.m_Hour), m_Minute(t.m_Minute), m_Second(t.m_Second) {}
	Time& operator = (const Time& t);

	Time& operator ++();  // 前置
	Time& operator --();
	Time operator ++(int);  // 后置
	Time operator --(int);
	Time& operator +=(unsigned int ui);
	Time& operator -=(unsigned int ui);
	Time& operator +=(const Time& t);
	Time& operator -=(const Time& t);
};

// 全局函数

inline bool operator == (const Time& a, const Time& b)
{
	return ((a.m_Hour == b.m_Hour) && (a.m_Minute == b.m_Minute) && (a.m_Second == b.m_Second));
}

inline bool operator != (const Time& a, const Time& b)
{
	return ((a.m_Hour != b.m_Hour) || (a.m_Minute != b.m_Minute) || (a.m_Second != b.m_Second));
}

inline bool operator > (const Time& a, const Time& b)
{
	return ((a.m_Hour > b.m_Hour) || (a.m_Hour == b.m_Hour && a.m_Minute > b.m_Minute) || (a.m_Hour == b.m_Hour && a.m_Minute == b.m_Minute && a.m_Second > b.m_Second));
}

inline bool operator < (const Time& a, const Time& b)
{
	return ((a.m_Hour < b.m_Hour) || (a.m_Hour == b.m_Hour && a.m_Minute < b.m_Minute) || (a.m_Hour == b.m_Hour && a.m_Minute == b.m_Minute && a.m_Second < b.m_Second));
}

inline bool operator >= (const Time& a, const Time& b)
{
	return ((a.m_Hour >= b.m_Hour) || (a.m_Hour == b.m_Hour && a.m_Minute >= b.m_Minute) || (a.m_Hour == b.m_Hour && a.m_Minute == b.m_Minute && a.m_Second >= b.m_Second));
}

inline bool operator <= (const Time& a, const Time& b)
{
	return ((a.m_Hour <= b.m_Hour) || (a.m_Hour == b.m_Hour && a.m_Minute <= b.m_Minute) || (a.m_Hour == b.m_Hour && a.m_Minute == b.m_Minute && a.m_Second <= b.m_Second));
}

// 成员函数

inline Time& Time::operator = (const Time& t)
{
	if(&t != this)
	{
		m_Hour = t.m_Hour;
		m_Minute = t.m_Minute;
		m_Second = t.m_Second;
	}

	return *this;
}

inline Time& Time::operator ++()
{
	IncreaseSecond(1);
	return *this;
}

inline Time& Time::operator --()
{
	DecreaseSecond(1);
	return *this;
}

inline Time Time::operator ++(int)
{
	Time ret = *this;
	IncreaseSecond(1);
	return ret;
}

inline Time Time::operator --(int)
{
	Time ret = *this;
	DecreaseSecond(1);
	return ret;
}

inline Time& Time::operator +=(unsigned int ui)
{
	IncreaseSecond(ui);
	return *this;
}

inline Time& Time::operator -=(unsigned int ui)
{
	DecreaseSecond(ui);
	return *this;
}

inline Time& Time::operator +=(const Time& t)
{
	IncreaseSecond(t.m_Second);
	IncreaseMinute(t.m_Minute);
	IncreaseHour(t.m_Hour);

	return *this;
}

inline Time& Time::operator -=(const Time& t)
{
	DecreaseSecond(t.m_Second);
	DecreaseMinute(t.m_Minute);
	DecreaseHour(t.m_Hour);

	return *this;
}

}

#endif