using System;
using System.Collections.Generic;
using System.Text;
using System.Data;


namespace DataTableRemotingCommon
{
    public interface IRemotingSystem
    {
        IDataTableRemoting GetDataTableRemoting(string clientkey);
        IBroadcastRemoting GetBroadcastRemoting(string clientkey);
        //INetClientRemoting GetNetClientRemoting(string clientkey);
        IMonitorRemoting GetMonitorRemoting(string clientkey);

        string GetDBSourceString(string database);

        void ConnectServer(NetClient client);
        void DisconnectServer(string clientkey);
    }

    public interface IDataTableRemoting
    {
        #region DataTable Interface

        DataTable GetDataTable(string tablename);
        DataTable GetDataTable(string tableName, string filter, string order);
        DataTable GetDataTableWithSql(string tableName, string sql);
        DataTable GetDataTableWithSql(string[] tableNames, string sql);
        DataTable GetDataTableTopRow(string tableName);

        void ReleaseTables(string tableName);

        #endregion

        #region Modify Interface

        void BeginUpdate(string tblname);
        void EndUpdate(string tblname);

        bool DeleteRow(string tblname, object[] keys);
        bool UpdateRow(string tblname, object[] keys, object[] values);
        bool AddRow(string tblname, object[] keys, object[] values);
        int SaveTable(string tblname);

        #endregion

        #region Command Interface

        int ExecuteCommand(string tableName, string sql);
        object ExecuteScalar(string tableName, string sql, bool bModify);
        object ExecuteScalar(string[] tableNames, string sql, bool bModify);

        int BeginTransaction();
        int ExecuteTransactionCommand(string tableName, string sql, int index);
        object ExecuteTransactionScalar(string tableName, string sql, int index, bool bModify);
        object ExecuteTransactionScalar(string[] tableNames, string sql, int index, bool bModify);
        bool CommitTransaction(int index);
        bool RollbackTransaction(int index);

        #endregion

        #region Table Lock
        void AcquireReadTableLock(string tblname);

        void AcquireWriteTableLock(string tblname);

        void ReleaseReadTableLock(string tblname);

        void ReleaseWriteTableLock(string tblname);

        void EnableMultiTableLock(bool enable);
        #endregion

        #region Unite Table

        string[] UTGetDatabases();
        bool UTExistDatabase(string dbname);
        bool UTExistUniteTable(string tblname);
        int[] UTUniteTable(string tblname, string todbname, string[] dblist);
        object[] UTGetTableID(string tblname, object[] ids);



        #endregion

        #region Helper Interface

        void HISetModel(string model);

        bool HIExistRemotingTable(string tblname);

        #endregion
    }

    public interface IBroadcastRemoting
    {
        #region Connect Event Interface

        void OnConnectEvent(object sender, ConnectEventArgs args);

        void AddConnectEventHandler(ConnectEventHandler handler);

        void DelConnectEventHandler(ConnectEventHandler handler);

        void AddLogToMonitorHandler(LogToMonitorHandler handler);

        void DelLogToMonitorHandler(LogToMonitorHandler handler);

        #endregion
    }

    public interface IMonitorRemoting
    {
        string[] GetDBNames();
        string[] GetNetClientKeys();
        string[] GetTablenames(string dbname);
        int GetTableRowCount(string tableName, string dbname);
        NetClient GetNetClient(string clientkey);

        object DoCommand(string[] args);
    }

    public interface INetClientRemoting
    { 
    
    }

}