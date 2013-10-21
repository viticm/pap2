#include "StdAfx.h"
#include "NameGenerator.h"
#include "KGameCenterWrapper.h"
#include <fstream>


NameGenerator::NameGenerator(void) : m_nIndex(0)
{
}

NameGenerator::~NameGenerator(void)
{
}

bool NameGenerator::Init()
{
	int bRetCode = false;
	int bResult = false;
	std::string buffer;

	std::ifstream NameListFile("WhiteList.txt");
	KGLOG_PROCESS_ERROR(NameListFile.is_open());

	while (NameListFile >> buffer)
	{
		m_NameBuff += buffer;
	}

	m_TextFilter.reset(CreateTextFilterInterface());
	KGLOG_PROCESS_ERROR(m_TextFilter.get());

	bRetCode = m_TextFilter->LoadFilterFile("NameFilter.txt");
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_TextFilter->LoadWhiteListFile("WhiteList.txt");
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

TestTools::KRoleInfo NameGenerator::Generator()
{
	TestTools::KRoleInfo Role;
	sprintf(Role.szAccount, "testAcount%d", m_nIndex);
	std::string Name;
	while (true)
	{
		if ((m_nIndex + GCSETTING->m_nMinRoleNameLength + GCSETTING->m_nMinRoleNameLength % 2) > m_NameBuff.length())
		{
			break;
		}
		Name = m_NameBuff.substr(m_nIndex, GCSETTING->m_nMinRoleNameLength + GCSETTING->m_nMinRoleNameLength % 2);
		m_nIndex++;
		if (m_TextFilter->Check(Name.c_str()))
		{
			strcpy(Role.szRoleName, Name.c_str());
			break;
		}
	}
	if (m_nIndex + GCSETTING->m_nMinRoleNameLength + GCSETTING->m_nMinRoleNameLength % 2 > m_NameBuff.length())
	{
		m_nIndex = 0;
	}
	return Role;
}

NameGenerator* NameGenerator::Instance()
{
	static NameGenerator ng;
	return &ng;
}

std::string NameGenerator::IllegalName(int nLength)
{
	string Name;
	int Count = 15;
	do 
	{
		Name.clear();
		if (!Count--)
		{
			break;
		}
		for (int i = 0; i < nLength; i++)
		{
			char c = rand() % 80 + 37;
			Name.push_back(c);
		}
	} while (m_TextFilter->Check(Name.c_str()));
	return Name;
}

std::string NameGenerator::LegalName(int nLength)
{
	if (m_NameBuff.size() < 2)
	{
		return "";
	}
	string Name;
	int Count = 15;
	do 
	{
		Name.clear();
		if (!Count--)
		{
			break;
		}
		for (int i = 0; i < nLength; i++)
		{
			int nIndex = rand() % m_NameBuff.size();
			nIndex += nIndex % 2;
			if (nIndex + 1 >= m_NameBuff.length())
			{
				nIndex = 0;
			}
			Name.push_back(m_NameBuff[nIndex]);
			Name.push_back(m_NameBuff[nIndex + 1]);
		}
	} while (!m_TextFilter->Check(Name.c_str()));
	return Name;
}