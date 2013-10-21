using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using DataBaseIO;

namespace GameLuaEditor
{
    public partial class HistoryForm : Form
    {
        SqlConnection conn; // sql连接
        private string id; // 脚本的id
        private List<string> labelList = new List<string>(); // 记录脚本id的链表
        private string scriptData; // 脚本内容
        private Hashtable labelTable = new Hashtable(); // 脚本label和内容的hash表
        DataTable logTable; // 日志数据表
        ImageList imageList; // 图标链表
        private bool recoverRecord = false; // 是否恢复已删除记录
        private string path; // 要恢复的历史记录的路径

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="id">脚本的id</param>
        public HistoryForm(SqlConnection conn, string id)
        {
            this.conn = conn;
            this.id = id;
            InitializeComponent();
            InitData(id);
        }

        /// <summary>
        /// 脚本内容属性
        /// </summary>
        public string ScriptData
        {
            get
            {
                return scriptData;
            }
        }

        /// <summary>
        /// 树结点的图标链表属性
        /// </summary>
        public ImageList TreeImageList
        {
            set
            {
                imageList = value;
            }

            get
            {
                return imageList;
            }
        }

        /// <summary>
        /// 是否恢复已删除的历史记录
        /// </summary>
        public bool RecoverRecord
        {
            get
            {
                return recoverRecord;
            }
        }

        /// <summary>
        /// 要恢复的历史记录的路径属性
        /// </summary>
        public string Path
        {
            get
            {
                return path;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData(string id)
        {
            listBox1.Items.Clear();
            labelList.Clear();

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT label, host, edittime, kind FROM {0} WHERE {1} = \'{2}\' ORDER BY {3}", 
                    "sys_script_log", "id", id, "edittime");
                logTable = GetDataTable(sqlString);

                if(logTable.Rows.Count > 0)
                {
                    string edittime;
                    string host;
                    string kind;
                    string label;

                    foreach(DataRow r in logTable.Rows)
                    {
                        if(r["edittime"] is DBNull)
                        {
                            edittime = "未记录时间";
                        }
                        else
                        {
                            edittime = r["edittime"].ToString();
                        }
                        
                        if(r["host"] is DBNull)
                        {
                            host = "未记录用户";
                        }
                        else
                        {
                            host = r["host"].ToString();
                        }

                        if(r["kind"] is DBNull)
                        {
                            kind = "未记录操作";
                        }
                        else
                        {
                            kind = r["kind"].ToString();
                        }
                        
                        label = r["label"].ToString();
                        listBox1.Items.Add(string.Format("{0} {1} [{2}]", edittime, host, kind));
                        labelList.Add(label);
                    }
                }
                else
                {
                    MessageBox.Show("没有找到该脚本的历史记录信息!", "历史记录查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取脚本的历史记录时产生sql异常: " + ex.ToString());
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
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tb = ds.Tables[0];
            return tb;
        }

        /// <summary>
        /// 用户改变选择
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = listBox1.SelectedIndex;
            if(index < 0 || index >= labelList.Count) // 越界
            {
                return;
            }
            string label = labelList[index];
            string data = labelTable[label] as string;
            if(data == null)
            {
                data = DataBaseManager.GetDataBaseManager().GetScriptHistoryData(label);
                labelTable[label] = data;
            }
                
            if(data == null)
            {
                data = "";
            }
            textBoxX1.Text = data;
        }

        /// <summary>
        /// 用户选择读取
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            scriptData = textBoxX1.Text;
            if(scriptData == "")
            {
                MessageBox.Show("脚本内容不能为空!", "历史脚本读取", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            DialogResult = DialogResult.Yes;
            this.Close();
        }

        /// <summary>
        /// 用户选择关闭
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 查看删除记录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void recoverToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RecordForm rForm = new RecordForm(conn, imageList);
            DialogResult result = rForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                InitData(rForm.ID);
                path = rForm.Path;
                this.Text = "查看历史记录 (已删除)  " + path;
                recoverRecord = true;
                buttonX3.Enabled = true;
                buttonX1.Enabled = false;
            }
        }

        /// <summary>
        /// 恢复已删除记录
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            scriptData = textBoxX1.Text;
            if (scriptData == "")
            {
                MessageBox.Show("脚本内容不能为空!", "历史脚本恢复", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            DialogResult = DialogResult.Yes;
            this.Close();
        }

        /// <summary>
        /// 关闭窗体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}