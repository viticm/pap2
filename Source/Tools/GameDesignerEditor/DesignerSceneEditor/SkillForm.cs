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
//using Prj2;

namespace GameDesingerTools
{
    public partial class SkillForm : Form, IKDocument
    {
        enum UpdateType { ADD, MOD, DEL};
        Object m_lastSelectedSkillID = null;
        PropertyGridHelper m_pghelper;
        PropertyGridHelper m_pghelper2;
        DataTable m_School_Table, m_KungfuCat_Table, m_Kungfutl_Table, m_Kungfuzs_Table, m_Caster_Table;
        
        const int StartID = 10000;
        static int m_SchoolNewCount = 0;
        static int m_KungfuCatNewCount = 0;
        static int m_KFTLCount = 0;
        static int m_KFZSCount = 0;
        string m_strsql = "SELECT * FROM tbl_skills";
        string m_strsql2 = "SELECT * FROM tbl_skill_caster";

        Lua m_lua;
        Hashtable m_scriptExecState = new Hashtable();

        public void ClearTreeNodes(ref TreeNodeCollection nodes)
        {
            nodes.Clear();
            //this.skillTree.Nodes.Clear();
        }
        public TreeNode AddTreeNodes(ref TreeNodeCollection nodes, string name, object tag)
        {
            TreeNode newNode = nodes.Add(name);
            newNode.Tag = tag;
            newNode.ImageIndex = Convert.ToInt32(tag);
            return newNode;
        }
        public void SqlSetPrimKey(ref DataTable table, string strKey)
        {
            DataColumn[] primaryKey = new DataColumn[1];
            primaryKey[0] = table.Columns[strKey];
            table.PrimaryKey = primaryKey;
        }
        public DataSet SqlQueryOnce(string sql, SqlConnection Conn) 
        {
            SqlDataAdapter adpt = new SqlDataAdapter(sql, Conn);
            DataSet ds = new System.Data.DataSet();
            ds.Clear();
            adpt.Fill(ds);
            return ds;
        }

        public SkillForm()
        {
            InitializeComponent();
        }

        public void ExecCommand(string strCmd, object data)
        {
            switch(strCmd.Trim().ToLower())
            {
                case "savedocument":
                    m_pghelper.Save();
                    m_pghelper2.Save();
                    break;
                case "cmdexport":
                    ExportSkillTab();
                    break;
                case "cmdimport":
                    ImportSkillTab();
                    break;
                default:
                    break;
            }
        }



        private void ImportSkillTab()
        {
            string strFile = FileFolderHelper.BrowserFile();
            if (strFile != "")
            {
                MainForm.ImportTab(strFile, m_strsql, new int[1] { 1 });
            }
        }

        private void ExportSkillTab()
        {
            // check if .ini exists
            string DestFolder;
            if (System.IO.File.Exists(Application.StartupPath + "\\GameDesingerTools.ini"))
            {
                StringBuilder sb = new StringBuilder(260);
                FileFolderHelper.ReadIniFile("Skill Tab", "DefaultStorageDir", "", sb, 260, Application.StartupPath + "\\GameDesingerTools.ini");
                DestFolder = sb.ToString();
            }
            else
            {
                DestFolder = FileFolderHelper.BrowserFolder("把 skill.tab 保存到哪个文件夹?");
                FileFolderHelper.WriteIniFile("Skill Tab", "DefaultStorageDir", DestFolder, "./GameDesingerTools.ini");
            }

            if (DestFolder != "")
            {
                MainForm.ExportTab(DestFolder + "\\Skills.tab", m_strsql);
            }
        }
        public bool SqlRemoveLevelRecord(int iSkillID, int iSkillLevel)
        {
            System.Data.DataRow row = m_Caster_Table.Rows.Find(new object[] { iSkillID, iSkillLevel });
            row.Delete();
            return Updatedata(m_strsql2, m_Caster_Table, UpdateType.DEL);
        }
        public void SqlInsertLevelRecord(int iSkillID, int iSkillLevel)
        {
            SqlDataAdapter sqlAdptr = new SqlDataAdapter(m_strsql2, MainForm.conn);
            System.Data.DataSet dataSet = new System.Data.DataSet();
            sqlAdptr.Fill(dataSet);
            DataTable tbl = dataSet.Tables[0];
            DataRow newRow = tbl.NewRow();
            newRow["skillid"] = iSkillID;
            newRow["bodyformid"] = 1;
            newRow["skilllevel"] = iSkillLevel;
            newRow["missileid"] = 0;
            newRow["preparecastskillanimationid"] = 10;
            newRow["castskillanimationid"] = 10;
            newRow["ispreparecastskillturntotarget"] = 0;
            newRow["iscastskillturntotarget"] = 0;
            newRow["physicsblockeffectresultid"] = 0;
            newRow["solarmagicblockeffectresultid"] = 0;
            newRow["neutralmagicblockeffectresultid"] = 0;
            newRow["lunarmagicblockeffectresultid"] = 0;
            newRow["poisonblockeffectresultid"] = 0;
            newRow["hiteffectresultid"] = 0;
            newRow["reflectdamageeffectresultid"] = 0;
            newRow["criticalstrikeeffectresultid"] = 0;
            newRow["physicsdamageeffectresultid"] = 0;
            newRow["solarmagicdamageeffectresultid"] = 0;
            newRow["neutralmagicdamageeffectresultid"] = 0;
            newRow["lunarmagicdamageeffectresultid"] = 0;
            newRow["poisondamageeffectresultid"] = 0;
            newRow["healeffectresultid"] = 0;
            newRow["steallifeeffectresultid"] = 0;
            newRow["absorbdamageeffectresultid"] = 0;
            newRow["shielddamageeffectresultid"] = 0;
            newRow["aoeselectionsfxscale"] = 1;
            newRow["aoeselectionsfxfile"] = "Data/source/other/特效/技能/SFX/特殊/halo.Sfx";
            tbl.Rows.Add(newRow);
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(sqlAdptr);
            sqlAdptr.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = sqlAdptr.Update(tbl);
            tbl.AcceptChanges();
        }
        private void InitLua()
        {
            try
            {
                m_lua = new Lua();
                m_lua["MainForm"] = (MainForm)ParentForm;
                m_lua["SkillForm"] = this;
                //m_lua["oo"] = new tcls1();
                //建立环境帮助函数
                m_lua.DoString("function trace() \r\n end\r\n");
                string sCode = "____main_env_varTable = {}\r\nlocal function _MakeEnv(envname)\r\n	____main_env_varTable[envname] = {}\r\n	_G[\"__fname__\"..envname] = ____main_env_varTable[envname]\r\n____main_env_varTable[envname].envname = envname\r\n	setmetatable(____main_env_varTable[envname], {__index = _G})\r\nend\r\nfunction _ChgEnv(envname)\r\n	if (envname == nil) then\r\n        setfenv(2, _G)\r\n	elseif (____main_env_varTable[envname] == nil) then\r\n  		_MakeEnv(envname)\r\n  		setfenv(2, ____main_env_varTable[envname])\r\n else\r\n    	setfenv(2, ____main_env_varTable[envname])\r\n	end\r\nend\r\n";
                m_lua.DoString(sCode);
                m_lua.RegisterFunction("ClearTreeNodes", this, typeof(SkillForm).GetMethod("ClearTreeNodes"));
                m_lua.RegisterFunction("AddTreeNodes", this, typeof(SkillForm).GetMethod("AddTreeNodes"));
                m_lua.RegisterFunction("SqlQueryOnce", this, typeof(SkillForm).GetMethod("SqlQueryOnce"));
                m_lua.RegisterFunction("SqlInsertLevelRecord", this, typeof(SkillForm).GetMethod("SqlInsertLevelRecord"));
                m_lua.RegisterFunction("SqlSetPrimKey", this, typeof(SkillForm).GetMethod("SqlSetPrimKey"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void TryLoadTree()
        {
            string sMainCode = "";
            SqlDataAdapter adp = new SqlDataAdapter("select * from tbl_scriptstore where tablename='tbl_skills' and fieldname='#'", MainForm.conn);
            System.Data.DataSet ds = new System.Data.DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            DataRow row = tbl.Rows[0];
            try
            {
                m_lua.DoString("_ChgEnv(\"#\")\r\n");
                //sMainCode = row["scriptcode"].ToString().Trim();
                sMainCode = Helper.GetStringFromByteArray(row["scriptcode"]);

                m_lua.DoString(sMainCode);
                LuaFunction fLoadTree = m_lua.GetFunction("__fname__#.LoadTree");
                if (fLoadTree != null)
                    fLoadTree.Call();
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
                ScriptCodeForm frm = ((MainForm)ParentForm).CodeForm;
                frm.CurCode = sMainCode;//frm.OutputBox.Text = sMainCode;
                frm.LoadHisCode(4, "0");
                    //"tbl_skills", "#");
                frm.ShowDialog();
                if (frm.DialogResult == DialogResult.OK)
                {
                    row["tablename"] = "tbl_skills";
                    row["fieldname"] = "#";
                    row["scriptlanguage"] = "LUA";
                    row["scriptcode"] = Helper.GetByteArrayFromString(frm.OutputBox.Text);

                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.UpdateCommand = cmdBuilder.GetUpdateCommand();

                    int val = adp.Update(tbl);
                    tbl.AcceptChanges();
                }
                this.Close();
            }
        }
        private void SkillForm_Load(object sender, EventArgs e)
        {
            InitLua();
            TryLoadTree();

            m_pghelper = new PropertyGridHelper(MainForm.m_strconn, m_strsql, "tbl_skills", new string[] { "SkillID" });
            m_pghelper2 = new PropertyGridHelper(MainForm.m_strconn, m_strsql2, "tbl_skill_caster", new string[] { "SkillID", "SkillLevel" });

            DoScript("#");//执行表函数体

            //CustomClass dynaobj = m_pghelper.GetDynamicClass(new object[] {0});
            //dynaobj.DataValidate += new CustomClass.PropertyChangeEventHandler(PropertyChanged);

            propertyGrid1.SelectedObject = null;// dynaobj;
            propertyGrid1.Refresh();
            propertyGrid2.SelectedObject = null;
            propertyGrid2.Refresh();
        }

        private void LoadDataBase()
        {
            //从tbl_skills中读取所有武功套路的数据到m_Kungfutl_Table中,并设"SkillID"列为主键
            SqlDataAdapter adp = new SqlDataAdapter("select * from tbl_skills where BelongKungfu = '0'", MainForm.conn);
            System.Data.DataSet dsKFtl = new System.Data.DataSet();
            adp.Fill(dsKFtl);
            m_Kungfutl_Table = dsKFtl.Tables[0];
            DataColumn[] primarKey = new DataColumn[1];
            primarKey[0] = m_Kungfutl_Table.Columns["SkillID"];
            m_Kungfutl_Table.PrimaryKey = primarKey;

            //从dic_school中读取所有门派类型数据到m_School_Table中
            System.Data.DataSet dsSchool = new System.Data.DataSet();
            adp = new SqlDataAdapter("select * from dic_school", MainForm.conn);
            adp.Fill(dsSchool);
            m_School_Table = dsSchool.Tables[0];
            primarKey[0] = m_School_Table.Columns["Name"];
            m_School_Table.PrimaryKey = primarKey;

            //从dic_kungfu_cat中读取所有武功类型数据到m_KungfuCat_Table;
            adp = new SqlDataAdapter("select * from dic_kungfu_cat", MainForm.conn);
            System.Data.DataSet dsKFCat = new System.Data.DataSet();
            adp.Fill(dsKFCat);
            m_KungfuCat_Table = dsKFCat.Tables[0];
            primarKey[0] = m_KungfuCat_Table.Columns["name"];
            m_KungfuCat_Table.PrimaryKey = primarKey;

            //从tbl_skills中读取所有数据到m_Skills_Table
            adp = new SqlDataAdapter("select * from tbl_skills where BelongKungfu <> '0'", MainForm.conn);
            System.Data.DataSet dsKFZS = new System.Data.DataSet();
            adp.Fill(dsKFZS);
            m_Kungfuzs_Table = dsKFZS.Tables[0];
            try
            {
                primarKey[0] = m_Kungfuzs_Table.Columns["SkillID"];
                m_Kungfuzs_Table.PrimaryKey = primarKey;
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

            //从tbl_skill_caster中读取所有数据到m_Caster_Table
            adp = new SqlDataAdapter("select * from tbl_skill_caster", MainForm.conn);
            System.Data.DataSet dsCaster = new System.Data.DataSet();
            adp.Fill(dsCaster);
            m_Caster_Table = dsCaster.Tables[0];
            DataColumn[] primKeyCaster = new DataColumn[2];
            try
            {
                primKeyCaster[0] = m_Caster_Table.Columns["SkillID"];
                primKeyCaster[1] = m_Caster_Table.Columns["SkillLevel"];
                m_Caster_Table.PrimaryKey = primKeyCaster;
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        void LoadTree()
        {
            //初始化左边的树
            this.skillTree.Nodes.Clear();

            SqlDataAdapter adp;
            //BelongKungfu=0 表示 这是套路 <> 0表示是招式，并归属于某套路
            string sfmt = "SELECT * FROM tbl_skills WHERE (Type = {0}) AND (BelongSchool = {1} AND BelongKungfu={2})"; 
            System.Data.DataSet dskungfutl = new System.Data.DataSet(); //套路
            System.Data.DataSet dskungfuzs = new System.Data.DataSet(); //招式

            foreach (System.Data.DataRow row in m_School_Table.Rows)//门派
            {
                TreeNode nodeschool = this.skillTree.Nodes.Add(row["Name"].ToString());
                nodeschool.Tag = row[0];//ID
                nodeschool.ImageIndex = Convert.ToInt32(nodeschool.Tag);
                foreach (System.Data.DataRow rowkfcat in m_KungfuCat_Table.Rows)//武功类别:内、外功
                {
                    TreeNode nodekfcat = nodeschool.Nodes.Add(rowkfcat["name"].ToString());
                    nodekfcat.Tag = rowkfcat[0];
                    //nodekfcat.ImageIndex = 1000;

                    string s = String.Format(sfmt, nodekfcat.Tag, nodeschool.Tag, 0); //belongkungfu=0:套路
                    adp = new SqlDataAdapter(s, MainForm.conn);
                    dskungfutl.Clear();
                    adp.Fill(dskungfutl);
                    foreach (System.Data.DataRow rowkf in dskungfutl.Tables[0].Rows) //套路
                    {
                        TreeNode nodekf = nodekfcat.Nodes.Add(rowkf["SkillName"].ToString());
                        nodekf.Tag = rowkf["SkillID"];
                        //nodekf.ImageIndex = 1000;

                        string szs = String.Format(sfmt, nodekfcat.Tag, nodeschool.Tag, nodekf.Tag);
                        adp = new SqlDataAdapter(szs, MainForm.conn);
                        dskungfuzs.Clear();
                        adp.Fill(dskungfuzs);
                        foreach (System.Data.DataRow rowkfzs in dskungfuzs.Tables[0].Rows) //招式
                        {
                            TreeNode nodekfzs = nodekf.Nodes.Add(rowkfzs["SkillName"].ToString());
                            nodekfzs.Tag = rowkfzs["SkillID"];
                            //nodekfzs.ImageIndex = 1000;
                            int nMaxLevel = Convert.ToInt32(rowkfzs["MaxLevel"]);
                            for (int i = 0; i < nMaxLevel; i ++)
                            {
                                TreeNode nodekflevel = nodekfzs.Nodes.Add("Level " + Convert.ToString(i + 1));
                                nodekflevel.Tag = i;

                                // 在表skill_caster中加入对应的skilllevel记录
                                int iSkillId = Convert.ToInt32(nodekflevel.Parent.Tag);
                                int iSkillLevel = Convert.ToInt32(nodekflevel.Tag);
                                string t_sql = string.Format("select * from tbl_skill_caster where skillid='{0}' and skilllevel='{1}'",
                                    iSkillId, iSkillLevel);
                                SqlDataAdapter t_adpt = new SqlDataAdapter(t_sql, MainForm.conn);
                                System.Data.DataSet t_ds = new System.Data.DataSet();
                                t_adpt.Fill(t_ds);
                                DataTable t_tbl = t_ds.Tables[0];
                                if (t_tbl.Rows.Count == 0)
                                    SqlInsertLevelRecord(iSkillId, iSkillLevel);
                            }
                        }
                    }
                }
            }
            
        }

        private object selectObject(PropertyGridHelper pgHelper, object[] objKeys)
        {
            object objReturn = null;
            CustomClass selectedObj = pgHelper.GetDynamicClass(objKeys);
            if (selectedObj != null)
            {
                object[] events = new object[] {
                    new CustomClass.PropertyChangeEventHandler(BeforePropertyChanged),
                    new CustomClass.PropertyAfterChangeEventHandler(AfterPropertyChanged),
                    new CustomClass.CustomEditValueEventHandler(OnCustomEditorEditValue),
                    new CustomClass.TreeValueEventHandler(OnGetTreeValue),
                };
                selectedObj.SetEvents(events);
                /*
                selectedObj.DataValidate -= new CustomClass.PropertyChangeEventHandler(BeforePropertyChanged);
                selectedObj.AfterDataValidate -= new CustomClass.PropertyAfterChangeEventHandler(AfterPropertyChanged);
                selectedObj.OnEditValue -= new CustomClass.CustomEditValueEventHandler(OnCustomEditorEditValue);
                selectedObj.OnGetTreeValue -= new CustomClass.TreeValueEventHandler(OnGetTreeValue);

                selectedObj.DataValidate += new CustomClass.PropertyChangeEventHandler(BeforePropertyChanged);
                selectedObj.AfterDataValidate += new CustomClass.PropertyAfterChangeEventHandler(AfterPropertyChanged);
                selectedObj.OnEditValue += new CustomClass.CustomEditValueEventHandler(OnCustomEditorEditValue);
                selectedObj.OnGetTreeValue += new CustomClass.TreeValueEventHandler(OnGetTreeValue);
                */
                objReturn = selectedObj;
            }
            return objReturn;
        }
        private void applyRestriction() // 添加编辑器对于表项之间的限制
        {
            try
            {
                if (m_lua != null)
                {
                    m_lua.DoString("_ChgEnv(\"#\")\r\n");
                    LuaFunction fun = m_lua.GetFunction("__fname__#.apply_restrict");
                    if (fun != null)
                    {
                        object[] retobjs = fun.Call();
                    }
                }
            }
            catch (Exception ex)
            {
                ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                frm.OutputBox.Text += ex.Message + "\r\n";
                frm.Show();
                frm.BringToFront();
            }
        }
        private void skillTree_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            object selectobj = null, selectobj2 = null;
            if (e.Button == MouseButtons.Right && e.Node != null)
            {
                skillTree.SelectedNode = e.Node;
                switch (skillTree.SelectedNode.Level)
                {
                    case 0: cmTreeSchool.Popup(Control.MousePosition); break;
                    case 1: cmTreeKungfutl.Popup(Control.MousePosition); break;
                    case 2: cmTreeZhaoShi.Popup(Control.MousePosition); break;
                    case 3: cRename.Popup(Control.MousePosition); break;
                    default: break;
                }
            }
            else if (e.Button == MouseButtons.Left && e.Node != null && e.Node.Level == 0 && e.Node.Text == "Default") //第0层：门派
            {
                selectobj = selectObject(m_pghelper, new object[] { e.Node.Tag });
            }
            else if (e.Button == MouseButtons.Left && e.Node != null && e.Node.Level == 2) //第2层的套路(也是技能)也要显示
            {
                selectobj = selectObject(m_pghelper, new object[] { e.Node.Tag });

                applyRestriction();
            }
            else if (e.Button == MouseButtons.Left && e.Node != null && e.Node.Level == 3) //第3层是技能
            {
                if (m_lastSelectedSkillID != e.Node.Tag)
                {
                    m_lastSelectedSkillID = e.Node.Tag;
                    // 把 LevelID 也选上(在tabpage2)
                    selectobj2 = selectObject(m_pghelper2, new object[] { e.Node.Tag, 0 });
                }
                else
                    selectobj2 = propertyGrid2.SelectedObject;

                //将上一个结点的数据存进数据库
                //todo...

                //读取指定的结点数据
                selectobj = selectObject(m_pghelper, new object[] { e.Node.Tag });

                applyRestriction();
            }
            else if (e.Button == MouseButtons.Left && e.Node != null && e.Node.Level == 4) // 第4层是SkillLevel
            {
                m_lastSelectedSkillID = e.Node.Parent.Tag;

                // 自动 update 技能逻辑与界面(即tabPage1)所选的SkillID
                selectobj = selectObject(m_pghelper, new object[] { e.Node.Parent.Tag });

                // 读取指定的 SkillLevel 结点数据
                selectobj2 = selectObject(m_pghelper2, new object[] { e.Node.Parent.Tag, e.Node.Tag });
            }
            else
                m_lastSelectedSkillID = null;

            //刷新属性框显示
            propertyGrid1.SelectedObject = selectobj;
            propertyGrid1.Refresh();
            propertyGrid2.SelectedObject = selectobj2;
            propertyGrid2.Refresh();
        }
        /*
        private void BeforePropertyChanged(object sender, PropertyChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                CustomProperty prop = (CustomProperty)sender;
                if (prop.Name == "Freight")
                {
                    double fnew = Convert.ToDouble(e.newvalue);
                    double fold = Convert.ToDouble(e.oldvalue);
                    if (fnew < fold)
                    {
                        throw new Exception("无效的修改，新值必须比旧值大。");
                    }
                }
            }
        }
        */
        /*
        private void AfterPropertyChanged(object sender, PropertyAfterChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                CustomProperty prop = (CustomProperty)sender;
                if (prop.ID.ToLower() == "belongkungfu")
                {
                    string svalue = e.value.ToString();
                    CustomClass dynobj = (CustomClass)propertyGrid1.SelectedObject;

                    DataRow dr = m_Kungfutl_Table.Rows.Find(e.id);
                    //门派
                    CustomProperty propschool = dynobj["belongschool"];
                    propschool.Value = propschool.ComboKeys[dr["BelongSchool"].ToString()];
                    //类型
                    CustomProperty proptype = dynobj["type"];
                    proptype.Value = proptype.ComboKeys[dr["Type"].ToString()];
                    //dynobj["type"].Value = newtypevalue;

                    if (false)
                    {
                        throw new Exception("无效的修改。");
                    }
                    //是修改了所属武功套路，需要刷新
                    GridItem item = propertyGrid1.SelectedGridItem;
                    CustomPropertyDescriptor desc = (CustomPropertyDescriptor)item.PropertyDescriptor;
                    CustomProperty specprop = desc.Property;
                    LoadTree();
                }
            }
        }
        */
        private void NewType_button_Click(object sender, EventArgs e)
        {
            TreeNode node = skillTree.SelectedNode;
          step0:
            string NewName = "新建武功类型" + m_KungfuCatNewCount.ToString();
            if (m_KungfuCat_Table.Rows.Find(NewName) != null)
            {
                m_KungfuCatNewCount++;
                goto step0;
            }
            System.Data.DataRow dr;
            dr = m_KungfuCat_Table.NewRow();
            dr.BeginEdit();
            dr["id"] = m_KungfuCat_Table.Rows.Count;
            dr["name"] = NewName;
            m_KungfuCat_Table.Rows.Add(dr);
            if(Updatedata("select * from dic_kungfu_cat", m_KungfuCat_Table,UpdateType.ADD))
                addNode(node, NewName, (int)dr["id"]);
            m_KungfuCatNewCount++;
        }

        private void NewSchool_button_Click(object sender, EventArgs e)
        {
            step0:
            string NewName = "新建门派" + m_SchoolNewCount.ToString();
            if(m_School_Table.Rows.Find(NewName)!=null)
            {
                m_SchoolNewCount++;
                goto step0;
            }
            TreeNode nodeschool = this.skillTree.Nodes.Add(NewName);
            nodeschool.Tag = m_School_Table.Rows.Count;
            nodeschool.Name = NewName;
            System.Data.DataRow dr;
            dr = m_School_Table.NewRow();
            dr.BeginEdit();
            dr["SchoolID"] = m_School_Table.Rows.Count;
            dr["IconID"] = 0;
            dr["Name"] = nodeschool.Name;
            m_School_Table.Rows.Add(dr);
            Updatedata("select * from dic_school", m_School_Table,UpdateType.ADD);
            m_SchoolNewCount++;
            LoadDataBase();
            // 添加类型子结点
            foreach (System.Data.DataRow row in m_KungfuCat_Table.Rows) // 武功类别
            {
                TreeNode nodekfcat = nodeschool.Nodes.Add(row["name"].ToString());
                nodekfcat.Tag = row[0];
            }
        }
        private TreeNode addNode(TreeNode node,string Nodename,int tag)
        {
            TreeNode Nextnode = node.Nodes.Add(Nodename);
            Nextnode.Name = Nodename;
            Nextnode.Tag = tag;
            return Nextnode;
        }
        private bool Updatedata(string sql,DataTable table,UpdateType type)
        {
            try
            {
                SqlDataAdapter adapter = new SqlDataAdapter(sql, MainForm.conn);
                //使用SqlCommandBuilder  对像填充SqlDataAdapter 的InsertCommand、DeleteCommand、UpdateCommand对像
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adapter);
                switch(type)
                {
                    case UpdateType.ADD: adapter.InsertCommand = cmdBuilder.GetInsertCommand();break;
                    case UpdateType.MOD: adapter.UpdateCommand = cmdBuilder.GetUpdateCommand(); break;
                    case UpdateType.DEL: adapter.DeleteCommand = cmdBuilder.GetDeleteCommand(); break;
                }
                
                int val = adapter.Update(table);
                table.AcceptChanges();
                return true;
    
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return false;
            }
        }

        private void ReName_button_Click(object sender, EventArgs e)
        {
           //  node = skillTree.GetNodeAt(MousePosition);
            TreeNode node = skillTree.SelectedNode;
            skillTree.LabelEdit = true;
            if(!node.IsEditing)
            {
                node.BeginEdit();
            }
        }
        DataTable m_Table;
        private void skillTree_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            TreeNode node = skillTree.SelectedNode;
            skillTree.LabelEdit = false;
            string SQL = null;
            string strName = null;
            System.Data.DataRow dr=null;
            switch (node.Level)
            {
                case 0: SQL = "select * from dic_school";
                        m_Table = m_School_Table;
                        strName = "Name";
                       // dr = m_Table.Rows[Convert.ToInt32(node.Tag)];
                        dr = m_Table.Rows.Find(node.Text);
                      
                        if (m_Table.Rows.Find(e.Label) != null || e.Label == null) 
                             goto exit0;
                        break;
                case 1: SQL = "select * from dic_kungfu_cat";
                        m_Table = m_KungfuCat_Table;
                        strName = "name";
                        //dr = m_Table.Rows[Convert.ToInt32(node.Tag)];
                        dr = m_Table.Rows.Find(node.Text);
                        if(m_Table.Rows.Find(e.Label) != null || e.Label == null ) 
                            goto exit0;
                        break;
               case 2: SQL = "select * from tbl_skills where BelongKungfu = '0'";
                        m_Table = m_Kungfutl_Table;
                        strName = "SkillName";
                        dr = m_Table.Rows.Find(Convert.ToInt32(node.Tag));
                        break;
                case 3: SQL = "select * from tbl_skills where BelongKungfu <> '0'";
                         m_Table = m_Kungfuzs_Table;
                        strName = "SkillName";
                        dr = m_Table.Rows.Find(Convert.ToInt32(node.Tag));
                        break;
           }
          
            node.Name = e.Label;
            dr.BeginEdit();
            dr[strName] = node.Name;
            dr.EndEdit();

            Updatedata(SQL, m_Table, UpdateType.MOD);
            return;
        exit0:
           
            e.CancelEdit = true;
            if (e.Label != null)
                MessageBox.Show(e.Label + "已存在！请重命名");
            skillTree.SelectedNode = node;
                  
        }

        private void NewZhaoShi_button_Click(object sender, EventArgs e)
        {
            TreeNode node = skillTree.SelectedNode;
            int SkillID = StartID + m_Kungfuzs_Table.Rows.Count;
            
            while (m_Kungfuzs_Table.Rows.Find(SkillID) != null)
            {
                SkillID++;
            }
            string NewName = "新建功夫招式" + SkillID.ToString();
            System.Data.DataRow dr;
            dr = m_Kungfuzs_Table.NewRow();
            dr.BeginEdit();
            dr["SkillID"] = SkillID;
            dr["SkillName"] = NewName;
            dr["Type"] = Convert.ToInt32(node.Parent.Tag);
            dr["BelongKungfu"] = Convert.ToInt32(node.Tag);
            dr["BelongSchool"] = Convert.ToInt32(node.Parent.Parent.Tag);
            dr["MaxLevel"] = 1;
            m_Kungfuzs_Table.Rows.Add(dr);

            if (Updatedata("select * from tbl_skills where BelongKungfu <> '0'", m_Kungfuzs_Table, UpdateType.ADD))
            {
                TreeNode nodeNewZS = addNode(node, NewName, SkillID);
                addNode(nodeNewZS, "Level 1", 0);
            }
            m_KFZSCount++;
            SqlInsertLevelRecord(SkillID, 0);

            LoadDataBase();
            m_pghelper.ReinitData();
            m_pghelper2.ReinitData();
        }

        private void NewKungfutl_button_Click(object sender, EventArgs e)
        {
            TreeNode node = skillTree.SelectedNode;
            int SkillID = StartID + m_Kungfutl_Table.Rows.Count;
            while (m_Kungfutl_Table.Rows.Find(SkillID) != null)
            {
                SkillID++;
            }
            string NewName = "新建功夫套路" + SkillID.ToString();//m_KFTLCount.ToString();
            
            System.Data.DataRow dr;
            dr = m_Kungfutl_Table.NewRow();
            dr.BeginEdit();
            dr["SkillID"] = SkillID;
            dr["SkillName"] = NewName;
            dr["Type"] = Convert.ToInt32(node.Tag);
            dr["BelongKungfu"] = 0;
            dr["BelongSchool"] = Convert.ToInt32(node.Parent.Tag);
            m_Kungfutl_Table.Rows.Add(dr);

            if(Updatedata("select * from tbl_skills where BelongKungfu = '0'", m_Kungfutl_Table, UpdateType.ADD))
                addNode(node, NewName, SkillID);
            m_KFTLCount++;

            LoadDataBase();
            m_pghelper.ReinitData();
            //m_pghelper2.ReinitData();
        }

        private void SkillForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_lua != null)
                m_lua.Dispose();
        }

        private object OnGetTreeValue(CustomClass sender, TreeValueEventArgs e)
        {
            CustomClass prop = sender;

            string svalue = e.tree.Description;
            try
            {
                if (m_lua != null && prop.ID != null)
                {
                    DoScript(prop.ID);
                    LuaFunction fun = m_lua.GetFunction("__fname__" + prop.ID + ".ongetreevalue");
                    if (fun != null)
                    {
                        object[] retobjs = fun.Call(prop, e);
                        if (retobjs.GetLength(0) > 0)
                        {
                            if (retobjs[0] is bool)
                            {
                                if ((bool)retobjs[0] == true && retobjs.GetLength(0) > 1)
                                    return retobjs[1];
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                frm.OutputBox.Text += ex.Message + "\r\n";
                frm.Show();
                frm.BringToFront();
            }
            return svalue;//取消修改值，或者未更改值
        }

        private object OnCustomEditorEditValue(CustomProperty sender, CustomEditorEventArgs e)
        {
            CustomProperty prop = sender;

            string svalue = e.value.ToString();
            try
            {
                if (m_lua != null)
                {
                    DoScript(prop.ID);
                    LuaFunction fun = m_lua.GetFunction("__fname__" + prop.ID + ".oneditvalue");
                    if (fun != null)
                    {
                        object[] retobjs = fun.Call(prop, e);
                        if (retobjs.GetLength(0) > 0)
                        {
                            if (retobjs[0] is bool)
                            {
                                if ((bool)retobjs[0] == true && retobjs.GetLength(0) > 1)
                                    return retobjs[1];
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                frm.OutputBox.Text += ex.Message + "\r\n";
                frm.Show();
                frm.BringToFront();
            }
            return e.value;//取消修改值，或者未更改值
        }

        private void BeforePropertyChanged(CustomProperty sender, PropertyChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                CustomProperty prop = (CustomProperty)sender;
                bool bRet = false;
                string sErrMsg = "未定义的错误";
                try
                {
                    if (m_lua != null)
                    {
                        DoScript(prop.ID);//可能会有重名的字段，因不同表字段现在可以在同一TAB页中，需要加上parentfield.
                        LuaFunction fun = m_lua.GetFunction("__fname__" + prop.ID + ".onbeforevaluechang");
                        if (fun != null)
                        {
                            object[] retobjs = fun.Call(prop, e);
                            if (retobjs.GetLength(0) > 0)
                            {
                                if (retobjs[0] is bool)
                                {
                                    bRet = (bool)retobjs[0];
                                    if (!bRet)
                                        sErrMsg = (string)retobjs[1];
                                }
                            }
                        }
                        else
                            bRet = true; //无函数表示没有脚本指定
                    }
                }
                catch (Exception ex)
                {
                    ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                    frm.OutputBox.Text += ex.Message + "\r\n";
                    frm.Show();
                    frm.BringToFront();
                }
                if (!bRet)
                {
                    throw new Exception(sErrMsg);
                }
            }
        }
        private void AfterPropertyChanged(CustomProperty sender, PropertyAfterChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                CustomProperty prop = (CustomProperty)sender;

                string svalue = e.value.ToString();
                try
                {
                    if (m_lua != null)
                    {
                        DoScript(prop.ID);
                        LuaFunction fun = m_lua.GetFunction("__fname__" + prop.ID + ".onaftervaluechanged");
                        if (fun != null)
                            fun.Call(prop, e);
                    }
                }
                catch (Exception ex)
                {
                    ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                    frm.OutputBox.Text += ex.Message + "\r\n";
                    frm.Show();
                    frm.BringToFront();
                }
                //GridItem item = propertyGrid1.SelectedGridItem;
                //CustomPropertyDescriptor desc = (CustomPropertyDescriptor)item.PropertyDescriptor;
                //CustomProperty specprop = desc.Property;                
            }
        }
        private string GetPrefixCode(string strFieldName)
        {
            string strcodeprefix = "";
            //切换到私有环境
            if (m_lua != null)
            {
                if (strFieldName == "#")
                    strcodeprefix = "_ChgEnv()\r\n"; //全局环境
                else
                    strcodeprefix = string.Format("_ChgEnv(\"{0}\")\r\n", strFieldName);
            }
            return strcodeprefix;
        }
        private void DoScript(string strFieldName)
        {
            try
            {
                string strcodeprefix = GetPrefixCode(strFieldName);
                m_lua.DoString(strcodeprefix);
                //m_lua.GetFunction("_ChgEnv").Call(strFieldName == "#" ? "" : strFieldName);

                object fn = m_scriptExecState[strFieldName];
                if (fn != null)
                {
                    bool bExecuted = (bool)fn;
                    if (bExecuted)
                        return;
                }
                else
                {
                    //return;//没有脚本内容，放弃执行
                    m_scriptExecState[strFieldName] = false;
                }
                if (m_pghelper.ScriptCode[strFieldName] == null)
                    return;
                string strcode = m_pghelper.ScriptCode[strFieldName].ToString();
                if (m_lua != null && strcode != null && strcode.Length > 0)
                {
                    m_lua.DoString(strcodeprefix + strcode);
                    m_scriptExecState[strFieldName] = true;
                }
            }
            catch (Exception ex)
            {
                ScriptDebugForm frm = ((MainForm)ParentForm).DebugForm;
                frm.OutputBox.Text += ex.Message + "\r\n";
                frm.Show();
                frm.BringToFront();
            }
        }

        private void propertyGrid1_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
        {

        }

        private void cmdShowCode_Click(object sender, EventArgs e)
        {
            GridItem item = propertyGrid1.SelectedGridItem;
            CustomPropertyDescriptor desc = item==null?null:(CustomPropertyDescriptor)item.PropertyDescriptor;
            string strfnID;
            if (desc == null)
                strfnID = "#";
            else
                strfnID = desc.Property.ID;

            object ocode = m_pghelper.ScriptCode[strfnID];
            string strcode;
            if (ocode == null)
                strcode = "";
            else
                strcode = ocode.ToString();

            ScriptCodeForm frm = ((MainForm)ParentForm).CodeForm;
            frm.CurCode = strcode; // frm.OutputBox.Text = strcode;
            //frm.LoadHisCode(4, "tbl_skills", strfnID);
            frm.ShowDialog();
            if (frm.DialogResult == DialogResult.OK)
            {
                m_scriptExecState[strfnID] = false;
                bool bSameCode = true;
                if (!(m_pghelper.ScriptCode[strfnID] == null && frm.OutputBox.Text.Length == 0)
                    && (m_pghelper.ScriptCode[strfnID] == null
                        || m_pghelper.ScriptCode[strfnID].ToString() != frm.OutputBox.Text)
                    )
                {
                    bSameCode = false;
                    m_pghelper.ScriptCode[strfnID] = frm.OutputBox.Text;
                }
                DoScript(strfnID);
                //上传至数据库
                if (!bSameCode)
                    m_pghelper.SaveScript(strfnID);
            }
        }

        private void cmdEditFile_Click(object sender, EventArgs e)
        {
            try
            {
                GridItem item = propertyGrid1.SelectedGridItem;
                CustomPropertyDescriptor desc = (CustomPropertyDescriptor)item.PropertyDescriptor;
                if (desc != null && desc.Property.ValueType == enumValueType.vtPath)
                {
                    // get file full-path
                    StringBuilder sb = new StringBuilder(255);
                    FileFolderHelper.ReadIniFile("Browsing Base Folder", "Script", "", sb, 255, "./GameDesingerTools.ini");
                    string strFileFullName = sb.ToString();
                    strFileFullName += "\\" + desc.Property.Value.ToString();
                    strFileFullName = strFileFullName.Replace("/", "\\");

                    // show file content to form
                    ScriptCodeForm frm = ((MainForm)ParentForm).CodeForm;
                    frm.CurCode = FileFolderHelper.FileToString(strFileFullName);//frm.OutputBox.Text = FileFolderHelper.FileToString(strFileFullName);
                    frm.ShowDialog();
                    if (frm.DialogResult == DialogResult.OK)
                        FileFolderHelper.StringToFile(frm.OutputBox.Text, strFileFullName);
                }
                else if (desc != null && desc.Property.ValueType == enumValueType.vtFileName)
                {
                    string strPrePath = desc.Property.Prefix;
                    string filename = Helper.CombinePaths(strPrePath, desc.Property.Value.ToString());
                    ScriptCodeForm frm = ((MainForm)ParentForm).CodeForm;
                    frm.CurCode = FileFolderHelper.FileToString(filename);//frm.OutputBox.Text = FileFolderHelper.FileToString(filename);
                    frm.ShowDialog();
                    if (frm.DialogResult == DialogResult.OK)
                        FileFolderHelper.StringToFile(frm.OutputBox.Text, filename);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void cmLoadTreeScript_Click(object sender, EventArgs e)
        {
            string strFileFullName = "./LoadTree.lua";
            ScriptCodeForm frm = ((MainForm)ParentForm).CodeForm;
            frm.CurCode = FileFolderHelper.FileToString(strFileFullName);//frm.OutputBox.Text = FileFolderHelper.FileToString(strFileFullName);
            frm.ShowDialog();
            if (frm.DialogResult == DialogResult.OK)
                FileFolderHelper.StringToFile(frm.OutputBox.Text, strFileFullName);
        }

        private void DeleteZS(int nZSid)
        {
            System.Data.DataRow row = m_Kungfuzs_Table.Rows.Find(new object[] { nZSid });

            // 删除tbl_skill_caster中的level行
            int nMaxLevel = Convert.ToInt32(row["MaxLevel"]);
            int nSkillID = Convert.ToInt32(row["SkillID"]);
            for (int i = 0; i < nMaxLevel; i++)
            {
                SqlRemoveLevelRecord(nSkillID, i);
            }
            // 删除tbl_skills中的技能行
            row.Delete();
            if (Updatedata("select * from tbl_skills where BelongKungfu <> '0'", m_Kungfuzs_Table, UpdateType.DEL))
                m_KFZSCount--;
        }

        // 删除招式
        private void buttonDelZS_Click(object sender, EventArgs e)
        {
            int nZSid = Convert.ToInt32(skillTree.SelectedNode.Tag);
            DeleteZS(nZSid);
            LoadDataBase();
            // 更新tree
            skillTree.SelectedNode.Remove();
        }

        private void DeleteTL(int nTLid)
        {
            int[] arr_zsid = new int[1024];
            int arr_zsid_count = 0;
            foreach (System.Data.DataRow row in m_Kungfuzs_Table.Rows)
            {
                int belongKungfu = Convert.ToInt32(row["BelongKungfu"]);
                int zsid = Convert.ToInt32(row["SkillID"]);
                if (belongKungfu == nTLid)
                {
                    arr_zsid[arr_zsid_count++] = zsid;
                    //DeleteZS(zsid);
                }
            }
            for (int i = 0; i < arr_zsid_count; i++)
            {
                DeleteZS(arr_zsid[i]);
            }

            // 删除tbl_skills中选中的套路
            System.Data.DataRow rowTL = m_Kungfutl_Table.Rows.Find(new object[] { nTLid });
            rowTL.Delete();
            if (Updatedata("select * from tbl_skills where BelongKungfu = '0'", m_Kungfutl_Table, UpdateType.DEL))
                m_KFTLCount--;
        }

        private void buttonDeleteTL_Click(object sender, EventArgs e)
        {
            int nTLid = Convert.ToInt32(skillTree.SelectedNode.Tag);
            DeleteTL(nTLid);
            LoadDataBase();
            // 更新tree
            skillTree.SelectedNode.Remove();
        }

        private void DeleteCat(int nCatID)
        {
            int[] arr_tlid = new int[1024];
            int arr_tlid_count = 0;
            foreach (System.Data.DataRow row in m_Kungfutl_Table.Rows)
            {
                int iCat = Convert.ToInt32(row["Type"]);
                int iTLid = Convert.ToInt32(row["SkillID"]);
                if (iCat == nCatID)
                {
                    arr_tlid[arr_tlid_count++] = iTLid;
                }
            }
            for (int i = 0; i < arr_tlid_count; i++)
            {
                DeleteTL(arr_tlid[i]);
            }
            // 删除
            string CatName = "";
            foreach (System.Data.DataRow row in m_KungfuCat_Table.Rows)
            {
                int catid = Convert.ToInt32(row["id"]);
                string catname = row["name"].ToString();
                if (catid == nCatID)
                {
                    CatName = catname;
                    break;
                }
            }
            System.Data.DataRow rowCat = m_KungfuCat_Table.Rows.Find(new object[] { CatName });
            rowCat.Delete();
            if (Updatedata("select * from dic_kungfu_cat", m_KungfuCat_Table, UpdateType.DEL))
                m_KungfuCatNewCount--;
        }

        private void buttonDelCat_Click(object sender, EventArgs e)
        {
            int nCatID = Convert.ToInt32(skillTree.SelectedNode.Tag);
            DeleteCat(nCatID);
            LoadDataBase();
            // 更新tree
            skillTree.SelectedNode.Remove();
        }

        private void buttonDelCatChild_Click(object sender, EventArgs e)
        {
            int nCatID = Convert.ToInt32(skillTree.SelectedNode.Tag);
            int[] arr_tlid = new int[1024];
            int arr_tlid_count = 0;
            foreach (System.Data.DataRow row in m_Kungfutl_Table.Rows)
            {
                int iCat = Convert.ToInt32(row["Type"]);
                int iTLid = Convert.ToInt32(row["SkillID"]);
                if (iCat == nCatID)
                {
                    arr_tlid[arr_tlid_count++] = iTLid;
                }
            }
            for (int i = 0; i < arr_tlid_count; i++)
            {
                DeleteTL(arr_tlid[i]);
            }
            LoadDataBase();
            // 更新tree
            foreach (TreeNode node in skillTree.SelectedNode.Nodes)
            {
                node.Remove();
            } 
        }

        private void buttonDelSchool_Click(object sender, EventArgs e)
        {
            int[] arr_tlid = new int[1024];
            int arr_tlid_count = 0;
            int nSchoolID = Convert.ToInt32(skillTree.SelectedNode.Tag);
            foreach (System.Data.DataRow row in m_Kungfutl_Table.Rows)
            {
                int nTLid = Convert.ToInt32(row["SkillID"]);
                int nBelongSchoolID = Convert.ToInt32(row["BelongSchool"]);
                if (nBelongSchoolID == nSchoolID)
                {
                    arr_tlid[arr_tlid_count++] = nTLid;
                }
            }
            for (int i = 0; i < arr_tlid_count; i++)
            {
                DeleteTL(arr_tlid[i]);
            }
            // 
            string SchoolName = "";
            foreach (System.Data.DataRow row in m_School_Table.Rows)
            {
                int SchoolID = Convert.ToInt32(row["SchoolID"]);
                string r_schoolName = row["Name"].ToString();
                if (SchoolID == nSchoolID)
                {
                    SchoolName = r_schoolName;
                    break;
                }
            }
            System.Data.DataRow rowSchool = m_School_Table.Rows.Find(new object[] { SchoolName });
            rowSchool.Delete();
            if (Updatedata("select * from dic_school", m_School_Table, UpdateType.DEL))
                m_SchoolNewCount--;
            LoadDataBase();
            // 更新tree
            skillTree.SelectedNode.Remove();
        }

        private void buttonItem6_Click(object sender, EventArgs e)
        {
            //跳转到元信息表中的项目
            ((MainForm)ParentForm).ShowMetainfoEditor();
            //获取字段名
            GridItem item = propertyGrid1.SelectedGridItem;
            CustomPropertyDescriptor desc = (CustomPropertyDescriptor)item.PropertyDescriptor;
            string strfnID;
            if (desc == null)
                return;
            else
                strfnID = desc.Property.ID;

            //((MainForm)ParentForm).DBForm.GotoField("tbl_skills", strfnID);
        }
    }
    /*struct kfdata
    {
        public int nID;
        public CustomClass PropertyContainer;
    };*/
}