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
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using DevComponents.DotNetBar;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    class Helper
    {
        // import
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public static IniStructure m_inis = new IniStructure();
        private static string m_filename = string.Empty;

        [DllImport("psapi.dll")]
        public static extern bool EnumProcesses(IntPtr processes, int bytes_cap, IntPtr bytes_ret);
        [DllImport("user32.dll")]
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

        // ahpho suntao
        private static string rootDir; // 客户端根目录
        // end ahpho

        /// <summary>
        /// 客户端根目录
        /// </summary>
        public static string RootDir
        {
            set
            {
                rootDir = value;
            }

            get
            {
                return rootDir;
            }
        }

        public static TreeNode AncestorNodeOf(TreeNode node)
        {
            TreeNode parent = node.Parent;
            if (parent == null)
            {
                return node;
            }
            while (parent != null)
            {
                node = node.Parent;
                parent = node.Parent;
            }
            return node;
        }

        public static void SceneToLogical(ref int X, ref int Y, ref int Z, float posX, float posY, float posZ, float fGroundHeight, float LogicalXStart, float LogicalZStart)
        {
            const int LOGICAL_CELL_CM_LENGTH = 50;
            const float ALTITUDE_UNIT = LOGICAL_CELL_CM_LENGTH / 4.0f;
            const float TERRAIN_MIN_HEIGHT = -65536.0f / 4.0f * ALTITUDE_UNIT;
            const int ALTITUDE_BIT_NUM = 6;
            const int POINT_PER_ALTITUDE = 1 << ALTITUDE_BIT_NUM;

            X = (int)((posX - LogicalXStart) / 50.0F * 32);
            Y = (int)((posZ - LogicalZStart) / 50.0F * 32);
            Z = (int)((posY - TERRAIN_MIN_HEIGHT) * POINT_PER_ALTITUDE / ALTITUDE_UNIT);
        }

        public static void SetAdditionalEnvPath(string strStartupPath)
        {
            string spath = Environment.GetEnvironmentVariable("path");
            DirectoryInfo di = new DirectoryInfo(strStartupPath);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                spath += ";" + d.FullName;
            }
            Environment.SetEnvironmentVariable("path", spath);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                SetAdditionalEnvPath(d.FullName);
            }
        }

        public static unsafe sbyte* ToAnsiChar(string strIn)
        {
            return (sbyte*)Marshal.StringToHGlobalAnsi(strIn);
        }

        public static void LockWndUpdate(Control ctrl)
        {
            if (ctrl != null)
                LockWindowUpdate(ctrl.Handle);
            else
                LockWindowUpdate(IntPtr.Zero);
        }

        public static string GetVersionInfo()
        {
            string strPath = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\";
            //string strFilename = Process.GetCurrentProcess().MainModule.FileName.Split(new string[]{".vshost.exe"}, StringSplitOptions.RemoveEmptyEntries)[0] + ".exe";
            string strFilename = strPath + "GameDesingerTools.exe";
            Assembly assembly = Assembly.LoadFile(strFilename);
            string fileVersion = null;
            object[] fileVersionAttributes = assembly.GetCustomAttributes(typeof(AssemblyFileVersionAttribute), true);
            if (fileVersionAttributes.Length != 0)
            {
                fileVersion = (fileVersionAttributes[0] as AssemblyFileVersionAttribute).Version.ToString();
            }
            return fileVersion;
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
//             Program.MainForm.LogEditBox.Text += strmsg + "\r\n";
//             Program.MainForm.LogEditBox.Update();
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
            SqlCommand cmd = Conn.CreateCommand();
            string sql = string.Format("sp_rename '{0}', '{1}', 'OBJECT'", oldTblName, newTblName);
            cmd.CommandText = sql;
            cmd.ExecuteNonQuery();
            return true;
        }

        public static bool RenameDbField(string tblName, string oldFieldName, string newFieldName, SqlConnection Conn)
        {
            SqlCommand cmd = Conn.CreateCommand();
            string _Old = tblName + "." + oldFieldName;
            string sql = string.Format("sp_rename '{0}', '{1}', 'COLUMN'", _Old, newFieldName);
            cmd.CommandText = sql;
            cmd.ExecuteNonQuery();
            return true;
        }

        public static bool RemoveDbTable(string tblName, SqlConnection Conn)
        {
            bool bSuc = false;
            Conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = "DROP TABLE " + tblName;
                cmd.ExecuteNonQuery();
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


        public static System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
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
            System.Data.DataTable tbl = GetDataTable("SELECT * FROM " + tblname, Conn);
            DataColumn col = tbl.Columns[field];
            return col.DataType.ToString();
        }

        public static string GetHostName()
        {
            return System.Net.Dns.GetHostName();
        }
        public static string GetIPAddress()
        {
            //System.Net.IPAddress addr;            
            //addr = new System.Net.IPAddress(System.Net.Dns.GetHostByName(System.Net.Dns.GetHostName()).AddressList[0].Address);
            //addr = new System.Net.IPAddress(ip);
            //return addr.ToString();
            string ip = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList[0].ToString();
            return ip;
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
                    cmd.ExecuteNonQuery();
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

        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }
        public static void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                CreatePath(strFileName);
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }
        public static string FileToString(string strFileName)
        {
            return FileToString(strFileName, Encoding.GetEncoding("gb2312"));
        }
        public static string FileToString(string strFileName, Encoding encoding)
        {
            StreamReader reader;
            try
            {
                reader = new StreamReader(strFileName, encoding);
            }
            catch //(Exception e)
            {
                //MessageBox.Show(e.Message);
                return "";
            }
            string strContent = reader.ReadToEnd();
            reader.Close();
            return strContent;
        }

        // 系统ini文件操作
        public static int ReadIniFile(string section, string key, string def, StringBuilder retVal, int size, string filePath)
        {
            return GetPrivateProfileString(section, key, def, retVal, size, filePath);
        }
        public static long WriteIniFile(string section, string key, string val, string filePath)
        {
            return WritePrivateProfileString(section, key, val, filePath);
        }
        // 优化的ini文件操作
        public static bool OpenIniFile(string filename, string content)
        {
            //m_inis = IniStructure.ReadIni(filename);
            m_inis = IniStructure.ReadIniWithContent(content);
            m_filename = filename;
            return true;
        }
        public static void CloseIniFile()
        {
            m_inis = null;
            m_filename = string.Empty;
        }
        public static string GetIniString(string section, string key)
        {
            return m_inis.GetValue(section, key);
        }

        // 批量添加记录，目前只支持单主键
        public static object[] BatchSqlInsertRecord(string tblName, string displayField, string[] newDisplays, SqlConnection Conn)
        {
            // 获得DataTable
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblName, Conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];

            // 获得目前最大id
            string sql = string.Format("SELECT MAX({0}) FROM {1}", tbl.PrimaryKey[0].ColumnName, tblName);
            System.Data.DataTable dt_maxid = GetDataTable(sql, Conn);
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
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();

            return IDs_ret;
        }

        public static void SwapF(ref float f1, ref float f2)
        {
            float dummi = f1;
            f1 = f2;
            f2 = dummi;
        }

        public static int Abs(int i)
        {
            if (i > 0) return i;
            else return -i;
        }

        public static bool PointsAreNear(Point p1, Point p2)
        {
            if (Abs(p1.X - p2.X) < 5 && Abs(p1.Y - p2.Y) < 5)
                return true;
            else
                return false;
        }

        public static bool ProgramAlreadyRunning()
        {
            IntPtr pid_buffer = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)) * 1000);
            IntPtr pbytes_ret_buffer = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)));

            EnumProcesses(pid_buffer, Marshal.SizeOf(typeof(int)) * 1000, pbytes_ret_buffer);
            int[] bytes_ret = new int[1];
            Marshal.Copy(pbytes_ret_buffer, bytes_ret, 0, 1);
            int pcount = bytes_ret[0] / Marshal.SizeOf(typeof(int));
            int[] pids = new int[pcount];
            Marshal.Copy(pid_buffer, pids, 0, pcount);

            int GameDesignerToolsRunningInstanceCount = 0;
            foreach (int pid in pids)
            {
                try // 不能获得对应id的process也抛异常，太傻了
                {
                    Process p = Process.GetProcessById(pid);
                    string ToolName = typeof(Program).Namespace;
                    if (p.ProcessName.ToLower() == ToolName.ToLower())
                    {
                        GameDesignerToolsRunningInstanceCount++;
                    }
                }
                catch
                {
                    continue;
                }
            }

            if (GameDesignerToolsRunningInstanceCount > 1)
                return true;
            else
                return false;
        }

        /// <summary>
        /// 获取对象信息数据表
        /// </summary>
        /// <param name="objectInfo">对象信息</param>
        /// <returns>数据表</returns>
        public static Hashtable GetInfoTable(_AtlObjInfo objectInfo)
        {
            Hashtable infoTable = new Hashtable();

            for (int i = 0; i < objectInfo.iFieldsCount; i++)
            {
                infoTable[objectInfo.strFields[i]] = objectInfo.strValues[i];
            }

            return infoTable;
        }

        public static string GetFileContent(string filePath)
        {
            string content = null;

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "读取数据文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return content;
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        public static DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }

            }

            return table;
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


}
