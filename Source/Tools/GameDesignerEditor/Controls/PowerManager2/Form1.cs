using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace ToolUserManager
{
    public partial class Form1 : Form
    {        
        private SqlConnection Conn;
        private DataSet ds;
        private string currentTableName;
        private string subOfTableName;
        private string hasTableName;

        enum UserStateType { NEW, GRANTED, REVOKED, PARENTGRANTED, PARENTREVOKED, UNKNOWN };
        private UserStateType currentUserState = UserStateType.UNKNOWN;
        private string currentUserId = string.Empty;

        public Form1()
        {
            InitializeComponent();
            Init();
        }
        private void Init()
        {
            try
            {
                ds = new DataSet();
                ds.ReadXml(Application.StartupPath + "\\dbConfig.xml");
                string strConn = ds.Tables["ToolUsers"].Rows[0][0].ToString();
                Conn = new SqlConnection(strConn);
                combxDBs.DataSource = ds.Tables["table"];
                combxDBs.DisplayMember = "discribe";
                combxDBs.ValueMember = "name";                
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (!CheckInput())
            {
                return;
            }            
            string userip = tboxIP.Text.ToString();

            if (currentUserState == UserStateType.NEW)
            {
                if (subOfTableName != "")
                {
                    AddUser(subOfTableName, currentUserId, userip);
                    CopyUser(currentTableName, subOfTableName, currentUserId);
                }
                else
                {
                    AddUser(currentTableName, currentUserId, userip);
                }
            }
            else
            {
                CopyUser(currentTableName, subOfTableName, currentUserId);
            }
            string selectId = currentUserId;
            UpdateCombxUserName();
            combxUserName.Text = selectId;

        }
        private bool CheckInput()
        {
            string userid = combxUserName.Text.ToString();
            string userip = tboxIP.Text.ToString();
            string strTip;

            if (userid == "" || userip == "")
            {
                return false;
            }
            if (!Helper.IsValidIpAddress(userip))
            {
                strTip = "ip格式错误！";
                MessageBox.Show(strTip, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            if (currentUserState == UserStateType.NEW && subOfTableName != "")
            {
                DataRow row = ((DataRow[])((combxDBs.DataSource as DataTable).Select("name = '" + subOfTableName + "'")))[0];
                string tip = string.Format("这将在数据库{0}也添加该用户！\r\n\r\n确定添加吗？", row["discribe"].ToString());
                if (MessageBox.Show(tip, "请你确认", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.Cancel)
                {
                    return false;
                }
            }

            return true;
        }
        private int CopyUser(string destTableName, string srcTableName, string userid)
        {
            string sql = string.Format("insert into {0} select * from {1} where userid = '{2}'", destTableName, srcTableName, userid);
            Helper.SqlNoneQureyExcute(sql, Conn);
            return 0;
        }
        private int AddUser(string dataTableName, string userid, string userip)
        {            
            string sql = string.Format("select MAX({0}) from {1}", "_index", dataTableName);
            string strMaxIndex = Helper.GetFirstCellStr(sql, Conn);
            if (strMaxIndex == string.Empty || !Helper.CanConverToNumber(strMaxIndex))
            {
                return 0;
            }
            int max_Index = Convert.ToInt32(strMaxIndex);
            sql = string.Format("insert into {0} (userid, ip, _Index) values('{1}', '{2}', {3})", dataTableName, userid, userip, max_Index + 1);
            
            int nCnt = Helper.SqlNoneQureyExcute(sql, Conn);

            return nCnt;
        }
        private void DeleteUser(string dataTableName, string userid)
        {
            string sql = string.Format("delete from {0} where userid = '{1}' or userid = '@{1}'", dataTableName, userid);
            int deletedCnt = Helper.SqlNoneQureyExcute(sql, Conn);

        }

        private void combxDBs_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateCombxUserName();
        }
        private void UpdateCombxUserName()
        {
            DataRowView drv = combxDBs.SelectedItem as DataRowView;
            if (drv == null)
            {
                return;
            }
            currentTableName = drv.Row["name"].ToString();
            subOfTableName = drv.Row["subof"].ToString();
            hasTableName = drv.Row["has"].ToString();

            string sql = string.Format("select [userid] from {0} order by userid", currentTableName);
            DataTable dt = Helper.GetDataTable(sql, Conn);

            combxUserName.DataSource = dt;
            combxUserName.ValueMember = "userid";
            combxUserName.DisplayMember = "userid";
        }
       
        private void btnDelete_Click(object sender, EventArgs e)
        {
            if (hasTableName != "")
            {
                DeleteUser(hasTableName, currentUserId);
            }
            DeleteUser(currentTableName, currentUserId);            
            UpdateCombxUserName();
        }

        private bool CheckOperate()
        {
            return true;

        }

        private void combxUserName_TextChanged(object sender, EventArgs e)
        {
            currentUserId = combxUserName.Text;
            if (currentUserId.Length == 0)
            {
                return;
            }
            if (currentUserId[0] == '@')
            {
                currentUserId = currentUserId.Remove(0, 1);
            }
            currentUserState = UserStateType.NEW;

            if (currentTableName == "")
            {
                return;
            }

            string sql = string.Format("select ip from {0} where userid = '{1}'", currentTableName, currentUserId);
            string _sql = string.Format("select ip from {0} where userid = '@{1}'", currentTableName, currentUserId);

            if (Helper.QueryHasResult(sql, Conn))
            {
                currentUserState = UserStateType.GRANTED;
            }
            else if (Helper.QueryHasResult(_sql, Conn))
            {
                currentUserState = UserStateType.REVOKED;
            }                 
            else if (subOfTableName != "")
            {
                sql = string.Format("select ip from {0} where userid = '{1}'", subOfTableName, currentUserId);
                _sql = string.Format("select ip from {0} where userid = '@{1}'", subOfTableName, currentUserId);
                
                if (Helper.QueryHasResult(sql, Conn))
                {
                    currentUserState = UserStateType.PARENTGRANTED;
                }
                else if (Helper.QueryHasResult(_sql, Conn))
                {
                    currentUserState = UserStateType.PARENTREVOKED;
                }
            }
           
            UpdateStatus();
        }
        private void UpdateStatus()
        {
            UpdateIPbox();
            UpdateButtonsState();
        }
        private void UpdateIPbox()
        {
            string whichTable = string.Empty;
            string userid = combxUserName.Text.ToString();

            if (userid[0] == '@')
            {
                userid = userid.Substring(1);
            }

            switch (currentUserState)
            {
                case UserStateType.GRANTED:
                    whichTable = currentTableName;               
                    break;
                case UserStateType.REVOKED:
                    whichTable = currentTableName;
                    userid = "@" + userid;
                    break;
                case UserStateType.PARENTGRANTED:
                    whichTable = subOfTableName;
                    break;
                case UserStateType.PARENTREVOKED:
                    whichTable = subOfTableName;
                    userid = "@" + userid;
                    break;                
                default:
                    tboxIP.Text = "192.168.";
                    return;
            }

            string sql = string.Format("select ip from {0} where userid = '{1}'", whichTable, userid);
            string ip = Helper.GetFirstCellStr(sql, Conn);
            tboxIP.Text = ip;
        }

        private void UpdateButtonsState()
        {
            btnGrant.Enabled = currentUserState == UserStateType.REVOKED;
            btnRevoke.Enabled = currentUserState == UserStateType.GRANTED;            
            btnAdd.Enabled = !(btnGrant.Enabled) && !(btnRevoke.Enabled);
            btnDelete.Enabled = !(btnAdd.Enabled);
        }

        private void btnGrant_Click(object sender, EventArgs e)
        {
            string sql = string.Format("update {0} set userid = '{1}' where userid = '@{1}'", currentTableName, currentUserId);
            Helper.SqlNoneQureyExcute(sql, Conn);
            if (subOfTableName != "")
            {
                sql = string.Format("update {0} set userid = '{1}' where userid = '@{1}'", subOfTableName, currentUserId);
                Helper.SqlNoneQureyExcute(sql, Conn);
            }
            string selectId = currentUserId;
            UpdateCombxUserName();
            combxUserName.Text = selectId;
        }      

        private void btnRevoke_Click(object sender, EventArgs e)
        {
            string sql = string.Format("update {0} set userid = '@{1}' where userid = '{1}'", currentTableName, currentUserId);
            Helper.SqlNoneQureyExcute(sql, Conn);
            if (hasTableName != "")
            {
                sql = string.Format("update {0} set userid = '@{1}' where userid = '{1}'", hasTableName, currentUserId);
                Helper.SqlNoneQureyExcute(sql, Conn);
            }
            string selectId = currentUserId;
            UpdateCombxUserName();
            combxUserName.Text = "@" + selectId;
        }

    }
}