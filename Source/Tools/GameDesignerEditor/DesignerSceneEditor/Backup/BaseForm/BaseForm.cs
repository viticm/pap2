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
using System.Threading;
using System.Diagnostics;
using LuaInterface;
using DevComponents.AdvTree;
using DevComponents.DotNetBar;
using AtlKG3DEngineProxyLib;


namespace DesignerSceneEditor
{
    unsafe public partial class BaseForm : Form, IKDocument
    {
        private BaseDoc m_doc;
        public BaseDoc DOC { get { return m_doc; } set { m_doc = value; } }

        private BaseView m_view;
        public BaseView VIEW { get { return m_view; } set { m_view = value; } }

        public MyPanel PANEL { get { return this.PanelView; } set { this.PanelView = value; } } 

        string m_filename = string.Empty;
        public string FileName { get { return m_filename; } }

        private SceneEditor m_scenesceneeditor = new SceneEditor();
        public SceneEditor SceneSceneEditor { get { return m_scenesceneeditor; } set { m_scenesceneeditor = value; } }

        private TopViewForm m_topView = null;

        private SaveOptionForm m_SaveOptionForm = null;

        //private bool m_bEditing = true;
        public bool m_bTopView = false;
        public bool m_bFullScreen = false;

        const int WM_KEYDOWN = 0x0100;
        const int WM_KEYUP = 0x0101;
        const int WM_CHAR = 0x0102;
        const int MAX_PATH = 260;

        private LuaEx m_lua = null;

        private string m_curMapName = string.Empty;
        private Preview previewform = null;

        public int curMapWidth = 0, curMapHeight = 0;

        private string[] m_types;
        public Hashtable htObjScenePosIndices = null;
        public Hashtable htObjLogicNXIndices = null;
        public Hashtable htObjLogicDirIndices = null;
        public Hashtable htObjLogicRotationIndices = null;

        private bool m_shiftPress = false;
        private bool m_ctrlPress = false;

        const int CAMERA_STORAGE = 4;
        private bool[] m_bPosSaved = new bool[CAMERA_STORAGE];
        private _AtlVector3[] m_SavedCameraPos = new _AtlVector3[CAMERA_STORAGE];
        private _AtlVector3[] m_SavedCameraLookat = new _AtlVector3[CAMERA_STORAGE];

        // ahpho suntao
        private enum EditMode { None, AddNpc, AddDoodad, AddWayPoint, AddTrafficPoint }; // 编辑模式
        private EditMode editMode = EditMode.None; // 当前编辑模式
        private Font normalTextFont = new Font("SimSun", 9, FontStyle.Regular); // 普通文本字体
        private Font boldTextFont = new Font("SimSun", 9, FontStyle.Bold); // 粗体文本字体
        private bool beginEdit = false; // 是否已经开始编辑
        public SqlConnection Conn; // sql连接
        private string rootDir; // 客户端根目录
        public static DataTable npcTable; // npc数据表
        private DataTable npcRepresentTable; // npc表现表
        private Hashtable npcInfoTable = new Hashtable(); // npc信息数据表
        private Hashtable npcAnimationTable = new Hashtable(); // npc动作文件数据表
        private DataTable doodadTable; // doodad数据表
        private DataTable doodadRepresentTable; // doodad表现表
        private Hashtable doodadInfoTable = new Hashtable(); // doodad信息数据表        
        private List<string> npcAnimationList; // npc动作文件链表
        private int npcAnimationIndex = 0; // npc动作编号
        private int doodadModelIndex = 0; // Doodad模型编号
        private TextBox cameraLookAtBox; // 摄像机焦点控件
        public TextBox CameraLookAtBox { set { cameraLookAtBox = value; } } // 摄像机焦点控件
        private SliderItem cameraSpeedBar; // 相机速度控制条
        private MainForm m_parent;

        public static DataTable patrolPathTable = new DataTable(); // npc巡逻路数据表
        public static DataTable npcOrderTable = new DataTable(); // npc阵型数据表

        // 场景逻辑大小相关数据
        public float m_fLogicTotalWidth = 0.0f;
        public float m_fLogicTotalHeight = 0.0f;
        public float m_fMiddleMapStartX = 0.0f;
        public float m_fMiddleMapStartY = 0.0f;
        public float m_fLogicStartX = 0.0f;
        public float m_fLogicStartZ = 0.0f;

        // 供GetObjDisplayInfo用的临时变量。
        int logicObj = 0;
        int representObj = 0;
        int hasScript = 0;
        string name = string.Empty;
        string nickName = string.Empty;
        int templateID = -1;

        // 改造逻辑面板为别名面板
        private bool m_bShowAllNpcDoodad = true;

        private CheckRelive m_CheckRelive;

        public RegionSplitFrom m_RegionSplitFrom = null;

        public BaseForm(string filename, MainForm parent)
        {
            InitializeComponent();

            InitVars();

            m_parent = parent;
            m_doc = new BaseDoc(this, filename);
            m_view = new BaseView(this, this.PANEL.Handle);
            this.PANEL.m_baseForm = this;
            m_doc.DocLogical.SetMainViewHwnd((int)this.PANEL.Handle);
            this.MouseWheel += new MouseEventHandler(this.BaseForm_MouseWheel);
            this.PanelView.GotFocus += new EventHandler(this.PanelView_GotFocus);
            this.PanelView.LostFocus += new EventHandler(this.PanelView_LostFocus);
            this.PanelView.MouseDown += new MouseEventHandler(this.PanelView_MouseDown);
            this.PanelView.Resize += new EventHandler(this.PanelView_Resize);
            this.KeyPreview = true;

            // 顶视图
            InitTopView_Tga(filename);
            InitTopView_Old();

            // lua脚本
            InitLua();

            // ahpho suntao
            // 初始化路径面板
            InitPatrol();

            // 初始化交通面板
            InitTraffic();

            //
            InitPolyComponents();
            InitPoly();

            // 初始化sql连接
            InitSqlConnection();

            // 初始化Npc面板
            InitNpc();
            EngineLayer.ATLBase.OnResizeNpcWindow();

            // 初始化Doodad面板
            InitDoodad();
            EngineLayer.ATLBase.OnResizeDoodadWindow();

            // 初始化预览面板
            InitPreviewCanvas();
            // end ahpho

            // loadtree
            DateTime t_begin = DateTime.Now;
            LogicHelper.LoadTree(m_doc.m_AllLogicObj, this.treeView1, m_bShowAllNpcDoodad);
            TimeSpan t_delta = DateTime.Now - t_begin;
            Debug.WriteLine(t_delta.ToString());
            //MessageBox.Show(t_delta.ToString());

            // init timer
            InitRenderTimer();

            // 读取路径和阵型信息
            DataColumn orderIDColumn = new DataColumn("ID");
            DataColumn orderNameColumn = new DataColumn("Name");
            DataColumn orderCountColumn = new DataColumn("Count");

            npcOrderTable.Columns.Clear();
            npcOrderTable.Columns.Add(orderIDColumn);
            npcOrderTable.Columns.Add(orderNameColumn);
            npcOrderTable.Columns.Add(orderCountColumn);

            InitNpcOrderTable();

            DataColumn patrolPathIDColumn = new DataColumn("ID");
            DataColumn patrolPathNameColumn = new DataColumn("Name");

            patrolPathTable.Columns.Clear();
            patrolPathTable.Columns.Add(patrolPathIDColumn);
            patrolPathTable.Columns.Add(patrolPathNameColumn);

            InitPatrolPathTable();

            InitCameraUI();

            m_CheckRelive = new CheckRelive();
            m_CheckRelive.Init(filename, m_doc.DocLogical, SceneSceneEditor);

        }

        private void InitCameraUI()
        {
            m_parent.UpdateCameraSpeedToUI();
        }

        private void checkShowAll_CheckedChanged(object sender, EventArgs e)
        {
            m_bShowAllNpcDoodad = this.checkShowAll.Checked;
            LogicHelper.LoadTree(m_doc.m_AllLogicObj, this.treeView1, m_bShowAllNpcDoodad);
        }

        /// <summary>
        /// 相机速度控制条
        /// </summary>
        public SliderItem CamerSpeedBar
        {
            set
            {
                cameraSpeedBar = value;
            }
        }

        private void InitLua()
        {
            m_lua = new LuaEx();
            string luafile = Application.StartupPath + "\\lua.lua";
            if (!File.Exists(luafile))
                return;

            m_lua.DoFile(luafile);
            LuaFunction fun = m_lua.GetFunction("transform");
            if (fun != null)
            {
                object[] retobjs = fun.Call(luafile);
            }
        }

        /// <summary>
        /// 设置对象选择掩码
        /// </summary>
        /// <param name="mask">掩码</param>
        public void SetObjectSelectMask(int nMask)
        {
            SceneSceneEditor.SetObjectSelectMask((uint)nMask);
        }

        public bool Save()
        {
            m_SaveOptionForm = new SaveOptionForm();
            DialogResult dr = m_SaveOptionForm.ShowDialog();
            if (dr != DialogResult.OK)
                return false;

            int nRet = m_CheckRelive.Recheck();
            if (nRet == 0)
            {
                return true;
            }

            EngineLayer.ATLBase.SetCurDir();
            EngineLayer.ATLBase.ShowProgressForm(1);
            m_doc.SaveEditorIni();
            if (m_SaveOptionForm.m_ComputeCell)
            {
                m_doc.ComputeCells(m_SaveOptionForm.m_AppointCell, m_SaveOptionForm.m_ClearCell);
            }
            if (m_SaveOptionForm.m_UpdateAllRest)
            {
                m_doc.UpdateAllRest(m_SaveOptionForm.m_AllRest);
            }
            if (!m_SaveOptionForm.m_SaveIniOnly && m_SaveOptionForm.m_SaveLogics)
            {
                m_doc.ExchangeLogics();
            }
            EngineLayer.ATLBase.ShowProgressForm(0);

            m_CheckRelive.SaveSetting();

            return true;
        }

        private void PanelView_LostFocus(object sender, System.EventArgs e)
        {

        }

        private void PanelView_GotFocus(object sender, System.EventArgs e)
        {
            SceneSceneEditor.SetFocus();
            SceneSceneEditor.SetCurrentOutputWindow(VIEW.MAINWINDOWID);
            
//             if (m_Scene != null && m_nOutputWindowID != -1)
//             {
//                 EngineLayer.EngineMgr.SetFocusScene(m_Scene);
//                 m_Scene.SetCurrentOutputWindow(m_nOutputWindowID);
// 
//                 if (EngineLayer.EngineMgr.GetAutoReloadTexture() != 0)
//                 {
//                     AtlIEKG3DTextureTable textureTable = null;
//                     EngineLayer.EngineMgr.GetIEKG3DTextureTable(ref textureTable);
//                     textureTable.AutoReload();
//                 }
//             }
        }        

        private void InitRenderTimer()
        {
            tmrRender.Enabled = true;
        }

        private void tmrRender_Tick(object sender, EventArgs e)
        {
            EngineLayer.ATLBase.UpdateKeyState();
            EngineLayer.ATLBase.FrameMove();
            EngineLayer.ATLBase.Render();
            previewform.m_panel1.Refresh();
        }

        private void BaseForm_MouseWheel(object sender, MouseEventArgs e)
        {
            // EngineLayer.ATLBase.MsgOnMouseWheel((short)e.Delta);
        }

        private void PanelView_MouseDown(object sender, MouseEventArgs e)
        {
            // ahpho suntao
            switch(e.Button)
            {
                case MouseButtons.Right:
                    {
                        if (m_shiftPress && CheckObjectCanRotate()) // 先检查当前选中的对象是否能够旋转
                        {
                            RotateSelectedObject(); // 旋转选中的部件
                        }
                        
                        break;
                    }
            }           
            // end ahpho

            this.ActiveControl = this.PanelView;
        }

        private void PanelView_MouseUp(object sender, MouseEventArgs e)
        {
            switch(e.Button)
            {
                case MouseButtons.Left:
                    {
                        switch (editMode)
                        {
                            case EditMode.AddWayPoint:
                                {
                                    RefreshPatrol();
                                    break;
                                }
                            case EditMode.AddTrafficPoint:
                                {
                                    RefreshTraffic();
                                    break;
                                }
                        }

                        break;
                    }
                case MouseButtons.Right:
                    {
                        cameraLookAtBox.Text = GetCameraLookAt();

                        break;
                    }
            }
        }

        private void BaseForm_KeyUp(object sender, KeyEventArgs e)
        {
            if (!e.Shift)
            {
                m_shiftPress = false;                
            }

            if (!e.Control)
            {
                m_ctrlPress = false;
            }

            EngineLayer.ATLBase.ProcessMessage((uint)WM_KEYUP, e.KeyValue, e.KeyValue);
        }   

        private void BaseForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Shift)
            {
                m_shiftPress = true;                
            }
            if (e.Control)
            {
                m_ctrlPress = true;
            }

            if (!(this.ActiveControl is PanelDockContainer) && this.ActiveControl != this.PanelView)
            {
                return;
            }
            if (!e.Alt && !e.Shift && e.Control && e.KeyCode == Keys.Enter)
            {
                ToggleFullScreen();
                return;
            }
            if (!e.Alt && !e.Shift && !e.Control && e.KeyCode == Keys.P)
            {
                TogglePMode();
                return;
            }
            if (!e.Alt && !e.Shift && !e.Control && e.KeyCode == Keys.T)
            {
                ToggleTopView();
                return;
            }
            if (!e.Alt && !e.Shift && !e.Control && e.KeyCode == Keys.X)
            {
                SceneSceneEditor.DropSelectedEntity();
            }
            if (!e.Alt && !e.Shift && !e.Control && e.KeyCode == Keys.Z)
            {
                SceneSceneEditor.ZoomToObj();
            }
            if (e.Shift && e.KeyCode >= Keys.F1 && e.KeyCode <= Keys.F1 + CAMERA_STORAGE - 1)
            {
                int saveindex = e.KeyCode - Keys.F1;
                m_bPosSaved[saveindex] = true;
                SceneSceneEditor.GetCameraPosLookat(ref m_SavedCameraPos[saveindex], ref m_SavedCameraLookat[saveindex]);
                return;
            }
            if (!e.Shift && e.KeyCode >= Keys.F1 && e.KeyCode <= Keys.F1 + CAMERA_STORAGE - 1)
            {
                int saveindex = e.KeyCode - Keys.F1;
                if (m_bPosSaved[saveindex])
                    SceneSceneEditor.SetCameraPosLookat(ref m_SavedCameraPos[saveindex], ref m_SavedCameraLookat[saveindex]);
                return;
            }
            if (!e.Shift && !e.Control && e.KeyCode == Keys.Enter)
            {
                ShowProperties();
            }

            switch(e.KeyCode)
            {
                case Keys.Oemcomma:
                    {
                        NpcPlayNextAction();
                        return;
                    }
                case Keys.OemPeriod:
                    {
                        NpcPlayPreviousAction();
                        return;
                    }
                case Keys.G:
                    {
                        EditGroup();
                        return;
                    }
                case Keys.Oem1:
                    {
                        editMode = EditMode.None;
                        break;
                    }
                case Keys.W:
                    {
                        cameraLookAtBox.Text = GetCameraLookAt();
                        break;
                    }
                case Keys.S:
                    {
                        cameraLookAtBox.Text = GetCameraLookAt();
                        break;
                    }
                case Keys.A:
                    {
                        cameraLookAtBox.Text = GetCameraLookAt();
                        break;
                    }
                case Keys.D:
                    {
                        cameraLookAtBox.Text = GetCameraLookAt();
                        break;
                    }
                case Keys.Delete:
                    {
                        DeleteSelectedObject();
                        break;
                    }
                case Keys.Add:
                    goto case Keys.Oemplus;
                case Keys.Oemplus:
                    {
                        m_parent.m_sliderCameraSpeed.Value++;
                        m_parent.UpdateCameraSpeedToUI();
                        break;
                    }
                case Keys.Subtract:
                    goto case Keys.OemMinus;
                case Keys.OemMinus:
                    {
                        m_parent.m_sliderCameraSpeed.Value--;
                        m_parent.UpdateCameraSpeedToUI();
                        break;
                    }
            }

            EngineLayer.ATLBase.ProcessMessage((uint)WM_KEYDOWN, e.KeyValue, e.KeyValue);
        }

        /// <summary>
        /// 模拟处理按键消息
        /// </summary>
        /// <param name="e">按键消息参数</param>
        public void DoKeyMessage(KeyEventArgs e)
        {
            BaseForm_KeyDown(this, e);
            BaseForm_KeyUp(this, e);
        }

        private void ShowProperties()
        {
            int entityCount = -1;
            SceneSceneEditor.GetSelectedEntityCount(ref entityCount);

            _AtlObjInfo[] objinfos = new _AtlObjInfo[entityCount];
            for (int i = 0; i < entityCount; i++)
            {
                int iLogicObjPtr = 0;
                string objTypeName = string.Empty;
                m_doc.DocLogical.GetSelectedLogicObjPtr(i, ref iLogicObjPtr, ref objTypeName);

                if (iLogicObjPtr != 0)
                {
                    _AtlObjInfo atlinfo = new _AtlObjInfo();
                    atlinfo.bChanged = 1;
                    atlinfo.iLogicObjPtr = iLogicObjPtr;
                    //_AtlObjInfo中要储存iLogicObjPtr entityPtr，方便C#对话框保存时 直接exchange去docLogical，
                    //再用iLogicObjPtr之逻辑更新entityPtr指向的表现。
                    m_doc.DocLogical.GetSetObjInfo(objTypeName, ref atlinfo, iLogicObjPtr, 1);
                    objinfos[i] = atlinfo;
                }
                else
                {
                    //objinfos[i] = _AtlObjInfo; 自定义valueType如何定义empty
                }
            }

            if (EditSelectedObjectsProperty(objinfos))
            {
                for (int i = 0; i < objinfos.Length; i++)
                {
                    _AtlObjInfo objinfo = objinfos[i];
                    //if (objinfo.bChanged != 0)
                    //{
                    m_doc.DocLogical.GetSetObjInfo(objinfo.strType, ref objinfo, objinfo.iLogicObjPtr, 0);
                    m_doc.DocLogical.UpdateObjRepresentByLogicObj(ref objinfo, objinfo.iLogicObjPtr);
                    //}
                }
            }
        }

        /// <summary>
        /// 删除选中的对象
        /// </summary>
        private void DeleteSelectedObject()
        {
            int entityCount = -1;
            SceneSceneEditor.GetSelectedEntityCount(ref entityCount);
            if (entityCount > 0)
            {
                DialogResult dialogResult = MessageBox.Show("确定要删除选中的对象吗？", "删除对象",
                                                            MessageBoxButtons.OKCancel, MessageBoxIcon.Information);
                if (dialogResult == DialogResult.OK)
                {
                    //m_doc.DocLogical.DeleteSelectedObject();
                    SceneSceneEditor.DeleteSelectedEntity();
                    RefreshPatrol();
                    RefreshTraffic();
                }
            }
        }

        public void ToggleTopView()
        {
            m_bTopView = !m_bTopView;
            if (m_bTopView)
            {
                m_topView.Init();
                m_topView.Show();
                m_topView.SetMyCurrentWindow();
            }
            else
            {
                m_topView.Hide();
                m_topView.UnInit();
            }
        }

        public void ToggleFullScreen()
        {
            m_bFullScreen = !m_bFullScreen;
            m_parent.SetFullScreen(m_bFullScreen);
            SetFullScreen(m_bFullScreen);
            SceneSceneEditor.SetFullScreen(m_bFullScreen ? 1 : 0);
        }

        public void TogglePMode()
        {
            SceneSceneEditor.TogglePMode();
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
        }

        private void PanelView_Resize(object sender, EventArgs e)
        {
            SceneSceneEditor.OnResizeWindow();
        }

        public void ExecCommand(string strCmd, object data)
        {
            switch (strCmd.Trim().ToLower())
            {
            case "savedocument":
                break;
            case "findrecord":
                break;
            case "cmdexport":
                break;
            case "cmdimport":
                break;
            default:
                break;
            }
        }

        /// <summary>
        /// 检查当前选中的对象是否能够旋转 ahpho suntao
        /// </summary>
        /// <returns>是否能够旋转</returns>
        private bool CheckObjectCanRotate()
        {
            bool canRotate = true;

            int entityCount = 0;
            SceneSceneEditor.GetSelectedEntityCount(ref entityCount);

            _AtlObjInfo[] objInfoArray = new _AtlObjInfo[entityCount];
            for (int i = 0; i < entityCount; i++)
            {
                string objectType = objInfoArray[i].strType;
                if (objectType == "NPC" || objectType == "Doodad")
                {
                    ;
                }
                else
                {
                    canRotate = false;
                    break;
                }
            }

            return canRotate;
        }

        /// <summary>
        /// 旋转选中的部件
        /// </summary>
        private void RotateSelectedObject()
        {
            string newRotation = null;
            SceneSceneEditor.RotateSelectedObject(ref newRotation);
            if(newRotation != null) // 已经改变朝向
            {
                string[] rotations = newRotation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                int entityCount = -1;
                SceneSceneEditor.GetSelectedEntityCount(ref entityCount);

                _AtlObjInfo[] objinfos = new _AtlObjInfo[entityCount];
                for (int i = 0; i < entityCount; i++)
                {
                    int iLogicObjPtr = 0;
                    string objTypeName = string.Empty;
                    m_doc.DocLogical.GetSelectedLogicObjPtr(i, ref iLogicObjPtr, ref objTypeName);
                    _AtlObjInfo atlinfo = new _AtlObjInfo();
                    atlinfo.bChanged = 1;
                    atlinfo.iLogicObjPtr = iLogicObjPtr;
                    //_AtlObjInfo中要储存iLogicObjPtr entityPtr，方便C#对话框保存时 直接exchange去docLogical，
                    //再用iLogicObjPtr之逻辑更新entityPtr指向的表现。
                    m_doc.DocLogical.GetSetObjInfo(objTypeName, ref atlinfo, iLogicObjPtr, 1);
                    objinfos[i] = atlinfo;

                    // 写入新的旋转角度信息
                    for(int j = 0; j < atlinfo.iFieldsCount; j++)
                    {
                        if(atlinfo.strFields[j] == "vRotation")
                        {
                            atlinfo.strValues[j] = newRotation;                            
                        }
                        else if(atlinfo.strFields[j] == "nFaceDirection")
                        {
                            atlinfo.strValues[j] = ConvertRotationToDirection(rotations[i]);
                        }
                    }

                    // 更新对象数据
                    m_doc.DocLogical.GetSetObjInfo(atlinfo.strType, ref atlinfo, atlinfo.iLogicObjPtr, 0);
                    m_doc.DocLogical.UpdateObjRepresentByLogicObj(ref atlinfo, atlinfo.iLogicObjPtr);
                }
            }
        }

        /// <summary>
        /// 选中角度转换为方向
        /// </summary>
        /// <returns>角度</returns>
        private string ConvertRotationToDirection(string rotation)
        {
            string[] sites = rotation.Split(new char[] { ',' });            
            float x = float.Parse(sites[0]);
            float y = float.Parse(sites[1]);
            float z = float.Parse(sites[2]);
            float w = float.Parse(sites[3]);
            int direction = 0;
            EngineLayer.ATLBase.QuaternionNDirByRotation(x, y, z, w, ref direction);
            
            return direction.ToString();
        }

        /// <summary>
        /// 设置摄像机移动速度 ahpho suntao
        /// </summary>
        /// <param name="value">基数值</param>
        public void SetCameraSpeed(int value)
        {
            //SceneSceneEditor.SetCameraSpeed(100.0f * value);
        }

        public void SetRenderRegionEdge(bool bRenderEdge)
        {
            SceneSceneEditor.SetRenderBlockEdge(bRenderEdge?1:0);
        }

        public void SetCameraDis(float fDis)
        {
            SceneSceneEditor.SetCameraDis(fDis);
        }

        /// <summary>
        /// 显示相机焦点坐标
        /// </summary>
        /// <param name="textBox">显示用的文本框</param>
        public void ShowCameraLookAt(TextBox textBox)
        {
            
        }


        /// <summary>
        /// 初始化预览面板 ahpho suntao
        /// </summary>
        private void InitPreviewCanvas()
        {
            
        }
        
        /// <summary>
        /// 初始化数据库连接
        /// </summary>
        private void InitSqlConnection()
        {
            IniStructure m_inis;
            string content;

            string fileName = Path.Combine(Application.StartupPath, "GameDesingerTools_Public.ini");
            FileInfo fi = new FileInfo(fileName);
            if (fi.Exists)
            {
                content = FileFolderHelper.FileToString(fileName);
                SymmetricCode sm = new SymmetricCode();
                content = sm.Decrypto(content);
                
                m_inis = IniStructure.ReadIniWithContent(content);
                string connectString = m_inis.GetValue("General", "ConnString");
                Conn = new SqlConnection(connectString);                
            }
            else
            {
                MessageBox.Show("读取配置信息失败，请确认GameDesingerTools_Public.ini文件在程序目录中！", "初始化",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            fileName = Path.Combine(Application.StartupPath, "GameDesingerTools.ini");
            fi = new FileInfo(fileName);
            if(fi.Exists)
            {
                content = FileFolderHelper.FileToString(fileName);
                m_inis = IniStructure.ReadIniWithContent(content);
                rootDir = m_inis.GetValue("General", "RootDir");
            }
        }                     

        /// <summary>
        /// 获取字段对应的序号 ahpho suntao
        /// </summary>
        /// <param name="objectInfo">对象信息</param>
        /// <param name="field">字段</param>
        /// <returns>字段对应的序号</returns>
        private int GetFieldIndex(_AtlObjInfo objectInfo, string field)
        {
            int index = -1;

            for(int i = 0; i < objectInfo.iFieldsCount; i++)
            {
                if(objectInfo.strFields[i] == field)
                {
                    index = i;
                    break;
                }
            }

            return index;
        }

        /// <summary>
        /// 编辑场景中选中对象的属性
        /// </summary>
        public bool EditSelectedObjectsProperty(_AtlObjInfo[] objectInfoArray)
        {
            ObjectsEditForm editFrm = new ObjectsEditForm(m_doc, objectInfoArray);
            DialogResult dr = editFrm.ShowDialog();

            if (dr == DialogResult.OK)
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        private _AtlVector3 GetAtlObjPos(_AtlObjInfo obj)
        {
            _AtlVector3 pos;
            pos.x = pos.y = pos.z = 0.0f;
            if (obj.strType == "NPC")
            {
                string[] _str = obj.strValues[16].Split(new char[] { ',' });
                pos.x = (float)(Convert.ToDouble(_str[0]));
                pos.y = (float)(Convert.ToDouble(_str[1]));
                pos.z = (float)(Convert.ToDouble(_str[2]));
            }
            return pos;
        }

        private void MoveCameraToPosition(_AtlVector3 newPos)
        {
            _AtlVector3 oldpos = new _AtlVector3(), oldlookat = new _AtlVector3();
            SceneSceneEditor.GetCameraPosLookat(ref oldpos, ref oldlookat);
            _AtlVector3 delta = new _AtlVector3();
            delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
            _AtlVector3 newpos = new _AtlVector3();
            newpos.x = newPos.x - delta.x; newpos.y = newPos.y - delta.y; newpos.z = newPos.z - delta.z;
            SceneSceneEditor.SetCameraPosLookat(ref newpos, ref newPos);
        }

        private void MoveCameraToRepresentObj(int representObjPtr)
        {
            _AtlVector3 objPos = new _AtlVector3();
            EngineLayer.ATLBase.GetRepresentObjTranslation(representObjPtr, ref objPos);

            MoveCameraToPosition(objPos);

            //_AtlVector3 oldpos = new _AtlVector3(), oldlookat = new _AtlVector3();
            //SceneSceneEditor.GetCameraPosLookat(ref oldpos, ref oldlookat);
            //_AtlVector3 delta = new _AtlVector3();
            //delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
            //_AtlVector3 newpos = new _AtlVector3();
            //newpos.x = objPos.x - delta.x; newpos.y = objPos.y - delta.y; newpos.z = objPos.z - delta.z;
            //SceneSceneEditor.SetCameraPosLookat(ref newpos, ref objPos);

            SceneSceneEditor.UpdateSelectableEntityList(5);
        }

        private void SelectOnlyRepresentObj(int representObjPtr)
        {
            SceneSceneEditor.ClearSelectedEntityList();
            SceneSceneEditor.AddSelectedRepresentObject(representObjPtr, 1);
            SceneSceneEditor.SetFirstSelectedKing();
        }

        /// <summary>
        /// 将摄像机移动到对象所在的位置
        /// </summary>
        /// <param name="objectInfo">对象信息</param>
        private void MoveCameraToObject(_AtlObjInfo objectInfo)
        {
            // 移动镜头
            Hashtable infoTable = Helper.GetInfoTable(objectInfo);
            string positionStr = infoTable["vPosition"] as string;

            if (positionStr != null) // 有效的坐标
            {
                string[] positionData = positionStr.Split(new char[] { ',' });
                float positionX = float.Parse(positionData[0]);
                float positionY = float.Parse(positionData[1]);
                float positionZ = float.Parse(positionData[2]);

                _AtlVector3 objPos = new _AtlVector3();
                objPos.x = positionX;
                objPos.y = positionY;
                objPos.z = positionZ;

                _AtlVector3 oldpos = new _AtlVector3(), oldlookat = new _AtlVector3();
                SceneSceneEditor.GetCameraPosLookat(ref oldpos, ref oldlookat);
                _AtlVector3 delta = new _AtlVector3();
                delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
                _AtlVector3 newpos = new _AtlVector3();
                newpos.x = objPos.x - delta.x; newpos.y = objPos.y - delta.y; newpos.z = objPos.z - delta.z;
                SceneSceneEditor.SetCameraLocation(newpos.x, newpos.y, newpos.z, objPos.x, objPos.y, objPos.z);
            }            
        }

        /// <summary>
        /// 设置相机坐标 ahpho suntao
        /// </summary>
        /// <param name="x">x坐标</param>
        /// <param name="y">y坐标</param>
        /// <param name="z">z坐标</param>
        public void SetCameraFocus(int x, int y, int z)
        {
            _AtlVector3 cameraLocation = new _AtlVector3();
            _AtlVector3 cameraLookAt = new _AtlVector3();
            _AtlVector3 delta = new _AtlVector3();
            
            SceneSceneEditor.GetCameraPosLookat(ref cameraLocation, ref cameraLookAt);

            delta.x = cameraLookAt.x - cameraLocation.x; 
            delta.y = cameraLookAt.y - cameraLocation.y; 
            delta.z = cameraLookAt.z - cameraLocation.z;

            cameraLookAt.x = x;
            cameraLookAt.y = y;
            cameraLookAt.z = z;

            cameraLocation.x = x - delta.x;
            cameraLocation.y = y - delta.y;
            cameraLocation.z = z - delta.z;

            SceneSceneEditor.SetCameraPosLookat(ref cameraLocation, ref cameraLookAt);
        }

        private void SetFullScreen(bool bFullScreen)
        {
            this.SideCtrlBar.Visible = !bFullScreen;
            if (bFullScreen)
            {
                this.SideCtrlBar.Hide();
                this.SideCtrlBar.Refresh();
            }
            else
            {
                this.SideCtrlBar.Show();
                this.SideCtrlBar.Refresh();
            }
        }

        /// <summary>
        /// 获取摄像机坐标 ahpho suntao
        /// </summary>
        /// <returns>摄像机坐标字符串</returns>
        private string GetCameraLookAt()
        {
            _AtlVector3 cameraLocation = new _AtlVector3();
            _AtlVector3 cameraLookAt = new _AtlVector3();
            SceneSceneEditor.GetCameraPosLookat(ref cameraLocation, ref cameraLookAt);
            string cameraLookAtString = string.Format("{0},{1},{2}", (int)cameraLookAt.x, (int)cameraLookAt.y, (int)cameraLookAt.z);

            return cameraLookAtString;
        }

        /// <summary>
        /// 更新对象数据
        /// </summary>
        /// <param name="objectInfo">对象信息</param>
        /// <param name="fieldName">需要更新的字段</param>
        /// <param name="fieldValue">需要更新的值</param>
        /// <param name="index">对象编号</param>
        /// <param name="parentIndex">父对象编号</param>
        private void UpdateObjectInfo(_AtlObjInfo objectInfo, string fieldName, string fieldValue, int index, int parentIndex)
        {
            int fieldIndex = GetFieldIndex(objectInfo, fieldName);

            if (fieldIndex >= 0)
            {
                string oldValue = objectInfo.strValues[fieldIndex];

                if (oldValue != fieldValue)
                {
                    string objectType = objectInfo.strType;
                    objectInfo.bChanged = 1;
                    m_doc.DocLogical.GetObjDisplayInfo(objectType, index, parentIndex, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    objectInfo.strValues[fieldIndex] = fieldValue;
                    m_doc.DocLogical.GetSetObjInfo(objectType, ref objectInfo, logicObj, 0);
                    m_doc.DocLogical.UpdateObjRepresentByLogicObj(ref objectInfo, logicObj);       
                }
            }
        }

        /// <summary>
        /// 保存对象数据
        /// </summary>
        /// <param name="currentCell">当前编辑的单元格</param>
        /// <param name="objectInfo">对象信息</param>
        /// <param name="index">对象编号</param>
        /// <param name="parentIndex">父对象编号</param>
        private void SaveObjectData(DataGridViewCell currentCell, _AtlObjInfo objectInfo, int index, int parentIndex)
        {                            
            string newValue = currentCell.EditedFormattedValue as string;
            string oldValue = currentCell.Tag as string;
            string objectType = objectInfo.strType;                
            int fieldIndex = GetFieldIndex(objectInfo, currentCell.OwningRow.Tag as string);

            if (newValue == oldValue)
            {
                currentCell.Style.Font = normalTextFont;
            }
            else
            {
                currentCell.Style.Font = boldTextFont;
            }

            objectInfo.bChanged = 1;
            m_doc.DocLogical.GetObjDisplayInfo(objectType, index, parentIndex, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
            objectInfo.strValues[fieldIndex] = newValue;
            m_doc.DocLogical.GetSetObjInfo(objectType, ref objectInfo, logicObj, 0);
            m_doc.DocLogical.UpdateObjRepresentByLogicObj(ref objectInfo, logicObj);            
        }

        private void BaseForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            bool cancelSave = false;
            DialogResult dr = MessageBox.Show("关闭前，你要保存当前场景吗？", "保存提示", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            if (dr == DialogResult.Cancel)
            {
                cancelSave = true;
            }
            else if (dr == DialogResult.Yes)
            {
                if (!Save())
                    cancelSave = true;
            }

            e.Cancel = cancelSave;
            if (!cancelSave)
                m_parent.ActiveFrm = null;
        }

        private void PanelView_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && e.Clicks == 1 && m_ctrlPress)
            {
                cmRightClick.Popup(Control.MousePosition);
            }
        }

        private void buttonJmpToTree_Click(object sender, EventArgs e)
        {
            int entityCount = -1;
            SceneSceneEditor.GetSelectedEntityCount(ref entityCount);
            if (entityCount != 1)
            {
                MessageBox.Show("不能选择2个或以上的对象来使用此功能。");
                return;
            }

            int iLogicObjPtr = 0;
            string objTypeName = string.Empty;
            m_doc.DocLogical.GetSelectedLogicObjPtr(0, ref iLogicObjPtr, ref objTypeName);

            switch (objTypeName)
            {
                case "LogicalPoly":
                    ToSelectPoly(iLogicObjPtr);
                    break;
                default:
                    break;
            }
        }

        private void buttonProp_Click(object sender, EventArgs e)
        {
            ShowProperties();
        }

        private void buttonDelSelObj_Click(object sender, EventArgs e)
        {
            DeleteSelectedObject();
        }

        /// <summary>
        /// 管理分组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void ManageGroup(object sender, EventArgs e)
        {
            GroupManageForm gForm = new GroupManageForm();
            gForm.CurrentBaseDoc = m_doc;
            gForm.Init();
            gForm.ShowDialog();
        }

        private void buttonX27_Click(object sender, EventArgs e)
        {
            m_CheckRelive.Show();
        }
    }
}
