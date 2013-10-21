using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;

namespace DataTableRemoting
{
    class Transaction
    {
        private SqlConnection mConn;
        private SqlCommand mCommand;
        private SqlTransaction mTransaction;

        public Transaction(string source, int index)
        {
            mConn = new SqlConnection(source);
            mConn.Open();
            mCommand = mConn.CreateCommand();
            mTransaction = mConn.BeginTransaction(IsolationLevel.ReadCommitted, index.ToString());
            mCommand.Transaction = mTransaction;
        }

        public int ExecuteCommand(string sql)
        {
            int nRet;
            mCommand.CommandText = sql;
            nRet = mCommand.ExecuteNonQuery();
            return nRet;
        }

        public object ExecuteScalar(string sql)
        {
            object result = null;
            mCommand.CommandText = sql;
            result = mCommand.ExecuteScalar();
            return result;
        }

        public void Commit()
        {
            mTransaction.Commit();
        }

        public void Rollback()
        {
            mTransaction.Rollback();
        }
    }
}
