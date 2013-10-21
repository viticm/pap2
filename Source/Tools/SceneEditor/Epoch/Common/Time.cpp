// File: Time.cpp
// Desc:

#include"Time.h"

namespace Common
{

void Time::IncreaseSecond(unsigned int ui)
{
	unsigned int uiCarry = (unsigned int)(((unsigned short)m_Second + ui) / 60);
	m_Second = (unsigned int)(((unsigned short)m_Second + ui) % 60);
	
	unsigned int uiCarry2 = (unsigned int)(((unsigned short)m_Minute + uiCarry) / 60);
	m_Minute = (unsigned int)(((unsigned short)m_Minute + uiCarry) % 60);

	m_Hour = (unsigned int)(((unsigned short)m_Hour + uiCarry2) % 24);
}

void Time::DecreaseSecond(unsigned int ui)
{
	unsigned int uiBorrow = 0;
	while(m_Second < ui)  // 不够减，需借位
	{
		++uiBorrow;
	}
	m_Second = (unsigned int)((unsigned short)m_Second + uiBorrow * 60 - ui);

	unsigned int uiBorrow2 = 0;
	while(m_Minute < uiBorrow)  // 不够借，向Hour借位
	{
		++uiBorrow2;
	}
	m_Minute = (unsigned int)((unsigned short)m_Minute + uiBorrow2 * 60 - uiBorrow);

	unsigned int uiBorrow3 = 0;
	while(m_Hour < uiBorrow2)  // 不够借，向虚空借位
	{
		++uiBorrow3;
	}
	m_Hour = (unsigned int)((unsigned short)m_Hour + uiBorrow3 * 24 - uiBorrow2);
}

void Time::IncreaseMinute(unsigned int ui)
{
	unsigned int uiCarry2 = (unsigned int)(((unsigned short)m_Minute + ui) / 60);
	m_Minute = (unsigned int)(((unsigned short)m_Minute + ui) % 60);

	m_Hour = (unsigned int)(((unsigned short)m_Hour + uiCarry2) % 24);
}

void Time::DecreaseMinute(unsigned int ui)
{
	unsigned int uiBorrow2 = 0;
	while(m_Minute < ui)  // 不够借，向Hour借位
	{
		++uiBorrow2;
	}
	m_Minute = (unsigned int)((unsigned short)m_Minute + uiBorrow2 * 60 - ui);

	unsigned int uiBorrow3 = 0;
	while(m_Hour < uiBorrow2)  // 不够借，向虚空借位
	{
		++uiBorrow3;
	}
	m_Hour = (unsigned int)((unsigned short)m_Hour + uiBorrow3 * 24 - uiBorrow2);
}

void Time::IncreaseHour(unsigned int ui)
{
	m_Hour = (unsigned int)(((unsigned short)m_Hour + ui) % 24);
}

void Time::DecreaseHour(unsigned int ui)
{
	unsigned int uiBorrow3 = 0;
	while(m_Hour < ui)  // 不够借，向虚空借位
	{
		++uiBorrow3;
	}
	m_Hour = (unsigned int)((unsigned short)m_Hour + uiBorrow3 * 24 - ui);
}

}  // namespace