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
    public partial class Form1 : Form
    {
        private SqlConnection conn; // sql连接
        private string id; // id号
        private string genre; // genre字段
        private string detail; // detail字段
        private string particular; // particular字段
        private string level; // level字段
        private string itemName; // itemName字段
        private List<string> tableNames = new List<string>(); // 物品表名称链表
        private Hashtable table = new Hashtable(); // 树结点ID对应hash表
        private Hashtable detailTable = new Hashtable(); // detail与相关信息的hash表
        private Hashtable particularTable = new Hashtable(); // particular与相关信息的hash表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="tableNames">表名链表</param>
        public Form1(SqlConnection conn, List<string> tableNames, Hashtable detailTable, Hashtable particularTable)
        {
            this.conn = conn;
            this.tableNames = tableNames;
            this.detailTable = detailTable;
            this.particularTable = particularTable;
            if(conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }
            InitializeComponent();
            InitTree();
            if(conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }
        }

        /// <summary>
        /// id属性
        /// </summary>
        public string ID
        {
            get
            {
                return id;
            }

            set
            {
                id = value;
            }
        }

        /// <summary>
        /// genre属性
        /// </summary>
        public string Genre 
        {
            get
            {
                return genre;
            }

            set
            {
                genre = value;
            }
        }

        /// <summary>
        /// detail属性
        /// </summary>
        public string Detail
        {
            get
            {
                return detail;
            }

            set
            {
                detail = value;
            }
        }

        /// <summary>
        /// particular属性
        /// </summary>
        public string Particular
        {
            get
            {
                return particular;
            }

            set
            {
                particular = value;
            }
        }

        /// <summary>
        /// level属性
        /// </summary>
        public string Level
        {
            get
            {
                return level;
            }

            set
            {
                level = value;
            }
        }

        /// <summary>
        /// itemName属性
        /// </summary>
        public string ItemName
        {
            get
            {
                return itemName;
            }

            set
            {
                itemName = value;
            }
        }

        /// <summary>
        /// 表名链表属性
        /// </summary>
        public List<string> TableNames
        {
            get 
            {
                return tableNames;
            }

            set
            {
                tableNames = value;
            }
        }

        /// <summary>
        /// 初始化物品树
        /// </summary>
        private void InitTree()
        {
            foreach(string s in tableNames)
            {
                TreeNode node1 = treeView1.Nodes.Add(s);
                string[] catFields = GetCatfields(s);
                List<string> existCatFields = new List<string>();

                string sql = string.Format("SELECT * FROM {0}", s);
                SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                DataSet ds = new DataSet();
                adp.Fill(ds);
                DataTable tbl = ds.Tables[0];
                
                foreach(DataRow r in tbl.Rows)
                {
                    TreeNode currentNode = node1;
                    string name = r["Name"].ToString(); 

                    foreach(string c in catFields)
                    {
                        bool find = false;
                        TreeNode findNode = null;
                        string catName = r[c].ToString().Trim();
                        if(c == "DetailType" && detailTable.Count != 0) // 对应table进行替换
                        {
                            catName = detailTable[catName] as string; 
                        }
                        else if(c == "ParticularType" && particularTable.Count != 0) // 对应table进行替换
                        {
                            catName = particularTable[catName] as string;
                        }

                        foreach(TreeNode node2 in currentNode.Nodes)
                        {
                            if(node2.Text == catName)
                            {
                                find = true;
                                findNode = node2;
                                break;
                            }
                        }

                        if(!find)
                        {
                            findNode = currentNode.Nodes.Add(catName);
                        }

                        currentNode = findNode;
                    }

                    string[] information = new string[4];
                    information[0] = r["Genre"].ToString().Trim();
                    information[1] = r["DetailType"].ToString().Trim();
                    information[2] = r["ParticularType"].ToString().Trim();
                    information[3] = r["Level"].ToString().Trim();
                    TreeNode addNode = currentNode.Nodes.Add(name);
                    addNode.Tag = information;
                    table[addNode] = information;
                }
            }
        }

        /// <summary>
        /// 初始化选择项
        /// </summary>
        public void InitSelection()
        {
            foreach(TreeNode node in table.Keys)
            {
                string[] information = table[node] as string[];
                if(information[0] == genre && information[1] == detail && information[2] == particular
                    && information[3] == level)
                {
                    treeView1.SelectedNode = node;
                    break;
                }
            }
        }

        /// <summary>
        /// 获取树结点的分类信息
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <returns>分类信息</returns>
        private string[] GetCatfields(string tableName)
        {
            string sql = string.Format("SELECT {0} FROM {1} WHERE {2} = \'{3}\'", "catfields", "sys_modl_tab_def",
                "tablename", tableName);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string[] classifyNames = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while(reader.Read())
                {
                    if(reader[0].ToString().Trim() != "")
                    {
                        classifyNames = reader[0].ToString().Trim().Split(',');
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
                reader.Close();
            }
            return classifyNames;
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if (currentNode != null && currentNode.Tag != null)
            {
                string[] information = currentNode.Tag as string[];
                if (information != null)
                {
                    Genre = information[0];
                    Detail = information[1];
                    Particular = information[2];
                    Level = information[3];
                    ItemName = currentNode.Text;
                    DialogResult = DialogResult.OK;
                    this.Close();
                }
            }
            else
            {
                MessageBox.Show("选择了无效的树结点!");
            }
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
        /// 用户选择树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if(currentNode != null && currentNode.Tag != null)
            {
                string[] information = currentNode.Tag as string[];
                if(information != null)
                {
                    textBoxX1.Text = information[0];
                    textBoxX2.Text = information[1];
                    textBoxX3.Text = information[2];
                    textBoxX4.Text = information[3];
                }
            }
        }
    }
}