using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using System.Data.SqlClient;
using System.IO;
using System.Collections;
using System.Drawing.Imaging;
using System.Diagnostics;
//using DataBaseTool;
using atlaxLib;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    public partial class MainForm : Form
    {
        private ScriptDebugForm m_debugform;
        private ScriptCodeForm m_codeform;
        //private SkillForm m_skillform;
        private ModelEdit m_dbtoolsform = null;
        private DBTableEditor m_dbTableEditor = null;
        private FieldRelationForm m_fieldRelationForm = null;

        public static string m_strconn = "";
        public static SqlConnection conn = null;//new SqlConnection(m_strconn);

        // 默认值相关
        // 是否在导入数据表时就更新DBNull为默认值。
        public static bool m_bModifyDbTableDefault = false;

        // 动态功能模块的按钮
        private ButtonItem[] cmdModels;

        // 预览
        public Test3DEngine K3DEngine;

        // 当前活动的baseform
        public BaseForm m_CurBaseForm = null;

        // 保存模块id与其对应的Button的hash表
        public Hashtable imageTable = new Hashtable();

        public MainForm()
        {
            InitializeComponent();
            if (Program.forPublic)
            {
                this.buttonItem3.Visible = false;
                this.buttonItem4.Visible = false;
                this.buttonItem5.Visible = false;
                this.buttonItem6.Visible = false;
            }

#if (!DEBUG)
            barLog.AutoHide = true;
#endif
            mdiClient1.ControlAdded += new ControlEventHandler(MdiClientControlAddRemove);
            mdiClient1.ControlRemoved += new ControlEventHandler(MdiClientControlAddRemove);
        }
        #region "换肤相关"
        private void ChangeDotNetBarStyle(eDotNetBarStyle style)
        {
            cmdStyleOffice2007Blue.Checked = (style == eDotNetBarStyle.Office2007);
            cmdStyleOffice2003.Checked = (style == eDotNetBarStyle.Office2003);
            cmdStyleOfficeXP.Checked = (style == eDotNetBarStyle.OfficeXP);
            cmdStyleVS2005.Checked = (style == eDotNetBarStyle.VS2005);

            //explorerBar1.ColorScheme = new DevComponents.DotNetBar.ColorScheme(style);

            dotNetBarManager1.Style = style;
            if (style == eDotNetBarStyle.Office2003 || style == eDotNetBarStyle.OfficeXP || style == eDotNetBarStyle.VS2005)
            {
                if (dotNetBarManager1.IsThemeActive)
                {
                    //bThemes.Visible = true;
                    barStandard.RecalcLayout();
                }
            }
            else if (dotNetBarManager1.IsThemeActive)
            {
                //bThemes.Visible = false;
                barStandard.RecalcLayout();
            }

            if (style == eDotNetBarStyle.Office2003)
            {
                tabStrip1.Style = eTabStripStyle.OneNote;
            }
            else if (style == eDotNetBarStyle.Office2007)
            {
                tabStrip1.Style = eTabStripStyle.Office2007Document;
            }
            else if (style == eDotNetBarStyle.VS2005)
            {
                tabStrip1.Style = eTabStripStyle.VS2005;
            }
            else
            {
                tabStrip1.Style = eTabStripStyle.Flat;
            }

            // Status bar style
            bar1.Style = style;

            foreach (Form frm in this.MdiChildren)
            {
                /*if (frm is NPCForm)
                {
                    ((NPCForm)frm).dotNetBarManager1.Style = style;
                    return;
                }
                if (frm is SkillForm)
                {
                    ((SkillForm)frm).dotNetBarManager1.Style = style;
                    return;
                }*/
            }
        }
        private void EnableThemes(bool bEnable)
        {
            dotNetBarManager1.SuspendLayout = true;
            dotNetBarManager1.ThemeAware = bEnable;
            dotNetBarManager1.SuspendLayout = false;
            tabStrip1.ThemeAware = bEnable;

            this.Refresh();
        }
        private void cmdStyleOffice2007Blue_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.Office2007);
        }
        private void cmdStyleOfficeXP_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.OfficeXP);
        }
        private void cmdStyleVS2005_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.VS2005);
        }
        private void cmdStyleOffice2003_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.Office2003);
        }
        #endregion
        #region "ＴＡＢ页相关"
        private void SwitchWorkingMode(bool tabbedMdi)
        {
            if (tabbedMdi)
            {
                // Switch to Tabbed Mdi Interface
                dotNetBarManager1.MdiSystemItemVisible = false;
                tabStrip1.MdiTabbedDocuments = true;
                dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
            else
            {
                // Switch to standard Mdi Interface
                tabStrip1.Visible = false;
                dotNetBarManager1.MdiSystemItemVisible = true;
                tabStrip1.MdiTabbedDocuments = false;
                tabStrip1.MdiForm = null;
                SyncMdiTabbedBar();
            }
        }
        private void SyncMdiTabbedBar()
        {
            if (cmdTabbedMdi.Checked)
            {
                // Hide window list bar
                dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
            else
            {
                if (this.MdiChildren.Length > 0)
                {
                    dotNetBarManager1.Bars["windowlist"].Visible = true;
                    dotNetBarManager1.Bars["windowlist"].DockLine = 1;
                    dotNetBarManager1.Bars["windowlist"].RecalcLayout();
                }
                else
                    dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
        }
        private void cmdTabbedMdi_Click(object sender, EventArgs e)
        {
            SwitchWorkingMode(!cmdTabbedMdi.Checked);
            cmdTabbedMdi.Checked = !cmdTabbedMdi.Checked;
        }
        #endregion
        //public MetainfoEditorForm DBForm 
        public ModelEdit MetainfoForm
        {
            get { return m_dbtoolsform; }
        }
        public ScriptCodeForm CodeForm
        {
            get { return m_codeform; }
        }
        public ScriptDebugForm DebugForm
        {
            get { return m_debugform; }
        }

        static public void InitConn()
        {

            if (Program.EnableRemoting)
            {
                if (!DataTableServerProxy.IsInit)
                {
                    DataTableServerProxy.InitServerProxy(Application.ExecutablePath + ".config", Program.Database);
                }
                m_strconn = DataTableServerProxy.GetDBSourceString();
                Program.ConnetionString = m_strconn;
                conn = new SqlConnection(m_strconn);
                DataTableServerProxy.GetDataTableProxy().InitRemoting(conn);
                
            }
            else
            {
                m_strconn = Program.ConnetionString;
                conn = new SqlConnection(m_strconn);
            }
            CustomClassCache.Conn = conn;
            CustomClassCache.DelAllCaches();
            //add by cuiwei 07.10.16
            DBProcess.SetConnection(conn);
            //add end
        }

        /// <summary>
        /// 从数据库中读取图标信息
        /// </summary>
        /// <param name="r">数据库行</param>
        /// <returns>图标</returns>
        private Image GetImageFromDataBase(DataRow r)
        {
            Image image = null;
            byte[] imageData;
            try
            {
                imageData = (byte[])r["imagedata"];
                MemoryStream ms = new MemoryStream();
                ms.Write(imageData, 0, imageData.Length);
                image = Image.FromStream(ms);
                ms.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("在读取数据库图标信息时产生异常: " + ex.ToString());
            }
            return image;
        }

        /// <summary>
        /// 刷新模块id对应的Button的图标
        /// </summary>
        /// <param name="id">模块id</param>
        /// <param name="image">Button的图标</param>
        public void RefreshIcons(string id, Image image)
        {
            if (imageTable[id] != null)
            {
                ButtonItem button = imageTable[id] as ButtonItem;
                button.Image = image;
            }
        }

        /// <summary>
        /// 刷新模块id对应的Button的显示样式
        /// </summary>
        /// <param name="id">模块id</param>
        /// <param name="text">Button的文本</param>
        /// <param name="style">Button的样式</param>
        public void RefreshButtonStyle(string id, string text, int style)
        {
            if (imageTable[id] != null)
            {
                ButtonItem button = imageTable[id] as ButtonItem;
                button.Text = text;
                switch(style)
                {
                    case 0: // 默认样式
                        {
                            button.ButtonStyle = eButtonStyle.Default;
                            break;
                        }
                    case 1: // 单一文本
                        {
                            button.ButtonStyle = eButtonStyle.TextOnlyAlways;
                            break;
                        }
                    case 2: // 图文混合
                        {
                            button.ButtonStyle = eButtonStyle.ImageAndText;
                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 重新加载所有的ButtonItem
        /// </summary>
        /// <param name="itemList">ButtonItem链表</param>
        public void ReloadButtonItems(List<ButtonItem> itemList)
        {
            this.moduletoolbar.Items.Clear();
            foreach(ButtonItem b in itemList)
            {
                this.moduletoolbar.Items.Add(b);
            }
            moduletoolbar.Items.Add(customizeItem1);
            moduletoolbar.RecalcLayout();
        }

        /// <summary>
        /// 获取用户的自定义模块显示字符串
        /// </summary>
        /// <param name="hostname">用户名</param>
        /// <returns>用户自定义模块显示字符串</returns>
        private string GetConfigString(string hostname)
        {
            SqlConnection conn = new SqlConnection(Program.ConnetionString); // sql连接
            string configString = null; // 用户的自定义模块显示字符串

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                /* remoting
                string sqlString = string.Format("SELECT data FROM {0} WHERE host = '{1}'", "sys_modl_display_cfg", hostname);
                DataTable table = Helper.GetDataTable(sqlString, conn);
                if (table.Rows.Count > 0)
                {
                    if (!(table.Rows[0][0] is DBNull)) // 字符串内容非空
                    {
                        configString = table.Rows[0][0].ToString();
                    }
                }
                */

                string filter = string.Format("host = '{0}'", hostname);
                DataTable table = Helper.GetDataTableProxy("sys_modl_display_cfg", filter, null, conn);
                if (table.Rows.Count > 0)
                {
                    if (!(table.Rows[0]["data"] is DBNull)) // 字符串内容非空
                    {
                        configString = table.Rows[0]["data"].ToString();
                    }
                }

            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("在获取用户名为{0}的用户自定义模块显示数据时产生sql异常: ", hostname) + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return configString;
        }

        /// <summary>
        /// 从配置文件中读取模块显示定义信息
        /// </summary>
        /// <returns>ButtonItem链表</returns>
        private List<ButtonItem> LoadModelConfigInfo()
        {
            StringBuilder sBuilder = new StringBuilder(255);
            List<ButtonItem> buttonItemList = new List<ButtonItem>(); // 保存ButtonItem的链表
            List<string> idList = new List<string>(); // 当前显示的所有模块id链表
            ButtonItem buttonItem; // 临时ButtonItem对象
            string[] modelIDs;
            bool readHideKind = false; // 是否已经读完自动隐藏分类

            string configString = GetConfigString(System.Net.Dns.GetHostName());
            if (configString != null) // 用户自定义模块显示字符串不为空
            {
                string[] kinds = configString.Split(new char[] { ';' });
                foreach (string s in kinds)
                {
                    if (!readHideKind) // 没有读完自动隐藏分类
                    {
                        modelIDs = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                        foreach (string t in modelIDs)
                        {
                            idList.Add(t);
                        }
                        readHideKind = true;
                        continue; // 跳过不显示自动隐藏分类里的内容
                    }

                    if (s == string.Empty)
                    {
                        continue;
                    }

                    modelIDs = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string t in modelIDs)
                    {
                        if (imageTable[t] != null)
                        {
                            buttonItem = imageTable[t] as ButtonItem;
                            if (modelIDs[0] == t) // 分类的第一个ButtonItem，加分类标记
                            {
                                buttonItem.BeginGroup = true;
                            }
                            else
                            {
                                buttonItem.BeginGroup = false;
                            }
                            buttonItemList.Add(buttonItem);
                        }
                        idList.Add(t);
                    }
                }
            }

            foreach (string s in imageTable.Keys) // 检查是否有新加的图标没有显示出来
            {
                if (!idList.Contains(s))
                {
                    if (imageTable[s] != null)
                    {
                        buttonItem = imageTable[s] as ButtonItem;
                        buttonItem.BeginGroup = false;
                        buttonItemList.Add(buttonItem);
                    }
                }
            }

            /*if(buttonItemList.Count == 0) // 配置文件中没有模块显示自定义信息
            {
                foreach(object o in imageTable.Values)
                {
                    buttonItemList.Add(o as ButtonItem);
                }
            }*/

            return buttonItemList;
        }


        /*
        private void InitUserAclDesc()
        {

            conn.Open();
            SqlCommand sqlCmd = new SqlCommand("delete from sys_useracl_desc", conn);
            sqlCmd.ExecuteNonQuery();
            sqlCmd.Dispose();
            conn.Close();

            DataTable tblAcl = Helper.GetDataTable("select userid,_index from sys_useracl", conn);
            DataTable tblModl = Helper.GetDataTable("select id,modelname_en from sys_modl_def", conn);
            DataTable tblAclDesc = Helper.GetDataTable("select * from sys_useracl_desc", conn);
            SqlDataAdapter adp = new SqlDataAdapter("select * from sys_useracl_desc", conn);

            foreach (DataRow rowAcl in tblAcl.Rows)
            {
                foreach (DataRow rowModl in tblModl.Rows)
                {
                    DataRow rowDesc = tblAclDesc.NewRow();
                    rowDesc.BeginEdit();
                    rowDesc["userindex"] = rowAcl["_index"];
                    rowDesc["moduleid"] = rowModl["id"];
                    rowDesc["groupname"] = "";
                    rowDesc["modulename_en"] = rowModl["modelname_en"];
                    rowDesc["canread"] = 1;
                    rowDesc["canwrite"] = 1;
                    rowDesc["canimport"] = 1;
                    rowDesc["canconfigmeta"] = 1;
                    rowDesc.EndEdit();
                    tblAclDesc.Rows.Add(rowDesc);
                }
            }

            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tblAclDesc);
            tblAclDesc.AcceptChanges();
        }
        private void InitUserAclDesc2()
        {

            DataTable tblAcl = Helper.GetDataTable("select userid,_index from sys_useracl", conn);
            SqlDataAdapter adp = new SqlDataAdapter("select * from sys_useracl_desc", conn);
            DataTable tblAclDesc = Helper.GetDataTable("select * from sys_useracl_desc", conn);
            foreach (DataRow rowDesc in tblAclDesc.Rows)
            {
                rowDesc.BeginEdit();
                int userid = Convert.ToInt32(rowDesc["userindex"]);
                string username = tblAcl.Rows.Find(userid)["userid"].ToString().Trim();
                string s = username.Substring(0, 1).ToLower();
                if (string.Compare(s, "j") <= 0)
                    rowDesc["groupname"] = "A - J";
                else if (string.Compare(s, "s") <= 0)
                    rowDesc["groupname"] = "K - S";
                else if (string.Compare(s, "x") <= 0)
                    rowDesc["groupname"] = "T - X";
                else
                    rowDesc["groupname"] = "Y - Z";
                rowDesc.EndEdit();

            }
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tblAclDesc);
            tblAclDesc.AcceptChanges();
        }

        private void GenQuestText()
        {
            string Output = "剑侠情缘网络版叁 任务脚本\r\n\r\n\r\n";

            StringBuilder sb = new StringBuilder();
            DataTable tblQuest = Helper.GetDataTable("select questname, objective, description, dunningdialogue, unfinisheddialogue, finisheddialogue from tbl_quests order by class, questid", conn);
            foreach (DataRow row in tblQuest.Rows)
            {
                if (row["QuestName"].ToString().Length != 0)
                {
                    sb.Append("任务名称：");
                    sb.Append(row["QuestName"].ToString());
                    sb.Append("\r\n任务目标：");
                    sb.Append(row["Objective"].ToString());
                    sb.Append("\r\n任务描述：");
                    sb.Append(row["Description"].ToString());
                    sb.Append("\r\n未完成对话1：");
                    sb.Append(row["DunningDialogue"].ToString());
                    sb.Append("\r\n未完成对话2：");
                    sb.Append(row["UnfinishedDialogue"].ToString());
                    sb.Append("\r\n完成对话：");
                    sb.Append(row["FinishedDialogue"].ToString());
                    sb.Append("\r\n\r\n");
                }
            }

            Output += sb.ToString();
            Output = Output.Replace("<G>", string.Empty);
            Output = Output.Replace("\\n", string.Empty);
            Output = Output.Replace("<C>", string.Empty);
            Output = Output.Replace("<H28>", string.Empty);
            Output = Output.Replace("<N>", string.Empty);
            FileFolderHelper.StringToFile(Output, "c:/q.txt");
        }
        private void GenNpcText()
        {
            string Output = "剑侠情缘网络版叁 任务脚本\r\n\r\n\r\n";

            StringBuilder sb = new StringBuilder();
            DataTable tblNpc = Helper.GetDataTable("select name, idledialog1, idledialog2, idledialog3, idledialogafterquest from npctemplate order by mapname, id", conn);
            foreach (DataRow row in tblNpc.Rows)
            {
                if (row["IdleDialog1"].ToString().Length != 0)
                {
                    sb.Append("NPC名称：");
                    sb.Append(row["Name"].ToString());
                    sb.Append("\r\n待机对话1：");
                    sb.Append(row["IdleDialog1"].ToString());
                    sb.Append("\r\n待机对话2：");
                    sb.Append(row["IdleDialog2"].ToString());
                    sb.Append("\r\n待机对话3：");
                    sb.Append(row["IdleDialog3"].ToString());
                    sb.Append("\r\n任务后待机对话：");
                    sb.Append(row["IdleDialogAfterQuest"].ToString());
                    sb.Append("\r\n\r\n");
                }
            }

            Output += sb.ToString();
            Output = Output.Replace("<G>", string.Empty);
            Output = Output.Replace("\\n", string.Empty);
            Output = Output.Replace("<C>", string.Empty);
            Output = Output.Replace("<H28>", string.Empty);
            Output = Output.Replace("<N>", string.Empty);
            FileFolderHelper.StringToFile(Output, "c:/n.txt");
        }
        private void OldIpToNewIp()
        {
            string sql = "select * from sys_useracl";
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            DataTable tbl = Helper.GetDataTable(sql, conn);
            foreach (DataRow row in tbl.Rows)
            {
                string hostname = row["userid"].ToString().Trim();
                System.Net.IPHostEntry ipEntry = null;
                try
                {
                    ipEntry = System.Net.Dns.GetHostByName(hostname);
                }
                catch (Exception ex)
                {
                    if ((ex as System.Net.Sockets.SocketException) != null)
                        ipEntry = null;
                }
                if (ipEntry == null)
                    continue;

                //获取IP地址列表
                foreach (System.Net.IPAddress ip in ipEntry.AddressList)
                {
                    string strIp = ip.ToString();
                    if (strIp.StartsWith("10.20."))
                    {
                        row["ip"] = strIp;
                    }
                }

            }

            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
        }
         */ 
        private void MainForm_Load(object sender, EventArgs e)
        {
            //InitUserAclDesc();
            //InitUserAclDesc2();
            //GenQuestText();
            //GenNpcText();
            //OldIpToNewIp();

            //Helper.LockWndUpdate(this);
            try
            {
                //System.Diagnostics.Process.Start("regsvr32.exe   /s", "atlax.dll");  

                //InitConn();

                tabStrip1.MdiForm = this;
                m_debugform = new ScriptDebugForm();
                m_codeform = new ScriptCodeForm(this);

                // 从sys_modl_def读取功能模块信息 只列出来该用户可读的
                string sql;
                if (Program.m_bUseNewAcl)
                    sql = string.Format("SELECT * FROM sys_modl_def WHERE (id IN (SELECT moduleid FROM sys_useracl_desc WHERE userindex = {0} AND canread = 1))", Program.m_rightmgr._userindex);
                else
                    sql = "SELECT * FROM sys_modl_def";

                string[] names = new string[] { "sys_modl_def", "sys_useracl_desc" };
                DataTable tbl = Helper.GetDataTableWithSqlProxy(names, sql, conn);
                cmdModels = new ButtonItem[tbl.Rows.Count];
                int i = 0;
                // 绑定新的点击事件处理方法
                customizeItem1.Click += new EventHandler(bConfigModel_Click);

                foreach (DataRow row in tbl.Rows)
                {
                    cmdModels[i] = new ButtonItem();                  
                    this.moduletoolbar.Items.AddRange(new BaseItem[] { cmdModels[i] });
                    cmdModels[i].Name = "cmd" + row["modelname_en"] + "editor";
                    cmdModels[i].GlobalName = cmdModels[i].Name;
                    //cmdModels[i].ImageIndex = Convert.ToInt32(row["iconid"]);
                    if (Convert.ToInt32(row["iconid"]) < imageList2.Images.Count)
                    {

                        if (Convert.ToInt32(row["iconid"]) == -1) // 自定义图标
                        {
                            cmdModels[i].Image = GetImageFromDataBase(row);
                        }
                        else
                        {
                            cmdModels[i].Image = imageList2.Images[Convert.ToInt32(row["iconid"])];
                        }
                    }
                    cmdModels[i].PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
                    if(!(row["displaystyle"] is DBNull)) // 自定义显示样式不为空
                    {
                        cmdModels[i].Text = row["displaytext"].ToString();
                        cmdModels[i].Tooltip = row["modelname_en"].ToString();

                        switch(row["displaystyle"].ToString())
                        {
                            case "0": // 默认样式
                                {
                                    cmdModels[i].ButtonStyle = eButtonStyle.Default;
                                    break;
                                }
                            case "1": // 单一文本
                                {
                                    cmdModels[i].ButtonStyle = eButtonStyle.TextOnlyAlways;
                                    break;
                                }
                            case "2": // 图文混合
                                {
                                    cmdModels[i].ButtonStyle = eButtonStyle.ImageAndText;
                                    break;
                                }
                        }
                    }
                    else
                    {
                        cmdModels[i].Text = row["modelname_en"].ToString();
                        cmdModels[i].Tooltip = cmdModels[i].Text;
                    }
    
                    cmdModels[i].Click += new System.EventHandler(this.cmdNewModelEditor_Click);
                    imageTable[row["id"].ToString()] = cmdModels[i]; // 记录模块id对应的Button
                    cmdModels[i].Tag = row["id"].ToString(); // 记录该模块的id
                }
                moduletoolbar.RecalcLayout();
                FileInfo _fi = new FileInfo("_layout.xml");
                FileInfo fi = new FileInfo("layout.xml");
                if (_fi.Exists)
                {
                    dotNetBarManager1.LoadLayout("_layout.xml"); // 读取布局设置
                }
                else if (fi.Exists)
                {
                    dotNetBarManager1.LoadLayout("layout.xml"); // 读取布局设置
                }

                // 从配置文件中读取模块显示的自定义内容
                ReloadButtonItems(LoadModelConfigInfo());

                //初始化状态条内容
                LDBName.Text = conn.DataSource + " : " + conn.Database;
                LHostName.Text = Helper.GetHostName() + " : " + Helper.GetIPAddress();
                
                //谁要是再截屏，hehe
                try
                {
                    string strHost = Helper.GetHostName().ToLower();
                    string _sql = string.Format("select * from sys_useracl where userid = '{0}'", strHost);

                    /*DataTable tbl_acl = CustomClassCache.GetDataTableWithKey(_sql);*/

                    string filter = string.Format("userid = '{0}'", strHost);
                    DataTable tbl_acl = CustomClassCache.GetDataTableWithKeyProxy("sys_useracl", filter, null);
                   
                    DataRow row_acl = tbl_acl.Rows[0];
                    this.Text = string.Format("剑3游戏设计工具(1.0.2.{0})", row_acl["_index"].ToString().Trim());

                    if (Program.forPublic)
                    {
                        this.Text = string.Format("策划工具(1.0.2.{0})", row_acl["_index"].ToString().Trim());
                    }
                }
                catch (Exception)
                {
                    Helper.AddLog("出错啦～～～");
                }
            }
            catch (Exception ex)
            {
                Helper.AddLog(ex.Message);
                this.Close();
                throw;
            }
            finally
            {
                //Helper.LockWndUpdate(null);
            }
        }

        public void UpdateAllBaseFormTmrRenderInterval()
        {
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is BaseForm)
                {
                    ((BaseForm)frm).SetTmrRenderInterval(Program.m_3d_tmrRender);
                }
            }
        }

        public void OpenForm(string modelname, object locate/*打开后定位到某个结点的依据,通常是一个id.*/)
        {
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is BaseForm && ((BaseForm)frm).ModelName == modelname)
                {
                    frm.Activate();
                    return;
                }
            }

            BaseForm doc = null;
            try
            {
                doc = new BaseForm(modelname);
                doc.MdiParent = this;
                doc.WindowState = FormWindowState.Maximized;
                doc.Show();
                doc.Update();
            }
            catch (Exception ex)
            {
                if (doc != null)
                    doc.Close();
                MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Stop);
            }

            m_CurBaseForm = doc;

            if (locate != null)
            {
                // 定位的代码.

            }
        }

        private void cmdNewModelEditor_Click(object sender, EventArgs e)
        {
            Helper.AddTimeLog("load tree begin");
            ButtonItem buttonSender = sender as ButtonItem;
            if (buttonSender != null)
            {
                string strModelName = buttonSender.Tooltip.Trim();
                OpenForm(strModelName, null);
            }
            else
            {
                MessageBox.Show("模块设置或者控件有误.", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            Helper.AddTimeLog("load tree end");
        }

        private void cmdNewDocument_Click(object sender, EventArgs e)
        {
        }
        private void buttonItem3_Click(object sender, EventArgs e)
        {
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is NPCForm)
                {
                    frm.Activate();
                    return;
                }
            }
            NPCForm doc = null;
            try
            {
                doc = new NPCForm();
                doc.MdiParent = this;
                doc.WindowState = FormWindowState.Maximized;
                doc.Show();
                doc.Update();
            }
            catch
            {
                doc.Close();
            }
        }
        private void MdiClientControlAddRemove(object sender, ControlEventArgs e)
        {
            if (e.Control is Form)
                SyncMdiTabbedBar();
        }
        private void LoadPopup(object sender, System.EventArgs e)
        {
            ButtonItem item = sender as ButtonItem;
            if (item == bTabColor)
            {
                DevComponents.DotNetBar.PopupContainerControl container = item.PopupContainerControl as PopupContainerControl;
                ColorPicker clr = new ColorPicker();
                container.Controls.Add(clr);
                if (cmdStyleOffice2003.Checked)
                {
                    clr.BackColor = dotNetBarManager1.Bars[0].ColorScheme.BarBackground2;
                    clr.tabStrip1.Style = eTabStripStyle.Office2003;
                }
                clr.Location = container.ClientRectangle.Location;
                container.ClientSize = clr.Size;
            }
        }
        private void UnloadPopup(object sender, System.EventArgs e)
        {
            ButtonItem item = sender as ButtonItem;
            if (item.Name == "bTabColor")
            {
                DevComponents.DotNetBar.PopupContainerControl container = item.PopupContainerControl as PopupContainerControl;
                ColorPicker clr = container.Controls[0] as ColorPicker;
                if (clr.SelectedColor != Color.Empty)
                {
                    tabStrip1.ColorScheme.TabBackground = ControlPaint.LightLight(clr.SelectedColor);
                    tabStrip1.ColorScheme.TabBackground2 = clr.SelectedColor;
                    tabStrip1.Refresh();
                }
                // Close popup menu, since it is not closed when Popup Container is closed...
                item.Parent.Expanded = false;
            }
        }
        private TabItem TabItemFromPoint(Point pClient)
        {
            if (!tabStrip1.ClientRectangle.Contains(pClient))
                return null;
            foreach (TabItem tab in tabStrip1.Tabs)
            {
                if (tab.Visible && tab.DisplayRectangle.Contains(pClient))
                    return tab;
            }

            return null;
        }

        private void dotNetBarManager1_ItemClick(object sender, EventArgs e)
        {
            BaseItem item = sender as BaseItem;
            if (!(ActiveMdiChild is IKDocument))
                return;

            IKDocument activedocument = this.ActiveMdiChild as IKDocument;

            // Activate the form
            if (item.Name == "window_list")
                ((Form)item.Tag).Activate();
            //else if (item == bThemes)
            //    EnableThemes(bThemes);
            //else if (item.GlobalName == bTextColor.GlobalName && activedocument != null)
            //    activedocument.ExecCommand(item.GlobalName, ((ColorPickerDropDown)item).SelectedColor);
            else if (activedocument != null)
            {
                // Pass command to the active document
                // Note the usage of GlobalName property! Since same command can be found on both menus and toolbars, for example Bold
                // you have to give two different names through Name property to these two instances. However, you can and should
                // give them same GlobalName. GlobalName will ensure that properties set on one instance are replicated to all
                // objects with the same GlobalName. You can read more about Global Items feature in help file.
                activedocument.ExecCommand(item.GlobalName, null);
            }
        }
        private void bOptions_Click(object sender, EventArgs e)
        {
            AppOptionsForm fm = null;
            try
            {
                fm = new AppOptionsForm();
                if (fm.ShowDialog() == DialogResult.OK)
                    fm.Update();
            }
            catch
            {
                //if (fm != null)
                //    fm.Close();
            }
        }

        private void bMetainfoEditor_Click(object sender, EventArgs e)
        {
            ShowMetainfoEditor();
        }
        public void ShowMetainfoEditor()
        {
            try
            {
                OpenMetaInfoEditorCommand cmd = new OpenMetaInfoEditorCommand();
                cmd.Execute();

                ModelEdit frm = m_dbtoolsform;
                if (frm != null)
                {
                    frm.Visible = true;
                    frm.BringToFront();
                    return;
                }
                try
                {
                    frm = new ModelEdit();
                    frm.Show();
                    m_dbtoolsform = frm;
                }
                catch
                {
                    throw;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void bDBTableEditor_Click(object sender, EventArgs e)
        {
            ShowDBTableEditor();
        }
        private void ShowDBTableEditor()
        {
            try
            {
                OpenDBToolsCommand cmd = new OpenDBToolsCommand();
                cmd.Execute();

                DBTableEditor frm = m_dbTableEditor;
                if (frm != null)
                {
                    frm.Visible = true;
                    frm.BringToFront();
                    return;
                }
                try
                {
                    frm = new DBTableEditor();
                    frm.Show();
                    m_dbTableEditor = frm;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void cmdExport_Click(object sender, EventArgs e)
        {
            if ((BaseForm)this.ActiveMdiChild == null)
            {
                MessageBox.Show("必须先激活一个模块窗口!", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            try
            {
                int nModelID = ((BaseForm)this.ActiveMdiChild).ModID;
                TabExport frm = new TabExport(nModelID, conn);
                frm.ShowDialog();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void MainForm_MdiChildActivate(object sender, EventArgs e)
        {
            BaseForm activefrm = (BaseForm)(((MainForm)sender).ActiveMdiChild);
            if (activefrm == null)
                return;
            string name = activefrm.ModelName + "[" + activefrm.tabControl.SelectedTabIndex.ToString() + "]";
            if (BaseForm.K3DEngine != null)
            {
                BaseForm.K3DEngine.ActivateScene(name);
                BaseForm.K3DEngine.OnResizeWindow();
            }
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            dotNetBarManager1.SaveLayout(Application.StartupPath + "/_layout.xml"); // 保存布局设置
            string sql = string.Format("delete from sys_lock_record where [lockip] = '{0}'", Helper.GetIPAddress());
            int count = Helper.ExecuteCommand("sys_lock_record", sql, conn);

        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (K3DEngine != null)
            {
                K3DEngine.UnInit();
                K3DEngine = null;
            }
        }

        private void bRestartForm_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("您确定要重启程序吗？", "提示", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button2);
            if (result == DialogResult.Yes)
            {
                if (Helper.CloseAllWindows(this))
                {
                    Program.Restart = true;
                }
            }
        }

        /// <summary>
        /// 编辑脚本文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem3_Click_1(object sender, EventArgs e)
        {
            string strWholeExePath = Application.StartupPath + "/GameLuaEditor.exe";
            if (!File.Exists(strWholeExePath))
            {
                MessageBox.Show("找不到脚本编辑相关程序。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Process.Start(strWholeExePath, "start");
        }

        private void buttonItem4_Click(object sender, EventArgs e)
        {
            string strWholeExeDir = Application.StartupPath + "/MapCutter.exe";
            if (!File.Exists(strWholeExeDir))
            {
                MessageBox.Show("找不到程序: MapCutter.exe。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Process.Start(strWholeExeDir);
        }

        private void buttonItem5_Click(object sender, EventArgs e)
        {
            string strWholeExeDir = Application.StartupPath + "/LogicalEditor.exe";
            if (!File.Exists(strWholeExeDir))
            {
                MessageBox.Show("找不到程序: LogicalEditor.exe。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Process.Start(strWholeExeDir);
        }

        /// <summary>
        /// 自定义显示模块
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bConfigModel_Click(object sender, EventArgs e)
        {
            DBTools.ModelConfigForm mForm = new DBTools.ModelConfigForm(this);
            DialogResult result = mForm.ShowDialog();
        }

        private void buttonItem6_Click(object sender, EventArgs e)
        {
            string strLua = @"
                luanet.load_assembly('IconSelector.dll')
		        local cls1 = luanet.import_type('IconSelector.Implement')
		        iconselector = cls1()
		        local os = iconselector
                os:EnterEditMode()
		        os:Init(Conn, RootDir..'\\Ui\\Image\\icon')
                os:Show()
                ";
            LuaEx m_lua = new LuaEx();
            m_lua["RootDir"] = Program.RootDir;
            m_lua["Conn"] = MainForm.conn;
            m_lua.DoString(strLua);
        }

        public void AddLog(string strlog)
        {
            this.LogEditBox.AppendText(strlog + "\r\n");
            this.LogEditBox.Select(this.LogEditBox.Text.Length, 1);
            //this.LogEditBox.ScrollToCaret();
        }

        private void bFieldRelation_Click(object sender, EventArgs e)
        {
            if (m_fieldRelationForm == null)
            {
                m_fieldRelationForm = new FieldRelationForm(conn);
            }

            m_fieldRelationForm.Show();
            m_fieldRelationForm.BringToFront();
        }
    }
    public interface IKDocument
    {
        void ExecCommand(string strCmd, object data);
    }
}