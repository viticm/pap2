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
using System.Text.RegularExpressions;
using DevComponents.DotNetBar;
using atlaxLib;


namespace GameDesingerTools
{
    public partial class BaseForm
    {
        private void ProducePreviewWindows()
        {
            int tab_count = this.tabControl1.Tabs.Count - 1;
            preview_panels = new MyPanelEx[tab_count];
            for (int i = 0; i < tab_count; i++)
            {
                preview_panels[i] = new MyPanelEx();
                preview_panels[i].bf_parent = this;
                preview_panels[i].BackgroundImageLayout = PreviewPanel.BackgroundImageLayout;
                preview_panels[i].CanvasColor = PreviewPanel.CanvasColor;
                preview_panels[i].CausesValidation = PreviewPanel.CausesValidation;
                preview_panels[i].Location = PreviewPanel.Location;
                preview_panels[i].Name = "PreviewPane" + Convert.ToString(i + 1);
                preview_panels[i].Size = PreviewPanel.Size;
                preview_panels[i].Style.Alignment = PreviewPanel.Style.Alignment;
                preview_panels[i].Style.BackColor1.Alpha = PreviewPanel.Style.BackColor1.Alpha;
                preview_panels[i].Style.BackColor1.ColorSchemePart = PreviewPanel.Style.BackColor1.ColorSchemePart;
                preview_panels[i].Style.BackColor2.Alpha = PreviewPanel.Style.BackColor2.Alpha;
                preview_panels[i].Style.BackColor2.ColorSchemePart = PreviewPanel.Style.BackColor2.ColorSchemePart;
                preview_panels[i].Style.Border = PreviewPanel.Style.Border;
                preview_panels[i].Style.BorderColor.ColorSchemePart = PreviewPanel.Style.BorderColor.ColorSchemePart;
                preview_panels[i].Style.CornerType = PreviewPanel.Style.CornerType;
                preview_panels[i].Style.ForeColor.ColorSchemePart = PreviewPanel.Style.ForeColor.ColorSchemePart;
                preview_panels[i].Style.GradientAngle = 90;
                preview_panels[i].Visible = false;
                preview_panels[i].Dock = DockStyle.Fill;
                preview_panels[i].MouseMove += new System.Windows.Forms.MouseEventHandler(this.PreviewPanel_MouseMove);
                preview_panels[i].DoubleClick += new System.EventHandler(this.PreviewPanel_DoubleClick);
                preview_panels[i].Resize += new System.EventHandler(this.PreviewPanel_Resize);
            }
            this.panelDockContainer2.Controls.AddRange(preview_panels);
            preview_panels[0].Visible = true;
        }

        //add by cuiwei 07.8.13
        public void InitPreviewPlugIn(Control cp)
        {
            this.SuspendLayout();
            dockSite1.SuspendLayout();
            this.ContainerPanel.Visible = true;
            this.panelDockContainer3.Show();
            this.ContainerPanel.Controls.Add(cp);
            cp.Dock = DockStyle.Fill;
            this.PreviewPluginBar.SuspendLayout();
            this.PreviewPluginBar.Visible = true;
            this.PreviewPluginBar.ResumeLayout();
            dockSite1.ResumeLayout();
            this.ResumeLayout();
            this.PreviewPluginBar.RecalcLayout();
        }
        //add end

        public void InitPreviewNpcPortrait()
        {
            try
            {
                if (!m_bInit3DEngineFaield)
                    doInitPreview(1);
            }
            catch
            {
                m_3DEngie = null;
                m_bInit3DEngineFaield = true;
            }
        }

        public void InitPreview()
        {
            try
            {
                if (!m_bInit3DEngineFaield)
                    doInitPreview(0);
            }
            catch
            {
                m_3DEngie = null;
                m_bInit3DEngineFaield = true;
            }
        }
        private void doInitPreview(int iOption)
        {
            int old_wrong_index = this.tabControl1.SelectedTabIndex;
            int new_right_index = init_preview_index;
            // new_right_index 是从1 开始的下标。
            string strTabIndex = "[" + new_right_index.ToString() + "]";

            if (Program.EngineDir.Trim().Length <= 0)
            {
                if (!m_bWarning3dEngine)
                {
                    m_bWarning3dEngine = true;
                    MessageBox.Show("3DEngine 路径为空，不能正常打开预览窗口", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
                return;
            }

            string strdir = "";
            if (Program.EngineDir.Substring(Program.EngineDir.Length - 1) != "\\")
                strdir = Program.EngineDir + "\\";
            else
                strdir = Program.EngineDir;

            if (m_3DEngie == null)
            {
                bool bSuc = Helper.RegComDll("atlax.dll");
                
                m_3DEngie = new Test3DEngine();

                if (m_3DEngie == null)
                {
                    MessageBox.Show("3D Engine 初始化失败！m_3DEngie = null。");
                    return;
                }

                m_3DEngie.Init((int)(preview_panels[new_right_index - 1].Handle), Program.EngineDir, Application.StartupPath);

                // 传给mainform, 让它关闭时uninit 引擎。
                ((MainForm)ParentForm).K3DEngine = m_3DEngie;

                string content = string.Format(@"
RegComDll: {0}
m_3DEngie: {1}
EngineDir: {2}
Engine:    {3}
Enginenull:{4}
", bSuc, m_3DEngie, strdir, m_3DEngie, m_3DEngie == null?"null":"not null");

                FileFolderHelper.StringToFile(content, Application.StartupPath + "/GameDesignerTools.log");
            }

            m_lua["EngineDir"] = strdir;
            m_lua["Engine"] = m_3DEngie;

            int count;
            m_3DEngie.GetEditorSceneCount(ModelName + strTabIndex, out count);
            if (count > 0)
                return;

            m_3DEngie.AddScene((int)preview_panels[new_right_index - 1].Handle, ModelName + strTabIndex, iOption);

            //string DirName = strdir;
            //string PlayerName = DirName + "\\Data\\public\\PlayerCheckTool\\Male\\Normal\\M2.mdl";
            //string AniFileName = DirName + "\\data\\source\\player\\M2\\动作\\M2b04ty空手普通待机.ani";
            //m_3DEngie.LoadPlayerModel(PlayerName);
            //m_3DEngie.LoadAni(AniFileName, 1);

            this.SetTmrRenderInterval(Program.m_3d_tmrRender);
            tmrRender.Enabled = true;

            PreviewBar.Visible = true;
            PreviewBar.RecalcLayout();
            this.ResumeLayout();
        }

        public void SetTmrRenderInterval(int iInterval)
        {
            this.tmrRender.Interval = iInterval;
        }

        private void InitModel(string modelname)
        {
            CustomClassCache.DelComboCache();
            /* remoting
            string sql = string.Format("SELECT * FROM sys_modl_def WHERE modelname_en='{0}'", modelname);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            */

            string filter = string.Format("modelname_en='{0}'", modelname);
            DataTable tbl = Helper.GetDataTableProxy("sys_modl_def", filter, null, Conn);

            DataRow row = tbl.Rows[0];
            string modl_id = row["id"].ToString();

            // 模块id
            ModID = Convert.ToInt32(modl_id);

            //不检查调试库中的内容
            if (MainForm.conn.Database.ToString().ToLower() != "s3design" && MainForm.conn.Database.ToString().ToLower() != "s3design_debug")
            {
                // 当前用户能否执行保存命令
                /* remoting
                sql = string.Format("SELECT * FROM sys_useracl_desc WHERE userindex = {0} AND moduleid = {1} AND canwrite = 1", Program.m_rightmgr._userindex, this.ModID);
                DataTable tbl_acl_desc = Helper.GetDataTable(sql, Conn);
                */

                filter = string.Format("userindex = {0} AND moduleid = {1} AND canwrite = 1", Program.m_rightmgr._userindex, this.ModID);
                DataTable tbl_acl_desc = Helper.GetDataTableProxy("sys_useracl_desc", filter, null, Conn);


                if (tbl_acl_desc.Rows.Count > 0)
                    this.CanSaveDB = true;

                // 当前用户能否配置表元
                /* remoting
                sql = string.Format("SELECT * FROM sys_useracl_desc WHERE userindex = {0} AND moduleid = {1} AND canconfigmeta = 1", Program.m_rightmgr._userindex, this.ModID);
                tbl_acl_desc = Helper.GetDataTable(sql, Conn);
                */

                filter = string.Format("userindex = {0} AND moduleid = {1} AND canconfigmeta = 1", Program.m_rightmgr._userindex, this.ModID);
                tbl_acl_desc = Helper.GetDataTableProxy("sys_useracl_desc", filter, null, Conn);

                if (tbl_acl_desc.Rows.Count > 0)
                    this.CanCfg = true;
            }

            // 模块名
            ModelName = modelname;
            this.bar1.Text = ModelName + "树";

            // 模块中文名
            this.Text = ModelName;

            // 主表名
            /*
            sql = string.Format("SELECT * FROM sys_modl_tab_def WHERE modid='{0}'", modl_id);
            DataTable tbl_tab_def = Helper.GetDataTable(sql, Conn);
            */

            filter = string.Format("modid='{0}'", modl_id);
            DataTable tbl_tab_def = Helper.GetDataTableProxy("sys_modl_tab_def", filter, null, Conn);


            if (tbl_tab_def.Rows.Count < 1)
            {
                MessageBox.Show("在表 sys_modl_tab_def 中找不到模块的信息。请检查相关数据表。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            DataRow row_tab = tbl_tab_def.Rows[0];
            MainTableName = row_tab["tablename"].ToString().Trim();

            // 主表
            //TblMain = Helper.GetDataTable("SELECT * FROM " + MainTableName, Conn);

            //TblMain = Helper.GetDataTableProxy(MainTableName, Conn);
           TblMain = Helper.GetDataTableTopRowProxy(MainTableName, Conn);

            HasDefaultRow = Helper.HasTableDefaultRow(MainTableName, Conn);

            // 显示的字段
            DisplayField = row_tab["display_field"].ToString().Trim();

            // tree分类的字段们
            CatFields = row_tab["catfields"].ToString().Trim().Split(new char[] { ',' });

            // 显示字段也用来作分类吗
            CatfieldsContainsDisplayfield = false;
            foreach (string str in CatFields)
            {
                if (str == DisplayField)
                {
                    CatfieldsContainsDisplayfield = true;
                    break;
                }
            }
            ArrayList arrDBPrimaryKey = new ArrayList();
            // 显示字段是key
            foreach (DataColumn col in TblMain.PrimaryKey)
            {
                if (col.ColumnName == DisplayField)
                {
                    KeysContainsDisplayfileld = true;
                    //break;
                }
                arrDBPrimaryKey.Add(col.ColumnName);
            }
            // 分类字段中有key吗
            foreach (DataColumn col in TblMain.PrimaryKey)
            {
                foreach (string strCat in CatFields)
                {
                    if (strCat == col.ColumnName)
                    {
                        CatFieldsContainsKey = true;
                        break;
                    }
                }
            }
            if (CatFieldsContainsKey)
            {
                for (int i = 0; i < TblMain.PrimaryKey.Length; i++)
                {
                    string strkey = TblMain.PrimaryKey[i].ColumnName;
                    bool bThisKeyInCat = false;
                    foreach (string strCat in CatFields)
                    {
                        if (strkey == strCat)
                        {
                            bThisKeyInCat = true;
                        }
                    }
                    if (!bThisKeyInCat)
                    {
                        ToChangeIndex = i;
                        break;
                    }
                }
            }

            // 新建记录时的起始 id
            UpdateStartIDs();

            // 分页们
            m_Pages = new ArrayList();
            bool bFirstTab = true;
            foreach (DataRow r in tbl_tab_def.Rows)
            {
                bool bCustom = false;
                if (r["custom_tab"] != DBNull.Value)
                    bCustom = (r["custom_tab"].ToString().Trim() == "1");

                PageDesc tab = new PageDesc();
                tab.id = Convert.ToInt32(r["modtabid"]);
                tab.name_cn = r["name"].ToString();
                tab.bCustomTab = bCustom;

                if (!bCustom)
                {
                    tab.pg = new PropertyGrid();
                    this.contextMenuBar1.SetContextMenuEx(tab.pg, this.cmScriptOp);
                }

                AddTab(ref tab, bFirstTab);
                bFirstTab = false;

                tab.dbClass = new DBCustomClass(null, m_lua, tab.id, -1);
                tab.Properties = tab.dbClass;
                tab.dbClass.OnAsynNotify += new DBCustomClass.AsynNotifyHandler(OnAsynNotify);
                //todo:增加子结点中的Onasynnotify
                m_Pages.Add(tab);
            }
            if (arrDBPrimaryKey.Count > 0)
                m_recLock = new RecordLockEx(Conn, MainTableName, (string[])arrDBPrimaryKey.ToArray(typeof(string)), ModID);
        }



        public bool LoadModel(string strModel, string strtype)
        {
            if (m_3DEngie != null)
            {
                FileInfo fi = new FileInfo(strModel);
                if (!fi.Exists)
                    return false;

                m_3DEngie.LoadPlayerModel(strModel);
                string stranifile = GetAnimationFile(strModel, strtype);
                if (stranifile != null && stranifile.Length > 0)
                {
                    m_3DEngie.LoadAni(stranifile, 1);
                    return true;
                }
            }
            return false;
        }

        public bool LoadModelAniOnce(string strModel, string strtype)
        {
            if (m_3DEngie != null)
            {
                m_3DEngie.LoadPlayerModel(strModel);
                string stranifile = GetAnimationFile(strModel, strtype);
                if (stranifile != null && stranifile.Length > 0)
                {
                    m_3DEngie.LoadAni(stranifile, 0);
                    return true;
                }
            }
            return false;
        }

        private string GetAnimationFile(string stffilenme, string strType)
        {
            string strStartDirectory = GetAnimationPath(stffilenme, strType);
            //string[] strAnimations = System.IO.Directory.GetFiles(strStartDirectory, "*.ani", System.IO.SearchOption.AllDirectories);
            string[] strAnimations = new string[0];
            if (System.IO.Directory.Exists(strStartDirectory))
                strAnimations = System.IO.Directory.GetFiles(strStartDirectory, "*.ani", System.IO.SearchOption.TopDirectoryOnly);

            if (strAnimations.Length == 0)
            {
                strStartDirectory = GetAnimationPath_UsingPlayerModel2(stffilenme);
                if (System.IO.Directory.Exists(strStartDirectory))
                    strAnimations = System.IO.Directory.GetFiles(strStartDirectory, "*.ani", System.IO.SearchOption.TopDirectoryOnly);
            }
            if (strAnimations.Length == 0)
            {
                strStartDirectory = GetAnimationPath_UsingPlayerModel(stffilenme);
                if (System.IO.Directory.Exists(strStartDirectory))
                    strAnimations = System.IO.Directory.GetFiles(strStartDirectory, "*.ani", System.IO.SearchOption.TopDirectoryOnly);
            }

            if (strAnimations.Length > 0)
            {
                foreach (string ani_file in strAnimations)
                {
                    if (ani_file.Contains("_st") || ani_file.Contains("普通待机"))
                    {
                        return ani_file;
                    }
                }

                return strAnimations[0];
            }

            return null;
        }

        private void PreviewPanel_DoubleClick(object sender, EventArgs e)
        {
            if (m_3DEngie != null)
            {
                m_3DEngie.ToggleRotateModelEditor();

                // run dbclick-handler in lua.
                DBCustomClass dbClass = ((PageDesc)m_Pages[0]).dbClass;
                if (dbClass.ExistLuaFunction("PreviewDoubleClick"))
                {
                    object[] results = dbClass.CallLuaFunction("PreviewDoubleClick");
                }
            }
        }

        public void GetCameraInfo()
        {
            if (m_3DEngie != null)
            {
                int px, py, pz, lx, ly, lz, width, height;
                m_3DEngie.GetCameraInfo(out px, out py, out pz, out lx, out ly, out lz, out width, out height);
                DBCustomClass dbClass = ((PageDesc)m_Pages[0]).dbClass;
                if (dbClass.ExistLuaFunction("GetCameraInfo"))
                {
                    object[] results = dbClass.CallLuaFunction("GetCameraInfo",
                        new object[] { px, py, pz, lx, ly, lz, width, height });
                }
            }
        }

        private void PreviewPanel_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_3DEngie == null)
                return;

            int xoff = m_oldX - e.X;
            int yoff = m_oldY - e.Y;

            if (e.Button == MouseButtons.Left)
            {
                DBCustomClass dbClass = ((PageDesc)m_Pages[0]).dbClass;
                if (dbClass.ExistLuaFunction("CustomPreviewMouseMove"))
                {
                    try
                    {
                        object[] results = dbClass.CallLuaFunction("CustomPreviewMouseMove", new object[] { yoff });
                    }
                    catch (Exception)
                    {
                    }
                }
                else
                {
                    m_3DEngie.ZoomView(yoff); // 只会在使用sceneeditor时起作用
                }
            }

            m_oldX = e.X;
            m_oldY = e.Y;
        }

        private string GetAnimationPath_UsingPlayerModel(string strFileName)
        {
            char[] Separators = { '\\', '/' };
            strFileName = strFileName.Substring(0, strFileName.LastIndexOfAny(Separators));
            strFileName = strFileName.Substring(0, strFileName.LastIndexOfAny(Separators));
            strFileName = strFileName.Substring(0, strFileName.LastIndexOfAny(Separators));
            strFileName += "\\动作";
            return strFileName;
        }
        private string GetAnimationPath_UsingPlayerModel2(string strFileName)
        {
            char[] Separators = { '\\', '/' };
            strFileName = strFileName.Substring(0, strFileName.LastIndexOfAny(Separators));
            strFileName = strFileName.Substring(0, strFileName.LastIndexOfAny(Separators));
            strFileName += "\\动作";
            return strFileName;
        }
        private string GetAnimationPath(string stffilenme, string strType)
        {
            MdlType eType;
            switch (strType.ToLower())
            {
                case "player":
                    eType = MdlType.MT_PLAYER;
                    break;
                case "npc":
                    eType = MdlType.MT_NPC;
                    break;
                case "dodad":
                    eType = MdlType.MT_DODEAD;
                    break;
                default:
                    eType = MdlType.MT_OTHER;
                    break;
            }

            switch (eType)
            {
                case MdlType.MT_PLAYER:
                case MdlType.MT_NPC:
                    {
                        string strMdlFileName = stffilenme;
                        int nPos = strMdlFileName.LastIndexOf('\\');
                        string strMdlDirName = strMdlFileName.Substring(0, nPos);
                        return strMdlDirName;
                        //return m_lua["EngineDir"] + "\\" + strMdlDefaultPath[(int)MdlType.MT_NPC] + strMdlDirName + "\\" + strMdlDefaultAnimationPathSuffix[(int)eType];
                    }
                case MdlType.MT_DODEAD:
                    {
                        char[] Separators = { '\\', '/' };
                        string strMdlFileName = stffilenme;
                        int nPos = strMdlFileName.LastIndexOfAny(Separators);
                        return strMdlFileName.Substring(0, nPos);
                    }
            }
            return null;
        }

        private void PreviewPanel_Resize(object sender, EventArgs e)
        {
            if (m_3DEngie != null)
            {
                m_3DEngie.OnResizeWindow();
            }
        }


    };
}