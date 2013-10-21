using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using LuaInterface;
using System.Collections;
using DevComponents.DotNetBar;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    public class BaseView
    {
        private BaseForm m_form;
        public BaseForm FORM { get { return m_form; } set { m_form = value; } }

        private BaseDoc m_doc;
        public BaseDoc DOC { get { return m_doc; } set { m_doc = value; } }

        private MyPanel m_panel;
        public MyPanel PANEL { get { return m_panel; } set { m_panel = value; } }

        //private SceneLogicalEditorBase m_LogicalEditor;
        //private AtlIEKG3DScene m_Scene;
        int m_nOutputWindowID = 0;
        public int MAINWINDOWID { get { return m_nOutputWindowID; } }
        int m_hWnd = 0;

        const int OUTPUTWND_SCENEEDITOR_MAIN = 0;
        const int SCENESTATE_SELECT = 0;

        public BaseView(BaseForm parent, IntPtr hwnd)
        {
            m_form = parent;
            m_doc = parent.DOC;
            m_panel = parent.PANEL;
            //m_LogicalEditor = logicEditor;
            //m_Scene = scene;
            m_hWnd = (int)hwnd;

            InitInputProxy();
            FORM.SceneSceneEditor.AddWindowing("SceneMain", m_hWnd, OUTPUTWND_SCENEEDITOR_MAIN, ref m_nOutputWindowID, SCENESTATE_SELECT);
            FORM.SceneSceneEditor.SetCameraDis(75000.0f);
        }

        private void InitInputProxy()
        {
            AtlInputProxy inputproxy = new AtlInputProxy();
            inputproxy.Init(m_hWnd);
            m_panel.InputProxy = inputproxy;
        }

        //private AtlIEKG3DSceneSceneEditor GetEditor()
        //{
        //    AtlIEKG3DSceneSceneEditor sceneSceneEditor = null;
        //    m_Scene.GetSceneSceneEditor(ref sceneSceneEditor);
        //    return sceneSceneEditor;
        //}

        //private AtlIEKG3DSceneEditorBase GetSceneBase()
        //{
        //    AtlIEKG3DSceneSceneEditor sceneSceneEditor = GetEditor();
        //    AtlIEKG3DSceneEditorBase sceneEditorBase = null;
        //    sceneSceneEditor.GetSceneEditorBase(out sceneEditorBase);
        //    return sceneEditorBase;
        //}
    }
}
