using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace AIEnumEditor
{
    public partial class AIChartSelector : Form
    {
        private SqlConnection conn; // sql连接
        private string lastID; // 旧值
        private string aiChartID; // AI图的id
        private string aiChartPath; // AI图的路径

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="lastID">旧值</param>
        public AIChartSelector(SqlConnection conn, string lastID)
        {
            this.conn = conn;
            this.lastID = lastID;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// AI图的ID
        /// </summary>
        public string AIChartID
        {
            get
            {
                return aiChartID;
            }
        }

        /// <summary>
        /// AI图的路径
        /// </summary>
        public string AIChartPath
        {
            get
            {
                return aiChartPath;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            DataTable table = null;

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT ID, FullPath FROM Diagram WHERE [Class] = 'AI图' AND [IsFolder] = 'False'";
                table = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
            	MessageBox.Show("在读取AI图数据时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            advTree1.Nodes.Clear();
            DevComponents.AdvTree.NodeCollection nodes;
            DevComponents.AdvTree.Node newNode;
            DevComponents.AdvTree.Node selectedNode = null;
            bool find = false;

            foreach(DataRow row in table.Rows)
            {
                string id = row["ID"].ToString();
                string fullPath = row["FullPath"].ToString();
                string[] datas = fullPath.Split(new char[] { '\\' });
                nodes = advTree1.Nodes;

                for(int i = 0; i < datas.Length; i++)
                {
                    string data = datas[i];
                    find = false;

                    foreach (DevComponents.AdvTree.Node node in nodes)
                    {
                        if (node.Text == data)
                        {
                            nodes = node.Nodes;
                            find = true;
                            break;
                        }
                    }

                    if (!find)
                    {
                        newNode = new DevComponents.AdvTree.Node();
                        newNode.Text = data;
                        nodes.Add(newNode);
                        nodes = newNode.Nodes;

                        if (i == datas.Length - 1)
                        {
                            newNode.Tag = id;
                            if (id == lastID)
                            {
                                selectedNode = newNode;
                            }
                        }
                    }
                }
            }

            // 选中旧值代表的树结点
            advTree1.SelectedNode = selectedNode;
        }

        /// <summary>
        /// 获取树结点的路径
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <returns>树结点的路径</returns>
        private string GetNodePath(DevComponents.AdvTree.Node node)
        {
            string path = node.Text;
            DevComponents.AdvTree.Node parentNode = node.Parent;
            while(parentNode != null)
            {
                path = string.Format("{0}/", parentNode.Text) + path;
                parentNode = parentNode.Parent;
            }

            return path;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            DataTable tb = null;
            
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            tb = ds.Tables[0];

            return tb;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            DevComponents.AdvTree.Node selectedNode = advTree1.SelectedNode;
            if(selectedNode != null && selectedNode.Tag != null)
            {
                aiChartID = selectedNode.Tag.ToString();
                aiChartPath = GetNodePath(selectedNode);                
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("无效的选择！", "选择AI图", MessageBoxButtons.OK,
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
    }
}