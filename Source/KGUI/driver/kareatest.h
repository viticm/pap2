////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kareatest.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-8-3 16:29:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KAREATEST_H_
#define _INCLUDE_KAREATEST_H_

////////////////////////////////////////////////////////////////////////////////

//只有添加，不释放，如果大规模运用，加引用计数
class KAreaTester
{
    typedef std::map<std::string, int> KFileToIndex;

	class KAreaInfo 
	{
	public:
		KAreaInfo()
		{
			m_uWidth = 0;
			m_uHeight = 0;
			m_pbyData = NULL;
		};


		int PtInArea(float fX, float fY)
		{
			INT nX = (INT)(fX * m_uWidth);
			if (nX < 0)
				return false;
			if ((UINT)nX >= m_uWidth)
				return false;
			INT nY = (INT)(fY * m_uHeight);
			if (nY < 0)
				return false;
			if ((UINT)nY >= m_uHeight)
				return false;
			if (m_pbyData)
			{
				const INT nIndex = nX + nY * m_uWidth;
				if (m_pbyData[nIndex / 8] & (1 << (nIndex % 8)))
					return true;
			}
			return false;
		};

		int FromFile(LPCSTR pcszFile)
		{
			int nResult = false;
			IFile *pFile = NULL;
			KGLOG_PROCESS_ERROR(pcszFile);

			pFile = g_OpenFile(pcszFile);
			KGLOG_PROCESS_ERROR(pFile);

			pFile->Read(&m_uWidth, sizeof(m_uWidth));
			pFile->Read(&m_uHeight, sizeof(m_uHeight));

			SAFE_DELETE_ARRAY(m_pbyData);

			m_pbyData = new BYTE[(m_uWidth * m_uHeight + 7) / 8];
			KGLOG_PROCESS_ERROR(m_pbyData);

			pFile->Read(m_pbyData, (m_uWidth * m_uHeight + 7) / 8);

			nResult = true;
Exit0:
			SAFE_RELEASE(pFile);
			return nResult;
		};

		~KAreaInfo()
		{
			SAFE_DELETE_ARRAY(m_pbyData);
		};

private:
		UINT m_uWidth;
		UINT m_uHeight;
		BYTE *m_pbyData;
	};
	typedef std::vector<KAreaInfo *> InfoArray;
public:
	KAreaTester();
	~KAreaTester();

	int GetID(LPCSTR pcszFile);
	int PtInArea(int nID, float fX, float fY);

protected:
private:
	KFileToIndex m_File;
	InfoArray m_aInfo;
};

extern KAreaTester g_AreaTester;

#endif //_INCLUDE_KAREATEST_H_
