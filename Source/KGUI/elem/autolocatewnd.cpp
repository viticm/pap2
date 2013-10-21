/*****************************************************************************************
//	窗口自动定位
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-17
*****************************************************************************************/
#include "stdafx.h"
#include "./wndwindow.h"
#include "./autolocatewnd.h"
#include "../KGUI.h"

namespace UI
{

#define	CURSOR_HEIGHT		32

    int ALW_CorrectWndPosition
    ( 
        KWndWindow *pWnd, 
        const KWndWindow *pWndSurround, 
        int nCorrectType 
    )
    {
        int nResult     = false;
        int nRetCode    = false;
        RECT rcSurround;
        float fTempX = 0;
        float fTempY = 0;

        KG_PROCESS_ERROR(pWnd);

        if (!pWndSurround)
        {
            nRetCode = ALW_CorrectWndPosition(pWnd);
            KG_PROCESS_SUCCESS(nRetCode);
            KG_PROCESS_ERROR(nRetCode);
        }

        fTempX = pWndSurround->GetAbsX();
		fTempY = pWndSurround->GetAbsY();;

        rcSurround.left = (LONG)fTempX;
        rcSurround.top  = (LONG)fTempY;

        pWndSurround->GetSize(&fTempX, &fTempY);

        rcSurround.right  = rcSurround.left + (int)fTempX;
        rcSurround.bottom = rcSurround.top + (int)fTempY;

        nRetCode = ALW_CorrectWndPosition(pWnd, rcSurround, nCorrectType);
        KG_PROCESS_ERROR(nRetCode);

Exit1:
        nResult = true;
Exit0:
        return nResult;
    }

    static void DoCorrectCaclateRightLeft(
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
        )
    {
        if (rcSurround.right + static_cast<int>(fWndWidth) < fClientWidth)
            fWndX = static_cast<float>(rcSurround.right);
        else if (rcSurround.left - static_cast<int>(fWndWidth) > 0)
            fWndX = static_cast<float>(rcSurround.left) - fWndWidth;
        else
            fWndX = 0.0f;

        if (static_cast<float>(rcSurround.top) + fWndHeight < fClientHeight)
            fWndY = static_cast<float>(rcSurround.top);
        else
            fWndY = fClientHeight - fWndHeight - 1;
    }

   static void DoCorrectCaclateLeftRight(
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
        )
    {
        if (rcSurround.left - static_cast<int>(fWndWidth) > 0)
            fWndX = static_cast<float>(rcSurround.left) - fWndWidth;
        else if (rcSurround.right + static_cast<int>(fWndWidth) < fClientWidth)
            fWndX = static_cast<float>(rcSurround.right);

        else
            fWndX = 0.0f;

        if (static_cast<float>(rcSurround.top) + fWndHeight < fClientHeight)
            fWndY = static_cast<float>(rcSurround.top);
        else
            fWndY = fClientHeight - fWndHeight - 1;
    }

   static void DoCorrectCaclateBottomTop
   (
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
   )
   {
        if (rcSurround.bottom + static_cast<int>(fWndHeight) < fClientHeight)
            fWndY = static_cast<float>(rcSurround.bottom);
        else if (rcSurround.top - static_cast<int>(fWndHeight) > 0)
            fWndY = static_cast<float>(rcSurround.top) - fWndHeight;
        else
            fWndY = 0.0f;

        if (rcSurround.left + static_cast<int>(fWndWidth) < fClientWidth)
            fWndX = static_cast<float>(rcSurround.left);
        else
            fWndX = fClientWidth - fWndWidth - 1;
   }

   static void DoCorrectCaclateTopBottom
   (
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
   )
   {
        if (rcSurround.top - static_cast<int>(fWndHeight) > 0)
            fWndY = static_cast<float>(rcSurround.top) - fWndHeight;
        else if (rcSurround.bottom + static_cast<int>(fWndHeight) < fClientHeight)
            fWndY = static_cast<float>(rcSurround.bottom);
        else
            fWndY = 0.0f;

        if (rcSurround.left + static_cast<int>(fWndWidth) < fClientWidth)
            fWndX = static_cast<float>(rcSurround.left);
        else
            fWndX = fClientWidth - fWndWidth - 1;
   }

   static void DoCorrectCaclateRightLeftAndBottomTop
   (
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
   )
   {
        if (rcSurround.right + static_cast<int>(fWndWidth) < fClientWidth)
            fWndX = static_cast<float>(rcSurround.right);
        else if (rcSurround.left - static_cast<int>(fWndWidth) > 0)
            fWndX = static_cast<float>(rcSurround.left) - fWndWidth;
        else
            fWndX = 0.0f;

        if (rcSurround.bottom + static_cast<int>(fWndHeight) < fClientHeight)
            fWndY = static_cast<float>(rcSurround.bottom);
        else if (rcSurround.top - static_cast<int>(fWndHeight) > 0)
            fWndY = static_cast<float>(rcSurround.top) - fWndHeight;
        else
            fWndY = 0.0f;
   }

   static void DoCorrectCaclateCenter
   (
        float &fWndX, float &fWndY, 
        const float &fWndWidth, const float &fWndHeight,
        const float &fClientWidth, const float &fClientHeight,
        const RECT &rcSurround
   )
   {
       float fTemp = 0.0f;

       if (rcSurround.left + rcSurround.right < fClientWidth)
            DoCorrectCaclateRightLeft(
                fWndX, fTemp, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
       else
            DoCorrectCaclateLeftRight(
                fWndX, fTemp, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );

       if (rcSurround.top + rcSurround.bottom < fClientHeight)
            DoCorrectCaclateBottomTop(
                fTemp, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
       else
            DoCorrectCaclateTopBottom(
                fTemp, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );

   }

    int ALW_CorrectWndPosition
    ( 
        KWndWindow *pWnd, 
        const RECT &rcSurround, 
        int nCorrectType 
    )
    {
        int nResult         = false;
        int nRetCode        = false;
        float fWndX         = 0.0f;
        float fWndY         = 0.0f;
        float fWndWidth     = 0.0f;
        float fWndHeight    = 0.0f;
        float fClientWidth    = 0;
        float fClientHeight   = 0;

        KG_PROCESS_ERROR(pWnd);
        
        pWnd->GetSize(&fWndWidth, &fWndHeight);
		fClientWidth = g_pUI->m_WndStation.GetClientWidth();
		fClientHeight = g_pUI->m_WndStation.GetClientHeight();

        switch(nCorrectType) 
        {
        case ALW_CENTER:
            DoCorrectCaclateCenter(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
        	break;
        case ALW_LEFT_RIGHT:
            DoCorrectCaclateLeftRight(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        case ALW_RIGHT_LEFT:
            DoCorrectCaclateRightLeft(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        case ALW_TOP_BOTTOM:
            DoCorrectCaclateTopBottom(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        case ALW_BOTTOM_TOP:
            DoCorrectCaclateBottomTop(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        case ALW_RIGHT_LEFT_AND_BOTTOM_TOP:
            DoCorrectCaclateRightLeftAndBottomTop(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        default:
            DoCorrectCaclateRightLeft(
                fWndX, fWndY, fWndWidth, fWndHeight, 
                fClientWidth, fClientHeight, rcSurround
            );
            break;
        }

        pWnd->SetRelPos(fWndX, fWndY);

        nResult = true;
Exit0:
        return nResult;

    }

    int ALW_CorrectWndPosition( KWndWindow *pWnd, const POINT &ptSurround, int nCorrectType)
    {
        int nResult     = false;
        int nRetCode    = false;
        RECT rcSurround;

        KG_PROCESS_ERROR(pWnd);

        rcSurround.left     = rcSurround.right  = ptSurround.x;
        rcSurround.top      = rcSurround.bottom = ptSurround.y;
        
        nRetCode = ALW_CorrectWndPosition(pWnd, rcSurround, nCorrectType);
        KG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

    int ALW_CorrectWndPosition( KWndWindow *pWnd )
    {
        int nResult         = false;
        int nRetCode        = false;
        float fWndX         = 0.0f;
        float fWndY         = 0.0f;
        float fWndWidth     = 0.0f;
        float fWndHeight    = 0.0f;
        float fClientWidth  = 0.0f;
        float fClientHeight = 0.0f;

        KG_PROCESS_ERROR(pWnd);

        fWndX = pWnd->GetAbsX();
		fWndY = pWnd->GetAbsY();

        pWnd->GetSize(&fWndWidth, &fWndHeight);

        fClientWidth = g_pUI->m_WndStation.GetClientWidth();
		fClientHeight = g_pUI->m_WndStation.GetClientHeight();

        if (fWndX + fWndWidth > fClientWidth)
            fWndX = fClientWidth - fWndWidth;
        if (fWndX < 0.0f)
            fWndX = 0.0f;
        if (fWndY + fWndHeight > fClientHeight)
            fWndY = fClientHeight - fWndHeight;
        if (fWndY < 0.0f)
            fWndY = 0.0f;
        
        pWnd->SetRelPos(fWndX, fWndY);

        nResult = true;
Exit0:
        return nResult;
    }

}
