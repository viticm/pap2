using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using DataTableRemotingCommon;

namespace DataTableRemoting
{
    class TableUnit
    {
        private string mTableName;

        private DataTableEx mTable;

        private SqlDataAdapter mAdapter;

        private bool mTableChanged;

        private uint mVersion;

        private string mSqlCmd;

        private SqlConnection mConn;

        /// <summary>
        /// 构造TableUnit
        /// </summary>
        /// <param name="conn">SQL连接</param>
        /// <param name="tblname">数据表名</param>
        public TableUnit(SqlConnection conn, string tblname)
        {
            mTableName = tblname;
            string sql = "select * from " + tblname;
            mSqlCmd = sql;
            mConn = conn;
            LoadDataTable(conn, sql);

        }

        /// <summary>
        /// 构造TableUnit
        /// </summary>
        /// <param name="conn">SQL连接</param>
        /// <param name="sql">SQL语句</param>
        /// <param name="obj">未使用</param>
        public TableUnit(SqlConnection conn, string sql, object obj)
        {
            mTableName = sql;
            mSqlCmd = sql;
            mConn = conn;
            LoadDataTable(conn, sql);
        }

        /// <summary>
        /// 取DataTable
        /// </summary>
        /// <returns></returns>
        public DataTable GetTable()
        {
            return mTable;
        }

        /// <summary>
        /// 保存表修改到数据库
        /// </summary>
        /// <returns>保存的行数</returns>
        public int SaveTable()
        {
            int nRet = 0;
            if (mTableChanged || true)
            {
                try
                {
                    mAdapter = new SqlDataAdapter(mSqlCmd, mConn);
                    SqlCommandBuilder cmd = new SqlCommandBuilder(mAdapter);
                    mAdapter.UpdateCommand = cmd.GetUpdateCommand();
                    DataTable tbl = mTable.GetChanges();
                    if (tbl != null && tbl.Rows.Count > 0)
                    {
                        nRet = mAdapter.Update(tbl);
                    }
                    mTable.AcceptChanges();
                    mTableChanged = false;
                }
                catch(Exception ex)
                {
                    throw ex;
                }
            }
            return nRet;
        }


        /// <summary>
        /// 释放资源
        /// </summary>
        public void ReleaseTable()
        {
            mTable.Dispose();
            mAdapter.Dispose();
        }

        /// <summary>
        /// 加载数据表
        /// </summary>
        /// <param name="conn">SQL连接</param>
        /// <param name="sql">SQL查询语句</param>
        private void LoadDataTable(SqlConnection conn, string sql)
        {
            mAdapter = new SqlDataAdapter(sql, conn);
            mAdapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            mTable = new DataTableEx();
            mAdapter.Fill(mTable);
            mTable.TableName = mTableName;
            mTableChanged = false;
            foreach (DataColumn column in mTable.Columns)
            {
                column.ReadOnly = false;
            }
        }

        /// <summary>
        /// 设置或获取是否有修改
        /// </summary>
        public bool TableChanged { get { return mTableChanged; } set { mTableChanged = value; } }

    }
}
