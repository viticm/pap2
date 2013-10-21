/*-----------------------------------------------
 * wangying1
 * 2009-7-27
 * ScriptLock
------------------------------------------------*/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;

namespace ScriptViewer
{
    /*
     *例子：
            ScriptLock l = new ScriptLock(conn, nFieldID, nModTabID, strTableName, strFieldName);
            l.Lock();
            l.UnLock();
            l.Clear();
     */
    public class ScriptLock
    {
        public ScriptLock(SqlConnection conn, int nFieldID, int nModTabID, string strTableName, string strFieldName)
        {
            m_conn = conn;
            m_nFieldID = nFieldID;
            m_nModTabID = nModTabID;
            m_strTableName = strTableName;
            m_strFieldName = strFieldName;
            if (m_conn == null || m_strTableName == null || m_strFieldName == null)
                throw new Exception("无法锁定，请检查 RecordLock 是否传入正确的参数。");

            m_strIP = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList[0].ToString();
            m_strHostName = System.Net.Dns.GetHostName();
        }

        public void Clear()
        {
            m_conn = null;
            if (m_adp != null)
                m_adp.Dispose();
            if (m_ds != null)
                m_ds.Dispose();
            if (m_cmdBuilder != null)
                m_cmdBuilder.Dispose();
        }

        private bool InitDBParams()
        {
            if (m_Values == null)
            {
                m_Values = new object[4];

                string strsql = string.Format("select * from sys_lock_script where fieldid='{0}' and modtabid='{1}' and tablename='{2}' and fieldname='{3}'", m_nFieldID, m_nModTabID, m_strTableName, m_strFieldName);
                m_adp = new SqlDataAdapter(strsql, m_conn);
                m_adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                m_ds = new DataSet();
                m_cmdBuilder = new SqlCommandBuilder(m_adp);
            }
            //int nCount = 1;
            m_Values[0] = m_nFieldID;
            m_Values[1] = m_nModTabID;
            m_Values[2] = m_strTableName;
            m_Values[3] = m_strFieldName;
            return true;
        }

        virtual public bool Lock()
        {
            InitDBParams();    //初始化m_Values[4]，主键列表

            DataTable tbl = GetTable();
            bool bRet = false;
            try
            {
                DataRow row = GetLockRow(tbl);
                string strlockip = row["lockip"] == DBNull.Value ? "" : row["lockip"].ToString();
                if (strlockip == m_strIP || strlockip.Trim().Length == 0) //如果是本机锁定或者没有人锁定
                {
                    bRet = UpdateRow(tbl, row, Convert.ToInt32(row["lastversion"]), DateTime.Now, DateTime.MinValue, true);
                    m_bLocked = bRet;
                }
                else
                    return false;
            }
            finally
            {
                tbl.Dispose();
            }
            return bRet;
        }

        public void DeleteAllLocalLock()
        {
            string strsql = string.Format("select * from sys_lock_script where lockip='{0}' and lockhostname='{1}'", m_strIP, m_strHostName);
            m_adp = new SqlDataAdapter(strsql, m_conn);
            m_adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            m_ds = new DataSet();
            m_cmdBuilder = new SqlCommandBuilder(m_adp);
            m_adp.Fill(m_ds);
            DataTable tab = GetTable();
            try
            {
                foreach (DataRow Row in tab.Rows)
                {
                    Row.Delete();
                }
                SqlCommandBuilder cmdBuilder = m_cmdBuilder;// new SqlCommandBuilder(m_adp);
                m_adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                m_adp.Update(tab);
                tab.AcceptChanges();
            }
            catch (Exception ex)
            {
                tab.RejectChanges();
                throw ex;
            }
        }

        virtual public int UnLock() //todo:在模块级，记录所有TAB页自己锁过的记录，不要每次去查数据库，太耗了
        {
            InitDBParams();

            DataTable tbl = GetTable();
            int nRet = -1;
            try
            {
                DataRow row = GetLockRow(tbl);
                string strlockip = row["lockip"] == null ? "" : row["lockip"].ToString();
                //strlockip为空，是之前已经解过锁了，不能再解原因如下：
                int nOldVer = Convert.ToInt32(row["lastversion"]);
                if (strlockip.Trim().Length == 0)
                {
                    nRet = nOldVer;
                    return nRet;
                }

                if (strlockip == m_strIP)// || strlockip.Trim().Length == 0) 
                {
                    nRet = UpdateRow(tbl, row, nOldVer + 1, DateTime.MinValue, DateTime.Now, false) ? nOldVer + 1 : -1;
                    //m_bLocked = false;
                }
                else
                    return -1;
            }
            finally
            {
                m_bLocked = nRet > 0 ? false : m_bLocked;
                tbl.Dispose();
            }
            return nRet;
        }
        //表示清除任何人加的锁，建议只在管理员帐号中使用。
        public bool ForceClearLock()
        {
            InitDBParams();
            DataTable tbl = GetTable();
            bool bRet = false;
            try
            {
                DataRow row = GetLockRow(tbl);
                bRet = UpdateRow(tbl, row, Convert.ToInt32(row["lastversion"]), DateTime.MinValue, DateTime.MinValue, false);
            }
            finally
            {
                tbl.Dispose();
            }
            return bRet;
        }

        //仅用于指示，是否由本实体加过锁，不能代表服务器上的本记录是否加过锁
        public bool Locked
        {
            get { return m_bLocked; }
        }

        private object GetFieldData(string fieldname)
        {
            InitDBParams();
            DataTable tbl = GetTable();
            try
            {
                //DataRow row = GetLockRow(tbl);
                DataRow row = tbl.Rows.Find(m_Values);
                if (row == null)
                {
                    return null;
                }
                else
                {
                    return row[fieldname];
                }
            }
            finally
            {
                tbl.Dispose();
            }
            //return null;
        }


        //返回0表示没有取到版本号
        public int GetServerLastVersion()
        {
            object o = GetFieldData("lastversion");
            if (o == null)
                return 0;
            else
                return Convert.ToInt32(o);
        }

        public string GetServerLastLockUser()
        {
            object o = GetFieldData("lockusername");
            if (o == null)
                m_strLastLockUser = "";
            else
                m_strLastLockUser = o.ToString();
            return m_strLastLockUser;
        }
        //Private:

        private DataTable GetTable()
        {
            m_adp.Fill(m_ds);
            return m_ds.Tables[0];
        }
        private DataRow GetLockRow(DataTable tbl)
        {
            DataRow row = tbl.Rows.Find(m_Values);
            if (row == null)
            {
                try
                {
                    DataRow newrow = tbl.NewRow();
                    newrow.BeginEdit();

                    newrow["fieldid"] = m_nFieldID;
                    newrow["modtabid"] = m_nModTabID;
                    newrow["tablename"] = m_strTableName;
                    newrow["fieldname"] = m_strFieldName;

                    newrow["lastversion"] = 0;
                    
                    newrow["lockip"] = DBNull.Value;
                    newrow["lockhostname"] = DBNull.Value;
                    newrow["lockusername"] = DBNull.Value;

                    newrow["locktime"] = DBNull.Value;// DateTime.MinValue;
                    newrow["unlocktime"] = DBNull.Value;//DateTime.MinValue;

                    newrow.EndEdit();
                    tbl.Rows.Add(newrow);

                    SqlCommandBuilder cmdBuilder = m_cmdBuilder;// new SqlCommandBuilder(m_adp);
                    m_adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = m_adp.Update(tbl);
                    tbl.AcceptChanges();
                    row = newrow;
                }
                catch (Exception ex)
                {
                    tbl.RejectChanges();
                    throw ex;
                }
            }
            return row;
        }
        private bool UpdateRow(DataTable tbl, DataRow row, int nLastVer, DateTime dtLock, DateTime dtUnLock, bool bLock)
        {
            int val = 0;

            try
            {
                if (bLock)
                {
                    row.BeginEdit();

                    row["fieldid"] = m_nFieldID;
                    row["modtabID"] = m_nModTabID;
                    row["tablename"] = m_strTableName;
                    row["fieldname"] = m_strFieldName;
                    row["lastversion"] = nLastVer;
               
                    row["lockip"] = m_strIP;
                    row["lockhostname"] = m_strHostName;
                    row["lockusername"] = m_strHostName;

                    if (dtLock == DateTime.MinValue)
                        row["locktime"] = DBNull.Value;
                    else
                        row["locktime"] = dtLock;

                    if (dtUnLock == DateTime.MinValue)
                        row["unlocktime"] = DBNull.Value;
                    else
                        row["unlocktime"] = dtUnLock;
                    row.EndEdit();
                }
                else
                {
                    row.Delete();
                }
                
                SqlCommandBuilder cmdBuilder = m_cmdBuilder;// new SqlCommandBuilder(m_adp);
                m_adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                val = m_adp.Update(tbl);

                tbl.AcceptChanges();
                return val > 0;
            }
            catch (Exception ex)
            {
                tbl.RejectChanges();
                throw ex;
            }
        }

        private SqlConnection m_conn;
        private DataSet m_ds;
        private int m_nFieldID, m_nModTabID;
        protected string m_strTableName, m_strFieldName, m_strIP, m_strHostName;
        SqlCommandBuilder m_cmdBuilder;
        private SqlDataAdapter m_adp;
        private object[] m_Values;//保存主键值
        private bool m_bLocked;
        private string m_strLastLockUser;
    };
}
