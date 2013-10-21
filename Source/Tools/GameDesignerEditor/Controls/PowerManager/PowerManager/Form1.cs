using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace PowerManager
{
    public partial class Form1 : Form
    {
        private string strConn_rtm = "Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
        private SqlConnection Conn_rtm;
        private string strConn_1285 = "Server = jx3web; DataBase = s3design-1-0-8-1380-new; Uid = s3design; Password = davidbowie;";
        private SqlConnection Conn_1285;
        private const string TableName = "sys_useracl";
        private HostState hsResult = HostState.NOTEXISTS;
        private ArrayList strsNameLike = null;
        
        enum HostState
        {
            GRANTED,
            REVOCKED,
            NOTEXISTS,
            CANNNOT,
            ALLREVOCKED
        }
       
        public Form1()
        {
            InitializeComponent();
            Conn_rtm = new SqlConnection(strConn_rtm);
            Conn_1285 = new SqlConnection(strConn_1285);
        }

        private void btnQurey_Click(object sender, EventArgs e)
        {
            if (combxHostName.Text.Trim().Length == 0) // 查询现在都有谁有权限
            {
                btnGrant.Enabled = false;
                string stroutput = string.Empty;

                ArrayList allGranted = QueryAllHostGranted(Conn_1285);
                if (allGranted.Count > 0)
                {
                    stroutput = "这些用户已开权限：";
                    foreach (string s in allGranted)
                    {
                        stroutput += s + " ";
                    }
                    btnRevoke.Enabled = true;
                }
                else
                {
                    stroutput = "没有已开权限的用户。";
                    btnRevoke.Enabled = false;
                }

                lbQureyResult.Text = stroutput;
                return;
            }

            QureyHostNameState(combxHostName.Text, ref hsResult);

            UpdateButtonsSatate();

            string str = combxHostName.Text;

            if (!combxHostName.Items.Contains(str))
                combxHostName.Items.Insert(0,str);
        }
        private bool UpdateButtonsSatate()
        {
            switch (hsResult)
            {
                case HostState.GRANTED:
                    lbQureyResult.Text = combxHostName.Text + "已有权限";
                    btnGrant.Enabled = false;
                    btnRevoke.Enabled = true;
                    break;
                case HostState.REVOCKED:
                case HostState.NOTEXISTS:
                    lbQureyResult.Text = combxHostName.Text + "木有权限";
                    btnRevoke.Enabled = false;
                    btnGrant.Enabled = true;
                    break;
                case HostState.CANNNOT:
                    lbQureyResult.Text = combxHostName.Text + "不能开通权限 ";
                    btnGrant.Enabled = false;
                    btnRevoke.Enabled = false;

                    // 有模糊匹配结果
                    if (strsNameLike.Count > 0)
                    {
                        string strEx = "模糊匹配结果：";
                        foreach (string str in strsNameLike)
                        {
                            strEx += str + " ";
                        }
                        lbQureyResult.Text += strEx;
                    }
                    strsNameLike.Clear();
                    break;
                case HostState.ALLREVOCKED:
                    lbQureyResult.Text = "已关闭全部权限 ";
                    btnGrant.Enabled = false;
                    btnRevoke.Enabled = false;
                    break;
            }
            return true;
        }

        private ArrayList QueryAllHostGranted(SqlConnection conn)
        {
            string strSql = string.Format("select * from {0} where userid not like '@%'", TableName);
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(strSql, conn);
            SqlDataReader dr = cmd.ExecuteReader();

            ArrayList strResults = new ArrayList();

            while (dr.Read())
            {
                strResults.Add(dr["userid"].ToString().Trim());
            }

            dr.Close();
            conn.Close();
            return strResults; 
        }

        private bool QureyHostNameState(string strHostName, ref HostState hostState)
        {
            if (IsHostNameExists(TableName, "userid", strHostName, Conn_1285))
            {
                hostState = HostState.GRANTED;
            }
            else if (IsHostNameExists(TableName, "userid", "@" + strHostName, Conn_1285))
            {
                hostState = HostState.REVOCKED;
            }
            else if (IsHostNameExists(TableName, "userid", strHostName, Conn_rtm))
            {
                hostState = HostState.NOTEXISTS;
            }
            else
            {
                strsNameLike = GetHostNamesAlike(TableName, "userid", strHostName, Conn_rtm);
                hostState = HostState.CANNNOT;
            }

            return true;
        }
        private bool IsHostNameExists(string strTableName, string strField, string strHostName, SqlConnection conn)
        {
            string strSql = string.Format("select * from {0} where {1} = '{2}'", strTableName, strField, strHostName);
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(strSql, conn);
            SqlDataReader dr = cmd.ExecuteReader();
            bool bResult = dr.HasRows;
            dr.Close();
            conn.Close();
            return bResult;           
        }

        private ArrayList GetHostNamesAlike(string strTableName, string strField, string strHostName, SqlConnection conn)
        {
            string strSql = string.Format("select * from {0} where {1} like '%{2}%'", strTableName, strField, strHostName);
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(strSql, conn);
            SqlDataReader dr = cmd.ExecuteReader();

            ArrayList strResults = new ArrayList();

            while (dr.Read())
            {
                strResults.Add(dr["userid"].ToString().Trim());
            }

            dr.Close();
            conn.Close();
            return strResults; 
        }
     
        private int ExcuteNodeQurey(string strSql, SqlConnection conn)
        {
            int nCopyCnt = 0;
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(strSql, conn);
            nCopyCnt = cmd.ExecuteNonQuery();
            conn.Close();
            return nCopyCnt;
        }
        private DataRow GetDataRow(string strSql, SqlConnection conn)
        {
            DataRow drResult = null;
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlDataAdapter dap = new SqlDataAdapter(strSql, conn);
            DataSet ds = new DataSet();
            dap.Fill(ds);
            if (ds.Tables[0].Rows.Count > 0)
            {
                drResult = ds.Tables[0].Rows[0];
            }
            conn.Close();
            return drResult;
        }
     
        private bool InsertDataRow(DataRow drNew, string strDestTableName, SqlConnection conn)
        {
            string strSql = string.Format("insert into {0} (userid,ip,_index) values(", strDestTableName);

            strSql = string.Format("insert into {0} (userid, ip, _index) values('{1}', '{2}', '{3}')", strDestTableName, drNew["userid"], drNew["ip"], drNew["_index"]);
            
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(strSql, conn);
            cmd.ExecuteNonQuery(); 
            conn.Close();
            return true;
        }
       
        private void btnGrant_Click(object sender, EventArgs e)
        {
            string strHostName = combxHostName.Text;
            int nAffectRowCnt = 0;
            if (hsResult == HostState.REVOCKED)
            {
                string strSql = string.Format("Update {0} set userid = '{1}' where userid = '{2}'", TableName, strHostName, "@" + strHostName);
                nAffectRowCnt = ExcuteNodeQurey(strSql, Conn_1285);
                hsResult = HostState.GRANTED;
            }
            else if (hsResult == HostState.NOTEXISTS)
            {
                string strSql = string.Format("select * from {0} where userid = '{1}'", TableName, strHostName);
                DataRow drNew = GetDataRow(strSql, Conn_rtm);
                if (drNew != null)
                {
                    InsertDataRow(drNew, TableName, Conn_1285);
                    hsResult = HostState.GRANTED;
                }
            }
            UpdateButtonsSatate();
        }

        private void btnRevoke_Click(object sender, EventArgs e)
        {
            string strHostName = combxHostName.Text;
            int nAffectRowCnt = 0;
            if (hsResult == HostState.GRANTED)
            {
                string strSql = string.Format("Update {0} set userid = '{1}' where userid = '{2}'", TableName, "@" + strHostName, strHostName);
                nAffectRowCnt = ExcuteNodeQurey(strSql, Conn_1285);
                hsResult = HostState.REVOCKED;
            }
            else if (hsResult == HostState.NOTEXISTS && strHostName.Trim().Length == 0)
            {
                string strSql = string.Format("Update {0} set userid = '@' + userid where userid not like '@%'");
                nAffectRowCnt = ExcuteNodeQurey(strSql, Conn_1285);
                hsResult = HostState.ALLREVOCKED;
            }
            UpdateButtonsSatate();
        }

        private void combxHostName_SelectedIndexChanged(object sender, EventArgs e)
        {
            hsResult = HostState.CANNNOT;
            lbQureyResult.Text = "查询结果：";
            btnRevoke.Enabled = false;
            btnGrant.Enabled = false;
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        
    }
}