using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameDesingerTools
{
    public partial class AddVirtualField : Form
    {
        public ModelEdit m_parent;
        public paraMetaInfo m_paramMetaInfo;
        public SqlDataAdapter m_adp;
        private int m_oldSelectItemIndex = -1;
        public AddVirtualField()
        {
            InitializeComponent();
        }

        public AddVirtualField(string tablename, ModelEdit parent, paraMetaInfo param)
        {
            InitializeComponent();
            TableName_comboBox.Left = edit_TableName.Left;
            TableName_comboBox.Top = edit_TableName.Top;
            FieldName_comboBox.Left = edit_FieldName.Left;
            FieldName_comboBox.Top = edit_FieldName.Top;

            m_parent = parent;
            m_paramMetaInfo = param;
            
            if (param.m_nFieldType != 1)
            {
                TableName_comboBox.Visible = false;
                TableName_comboBox.Enabled = false;
                edit_TableName.Visible = true;
                edit_TableName.Enabled = false;
                edit_TableName.Text = tablename;
                FieldName_comboBox.Visible = true;
                FieldName_comboBox.Enabled = true;
                edit_FieldName.Visible = false;
                edit_FieldName.Enabled = false;
                FieldName_comboBox.Items.Clear();

                //DataTable table = m_parent.QueryData("SELECT * FROM " + tablename);
                DataTable table = Helper.GetDataTableProxy(tablename, m_parent.m_conn);

                FieldName_comboBox.Items.Clear();
                for (int i = 0; i < table.Columns.Count; i++)
                {
                    string strColumnName = table.Columns[i].ColumnName;
                    FieldName_comboBox.Items.AddRange(new object[] { strColumnName });
                }
                // 设置光标位置
                FieldName_comboBox.Select();
            }
            else
            {
                TableName_comboBox.Visible = true;
                TableName_comboBox.Enabled = true;
                edit_TableName.Visible = false;
                edit_TableName.Enabled = false;
                FieldName_comboBox.Visible = false;
                FieldName_comboBox.Enabled = false;
                edit_FieldName.Visible = true;
                edit_FieldName.Enabled = true;
                FieldName_comboBox.Items.Clear();
                
                //DataTable table = m_parent.QueryData("SELECT name FROM sysobjects WHERE xtype = 'u' order by name asc");
                DataTable table = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name asc", m_parent.m_conn);
                foreach (DataRow row in table.Rows)
                {
                    TableName_comboBox.Items.Add(row["name"].ToString().Trim());
                }
                // 设置光标位置
                TableName_comboBox.Select();
            }
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            //if (edit_TableName.Text == null || edit_TableName.Text.Trim() == "")
            //{
            //    MessageBox.Show("必须指定虚拟字段下子字段所属的表名", "提示", MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2); ;
            //    return;
            //}

            //if (edit_FieldName.Text == null || edit_FieldName.Text.Trim() == "")
            //{
            //    MessageBox.Show("必须指定虚拟字段名", "提示", MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2); ;
            //    return;
            //}

            //if (edit_FieldCNName.Text == null || edit_FieldCNName.Text.Trim() == "")
            //{
            //    edit_FieldCNName.Text = edit_FieldName.Text;
            //}

            // 添加一条metaInfo
            string sqltxt = "SELECT * FROM sys_meta_info WHERE tablename = '" + edit_TableName.Text + "'";
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter(sqltxt, m_parent.m_conn);
            System.Data.DataSet DS = new System.Data.DataSet();
            adp.Fill(DS);
            System.Data.DataTable tbl = DS.Tables[0];
            */

            DataTable tbl = Helper.GetDataTableProxy("sys_meta_info", "tablename = '" + edit_TableName.Text + "'", null, m_parent.m_conn);
             
            System.Data.DataRow dataRow = tbl.NewRow();
            dataRow.BeginEdit();
            dataRow["fieldtype"] = m_paramMetaInfo.m_nFieldType;
            dataRow["parentfield"] = m_paramMetaInfo.m_nParentField;
            dataRow["subtablefilter"] = m_paramMetaInfo.m_strSubTableFilter;
            if (m_paramMetaInfo.m_nFieldType != 1)
            { 
                dataRow["tablename"] = edit_TableName.Text.Trim();
                dataRow["fieldname"] = FieldName_comboBox.Text.Trim();                
            }
            else
            {
                string strTableName = TableName_comboBox.Text.Trim();
                if (strTableName == "")
                {
                    MessageBox.Show("请指定一张表名!");
                    return;
                }
                dataRow["tablename"] = strTableName;
                dataRow["fieldname"] = edit_FieldName.Text.Trim();  

                // 虚拟字段不能与它所在同层其他字段同名
                if (m_paramMetaInfo.m_nParentField > 0)
                {
                    string strSQL =
                        string.Format("SELECT * FROM sys_meta_info WHERE parentfield = {0} and fieldname = '{1}'",
                        m_paramMetaInfo.m_nParentField, edit_FieldName.Text.Trim());

                    string filter = string.Format("parentfield = {0} and fieldname = '{1}'",
                        m_paramMetaInfo.m_nParentField, edit_FieldName.Text.Trim());

                    //DataTable table = m_parent.QueryData(strSQL);

                    DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_parent.m_conn);
                    if (table != null && table.Rows.Count > 0)
                    {
                        MessageBox.Show("虚拟字段英文名与该字段所在同一级某个字段同名，请换个名字!");
                        return;
                    }

                    DataTable metaInfo = m_paramMetaInfo.m_DicMetaInfo;
                    DataRow parentRow = metaInfo.Rows.Find(m_paramMetaInfo.m_nParentField);
                    if (parentRow == null)
                        return;

                    if (parentRow["fieldtype"].ToString().Trim() == "1")
                    {
                        string strParentTable = parentRow["tablename"].ToString().Trim();
                        bool bRetCode = m_parent.CheckFieldExist(strParentTable, edit_FieldName.Text.Trim());
                        if (bRetCode)
                        {
                            MessageBox.Show("虚拟字段英文名与父虚拟字段对应的表中的某个字段重名，请换个名字!");
                            return;
                        }
                    }
                }
                else // 标签下的虚拟字段检查
                {
                    string strSQL =
                        string.Format("SELECT * FROM sys_meta_info WHERE parentfield = 0 and modtabid = {0} and fieldname = '{1}'",
                        m_paramMetaInfo.m_nModTableID, edit_FieldName.Text.Trim());
                    //DataTable table = m_parent.QueryData(strSQL);
                    string filter = string.Format("parentfield = 0 and modtabid = {0} and fieldname = '{1}'",
                        m_paramMetaInfo.m_nModTableID, edit_FieldName.Text.Trim());
                    DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_parent.m_conn);
                    if (table != null && table.Rows.Count > 0)
                    {
                        MessageBox.Show("虚拟字段英文名与该字段所在同一级某个字段同名，请换个名字!");
                        return;
                    }

                    strSQL = string.Format("SELECT * FROM sys_modl_tab_def WHERE modtabid = {0}", m_paramMetaInfo.m_nModTableID);
                    filter = string.Format("modtabid = {0}", m_paramMetaInfo.m_nModTableID);

                    //table = m_parent.QueryData(strSQL);
                    table = Helper.GetDataTableProxy("sys_modl_tab_def", filter, null, m_parent.m_conn);

                    string tablename = table.Rows[0]["tablename"].ToString().Trim();
                    bool bRetCode = m_parent.CheckFieldExist(tablename, edit_FieldName.Text.Trim());
                    if (bRetCode)
                    {
                        MessageBox.Show("虚拟字段英文名与主表中某个字段同名，请换个名字!");
                        return;
                    }
                }
            }

            dataRow["modtabid"] = m_paramMetaInfo.m_nModTableID;
            dataRow["fieldcnname"] = edit_FieldCNName.Text.Trim();
            dataRow["editortype"] = "text"; // 默认为文本类型
            dataRow["catname"] = m_paramMetaInfo.m_strCatName;
            dataRow["orderno"] = "0";
            dataRow["description"] = m_paramMetaInfo.m_strDescription;
            dataRow["listtable"] = m_paramMetaInfo.m_strListTable;
            dataRow["listcondition"] = m_paramMetaInfo.m_strCondition;
            dataRow["keyfield"] = m_paramMetaInfo.m_strKeyField;
            dataRow["listfield"] = m_paramMetaInfo.m_strListField;
            dataRow["listvalues"] = m_paramMetaInfo.m_strTextCombo;
            dataRow["relativepath"] = m_paramMetaInfo.m_strRelativePath;
            dataRow["visible"] = m_paramMetaInfo.m_bVisible ? 1 : 0;
            dataRow["readonly"] = m_paramMetaInfo.m_bReadOnly ? 1 : 0;
            tbl.Rows.Add(dataRow);
            dataRow.EndEdit();
            try
            {
                /* remoting
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.InsertCommand = cmdBuilder.GetInsertCommand();
                int val = adp.Update(tbl);
                tbl.AcceptChanges();
                */

                int val = Helper.SaveTable(tbl, sqltxt, m_parent.m_conn);

                m_parent.m_conn.Open();
                /* remoting
                SqlCommand sqlcmd = m_parent.m_conn.CreateCommand();

                sqlcmd.CommandText = "SELECT max(fieldid) FROM sys_meta_info";
                int nID = (int)sqlcmd.ExecuteScalar();
                */

                string sql = "SELECT max(fieldid) FROM sys_meta_info";
                int nID = (int)Helper.ExecuteScalar("sys_meta_info", sql, m_parent.m_conn, false);

                 
                dataRow["fieldid"] = nID;
                val = Helper.SaveTable(tbl, sqltxt, m_parent.m_conn);
                tbl.AcceptChanges();
                m_parent.m_conn.Close();

                // 更新类别名下拉框
                //m_parent.FillCatNames(edit_TableName.Text);
                TreeNode newNode = null;
                string strFieldName = dataRow["fieldname"].ToString().Trim();
                if (m_paramMetaInfo.m_nFieldType == 1)
                    newNode = m_parent.AddTreeNode(m_parent.treeView.SelectedNode, edit_FieldCNName.Text + " [" + strFieldName + "]", nID, "virtualfield");
                else
                    newNode = m_parent.AddTreeNode(m_parent.treeView.SelectedNode, edit_FieldCNName.Text + " [" + strFieldName + "]", nID, "text");
                
                m_parent.LoadDicMetaInfoTable();
                m_parent.SetTreeViewSelectNode(newNode);
            }
            catch (Exception ex)
            {
                m_parent.m_conn.Close();
                MessageBox.Show(ex.Message);
            }

            DialogResult = DialogResult.OK;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void buttonCheck_Click(object sender, EventArgs e)
        {
            try
            {
                /* remoting
                m_adp = new SqlDataAdapter("SELECT * FROM " + edit_TableName.Text, m_parent.m_conn);
                System.Data.DataSet DS = new System.Data.DataSet();
                m_adp.Fill(DS);
                System.Data.DataTable tbl = DS.Tables[0];
                */

                DataTable tbl = Helper.GetDataTableProxy(edit_TableName.Text, m_parent.m_conn);


                foreach (System.Data.DataColumn col in tbl.Columns)
                {
                    string colname = col.ColumnName.ToString().Trim().ToLower();
                    if (edit_FieldName.Text.ToLower() == colname)
                    {
                        System.Windows.Forms.MessageBox.Show("该字段已经被使用了！");
                        edit_FieldName.SelectAll();
                        return;
                    }
                }
                System.Windows.Forms.MessageBox.Show("该字段还未使用，可以添加。");
            }
            catch (Exception ex)
            {
                string str = string.Format("在检查字段名时发生错误，请检查表名，字段名等。\r\n\r\n错误信息：{0}", ex.Message);
                System.Windows.Forms.MessageBox.Show(str);
            }
        }

        private void FieldName_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (FieldName_comboBox.SelectedIndex >= 0)
            {
                if ((m_oldSelectItemIndex < 0 || edit_FieldCNName.Text.Trim().Length <= 0) || 
                    FieldName_comboBox.Items[m_oldSelectItemIndex].ToString().Trim().ToLower() == edit_FieldCNName.Text.Trim().ToLower())
                    edit_FieldCNName.Text = FieldName_comboBox.SelectedItem.ToString();
                m_oldSelectItemIndex = FieldName_comboBox.SelectedIndex;
            }
        }
    }
}

namespace GameDesingerTools
{
    public class paraMetaInfo
    {
        public int m_nFieldType;
        public int m_nParentField;
        public string m_strSubTableFilter;
        public string m_strTableName;
        public string m_strFieldName;
        public int m_nModTableID;
        public string m_strFieldNameCN;
        public string m_strCatName;
        public string m_strDescription;
        public string m_strEditType;
        public string m_strRelativePath; // 当editType为filename时有用
        public string m_strListTable;    // 当editType为lookupcombo时有用
        public string m_strKeyField;
        public string m_strListField;
        public string m_strCondition;
        public string m_strTextCombo;    // 当editType为textcombo时有用
        public bool m_bVisible = true;
        public bool m_bReadOnly;
        public DataTable m_DicMetaInfo;
        public paraMetaInfo()
        {
        }
    }
}