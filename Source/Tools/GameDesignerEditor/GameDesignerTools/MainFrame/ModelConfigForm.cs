using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.IO;
using DevComponents.DotNetBar;

namespace GameDesingerTools.DBTools
{
    public partial class ModelConfigForm : Form
    {
        private TreeNode OldDragOver = null; // 旧的拖动树结点
        private MainForm mainForm; // 主窗体面板
        private ImageList imageList = new ImageList(); // 图标链表
        private Hashtable modelTable = new Hashtable(); // 模块和其树结点的hash表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="mainForm">主窗体对象</param>
        public ModelConfigForm(MainForm mainForm)
        {
            this.mainForm = mainForm;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            InitImageList();
            InitNode();
        }

        /// <summary>
        /// 获取用户的自定义模块显示字符串
        /// </summary>
        /// <param name="hostname">用户名</param>
        /// <returns>用户自定义模块显示字符串</returns>
        private string GetConfigString(string hostname)
        {
            SqlConnection conn = new SqlConnection(Program.ConnetionString); // sql连接
            string configString = null; // 用户的自定义模块显示字符串

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }
                /* remoting
                string sqlString = string.Format("SELECT data FROM {0} WHERE host = '{1}'", "sys_modl_display_cfg", hostname);
                DataTable table = Helper.GetDataTable(sqlString, conn);
                */

                string filter = string.Format("host = '{0}'", hostname);
                DataTable table = Helper.GetDataTableProxy("sys_modl_display_cfg", filter, null, conn);

                if(table.Rows.Count > 0)
                {
                    if (!(table.Rows[0]["data"] is DBNull)) // 字符串内容非空
                    {
                        configString = table.Rows[0]["data"].ToString();
                    }
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show(string.Format("在获取用户名为{0}的用户自定义模块显示数据时产生sql异常: ", hostname) + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return configString;
        }

        /// <summary>
        /// 初始化树结点
        /// </summary>
        private void InitNode()
        {
            int index = 1; // key序号
            TreeNode newKindNode; // 分类树结点
            TreeNode newValueNode; // 值结点
            List<TreeNode> nodeList = new List<TreeNode>(); // 已经添加了的树结点的链表

            bool readHideKind = false; // 是否已读自动隐藏分类 
            
            string configString = GetConfigString(System.Net.Dns.GetHostName());
            if(configString != null) // 用户自定义模块显示字符串不为空
            {
                string[] kinds = configString.Split(new char[] { ';' });
                foreach(string s in kinds)
                {
                    if (readHideKind) // 已经读完自动隐藏分类
                    {
                        newKindNode = treeView1.Nodes.Add("分类" + index);
                        index++;
                    }
                    else // 没有读完自动隐藏分类
                    {
                        readHideKind = true;
                        newKindNode = treeView1.Nodes.Add("自动隐藏");
                    }

                    newKindNode.ImageIndex = 6;
                    newKindNode.SelectedImageIndex = 6;
                    if (s == string.Empty)
                    {
                        continue;
                    }

                    string[] modelIDs = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string t in modelIDs)
                    {
                        if (modelTable[t] != null)
                        {
                            newValueNode = modelTable[t] as TreeNode;
                            newKindNode.Nodes.Add(newValueNode);
                            nodeList.Add(newValueNode);
                        }
                    }
                }
            }

            switch(treeView1.Nodes.Count)
            {
                case 0: // 配置文件中没有当前库的模块显示自定义信息
                    {
                        // 添加自动隐藏分类
                        newKindNode = treeView1.Nodes.Add("自动隐藏");
                        newKindNode.ImageIndex = 6;
                        newKindNode.SelectedImageIndex = 6;
                        // 添加新分类
                        newKindNode = treeView1.Nodes.Add("分类1");
                        newKindNode.ImageIndex = 6;
                        newKindNode.SelectedImageIndex = 6;
                        foreach (object o in modelTable.Values)
                        {
                            newValueNode = o as TreeNode;
                            newKindNode.Nodes.Add(newValueNode);
                        }
                        break;
                    }
                case 1: // 配置文件中没有模块显示自定义信息
                    {
                        // 添加新分类
                        newKindNode = treeView1.Nodes.Add("分类1");
                        newKindNode.ImageIndex = 6;
                        newKindNode.SelectedImageIndex = 6;
                        foreach(object o in modelTable.Values)
                        {
                            newValueNode = o as TreeNode;
                            newKindNode.Nodes.Add(newValueNode);
                        }
                        break;
                    }
                default: // 配置文件中所有模块显示自定义信息都已经正确读取              
                    {
                        newKindNode = treeView1.Nodes[treeView1.Nodes.Count - 1];
                        foreach (object o in modelTable.Values) // 检查新加的模块有没有放到显示列表中
                        {
                            newValueNode = o as TreeNode;
                            if (!nodeList.Contains(newValueNode))
                            {
                                newKindNode.Nodes.Add(newValueNode);
                            }
                        }
                                               
                        break;
                    }
            }
        }

        /// <summary>
        /// 初始化图标链表
        /// </summary>
        private void InitImageList()
        {
            SqlConnection conn = new SqlConnection(Program.ConnetionString); // sql连接
            List<TreeNode> nodeList = new List<TreeNode>(); // 树结点链表
            treeView1.ImageList = imageList; // 图标链表
            Image newImage; // 临时图标

            foreach(Image image in mainForm.imageList2.Images)
            {
                imageList.Images.Add(image);
            }

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                // 从sys_modl_def读取功能模块信息
                //DataTable tbl = Helper.GetDataTable("SELECT * FROM sys_modl_def", conn);

                DataTable tbl = Helper.GetDataTableProxy("sys_modl_def", conn);

                foreach (DataRow row in tbl.Rows)
                {
                    int iconid = Convert.ToInt32(row["iconid"]);
                    string id = row["id"].ToString();

                    if (iconid < imageList.Images.Count)
                    {
                        if (iconid == -1) // 自定义图标
                        {
                            TreeNode newNode = new TreeNode(row["modelname_en"].ToString());
                            newImage = GetImageFromDataBase(row);
                            imageList.Images.Add(newImage);
                            newNode.ImageIndex = imageList.Images.Count - 1;
                            newNode.SelectedImageIndex = imageList.Images.Count - 1;
                            newNode.Tag = id;
                            nodeList.Add(newNode);
                            modelTable[id] = newNode;
                        }
                        else
                        {
                            TreeNode newNode = new TreeNode(row["modelname_en"].ToString()); 
                            newNode.ImageIndex = iconid;
                            newNode.SelectedImageIndex = iconid;
                            newNode.Tag = id;
                            nodeList.Add(newNode);
                            modelTable[id] = newNode;
                        }
                    }
                }

            }
            catch(SqlException ex)
            {
                MessageBox.Show("在初始化图标链表时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }      
        }

        /// <summary>
        /// 从数据库中读取图标信息
        /// </summary>
        /// <param name="r">数据库行</param>
        /// <returns>图标</returns>
        private Image GetImageFromDataBase(DataRow r)
        {
            Image image = null;
            byte[] imageData;
            try
            {
                imageData = (byte[])r["imagedata"];
                MemoryStream ms = new MemoryStream();
                ms.Write(imageData, 0, imageData.Length);
                image = Image.FromStream(ms);
                ms.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("在读取数据库图标信息时产生异常: " + ex.ToString());
            }
            return image;
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        private void SaveData()
        {
            List<ButtonItem> buttonItemList = new List<ButtonItem>(); // 保存ButtonItem的链表
            ButtonItem buttonItem; // 临时ButtonItem对象
            string values = string.Empty; // 保存在自定义模块显示内容

            foreach(TreeNode kindNode in treeView1.Nodes)
            {
                if(kindNode.Text == "自动隐藏") // 是自动隐藏结点
                {
                    string value = string.Empty;
                    foreach (TreeNode node in kindNode.Nodes)
                    {
                        value += string.Format("{0},", node.Tag);
                    }
                    values = values + value.TrimEnd(',') + ";";
                }
                else // 不是自动隐藏结点
                {
                    if(kindNode.Nodes.Count == 0) // 没有子结点
                    {
                        continue;
                    }

                    string value = string.Empty;
                    foreach (TreeNode node in kindNode.Nodes)
                    {
                        value += string.Format("{0},", node.Tag);
                        if(mainForm.imageTable[node.Tag] != null)
                        {
                            buttonItem = mainForm.imageTable[node.Tag] as ButtonItem;
                            if(node == kindNode.Nodes[0]) // 是该分类的第一个ButtonItem，要加分组标记
                            {
                                buttonItem.BeginGroup = true;
                            }
                            else
                            {
                                buttonItem.BeginGroup = false;
                            }
                            buttonItemList.Add(buttonItem);
                        }
                    }
                    values = values + value.TrimEnd(',') + ";";             
                }
            }

            values = values.TrimEnd(';'); 

            SqlConnection conn = new SqlConnection(Program.ConnetionString); // sql连接
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string hostName = System.Net.Dns.GetHostName();

                
                string sqlString = string.Format("SELECT host FROM {0} WHERE host = '{1}'", "sys_modl_display_cfg", hostName);
                /* remoting
                * DataTable table = Helper.GetDataTable(sqlString, conn);
                */

                string filter = string.Format("host = '{0}'", hostName);
                DataTable table = Helper.GetDataTableProxy("sys_modl_display_cfg", filter, null, conn);

                if(table.Rows.Count > 0) // 该用户记录已经存在，修改
                {
                    sqlString = string.Format("UPDATE {0} SET data = '{1}' WHERE host = '{2}'", "sys_modl_display_cfg", values, hostName);
                }
                else // 该用户记录未存在，添加
                {
                    sqlString = string.Format("INSERT INTO {0} (host, data) VALUES ('{1}', '{2}')", "sys_modl_display_cfg", hostName,
                        values);
                }
                /* remoting
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
                 */

                Helper.ExecuteCommand("sys_modl_display_cfg", sqlString, conn);

            }
            catch(SqlException ex)
            {
                MessageBox.Show("在保存用户自定义模块显示信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            mainForm.ReloadButtonItems(buttonItemList); // 重新加载ButtonItem
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
            if (srcnode.Level == 0) // 不支持分类拖动
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
                    foreach (TreeNode node in list1)
                    {
                        parentNode.Nodes.Add(node);
                    }

                    // 在新分类结点上添加属性结点
                    list1.Clear();
                    parentNode = desnode;
                    foreach (TreeNode node in parentNode.Nodes)
                    {
                        list1.Add(node);
                    }
                    parentNode.Nodes.Clear();
                    parentNode.Nodes.Add(srcnode);
                    foreach (TreeNode node in list1)
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
        /// 新建分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            TreeNode newKindNode = treeView1.Nodes.Add("分类" + treeView1.Nodes.Count);
            newKindNode.ImageIndex = 6;
            newKindNode.SelectedImageIndex = 6;
            treeView1.SelectedNode = newKindNode;
        }

        /// <summary>
        /// 确定自定义选择
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            SaveData();
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 取消自定义选择
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
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
                if (Des_Node == null)
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

                if(OldDragOver != null)
                {
                    OldDragOver.BackColor = Color.Empty;
                }
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
                if(OldDragOver != null)
                {
                    OldDragOver.BackColor = Color.BlueViolet;
                }
            }

            if (tvhti.Location == TreeViewHitTestLocations.PlusMinus)
            {
                tvhti.Node.Expand();
            }
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
        /// 重设分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("是否确认重设所有分类", "重设所有分类", 
                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if(result == DialogResult.Yes) // 用户选择确定
            {
                TreeNode kindNode = null;
                treeView1.Nodes.Clear();
                kindNode = treeView1.Nodes.Add("自动隐藏");
                kindNode.ImageIndex = 6;
                kindNode.SelectedImageIndex = 6;
                kindNode = treeView1.Nodes.Add("分类1");
                kindNode.ImageIndex = 6;
                kindNode.SelectedImageIndex = 6;
                foreach (object o in modelTable.Values)
                {
                    kindNode.Nodes.Add(o as TreeNode);
                }
            }
            
        }
    }
}