using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using DataTableRemotingCommon;

namespace DataTableRemoting.RemotingInterface
{
    public class DataTableRemoting : MarshalByRefObject, IDataTableRemoting
    {
        private string mClientKey;
        private string mHostName;
        private string mDBName;
        private TableUnitManager mTableUnitManager;
        private LogHelper mLogHelper;

        private bool mMultiLock = false;
        private object mLockThreadLock = new object();
        private List<string> mReadLockList = new List<string>();
        private List<string> mWriteLockList = new List<string>();

        private List<string> mUniteReadLockList = new List<string>();
        private List<string> mUniteWriteLockList = new List<string>();

        private List<string> mIDReadLockList = new List<string>();

        public DataTableRemoting(string clientKey)
        {
            mLogHelper = TableHelper.GetLogHelper(clientKey);
            string message = "get new loghelper success.";
            mLogHelper.LogMessage(clientKey, message, LogType.Program);

            mClientKey = clientKey;
            NetClient netclient = NetClientManager.GetNetClient(clientKey);
            mHostName = netclient.HostName;
            mDBName = netclient.Database;
            message = "get net client success.";
            mLogHelper.LogMessage(clientKey, message, LogType.Program);

            mTableUnitManager = TableHelper.GetTableUnitManager(clientKey);
            message = "get table unit manager success.";
            mLogHelper.LogMessage(clientKey, message, LogType.Program);

            message = "get new DataTableRemoting.";
            mLogHelper.LogMessage(mClientKey, message, LogType.Program);
        }

        #region Table Lock

        public void AcquireReadTableLock(string tblname)
        {
            lock (mReadLockList)
            {
                mTableUnitManager.AcquireReadTableLock(tblname);
                mReadLockList.Add(tblname);
            }

            string message = string.Format("AcquireReadTableLock: dbname = {0}, tblname = {1}.", mDBName, tblname);
            mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);

        }

        public void AcquireWriteTableLock(string tblname)
        {
            lock (mWriteLockList)
            {
                mTableUnitManager.AcquireWriteTableLock(tblname);
                mWriteLockList.Add(tblname);
            }

            string message = string.Format("AcquireWriteTableLock: dbname = {0}, tblname = {1}.", mDBName, tblname);
            mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
        }

        public void ReleaseReadTableLock(string tblname)
        {
            lock (mReadLockList)
            {
                mTableUnitManager.ReleaseReadTableLock(tblname);
                int nRet = mReadLockList.Count;
                int index = -1;
                foreach (string name in mReadLockList)
                {
                    index++;
                    if (name == tblname)
                    {
                        break;
                    }
                }

                if (index != -1)
                {
                    mReadLockList.RemoveAt(index);
                }
                string sRet = "success";

                if (nRet < 1 || (nRet > 1 && !mMultiLock))
                {
                    sRet = string.Format("Fail {0} locks", nRet);
                }

                string message = string.Format("ReleaseReadTableLock {0}: dbname = {1}, tblname = {2}", sRet, mDBName, tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
            }
            
            
        }

        public void ReleaseWriteTableLock(string tblname)
        {
            lock (mWriteLockList)
            {
                mTableUnitManager.ReleaseReadTableLock(tblname);
                int nRet = mWriteLockList.Count;
                int index = -1;
                foreach (string name in mWriteLockList)
                {
                    index++;
                    if (name == tblname)
                    {
                        break;
                    }
                }

                if (index != -1)
                {
                    mWriteLockList.RemoveAt(index);
                }
                string sRet = "success";

                if (nRet < 1 || (nRet > 1 && !mMultiLock))
                {
                    sRet = string.Format("Fail {0} locks", nRet);
                }

                string message = string.Format("ReleaseWriteTableLock {0}: dbname = {1}, tblname = {2}", sRet, mDBName, tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
            }
        }

        private void ReleaseAllTableLock()
        {
            string message = "Begin release all table lock.";
            mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);

            lock (mReadLockList)
            {
                foreach (string name in mReadLockList)
                {
                    mTableUnitManager.ReleaseReadTableLock(name);
                    message = string.Format("Table Lock Error: {0} ReadLock.", name);
                    mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
                }
                mReadLockList.Clear();
            }

            lock (mWriteLockList)
            {
                foreach (string name in mWriteLockList)
                {
                    mTableUnitManager.ReleaseWriteTableLock(name);
                    message = string.Format("Table Lock Error: {0} WriteLock.", name);
                    mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
                }
                mWriteLockList.Clear();
            }

            message = "End release all table lock.";
            mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);

        }

        public void EnableMultiTableLock(bool enable)
        {
            mMultiLock = enable;
        }

        #endregion

        #region DataTable Interface

        public DataTable GetDataTable(string tblname)
        {
            TableUnit unit = null;
            DataTable table = null;

            AcquireReadTableLock(tblname);

            try
            {
                unit = mTableUnitManager.GetTableUnit(tblname);
                table = unit.GetTable();

                string message = string.Format("GetDataTable {0}: {1} rows", tblname, table.Rows.Count);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTable);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseReadTableLock(tblname);
            }

            return table;
        }

        public DataTable GetDataTable(string tblname, string filter, string order)
        {
            DataRow[] rows = null;
            DataTable tbl = null;
            DataTable table = null;
            AcquireReadTableLock(tblname);
            try
            {
                TableUnit unit = mTableUnitManager.GetTableUnit(tblname);
                table = unit.GetTable();
                rows = table.Select(filter, order);
                tbl = table.Clone();

                if (rows != null)
                {
                    foreach (DataRow row in rows)
                    {
                        tbl.ImportRow(row);
                    }
                }

                string message = string.Format("GetDataTable {0}: ", tblname);

                if (filter != null && filter != string.Empty)
                {
                    message += string.Format("where {0};", filter);
                }

                if (order != null && order != string.Empty)
                {
                    message += string.Format("order by {0};", order);
                }

                message += string.Format(" {0} rows", tbl.Rows.Count);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTable);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseReadTableLock(tblname);
            }

            return tbl;
        }


        public DataTable GetDataTableWithSql(string tblname, string sql)
        {
            TableUnit unit = null;
            DataTable table = null;
            AcquireReadTableLock(tblname);
            try
            {
                SaveTableNoLock(tblname);

                unit = mTableUnitManager.GetTableUnitWithSql(sql);

                table = unit.GetTable();

                string message = string.Format("GetDataTable {0}: {1} rows", sql, table.Rows.Count);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTableSql);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseReadTableLock(tblname);
            }

            return table;
        }


        public DataTable GetDataTableWithSql(string[] tableNames, string sql)
        {
           
            TableUnit unit = null;
            DataTable table = null;
            foreach (string tblname in tableNames)
            {
                AcquireReadTableLock(tblname);
            }
            try
            {
                foreach (string name in tableNames)
                {
                    SaveTableNoLock(name);
                }

                unit = mTableUnitManager.GetTableUnitWithSql(sql);

                table = unit.GetTable();

                string message = string.Format("{0}: {1}", sql, table.Rows.Count);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTableSql);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                foreach (string tblname in tableNames)
                {
                    ReleaseReadTableLock(tblname);
                }
            }
            return table;
        }

        
        public DataTable GetDataTableTopRow(string tblname)
        {
            DataTable table = null;
            DataTable tbl = null;
            AcquireReadTableLock(tblname);
            try
            {
                TableUnit unit = mTableUnitManager.GetTableUnit(tblname);

                table = unit.GetTable();
                tbl = table.Clone();

                if (table.Rows.Count > 0)
                {
                    DataRow row = table.Rows[0];
                    tbl.ImportRow(row);
                }

                string message = string.Format("GetDataTableTop1 {0}", tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTable);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseReadTableLock(tblname);
            }

            return tbl;
        }

        public void ReleaseTables(string tblname)
        {
            try
            {
                mTableUnitManager.ReleaseSqlTable(tblname);
                mTableUnitManager.ReleaseTable(tblname);

                string message = string.Format("ReleaseTables {0}", tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.GetTable);
            }
            catch (Exception ex)
            {
                throw ex;
            }

        }



        #endregion

        #region Modify Interface


        public void BeginUpdate(string tblname)
        {
            string message = string.Format("BeginUpdate {0}", tblname);
            mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
        }

        /// <summary>
        /// 结束修改
        /// </summary>
        /// <param name="tblname">数据表名</param>
        public void EndUpdate(string tblname)
        {
            string message = string.Format("EndUpdate {0}", tblname);
            mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
        }

        public bool DeleteRow(string tblname, object[] keys)
        {
            AcquireWriteTableLock(tblname);
            bool bRet = false;
            DataTable table = null;
            try
            {
                table = mTableUnitManager.GetTableUnit(tblname).GetTable();
                DataRow row = table.Rows.Find(keys);
                if (row != null)
                {
                    row.Delete();
                    mTableUnitManager.MarkTableChanged(tblname);
                    bRet = true;
                }
                string key = TableHelper.GetKeyString(table.PrimaryKey, keys);
                string message = string.Format("DeleteRow {0}: key = {1}", tblname, key);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            }
            catch(Exception ex)
            {
                throw ex;
            }
            finally
            {
               ReleaseWriteTableLock(tblname);
            }

            
            
            return bRet;
        }

        public bool UpdateRow(string tblname, object[] keys, object[] values)
        {
            AcquireWriteTableLock(tblname);
            bool bRet = false;
            DataTable table = null;
            string info = "{";
            try
            {
                table = mTableUnitManager.GetTableUnit(tblname).GetTable();
                DataRow row = table.Rows.Find(keys);
                
                if (row != null)
                {
                    for (int index = 0; index < values.Length; ++index)
                    {
                        if (!row[index].Equals(values[index]))
                        {
                            row[index] = values[index];
                            info += string.Format(" {0} = {1} ", table.Columns[index].ColumnName, values[index]);
                        }
                        bRet = true;
                    }
                    mTableUnitManager.MarkTableChanged(tblname);
                }
                info += "}";
                string key = TableHelper.GetKeyString(table.PrimaryKey, keys);
                string message = string.Format("UpdateRow {0}: key = {1}; info = {2}", tblname, key, info);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseWriteTableLock(tblname);
            }

            

            return bRet;
        }

        public bool AddRow(string tblname, object[] keys, object[] values)
        {
            AcquireWriteTableLock(tblname);
            bool result = false;
            string info = "{";
            DataTable table = null;
            try
            {
                table = mTableUnitManager.GetTableUnit(tblname).GetTable();
                DataRow row = table.Rows.Find(keys);
                
                if (row == null)
                {
                    row = table.NewRow();
                    for (int index = 0; index < values.Length; ++index)
                    {
                        row[index] = values[index];
                        info += string.Format(" {0} = {1} ", table.Columns[index].ColumnName, values[index]);
                    }
                    table.Rows.Add(row);
                    mTableUnitManager.MarkTableChanged(tblname);
                    result = true;
                }
                info += "}";
                string key = TableHelper.GetKeyString(table.PrimaryKey, keys);

                string message = string.Format("AddRow {0}: key = {1}; info = {2}", tblname, key, info);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseWriteTableLock(tblname);
            }

           

            return result;
        }

        public int SaveTable(string tblname)
        {
            int nRet = 0;
            AcquireWriteTableLock(tblname);
            try
            {
                nRet = mTableUnitManager.SaveTable(tblname);


                string message = string.Format("SaveTable {0}: {1} rows", tblname, nRet);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);

                if (nRet > 0)
                {
                    ReleaseTables(tblname);
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseWriteTableLock(tblname);
            }

            return nRet;
        }

        private int SaveTableNoLock(string tblname)
        {
            int nRet = -1;
            try
            {
                nRet = mTableUnitManager.SaveTable(tblname);


                string message = string.Format("SaveTable {0}: {1} rows", tblname, nRet);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);

                if (nRet > 0)
                {
                    ReleaseTables(tblname);
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            return nRet;
        }

        #endregion

        #region Command Interface

        public int ExecuteCommand(string tblname, string sql)
        {
            AcquireWriteTableLock(tblname);
            int nRet = 0;
            try
            {
                SaveTableNoLock(tblname);
                nRet = mTableUnitManager.ExecuteCommand(sql);

                string message = string.Format("ExecuteCommand {0}", sql);
                mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

                if (nRet != 0)
                {
                    ReleaseTables(tblname);
                }
                
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseWriteTableLock(tblname);
            }

            return nRet;
        }

        public object ExecuteScalar(string tblname, string sql, bool bModify)
        {
            if (bModify)
            {
                AcquireWriteTableLock(tblname);
            }
            else
            {
                AcquireReadTableLock(tblname);
            }
            object oRet = null;
            try
            {
                SaveTableNoLock(tblname);
                oRet = mTableUnitManager.ExecuteScalar(sql);
  
                string message = string.Format("ExecuteScalar {0}", sql);
                mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

                if (bModify)
                {
                    ReleaseTables(tblname);
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                if (bModify)
                {
                    ReleaseWriteTableLock(tblname);
                }
                else
                {
                    ReleaseReadTableLock(tblname);
                }
            }
            return oRet;
        }


        public object ExecuteScalar(string[] tableNames, string sql, bool bModify)
        {
            object oRet = null;

            lock (mLockThreadLock)
            {
                mMultiLock = true;
                if (bModify)
                {
                    foreach (string name in tableNames)
                    {
                        AcquireWriteTableLock(name);
                    }
                }
                else
                {
                    foreach (string name in tableNames)
                    {
                        AcquireReadTableLock(name);
                    }
                }

                try
                {
                    foreach (string name in tableNames)
                    {
                        SaveTableNoLock(name);
                    }
                    oRet = mTableUnitManager.ExecuteScalar(sql);

                    string message = string.Format("ExecuteScalar {0}", sql);
                    mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

                    if (bModify)
                    {
                        foreach (string name in tableNames)
                        {
                            ReleaseTables(name);
                        }
                    }
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                finally
                {
                    if (bModify)
                    {
                        foreach (string name in tableNames)
                        {
                            ReleaseWriteTableLock(name);
                        }
                    }
                    else
                    {
                        foreach (string name in tableNames)
                        {
                            ReleaseReadTableLock(name);
                        }
                    }
                    mMultiLock = false;
                }
            }

            return oRet;
        }

        #endregion

        #region Transaction Interface

        public int BeginTransaction()
        {
            int index = mTableUnitManager.BeginTranction();

            string message = string.Format("BeginTransaction {0}", index);
            mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

            return index;
        }

        public int ExecuteTransactionCommand(string tblname, string sql, int index)
        {
            AcquireWriteTableLock(tblname);
            int nRet = 0;
            try
            {
                SaveTableNoLock(tblname);
                nRet = mTableUnitManager.ExecuteTransactionCommand(sql, index);
                if (nRet > 0)
                {
                    ReleaseTables(tblname);
                }

                string message = string.Format("ExecuteTransactionCommand {0}", sql);
                mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                ReleaseWriteTableLock(tblname);
            }
            return nRet;
        }

        public object ExecuteTransactionScalar(string tblname, string sql, int index, bool bModify)
        {
            if (bModify)
            {
                AcquireWriteTableLock(tblname);
            }
            else
            {
                AcquireReadTableLock(tblname);
            }
            object oRet = null;
            try
            {
                SaveTableNoLock(tblname);
                oRet = mTableUnitManager.ExecuteTransactionScalar(sql, index);
                if (bModify)
                {
                    ReleaseTables(tblname);
                }

                string message = string.Format("ExecuteTransactionScalar {0}", sql);
                mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                if (bModify)
                {
                    ReleaseWriteTableLock(tblname);
                }
                else
                {
                    ReleaseReadTableLock(tblname);
                }
            }
            return oRet;
        }

        public object ExecuteTransactionScalar(string[] tableNames, string sql, int index, bool bModify)
        {
            if (bModify)
            {
                foreach (string name in tableNames)
                {
                    AcquireWriteTableLock(name);
                }
            }
            else
            {
                foreach (string name in tableNames)
                {
                    AcquireReadTableLock(name);
                }
            }
            object oRet = null;
            try
            {
                foreach (string name in tableNames)
                {
                    SaveTableNoLock(name);
                }
                oRet = mTableUnitManager.ExecuteTransactionScalar(sql, index);
                if (bModify)
                {
                    foreach (string name in tableNames)
                    {
                        ReleaseTables(name);
                    }
                }

                string message = string.Format("ExecuteTransactionScalar {0}", sql);
                mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                if (bModify)
                {
                    foreach (string name in tableNames)
                    {
                        ReleaseWriteTableLock(name);
                    }
                }
                else
                {
                    foreach (string name in tableNames)
                    {
                        ReleaseReadTableLock(name);
                    }
                }
            }
            return oRet;
        }

        public bool CommitTransaction(int index)
        {
            bool bRet = mTableUnitManager.CommitTransaction(index);


            string message = string.Format("CommitTransaction {0}", index);
            mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

            return bRet;
        }

        public bool RollbackTransaction(int index)
        {
            bool bRet = mTableUnitManager.RollbackTransaction(index);

            string message = string.Format("RollbackTransaction {0}", index);
            mLogHelper.LogMessage(mClientKey, message, LogType.SQLCommand);

            return bRet;
        }

        #endregion


        #region Unite Table

        public string[] UTGetDatabases()
        {
            return AppTableUnite.GetUniteDBNames();
        }

        public bool UTExistDatabase(string dbname)
        {
            bool bRet = AppTableUnite.ExistDatabase(dbname);
            return bRet;
        }

        public int[] UTUniteTable(string tblname, string todbname, string[] dblist)
        {
            string dbs = TableHelper.ArrayToString(dblist);
            string message = string.Format("Begin unite table. tblname = {0}, dbnames = {1}.", tblname, dbs);
            mLogHelper.LogMessage(mClientKey, message, LogType.Modify);

            int[] nRet = new int[] { -1, -1 };
            bool bExist = true;
            foreach (string db in dblist)
            {
                if (!UTExistDatabase(db))
                {
                    bExist = false;
                }
            }
            if (UTExistDatabase(todbname) && bExist)
            {
                
                foreach (string db in dblist)
                {
                    TableUnitManager mgr = TableHelper.GetTableUnitManangerByDbname(db);
                    if (db == todbname)
                    {
                        mgr.AcquireWriteTableLock(tblname);
                        mUniteWriteLockList.Add(tblname);
                        message = string.Format("AcquireWriteTableLock: dbname = {0}, tblname = {1}.", db, tblname);
                    }
                    else
                    {
                        mgr.AcquireReadTableLock(tblname);
                        mUniteReadLockList.Add(tblname);
                        message = string.Format("AcquireReadTableLock: dbname = {0}, tblname = {1}.", db, tblname);
                    }
                    mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
                }
               
                try
                {
                    nRet = AppTableUnite.UniteTables(tblname, todbname, dblist);
                    message = string.Format("End unit table. modify {0} rows, add {1} rows.", nRet[1], nRet[0]);
                    mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                finally
                {
                   
                    foreach (string db in dblist)
                    {
                        TableUnitManager mgr = TableHelper.GetTableUnitManangerByDbname(db);
                        if (db == todbname)
                        {
                            mgr.ReleaseWriteTableLock(tblname);
                            mUniteWriteLockList.RemoveAt(0);
                            message = string.Format("ReleaseWriteTableLock: dbname = {0}, tblname = {1}.", db, tblname);
                        }
                        else
                        {
                            mgr.ReleaseReadTableLock(tblname);
                            mUniteReadLockList.RemoveAt(0);
                            message = string.Format("ReleaseWriteTableLock: dbname = {0}, tblname = {1}.", db, tblname);
                        }
                        mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
                    }
                    
                }
            }

            return nRet;
        }

        public object[] UTGetTableID(string tblname, object[] ids)
        {
            string[] dbnames = AppTableUnite.GetUniteDBNames();
            object[] ret = null;
            string message = string.Empty;
            foreach (string dbname in dbnames)
            {
                TableUnitManager mgr = TableHelper.GetTableUnitManangerByDbname(dbname);
                mgr.AcquireReadTableLock(dbname);
                message = string.Format("AcquireReadTableLock: dbname = {0}, tblname = {1}.", dbname, tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
            }
            try
            {
                ret = AppTableUnite.GetTableID(mDBName, tblname, ids);
                message = string.Format("get new id({0}) for {1}", ret[0], tblname);
                mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                foreach (string dbname in dbnames)
                {
                    TableUnitManager mgr = TableHelper.GetTableUnitManangerByDbname(dbname);
                    mgr.ReleaseReadTableLock(dbname);
                    message = string.Format("ReleaseReadTableLock: dbname = {0}, tblname = {1}.", dbname, tblname);
                    mLogHelper.LogMessage(mClientKey, message, LogType.ReadWriteTableLock);
                }
            }

            return ret;
        }

        public bool UTExistUniteTable(string tblname)
        {
            bool bRet = AppTableUnite.IsUniteTable(tblname, mDBName);
            string message = string.Format("UTExistUniteTable: tblname = {0}, {1}.", tblname, bRet);
            mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            return bRet;
        }

        #endregion

        #region Helper Interface

        public void HISetModel(string model)
        {
            mLogHelper.SetModel(mClientKey, model);
        }

        public bool HIExistRemotingTable(string tblname)
        {
            bool bRet = AppTableUnite.IsRemoteTable(tblname, mDBName);

            string message = string.Format("HIExistRemotingTable: tblname = {0}, {1}.", tblname, bRet);
            mLogHelper.LogMessage(mClientKey, message, LogType.Modify);
            return bRet;
        }

        #endregion


        public void Release()
        {
            
        }

        public override object InitializeLifetimeService()
        {
            return null;
        }
        
    }
}
