using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using DataTableRemotingCommon;


namespace DataTableRemoting
{
    static class TableHelper
    {

        static TableHelper()
        {
            mTableUnitManangers = new Dictionary<string, TableUnitManager>();
            mLogHelpers = new Dictionary<string, LogHelper>();
            InitDBSourceString();
        }

        #region database source

        static Dictionary<string, TableUnitManager> mTableUnitManangers;
        static Dictionary<string, LogHelper> mLogHelpers;
        static Dictionary<string, string> mSourceStringMap;


        private static void InitDBSourceString()
        {
            mSourceStringMap = new Dictionary<string, string>();

            INIFileHelper inifile = INIFileHelper.CfgINIHelper;

            string tmp = inifile.INIReadFile("Database", "dbcount");
            int count = Convert.ToInt32(tmp);

            for (int i = 0; i < count; ++i)
            {
                string key = string.Format("dbname{0}", i);
                tmp = inifile.INIReadFile("Database", key);
                string source = string.Format("Server = jx3web; DataBase = {0}; Uid = s3design; Password = davidbowie;", tmp);
                mSourceStringMap[tmp] = source;
            }

            mSourceStringMap["monitor"] = mSourceStringMap["s3design_test"];

        }

        public static TableUnitManager GetTableUnitManager(string clientkey)
        {
            NetClient client = NetClientManager.GetNetClient(clientkey);
            string source = mSourceStringMap[client.Database];
            if (!mTableUnitManangers.ContainsKey(client.Database))
            {
                lock (mTableUnitManangers)
                {
                    if (!mTableUnitManangers.ContainsKey(client.Database))
                    {
                        TableUnitManager tm = new TableUnitManager();
                        tm.Init(source, client.Database);
                        mTableUnitManangers.Add(client.Database, tm);
                    }
                }
            }
            return mTableUnitManangers[client.Database];
        }

        
        public static LogHelper GetLogHelper(string clientkey)
        {
            NetClient client = NetClientManager.GetNetClient(clientkey);
            string source = client.Database;
            if (!mLogHelpers.ContainsKey(source))
            {
                lock (mLogHelpers)
                {
                    if (!mLogHelpers.ContainsKey(source))
                    {
                        LogHelper log = new LogHelper(clientkey);
                        mLogHelpers.Add(source, log);
                    }
                }
            }
            return mLogHelpers[source];
        }
        

        public static string[] GetDBNames()
        {
            string[] result = null;
            lock (mTableUnitManangers)
            {
                result = new string[mTableUnitManangers.Keys.Count];
                int i = 0;
                foreach (string key in mTableUnitManangers.Keys)
                {
                    result[i] = key;
                    i++;
                }
            }
            return result;
        }

        public static string[] GetAllDBNames()
        {
            string[] result = new string[mSourceStringMap.Count];
            int index = 0;
            foreach (string dbname in mSourceStringMap.Keys)
            {
                result[index] = dbname;
                index++;
            }
            return result;
        }

        public static string GetDBSourceString(string database)
        {
            string source = string.Empty;
            bool bRet = mSourceStringMap.TryGetValue(database, out source);
            return source;
        }

        public static TableUnitManager GetTableUnitManangerByDbname(string dbname)
        {
            string source = mSourceStringMap[dbname];
            if (!mTableUnitManangers.ContainsKey(dbname))
            {
                lock (mTableUnitManangers)
                {
                    if (!mTableUnitManangers.ContainsKey(dbname))
                    {
                        TableUnitManager tm = new TableUnitManager();
                        tm.Init(source, dbname);
                        mTableUnitManangers.Add(dbname, tm);
                    }
                }
            }
            return mTableUnitManangers[dbname];
        }

        public static int SaveAndReleaseAllTable()
        {
            int count = 0;
            lock (mTableUnitManangers)
            {
                foreach (TableUnitManager mgr in mTableUnitManangers.Values)
                {
                    count += mgr.SaveAllTable();
                    mgr.ReleaseAllTable();
                }
            }
            return count;
        }

        public static string ArrayToString(object[] data)
        {
            string sRet = string.Empty;
            foreach (object obj in data)
            {
                sRet += obj + ",";
            }

            sRet = sRet.Trim(new char[] { ',' });

            return sRet;
        }

        public static object[] GetKeyvalues(DataRow row)
        {
            DataColumn[] columns = row.Table.PrimaryKey;
            object[] values = new object[columns.Length];
            int index = 0;
            foreach (DataColumn col in columns)
            {
                values[index] = row[col];
                index++;
            }
            return values;
        }

        public static string GetKeyString(DataColumn[] keycol, object[] value)
        {
            string info = "{";
            for (int index = 0; index < value.Length; ++index)
            {
                info += string.Format(" {0} = {1}", keycol[index].ColumnName, value[index]);
            }
            info += " }";
            return info;
        }


        public static void Release()
        {
            lock (mLogHelpers)
            {
                foreach (LogHelper help in mLogHelpers.Values)
                {
                    help.Release();
                }
            }

            lock (mTableUnitManangers)
            {
                foreach (TableUnitManager tm in mTableUnitManangers.Values)
                {
                    tm.Release();
                }
            }

        }

        #endregion



    }

}
