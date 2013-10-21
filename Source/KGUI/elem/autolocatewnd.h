/*****************************************************************************************
//	窗口自动定位
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-17
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef UI_ELEM_AUTOLOCAEWND_H
#define UI_ELEM_AUTOLOCAEWND_H

namespace UI
{

    class KWndWindow;

    enum
    {
        ALW_CENTER = 0,
        ALW_LEFT_RIGHT,
        ALW_RIGHT_LEFT,
        ALW_TOP_BOTTOM,
        ALW_BOTTOM_TOP,
        ALW_RIGHT_LEFT_AND_BOTTOM_TOP,
    
    };

    int ALW_CorrectWndPosition
    (
        KWndWindow *pWnd, 
        const KWndWindow *pWndSurround,
        int nCorrectType            
     );

     int ALW_CorrectWndPosition
     (
        KWndWindow *pWnd, 
        const RECT &rcSurround, 
        int nCorrectType
     );

     int ALW_CorrectWndPosition
     ( 
        KWndWindow *pWnd, 
        const POINT &ptSurround,
        int nCorrectType
     );

     int ALW_CorrectWndPosition
     ( 
         KWndWindow *pWnd 
     );
}

#endif // UI_ELEM_AUTOLOCAEWND_H

