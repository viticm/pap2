#include "stdafx.h"
#include "KListCtrl.h"
#include "KSceneSceneEditorDialogLogical.h"

BEGIN_MESSAGE_MAP(KListCtrl, CListCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void KListCtrl::OnPaint()
{
	CListCtrl::OnPaint();
	CRect r;
	CBrush NewBrush,*pOldBrush;
	COLORREF NewColor;
	
	int nCount = GetItemCount();
	
	CDC* pDC = GetDC();
	for (int i = 0; i < nCount; i++)
	{
		LogicDataItem* pItem = (LogicDataItem*)GetItemData(i);

		if (pItem)
		{
			pItem->GetColor(&NewColor);
			pItem->GetRect(&r);
			NewBrush.CreateSolidBrush(NewColor);
			pOldBrush = pDC->SelectObject(&NewBrush);
			KG_PROCESS_ERROR(pDC->Rectangle(r));
			pDC->SelectObject(pOldBrush);
			NewBrush.DeleteObject();
		}
	}
	ReleaseDC(pDC);

Exit0:
	;
}