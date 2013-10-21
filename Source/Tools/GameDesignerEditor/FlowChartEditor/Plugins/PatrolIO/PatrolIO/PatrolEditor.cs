using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace PatrolIO
{
    public partial class PatrolEditor : Form
    {
        private SqlConnection conn; // sql连接
        private string mapName; // 地图名
        private string patrolIndex; // 路径的编号
        private string patrolPointIndex; // 路径点的编号 
        private string patrolName; // 路径的名称
        private DataTable table; // 数据表
        private bool beginEdit = false; // 是否已经开始编辑
        private Hashtable patrolTable = new Hashtable(); // 保存路径ID的哈希表
        private Hashtable patrolPointTable = new Hashtable(); // 保存路径点ID的哈希表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="mapName">地图名</param>
        public PatrolEditor(SqlConnection conn, string mapName)
        {
            this.conn = conn;
            this.mapName = mapName;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM PatrolInfo WHERE MapName = '{0}' ORDER BY PatrolID", mapName);
                table = GetDataTable(sqlString);

                List<string> list = new List<string>();
                foreach (DataRow row in table.Rows)
                {
                    if (!list.Contains(row["PatrolID"].ToString()))
                    {
                        string displayText = string.Format("{0} {1}", row["PatrolID"], row["PatrolName"]);
                        patrolTable[displayText] = row["PatrolID"].ToString();
                        comboBoxEx1.Items.Add(displayText);
                        list.Add(row["PatrolID"].ToString());
                    }                    
                }

                beginEdit = true;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取路径信息时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }                
            }
        }

        /// <summary>
        /// 路径的编号
        /// </summary>
        public string PatrolIndex
        {
            get
            {
                return patrolIndex;
            }
        }

        /// <summary>
        /// 路径的名称
        /// </summary>
        public string PatrolName
        {
            get
            {
                return patrolName;
            }
        }

        /// <summary>
        /// 路径点的编号
        /// </summary>
        public string PatrolPointIndex
        {
            get
            {
                return patrolPointIndex;
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(comboBoxEx1.Text != "" && comboBoxEx2.Text != "")
            {
                patrolIndex = patrolTable[comboBoxEx1.Text].ToString();
                patrolPointIndex = patrolPointTable[comboBoxEx2.Text].ToString();
                patrolName = comboBoxEx1.Text.Split(new char[] { ' ' })[1];
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("选择框的内容不能为空！", "有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 切换路径
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(beginEdit)
            {
                comboBoxEx2.Items.Clear();
                string patrolID = patrolTable[comboBoxEx1.Text].ToString();
                DataRow[] rows = table.Select(string.Format("PatrolID = '{0}'", patrolID), "PatrolPointID");
                foreach (DataRow row in rows)
                {
                    if(row["PatrolPointID"].ToString() != "-1") // 忽略掉坐标为"0, 0"的数据行
                    {
                        string displayText = string.Format("{0} ({1} ,{2})", row["PatrolPointID"], row["X"], row["Y"]);
                        patrolPointTable[displayText] = row["PatrolPointID"];
                        comboBoxEx2.Items.Add(displayText);
                    }                    
                }
            }            
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
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