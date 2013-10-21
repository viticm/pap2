#ifndef _KINFO_GATHER_H_
#define _KINFO_GATHER_H_

#include "Global.h"

#define INFO_KEY_LEN		16
#define MAX_GET_INFO_COUNT	128

class KInfoGather
{
public:
	KInfoGather(void);
	virtual ~KInfoGather(void);

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL AddInfo(const char* pcszInfo);
	BOOL AddKey(const char* pcszKey, int nWeight);

	const char* GetInfo(const char* pcszExample);
	const char* GetSpecialInfo(const char* pcszDistrictName, 
		const char* pcszNpcName, const char* pcszPlayerName, int nKeyCount, ...);

private:
	struct KInformation
	{
		int		m_nAccessCount;
		int		m_nKeyRefCount;
		char	m_szDistrictName[_NAME_LEN];
		char	m_szNpcName[_NAME_LEN];
		char	m_szPlayerName[_NAME_LEN];
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
	KInfoKey*			m_pInfoKeyHead;

	struct ltstr
	{
		bool operator()(const char* X, const char* Y) const
		{
			return strncmp(X, Y, _NAME_LEN) < 0;
		}
	};

	typedef std::map<const char*, KInfoKey*, ltstr>	MAP_STR_2_KEY;
	MAP_STR_2_KEY		m_mapStrToKey;

	typedef std::multimap<const char*, KInformation*, ltstr> MAP_STR_2_INFO;
	MAP_STR_2_INFO		m_mapDistrict2Info;
	MAP_STR_2_INFO		m_mapNpc2Info;
	MAP_STR_2_INFO		m_mapPlayer2Info;

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
