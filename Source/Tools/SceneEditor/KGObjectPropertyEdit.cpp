////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectPropertyEdit.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-30 15:01:24
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGObjectPropertyEdit.h"
#include "KGObjectPropertyEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGObjectPropertyEdit, CDialog)
/*
KGObjectPropertyEdit::KGObjectPropertyEdit(
    CString szName, CString szValue, DWORD valueType,
    unsigned nCurIndex, CString szCurItem,
    IKG3DObjectPropertyModifior* lpPropertyModifior,
	CString szIniFileName, CString szSection,
	PROPERTY_EDIT_TYPE eEditType,
    CWnd* pParent )
	: CDialog(KGObjectPropertyEdit::IDD, pParent)
{
    m_lpPropertyModifior = lpPropertyModifior;
    m_szPropertyName     = szName;
    m_szPropertyValueOld = szValue;
    m_enumValueType      = (ITEM_VILUE_TYPE)valueType;
    m_nCurIndex          = nCurIndex;
    m_szCurItem          = szCurItem;
	m_eEditType          = eEditType;
	m_szSection          = szSection;
	m_szIniFileName      = szIniFileName;
}*/

KGObjectPropertyEdit::KGObjectPropertyEdit(
	IIniFile* pIniFile, CString szSectionName, CString szKeyName,
	CString szPlate, CString szValue, int nItem,
	KGListCtrl* pList, DWORD dwValueType, CString szDirectory,
	PROPERTY_EDIT_TYPE eEditType, CWnd* pParent
): CDialog(KGObjectPropertyEdit::IDD, pParent)
{
	m_pIniFile           = pIniFile;
	m_szPropertyName     = szPlate;
	m_szPropertyValueOld = szValue;
	//m_enumValueType      = (ITEM_VILUE_TYPE)dwValueType;
	m_eEditType          = eEditType;
	m_szSectionName      = szSectionName;
	m_nItem              = nItem;
	m_pList              = pList;
	m_szDirectory        = szDirectory;
	m_szKeyName          = szKeyName;
}

KGObjectPropertyEdit::~KGObjectPropertyEdit()
{
}

void KGObjectPropertyEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_OLD,  m_szPropertyValueOld);
    DDX_Text(pDX, IDC_NEW,  m_szPropertyValueNew);
    DDX_Text(pDX, IDC_BOOL, m_szComboBool);
    DDX_Text(pDX, IDC_TYPE, m_szComboType);
    DDX_Control(pDX, IDC_VIEW, m_buttonView);
    DDX_Control(pDX, IDC_NEW, m_editNew);
    DDX_Control(pDX, IDC_OLD, m_editOld);
    DDX_Control(pDX, IDC_BOOL, m_comboBool);
    DDX_Control(pDX, IDC_TYPE, m_comboType);
}


BEGIN_MESSAGE_MAP(KGObjectPropertyEdit, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_VIEW, OnBnClickedView)
END_MESSAGE_MAP()


void KGObjectPropertyEdit::OnBnClickedOk()
{
	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_OBJECT :
	case PROPERTY_EDIT_TYPE_NPC :
	case PROPERTY_EDIT_TYPE_PROP :
	case PROPERTY_EDIT_TYPE_MAGIC :
	case PROPERTY_EDIT_TYPE_EFFECT :
	case PROPERTY_EDIT_TYPE_SKILL:
		UpdateObjectData();
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
	case PROPERTY_EDIT_TYPE_NPC_ANI :
	case PROPERTY_EDIT_TYPE_EQUIP :
		if (!UpdateNpcAniData())
		{
			::MessageBox(this->m_hWnd, "Update Data Error!!", "Error", MB_OK);
		}
		break;
	default:;
	}
	OnOK();
}

BOOL KGObjectPropertyEdit::UpdateNpcAniData()
{
	UpdateData();
/*
	if(!m_pIniFile)
	{
		return FALSE;
	}

	switch (m_enumValueType)
	{
	case VALUE_FILE_PATH :
		m_pIniFile->WriteString(
			m_szSectionName, m_szKeyName, m_szPropertyValueNew.GetBuffer()
		);
		m_pList->SetItemText(m_nItem, 1, m_szPropertyValueNew.GetBuffer());
		break;
	case VALUE_INT :
		m_pIniFile->WriteInteger(
			m_szSectionName, m_szKeyName, atoi(m_szPropertyValueNew.GetBuffer())
		);
		m_pList->SetItemText(m_nItem, 1, m_szPropertyValueNew.GetBuffer());
		break;
	default:;
	}
*/
	//pFile->Save(m_szIniFileName);
	//pFile->Release();

	return TRUE;
}

BOOL KGObjectPropertyEdit::UpdateObjectData()
{
	UpdateData();

	IIniFile* pFile = g_OpenIniFile(
        m_lpPropertyModifior->GetObjectFileName(m_nCurIndex)
    );

    if(!pFile)
    {
        return FALSE;
    }

    DWORD          dwColor = 0;
    int            ARGB[4];

    // 标示相关
    if (m_szCurItem == "物件名")
    {
       m_lpPropertyModifior->SetName(
           m_nCurIndex, m_szPropertyValueNew.GetBuffer()
       );
       pFile->WriteString(
           "caption", "szName",
           m_szPropertyValueNew.GetBuffer()
       );
    }
    if (m_szCurItem == "名字后缀")
    {
       m_lpPropertyModifior->SetSuffix(
           m_nCurIndex, m_szPropertyValueNew.GetBuffer()
       );
       pFile->WriteString(
           "caption", "szSuffix",
           m_szPropertyValueNew.GetBuffer()
       );
    }
    if (m_szCurItem == "说明")
    {
        m_lpPropertyModifior->SetNote(
            m_nCurIndex, m_szPropertyValueNew.GetBuffer()
        );
        pFile->WriteString(
            "caption", "szNote",
            m_szPropertyValueNew.GetBuffer()
        );
    }

    if (m_szCurItem == "类别")
    {
        if (m_szComboType == "物件")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_DEFAULT);
        }
        if (m_szComboType == "NPC")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_NPC);
        }
        if (m_szComboType == "道具")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_PROP);
        }
        if (m_szComboType == "魔法")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_MAGIC);
        }
        if (m_szComboType == "特效")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_EFFECT);
        }
        if (m_szComboType == "技能")
        {
            m_lpPropertyModifior->SetType(m_nCurIndex, REPRESENTOBJECT_SKILL);
        }
        pFile->WriteString(
            "caption", "nType", m_szComboType
        );
    }

     // 显示相关
    if (m_szCurItem == "模型文件路径")
    {
        m_lpPropertyModifior->SetModelFile(
            m_nCurIndex, m_szPropertyValueNew.GetBuffer()
        );
        pFile->WriteString(
            "display", "szModelFile",
            m_szPropertyValueNew.GetBuffer()
        );
    }
    if (m_szCurItem == "纹理文件")
    {
        m_lpPropertyModifior->SetReplaceableTextureFile(
            m_nCurIndex, m_szPropertyValueNew.GetBuffer()
        );
        pFile->WriteString(
            "display", "szReplaceableTextureFile",
            m_szPropertyValueNew.GetBuffer()
        );
    }
    if (m_szCurItem == "特效绑定")
    {
        m_lpPropertyModifior->SetSpecial(
            m_nCurIndex, m_szPropertyValueNew.GetBuffer()
        );
        pFile->WriteString(
            "display", "szSpecial",
            m_szPropertyValueNew.GetBuffer()
        );
    }
    if (m_szCurItem == "小地图显示文字")
    {
        m_lpPropertyModifior->SetMinimapText(
            m_nCurIndex, m_szPropertyValueNew.GetBuffer()
        );
        pFile->WriteString(
            "display", "szMinimapText",
            m_szPropertyValueNew.GetBuffer()
        );
    }
    if (m_szCurItem == "整体偏色")
    {
        sscanf(m_szPropertyValueNew.GetBuffer(), "%X", &dwColor);
        m_lpPropertyModifior->SetTintingColor(m_nCurIndex, dwColor);
        ARGB[0] = (dwColor & 0xFF000000)>>24;
        ARGB[1] = (dwColor & 0x00FF0000)>>16;
        ARGB[2] = (dwColor & 0x0000FF00)>>8;
        ARGB[3] = (dwColor & 0x000000FF);
        pFile->WriteMultiInteger("display", "dwTintingColor", ARGB, 4);
    }
    if (m_szCurItem == "区域染色")
    {
        sscanf(m_szPropertyValueNew.GetBuffer(), "%X", &dwColor);
        m_lpPropertyModifior->SetChangeColor(m_nCurIndex, dwColor);
        ARGB[0] = (dwColor & 0xFF000000)>>24;
        ARGB[1] = (dwColor & 0x00FF0000)>>16;
        ARGB[2] = (dwColor & 0x0000FF00)>>8;
        ARGB[3] = (dwColor & 0x000000FF);
        pFile->WriteMultiInteger("display", "dwChangeColor", ARGB, 4);
    }
    if (m_szCurItem == "阴影颜色")
    {
        sscanf(m_szPropertyValueNew.GetBuffer(), "%X", &dwColor);
        m_lpPropertyModifior->SetShadowColor(m_nCurIndex, dwColor);
        ARGB[0] = (dwColor & 0xFF000000)>>24;
        ARGB[1] = (dwColor & 0x00FF0000)>>16;
        ARGB[2] = (dwColor & 0x0000FF00)>>8;
        ARGB[3] = (dwColor & 0x000000FF);
        pFile->WriteMultiInteger("display", "dwShadowColor", ARGB, 4);
    }
    if (m_szCurItem == "小地图显示文字颜色")
    {
        sscanf(m_szPropertyValueNew.GetBuffer(), "%X", &dwColor);
        m_lpPropertyModifior->SetMinimapShowColor(m_nCurIndex, dwColor);
        ARGB[0] = (dwColor & 0xFF000000)>>24;
        ARGB[1] = (dwColor & 0x00FF0000)>>16;
        ARGB[2] = (dwColor & 0x0000FF00)>>8;
        ARGB[3] = (dwColor & 0x000000FF);
        pFile->WriteMultiInteger("display", "dwMinimapShowColor", ARGB, 4);
    }
    if (m_szCurItem == "死亡后消失时间")
    {
        m_lpPropertyModifior->SetDecayTime(
            m_nCurIndex, atoi(m_szPropertyValueNew.GetBuffer())
        );
        pFile->WriteInteger(
            "display", "dwDecayTime", atoi(m_szPropertyValueNew.GetBuffer())
        );
    }
    if (m_szCurItem == "与地表拼接高度")
    {
        m_lpPropertyModifior->SetElevationRadius(
            m_nCurIndex, (float)atof(m_szPropertyValueNew.GetBuffer())
        );
        pFile->WriteFloat(
            "display", "fElevationRadius", (float)atof(m_szPropertyValueNew.GetBuffer())
        );
    }
    if (m_szCurItem == "互动特效")
    {
        m_lpPropertyModifior->SetInteractionSpecial(
            m_nCurIndex, atoi(m_szPropertyValueNew.GetBuffer())
        );
        pFile->WriteInteger(
            "display", "nInteractionSpecial", atoi(m_szPropertyValueNew.GetBuffer())
        );
    }
    if (m_szCurItem == "显示层次")
    {
        m_lpPropertyModifior->SetOrder(
            m_nCurIndex, atoi(m_szPropertyValueNew.GetBuffer())
        );
        pFile->WriteInteger(
            "display", "nOrder", atoi(m_szPropertyValueNew.GetBuffer())
        );
    }
    if (m_szCurItem == "可见优先级")
    {
        m_lpPropertyModifior->SetVisabilityLevel(
            m_nCurIndex, atoi(m_szPropertyValueNew.GetBuffer())
        );
        pFile->WriteInteger(
            "display", "nVisabilityLevel", atoi(m_szPropertyValueNew.GetBuffer())
        );
    }
    if (m_szCurItem == "是否受高光影响")
    {
        m_lpPropertyModifior->SetIfHighLight(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bHighLight", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "是否受环境光影响")
    {
        m_lpPropertyModifior->SetIfAmbientLight(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bAmbientLight", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "有否阴影")
    {
       m_lpPropertyModifior->SetIfShadow(
           m_nCurIndex, m_szComboBool == "是" ? true : false
       );
       pFile->WriteInteger(
           "display", "bShadow", m_szComboBool == "是" ? 1 : 0
       );
    }
    if (m_szCurItem == "是否在小地图显示图像")
    {
        m_lpPropertyModifior->SetIfMinimapShow(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bMinimapShow", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "是否在小地图显示文字")
    {
        m_lpPropertyModifior->SetIfMinimapShowText(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bMinimapShowText", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "是否能换纹理")
    {
        m_lpPropertyModifior->SetIfReplaceableTexture(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bReplaceableTexture", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "是否会被半透明")
    {
        m_lpPropertyModifior->SetIfTranslucence(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bTranslucence", m_szComboBool == "是" ? 1 : 0
        );
    }
    if (m_szCurItem == "是否参与场景高光运算")
    {
        m_lpPropertyModifior->SetIfHighLightScene(
            m_nCurIndex, m_szComboBool == "是" ? true : false
        );
        pFile->WriteInteger(
            "display", "bHighLightScene", m_szComboBool == "是" ? 1 : 0
        );
    }

    pFile->Save(m_lpPropertyModifior->GetObjectFileName(m_nCurIndex));
    pFile->Release();

    return TRUE;
}

BOOL KGObjectPropertyEdit::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_szPropertyValueNew = m_szPropertyValueOld;
    UpdateData(FALSE);
    SetWindowText(m_szPropertyName);
    CRect rect;
    m_editOld.MoveWindow(88, 3,  385, 20);
    m_editNew.MoveWindow(88, 28, 385, 20);
    m_comboBool.MoveWindow(88, 28, 100, 10);
    m_comboType.MoveWindow(88, 28, 100, 10);
    m_comboBool.ShowWindow(FALSE);
    m_comboBool.InsertString(0, "是");
    m_comboBool.InsertString(1, "否");
    m_comboType.InsertString(0, "物件");
    m_comboType.InsertString(1, "NPC");
    m_comboType.InsertString(2, "道具");
    m_comboType.InsertString(3, "魔法");
    m_comboType.InsertString(4, "特效");
    m_comboType.InsertString(5, "技能");

    if (m_szPropertyValueOld == "是")
    {
        m_comboBool.SetCurSel(0);
    }
    else
    {
        m_comboBool.SetCurSel(1);
    }

    if (m_szPropertyValueOld == "物件")
    {
        m_comboType.SetCurSel(0);
    }
    else if (m_szPropertyValueOld == "NPC")
    {
        m_comboType.SetCurSel(1);
    }
    else if (m_szPropertyValueOld == "道具")
    {
        m_comboType.SetCurSel(2);
    }
    else if (m_szPropertyValueOld == "魔法")
    {
        m_comboType.SetCurSel(3);
    }
    else if (m_szPropertyValueOld == "特效")
    {
        m_comboType.SetCurSel(4);
    }
    else if (m_szPropertyValueOld == "技能")
    {
        m_comboType.SetCurSel(5);
    }
/*
    switch(m_enumValueType)
    {
    case VALUE_FILE_PATH :
        m_buttonView.ShowWindow(TRUE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(TRUE);
        m_editNew.MoveWindow(88, 28, 350, 20);
        break;
    case VALUE_STRING :
        m_buttonView.ShowWindow(FALSE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(TRUE);
        m_editNew.MoveWindow(88, 28, 385, 20);
        break;
    case VALUE_INT :
        m_buttonView.ShowWindow(FALSE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(TRUE);
        m_editNew.MoveWindow(88, 28, 385, 20);
        break;
    case VALUE_FLOAT :
        m_buttonView.ShowWindow(FALSE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(TRUE);
        m_editNew.MoveWindow(88, 28, 385, 20);
        break;
    case VALUE_BOOL :
        m_buttonView.ShowWindow(FALSE);
        m_comboBool.ShowWindow(TRUE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(FALSE);
        m_editNew.MoveWindow(88, 28, 385, 20);
        break;
    case VALUE_COLOR :
        m_buttonView.ShowWindow(TRUE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(FALSE);
        m_editNew.ShowWindow(TRUE);
        m_editNew.MoveWindow(88, 28, 350, 20);
        break;
    case VALUE_TYPE :
        m_buttonView.ShowWindow(FALSE);
        m_comboBool.ShowWindow(FALSE);
        m_comboType.ShowWindow(TRUE);
        m_editNew.ShowWindow(FALSE);
        m_editNew.MoveWindow(88, 28, 385, 20);
        break;
    default:;
    }
*/
    return TRUE;
}

void KGObjectPropertyEdit::OnBnClickedView()
{
//    COLORREF color;
//	char szColor[16];
/*
    if (m_enumValueType == VALUE_FILE_PATH)
    {
        CFileDialog fileDlg(
			TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			NULL, this
		);
		fileDlg.m_ofn.lpstrInitialDir = m_szDirectory.GetBuffer();
        if (fileDlg.DoModal() == IDOK)
        {
			m_szPropertyValueNew = fileDlg.GetPathName();
			/*
			int nIndex = m_szPropertyValueNew.Find(m_szDirectory);
			if (!nIndex)
			{
				int nSize =
					m_szPropertyValueNew.GetLength() - m_szDirectory.GetLength();
				m_szPropertyValueNew = m_szPropertyValueNew.Right(nSize + 1);
			}
			*//*
            UpdateData(FALSE);
        }
    }
    else if (m_enumValueType == VALUE_COLOR)
    {
        CColorDialog colorDlg(NULL, NULL, this);
        if (colorDlg.DoModal() == IDOK)
        {
            color = colorDlg.GetColor();
			sprintf(szColor, "%X", color);
			m_szPropertyValueNew = szColor;
            UpdateData(FALSE);
        }
    }*/
}
