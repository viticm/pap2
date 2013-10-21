#include "header.h"
#include "KG3DAnimationCollector.h"

KG3DAnimationCollector::KG3DAnimationCollector(void)
{
}

KG3DAnimationCollector::~KG3DAnimationCollector(void)
{
	CleanUp();
}

void KG3DAnimationCollector::Attach(KG3DAnimationSupporter* pSupporter)
{
	assert(pSupporter);
	m_pSupporter = pSupporter;
}

void KG3DAnimationCollector::Dettach()
{
	m_pSupporter = NULL;
}


void KG3DAnimationCollector::CleanUp()
{
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		SAFE_DELETE_ARRAY(m_Data[i].pData);
	}
	m_Data.clear();
}
