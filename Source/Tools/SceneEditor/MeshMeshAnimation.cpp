#include "stdafx.h"
#include ".\meshmeshanimation.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelTools
{
//KModelToolAnimation::KModelToolAnimation(void)
//{
//	m_lpSave   = NULL;
//	m_lpDelete = NULL;
//	m_lpPlay   = NULL;
//
//	m_lpPlayNormal   = NULL;
//	m_lpPlayCircle   = NULL;
//	m_lpPlayPingPang = NULL;
//
//	m_bHide = TRUE;
//
/////	m_lpFileListBox = NULL;
//	m_lpAniListBox  = NULL;
//	m_lpAniNameEdit = NULL;
//	m_lpTimeSlideBar= NULL;
//
//	m_lpAniSpeedEdit  = NULL;
//	m_lpAniFramesEdit = NULL;
//	m_lpAniFrTimeEdit = NULL;
//	m_lpAniToTimeEdit = NULL;
//
//	m_lpScene = NULL;
//}
//
//KModelToolAnimation::~KModelToolAnimation(void)
//{
//}
//
//HRESULT KModelToolAnimation::CleanUp()
//{
//
//    return E_FAIL;
//}
//
//
//HRESULT KModelToolAnimation::LoadMesh(LPSTR pFileName)
//{
//	SetTitleCaption("动画工具条");
//
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Anitool.tga");
//	//m_CmdBack.Create(0,0.85f,1,0.15f,0,0,Name,0);
//	//g_cGraphicsTool.LoadTexture4Command(&m_CmdBack);
//	ModelUI::FRECT rect(0,0.835f,1,0.165f);
//	Create(&rect,"",0,0);
//
//
//	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpItemTest,MODELTYPE_ITEM);
//	FRECT rect_0(0.003f,0.56f,0.15f,0.05f);
//	m_lpItemTest->Create(&rect_0,"",0,ITEMSTYLE_TEXTIMAGE);
//	m_lpItemTest->SetText("ABCDEFG");
//	m_lpItemTest->SetIcon("ToolBar\\Save.tga");
//	m_lpItemTest->SetParentWnd(this);*/
//
//	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpFileListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_1(0.003f,0.86f,0.15f,0.14f);
//	m_lpFileListBox->Create(&rect_1,"",0,0);
//	m_lpFileListBox->SetParentWnd(this);*/
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpOpen,MODELTYPE_BUTTON);
//	ModelUI::FRECT rect_1(0.003f,0.86f,0.15f,0.14f);
//	m_lpOpen->Create(&rect_1,"",0,0);
//	m_lpOpen->SetIconTexture("ToolBar\\Load.tga");
//	m_lpOpen->SetParentWnd(this);
//
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_2(0.156f,0.86f,0.15f,0.14f);
//	m_lpAniListBox->Create(&rect_2,"",0,0);
//	m_lpAniListBox->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniNameEdit,MODELTYPE_EDITCTRL);
//	ModelUI::FRECT Rect(0.31f,0.86f,0.155f,0.03f);
//	m_lpAniNameEdit->Create(&Rect,"",0,0);
//	m_lpAniNameEdit->SetParentWnd(this);
//
//	//////////////////////////////////////////////////////////////////////////
//	ModelUI::FRECT Rect_Save(0.31f ,0.905f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSave,MODELTYPE_BUTTON);
//	m_lpSave->Create(&Rect_Save,"",0,0);
//	m_lpSave->SetIconTexture("ToolBar\\Save.tga");
//	m_lpSave->SetParentWnd(this);
//
//	ModelUI::FRECT Rect_Delete(0.362f ,0.905f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpDelete,MODELTYPE_BUTTON);
//	m_lpDelete->Create(&Rect_Delete,"",0,0);
//	m_lpDelete->SetIconTexture("ToolBar\\AniDelete.tga");
//	m_lpDelete->SetParentWnd(this);
//
//	ModelUI::FRECT Rect_Play(0.414f ,0.905f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpPlay,MODELTYPE_BUTTON);
//	m_lpPlay->Create(&Rect_Play,"",0,0);
//	m_lpPlay->SetIconTexture("ToolBar\\AniPlay.tga");
//	m_lpPlay->SetParentWnd(this);
//
//	//////////////////////////////////////////////////////////////////////////
//	ModelUI::FRECT Rect_PlayNormal(0.31f ,0.955f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpPlayNormal,MODELTYPE_BUTTON);
//	m_lpPlayNormal->Create(&Rect_PlayNormal,"",0,0);
//	m_lpPlayNormal->SetIconTexture("ToolBar\\AniPlayNormal.tga");
//	m_lpPlayNormal->SetParentWnd(this);
//
//	ModelUI::FRECT Rect_PlayCircle(0.362f ,0.955f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpPlayCircle,MODELTYPE_BUTTON);
//	m_lpPlayCircle->Create(&Rect_PlayCircle,"",0,0);
//	m_lpPlayCircle->SetIconTexture("ToolBar\\AniPlayCircle.tga");
//	m_lpPlayCircle->SetParentWnd(this);
//
//	ModelUI::FRECT Rect_PlayPingPang(0.414f ,0.955f,0.047f,0.04f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpPlayPingPang,MODELTYPE_BUTTON);
//	m_lpPlayPingPang->Create(&Rect_PlayPingPang,"",0,0);
//	m_lpPlayPingPang->SetIconTexture("ToolBar\\AniPlayPingPang.tga");
//	m_lpPlayPingPang->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpTimeSlideBar,MODELTYPE_SLIDEBAR);
//	ModelUI::FRECT RectSlider(0.52f,0.92f,0.45f,0.06f);
//	m_lpTimeSlideBar->Create(&RectSlider,"",0,0);
//	m_lpTimeSlideBar->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniSpeedEdit ,MODELTYPE_EDITCTRL);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniFramesEdit,MODELTYPE_EDITCTRL);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniFrTimeEdit,MODELTYPE_EDITCTRL);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAniToTimeEdit,MODELTYPE_EDITCTRL);
//
//	ModelUI::FRECT Rect1(0.53f,0.86f,0.05f,0.03f);
//	m_lpAniSpeedEdit-> Create(&Rect1,"",0,0);
//	m_lpAniSpeedEdit->SetParentWnd(this);
//
//	ModelUI::FRECT Rect2(0.67f,0.86f,0.05f,0.03f);
//	m_lpAniFramesEdit->Create(&Rect2,"",0,0);
//	m_lpAniFramesEdit->SetParentWnd(this);
//
//	ModelUI::FRECT Rect3(0.81f,0.86f,0.05f,0.03f);
//	m_lpAniFrTimeEdit->Create(&Rect3,"",0,0);
//	m_lpAniFrTimeEdit->SetParentWnd(this);
//
//	ModelUI::FRECT Rect4(0.94f,0.86f,0.05f,0.03f);
//	m_lpAniToTimeEdit->Create(&Rect4,"",0,0);
//	m_lpAniToTimeEdit->SetParentWnd(this);
//
//	m_lpAniNameEdit->SetText("动作名称");
//
//	m_lpAniSpeedEdit ->SetText("1.0");
//	m_lpAniFramesEdit->SetText("0");
//	m_lpAniFrTimeEdit->SetText("0");
//	m_lpAniToTimeEdit->SetText("0");
//
//	//m_lpAniListBox->AddString("None");
//
//	FillAniFileListBox();
//	return S_OK;
//}
//
//HRESULT KModelToolAnimation::Render(void)
//{
//	if (m_bHide)
//		return S_OK;
//
//	KModelUIBase::Render();
//
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
//
//
//	FillAniListBox();
//
//
//	SAFE_RENDER(m_lpOpen);
//	//SAFE_RENDER(m_lpFileListBox);
//	SAFE_RENDER(m_lpAniListBox);
//	SAFE_RENDER(m_lpAniNameEdit);
//
//	int Frame1,F2;
//	float S;
//	int AnimationID = pMesh->m_nCurAnimationIndex;
//	if (AnimationID>=0)
//	{
//		KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//		pMesh->GetCurFrame(&Frame1,&F2,&S);
//		float Percent = 0;
//		if (Frame1<=F2)
//			Percent = (Frame1+S)*100.0f/pAni->NumFrames;
//		else
//			Percent = (Frame1-S)*100.0f/pAni->NumFrames;
//		if (Percent>100) Percent = 100;
//		if (Percent<0  ) Percent = 0;
//		m_lpTimeSlideBar->m_fPercent = Percent;
//	}
//
//	SAFE_RENDER(m_lpTimeSlideBar);
//
//	SAFE_RENDER(m_lpAniSpeedEdit);
//	SAFE_RENDER(m_lpAniFramesEdit);
//	SAFE_RENDER(m_lpAniFrTimeEdit);
//	SAFE_RENDER(m_lpAniToTimeEdit);
//
//	SAFE_RENDER(m_lpSave);
//	SAFE_RENDER(m_lpDelete);
//	SAFE_RENDER(m_lpPlay);
//
//	SAFE_RENDER(m_lpPlayNormal);
//	SAFE_RENDER(m_lpPlayCircle);
//	SAFE_RENDER(m_lpPlayPingPang);
//
//
//	//SAFE_RENDER(m_lpItemTest);
//
//	float X;
//	X = m_lpAniSpeedEdit-> m_RelativeRect.Left - 0.045f;
//	g_cGraphicsTool.DrawTextScale(X,m_RelativeRect.Top+0.03f,0.6f,0.6f,0xFF000000,24,"宋体","速度",0);
//	X = m_lpAniFramesEdit->m_RelativeRect.Left - 0.045f;
//	g_cGraphicsTool.DrawTextScale(X,m_RelativeRect.Top+0.03f,0.6f,0.6f,0xFF000000,24,"宋体","桢数",0);
//	X = m_lpAniFrTimeEdit->m_RelativeRect.Left - 0.06f;
//	g_cGraphicsTool.DrawTextScale(X,m_RelativeRect.Top+0.03f,0.6f,0.6f,0xFF000000,24,"宋体","桢间隔",0);
//	X = m_lpAniToTimeEdit->m_RelativeRect.Left - 0.06f;
//	g_cGraphicsTool.DrawTextScale(X,m_RelativeRect.Top+0.03f,0.6f,0.6f,0xFF000000,24,"宋体","总时间",0);
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolAnimation::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message==WM_KEYDOWN)
//	{
//		if (wParam==VK_F4)
//		{
//			if (m_bHide == FALSE)
//				m_bHide = TRUE;
//			else
//				m_bHide = FALSE;
//		}
//	}
//	if (m_bHide)
//	{
//		return S_OK;
//	}
//
//	DWORD MSG =0;
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
//
//	if(ITEM_CLICK == m_lpOpen->ProcessMessage(message,wParam,lParam))
//	{
//		OpenAnimationFile();
//		m_lpOpen->m_dwState = 0;
//	}
//	/*MSG = m_lpFileListBox->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case LISTBOX_SELECTCHANGE:
//		break;
//	case LISTBOX_ITEMDBCLICK:
//		{
//			TCHAR Name[256];
//			TCHAR str[256];
//
//			if (SUCCEEDED(m_lpFileListBox->GetCurSelectedString(Name)))
//			{
//				wsprintf(str,"%s\\%s",g_Def_ModelDirectory,Name);
//				pMesh->LoadAnimation(str);
//			}
//			break;
//		}
//	}*/
//
//	MSG = m_lpAniListBox->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case LISTBOX_SELECTCHANGE:
//		{
//			TCHAR Name[256];
//			if (SUCCEEDED(m_lpAniListBox->GetCurSelectedString(Name)))
//			{
//				m_lpAniNameEdit->SetText(Name);
//
//				TCHAR str[256];
//				sprintf(str,"%.2f",pMesh->m_fCurAnimationSpeed);
//				m_lpAniSpeedEdit->SetText(str);
//
//				int AnimationID = pMesh->FindAnimation(Name);
//				if (AnimationID<0)
//					break;
//				KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//
//				sprintf(str,"%d",pAni->NumFrames);
//				m_lpAniFramesEdit->SetText(str);
//
//				sprintf(str,"%.2f",pAni->FrameLength);
//				m_lpAniFrTimeEdit->SetText(str);
//
//				sprintf(str,"%.2f",pAni->FrameLength*pAni->NumFrames);
//				m_lpAniToTimeEdit->SetText(str);
//				pMesh->m_nCurAnimationIndex = AnimationID;
//				pMesh->m_dCurAnimationMethod = PLAY_HOLD;
//			}
//			break;
//		}
//	case LISTBOX_ITEMDBCLICK:
//		break;
//	}
//
//	if (ITEM_CLICK==m_lpSave->ProcessMessage(message,wParam,lParam))
//	{
//		if (pMesh->m_dNumAnimation>0)
//			SaveAnimation2File();
//		m_lpSave->m_dwState = 1;
//	}
//	if (ITEM_CLICK==m_lpDelete->ProcessMessage(message,wParam,lParam))
//	{
//		TCHAR Name[256];
//		if (SUCCEEDED(m_lpAniListBox->GetCurSelectedString(Name)))
//		{
//			pMesh->DeleteAnimation(Name);
//		}
//		m_lpDelete->m_dwState = 1;
//	}
//
//	if (ITEM_CLICK==m_lpPlay->ProcessMessage(message,wParam,lParam))
//	{
//		TCHAR Name[256];
//		if (SUCCEEDED(m_lpAniListBox->GetCurSelectedString(Name)))
//		{
//			if (pMesh->m_dCurAnimationMethod==PLAY_HOLD)
//				pMesh->m_dCurAnimationMethod = PLAY_NONE;
//			pMesh->PlayAnimation(Name,
//				pMesh->m_dCurAnimationMethod,
//				pMesh->m_fCurAnimationSpeed);
//		}
//		m_lpPlay->m_dwState = 1;
//	}
//
//	if (ITEM_CLICK==m_lpPlayNormal->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpPlayNormal->m_dwState = 2;
//		m_lpPlayCircle->m_dwState = 0;
//		m_lpPlayPingPang->m_dwState = 0;
//		pMesh->m_dCurAnimationMethod = PLAY_NONE;
//	}
//	if (ITEM_CLICK==m_lpPlayCircle->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpPlayNormal->m_dwState = 0;
//		m_lpPlayCircle->m_dwState = 2;
//		m_lpPlayPingPang->m_dwState = 0;
//		pMesh->m_dCurAnimationMethod = PLAY_CIRCLE;
//	}
//	if (ITEM_CLICK==m_lpPlayPingPang->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpPlayNormal->m_dwState = 0;
//		m_lpPlayCircle->m_dwState = 0;
//		m_lpPlayPingPang->m_dwState = 2;
//		pMesh->m_dCurAnimationMethod = PLAY_PINGPANG;
//	}
//
//	MSG = m_lpAniNameEdit->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case EDIT_TEXTCHANGE:
//		int AnimationID = pMesh->m_nCurAnimationIndex;
//		if (AnimationID>=0)
//		{
//			KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//			wsprintf(pAni->Name,"%s",m_lpAniNameEdit->m_szString);
//			m_lpAniListBox->SetCurSelectedString(m_lpAniNameEdit->m_szString);
//		}
//		break;
//	}
//
//	MSG = m_lpTimeSlideBar->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case SLIDER_MOVE:
//		int AnimationID = pMesh->m_nCurAnimationIndex;
//		if (AnimationID>=0)
//		{
//			KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//			float Percent = m_lpTimeSlideBar->m_fPercent;
//			float Time = Percent * pAni->NumFrames*pAni->FrameLength * 0.01f;
//			pMesh->m_fCurAnimationStart = Time;
//			pMesh->m_dCurAnimationMethod = PLAY_HOLD;
//		}
//		break;
//	}
//
//
//	MSG = m_lpAniSpeedEdit->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case EDIT_TEXTCHANGE:
//		float Speed = 0;
//		sscanf(m_lpAniSpeedEdit->m_szString,"%f",&Speed);
//		pMesh->m_fCurAnimationSpeed = Speed;
//		break;
//	}
//
//	//m_lpAniFramesEdit->ProcessMessage(message,wParam,lParam);
//
//	MSG = m_lpAniFrTimeEdit->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case EDIT_TEXTCHANGE:
//		int AnimationID = pMesh->m_nCurAnimationIndex;
//		if (AnimationID>=0)
//		{
//			KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//			float FrTime = 0;
//			sscanf(m_lpAniFrTimeEdit->m_szString,"%f",&FrTime);
//			pAni->FrameLength = FrTime;
//
//			TCHAR str[256];
//			sprintf(str,"%.2f",pAni->FrameLength*pAni->NumFrames);
//			m_lpAniToTimeEdit->SetText(str);
//		}
//		break;
//	}
//
//	MSG = m_lpAniToTimeEdit->ProcessMessage(message,wParam,lParam);
//	switch(MSG)
//	{
//	case EDIT_TEXTCHANGE:
//		int AnimationID = pMesh->m_nCurAnimationIndex;
//		if (AnimationID>=0)
//		{
//			KModel::_Animation* pAni = pMesh->m_lpAnimations[AnimationID];
//			float ToTime = 0;
//			sscanf(m_lpAniToTimeEdit->m_szString,"%f",&ToTime);
//			pAni->FrameLength = ToTime/pAni->NumFrames;
//
//			TCHAR str[256];
//			sprintf(str,"%.2f",pAni->FrameLength);
//			m_lpAniFrTimeEdit->SetText(str);
//		}
//		break;
//	}
//
//	return 	KModelUIBase::ProcessMessage(message,wParam,lParam);
//}
//
//HRESULT KModelToolAnimation::FillAniFileListBox(void)
//{
//	/*WIN32_FIND_DATA Find;
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\*.Ani",g_Def_ModelDirectory);
//
//	m_lpFileListBox->Clear();
//	HANDLE hFind = FindFirstFile(Name,&Find);
//	if (hFind != INVALID_HANDLE_VALUE)
//	{
//
//		m_lpFileListBox->AddString(Find.cFileName);
//		while(FindNextFile(hFind,&Find))
//		{
//			m_lpFileListBox->AddString(Find.cFileName);
//		}
//		FindClose(hFind);
//	}*/
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolAnimation::FillAniListBox(void)
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	if ((int)pMesh->m_dNumAnimation!=m_lpAniListBox->GetItemCount())
//	{
//		m_lpAniListBox->Clear();
//		for(DWORD i=0;i<pMesh->m_dNumAnimation;i++)
//		{
//			KModel::LPANIMATION pAni = pMesh->m_lpAnimations[i];
//			m_lpAniListBox->AddString(pAni->Name);
//		}
//	}
//	return E_NOTIMPL;
//}
//HRESULT KModelToolAnimation::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolAnimation::SaveAnimation2File()
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Animation Files (*.Ani)|*.Ani|";
//
//	CFileDialog dlg(false, NULL, NULL,
//		OFN_HIDEREADONLY , szFilter);
//	// Modify OPENFILENAME members directly to point to bigBuff
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (	dlg.DoModal() == IDOK)
//	{
//		TCHAR path_buffer[256];
//		TCHAR drive[256];
//		TCHAR dir[256];
//		TCHAR fname[256];
//		TCHAR ext[256];
//
//		CString Vs = dlg.GetPathName();
//
//		wsprintf(path_buffer,"%s",Vs);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//
//
//		TCHAR filename[256];
//		wsprintf(filename,"%s%s%s.Ani",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (!pMesh)
//			return E_FAIL;
//
//		if (FAILED(pMesh->SaveAnimation(filename)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolAnimation::OpenAnimationFile()
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Animation Files (*.Ani)|*.Ani|";
//
//	CFileDialog dlg(true, NULL, NULL,
//		OFN_HIDEREADONLY , szFilter);
//	// Modify OPENFILENAME members directly to point to bigBuff
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (	dlg.DoModal() == IDOK)
//	{
//		TCHAR path_buffer[256];
//		TCHAR drive[256];
//		TCHAR dir[256];
//		TCHAR fname[256];
//		TCHAR ext[256];
//
//		CString Vs = dlg.GetPathName();
//
//		wsprintf(path_buffer,"%s",Vs);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//
//
//		TCHAR filename[256];
//		wsprintf(filename,"%s%s%s.Ani",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (!pMesh)
//			return E_FAIL;
//
//		if (FAILED(pMesh->LoadAnimation(filename)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
};
