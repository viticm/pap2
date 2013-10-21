using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;

namespace GameDesingerTools
{
    class TimeLog
    {
        private DataTable m_datatable;
        private SqlDataAdapter m_adp;
        private object m_findkey;
        public enum enumLogType {ltstart, ltend, ltnone};

        public void Clear()
        {
            if (m_datatable != null)
            {
                m_datatable.Dispose();
                m_datatable = null;
            }
            if (m_adp != null)
            {
                m_adp.Dispose();
                m_adp = null;
            }
        }
        private bool EditRow(string ip, string machinaename, string username, DateTime dtStart, DateTime dtEnd, string smodname, string action, string comment)
        {
            try
            {
                DataRow row = m_datatable.Rows.Find(m_findkey);
                if (row == null) return false;
                row.BeginEdit();
                
                row["userip"] = ip;
                row["usermachinename"] = machinaename;
                row["username"] = username;
                if (dtStart != DateTime.MinValue)
                    row["starttime"] = dtStart;
                if (dtEnd != DateTime.MinValue)
                    row["endtime"] = dtEnd;
                if (smodname != null)
                    row["modname"] = smodname;
                if (action != null)
                    row["useraction"] = action;
                if (comment != null)
                    row["comment"] = comment;

                row.EndEdit();
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(m_adp);
                m_adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                int val = m_adp.Update(m_datatable);
                m_datatable.AcceptChanges();                
                if (val == 0)
                {
                    Helper.AddLog("log 失败");
                    return false;
                }
            }
            catch (Exception ex)
            {
                m_datatable.RejectChanges();
                Helper.AddLog(ex.Message);
                return false; ;
            }
            return true;
        }
        private bool NewRow(string ip, string machinaename, string username, DateTime dtStart, DateTime dtEnd, string smodname, string action, string comment)
        {

            try
            {
                DataRow newrow = m_datatable.NewRow();
                newrow.BeginEdit();
                m_datatable.Rows.Add(newrow);

                newrow["userip"] = ip;
                newrow["usermachinename"] = machinaename;
                newrow["username"] = username;
                if (dtStart != DateTime.MinValue)
                    newrow["starttime"] = dtStart;
                if (dtEnd != DateTime.MinValue)
                    newrow["endtime"] = dtEnd;

                if (smodname != null)
                    newrow["modname"] = smodname;
                if (action != null)
                    newrow["useraction"] = action;
                if (comment != null)
                    newrow["comment"] = comment;

                newrow.EndEdit();

                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(m_adp);
                m_adp.InsertCommand = cmdBuilder.GetInsertCommand();
                int val = m_adp.Update(m_datatable);
                m_datatable.AcceptChanges();  
                m_findkey = newrow["id"]; //奇怪,在数据表中没有记录时,这里为0,如果有记录了,就不会错.
            }
            catch (Exception ex)
            {
                m_datatable.RejectChanges();
                Helper.AddLog(ex.Message);
                return false;
            }
            return true;
        }

        public void NewLine()
        {
            m_findkey = null;
        }


        public bool Log(enumLogType eltLogType, string smodname, string action, string comment)
        {
            if (m_datatable == null)
            {
                m_adp = new SqlDataAdapter("SELECT * FROM sys_user_log WHERE (id = (SELECT Max(id) FROM sys_user_log))", MainForm.conn);
                m_adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                m_adp.Fill(ds);
                m_datatable = ds.Tables[0];
            }
            else
            {
                m_adp.Fill(m_datatable);
            }
            string shostname = Helper.GetHostName();
            DateTime dtStart, dtEnd;
            if (eltLogType == enumLogType.ltstart)
            {
                dtStart = DateTime.Now;
                dtEnd = DateTime.MinValue;
            }
            else
            {
                dtStart = DateTime.MinValue;
                dtEnd = DateTime.Now;
            }
            if (m_findkey == null)
            {
                NewRow(Helper.GetIPAddress(), shostname, shostname, dtStart, dtEnd, smodname, action, comment);
            }
            else
            {
                EditRow(Helper.GetIPAddress(), shostname, shostname, dtStart, dtEnd, smodname, action, comment);
            }
            return true;
        }
    }
}
