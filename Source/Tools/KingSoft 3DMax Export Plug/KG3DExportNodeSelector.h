#pragma once

class KG3DExportNodeSelector
{
public:
	KG3DExportNodeSelector(Interface* pIP);
	virtual ~KG3DExportNodeSelector();
	virtual void CleanUp();
	virtual int GetNumNodes();
	virtual INode* GetNode(unsigned int uIndex);
	virtual void BeginExport();
	virtual void EndExport();
protected:
	virtual void GetExportNode();
	Interface* m_ip;
	std::vector<INode*> m_Parts;
};
