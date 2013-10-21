// File: Date.h
// Desc:

#ifndef DATE_H
#define DATE_H

#include"DLLAPI.h"

namespace Common
{

class DLLAPI Date
{
public:
	unsigned int m_Year;
	unsigned int m_Month;  // 1- 12
	unsigned int m_Day;  // 1 - 31

	unsigned int m_WeekDay;  // 1 - 7

public:
	Date(unsigned int Year = 1900, unsigned int Month = 1, unsigned int Day = 1, unsigned int WeekDay = 1): m_Year(Year), m_Month(Month), m_Day(Day), m_WeekDay(WeekDay) {}
	Date(const Date& d): m_Year(d.m_Year), m_Month(d.m_Month), m_Day(d.m_Day), m_WeekDay(d.m_WeekDay) {}
	Date& operator = (const Date& d);

	unsigned int GetYear() const { return m_Year; }
	unsigned int GetMonth() const { return m_Month; }
	unsigned int GetDay() const { return m_Day; }
};

// 全局函数

inline bool operator == (const Date& a, const Date& b)
{
	return ((a.m_Year == b.m_Year) && (a.m_Month == b.m_Month) && (a.m_Day == b.m_Day));
}

inline bool operator != (const Date& a, const Date& b)
{
	return ((a.m_Year != b.m_Year) || (a.m_Month != b.m_Month) || (a.m_Day != b.m_Day));
}

inline bool operator > (const Date& a, const Date& b)
{
	return ((a.m_Year > b.m_Year) && (a.m_Month > b.m_Month) && (a.m_Day > b.m_Day));
}

inline bool operator < (const Date& a, const Date& b)
{
	return ((a.m_Year < b.m_Year) && (a.m_Month < b.m_Month) && (a.m_Day < b.m_Day));
}

inline bool operator >= (const Date& a, const Date& b)
{
	return ((a.m_Year >= b.m_Year) && (a.m_Month >= b.m_Month) && (a.m_Day >= b.m_Day));
}

inline bool operator <= (const Date& a, const Date& b)
{
	return ((a.m_Year <= b.m_Year) && (a.m_Month <= b.m_Month) && (a.m_Day <= b.m_Day));
}

// 成员函数

inline Date& Date::operator = (const Date& d)
{
	if(&d != this)
	{
		m_Year = d.m_Year;
		m_Month = d.m_Month;
		m_Day = d.m_Day;

		m_WeekDay = d.m_WeekDay;
	}

	return *this;
}

}

#endif