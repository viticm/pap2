using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;
using System.Data.SqlClient;
using System.Collections;

namespace CustomLoadTree_Activity
{
    class Helper
    {
        private const string SubsequenceMark = "↑"; // ↑┛▲△＾∧
        public static SqlConnection Conn;
        public static Hashtable adp_hash;

        public static SqlDataAdapter GetAdp(string sql)
        {
            if (adp_hash[sql] == null)
            {
                adp_hash[sql] = new SqlDataAdapter(sql, Conn);
            }
            return (SqlDataAdapter)(adp_hash[sql]);
        }

        public static DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = GetAdp(sql);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }

                /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        public static DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }

            }

            return table;
        }

        public static object[] GetUnusedID(string tblname, ref object[] StartIDs, SqlConnection Conn)
        {
            string sql = string.Format("SELECT * FROM {0}", tblname);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            while (tbl.Rows.Find(StartIDs) != null)
            //while (tbl.Rows.Find(StartIDs[0]) != null)
            {
                StartIDs[0] = Convert.ToInt32(StartIDs[0]) + 1;
            }
            SetLastID(tblname, tbl.PrimaryKey[0].ColumnName, Convert.ToInt32(StartIDs[0]), Conn);
            return StartIDs;
        }

        public static void SetLastID(string tblname, string field, int id, SqlConnection Conn)
        {
            string sql = string.Format("SELECT * FROM sys_id_pool WHERE tablename='{0}' AND fieldname='{1}'", tblname, field);
            DataTable tbl = Helper.GetDataTable(sql, Conn);

            DataRow row = null;
            try
            {
                SqlDataAdapter adp = GetAdp(sql);
                if (tbl.Rows.Count <= 0)
                {
                    row = tbl.NewRow();
                    row.BeginEdit();
                    row["tablename"] = tblname;
                    row["fieldname"] = field;
                    row["lastid"] = id;
                    row.EndEdit();
                    tbl.Rows.Add(row);

                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                }
                else
                {
                    row = tbl.Rows[0];
                    row["lastid"] = id;

                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                }

                int val = adp.Update(tbl);
                tbl.AcceptChanges();
            }
            catch (Exception ex)
            {
                string err = string.Format("在 SetLastID() 时发生错误。\r\n\r\n错误信息为：{0}", ex.Message);
                MessageBox.Show(err, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                tbl.RejectChanges();
            }
        }

        public static DataRow SqlInsertRecord(ref DataTable tbl, string tblname, object keys, SqlConnection Conn)
        {
            DataRow newRow = tbl.NewRow();

            // 获得默认行
            DataRow defaultRow = null;
            if (tbl.PrimaryKey.Length > 0)
            {
                object[] default_keys = new object[tbl.PrimaryKey.Length];
                for (int i = 0; i < default_keys.Length; i++)
                {
                    default_keys[i] = 0;
                }
                defaultRow = tbl.Rows.Find(default_keys);
            }
            else
            {
                string err = string.Format("在调用函数 SqlInsertRecord() 时发生错误，数据表没有主键。");
                MessageBox.Show(err, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return null;
            }
            if (defaultRow == null)
                defaultRow = tbl.Rows[0];

            // 编辑此行
            newRow.BeginEdit();
            foreach (DataColumn col in tbl.Columns)
                newRow[col] = defaultRow[col];
            for (int i = 0; i < tbl.PrimaryKey.Length; i++)
            {
                if (keys is object[])
                {
                    newRow[tbl.PrimaryKey[i]] = ((object[])keys)[i];
                }
                else
                {
                    MessageBox.Show("函数：SqlInsertRecord() 遇到错误的输入参数 keys, 需要的是 object[] ()。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            newRow.EndEdit();
            tbl.Rows.Add(newRow);

            // 提交
            string key = (((object[])keys)[0]).ToString();
            SqlDataAdapter adp = GetAdp("SELECT * FROM " + tblname + " WHERE QuestID=" + key);
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();

            return newRow;
        }

        public static void SqlUpdateRecord(string tblname, object keys, string[] fields, object[] values, SqlConnection Conn)
        {
            if(Conn.State == ConnectionState.Closed)
            {
                Conn.Open();
            }

            string strKey = ((object[])keys)[0].ToString();
            SqlCommand cmd = Conn.CreateCommand();

            for (int i = 0; i < fields.Length; i++)
            {
                string value = values[i] == DBNull.Value ? "NULL"
                                                         : ((values[i] is string) ? ((values[i].ToString() == string.Empty) ? "''"
                                                                                                                            : "'" + values[i] + "'")
                                                                                  : ((values[i] is bool) ? (Convert.ToBoolean(values[i]) ? "'1'"
                                                                                                                                         : "'0'")
                                                                                                         : values[i].ToString())); 
                cmd.CommandText = string.Format("UPDATE {0} SET {1}={2} WHERE QuestID={3}", tblname, fields[i], value, strKey);
                cmd.ExecuteNonQuery();
            }

            if(Conn.State == ConnectionState.Open)
            {
                Conn.Close();
            }
        }

        public static void SqlUpdateRecordAs(ref DataTable tbl, string tblname, object src_keys, object des_keys, string[] cols_remain, SqlConnection Conn)
        {
            DataRow des_row = tbl.Rows.Find((object[])des_keys);
            DataRow src_row = tbl.Rows.Find((object[])src_keys);

            foreach (DataColumn col in tbl.Columns)
            {
                bool bRemain = false;
                foreach (string sr in cols_remain)
                {
                    if (col.ColumnName == sr)
                    {
                        bRemain = true;
                        break;
                    }
                }
                if (!bRemain)
                {
                    des_row[col.ColumnName] = src_row[col.ColumnName];
                }
            }

            string key = (((object[])des_keys)[0]).ToString();

            SqlDataAdapter adp = GetAdp("SELECT * FROM " + tblname + " WHERE QuestID=" + key);
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            DataRow[] rowsToUpdate = new DataRow[1];
            rowsToUpdate[0] = des_row;
            int val = adp.Update(rowsToUpdate);
            tbl.AcceptChanges();
        }

        public static TreeNode FindAncestorNode(TreeNode node)
        {
            TreeNode tra_node = node;
            while (tra_node.Parent != null)
            {
                tra_node = tra_node.Parent;
            }
            return tra_node;
        }

        public static bool IsAncestorOffspring(TreeNode father, TreeNode child)
        {
            TreeNode parent = child.Parent;
            while (parent != null)
            {
                if (parent == father)
                    return true;
                parent = parent.Parent;
            }
            return false;
        }

        public static ArrayList FindAllActualNodeByName(TreeView tree, string name)
        {
            ArrayList lsRet = new ArrayList();
            foreach (TreeNode node in tree.Nodes)
            {
                FindAllActualNodeByNameWithinNode(node, name, ref lsRet);
            }
            return lsRet;
        }

        public static void FindAllActualNodeByNameWithinNode(TreeNode node, string name, ref ArrayList ls)
        {
            int key = Convert.ToInt32(((object[])(node.Tag))[0]);
            int fatherkey = -100;
            if (node.Parent != null)
                fatherkey = Convert.ToInt32(((object[])(node.Parent.Tag))[0]);
            string questname = node.Text.Contains(SubsequenceMark) ? node.Text.Remove(node.Text.Length - SubsequenceMark.Length) : node.Text;

            if (key > 0 && questname.Contains(name) && fatherkey != -3 && fatherkey != -4)
            {
                ls.Add(node);
            }
            foreach (TreeNode n in node.Nodes)
            {
                FindAllActualNodeByNameWithinNode(n, name, ref ls);
            }
        }

        public static ArrayList FindAllActualNodeByID(TreeView tree, int id)
        {
            ArrayList lsRet = new ArrayList();
            foreach (TreeNode node in tree.Nodes)
            {
                FindAllActualNodeByIDWithinNode(node, id, ref lsRet);
            }
            return lsRet;
        }

        public static void FindAllActualNodeByIDWithinNode(TreeNode node, int id, ref ArrayList ls)
        {
            int key = Convert.ToInt32(((object[])(node.Tag))[0]);
            int fatherkey = -100;
            if (node.Parent != null)
                fatherkey = Convert.ToInt32(((object[])(node.Parent.Tag))[0]);

            if (key > 0 && key == id && fatherkey != -3 && fatherkey != -4)
            {
                ls.Add(node);
            }
            foreach (TreeNode n in node.Nodes)
            {
                FindAllNodesByIDWithinNode(n, id, ref ls);
            }
        }

        public static ArrayList FindAllNodesByID(TreeView tree, int id)
        {
            ArrayList lsRet = new ArrayList();
            foreach (TreeNode node in tree.Nodes)
            {
                FindAllNodesByIDWithinNode(node, id, ref lsRet);
            }
            return lsRet;
        }

        public static void FindAllNodesByIDWithinNode(TreeNode node, int id, ref ArrayList ls)
        {
            int key = Convert.ToInt32(((object[])(node.Tag))[0]);
            if (key > 0 && key == id)
            {
                ls.Add(node);
            }
            foreach (TreeNode n in node.Nodes)
            {
                FindAllNodesByIDWithinNode(n, id, ref ls);
            }
        }

        public static TreeNode FindNodeByName(TreeNode node, string name, ArrayList AlreadyFound)
        {
            int key = Convert.ToInt32(((object[])(node.Tag))[0]);
            string questname = node.Text.Contains(SubsequenceMark) ? node.Text.Remove(node.Text.Length - SubsequenceMark.Length) : node.Text;
            int parent_key = -100;
            if (node.Parent != null)
                parent_key = Convert.ToInt32(((object[])(node.Parent.Tag))[0]);
            bool bIsActualNode = key > 0 && parent_key != -3 && parent_key != -4;

            if (bIsActualNode && questname.Contains(name))
            {
                if (AlreadyFound.IndexOf(node) == -1)
                    return node;
            }
            else
            {
                foreach (TreeNode n in node.Nodes)
                {
                    TreeNode found = FindNodeByName(n, name, AlreadyFound);
                    if (found != null)
                        if (AlreadyFound.IndexOf(found) == -1)
                            return found;
                }
            }
            return null;
        }

        public static int SqlRemoveRecords(string tblname, ArrayList keys, SqlConnection Conn)
        {
            if(keys.Count == 0) // 要删除的记录为空
            {
                return 0;
            }

            string[] strKeys = new string[keys.Count];
            for (int i = 0; i < keys.Count; i++)
            {
                object[] _tag = (object[])(keys[i]);
                strKeys[i] = (_tag[0]).ToString();
            }

            if(Conn.State == ConnectionState.Closed)
            {
                Conn.Open();
            }
            
            SqlCommand cmd = Conn.CreateCommand();

            // 删除要删除的记录们
            cmd.CommandText = "DELETE FROM " + tblname + " WHERE ";
            for (int i = 0; i < strKeys.Length; i++)
            {
                cmd.CommandText += "QuestID=";
                cmd.CommandText += strKeys[i];
                cmd.CommandText += " OR ";
            }
            cmd.CommandText = cmd.CommandText.Remove(cmd.CommandText.Length - 4); // 去掉多余的 “ OR ”
            cmd.ExecuteNonQuery();

            // 把所有 前续任务字段=要删除的记录id 的地方清空
            for (int i = 0; i < strKeys.Length; i++)
            {
                for (int j = 1; j <= 4; j++)
                {
                    cmd.CommandText = string.Format("UPDATE {0} SET PrequestID{1}=NULL WHERE PrequestID{2}={3}",
                        tblname, j, j, strKeys[i]);
                    cmd.ExecuteNonQuery();
                }
            }

            if(Conn.State == ConnectionState.Open)
            {
                Conn.Close();
            }

            return strKeys.Length;
        }

        public static string GetIPAddress()
        {
            string ip = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList[0].ToString();
            return ip;
        }

        public static string GetHostName()
        {
            return System.Net.Dns.GetHostName();
        }

        public static TreeNode FindQuestNode(int quest_id, TreeView Tree) // 在树中找quest的对应结点：看树的tag，数组长度是否为1，数字是否对应。
        {
            foreach (TreeNode node in Tree.Nodes)
            {
                TreeNode found_node = FindNode(node, quest_id);
                if (found_node != null)
                    return found_node;
            }
            return null;
        }

        public static TreeNode FindNode(TreeNode node, int quest_id)
        {
            int father_id = -100;
            if (node.Parent != null)
                father_id = Convert.ToInt32(((object[])(node.Parent.Tag))[0]);

            object[] node_tags = (object[])(node.Tag);
            if (node_tags.Length == 1 && Convert.ToInt32(node_tags[0]) == quest_id && father_id != -3 && father_id != -4)
            {
                return node;
            }
            else
            {
                foreach (TreeNode n in node.Nodes)
                {
                    TreeNode n_f = FindNode(n, quest_id);
                    if (n_f != null)
                        return n_f;
                }
            }
            return null;
        }

        public static TreeNode FindCatNode(string value, TreeView Tree) // 在树中根据分类值找到分类字段，都在第一层。
        {
            foreach (TreeNode node in Tree.Nodes)
            {
                object[] keys = (object[])(node.Tag);
                if (keys[0].ToString() == "-2" && node.Text == value)
                {
                    return node;
                }
            }
            return null;
        }

        public static TreeNode FindSetNode(TreeNode fathernode, bool prequest_logic, int prequest_count, int[] prequest_ids) // 找包含指定镜像的集合型结点，即“完成其一”，“全部完成”
        {
            string prequest_type = prequest_logic ? "-3" : "-4";

            foreach (TreeNode node in fathernode.Nodes)
            {
                if (((object[])node.Tag)[0].ToString() == prequest_type)
                {
                    bool AllIdMatch = true;
                    for (int i = 0; i < prequest_count; i++)
                    {
                        bool HasThisPrequest = false;
                        foreach (TreeNode n in node.Nodes)
                        {
                            int key = Convert.ToInt32(((object[])n.Tag)[0]);
                            if (key != prequest_ids[i])
                            {
                                HasThisPrequest = true;
                                break;
                            }
                        }
                        if (!HasThisPrequest)
                        {
                            AllIdMatch = false;
                            break;
                        }
                    }

                    if (AllIdMatch)
                    {
                        return node;
                    }
                }
            }

            return null;
        }
    }
}
