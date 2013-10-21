#pragma once

class KG3DBlackList
{
public:
	KG3DBlackList(void);
	~KG3DBlackList(void);
	void GetBlackList(FILE* fpFile);
	BOOL CheckBoneInList(const TCHAR* strBoneName);
	void CleanUp();
private:
	std::vector<std::string> m_BlackList;
};
