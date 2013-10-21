using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

using DevComponents.AdvTree;

namespace GameDesingerTools
{
    public partial class FieldRelationSearchForm : Form
    {
        private SqlConnection conn;
        private string displayInfo;        
        private string sourceFieldValue;
        private Hashtable infoTable;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="displayInfo">显示信息</param>
        /// <param name="sourceFieldValue">字段值</param>
        /// <param name="infoTable">数据表</param>
        public FieldRelationSearchForm(SqlConnection conn, string displayInfo, string sourceFieldValue, Hashtable infoTable)
        {
            this.conn = conn;
            this.displayInfo = displayInfo;
            this.sourceFieldValue = sourceFieldValue;
            this.infoTable = infoTable;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            DataGridViewTextBoxColumn tableNameColumn = new DataGridViewTextBoxColumn();
            tableNameColumn.HeaderText = "数据表名";
            tableNameColumn.Name = "TableName";

            DataGridViewTextBoxColumn keyValueColumn = new DataGridViewTextBoxColumn();
            keyValueColumn.HeaderText = "主键值";
            keyValueColumn.Name = "KeyValue";

            DataGridViewTextBoxColumn fieldNameColumn = new DataGridViewTextBoxColumn();
            fieldNameColumn.HeaderText = "字段名";
            fieldNameColumn.Name = "FieldName";

            resultView.Columns.Add(tableNameColumn);
            resultView.Columns.Add(keyValueColumn);
            resultView.Columns.Add(fieldNameColumn);

            fieldValueBox.Text = sourceFieldValue;
            ShowSearchResult(sourceFieldValue);
        }

        /// <summary>
        /// 显示查找结果
        /// </summary>
        /// <param name="value">显示查找结果</param>
        private void ShowSearchResult(string value)
        {
            resultView.Rows.Clear();

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            foreach (string tableName in infoTable.Keys)
            {
                List<string> fieldList = infoTable[tableName] as List<string>;
                StringBuilder conditionString = new StringBuilder();

                foreach (string s in fieldList)
                {
                    conditionString.Append(string.Format("[{0}] = '{1}' OR ", s, value));
                }

                conditionString.Remove(conditionString.Length - 4, 4);

                string sqlString = string.Format("SELECT * FROM {0} WHERE {1}", tableName, conditionString.ToString());
               // DataTable dataTable = Helper.GetDataTable(sqlString, conn);
                DataTable dataTable = Helper.GetDataTableProxy(tableName, conditionString.ToString(), null, conn);
                foreach (DataRow dataRow in dataTable.Rows)
                {
                    StringBuilder fieldString = new StringBuilder();
                    StringBuilder keyString = new StringBuilder();

                    foreach (DataColumn dataColumn in dataTable.PrimaryKey)
                    {
                        keyString.Append(string.Format("{0} = {1}, ", dataColumn.ColumnName, dataRow[dataColumn].ToString()));
                    }

                    keyString.Remove(keyString.Length - 2, 2);

                    foreach (string s in fieldList)
                    {
                        if (dataRow[s].ToString() == value)
                        {
                            fieldString.Append(string.Format("{0}, ", s));
                        }
                    }

                    fieldString.Remove(fieldString.Length - 2, 2);

                    resultView.Rows.Add(1);
                    DataGridViewRow newRow = resultView.Rows[resultView.Rows.Count - 1];
                    newRow.Cells["TableName"].Value = tableName;
                    newRow.Cells["KeyValue"].Value = keyString.ToString();
                    newRow.Cells["FieldName"].Value = fieldString;
                }
            }           

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            this.Text = string.Format("关联关系查找 共有{0}条匹配记录", resultView.Rows.Count.ToString());
            labelX2.Text = displayInfo;
        }

        /// <summary>
        /// 开始查找
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSearch_Click(object sender, EventArgs e)
        {
            string fieldValue = fieldValueBox.Text;

            if (fieldValue != "")
            {
                ShowSearchResult(fieldValue);
            }
            else
            {
                MessageBox.Show("字段值不能为空！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            
        }
    }
}