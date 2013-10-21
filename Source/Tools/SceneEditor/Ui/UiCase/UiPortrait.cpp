/*******************************************************************************
File        : UiPortrait.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 11-18-2003(mm-dd-yyyy)
Description : 头像选择界面以及相关处理的cpp文件
********************************************************************************/


#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiPortrait.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define UI_PORTRAIT_INI		"玩家头像.ini"
#define PORTRAIT_IMAGE_INI	"\\settings\\portrait.ini"


#define HILIGHT_ALPHA 255
#define LOLIGHT_ALPHA 175


KUiPortrait*	KUiPortrait::ms_pSelf = NULL;
KUiPortrait::PORTRAIT_PARAM	KUiPortrait::m_PortraitParam;
KUiImageRef*	KUiPortrait::m_pImage = NULL;

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiPortrait::KUiPortrait()
{
	m_nSelectIndex = -1;
	m_nFirstLine = 0;
}

KUiPortrait::~KUiPortrait()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiPortrait* KUiPortrait::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiPortrait;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->UpdateInterface();
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiPortrait* KUiPortrait::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiPortrait::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


/*********************************************************************
* 功能：初始化
**********************************************************************/
void KUiPortrait::Initialize()
{
	AddChild(&m_Confirm);
	AddChild(&m_Cancel);
	AddChild(&m_ScrollBar);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiPortrait::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, UI_PORTRAIT_INI);
    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");
			ms_pSelf->m_Confirm.Init(&Ini, "Confirm");
			ms_pSelf->m_Cancel.Init(&Ini, "Cancel");
			ms_pSelf->m_ScrollBar.Init(&Ini, "ScrollBar");

			Ini.GetInteger("Set", "Column", 0, &ms_pSelf->m_nColumnCount);
			Ini.GetInteger("Set", "Line", 0, &ms_pSelf->m_nLineCount);
			Ini.GetInteger("Set", "SpacingH", 0, &ms_pSelf->m_nSpacingH);
			Ini.GetInteger("Set", "SpacingV", 0, &ms_pSelf->m_nSpacingV);

			Ini.GetInteger("Set", "ListLeft", 0, (int *)&ms_pSelf->m_LTPosition.x);
			Ini.GetInteger("Set", "ListTop", 0, (int *)&ms_pSelf->m_LTPosition.y);

			ms_pSelf->m_ScrollBar.SetValueRange(0, ms_pSelf->m_PortraitParam.nPortraitCount / ms_pSelf->m_nColumnCount - 2);
		}
	}
}


/*********************************************************************
* 功能：处理界面消息的处理方法
**********************************************************************/
int KUiPortrait::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			int nIndex;
			if((nIndex = GetIndexAtPos(GET_X_LPARAM(nParam) - m_nAbsoluteLeft, GET_Y_LPARAM(nParam) - m_nAbsoluteTop)) > 0)
			{
				m_nSelectIndex = nIndex;
			}
		}
		break;

	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Confirm && g_pCoreShell && m_nSelectIndex > 0 && m_nSelectIndex <= m_PortraitParam.nPortraitCount)
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_SET_PORTRAIT, 0, m_nSelectIndex);
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_Cancel)
		{
			CloseWindow();
		}
		break;

	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ScrollBar)
		{
    		m_nFirstLine = nParam;
		}
		break;

	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}


/*********************************************************************
* 功能：绘画界面的方法
**********************************************************************/
void KUiPortrait::PaintWindow()
{
	KWndImage::PaintWindow();
	if(g_pRepresentShell)
	{
		int nXBase, nYBase, nX, nY, nIndex;

		nXBase = KWndWindow::m_nAbsoluteLeft + m_LTPosition.x;
		nYBase = KWndWindow::m_nAbsoluteTop  + m_LTPosition.y;

		nY = nYBase;
		nIndex = m_nFirstLine * m_nColumnCount + 1;
		for(int i = 0;i < m_nLineCount;i++)
		{
			nX = nXBase;
			for(int j = 0;j < m_nColumnCount;j++)
			{
				if(nIndex >= m_PortraitParam.nPortraitCount)
				{
					break;
				}
				//begin
				if(m_nSelectIndex == nIndex)
				{
					KUiImageRef Param;
					memset(&Param, 0, sizeof(KUiImageRef));
					DrawPortrait(nIndex, nX, nY);
					Param.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
					Param.nType       = ISI_T_SPR;
					Param.oPosition.nX = nX - 2;
					Param.oPosition.nY = nY - 2;
					Param.oEndPos.nX   = nX + m_PortraitParam.nPortraitWidth  + 2;
					Param.oEndPos.nY   = nY + m_PortraitParam.nPortraitHeight + 2;
					Param.Color.Color_b.a = 255;
					Param.Color.Color_b.r = 222;
					Param.Color.Color_b.g = 221;
					Param.Color.Color_b.b = 149;
					g_pRepresentShell->DrawPrimitives(1, &Param, RU_T_RECT,TRUE);
				}
				else
				{
					DrawPortrait(nIndex, nX, nY, TRUE);
				}
				//end
				nX += m_PortraitParam.nPortraitWidth + m_nSpacingH;
				nIndex++;
			}
			nY += m_PortraitParam.nPortraitHeight + m_nSpacingV;
		}
	}
}


/*********************************************************************
* 功能：通过坐标取得所选择的头像的索引
**********************************************************************/
int	KUiPortrait::GetIndexAtPos(int nX, int nY)
{
	int nH, nV, nIndex;
	int nBaseIndex = m_nFirstLine * m_nColumnCount + 1;

	nX -= m_LTPosition.x;
	nY -= m_LTPosition.y;

	if(nX < 0 || nY < 0)
	{
		return -1;
	}

	nH = nX / (m_PortraitParam.nPortraitWidth + m_nSpacingH);
	nV = nY / (m_PortraitParam.nPortraitHeight + m_nSpacingV);

	nX %= (m_PortraitParam.nPortraitWidth + m_nSpacingH);
	nY %= (m_PortraitParam.nPortraitHeight + m_nSpacingV);

	nIndex = nV * m_nColumnCount + nH + nBaseIndex;
	if(nX > m_PortraitParam.nPortraitWidth || nY > m_PortraitParam.nPortraitHeight ||
	   nH >= m_nColumnCount || nV >= m_nLineCount || nIndex >= m_PortraitParam.nPortraitCount)
	{
		return -1;
	}
	else
	{
		return nIndex;
	}
}


/*********************************************************************
* 功能：根据玩家的头像更新界面的显示
**********************************************************************/
int	KUiPortrait::UpdateInterface()
{
	if(g_pCoreShell)
	{
		KUiPlayerBaseInfo Info;
		g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)&Info, 0);
		if(Info.nPortrait > 0 && Info.nPortrait <= m_PortraitParam.nPortraitCount)
		{
			m_nSelectIndex = Info.nPortrait;
		}
		return 1;
	}
	return 0;
}


/*
                      .-~~-.--.                     :         )

               .~ ~ -.\       /.- ~~ .               >       `.   .'       <

              (         .- -.         )               `- -.-~  `- -'  ~-.- -'

                 (        :        )           _ _ .-:

                  ~--.    :    .--~        .-~  .-~  }

                      ~-.-^-.-~ \_      .~  .-~   .~

                               \ \'     \ '_ _ -~

                                `.`.    //

                       . - ~ ~-.__`.`-.//

                   .-~   . - ~  }~ ~ ~-.~-.

                 .' .-~      .-~       :/~-.~-./:

                /_~_ _ . - ~                 ~-.~-._

                                                 ~-.<
(给你，最可爱的人 :)
//初始化头像
*/
int	KUiPortrait::InitializeProtrait()
{
	KIniFile Ini;

	if(Ini.Load(PORTRAIT_IMAGE_INI))
	{
		Ini.GetInteger("Main", "Count", 0, &m_PortraitParam.nPortraitCount);
		Ini.GetInteger("Main", "Width", 0, &m_PortraitParam.nPortraitWidth);
		Ini.GetInteger("Main", "Height", 0, &m_PortraitParam.nPortraitHeight);

		if(m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
		m_pImage = new KUiImageRef[m_PortraitParam.nPortraitCount + 1];
		if(m_pImage)
		{
			char szBuf[8];
			for(int i = 1;i < m_PortraitParam.nPortraitCount + 1;i++)
			{
				IR_InitUiImageRef(m_pImage[i]);
				Ini.GetString("Image", itoa(i, szBuf, 10), "", m_pImage[i].szImage, sizeof(m_pImage[i].szImage));
				m_pImage[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
				m_pImage[i].nType = ISI_T_SPR;
				m_pImage[i].nFlipTime  = 0;
				m_pImage[i].nInterval  = 0;
				m_pImage[i].nNumFrames = 0;
				m_pImage[i].Color.Color_b.r = 0;
				m_pImage[i].Color.Color_b.g = 0;
				m_pImage[i].Color.Color_b.b = 0;
				m_pImage[i].Color.Color_b.a = LOLIGHT_ALPHA;
			}
			return 1;
		}
	}
	return 0;
}


int	KUiPortrait::ReleasePortrait()
{
	if(m_pImage)
	{
		delete(m_pImage);
		m_pImage  = NULL;
	}
	return 1;
}


/*=================================================================
       _   /  |
       | \  \/_/
       \_\| / __
          \/_/__\           .--='/~\     //绘画出梦一般色彩的头像:D
   ____,__/__,_____,______)/   /{~}}}    //传入的坐标是Screen坐标
   -,-----,--\--,-----,---,\'-' {{~}}
           __/\_            '--=.\}/
          /_/ |\\
               \/
*/
int	KUiPortrait::DrawPortrait(int nIndex, int nX, int nY, int nbDarkness)
{
	if(nIndex > 0 && nIndex < m_PortraitParam.nPortraitCount + 1 && m_pImage[nIndex].szImage[0])
	{
		m_pImage[nIndex].oPosition.nX = nX;
		m_pImage[nIndex].oPosition.nY = nY;
		if(nbDarkness)
		{
			m_pImage[nIndex].Color.Color_b.a = LOLIGHT_ALPHA;
		}
		else
		{
			m_pImage[nIndex].Color.Color_b.a = HILIGHT_ALPHA;
		}
		g_pRepresentShell->DrawPrimitives(1, m_pImage + nIndex, RU_T_IMAGE, TRUE);
		IR_NextFrame(m_pImage[nIndex]);
		return 1;
	}
	return 0;
}

/*        (战国群雄传)        */
/*获取梦幻一般的肖像的来源之地*/
char* KUiPortrait::GetPortraitImagePath(int nPortraitIndex, char *szImage)
{
	if(nPortraitIndex > 0 && nPortraitIndex <= m_PortraitParam.nPortraitCount)
	{
		strcpy(szImage, m_pImage[nPortraitIndex].szImage);
	}
	return szImage;
}
