////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectPropertyEditDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-6 20:11:05
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGObjectPropertyEditDlg.h"
//#include "KG3dsceneObjectEditor.h"
#include "KSceneObjectEditorPanelView.h"
#include "KGBuildIniFileDlg.h"
#include "KGNPCAniEditSettingDlg.h"
//#include "KG3DObjectPropertyDefine.h"
#include "KGObjectPropertyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGObjectPropertyEditDlg, CDialog)

KGObjectPropertyEditDlg::KGObjectPropertyEditDlg(
	PROPERTY_EDIT_TYPE eType,
	KSceneObjectEditorPanelView* pView,
	CWnd* pParent
): CDialog(KGObjectPropertyEditDlg::IDD, pParent)
{
	m_eEditType = eType;
	m_pView     = pView;
}

KGObjectPropertyEditDlg::~KGObjectPropertyEditDlg()
{
}

void KGObjectPropertyEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeObjectView);
	DDX_Control(pDX, IDC_LIST_PROPERTY, m_listProperty);
	DDX_Control(pDX, IDC_OK, m_bnOK);
	DDX_Control(pDX, IDC_SAVE, m_bnSave);
}


BEGIN_MESSAGE_MAP(KGObjectPropertyEditDlg, CDialog)
	ON_NOTIFY(
		TVN_SELCHANGED, IDC_TREE_PROPERTY, OnTvnSelchangedObjectPropertyTree
	)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROPERTY, OnNMDblclkObjectPropertyList)
	ON_WM_SIZE()
	//ON_COMMAND(ID_SETTER, OnSetting)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROPERTY, OnNMRclickObjectPropertyList)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PROPERTY, OnNMRclickObjectPropertyTree)
END_MESSAGE_MAP()

int KGObjectPropertyEditDlg::Init()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditDlg::OnNMRclickObjectPropertyList(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	POSITION pos = m_listProperty.GetFirstSelectedItemPosition();
	int nItem    = m_listProperty.GetNextSelectedItem(pos);
	CString szSection = "";

	if (nItem != -1)
	{
		CString szSelName       = m_listProperty.GetItemText(nItem,0);
		CString szSelValue      = m_listProperty.GetItemText(nItem,1);
		int		nValueType		= (int)m_listProperty.GetItemData(nItem);

		switch(m_eEditType)
		{
		case PROPERTY_EDIT_TYPE_NPC_ANI :
			{
				//
			}
			break;
		default :;
		}
	}
}

void KGObjectPropertyEditDlg::OnNMRclickObjectPropertyTree(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	DWORD dwRetCode = false;
	POINT pt;
	CString szName;
	GetCursorPos(&pt);
	CPoint point(pt.x, pt.y);
	m_treeObjectView.ScreenToClient(&point);

	HTREEITEM hItem = m_treeObjectView.HitTest(point);
	HTREEITEM hSub  = m_treeObjectView.HitTest(point);
	KG_PROCESS_ERROR(hItem);
	m_treeObjectView.SelectItem(hItem);

	do
	{
		szName =
			m_treeObjectView.GetItemText(hItem) + "\\" + szName;
		hItem  = m_treeObjectView.GetParentItem(hItem);
	}
	while (hItem);

	szName = m_szDirectory + szName;

	dwRetCode = GetFileAttributes(szName);
	if (dwRetCode == FILE_ATTRIBUTE_DIRECTORY)
	{
		KGBuildIniFileDlg dlg(&szName, this);
		if (dlg.DoModal() == IDOK)
		{
			CString szPointName = dlg.GetName();
			KG_PROCESS_ERROR(szPointName != "");
			if (dlg.IsBuildedAFile())
			{
				szPointName = szPointName + ".ini";
			}
			HTREEITEM hCurItme =
				m_treeObjectView.InsertItem(szPointName, hSub);
			KG_PROCESS_ERROR(hCurItme);
			m_treeObjectView.SortChildren(hSub);
			m_treeObjectView.SelectItem(hCurItme);
		}
	}

Exit0:
	return;
}

void KGObjectPropertyEditDlg::OnSetting()
{
	/*
	TCHAR szAbsName[MAX_PATH];
	TCHAR szRetName[MAX_PATH];

	if (FAILED(
		g_pFileFinder->FindFile(
		szAbsName, szRetName, "ObjectPropertyEditConfig.ini"
		))
	)
	{
		return;
	}
	*/

	KGProjectEditConfigSettingDlg configDlg(m_eEditType, this);
	if (configDlg.DoModal() == IDOK)
	{
		LoadConfig(configDlg.GetFilePath().GetBuffer());
		UpdateDataTree();
		ShowPropertyTable(m_szNamePlate, m_szTemplate, m_szCurIniFileName);
	}

}

BOOL KGObjectPropertyEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

//	TCHAR szAbsName[MAX_PATH];
//	TCHAR szRetName[MAX_PATH];
//	TCHAR szValue[MAX_PATH];

	m_szCurIniFileName = "";
	m_pIniFile  = NULL;
	m_bModify   = FALSE;
	/*
	while (FAILED(
		g_pFileFinder->FindFile(
			szAbsName, szRetName, "ObjectPropertyEditConfig.ini"
		))
	)
	{
		KGProjectEditConfigSettingDlg configDlg(m_eEditType, this);
		configDlg.DoModal();
	}

	if (!LoadConfig(szAbsName))
	{
		return FALSE;
	}
*/

	m_treeObjectView.SetBkColor(RGB( 96,  96, 160));
	m_treeObjectView.SetTextColor(RGB(255, 255, 255));
	m_treeObjectView.SetLineColor(RGB(255, 255, 255));

	DWORD dwStyle = GetWindowLong(m_treeObjectView.m_hWnd,GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeObjectView.m_hWnd,GWL_STYLE,dwStyle);

	dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle |= WS_SIZEBOX;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

    return UpdateDataTree();
}

int KGObjectPropertyEditDlg::UpdateDataTree()
{
	m_treeObjectView.DeleteAllItems();
	CString szPathRoot;
	switch(m_eEditType)
	{
		/*
	case PROPERTY_EDIT_TYPE_OBJECT :
		SetWindowText("物件属性管理器 [物件属性]");
		szPathRoot = CString(OBJECT_OBJECT_PATH);
		break;
	case PROPERTY_EDIT_TYPE_NPC :
		SetWindowText("物件属性管理器 [NPC属性]");
		szPathRoot = CString(OBJECT_NPC_PATH);
		break;
	case PROPERTY_EDIT_TYPE_PROP :
		SetWindowText("物件属性管理器 [道具属性]");
		szPathRoot = CString(OBJECT_PROP_PATH);
		break;
	case PROPERTY_EDIT_TYPE_MAGIC :
		SetWindowText("物件属性管理器 [魔法属性]");
		szPathRoot = CString(OBJECT_MAGIC_PATH);
		break;
	case PROPERTY_EDIT_TYPE_EFFECT :
		SetWindowText("物件属性管理器 [特效属性]");
		szPathRoot = CString(OBJECT_EFFECT_PATH);
		break;
	case PROPERTY_EDIT_TYPE_SKILL :
		SetWindowText("物件属性管理器 [技能属性]");
		szPathRoot = CString(OBJECT_SKILL_PATH);
		break;
		*/
	case PROPERTY_EDIT_TYPE_EQUIP :
		m_szWindowTextt = "物件属性管理器 [装备配置表]";
		SetWindowText(m_szWindowTextt);
		szPathRoot = m_szDirectory;
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		m_szWindowTextt = "物件属性管理器 [主角动作配置表]";
		SetWindowText(m_szWindowTextt);
		szPathRoot = m_szDirectory;
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		m_szWindowTextt = "物件属性管理器 [NPC动作配置表]";
		SetWindowText(m_szWindowTextt);
		szPathRoot = m_szDirectory;
		break;
	default:;
	}
	//szPathRoot = szPathRoot + "\\";

	GetRootPoint(szPathRoot);
	GetChildPoint(m_treeRoot, szPathRoot, TYPE_FIRST);

	m_treeObjectView.Expand(TVI_ROOT, TVE_EXPAND);

	//m_listProperty.SetItemHeight(16);
	m_listProperty.InsertColumn(0, "物件属性名");
	m_listProperty.InsertColumn(1, "物件属性值");
	m_listProperty.SetColumnWidth(0, 160);
	m_listProperty.SetColumnWidth(1, 662);

	return TRUE;
}

int KGObjectPropertyEditDlg::LoadConfig(const TCHAR pcszAbsName[])
{
	int nResult  = false;
	int nRetCode = false;

	TCHAR     szValue[MAX_PATH] = {0};
	CString   szSection;
	IIniFile* pIniFile = NULL;

	CString szPath      = /*g_Work_Directory +*/ CString("\\");
	CString szNamePlate = szPath + "nameplate.ini";
	CString szTemplate  = szPath + "template.ini";

	ASSERT(pcszAbsName);
	KG_PROCESS_ERROR(pcszAbsName);

	pIniFile = g_OpenIniFile(pcszAbsName);
	KG_PROCESS_ERROR(pIniFile);

	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_EQUIP :
		szSection = "EQUIP";
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		szSection = "NPC_ANI";
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		szSection = "PLAYER_ANI";
		break;
	default:
		KG_PROCESS_ERROR(false);
		break;
	}

	pIniFile->GetString(
		szSection.GetBuffer(), "Directory", szPath.GetBuffer(),
		szValue, sizeof(szValue)
	);
	m_szDirectory = CString(szValue);

	pIniFile->GetString(
		szSection.GetBuffer(), "Nameplate", szNamePlate.GetBuffer(),
		szValue, sizeof(szValue)
	);
	m_szNamePlate = CString(szValue);

	pIniFile->GetString(
		szSection.GetBuffer(), "Template", szTemplate.GetBuffer(),
		szValue, sizeof(szValue)
	);

	m_szTemplate = CString(szValue);

	nResult = true;
Exit0:
	if (pIniFile)
	{
		pIniFile->Release();
	}
	return nResult;
}
/*

int KGObjectPropertyEditDlg::UpdateDataObjectProperty(NPC_PROPERTY* lpNpc)
{
	OBJECT_PROPERTY* lpObj = lpNpc;
	UpdateDataObjectProperty(lpObj);
	return TRUE;
}
*/

/*
int KGObjectPropertyEditDlg::UpdateDataObjectProperty(OBJECT_PROPERTY* lpObj)
{/ *
	m_listProperty.DeleteAllItems();

	CString szTemp;
	UINT uIndex = 0;

	// 标示相关
	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "标示相关", ITEM_TYPE_TITLE);
	m_listProperty.SetItemData(uIndex, VALUE_TITLE);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件名");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szName);
	m_listProperty.SetItemData(uIndex, VALUE_STRING);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "名字后缀");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szSuffix);
	m_listProperty.SetItemData(uIndex, VALUE_STRING);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "说明");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szNote);
	m_listProperty.SetItemData(uIndex, VALUE_STRING);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "类别");
	switch(lpObj->nType)
	{
	case REPRESENTOBJECT_DEFAULT:
		m_listProperty.SetItemText(uIndex, 1, "物件");
		break;
	case REPRESENTOBJECT_NPC:
		m_listProperty.SetItemText(uIndex, 1, "NPC");
		break;
	case REPRESENTOBJECT_PROP:
		m_listProperty.SetItemText(uIndex, 1, "道具");
		break;
	case REPRESENTOBJECT_MAGIC:
		m_listProperty.SetItemText(uIndex, 1, "魔法");
		break;
	case REPRESENTOBJECT_EFFECT:
		m_listProperty.SetItemText(uIndex, 1, "特效");
		break;
	case REPRESENTOBJECT_SKILL:
		m_listProperty.SetItemText(uIndex, 1, "技能");
		break;
	default:;
	}
	m_listProperty.SetItemData(uIndex, VALUE_TYPE);
	uIndex++;

	// 显示相关
	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "显示相关", ITEM_TYPE_TITLE);
	m_listProperty.SetItemData(uIndex, VALUE_TITLE);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "模型文件路径");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szModelFile);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "纹理文件");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szReplaceableTextureFile);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "特效绑定");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szSpecial);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "小地图显示文字");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szMinimapText);
	m_listProperty.SetItemData(uIndex, VALUE_STRING);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "整体偏色");
	szTemp.Format("%X", lpObj->dwTintingColor);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_COLOR);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "阴影颜色");
	szTemp.Format("%X", lpObj->dwShadowColor);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_COLOR);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "小地图显示文字颜色");
	szTemp.Format("%X", lpObj->dwMinimapShowColor);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_COLOR);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "死亡后消失时间");
	szTemp.Format("%d ms", lpObj->dwDecayTime);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "与地表拼接高度");
	szTemp.Format("%f", lpObj->fElevationRadius);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "互动特效");
	szTemp.Format("%d", lpObj->nInteractionSpecial);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "显示层次");
	szTemp.Format("%d", lpObj->nOrder);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "可见优先级");
	szTemp.Format("%d", lpObj->nVisabilityLevel);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否受高光影响");
	lpObj->bHighLight ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否受环境光影响");
	lpObj->bAmbientLight ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "有否阴影");
	lpObj->bShadow ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否在小地图显示图像");
	lpObj->bMinimapShow ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否在小地图显示文字");
	lpObj->bMinimapShowText ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否能换纹理");
	lpObj->bReplaceableTexture ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否会被半透明");
	lpObj->bTranslucence ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "是否参与场景高光运算");
	lpObj->bHighLightScene ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	// 声音相关
	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "声音相关", ITEM_TYPE_TITLE);
	m_listProperty.SetItemData(uIndex, VALUE_TITLE);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件重生音效");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szRevive);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件死亡音效");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szDeath);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物价移动音效");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szMove);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件随机音效");
	m_listProperty.SetItemText(uIndex, 1, lpObj->szLooping);
	m_listProperty.SetItemData(uIndex, VALUE_FILE_PATH);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "随机音效间隔");
	szTemp.Format("%d", lpObj->dwLoopingTime);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "音效淡出率");
	szTemp.Format("%d", lpObj->dwFadeOutRate);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "音效淡入率");
	szTemp.Format("%d", lpObj->dwFadeInRate);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "音效影响范围");
	szTemp.Format("%d", lpObj->dwAffectArea);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	// 状态相关
	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "状态相关", ITEM_TYPE_TITLE);
	m_listProperty.SetItemData(uIndex, VALUE_TITLE);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "生命值");
	szTemp.Format("%f", lpObj->fLife);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "生命回复速度");
	szTemp.Format("%f", lpObj->fLifeReplenish);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "内功防御");
	szTemp.Format("%f", lpObj->fInsaideDefence);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "外功防御");
	szTemp.Format("%f", lpObj->fOutsaideDefence);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "可否被攻击");
	lpObj->bAttackable ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "可否被角色点选");
	lpObj->bSelectable ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "角色可否能通过");
	lpObj->bUnitWalkable ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "角色可否能跳过");
	lpObj->bUnitCanPass ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "子弹可否能通过");
	lpObj->bBulletWalkable ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "子弹可否能跳过");
	lpObj->bBulletCanPass ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "子弹可否能跳过");
	lpObj->bBulletCanPass ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "自动生成障碍");
	lpObj->bAutoPathing ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "死亡是否掉东西");
	lpObj->bDropOnDeath ? szTemp = "是" : szTemp = "否";
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_BOOL);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "障碍类型");
	szTemp.Format("%d", lpObj->nPathingType);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "障碍高度");
	szTemp.Format("%f", lpObj->fPathingHeight);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "死亡类型");
	szTemp.Format("%d", lpObj->nDeathType);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件再生时间间隔");
	szTemp.Format("%f", lpObj->fReviveDelay);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物件再生过程时间");
	szTemp.Format("%f", lpObj->fReviveTime);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_FLOAT);
	uIndex++;

	m_listProperty.InsertItem (uIndex, NULL);
	m_listProperty.SetItemText(uIndex, 0, "物品掉落数量");
	szTemp.Format("%d", lpObj->nDropNumber);
	m_listProperty.SetItemText(uIndex, 1, szTemp);
	m_listProperty.SetItemData(uIndex, VALUE_INT);
	uIndex++;
* /
	return TRUE;
}

*/
void KGObjectPropertyEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rectList;
	CRect rectTree;
	CRect rectDlg;
	CRect rectBnOk;
	CRect rectCue;
	CRect rectSave;

	GetClientRect(rectDlg);
	rectList = rectDlg;
	rectTree = rectDlg;
	rectBnOk = rectDlg;

	rectTree.left   = 3;
	rectTree.top    = 8;
	rectTree.right  = 248;
	rectTree.bottom = rectTree.bottom - 33;

	rectList.left   = 252;
	rectList.top    = 8;
	rectList.right  = rectList.right  - 3;
	rectList.bottom = rectList.bottom -33;

	rectBnOk.left   = rectList.right  - 88;
	rectBnOk.top    = rectList.bottom + 6;
	rectBnOk.right  = rectList.right;
	rectBnOk.bottom = rectBnOk.top + 23;

	rectCue         = rectBnOk;
	rectCue.left    = rectTree.left;

	rectSave        = rectBnOk;
	rectSave.left   = rectList.left;
	rectSave.right  = rectList.left + 88;

	HWND m_hStatic;
	GetDlgItem(IDC_STATIC, &m_hStatic);

	m_bnOK.MoveWindow(rectBnOk);
	m_bnSave.MoveWindow(rectSave);
	::MoveWindow(
		m_hStatic, rectCue.left, rectCue.top,
		rectCue.right, rectCue.bottom, TRUE
	);
	m_treeObjectView.MoveWindow(rectTree);
	m_listProperty.MoveWindow(rectList);

}

int KGObjectPropertyEditDlg::GetChildPoint(
	HTREEITEM hParent, CString szPath, ENUM_CALL_TYPE nType
)
{
	int nResult  = false;
	int nRetCode = false;

	ASSERT(hParent);

	HTREEITEM hChild;
	KG_PROCESS_ERROR(szPath != "");

	if (nType == TYPE_FIRST)
	{
		hChild = hParent;
	}
	else
	{
		hChild = m_treeObjectView.GetChildItem(hParent);
	}

	while (hChild)
	{
		CString strText = m_treeObjectView.GetItemText(hChild);
		if (strText.Right(1) != "\\")
		{
			strText += _T("\\");
		}

		CFileFind file;
		BOOL bContinue = file.FindFile(szPath + strText + "*.*");

		while (bContinue)
		{
			bContinue = file.FindNextFile();
			if (file.IsDirectory() && !file.IsDots())
 			{
				m_treeObjectView.InsertItem(file.GetFileName(), hChild);
			}
			else if (file.GetFileName().Right(4) == ".ini")
			{
				char szName[MAX_TEXT];
				strncpy(
					szName, file.GetFileName().GetBuffer(), sizeof(szName)
				);
				szName[sizeof(szName) - 1] = '\0';
				m_treeObjectView.InsertItem(szName, hChild);
			}
		}

		file.Close();
		GetChildPoint(hChild, szPath + strText, TYPE_OTHER);
		hChild = m_treeObjectView.GetNextItem(hChild, TVGN_NEXT);
	}

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditDlg::OnTvnSelchangedObjectPropertyTree(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (m_bModify)
	{
		if (
			::MessageBox(
				this->m_hWnd, "是否保存你前面的更改?", "Tip", MB_YESNO
			) == IDYES
		)
		{
			OnBnClickedSave();
		}
		m_bModify = FALSE;
	}

	HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
	CString szName  = m_treeObjectView.GetItemText(hItem);

	m_treeObjectView.SortChildren(hItem);

	IEKG3DScene* pScene = m_lpSceneObjectEditorDoc->GetScene();
	IEKG3DSceneObjectEditor* pEditor = NULL;
	pScene->GetSceneObjectEditor(&pEditor);
	_ASSERTE(pEditor);
		//static_cast<KG3DSceneObjectEditor*>(pScene);

    DWORD i = 0;
    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    ASSERT(piManager);

	switch(m_eEditType)
	{
	/*
		case PROPERTY_EDIT_TYPE_NPC :
				{
		            NPC_PROPERTY* lpCurNpc = NULL;
		            DWORD dwNPCCount = 0;
		            piManager->GetRepresentNPCPropertyCount(&dwNPCCount);
		            for (i = 0; i < dwNPCCount; ++i)
					{
						piManager->GetRepresentNPCProperty(i, &lpCurNpc);
		                ASSERT(lpCurNpc);
						if (strcmp(szName, lpCurNpc->szName) == 0)
						{
							pEditor->OpenObject(lpCurNpc->szModelFile);
							UpdateDataObjectProperty(lpCurNpc);
							m_nCurIndex = i;
							break;
						}
					}
				}
				break;*/
	case PROPERTY_EDIT_TYPE_OBJECT :
		{
/*
            OBJECT_PROPERTY* lpCurObj = NULL;
            DWORD dwObjectCount = 0;
                    piManager->GetRepresentObjectPropertyCount(&dwObjectCount);
            for (i = 0; i < dwObjectCount; ++i)
			{
                piManager->GetRepresentObjectProperty(i, &lpCurObj);
				ASSERT(lpCurObj);
				if (strcmp(szName, lpCurObj->szName) == 0)
				{
					pEditor->OpenObject(lpCurObj->szModelFile);
					UpdateDataObjectProperty(lpCurObj);
					m_nCurIndex = i;
					break;
				}
			}
*/
		}
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
	case PROPERTY_EDIT_TYPE_NPC_ANI :
	case PROPERTY_EDIT_TYPE_EQUIP :
		{
			do
			{
				hItem  = m_treeObjectView.GetParentItem(hItem);
				szName =
					m_treeObjectView.GetItemText(hItem) + "\\" + szName;
			}
			while (hItem);
			szName = m_szDirectory + CString(&(szName.GetBuffer()[1]));

			//CString szNamePlate = m_szDirectory + m_szNamePlate;
			//CString szTemplate  = m_szDirectory + m_szTemplate;

			if (GetFileAttributes(szName) != FILE_ATTRIBUTE_DIRECTORY)
			{
				ShowPropertyTable(m_szNamePlate, m_szTemplate, szName);
			}
		}
		break;
	default:;
	}

	*pResult = 0;
}

int  KGObjectPropertyEditDlg::ShowPropertyTable(
	CString szNamePlate, CString szTemplate, CString szIniFile
)
{
	m_listProperty.DeleteAllItems();
	m_arrayPropertyListItem.RemoveAll();

	m_szCurIniFileName = szIniFile;
	SetWindowText(m_szWindowTextt + " 正在编辑 : " + m_szCurIniFileName);

	SAFE_RELEASE(m_pIniFile);
	m_pIniFile          = g_OpenIniFile(szTemplate.GetBuffer());
	IIniFile* pIniFile  = g_OpenIniFile(szIniFile);
	IIniFile* pNameFile = g_OpenIniFile(szNamePlate.GetBuffer());

	if (!pNameFile)  return FALSE;
	if (!m_pIniFile) return FALSE;
	if (!pIniFile)   return FALSE;

	char szSection[256] = "";
	char szKey[256]     = "";
	char szPlate[256]   = "";
	char szValue[512]   = "";
	char szType[256]    = "";

	UINT uIndex = 0;

	while (m_pIniFile->GetNextSection(szSection, szSection))
	{
		PropertyListItem item(szSection, szSection);
		m_arrayPropertyListItem.Add(item);
		pNameFile->GetString(
			szSection, "Title", "未知标题", szPlate, sizeof(szPlate)
		);
		m_listProperty.InsertItem (uIndex, NULL);
		m_listProperty.SetItemText(uIndex, 0, szPlate);
		//m_listProperty.SetItemData(uIndex, VALUE_TITLE);
		uIndex++;

		*szKey = '\0';
		while (m_pIniFile->GetNextKey(szSection, szKey, szKey))
		{
			pNameFile->GetString(
				"Globe", szKey, "未知键名", szPlate, sizeof(szPlate)
			);
			if (CString(szPlate) == "未知键名")
			{
				pNameFile->GetString(
					szSection, szKey, "未知键名", szPlate, sizeof(szPlate)
				);
			}

			CString szKeyType = CString(szKey) + CString("Type");

			pNameFile->GetString(
				"Globe", szKeyType.GetBuffer(), "VALUE_UNKNOW",
				szType, sizeof(szType)
			);
			if (CString(szType) == "VALUE_UNKNOW")
			{
				pNameFile->GetString(
					szSection, szKeyType.GetBuffer(), "STRING",
					szType, sizeof(szType)
				);
			}

			m_pIniFile->GetString(
				szSection, szKey, "未知键值", szValue, sizeof(szValue)
			);
			pIniFile->GetString(
				szSection, szKey, szValue, szValue, sizeof(szValue)
			);
			m_pIniFile->WriteString(szSection, szKey, szValue);

			PropertyListItem item(szSection, szKey);
			m_arrayPropertyListItem.Add(item);

			m_listProperty.InsertItem (uIndex, NULL);
			m_listProperty.SetItemText(uIndex, 0, szPlate);
			m_listProperty.SetItemText(uIndex, 1, szValue);
			m_listProperty.SetItemData(uIndex, GetKeyType(CString(szType)));
			uIndex++;
		}
	}

	pIniFile->Release();
	pNameFile->Release();

	return TRUE;
}

ITEM_VILUE_TYPE KGObjectPropertyEditDlg::GetKeyType(CString szKeyType)
{
	ITEM_VILUE_TYPE eKetType = VALUE_UNKNOW;
/*
	if (szKeyType == "STRING")
	{
		eKetType = VALUE_STRING;
	}
	else if (szKeyType == "INT")
	{
		eKetType = VALUE_INT;
	}
	else if (szKeyType == "FLOAT")
	{
		eKetType = VALUE_FLOAT;
	}
	else if (szKeyType == "BOOL")
	{
		eKetType = VALUE_BOOL;
	}
	else if (szKeyType == "COLOR")
	{
		eKetType = VALUE_COLOR;
	}
	//else if (szKeyType == "TITLE")
	//{
		//eKetType = VALUE_TITLE;
	//}
	else if (szKeyType == "TYPE")
	{
		eKetType = VALUE_TYPE;
	}
	else if (szKeyType == "FILE")
	{
	//	eKetType = VALUE_FILE_PATH;
	}
	else
	{
		eKetType = VALUE_UNKNOW;
	}*/
	return eKetType;

}

void KGObjectPropertyEditDlg::OnNMDblclkObjectPropertyList(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	int nResult  = false;
	int nRetCode = false;

	POSITION pos = m_listProperty.GetFirstSelectedItemPosition();
	int nItem = m_listProperty.GetNextSelectedItem(pos);
	CString szSection = "";
	if (nItem != -1)
	{
		CString szSelName       = m_listProperty.GetItemText(nItem,0);
		CString szSelValue      = m_listProperty.GetItemText(nItem,1);
		int nValueType			= (int)m_listProperty.GetItemData(nItem);

		// if (dwValueType == VALUE_TITLE) return;

		KGObjectPropertyEdit dlgEdit(
			m_pIniFile, m_arrayPropertyListItem[nItem].m_szSectionName,
			m_arrayPropertyListItem[nItem].m_szKeyName, szSelName,
			szSelValue, nItem, &m_listProperty, nValueType,
			m_szDirectory, m_eEditType, this
		);
		if (dlgEdit.DoModal() == IDOK)
		{
					m_bModify = TRUE;
		}
	}

	*pResult = 0;
}

int KGObjectPropertyEditDlg::GetRootPoint(CString szPath)
{
	int nResult  = false;
	int nRetCode = false;

	CFileFind file;
	BOOL bContinue = file.FindFile(szPath + "*");

	KG_PROCESS_ERROR(szPath != "");

	int temp = 0;
	while (bContinue)
	{
		bContinue = file.FindNextFile();
		if (file.IsDirectory() && !file.IsDots())
		{
			if (temp == 0)
			{
				m_treeRoot = m_treeObjectView.InsertItem(file.GetFileName());
				temp = 1;
			}
			else
			{
				m_treeObjectView.InsertItem(file.GetFileName());
			}
		}
	}

	nResult = true;
Exit0:
	file.Close();

	return nResult;
}

int KGObjectPropertyEditDlg::UnInit()
{
	int nResult  = false;
	int nRetCode = false;

	SAFE_RELEASE(m_pIniFile);
	KG_PROCESS_ERROR(m_pView);

	switch(m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_OBJECT :
		if (m_pView->m_pObjectDlg)
		{
			KG3DObjectEditor* pObjDlg = m_pView->m_pObjectDlg;
			m_pView->m_pObjectDlg = NULL;
			delete pObjDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_NPC :
		if (m_pView->m_pNpcDlg)
		{
			KGObjectPropertyEditDlg* pNpcDlg = m_pView->m_pNpcDlg;
			m_pView->m_pNpcDlg = NULL;
			delete pNpcDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_PROP :
		if (m_pView->m_pPropDlg)
		{
			KGObjectPropertyEditDlg* pPorpDlg = m_pView->m_pPropDlg;
			m_pView->m_pPropDlg = NULL;
			delete pPorpDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_MAGIC :
		if (m_pView->m_pMagicDlg)
		{
			KGObjectPropertyEditDlg* pMagicDlg = m_pView->m_pMagicDlg;
			m_pView->m_pMagicDlg = NULL;
			delete pMagicDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_EFFECT :
		if (m_pView->m_pEffectDlg)
		{
			KGObjectPropertyEditDlg* pEffectDlg = m_pView->m_pEffectDlg;
			m_pView->m_pEffectDlg = NULL;
			delete pEffectDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_SKILL :
		if (m_pView->m_pSkillDlg)
		{
			KGObjectPropertyEditDlg* pSkillDlg = m_pView->m_pSkillDlg;
			m_pView->m_pSkillDlg = NULL;
			delete pSkillDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		if (m_pView->m_pAniDlg)
		{
			KGObjectPropertyEditDlg* pAniDlg = m_pView->m_pAniDlg;
			m_pView->m_pAniDlg = NULL;
			delete pAniDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		if (m_pView->m_pPlayerAinDlg)
		{
			KGObjectPropertyEditDlg* pAniDlg = m_pView->m_pPlayerAinDlg;
			m_pView->m_pPlayerAinDlg = NULL;
			delete pAniDlg;
		}
		break;
	case PROPERTY_EDIT_TYPE_EQUIP :
		if (m_pView->m_pEquipDlg)
		{
			KGObjectPropertyEditDlg* pAniDlg = m_pView->m_pEquipDlg;
			m_pView->m_pEquipDlg = NULL;
			delete pAniDlg;
		}
		break;
	default:
		KG_PROCESS_ERROR(false);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGObjectPropertyEditDlg::CheckIniValidity()
{
	int nResult  = false;
	int nRetCode = false;

	char szSectionName[256] = "";
	char szKeyName[256] = "";
	int  nIniValue = 0;
	int  nValue = 0;
	BOOL bCheck    = FALSE;
	BOOL bValidate = FALSE;

	KG_PROCESS_ERROR(m_pIniFile);

	while (m_pIniFile->GetNextSection(szSectionName, szSectionName))
	{
		bCheck     = FALSE;
		bValidate  = FALSE;
		*szKeyName = '\0';
		nIniValue  = 0;

		while (m_pIniFile->GetNextKey(szSectionName, szKeyName, szKeyName))
		{
			if (
				CString(szKeyName) == "Ratio1" ||
				CString(szKeyName) == "Ratio2" ||
				CString(szKeyName) == "Ratio3" ||
				CString(szKeyName) == "Ratio4"
			)
			{
				bCheck = TRUE;
				break;
			}
		}

		if (bCheck)
		{
			m_pIniFile->GetInteger(szSectionName, "Ratio1", 0, &nValue);
			nIniValue += nValue;
			m_pIniFile->GetInteger(szSectionName, "Ratio2", 0, &nValue);
			nIniValue += nValue;
			m_pIniFile->GetInteger(szSectionName, "Ratio3", 0, &nValue);
			nIniValue += nValue;
			m_pIniFile->GetInteger(szSectionName, "Ratio4", 0, &nValue);
			nIniValue += nValue;
		}
		KG_PROCESS_ERROR(nIniValue);
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		CString szTip =
			CString("Total Value Of The Ratio In Section [") +
			szSectionName + CString("] Must Bigger Than Zero!");
		::MessageBox(this->m_hWnd, szTip, "Error", MB_OK);
	}
	return nResult;
}

int KGObjectPropertyEditDlg::TrimIniFile()
{
	int nResult  = false;
	int nRetCode = false;

	TCHAR szSectionName[256] = "";
	TCHAR szKeyName[256] = "";
	TCHAR szValue[512] = "";
	CString szSectionNameSave = "";
	CString szKeyNameSave     = "";

	KG_PROCESS_ERROR(m_pIniFile);

	while (
		m_pIniFile->GetNextSection(szSectionNameSave.GetBuffer(), szSectionName)
	)
	{
		szKeyNameSave  = "";
		*szKeyName     = '\0';
		while (
			m_pIniFile->GetNextKey(
				szSectionName, szKeyNameSave.GetBuffer(), szKeyName
			)
		)
		{
			m_pIniFile->GetString(
				szSectionName, szKeyName, "", szValue, sizeof(szValue)
			);
			if (CString(szValue) == "")
			{
				m_pIniFile->EraseKey(szSectionName, szKeyName);
			}
			else
			{
				szKeyNameSave = CString(szKeyName);
			}
		}
		if (!m_pIniFile->GetNextKey(szSectionName, "", szKeyName))
		{
			m_pIniFile->EraseSection(szSectionName);
		}
		else
		{
			szSectionNameSave = CString(szSectionName);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditDlg::OnBnClickedSave()
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(m_pIniFile);

	nRetCode = TrimIniFile();
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = CheckIniValidity();
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pIniFile->Save(m_szCurIniFileName);
	KG_PROCESS_ERROR(nRetCode);

	ShowPropertyTable(m_szNamePlate, m_szTemplate, m_szCurIniFileName);
	m_bModify = FALSE;

	nResult = true;
Exit0:
	if (!nResult)
	{
		::MessageBox(this->m_hWnd, "Save Error", "Save", MB_OK);
	}
}

void KGObjectPropertyEditDlg::OnBnClickedOk()
{
	UnInit();
}

void KGObjectPropertyEditDlg::OnBnClickedCancel()
{
	UnInit();
}

void KGObjectPropertyEditDlg::OnTvnKeydownTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
