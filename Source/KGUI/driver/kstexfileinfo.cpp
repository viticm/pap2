#include "stdafx.h"
#include "./kstexfileinfo.h"

namespace UI
{

	KSTexFileInfo::KSTexFileInfo()
		: m_dwImageID(0)
		, m_nFilePathID(-1)
	{
	}

	KSTexFileInfo::KSTexFileInfo(DWORD dwImageID, int nFilePathID)
		: m_dwImageID(dwImageID)
		, m_nFilePathID(nFilePathID)
	{
	}

}

