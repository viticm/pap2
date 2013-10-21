using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameLuaEditor
{
    public partial class RecordForm : Form
    {
        private SqlConnection conn; // sql连接
        private ImageList imageList; // 树图标链表
        private string id; // 选中的id
        private string path; // 选中的树结点路径

        public RecordForm(SqlConnection conn, ImageList imageList)
        {
            this.conn = conn;
            this.imageList = imageList;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 选中的id属性
        /// </summary>
        public string ID
        {
            get
            {
                return id;
            }
        }

        /// <summary>
        /// 选中的树结点的路径属性
        /// </summary>
        public string Path
        {
            get
            {
                return path;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            treeView1.Nodes.Clear();
            treeView1.ImageList = imageList;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }


                string sqlString = "SELECT DISTINCT id, path FROM sys_script_log WHERE id NOT IN " +
                    "(SELECT id FROM sys_script_script)";
                DataTable table = GetDataTable(sqlString);

                foreach (DataRow r in table.Rows)
                {
                    string path = r["path"].ToString();
                    string id = r["id"].ToString();

                    string[] paths = path.Split('\\');

                    TreeNodeCollection currentNodes = treeView1.Nodes;
                    TreeNode currentNode = null;
                    bool findNode = false;

                    foreach (string s in paths)
                    {
                        findNode = false;
                        if (s != "")
                        {
                            foreach (TreeNode node1 in currentNodes)
                            {
                                if (node1.Text == s)
                                {
                                    findNode = true;
                                    currentNodes = node1.Nodes;
                                    break;
                                }
                            }

                            if (!findNode)
                            {
                                currentNode = currentNodes.Add(s);
                                currentNode.Tag = id;
                                currentNodes = currentNode.Nodes;
                                if (s.EndsWith(".lua"))
                                {
                                    currentNode.ImageIndex = 1;
                                    currentNode.SelectedImageIndex = 1;
                                }
                                else
                                {
                                    currentNode.ImageIndex = 0;
                                    currentNode.SelectedImageIndex = 0;
                                }
                            }
                        }
                    }
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在初始化删除历史数据时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tb = ds.Tables[0];
            return tb;
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if(currentNode != null)
            {
                id = currentNode.Tag as string;
                path = currentNode.FullPath;
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("必须先选择要读取的历史记录!");
            }
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