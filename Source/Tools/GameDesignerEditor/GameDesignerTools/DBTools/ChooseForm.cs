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
    public partial class ChooseForm : Form
    {
        string tableName; // 主表名称
        SqlConnection conn; // sql连接

        /// <summary>
        /// 构造函数
        /// </summary>
        public ChooseForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 主表名称属性
        /// </summary>
        public string TableName
        {
            get
            {
                return tableName;
            }
        }

        /// <summary>
        /// 填充列表数据
        /// </summary>
        private void InitData()
        {
            /* remoting
            string sql = "SELECT name FROM sysobjects WHERE xtype = 'u' ORDER BY name";
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while(reader.Read())
                {
                    string tableName = reader[0].ToString().Trim();
                    if (tableName != "" && !tableName.StartsWith("sys"))
                    {
                        comboBoxEx1.Items.Add(tableName);
                    }
                }
            }
            */

            try
            {
                DataTable table = Helper.GetDataTableProxy("sysobjects", "xtype = 'u'", "name", conn);
                foreach (DataRow row in table.Rows)
                {
                    string tableName = row["name"] as string;
                    if (tableName != "" && !tableName.StartsWith("sys"))
                    {
                        comboBoxEx1.Items.Add(tableName);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("获取表名信息时产生异常: " + ex.ToString());
            }
            finally
            {
                /* remoting
                reader.Close();
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
                 * */
            }
        }

        /// <summary>
        ///  用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string text = comboBoxEx1.Text;
            if(text == "")
            {
                MessageBox.Show("主表名不能为空!");
                return;
            }
            tableName = text;
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}