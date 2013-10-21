#include "stdafx.h"
#include "KRecordUITools.h"
#include "IRecPlayer.h"
#include "KG3DTypes.h"

KRecordUITools g_RecordUITools;

struct GlobleInfo
{
	int nObjNum;
	int nEventNum;
};

void KRecordUITools::FillListCtrl(IERecPlayer *pPlayer,
								  int nDeeps,
								  DWORD_PTR pPoint,
								  KGMemDateList& listctrlObj)
{
	listctrlObj.RemoveAll();
	LVCOLUMN lvcolumn;
	if (!listctrlObj.GetColumn(0,&lvcolumn))
		listctrlObj.InsertColumn(0, _T("属性名"));
	if (!listctrlObj.GetColumn(1,&lvcolumn))
		listctrlObj.InsertColumn(1, _T("属性值"));
	listctrlObj.SetColumnWidth(0, 150);
	listctrlObj.SetColumnWidth(1, 500);
	listctrlObj.SetBkColor(RGB(255, 255, 255));
	listctrlObj.SetBkColor(RGB(92, 92, 92));
	listctrlObj.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
	listctrlObj.SetHoverTime(100);
	if (nDeeps == 0)
	{
		KGMEMDATEELEMENT element[] =   
		{
			ELEMENT_TITLE("全局属性"),
			{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("对象总数")},
			{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("事件总数")},
			ELEMENT_END()
		};	

		GlobleInfo Info = {pPlayer->GetObjNum(), pPlayer->GetEventNum()};
		listctrlObj.BandDate((BYTE*)(&Info),element);
	}
	else if (nDeeps == 1)
	{

	}
	else if (nDeeps == 2)
	{

	}
	else if (nDeeps == 3)
	{
		EventRecord* pRecord = (EventRecord*)pPoint;
		KG_PROCESS_ERROR(pRecord);
		switch (pRecord->EventFunction)
		{
		case FT_NONE:
			break;
		case FT_KG3DMODEL_LOADFROMFILE:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("对象加载属性"),
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("对象实际类型")},
					{ELEMENT_AUTO, MAX_PATH,   MEM_TYPE_PATH, _T("加载文件路径")},
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("加载文件哈希")},
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("加载文件选项")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DSCENEEDITOR_LOADFROMFILE:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("场景加载属性"),
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("场景实际类型")},
					{ELEMENT_AUTO, MAX_PATH,   MEM_TYPE_PATH, _T("加载文件路径")},
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("加载文件哈希")},
					{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT, _T("加载文件选项")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_SETTRANSLATION:	
			{		
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置位置属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("位置x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("位置y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("位置z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_SETROTATION:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置旋转属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转z分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转w分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_SETROTATIONCENTER:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置旋转中心属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转中心x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转中心y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("旋转中心z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_SETSCALING:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置缩放属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_SETSCALINGCENTER:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置缩放中心属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放中心x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放中心y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放中心z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_SETSCALINGROTATION:
			{	
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置缩放旋转属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放旋转x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放旋转y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放旋转z分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("缩放旋转w分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_RESETTRANSFORM:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("重置位置缩放旋转"),
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DMODEL_BINDTOSOCKET:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("绑定关节属性"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("绑定到对象ID")},
					{ELEMENT_AUTO, MAX_PATH,   MEM_TYPE_PATH, _T("绑定关节名称")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_BINDTOBONE:	
			{				
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("绑定骨骼属性"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("绑定到对象ID")},
					{ELEMENT_AUTO, MAX_PATH,   MEM_TYPE_PATH, _T("绑定骨骼名称")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_ATTACHMODEL:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("绑定对象属性"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("即将绑定对象ID")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_DETCHMODEL:		
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("绑定对象属性"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("即将绑定对象ID")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_PLAYANIMATION:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("播放动画属性"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("播放类型")},
					{ELEMENT_AUTO, MAX_PATH,        MEM_TYPE_PATH, _T("动画路径")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("播放速度")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("开始时间")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_PALYSPLITANIMATION:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("播放SPLIT动画属性"),
					{ELEMENT_AUTO, MAX_PATH,        MEM_TYPE_PATH, _T("动画路径")},
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("SPLIT类型")},
					{ELEMENT_AUTO, sizeof(BOOL),    MEM_TYPE_BOOL, _T("是否保持旧的")},
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("附加信息")},
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("播放类型")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("播放速度")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("开始时间")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_LOADMATERIALFROMFILE:		
			{				
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("加载材质选项"),
					{ELEMENT_AUTO, MAX_PATH,        MEM_TYPE_PATH, _T("材质路径")},
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("材质选项")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_SETALPHA:		
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置对象透明属性"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("设置透明度")},
					{ELEMENT_AUTO, sizeof(BOOL),   MEM_TYPE_BOOL, _T("是否使用特殊模式")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_SETCALLBACKRENDER:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置对象回调函数"),
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DMODEL_DESTRUCT:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("对象析构"),
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DSCENEEDITOR_DESTRUCT:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("场景析构"),
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:	
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("加入渲染对象"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("对象ID")},
					{ELEMENT_AUTO, sizeof(INT),     MEM_TYPE_INT, _T("类别")},
					{ELEMENT_AUTO, sizeof(BOOL),   MEM_TYPE_BOOL, _T("是否可选择")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
			{
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("删除渲染对象"),
					{ELEMENT_AUTO, sizeof(DWORD),   MEM_TYPE_DWORD, _T("对象ID")},
					{ELEMENT_AUTO, sizeof(INT),     MEM_TYPE_INT, _T("类别")},
					{ELEMENT_AUTO, sizeof(BOOL),   MEM_TYPE_BOOL, _T("是否可选择")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);
			}
			break;
		case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:

			break;
		case FT_KG3DCAMERA_UPDATECAMERA:	

			break;
		case FT_KG3DCAMERA_SETPOSITION:	
			{	
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置镜头位置"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头位置x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头位置y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头位置z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DCAMERA_SETLOOKATPOSITION:		
			{	
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置镜头目标位置"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头目标位置x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头目标位置y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头目标位置z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DCAMERA_SETUPDIRECTION:		
			{	
				KGMEMDATEELEMENT element[] =   
				{
					ELEMENT_TITLE("设置镜头上方向"),
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头上方向x分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头上方向y分量")},
					{ELEMENT_AUTO, sizeof(FLOAT),   MEM_TYPE_FLOAT, _T("镜头上方向z分量")},
					ELEMENT_END()
				};	
				listctrlObj.BandDate((BYTE*)(pRecord->AParameters.pData),element);	
			}
			break;
		case FT_KG3DCAMERA_SETGLOBALPERSPECTIVE:		

			break;
		case FT_KG3DCAMERA_SETGLOBALORTHOGONAL:

			break;
		case FT_KG3DCAMERA_SETTRACKINFO:

			break;	
		default:
			break;
		}		
	}
Exit0:
	;
}

void KRecordUITools::FillEvent(IERecPlayer *pPlayer, HTREEITEM& hItem, CTreeCtrl& ListTree)
{
	if (ListTree.ItemHasChildren(hItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = ListTree.GetChildItem(hItem);
		if (hChildItem != NULL)
		{
			CString strName = ListTree.GetItemText(hChildItem);
			if (strName != "Test")
				return;
		}		
		while (hChildItem != NULL)
		{
			hNextItem = ListTree.GetNextItem(hChildItem, TVGN_NEXT);
			ListTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
	RecObj* pObj = (RecObj*)(ListTree.GetItemData(hItem));
	if (pObj)
	{
		EventRecord* pEventRecord = pObj->pEventHead;
		string strEventType = "";
		char szMaxName[MAX_PATH];
		while (pEventRecord != NULL)
		{					
			GetDesByID(strEventType,pEventRecord->EventFunction);
			sprintf_s(szMaxName,"Time:%d,Frame:%d,Event:%s",pEventRecord->dwTime,pEventRecord->dwFrame,strEventType.c_str());
			HTREEITEM hEventParent = ListTree.InsertItem(szMaxName,hItem,TVI_LAST);
			ListTree.SetItemData(hEventParent,(DWORD_PTR)pEventRecord);
			pEventRecord = pEventRecord->pNextEvent;
		}
	}
}

HRESULT KRecordUITools::GetDesByID(string& strDes,FunctionType Ftype)
{
	switch (Ftype)
	{
	case FT_NONE:
		strDes = "FT_NONE";
		break;
	case FT_KG3DMODEL_LOADFROMFILE:
		strDes = "FT_KG3DMODEL_LOADFROMFILE";
		break;
	case FT_KG3DSCENEEDITOR_LOADFROMFILE:
		strDes = "FT_KG3DSCENEEDITOR_LOADFROMFILE";
		break;
	case FT_KG3DMODEL_SETTRANSLATION:		
		strDes = "FT_KG3DMODEL_SETTRANSLATION";
		break;
	case FT_KG3DMODEL_SETROTATION:		
		strDes = "FT_KG3DMODEL_SETROTATION";
		break;
	case FT_KG3DMODEL_SETROTATIONCENTER:		
		strDes = "FT_KG3DMODEL_SETROTATIONCENTER";
		break;
	case FT_KG3DMODEL_SETSCALING:	
		strDes = "FT_KG3DMODEL_SETSCALING";
		break;
	case FT_KG3DMODEL_SETSCALINGCENTER:		
		strDes = "FT_KG3DMODEL_SETSCALINGCENTER";
		break;
	case FT_KG3DMODEL_SETSCALINGROTATION:		
		strDes = "FT_KG3DMODEL_SETSCALINGROTATION";
		break;
	case FT_KG3DMODEL_RESETTRANSFORM:
		strDes = "FT_KG3DMODEL_RESETTRANSFORM";
		break;
	case FT_KG3DMODEL_BINDTOSOCKET:	
		strDes = "FT_KG3DMODEL_BINDTOSOCKET";
		break;
	case FT_KG3DMODEL_BINDTOBONE:		
		strDes = "FT_KG3DMODEL_BINDTOBONE";
		break;
	case FT_KG3DMODEL_ATTACHMODEL:		
		strDes = "FT_KG3DMODEL_ATTACHMODEL";
		break;
	case FT_KG3DMODEL_DETCHMODEL:		
		strDes = "FT_KG3DMODEL_DETCHMODEL";
		break;
	case FT_KG3DMODEL_PLAYANIMATION:		
		strDes = "FT_KG3DMODEL_PLAYANIMATION";
		break;
	case FT_KG3DMODEL_PALYSPLITANIMATION:	
		strDes = "FT_KG3DMODEL_PALYSPLITANIMATION";
		break;
	case FT_KG3DMODEL_LOADMATERIALFROMFILE:		
		strDes = "FT_KG3DMODEL_LOADMATERIALFROMFILE";
		break;
	case FT_KG3DMODEL_SETALPHA:		
		strDes = "FT_KG3DMODEL_SETALPHA";
		break;
	case FT_KG3DMODEL_SETCALLBACKRENDER:
		strDes = "FT_KG3DMODEL_SETCALLBACKRENDER";
		break;
	case FT_KG3DMODEL_DESTRUCT:
		strDes = "FT_KG3DMODEL_DESTRUCT";
		break;
	case FT_KG3DSCENEEDITOR_DESTRUCT:
		strDes = "FT_KG3DSCENEEDITOR_DESTRUCT";
		break;
	case FT_KG3DSCENEEDITOR_ADDRENDERENTITY:	
		strDes = "FT_KG3DSCENEEDITOR_ADDRENDERENTITY";
		break;
	case FT_KG3DSCENEEDITOR_REMOVERENDERENTITY:
		strDes = "FT_KG3DSCENEEDITOR_REMOVERENDERENTITY";
		break;
	case FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW:
		strDes = "FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW";
		break;
	case FT_KG3DCAMERA_UPDATECAMERA:	
		strDes = "FT_KG3DCAMERA_UPDATECAMERA";
		break;
	case FT_KG3DCAMERA_SETPOSITION:		
		strDes = "FT_KG3DCAMERA_SETPOSITION";
		break;
	case FT_KG3DCAMERA_SETLOOKATPOSITION:		
		strDes = "FT_KG3DCAMERA_SETLOOKATPOSITION";
		break;
	case FT_KG3DCAMERA_SETUPDIRECTION:		
		strDes = "FT_KG3DCAMERA_SETUPDIRECTION";
		break;
	case FT_KG3DCAMERA_SETGLOBALPERSPECTIVE:		
		strDes = "FT_KG3DCAMERA_SETGLOBALPERSPECTIVE";
		break;
	case FT_KG3DCAMERA_SETGLOBALORTHOGONAL:
		strDes = "FT_KG3DCAMERA_SETGLOBALORTHOGONAL";
		break;
	case FT_KG3DCAMERA_SETTRACKINFO:
		strDes = "FT_KG3DCAMERA_SETTRACKINFO";
		break;	
	default:
		strDes = "DEFAULT";
		break;
	}
	return S_OK;
}

void KRecordUITools::FillListTree(IERecPlayer *pPlayer, CTreeCtrl& ListTree)
{
	HRESULT hResult = E_FAIL;
	string strEventType = "";
	ListTree.DeleteAllItems();
	DWORD dwID = 0;
	RecObj *pObj = NULL;
	HTREEITEM hParent = ListTree.InsertItem("All Objects",TVI_ROOT);
	HTREEITEM hSceneEditorParent = ListTree.InsertItem("SceneEditor",hParent,TVI_LAST);
	HTREEITEM hCameraParent = ListTree.InsertItem("Camera",hParent,TVI_LAST);
	HTREEITEM hModelParent = ListTree.InsertItem("Model",hParent,TVI_LAST);
	HTREEITEM hModelSTParent = ListTree.InsertItem("ModelST",hParent,TVI_LAST);
	HTREEITEM hSFXParent = ListTree.InsertItem("SFX",hParent,TVI_LAST);

	hResult = pPlayer->GetFirstObj(&dwID, &pObj);
	while (SUCCEEDED(hResult))
	{
		char szMaxName[MAX_PATH];
		HTREEITEM hMyObjParent;
		sprintf_s(szMaxName,"ID:%d,Name:%s", dwID,pObj->ObjName.c_str());
		switch (pObj->ObjType)
		{
		case TT_MODEL:
			{
				hMyObjParent = ListTree.InsertItem(szMaxName,hModelParent,TVI_LAST);
				ListTree.SetItemData(hMyObjParent,(DWORD_PTR)(pObj));
				ListTree.InsertItem("Test",hMyObjParent,TVI_LAST);
			}
			break;
		case TT_TRACKCAMERA:
			{
				hMyObjParent = ListTree.InsertItem(szMaxName,hCameraParent,TVI_LAST);
				ListTree.SetItemData(hMyObjParent,(DWORD_PTR)(pObj));
				ListTree.InsertItem("Test",hMyObjParent,TVI_LAST);
			}
			break;
		case TT_MODELST:
			{
				hMyObjParent = ListTree.InsertItem(szMaxName,hModelSTParent,TVI_LAST);
				ListTree.SetItemData(hMyObjParent,(DWORD_PTR)(pObj));
				ListTree.InsertItem("Test",hMyObjParent,TVI_LAST);
			}
			break;
		case TT_SCENEEDITOR:
			{
				hMyObjParent = ListTree.InsertItem(szMaxName,hSceneEditorParent,TVI_LAST);
				ListTree.SetItemData(hMyObjParent,(DWORD_PTR)(pObj));
				ListTree.InsertItem("Test",hMyObjParent,TVI_LAST);
			}
			break;
		case TT_SFX:
			{
				hMyObjParent = ListTree.InsertItem(szMaxName,hSFXParent,TVI_LAST);
				ListTree.SetItemData(hMyObjParent,(DWORD_PTR)(pObj));
				ListTree.InsertItem("Test",hMyObjParent,TVI_LAST);
			}
			break;
		default:
			break;
		}
		hResult = pPlayer->GetNextObj(&dwID, &pObj);
	}
}