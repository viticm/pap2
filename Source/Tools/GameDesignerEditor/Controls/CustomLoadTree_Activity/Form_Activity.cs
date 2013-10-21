using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;

namespace CustomLoadTree_Activity
{
    public partial class Form_Activity : Form
    {
        public Form_Activity()
        {
            InitializeComponent();
        }
        public Form_Activity(Class1 cls, SqlConnection conn, bool isReadOnly)
        {
            InitializeComponent();

            m_Conn = conn;
            m_ParentClass = cls;
        }

        SqlConnection m_Conn;
        Class1 m_ParentClass;
        TreeNode PopupNode;

        //FindForm findform;

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
            //if (findform == null)
            //    findform = new FindForm(this);
            //findform.Show();
        }

        public TreeView ReloadTree()
        {
            treeView1.Nodes.Clear();
            TreeNode DefNode = treeView1.Nodes.Add("默认");
            DefNode.Tag = new Object[] { -1, -1, -1 };

            //////////////////////////////////////////////////////////////////////////
            //"tbl_Guide_ActivityInfo" load tree
            //////////////////////////////////////////////////////////////////////////
            
            DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_ActivityInfo", m_Conn);
            DataRow RootRow = dt.Rows.Find(new Object[] { 0, 0 });
            if (RootRow == null)
            {
                MessageBox.Show("未找到根节点.");
            }

            int nClassIDRoot = Int32.Parse(RootRow["ClassID"].ToString());
            int nActivityIDRoot = Int32.Parse(RootRow["ActivityID"].ToString()); 

            TreeNode RootNode = Tree.Nodes.Add(RootRow["ClassName"].ToString());
            RootNode.Tag = new Object[] { "ACTIVITY", nClassIDRoot, nActivityIDRoot };

            foreach (DataRow rowL1 in dt.Rows)
            {
                int nClassIDL1 = Int32.Parse(rowL1["ClassID"].ToString());
                int nActivityIDL1 = Int32.Parse(rowL1["ActivityID"].ToString());

                if (nClassIDL1 > 0 && nActivityIDL1 == 0)
                {
                    TreeNode NodeL1 = RootNode.Nodes.Add(rowL1["ClassName"].ToString());
                    NodeL1.Tag = new Object[] { "ACTIVITY", nClassIDL1, nActivityIDL1 };

                    foreach (DataRow rowL2 in dt.Rows)
                    {
                        int nClassIDL2 = Int32.Parse(rowL2["ClassID"].ToString());
                        int nActivityIDL2 = Int32.Parse(rowL2["ActivityID"].ToString());

                        if (nClassIDL1 == nClassIDL2 && nActivityIDL2 > 0)
                        {
                            TreeNode NodeL2 = NodeL1.Nodes.Add(rowL2["Title"].ToString());
                            NodeL2.Tag = new Object[] { "ACTIVITY", nClassIDL2, nActivityIDL2 };
                        }
                    }
                }
            }



            //////////////////////////////////////////////////////////////////////////
            //"tbl_Guide_DailyQuestInfo" load tree
            //////////////////////////////////////////////////////////////////////////

            dt = TableHelper.GetDataTableProxy("tbl_Guide_DailyQuestInfo", m_Conn);
            RootRow = dt.Rows.Find(new Object[] { 0, 0 });
            if (RootRow == null)
            {
                MessageBox.Show("未找到根节点.");
            }

            int nTypeIDRoot = Int32.Parse(RootRow["TypeID"].ToString());
            int nQuestIDRoot = Int32.Parse(RootRow["QuestID"].ToString());

            RootNode = Tree.Nodes.Add(RootRow["TypeName"].ToString());
            RootNode.Tag = new Object[] { "DAILYQUEST", nTypeIDRoot, nQuestIDRoot };

            foreach (DataRow rowL1 in dt.Rows)
            {
                int nTypeIDL1 = Int32.Parse(rowL1["TypeID"].ToString());
                int nQuestIDL1 = Int32.Parse(rowL1["QuestID"].ToString());

                if (nTypeIDL1 > 0 && nQuestIDL1 == 0)
                {
                    TreeNode NodeL1 = RootNode.Nodes.Add(rowL1["TypeName"].ToString());
                    NodeL1.Tag = new Object[] { "DAILYQUEST", nTypeIDL1, nQuestIDL1 };

                    foreach (DataRow rowL2 in dt.Rows)
                    {
                        int nTypeIDL2 = Int32.Parse(rowL2["TypeID"].ToString());
                        int nQuestIDL2 = Int32.Parse(rowL2["QuestID"].ToString());

                        if (nTypeIDL1 == nTypeIDL2 && nQuestIDL2 > 0)
                        {
                            TreeNode NodeL2 = NodeL1.Nodes.Add(rowL2["TypeName"].ToString());
                            NodeL2.Tag = new Object[] { "DAILYQUEST", nTypeIDL2, nQuestIDL2 };
                        }
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
                    cmNodeRightClick.SubItems[0].Enabled = false;
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
                if ((PopupNode.Tag as Object[])[0].ToString() != "-1")
                    cmNodeRightClick.Popup(Control.MousePosition);
            }
        }

        private void buttonItemDel_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];

            string[] Strings = GetStringsFromTagMark(Tags[0].ToString());
            string strTableName = Strings[0];
            string strKey1 = Strings[1];
            string strKey2 = Strings[2];

            string filter = string.Format("{0} = {1} and {2} = {3}", strKey1, Tags[1], strKey2, Tags[2]);
            DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, null, m_Conn);

            dt.Rows[0].Delete();
            string sql = "select * from " + strTableName + " where " + filter;
            TableHelper.SaveTable(dt, sql, m_Conn);
        }

        public string[] GetStringsFromTagMark(string strMark)
        {
            string strTabName = string.Empty;
            string strKeys1 = string.Empty;
            string strKeys2 = string.Empty;

            if (strMark == "ACTIVITY")
            {
                strTabName = "tbl_Guide_ActivityInfo";
                strKeys1 = "ClassID";
                strKeys2 = "ActivityID";
            }
            else if (strMark == "DAILYQUEST")
            {
                strTabName = "tbl_Guide_DailyQuestInfo";
                strKeys1 = "TypeID";
                strKeys2 = "QuestID";
            }
            else if (strMark == "-1")
            {

            }
            else
            {
                MessageBox.Show(strMark + " -- 异常的标记位");
            }

            return new string[] { strTabName, strKeys1, strKeys2 };
        }

        private void buttonItemAdd_Click(object sender, EventArgs e)
        {
            //Object[] Tags = PopupNode.Tag as Object[];
            ////string classid = Tags[0].ToString();
            ////string subclassid = Tags[1].ToString();
            ////string id = Tags[2].ToString();

            ////if (classid == "-1")
            ////    return;
            //string[] Strings = GetStringsFromTagMark(Tags[0].ToString());
            //string strTableName = Strings[0];
            //string strKey1 = Strings[1];
            //string strKey2 = Strings[2];

            //DataTable dt = TableHelper.GetDataTableProxy(strTabName, null, strKeys1 + " desc", m_Conn);
            //DataRow newRow = dt.NewRow();
            //int nMaxClassID = Int32.Parse(dt.Rows[0][strKey1].ToString());
            //newRow[strKey1] = nMaxClassID + 1;
            //string newClassName = string.Format("新建{0}", nMaxClassID + 1);
            //newRow["classname"] = newClassName;
            //newRow["subclassid"] = 0;
            //newRow["id"] = 0;

            //dt.Rows.Add(newRow);
            //string sql = "select * from " + m_strTabName + " order by classid desc";
            //TableHelper.SaveTable(dt, sql, m_Conn);

            //TreeNode newNode = treeView1.Nodes.Add(newClassName);
            //newNode.Tag = new Object[] { nMaxClassID + 1, 0, 0 };
        }

        private void buttonItemAddChild_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];
 
            string[] Strings = GetStringsFromTagMark(Tags[0].ToString());
            string strTableName = Strings[0];
            string strKey1 = Strings[1];
            string strKey2 = Strings[2];

            if (Tags[0].ToString() == "ACTIVITY")
            {
                if (PopupNode.Level == 0)
                {
                    string filter = null;
                    DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, strKey1 + " desc", m_Conn);

                    int nMaxID = Int32.Parse(dt.Rows[0][strKey1].ToString());
                    DataRow newRow = dt.NewRow();

                    newRow[strKey1] = nMaxID + 1;
                    newRow["ClassName"] = string.Format("新建{0}", nMaxID + 1);
                    newRow[strKey2] = 0;

                    dt.Rows.Add(newRow);
                    string sql = "select * from " + strTableName + " order by " + strKey1 + " desc";
                    TableHelper.SaveTable(dt, sql, m_Conn);

                    TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                    newNode.Tag = new Object[] { "ACTIVITY", nMaxID + 1, 0 };
                }
                else
                {
                    string filter = string.Format("{0} = {1}", strKey1, Tags[1]);
                    DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, strKey2 + " desc", m_Conn);

                    int nMaxID = Int32.Parse(dt.Rows[0][strKey2].ToString());
                    DataRow newRow = dt.NewRow();

                    newRow[strKey1] = dt.Rows[0][strKey1];
                    newRow["ClassName"] = dt.Rows[0]["ClassName"];
                    newRow[strKey2] = nMaxID + 1;
                    newRow["Title"] = string.Format("新建{0}", nMaxID + 1);

                    dt.Rows.Add(newRow);
                    string sql = "select * from " + strTableName + " where " + filter + " order by " + strKey2 + " desc";
                    TableHelper.SaveTable(dt, sql, m_Conn);

                    TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                    newNode.Tag = new Object[] { "ACTIVITY", Tags[1], nMaxID + 1 };
                }
            }
            else if (Tags[0].ToString() == "DAILYQUEST")
            {
                if (PopupNode.Level == 0)
                {
                    string filter = null;
                    DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, strKey1 + " desc", m_Conn);

                    int nMaxID = Int32.Parse(dt.Rows[0][strKey1].ToString());
                    DataRow newRow = dt.NewRow();

                    newRow[strKey1] = nMaxID + 1;
                    newRow["TypeName"] = string.Format("新建{0}", nMaxID + 1);
                    newRow[strKey2] = 0;

                    dt.Rows.Add(newRow);
                    string sql = "select * from " + strTableName + " order by " + strKey1 + " desc";
                    TableHelper.SaveTable(dt, sql, m_Conn);

                    TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                    newNode.Tag = new Object[] { "DAILYQUEST", nMaxID + 1, 0 };
                }
                else
                {
                    string filter = string.Format("{0} = {1}", strKey1, Tags[1]);
                    DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, strKey2 + " desc", m_Conn);

                    int nMaxID = Int32.Parse(dt.Rows[0][strKey2].ToString());
                    DataRow newRow = dt.NewRow();

                    newRow[strKey1] = dt.Rows[0][strKey1];
                    newRow[strKey2] = nMaxID + 1;
                    newRow["TypeName"] = string.Format("新建{0}", nMaxID + 1);

                    dt.Rows.Add(newRow);
                    string sql = "select * from " + strTableName + " where " + filter + " order by " + strKey2 + " desc";
                    TableHelper.SaveTable(dt, sql, m_Conn);

                    TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                    newNode.Tag = new Object[] { "DAILYQUEST", Tags[1], nMaxID + 1 };
                }
            }
           
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
                    string[] Strings = GetStringsFromTagMark(Tags[0].ToString());
                    string strTableName = Strings[0];
                    string strKey1 = Strings[1];
                    string strKey2 = Strings[2];

                    string filter = string.Format("{0} = {1} and {2} = {3}", strKey1, Tags[1], strKey2, Tags[2]);
                    DataTable dt = TableHelper.GetDataTableProxy(strTableName, filter, null, m_Conn);
                    string strField = string.Empty;

                    if (Tags[0].ToString() == "DAILYQUEST")
                    {
                        strField = "TypeName";
                    }
                    else if (Tags[0].ToString() == "ACTIVITY")
                    {
                        if (e.Node.Level == 1)
                        {
                            strField = "ClassName";
                        }
                        else if (e.Node.Level == 2)
                        {
                            strField = "Title";
                        }
                    }
                        
                    if (strField == string.Empty)
                    {
                        MessageBox.Show("未定义的标记位。");
                        return;
                    }


                    dt.Rows[0][strField] = e.Label;

                    string sql = string.Format("select * from {0} where {1}", strTableName, filter);
                    TableHelper.SaveTable(dt, sql, m_Conn);

                    e.Node.EndEdit(false);
                    this.treeView1.LabelEdit = false;

            }
        }
    }
}

