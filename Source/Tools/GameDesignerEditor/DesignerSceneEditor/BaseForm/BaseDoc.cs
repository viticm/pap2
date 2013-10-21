using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.Runtime.InteropServices;
using DevComponents.DotNetBar;
using LuaInterface;
using AtlKG3DEngineProxyLib;


namespace DesignerSceneEditor
{
    unsafe public class BaseDoc
    {
        private BaseForm m_form;
        public BaseForm FORM { get { return m_form; } set { m_form = value; } }

        private BaseView m_view;
        public BaseView VIEW { get { return m_view; } set { m_view = value; } }

        //private SceneLogicalEditorBase m_LogicalEditor;
        //public SceneLogicalEditorBase LogicalEditor { get { return m_LogicalEditor; } }

        //private AtlIEKG3DScene m_Scene = null;
        //public AtlIEKG3DScene SCENE { get { return m_Scene; } }

        //private AtlIELogicalSceneConnection m_LogicSceneConnection = null;
        //public AtlIELogicalSceneConnection LogicSceneConnection { get { return m_LogicSceneConnection; } }

        private IAtlIEKSceneEditorDocLogical m_DocLogical = null;
        public IAtlIEKSceneEditorDocLogical DocLogical { get { return m_DocLogical; } }

        ////private AtlIEKG3DSceneSceneEditor m_SceneSceneEditor = null;
        ////public AtlIEKG3DSceneSceneEditor SceneSceneEditor { get { return m_SceneSceneEditor; } }

//         private int m_LogicalData = 0;
//         public int LogicalDat { get { return m_LogicalData; } }

//         private int m_LogicalScene = 0;
//         public int LogicalScene { get { return m_LogicalScene; } }

        private string m_strFileName = string.Empty;

        const int NEW_TERRAIN_WIDTH = 128;
        const int REGION_GRID_WIDTH_BIT_NUM = 6;
        const int REGION_GRID_WIDTH = 1 << REGION_GRID_WIDTH_BIT_NUM;
        const int SCENETYPE_SCENEEDITOR = 1;

        public LO_Map m_AllLogicObj = new LO_Map();

        // ahpho suntao
        private string mapName; // 当前场景名
        // end ahpho
        public BaseDoc(BaseForm parent, string filename/*, ref AtlIEKG3DScene baseFormScene*/)
        {
            m_form = parent;
            m_view = parent.VIEW;
            m_strFileName = filename;

            if (EditorLayer.UseSO3Logic)
            {
                // Init LogicEditor
                EngineLayer.ATLBase.InitLogicalEditorBase();

                LogicsLayer.SO3world.LoadEditorScene(filename);

                LogicsLayer.SO3world.AttachLogicalSceneTo3DScene();

                LogicsLayer.SO3world.NewLogicSceneConnectionAndBindLogicalSceneAndSceneEditor();
            }

            LogicsLayer.SO3world.NewLogicalDataAndAdviseWithSceneEditor();

            string strIniFile = m_strFileName + ".Logical";
            LogicsLayer.SO3world.LoadLogicalData(strIniFile);

            // 加载树所要的数据
            LogicsLayer.SO3world.GetLogicalDataPtr(ref m_DocLogical);
            LogicHelper.Init(m_DocLogical);
            mapName = FileFolderHelper.GetMapName(m_strFileName);
            m_AllLogicObj = LogicHelper.DoImportOneTarget(strIniFile, mapName);
        }

        /// <summary>
        /// 当前场景名
        /// </summary>
        public string MapName
        {
            get
            {
                return mapName;
            }
        }

        public void SaveEditorIni()
        {
            string relaPath = m_strFileName.Substring(EditorLayer.EngineDir.Length) + ".Logical";
            if (!relaPath.TrimStart(new char[] { '\\' }).ToLower().StartsWith("data"))
                MessageBox.Show("保存时发现错误。请检查GameDesingerTools.ini的RootDir");
            else
                LogicsLayer.SO3world.SaveEditorIni(relaPath);
        }

        public void ComputeCells(bool bAppointCell, bool bRemoveWhenCalc)
        {
            LogicsLayer.SO3world.SaveCalcCollision(bAppointCell ? 1 : 0, bRemoveWhenCalc ? 1 : 0);
        }

        public void UpdateAllRest(bool bRest)
        {
            LogicsLayer.SO3world.UpdateAllRest(bRest ? 1 : 0);
        }

        public void ExchangeLogics()
        {
            LogicsLayer.SO3world.SaveLogicalScene(m_strFileName);
        }

        //private AtlIEKG3DSceneSceneEditor GetEditor()
        //{
        //    AtlIEKG3DSceneSceneEditor sceneSceneEditor = null;
        //    m_Scene.GetSceneSceneEditor(ref sceneSceneEditor);
        //    return sceneSceneEditor;
        //}

    }
}
