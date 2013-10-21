using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameLuaEditor
{
    public partial class HistoryTotalForm : Form
    {
        private SqlConnection Conn;
        private Hashtable labelTable = new Hashtable(); // 脚本label和内容的hash表
        public HistoryTotalForm(SqlConnection conn)
        {
            Conn = conn;
            InitializeComponent();
            this.iiHourBegin.Value = 0;
            this.iIHourEnd.Value = 0;
            this.iIMinBegin.Value = 0;
            this.iIMinEnd.Value = 0;
            this.dtiBegin.Value = System.DateTime.Now;
            this.dtiEnd.Value = System.DateTime.Now;
        }
      
        private void HistoryTotalForm_Load(object sender, EventArgs e)
        {

        }

        private void btxQurey_Click(object sender, EventArgs e)
        { 
            if (!CheckConditon())
            {
                return;
            }
            string strSql = string.Empty;
            if (ConstructSql(ref strSql))
            {
                SqlDataReader dr = null;
                Qurey(strSql, ref dr);
                AddTreeViewNodes(dr);
                dr.Close();
                if (Conn.State == ConnectionState.Open)
                {
                    Conn.Close();
                }
            }
        }
        /// <summary>
        /// 在查询前，检查条件设置
        /// </summary>
        /// <returns></returns>
        private bool CheckConditon()
        {
            treeViewQureyResult.Nodes.Clear();
            DateTime dtBegin = new DateTime(dtiBegin.Value.Year, dtiBegin.Value.Month, dtiBegin.Value.Day, iiHourBegin.Value, iIMinBegin.Value, 0);
            DateTime dtEnd = new DateTime(dtiEnd.Value.Year, dtiEnd.Value.Month, dtiEnd.Value.Day, iIHourEnd.Value, iIMinEnd.Value, 59);
            if (DateTime.Compare(dtBegin, dtEnd) >= 0)
            {
                string strTip = "查询失败:开始时间大于结束时间";
                this.Text = strTip;
                MessageBox.Show(strTip, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            else if (checkboxHost.Checked && tbxHost.Text == string.Empty)
            {
                string strTip = "警告：操作者筛选为空！";
                this.Text = strTip;
                MessageBox.Show(strTip, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            return true;
        }

        /// <summary>
        ///  需保证开始时间小于结束时间，点击“查询”按钮时要先作检查在调用该函数
        /// </summary>
        /// <param name="strSql"></param>
        /// <returns></returns>
        private bool ConstructSql(ref string strSql)
        {           
            DateTime dtBegin = new DateTime(dtiBegin.Value.Year, dtiBegin.Value.Month, dtiBegin.Value.Day, iiHourBegin.Value, iIMinBegin.Value, 0);            
            DateTime dtEnd = new DateTime(dtiEnd.Value.Year, dtiEnd.Value.Month, dtiEnd.Value.Day, iIHourEnd.Value, iIMinEnd.Value, 59);
            if (DateTime.Compare(dtBegin, dtEnd) >= 0)
            {
                return false;
            }
            this.Text = string.Format("从{0}到{1}时间段内的操作记录查询结果", dtBegin, dtEnd);
            string strSqlHost = string.Empty;
            if (checkboxHost.Checked)
            {
                strSqlHost = " and [host] = '" + tbxHost.Text + "'";
            }
            strSql = string.Format("select label, host, edittime, kind, path  from sys_script_log where edittime >= '{0}' and edittime <= '{1}'{2} order by path, edittime", dtBegin, dtEnd, strSqlHost); 
            return true;
        }

        /// <summary>
        /// 查询数据库获得dr
        /// </summary>
        /// <param name="strSql"></param>
        /// <param name="dr"></param>
        /// <returns></returns>
        private bool Qurey(string strSql, ref SqlDataReader dr)
        {
            try
            {
                if (Conn.State == ConnectionState.Closed)
                {
                    Conn.Open();
                }
                SqlCommand cmd = new SqlCommand(strSql, Conn);                
                dr = cmd.ExecuteReader(); 
            }
            catch(SqlException sqlExp)
            {
                MessageBox.Show("sql异常: " + sqlExp.ToString(),"sql错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;           
        }
        /// <summary>
        /// 向树添加节点
        /// </summary>
        /// <param name="dr">查询数据库sys_script_log的结果</param>
        /// <returns></returns>
        private bool AddTreeViewNodes(SqlDataReader dr)
        {            
            if (checkboxHost.Checked)
            {
                this.Text = tbxHost.Text + this.Text;
            }
            treeViewQureyResult.Nodes.Clear();

            string strPath = string.Empty;
            string strId = string.Empty;  
            Hashtable ht = new Hashtable();

            while (dr.Read())
            {
                strPath = dr["path"].ToString();                
                
                if (!ht.ContainsKey(strPath))
                {
                    TreeNode newNode = treeViewQureyResult.Nodes.Add(strPath);
                    ht.Add(strPath, newNode);                   
                    AddChildNode(dr, newNode);
                }
                else
                {
                    TreeNode node = (TreeNode)ht[strPath];
                    AddChildNode(dr, node);
                }
                
            }
            ht.Clear();
            return true;
        }
        /// <summary>
        /// 向节点node添加一个子节点,内容从dr读
        /// </summary>
        /// <param name="dr"></param>
        /// <param name="node"></param>
        /// <returns></returns>
        private bool AddChildNode(SqlDataReader dr, TreeNode node)
        {
            string strHost = string.Empty;
            string strKind = string.Empty;       
            string strEditTime = string.Empty;
            string strDisplay = string.Empty;

            if (dr["edittime"] is DBNull)
            {
                strEditTime = "未记录时间";
            }
            else
            {
                strEditTime = dr["edittime"].ToString();
            }
            if (dr["host"] is DBNull)
            {
                strHost = "未记录用户";
            }
            else
            {
                strHost = dr["host"].ToString();
            }
            if (dr["kind"] is DBNull)
            {
                strKind = "未记录操作";
            }
            else
            {
                strKind = dr["kind"].ToString();
            }
            strDisplay = string.Format("{0} {1} [{2}]", strEditTime, strHost, strKind);
            TreeNode childnode = node.Nodes.Add(strDisplay);
            childnode.Tag = dr["label"].ToString();
            return true;
        }
        private void checkboxHost_CheckedChanged(object sender, EventArgs e)
        {
            tbxHost.Visible = checkboxHost.Checked;
        }

        private void treeViewQureyResult_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Nodes.Count != 0)
            {
                return;
            }
            string label = e.Node.Tag.ToString();
            string data = labelTable[label] as string;
            if (data == null)
            {
                data = DataBaseManager.GetDataBaseManager().GetScriptHistoryData(label);
                labelTable[label] = data;
            }

            if (data == null)
            {
                data = "";
            }
            tbxData.Text = data;
        }

        private void dtiBegin_Leave(object sender, EventArgs e)
        {
            dtiEnd.MinDate = dtiBegin.Value;
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            labelTable.Clear();
            treeViewQureyResult.Nodes.Clear();
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}