#ifndef _KBBOX_H_
#define _KBBOX_H_

#include <float.h>
#include "KApplication.h"
#include "MathTools.h"

class KBBox
{
public:
    KBBox()
    {
        m_vecMin.x = FLT_MAX;
        m_vecMin.y = FLT_MAX;
        m_vecMin.z = FLT_MAX;

        m_vecMax.x = FLT_MIN;
        m_vecMax.y = FLT_MIN;
        m_vecMax.z = FLT_MIN;
    };

    KBBox(const D3DXVECTOR3& crPointA, const D3DXVECTOR3& crPointB, const D3DXVECTOR3& crPointC)
    {
        m_vecMin.x = min(crPointA.x, crPointB.x);
        m_vecMin.y = min(crPointA.y, crPointB.y);
        m_vecMin.z = min(crPointA.z, crPointB.z);

        m_vecMax.x = max(crPointA.x, crPointB.x);
        m_vecMax.y = max(crPointA.y, crPointB.y);
        m_vecMax.z = max(crPointA.z, crPointB.z);

        m_vecMin.x = min(m_vecMin.x, crPointC.x);
        m_vecMin.y = min(m_vecMin.y, crPointC.y);
        m_vecMin.z = min(m_vecMin.z, crPointC.z);

        m_vecMax.x = max(m_vecMax.x, crPointC.x);
        m_vecMax.y = max(m_vecMax.y, crPointC.y);
        m_vecMax.z = max(m_vecMax.z, crPointC.z);
    };

    inline void AddPoint(const D3DXVECTOR3& crPoint)
    {
        m_vecMin.x = min(m_vecMin.x, crPoint.x);
        m_vecMin.y = min(m_vecMin.y, crPoint.y);
        m_vecMin.z = min(m_vecMin.z, crPoint.z);

        m_vecMax.x = max(m_vecMax.x, crPoint.x);
        m_vecMax.y = max(m_vecMax.y, crPoint.y);
        m_vecMax.z = max(m_vecMax.z, crPoint.z);
    };

    inline BOOL IsIntersect(const KBBox& crBox)
    {
        BOOL bRetCode = false;

        bRetCode =  (FloatComp(m_vecMax.x, crBox.m_vecMin.x) < 0) || (FloatComp(crBox.m_vecMax.x, m_vecMin.x) < 0) ||
                    (FloatComp(m_vecMax.y, crBox.m_vecMin.y) < 0) || (FloatComp(crBox.m_vecMax.y, m_vecMin.y) < 0) ||
                    (FloatComp(m_vecMax.z, crBox.m_vecMin.z) < 0) || (FloatComp(crBox.m_vecMax.z, m_vecMin.z) < 0);

        return !bRetCode;
    }

public:
    D3DXVECTOR3 m_vecMin;
    D3DXVECTOR3 m_vecMax;
};

#endif // _KBBOX_H_
