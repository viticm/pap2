using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameDesingerTools.DBTools
{
    public partial class PreviewForm : Form
    {
        SqlConnection conn; // 数据库连接
        string m_TableName;
        DataTable m_tbl;

        public PreviewForm(SqlConnection conn, string tableName)
        {
            this.conn = conn;
            InitializeComponent();
            InitData(tableName);
            UpdateNullRowColor();
        }

        private void UpdateNullRowColor()
        {
//             foreach (DataGridViewRow gridrow in this.dataGridView1.Rows)
//             {
//                 DataRow datarow = m_tbl.Rows[gridrow.Index];
//                 if (datarow != null)
//                 {
//                     gridrow.DefaultCellStyle.BackColor = Color.Red;
//                     
//                 }
//             }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData(string tableName)
        {
            m_TableName = tableName;
            m_tbl = null;
            try
            {
                string sql = string.Format("SELECT * FROM {0}", tableName);
                //m_tbl = Helper.GetDataTable(sql, conn);
                m_tbl = Helper.GetDataTableProxy(tableName, conn);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在初始化表预览数据时产生sql异常: " + ex.ToString());
                return;
            }
            dataGridView1.DataSource = m_tbl;
            AutoEnlargeSize();
        }

        /// <summary>
        /// 当列数量较少的时候自动增大列宽
        /// </summary>
        private void AutoEnlargeSize()
        {
            int columnNumber = dataGridView1.Columns.Count;
            int columnWidth = dataGridView1.Columns[0].Width;
            int width = dataGridView1.Width;
            if(columnWidth * columnNumber < width)
            {
                dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            }
        }

        private void buttonX1_Click(object sender, EventArgs e) // 删除行
        {
            try
            {
                DataGridViewRow row = dataGridView1.CurrentRow;
                if (row != null)
                {
                    // 找出这行的key
                    string strKey = string.Empty;
                    object[] keys = new object[m_tbl.PrimaryKey.Length];
                    int i = 0;
                    foreach (DataColumn col in m_tbl.PrimaryKey)
                    {
                        keys[i] = row.Cells[col.ColumnName].Value;
                        strKey += keys[i].ToString();
                        strKey += ",";
                        i++;
                    }
                    strKey = strKey.Substring(0, strKey.Length - 1);
                    string strQ = string.Format("你确定要删除键值为 {0} 的行吗？", strKey);
                    DialogResult dr = MessageBox.Show(strQ, "确认删除", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    if (dr == DialogResult.Yes)
                    {
                        // 删除之
                        DataRow rowToDel = m_tbl.Rows.Find(keys);
                        if (rowToDel != null)
                        {
                            rowToDel.Delete();
                            string sql = string.Format("SELECT * FROM {0}", m_TableName);
                            /*
                            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                            adp.DeleteCommand = cmdBuilder.GetDeleteCommand();
                            int val = adp.Update(m_tbl);*/

                            int val = Helper.SaveTable(m_tbl, sql, conn);

                            m_tbl.AcceptChanges();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                string err = string.Format("删除失败。错误信息为：{0}", ex.Message);
                MessageBox.Show(err);
            }
        }

        private void buttonX2_Click(object sender, EventArgs e) // 关闭
        {
            this.Dispose();
        }
    }
}