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
using DevComponents.DotNetBar;
using System.Runtime.InteropServices;
using GameDesingerTools;

namespace GameDesingerTools
{
    public partial class ModelEdit : Form
    {
        [DllImport("user32.dll")]//, CharSet = CharSet.Unicode)]
        public static extern IntPtr PostMessage(IntPtr hwnd, int wMsg, IntPtr wParam, string lParam);
        enum UpdateType { ADD, MOD, DEL };
        TreeNode topNode;
        TreeNode CopiedNode;
        public System.Data.SqlClient.SqlConnection m_conn;
        DataTable m_ModelDefTable = new DataTable();
        DataTable m_ModelTabDefTable = new DataTable();
        DataTable m_DicMetaInfoTable = new DataTable();
        private string[] EditorTypeEn = { "text", "pathname", "filename", "bool", "lookupcombo", "textcombo", "customeditor" };
        private string[] EditorTypeCn = { "文本","文件路径浏览","文件名浏览","真假值","下拉框", "文本下拉框", "自定义编辑" };
        private int m_nMaxTabID = 0;
        private int m_nMaxModelID = 0;
        private TimeLog log = new TimeLog();
        private bool selectionChanged = false; // 主表名是否已经改变
        private bool lockSelection = false; // 是否锁定选择
        private bool lockCheckChange = false; // 是否锁定检查内容被修改过
        private Hashtable modelTable = new Hashtable(); // 记录模块表名对的哈希表
        private ContentChanged m_ContentChanged = ContentChanged.none;
        private enum ContentChanged { none, model, tab, field };
        private bool canCfgMeta = false; // 当选择一个树结点时，会刷新这个值。能否修改表元的依据。

        const int WM_RESET_SELECTED_NODE = 12345 + 1000;

        public ModelEdit()
        {
            InitializeComponent();

            BindingCheckChange();

            //this.treeView.ImageList = ((MainForm)Program.MainForm).imageList2; //使用复制，不再需要用大图标
            LoadIcons();

            //ConnectDataBase();
            m_conn = MainForm.conn;            
            LoadModelDefTable();
            LoadModelTabDefTable();
            LoadDicMetaInfoTable();
            LoadTree();
            
            #if DEBUG
                this.groupDebug.Visible = true;
            #endif

            log.Log(TimeLog.enumLogType.ltstart, "表元编辑器", "工具模块启动", null);
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case WM_RESET_SELECTED_NODE:
                    treeView.SelectedNode = m_Node;
                    break;
            }
            base.WndProc(ref　m);
        }

        /// <summary>
        /// 将检查是否改变的方法绑定到控件事件上
        /// </summary>
        private void BindingCheckChange()
        {
            foreach(Control c in this.Controls)
            {
                BindingCheckChange(c);
            }
        }        

        /// <summary>
        /// 将检查是否改变的方法绑定到控件事件上
        /// </summary>
        private void BindingCheckChange(Control control)
        {
            if(control is DevComponents.DotNetBar.Controls.TextBoxX)
            {
                (control as DevComponents.DotNetBar.Controls.TextBoxX).TextChanged += new EventHandler(CheckChange);
            }
            else if(control is DevComponents.DotNetBar.Controls.CheckBoxX)
            {
                (control as DevComponents.DotNetBar.Controls.CheckBoxX).CheckedChanged += new EventHandler(CheckChange);
            }
            else if (control is DevComponents.DotNetBar.Controls.ComboBoxEx)
            {
                (control as DevComponents.DotNetBar.Controls.ComboBoxEx).TextChanged += new EventHandler(CheckChange);
            }
            else if(control is TextBox)
            {
                (control as TextBox).TextChanged += new EventHandler(CheckChange);
            }
            else if(control is ButtonX)
            {
                if (control != UpdateMod_button)
                {
                    (control as ButtonX).Click += new EventHandler(CheckChange);
                }
            }
            foreach(Control c in control.Controls)
            {
                BindingCheckChange(c);
            }
        }

        //将主窗口中的图标复制过来，但缩小为 16 x 16
        private void LoadIcons()
        {
            imglstIcons.Images.Clear();
            foreach (Image img in ((MainForm)Program.MainForm).imageList2.Images)
            {
                imglstIcons.Images.Add(img);
            }
        }

        private void ModelEdit_Load(object sender, EventArgs e)
        {
            /* remoting
            string strsql = "SELECT name FROM sysobjects WHERE (xtype = 'u') order by name";
            SqlDataAdapter adp = new SqlDataAdapter(strsql, m_conn);
            
            System.Data.DataSet DS2 = new System.Data.DataSet();
            adp.Fill(DS2, "sysobjects");*/

            DataTable table = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name", m_conn);
            List<string> list = new List<string>();
            foreach (DataRow row in table.Rows)
            {
                list.Add(row["name"].ToString());
            }
            

            this.listtable_comboBox.DataSource = list;
            this.listtable_comboBox.DisplayMember = "sysobjects.name";
            if (this.listtable_comboBox.Items.Count > 0)
                this.listtable_comboBox.SelectedIndex = 0;

            TextCombo_groupBox.Left = LookUpCombo_groupBox.Left;
            TextCombo_groupBox.Top = LookUpCombo_groupBox.Top;
            FileName_groupBox.Left = LookUpCombo_groupBox.Left;
            FileName_groupBox.Top = LookUpCombo_groupBox.Top;

            FillEditorType();
        }

        /* remoting
        private bool LoadTable(string sql, ref DataTable table)
        {
            try
            {
                SqlDataAdapter adp = new SqlDataAdapter(sql, m_conn);
                DataSet ds = new DataSet();
                adp.Fill(ds);
                table = ds.Tables[0];
                return true;
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
                return false;
            }
        }
        */
        public bool LoadModelDefTable()
        {
            bool bOK = true; 
            try
            {
                m_ModelDefTable = Helper.GetDataTableProxy("sys_modl_def", m_conn);
                //bOK = LoadTable("SELECT * FROM sys_modl_def", ref m_ModelDefTable);
            }
            catch
            {
                bOK = false;
            }
            if (bOK)
            {

                DataColumn[] primaryKey = new DataColumn[1];
                m_ModelDefTable.Columns["id"].ReadOnly = false;
                primaryKey[0] = m_ModelDefTable.Columns["modelname_en"];
                m_ModelDefTable.PrimaryKey = primaryKey;
            }
            return bOK;
        }

        public bool LoadModelTabDefTable()
        {
            bool bOK = true;
            m_ModelTabDefTable = Helper.GetDataTableProxy("sys_modl_tab_def", m_conn);
            if (bOK)
            {
                DataColumn[] primaryKey = new DataColumn[1];
                primaryKey[0] = m_ModelTabDefTable.Columns["name"];
                m_ModelTabDefTable.PrimaryKey = primaryKey;
            }
            return bOK;
        }

        public bool LoadDicMetaInfoTable()
        {
            //m_DicMetaInfoTable = Helper.GetDataTable("SELECT * FROM sys_meta_info", m_conn);
            m_DicMetaInfoTable = Helper.GetDataTableProxy("sys_meta_info", m_conn);
            return true;
            //bool bOK = LoadTable("SELECT * FROM sys_meta_info", ref m_DicMetaInfoTable);
            //if (bOK)
            //{
            //    DataColumn[] primaryKey = new DataColumn[1];
            //    primaryKey[0] = m_DicMetaInfoTable.Columns["fieldid"];
            //    m_DicMetaInfoTable.PrimaryKey = primaryKey;
            //}
            //return bOK;
        }

        /* remoting
        public System.Data.DataTable QueryData(string sql)
        {
            DataTable table = new DataTable();
            bool bRetCode = LoadTable(sql, ref table);
            if (bRetCode)
                return table;
            else
                return null;
        }*/


         public bool CheckFieldExist(string tablename, string fieldname)
         {
            if (tablename.Trim() != "")
            {
                string sql;
                if (tablename.Contains("."))
                {
                    string[] strArr = tablename.Split(new char[] { '.' });
                    string svrName = strArr[0];
                    string dbName = strArr[1];
                    string _tblname = strArr[3];
                    sql = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}')", svrName, _tblname, dbName);
                }
                else
                {
                    sql = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", tablename);
                }
                //string sql = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", tablename);
                //DataTable table = Helper.GetDataTable(sql, m_conn);
                DataTable table = Helper.GetDataTableWithSqlProxy("syscolumns", sql, m_conn);
                foreach (DataRow row in table.Rows)
                {
                    if (row["name"].ToString().Trim() == fieldname)
                        return true;
                }
                //for (int i = 0; i < table.Columns.Count; ++i)
                //{
                //    if (table.Columns[i].ColumnName == fieldname)
                //        return true;
                //}
            }
            return false;
         }

        public TreeNode AddTreeNode(TreeNode node, string text, object tag, string edittype)
        {
            TreeNode newNode = null;
            if (node != null)
            {
                newNode = new TreeNode();
                newNode.Text = text; // +" [" + tag.ToString().Trim() + "]";
                newNode.Tag = tag;
                node.Nodes.Add(newNode);

                TreeNodeSetImage(newNode, edittype);
            }

            return newNode;
        }

        private void TreeNodeSetImage(TreeNode node, string editType)
        {
            switch (editType)
            {
                case "virtualfield": node.ImageIndex = 7; node.SelectedImageIndex = 7; break;
                case "tab": node.ImageIndex = 6; node.SelectedImageIndex = 6; break; 
                case "text": node.ImageIndex = 2; node.SelectedImageIndex = 2; break;
                case "pathname": node.ImageIndex = 2; node.SelectedImageIndex = 2; break;
                case "filename": node.ImageIndex = 2; node.SelectedImageIndex = 2; break;
                case "bool": node.ImageIndex = 1; node.SelectedImageIndex = 1; break;
                case "lookupcombo": node.ImageIndex = 5; node.SelectedImageIndex = 5; break;
                case "textcombo": node.ImageIndex = 5; node.SelectedImageIndex = 5; break;
                case "customeditor": node.ImageIndex = 2; node.SelectedImageIndex = 2; break;
                default:
                    break;
            }
        }

        private void SetConfigIcon(TreeNode node, DataRow r)
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
            imglstIcons.Images.Add(image);
            node.ImageIndex = imglstIcons.Images.Count - 1;
            node.SelectedImageIndex = imglstIcons.Images.Count - 1;
        }

        private void LoadTree()
        {
            treeView.Nodes.Clear();
            // add top node
            string topNodeName = "模块编辑";
            topNode = new TreeNode(topNodeName);
            topNode.Tag = 0;
            treeView.Nodes.Add(topNode);
            
            for (int i = 0; i < m_ModelDefTable.Rows.Count; ++i)
            {
                DataRow  row         = m_ModelDefTable.Rows[i];
                int      modelid     = (int)row["id"];
                string modelName = row["modelname_en"].ToString().Trim();
                string   modelNameCn = row["modelname_cn"].ToString().Trim();
                int      nIconid     = (int)row["iconid"];
                TreeNode modelNode   = new TreeNode(modelNameCn);

                modelNode.Text = modelName;
                modelNode.Tag = modelid;
                topNode.Nodes.Add(modelNode);
                if(nIconid == -1) // 自定义图标
                {
                    SetConfigIcon(modelNode, row);
                }
                else
                {
                    modelNode.ImageIndex = nIconid;
                    modelNode.SelectedImageIndex = nIconid;
                }
                m_nMaxModelID = m_nMaxModelID > modelid ? m_nMaxModelID : modelid;

                foreach (DataRow tabrow in m_ModelTabDefTable.Select("modid = " + modelid))
                {
                    {
                        int nModtabid = (int)tabrow["modtabid"];
                        int nModid = (int)tabrow["modid"];
                        string tabname = tabrow["name"].ToString().Trim();
                        string tablename = tabrow["tablename"].ToString().Trim();
                        string catfields = tabrow["catfields"].ToString().Trim();
                        string displayfield = tabrow["display_field"].ToString().Trim();

                        TreeNode tabNode = AddTreeNode(modelNode, tabname, tablename, "tab");
                        if (tabNode == null)
                            goto Exit0;

                        m_nMaxTabID = m_nMaxTabID > nModtabid ? m_nMaxTabID : nModtabid;

                        // 显示标签下的属性
                        foreach (DataRow r in m_DicMetaInfoTable.Select("modtabid = " + nModtabid + " AND (parentfield is null OR parentfield = 0)", "orderno"))
                        {                         
                            TreeNode attribNode = null;
                            string attriName = r["fieldcnname"].ToString().Trim();
                            string attrifieldname = r["fieldname"].ToString().Trim();
                            if (r["fieldtype"].ToString() == "" || (int)r["fieldtype"] != 1)
                                attribNode = AddTreeNode(tabNode, attriName + " [" + attrifieldname + "]", r["fieldid"], r["editortype"].ToString().Trim());
                            else
                                attribNode = AddTreeNode(tabNode, attriName + " [" + attrifieldname + "]", r["fieldid"], "virtualfield");

                            if (attribNode == null)
                                goto Exit0;

                            LoadSubNodes(attribNode, (int)r["fieldid"]);                       
                        }
                    }
                }
            }
            m_nMaxModelID++;
            m_nMaxTabID++;

            if (treeView.Nodes != null && treeView.Nodes.Count > 0)
                treeView.Nodes[0].Expand();

        Exit0:
            return;
        }

        private bool LoadSubNodes(TreeNode parentNode, int nParentField)
        {
            bool bRetCode = false;
            try
            {                
                foreach (DataRow row in m_DicMetaInfoTable.Select("parentfield = " + nParentField, "orderno"))
                {
                    string strFieldName = row["fieldname"].ToString().Trim();
                    string strFieldCNName = row["fieldcnname"].ToString().Trim();
                    int nFieldID = (int)row["fieldid"];
                    string strEditorType = row["editortype"].ToString().Trim();
                    if (row["fieldtype"].ToString().Trim() == "1")
                        strEditorType = "virtualfield";
                    TreeNode newNode = AddTreeNode(parentNode, strFieldCNName + " [" + strFieldName + "]", nFieldID, strEditorType);
                    if (newNode == null)
                        return false;
                    bRetCode = LoadSubNodes(newNode, (int)newNode.Tag);
                    if (!bRetCode)
                        return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return false;
            }
        }

        private bool DeleteSubNodes(TreeNode parentNode)
        {
            bool bRetCode = false;
            try
            {
                foreach (TreeNode node in parentNode.Nodes)
                {
                    bRetCode = DeleteSubNodes(node);
                    if (!bRetCode)
                        return false;
                    DataRow row = m_DicMetaInfoTable.Rows.Find(new object[]{(int)node.Tag});
                    row.Delete();
                    UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);
                }
                parentNode.Nodes.Clear();
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return false;
            }
        }

        private bool UpdateData(string sql, DataTable table, UpdateType type)
        {
            try
            {
                /*
                SqlDataAdapter adapter = new SqlDataAdapter(sql, m_conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adapter);
             
                switch (type)
                {
                    case UpdateType.ADD: adapter.InsertCommand = cmdBuilder.GetInsertCommand(); break;
                    case UpdateType.MOD: adapter.UpdateCommand = cmdBuilder.GetUpdateCommand(); break;
                    case UpdateType.DEL: adapter.DeleteCommand = cmdBuilder.GetDeleteCommand(); break;
                }

                int val = adapter.Update(table);
                table.AcceptChanges();*/

                Helper.SaveTable(table, sql, m_conn);

                return true;

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return false;
            }
        }

        /// <summary>
        /// 填充主表选择的内容
        /// </summary>
        private void FillMainTableComboBox()
        {
            //string sql = "SELECT name FROM sysobjects WHERE order by name asc";
            //DataTable tbl = Helper.GetDataTable(sql, m_conn);
            DataTable tbl = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name asc", m_conn);
            comboBoxEx1.Items.Clear();
            foreach (DataRow row in tbl.Rows)
            {
                string str = row["name"].ToString().Trim();
                if (str != "")
                {
                    this.comboBoxEx1.Items.AddRange(new object[] { str });
                }
            }
        }

        private void FillDisplayFieldComboBox(string tablename)
        {
            if (tablename == null || tablename.Trim() == "")
                return;
            
            //string sqltxt = string.Format("SELECT name FROM syscolumns WHERE id=object_id(\'{0}\') order by name asc", tablename);
            string sqltxt;
            if (tablename.Contains("."))
            {
                string[] strArr = tablename.Split(new char[] { '.' });
                string svrName = strArr[0];
                string dbName = strArr[1];
                string _tblname = strArr[3];
                sqltxt = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}') order by name asc", svrName, _tblname, dbName);
            }
            else
            {
                sqltxt = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\') order by name asc", tablename);
            }

            //DataTable tbl = Helper.GetDataTable(sqltxt, m_conn);
            DataTable tbl = Helper.GetDataTableWithSqlProxy("syscolumns", sqltxt, m_conn);
            DisplayField_comboBox.Items.Clear();
            foreach(DataRow row in tbl.Rows)
            {
                string strColumnName = row["name"].ToString().Trim();
                DisplayField_comboBox.Items.AddRange(new object[] { strColumnName });
            }
        }
        
        private void FillCatName()
        {
            if (MainTable_text.Tag == null)
                return;

            DataRow row = m_DicMetaInfoTable.Rows.Find(new object[] { MainTable_text.Tag });
            if (row != null)
            {
                try
                {
                    string sql = "SELECT catname FROM sys_meta_info WHERE tablename = '" + row["tablename"].ToString().Trim() + "' group by catname";
                    /* remoting
                    SqlDataAdapter adp = new SqlDataAdapter(sql, m_conn);
                    DataSet ds = new DataSet();
                    adp.Fill(ds);
                    DataTable table = ds.Tables[0];
                     */
                    DataTable table = Helper.GetDataTableWithSqlProxy("sys_meta_info", sql, m_conn);
                    catname_comboBox.Items.Clear();
                    foreach (DataRow rowindex in table.Rows)
                    {
                        if (rowindex["catname"].ToString().Trim() == "")
                            continue;
                        catname_comboBox.Items.Add(rowindex["catname"].ToString().Trim());
                    }
                }
                catch (Exception exp)
                {
                    MessageBox.Show(exp.Message);
                }
            }
        }

        private void FillEditorType()
        {
            EditorType_comboBox.Items.Clear();
            for (int i = 0; i < EditorTypeCn.Length; ++i)
            {
                EditorType_comboBox.Items.Add(EditorTypeCn[i]);
            }
        }
      
        private void treeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            treeView.SelectedNode = e.Node;
            if (e.Button == MouseButtons.Right && e.Node != null)
            {
                switch (treeView.SelectedNode.Level)
                {
                    case 0: Model.Popup(Control.MousePosition); break;
                    case 1: ModButton.Popup(Control.MousePosition); break;
                    case 2: TabButton.Popup(Control.MousePosition); break;
                    default: CreateVirtualField_buttom.Popup(Control.MousePosition); break;
                }
            }
        }

        /// <summary>
        /// 保存当前内容
        /// </summary>
        /// <param name="selectNode">选中的结点</param>
        private void FillTabContain(TreeNode selectNode)
        {
            if(selectNode == null)
            {
                return;
            }

            switch (selectNode.Level)
            {
                case 0: // 模块编辑
                    this.tabControlPanel1.Enabled = false;
                    this.tabControlPanel2.Enabled = false;
                    this.tabControlPanel3.Enabled = false;
                    break;
                case 1: // 选择模块
                    this.tabControl1.SelectedTabIndex = 0;
                    this.tabControlPanel1.Enabled = true;
                    this.tabControlPanel2.Enabled = false;
                    this.tabControlPanel3.Enabled = false;
                    break;
                case 2: // 选择分页
                    this.tabControl1.SelectedTabIndex = 1;
                    this.tabControlPanel1.Enabled = false;
                    this.tabControlPanel2.Enabled = false;
                    this.tabControlPanel3.Enabled = true;
                    break;
                case 3:  // 选择属性
                    this.tabControl1.SelectedTabIndex = 2;
                    this.tabControlPanel1.Enabled = false;
                    this.tabControlPanel2.Enabled = true;
                    this.tabControlPanel3.Enabled = false;
                    break;
                default: // 选择属性
                    this.tabControl1.SelectedTabIndex = 2;
                    this.tabControlPanel1.Enabled = false;
                    this.tabControlPanel2.Enabled = true;
                    this.tabControlPanel3.Enabled = false;
                    break;
            }

            lockCheckChange = true; // 锁定检查是否修改内容
            switch(selectNode.Level)
            {
                case 1:
                {
                    try
                    {
                        TreeNode childNode = selectNode.FirstNode;  
                        if(!selectionChanged)
                        {
                            FillMainTableComboBox();
                        }
                                         
                        // 刷新'编辑模块'标签页内容
                        DataRow row = null;
                        if (childNode != null)
                        {
                            row = m_ModelTabDefTable.Rows.Find(childNode.Text);
                        }
                        else
                        {
                            row = row = m_ModelTabDefTable.Rows.Find(selectNode.Tag.ToString().Trim());
                        }
                        
                        if (row != null && tabControlPanel1.Enabled)
                        {
                            this.labelDebug.Text = "Mod ID";
                            this.textDebug.Text = selectNode.Tag.ToString().Trim();
                            if(!selectionChanged)
                            {
                                lockSelection = true;
                                this.comboBoxEx1.Text = childNode.Tag.ToString().Trim();
                                lockSelection = false;
                                this.comboBoxEx1.Tag = childNode.Tag.ToString().Trim();
                            }
                            selectionChanged = false;
                            
                            FillDisplayFieldComboBox(childNode.Tag.ToString().Trim());
                            this.DisplayField_comboBox.Tag = row["display_field"].ToString().Trim();
                            this.DisplayField_comboBox.Text = row["display_field"].ToString().Trim();
                            this.FieldNames.Items.Clear();
                            this.SelectedNames.Items.Clear();

                            string tablename = childNode.Tag.ToString().Trim();
                            string catfields = row["catfields"].ToString().Trim();
                            string[] fieldnames = catfields.Split(new char[] { '\r', '\n', ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);

                            foreach (string name in fieldnames)
                            {
                                this.SelectedNames.Items.Add(name);
                            }

                            string sqltxt;
                            if (tablename.Contains("."))
                            {
                                string[] strArr = tablename.Split(new char[] { '.' });
                                string svrName = strArr[0];
                                string dbName = strArr[1];
                                string _tblname = strArr[3];
                                sqltxt = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}') order by name asc", svrName, _tblname, dbName);
                            }
                            else
                            {
                                sqltxt = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\') order by name asc", tablename);
                            }

                            //DataTable ColumnsTable = Helper.GetDataTable(sqltxt, m_conn);
                            DataTable ColumnsTable = Helper.GetDataTableWithSqlProxy("syscolumns", sqltxt, m_conn);
                            foreach (DataRow colomnRow in ColumnsTable.Rows)
                            {
                                string colomnName = colomnRow[0].ToString().Trim();
                                bool bSelected = false;
                                foreach (string selectedfield in fieldnames)
                                {
                                    if (colomnName.ToLower() == selectedfield.ToLower())
                                    {
                                        bSelected = true;
                                        break;
                                    }
                                }
                                if (!bSelected)
                                    this.FieldNames.Items.Add(colomnName);
                            }               

                            this.SelectedNames.Tag = row["catfields"].ToString().Trim();
                        }
                        else
                        {
                            this.TabName_text.Tag = null;
                            this.TabName_text.Text = null;
                            if (modelTable[selectNode] != null)
                            {
                                lockSelection = true;
                                this.comboBoxEx1.Text = modelTable[selectNode].ToString().Trim();
                                lockSelection = false;
                            }
                            
                            FillDisplayFieldComboBox(this.comboBoxEx1.Text);
                            DisplayField_comboBox.Text = "";
                            this.FieldNames.Items.Clear();
                            foreach(object o in DisplayField_comboBox.Items)
                            {
                                this.FieldNames.Items.Add(o);
                            }              
                            this.SelectedNames.Items.Clear();
                        }
                    }
                    catch (Exception exp)
                    {
                        MessageBox.Show(exp.Message);
                    }
                    break;
                }
                case 2: //编辑标签页
                {
                    DataRow row = m_ModelTabDefTable.Rows.Find(selectNode.Text);
                    this.TabName_text.Tag = selectNode.Text;
                    this.TabName_text.Text = selectNode.Text;
                    this.labelDebug.Text = "Mod Tab ID:";
                    if (row != null)
                    {
                        this.textDebug.Text = row["modtabid"].ToString().Trim();
                    }
                    this.MainTable_comboBox.Text = selectNode.Tag.ToString();
                    this.MainTable_comboBox.Tag = selectNode.Tag.ToString();
                    //检查自定义标签页
                    this.checkboxCustomTab.Checked = false;
                    if (row["custom_tab"] != DBNull.Value)
                    {
                        this.checkboxCustomTab.Checked = (row["custom_tab"].ToString().Trim() == "1");
                    }
                    break;
                }
                default: // level >= 3
                {
                    // 刷新'编辑字段'标签页内容
                    DataRow row = m_DicMetaInfoTable.Rows.Find(new object[] { (int)selectNode.Tag });
                    if (row != null && tabControlPanel2.Enabled)
                    {
                        this.labelDebug.Text = "Field ID:";
                        this.textDebug.Text = row["fieldid"].ToString();
                        this.MainTable_text.Tag = selectNode.Tag;   // 一条记录在表元里对应的fieldid         
                        this.MainTable_text.Text = row["tablename"].ToString().Trim();
                        this.FieldName_text.Text = row["fieldname"].ToString().Trim();
                        this.FieldCNName_Text.Text = row["fieldcnname"].ToString().Trim();
                        this.catname_comboBox.Text = row["catname"].ToString().Trim();
                        this.descriptiontxt.Text = row["description"].ToString().Trim();
                        string strFieldType = row["fieldtype"].ToString().Trim();
                        this.ReadOnly_checkBoxX.Checked = row["readonly"].ToString().ToLower() == "true";
                        this.CHKHide.Checked = row["visible"].ToString().ToLower() == "false";
                        if (strFieldType == "1")
                        {
                            Filters_groupBox.Enabled = true;
                            Filters_groupBox.Visible = true;
                            sortType_Combox.Enabled = true;
                            sortType_Combox.Visible = true;

                            this.FieldName_text.Enabled = true;

                            string strFilters = row["subtablefilter"].ToString().Trim();
                            string[] strValues = strFilters.Split(new char[] { ';', '\r', '\n', ' ' }, StringSplitOptions.RemoveEmptyEntries);
                            FilterListBox.Items.Clear();
                            for (int i = 0; i < strValues.Length; ++i)
                            {
                                FilterListBox.Items.Add(strValues[i]);
                            }

                            string strTableName = "";
                            if (treeView.SelectedNode.Level == 3)
                            {
                                int nModTabID = (int)row["modtabid"];
                                DataRow[] modRows = m_ModelTabDefTable.Select("modtabid = " + nModTabID);
                                if (modRows.Length == 0)
                                    return;
                                strTableName = modRows[0]["tablename"].ToString().Trim();
                            }
                            else
                            {
                                TreeNode parentNode = treeView.SelectedNode.Parent;
                                int nFieldID = (int)parentNode.Tag;
                                DataRow[] parentRows = m_DicMetaInfoTable.Select("fieldid = " + nFieldID);
                                if (parentRows.Length == 0)
                                    return;
                                strTableName = parentRows[0]["tablename"].ToString().Trim();
                            }


                            string strSQL;
                            if (strTableName.Contains("."))
                            {
                                string[] strArr = strTableName.Split(new char[] { '.' });
                                string svrName = strArr[0];
                                string dbName = strArr[1];
                                string _tblname = strArr[3];
                                strSQL = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}')", svrName, _tblname, dbName);
                            }
                            else
                            {
                                strSQL = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", strTableName);
                            }

                            string[] names = new string[] { "syscolumns", "sysobjects" };
                            //DataTable mainTableFields = Helper.GetDataTable(strSQL, m_conn);
                            DataTable mainTableFields = Helper.GetDataTableWithSqlProxy(names, strSQL, m_conn);
                            if (mainTableFields == null)
                                return;
                            MainTableFieldsComboBox.Items.Clear();
                            foreach (DataRow mainfield in mainTableFields.Rows)
                            {
                                MainTableFieldsComboBox.Items.Add(mainfield["name"].ToString().Trim());
                            }

                            //strSQL = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", this.MainTable_text.Text);
                            if (this.MainTable_text.Text.Contains("."))
                            {
                                string[] strArr = this.MainTable_text.Text.Split(new char[] { '.' });
                                string svrName = strArr[0];
                                string dbName = strArr[1];
                                string _tblname = strArr[3];
                                strSQL = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}')", svrName, _tblname, dbName);
                            }
                            else
                            {
                                strSQL = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", this.MainTable_text.Text);
                            }
                            //DataTable subTableFields = Helper.GetDataTable(strSQL, m_conn);
                            DataTable subTableFields = Helper.GetDataTableWithSqlProxy(names, strSQL, m_conn);
                            if (subTableFields == null)
                                return;
                            SubTableFieldsComboBox.Items.Clear();
                            foreach (DataRow subField in subTableFields.Rows)
                            {
                                SubTableFieldsComboBox.Items.Add(subField["name"].ToString().Trim());
                            }
                        }
                        else
                        {
                            Filters_groupBox.Enabled = false;
                            Filters_groupBox.Visible = false;
                            sortType_Combox.Enabled = false;
                            sortType_Combox.Visible = false;
                            FieldName_text.Enabled = true;
                        }
                        lSortType.Visible = sortType_Combox.Visible;
                        FillCatName();

                        int nEditorTypeID = 0;
                        string strEditorType = row["editortype"].ToString().Trim();

                        for (nEditorTypeID = 0; nEditorTypeID < EditorTypeEn.Length; nEditorTypeID++)
                        {
                            if (string.Compare(strEditorType, EditorTypeEn[nEditorTypeID]) == 0)
                                break;

                        }

                        if (nEditorTypeID >= EditorTypeEn.Length)
                            nEditorTypeID = 0;
                        this.EditorType_comboBox.SelectedIndex = nEditorTypeID;

                        int nSortType = row["sorttype"] == DBNull.Value ? 0 : Convert.ToInt32(row["sorttype"].ToString());
                        this.sortType_Combox.SelectedIndex = nSortType;

                        switch (strEditorType)
                        {
                            case "filename":
                                txtBoxRelativePath.Text = row["relativepath"].ToString().Trim();
                                txtBoxCutRelativePath.Text = row["cutrelativepath"].ToString().Trim();
                                chkCutSlash.Checked = (row["cutpreslash"] == null || row["cutpreslash"] == DBNull.Value) ? true : Convert.ToBoolean(row["cutpreslash"]);
                                LookUpCombo_groupBox.Enabled = false;
                                LookUpCombo_groupBox.Visible = false;
                                TextCombo_groupBox.Enabled = false;
                                TextCombo_groupBox.Visible = false;
                                FileName_groupBox.Enabled = true;
                                FileName_groupBox.Visible = true;
                                break;
                            case "lookupcombo":
                                listtable_comboBox.Text = row["listtable"].ToString().Trim();
                                RefreshKeyfiedAndListfield();
                                keyfield_comboBox.Text = row["keyfield"].ToString().Trim();
                                listfield_comboBox.Text = row["listfield"].ToString().Trim();
                                listconditiontxt.Text = row["listcondition"].ToString().Trim();
                                listexfields.Text = row["listexfields"].ToString().Trim();
                                LookUpCombo_groupBox.Enabled = true;
                                LookUpCombo_groupBox.Visible = true;
                                TextCombo_groupBox.Enabled = false;
                                TextCombo_groupBox.Visible = false;
                                FileName_groupBox.Enabled = false;
                                FileName_groupBox.Visible = false;

                                break;
                            case "textcombo":
                                txtBoxListValues.Text = row["listvalues"].ToString().Trim();
                                LookUpCombo_groupBox.Enabled = false;
                                LookUpCombo_groupBox.Visible = false;
                                TextCombo_groupBox.Enabled = true;
                                TextCombo_groupBox.Visible = true;
                                FileName_groupBox.Enabled = false;
                                FileName_groupBox.Visible = false;
                                break;
                            default:
                                LookUpCombo_groupBox.Enabled = false;
                                LookUpCombo_groupBox.Visible = false;
                                TextCombo_groupBox.Enabled = false;
                                TextCombo_groupBox.Visible = false;
                                FileName_groupBox.Enabled = false;
                                FileName_groupBox.Visible = false;
                                break;
                        }
                    }
                    break;
                }
            }
            m_ContentChanged = ContentChanged.none;
            lockCheckChange = false;
        }

        /// <summary>
        /// 新建模块
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void CreatMod_Click(object sender, EventArgs e)
        {
            // 获得主表名
            DBTools.ChooseForm cf = new DBTools.ChooseForm(m_conn);
            DialogResult result = cf.ShowDialog();
            if(result != DialogResult.OK) // 用户未确认输入
            {
                return;
            }
            string tableName = cf.TableName;

            // 新建模块和主表
            if (treeView.SelectedNode != null)
            {
                TreeNode selectNode = null;
                try
                {
                    DataRow newRow = m_ModelDefTable.NewRow();
                    newRow.BeginEdit();
                    newRow["modelname_en"] = "新模块_" + m_nMaxModelID;
                    newRow["modelname_cn"] = "新模块_" + m_nMaxModelID;
                    newRow["iconid"] = 1;
                    newRow.EndEdit();
                    m_ModelDefTable.Rows.Add(newRow);

                    UpdateData("SELECT * FROM sys_modl_def", m_ModelDefTable, UpdateType.ADD);

                    string strUserID = Helper.GetHostName();
                    string strIP = Helper.GetIPAddress();
                    string strSQL = string.Format("select _index from sys_useracl where userid ='{0}' and ip='{1}'", strUserID, strIP);
                    //DataTable dtab = Helper.GetDataTable(strSQL, m_conn);
                    DataTable dtab = Helper.GetDataTableWithSqlProxy("sys_useracl", strSQL, m_conn);

                    string userindex = dtab.Rows[0][0].ToString();

                    string strGroupName = string.Empty;
                    if (String.Compare(strUserID.ToUpper(), "A") > 0 && String.Compare(strUserID.ToUpper(), "J") < 0)
                    {
                        strGroupName = "A - J";
                    }
                    if (String.Compare(strUserID.ToUpper(), "K") > 0 && String.Compare(strUserID.ToUpper(), "S") < 0)
                    {
                        strGroupName = "K - S";
                    }
                    if (String.Compare(strUserID.ToUpper(), "T") > 0 && String.Compare(strUserID.ToUpper(), "X") < 0)
                    {
                        strGroupName = "T - X";
                    }
                    if (String.Compare(strUserID.ToUpper(), "Y") > 0 && String.Compare(strUserID.ToUpper(), "Z") < 0)
                    {
                        strGroupName = "Y - Z";
                    }

                    //DataTable newDataTable = Helper.GetDataTable("SELECT * FROM sys_useracl_desc", m_conn);
                    DataTable newDataTable = Helper.GetDataTableProxy("sys_useracl_desc", m_conn);

                    DataRow newDataRow = newDataTable.NewRow();
                    newDataRow.BeginEdit();
                    newDataRow["userindex"] = userindex;
                    newDataRow["moduleid"] = m_nMaxModelID;
                    newDataRow["groupname"] = strGroupName;
                    newDataRow["modulename_en"] = "新模块_" + m_nMaxModelID;
                    newDataRow["canread"] = 1;
                    newDataRow["canwrite"] = 1;
                    newDataRow["canimport"] = 1;
                    newDataRow["canconfigmeta"] = 1;
                    newDataRow["canmodifydefaultval"] = 1;
                    newDataRow.EndEdit();
                    newDataTable.Rows.Add(newDataRow);
                    UpdateData("SELECT * FROM sys_useracl_desc", newDataTable, UpdateType.ADD);
                    canCfgMeta = true;

                    TreeNode newNode = new TreeNode("新模块_" + m_nMaxModelID);
                    selectNode = newNode;
                    //
                    // 以下代码要做修正
                    //
                    //DataTable table = Helper.GetDataTable("SELECT max(id) as modelid FROM sys_modl_def", m_conn);
                    DataTable table = Helper.GetDataTableWithSqlProxy("sys_modl_def", "SELECT max(id) as modelid FROM sys_modl_def", m_conn);
                    newNode.Tag = (int)table.Rows[0]["modelid"];
                    newRow["id"] = (int)table.Rows[0]["modelid"];
                    m_ModelDefTable.AcceptChanges();
                    treeView.SelectedNode.Nodes.Add(newNode);
                    m_nMaxModelID++;
                    lockSelection = true;
                    comboBoxEx1.Text = tableName;
                    lockSelection = false;
                    comboBoxEx1.Tag = tableName;
                    treeView.SelectedNode = newNode;
                    modelTable[newNode] = tableName;
                    FillTabContain(newNode);

                    TimeLog modaddlog = new TimeLog();
                    modaddlog.Log(TimeLog.enumLogType.ltstart, "表元编辑器", "添加模块定义", tableName);
                    modaddlog.Log(TimeLog.enumLogType.ltend, null, null, null);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }     
            }
        }

        /// <summary>
        /// 删除功能模块
        /// </summary>
        /// <param name="node">要删除的树结点</param>
        private void DeletaMod(TreeNode node)
        {
            bool bRetCode = false;
            bool bResult = true;

            if (node != null)
            {
                TreeNode modelNode = node;
                DataRow row = m_ModelDefTable.Rows.Find(modelNode.Text);
                if (row != null)
                {                   
                    // 删除模块节点
                    row.Delete();
                    bRetCode = UpdateData("SELECT * FROM sys_modl_def", m_ModelDefTable, UpdateType.DEL);
                    if (!bRetCode)
                        goto Exit0;
                }
                else
                {
                    MessageBox.Show("要删除的模块不存在!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2);
                }
            }

            Exit0:
            if (!bResult)
            {
                MessageBox.Show("在删除某个节点时，发生错误!");
            }
        }

        /// <summary>
        /// 删除功能模块
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void DeleteMod_Click(object sender, EventArgs e)
        {
            bool bRetCode = false;
            bool bResult = true;

            if (treeView.SelectedNode != null)
            {
                TreeNode modelNode  = treeView.SelectedNode;
                DataRow row         = m_ModelDefTable.Rows.Find(modelNode.Text);
                if (row != null)
                {
                    DialogResult result = MessageBox.Show("确定要删除这个模块，以及这个模块下的所有内容吗?", "提示", MessageBoxButtons.YesNo, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2);
                    if (result != DialogResult.Yes)
                        goto Exit1;

                    foreach (TreeNode tabNode in modelNode.Nodes)
                    {
                        // 删除标签页下的所有叶子节点
                        if (tabNode.Nodes.Count > 0)
                        {
                            bRetCode = DeleteSubNodes(tabNode);
                            if (!bRetCode)
                                goto Exit0;
                            bRetCode = UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);
                            if (!bRetCode)
                                goto Exit0;
                        }

                        DataRow tabRow = m_ModelTabDefTable.Rows.Find(tabNode.Text.Trim());
                        if (tabRow == null)
                            continue;

                        // 删除标签页节点
                        tabRow.Delete();
                    }

                    bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.DEL);
                    if (!bRetCode)
                        goto Exit0;

                    // 删除模块节点
                    row.Delete();
                    bRetCode = UpdateData("SELECT * FROM sys_modl_def", m_ModelDefTable, UpdateType.DEL);
                    if (!bRetCode)
                        goto Exit0;
                    treeView.SelectedNode.Remove();

                    TimeLog moddellog = new TimeLog();
                    moddellog.Log(TimeLog.enumLogType.ltstart, "表元编辑器", "删除模块定义", modelNode.Text);
                    moddellog.Log(TimeLog.enumLogType.ltend, null, null, null);
                }
                else
                {
                    MessageBox.Show("要删除的模块不存在!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2);
                }
            }
        Exit1:
                bResult = true;
        Exit0:
            if (!bResult)
            {
                MessageBox.Show("在删除某个节点时，发生错误!");
            }
        }

        private void treeView_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            TreeNode node = treeView.SelectedNode;
            treeView.LabelEdit = false;
            if (e.Label == null)
            {
                node.EndEdit(false);
                return;
            }
            string newName = e.Label.ToString().Trim();
            if (node.Level == 1 && newName.Length > 0)
            {
                DataRow row = m_ModelDefTable.Rows.Find(new object[] { node.Text });
                if (row != null)
                {
                    row.BeginEdit();
                    row["modelname_en"] = newName;
                    row["modelname_cn"] = newName;
                    row.EndEdit();
                    bool bRetCode = UpdateData("SELECT * FROM sys_modl_def", m_ModelDefTable, UpdateType.MOD);
                    if (bRetCode)                
                        node.Name = e.Label;
                }
            }
            node.EndEdit(false);
        }

        /// <summary>
        /// 重命名属性页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void RenameMod_Click(object sender, EventArgs e)
        {
            TreeNode node = treeView.SelectedNode;
            treeView.LabelEdit = true;
            if (!node.IsEditing)
            {
                node.BeginEdit();
            }
        }

        /// <summary>
        /// 新建属性页
        /// </summary>
        /// <param name="tableName">数据表名</param>
        private void CreateTab(string tableName)
        {
            bool bRetCode = false;

            if (treeView.SelectedNode != null)
            {
                TreeNode newNode = new TreeNode();
                newNode.Text = "新标签页" + m_nMaxTabID;

                TreeNode modNode = treeView.SelectedNode;
                DataRow newRow = m_ModelTabDefTable.NewRow();
                string tablename = tableName;
                string catfields = "";
                string displayfield = "";

                newRow.BeginEdit();
                newRow["modid"] = modNode.Tag;
                newRow["name"] = newNode.Text;
                newRow["tablename"] = tablename;
                newRow["catfields"] = catfields;
                newRow["display_field"] = displayfield;
                newRow.EndEdit();

                m_ModelTabDefTable.Rows.Add(newRow);
                bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.ADD);
                if (bRetCode)
                {
                    newNode.ImageIndex = 6;
                    newNode.SelectedImageIndex = 6;
                    //
                    // 以下代码要修正
                    //
                    //DataTable table = Helper.GetDataTable("SELECT max(modtabid) as id FROM sys_modl_tab_def", m_conn);
                    DataTable table = Helper.GetDataTableWithSqlProxy("sys_modl_tab_def", "SELECT max(modtabid) as id FROM sys_modl_tab_def", m_conn);
                    newRow["modtabid"] = (int)table.Rows[0]["id"];
                    newNode.Tag = tableName;
                    m_ModelTabDefTable.AcceptChanges();
                    treeView.SelectedNode.Nodes.Add(newNode);
                    ++m_nMaxTabID;
                }
                if (newNode.Parent.Nodes.Count == 1)
                {
                    FillTabContain(treeView.SelectedNode);
                }

                treeView.SelectedNode = newNode;
            }
        }

        /// <summary>
        /// 新建属性页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void CreatTab_Click(object sender, EventArgs e)
        {
            bool bRetCode = false;

            if (treeView.SelectedNode != null)
            {
                if(comboBoxEx1.Text == "")
                {
                    MessageBox.Show("请先指定模块的主表名!");
                    return;
                }
                TreeNode newNode = new TreeNode();
                newNode.Text = "新标签页" + m_nMaxTabID;

                TreeNode modNode = treeView.SelectedNode;
                DataRow newRow = m_ModelTabDefTable.NewRow();
                string tablename = comboBoxEx1.Text;
                string catfields = "";
                string displayfield = "";

                DataRow[] rows = m_ModelTabDefTable.Select("modid = " + modNode.Tag.ToString());
                if (rows.Length > 0)
                {
                    tablename = rows[0]["tablename"].ToString();
                    catfields = rows[0]["catfields"].ToString();
                    displayfield = rows[0]["display_field"].ToString();
                }
                
                newRow.BeginEdit();
                newRow["modid"] = modNode.Tag;
                newRow["name"] = newNode.Text;
                newRow["tablename"] = tablename;
                newRow["catfields"] = catfields;
                newRow["display_field"] = displayfield;
                newRow.EndEdit();

                m_ModelTabDefTable.Rows.Add(newRow);
                bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.ADD);
                if (bRetCode)
                {
                    newNode.ImageIndex = 6;
                    newNode.SelectedImageIndex = 6;
                    //
                    // 以下代码要修正
                    //
                    //DataTable table = Helper.GetDataTable("SELECT max(modtabid) as id FROM sys_modl_tab_def", m_conn);
                    DataTable table = Helper.GetDataTableWithSqlProxy("sys_modl_tab_def", "SELECT max(modtabid) as id FROM sys_modl_tab_def", m_conn);
                    newRow["modtabid"] = (int)table.Rows[0]["id"];
                    newNode.Tag = tablename;
                    m_ModelTabDefTable.AcceptChanges();
                    treeView.SelectedNode.Nodes.Add(newNode);
                    ++m_nMaxTabID;
                }
                if(newNode.Parent.Nodes.Count == 1)
                {
                    SaveModContain(false);
                }
                treeView.SelectedNode = newNode;
            }
        }

        private void DeleteTab_Click(object sender, EventArgs e)
        {
            bool bRetCode = false;
            try
            {
                if (treeView.SelectedNode == null)
                    return;
        
                DataRow row = m_ModelTabDefTable.Rows.Find(treeView.SelectedNode.Text.Trim());
                if (row != null)
                {
                    DialogResult result = MessageBox.Show("确定要删除这个属性页，以及这个属性页下的所有内容吗?", "提示", MessageBoxButtons.YesNo, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2);
                    if (result != DialogResult.Yes)
                        return;

                    bRetCode = DeleteSubNodes(treeView.SelectedNode);
                    if (!bRetCode)
                        goto Exit0;
                    UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);

                    row.Delete();
                    bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.DEL);
                    if (bRetCode)
                    {
                        treeView.SelectedNode.Remove();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        Exit0:
            return;
        }
        
        /// <summary>
        /// 导出字段信息
        /// </summary>
        /// <param name="fieldNode">字段树节点</param>
        /// <param name="modelName">模块名称</param>
        /// <param name="tabName">分页名称</param>
        /// <param name="infoList">信息链表</param>
        private void ExportFieldInfo(TreeNode fieldNode, string modelName, string tabName, List<Hashtable> infoList)
        {
            DataRow row = m_DicMetaInfoTable.Rows.Find(new object[] { (int)fieldNode.Tag });      
            string tableName = row["tablename"].ToString().Trim();
            string fieldName = row["fieldname"].ToString().Trim();
            string fieldChineseName = row["fieldcnname"].ToString().Trim();
            string categoryName = row["catname"].ToString().Trim();
            string description = row["description"].ToString().Trim().Replace("\r\n", " ");

            Hashtable infoTable = new Hashtable();
            infoTable["TableName"] = tableName;
            infoTable["FieldName"] = fieldName;
            infoTable["FieldChineseName"] = fieldChineseName;
            infoTable["CategoryName"] = categoryName;
            infoTable["Description"] = description;
            infoTable["ModelName"] = modelName;
            infoTable["TabName"] = tabName;

            infoList.Add(infoTable);

            foreach (TreeNode childNode in fieldNode.Nodes)
            {
                ExportFieldInfo(childNode, modelName, tabName, infoList);
            }
        }

        /// <summary>
        /// 删除属性
        /// </summary>
        /// <param name="node">要删除的树结点</param>
        private void DeleteAttribute(TreeNode node)
        {
            bool bRetCode = false;
            bool bResult = false;
            if (node != null)
            {
                bRetCode = DeleteSubNodes(node);
                if (!bRetCode)
                    goto Exit0;

                DataRow row = m_DicMetaInfoTable.Rows.Find(node.Tag.ToString());
                if (row != null)
                {
                    row.Delete();
                    bRetCode = UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);
                    if (!bRetCode)
                        goto Exit0;

                    TreeNode parentNode = node.Parent;
                    if (parentNode.Level != 3 || parentNode.Nodes.Count != 0)
                        return;
                    DataRow attriRow = m_DicMetaInfoTable.Rows.Find(parentNode.Tag);
                    if (attriRow["fieldtype"].ToString().Trim() == "2")
                    {
                        attriRow.BeginEdit();
                        attriRow["fieldtype"] = 0;
                        attriRow.EndEdit();
                        UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.MOD);
                    }
                }
            }

            bResult = true;
            Exit0:
            if (!bResult)
            {
                MessageBox.Show("删除操作失败!");
            }
        }

        /// <summary>
        /// 删除属性
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void DeleteAttrib_Click(object sender, EventArgs e)
        {
            bool bRetCode = false;
            bool bResult = false;
            if (treeView.SelectedNode != null)
            {
                DialogResult result = MessageBox.Show("是否确认删除该字段", "删除提示", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
                if(result != DialogResult.Yes) // 用户未选择确定
                {
                    return;
                }

                bRetCode = DeleteSubNodes(treeView.SelectedNode);
                if (!bRetCode)
                    goto Exit0;

                DataRow row = m_DicMetaInfoTable.Rows.Find(treeView.SelectedNode.Tag.ToString());
                if (row != null)   
                {
                    row.Delete();
                    bRetCode = UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);
                    if (!bRetCode)
                        goto Exit0;

                    TreeNode parentNode = treeView.SelectedNode.Parent;
                    treeView.SelectedNode.Remove();
                    if (parentNode.Level != 3 || parentNode.Nodes.Count != 0)
                        return;
                    DataRow attriRow = m_DicMetaInfoTable.Rows.Find(parentNode.Tag);
                    if (attriRow["fieldtype"].ToString().Trim() == "2")
                    {
                        attriRow.BeginEdit();
                        attriRow["fieldtype"] = 0;
                        attriRow.EndEdit();
                        UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.MOD);
                    }
                }
            }

            bResult = true;
            Exit0:
            if (!bResult)
            {
                MessageBox.Show("删除操作失败!");
            }
        }

        private void CopyAttrib_Click(object sender, EventArgs e) // 复制
        {
            CopiedNode = treeView.SelectedNode;
        }

        private void PasteAttrib_Click(object sender, EventArgs e) // 粘贴
        {
            TreeNode toPaste = treeView.SelectedNode;

            // 只拷贝下面的子结点, 本身不拷贝
            foreach (TreeNode node in CopiedNode.Nodes)
            {
                CopyPaste(node, toPaste);
            }

            LoadDicMetaInfoTable();
        }
        private void CopyPaste(TreeNode srcNode, TreeNode whereToPaste)
        {
            /* remoting
            
            SqlDataAdapter adp = new SqlDataAdapter(sqltxt, m_conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet DS = new System.Data.DataSet();
            adp.Fill(DS);
            DataTable tbl = DS.Tables[0];
             */
            string sqltxt = "SELECT * FROM sys_meta_info";
            DataTable tbl = Helper.GetDataTableProxy("sys_meta_info", m_conn);

            DataRow desRow = tbl.NewRow();
            DataRow srcRow = tbl.Rows.Find(srcNode.Tag);
            DataRow fatherRow = tbl.Rows.Find(whereToPaste.Tag);
            desRow.BeginEdit();
            foreach (DataColumn col in tbl.Columns)
            {
                if (col.ColumnName != "fieldid")
                {
                    desRow[col.ColumnName] = srcRow[col.ColumnName];
                }
            }
            desRow["parentfield"] = fatherRow["fieldid"];
            desRow.EndEdit();
            tbl.Rows.Add(desRow);

            if (desRow["fieldtype"].ToString().Trim() == "1")
            {
                fatherRow["fieldtype"] = "2";
            }
            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sqltxt, m_conn);
            
            string nodeText = desRow["fieldcnname"].ToString().Trim() + " [" + desRow["fieldname"].ToString().Trim() + "]";

            TreeNode newNode = AddTreeNode(whereToPaste, nodeText, desRow["fieldid"], desRow["editortype"].ToString().Trim());

            foreach (TreeNode node in srcNode.Nodes)
            {
                CopyPaste(node, newNode);
            }
        }

        private void UpdateTab_button_Click(object sender, EventArgs e)
        {
            if (!SaveTabContain(true))
            {
                FillTabContain(treeView.SelectedNode);
//                 m_Node = treeView.SelectedNode;
//                 PostMessage(this.Handle, WM_RESET_SELECTED_NODE, IntPtr.Zero, "");
            }
        }

        /// <summary>
        /// 保存数据内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void UpdateMod_button_Click(object sender, EventArgs e)
        {
            bool bRetCode = false;
            if (treeView.SelectedNode == null)
            {
                return;
            }

            switch (treeView.SelectedNode.Level)
            {
                case 0:
                    {
                        bRetCode = true;
                        break;
                    }
                case 1: // 保存模块信息
                    {
                        if(treeView.SelectedNode.Nodes.Count == 0) // 无效的模块信息
                        {
                            bRetCode = false;
                            break;
                        }
                        if (this.comboBoxEx1.Tag != null && this.comboBoxEx1.Text.Trim() != string.Empty)
                        {
                            bRetCode = SaveModContain(false);
                        }
                        else
                        {
                            bRetCode = true;
                        }
                        break;
                    }
                case 2: // 保存分页信息
                    {
                        bRetCode = SaveTabContain(false);
                        break;
                    }
                default: // 保存属性信息
                    {
                        bRetCode = SaveFieldContain(false);
                        break;
                    }
            }

            m_ContentChanged = ContentChanged.none;
            if (bRetCode == false)
            {
                m_Node = treeView.SelectedNode;
                PostMessage(this.Handle, WM_RESET_SELECTED_NODE, IntPtr.Zero, "");
            }
            else
            {
                MessageBox.Show("数据更新成功!");
            }
        }

        /// <summary>
        /// 保存模块设置信息
        /// </summary>
        /// <param name="bShowSuccessBox">事件发送者</param>
        /// <returns>事件</returns>
        private bool SaveModContain(bool bShowSuccessBox)
        {
            if (Program.m_bUseNewAcl && !this.canCfgMeta)
            {
                MessageBox.Show("您对于当前模块没有配置表元权限！请申请开通。", "权限不足", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            if(treeView.SelectedNode.Nodes.Count == 0) // 未设置属性页
            {
                MessageBox.Show(string.Format("未为模块{0}设置属性页，该模块信息将不能被保存!",
                    treeView.SelectedNode.Text.ToString().Trim()));
                return true;
            }

            bool bRetCode = false;
            bool bChanged = false;

            string oldtablename = string.Empty;
            if (this.comboBoxEx1.Tag != null)
                oldtablename = this.comboBoxEx1.Tag.ToString().Trim();
            string tablename = this.comboBoxEx1.Text.Trim();
            bChanged |= (oldtablename != tablename);

            string olddisplayfield = string.Empty;
            if (this.DisplayField_comboBox.Tag != null)
                olddisplayfield = this.DisplayField_comboBox.Tag.ToString().Trim();
            string displayfield = this.DisplayField_comboBox.Text.Trim();
            bChanged |= (olddisplayfield != displayfield);

            string oldcatfields = string.Empty;
            if (this.SelectedNames.Tag != null)
                oldcatfields = this.SelectedNames.Tag.ToString().Trim();
            string catfields = "";
            for (int i = 0; i < this.SelectedNames.Items.Count; ++i)
            {
                if (catfields != "")
                {
                    catfields += ",";
                }
                catfields += this.SelectedNames.Items[i].ToString().Trim();
            }
            bChanged |= (oldcatfields.ToLower() != catfields.ToLower());

            if (tablename == "")
            {
                MessageBox.Show("请选择一张主表!");
                return false;
            }

            if (bChanged == false)
                return true;

            try
            {
                //int nModid = int.Parse(textDebug.Text);
                int nModid = int.Parse(treeView.SelectedNode.Tag.ToString());
                DataRow[] rows = m_ModelTabDefTable.Select("modid = " + nModid);
                foreach (DataRow dr in rows)
                {
                    dr.BeginEdit();
                    dr["tablename"] = tablename;
                    dr["display_field"] = displayfield;
                    dr["catfields"] = catfields;
                    dr.EndEdit();
                }

                bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.MOD);
                if (bRetCode)
                {
                    foreach (TreeNode node in treeView.SelectedNode.Nodes)
                    {
                        node.Tag = tablename;
                    }

                    if (bShowSuccessBox)
                        MessageBox.Show("更新成功!");
                }
                return bRetCode;
            }
            catch (Exception error)
            {
                MessageBox.Show(error.Message.ToString().Trim());
                return false;
            }
        }

        /// <summary>
        /// 保存分页设置信息
        /// </summary>
        /// <param name="bShowSuccessBox">是否显示是否更新成功</param>
        /// <returns>是否更新成功</returns>
        private bool SaveTabContain(bool bShowSuccessBox)
        {
            if (Program.m_bUseNewAcl && !this.canCfgMeta)
            {
                MessageBox.Show("您对于当前模块没有配置表元权限！请申请开通。", "权限不足", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            bool bRetCode = false;
            bool bChanged = false;

            string oldname = string.Empty;
            if (this.TabName_text.Tag != null)
            {
                oldname = this.TabName_text.Tag.ToString().Trim();
            }
            else
            {
                return true;
            }
            string name = this.TabName_text.Text.Trim();
            bChanged |= (oldname != name);

            string custom_tab = this.checkboxCustomTab.Checked ? "1" : "0";
            string old_custom_tab = string.Empty;
            if (this.checkboxCustomTab.Tag != null)
                old_custom_tab = this.checkboxCustomTab.Tag.ToString().Trim();
            bChanged |= (old_custom_tab != custom_tab);

            if (bChanged == false)
                return true;

            try
            {
                DataRow row;
                string tabOldName = (string)this.TabName_text.Tag;
                if (name != tabOldName)
                {
                    row = m_ModelTabDefTable.Rows.Find(name);
                    if (row != null)
                    {
                        MessageBox.Show("标签名字重名，请另选一个名字!");
                        return false;
                    }
                }

                row = m_ModelTabDefTable.Rows.Find(this.TabName_text.Tag);
                if (row != null)
                {
                    row.BeginEdit();
                    row["name"] = name;
                    row["custom_tab"] = custom_tab;
                    row.EndEdit();
                }
                else
                {
                    MessageBox.Show("在数据库里找不到该标签页对应的记录!");
                    return false;
                }

                bRetCode = UpdateData("SELECT * FROM sys_modl_tab_def", m_ModelTabDefTable, UpdateType.MOD);

                if (bRetCode)
                {
                    treeView.SelectedNode.Text = name;
                    this.TabName_text.Tag = name;
                    this.checkboxCustomTab.Tag = custom_tab;

                    if (bShowSuccessBox)
                        MessageBox.Show("更新成功!");
                }
                return bRetCode;
            }
            catch (Exception error)
            {
                MessageBox.Show(error.Message.ToString().Trim());
                return false;
            }
        }

        private void MainTable_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            string tablename = this.MainTable_comboBox.Text;
            if (tablename != null && tablename != "")
            {
                FillDisplayFieldComboBox(tablename);
            }
        }

        // 标签那层的新建虚拟字段按钮
        private void CreatVirtualField_buttom_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode != null)
            {
                TreeNode tabNode = treeView.SelectedNode;
                string strTableName = tabNode.Tag.ToString().Trim();
                if (strTableName == "")
                {
                    MessageBox.Show("请先为标签指定主表!");
                    return;
                }

                // 标签页那层以下才能建虚拟字段
                if (treeView.SelectedNode.Level != 2)
                    return;

                DataRow row = m_ModelTabDefTable.Rows.Find(tabNode.Text.Trim());
                if (row == null)
                    return;
                
                paraMetaInfo param = new paraMetaInfo();
                param.m_nFieldType = 1;
                param.m_nModTableID = (int)row["modtabid"];
                param.m_DicMetaInfo = m_DicMetaInfoTable;
                AddVirtualField form = new AddVirtualField(null, this, param);
                form.ShowDialog();
            }
        }

        private void CreateSubAttib_buttom_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode != null)
            {
                // 检查所选的叶子节点是否为虚拟字段
                DataRow row = m_DicMetaInfoTable.Rows.Find(new object[]{treeView.SelectedNode.Tag});
                if (row == null)
                    return;

                if ((int)row["fieldtype"] != 1)
                    return;

                // 设置parentid, 
                paraMetaInfo param = new paraMetaInfo();

                param.m_nFieldType = 0;
                param.m_nModTableID = (int)row["modtabid"];
                param.m_nParentField = (int)treeView.SelectedNode.Tag;
                param.m_DicMetaInfo = m_DicMetaInfoTable;
                AddVirtualField form = new AddVirtualField(row["tablename"].ToString().Trim(), this, param);
                DialogResult result = form.ShowDialog();
            
            }
        }

        // 子字段那层的新建虚拟字段按钮
        private void CreateVirtualField2_buttom_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode != null)
            {
                if (treeView.SelectedNode.Level <= 2)
                    return;
                int nFieldID = (int)treeView.SelectedNode.Tag;
                DataRow row = m_DicMetaInfoTable.Rows.Find(new object[] { nFieldID });
                if (row == null)
                    return;

                paraMetaInfo param = new paraMetaInfo();
                param.m_nFieldType = 1;
                param.m_nModTableID = (int)row["modtabid"];
                param.m_nParentField = (int)treeView.SelectedNode.Tag;
                param.m_DicMetaInfo = m_DicMetaInfoTable;
                AddVirtualField form = new AddVirtualField(null, this, param);
                DialogResult result = form.ShowDialog();
                if (result == DialogResult.OK)
                {
                    if (row["fieldtype"] != null && row["fieldtype"].ToString().Trim() != "2")
                    {
                        DataRow modRow = m_DicMetaInfoTable.Rows.Find(new object[] { nFieldID });
                        if (modRow["fieldtype"].ToString().Trim() == "1")
                            return;
                        modRow.BeginEdit();
                        modRow["fieldtype"] = 2;
                        modRow.EndEdit();
                        UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.MOD);
                    }
                }
            }
        }

        private void EditorType_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (EditorTypeEn[EditorType_comboBox.SelectedIndex])
            {
                case "filename":
                    LookUpCombo_groupBox.Enabled = false;
                    LookUpCombo_groupBox.Visible = false;
                    TextCombo_groupBox.Enabled = false;
                    TextCombo_groupBox.Visible = false;
                    FileName_groupBox.Enabled = true;
                    FileName_groupBox.Visible = true;
                    break;
                case "lookupcombo":
                    LookUpCombo_groupBox.Enabled = true;
                    LookUpCombo_groupBox.Visible = true;
                    TextCombo_groupBox.Enabled = false;
                    TextCombo_groupBox.Visible = false;
                    FileName_groupBox.Enabled = false;
                    FileName_groupBox.Visible = false;

                    break;
                case "textcombo":
                    LookUpCombo_groupBox.Enabled = false;
                    LookUpCombo_groupBox.Visible = false;
                    TextCombo_groupBox.Enabled = true;
                    TextCombo_groupBox.Visible = true;
                    FileName_groupBox.Enabled = false;
                    FileName_groupBox.Visible = false;
                    break;
                default:
                    LookUpCombo_groupBox.Enabled = false;
                    LookUpCombo_groupBox.Visible = false;
                    TextCombo_groupBox.Enabled = false;
                    TextCombo_groupBox.Visible = false;
                    FileName_groupBox.Enabled = false;
                    FileName_groupBox.Visible = false;
                    break;
            }
        }

        private void UpdateField_button_MouseClick(object sender, MouseEventArgs e)
        {
            SaveFieldContain(true);
        }

        private bool SaveFieldContain(bool bShowSuccessBox)
        {
            if (Program.m_bUseNewAcl && !this.canCfgMeta)
            {
                MessageBox.Show("您对于当前模块没有配置表元权限！请申请开通。", "权限不足", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            bool bRetCode = false;
            int nFieldID = (int)this.MainTable_text.Tag;
            LoadDicMetaInfoTable();
            DataRow row = m_DicMetaInfoTable.Rows.Find(nFieldID);
            string strFieldName = this.FieldName_text.Text.ToString().Trim();
            string strOldFieldName = row["fieldname"].ToString().Trim();
            string strParentField = row["parentfield"].ToString().Trim();
            string strFieldType = row["fieldtype"].ToString().Trim();
            string strModTabID = row["modtabid"].ToString().Trim();

            if (strFieldName == "")
            {
                MessageBox.Show("请输入字段英文名!");
                return false;
            }

            if (strFieldType == "1")
            {
                // 标签下的虚拟字段
                if (strModTabID != "" && strFieldName != strOldFieldName)
                {
                    string strSQL = string.Format("SELECT * FROM sys_modl_tab_def WHERE modtabid = {0}", strModTabID);
                    string filter = string.Format("modtabid = {0}", strModTabID);
                    //DataTable table = Helper.GetDataTable(strSQL, m_conn);
                    DataTable table = Helper.GetDataTableProxy("sys_modl_tab_def", filter, null, m_conn);
                    string tablename = table.Rows[0]["tablename"].ToString().Trim();
                    bRetCode = CheckFieldExist(tablename, strFieldName);
                    if (bRetCode)
                    {
                        MessageBox.Show("虚拟字段英文名与主表中某个字段同名，请换个名字!");
                        return false; 
                    }

                    strSQL = string.Format(
                        "SELECT * FROM sys_meta_info WHERE parentfield = 0 AND modtabid = {0} AND fieldname = '{1}'",
                        strModTabID, strFieldName);
                    filter = string.Format("parentfield = 0 AND modtabid = {0} AND fieldname = '{1}'", strModTabID, strFieldName);
                    //table = Helper.GetDataTable(strSQL, m_conn);
                    table = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_conn);
                    if (table != null && table.Rows.Count > 0)
                    {
                        MessageBox.Show("虚拟字段英文名与该字段所在同一级某个字段同名，请换个名字!");
                        this.FieldName_text.Text = strOldFieldName;
                        return false;
                    }
                }
                else if (strFieldName != strOldFieldName)// 字段下的虚拟字段
                {
                    TreeNode selectNode = treeView.SelectedNode;
                    TreeNode parentNode = selectNode.Parent;
                    int nParentFieldID = (int)parentNode.Tag;

                    DataRow parentRow = m_DicMetaInfoTable.Rows.Find(nParentFieldID);
                    if (parentRow == null)
                        return false;

                    if (parentRow["fieldtype"].ToString().Trim() == "1")
                    {
                        string strTableName = parentRow["tablename"].ToString().Trim();
                        bRetCode = CheckFieldExist(strTableName, strFieldName);
                        if (bRetCode)
                        {
                            MessageBox.Show("虚拟字段英文名与父虚拟字段对应的表中的某个字段重名，请换个名字!");
                            this.FieldName_text.Text = strOldFieldName;
                            return false;
                        }
                    }

                    string strSQL =
                        string.Format("SELECT * FROM sys_meta_info WHERE parentfield = {0} AND fieldname = '{1}'",
                            strParentField, strFieldName);
                    string filter = string.Format("parentfield = {0} AND fieldname = '{1}'", strParentField, strFieldName);
                    //DataTable table = Helper.GetDataTable(strSQL, m_conn);
                    DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_conn);
                    if (table != null && table.Rows.Count > 0)
                    {
                        MessageBox.Show("虚拟字段英文名与该字段所在同一级某个字段同名，请换个名字!");
                        this.FieldName_text.Text = strOldFieldName;
                        return false;
                    }
                }
            }

            row.BeginEdit();
            row["fieldname"] = this.FieldName_text.Text.ToString().Trim();
            row["fieldcnname"] = this.FieldCNName_Text.Text.ToString().Trim();
            row["catname"] = this.catname_comboBox.Text.ToString().Trim();
            row["description"] = this.descriptiontxt.Text.ToString().Trim();

            int nSelectedIndex = 0;
            if (EditorType_comboBox.SelectedIndex < 0 || EditorType_comboBox.SelectedIndex >= EditorTypeEn.Length)
                nSelectedIndex = 0;
            else
                nSelectedIndex = EditorType_comboBox.SelectedIndex;

            switch (EditorTypeEn[nSelectedIndex])
            {
                case "filename":
                    row["relativepath"] = this.txtBoxRelativePath.Text.ToString().Trim();
                    row["cutrelativepath"] = txtBoxCutRelativePath.Text.ToString().Trim();
                    row["editortype"] = EditorTypeEn[nSelectedIndex];
                    row["cutpreslash"] = chkCutSlash.Checked;
                    break;
                case "lookupcombo":
                    row["listtable"] = this.listtable_comboBox.Text.ToString().Trim();
                    row["keyfield"] = this.keyfield_comboBox.Text.ToString().Trim();
                    row["listfield"] = this.listfield_comboBox.Text.ToString().Trim();
                    row["listcondition"] = this.listconditiontxt.Text.ToString().Trim();
                    row["listexfields"] = this.listexfields.Text.ToString().Trim();
                    row["editortype"] = EditorTypeEn[nSelectedIndex];
                    break;
                case "textcombo":
                    row["listvalues"] = txtBoxListValues.Text.ToString().Trim();
                    row["editortype"] = EditorTypeEn[nSelectedIndex];
                    break;
                default:
                    row["editortype"] = EditorTypeEn[nSelectedIndex];
                    break;
            }

            if (strFieldType == "1")
            {
                string strFilters = "";
                for (int i = 0; i < FilterListBox.Items.Count; ++i)
                {
                    strFilters += FilterListBox.Items[i].ToString().Trim() + ";";
                }
                row["subtablefilter"] = strFilters;
            }
            //保存虚拟字段排序方式
            row["sorttype"] = sortType_Combox.SelectedIndex == 1 ? 1 : 0;

            row["readonly"] = ReadOnly_checkBoxX.Checked ? 1 : 0;
            row["visible"] = CHKHide.Checked ? 0 : 1;
            
            row.EndEdit();
            bRetCode = UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.MOD);
            if (bRetCode)
            {
                treeView.SelectedNode.Text = this.FieldCNName_Text.Text.ToString().Trim() + " [" + this.FieldName_text.Text.ToString().Trim() + "]";
                TreeNode node = treeView.SelectedNode;
                if (row["fieldtype"].ToString().Trim() == "1")
                    TreeNodeSetImage(node, "virtualfield");
                else
                    TreeNodeSetImage(node, EditorTypeEn[nSelectedIndex]);
                if (bShowSuccessBox)
                    MessageBox.Show("更新成功!");
                return true;
            }
            else
            {
                MessageBox.Show("更新失败!");
                return false;
            }
        }

        private void RefreshKeyfiedAndListfield()
        {
            if (listtable_comboBox.Text != "")
            {
                try
                {
                    keyfield_comboBox.Items.Clear();
                    listfield_comboBox.Items.Clear();

                    //string sql = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\') order by name asc", listtable_comboBox.Text.Trim());
                    string sql;
                    string tablename = listtable_comboBox.Text.Trim();
                    if (tablename.Contains("."))
                    {
                        string[] strArr = tablename.Split(new char[] { '.' });
                        string svrName = strArr[0];
                        string dbName = strArr[1];
                        string _tblname = strArr[3];
                        sql = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}') order by name asc", svrName, _tblname, dbName);
                    }
                    else
                    {
                        sql = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\') order by name asc", tablename);
                    }

                    //System.Data.DataTable Table = Helper.GetDataTable(sql, m_conn);
                    DataTable Table = Helper.GetDataTableWithSqlProxy("syscolumns", sql, m_conn);
                    foreach (DataRow row in Table.Rows)
                    {
                        string strColumnName = row["name"].ToString().Trim();
                        keyfield_comboBox.Items.Add(strColumnName);
                        listfield_comboBox.Items.Add(strColumnName);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void listtable_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            RefreshKeyfiedAndListfield();
        }

        private void DeleteSubAttrib_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode != null)
            {
                DialogResult result = MessageBox.Show("是否确认删除该字段", "删除提示", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
                if (result != DialogResult.Yes) // 用户未选择确定
                {
                    return;
                }
                TreeNode node = treeView.SelectedNode;
                DeleteSubNodes(node);
                DataRow row = m_DicMetaInfoTable.Rows.Find(new object[] { (int)node.Tag });
                row.Delete();
                node.Remove();
                UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.DEL);
            }
        }

        /// <summary>
        /// 窗口关闭事件处理
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void ModelEdit_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
  
            // 保存信息
            if (treeView.SelectedNode != null)
            {
                bool bRetCode = false;
                switch (treeView.SelectedNode.Level)
                {
                    case 0:
                        {
                            break;
                        }
                    case 1: // 保存模块信息
                        {
                            if (this.comboBoxEx1.Tag != null && this.comboBoxEx1.Text.Trim() != string.Empty)
                                bRetCode = SaveModContain(false);
                            else
                                bRetCode = true;
                            break;
                        }
                    case 2: // 保存分页信息
                        {
                            bRetCode = SaveTabContain(false);
                            break;
                        }
                    default: // 保存属性信息
                        {
                            bRetCode = SaveFieldContain(false);
                            break;
                        }
                }

                // 删除没有新建属性页的模块
                List<TreeNode> list = new List<TreeNode>();
                foreach(TreeNode node in treeView.Nodes[0].Nodes)
                {
                    if(node.Nodes.Count == 0) // 该模块没有新建属性页
                    {
                        DeletaMod(node);
                        list.Add(node);
                    }
                }
                foreach(TreeNode node in list)
                {
                    node.Remove();
                }

                if (bRetCode == false)
                {
                    m_Node = treeView.SelectedNode;
                    PostMessage(this.Handle, WM_RESET_SELECTED_NODE, IntPtr.Zero, "");
                }
            }

            Hide();
            log.Log(TimeLog.enumLogType.ltend, null, null, null);
        }

        private void ExportConfig_button_Click(object sender, EventArgs e)
        {
            int nModelID = (int)treeView.SelectedNode.Tag;
            ExportEditor editor = new ExportEditor(nModelID, m_conn);
            editor.ShowDialog();
        }

        public void SetTreeViewSelectNode(TreeNode node)
        {
            treeView.SelectedNode = node;
            FillTabContain(treeView.SelectedNode);
        }

        private TreeNode GetModuleNode(TreeNode node)
        {
            if (node.Level == 0)
            {
                return null;
            }
            TreeNode nodeReturn = node;
            for (int i = 0; i < node.Level - 1; i++)
            {
                nodeReturn = nodeReturn.Parent;
            }
            return nodeReturn;
        }

        private void treeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            // 判断能否更改配置
            TreeNode moduleNode = GetModuleNode(e.Node);
            if (Program.m_bUseNewAcl && moduleNode != null)
            {
                string moduleName = moduleNode.Text;
                DataTable tbl = Helper.GetDataTableWithSqlProxy("sys_modl_def", "SELECT id FROM sys_modl_def WHERE modelname_en = '" + moduleName + "'", m_conn);
                int moduleid = Convert.ToInt32(tbl.Rows[0][0]);
                string _sql = string.Format("SELECT * FROM sys_useracl_desc WHERE userindex = {0} AND moduleid = {1} AND canconfigmeta = 1", Program.m_rightmgr._userindex, moduleid);
                string filter = string.Format("userindex = {0} AND moduleid = {1} AND canconfigmeta = 1", Program.m_rightmgr._userindex, moduleid);
                //canCfgMeta = Helper.GetDataTable(_sql, m_conn).Rows.Count > 0;
                canCfgMeta = Helper.GetDataTableProxy("sys_useracl_desc", filter, null, m_conn).Rows.Count > 0;
            }
            
            FillTabContain(e.Node);
        }
        
        TreeNode m_Node = null;

        /// <summary>
        /// 选中结点改变之前的处理
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            bool bRetCode = false;
            if(treeView.SelectedNode == null)
            {
                return;
            }

            switch(treeView.SelectedNode.Level)
            {
                case 0:
                    {
                        bRetCode = true;
                        break;
                    }
                case 1: // 保存模块信息
                    {
                        if(m_ContentChanged == ContentChanged.model) // 模块信息发生变化
                        {
                            if (this.comboBoxEx1.Tag != null && this.comboBoxEx1.Text.Trim() != string.Empty)
                                bRetCode = SaveModContain(false);
                            else
                                bRetCode = true;
                        }
                        else
                        {
                            bRetCode = true;
                        }
                        break;
                    }
                case 2: // 保存分页信息
                    {
                        if(m_ContentChanged == ContentChanged.tab) // 分页信息发生变化
                        {
                            bRetCode = SaveTabContain(false);
                        }
                        else
                        {
                            bRetCode = true;
                        }
                        break;
                    }
                default: // 保存属性信息
                    {
                        if(m_ContentChanged == ContentChanged.field) // 属性信息发生变化
                        {
                            bRetCode = SaveFieldContain(false);
                        }
                        else
                        {
                            bRetCode = true;
                        }
                        break;
                    }
            }

            m_ContentChanged = ContentChanged.none;
            if (bRetCode == false)
            {
                m_Node = treeView.SelectedNode;
                PostMessage(this.Handle, WM_RESET_SELECTED_NODE, IntPtr.Zero, "");
            }
        }

        /// <summary>
        /// 导入字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void ImortAllField_buttonItem1_Click(object sender, EventArgs e)
        {
            int nTotalCount = 0;
            int nDeleteCount = 0;
            int nExistCount = 0;
            int nAddCount = 0;
            int nParentID = 0;
            int nModTabId = 0;
            string strTableName = "";

            TreeNode selectNode = treeView.SelectedNode;
           
            if (selectNode.Level == 2)
            {
                DataRow row = m_ModelTabDefTable.Rows.Find(selectNode.Text.Trim());
                if (row == null)
                    return;
                nModTabId    = (int)row["modtabid"];
                strTableName = row["tablename"].ToString().Trim();
            }
            else // (selectNode.Level > 2)
            {
                int nFieldID = (int)selectNode.Tag;
                DataRow row = m_DicMetaInfoTable.Rows.Find(nFieldID);
                if (row == null)
                    return;
                string strFieldType = row["fieldtype"].ToString().Trim();
                if (strFieldType != "1")
                    return;
                strTableName = row["tablename"].ToString().Trim();
                nParentID = nFieldID;
                nModTabId = (int)row["modtabid"];
            }

            string sqltxt = "SELECT * FROM sys_meta_info WHERE tablename = '" + strTableName + "'";
            string filter = "tablename = '" + strTableName + "'";

            //DataTable metaInfo = Helper.GetDataTable(sqltxt, m_conn);
            DataTable metaInfo = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_conn);
            
            //DataTable columnsExist = Helper.GetDataTable("SELECT fieldname FROM sys_meta_info WHERE tablename = '" + strTableName + "' AND modtabid = " + nModTabId + " AND parentfield = " + nParentID, m_conn);

            filter = string.Format("tablename = '" + strTableName + "' AND modtabid = " + nModTabId + " AND parentfield = " + nParentID, m_conn);
            DataTable columnsExist = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_conn);

            string sql;
            if (strTableName.Contains("."))
            {
                string[] strArr = strTableName.Split(new char[] { '.' });
                string svrName = strArr[0];
                string dbName = strArr[1];
                string _tblname = strArr[3];
                sql = string.Format("SELECT name FROM {0}.{2}.dbo.syscolumns WHERE id = (SELECT id from {0}.{2}.dbo.sysobjects WHERE name = '{1}')", svrName, _tblname, dbName);
            }
            else
            {
                sql = string.Format("SELECT name FROM syscolumns WHERE id = object_id(\'{0}\')", strTableName);
            }

            DataTable columns = Helper.GetDataTableWithSqlProxy("syscolumns", sql, m_conn);

            // 显示部分导入窗口         
            List<string> list1 = new List<string>();
            List<string> list2 = new List<string>();
            foreach(DataRow r in columns.Rows)
            {
                list1.Add(r["name"].ToString().Trim());
            }
            foreach(TreeNode node in selectNode.Nodes)
            {
                string name = node.Tag.ToString().Trim();
                list2.Add(name);
            }
            DBTools.ChooseField cf = new DBTools.ChooseField(m_conn, list1, list2);
            DialogResult result = cf.ShowDialog();
            if(result != DialogResult.OK) // 用户未确认选择
            {
                return;
            }

            List<TreeNode> deleteNodes = new List<TreeNode>(); // 要删除的树结点链表
            List<string> attributeNames = new List<string>(); // 原先存在的属性名链表
            nTotalCount = cf.ColumnNames.Count;

            // 计算删除的属性数目
            foreach(TreeNode node in selectNode.Nodes)
            {
                if (node.ImageIndex == 7)
                {
                    continue;
                }

                string s = node.Tag.ToString();
                DataRow r = metaInfo.Rows.Find(s);
                string name = r["fieldname"].ToString().Trim();
                attributeNames.Add(name);
                if (!cf.ColumnNames.Contains(name))
                {                 
                    deleteNodes.Add(node);
                    DeleteAttribute(node);
                    nDeleteCount++;    
                }
            }

            // 删除属性
            foreach(TreeNode node in deleteNodes)
            {
                node.Remove();
            }

            // 计算添加的属性数目
            foreach(string s in cf.ColumnNames)
            {
                foreach(string t in attributeNames)
                {
                    if(t.Contains(s))
                    {
                        nAddCount++;
                        break;
                    }
                }
            }
            nAddCount = nTotalCount - nAddCount;

            // 重新刷新数据表内容
           // metaInfo = Helper.GetDataTable(sqltxt, m_conn);
            metaInfo = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_conn);

            foreach(string s in cf.ColumnNames)
            {
                string strAddColumn = s;
                bool exist = false;

                foreach (string t in attributeNames)
                {
                    if (t.Contains(s))
                    {
                        nExistCount++;
                        exist = true;
                        break;
                    }
                }

                if(exist)
                {
                    continue;
                }

                DataRow newRow = m_DicMetaInfoTable.NewRow();
                newRow.BeginEdit();
                newRow["fieldtype"] = 0;
                newRow["parentfield"] = nParentID;
                newRow["tablename"] = strTableName;
                newRow["fieldname"] = strAddColumn;
                newRow["modtabid"] = nModTabId;
                newRow["fieldcnname"] = strAddColumn;
                newRow["editortype"] = "text";
                newRow["readonly"] = 0;
                newRow.EndEdit();
                m_DicMetaInfoTable.Rows.Add(newRow);
                UpdateData("SELECT * FROM sys_meta_info", m_DicMetaInfoTable, UpdateType.ADD);
                m_DicMetaInfoTable.AcceptChanges();

                AddTreeNode(selectNode, strAddColumn + "[" + strAddColumn + "]", newRow["fieldid"], "text");
            }

            string strMsg = string.Format("总共{0}条记录，已存在{3}条记录，添加了{1}条记录，删除了{2}条记录", 
                nTotalCount, nAddCount, nDeleteCount, nExistCount);
            MessageBox.Show(strMsg);
        }

        private TreeNode FindNode(int nFieldID, TreeNodeCollection ParentNodes)
        {
            if (ParentNodes == null)
                return null;

            foreach (TreeNode node in ParentNodes)
            {
                if (node.Level > 2)
                {
                    if ((int)node.Tag == nFieldID)
                        return node;
                }
                TreeNode find = FindNode(nFieldID, node.Nodes);
                if (find != null)
                    return find;
            }
            return null;
        }

        public void EditField(int nFieldID)
        {
            foreach (TreeNode node in treeView.Nodes)
            {
                if (node.Nodes != null)
                {
                    TreeNode find = FindNode(nFieldID, node.Nodes);
                    if (find != null)
                    {
                        treeView.SelectedNode = find;
                        break;
                    }
                }
            }
        }

        /// <summary>
        /// 检查所有的控件是否值发生变化
        /// </summary>
        private void CheckChange(object sender, EventArgs e)
        {
            if(lockCheckChange) // 锁定检查内容是否修改
            {
                return;
            }
            TreeNode currentNode = treeView.SelectedNode; // 当前选中的树结点
            if(currentNode != null)
            {
                switch(currentNode.Level)
                {
                    case 0: // 新家模块结点
                        {
                            break;
                        }
                    case 1: // 模块结点
                        {
                            m_ContentChanged = ContentChanged.model;
                            break;
                        }
                    case 2: // tab页结点
                        {
                            m_ContentChanged = ContentChanged.tab;
                            break;
                        }
                    case 3: // 属性结点
                        {
                            m_ContentChanged = ContentChanged.field;
                            break;
                        }
                    default: // 默认处理
                        {
                            m_ContentChanged = ContentChanged.field;
                            break;
                        }
                }
            }
        }

        private void Add_buttonX_Click(object sender, EventArgs e)
        {
            if (this.FieldNames.SelectedIndex == -1)
                return;

            string newSelectedField = this.FieldNames.Text.Trim();
            this.SelectedNames.Items.Add(newSelectedField);
            this.FieldNames.Items.RemoveAt(this.FieldNames.SelectedIndex);
            this.FieldNames.SelectedIndex = -1;
        }

        private void Delete_bottonX_Click(object sender, EventArgs e)
        {
            if (this.SelectedNames.SelectedIndex == -1)
                return;
            string deleteField = this.SelectedNames.Text.Trim();
            this.FieldNames.Items.Add(deleteField);
            this.SelectedNames.Items.RemoveAt(this.SelectedNames.SelectedIndex);
            this.SelectedNames.SelectedIndex = -1;
        }

        private void AddFilterBotton_Click(object sender, EventArgs e)
        {
            /*if (this.MainTableFieldsComboBox.SelectedIndex == -1 ||
                this.SubTableFieldsComboBox.SelectedIndex == -1
            )
            {
                return;
            }*/

            List<string> attributeList1 = new List<string>(); // 主表中的属性链表
            List<string> attributeList2 = new List<string>(); // 虚拟字段中的表中的属性链表
            List<string> existAttributes1 = new List<string>(); // 已经存在的主表中的属性链表
            List<string> existAttributes2 = new List<string>(); // 已经存在的虚拟字段中的表中的属性链表

            foreach (string s in this.FilterListBox.Items) // 获取当前关联关系
            {
                string[] t = s.Split(',');
                existAttributes1.Add(t[0]); // 填充已经存在的主表中的属性链表 
                existAttributes2.Add(t[1]); // 填充已经存在的虚拟字段中的表中的属性链表
            }

            foreach(string s in this.MainTableFieldsComboBox.Items) // 填充主表中的属性链表
            {
                if(!existAttributes1.Contains(s))
                {
                    attributeList1.Add(s);
                }
            }

            foreach(string s in this.SubTableFieldsComboBox.Items) // 填充虚拟字段中的表中的属性链表
            {
                if(!existAttributes2.Contains(s))
                {
                    attributeList2.Add(s);
                }
            }

            // 显示拖动窗口
            AttachAttribute aForm = new AttachAttribute(imglstIcons);
            aForm.AttributeList1 = attributeList1;
            aForm.AttributeList2 = attributeList2;
            aForm.ExistAttributes1 = existAttributes1;
            aForm.ExistAttributes2 = existAttributes2;
            aForm.FillContent();
            DialogResult result = aForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                FilterListBox.Items.Clear();
                foreach(string[] s in aForm.DataList) // 更新关联信息
                {
                    this.FilterListBox.Items.Add(s[0] + "," + s[1]);
                }
                this.MainTableFieldsComboBox.SelectedIndex = -1;
                this.SubTableFieldsComboBox.SelectedIndex = -1;
            }
            else
            {
                return;
            }

            /*string strMainField = this.MainTableFieldsComboBox.Text.Trim();
            string strSubField = this.SubTableFieldsComboBox.Text.Trim();
            string strFilter = string.Format("{0},{1}", strMainField, strSubField);

            for (int i = 0; i < this.FilterListBox.Items.Count; ++i)
            {
                string strExistFilter = this.FilterListBox.Items[i].ToString().Trim();
                if (strExistFilter == strFilter)
                {
                    MessageBox.Show(string.Format("{0}此关联条件已存在!", strFilter));
                    return;
                }
            }
            this.FilterListBox.Items.Add(strFilter);
            this.MainTableFieldsComboBox.SelectedIndex = -1;
            this.SubTableFieldsComboBox.SelectedIndex = -1;*/
        }

        private void DelFilterButton_Click(object sender, EventArgs e)
        {
            if (this.FilterListBox.SelectedIndex == -1)
                return;
            this.FilterListBox.Items.RemoveAt(this.FilterListBox.SelectedIndex);
        }

        private void txtBoxRelativePath_TextChanged(object sender, EventArgs e)
        {
            string strRelativePath = txtBoxRelativePath.Text;
            int nFind = strRelativePath.IndexOf('*');
            nFind = nFind >= 0 ? nFind : strRelativePath.IndexOf('?');
            if (nFind >= 0)
                strRelativePath = txtBoxRelativePath.Text.Substring(0, nFind);
            txtBoxbrowPathPreview.Text = Helper.AddSlash(Helper.CombinePaths(Program.RootDir, strRelativePath));
            Color oldcolor = txtBoxbrowPathPreview.BackColor;
            bool bDirExists = Directory.Exists(txtBoxbrowPathPreview.Text);
            txtBoxbrowPathPreview.BackColor = bDirExists == true ? SystemColors.Control : Color.Red;            
            txtBoxCutRelativePath_TextChanged(sender, e);
            if (txtBoxbrowPathPreview.BackColor != oldcolor)
                FileName_groupBox.Refresh();
            if (!bDirExists)
            {
                this.balloonTipFileBrowser.ShowBalloon(txtBoxbrowPathPreview);
                //this.balloonTipFileBrowser.BalloonControl.AutoResize();
                //this.balloonTipFileBrowser.BalloonControl.Width = 256;
                //this.balloonTipFileBrowser.BalloonControl.Height = 100;
            }
        }

        private void txtBoxCutRelativePath_TextChanged(object sender, EventArgs e)
        {
            string strCutPath = Helper.AddSlash(Helper.CombinePaths(Program.RootDir, txtBoxCutRelativePath.Text));
            string[] newvaluearray = Helper.split(txtBoxbrowPathPreview.Text.ToLower(), strCutPath.ToLower());
            if (txtBoxbrowPathPreview.Text.ToLower().IndexOf(strCutPath.ToLower()) >= 0)
            {
                if (newvaluearray != null && newvaluearray.Length > 1)
                {
                    txtBoxFinalPreview.Text = (chkCutSlash.Checked ? "" : "\\") + (Helper.AddSlash(newvaluearray[1]) + "xxx.lua");
                }
                else if (newvaluearray != null && newvaluearray.Length == 1)
                {
                    txtBoxFinalPreview.Text = chkCutSlash.Checked ? "xxx.lua" : "\\xxx.lua";
                }
                else
                    txtBoxFinalPreview.Text = "非法的设置";
            }
            else
                txtBoxFinalPreview.Text = "非法的设置";
        }

        private void balloonTipFileBrowser_BalloonDisplaying(object sender, EventArgs e)
        {
            this.balloonTipFileBrowser.BalloonControl.Width = 200;
            this.balloonTipFileBrowser.BalloonControl.Height = 80;
        }

        private void chkCutSlash_CheckedChanged(object sender, EventArgs e)
        {
            if (txtBoxFinalPreview.Text != "非法的设置")
            {
                txtBoxRelativePath_TextChanged(sender, e);
            }
        }

        /// <summary>
        /// 管理显示顺序
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem1_Click(object sender, EventArgs e)
        {
            TreeNode node = treeView.SelectedNode;
            if(node == null) // 选中结点为空
            {
                return;
            }
            DBTools.SortForm sf = new DBTools.SortForm(m_conn, node, this.treeView.ImageList);
            DialogResult result = sf.ShowDialog();
            LoadDicMetaInfoTable(); // 刷新表内容
        }

        /// <summary>
        /// 用户选择新的主表
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(treeView.SelectedNode != null && !lockSelection)
            {
                foreach(TreeNode node in treeView.SelectedNode.Nodes)
                {
                    node.Tag = comboBoxEx1.SelectedItem.ToString();
                }
                selectionChanged = true;
                FillTabContain(treeView.SelectedNode);
            }
        }

        /// <summary>
        /// 用户选择设置显示样式
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void ConfigStyle_Click(object sender, EventArgs e)
        {
            DBTools.ConfigModelForm configForm = new DBTools.ConfigModelForm(m_conn, treeView.SelectedNode, this);
            configForm.ShowDialog();
        }

        /// <summary>
        /// 导出配置信息
        /// </summary>
        private void ExportConfigInfo(object sender, EventArgs e)
        {
            TreeNode currentNode = treeView.SelectedNode;
            string modelName = null;
            string tabName = null; 
            List<Hashtable> infoList = new List<Hashtable>();

            if (currentNode != null)
            {
                switch (currentNode.Level)
                {
                    case 0:
                        {
                            break;
                        }
                    case 1: // 导出模块配置信息
                        {
                            modelName = currentNode.Text;

                            foreach (TreeNode tabNode in currentNode.Nodes)
                            {
                                tabName = tabNode.Text;

                                foreach (TreeNode fieldNode in tabNode.Nodes)
                                {
                                    ExportFieldInfo(fieldNode, modelName, tabName, infoList);
                                }
                            }

                            tabName = "所有分页";

                            break;
                        }
                    case 2: // 打开分页配置信息
                        {
                            modelName = currentNode.Parent.Text;
                            tabName = currentNode.Text;

                            foreach (TreeNode fieldNode in currentNode.Nodes)
                            {
                                ExportFieldInfo(fieldNode, modelName, tabName, infoList);
                            }

                            break;
                        }
                }
            }

            // 生成字符串内容
            StringBuilder fileContent = new StringBuilder();

            fileContent.AppendLine("模块名\t分页名\t数据表名\t字段中文名\t字段英文名\t字段分类\t字段描述");

            foreach (Hashtable infoTable in infoList)
            {
                fileContent.AppendLine(string.Format("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}", infoTable["ModelName"],
                                                     infoTable["TabName"], infoTable["TableName"], infoTable["FieldName"],
                                                     infoTable["FieldChineseName"], infoTable["CategoryName"], infoTable["Description"]));
            }

            FileInfo fi = new FileInfo(Application.StartupPath);

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Title = "请选择数据文件保存的目录";
            saveFileDialog.Filter = "tab文件|*.tab";
            saveFileDialog.FileName = Path.Combine(fi.DirectoryName, string.Format("{0} {1} 字段数据.tab", modelName, tabName));

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                FileFolderHelper.StringToFile(fileContent.ToString(), saveFileDialog.FileName);
                MessageBox.Show("数据导出成功！", "导出配置数据", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 查找字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSearchField_Click(object sender, EventArgs e)
        {
            FieldSearchForm fForm = new FieldSearchForm(treeView);
            fForm.ShowDialog();
        }

        private void UniteModel_Click(object sender, EventArgs e)
        {
            TreeNode node = GetModuleNode(treeView.SelectedNode);
            string mdlname = node.Text;
            UniteModel frm = new UniteModel(mdlname);
            if (frm.RunUnite)
            {
                frm.ShowDialog(this);
            }
        }
    }
}