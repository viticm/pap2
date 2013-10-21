#include "header.h"
#include "KG3DExportNodeSelector.h"

KG3DExportNodeSelector::KG3DExportNodeSelector(Interface* pIP)
{
	m_ip = pIP;
}

KG3DExportNodeSelector::~KG3DExportNodeSelector(void)
{
	CleanUp();
}

int KG3DExportNodeSelector::GetNumNodes()
{
	return static_cast<int>(m_Parts.size());
}

INode* KG3DExportNodeSelector::GetNode(unsigned int uIndex)
{
	if (uIndex >= GetNumNodes())
		return NULL;
	else
		return m_Parts[uIndex];
}

void KG3DExportNodeSelector::GetExportNode()
{
	if (!m_ip)
		return;

	m_Parts.clear();

	int nNumSel = m_ip->GetSelNodeCount();
	for (int i = 0; i < nNumSel; i++)
	{
		INode* pNode = m_ip->GetSelNode(i);
		m_Parts.push_back(pNode);
	}
}

void KG3DExportNodeSelector::CleanUp()
{
	m_Parts.clear();
	m_ip = NULL;
}


void KG3DExportNodeSelector::BeginExport()
{
	GetExportNode();
}

void KG3DExportNodeSelector::EndExport()
{
	return;
}