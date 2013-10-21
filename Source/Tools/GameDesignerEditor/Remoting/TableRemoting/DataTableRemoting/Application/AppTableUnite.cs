using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

namespace DataTableRemoting
{
    public static class AppTableUnite
    {
        
        private static string[] mDBNames = null;
        private static object mThreadLock = new object();
        private static Dictionary<string, DataTable> mRemoteTables = new Dictionary<string,DataTable>();
        
        static AppTableUnite()
        {
            INIFileHelper helper = INIFileHelper.CfgINIHelper;
            string tmp = helper.INIReadFile("Branch", "dbcount");
            int count = Convert.ToInt32(tmp);
            mDBNames = new string[count];
            for (int i = 0; i < count; ++i)
            {
                string key = string.Format("dbname{0}", i);
                tmp = helper.INIReadFile("Branch", key);
                mDBNames[i] = tmp;
            }
            ReloadRemoteTable();
        }

        public static void ReloadRemoteTable()
        {
            string[] dbnames = TableHelper.GetAllDBNames();
            foreach (string dbname in dbnames)
            {
                TableUnitManager mgr = TableHelper.GetTableUnitManangerByDbname(dbname);
                mgr.ReleaseTable("sys_TableRemoting");
                mRemoteTables[dbname] = mgr.GetTableUnit("sys_TableRemoting").GetTable();
            }
        }


        public static bool IsRemoteTable(string tblname, string dbname)
        {
            if (dbname == "s3design_test")
            {
                return true;
            }
            bool bRet = false;
            DataRow row = mRemoteTables[dbname].Rows.Find(tblname);
            if (row != null)
            {
                bRet = Convert.ToBoolean(row["remote"]);
            }
            return bRet;
        }

        public static bool IsUniteTable(string tblname, string dbname)
        {
            bool bRet = false;
            if (ExistDatabase(dbname))
            {
                DataRow row = mRemoteTables[dbname].Rows.Find(tblname);
                if (row != null)
                {
                    bRet = Convert.ToBoolean(row["remote"]);
                    if (bRet)
                    {
                        bRet = Convert.ToBoolean(row["unite"]);
                    }
                }
            }
            return bRet;
        }


        public static string[] GetUniteDBNames()
        {
            return mDBNames;
        }

        public static object[] GetTableID(string dbname, string tblname, object[] ids)
        {
            string[] dbnames = null;
            if (ExistDatabase(dbname) && IsUniteTable(tblname, dbname))
            {
                dbnames = mDBNames;
            }
            else
            {
                dbnames = new string[] { dbname };
            }
            return GetTableIDs(tblname, ids, dbnames);
        }

        private static object[] GetTableIDs(string tblname, object[] ids, string[] dbnames)
        {
            DataTable[] tables = new DataTable[dbnames.Length];
            int index = 0;
            foreach (string db in dbnames)
            {
                TableUnitManager tm = TableHelper.GetTableUnitManangerByDbname(db);
                TableUnit unit = tm.GetTableUnit(tblname);
                tables[index] = unit.GetTable();
                index++;
            }

            bool bRet = false;
            while (!bRet)
            {
                bRet = true;
                foreach (DataTable table in tables)
                {
                    if (ExistID(table, ids))
                    {
                        bRet = false;
                        break;
                    }
                }
                if (!bRet)
                {
                    ids[0] = (int)(ids[0]) + 1;
                }
            }

            return ids;
        }

        private static bool ExistID(DataTable table, object[] ids)
        {
            bool bRet = true;
            if (table.Rows.Find(ids) == null)
            {
                bRet = false;
            }
            return bRet;
        }

        public static bool ExistDatabase(string dbname)
        {
            bool bRet = false;
            foreach (string name in mDBNames)
            {
                if (name == dbname)
                {
                    bRet = true;
                    break;
                }
            }
            return bRet;
        }

        public static int[] UniteTables(string tblname, string todbname, string[] dblist)
        {

            DataTable[] tables = new DataTable[dblist.Length - 1];
            DataTable totable = null;
            DataTable untable = null;
            int nInsert = 0;
            int nModify = 0;
            int index = 0;
            bool priority = true;

            foreach (string db in dblist)
            {
                TableUnitManager tm = TableHelper.GetTableUnitManangerByDbname(db);
                TableUnit unit = tm.GetTableUnit(tblname);
                if (db != todbname)
                {

                    tables[index] = unit.GetTable();
                    index++;
                }
                else
                {
                    priority = false;
                    totable = unit.GetTable();
                }

                DataTable tbl = unit.GetTable();
                tbl.ExtendedProperties["dbname"] = priority;
            }

            int count = 0;
            foreach (DataTable table in tables)
            {
                if (table != null)
                {
                    bool cover = Convert.ToBoolean(table.ExtendedProperties["dbname"]);
                    int[] result = UniteTables(totable, table, cover);
                    nInsert += result[0];
                    nModify += result[1];
                }
            }

            TableUnitManager totm = TableHelper.GetTableUnitManangerByDbname(todbname);
            totm.SaveTable(tblname);

            return new int[] { nInsert, nModify };
        }

        private static int[] UniteTables(DataTable totable, DataTable table, bool cover)
        {
            int nInsert = 0;
            int nModify = 0;
            foreach (DataRow row in table.Rows)
            {
                object[] keys = TableHelper.GetKeyvalues(row);
                if (!ExistID(totable, keys))
                {
                    DataRow newrow = totable.NewRow();
                    foreach (DataColumn col in table.Columns)
                    {
                        string colname = col.ColumnName;
                        newrow[colname] = row[colname];
                    }
                    totable.Rows.Add(newrow);
                    nInsert++;
                }
                else if (cover)
                {
                    DataRow dr = totable.Rows.Find(keys);
                    bool bEqual = true;
                    foreach (DataColumn col in table.Columns)
                    {
                        string colname = col.ColumnName;
                        if (!dr[colname].Equals(row[colname]))
                        {
                            bEqual = false;
                            dr[colname] = row[colname];
                        }
                        if (!bEqual)
                        {
                            nModify++;
                        }
                    }
                }
            }

            return new int[] { nInsert, nModify };
        }

    }
}
