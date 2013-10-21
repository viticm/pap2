using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Data.OleDb;

using DataTableRemotingClient;

namespace GameDesingerTools
{
    static class Helper
    {
        [DllImport("user32.dll")]//, CharSet = CharSet.Unicode)]
        public static extern IntPtr PostMessage(IntPtr hwnd, int wMsg, IntPtr wParam, string lParam);
        [DllImport("user32.dll")]
        static extern void LockWindowUpdate(IntPtr handle);
        [DllImport("Kernel32")]
        public static extern int GetProcAddress(int handle, String funcname);
        [DllImport("Kernel32")]
        public static extern int LoadLibrary(String funcname);
        [DllImport("Kernel32")]
        public static extern int FreeLibrary(int handle);
        public delegate void DllEntryPoint();
        [DllImport("user32.dll", EntryPoint = "FindWindow", CharSet = CharSet.Auto)]
        public extern static IntPtr FindWindow(string lpClassName, string lpWindowName);

        private static DataTableProxy mDataTableProxy = null;
        private static Dictionary<int, SqlCommand> mCommands;

        static Helper()
        {
            if (Program.EnableRemoting)
            {
                mDataTableProxy = DataTableServerProxy.GetDataTableProxy();
            }
            mCommands = new Dictionary<int, SqlCommand>();
        }

        public static void LockWndUpdate(Control ctrl)
        {
            if (ctrl != null)
                LockWindowUpdate(ctrl.Handle);
            else
                LockWindowUpdate(IntPtr.Zero);
        }

        /// <summary>
        /// 转换sql查询语句，主要是为了临时解决item_Custom_Weapon表执行order by操作报内存不足错误的问题。 add by suntao
        /// </summary>
        /// <param name="tableName">数据表名</param>
        /// <param name="sqlString">查询语句</param>
        /// <returns>新的查询语句</returns>
        private static string ConvertSqlString(string tableName, string sqlString)
        {
            string newString = sqlString;

            if (tableName == "item_Custom_Weapon")
            {
                newString = sqlString.ToLower().Replace("select ", "select top 10000");
            }

            return newString;
        }

        /// <summary>
        /// 关闭窗体及其所有的属性中定义的窗体
        /// </summary>
        /// <param name="form">窗体</param>
        /// <returns>属性中定义的窗体是否已经全部关闭</returns>
        public static bool CloseAllWindows(Form form)
        {
            bool result = true;

            try // 获取属性的过程可能会产生异常
            {
                Type t = form.GetType();
                foreach (PropertyInfo pi in t.GetProperties()) // 用反射获取窗体的所有属性，并将属性窗体关闭
                {
                    object o = t.InvokeMember(pi.Name, BindingFlags.GetProperty, null, form, null);
                    if (o is Form) // 窗口属性
                    {
                        Form newForm = o as Form;
                        if (newForm != null)
                        {
                            newForm.Close();
                            if (!newForm.IsDisposed && newForm.Visible) // 部分窗口如表元编辑器，只隐藏没关闭，需要做特殊判断
                            {
                                result = false;
                                break;
                            }
                        }
                    }
                }
            }
            catch { } // 忽略异常

            if(result) // 所有窗体属性已经成功关闭
            {
                IntPtr ptr = FindWindow(null, form.Text);
                PostMessage(ptr, 0x10, IntPtr.Zero, null);
            }

            return result;
        }

        public static string GetVersionInfo()
        {
            string strPath = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\";
            //string strFilename = Process.GetCurrentProcess().MainModule.FileName.Split(new string[]{".vshost.exe"}, StringSplitOptions.RemoveEmptyEntries)[0] + ".exe";
            string strFilename = strPath + "GameDesingerTools.exe";
            if (Program.forPublic)
            {
                strFilename = strPath + "GameDesingerTools_Public.exe";
            }
            Assembly assembly = Assembly.LoadFile(strFilename);
            string fileVersion = null;
            object[] fileVersionAttributes = assembly.GetCustomAttributes(typeof(AssemblyFileVersionAttribute), true);
            if (fileVersionAttributes.Length != 0)
            {
                fileVersion = (fileVersionAttributes[0] as AssemblyFileVersionAttribute).Version.ToString();
            }
            return fileVersion;
        }
        //获取表是否有默认行的设置
        public static bool HasTableDefaultRow(String strTableName, SqlConnection Conn)
        {
            bool bDefaultRecordExist = false;

            /*
            string sql = string.Format("SELECT * FROM sys_table_cfg WHERE tablename='{0}'", strTableName);
            DataTable tbl_tbl_cfg = Helper.GetDataTable(sql, Conn);
            */

            string filter = string.Format("tablename='{0}'", strTableName);
            DataTable tbl_tbl_cfg = Helper.GetDataTableProxy("sys_table_cfg", filter, null, Conn);

            if (tbl_tbl_cfg.Rows.Count > 0)
            {
                DataRow row = tbl_tbl_cfg.Rows[0];
                if (row["hasdefaultrow"] != DBNull.Value)
                {
                    bDefaultRecordExist = Convert.ToBoolean(row["hasdefaultrow"]);
                }
            }
            return bDefaultRecordExist;
        }
        //获取表保存值的类型，0表示与默认行相同则存为空，1表示保存实际值
        public static int GetTableSaveType(String strTableName, SqlConnection Conn)
        {
            /*
            string sql = string.Format("SELECT * FROM sys_table_cfg WHERE tablename='{0}'", strTableName);
            DataTable TblCfg = Helper.GetDataTable(sql, Conn);
            */

            string filter = string.Format("tablename='{0}'", strTableName);

            DataTable TblCfg = Helper.GetDataTableProxy("sys_table_cfg", filter, null, Conn);

            int savetype = 0;
            if (TblCfg.Rows.Count > 0)
            {
                savetype = Convert.ToInt32(TblCfg.Rows[0]["savetype"]); //是否直接保存默认值 1 为保存实际值
            }            
            return savetype;
        }
        //将HASHTABLE的KEY值转为全大写或者全小写
        public static Hashtable TransHashTableCase(Hashtable htSrc, bool bConverToLower)
        {
            Hashtable ht = new Hashtable();
            foreach (DictionaryEntry entry in htSrc)
            {
                if (bConverToLower)
                    ht[entry.Key.ToString().ToLower()] = entry.Value;
                else
                    ht[entry.Key.ToString().ToUpper()] = entry.Value;
            }
            return ht;
        }
        public static Hashtable TransToHashTable(object[] objarr, bool bForceToLower)
        {
            Hashtable ht = new Hashtable();
            foreach(object obj in objarr)
            {
                if (bForceToLower)
                    ht[obj.ToString().ToLower()] = obj;
                else
                    ht[obj.ToString()] = obj;
            }
            return ht;
        }
        private static Delegate GetAddress(int dllModule, string functionname, Type t)
        {
            int addr = GetProcAddress(dllModule, functionname);
            if (addr == 0)
                return null;
            else
                return Marshal.GetDelegateForFunctionPointer(new IntPtr(addr), t);
        }

        public static bool RegComDll(string strFullFileName)
        {
            int hdll = 0;
            hdll = LoadLibrary(strFullFileName);
            try
            {
                DllEntryPoint regdll = (DllEntryPoint)GetAddress(hdll, "DllRegisterServer", typeof(DllEntryPoint));
                if (regdll == null)
                    return false;
                regdll();
            }
            finally
            {
                FreeLibrary(hdll);
            }
            return true;
        }

        public static void AddLog(string strmsg)
        {
            //ScriptDebugForm frm = Program.MainForm.DebugForm;            
            //frm.OutputBox.Text += strmsg + "\r\n";
            //frm.Show();
            Program.MainForm.LogEditBox.Text += strmsg + "\r\n";
            Program.MainForm.LogEditBox.Update();
        }

        public static void AddTimeLog(string s)
        {
            string str = DateTime.Now.ToLongTimeString() + " " + s;
            AddLog(str);
        }

        public static bool CompareStringArray(string[] src, string[] des, bool bIgoreCase)
        {
            if (des.Length != src.Length)
                return false;
            
            Hashtable htsrc = new Hashtable();
            for(int i = 0; i < src.Length; i ++)
            {
                string strkey = bIgoreCase ? src[i].ToLower() : src[i];
                htsrc.Add(strkey, i + 1);
            }
            for (int i = 0; i < des.Length; i++)
            {
                string strkey = bIgoreCase ? des[i].ToLower() : des[i];
                if (htsrc[strkey] == null)
                    return false;
            }
            return true;
        }

        public static void FillDBNullWithDefault(ref System.Data.DataTable tbl)
        {
            if (tbl.PrimaryKey.Length == 0)
                return;

            object[] keys = new object[tbl.PrimaryKey.Length];
            for (int i = 0; i < keys.Length; i++)
            {
                keys[i] = 0;
            }
            DataRow default_row = tbl.Rows.Find(keys);
            if (default_row != null)
            {
                foreach (DataRow row in tbl.Rows)
                {
                    foreach (DataColumn col in tbl.Columns)
                    {
                        if (row[col.ColumnName] == DBNull.Value)
                            row[col.ColumnName] = default_row[col.ColumnName];
                    }
                }
            }
        }

        public static bool IsColumnInTable(string colname, System.Data.DataTable tbl)
        {
            foreach (DataColumn col in tbl.Columns)
            {
                if (colname.Trim().ToLower() == col.ColumnName.Trim().ToLower())
                {
                    return true;
                }
            }
            return false;
        }

        public static bool FieldIsTableKey(string fieldname, System.Data.DataTable tbl)
        {
            foreach (DataColumn col in tbl.PrimaryKey)
            {
                if (col.ColumnName.Trim() == fieldname)
                {
                    return true;
                }
            }
            return false;
        }

        public static bool FieldIsTableKey(DataColumn col, System.Data.DataTable tbl)
        {
            foreach (DataColumn c in tbl.PrimaryKey)
            {
                if (col.ColumnName.Trim() == c.ColumnName.Trim())
                {
                    return true;
                }
            }
            return false;
        }

        public static bool RenameDbTable(string oldTblName, string newTblName, SqlConnection Conn)
        {
            
            string sql = string.Format("sp_rename '{0}', '{1}', 'OBJECT'", oldTblName, newTblName);
            
            /* remoting
            SqlCommand cmd = Conn.CreateCommand();
            cmd.CommandText = sql;
            cmd.ExecuteNonQuery();
            */

            Helper.ExecuteCommand(oldTblName, sql, Conn);
            return true;
        }

        public static bool RenameDbField(string tblName, string oldFieldName, string newFieldName, SqlConnection Conn)
        {
            string _Old = tblName + "." + oldFieldName;
            string sql = string.Format("sp_rename '{0}', '{1}', 'COLUMN'", _Old, newFieldName);
            /* remoting
            
            SqlCommand cmd = Conn.CreateCommand();
            cmd.CommandText = sql;
            cmd.ExecuteNonQuery();
             
            */

            Helper.ExecuteCommand(tblName, sql, Conn);
            return true;
        }

        public static bool RemoveDbTable(string tblName, SqlConnection Conn)
        {
            bool bSuc = false;
            Conn.Open();
            try
            {
                string sql = "DROP TABLE " + tblName;

                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = "DROP TABLE " + tblName;
                cmd.ExecuteNonQuery();
                */
                if (Program.EnableRemoting)
                {
                    mDataTableProxy.OnlyReleaseTable(tblName);
                }
                Helper.ExecuteCommand(sql, Conn);

                bSuc = true;
            }
            catch (Exception ex)
            {
                string err = string.Format("在删除表 {0} 时发生错误。\r\n\r\n错误信息：{1}", tblName, ex.Message);
                MessageBox.Show(err);
                bSuc = false;
            }
            Conn.Close();
            return bSuc;
        }


        //该接口改为private, 用Helper.GetDataTable...Proxy()代替
        private static System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
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
            return tbl;
        }

        public static DataTable GetDataTableWithSqlProxy(string tableName, string sql, SqlConnection conn)
        {
            DataTable table = null;
            if (Program.EnableRemoting)
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
            if (Program.EnableRemoting)
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
            if (Program.EnableRemoting)
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
            if (Program.EnableRemoting)
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
            if (Program.EnableRemoting)
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
            int nRet = 1;
            if (Program.EnableRemoting)
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

        public static int ExecuteCommand(string sql, SqlConnection conn)
        {
            int nRet = 1;
            if (Program.EnableRemoting && false)
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
            if (Program.EnableRemoting)
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

        /*
        public static int ExecuteCommand(string[] tableNames, string sql, SqlConnection conn)
        {
            int nRet = 1;
            if (Program.EnableRemoting)
            {
                nRet = mDataTableProxy.ExecuteCommand(tableNames, sql);
            }
            else
            {
                nRet = ExecuteCommand(sql, conn);
            }
            return nRet;
        }
        */

        public static object ExecuteScalar(string tableName, string sql, SqlConnection conn, bool modify)
        { 
            object result = null;
            if (Program.EnableRemoting)
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
            if (Program.EnableRemoting && false)
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
            if (Program.EnableRemoting && false)
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
            if (Program.EnableRemoting && false)
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
            if (Program.EnableRemoting && false)
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
            if (Program.EnableRemoting && false)
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



        public static byte[] GetByteArrayFromString(string str)
        {
            System.Text.Encoding encoding = System.Text.Encoding.UTF8;
            return encoding.GetBytes(str);
        }
        public static string GetStringFromByteArray(object value)
        {
            if (value == null || value == DBNull.Value)
                return "";
            byte[] byArray = (byte[])value;
            System.Text.Encoding encoding = System.Text.Encoding.UTF8;
            return encoding.GetString(byArray);
        }

        public static string AddSlash(string sPath)
        {
            if (sPath.Length > 0 && sPath.Substring(sPath.Length - 1) != "\\")
            {
                sPath += "\\";
                return sPath;
            }
            return sPath;
        }
        private static string PreProcessString(ref string s1, ref string s2)
        {
            bool b2 = s2.Trim().Length > 0;
            string strfullpath = s2.Trim().Length > 0 ? s2 : s1;
            int nFindFileName = strfullpath.LastIndexOf('\\');
            string filename = "";
            if (nFindFileName >= 0)
            {
                filename = strfullpath.Substring(nFindFileName + 1);
                if (b2)
                    s2 = s2.Substring(0, nFindFileName + 1);
                else
                    s1 = s1.Substring(0, nFindFileName + 1);
            }
            return filename;

        }
        public static string CombinePaths(string p1, string p2)
        {
            string strRet;
            try
            {
                string strp1 = p1 == null ? "" : p1;
                string strp2 = p2 == null ? "" : p2;
                string filename = PreProcessString(ref strp1, ref strp2);

                strRet = Path.GetFullPath(Path.Combine(strp1, strp2));
                if (filename.Length > 0)
                    strRet += filename;
            }
            catch (Exception e)
            {
                //throw new Exception(string.Format("不能把 '{0}' 和 '{1}' 组合为路径，原因：{2}{3}",
                //            p1, p2, Environment.NewLine, e.Message));
                Helper.AddLog(string.Format("不能把 '{0}' 和 '{1}' 组合为路径，原因：{2}{3}",
                            p1, p2, Environment.NewLine, e.Message));
                return "";
            }

            return strRet;
        }

        public static string[] split(string strinput, string sp)
        {
            string tmp = "";
            int strlen = 0, splen = 0;
            int found = 0;
            string[] rt = null;//返回数组

            try
            {
                //输入判断
                if (strinput == null || sp == null || strinput.Length == 0 || sp.Length == 0) return null;

                //初始化一个数组列表（当做动态数组）
                ArrayList tmp3 = new ArrayList();

                strlen = strinput.Length;//获得输入字符串的长度
                splen = sp.Length;//分隔符的长度
                for (int i = 0; i < strlen; i++)
                {
                    //查找分隔符
                    found = strinput.IndexOf(sp, i);
                    if (found >= 0)
                    {
                        tmp = "";
                        //取分隔符前的字符串
                        tmp = strinput.Substring(i, found - i);
                        //添加到数组列表
                        tmp3.Add(tmp);
                        i = found + splen - 1;
                    }
                    else
                    {
                        string tmp2 = "";
                        //取最后的字符串
                        tmp2 = strinput.Substring(i);
                        if (tmp2 != "")
                            tmp3.Add(tmp2);
                        break;
                    }
                }

                //将动态数组的维数设置成实际存在的元素个数，因为数组列表是以16的倍数递增维数的
                tmp3.TrimToSize();
                //转换数组列表为字符串数组，并返回。
                rt = (string[])tmp3.ToArray(typeof(String));
                tmp3.Clear();
            }
            catch (Exception e)
            {
                //Console.WriteLine("{0}", e.Message);
                throw e;
            }

            return rt;
        }

        public static string GetFieldDataType(string tblname, string field, SqlConnection Conn)
        {
            /* remoting
            System.Data.DataTable tbl = GetDataTable("SELECT * FROM " + tblname, Conn);
            */

            string sql = "select top 1 * from " + tblname;

            DataTable tbl = GetDataTableWithSqlProxy(tblname, sql, Conn);
            DataColumn col = tbl.Columns[field];
            return col.DataType.ToString();
        }

        public static string GetHostName()
        {
            return System.Net.Dns.GetHostName();
        }

        public static string GetHostNameByIp(string ip)
        {
            ip = ip.Trim();
            if (ip == string.Empty)
                return string.Empty;
            try
            {
                // 是否 Ping 的通
                if (Ping(ip))
                {
                    System.Net.IPHostEntry host = System.Net.Dns.GetHostEntry(ip);
                    return host.HostName;
                }
                else
                    return ip;
            }
            catch (Exception)
            {
                return string.Empty;
            }
        }

        public static bool Ping(string ip)
        {
            System.Net.NetworkInformation.Ping p = new System.Net.NetworkInformation.Ping();
            System.Net.NetworkInformation.PingOptions options = new System.Net.NetworkInformation.PingOptions();
            options.DontFragment = true;
            string data = "Test Data!";
            byte[] buffer = Encoding.ASCII.GetBytes(data);
            int timeout = 1000; // Timeout 时间，单位：毫秒
            System.Net.NetworkInformation.PingReply reply = p.Send(ip, timeout, buffer, options);
            if (reply.Status == System.Net.NetworkInformation.IPStatus.Success)
                return true;
            else
                return false;
        }

        public static string GetIPAddress()
        {
            //System.Net.IPAddress addr;            
            //addr = new System.Net.IPAddress(System.Net.Dns.GetHostByName(System.Net.Dns.GetHostName()).AddressList[0].Address);
            //addr = new System.Net.IPAddress(ip);
            //return addr.ToString();

            //string ip = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList[0].ToString();
            //return ip;

            foreach (System.Net.IPAddress ip in System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList)
            {
                if (ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetworkV6)
                    continue;

                return ip.ToString();
            }

            throw new Exception("本机未找到合适的IPv4。");
            return string.Empty;
        }
        public static System.Net.IPAddress[] GetIPAddresses()
        {
            //System.Net.IPAddress addr;            
            //addr = new System.Net.IPAddress(System.Net.Dns.GetHostByName(System.Net.Dns.GetHostName()).AddressList[0].Address);
            //addr = new System.Net.IPAddress(ip);
            //return addr.ToString();
            return System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList;
        }

        public static string StringArrayToString(object[] strarray)
        {
            string str = "";
            if (strarray != null)
                foreach (object obj in strarray)
                {
                    str += obj + ";";
                }
            return str;
        }


        public static bool SqlUpdateRecord(string tblname, string[] keynames, string[] keys, string[] fields, string[] values, SqlConnection Conn)
        {
            bool bSuc = true;
            Conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                string str_key_rest = " WHERE ";
                for (int i = 0; i < keynames.Length; i++)
                {
                    str_key_rest += string.Format("{0}='{1}'", keynames[i], keys[i]);
                    if (i < keynames.Length - 1)
                    {
                        str_key_rest += " AND ";
                    }
                }

                for (int i = 0; i < fields.Length; i++)
                {
                    cmd.CommandText = string.Format("UPDATE {0} SET {1} = {2} {3}", tblname, fields[i], (values[i] == "<NULL>" ? "NULL" : "'" + values[i] + "'"), str_key_rest);
                    //cmd.ExecuteNonQuery();
                    ExecuteCommand(tblname, cmd.CommandText, Conn);
                }
            }
            catch (Exception ex)
            {
                bSuc = false;
                string err = string.Format("在更新表 {0} 时发生错误。\r\n\r\n错误信息：{1}", tblname, ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();
            return bSuc;
        }

        public static System.Data.DataTable ExcelToDataTable(string strXLS)
        {
            System.Data.DataTable dtRet = null;
            string strConn = string.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source={0};Extended Properties=Excel 8.0;", strXLS);
            OleDbConnection conn = new OleDbConnection(strConn);
            try
            {
                conn.Open();
                string strExcel = "";
                OleDbDataAdapter myCommand = null;
                strExcel = "select * from [sheet1$]";
                myCommand = new OleDbDataAdapter(strExcel, strConn);
                DataSet ds = new DataSet();
                myCommand.Fill(ds, "table1");
                dtRet = ds.Tables[0];
            }
            catch (Exception ex)
            {
                string err = string.Format("在打开xls文件时发生错误。\r\n\r\n错误信息为：{0}", ex.Message);
                MessageBox.Show(err);
            }
            finally
            {
                conn.Close();
            }
            return dtRet;
        }

        // 批量添加记录，目前只支持单主键
        public static object[] BatchSqlInsertRecord(string tblName, string displayField, string[] newDisplays, SqlConnection Conn)
        {
            // 获得DataTable
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblName, Conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            */

            string _sql = "SELECT * FROM " + tblName;

            DataTable tbl = Helper.GetDataTableProxy(tblName, Conn);
            // 获得目前最大id
            string sql = string.Format("SELECT MAX({0}) FROM {1}", tbl.PrimaryKey[0].ColumnName, tblName);
            //System.Data.DataTable dt_maxid = GetDataTable(sql, Conn);
            DataTable dt_maxid = GetDataTableWithSqlProxy(tblName, sql, Conn);
            int iCurMaxId = Convert.ToInt32(dt_maxid.Rows[0][0]);

            // 获得默认行
            DataRow defaultRow = tbl.Rows[0];

            // 批量加入行
            object[] IDs_ret = new object[newDisplays.Length];
            for (int i = 0; i < newDisplays.Length; i++)
            {
                DataRow newRow = tbl.NewRow();
                newRow.BeginEdit();
                foreach (DataColumn col in tbl.Columns)
                {
                    newRow[col] = defaultRow[col];
                }
                IDs_ret[i] = ++iCurMaxId;
                newRow[tbl.PrimaryKey[0]] = IDs_ret[i];
                newRow[displayField] = newDisplays[i];
                newRow.EndEdit();
                tbl.Rows.Add(newRow);
            }

            // 提交
            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            int val = Helper.SaveTable(tbl, _sql, Conn);

            return IDs_ret;
        }
    }

    class ComboComparer : IComparer
    {
        public int Compare(object o1, object o2)
        {
            return Convert.ToInt32(((DictionaryEntry)o1).Value) - Convert.ToInt32(((DictionaryEntry)o2).Value);
        }
    };

    class StringComparer : IComparer
    {
        public int Compare(object o1, object o2)
        {
            string sK1 = ((DictionaryEntry)o1).Key.ToString().Trim();
            string sK2 = ((DictionaryEntry)o2).Key.ToString().Trim();
            return string.Compare(sK1, sK2);
        }
    };

    class DictionaryEntryValueComparer: IComparer
    {
        public int Compare(object o1, object o2)
        {
            string sK1 = ((DictionaryEntry)o1).Value.ToString().Trim();
            string sK2 = ((DictionaryEntry)o2).Value.ToString().Trim();
            return string.Compare(sK1, sK2);
        }
    }
}
