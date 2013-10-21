#ifndef _KATTRIB_GROUP_H_
#define _KATTRIB_GROUP_H_

#define	MAX_ATTRIB_GROUP_NUM	1024

struct KAttribRate
{
	int		nAttribIndex;
	int		nRate;
};

class KAttribGroup
{
public:
	BOOL	Init(const char cszFileName[]);
	void	UnInit();
	int		GetAttribIndex(int nGroup);

private:
	typedef std::vector<KAttribRate>   ATTR_GROUP;
    typedef std::map<int, ATTR_GROUP>  ATTR_GROUP_MAP;

	ATTR_GROUP_MAP	m_AttrGroupMap;
};
#endif
