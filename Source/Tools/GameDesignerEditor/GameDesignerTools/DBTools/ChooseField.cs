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
    public partial class ChooseField : Form
    {
        SqlConnection conn; // sql连接
        List<string> columnNames = new List<string>(); // 选择列名链表
        List<string> idList; // 存在的ID链表
        List<string> existList = new List<string>(); // 存在的列名链表
        List<string> list; // 所有列名链表 

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="list">所有列名链表</param>
        /// <param name="existList">存在的列名链表</param>
        public ChooseField(SqlConnection conn, List<string> list, List<string> idList)
        {
            this.conn = conn;
            this.list = list;
            this.idList = idList;
            InitializeComponent();
            FillExistList();
            InitData();
        }

        /// <summary>
        /// 选择列名链表属性
        /// </summary>
        public List<string> ColumnNames
        {
            get
            {
                return columnNames;
            }
        }

        /// <summary>
        /// 填充存在列名链表
        /// </summary>
        private void FillExistList()
        {
            //SqlCommand cmd;
            SqlDataReader reader = null;

            if(conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }
            foreach (string s in idList)
            {
                try
                {
                    string sql = string.Format("SELECT {0} FROM {1} WHERE {2} = {3}", "fieldname", "sys_meta_info", "fieldid",
                        s);
                    string filter = string.Format("{0} = {1}", "fieldid", s);
                    /* remoting
                    cmd = new SqlCommand(sql, conn);
                    reader = cmd.ExecuteReader();
                    reader.Read();
                    existList.Add(reader[0].ToString().Trim());
                    */

                    DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);
                    existList.Add(table.Rows[0]["fieldname"].ToString().Trim());
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("获取当前结点已存属性时产生Sql异常: " + ex.ToString());
                }
                finally
                {
                    if (reader != null) // reader是否已关闭
                    {
                        reader.Close();
                    }
                }
            }
            if(conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            foreach(string s in list)
            {
                if(existList.Contains(s)) // 该属性已存在结点的子属性中
                {
                    listBox2.Items.Add(s);
                }
                else 
                {
                    listBox1.Items.Add(s);
                }
            }
        }

        /// <summary>
        /// 添加内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            List<object> list = new List<object>(); 
            ListBox.SelectedObjectCollection items = listBox1.SelectedItems;

            if (items != null && items.Count != 0)
            {
                foreach(object o in items)
                {
                    list.Add(o);
                }
            }

            foreach(object o in list)
            {
                listBox2.Items.Add(o);
                listBox2.SelectedItem = o;
                listBox1.Items.Remove(o);
            }
        }

        /// <summary>
        /// 删除内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            List<object> list = new List<object>();
            ListBox.SelectedObjectCollection items = listBox2.SelectedItems;

            if (items != null && items.Count != 0)
            {
                foreach (object o in items)
                {
                    list.Add(o);
                }
            }

            foreach (object o in list)
            {
                listBox1.Items.Add(o);
                listBox1.SelectedItem = o;
                listBox2.Items.Remove(o);
            }
        }

        /// <summary>
        /// 添加所有内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            foreach(object o in listBox1.Items)
            {
                listBox2.Items.Add(o);
            }
            listBox1.Items.Clear();
        }

        /// <summary>
        /// 删除所有内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            foreach (object o in listBox2.Items)
            {
                listBox1.Items.Add(o);
            }
            listBox2.Items.Clear();
        }

        /// <summary>
        /// 确定操作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX5_Click(object sender, EventArgs e)
        {
            foreach(object o in listBox2.Items)
            {
                columnNames.Add(o.ToString());
            }
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 取消操作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX6_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}