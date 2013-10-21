using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace NpcModelNevigate
{
    public partial class Form1 : Form
    {
        public int ret = 0;
        private Hashtable nodeTable = new Hashtable(); // 树结点哈希表
        private string filePath; // 文件路径
        private string npcName; // npc名称

        public Form1()
        {
            InitializeComponent();
        }

        public Form1(SqlConnection Conn, string tblname, string catfieldname, string mapname)
        {
            InitializeComponent();
            LoadTree(Conn, tblname, catfieldname, mapname);
        }

        /// <summary>
        /// 文件路径属性
        /// </summary>
        public string FilePath
        {
            get
            {
                return filePath;
            }
        }

        /// <summary>
        /// npc名称属性
        /// </summary>
        public string NpcName
        {
            get
            {
                return npcName;
            }
        }

        private void LoadTree(SqlConnection Conn, string tblname, string catfieldname, string mapname)
        {
            string sql = string.Format("SELECT TOP 1 * FROM {0}", tblname);
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            string default_map = tbl.Rows[0][catfieldname].ToString();
            string default_model = tbl.Rows[0]["Model"].ToString();
            string default_mainModelFile = tbl.Rows[0]["MainModelFile"].ToString();
            
            if (mapname == string.Empty)
            {
                // 生成分类结点
                sql = string.Format("SELECT {0} FROM {1} GROUP BY {2}", catfieldname, tblname, catfieldname);
                adp = new SqlDataAdapter(sql, Conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                ds = new DataSet();
                adp.Fill(ds);
                tbl = ds.Tables[0];

                foreach (DataRow row in tbl.Rows)
                {
                    if (row[0] != DBNull.Value)
                    {
                        string str = row[0].ToString().Trim();
                        TreeNode node = this.tree.Nodes.Add(str);
                        node.Tag = str;
                    }
                }
            }
            else
            {
                // 添加分类结点
                TreeNode catNode = this.tree.Nodes.Add(mapname);
                catNode.Tag = mapname;
            }

            // 代表记录的叶子结点
            foreach (TreeNode node in this.tree.Nodes)
            {
                string s_map = node.Tag.ToString();

                if (s_map != default_map)
                    sql = string.Format("SELECT * FROM {0} WHERE {1}='{2}'", tblname, catfieldname, s_map);
                else
                    sql = string.Format("SELECT * FROM {0} WHERE {1}='{2}' OR {3} IS NULL", tblname, catfieldname, s_map, catfieldname);

                adp = new SqlDataAdapter(sql, Conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                ds = new DataSet();
                adp.Fill(ds);
                tbl = ds.Tables[0];

                foreach (DataRow row in tbl.Rows)
                {
                    int id = Convert.ToInt32(row["RepresentID"]);

                    string model = row["Model"].ToString().Trim();
                    if (model == string.Empty)
                    {
                        model = default_model;
                    }

                    string mainModelFile = row["MainModelFile"].ToString();
                    if (mainModelFile == string.Empty)
                    {
                        mainModelFile = default_mainModelFile;
                    }

                    string node_text = string.Format("<{0}> {1}", id, model);
                    TreeNode new_node = node.Nodes.Add(node_text);
                    new_node.Tag = id;
                    nodeTable[new_node] = mainModelFile;                   
                }
            }
        }

        private void buttonX1_Click(object sender, EventArgs e) // 确定
        {
            try
            {
                ret = Convert.ToInt32(this.tree.SelectedNode.Tag);
                if(tree.SelectedNode != null)
                {
                    filePath = nodeTable[tree.SelectedNode].ToString();
                    if(filePath != null)
                    {
                        string[] data = filePath.Split(new char[] { '\\' });
                        filePath = "";
                        npcName = data[3];
                        for(int i = 0; i < 4; i++)
                        {
                            filePath += data[i] + "\\";
                        }
                    }
                }

                DialogResult = DialogResult.OK;
            }
            catch
            {
                return;
            }
            this.Close();
        }

        private void buttonX2_Click(object sender, EventArgs e) // 取消
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}