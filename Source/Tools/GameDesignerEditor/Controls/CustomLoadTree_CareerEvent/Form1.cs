using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;
using DevComponents.DotNetBar;

namespace CustomLoadTree_CareerEvent
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        public Form1(string tblname, string catfield, Class1 cls, SqlConnection conn, bool isReadOnly) 
        {
            InitializeComponent();

            m_TblName = tblname;
            m_CatField = catfield;
            m_MyParentClass = cls;
            m_Conn = conn;
            //m_buttonItemID.Checked = DisplayID;
            //m_buttonItemLevel.Checked = DisplayLevel;
            //m_TblLock = new TableLock(m_Conn, m_TblName);
            //m_TblLock.UnLock();
            ////m_OrderLock = new TableLock(m_Conn, orderTable);
            ////m_OrderLock.UnLock();
            //m_nLastVer = m_TblLock.GetServerLastVersion();
            //m_nLastOrderVer = m_OrderLock.GetServerLastVersion();
          //  m_bReadOnly = isReadOnly;
            
        }

        public SplitContainer Spliter { get { return splitContainer1; } set { splitContainer1 = value; } }

        public TreeView ReloadTree()
        {
            treeView1.Nodes.Clear();
            //string sql = string.Format("select * from {0} where level > 0", m_TblName);
            //DataTable tbl = Helper.GetDataTable(sql, m_Conn);
           
            DataTable tbl = TableHelper.GetDataTableProxy(m_TblName, "level <> 0", null, m_Conn);

            foreach (DataRow row in tbl.Rows)
            {
                //if (Int32.Parse(row["level"].ToString()) > 0)
                //{
                    TreeNode parentNode = treeView1.Nodes.Add(row["name"].ToString());
                    parentNode.Text = parentNode.Text.Trim(new char[] { '\n', '\r', '\t', ' ' });
                    parentNode.Tag = new Object[] { row["level"], (Object)(int)-1 };

                    if (Int32.Parse(row["level"].ToString()) == -1)
                    {
                        continue;
                    }

                    for (int i = 0; i <= 7; i++)
                    {
                        string strIndex = string.Format("tab{0}", i);
                        Object TabID = row[strIndex];
                        if (TabID.ToString() == string.Empty)
                        {
                            continue;
                        }

                        //string sql = string.Format("select * from {0} where {1}", m_TblName, "level = 0 and tabid =" + TabID.ToString());
                        DataTable dt = TableHelper.GetDataTableProxy(m_TblName, "level = 0 and tabid =" + TabID.ToString(), null, m_Conn);

                        if (dt.Rows.Count == 0)
                        {
                            MessageBox.Show(parentNode.Text + "__  警告：有未找到的索引ID: " + TabID.ToString());
                            continue;
                        }
                        string name = dt.Rows[0]["tabname"].ToString();
                        if (name == string.Empty)
                        {
                            continue;
                        }
                        TreeNode node = parentNode.Nodes.Add(name);
                        node.Text = node.Text.Trim(new char[] { '\n', '\r', '\t', ' '});
                        node.Tag = new Object[] { (Object)(int)0, TabID };
                    }
 //               }
            }
            return treeView1;
        }

        string m_TblName;
        string m_CatField;
        Class1 m_MyParentClass;
        SqlConnection m_Conn;
       // TableLock m_TblLock;
       // int m_nLastVer;

        TreeNode PopupNode;
        //int m_nLastOrderVer;
       // bool m_bReadOnly;

        public ButtonItem[] m_ExtraMenuItem = null;

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

        private void buttonItemAdd_Click(object sender, EventArgs e)
        {
            DataTable dt = TableHelper.GetDataTableProxy(m_TblName, null, "tabid desc", m_Conn);
            int nMaxTabID = Int32.Parse((dt.Rows[0]["tabid"].ToString()));
            DataRow tabRow = dt.NewRow();
            tabRow["level"] = 0;
            tabRow["tabid"] = nMaxTabID + 1;
            tabRow["tabname"] = "默认首页";
            tabRow["indexpage"] = true;
            dt.Rows.Add(tabRow);
            string sql = "select * from " + m_TblName + " order by tabid desc";
            TableHelper.SaveTable(dt, sql, m_Conn);

            DataTable tab = TableHelper.GetDataTableProxy(m_TblName, null, "level desc", m_Conn);
            int level = Int32.Parse((tab.Rows[0]["level"].ToString())) + 1;
            DataRow newRow =  tab.NewRow();
            newRow["level"] = level;
            newRow["tabid"] = -1;
            newRow["name"] = "新建触发等级:" + level.ToString();
            newRow["tab0"] = nMaxTabID + 1;
            tab.Rows.Add(newRow);
            sql = "select * from " + m_TblName + " order by level desc";
            TableHelper.SaveTable(tab, sql, m_Conn);

            TreeNode newNode = treeView1.Nodes.Add("新建触发等级:" + level.ToString());
            newNode.Tag = new Object[] { level, -1 };

            newNode = newNode.Nodes.Add("默认首页");
            newNode.Tag = new Object[] { 0, nMaxTabID + 1 };
        }

        private void buttonItemDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定要删除？", "确认操作", MessageBoxButtons.OKCancel) != DialogResult.OK)
            {
                return;
            }
            Object[] Tags = PopupNode.Tag as Object[];
            int level = Int32.Parse(Tags[0].ToString());
            int tabid = Int32.Parse(Tags[1].ToString());

            if (level == -1)
            {
                return;
            }

            if (level != 0)
            {
                DataTable dt = TableHelper.GetDataTableProxy(m_TblName, "level = " + level.ToString(), null, m_Conn);
                DataRow row = dt.Rows[0];
               
                for (int i = 0; i < 8; i++)
                {
                    string strTabID = row["tab" + i.ToString()].ToString();
                    if (strTabID != string.Empty)
                    {
                        int ntabid = Int32.Parse(strTabID);
                        DeleteTabByID(ntabid, true);
                    }
                }

                row.Delete();
                //dt.Rows.Remove(row);
                string sql = "select * from " + m_TblName + " where level = " + level.ToString();
                TableHelper.SaveTable(dt, sql, m_Conn);
            }
            else
            {
                if (DeleteTabByID(tabid, false) == -1)
                {
                    MessageBox.Show("未能删除");
                    return;
                } 
                

                StringBuilder filter = new StringBuilder();
                filter.Append("tab0 = " + tabid.ToString());
                filter.Append(" or tab1 = " + tabid.ToString());
                filter.Append(" or tab2 = " + tabid.ToString());
                filter.Append(" or tab3 = " + tabid.ToString());
                filter.Append(" or tab4 = " + tabid.ToString());
                filter.Append(" or tab5 = " + tabid.ToString());
                filter.Append(" or tab6 = " + tabid.ToString());
                filter.Append(" or tab7 = " + tabid.ToString());
              
                DataTable dt = TableHelper.GetDataTableProxy(m_TblName, filter.ToString(), null, m_Conn);
                DataRow row = dt.Rows[0];
                for (int i = 0; i < 8; i++)
                {
                    string strValue = row["tab" + i.ToString()].ToString();
                    if (strValue != string.Empty)
                    {
                        if (Int32.Parse(strValue) == tabid)
                        {
                            row["tab" + i.ToString()] = DBNull.Value;
                        }
                    }
                  
                }
                TableHelper.SaveTable(dt, "select * from " + m_TblName + " where " + filter.ToString(), m_Conn);
            }
            treeView1.Nodes.Remove(PopupNode);
        }

        private int DeleteTabByID(int nTabID, bool bForceToDel)
        {
            DataTable dt = TableHelper.GetDataTableProxy(m_TblName, "tabid = " + nTabID.ToString(), null, m_Conn);
            
            string strIndexPage = dt.Rows[0]["indexpage"].ToString();
            if (strIndexPage == "1" && !bForceToDel)
            {
                return -1;
            }
            
            dt.Rows[0].Delete();
            //dt.Rows.RemoveAt(0);
            string sql = "select * from " + m_TblName + " where tabid = " + nTabID.ToString();
            return TableHelper.SaveTable(dt, sql, m_Conn);
        }

        //private void buttonItemRename_Click(object sender, EventArgs e)
        //{
        //    treeView1.LabelEdit = true;
        //    if (!PopupNode.IsEditing)
        //    {
        //        PopupNode.BeginEdit();
        //    }
        //}


        private void buttonItemAddChild_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];
            int level = Int32.Parse(Tags[0].ToString());
            int tabid = Int32.Parse(Tags[1].ToString());

            DataTable dt = TableHelper.GetDataTableProxy(m_TblName, "level = " + level.ToString(), null, m_Conn);
            DataRow row = dt.Rows[0];

            int nID = 0;
            for (int i = 0; i < 8; i++)
            {
                string strID = row["tab" + i.ToString()].ToString();
                if (strID == string.Empty)
                {
                    nID = i;
                    break;
                }
                if (i == 7)
                {
                    MessageBox.Show("子层已经达到最大数量。");
                    return;
                }
            }

            DataTable dtTabID = TableHelper.GetDataTableProxy(m_TblName, "level = 0", "tabid desc", m_Conn);
            DataRow rowTabID = dtTabID.Rows[0];
            string strTabID = rowTabID["tabid"].ToString();
            int nTabID = Int32.Parse(strTabID) + 1;

            row["tab" + nID.ToString()] = nTabID;
            DataRow newRow = dt.NewRow();

            newRow["level"] = 0;
            newRow["tabid"] = nTabID;
            newRow["tabname"] = "新建：" + nTabID.ToString();

            dt.Rows.Add(newRow);

            string sql = "select * from " + m_TblName + " where level = " + level.ToString();
            TableHelper.SaveTable(dt, sql, m_Conn);
            
            TreeNode newNode = PopupNode.Nodes.Add("新建：" + nTabID.ToString());
            newNode.Tag = new object[] { 0, nTabID};
            treeView1.SelectedNode = newNode;
        }

        private void OnTreeNodeClicked(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                treeView1.SelectedNode = e.Node;
                Object[] Tags = e.Node.Tag as Object[];
                if (Int32.Parse(Tags[0].ToString()) == 0)
                {
                    cmNodeRightClick.SubItems[0].Enabled = false;
                    cmNodeRightClick.SubItems[1].Enabled = false;
                }
                else
                {
                    cmNodeRightClick.SubItems[0].Enabled = true;
                    cmNodeRightClick.SubItems[1].Enabled = true;
                }

                PopupNode = e.Node;
                cmNodeRightClick.Popup(Control.MousePosition);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ReloadTree();
        }
    }
}