using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace ChooseItem
{
    public partial class MultiAddForm : Form
    {
        private TreeNode node; // 树结点
        private SqlConnection conn; // sql连接
        private List<string> existAttributes = new List<string>(); // 已经存在的属性
        private List<string> totalAttributes = new List<string>(); // 总的属性
        private List<string> addedAttributes = new List<string>(); // 已经添加的属性
        private string fileName; // ini文件名称
        private DataTable dt; // 数据表
        private TreeNode findNode; // 找到的树结点
        private Hashtable table = new Hashtable(); // 属性与相关信息的hash表
        private Hashtable detailTable = new Hashtable(); // detail与相关信息的hash表
        private string itemTable; // 物品表名
        private string dropTable; // 掉落率表名 
        private int catLength = 0; // 分类长度
        private bool autoResetItemName; // 是否自动重设道具名称

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="node">当前树结点</param>
        /// <param name="itemTable">物品表名</param>
        /// <param name="dropTable">掉落率表名</param>
        /// <param name="autoResetItemName">是否自动重设道具名称</param>
        public MultiAddForm(SqlConnection conn, TreeNode node, string itemTable, string dropTable, bool autoResetItemName)
        {
            this.conn = conn;
            while(node != null && node.Level != 0)
            {
                node = node.Parent;
            }
            this.node = node;
            this.autoResetItemName = autoResetItemName;
            this.itemTable = itemTable;
            this.dropTable = dropTable;
            fileName = node.Text;
            InitializeComponent();
            FillDetailTable();
            FillTotalAttributes();
            FillExistAttributes();
            FillLists();
        }

        /// <summary>
        /// 填充detail与相关信息的hash表
        /// </summary>
        private void FillDetailTable()
        {
            detailTable["1"] = "近身武器";
            detailTable["2"] = "远程武器";
            detailTable["3"] = "衣服";
            detailTable["4"] = "戒指";
            detailTable["5"] = "项链";
            detailTable["6"] = "护身符";
            detailTable["7"] = "鞋子";
            detailTable["8"] = "腰带";
            detailTable["9"] = "帽子";
            detailTable["10"] = "护腕手镯";
            detailTable["11"] = "玉佩香囊";
        }

        /// <summary>
        /// 填充列表
        /// </summary>
        public void FillLists()
        {
            foreach(string s in totalAttributes)
            {
                if(existAttributes.Contains(s)) // 填充已选择属性列表
                {
                    listBox2.Items.Add(s);
                    addedAttributes.Add(s);

                    // 修改树结点的选择状态
                    foreach (TreeNode node1 in treeView1.Nodes)
                    {
                        GetNode(node1, s);
                    }  

                    if (findNode != null)
                    {
                        findNode.Checked = true;
                        findNode = null;
                    }  
                }
            }
        }

        /// <summary>
        /// 填充所有属性
        /// </summary>
        private void FillTotalAttributes()
        {
            try
            {
                string sql = string.Format("SELECT * FROM {0}", itemTable);
                SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                DataTable tbl = ds.Tables[0];
                dt = tbl;

                // 记录属性和附加信息
                foreach(DataRow r in tbl.Rows)
                {
                    object o = r["Name"].ToString();
                    string[] information = new string[4];
                    information[0] = r["Genre"].ToString();
                    information[1] = r["DetailType"].ToString();
                    information[2] = r["ParticularType"].ToString();
                    information[3] = r["Level"].ToString();
                    totalAttributes.Add(r["Name"].ToString());
                    table[o] = information;
                }

                // 新的添加方法
                string[] catFields = GetCatfields("dropitem");
                foreach (DataRow r in tbl.Rows)
                {
                    TreeNodeCollection currentNodes = treeView1.Nodes; // 当前树结点集合
                    string name = r["Name"].ToString();

                    foreach (string c in catFields)
                    {
                        bool find = false;
                        TreeNode findNode = null;
                        string catName = r[c].ToString().Trim();
                        if(c == "DetailType") // 将DetailType分类替换为中文名
                        {
                            catName = detailTable[catName].ToString();
                        }

                        foreach (TreeNode node2 in currentNodes)
                        {
                            if (node2.Text == catName)
                            {
                                find = true;
                                findNode = node2;
                                break;
                            }
                        }

                        if (!find)
                        {
                            
                            findNode = currentNodes.Add(catName);
                        }

                        currentNodes = findNode.Nodes;
                    }

                    TreeNode addNode = currentNodes.Add(name);
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("批量增加删除物品信息时产生sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取树结点的分类信息
        /// </summary>
        /// <param name="itemTable">表名</param>
        /// <returns>分类信息</returns>
        private string[] GetCatfields(string itemTable)
        {
            string sql = string.Format("SELECT {0} FROM {1} WHERE {2} = \'{3}\'", "catfields", "sys_modl_tab_def",
                "tablename", itemTable);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string[] classifyNames = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while (reader.Read())
                {
                    if (reader[0].ToString().Trim() != "")
                    {
                        classifyNames = reader[0].ToString().Trim().Split(',');
                        catLength = classifyNames.Length; // 记录长度
                        break;
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("获取树结点分类信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(reader != null)
                {
                    reader.Close();
                }
            }
            return classifyNames;
        }

        /// <summary>
        /// 填充现有属性
        /// </summary>
        private void FillExistAttributes()
        {
            try
            {
                List<string[]> informations = new List<string[]>();
                string sql = string.Format("SELECT * FROM {0}", dropTable);
                SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                DataTable tbl = ds.Tables[0];

                foreach (TreeNode node1 in node.Nodes)
                {
                    string id = (node1.Tag as object[])[0].ToString();
                    if (node1.Text == "-1" || node1.Text == "无效物品")
                    {
                        continue;
                    }
                    DataRow r = tbl.Rows.Find(id);
                    string[] information = new string[6];
                    information[0] = r["Genre"].ToString();
                    information[1] = r["Detail"].ToString();
                    information[2] = r["Particular"].ToString();
                    information[3] = r["Level"].ToString();
                    information[4] = r["RandRate"].ToString();
                    information[5] = r["LuckyRate"].ToString();
                    informations.Add(information);
                }

                foreach (string[] s in informations)
                {
                    string[] t = new string[4];
                    for(int i = 0; i < 4; i++)
                    {
                        t[i] = s[i];
                    }
                    DataRow r = dt.Rows.Find(t);
                    if(r != null)
                    {
                        existAttributes.Add(r["Name"].ToString());
                        table[r["Name"].ToString()] = s; // 重新记录信息
                    }
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("批量增加删除物品信息时产生sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 删除数据
        /// </summary>
        /// <param name="id">id号</param>
        private void DeleteData(string id)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("DELETE FROM {0} WHERE {1} = {2}", dropTable, "id", id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("删除id为{0}的记录时产生sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 添加数据
        /// </summary>
        /// <param name="name">物品名称</param>
        /// <param name="label">掉落率序号</param>
        private void InsertData(string name, int label)
        {
            try
            {
                string[] information = table[name] as string[];
                if(information == null)
                {
                    return;
                }
                string randRate = "0";
                string luckyRate = "0";
                if(information.Length == 6)
                {
                    randRate = information[4];
                    luckyRate = information[5];
                }

                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", dropTable,
                    "filename, label, Genre, Detail, Particular, Level, RandRate, LuckyRate", string.Format("\'{0}\'", 
                    fileName) + ", " + string.Format("\'{0}\'", label.ToString()) + ", " +
                    string.Format("\'{0}\'", information[0]) + ", " + string.Format("\'{0}\'", information[1]) + 
                    ", " + string.Format("\'{0}\'", information[2]) + ", " + string.Format("\'{0}\'", information[3]) +
                    string.Format(", \'{0}\', \'{1}\'", randRate, luckyRate));
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("添加物品名称为{0}的记录时产生sql异常: " + ex.ToString(), name));
            }      
        }

        /// <summary>
        /// 添加数据(自动重设物品名称)
        /// </summary>
        /// <param name="name">物品名称</param>
        private void InsertData(string name)
        {
            try
            {
                string[] information = table[name] as string[];
                if (information == null)
                {
                    return;
                }
                string randRate = "0";
                string luckyRate = "0";
                if (information.Length == 6)
                {
                    randRate = information[4];
                    luckyRate = information[5];
                }

                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("INSERT INTO {0} (filename, Genre, Detail, Particular, Level, RandRate, LuckyRate, Name) VALUES ('{1}', '{2}', '{3}', '{4}', '{5}', '{6}', '{7}', '{8}')", 
                    dropTable, fileName, information[0], information[1], information[2], information[3], randRate, luckyRate, name);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("添加物品名称为{0}的记录时产生sql异常: " + ex.ToString(), name));
            }     
        }

        /// <summary>
        /// 获取id号
        /// </summary>
        /// <param name="information">掉落物品信息</param>
        /// <returns>id号</returns>
        private string GetID(string[] information)
        {
            string id = null;

            if(information == null) // 掉落物品信息为空
            {
                return null;
            }

            try
            {
                
                string sqlString = string.Format("SELECT id FROM {0} WHERE filename = '{1}' AND Genre = '{2}' AND Detail = '{3}' AND Particular = '{4}' AND Level = '{5}'",
                    dropTable, fileName, information[0], information[1], information[2], information[3]);
                DataTable table = GetDataTable(sqlString);
                if(table.Rows.Count > 0)
                {
                    id = table.Rows[0][0].ToString();
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在获取掉落物品对应id时产生sql异常: " + ex.ToString());
            }

            return id;
        }

        /// <summary>
        /// 获取id号
        /// </summary>
        /// <param name="label">掉落率序号</param>
        /// <returns>id号</returns>
        private string GetID(int label)
        {
            string id = null;
            string sql = string.Format("SELECT {0} FROM {1} WHERE {2} = \'{3}\' AND {4} = \'{5}\'", "id",
                dropTable, "filename", fileName, "label", label.ToString());
            SqlCommand cmd;
            SqlDataReader reader = null;
            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if (!reader.HasRows) // 查询结果为空
                {
                    reader.Close();
                    return null;
                }
                else
                {
                    id = reader[0].ToString();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("检查ID是否存在时产生异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // reader尚未关闭
                {
                    reader.Close();
                }
            }
            return id;
        }

        /// <summary>
        /// 保存当前数据(自动重设道具名称)
        /// </summary>
        private void SaveDataResetItemName()
        {
            List<string> deleteIdList = new List<string>(); // 要删除的id链表
            List<string[]> addNodeList = new List<string[]>(); // 要添加的树结点链表
            List<TreeNode> deleteNodeList = new List<TreeNode>(); // 要删除的树结点链表 

            foreach (TreeNode node1 in node.Nodes) // 遍历树结点
            {
                if (node1.Text != "-1" && node1.Text != "无效物品") // 非特殊记录
                {
                    deleteIdList.Add((node1.Tag as object[])[0].ToString());
                    deleteNodeList.Add(node1);
                }
            }

            foreach (string s in deleteIdList) // 删除记录
            {
                DeleteData(s);
            }

            foreach (object o in listBox2.Items) // 遍历已选属性
            {
                string name = o as string;
                InsertData(name);
                string id = GetID(table[name] as string[]);
                if (id != null)
                {
                    //addNodeList.Add(new string[2]{id, label.ToString()});
                    addNodeList.Add(new string[2] { id, name });
                }
            }

            foreach (TreeNode node1 in deleteNodeList) // 删除树结点
            {
                node1.Remove();
            }

            foreach (string[] s in addNodeList) // 添加树结点
            {
                TreeNode node1 = node.Nodes.Add(s[1]);
                node1.Tag = new object[] { s[0] };
            }
        }

        /// <summary>
        /// 保存当前数据
        /// </summary>
        private void SaveData()
        {
            List<string> deleteIdList = new List<string>(); // 要删除的id链表
            List<string[]> addNodeList = new List<string[]>(); // 要添加的树结点链表
            List<TreeNode> deleteNodeList = new List<TreeNode>(); // 要删除的树结点链表 
            int label = 1; // 新的序号

            foreach(TreeNode node1 in node.Nodes) // 遍历树结点
            {
                if(node1.Text != "-1" && node1.Text != "无效物品") // 非特殊记录
                {
                    deleteIdList.Add((node1.Tag as object[])[0].ToString());
                    deleteNodeList.Add(node1);
                }
            }

            foreach(string s in deleteIdList) // 删除记录
            {
                DeleteData(s);
            }

            foreach(object o in listBox2.Items) // 遍历已选属性
            {
                string name = o as string;
                InsertData(name, label);
                string id = GetID(label);
                if(id != null)
                {
                    //addNodeList.Add(new string[2]{id, label.ToString()});
                    addNodeList.Add(new string[2] { id, name });
                }
                label++;
            }

            foreach(TreeNode node1 in deleteNodeList) // 删除树结点
            {
                node1.Remove();
            }

            foreach(string[] s in addNodeList) // 添加树结点
            {
                TreeNode node1 = node.Nodes.Add(s[1]);
                node1.Tag = new object[] {s[0]};
            }
        }

        /// <summary>
        /// 将结点的内容填充入选中的属性列表中
        /// </summary>
        /// <param name="node">选择的树结点</param>
        private void FillSellectedAttributes(TreeNode node)
        {
            if(node.Level == catLength)
            {
                if(node.Checked)
                {
                    string text = node.Text;
                    if(!addedAttributes.Contains(text))
                    {
                        listBox2.Items.Add(text);
                        listBox2.SelectedItem = text;
                        addedAttributes.Add(text);
                    }
                }
            }
            else
            {
                foreach(TreeNode node1 in node.Nodes)
                {
                    FillSellectedAttributes(node1);
                }
            }
        }

        /// <summary>
        /// 自动填充结点的选择框
        /// </summary>
        /// <param name="node"></param>
        private void AutoFill(TreeNode node, bool isChecked)
        {
            if(node.Checked != isChecked)
            {
                node.Checked = isChecked;
            }
        }

        /// <summary>
        /// 获取文本为参数值的树结点
        /// </summary>
        /// <param name="node">树结点</param>
        /// <param name="text">文本参数</param>
        private void GetNode(TreeNode node, string text)
        {
            if(node.Text == text)
            {
                findNode = node;
                return;
            }
            else
            {
                foreach(TreeNode node2 in node.Nodes)
                {
                    GetNode(node2, text);
                }
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(autoResetItemName) // 自动重设物品名称
            {
                SaveDataResetItemName();
            }
            else
            {
                SaveData();
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
        /// 添加
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            foreach(TreeNode node in treeView1.Nodes)
            {
                FillSellectedAttributes(node);
            }
        }

        /// <summary>
        /// 全部添加
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            foreach(TreeNode node1 in treeView1.Nodes) // 全部选中第一层结点
            {
                if(!node1.Checked)
                {
                    node1.Checked = true;
                }
            }
            buttonX3_Click(null, null); // 完成添加
        }

        /// <summary>
        /// 删除
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX5_Click(object sender, EventArgs e)
        {
            object o = listBox2.SelectedItem;
            string name = o as string;
            foreach(TreeNode node1 in treeView1.Nodes)
            {
                GetNode(node1, name);
            }
            addedAttributes.Remove(name);
            listBox2.Items.Remove(name);

            if(findNode != null)
            {
                treeView1.SelectedNode = findNode;
                findNode.Checked = false;
                findNode = null;
            }
        }

        /// <summary>
        /// 全部删除
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX6_Click(object sender, EventArgs e)
        {
            List<string> deleteAttributes = new List<string>();
            foreach(object o in listBox2.Items) // 遍历所有已经选择的属性
            {
                deleteAttributes.Add(o as string);
            }
            foreach(string s in deleteAttributes)
            {
                listBox2.SelectedItem = s;
                buttonX5_Click(null, null);
            }
        }

        /// <summary>
        /// 用户在右列表中的选择改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            object o = listBox2.SelectedItem;
            if (o == null)
            {
                return;
            }
            string[] information = table[o] as string[];
            if (information != null)
            {
                textBoxX1.Text = information[0];
                textBoxX2.Text = information[1];
                textBoxX3.Text = information[2];
                textBoxX4.Text = information[3];
            }  
        }

        /// <summary>
        /// 树结点选择改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if(currentNode != null)
            {
                if(currentNode.Level == catLength) // 最底层的结点
                {
                    string text = currentNode.Text;
                    string[] information = table[text] as string[];
                    if (information != null)
                    {
                        textBoxX1.Text = information[0];
                        textBoxX2.Text = information[1];
                        textBoxX3.Text = information[2];
                        textBoxX4.Text = information[3];
                    }
                }
            }
        }

        /// <summary>
        /// 树结点的选择框被选中
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_AfterCheck(object sender, TreeViewEventArgs e)
        {
            TreeNode currentNode = e.Node;
            if (currentNode != null)
            {
                foreach (TreeNode node1 in currentNode.Nodes)
                {
                    AutoFill(node1, currentNode.Checked);
                }
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
    }
}