using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace Item_clear
{
    public partial class PreviewForm : Form
    {
        private SqlConnection Conn;
        private DataTable m_tbl;

        public PreviewForm(SqlConnection conn, string sql)
        {
            this.Conn = conn;
            InitializeComponent();
            InitData(sql);
            AutoEnlargeSize();
            UpdateNullRowColor();
        }

        private void UpdateNullRowColor()
        {
        }

        private void InitData(string strSql)
        {
            m_tbl = null;
            try
            {
                m_tbl = GetDataTable(strSql);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在初始化表预览数据时产生sql异常: " + ex.ToString());
                return;
            }
            dataGridView1.DataSource = m_tbl;
        }

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

        private void buttonX1_Click(object sender, EventArgs e) // 删除所选
        {
            ArrayList lsIdToDel = new ArrayList();

            foreach (DataGridViewRow row in dataGridView1.Rows)
            {
                if (row.Selected)
                {
                    object v = row.Cells["ItemID"].Value;
                    if (v == null) continue;
                    int itemID = Convert.ToInt32(v);
                    lsIdToDel.Add(itemID);
                }
            }

            foreach (int i in lsIdToDel)
            {
                DataRow RowToDel = m_tbl.Rows.Find(i);
                if (RowToDel != null)
                    RowToDel.Delete();
            }

            int nAffectedRowCount = 0;
            try
            {
                string sql = "SELECT * FROM tbl_icon_item";
                SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.DeleteCommand = cmdBuilder.GetDeleteCommand();
                nAffectedRowCount = adp.Update(m_tbl);
                m_tbl.AcceptChanges();
                MessageBox.Show("数据删除成功，删除行数: " + nAffectedRowCount.ToString() + " 。",
                    "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("数据更新时产生异常: " + ex.ToString() + "\r\n\r\n新增数据将不被保存",
                    "删除数据", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void buttonX3_Click(object sender, EventArgs e) // 全选
        {
            dataGridView1.SelectAll();
            dataGridView1.Rows[dataGridView1.Rows.Count - 1].Selected = false;
        }

        private void buttonX2_Click(object sender, EventArgs e) // 关闭
        {
            this.Dispose();
        }

        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }

        private void buttonX4_Click(object sender, EventArgs e) // 先保存原item表 to 表bak
        {
            Conn.Open();

            SqlCommand cmd = Conn.CreateCommand();
            string sql = string.Format("SELECT * INTO tbl_icon_item_bak FROM tbl_icon_item");
            cmd.CommandText = sql;
            cmd.ExecuteNonQuery();
            Conn.Close();
        }
    }
}