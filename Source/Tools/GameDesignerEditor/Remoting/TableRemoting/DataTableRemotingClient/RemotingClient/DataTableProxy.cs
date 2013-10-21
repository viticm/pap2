using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using DataTableRemotingCommon;

namespace DataTableRemotingClient
{
    public class DataTableProxy
    {
        #region all remoting

        private IDataTableRemoting mDataTableRemoting;

        public DataTableProxy(IDataTableRemoting remoting)
        {
            mDataTableRemoting = remoting;
        }

        public DataTable GetDataTable(string tblname)
        {
            tblname = ToLower(tblname);
            if (ExistRemotingTable(tblname))
            {
                DataTableEx table = null;
                try
                {
                    table = mDataTableRemoting.GetDataTable(tblname) as DataTableEx;
                    table.SetCanWrite(true);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                return table;
            }
            else
            {
                string sql = "select * from " + tblname;
                sql = ConvertSqlString(tblname, sql);
                DataTable table = GetTableNotRemote(sql);
                return table;
            }
        }

        public DataTable GetDataTableTopRow(string tblname)
        {
            tblname = ToLower(tblname);
            if (ExistRemotingTable(tblname))
            {
                DataTableEx table = null;
                try
                {
                    table = mDataTableRemoting.GetDataTableTopRow(tblname) as DataTableEx;
                    table.SetCanWrite(true);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                return table;
            }
            else
            {
                string sql = "select top 1 * from " + tblname;
                DataTable table = GetTableNotRemote(sql);
                return table;
            }

        }

        public DataTable GetDataTable(string tblname, string filter, string order)
        {
            tblname = ToLower(tblname);
            if (order == "[]")
            {
                order = string.Empty;
            }
            if (ExistRemotingTable(tblname))
            {
                DataTableEx table;
                if (filter == null)
                {
                    filter = string.Empty;
                }

                if (order == null)
                {
                    order = string.Empty;
                }

                try
                {

                    if (filter == string.Empty && order == string.Empty)
                    {
                        table = GetDataTable(tblname) as DataTableEx;
                    }
                    else
                    {
                        table = mDataTableRemoting.GetDataTable(tblname, filter, order) as DataTableEx;
                    }
                    table.SetCanWrite(true);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                return table;
            }
            else
            {
                string sort = string.Empty;
                if (order != null && order != string.Empty)
                {
                    sort = string.Format(" order by {0}", order);
                }
                string where = string.Empty;
                if (filter != null && filter != string.Empty)
                {
                    where = string.Format(" where {0}", filter);
                }


                string sql = string.Format("select * from {0}", tblname);
                sql += where + sort;

                sql = ConvertSqlString(tblname, sql);
                DataTable table = GetTableNotRemote(sql);
                return table;
            }
        }

        public DataTable GetDataTableWithSql(string tblname, string sql)
        {
            tblname = ToLower(tblname);
            sql = ToLower(sql);
            if (ExistRemotingTable(tblname))
            {
                DataTableEx table = null;
                try
                {
                    table = mDataTableRemoting.GetDataTableWithSql(tblname, sql) as DataTableEx;
                    table.SetCanWrite(false);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                return table;
            }
            else
            {
                sql = ConvertSqlString(tblname, sql);
                DataTable table = GetTableNotRemote(sql);
                return table;
            }

        }

        public DataTable GetDataTableWithSql(string[] tblnames, string sql)
        {
            tblnames = ToLower(tblnames);
            sql = ToLower(sql);
            if (ExistRemotingTable(tblnames[0]))
            {
                DataTableEx table = null;
                try
                {
                    table = mDataTableRemoting.GetDataTableWithSql(tblnames, sql) as DataTableEx;
                    table.SetCanWrite(false);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                return table;
            }
            else
            {
                DataTable table = GetTableNotRemote(sql);
                return table;
            }


        }

        public int SaveTable(DataTable table, string sql)
        {
            sql = ToLower(sql);
            int nRet = 0;
            if (table is DataTableEx)
            {

                if (!(table is DataTableEx))
                {
                    Exception ex = new Exception("table must be DataTableEx!");
                    throw ex;
                }

                if (!((DataTableEx)table).CanWrite)
                {
                    Exception ex = new Exception("table must can be writed!");
                    throw ex;
                }

                List<object[]> list = ((DataTableEx)table).DeleteRowList;

                mDataTableRemoting.BeginUpdate(table.TableName);
                try
                {

                    foreach (object[] keys in list)
                    {
                        mDataTableRemoting.DeleteRow(table.TableName, keys);
                    }

                    nRet += list.Count;

                    list.Clear();

                    List<DataRowEx> newRows = new List<DataRowEx>();

                    foreach (DataRowEx row in table.Rows)
                    {
                        if (row.IsNewRow)
                        {
                            newRows.Add(row);
                            row.Update();
                            nRet++;
                        }
                        else if (row.RowState == DataRowState.Modified && row.KeyValues != null)
                        {
                            bool bModify = mDataTableRemoting.UpdateRow(table.TableName, row.KeyValues, row.GetValues());
                            if (bModify)
                            {
                                nRet++;
                            }
                            row.Update();
                        }
                    }

                    foreach (DataRowEx row in newRows)
                    {
                        mDataTableRemoting.AddRow(table.TableName, row.GetKeyValues(), row.GetValues());
                    }

                  
                    mDataTableRemoting.SaveTable(table.TableName);
               
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                finally
                {
                    mDataTableRemoting.EndUpdate(table.TableName);
                }

            }
            else
            {
                SaveTableNotRemote(table, sql);
            }
            return nRet;
        }

        public int SaveTable(DataTable table)
        {
            string sql = string.Empty;
            if (!(table is DataTableEx) && table.ExtendedProperties.ContainsKey("sql"))
            {
                sql = table.ExtendedProperties["sql"].ToString();
            }
            int nRet = SaveTable(table, sql);
            return nRet;
        }




        public int ExecuteCommand(string tblname, string sql)
        {
            tblname = ToLower(tblname);
            sql = ToLower(sql);
            int nRet = mDataTableRemoting.ExecuteCommand(tblname, sql);
            return nRet;
        }



        public object ExecuteScalar(string tblname, string sql, bool modify)
        {
            tblname = ToLower(tblname);
            sql = ToLower(sql);
            object result = mDataTableRemoting.ExecuteScalar(tblname, sql, modify);
            return result;
        }

        public int BeginTransaction()
        {
            int index = mDataTableRemoting.BeginTransaction();
            return index;
        }

        public int ExecuteTransactionCommand(string tblname, string sql, int index)
        {
            tblname = ToLower(tblname);
            sql = ToLower(sql);
            int nRet = -1;
            nRet = mDataTableRemoting.ExecuteTransactionCommand(tblname, sql, index);
            return nRet;
        }

        public object ExecuteTransactionScalar(string tblname, string sql, int index, bool modify)
        {
            tblname = ToLower(tblname);
            sql = ToLower(sql);
            object result = null;
            result = mDataTableRemoting.ExecuteTransactionScalar(tblname, sql, index, modify);
            return result;
        }

        public bool CommitTransaction(int index)
        {
            bool bRet = false;
            bRet = mDataTableRemoting.CommitTransaction(index);
            return bRet;
        }

        public bool RollbackTransaction(int index)
        {
            bool bRet = false;
            bRet = mDataTableRemoting.RollbackTransaction(index);
            return bRet;
        }

        #endregion


        #region part remoting

        private Dictionary<string, bool> mRemoteTable = new Dictionary<string, bool>();

        public void InitRemoting(SqlConnection conn)
        {
            mConn = conn;

        }

        private DataTable GetTableNotRemote(string sql)
        {
            sql = ToLower(sql);
            SqlDataAdapter adapter = new SqlDataAdapter(sql, mConn);
            adapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataTable table = new DataTable();
            adapter.Fill(table);
            foreach (DataColumn column in table.Columns)
            {
                column.ReadOnly = false;
            }
            table.ExtendedProperties["sql"] = sql;
            return table;
        }

        private int SaveTableNotRemote(DataTable table, string sql)
        {
            sql = ToLower(sql);
            int nRet = 0;
            SqlDataAdapter adapter = new SqlDataAdapter(sql, mConn);
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adapter);
            adapter.UpdateCommand = cmdBuilder.GetUpdateCommand();
            nRet = adapter.Update(table);
            return nRet;
        }



        public void ReleaseRemoteTable(string tblname)
        {
            tblname = ToLower(tblname);
            if (ExistRemotingTable(tblname))
            {
                mDataTableRemoting.SaveTable(tblname);
                mDataTableRemoting.ReleaseTables(tblname);
            }
        }


        public bool ExistRemotingTable(string tblname)
        {
            tblname = ToLower(tblname);
            if (mDelTables.Contains(tblname))
            {
                return false;
            }

            bool bRet = false;
            if (mRemoteTable.ContainsKey(tblname))
            {
                bRet = mRemoteTable[tblname];
            }
            else
            {
                bRet = mDataTableRemoting.HIExistRemotingTable(tblname);
                mRemoteTable[tblname] = bRet;
            }
            return bRet;
        }

        public void DeleteRemotingTable(string tblname)
        {
            tblname = ToLower(tblname);
            mDelTables.Add(tblname);
        }

        public void AddRemotingTable(string tblname)
        {
            tblname = ToLower(tblname);
            if (mDelTables.Contains(tblname))
            {
                mDelTables.Remove(tblname);
            }
            mDataTableRemoting.ReleaseTables(tblname);
        }

        public void OnlyReleaseTable(string tblname)
        {
            tblname = ToLower(tblname);
            mDataTableRemoting.ReleaseTables(tblname);
        }


        private SqlConnection mConn;
        private List<string> mDelTables = new List<string>();
        #endregion


        #region Unite Table

        public string[] UTGetDatabases()
        {
            return mDataTableRemoting.UTGetDatabases();
        }

        public bool UTExistDatabase(string dbname)
        {
            bool bRet = mDataTableRemoting.UTExistDatabase(dbname);
            return bRet;
        }

        public bool UTExistUniteTable(string tblname)
        {
            tblname = ToLower(tblname);
            bool bRet = mDataTableRemoting.UTExistUniteTable(tblname);
            return bRet;
        }

        public int[] UTUniteTable(string tblname, string todbname, string[] dblist)
        {
            tblname = ToLower(tblname);
            int[] nRet = mDataTableRemoting.UTUniteTable(tblname, todbname, dblist);
            return nRet;
        }

        public object[] UTGetTableID(string tblname, object[] ids)
        {
            tblname = ToLower(tblname);
            object[] ret = ids;
            if (ExistRemotingTable(tblname))
            {
                ret = mDataTableRemoting.UTGetTableID(tblname, ids);
            }
            else
            {
                DataTable table = GetDataTable(tblname);
                while (true)
                {
                    DataRow row = table.Rows.Find(ret);
                    if (row == null)
                    {
                        break;
                    }
                    else
                    {
                        ret[0] = (int)(ret[0]) + 1;
                    }
                }
            }
            return ret;
        }

        #endregion

        #region Helper Interface

        public void HISetModel(string model)
        {
            mDataTableRemoting.HISetModel(model);
        }

        #endregion

        #region private funtion

        private string ToLower(string str)
        {
            return str.ToLower();
        }

        private string[] ToLower(string[] str)
        {
            for (int i = 0; i < str.Length; ++i)
            {
                str[i] = str[i].ToLower();
            }
            return str;
        }

        /// <summary>
        /// 转换sql查询语句，主要是为了临时解决item_Custom_Weapon表执行order by操作报内存不足错误的问题。 add by suntao
        /// </summary>
        private static string ConvertSqlString(string tableName, string sqlString)
        {
            string newString = sqlString;
            tableName = tableName.ToLower();
            if (tableName == "item_custom_weapon")
            {
                newString = sqlString.ToLower().Replace("select ", "select top 10000");
            }

            return newString;
        }

        #endregion

    }
}
