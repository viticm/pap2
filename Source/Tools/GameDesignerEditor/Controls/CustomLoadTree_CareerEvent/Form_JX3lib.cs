using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;
using System.Runtime.InteropServices;

namespace CustomLoadTree_CareerEvent
{
    public partial class Form_JX3lib : Form
    {
        [DllImport("user32.dll")]
        static extern void LockWindowUpdate(IntPtr handle);

        public Form_JX3lib()
        {
            InitializeComponent();
        }
        public Form_JX3lib(string tblname, string catfield, Class1 cls, SqlConnection conn, bool isReadOnly)
        {
            InitializeComponent();

            m_strTabName = tblname;
            m_CatField = catfield;
            m_Conn = conn;
            m_ParentClass = cls;
        }

        string m_strTabName;
        string m_CatField;
        SqlConnection m_Conn;
        Class1 m_ParentClass;
        TreeNode PopupNode;
        TreeNode OldDragOver;

        FindForm findform;

        private void button2_Click(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode == null)
                return;

            if (treeView1.SelectedNode.IsExpanded)
            {
                treeView1.SelectedNode.Collapse(false);
            }
            else
            {
                treeView1.SelectedNode.ExpandAll();
            }
            treeView1.Focus();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (findform == null)
                findform = new FindForm(this);
            findform.Show();
        }

        public TreeView ReloadTree()
        {
            treeView1.Nodes.Clear();
            TreeNode DefNode = treeView1.Nodes.Add("默认");
            DefNode.Tag = new Object[] { -1, -1, -1 };

            string filter = "subclassid = 0 and id = 0 and classid <> 0";
            string order = "ClassID";
            DataTable dtL1 = TableHelper.GetDataTableProxy(m_strTabName, filter, order, m_Conn);

            foreach (DataRow rowL1 in dtL1.Rows)
            {
                TreeNode NodeL1 = treeView1.Nodes.Add(rowL1["classname"].ToString().TrimEnd(new char[] { ' ' }));
                NodeL1.Tag = new Object[] { rowL1["classid"], 0, 0 };

                filter = "classid = " + rowL1["classid"].ToString() + " and id = 0 and subclassid <> 0";
                order = "subclassid";
                DataTable dtL2 = TableHelper.GetDataTableProxy(m_strTabName, filter, order, m_Conn);

                foreach (DataRow rowL2 in dtL2.Rows)
                {
                    TreeNode NodeL2 = NodeL1.Nodes.Add(rowL2["subclassname"].ToString().TrimEnd(new char[] { ' ' }));
                    NodeL2.Tag = new Object[] { rowL1["classid"], rowL2["subclassid"], 0 };

                    filter = "classid = " + rowL1["classid"].ToString() + " and SubClassID = " + rowL2["subclassid"].ToString() + " and id <>0";
                    order = "id";
                    DataTable dtL3 = TableHelper.GetDataTableProxy(m_strTabName, filter, order, m_Conn);
                    foreach (DataRow rowL3 in dtL3.Rows)
                    {
                        TreeNode NodeL3 = NodeL2.Nodes.Add(rowL3["title"].ToString().TrimEnd(new char[] { ' ' }));
                        NodeL3.Tag = new Object[] { rowL1["classid"], rowL2["subclassid"], rowL3["id"] };
                    }
                }
            }

            return treeView1;
        }

        public SplitContainer Spliter { get { return splitContainer1; } set { splitContainer1 = value; } }

        public TreeView Tree { get { return treeView1; } }

        private void treeView1_OnNodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                //treeView1.SelectedNode = e.Node;
                //Object[] Tags = e.Node.Tag as Object[];
                //Int32.Parse(Tags[1].ToString()) == 0)
                if (e.Node.Level == 0)
                {
                    cmNodeRightClick.SubItems[0].Enabled = true;
                    cmNodeRightClick.SubItems[1].Enabled = true;
                }
                else if (e.Node.Level == 1)
                {
                    cmNodeRightClick.SubItems[0].Enabled = false;
                    cmNodeRightClick.SubItems[1].Enabled = true;
                }
                else
                {
                    cmNodeRightClick.SubItems[0].Enabled = false;
                    cmNodeRightClick.SubItems[1].Enabled = false;
                }

                PopupNode = e.Node;
                cmNodeRightClick.Popup(Control.MousePosition);
            }
        }

        private void buttonItemDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定要删除？", "确认操作", MessageBoxButtons.OKCancel) != DialogResult.OK)
            {
                return;
            }
            Object[] Tags = PopupNode.Tag as Object[];
            string filter = "classid = " + Tags[0].ToString() + " and subclassid = " + Tags[1].ToString() + " and id = " + Tags[2].ToString();
            DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, null, m_Conn);

            dt.Rows[0].Delete();
            string sql = "select * from " + m_strTabName + " where " + filter;
            TableHelper.SaveTable(dt, sql, m_Conn);
            treeView1.Nodes.Remove(PopupNode);

            if (Tags[1].ToString() == "0")
                filter = "classid = " + Tags[0].ToString();
            else if (Tags[2].ToString() == "0")
                filter = "classid = " + Tags[0].ToString() + " and subclassid = " + Tags[1].ToString();

            dt = TableHelper.GetDataTableProxy(m_strTabName, filter, null, m_Conn);
            foreach (DataRow row in dt.Rows)
            {
                row.Delete();
            }
            sql = "select * from " + m_strTabName + " where " + filter;
            TableHelper.SaveTable(dt, sql, m_Conn);
        }

        private void buttonItemAdd_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];
            //string classid = Tags[0].ToString();
            //string subclassid = Tags[1].ToString();
            //string id = Tags[2].ToString();

            //if (classid == "-1")
            //    return;

            DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, null, "classid desc", m_Conn);
            DataRow newRow = dt.NewRow();
            int nMaxClassID = Int32.Parse(dt.Rows[0]["classid"].ToString());
            newRow["classid"] = nMaxClassID + 1;
            string newClassName = string.Format("新建{0}", nMaxClassID + 1);
            newRow["classname"] = newClassName;
            newRow["subclassid"] = 0;
            newRow["id"] = 0;
            newRow["PKey"] = GetNewPKey();

            dt.Rows.Add(newRow);
            string sql = "select * from " + m_strTabName + " order by classid desc";
            TableHelper.SaveTable(dt, sql, m_Conn);

            TreeNode newNode = treeView1.Nodes.Add(newClassName);
            newNode.Tag = new Object[] { nMaxClassID + 1, 0, 0 };
        }

        private void buttonItemAddChild_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];
            string classid = Tags[0].ToString();
            string subclassid = Tags[1].ToString();
            string id = Tags[2].ToString();

            if (PopupNode.Level == 0)
            {
                string filter = string.Format("classid = {0}", classid);
                DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, "subclassid desc", m_Conn);

                int nMaxID = Int32.Parse(dt.Rows[0]["subclassid"].ToString());
                DataRow newRow = dt.NewRow();
                newRow["classid"] = classid;
                newRow["classname"] = dt.Rows[0]["classname"];
                newRow["subclassname"] = string.Format("新建{0}", nMaxID + 1);
                newRow["subclassid"] = nMaxID + 1;
                newRow["id"] = 0;
                newRow["PKey"] = GetNewPKey();
                dt.Rows.Add(newRow);
                string sql = "select * from " + m_strTabName + " where " + filter + " order by subclassid desc";
                TableHelper.SaveTable(dt, sql, m_Conn);

                TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                newNode.Tag = new Object[] { classid, nMaxID + 1, 0 };
            }
            else
            {
                string filter = string.Format("classid = {0} and subclassid = {1}", classid, subclassid);
                DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, "id desc", m_Conn);

                int nMaxID = Int32.Parse(dt.Rows[0]["id"].ToString());
                DataRow newRow = dt.NewRow();
                newRow["classid"] = classid;
                newRow["classname"] = dt.Rows[0]["classname"];
                newRow["subclassname"] = dt.Rows[0]["subclassname"];
                newRow["subclassid"] = subclassid;
                newRow["id"] = nMaxID + 1;
                newRow["title"] = string.Format("新建{0}", nMaxID + 1);
                newRow["PKey"] = GetNewPKey();
                dt.Rows.Add(newRow);
                string sql = "select * from " + m_strTabName + " where " + filter + " order by id desc";
                TableHelper.SaveTable(dt, sql, m_Conn);

                TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                newNode.Tag = new Object[] { classid, subclassid, nMaxID + 1 };
            }
        }

        private int GetNewPKey()
        {
            DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, null, "PKey desc", m_Conn);
            return Convert.ToInt32(dt.Rows[0]["PKey"]) + 1;
        }

        private void buttonItemRename_Click(object sender, EventArgs e)
        {
            treeView1.LabelEdit = true;
            if (!PopupNode.IsEditing)
            {
                PopupNode.BeginEdit();
            }
        }

        private void treeView1_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Label != null)
            {
                //if (e.Label.Length > 0)
                //{
                Object[] Tags = e.Node.Tag as Object[];
                int classid = Int32.Parse(Tags[0].ToString());
                int subclassid = Int32.Parse(Tags[1].ToString());
                int id = Int32.Parse(Tags[2].ToString());

                string filter = string.Format("classid = {0} and subclassid = {1} and id = {2}", classid, subclassid, id);
                DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, null, m_Conn);
                string strField;
                if (e.Node.Level == 0)
                {
                    strField = "classname";
                }
                else if (e.Node.Level == 1)
                {
                    strField = "subclassname";
                }
                else
                {
                    strField = "title";
                }

                dt.Rows[0][strField] = e.Label;

                string sql = string.Format("select * from {0} where {1}", m_strTabName, filter);
                TableHelper.SaveTable(dt, sql, m_Conn);

                e.Node.EndEdit(false);
                this.treeView1.LabelEdit = false;
                //}
                //else
                //{
                //    /* Cancel the label edit action, inform the user, and 
                //       place the node in edit mode again. */
                //    e.CancelEdit = true;
                //    MessageBox.Show("Invalid tree node label.\nThe label cannot be blank",
                //       "Node Label Edit");
                //    e.Node.BeginEdit();
                //}
            }
        }

        private void treeView1_DragDrop(object sender, DragEventArgs e)
        {
            //if (this.m_isReadOnly) return;
            DialogResult result = MessageBox.Show("是否确认移动该任务结点", "任务结点变更", MessageBoxButtons.YesNo);

            if (result == DialogResult.Yes)
            {
                if (e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false))
                {
                    Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                    TreeNode Des_Node = ((TreeView)sender).GetNodeAt(pt);
                    TreeNode Src_Node = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");
                    if (Des_Node == Src_Node)
                    {
                        return;
                    }

                    if ((Des_Node.Tag as Object[])[0].ToString() != (Src_Node.Tag as Object[])[0].ToString() ||
                        (Des_Node.Tag as Object[])[1].ToString() != (Src_Node.Tag as Object[])[1].ToString())
                    {
                        MessageBox.Show("不支持的拖动。");
                        return;
                    }

                    LockWindowUpdate(Tree.Handle);

                    TreeNode newAddNode = null;
                    try
                    {
                        newAddNode = DragNode(Des_Node, Src_Node);
                    }
                    catch (System.Exception ex)
                    {
                        MessageBox.Show("在拖动结点 (DragNode) 时发生错误。\r\n\r\n错误信息为：" + ex.Message);
                    }

                    if (newAddNode != null) // 成功的，有效果的拖动
                    {
                        Tree.SelectedNode = newAddNode;
                    }
                    OldDragOver.BackColor = Color.Empty;

                    LockWindowUpdate(IntPtr.Zero);
                }
            }
            else
            {
                OldDragOver.BackColor = Color.Empty;
                OldDragOver = null;
            }

            // 解锁
            //TblLock.UnLock();
            //LastVer = TblLock.GetServerLastVersion();
            //OrderLock.UnLock();
            //LastOrderVer = OrderLock.GetServerLastVersion();
        }

        private void treeView1_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.AllowedEffect;
        }

        private void treeView1_DragOver(object sender, DragEventArgs e)
        {
            // if (this.m_isReadOnly) return;
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

        private void treeView1_ItemDrag(object sender, ItemDragEventArgs e)
        {
            //if (this.m_isReadOnly) return;

            // 不能拖动虚结点
            TreeNode src_node = ((TreeNode)e.Item);
            int src_key = Convert.ToInt32(((object[])src_node.Tag)[2]);
            if (src_key == 0)
            {
                return;
            }
            // 不能拖动镜像结点或虚结点
            //int src_parent_key = Convert.ToInt32(((object[])src_node.Parent.Tag)[0]);
            //if (src_parent_key == -3 || src_parent_key == -4)
            //{
            //    return;
            //}

            //    if (LastVer != TblLock.GetServerLastVersion())
            //    {
            //        int selected_key = Convert.ToInt32(((object[])src_node.Tag)[0]);
            //        MyParentClass.LoadTreePlugin(selected_key);
            //        LastVer = TblLock.GetServerLastVersion();
            //        return;
            //    }

            //    if (LastOrderVer != OrderLock.GetServerLastVersion())
            //    {
            //        int selected_key = Convert.ToInt32(((object[])src_node.Tag)[0]);
            //        MyParentClass.LoadTreePlugin(selected_key);
            //        LastOrderVer = OrderLock.GetServerLastVersion();
            //        return;
            //      }
            Tree.Focus();

            if (e.Button == MouseButtons.Left)
            {
                DragDropEffects effects = DoDragDrop(e.Item as TreeNode, DragDropEffects.Move);
            }
        }
        private TreeNode DragNode(TreeNode desnode, TreeNode srcnode)
        {
            int minNodeTag;
            int maxNodeTag;

            int nClassID = Convert.ToInt32((desnode.Tag as Object[])[0]);
            int nSubClassID = Convert.ToInt32((desnode.Tag as Object[])[1]);

            int desnodeTag = Convert.ToInt32((desnode.Tag as Object[])[2]);
            int srcnodeTag = Convert.ToInt32((srcnode.Tag as Object[])[2]);

            TreeNode newNodeToAdd;

            if (desnodeTag < srcnodeTag)
            {
                minNodeTag = desnodeTag;
                maxNodeTag = srcnodeTag;
            }
            else
            {
                minNodeTag = srcnodeTag;
                maxNodeTag = desnodeTag;
            }

            string filter = string.Format("ClassID = {0} and SubClassID = {1} and ID >= {2} and ID <= {3}", nClassID, nSubClassID, minNodeTag, maxNodeTag);
            DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, "ID", m_Conn);

            if (desnodeTag < srcnodeTag)//下面的拖到上面
            {
                foreach (DataRow row in dt.Rows)
                {
                    if (row == dt.Rows[dt.Rows.Count - 1])
                        row["ID"] = minNodeTag;
                    else
                        row["ID"] = Convert.ToInt32(row["ID"]) + 1;
                }

                Object[] Tags;
                for (int i = desnode.Index; i < srcnode.Index; i++)
                {
                    Tags = desnode.Parent.Nodes[i].Tag as Object[];
                    Tags[2] = Convert.ToInt32(Tags[2]) + 1;
                    desnode.Parent.Nodes[i].Tag = Tags;
                }
                Tags = desnode.Parent.Nodes[srcnode.Index].Tag as Object[];
                Tags[2] = minNodeTag;
                string newText = srcnode.Text;

                desnode.Parent.Nodes.Remove(srcnode);
                newNodeToAdd = desnode.Parent.Nodes.Insert(desnode.Index, newText);
                newNodeToAdd.Tag = Tags;
            }
            else//上面的拖到下面
            {
                foreach (DataRow row in dt.Rows)
                {
                    if (row == dt.Rows[0])
                        row["ID"] = maxNodeTag;
                    else
                        row["ID"] = Convert.ToInt32(row["ID"]) - 1;
                }

                Object[] TempTags = srcnode.Tag as Object[];
                TempTags[2] = maxNodeTag;

                for (int i = srcnode.Index + 1; i <= desnode.Index; i++)
                {
                    Object[] Tags = srcnode.Parent.Nodes[i].Tag as Object[];
                    Tags[2] = Convert.ToInt32(Tags[2]) - 1;
                    srcnode.Parent.Nodes[i].Tag = Tags;
                }
                string newText = srcnode.Text;
                
                desnode.Parent.Nodes.RemoveAt(srcnode.Index);
                newNodeToAdd = desnode.Parent.Nodes.Insert(desnode.Index + 1, newText);
                newNodeToAdd.Tag = TempTags;
            }
            
            string sql = string.Format("select * from {0} where {1} order by ID", m_strTabName, filter);
            TableHelper.SaveTable(dt, sql, m_Conn);

            return newNodeToAdd;
        }

    }
}