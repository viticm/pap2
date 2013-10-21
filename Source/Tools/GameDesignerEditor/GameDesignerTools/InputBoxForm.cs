using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameDesingerTools
{
    public partial class InputBoxForm : Form
    {   
        //private const string ConnString = "Server = 192.168.27.31; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
        private SqlConnection Conn = MainForm.conn;
        private string hostName = Helper.GetHostName();
        private string ip = Helper.GetIPAddress();

        const string TableName = "sys_RecentRemarks";
        const string fieldLastTime = "lasttime";
        const string fieldHostName = "hostname";
        const string fieldRemarks = "remarks";
        const string fieldIP = "ip";

        const int RemarkMaxCount = 9;
        

        public InputBoxForm()
        {
            InitializeComponent();
            MyInit();
     
        }        
        public string Remarks
        {
            get
            {
                return tbxMarks.Text;
            }            
        }
        private bool MyInit()
        {
            string SqlStr = string.Format("Select id, remarks From sys_RecentRemarks Where hostname = '{0}' Order by lasttime Desc", hostName);

            string filter = string.Format("hostname = '{0}'", hostName);

            //DataTable dt = Helper.GetDataTable(SqlStr, Conn);
            DataTable dt = Helper.GetDataTableProxy("sys_RecentRemarks", filter, "lasttime Desc", Conn);


            if (dt == null)
            {
                return false;
            }
            combxHistory.DataSource = dt;
            combxHistory.DisplayMember = "remarks";
            combxHistory.ValueMember = "id";
            if (dt.Rows.Count > 0)
            {
                tbxMarks.Text = dt.Rows[0]["remarks"].ToString();
            }            
            return true;
        }
        /// <summary>
        /// 将备注插入数据库
        /// </summary>
        /// <returns></returns>
        private bool Add2DB()
        {
            string Sqlstr = string.Format("Insert into sys_RecentRemarks (hostname, ip, remarks) Values('{0}', '{1}','{2}')", hostName, ip, tbxMarks.Text);
            if (Conn.State == ConnectionState.Closed)
            {
                Conn.Open();
            }
            SqlCommand cmd = new SqlCommand(Sqlstr, Conn);
            try
            {
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand("sys_RecentRemarks", Sqlstr, Conn);
            }
            catch (SqlException exc)
            {
                MessageBox.Show("历史备注插入异常：" + exc.ToString(), "Sql异常", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            finally
            {
                Conn.Close();
            }
            return true;
        }
        private void btnOK_Click(object sender, EventArgs e)
        {

            string strTemp = tbxMarks.Text;
            strTemp = strTemp.TrimStart(new char[] { ' ', '\r', '\n', '\t' });
            if (strTemp.Length > 100 || strTemp.Length ==0)
            {
                MessageBox.Show("备注内容太长或者太短", "字数错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            string sql = string.Empty;

            sql = string.Format("update {0} set {1}='{2}', {3}='{4}' where {5}='{6}' and {7}='{8}'",
                    TableName, 
                    fieldLastTime, DateTime.Now.ToString(),
                    fieldHostName, hostName, 
                    fieldIP,       ip,
                    fieldRemarks, strTemp
                    );

            //int updateRows = ExcuteNoneQuerySQL(sql);
            int updateRows = Helper.ExecuteCommand(TableName, sql, Conn);

            if (updateRows == -1)
            {
                return;
            }
            if (updateRows == 0)
            {
                if (combxHistory.Items.Count >= RemarkMaxCount)
                {  
                    string newRemarks = strTemp;
                    sql = string.Format("update {0} set {1}='{2}', {3}='{4}', {5}='{6}' where {7}='{8}' and {1}=(select min({1}) from {0} where {7}='{8}')",
                        TableName,
                        fieldLastTime, DateTime.Now.ToString(),
                        fieldRemarks, newRemarks,
                        fieldIP,      ip,
                        fieldHostName, hostName
                        );
                }
                else
                {
                    string newRemarks = strTemp;
                    sql = string.Format("Insert into {0} ({1}, {2}, {3}, {4}) Values('{5}', '{6}','{7}', '{8}')",
                        TableName,
                        fieldRemarks, fieldHostName, fieldIP, fieldLastTime,
                        newRemarks, hostName, ip, DateTime.Now.ToString());

                }
                //int insertRows = ExcuteNoneQuerySQL(sql);
                int insertRows = Helper.ExecuteCommand(TableName, sql, Conn);
                if (insertRows == -1)
                {
                    return;
                }
            }

            this.DialogResult = DialogResult.OK;
        }
        //??
        private int ExcuteNoneQuerySQL(string sql)
        {
            int effectRowCnt = 0;
            if (Conn.State == ConnectionState.Closed)
            {
                Conn.Open();
            }
            SqlCommand cmd = new SqlCommand(sql, Conn);
            try
            {
                effectRowCnt = cmd.ExecuteNonQuery();
            }
            catch (SqlException exc)
            {
                MessageBox.Show("历史备注操作异常：" + exc.ToString(), "Sql异常", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return -1;
            }
            finally
            {
                Conn.Close();
            }
            return effectRowCnt;
        }

        private void combxHistory_SelectedIndexChanged(object sender, EventArgs e)
        {
            tbxMarks.Text = combxHistory.Text;
        }

        private void btnCancle_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }
    }
}