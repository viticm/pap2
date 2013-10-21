using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using System.Diagnostics;
using System.IO;
using LuaInterface;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    public partial class DBTableEditor : Form
    {
        SqlConnection m_conn = MainForm.conn;
        public SqlConnection Conn { get { return m_conn; } }

        Hashtable m_htAllDbTables = null;
        public Hashtable AllTables { get { return m_htAllDbTables; } set { m_htAllDbTables = value; } }

        DataTable m_dtExportTbl = null;
        public DataTable ExportTbl { get { return m_dtExportTbl; } set { m_dtExportTbl = value; } }

        DataTable m_dtTblCfgTbl = null;
        public DataTable TblCfgTbl { get { return m_dtTblCfgTbl; } set { m_dtTblCfgTbl = value; } }

        static string TRASH_FIELD = "___delete_me";
        private TimeLog m_log = new TimeLog();

        private List<TreeNode> newNodes = new List<TreeNode>(); // 新建表的树结点

        public DBTableEditor()
        {
            InitializeComponent();
            m_log.Log(TimeLog.enumLogType.ltstart, "数据表编辑器", "工具模块启动", null);
        }
        

        private void DBTableEditor_Load(object sender, EventArgs e)
        {
            AllTables = new Hashtable();
            /*
            string sql = "SELECT * FROM sys_export_table_cfg";
            ExportTbl = Helper.GetDataTable(sql, Conn);
             */
            ExportTbl = Helper.GetDataTableProxy("sys_export_table_cfg", Conn);
            /*
            sql = "SELECT * FROM sys_table_cfg";
            TblCfgTbl = Helper.GetDataTable(sql, Conn);
            */

            TblCfgTbl = Helper.GetDataTableProxy("sys_table_cfg", Conn);
            
            /*
            sql = "SELECT name FROM sysobjects WHERE (xtype = 'u') ORDER BY name";
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            DataSet ds = new DataSet();
            adp.Fill(ds, "sysobjects");
            DataTable dt = ds.Tables[0];
            */

            DataTable dt = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name", Conn);
            foreach (DataRow row in dt.Rows)
            {
                string tblName = row[0].ToString().Trim();
                if(!tblName.StartsWith("sys"))
                {
                    TreeNode tbl_node = this.tree.Nodes.Add(tblName);
                    tbl_node.ImageIndex = 3;
                    tbl_node.SelectedImageIndex = 3;
                    tbl_node.Nodes.Add("无效属性");
                }
            }

            // 伸展树结点
            tree.TopNode.ExpandAll();
            tree.TopNode.Collapse();
        }

        private void ShowFieldInfo(TreeNode node)
        {
            this.groupField.Visible = true;
            this.groupTable.Visible = false;

            string tblName = node.Parent.Text;
            DataTable tbl = ((DataTable)AllTables[tblName]);
            string fieldName = node.Text;
            if (tbl.Columns[fieldName] == null)
                return;
            Type type = tbl.Columns[fieldName].DataType;
            int len = tbl.Columns[fieldName].MaxLength;

            if (type.Name == "String")
            {
                this.combo_data_type.SelectedIndex = 1;
                this.textBox_data_len.Text = Convert.ToString(len);
                this.textBox_data_len.Enabled = true;
                this.textBox_Zhengshu_len.Text = "";
                this.textBox_Zhengshu_len.Enabled = false;
                this.textBox_Xiaoshu_len.Text = "";
                this.textBox_Xiaoshu_len.Enabled = false;
            }
            else if (type.Name == "Decimal")
            {
                this.combo_data_type.SelectedIndex = 0;
                this.textBox_data_len.Text = "";
                this.textBox_data_len.Enabled = false;

                string sql = string.Format("SELECT * FROM syscolumns WHERE name='{0}' AND id=object_id('{1}')", fieldName, tblName);

               
                DataTable table = Helper.GetDataTableWithSqlProxy("syscolumns", sql, Conn);
                DataRow row = table.Rows[0];
                //DataRow row = Helper.GetDataTable(sql, Conn).Rows[0];
                int prec = Convert.ToInt32(row["prec"]);
                int scale = Convert.ToInt32(row["scale"]);

                this.textBox_Zhengshu_len.Text = Convert.ToString(prec);
                this.textBox_Zhengshu_len.Enabled = true;
                this.textBox_Xiaoshu_len.Text = Convert.ToString(scale);
                this.textBox_Xiaoshu_len.Enabled = true;
            }
            else if (type.Name == "Boolean")
            {
                this.combo_data_type.SelectedIndex = 2;
                this.textBox_data_len.Text = "";
                this.textBox_data_len.Enabled = false;
                this.textBox_Zhengshu_len.Text = "";
                this.textBox_Zhengshu_len.Enabled = false;
                this.textBox_Xiaoshu_len.Text = "";
                this.textBox_Xiaoshu_len.Enabled = false;
            }
            else if (type.Name == "Int32")
            {
                this.combo_data_type.SelectedIndex = 3;
                this.textBox_data_len.Text = "";
                this.textBox_data_len.Enabled = false;
                this.textBox_Zhengshu_len.Text = "";
                this.textBox_Zhengshu_len.Enabled = false;
                this.textBox_Xiaoshu_len.Text = "";
                this.textBox_Xiaoshu_len.Enabled = false;
            }
            else
            {
                this.combo_data_type.SelectedIndex = -1;
                this.textBox_data_len.Text = "";
                this.textBox_Zhengshu_len.Text = "";
                this.textBox_Xiaoshu_len.Text = "";
            }
        }

        private void ShowTableInfo(TreeNode node)
        {
            this.groupField.Visible = false;
            this.groupTable.Visible = true;

            this.textBox_rela_path.Text = "";
            this.checkHasHead.Checked = true;
            foreach (DataRow row in ExportTbl.Rows)
            {
                if (row["tablename"].ToString().Trim() == node.Text)
                {
                    this.textBox_rela_path.Text = row["filename"].ToString().Trim();

                    if (row["hastitle"] != DBNull.Value)
                    {
                        this.checkHasHead.Checked = Convert.ToBoolean(row["hastitle"]);
                    }

                    // 显示不导出的字段
                    this.textBox_Exclude_Field.Text = row["export_ignore"].ToString();

                    break;
                }
            }

            this.checkHasDefault.Checked = true;
            this.comboExportRule.SelectedIndex = 0;
            this.checkBrowseFolder.Checked = false;

            foreach (DataRow row in TblCfgTbl.Rows)
            {
                if (row["tablename"].ToString().Trim() == node.Text)
                {
                    if (row["hasdefaultrow"] != DBNull.Value)
                        this.checkHasDefault.Checked = Convert.ToBoolean(row["hasdefaultrow"]);
                    if (row["savetype"] != DBNull.Value)
                        this.comboExportRule.SelectedIndex = Convert.ToInt32(row["savetype"]);
                    if (row["bBrowseFolderWhileImport"] != DBNull.Value)
                        this.checkBrowseFolder.Checked = Convert.ToBoolean(row["bBrowseFolderWhileImport"]);
                    break;
                }
            }
        }

        private void TreeNodeClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Node != null && (e.Button == MouseButtons.Left || e.Button == MouseButtons.Right))
            {
                if (e.Node.Level == 0) // 点选表
                {
                    if(e.Node.FirstNode != null && e.Node.FirstNode.Text == "无效属性") // 需要加载子结点
                    {
                        ReloadAttributes(e.Node);
                    }
                    ShowTableInfo(e.Node);
                }
                else if (e.Node.Level == 1) // 点选字段
                {
                    ShowFieldInfo(e.Node);
                }
            }

            if (e.Node != null && e.Button == MouseButtons.Right)
            {
                this.tree.SelectedNode = e.Node;
                if (e.Node.Level == 0)
                    cTblNodeClick.Popup(Control.MousePosition);
                else if (e.Node.Level == 1)
                    cFieldNodeClick.Popup(Control.MousePosition);
            }
        }

        private void DBTableEditor_FormClosing(object sender, FormClosingEventArgs e)
        {
            //foreach (TreeNode node in this.tree.Nodes)
            foreach(TreeNode node in newNodes)
            {
                string tblName = node.Text.Trim();
                DataTable tbl = ((DataTable)AllTables[tblName]);
                if (tbl.Columns.Count == 1)
                {
                    if (tbl.Columns[0].ColumnName == TRASH_FIELD)
                    {
                        string warning = string.Format("存在空表{0}, 该表不会被保存到数据库。", tblName);
                        MessageBox.Show(warning);

                        if (Helper.RemoveDbTable(tblName, Conn))
                        {
                            node.Remove();
                            AllTables[tblName] = null;
                        }
                    }
                }
            }

            newNodes.Clear();
            e.Cancel = true;
            Hide();

            m_log.Log(TimeLog.enumLogType.ltend, null, null, null);
        }

        private void OK_button_Click(object sender, EventArgs e) // 修改
        {
            TreeNode node = this.tree.SelectedNode;

            if (node.Level == 0) // 修改表信息
            {
                string tblName = node.Text.Trim();
                bool bSuc = false;
                try
                {
                    // 更新 sys_export_table_cfg 表
                    //DataTable tbl_export_tbl_cfg = Helper.GetDataTable("SELECT * FROM sys_export_table_cfg", Conn);

                    DataTable tbl_export_tbl_cfg = Helper.GetDataTableProxy("sys_export_table_cfg", Conn);

                    bool bFindTbl = false;
                    foreach (DataRow row in tbl_export_tbl_cfg.Rows)
                    {
                        if (row["tablename"].ToString().Trim() == tblName)
                        {
                            row["filename"] = this.textBox_rela_path.Text;
                            row["hastitle"] = this.checkHasHead.Checked;
                            row["export_ignore"] = this.textBox_Exclude_Field.Text;

                            bFindTbl = true;
                            break;
                        }
                    }
                    if (!bFindTbl)
                    {
                        DataRow newRow = tbl_export_tbl_cfg.NewRow();
                        newRow.BeginEdit();
                        newRow["tablename"] = tblName;
                        newRow["filename"] = this.textBox_rela_path.Text;
                        newRow["hastitle"] = this.checkHasHead.Checked;
                        newRow.EndEdit();

                        tbl_export_tbl_cfg.Rows.Add(newRow);
                    }

                    /* remoting
                     * SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM sys_export_table_cfg", Conn);
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    if (bFindTbl)
                        adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                    else
                        adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = adp.Update(tbl_export_tbl_cfg);
                    tbl_export_tbl_cfg.AcceptChanges();
                    */

                    int val = Helper.SaveTable(tbl_export_tbl_cfg, "SELECT * FROM sys_export_table_cfg", Conn);

                    // 更新 sys_table_cfg
                    //DataTable tbl_tbl_cfg = Helper.GetDataTable("SELECT * FROM sys_table_cfg", Conn);

                    DataTable tbl_tbl_cfg = Helper.GetDataTableProxy("sys_table_cfg", Conn);

                    bFindTbl = false;
                    foreach (DataRow row in tbl_tbl_cfg.Rows)
                    {
                        if (row["tablename"].ToString().Trim() == tblName)
                        {
                            row["hasdefaultrow"] = this.checkHasDefault.Checked;
                            row["savetype"] = this.comboExportRule.SelectedIndex;
                            bFindTbl = true;
                            break;
                        }
                    }
                    if (!bFindTbl)
                    {
                        DataRow newRow = tbl_tbl_cfg.NewRow();
                        newRow.BeginEdit();
                        newRow["tablename"] = tblName;
                        newRow["hasdefaultrow"] = this.checkHasDefault.Checked;
                        newRow["savetype"] = this.comboExportRule.SelectedIndex;
                        newRow.EndEdit();

                        tbl_tbl_cfg.Rows.Add(newRow);
                    }
                    /* remoting
                    SqlDataAdapter adp2 = new SqlDataAdapter("SELECT * FROM sys_table_cfg", Conn);
                    SqlCommandBuilder cmdBuilder2 = new SqlCommandBuilder(adp2);
                    if (bFindTbl)
                        adp2.UpdateCommand = cmdBuilder2.GetUpdateCommand();
                    else
                        adp2.InsertCommand = cmdBuilder2.GetInsertCommand();
                    val = adp2.Update(tbl_tbl_cfg);
                    tbl_tbl_cfg.AcceptChanges();
                    */

                    val = Helper.SaveTable(tbl_tbl_cfg, "SELECT * FROM sys_table_cfg", Conn);
                     
                    bSuc = true;
                }
                catch (Exception ex)
                {
                    string err = string.Format("在修改表 {0} 的信息时发生错误。\r\n\r\n错误消息为\r\n{1}", tblName, ex.Message);
                    MessageBox.Show(err);
                }

                if (bSuc)
                {
                    //ExportTbl = Helper.GetDataTable("SELECT * FROM sys_export_table_cfg", Conn);
                    //TblCfgTbl = Helper.GetDataTable("SELECT * FROM sys_table_cfg", Conn);
                    ExportTbl = Helper.GetDataTableProxy("sys_export_table_cfg", Conn);
                    TblCfgTbl = Helper.GetDataTableProxy("sys_table_cfg", Conn);
                }
            }

            else if (node.Level == 1) // 修改字段信息
            {
                bool fieldIsKey = false;
                string tblName = node.Parent.Text.Trim();
                string fieldName = node.Text.Trim();

                string dataType = "";
                string len = "";
                int prec = -1;
                if (this.textBox_Zhengshu_len.Text.Trim() != "")
                    prec = Convert.ToInt32(this.textBox_Zhengshu_len.Text.Trim());
                else
                    prec = 0;
                int scale = -1;
                if (this.textBox_Xiaoshu_len.Text.Trim() != "")
                    scale = Convert.ToInt32(this.textBox_Xiaoshu_len.Text.Trim());
                else
                    scale = 0;
                switch (this.combo_data_type.SelectedIndex)
                {
                    case 0:
                        dataType = "decimal";
                        len = string.Format("({0},{1})", prec, scale); // 代表精度, 例如 "(18,0)" .
                        break;
                    case 1:
                        dataType = "varchar";
                        len = "(" + textBox_data_len.Text.Trim() + ")"; // 代表长度
                        if (len == "()")
                        {
                            MessageBox.Show("请指定字符串的长度.");
                            return;
                        }
                        break;
                    case 2:
                        dataType = "bit";
                        len = "";
                        break;
                    case 3:
                        dataType = "int";
                        len = "";
                        break;
                    default:
                        MessageBox.Show("请选择数据类型.");
                        return;
                }
                string can_be_null = "";
                if (Helper.FieldIsTableKey(fieldName, ((DataTable)AllTables[tblName])))
                {
                    fieldIsKey = true;
                    can_be_null = "NOT";
                }
                else
                {
                    fieldIsKey = false;
                    can_be_null = "";
                }

                string sql = string.Format("ALTER TABLE {0} ALTER COLUMN [{1}] {2}{3} {4} NULL",
                    tblName, fieldName, dataType, len, can_be_null);

                bool bSuc = false;
                Conn.Open();
                try
                {
                    /* remoting
                    SqlCommand cmd = Conn.CreateCommand();
                    cmd.CommandText = sql;
                    cmd.ExecuteNonQuery();
                    */

                    Helper.ExecuteCommand(tblName, sql, Conn);

                    bSuc = true;
                }
                catch (Exception ex)
                {
                    string err = string.Format("在修改字段 {0} 时发生错误。\r\n\r\n错误信息：{1}", fieldName, ex.Message);
                    MessageBox.Show(err);
                }
                Conn.Close();

                if (bSuc)
                {
                    if (fieldIsKey)
                        this.tree.SelectedNode.StateImageIndex = 0;
                    else if (dataType == "varchar" && len == "(3000)")
                        this.tree.SelectedNode.StateImageIndex = 7;
                    else
                        this.tree.SelectedNode.StateImageIndex = -1;

                    //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tblName, Conn);
                    DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);
                    AllTables[tblName] = tbl;
                }
            }
        }

        private void Cancel_button_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Tree_AfterLabelEdit(object sender, NodeLabelEditEventArgs e) // 重命名
        {
            string oldName = e.Node.Text.Trim();
            string newName = "";
            if (e.Label != null)
                newName = e.Label.Trim();
            else
                newName = e.Node.Text.Trim();

            if (oldName == newName) // 实际没有改名，返回
                return;

            if (e.Node.Level == 0) // 重命名表
            {
                bool bRenameSuc = false;
                Conn.Open();
                try
                {
                    bRenameSuc = Helper.RenameDbTable(oldName, newName, Conn);
                }
                catch (Exception ex)
                {
                    string str = string.Format("在重命名表 {0} 时发生错误。\r\n\r\n错误信息：{1}", oldName, ex.Message);
                    MessageBox.Show(str);
                }
                Conn.Close();

                if (bRenameSuc)
                {
                    DataTable tbl_ren = ((DataTable)AllTables[oldName]);
                    AllTables[oldName] = null;
                    AllTables[newName] = tbl_ren;
                }
            }

            else if (e.Node.Level == 1) // 重命名字段
            {
                string tblname = e.Node.Parent.Text.Trim();
                bool bRenameSuc = false;
                Conn.Open();
                try
                {
                    bRenameSuc = Helper.RenameDbField(tblname, oldName, newName, Conn);
                }
                catch (Exception ex)
                {
                    string str = string.Format("在重命名字段 {0} 时发生错误。\r\n\r\n错误信息：{1}", tblname + "." + oldName, ex.Message);
                    MessageBox.Show(str);
                }
                Conn.Close();

                if (bRenameSuc)
                {
                    //DataTable tbl_ren = Helper.GetDataTable("SELECT * FROM " + tblname, Conn);
                    DataTable tbl_ren = Helper.GetDataTableProxy(tblname, Conn);
                    AllTables[tblname] = tbl_ren;
                }
            }
        }

        private void bNewTbl_Click(object sender, EventArgs e) // 新建表
        {
            string newTblName = "";
            NameInput frm = new NameInput();
            DialogResult result = frm.ShowDialog();
            if (result == DialogResult.OK)
            {
                newTblName = frm.StringInput;
            }
            if (newTblName == "")
            {
                return;
            }

            bool bSucCreate = false;
            Conn.Open();
            try
            {
                string strcmd = string.Format("CREATE TABLE {0} ({1} varchar(3000))", newTblName, TRASH_FIELD);
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
               
                cmd.CommandText = strcmd;
                cmd.ExecuteNonQuery();
                 */

                Helper.ExecuteCommand(newTblName, strcmd, Conn);
                bSucCreate = true;
            }
            catch (Exception ex)
            {
                string str = string.Format("在添加表 {0} 时发生错误。\r\n\r\n错误信息：{1}", newTblName, ex.Message);
                MessageBox.Show(str);
            }
            Conn.Close();

            if (bSucCreate)
            {
                //DataTable new_tbl = Helper.GetDataTable("SELECT * FROM " + newTblName, Conn);
                DataTable new_tbl = Helper.GetDataTableProxy(newTblName, Conn);

                AllTables[newTblName] = new_tbl;

                TreeNode new_node = this.tree.Nodes.Add(newTblName);
                new_node.ImageIndex = 3;
                new_node.SelectedImageIndex = 3;

                this.tree.SelectedNode = new_node;
                ShowTableInfo(new_node);
                newNodes.Add(new_node);
            }

            // 显示新建表的结点
            tree.TopNode.ExpandAll();
            tree.TopNode.Collapse();
        }

        private void bDelTbl_Click(object sender, EventArgs e) // 删除表
        {
            string tbl_del = this.tree.SelectedNode.Text;
            string confirm = string.Format("你确定要删除整个表 {0} 吗?", tbl_del);
            DialogResult result = MessageBox.Show(confirm, "警告", MessageBoxButtons.YesNo);
            if (result == DialogResult.No)
                return;

            // 删除在链表中记录的树结点
            TreeNode currentNode = this.tree.SelectedNode;
            if(newNodes.Contains(currentNode))
            {
                newNodes.Remove(currentNode);
            }

            if (Helper.RemoveDbTable(tbl_del, Conn))
            {
                this.tree.SelectedNode.Remove();
                AllTables[tbl_del] = null;
            }
        }

        private void bRenameTbl_Click(object sender, EventArgs e) // 重命名表
        {
            TreeNode theNode = this.tree.SelectedNode;
            if (!theNode.IsEditing)
            {
                this.tree.LabelEdit = true;
                theNode.BeginEdit();
            }
        }

        private bool IsRowIDAcceptable(ref string[] strFields, ref string[] strColnames, string strKeyField, int iBeginNum, int iEndNum)
        {
            for (int i = 0; i < strColnames.Length; i++)
            {
                if (strColnames[i].Trim() == strKeyField)
                {
                    int id = Convert.ToInt32(strFields[i].Trim());
                    if (id >= iBeginNum && id <= iEndNum)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            MessageBox.Show("无法判断某行是否应该被导入，这通常是因为指定了错误的主键表项。");
            return false;
        }

        /// <summary>
        /// 重新加载表中的属性
        /// </summary>
        /// <param name="node">表对应的树结点</param>
        private void ReloadAttributes(TreeNode node)
        {
            string tblName = node.Text;
            //string sql = "SELECT TOP 1 * FROM " + tblName;
            //DataTable tbl = Helper.GetDataTable(sql, Conn);

            DataTable tbl = Helper.GetDataTableTopRowProxy(tblName, Conn);

            AllTables[tblName] = tbl;

            node.Nodes.Clear();
            foreach (DataColumn col in tbl.Columns)
            {
                TreeNode field_node = node.Nodes.Add(col.ColumnName.Trim());
                field_node.ImageIndex = 4;
                field_node.SelectedImageIndex = 4;

                if (Helper.FieldIsTableKey(col, tbl))
                {
                    field_node.StateImageIndex = 0;
                }
                else if (col.DataType.Name.Trim() == "String" && col.MaxLength == 3000)
                {
                    field_node.StateImageIndex = 7;
                }
            }
        }


        private bool WriteRow(ref DataTable tbl, ref DataRow row, ref string[] strColNames, ref string[] strFields)
        {
            try
            {
                for (int i = 0; i < strColNames.Length; i++)
                {
                    if (i < strFields.Length)
                    {
                        if (strFields[i] == "")
                            row[strColNames[i]] = DBNull.Value;
                        else
                        {
                            if (tbl.Columns[strColNames[i]].DataType.Name == "Boolean")
                            {
                                if (strFields[i].Trim() == "0")
                                    row[strColNames[i]] = false;
                                else
                                    row[strColNames[i]] = true;
                            }
                            else
                            {
                                try
                                {
                                    row[strColNames[i]] = strFields[i];
                                }
                                catch (Exception ex)
                                {
                                    string err = string.Format("在修改第 {0} 个数据行 (字段名: {1}; 字段值: {2}) 时发生错误, 请修改数据表后重新导入。\r\n\r\n错误信息：{3}", i, strColNames[i], strFields[i], ex.Message);
                                    MessageBox.Show(err);
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    {
                        row[strColNames[i]] = DBNull.Value;
                    }
                }
            }
            catch//Exception ex)
            {
                return false;
            }
            
            return true;
        }

        /// <summary>
        /// 调用lua脚本同时导入多文件
        /// </summary>
        /// <param name="lua">lua对象</param>
        /// <param name="path">导入根路径</param>
        /// <param name="tableName">数据表名</param>
        private object[] CallLuaLoadMultiFile(LuaEx lua, string path, string tableName)
        {
            //LuaEx lua = new LuaEx();
            String luafile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\import.lua";
            if (!File.Exists(luafile))
                return null;
            try
            {
                lua.DoFile(luafile);
                LuaFunction fun = lua.GetFunction("loadmultifile");
                if (fun != null)
                {
                    object[] retobjs = fun.Call(path, tableName);

                    return retobjs;
                }
                return null;
            }
            catch (Exception ex)
            {
                Helper.AddLog(ex.Message);
                throw;
            }    
        }

        private bool CallLuaImport(LuaEx lua, string strTableName, string filename, ref string[] fields, ref string[] values)
        {
            //LuaEx lua = new LuaEx();
            lua["Conn"] = m_conn; // 注册sql连接
            String luafile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\import.lua";
            if (!File.Exists(luafile))
                return false;
            try
            {
                lua.DoFile(luafile);
                LuaFunction fun = lua.GetFunction("onimport");
                if (fun != null)
                {
                    ImportInfoHelper helper = new ImportInfoHelper();
                    ArrayList feildlist = new ArrayList();
                    if (fields != null)
                        feildlist.AddRange(fields);

                    helper.Fields = feildlist;
                    helper.Values = values;
                    object[] retobjs = fun.Call(strTableName, Path.GetDirectoryName(filename), filename, helper);

                    //fields = (string[])feildlist.ToArray(typeof(string));
                    fields = (string[])(helper.Fields).ToArray(typeof(string));
                    values = helper.Values;

                    if (retobjs != null && retobjs.GetLength(0) > 0)
                    {
                        bool ret = (bool)retobjs[0];
                        return ret;
                    }
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
                throw;
            }
            return false;
        }

        private void bImportTbl_Click(object sender, EventArgs e) // 导入表
        {
            if (Program.m_bLockDBForCompetition)
            {
                MessageBox.Show("比赛结束了，请不要导表。");
                return;
            }

            //////////////////////////////////////////////////////////////////////////
            // 准备工作
            //////////////////////////////////////////////////////////////////////////
            string[] strLines = null;
            string[] strColNames = null;
            string tblName = this.tree.SelectedNode.Text.Trim();
            string rootPath = Program.RootDir; // 导入的根目录
            string strFile = ""; // 导入的文件路径

            // 判断该用户能不能导入先
            if (Program.m_bUseNewAcl)
            {
                try
                {
                    //DataTable tblExportTable = Helper.GetDataTable("SELECT id FROM sys_export_table_cfg WHERE tablename = '" + tblName + "' ", Conn);

                    DataTable tblExportTable = Helper.GetDataTableProxy("sys_export_table_cfg", "tablename = '" + tblName + "' ", null, Conn);

                    if (tblExportTable.Rows.Count > 0)
                    {
                        int tbl_id = Convert.ToInt32(tblExportTable.Rows[0]["id"]);
                        //DataTable tblExportModlCfg = Helper.GetDataTable("SELECT modid FROM sys_export_modl_cfg WHERE tableid = " + tbl_id.ToString() + " ", Conn);
                        DataTable tblExportModlCfg = Helper.GetDataTableProxy("sys_export_modl_cfg", "tableid = " + tbl_id.ToString(), null, Conn);
                        //if (tblExportModlCfg.Rows.Count > 0)
                        //{
                        //    int moduleid = Convert.ToInt32(tblExportModlCfg.Rows[0][0]);
                        //    string _sql = string.Format("SELECT * FROM sys_useracl_desc WHERE userindex = {0} AND moduleid = {1} AND canimport = 1", Program.m_rightmgr._userindex, moduleid);
                        //    bool can_import = Helper.GetDataTable(_sql, Conn).Rows.Count > 0;
                        //    if (Program.m_bUseNewAcl && !can_import)
                        //    {
                        //        MessageBox.Show("您对于当前数据表 " + tblName + " 没有导表权限！请申请开通。", "权限不足", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        //        return;
                        //    }
                        //}
                        if (Program.m_bUseNewAcl)
                        {
                            bool can_import = false;
                            foreach (DataRow rowex in tblExportModlCfg.Rows)
                            {
                                int moduleid = Convert.ToInt32(rowex["modid"]);
                                //string _sql = string.Format("SELECT * FROM sys_useracl_desc WHERE userindex = {0} AND moduleid = {1} AND canimport = 1", Program.m_rightmgr._userindex, moduleid);
                                string filter = string.Format("userindex = {0} AND moduleid = {1} AND canimport = 1", Program.m_rightmgr._userindex, moduleid);
                                DataTable table = Helper.GetDataTableProxy("sys_useracl_desc", filter, null, Conn);
                                bool _b = table.Rows.Count > 0;
                                //bool _b = Helper.GetDataTable(_sql, Conn).Rows.Count > 0;
                                if (_b)
                                {
                                    can_import = true;
                                    break;
                                }
                            }

                            // 如果一行都没有，证明这个物理表不在任何一个编辑模块里。那么允许导入
                            if (tblExportModlCfg.Rows.Count == 0)
                                can_import = true;

                            if (!can_import)
                            {
                                MessageBox.Show("您对于当前数据表 " + tblName + " 没有导表权限！请申请开通。", "权限不足", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                                return;
                            }
                        }
                    }

                }
                catch (Exception ex)
                {
                    MessageBox.Show("该表无法导入。\r\n错误信息为：" + ex.Message, "无法导入", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            
            bool bBrowseFolder = false;
            bool jump = false; // 是否跳过对tab和text文件的字符处理
            LuaEx lua = new LuaEx();
            lua["Conn"] = Conn;
            lua.DoString("function trace(txt)\r\n    MainForm.LogEditBox.Visible = true\r\n    MainForm.LogEditBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n' \r\n end");
            object[] results = null;
            try
            {
                results = CallLuaLoadMultiFile(lua, rootPath, tblName);
            }
            catch (Exception ex)
            {
            	MessageBox.Show("脚本函数 CallLuaLoadMultiFile() 异常。请检查 import.lua 文件。\r\n\r\n错误信息：" + ex.Message, "导入失败", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (results != null && results[0] is bool && (bool)results[0] && results.Length >= 3)
            { // 脚本调用成功
                strColNames = results[1] as string[];
                strLines = results[2] as string[];
                jump = true;
            }
            else
            { // 脚本失败或不导入, 按原先导入流程选择单个文件 或者 文件夹
                /*
                string s = string.Format("SELECT * FROM sys_table_cfg WHERE tablename='{0}'", tblName);
                DataTable tbl_cfg = Helper.GetDataTable(s, Conn);
                */

                string filter = string.Format("tablename='{0}'", tblName);
                DataTable tbl_cfg = Helper.GetDataTableProxy("sys_table_cfg", filter, null, Conn);

                if (tbl_cfg.Rows.Count == 1 && tbl_cfg.Rows[0]["bBrowseFolderWhileImport"] != DBNull.Value && Convert.ToBoolean(tbl_cfg.Rows[0]["bBrowseFolderWhileImport"]) == true)
                    bBrowseFolder = true;

                if (bBrowseFolder)
                    strFile = FileFolderHelper.BrowserFolder("请选择 Client/data/source/maps 目录。");
                else
                    strFile = FileFolderHelper.BrowserFile(new string[] { "tab", "txt" });

                if (strFile == "") // 用户是否选择文件的检查放到这里来做
                    return;
            }
             
            //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tblName, Conn);
            DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);

            List<string> newNodeList = new List<string>(); // 新增加的树结点名称链表
            List<string> newColumnList = new List<string>(); // 新增加的列的名称链表
            List<DataRow> overwriteRowList = new List<DataRow>(); // 被重写的数据行的链表
            bool deleteMe = false; // 是否有___delete_me字段
            int rowNumber = tbl.Rows.Count; // 数据表的行数
            int iOverwriteCount = 0; // 覆盖的行数
            int iAddCount = 0; // 新增的行数
            
            //TimeLog log = new TimeLog();
            //log.Log(TimeLog.enumLogType.ltstart, "数据表导入", "功能启动", string.Format("导入文件名:{0}, 导入目标数据表:{1}", strFile, tblName));          

            bool bHasTitle = true;
            if(jump)
            {
                goto label0;
            }

            // split
            string strContent = FileFolderHelper.FileToString(strFile);
            if (!bBrowseFolder && strContent == string.Empty)
            {
                MessageBox.Show("不能读取文件内容，可能文件正在被其他程序打开。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            
            strLines = strContent.Split(new char[2] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

            // 是否有表头
            /*
            string sql = string.Format("SELECT * FROM sys_export_table_cfg WHERE tablename='{0}'", tblName);
            DataTable tbl_exp = Helper.GetDataTable(sql, Conn);
             */

            string _filter = string.Format("tablename='{0}'", tblName);
            DataTable tbl_exp = Helper.GetDataTableProxy("sys_export_table_cfg", _filter, null, Conn);
            if (tbl_exp.Rows.Count > 0 && tbl_exp.Rows[0]["hastitle"] != DBNull.Value && Convert.ToBoolean(tbl_exp.Rows[0]["hastitle"]) == false)
            {
                bHasTitle = false;
            }

            if (!bBrowseFolder && bHasTitle && strLines.Length < 2)
                return;
            
            if (!bBrowseFolder)
                if (bHasTitle)
                {
                    strColNames = strLines[0].Split(new char[1] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
                    for (int i = 0; i < strColNames.Length; i++)
                    {
                        strColNames[i] = strColNames[i].Trim();
                    }
                }
                else // 没表头(列名), 自己造
                {
                    int col_count = strLines[0].Split(new char[1] { '\t' }).Length;
                    strColNames = new string[col_count];
                    for (int i = 0; i < col_count; i++)
                    {
                        strColNames[i] = string.Format("Field_{0}", i);
                    }
                }

            //////////////////////////////////////////////////////////////////////////
            // 询问是否全部导入
            //////////////////////////////////////////////////////////////////////////

            label0:

            bool bImportAllRow = true;
            string strKeyField = "";
            int iBeginNum = -1;
            int iEndNum = -1;

            ImportInfo imp_frm = new ImportInfo(strColNames);
            DialogResult imp_r = imp_frm.ShowDialog();

            if (imp_r == DialogResult.Cancel)
            {
                return;
            }

            if (imp_r == DialogResult.OK)
            {
                bImportAllRow = imp_frm.All;
                strKeyField = imp_frm.KeyField;
                iBeginNum = imp_frm.BeginNum;
                iEndNum = imp_frm.EndNum;
            }

            if(!jump) // 如果是跳转过来的，就不用再用lua脚本来干涉
            {
                lua = new LuaEx();
                lua["Conn"] = m_conn;
                lua.DoString("function trace(txt)\r\n    MainForm.LogEditBox.Visible = true\r\n    MainForm.LogEditBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n' \r\n end");
                try
                {
                    if (CallLuaImport(lua, tblName, strFile, ref strColNames, ref strLines))
                        return;
                }
                catch//(Exception ex)
                {
                    return;
                }
            }        

            // log & ftp backup
            TimeLog log = new TimeLog();
            string backupName = Helper.GetHostName() + "_" + Convert.ToString(DateTime.Now);
            backupName = backupName.Replace(':', '-');
            string importParams = string.Format("bImportAllRow:{0} strKeyField:{1} iBeginNum:{2} iEndNum:{3}", bImportAllRow, strKeyField, iBeginNum, iEndNum);
            log.Log(TimeLog.enumLogType.ltstart, "数据表导入", "功能启动", string.Format("导入文件名:{0}, 导入目标数据表:{1}, 导入参数:{2}, 备份名:{3}", strFile, tblName, importParams, backupName));
            if (File.Exists(strFile))
                Program.m_ftp.Upload(strFile, Program.m_ftp_basedir + backupName);


            try // 开始对临时表进行操作，不实际写入数据库，可以忽略所有产生的异常
            {
                // 删除___delete_me字段
                if (tbl.Columns.Contains(TRASH_FIELD))
                {
                    deleteMe = true;
                    tbl.Columns.Remove(TRASH_FIELD);
                }

                // 添加字段
                foreach (string colname in strColNames)
                {
                    if (!tbl.Columns.Contains(colname)) // 列名未在表中
                    {
                        DataColumn c = tbl.Columns.Add(colname);
                        c.MaxLength = 3000; // 设置最大长度
                        newColumnList.Add(colname);
                    }
                }

                // 添加数据行
                int key_col_index = -1;
                if (!bImportAllRow)
                {
                    for (int i = 0; i < strColNames.Length; i++)
                    {
                        if (strColNames[i].Trim() == strKeyField)
                        {
                            key_col_index = i;
                            break;
                        }
                    }
                    if (key_col_index == -1)
                    {
                        MessageBox.Show("无法判断某行是否应该被导入，这通常是因为指定了错误的主键表项。\r\n\r\n导入失败。",
                            "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                }

                // 读取数据, 考虑是否是新表
                bool bFirstLine = true;
                bool bYesToAll = false;
                bool bNoToAll = false;
                bool bTblHasPrimKey = (tbl.PrimaryKey.Length != 0);

                foreach (string eachLine in strLines)
                {
                    // 第一行是字段名; 检查
                    if (bHasTitle)
                    {
                        if (bFirstLine)
                        {
                            bFirstLine = false;
                            continue;
                        }
                    }
                    string[] strFields = eachLine.Split(new char[1] { '\t' });
                    if (eachLine.Split(new char[1] {'\t'}, StringSplitOptions.RemoveEmptyEntries).Length == 0)
                    {
                        MessageBox.Show("发现文件中有空行，请检查。");
                        return;
                    }

                    // 判断行号是否在要求导入的范围内
                    bool bRowAcceptable = true;
                    if (bImportAllRow)
                    {
                        bRowAcceptable = true;
                    }
                    else
                    {
                        try
                        {
                            int id = Convert.ToInt32(strFields[key_col_index].Trim());
                            bRowAcceptable = bImportAllRow || (id >= iBeginNum && id <= iEndNum);
                        }
                        catch (Exception ex)
                        {
                            string err = string.Format("无法判断某行是否应该被导入，这通常是因为指定了错误的主键表项。\r\n\r\n错误信息为：{0}", ex.Message);
                            MessageBox.Show(err, "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }
                    }

                    if (!bRowAcceptable)
                        continue;

                    // 开始导入
                    if (!bTblHasPrimKey) // 没定义key，当新表处理，添加全部行
                    {
                        DataRow newRow = tbl.NewRow();
                        newRow.BeginEdit();

                        if (!WriteRow(ref tbl, ref newRow, ref strColNames, ref strFields))
                        {
                            MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }

                        tbl.Rows.Add(newRow);
                        newRow.EndEdit();

                        iAddCount++;
                    }

                    else // 是旧表，需要根据key判断原来是否存在该行
                    {
                        string strAsking = "已经存在 ";

                        // format key values to find
                        object[] objKeyValues = new object[tbl.PrimaryKey.Length];
                        int iKeyIndex = 0;
                        foreach (DataColumn col in tbl.PrimaryKey)
                        {
                            string primKeyToFind = col.ColumnName;
                            for (int i = 0; i < strColNames.Length; i++)
                            {
                                if (strColNames[i].Trim().ToLower() == primKeyToFind.Trim().ToLower())
                                {
                                    if (strFields[i] == string.Empty)
                                    {
                                        string strError = string.Format("在导入表 {0} 时发现某些主键为空的行。\r\n请检查源文件 {1}。", tblName, strFile);
                                        MessageBox.Show(strError, "失败", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                        return;
                                    }

                                    objKeyValues[iKeyIndex++] = strFields[i];
                                    strAsking += primKeyToFind;
                                    strAsking += "为";
                                    strAsking += strFields[i];
                                    strAsking += " ";
                                }
                            }
                        }
                        strAsking += "的行。";

                        // find it
                        DataRow row = tbl.Rows.Find(objKeyValues);
                        if (row != null) // 有相同key的行，询问
                        {
                            if (bYesToAll)
                                goto label_overwrite_it;

                            if (bNoToAll)
                                continue;

                            // 询问用户
                            TabImportComfirming cfmForm = new TabImportComfirming(strAsking);
                            DialogResult result = cfmForm.ShowDialog();
                            switch (result)
                            {
                                case DialogResult.Yes:
                                    goto label_overwrite_it;
                                case DialogResult.OK:
                                    bYesToAll = true;
                                    goto label_overwrite_it;
                                case DialogResult.No:
                                    continue;
                                case DialogResult.Ignore:
                                    bNoToAll = true;
                                    continue;
                                case DialogResult.Cancel:
                                    return;
                                default:
                                    break;
                            }

                        label_overwrite_it: // 覆盖行数据
                            if (!WriteRow(ref tbl, ref row, ref strColNames, ref strFields))
                            {
                                MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return;
                            }

                            overwriteRowList.Add(row); // 添加入重写行链表
                            iOverwriteCount++;
                        }

                        // 加入新行
                        else
                        {
                            DataRow newRow = tbl.NewRow();
                            newRow.BeginEdit();

                            if (!WriteRow(ref tbl, ref newRow, ref strColNames, ref strFields))
                            {
                                MessageBox.Show("导入失败，所有新增数据将不被导入!",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return;
                            }

                            tbl.Rows.Add(newRow);
                            newRow.EndEdit();

                            iAddCount++;
                        }
                    }
                }

                // 可以填充默认值。
                if (MainForm.m_bModifyDbTableDefault)
                {
                    if (bTblHasPrimKey)
                    {
                        DialogResult result = MessageBox.Show("你希望把该表中的空项填充成相应的默认值吗？\r\n\r\n推荐您选 < 是(Y) >。", "提示", MessageBoxButtons.YesNo);
                        if (result == DialogResult.Yes)
                        {
                            try
                            {
                                Helper.FillDBNullWithDefault(ref tbl);
                            }
                            catch (SqlException ex)
                            {
                                MessageBox.Show("将表中空项填充成默认值时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return;
                            }
                        }
                    }
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("读取数据行时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                                    "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 提交 
            if(Conn.State == ConnectionState.Closed) // 打开sql连接
            {
                Conn.Open();
            }

            // 允许数据库系统表更新
            /* remoting
            SqlCommand cfg_cmd = Conn.CreateCommand();
            cfg_cmd.CommandText = "sp_configure 'allow updates', '1'";
            cfg_cmd.ExecuteNonQuery();
            cfg_cmd.CommandText = "reconfigure with override";
            cfg_cmd.ExecuteNonQuery();
            */

            Helper.ExecuteCommand("sp_configure 'allow updates', '1'", Conn);
            Helper.ExecuteCommand("reconfigure with override", Conn);

            /*
            SqlTransaction transaction = Conn.BeginTransaction(); //** 开启事务
            SqlCommand cmd = new SqlCommand(); // 新建sql命令
            cmd.Connection = Conn;
            cmd.Transaction = transaction; // 绑定事务对象
            */

            DataTableProxy proxy = null;
            if (Program.EnableRemoting)
            {
                proxy = DataTableServerProxy.GetDataTableProxy();
                proxy.DeleteRemotingTable(tblName);
            }

            int nID = Helper.BeginTransaction(Conn);
            

            int col_index = 0; // 列序号
            int index = 0; // 行计数游标

            List<string> strColNameList = new List<string>(); // 记录导入列名的链表
            foreach(string s in strColNames)
            {
                strColNameList.Add(s);
            }

            try
            {
                // 添加字段
                foreach (string colname in newColumnList)
                {
                    string columnName = colname.Replace("]", "]]"); // 替换特殊字符
                    /* remoting
                    cmd.CommandText = string.Format("ALTER TABLE {0} ADD [{1}] [varchar] (3000) NULL", tblName, columnName);
                    cmd.ExecuteNonQuery();  
                    */

                    string sql = string.Format("ALTER TABLE {0} ADD [{1}] [varchar] (3000) NULL", tblName, columnName);
                    Helper.ExecuteTransaction(tblName, sql, nID);
                     
                    newNodeList.Add(colname);            
                }

                // 删除___delete_me字段
                if(deleteMe)
                {
                    /* remoting
                    cmd.CommandText = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tblName, TRASH_FIELD);
                    cmd.ExecuteNonQuery();
                     */
                    string sql = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tblName, TRASH_FIELD);
                    Helper.ExecuteTransaction(tblName, sql, nID);

                }

                // 检查是否导入列的顺序发生了变化
                bool sameOrder = true; // 列顺序是否变化的标记
                for (int i = 0; i < strColNames.Length; i++)
                {
                    if (strColNames[i] != tbl.Columns[i].ColumnName)
                    {
                        sameOrder = false;
                        break;
                    }
                }

                if (!sameOrder) // 导入列的顺序发生了变化，需要重排序
                {
                    // 因为系统表的列重排序后可能会出现数据问题，所以先清空所有行数据，调整后再重新加入
                    /*
                    cmd.CommandText = string.Format("DELETE FROM {0}", tblName);
                    cmd.ExecuteNonQuery();
                    */

                    string _sql = string.Format("DELETE FROM {0}", tblName);
                    Helper.ExecuteTransaction(tblName, _sql, nID);

                    foreach (DataColumn c in tbl.Columns) // 所有列colid加1000，为重排序做准备
                    {
                        col_index++;
                        //cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index + 1000, c.ColumnName, tblName);
                        //cmd.ExecuteNonQuery();
                        string sql = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index + 1000, c.ColumnName, tblName);
                        Helper.ExecuteTransaction("syscolumns", sql, nID);
                    }

                    col_index = 0;

                    foreach (string colname in strColNames) // 新列重排序
                    {
                        col_index++;
                        //cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                        //cmd.ExecuteNonQuery();

                        string sql = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                        Helper.ExecuteTransaction("syscolumns", sql, nID);
                    }

                    foreach (DataColumn c in tbl.Columns) // 旧列重排序
                    {
                        string colname = c.ColumnName;
                        if (!strColNameList.Contains(colname))
                        {
                            col_index++;
                            //cmd.CommandText = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                            //cmd.ExecuteNonQuery();
                            string sql = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", col_index, colname, tblName);
                            Helper.ExecuteTransaction("syscolumns", sql, nID);

                        }
                    }

                    // 恢复主键约束 列重排序可能改变了原先的主键约束
                    DataColumn[] primaryKeys = tbl.PrimaryKey;
                    if (primaryKeys.Length > 0) // 存在主键约束
                    {
                        string keyString = string.Empty;
                        foreach (DataColumn c in primaryKeys)
                        {
                            keyString = keyString + "[" + c.ColumnName + "]" + ",";
                        }

                        // 获取主键约束
                        
                        ////cmd.CommandText = string.Format("SELECT name FROM sysindexes WHERE id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tblName);
                        //string filter = string.Format("id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tblName);
                        string sql = string.Format("SELECT name FROM sysindexes WHERE id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tblName);

                        object constraint = Helper.ExecuteTransactionScalar("sysindexes", sql, nID, false);


                        // 删除主键约束
                        /*cmd.CommandText = string.Format("ALTER TABLE {0} DROP CONSTRAINT {1}", tblName, constraint);
                        cmd.ExecuteNonQuery();
                        */
                         
                        sql = string.Format("ALTER TABLE {0} DROP CONSTRAINT {1}", tblName, constraint);
                        Helper.ExecuteTransaction(tblName, sql, nID);

                        // 增加新的主键约束
                        /*
                        cmd.CommandText = string.Format("ALTER TABLE {0} ADD CONSTRAINT PK_{1} PRIMARY KEY({2})", tblName, tblName, keyString.TrimEnd(','));
                        cmd.ExecuteNonQuery();
                         */

                        sql = string.Format("ALTER TABLE {0} ADD CONSTRAINT PK_{1} PRIMARY KEY({2})", tblName, tblName, keyString.TrimEnd(','));
                        Helper.ExecuteTransaction(tblName, sql, nID);
                    }
                    /* remoting
                    cmd.CommandText = string.Format("SELECT OBJECTPROPERTY(OBJECT_ID('{0}'), 'TableHasIdentity')", tblName);
                    object executeResult = cmd.ExecuteScalar();
                    */

                    string sqlCmd = string.Format("SELECT OBJECTPROPERTY(OBJECT_ID('{0}'), 'TableHasIdentity')", tblName);
                    object executeResult = Helper.ExecuteTransactionScalar(tblName, sqlCmd, nID, false);

                    if (executeResult != null && executeResult.ToString() == "1") // 检查数据表是否有自动编号列
                    {
                        // 允许将值插入到自动编号列中
                        /* remoting
                        cmd.CommandText = string.Format("SET IDENTITY_INSERT {0} ON", tblName);
                        cmd.ExecuteNonQuery();
                         */

                        _sql = string.Format("SET IDENTITY_INSERT {0} ON", tblName);
                        Helper.ExecuteTransaction(tblName, _sql, nID);
                    }

                    // 重新添加表数据
                    foreach (DataRow r in tbl.Rows)
                    {
                        string newValue = "";
                        string columns = "";

                        //for (int i = 0; i < strColNames.Length; i++)
                        for (int i = 0; i < tbl.Columns.Count; i++)
                        {
                            string raw_colname = tbl.Columns[i].ColumnName;

                            string value = r[raw_colname].ToString().Replace("'", "''"); // 替换特殊字符
                            string columnName = raw_colname.Replace("]", "]]"); // 替换特殊字符
                            if (r[raw_colname] is Boolean) // 判断是否是布尔值
                            {
                                value = (bool)r[raw_colname] ? "1" : "0";
                            }

                            if (value == string.Empty) // 自动忽略空值，解决数字的空值转换的问题
                            {
                                continue;
                            }
                            else
                            {
                                columns += string.Format("[{0}], ", columnName);
                                newValue += string.Format("'{0}', ", value);
                            }
                        }

                        newValue = newValue.Trim().TrimEnd(',');
                        columns = columns.Trim().TrimEnd(',');
                        /*cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);*/
                        try
                        {
                           // cmd.ExecuteNonQuery();
                            string sql = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);
                            Helper.ExecuteTransaction(tblName, sql, nID);
                        }
                        catch (SqlException ex)
                        {
                            MessageBox.Show(ex.Message + "\r\n\r\n" + string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue));
                            throw ex;
                        }
                    }
                }
                else // 列顺序不变，只需添加行数据即可
                {
                    // 覆盖旧的数据行
                    if (overwriteRowList.Count > 0)
                    {
                        DataColumn[] keys = tbl.PrimaryKey;
                        foreach (DataRow r in overwriteRowList)
                        {
                            string setValue = "";
                            string keyValue = "";

                            foreach (string s in strColNames)
                            {
                                string columnName = s.Replace("]", "]]"); // 替换特殊字符
                                string value = r[s].ToString().Replace("'", "''"); // 替换特殊字符
                                if (r[s] is Boolean) // 判断是否是布尔值
                                {
                                    value = (bool)r[s] ? "1" : "0";
                                }

                                if (value == string.Empty) // 空值是特殊条件，需要特殊处理
                                {
                                    setValue += string.Format("[{0}] = NULL, ", columnName);
                                }
                                else
                                {
                                    setValue += string.Format("[{0}] = \'{1}\', ", columnName, value);
                                }
                            }
                            setValue = setValue.Trim().TrimEnd(',');

                            foreach (DataColumn c in keys)
                            {
                                string columnName = c.ColumnName.Replace("]", "]]"); // 替换特殊字符
                                string value = r[c.ColumnName].ToString().Replace("'", "''"); // 替换特殊字符
                                keyValue += string.Format("[{0}] = \'{1}\' AND ", columnName, value);
                            }
                            keyValue = keyValue.Substring(0, keyValue.Length - 5);

                            /*cmd.CommandText = string.Format("UPDATE {0} SET {1} WHERE {2}", tblName, setValue, keyValue);*/

                            try
                            {
                                //cmd.ExecuteNonQuery();
                                string sql = string.Format("UPDATE {0} SET {1} WHERE {2}", tblName, setValue, keyValue);
                                Helper.ExecuteTransaction(tblName, sql, nID);
                            }
                            catch (SqlException ex)
                            {
                                MessageBox.Show(ex.Message + "\r\n\r\n" + string.Format("UPDATE {0} SET {1} WHERE {2}", tblName, setValue, keyValue));
                                throw ex;
                            }
                        }
                    }

                    // 增加新数据行
                    foreach (DataRow r in tbl.Rows)
                    {
                        if (index < rowNumber) // 忽略旧的数据行
                        {
                            index++;
                            continue;
                        }

                        string newValue = "";
                        string columns = "";

                        for (int i = 0; i < strColNames.Length; i++)
                        {
                            string value = r[strColNames[i]].ToString().Replace("'", "''"); // 替换特殊字符
                            string columnName = strColNames[i].Replace("]", "]]"); // 替换特殊字符
                            if (r[strColNames[i]] is Boolean) // 判断是否是布尔值
                            {
                                value = (bool)r[strColNames[i]] ? "1" : "0";
                            }

                            if (value == string.Empty) // 自动忽略空值，解决数字的空值转换的问题
                            {
                                continue;
                            }
                            else
                            {
                                columns += string.Format("[{0}], ", columnName);
                                newValue += string.Format("'{0}', ", value);
                            }
                        }

                        newValue = newValue.Trim().TrimEnd(',');
                        columns = columns.Trim().TrimEnd(',');
                        /*cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);*/
                        try
                        {
                            //cmd.ExecuteNonQuery();
                            string sql = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue);
                            Helper.ExecuteTransaction(tblName, sql, nID);
                        }
                        catch (SqlException ex)
                        {
                            MessageBox.Show(ex.Message + "\r\n\r\n" + string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tblName, columns, newValue));
                            throw ex;
                        }
                    }
                }

                tbl.AcceptChanges();
                //transaction.Commit(); // 提交事务
                Helper.CommitTransaction(nID);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("数据更新时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                     "导入数据表", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                //transaction.Rollback(); // 事务回滚
                Helper.RollbackTransaction(nID);
                if (Program.EnableRemoting)
                {
                    proxy.AddRemotingTable(tblName);
                }
            }
            finally
            {
                // 禁止数据库系统标更新
                /* remoting
                cfg_cmd.CommandText = "sp_configure 'allow updates', '0'";
                cfg_cmd.ExecuteNonQuery();
                cfg_cmd.CommandText = "reconfigure with override";
                cfg_cmd.ExecuteNonQuery();
                */


                Helper.ExecuteCommand("sp_configure 'allow updates', '0'", Conn);
                Helper.ExecuteCommand("reconfigure with override", Conn);

                if (Conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    Conn.Close();
                }

                if (Program.EnableRemoting)
                {
                    proxy.AddRemotingTable(tblName);
                }
            }

            // 增加树结点
            tree.SelectedNode.Collapse();
            tree.SelectedNode.Nodes.Clear();
            tree.SelectedNode.Nodes.Add("无效属性");
            
            //////////////////////////////////////////////////////////////////////////
            // 更新内存 报告
            //////////////////////////////////////////////////////////////////////////
            AllTables[tblName] = tbl;
            string rpt = string.Format("成功导入数据,覆盖行：{0}  增加行：{1}", iOverwriteCount, iAddCount);
            log.Log(TimeLog.enumLogType.ltend, null, null, string.Format("导入文件名:{0}, 导入目标数据表:{1}, 导入参数:{2}, 备份名:{3}, 导入结果:{4}", strFile, tblName, importParams, backupName, rpt));
            log.Clear();
            MessageBox.Show(rpt);
            lua.Dispose();
        }

        private void DeleteTrashField(string tblName)
        {
            // 安静地删除___delete_me字段
            Conn.Open();
            try
            {
                //SqlCommand cmd = Conn.CreateCommand();
                //cmd.CommandText = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tblName, TRASH_FIELD);
                //cmd.ExecuteNonQuery();
                string sql = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tblName, TRASH_FIELD);
                Helper.ExecuteCommand(tblName, sql, Conn);
            }
            catch (Exception ex)
            {
                string err = string.Format("在删除表 {0} 中的字段 {1} 时发生错误。\r\n\r\n错误信息：{2}", tblName, TRASH_FIELD, ex.Message);
                //MessageBox.Show(err);
            }
            Conn.Close();
        }

        private void bNewField_Click(object sender, EventArgs e) // 新建字段
        {
            string newFieldName = "";
            NameInput frm = new NameInput();
            DialogResult result = frm.ShowDialog();
            if (result == DialogResult.OK)
            {
                newFieldName = frm.StringInput.Trim();
            }
            if (newFieldName == "")
            {
                return;
            }

            string tblName = this.tree.SelectedNode.Text.Trim();
            bool bSucNewField = false;
            Conn.Open();
            try
            {
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = string.Format("ALTER TABLE {0} ADD {1} varchar (3000) NULL", tblName, newFieldName);
                cmd.ExecuteNonQuery();
                */

                string sql = string.Format("ALTER TABLE {0} ADD [{1}] varchar (3000) NULL", tblName, newFieldName);
                Helper.ExecuteCommand(tblName, sql, Conn);
                 
                bSucNewField = true;
            }
            catch (Exception ex)
            {
                string str = string.Format("在添加字段时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                MessageBox.Show(str);
            }
            Conn.Close();

            if (bSucNewField)
            {
                //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tblName, Conn);
                DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);

                AllTables[tblName] = tbl;

                TreeNode new_node = this.tree.SelectedNode.Nodes.Add(newFieldName);
                new_node.Tag = newFieldName;
                new_node.ImageIndex = 4;
                new_node.SelectedImageIndex = 4;
                new_node.StateImageIndex = 7;

                this.tree.SelectedNode = new_node;
                ShowFieldInfo(new_node);
                DeleteTrashField(tblName); // 安静地删除___delete_me字段
            }
        }

        private void bRenameField_Click(object sender, EventArgs e) // 重命名字段
        {
            TreeNode theNode = this.tree.SelectedNode;
            if (!theNode.IsEditing)
            {
                this.tree.LabelEdit = true;
                theNode.BeginEdit();
            }
        }

        private void bDelField_Click(object sender, EventArgs e) // 删除字段
        {
            string tbl_Name = this.tree.SelectedNode.Parent.Text;
            string field_Name = this.tree.SelectedNode.Text;
            bool bSucDelField = false;
            Conn.Open();
            try
            {
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tbl_Name, field_Name);
                cmd.ExecuteNonQuery();
                 */
                string sql = string.Format("ALTER TABLE {0} DROP COLUMN {1}", tbl_Name, field_Name);

                Helper.ExecuteCommand(tbl_Name, sql, Conn);

                bSucDelField = true;
            }
            catch (Exception ex)
            {
                string err = string.Format("在删除表 {0} 中的字段 {1} 时发生错误。\r\n\r\n错误信息：{2}", tbl_Name, field_Name, ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();

            if (bSucDelField)
            {

                //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tbl_Name, Conn);

                DataTable tbl = Helper.GetDataTableProxy(tbl_Name, Conn);

                AllTables[tbl_Name] = tbl;

                this.tree.SelectedNode.Remove();
            }
        }

        private void bSetKey_Click(object sender, EventArgs e) // 设为主键
        {
            string tblName = this.tree.SelectedNode.Parent.Text.Trim();
            string fieldName = this.tree.SelectedNode.Text.Trim();
            string oldKeys = "";
            foreach (DataColumn col in ((DataTable)AllTables[tblName]).PrimaryKey)
            {
                oldKeys += col.ColumnName;
                oldKeys += ",";
            }

            if (oldKeys.IndexOf(fieldName) != -1)
            {
                MessageBox.Show("这个已经是主键了.");
                return;
            }

            bool bSucSetKey = false;
            Conn.Open();
            try
            {
                // 让这个字段不可为空
                string dataType = "";
                string len = "";
                switch (this.combo_data_type.SelectedIndex)
                {
                    case 0:
                        dataType = "decimal";
                        len = "(18)";
                        break;
                    case 1:
                        dataType = "varchar";
                        len = "(" + textBox_data_len.Text.Trim() + ")";
                        if (len == "()")
                        {
                            MessageBox.Show("请先指定字符串的长度.");
                            return;
                        }
                        break;
                    case 2:
                        dataType = "bit";
                        len = "";
                        break;
                    case 3:
                        dataType = "int";
                        len = "";
                        break;
                    default:
                        MessageBox.Show("请先设置数据类型.");
                        return;
                }

                // 测试主键长度总和是否超过 900
                int maxlength = 0;
                foreach (DataColumn col in ((DataTable)AllTables[tblName]).PrimaryKey)
                {
                    maxlength += col.MaxLength;
                }
                int length = 0;
                if (textBox_data_len.Text != null && textBox_data_len.Text != string.Empty)
                    length = Convert.ToInt32(textBox_data_len.Text);
                if (maxlength + length > 900)
                {
                    MessageBox.Show("作为主键的字段长度总和不能超过 900 ，请修改字段的类型或者长度后再试。", "失败", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    Conn.Close();
                    return;
                }

                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                string sql = string.Format("ALTER TABLE {0} ALTER COLUMN {1} {2}{3} NOT NULL",
                    tblName, fieldName, dataType, len);
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                string sql = string.Format("ALTER TABLE {0} ALTER COLUMN [{1}] {2}{3} NOT NULL",
                    tblName, fieldName, dataType, len);
                Helper.ExecuteCommand(tblName, sql, Conn);

                // 设置主键
               
                sql = string.Format("SELECT * INTO {0} FROM {1}", "tmp" + tblName, tblName);
                 /* remoting
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                Helper.ExecuteCommand(tblName, sql, Conn);

                sql = string.Format("DROP TABLE {0}", tblName);
                /* remoting
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                Helper.ExecuteCommand(tblName, sql, Conn);

                Helper.RenameDbTable("tmp" + tblName, tblName, Conn);

                sql = string.Format("ALTER TABLE {0} ADD PRIMARY KEY ({1})", tblName, oldKeys + "[" + fieldName + "]");
                /* remoting
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                Helper.ExecuteCommand(tblName, sql, Conn);
                bSucSetKey = true;
            }
            catch (Exception ex)
            {
                string err = string.Format("在将表 {0} 中的字段 {1} 设置为主键时发生错误。\r\n\r\n错误信息：{2}", tblName, fieldName, ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();

            if (bSucSetKey)
            {
                //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tblName, Conn);
                DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);

                AllTables[tblName] = tbl;

                this.tree.SelectedNode.StateImageIndex = 0;

                // 可以顺便把这个table中dbnull的项填成默认值
                if (MainForm.m_bModifyDbTableDefault)
                {
                    DialogResult dr = MessageBox.Show("已成功设置主键。\r\n\r\n你希望把该表中的空项填充成相应的默认值吗？\r\n\r\n推荐您选 < 是(Y) >。", "提示", MessageBoxButtons.YesNo);
                    if (dr == DialogResult.Yes)
                    {
                        try
                        {
                            Helper.FillDBNullWithDefault(ref tbl);
                            /*
                            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblName, Conn);
                            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                            int val = adp.Update(tbl);
                            tbl.AcceptChanges();
                             */

                            int val = Helper.SaveTable(tbl, "SELECT * FROM " + tblName, Conn);

                            MessageBox.Show(string.Format("成功填充默认值。\r\n\r\n受影响的行数：{0}。", val));
                        }
                        catch (Exception ex)
                        {
                            string err = string.Format("在为数据表 {0} 填充默认值时发生错误。\r\n\r\n错误信息为：{1}", tblName, ex.Message);
                            MessageBox.Show(err);
                        }
                    }
                }
            }
        }

        private void bUnsetKey_Click(object sender, EventArgs e) // 删除主键
        {
            string tblName = this.tree.SelectedNode.Parent.Text.Trim();
            string fieldName = this.tree.SelectedNode.Text.Trim();
            string oldKeys = "";
            foreach (DataColumn col in ((DataTable)AllTables[tblName]).PrimaryKey)
            {
                oldKeys += col.ColumnName;
                oldKeys += ",";
            }
            oldKeys = oldKeys.Substring(0, oldKeys.Length - 1);

            int iIndex = oldKeys.IndexOf(fieldName);
            if (iIndex < 0)
            {
                MessageBox.Show("这个字段并不是主键.");
                return;
            }

            oldKeys = oldKeys.Remove(iIndex, fieldName.Length);
            if (oldKeys.Length > 0)
            {
                oldKeys = oldKeys.Replace(",,", ",");

                if (oldKeys[0] == ',')
                    oldKeys = oldKeys.Substring(1);
                if (oldKeys[oldKeys.Length - 1] == ',')
                    oldKeys = oldKeys.Substring(0, oldKeys.Length - 1);
            }

            string newKeys = oldKeys;

            bool bString8000 = false;
            bool bSucUnsetKey = false;
            Conn.Open();
            try
            {

                SqlCommand cmd = Conn.CreateCommand();
                string sql = string.Format("SELECT * INTO {0} FROM {1}", "tmp" + tblName, tblName);
                /* remoting
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */
                Helper.ExecuteCommand(tblName, sql, Conn);

                sql = string.Format("DROP TABLE {0}", tblName);
                /*
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                Helper.ExecuteCommand(tblName, sql, Conn);

                Helper.RenameDbTable("tmp" + tblName, tblName, Conn);

                if (newKeys.Length > 0)
                {
                    sql = string.Format("ALTER TABLE {0} ADD PRIMARY KEY ({1})", tblName, newKeys);
                    /* remoting
                    cmd.CommandText = sql;
                    cmd.ExecuteNonQuery();
                    */
                    Helper.ExecuteCommand(tblName, sql, Conn);
                }

                // 让字段允许空
                string dataType = "";
                string len = "";
                switch (this.combo_data_type.SelectedIndex)
                {
                    case 0:
                        dataType = "decimal";
                        len = "(18)";
                        break;
                    case 1:
                        dataType = "varchar";
                        len = "(" + textBox_data_len.Text.Trim() + ")";
                        if (len == "()")
                        {
                            MessageBox.Show("请先指定字符串的长度.");
                            return;
                        }
                        break;
                    case 2:
                        dataType = "bit";
                        len = "";
                        break;
                    case 3:
                        dataType = "int";
                        len = "";
                        break;
                    default:
                        MessageBox.Show("请先设置数据类型.");
                        return;
                }
                if (dataType == "varchar" && len == "(3000)")
                    bString8000 = true;
                sql = string.Format("ALTER TABLE {0} ALTER COLUMN {1} {2}{3} NULL",
                    tblName, fieldName, dataType, len);
                /* remoting
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */
                Helper.ExecuteCommand(tblName, sql, Conn);

                bSucUnsetKey = true;
            }
            catch (Exception ex)
            {
                string err = string.Format("在将表 {0} 中的字段 {1} 删除主键时发生错误。\r\n\r\n错误信息：{2}", tblName, fieldName, ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();

            if (bSucUnsetKey)
            {
                //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tblName, Conn);

                DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);

                AllTables[tblName] = tbl;

                if (bString8000)
                    this.tree.SelectedNode.StateImageIndex = 7;
                else
                    this.tree.SelectedNode.StateImageIndex = -1;
            }
        }

        private void combo_data_type_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.tree.SelectedNode.Level == 0)
            {
                return;
            }

            string tblName = this.tree.SelectedNode.Parent.Text;
            DataTable tbl = ((DataTable)AllTables[tblName]);
            string fieldName = this.tree.SelectedNode.Text;
            Type type = tbl.Columns[fieldName].DataType;
            int len = tbl.Columns[fieldName].MaxLength;
           
            string sql = string.Format("SELECT * FROM syscolumns WHERE name='{0}' AND id=object_id('{1}')", fieldName, tblName);
            /* DataRow row = Helper.GetDataTable(sql, Conn).Rows[0];
             */

            
            DataTable table = Helper.GetDataTableWithSqlProxy("syscolumns", sql, Conn);
            DataRow row = table.Rows[0];


            int prec = Convert.ToInt32(row["prec"]);
            int scale = -1;
            if (row["scale"] == DBNull.Value)
                scale = 0;
            else
                scale = Convert.ToInt32(row["scale"]);
            bool bMatch = ((type.Name == "Decimal" && this.combo_data_type.SelectedIndex == 0)
                        || (type.Name == "String"  && this.combo_data_type.SelectedIndex == 1)
                        || (type.Name == "Boolean" && this.combo_data_type.SelectedIndex == 2)
                        || (type.Name == "Int32"   && this.combo_data_type.SelectedIndex == 3));

            switch (this.combo_data_type.SelectedIndex)
            {
                case 0: // 数值(浮点数)
                    this.textBox_data_len.Text = "";
                    this.textBox_data_len.Enabled = false;
                    this.textBox_Xiaoshu_len.Enabled = true;
                    this.textBox_Zhengshu_len.Enabled = true;
                    if (bMatch)
                    {
                        this.textBox_Xiaoshu_len.Text = Convert.ToString(scale);
                        this.textBox_Zhengshu_len.Text = Convert.ToString(prec);
                    }
                    else
                    {
                        this.textBox_Xiaoshu_len.Text = "2";
                        this.textBox_Zhengshu_len.Text = "10";
                    }
                    break;
                case 1: // 字符串
                    this.textBox_Xiaoshu_len.Text = "";
                    this.textBox_Xiaoshu_len.Enabled = false;
                    this.textBox_Zhengshu_len.Text = "";
                    this.textBox_Zhengshu_len.Enabled = false;
                    this.textBox_data_len.Enabled = true;
                    if (bMatch)
                    {
                        this.textBox_data_len.Text = Convert.ToString(len);
                    }
                    else
                    {
                        this.textBox_data_len.Text = "3000";
                    }
                    break;
                case 2: // 真假值
                    this.textBox_data_len.Text = "";
                    this.textBox_data_len.Enabled = false;
                    this.textBox_Xiaoshu_len.Text = "";
                    this.textBox_Xiaoshu_len.Enabled = false;
                    this.textBox_Zhengshu_len.Text = "";
                    this.textBox_Zhengshu_len.Enabled = false;
                    break;
                case 3: // 整型
                    this.textBox_data_len.Text = "";
                    this.textBox_data_len.Enabled = false;
                    this.textBox_Xiaoshu_len.Text = "";
                    this.textBox_Xiaoshu_len.Enabled = false;
                    this.textBox_Zhengshu_len.Text = "";
                    this.textBox_Zhengshu_len.Enabled = false;
                    break;
                default:
                    break;
            }
        }

        private void bEmptyTbl_Click(object sender, EventArgs e)
        {
            string tblName = this.tree.SelectedNode.Text.Trim();
            string confirm = string.Format("你确定要清空整个表 {0} 的数据吗?", tblName);
            DialogResult result = MessageBox.Show(confirm, "警告", MessageBoxButtons.YesNo);
            if (result == DialogResult.No)
                return;

            Conn.Open();
            try
            {
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                string sql = "DELETE FROM " + tblName;
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                 */

                Helper.ExecuteCommand(tblName, "DELETE FROM " + tblName, Conn);
            }
            catch (Exception ex)
            {
                string err = string.Format("在将表 {0} 清空数据时发生错误。\r\n\r\n错误信息：{1}", tblName, ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();
        }

        private void checkHasDefault_CheckedChanged(object sender, EventArgs e)
        {
            bool bChecked = ((DevComponents.DotNetBar.Controls.CheckBoxX)sender).Checked;
            if (!bChecked)
            {
                comboExportRule.SelectedIndex = 1;
            }
        }

        private void cFieldNodeClick_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// 预览数据表信息
        /// </summary>
        /// <param name="sender">消息发送者</param>
        /// <param name="e">消息</param>
        private void buttonItem1_Click(object sender, EventArgs e)
        {
            TreeNode node = tree.SelectedNode;
            if (node == null)
            {
                return;
            }
            string tableName = node.Text;
            DBTools.PreviewForm pf = new DBTools.PreviewForm(m_conn, tableName);
            DialogResult result = pf.ShowDialog();
        }

        /// <summary>
        /// 树结点即将展开
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void tree_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            TreeNode currentNode = e.Node;
            if(currentNode.FirstNode.Text == "无效属性") // 需要重新载入
            {
                ReloadAttributes(currentNode);
            }
        }

        private void buttonItem2_Click(object sender, EventArgs e) // 按与默认值相比，填充NULL，全表更新
        {
            Conn.Open();
            try
            {
                string tbl_Name = this.tree.SelectedNode.Text;

                //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + tbl_Name, Conn);

                DataTable tbl = Helper.GetDataTableProxy(tbl_Name, Conn);

                string strKeyRestriction = "";
                foreach (DataColumn col in tbl.PrimaryKey)
                {
                    strKeyRestriction += col.ColumnName;
                    strKeyRestriction += " <> 0 AND";
                }
                strKeyRestriction = strKeyRestriction.Substring(0, strKeyRestriction.Length - 4);
                SqlCommand cmd = Conn.CreateCommand();
                foreach (DataColumn col in tbl.Columns)
                {
                    string sql = string.Format("UPDATE {0} SET {1} = NULL WHERE ({2} = (SELECT TOP 1 {3} FROM {4})) AND ({5})",
                        tbl_Name, col.ColumnName, col.ColumnName, col.ColumnName, tbl_Name, strKeyRestriction);
                    /* remoting
                    cmd.CommandText = sql;
                    cmd.ExecuteNonQuery();
                    */

                    Helper.ExecuteCommand(tbl_Name, sql, Conn);
                }
                   
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(string.Format("错误：{0}", ex.Message));
            }
            Conn.Close();
            MessageBox.Show("成功");
        }

        /// <summary>
        /// 配置导出过滤字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bConfigExcludeField_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null)
            {
                List<string> fieldList = new List<string>();
                
                foreach (TreeNode fieldNode in currentNode.Nodes)
                {
                    fieldList.Add(fieldNode.Text);
                }

                ExportFieldConfigForm eForm = new ExportFieldConfigForm(fieldList, textBox_Exclude_Field.Text);

                if (eForm.ShowDialog() == DialogResult.OK)
                {
                    textBox_Exclude_Field.Text = eForm.ExcludeFieldString;
                }
            }
        }

        private void buttonItemModColOrder_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = tree.SelectedNode;
            string[] columns = new string[currentNode.Nodes.Count];
            int index = 0;
            foreach (TreeNode node in currentNode.Nodes)
            {
                columns[index] = node.Text;
                index++;
            }

            ColumnOrder frm = new ColumnOrder(columns, currentNode.Text);
            frm.ShowDialog(this);
            if (frm.Changed)
            {
                SetColumnsOrder(currentNode.Text, frm.Result, null);
                tree.Nodes.Clear();
                DBTableEditor_Load(null, null);
            }
        }

        private void SetColumnsOrder(string tablename, string[] columns)
        {
            DataTable table = Helper.GetDataTableTopRowProxy(tablename, m_conn);
            string sqlColumn = string.Empty;
            foreach (string column in columns)
            {
                sqlColumn += "[" + column + "],";
            }
            sqlColumn = sqlColumn.Substring(0, sqlColumn.Length - 1);

            string newtable = tablename + "_new";
            //1.拷贝表数据
            string sql1 = string.Format("select {0} into {1} from {2}", sqlColumn, newtable, tablename);
            Helper.ExecuteCommand(tablename, sql1, m_conn);

            //2.获取并设置主键约束
            if (table.PrimaryKey.Length > 0)
            {
                string sqlKey = string.Empty;
                foreach (DataColumn column in table.PrimaryKey)
                {
                    sqlKey += column.ColumnName + ",";
                }
                sqlKey = sqlKey.Substring(0, sqlKey.Length - 1);
                string sql2 = string.Format("SELECT name FROM sysindexes WHERE id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tablename);
                object constraint = Helper.ExecuteScalar("sysindexes", sql2, m_conn, false);

                sql2 = string.Format("ALTER TABLE {0} ADD CONSTRAINT PK_{1} PRIMARY KEY({2})", newtable, newtable, sqlKey);
                Helper.ExecuteCommand(newtable, sql2, m_conn);
            }

            //3.自动编号列
            string sqlCmd = string.Format("SELECT OBJECTPROPERTY(OBJECT_ID('{0}'), 'TableHasIdentity')", tablename);
            object executeResult = Helper.ExecuteScalar(tablename, sqlCmd, m_conn, false);
            if (executeResult != null && executeResult.ToString() == "1")
            {
                string sql3 = string.Format("SET IDENTITY_INSERT {0} ON", newtable);
                Helper.ExecuteCommand(tablename, sql3, m_conn);
            }
        }
        
        private void SetColumnsOrder(string tablename, string[] columns, object flag)
        {
            DataTable table = Helper.GetDataTableProxy(tablename, m_conn);

            Helper.ExecuteCommand("sp_configure 'allow updates', '1'", m_conn);
            Helper.ExecuteCommand("reconfigure with override", m_conn);
            try
            {
                Helper.ExecuteCommand(tablename, "delete from " + tablename, m_conn);

                //1.修改使用syscolumns
                int index = 1;
                foreach (string column in columns)
                {
                    string sql1 = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", index + 1000, column, tablename);
                    Helper.ExecuteCommand("syscolumns", sql1, m_conn);
                    index++;
                }

                index = 1;
                foreach (string column in columns)
                {
                    string sql1 = string.Format("UPDATE syscolumns SET colid = '{0}' WHERE name = '{1}' AND id = object_id('{2}')", index, column, tablename);
                    Helper.ExecuteCommand("syscolumns", sql1, m_conn);
                    index++;
                }

                //2.获取并设置主键约束
                if (table.PrimaryKey.Length > 0)
                {
                    string sqlKey = string.Empty;
                    foreach (DataColumn column in table.PrimaryKey)
                    {
                        sqlKey += column.ColumnName + ",";
                    }
                    sqlKey = sqlKey.Substring(0, sqlKey.Length - 1);
                    string sql2 = string.Format("SELECT name FROM sysindexes WHERE id = OBJECT_ID('{0}') AND name LIKE 'PK%'", tablename);
                    object constraint = Helper.ExecuteScalar("sysindexes", sql2, m_conn, false);

                    sql2 = string.Format("ALTER TABLE {0} DROP CONSTRAINT {1}", tablename, constraint);
                    Helper.ExecuteCommand(tablename, sql2, m_conn);

                    sql2 = string.Format("ALTER TABLE {0} ADD CONSTRAINT PK_{1} PRIMARY KEY({2})", tablename, tablename, sqlKey);
                    Helper.ExecuteCommand(tablename, sql2, m_conn);
                }

                //3.自动编号列
                string sqlCmd = string.Format("SELECT OBJECTPROPERTY(OBJECT_ID('{0}'), 'TableHasIdentity')", tablename);
                object executeResult = Helper.ExecuteScalar(tablename, sqlCmd, m_conn, false);
                if (executeResult != null && executeResult.ToString() == "1")
                {
                    string sql3 = string.Format("SET IDENTITY_INSERT {0} ON", tablename);
                    Helper.ExecuteCommand(tablename, sql3, m_conn);
                }

                //4.重填数据
                foreach (DataRow row in table.Rows)
                {
                    string newValue = "";
                    string _columns = "";

                    for (int i = 0; i < table.Columns.Count; i++)
                    {
                        string raw_colname = table.Columns[i].ColumnName;

                        string value = row[raw_colname].ToString().Replace("'", "''");
                        string columnName = raw_colname.Replace("]", "]]");
                        if (row[raw_colname] is Boolean)
                        {
                            value = (bool)row[raw_colname] ? "1" : "0";
                        }

                        if (value == string.Empty)
                        {
                            continue;
                        }
                        else
                        {
                            _columns += string.Format("[{0}], ", columnName);
                            newValue += string.Format("'{0}', ", value);
                        }
                    }

                    newValue = newValue.Trim().TrimEnd(',');
                    _columns = _columns.Trim().TrimEnd(',');

                    string sql = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", tablename, _columns, newValue);
                    Helper.ExecuteCommand(tablename, sql, m_conn);

                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                Helper.ExecuteCommand("sp_configure 'allow updates', '0'", m_conn);
                Helper.ExecuteCommand("reconfigure with override", m_conn);
            }
        }

    }
    public class ImportInfoHelper
    {
        public ArrayList Fields;
        public string[] Values;
        public string ResultInfo; //输出信息
    };
}