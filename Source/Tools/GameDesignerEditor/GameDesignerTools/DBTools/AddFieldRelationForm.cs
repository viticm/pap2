using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace GameDesingerTools
{
    public partial class AddFieldRelationForm : Form
    {
        private SqlConnection conn;
        private DataTable metaDataTable;
        private string tableName;
        private List<string> fieldNameList;
        private Hashtable columnNameTable = new Hashtable();
        //private bool multiSelect;
        private List<string> allTableNameList = new List<string>();
        private List<string> allFieldNameList = new List<string>();

        public AddFieldRelationForm(SqlConnection conn, string tableName, List<string> fieldNameList, DataTable metaDataTable)
        {
            this.conn = conn;
            this.tableName = tableName;
            this.fieldNameList = fieldNameList;
            this.metaDataTable = metaDataTable;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 数据表名
        /// </summary>
        public string TableName
        {
            get
            {
                return tableNameBox.SelectedItem as string;
            }
        }

        /// <summary>
        /// 字段名链表
        /// </summary>
        public List<string> FieldNameList
        {
            get
            {
                return fieldNameList;
            }
        }

        /// <summary>
        /// 获取字段显示名
        /// </summary>
        /// <param name="fieldName">字段名</param>
        /// <returns>显示名</returns>
        private string GetFieldDisplayName(string tableName, string fieldName)
        {
            string displayName = fieldName;

            DataRow[] rows = metaDataTable.Select(string.Format("tablename = '{0}' AND fieldname = '{1}'",
                                                                tableName, fieldName));
            if (rows.Length > 0)
            {
                string chineseName = rows[0]["fieldcnname"].ToString().Trim();

                if (!string.IsNullOrEmpty(chineseName))
                {
                    displayName = string.Format("{0} ({1})", chineseName, fieldName);
                }
            }

            return displayName;
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            fieldNameBox.SelectionMode = SelectionMode.MultiSimple;
            tableNameBox.Items.Clear();

            if (conn.State == ConnectionState.Open)
            {
                conn.Open();
            }

            //DataTable dataTable = Helper.GetDataTable("SELECT name FROM sysobjects WHERE (xtype = 'u') ORDER BY name", conn);
            DataTable dataTable = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name", conn);
            foreach (DataRow dataRow in dataTable.Rows)
            {
                string tableNameString = dataRow["name"].ToString();

                if (!tableNameString.StartsWith("sys_"))
                {
                    tableNameBox.Items.Add(tableNameString);
                    allTableNameList.Add(tableNameString);
                }
            }
        
            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            if (tableName != "")
            {
                tableNameBox.SelectedItem = tableName;

                foreach (string s in fieldNameList)
                {
                    fieldNameBox.SelectedItems.Add(s);
                }
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            if (tableNameBox.SelectedItem != null || fieldNameBox.SelectedItems.Count > 0)
            {
                fieldNameList = new List<string>();

                foreach (string s in fieldNameBox.SelectedItems)
                {
                    fieldNameList.Add(s);
                }

                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("表名或字段名不能为空！", "字段选择", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 切换数据表
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tableNameBox_SelectedIndexChanged(object sender, EventArgs e)
        {            
            string tableName = tableNameBox.SelectedItem as string;
            List<string> columnNameList = columnNameTable[tableName] as List<string>;

            if (columnNameList == null)
            {
                columnNameList = new List<string>();
                
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT TOP 1 * FROM {0}", tableName);
                //DataTable dataTable = Helper.GetDataTable(sqlString, conn);
                DataTable dataTable = Helper.GetDataTableTopRowProxy(tableName, conn);

                foreach (DataColumn column in dataTable.Columns)
                {
                    columnNameList.Add(GetFieldDisplayName(tableName, column.ColumnName));
                }
            
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }                
            }

            fieldNameBox.Items.Clear();

            foreach (string s in columnNameList)
            {
                fieldNameBox.Items.Add(s);
                allFieldNameList.Add(s);
            }
        }

        /// <summary>
        /// 数据表名过滤
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tableNameFilter_TextChanged(object sender, EventArgs e)
        {
            tableNameBox.Items.Clear();

            if (tableNameFilter.Text != "")
            {
                foreach (string s in allTableNameList)
                {
                    if (s.ToLower().Contains(tableNameFilter.Text.ToLower()))
                    {
                        tableNameBox.Items.Add(s);
                    }
                }
            }
            else
            {
                foreach (string s in allTableNameList)
                {
                    tableNameBox.Items.Add(s);
                }
            }
        }

        /// <summary>
        /// 字段名过滤
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void fieldNameFilter_TextChanged(object sender, EventArgs e)
        {
            fieldNameBox.Items.Clear();

            if (fieldNameFilter.Text != "")
            {
                foreach (string s in allFieldNameList)
                {
                    if (s.ToLower().Contains(fieldNameFilter.Text.ToLower()))
                    {
                        fieldNameBox.Items.Add(s);
                    }
                }
            }
            else
            {
                foreach (string s in allFieldNameList)
                {
                    fieldNameBox.Items.Add(s);
                }
            }
        }                                
    }
}