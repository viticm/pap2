////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : WndBuilderForLua.cpp
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 11/29/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef KGUI_LUA_LOADER
#include "LuaWndBuilder.h"
#include "../../script/kscriptmgr.h"

#include "../wndwindow.h"
#include "../wndedit.h"
#include "../wndbutton.h"
#include "../wndpageset.h"
#include "../wndframe.h"
#include "../wndcheckbox.h"
#include "../wndscene.h"
#include "../wndminimap.h"
#include "../wndtexture.h"
#include "../wndmovie.h"
#include "../wndnewscrollbar.h"

#include "../Components/itemhandle.h"
#include "../Components/itemtreeleaf.h"

namespace UI
{

    KLuaWndBuilder gs_LuaWndBuilder;

    KLuaWndBuilder::KLuaWndBuilder()
    {
        m_ObjCreatorMap["WndWindow"] = &KWndWindow::Create;
        m_ObjCreatorMap["WndFrame"] = &KWndFrame::Create;
        m_ObjCreatorMap["WndButton"] = &KWndButton::Create;
        m_ObjCreatorMap["WndCheckBox"] = &KWndCheckBox::Create;
        m_ObjCreatorMap["WndEdit"] = &KWndEdit::Create;
        m_ObjCreatorMap["WndMinimap"] = &KWndMinimap::Create;
        m_ObjCreatorMap["WndMovie"] = &KWndMovie::Create;
        m_ObjCreatorMap["WndNewScrollBar"] = &KWndNewScrollBar::Create;
        m_ObjCreatorMap["WndPageSet"] = &KWndPageSet::Create;
		m_ObjCreatorMap["WndPage"] = &KWndWindow::Create;	// TODO: remove
        m_ObjCreatorMap["WndScene"] = &KWndScene::Create;
        m_ObjCreatorMap["WndTexture"] = &KWndTexture::Create;

        m_ObjCreatorMap["Null"] = &KItemNull::Create;
        m_ObjCreatorMap["Animate"] = &KItemAnimate::Create;
        m_ObjCreatorMap["Box"] = &KItemBox::Create;
        m_ObjCreatorMap["Handle"] = &KItemHandle::Create;
        m_ObjCreatorMap["Image"] = &KItemImage::Create;
        m_ObjCreatorMap["Scene"] = &KItemScene::Create;
        m_ObjCreatorMap["Shadow"] = &KItemShadow::Create;
        m_ObjCreatorMap["Text"] = &KItemText::Create;
        m_ObjCreatorMap["TreeLeaf"] = &KItemTreeLeaf::Create;
    }

    KLuaWndBuilder::~KLuaWndBuilder()
    {
        m_ObjCreatorMap.clear();
    }

    // 放在WndStation里更合理，但是为了新旧代码尽量分开，先放这里
    int KLuaWndBuilder::OpenFrame(LPCSTR pcszFrame, LPCSTR pcszName, KWndFrame **ppRetFrame)
    {
    	int nResult  = false;
    	int nRetCode = false;
    
        KWndFrame *pFrame        = NULL;
        lua_State *pLua          = NULL;
        float      fScale        = 0.0f;
        LPCSTR     pcszFrameName = NULL;

        KGLOG_PROCESS_ERROR(pcszFrame);
        KGLOG_PROCESS_ERROR(pcszFrame[0]);
        KGLOG_PROCESS_ERROR(ppRetFrame);

        pFrame = g_pUI->m_WndStation.SearchFrame(pcszFrame);
        KG_PROCESS_SUCCESS(pFrame);

        pLua = g_pUI->m_Script.GetLuaState();
        KGLOG_PROCESS_ERROR(pLua);

        lua_getglobal(pLua, "WndCfgList");
        nRetCode = lua_isnil(pLua, -1);
        KGLOG_PROCESS_ERROR(!nRetCode);

        lua_getfield(pLua, -1, pcszFrame);
        nRetCode = lua_isnil(pLua, -1);
        KGLOG_PROCESS_ERROR(!nRetCode);

        nRetCode = CreateFrame(pLua, -1, (IKUIObject **)&pFrame);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pFrame);
        KGLOG_PROCESS_ERROR(pFrame->m_emObjType == em_OBJ_TYPE_FRAME);  // 保证他是Wnd类型控件

        fScale = g_pUI->m_WndStation.GetUIScale();
        pFrame->Scale(fScale, fScale);

        pcszFrameName = (pcszName && pcszName[0]) ? pcszName : pcszFrame;
        nRetCode = pFrame->SetName(pcszFrameName);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = g_pUI->m_WndStation.AddFrame(pFrame, pFrame->GetLayer());
        KGLOG_PROCESS_ERROR(nRetCode);

        
        nRetCode = g_pUI->m_Script.BindScript(pFrame);
        // KG_PROCESS_ERROR(nRetCode);

        // 只提供父窗口的OnCreate调用
        pFrame->FireCreate();

        if (pFrame->IsVisible())
            pFrame->Show();

    Exit1:
        *ppRetFrame = pFrame;
        nResult = true;
    Exit0:
        if (!nResult && pFrame)
        {
            pFrame->Destroy(WND_DO_NOT_DESTROY_MY_BROTHER);
            pFrame->Breathe();
            pFrame = NULL;
        }
    	return nResult;
    }

    int KLuaWndBuilder::CreateFrame(Lua_State *pLua, int nTableIndex, IKUIObject **ppRetObj)
    {
        int nResult  = false;
        int nRetCode = false;

        int nChildTableIndex = 0;
        int nFieldIndex      = 1;
        int nTopIndex        = 0;
        int nChildAddFlag    = false;

        IKUIObject *pObj = NULL;
        IKUIObject *pChild = NULL;

        KGLOG_PROCESS_ERROR(pLua);
        KGLOG_PROCESS_ERROR(nTableIndex != 0);
        KGLOG_PROCESS_ERROR(ppRetObj);

        nTopIndex = lua_gettop(pLua);    // 保存栈顶位置

        nRetCode = CreateObject(pLua, -1, &pObj);
        ASSERT(nRetCode);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pObj);

        // 特别注意：以下是一个递归调用
        // 如果出现问题，可以考虑改用控件本身创建所有子控件的思想来实现
        
        while (true)
        {
            lua_pushnumber(pLua, nFieldIndex);
            lua_gettable(pLua, -2);

            nRetCode = lua_isnil(pLua, -1);
            KG_PROCESS_SUCCESS(nRetCode);

            nChildAddFlag = false;
            nRetCode = CreateFrame(pLua, -1, &pChild);
            ASSERT(nRetCode);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(pChild);

            nRetCode = pObj->AddChild(pChild);
            ASSERT(nRetCode);
            KGLOG_PROCESS_ERROR(nRetCode);
            nChildAddFlag = true;

            lua_pop(pLua, 1);
            nFieldIndex++;
        }

    Exit1:
        *ppRetObj = pObj;
        nResult = true;
    Exit0:
        if (!nResult)
        {
            if (pChild && !nChildAddFlag)
            {
                SAFE_RELEASE(pChild);
            }
            if (pObj)
            {
                ASSERT(pObj->m_emObjType == em_OBJ_TYPE_FRAME);
                KWndFrame *pFrame = (KWndFrame *)pObj;
                pFrame->Destroy(WND_DO_NOT_DESTROY_MY_BROTHER);
                pFrame->Breathe();
                pFrame = NULL;
                pObj   = NULL;
            }
        }
        lua_settop(pLua, nTopIndex);
        return nResult;
    }

    int KLuaWndBuilder::CreateObject(Lua_State *pLua, int nTableIndex, IKUIObject **ppRetObj)
    {
        int nResult  = false;
        int nRetCode = false;

        char szObjType[WND_TYPE_NAME_LEN];
        char szObjName[KD_SMALL_BUFFER_SIZE];
        KOBJ_MAP::iterator itCreator;
        IKUIObject *pObj = NULL;

        KLuaTableField aObjInfo[] = {
            KLUA_TABLE_FIELD("ObjType", emVAR_TYPE_STRING, "", szObjType, _countof(szObjType)),
            KLUA_TABLE_FIELD("name",    emVAR_TYPE_STRING, "", szObjName, _countof(szObjName)),
        };

        KGLOG_PROCESS_ERROR(pLua);
        KGLOG_PROCESS_ERROR(nTableIndex != 0);
        KGLOG_PROCESS_ERROR(ppRetObj);

        for (int i = 0; i < _countof(aObjInfo); i++)
        {
            nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aObjInfo[i]);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        KGLOG_PROCESS_ERROR(szObjType[0]);

        itCreator = m_ObjCreatorMap.find(szObjType);
        KGLOG_PROCESS_ERROR(itCreator != m_ObjCreatorMap.end());

        nRetCode = (itCreator->second)(&pObj, pLua, nTableIndex);
        ASSERT(nRetCode && "DEBUG 控件创建失败");
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pObj);

        KG_PROCESS_SUCCESS(szObjName[0] == '\0');
        if (
            (pObj->m_emObjType == em_OBJ_TYPE_WND) || 
            (pObj->m_emObjType == em_OBJ_TYPE_FRAME)
        )
        {
            nRetCode = ((KWndWindow *)pObj)->SetName(szObjName);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

    Exit1:
        *ppRetObj = pObj;
        nResult = true;
    Exit0:
        if (!nResult)
        {
            SAFE_RELEASE(pObj);
        }
        return nResult;
    }

}

#endif