using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DevComponents.DotNetBar;

namespace ChooseItem
{
    public partial class OperateTree : Form
    {
        private PanelDockContainer PDC;
        private TreeViewEventHandler afterSelect;
        private TreeViewCancelEventHandler beforeSelect;
        private NodeLabelEditEventHandler beforeLabelEdit;
        private NodeLabelEditEventHandler afterLabelEdit;

        private SqlConnection conn; // sql连接
        private string dropTable = "droprate";
        private string itemTable = "dropitem";
        private bool autoResetItemName = false; // 是否自动重设道具的名称

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="tree">树</param>
        public OperateTree(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
        }

        /// <summary>
        /// 掉落率表属性
        /// </summary>
        public string DropTable
        {
            set
            {
                dropTable = value;
            }
        }

        /// <summary>
        /// 掉落物品表属性
        /// </summary>
        public string ItemTable
        {
            set
            {
                itemTable = value;
            }
        }

        /// <summary>
        /// 是否自动重设道具的名称属性
        /// </summary>
        public bool AutoResetItemName
        {
            set
            {
                autoResetItemName = value;
            }
        }

        /// <summary>
        /// 对树进行操作
        /// </summary>
        public void Operate(PanelDockContainer pdc, TreeViewEventHandler afterselect, TreeViewCancelEventHandler beforeselect, NodeLabelEditEventHandler beforelabeledit, NodeLabelEditEventHandler afterlabeledit)
        {
            //tree.ContextMenuStrip = this.contextMenuStrip1;
            PDC = pdc;
            this.afterSelect = afterselect;
            this.beforeSelect = beforeselect;
            this.beforeLabelEdit = beforelabeledit;
            this.afterLabelEdit = afterlabeledit;
        }

        /// <summary>
        /// 构造新树
        /// </summary>
        /// <returns>新树</returns>
        private TreeView ReloadTree()
        {
            string checkFileName = ""; // 出错检查的文件名
            string checkID = ""; // 出错检查的数据行id 

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", dropTable);
                DataTable dTable = GetDataTable(sqlString);

                sqlString = string.Format("SELECT * FROM {0}", itemTable);
                DataTable iTable = GetDataTable(sqlString);

                foreach(DataRow r in dTable.Rows)
                {
                    string fileName = r["filename"].ToString();
                    string id = r["id"].ToString();
                    string nodeName = "无效物品";
                    bool found = false;
                    TreeNode currentNode = null;
                    checkFileName = fileName;
                    checkID = id;

                    foreach(TreeNode node1 in treeView1.Nodes)
                    {
                        if(node1.Text == fileName)
                        {
                            currentNode = node1;
                            found = true;
                            break;
                        }
                    }

                    if(!found)
                    {
                        currentNode = treeView1.Nodes.Add(fileName);
                        currentNode.Tag = new object[]{"-2"};
                    }
               
                    DataRow row;

                    if((r["Genre"] is DBNull) || (r["Genre"] == null) ||
                        (r["Detail"] is DBNull) || (r["Detail"] == null)||
                        (r["Particular"] is DBNull) || (r["Particular"] == null)||
                        (r["Level"] is DBNull) || (r["Level"] == null)) // 无效的掉落物品
                    {
                        row = null;
                    }
                    else
                    {
                        string[] keys = new string[4];
                        keys[0] = r["Genre"].ToString();
                        keys[1] = r["Detail"].ToString();
                        keys[2] = r["Particular"].ToString();
                        keys[3] = r["Level"].ToString();
                        row = iTable.Rows.Find(keys);
                    }
             
                    if(row != null) // 找到匹配的物品
                    {
                        nodeName = row["Name"].ToString();
                        if (autoResetItemName) // 需要重设道具的名称
                        {
                            if((r["Name"] is DBNull) || (r["Name"] == null) || (r["Name"].ToString() == "")) // 原道具名称为空
                            {
                                ReSetItemName(r["id"].ToString(), nodeName);
                            }
                        }
                    }
                    else // 没有找到匹配的物品
                    {
                        if(autoResetItemName) // 自动设置物品名称
                        {
                            if (!(r["Name"] is DBNull) && (r["Name"] != null) && (r["Name"].ToString() != "")) // 原道具名称不为空
                            {
                                nodeName = r["Name"].ToString();
                            }
                        }
                    }

                    TreeNode addNode = currentNode.Nodes.Add(nodeName);
                    addNode.Tag = new object[]{id};
                }
            
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在重新构造掉落率树时产生异常，请检查文件{0}中的id为{1}数据行\n\n", checkFileName, checkID) + 
                    ex.ToString());
                return null;
            }
            return treeView1;
        }

        /// <summary>
        /// 重设物品名称
        /// </summary>
        /// <param name="id">掉落物品对应的id</param>
        private void ReSetItemName(string id, string itemName)
        {
            try
            {
                string sqlString = string.Format("UPDATE {0} SET Name = '{1}' WHERE id = '{2}'", dropTable, itemName, id);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在更新掉落率表的物品名称信息时产生sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable table = ds.Tables[0];
            return table; 
        }

        /// <summary>
        /// 获取插件树
        /// </summary>
        /// <returns>插件树</returns>
        public TreeView LoadTreePlugin()
        {
            TreeView tree = ReloadTree();
            PDC.Controls.Add(tree);
            tree.Dock = DockStyle.Fill;

            tree.ContextMenuStrip = this.contextMenuStrip1;
            tree.AfterSelect += afterSelect;
            tree.BeforeSelect += beforeSelect;
            tree.BeforeLabelEdit += beforeLabelEdit;
            tree.AfterLabelEdit += afterLabelEdit;

            return tree;
        }

        /// <summary>
        /// 批量添加
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void multipleAddToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if(currentNode != null && currentNode.Level == 1)
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                MultiAddForm mForm = new MultiAddForm(conn, currentNode, itemTable, dropTable, autoResetItemName);
                DialogResult result = mForm.ShowDialog();
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 计算特定文件的掉落总值总值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void computeTotalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                TreeNode currentNode = treeView1.SelectedNode;
                if(currentNode != null)
                {
                    int sumRandRate = 0;

                    if(currentNode.Parent != null)
                    {
                        currentNode = currentNode.Parent;
                    }

                    string fileName = currentNode.Text;

                    string sql = string.Format("SELECT RandRate FROM {0} WHERE {1} = \'{2}\'",
                        dropTable, "filename", fileName);
                    SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                    adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                    DataSet ds = new DataSet();
                    adp.Fill(ds);
                    DataTable dTable = ds.Tables[0];

                    foreach(DataRow r in dTable.Rows)
                    {
                        if(!(r["RandRate"] is DBNull))
                        {
                            int value = int.Parse(r["RandRate"].ToString());
                            if(value > 0)
                            {
                                sumRandRate += value;
                            }
                        }
                    }

                    Clipboard.SetDataObject(sumRandRate.ToString(), true);
                    MessageBox.Show(string.Format("文件{0}的掉落总值为{1}!\n\n该值已经复制入剪贴板!", fileName, sumRandRate.ToString()), 
                        "掉落总值计算", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("在计算掉落总值时产生异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }
    }
}