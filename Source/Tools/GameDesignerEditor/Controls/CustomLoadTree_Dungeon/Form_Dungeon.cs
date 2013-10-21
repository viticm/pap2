using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;

namespace CustomLoadTree_Dungeon
{
    public partial class Form_Dungeon : Form
    {
        public Form_Dungeon()
        {
            InitializeComponent();
        }
        public Form_Dungeon(Class1 cls, SqlConnection conn, bool isReadOnly)
        {
            InitializeComponent();

            m_Conn = conn;
            m_ParentClass = cls;
            m_strTableName = "tbl_Guide_Dungeon";
        }

        string m_strTableName;
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
            Tree.Nodes.Clear();
            TreeNode DefNode = Tree.Nodes.Add("默认");
            DefNode.Tag = new Object[] { -1, -1 };

            string filter = "ClassID > -1 and MapID > -1";
            string order = "ClassID";
            
            DataTable dt = TableHelper.GetDataTableProxy(m_strTableName, filter, order, m_Conn);
            DataRow RootRow = dt.Rows.Find(new Object[] { 0, 0 });
            TreeNode RootNode = Tree.Nodes.Add(RootRow["ClassName"].ToString());
            RootNode.Tag = new Object[] { 0, 0 };

            foreach (DataRow rowL1 in dt.Rows)
            {
                int nClassID = Int32.Parse(rowL1["ClassID"].ToString());
                int nMapID = Int32.Parse(rowL1["MapID"].ToString());
                
                if (nClassID > 0 && nMapID == 0)
                {
                    TreeNode NodeL1 = RootNode.Nodes.Add(rowL1["ClassName"].ToString());
                    NodeL1.Tag = new Object[] { nClassID, nMapID };

                    foreach (DataRow rowL2 in dt.Rows)
                    {
                        int ClassID = Int32.Parse(rowL2["ClassID"].ToString());
                        int MapID = Int32.Parse(rowL2["MapID"].ToString());

                        if (ClassID == nClassID && MapID > 0)
                        {
                            TreeNode NodeL2 = NodeL1.Nodes.Add(GetMapNameByID(MapID));
                            NodeL2.Tag = new Object[] { ClassID, MapID };
                        }
                    }
                }
            }
            
            return treeView1;
        }

        private string GetMapNameByID(int nMapID)
        {
            string filter = string.Format("ID = {0}", nMapID);
            DataTable dt = TableHelper.GetDataTableProxy("maplist", filter, null, m_Conn);

            if (dt.Rows.Count == 0)
                return string.Empty;
            else
                return dt.Rows[0]["Name"].ToString();
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

           // string[] Strings = GetStringsFromTagMark(Tags[0].ToString());

            string strKey1 = "ClassID";
            string strKey2 = "MapID";

            string filter = string.Format("{0} = {1} and {2} = {3}", strKey1, Tags[0], strKey2, Tags[1]);
            DataTable dt = TableHelper.GetDataTableProxy(m_strTableName, filter, null, m_Conn);

            dt.Rows[0].Delete();
            string sql = "select * from " + m_strTableName + " where " + filter;
            TableHelper.SaveTable(dt, sql, m_Conn);
        }

        //public string[] GetStringsFromTagMark(string strMark)
        //{
        //    string strTabName = string.Empty;
        //    string strKeys1 = string.Empty;
        //    string strKeys2 = string.Empty;

        //    if (strMark == "ACTIVITY")
        //    {
        //        strTabName = "tbl_Guide_ActivityInfo";
        //        strKeys1 = "ClassID";
        //        strKeys2 = "ActivityID";
        //    }
        //    else if (strMark == "DAILYQUEST")
        //    {
        //        strTabName = "tbl_Guide_DailyQuestInfo";
        //        strKeys1 = "TypeID";
        //        strKeys2 = "QuestID";
        //    }
        //    else if (strMark == "-1")
        //    {

        //    }
        //    else
        //    {
        //        MessageBox.Show(strMark + " -- 异常的标记位");
        //    }

        //    return new string[] { strTabName, strKeys1, strKeys2 };
        //}

        private void buttonItemAdd_Click(object sender, EventArgs e)
        {

        }

        private void buttonItemAddChild_Click(object sender, EventArgs e)
        {
            Object[] Tags = PopupNode.Tag as Object[];
 
          //  string[] Strings = GetStringsFromTagMark(Tags[0].ToString());
            
            string strKey1 = "ClassID";
            string strKey2 = "MapID";


            if (PopupNode.Level == 0)
            {
                string filter = null;
                DataTable dt = TableHelper.GetDataTableProxy(m_strTableName, filter, strKey1 + " desc", m_Conn);

                int nMaxID = Int32.Parse(dt.Rows[0][strKey1].ToString());
                DataRow newRow = dt.NewRow();

                newRow[strKey1] = nMaxID + 1;
                newRow["ClassName"] = string.Format("新建{0}", nMaxID + 1);
                newRow[strKey2] = 0;

                dt.Rows.Add(newRow);
                string sql = "select * from " + m_strTableName + " order by " + strKey1 + " desc";
                TableHelper.SaveTable(dt, sql, m_Conn);

                TreeNode newNode = PopupNode.Nodes.Add(string.Format("新建{0}", nMaxID + 1));
                newNode.Tag = new Object[] { nMaxID + 1, 0 };
            }
            else
            {
                string filter = string.Format("{0} = {1}", strKey1, Tags[0]);
                DataTable dt = TableHelper.GetDataTableProxy(m_strTableName, filter, strKey2 + " desc", m_Conn);

                int nMaxID = Int32.Parse(dt.Rows[0][strKey2].ToString());
                if (nMaxID < 1024) //让他取不到地图名字
                {
                    nMaxID += 1024;
                }

                DataRow newRow = dt.NewRow();

                newRow[strKey1] = dt.Rows[0][strKey1];
                newRow[strKey2] = nMaxID + 1;

                dt.Rows.Add(newRow);
                string sql = "select * from " + m_strTableName + " where " + filter + " order by " + strKey2 + " desc";
                TableHelper.SaveTable(dt, sql, m_Conn);

                TreeNode newNode = PopupNode.Nodes.Add((nMaxID + 1).ToString());
                newNode.Tag = new Object[] { Tags[0], nMaxID + 1 };

            }

        }

        private void buttonItemRename_Click(object sender, EventArgs e)
        {
            if (PopupNode.Level != 1)
                return;
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

                string strKey1 = "ClassID";
                string strKey2 = "MapID";

                string filter = string.Format("{0} = {1} and {2} = {3}", strKey1, Tags[0], strKey2, Tags[1]);
                DataTable dt = TableHelper.GetDataTableProxy(m_strTableName, filter, null, m_Conn);
                string strField = string.Empty;

                strField = "ClassName";
                dt.Rows[0][strField] = e.Label;

                string sql = string.Format("select * from {0} where {1}", m_strTableName, filter);
                TableHelper.SaveTable(dt, sql, m_Conn);

                e.Node.EndEdit(false);
                this.treeView1.LabelEdit = false;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ReloadTree();
        }
    }
}

