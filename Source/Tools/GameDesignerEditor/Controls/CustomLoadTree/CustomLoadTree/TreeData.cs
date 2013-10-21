using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Collections;

namespace CustomLoadTree
{
    /// <summary>
    /// 树结点序号处理类
    /// </summary>
    class TreeData
    {
        private SqlConnection conn; // sql连接
        private Hashtable table = new Hashtable(); // 树结点与序号的哈希表

        /// <summary>
        /// 构造函数
        /// </summary>
        public TreeData()
        {
        }

        /// <summary>
        /// 初始化第一层树结点的顺序
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="tree">树</param>
        public void InitTreeOrder(SqlConnection conn, TreeView tree)
        {
            int index = 0; // 结点序号
            this.conn = conn; // 初始化sql连接
            if (conn.State != System.Data.ConnectionState.Open) // 打开连接
            {
                conn.Open();
            }

            foreach (TreeNode node in tree.Nodes)
            {
                InitTreeOrder(node, "", index);
                index++;
            }
            if (conn.State != System.Data.ConnectionState.Closed) // 关闭连接
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 初始化非第一层树结点的顺序
        /// </summary>
        /// <param name="node">当前结点</param>
        /// <param name="fatherIndex">父结点的序号</param>
        /// <param name="index">结点的子序号</param>
        private void InitTreeOrder(TreeNode node, string fatherIndex, int index)
        {
            string questID = ((object[])node.Tag)[0].ToString().Trim(); // 结点的ID值
            string questIndex = index.ToString(); // 结点的序号

            for (int i = 0; i < 3 - index.ToString().Length; i++)
            {
                questIndex = "0" + questIndex;
            }
            questIndex = fatherIndex + questIndex;

            try
            {
                //SqlCommand cmd = conn.CreateCommand();
                //cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "sys_quests_order", "QuestIndex", questIndex, questID);
                //cmd.ExecuteNonQuery();
                table.Add(node, questIndex); // 将信息添加入hashtable
            }
            catch (Exception ex)
            {
                MessageBox.Show("初始化树结点序号时产生异常: " + ex.ToString());
            }

            int index1 = 0; // 子结点的序号
            foreach (TreeNode node1 in node.Nodes)
            {
                InitTreeOrder(node1, questIndex, index1);
                index1++;
            }
        }

        /// <summary>
        /// 重设同级结点的顺序
        /// </summary>
        /// <param name="node">当前结点</param>
        public void ResetOrder(TreeNode node)
        {
            TreeNode fatherNode = node.Parent; // 当前结点的父亲结点
            string fatherIndex = table[fatherNode].ToString(); // 父亲结点的序号
            int index = 0; // 结点的序号
            SqlCommand cmd;

            try
            {
                if (conn.State != System.Data.ConnectionState.Open) // 打开连接
                {
                    conn.Open();
                }
                foreach (TreeNode node1 in fatherNode.Nodes)
                {
                    string questIndex = index.ToString(); // 结点的序号
                    string questID = ((object[])node1.Tag)[0].ToString().Trim(); // 结点的ID值
                    for (int i = 0; i < 3 - index.ToString().Length; i++)
                    {
                        questIndex = "0" + questIndex;
                    }
                    questIndex = fatherIndex + questIndex;
                    cmd = conn.CreateCommand();
                    cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "sys_quests_order", "QuestIndex", questIndex, questID);
                    cmd.ExecuteNonQuery();
                    table[node1] = questIndex;
                    ResetOrder(node1, questIndex, index);
                    index++;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("重设同级结点顺序时产生异常: " + ex.ToString());
            }
            if (conn.State != System.Data.ConnectionState.Closed) // 关闭连接
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 重设子结点的顺序
        /// </summary>
        /// <param name="node">初始结点</param>
        /// <param name="fatherIndex">父亲结点的序号</param>
        /// <param name="index">结点的子序号</param>
        private void ResetOrder(TreeNode node, string fatherIndex, int index)
        {
            string questID = ((object[])node.Tag)[0].ToString().Trim(); // 结点的ID值
            string questIndex = index.ToString(); // 结点的序号

            for (int i = 0; i < 3 - index.ToString().Length; i++)
            {
                questIndex = "0" + questIndex;
            }
            questIndex = fatherIndex + questIndex;

            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "sys_quests_order", "QuestIndex", questIndex, questID);
                cmd.ExecuteNonQuery();
                table[node] = questIndex;
            }
            catch (Exception ex)
            {
                MessageBox.Show("初始化树结点序号时产生异常: " + ex.ToString());
            }

            int index1 = 0; // 子节点的序号
            foreach (TreeNode node1 in node.Nodes)
            {
                ResetOrder(node1, questIndex, index1);
                index1++;
            }
        }

        /// <summary>
        /// 获取结点的序号
        /// </summary>
        /// <param name="node">需要获得序号的结点</param>
        /// <returns></returns>
        public string GetQuestIndex(TreeNode node)
        {
            if (table[node] == null)
            {
                return null;
            }
            return table[node].ToString();
        }

        /// <summary>
        /// 重设sql连接
        /// </summary>
        /// <param name="conn">sql连接</param>
        public void ResetConnection(SqlConnection conn)
        {
            this.conn = conn;
        }

        /// <summary>
        /// 添加数据
        /// </summary>
        /// <param name="id">要添加记录的ID号</param>
        public void AddData(string id)
        {
            try
            {
                if (conn.State != System.Data.ConnectionState.Open) // 打开连接
                {
                    conn.Open();
                }

                if (!CheckIDExist(id)) // id未存在
                {
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", "sys_quests_order", "QuestID", id);
                    cmd.ExecuteNonQuery();
                }

                if (conn.State != System.Data.ConnectionState.Closed) // 关闭连接
                {
                    conn.Close();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("在添加结点序号数据时产出异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 添加分区
        /// </summary>
        /// <param name="node">分区结点</param>
        public void AddSection(TreeNode node)
        {
            TreeNode preNode = node.PrevNode; // 前一个分区结点
            if(preNode != null)
            {
                string indexString = GetQuestIndex(preNode);
                if(indexString != null)
                {
                    int index = int.Parse(indexString);
                    index++;
                    table[node] = index.ToString();
                }
            }
            else
            {
                table[node] = "0";
            }
        }

        /// <summary>
        /// 检查id是否已经存在
        /// </summary>
        /// <param name="id">检测的ID号</param>
        /// <returns></returns>
        private bool CheckIDExist(string id)
        {
            string sql = string.Format("SELECT QuestID FROM sys_quests_order WHERE QuestID = {0}", id);
            SqlCommand cmd;
            SqlDataReader reader = null;
            bool result = true;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if (!reader.HasRows) // 查询结果为空
                {
                    result = false;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("检查ID是否存在时产生异常: " + ex.ToString());
            }
            finally
            {
                if (!reader.IsClosed) // reader尚未关闭
                {
                    reader.Close();
                }
            }
            return result;
        }

        /// <summary>
        /// 检测两个表中的ID是否匹配
        /// </summary>
        /// <param name="conn">sql连接</param>
        public void CheckIDCompare(SqlConnection conn)
        {
            try
            {
                if (conn.State != System.Data.ConnectionState.Open) // 打开连接
                {
                    conn.Open();
                }

                string sql = "insert into sys_quests_order (questid) select questid from tbl_quests where questid not in (select questid from sys_quests_order)";
                SqlCommand cmd = new SqlCommand(sql, conn);
                cmd.ExecuteNonQuery(); 
             
            }
            catch (Exception ex)
            {
                MessageBox.Show("检查两个表中QuestID是否匹配时产生异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State != System.Data.ConnectionState.Closed) // 关闭连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 删除ID序号记录
        /// </summary>
        /// <param name="id">要删除记录的ID号</param>
        public void RemoveRecord(string id)
        {
            try
            {
                if (conn.State != System.Data.ConnectionState.Open) // 打开连接
                {
                    conn.Open();
                }
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("DELETE FROM {0} WHERE {1} = {2}", "sys_quests_order", "QuestID", id);
                cmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                MessageBox.Show("在删除ID序号记录时产生异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State != System.Data.ConnectionState.Closed) // 关闭连接
                {
                    conn.Close();
                }
            }
        }
    }
}
