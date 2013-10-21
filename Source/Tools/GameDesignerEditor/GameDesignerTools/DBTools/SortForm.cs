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
    public partial class SortForm : Form
    {
        private TreeNode node; // 树结点
        private SqlConnection conn; // sql连接
        private ImageList imageList;
        List<string> cat = new List<string>(); // 分类链表
        private TreeNode OldDragOver = null;

        public SortForm(SqlConnection conn, TreeNode node, ImageList imageList)
        {
            this.conn = conn;
            this.node = node;
            this.imageList = imageList;
            InitializeComponent();
            treeView1.ImageList = imageList;
            InitData();
        }

        /// <summary>
        /// 初始化树结点数据
        /// </summary>
        private void InitData()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                foreach(TreeNode node1 in node.Nodes)
                {
                    InitNode(node1);
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("填充分类字段排序树结点时产生Sql异常: " + ex.ToString());
            }
            finally
            {            
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
            if(node.ImageIndex == 7) // 虚拟字段无法新建类
            {
                buttonX3.Enabled = false;
            }
        }

        /// <summary>
        /// 初始化结点
        /// </summary>
        /// <param name="node">树结点</param>
        private void InitNode(TreeNode node)
        {
            string fieldid = node.Tag.ToString().Trim();
            string sql = string.Format("SELECT {0}, {1} FROM {2} WHERE {3} = {4}", "fieldname", "catname", "sys_meta_info", 
                "fieldid", fieldid);
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {              
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while (reader.Read())
                {
                    string fieldName = node.Text;
                    string catName = reader[1].ToString().Trim();
                    if(catName == "") // 分类未命名
                    {
                        catName = "未分类";
                    }
                    if (!cat.Contains(catName)) // 分类名称未保存在链表中
                    {
                        TreeNode node1 = treeView1.Nodes.Add(catName);
                        node1.ImageIndex = 0;
                        node1.SelectedImageIndex = 0;
                        TreeNode node2 = node1.Nodes.Add(fieldName);
                        node2.Tag = fieldid;
                        node2.ImageIndex = node.ImageIndex;
                        node2.SelectedImageIndex = node.SelectedImageIndex;
                        cat.Add(catName);
                    }
                    else // 分类名称已存在
                    {
                        foreach (TreeNode node1 in treeView1.Nodes)
                        {
                            if (node1.Text == catName)
                            {
                                TreeNode node2 = node1.Nodes.Add(fieldName);
                                node2.Tag = fieldid;
                                node2.ImageIndex = node.ImageIndex;
                                node2.SelectedImageIndex = node.SelectedImageIndex;
                                break;
                            }
                        }
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("填充分类字段排序树结点时产生Sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 关闭reader
                {
                    reader.Close();
                }
            }
        }

        /// <summary>
        /// 保存结点顺序信息
        /// </summary>
        /// <param name="fieldid">属性id</param>
        /// <param name="orderNo">序号</param>
        private void SaveData(string fieldid, string orderNo)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE {3} = {4}", "sys_meta_info", "orderno", orderNo, 
                    "fieldid", fieldid);
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand("sys_meta_info", cmd.CommandText, conn);
            }
            catch(SqlException ex)
            {
                MessageBox.Show("保存自定义字段顺序信息时产生Sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 保存结点的类型
        /// </summary>
        /// <param name="fieldid">属性id</param>
        /// <param name="catName">类型</param>
        private void SaveType(string fieldid, string catName)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE {3} = {4}", "sys_meta_info", "catname", catName,
                    "fieldid", fieldid);
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand("sys_meta_info", cmd.CommandText, conn);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("保存自定义字段类型信息时产生Sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取新结点
        /// </summary>
        /// <param name="desnode">目标结点</param>
        /// <param name="srcnode">源结点</param>
        /// <param name="type">拖动类型</param>
        /// <returns>新结点</returns>
        private TreeNode DragNode(TreeNode desnode, TreeNode srcnode, DragDropEffects type)
        {
            TreeNode newAddNode = null;
            if(srcnode.Level == 0) // 不支持分类拖动
            {
                MessageBox.Show("目前不支持分类拖动!");
                return null;
            }

            if (srcnode.Parent == desnode.Parent || srcnode.Parent == desnode) // 同一属性结点拖动
            {
                if (desnode.Level == 0) // 拖动到分类结点上
                {
                    List<TreeNode> list1 = new List<TreeNode>();
                    foreach (TreeNode node in desnode.Nodes)
                    {
                        if (node != srcnode)
                        {
                            list1.Add(node);
                        }
                    }
                    desnode.Nodes.Clear();
                    desnode.Nodes.Add(srcnode);
                    foreach (TreeNode node in list1)
                    {
                        desnode.Nodes.Add(node);
                    }
                }
                else // 拖到属性结点上
                {
                    TreeNodeCollection nodes = srcnode.Parent.Nodes;
                    List<TreeNode> list1 = new List<TreeNode>();
                    List<TreeNode> list2 = new List<TreeNode>();
                    bool findDesNode = false;
                    foreach (TreeNode node in nodes)
                    {
                        if (node != srcnode) // 是否为起始结点
                        {
                            if (node == desnode)
                            {
                                findDesNode = true;
                                list1.Add(node);
                                continue;
                            }

                            if (!findDesNode)
                            {
                                list1.Add(node);
                            }
                            else
                            {
                                list2.Add(node);
                            }
                        }
                    }

                    nodes.Clear();
                    foreach (TreeNode node in list1)
                    {
                        nodes.Add(node);
                    }
                    nodes.Add(srcnode);
                    foreach (TreeNode node in list2)
                    {
                        nodes.Add(node);
                    }
                }
            }
            else // 不同属性的结点拖动
            {
                if (desnode.Level == 0) // 拖动到分类结点上
                {
                    // 在原分类结点上删除属性结点
                    List<TreeNode> list1 = new List<TreeNode>();
                    TreeNode parentNode = srcnode.Parent;
                    foreach (TreeNode node in parentNode.Nodes)
                    {
                        if (node != srcnode)
                        {
                            list1.Add(node);
                        }
                    }
                    parentNode.Nodes.Clear();
                    foreach(TreeNode node in list1)
                    {
                        parentNode.Nodes.Add(node);
                    }

                    // 在新分类结点上添加属性结点
                    list1.Clear();
                    parentNode = desnode;
                    foreach(TreeNode node in parentNode.Nodes)
                    {
                        list1.Add(node);
                    }
                    parentNode.Nodes.Clear();
                    parentNode.Nodes.Add(srcnode);
                    foreach(TreeNode node in list1)
                    {
                        parentNode.Nodes.Add(node);
                    }
                }
                else // 拖到属性结点上
                {
                    // 在原分类结点上删除属性结点
                    List<TreeNode> list1 = new List<TreeNode>();
                    TreeNode parentNode = srcnode.Parent;
                    foreach (TreeNode node in parentNode.Nodes)
                    {
                        if (node != srcnode)
                        {
                            list1.Add(node);
                        }
                    }
                    parentNode.Nodes.Clear();
                    foreach (TreeNode node in list1)
                    {
                        parentNode.Nodes.Add(node);
                    }

                    // 在新分类结点上添加属性结点
                    TreeNodeCollection nodes = desnode.Parent.Nodes;
                    list1.Clear();
                    List<TreeNode> list2 = new List<TreeNode>();
                    bool findDesNode = false;
                    foreach (TreeNode node in nodes)
                    {            
                        if (node == desnode)
                        {
                            findDesNode = true;
                            list1.Add(node);
                            continue;
                        }

                        if (!findDesNode)
                        {
                            list1.Add(node);
                        }
                        else
                        {
                            list2.Add(node);
                        }
                    }

                    nodes.Clear();
                    foreach (TreeNode node in list1)
                    {
                        nodes.Add(node);
                    }
                    nodes.Add(srcnode);
                    foreach (TreeNode node in list2)
                    {
                        nodes.Add(node);
                    }
                }
            }

            newAddNode = srcnode;           
            return newAddNode;
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            int index = 0;
            List<string> idList = new List<string>();
            if(conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }
            foreach(TreeNode node1 in treeView1.Nodes)
            {
                foreach(TreeNode node2 in node1.Nodes)
                {
                    string fieldid = node2.Tag.ToString().Trim();
                    idList.Add(node2.Tag.ToString());
                    SaveData(fieldid, index.ToString());
                    if(node1.Text != "未分类")
                    {
                        SaveType(fieldid, node1.Text);
                    }
                    else
                    {
                        SaveType(fieldid, "");
                    }
                    index++;
                }
                index++;
            }
            if(conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }

            // 改变原结点的顺序
            List<TreeNode> nodeList = new List<TreeNode>();
            foreach(TreeNode node1 in node.Nodes)
            {
                nodeList.Add(node1);
            }
            node.Nodes.Clear();
            foreach(string s in idList)
            {
                foreach(TreeNode node1 in nodeList)
                {
                    if(s == node1.Tag.ToString())
                    {
                        node.Nodes.Add(node1);
                    }
                }
            }

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

        /// <summary>
        /// 拖动结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_ItemDrag(object sender, ItemDragEventArgs e)
        {
            DoDragDrop(e.Item, DragDropEffects.Move);
        }

        /// <summary>
        /// 拖动结点移动到结点上方
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_DragOver(object sender, DragEventArgs e)
        {
            Point targetPoint = treeView1.PointToClient(new Point(e.X, e.Y));
            TreeViewHitTestInfo tvhti = treeView1.HitTest(targetPoint);

            if (OldDragOver != tvhti.Node)
            {
                if (OldDragOver != null)
                    OldDragOver.BackColor = Color.Empty;

                OldDragOver = tvhti.Node;
                OldDragOver.BackColor = Color.BlueViolet;
            }

            if (tvhti.Location == TreeViewHitTestLocations.PlusMinus)
            {
                tvhti.Node.Expand();
            }
        }

        /// <summary>
        /// 完成结点拖动
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                TreeNode Des_Node = ((TreeView)sender).GetNodeAt(pt);
                if(Des_Node == null)
                {
                    MessageBox.Show("无效的拖动!");
                    return;
                }
                TreeNode Src_Node = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");
                TreeNode newAddNode = null;
                try
                {
                    newAddNode = DragNode(Des_Node, Src_Node, e.Effect);
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show("在拖动结点 (DragNode) 时发生错误。\r\n\r\n错误信息为：" + ex.Message);
                }

                if (newAddNode != null) // 成功的，有效果的拖动
                {
                    treeView1.SelectedNode = newAddNode;
                    if (e.Effect != DragDropEffects.Copy) // 不是同级结点拖动
                    {
                        Des_Node.Expand();
                    }
                }
                OldDragOver.BackColor = Color.Empty;
            }
        }

        /// <summary>
        /// 结点进入区域
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.AllowedEffect;
        }

        /// <summary>
        /// 增加新的分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            TypeForm tf = new TypeForm();
            DialogResult result = tf.ShowDialog();
            if(result == DialogResult.OK)
            {
                foreach(TreeNode node in treeView1.Nodes)
                {
                    if(node.Text == tf.TypeName)
                    {
                        MessageBox.Show("该分类已经存在!");
                        return;
                    }
                }
                TreeNode node1 = treeView1.Nodes.Add(tf.TypeName);
                node1.ImageIndex = 0;
                node1.SelectedImageIndex = 0;
            }
        }
    }
}