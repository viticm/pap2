using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using System.Data;
using System.Data.SqlClient;

namespace OptimizeLoadTreeFinder
{
    class NodeFinder
    {
        private string FindField = string.Empty;
        private string FindValue = string.Empty;
        private bool FindByNodeText = false;
        private TreeView treeView = null;       
        private DataRowCollection FindResultRows = null;
        private DataRowCollection FindResultMapRows = null;
        private DataRowCollection FindResultModelRows = null;
        private SqlConnection Conn;
        private FindType findType;

        private int CurIndex = -1;

        const string RepresentIdField = "RepresentId";
        const string AnimationIDField = "AnimationID";
        const string mapField = "map";
        const string ModelField = "Model";

        const string npcTableName = "npc";
        const string animation_npcTableName = "animation_npc";

        enum FindType {FindRepresentId, FindAnimationID, FindNodeText, };


        bool bHasErr = false;


        public void Init(string strField, string strValue, bool bFindByNodeText, TreeView tree, SqlConnection conn)
        {
            if (FindField != strField || FindValue != strValue || FindByNodeText != bFindByNodeText)
            {
                FindField = strField;
                FindValue = strValue;
                treeView = tree;
                Conn = conn;
                FindByNodeText = bFindByNodeText;

                CurIndex = -1;
                FindResultRows = null;
                FindResultMapRows = null;
                FindResultModelRows = null;
                bHasErr = !ReInit();
            }            
        
        }
        private bool ReInit()
        {
            string sql = string.Empty;
            DataTable dt = null;

            if (FindByNodeText)
            {
                findType = FindType.FindNodeText;

                sql = string.Format("select distinct [{0}] from {1} where [{0}] like '%{2}%' order by [{0}]", mapField, animation_npcTableName, FindValue);
                dt = GetDataTable(sql, Conn);
                if (dt == null)
                {
                    return false;
                }
                FindResultMapRows = dt.Rows;

                sql = string.Format("select [{0}], {1}, {2} from {3} where {1} like '%{4}%' order by [{0}], {1}", mapField, ModelField, RepresentIdField, npcTableName, FindValue);
                dt = GetDataTable(sql, Conn);
                if (dt == null)
                {
                    return false;
                }
                FindResultModelRows = dt.Rows;

            }
            else if (FindField.ToLower() == RepresentIdField.ToLower())
            {
                findType = FindType.FindRepresentId;

                sql = string.Format("select [{0}], {1} from {2} where {1} = {3} order by [{0}], {1} ", mapField, RepresentIdField, animation_npcTableName, FindValue);
                dt = GetDataTable(sql, Conn);
                if (dt == null)
                {
                    return false;
                }
                FindResultRows = dt.Rows;
            }
            else
            {
                findType = FindType.FindAnimationID;

                sql = string.Format("select [{0}], {1}, {2} from {3} where {2} = {4} order by [{0}], {1}, {2} ", mapField, RepresentIdField, AnimationIDField, animation_npcTableName, FindValue);
                dt = GetDataTable(sql, Conn);
                if (dt == null)
                {
                    return false;
                }
                FindResultRows = dt.Rows;
            }

            return true;
        }

        public void FindNext()
        {
            if (bHasErr)
            {
                return;
            }
            switch (findType)
            {
                case FindType.FindRepresentId:
                    FindNextRepresentId();
                    break;
                case FindType.FindAnimationID:
                    FindNextAnimationID();
                    break;
                case FindType.FindNodeText:
                    FindNextNodeText();
                    break;
                default:
                    break;
            } 
        }
        //查找下一个表现id
        private void FindNextRepresentId()
        {
            if (!FindTip(FindResultRows.Count))
            {
                return;
            }
            CurIndex = (CurIndex + 1) % FindResultRows.Count;
            string mapName = FindResultRows[CurIndex][mapField].ToString();
            int RepresentId = Convert.ToInt32(FindResultRows[CurIndex][RepresentIdField]);

            TreeNode node = GetChildNode(treeView.Nodes, mapName);
            if (node == null)
            {
                return;
            }
            node.Expand();
            node.Collapse();
            node = GetChildNode(node.Nodes, RepresentId);
            if (node == null)
            {
                return;
            }
            node.Expand();
            treeView.SelectedNode = node;

        }
        //查找下一个动作id
        private void FindNextAnimationID()
        {
            if (!FindTip(FindResultRows.Count))
            {
                return;
            }
            CurIndex = (CurIndex + 1) % FindResultRows.Count;
            string mapName = FindResultRows[CurIndex][mapField].ToString();
            int RepresentId = Convert.ToInt32(FindResultRows[CurIndex][RepresentIdField]);
            int AnimationID = Convert.ToInt32(FindResultRows[CurIndex][AnimationIDField]);

            TreeNode node = GetChildNode(treeView.Nodes, mapName);
            if (node == null)
            {
                return;
            }
            node.Expand();
            node.Collapse();
            node = GetChildNode(node.Nodes, RepresentId);
            if (node == null)
            {
                return;
            } 
            node.Expand();
            node.Collapse();
            node = GetChildNode(node.Nodes, RepresentId, AnimationID);
            if (node == null)
            {
                return;
            }
            node.Expand();           
            treeView.SelectedNode = node;
        }
        //查找下一个节点文本
        private void FindNextNodeText()
        {
            if (!FindTip(FindResultMapRows.Count + FindResultModelRows.Count))
            {
                return;
            }
            CurIndex = (CurIndex + 1) % (FindResultMapRows.Count + FindResultModelRows.Count);
          
            if (CurIndex < FindResultMapRows.Count && FindResultMapRows.Count > 0)
            {
                string mapName = FindResultMapRows[CurIndex][mapField].ToString();
                TreeNode node = GetChildNode(treeView.Nodes, mapName);
                if (node == null)
                {
                    return;
                }
                node.Expand();
                treeView.SelectedNode = node;
            }
            else
            {
                int modelRowIndex = CurIndex - FindResultMapRows.Count;
                string mapName = FindResultModelRows[modelRowIndex][mapField].ToString();
                //string modelName = FindResultModelRows[modelRowIndex][ModelField].ToString();
                int RepresentId = Convert.ToInt32(FindResultModelRows[modelRowIndex][RepresentIdField]);

                TreeNode node = GetChildNode(treeView.Nodes, mapName);
                if (node == null)
                {
                    return;
                }
                node.Expand();
                node.Collapse();
                node = GetChildNode(node.Nodes, RepresentId);
                if (node == null)
                {
                    return;
                }
                node.Expand();
                treeView.SelectedNode = node;
            }
            

        }

        //找Text等于指定文本的节点
        private TreeNode GetChildNode(TreeNodeCollection Nodes, string childText)
        {
            TreeNode nodeRet = null;

            foreach (TreeNode node in Nodes)
            {
                if (node.Text.ToString().Trim().ToLower() == childText.Trim().ToLower())
                {
                    nodeRet = node;
                    break;
                }
            }
            return nodeRet;
        }
        //找指定表现id的节点
        private TreeNode GetChildNode(TreeNodeCollection Nodes, int tagRepresentId)
        {
            TreeNode nodeRet = null;
           
            foreach (TreeNode node in Nodes)
            {
                if (Convert.ToInt32(node.Tag) == tagRepresentId)
                {
                    nodeRet = node;
                    break;
                }
            }
            return nodeRet;
        }
        //找指定表现id和动作id的节点
        private TreeNode GetChildNode(TreeNodeCollection Nodes, int tagRepresentId, int tagAnimationID)
        {
            TreeNode nodeRet = null;
            object[] tag = null;

            foreach (TreeNode node in Nodes)
            {
                tag = node.Tag as object[];
                if (Convert.ToInt32(tag[0]) == tagRepresentId && Convert.ToInt32(tag[1]) == tagAnimationID)
                {
                    nodeRet = node;
                    break;
                }
            }
            return nodeRet;
        }
        //查找结果提示
        private bool FindTip(int findCnt)
        {
            if (findCnt == 0)
            {
                MessageBox.Show("没有符合条件的节点");
                return false;
            }
            return true;
        }



        // db相关
        public System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
    }
}
