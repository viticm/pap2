using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DevComponents.DotNetBar.Controls;

namespace VarManager
{
    public partial class SelectForm : Form
    {
        private SqlConnection conn; // sql连接
        private string conditionFields; // 过滤条件列
        private string conditionValues; // 过滤条件值
        private int varIndex; // 变量的偏移序号
        private string varString; // 变量的显示字符串

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="conditonFields">过滤条件类</param>
        /// <param name="conditonValues">过滤条件值</param>
        public SelectForm(SqlConnection conn, string conditionFields, string conditionValues)
        {
            this.conn = conn;
            this.conditionFields = conditionFields;
            this.conditionValues = conditionValues;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 变量的偏移序号
        /// </summary>
        public int VarIndex
        {
            get
            {
                return varIndex;
            }
        }

        /// <summary>
        /// 变量的显示字符串
        /// </summary>
        public string VarString
        {
            get
            {
                return varString;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化列数据
            DataGridViewTextBoxColumn idColumn = new DataGridViewTextBoxColumn();
            idColumn.Name = "idColumn";
            idColumn.HeaderText = "变量ID";
            idColumn.ReadOnly = true;
            DataGridViewTextBoxColumn offsetColumn = new DataGridViewTextBoxColumn();
            offsetColumn.Name = "offsetColumn";
            offsetColumn.HeaderText = "变量偏移";
            offsetColumn.ReadOnly = true;
            DataGridViewTextBoxColumn nameColumn = new DataGridViewTextBoxColumn();
            nameColumn.Name = "nameColumn";
            nameColumn.HeaderText = "变量名称";
            nameColumn.ReadOnly = true;
            DataGridViewTextBoxColumn descriptionColumn = new DataGridViewTextBoxColumn();
            descriptionColumn.Name = "descriptionColumn";
            descriptionColumn.HeaderText = "变量描述";
            descriptionColumn.ReadOnly = true;
            DataGridViewTextBoxColumn userColumn = new DataGridViewTextBoxColumn();
            userColumn.Name = "userColumn";
            userColumn.HeaderText = "申请人";
            userColumn.ReadOnly = true;

            dataGridViewX1.Columns.Add(idColumn);
            dataGridViewX1.Columns.Add(offsetColumn);
            dataGridViewX1.Columns.Add(nameColumn);
            dataGridViewX1.Columns.Add(descriptionColumn);
            dataGridViewX1.Columns.Add(userColumn);

            // 初始化行数据
            DataTable table = null;
            string[] fieldsArray = conditionFields.Split(new char[]{','});
            string[] valuesArray = conditionValues.Split(new char[]{','});
            string conditonString = "";
            for(int i = 0; i < fieldsArray.Length; i++)
            {
                conditonString += string.Format("[{0}] = '{1}' AND ", fieldsArray[i], valuesArray[i]);
            }
            conditonString = conditonString.Substring(0, conditonString.Length - 5);

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT * FROM VarData WHERE " + conditonString;
                table = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取变量信息时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            int index = 0;
            foreach(DataRow row in table.Rows)
            {
                dataGridViewX1.Rows.Add(1);
                DataGridViewRow newRow = dataGridViewX1.Rows[index];
                newRow.Cells["idColumn"].Value = row["ID"];
                newRow.Cells["offsetColumn"].Value = row["StartIndex"];
                newRow.Cells["nameColumn"].Value = row["Name"];
                newRow.Cells["descriptionColumn"].Value = row["Description"];
                newRow.Cells["userColumn"].Value = row["UserName"];
                index++;
            }
        }

        /// <summary>
        /// 重新加载数据
        /// </summary>
        private void ReloadData()
        {
            // 初始化行数据
            DataTable table = null;
            string[] fieldsArray = conditionFields.Split(new char[] { ',' });
            string[] valuesArray = conditionValues.Split(new char[] { ',' });
            string conditonString = "";
            for (int i = 0; i < fieldsArray.Length; i++)
            {
                conditonString += string.Format("[{0}] = '{1}' AND ", fieldsArray[i], valuesArray[i]);
            }
            conditonString = conditonString.Substring(0, conditonString.Length - 5);

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT * FROM VarData WHERE " + conditonString;
                table = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取变量信息时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            int index = 0;
            foreach (DataRow row in table.Rows)
            {
                dataGridViewX1.Rows.Add(1);
                DataGridViewRow newRow = dataGridViewX1.Rows[index];
                newRow.Cells["idColumn"].Value = row["ID"];
                newRow.Cells["offsetColumn"].Value = row["StartIndex"];
                newRow.Cells["nameColumn"].Value = row["Name"];
                newRow.Cells["descriptionColumn"].Value = row["Description"];
                newRow.Cells["userColumn"].Value = row["UserName"];
                index++;
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(dataGridViewX1.SelectedCells.Count > 0)
            {
                DataGridViewRow selectedRow = dataGridViewX1.SelectedCells[0].OwningRow;
                varIndex = int.Parse(selectedRow.Cells["offsetColumn"].EditedFormattedValue.ToString());
                varString = string.Format("{0},{1}", selectedRow.Cells["nameColumn"].EditedFormattedValue,
                    selectedRow.Cells["offsetColumn"].EditedFormattedValue);
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("没有选择自定义变量！", "变量选择", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }           
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 管理变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            MainForm mForm = new MainForm(conn);
            DialogResult result = mForm.ShowDialog();            
            ReloadData();            
        }
    }
}