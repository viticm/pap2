#include "stdafx.h"
#include "./krldoodadframedata.h"
#include "../../common/kmathtools.h"
#include "../../Source/Common/SO3World/Src/KDoodad.h"

KRLDoodadFrameData::KRLDoodadFrameData()
: m_pDoodad(NULL)
{
	memset(&m_Current, 0, sizeof(m_Current));
	memset(&m_Previous, 0, sizeof(m_Previous));
}

void KRLDoodadFrameData::UpdateData()
{
	if (m_pDoodad == NULL)
		return;

	m_Previous = m_Current;
	
	m_Current.nX = m_pDoodad->m_nX;
	m_Current.nY = m_pDoodad->m_nY;
	m_Current.nZ = m_pDoodad->m_nZ;
	m_Current.fFaceDirection = YawRHToLH(ToSceneAngle(m_pDoodad->m_nFaceDirection));
}
