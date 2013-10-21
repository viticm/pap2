using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.IO;
using System.Xml;
using System.Threading;
using System.Xml.Serialization;

namespace DataTableRemoting
{
    class LockInfo
    {
        public int LockType = 0;
        public string TblName = string.Empty;
        public LockInfo(int type, string tblname)
        {
            LockType = type;
            TblName = tblname;
        }
        
    };

    class TableUnitManager
    {
        
        private string mSource;
        private SqlConnection mConn;
        private string mDBName;

        private Dictionary<string, TableUnit> mTables;
        private Dictionary<string, Dictionary<string, TableUnit>> mSqlTables;

        private Dictionary<string, List<LockInfo>> mLockInfo;

        private Transaction[] mTransactions;

        private uint mMaxTransactions = 200;
        private uint mMaxSqlTable = 10000;
        private uint mSqlTableCount = 0;

        private object mTransactionLock;
        private object mTableLock;
        private object mSqlTableLock;

        private Dictionary<string, ReaderWriterLock> mReadWriteLock;
        private int mLockTimeout = 10000;

        public void Init(string source, string dbname)
        {
            mSource = source;
            mDBName = dbname;
            mConn = new SqlConnection(mSource);
            mConn.Open();
            mTables = new Dictionary<string, TableUnit>();
            mSqlTables = new Dictionary<string, Dictionary<string, TableUnit>>();
            mTransactions = new Transaction[mMaxTransactions];

            mTransactionLock = new object();
            mTableLock = new object();
            mSqlTableLock = new object();

            mReadWriteLock = new Dictionary<string, ReaderWriterLock>();
            mLockInfo = new Dictionary<string, List<LockInfo>>();

        }

        #region thread lock

        public void AcquireReadTableLock(string tblname)
        {
            ReaderWriterLock rwLock = null;
            bool bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
            if (!bRet)
            {
                lock (mReadWriteLock)
                {
                    bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
                    if (!bRet)
                    {
                        rwLock = new ReaderWriterLock();
                        mReadWriteLock[tblname] = rwLock;
                    }
                }
            }
            rwLock.AcquireReaderLock(mLockTimeout);
        }

        public void AcquireWriteTableLock(string tblname)
        {
            ReaderWriterLock rwLock = null;
            bool bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
            if (!bRet)
            {
                lock (mReadWriteLock)
                {
                    bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
                    if (!bRet)
                    {
                        rwLock = new ReaderWriterLock();
                        mReadWriteLock[tblname] = rwLock;
                    }
                }
            }

            rwLock.AcquireWriterLock(mLockTimeout);
        }

        public void ReleaseReadTableLock(string tblname)
        {
            ReaderWriterLock rwLock = null;

            bool bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
            if (!bRet)
            {
                Exception ex = new Exception("no read lock handle");
                throw ex;
            }

            rwLock.ReleaseReaderLock();
        }

        public void ReleaseWriteTableLock(string tblname)
        {
            ReaderWriterLock rwLock = null;

            bool bRet = mReadWriteLock.TryGetValue(tblname, out rwLock);
            if (!bRet)
            {
                Exception ex = new Exception("no write lock handle");
                throw ex;
            }

            rwLock.ReleaseWriterLock();
        }

        #endregion

        #region DataTable Interface

        public TableUnit GetTableUnit(string tblname)
        {
            TableUnit unit = null;
            bool bRet = mTables.TryGetValue(tblname, out unit);
            if (!bRet)
            {
                lock (mTables)
                {
                    bRet = mTables.TryGetValue(tblname, out unit);
                    if (!bRet)
                    {
                        unit = new TableUnit(mConn, tblname);
                        if ((!tblname.StartsWith("sys")) || tblname.StartsWith("sys_"))
                        {
                            mTables.Add(tblname, unit);
                        }
                    }
                }
            }
            return unit;
        }

        public TableUnit GetTableUnitWithSql(string sql)
        {
            TableUnit unit = null;
            unit = new TableUnit(mConn, sql, null);
            return unit;
        }

        public TableUnit GetSqlTableUnit(string sql, string tblname)
        {
            if (mSqlTableCount > mMaxSqlTable)
            {
                lock (mSqlTables)
                {
                    if (mSqlTableCount > mMaxSqlTable)
                    {
                        foreach (Dictionary<string, TableUnit> units in mSqlTables.Values)
                        {
                            foreach (TableUnit u in units.Values)
                            {
                                u.ReleaseTable();
                            }
                        }
                        mSqlTables.Clear();
                        mSqlTableCount = 0;
                    }
                }
                
            }
            TableUnit unit = null;
            bool bRet = mSqlTables.ContainsKey(tblname);
            if (!bRet)
            {
                mSqlTables[tblname] = new Dictionary<string, TableUnit>();
            }
            bRet = mSqlTables[tblname].TryGetValue(sql, out unit);
            if (!bRet)
            {
                unit = new TableUnit(mConn, sql, null);
                mSqlTableCount++;
                if ((!tblname.StartsWith("sys")) || tblname.StartsWith("sys_"))
                {
                    mSqlTables[tblname][sql] = unit;
                }
            }

            return unit;
        }

        public void MarkTableChanged(string tblname)
        {
            TableUnit unit = null;
            bool bRet = mTables.TryGetValue(tblname, out unit);
            if (bRet)
            {
                unit.TableChanged = true;
            }
        }

        public int SaveTable(string tblname)
        {
            TableUnit unit = null;
            bool bRet = mTables.TryGetValue(tblname, out unit);
            int nRet = 0;
            if (bRet)
            {
                nRet = unit.SaveTable();
            }
            return nRet;
        }

        public int SaveAllTable()
        {
            int count = 0;
            lock (mTables)
            {
                foreach (TableUnit unit in mTables.Values)
                {
                    int nRet = unit.SaveTable();
                    if (nRet > 0)
                    {
                        count += 1;
                    }
                }
            }
            return count;
        }

        public void ReleaseTable(string tblname)
        {
            TableUnit unit = null;
            bool bRet = mTables.TryGetValue(tblname, out unit);
            if (bRet)
            {
                unit.ReleaseTable();
                mTables.Remove(tblname);
            }
        }

        public void ReleaseSqlTable(string tblname)
        {
            Dictionary<string, TableUnit> units = null;
            lock (mSqlTables)
            {
                bool bRet = mSqlTables.TryGetValue(tblname, out units);
                if (bRet)
                {
                    foreach (TableUnit unit in units.Values)
                    {
                        unit.ReleaseTable();
                        mSqlTableCount--;
                    }
                    mSqlTables.Remove(tblname);
                }
            }
        }

        public void ReleaseAllTable()
        {
            lock (mTables)
            {
                foreach (TableUnit unit in mTables.Values)
                {
                    unit.ReleaseTable();
                }
                mTables.Clear();
            }
            lock (mSqlTables)
            {
                foreach (Dictionary<string, TableUnit> units in mSqlTables.Values)
                {
                    foreach (TableUnit unit in units.Values)
                    {
                        unit.ReleaseTable();
                    }
                }
                mSqlTables.Clear();
                mSqlTableCount = 0;
            }
        }

        public string SerializeTable(DataTable table)
        {
            StringBuilder sb = new StringBuilder();
            XmlWriter writer = XmlWriter.Create(sb);
            XmlSerializer serializer = new XmlSerializer(typeof(DataTable));
            serializer.Serialize(writer, table);
            writer.Close();
            return sb.ToString();
        }

        public DataTable DeserializeTable(string xmlString)
        {
            StringReader strReader = new StringReader(xmlString);
            XmlReader xmlReader = XmlReader.Create(strReader);
            XmlSerializer serializer = new XmlSerializer(typeof(DataTable));
            DataTable dt = serializer.Deserialize(xmlReader) as DataTable;
            return dt;
        }

        #endregion

        #region Command Interface

        public int ExecuteCommand(string sql)
        {
            int nRet = 0;
            if (mConn.State == ConnectionState.Closed)
            {
                mConn.Open();
            }
            SqlCommand cmd = mConn.CreateCommand();
            cmd.CommandText = sql;
            nRet = cmd.ExecuteNonQuery();
            return nRet;
        }

        /// <summary>
        /// ‘›Œ¥ π”√
        /// </summary>
        public int ExecuteCommand(string sql, byte[] param, string[] paramName)
        {
            int nRet = 0;
            return nRet;
        }


        public object ExecuteScalar(string sql)
        {
            object oRet = null;

            SqlCommand cmd = mConn.CreateCommand();
            cmd.CommandText = sql;
            oRet = cmd.ExecuteScalar();


            return oRet;
        }

        #endregion

        #region Transaction Interface

        public int BeginTranction()
        {
            int index = 0;
            lock (mTransactionLock)
            {
                for (index = 0; index < mMaxTransactions; ++index)
                {
                    if (mTransactions[index] == null)
                    {
                        break;
                    }
                }

                mTransactions[index] = new Transaction(mSource, index);
            }
            return index;
        }

        public int ExecuteTransactionCommand(string sql, int index)
        {
            int nRet = -1;
            if (mTransactions[index] != null)
            {
                nRet = mTransactions[index].ExecuteCommand(sql);
            }
            return nRet;
        }

        public object ExecuteTransactionScalar(string sql, int index)
        {
            object oRet = null;
            if (mTransactions[index] != null)
            {
                oRet = mTransactions[index].ExecuteScalar(sql);
            }
            return oRet;
        }

        public bool CommitTransaction(int index)
        {
            bool bRet = false;
            if (mTransactions[index] != null)
            {
                mTransactions[index].Commit();
            }
            return bRet;
        }

        public bool RollbackTransaction(int index)
        {
            bool bRet = false;
            if (mTransactions[index] != null)
            {
                mTransactions[index].Rollback();
            }
            return bRet;
        }

        #endregion

        public string[] GetTableNames()
        {
            string[] names = new string[mTables.Count];
            int index = 0;
            foreach (string name in mTables.Keys)
            {
                names[index] = name;
                index++;
            }
            return names;
        }

        public bool ExistTable(string tblname)
        {
            bool bRet = mTables.ContainsKey(tblname);
            return bRet;
        }

        public void Release()
        {
            SaveAllTable();
            ReleaseAllTable();
        }
    }
}
