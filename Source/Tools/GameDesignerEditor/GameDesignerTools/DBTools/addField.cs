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
    public partial class addField : Form
    {
        public MetainfoEditorForm m_parent;
        public paraMetaInfo m_paramMetaInfo;
        public SqlDataAdapter m_adp;
        public addField()
        {
            InitializeComponent();
        }

        public addField(string tbl_name, string type, MetainfoEditorForm parent, paraMetaInfo param)
        {
            InitializeComponent();          
            m_parent = parent;
            m_paramMetaInfo = param;
            edit_TableName.Text = tbl_name;
            m_adp = new SqlDataAdapter("SELECT * FROM " + edit_TableName.Text, m_parent.m_conn);
            combo_DataType.Items.AddRange(new object[] { "文本",
                                                         "浮点数"
                                                       });
            switch (type)
            {
                default:
                    combo_DataType.Text = "文本";
                    break;
            }
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            string strType;
            string strLength = "3000";
            switch (combo_DataType.Text.Trim())
            {
                default:
                    strType = "varchar";
                    break;
            }

            // 添加字段
            m_parent.m_conn.Open();
            try
            {               
                SqlCommand cmd = m_parent.m_conn.CreateCommand();
                cmd.CommandText = string.Format("alter table {0} add {1} {2} ({3}) NULL",
                    edit_TableName.Text.Trim(), edit_FieldName.Text.Trim(), strType, strLength);
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand(edit_TableName.Text.Trim(), cmd.CommandText, m_parent.m_conn);
            }
            catch (Exception ex)
            {
                string str = string.Format("在添加字段时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                System.Windows.Forms.MessageBox.Show(str);
            }
            m_parent.m_conn.Close();

            // 添加一条metaInfo
           
            string sqltxt = "SELECT * FROM sys_meta_info WHERE tablename = '" + edit_TableName.Text + "'";
             /* remoting
              * SqlDataAdapter adp = new SqlDataAdapter(sqltxt, m_parent.m_conn);
            System.Data.DataSet DS = new System.Data.DataSet();
            adp.Fill(DS);
            System.Data.DataTable tbl = DS.Tables[0];
             */

            string filter = "tablename = '" + edit_TableName.Text + "'";
            DataTable tbl = Helper.GetDataTableProxy("sys_meta_info", filter, null, m_parent.m_conn);
            System.Data.DataRow dataRow = tbl.NewRow();
            dataRow.BeginEdit();
            dataRow["tablename"] = edit_TableName.Text.Trim();
            dataRow["fieldname"] = edit_FieldName.Text.Trim();
            dataRow["fieldcnname"] = m_paramMetaInfo.m_strFieldNameCN;
            dataRow["editortype"] = m_paramMetaInfo.m_strEditType;
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

                // 更新类别名下拉框
                m_parent.FillCatNames(edit_TableName.Text);
            }
            catch (Exception ex)
            {
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
                System.Data.DataSet DS = new System.Data.DataSet();
                m_adp.Fill(DS);
                System.Data.DataTable tbl = DS.Tables[0];

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
    }
}
