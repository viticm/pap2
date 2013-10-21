using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;

namespace DataTableRemotingClient
{
    public class TableCache
    {
        private DataTable mTable;
        private DateTime mTime;
        private double mDifSeconds;

        public bool HasDefVal = false;

        public DataTable Table
        {
            get 
            {
                DateTime now = DateTime.Now;
                TimeSpan span =  now.Subtract(mTime);
                if (span.TotalSeconds < mDifSeconds)
                {
                    return mTable;
                }
                else
                {
                    return null;
                }
            }
        }

        public TableCache(DataTable table, bool hasDefVal)
        {
            mTable = table;
            mTime = DateTime.Now;
            mDifSeconds = 3.0f;
            HasDefVal = hasDefVal;
        }

        public TableCache(DataTable table, bool hasDefVal, double difSeconds)
        {
            mTable = table;
            mTime = DateTime.Now;
            HasDefVal = hasDefVal;
            mDifSeconds = difSeconds;
        }
        
    }

    public static class TableHelper
    {
        private static bool mEnableRemoting = false;
        private static DataTableProxy mDataTableProxy;
        private static Dictionary<int, SqlCommand> mCommands;
        private static Dictionary<string, string[]> mPrimarykeyColumns = new Dictionary<string, string[]>();

        public static Dictionary<string, TableCache> mDefRowTables = new Dictionary<string, TableCache>();
        static TableHelper()
        {
            mCommands = new Dictionary<int, SqlCommand>();
        }

        public static void EnableRemoting(bool enable)
        {
            mEnableRemoting = enable;
            if (mEnableRemoting)
            {
                mDataTableProxy = DataTableServerProxy.GetDataTableProxy();
            }
        }

        private static DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            foreach (DataColumn column in tbl.Columns)
            {
                column.ReadOnly = false;
            }
            tbl.ExtendedProperties["sql"] = sql;
            return tbl;
        }

        public static string[] GetPrimarykeyColumnName(string tblname, SqlConnection conn)
        {
            string[] keyname = null;
            if (mPrimarykeyColumns.ContainsKey(tblname))
            {
                keyname = mPrimarykeyColumns[tblname];
            }
            else
            {
                DataTable tbl = GetDataTableTopRowProxy(tblname, conn);
                keyname = new string[tbl.PrimaryKey.Length];
                for (int i = 0; i < keyname.Length; ++i)
                {
                    keyname[i] = tbl.PrimaryKey[i].ColumnName;
                }
                mPrimarykeyColumns[tblname] = keyname;
            }

            return keyname;
        }

        public static object[] GetKeyValues(DataRow row)
        {
            object[] keyvalues = new object[row.Table.PrimaryKey.Length];
            for (int i = 0; i < keyvalues.Length; i++)
            {
                string colname = row.Table.PrimaryKey[i].ColumnName;
                keyvalues[i] = row[colname];
            }
            return keyvalues;
        }

        public static DataRow GetDefaultRowProxy(string tblname, SqlConnection conn)
        {
            DataRow row = null;
            TableCache cache = null;
            DataTable tbl = null;
            if (mDefRowTables.ContainsKey(tblname))
            {
                cache = mDefRowTables[tblname];
                tbl = cache.Table;
            }

            if (tbl == null)
            {
                tbl = GetDataTableTopRowProxy(tblname, conn);
                bool bRet = false;
                if (tbl.Rows.Count > 0)
                {
                    row = tbl.Rows[0];
                    bRet = true;
                    foreach (DataColumn col in tbl.PrimaryKey)
                    {
                        if (row[col].ToString() != "0")
                        {
                            bRet = false;
                            break;
                        }
                    }
                    if (!bRet)
                    {
                        row = null;
                    }
                }
                cache = new TableCache(tbl, bRet);
                mDefRowTables[tblname] = cache;
            }
            else
            {
                if (cache.HasDefVal)
                {
                    row = tbl.Rows[0];
                }
                else
                {
                    row = null;
                }
            }

            return row;
        }

        public static DataTable GetDataTableWithSqlProxy(string tableName, string sql, SqlConnection conn)
        {
            DataTable table = null;
            if (mEnableRemoting)
            {
                table = mDataTableProxy.GetDataTableWithSql(tableName, sql);
            }
            else
            {
                sql = ConvertSqlString(tableName, sql);
                table = GetDataTable(sql, conn);
            }
            return table;
        }

        public static DataTable GetDataTableWithSqlProxy(string[] tableNames, string sql, SqlConnection conn)
        {
            DataTable table = null;
            if (mEnableRemoting)
            {
                table = mDataTableProxy.GetDataTableWithSql(tableNames, sql);
            }
            else
            {
                table = GetDataTable(sql, conn);
            }
            return table;
        }

        public static DataTable GetDataTableProxy(string tableName, SqlConnection conn)
        {
            DataTable table = null;
            if (mEnableRemoting)
            {
                table = mDataTableProxy.GetDataTable(tableName);
            }
            else
            {
                string sql = string.Format("select * from {0}", tableName);
                sql = ConvertSqlString(tableName, sql);
                table = GetDataTable(sql, conn);
            }
            return table;
        }

        public static DataTable GetDataTableProxy(string tableName, string filter, string order, SqlConnection conn)
        {
            DataTable table = null;
            if (mEnableRemoting)
            {
                table = mDataTableProxy.GetDataTable(tableName, filter, order);
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

                if (sort == string.Empty && where == string.Empty)
                {
                    return GetDataTableProxy(tableName, conn);
                }

                string sql = string.Format("select * from {0}", tableName);
                sql += where + sort;

                sql = ConvertSqlString(tableName, sql);

                table = GetDataTable(sql, conn);
            }
            return table;
        }

        public static DataTable GetDataTableTopRowProxy(string tableName, SqlConnection conn)
        {
            string sql = string.Format("select top 1 * from {0}", tableName);
            DataTable table = null;
            if (mEnableRemoting)
            {
                table = mDataTableProxy.GetDataTableTopRow(tableName);
            }
            else
            {
                table = GetDataTable(sql, conn);
            }

            return table;
        }

        public static int SaveTable(DataTable table, string sql, SqlConnection conn)
        {
            int nRet = 0;
            if (mEnableRemoting)
            {
                nRet = mDataTableProxy.SaveTable(table, sql);
            }
            else
            {
                SqlDataAdapter adapter = new SqlDataAdapter(sql, conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adapter);
                adapter.UpdateCommand = cmdBuilder.GetUpdateCommand();
                nRet = adapter.Update(table);
            }
            table.AcceptChanges();
            return nRet;
        }

        public static int SaveTable(DataTable table, SqlConnection conn)
        {
            int nRet = 0;
            if (mEnableRemoting)
            {
                nRet = mDataTableProxy.SaveTable(table);
            }
            else
            {
                string sql = string.Empty;
                if (table.ExtendedProperties.ContainsKey(sql))
                {
                    table.ExtendedProperties["sql"].ToString();
                }
                SqlDataAdapter adapter = new SqlDataAdapter(sql, conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adapter);
                adapter.UpdateCommand = cmdBuilder.GetUpdateCommand();
                nRet = adapter.Update(table);
            }
            table.AcceptChanges();
            return nRet;
        }

        public static int ExecuteCommand(string sql, SqlConnection conn)
        {
            int nRet = 1;
            if (mEnableRemoting && false)
            {
                nRet = mDataTableProxy.ExecuteCommand(string.Empty, sql);
            }
            else
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }
                try
                {
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.CommandText = sql;
                    nRet = cmd.ExecuteNonQuery();
                }
                catch
                {
                    throw;
                }
                finally
                {
                    conn.Close();
                }
            }
            return nRet;
        }

        public static int ExecuteCommand(string tableName, string sql, SqlConnection conn)
        {
            int nRet = 1;
            if (mEnableRemoting)
            {
                if (mDataTableProxy.ExistRemotingTable(tableName))
                {
                    nRet = mDataTableProxy.ExecuteCommand(tableName, sql);
                }
                else
                {
                    nRet = ExecuteCommand(sql, conn);
                }
            }
            else
            {
                nRet = ExecuteCommand(sql, conn);
            }
            return nRet;
        }

        public static object ExecuteScalar(string tableName, string sql, SqlConnection conn, bool modify)
        {
            object result = null;
            if (mEnableRemoting)
            {
                if (mDataTableProxy.ExistRemotingTable(tableName))
                {
                    result = mDataTableProxy.ExecuteScalar(tableName, sql, modify);
                }
                else
                {
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.CommandText = sql;
                    result = cmd.ExecuteScalar();
                }
            }
            else
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sql;
                result = cmd.ExecuteScalar();
            }
            return result;
        }

        public static int BeginTransaction(SqlConnection conn)
        {
            int index;
            if (mEnableRemoting && false)
            {
                index = mDataTableProxy.BeginTransaction();
            }
            else
            {
                for (index = 0; index < int.MaxValue; ++index)
                {
                    if (!mCommands.ContainsKey(index))
                    {
                        break;
                    }
                }

                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }
                SqlCommand cmd = conn.CreateCommand();
                SqlTransaction transaction = conn.BeginTransaction();
                cmd.Transaction = transaction;
                mCommands[index] = cmd;
            }
            return index;
        }

        public static int ExecuteTransaction(string tableName, string sql, int index)
        {
            int nRet = -1;
            if (mEnableRemoting && false)
            {
                mDataTableProxy.ReleaseRemoteTable(tableName);
                nRet = mDataTableProxy.ExecuteTransactionCommand(tableName, sql, index);
            }
            else
            {
                if (mCommands.ContainsKey(index))
                {
                    SqlCommand cmd = mCommands[index];
                    cmd.CommandText = sql;
                    nRet = cmd.ExecuteNonQuery();
                }

            }
            return nRet;
        }

        public static object ExecuteTransactionScalar(string tableName, string sql, int index, bool modify)
        {
            object result = null;
            if (mEnableRemoting && false)
            {
                mDataTableProxy.ReleaseRemoteTable(tableName);
                result = mDataTableProxy.ExecuteTransactionScalar(tableName, sql, index, modify);
            }
            else
            {
                if (mCommands.ContainsKey(index))
                {
                    SqlCommand cmd = mCommands[index];
                    cmd.CommandText = sql;
                    result = cmd.ExecuteScalar();
                }

            }
            return result;
        }

        public static bool CommitTransaction(int index)
        {
            bool bRet = false;
            if (mEnableRemoting && false)
            {
                bRet = mDataTableProxy.CommitTransaction(index);
            }
            else
            {
                if (mCommands.ContainsKey(index))
                {
                    SqlCommand cmd = mCommands[index];
                    SqlTransaction transaction = cmd.Transaction;
                    transaction.Commit();
                    mCommands.Remove(index);
                    bRet = true;
                }
            }
            return bRet;
        }

        public static bool RollbackTransaction(int index)
        {
            bool bRet = false;
            if (mEnableRemoting && false)
            {
                bRet = mDataTableProxy.RollbackTransaction(index);
            }
            else
            {
                if (mCommands.ContainsKey(index))
                {
                    SqlCommand cmd = mCommands[index];
                    SqlTransaction transaction = cmd.Transaction;
                    transaction.Rollback();
                    mCommands.Remove(index);
                    bRet = true;
                }
            }

            return bRet;
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


    }
}
