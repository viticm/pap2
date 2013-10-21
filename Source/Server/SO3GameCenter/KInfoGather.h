#ifndef _KINFO_GATHER_H_
#define _KINFO_GATHER_H_

#define INFO_KEY_LEN		16
#define MAX_GET_INFO_COUNT	128

class KInfoGather
{
public:
	KInfoGather(void);
	virtual ~KInfoGather(void);

	BOOL AddInfo(const char* pcszInfo);
	const char* GetInfo(const char* pcszExample);
	BOOL AddKey(const char* pcszKey, int nWeight);

private:
	struct KInformation
	{
		int		m_nAccessCount;
		int		m_nKeyRefCount;
		int		m_nInfoLen;
		char	m_szInfo[0];
	};

	struct KInfoNode 
	{
		KInformation*	m_pInfo;
		KInfoNode*		m_pNext;
	};

	struct KInfoKey
	{
		int				m_nWeight;
		BYTE			m_byPatternPos;
		BYTE			m_byEndPos;
		char			m_szKey[INFO_KEY_LEN];
		char			m_NextTable[INFO_KEY_LEN];
		KInfoNode		m_InfoNode;
		KInfoKey*		m_pNext;
	};

	KInfoKey*	m_pInfoKeyHead;

private:
	BOOL CreateNextTable(const char* pcszPattern, char* pNextTable);

	template <class KEY_PROCESS_FUNC>	
	int AnalysisKey(const char* pcszText, KEY_PROCESS_FUNC& pFunc);

	struct KAddInfoProcessor
	{
		KInformation* m_pInfo;
		BOOL operator() (const char* pcszKeyPos, 
			int nKeyIndex, KInfoKey* pInfoKey);
	};

	struct KGetInfoProcessor
	{
		BOOL operator() (const char* pcszKeyPos, 
			int nKeyIndex, KInfoKey* pInfoKey);
		int			m_nCount;
		KInfoKey*	m_pResultList[MAX_GET_INFO_COUNT];
	};
};
#endif	//_KINFO_GATHER_H_