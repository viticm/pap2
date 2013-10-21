using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using LuaInterface;
using System.Diagnostics;
using System.IO;

namespace GameDesingerTools
{
    public partial class TabExport : Form
    {
        public static System.Data.SqlClient.SqlConnection m_conn;
        DataTable m_ExportTable = new DataTable();
        int[] m_anTableID;
        private TimeLog log = new TimeLog();
        private string m_strModlName;
        
        public TabExport(int nModelID, SqlConnection conn)
        {
            InitializeComponent();
            //m_conn = new SqlConnection(m_strconn);
            m_conn = conn;
            try
            {
                string strSQL = string.Format("SELECT sys_export_table_cfg.id, tablename, filename as filepath, modl.modelname_en FROM sys_export_modl_cfg, sys_export_table_cfg, sys_modl_def modl WHERE sys_export_modl_cfg.modid = {0} and modl.id = {0} and sys_export_modl_cfg.tableid = sys_export_table_cfg.id"
                    , nModelID);
                /* remoting
                SqlDataAdapter adp = new SqlDataAdapter(strSQL, m_conn);
                DataSet ds = new DataSet();
                adp.Fill(ds);
                m_ExportTable = ds.Tables[0];
                */
                string[] names = new string[] { "sys_export_table_cfg", "sys_export_modl_cfg", "sys_modl_def" };
                m_ExportTable = Helper.GetDataTableWithSqlProxy(names, strSQL, m_conn);


                DataColumn[] primaryKey = new DataColumn[1];
                primaryKey[0] = m_ExportTable.Columns["id"];
                m_ExportTable.PrimaryKey = primaryKey;
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
                return;
            }

            int nRowCount = m_ExportTable.Rows.Count;
            int nIndex = 0;
            string[] str = new string[nRowCount];
            m_anTableID = new int[nRowCount];

            foreach (DataRow row in m_ExportTable.Rows)
            {
                if (m_strModlName == null)
                    m_strModlName = row["modelname_en"].ToString();
                string strTableName = row["tablename"].ToString().Trim();
                string strFilePath = row["filepath"].ToString().Trim();
                str[nIndex] = string.Format("表名:{0} ; 导出的相对路径:{1}", strTableName, strFilePath);
                m_anTableID[nIndex] = (int)row["id"];
                nIndex++;
            }

            this.checkedListBox1.Items.AddRange(str);
            log.Log(TimeLog.enumLogType.ltstart, "数据导出", "功能启动", null);
        }

        private bool CallLuaExport(string strTableName, ref string strResult)
        {
            string strHeader = string.Empty;
            string strContent = string.Empty;
            String luafile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\export.lua";
            if (!File.Exists(luafile))
                return false;

            if (GenerateString(strTableName, ref strHeader, ref strContent))
            {
                bool bRet = CallLuaExport(luafile, strTableName, ref strHeader, ref strContent);

                return bRet;
            }
            return false;
        }
        private bool CallLuaPostExport(string strTableName, string strTargetPath)
        {
            string oldcurdir = System.IO.Directory.GetCurrentDirectory();
            System.IO.Directory.SetCurrentDirectory(Application.StartupPath);

            string luafile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\post_export.lua";
            if (!File.Exists(luafile))
                return false;
            LuaEx lua = new LuaEx();
            lua["RootDir"] = Program.RootDir;
            lua["Conn"] = m_conn;
            lua.DoString("function trace(txt)\r\n    MainForm.LogEditBox.Visible = true\r\n    MainForm.LogEditBox.OutputBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n' \r\n end");
            lua.RegisterFunction("msgbox", this, typeof(TabExport).GetMethod("ShowMessage"));
            lua.RegisterFunction("GetDataTableRow", this, typeof(TabExport).GetMethod("GetDataTableRow"));
            lua.DoFile(luafile);
            LuaFunction fun = lua.GetFunction("onpostexport");
            if (fun != null)
            {
                object[] retobjs = fun.Call(strTableName, strTargetPath);
                if (retobjs.Length > 0)
                {
                    System.IO.Directory.SetCurrentDirectory(oldcurdir);
                    return (bool)retobjs[0];
                }
            }
            System.IO.Directory.SetCurrentDirectory(oldcurdir);
            return false;
        }

        private void Export_button_Click(object sender, EventArgs e)
        {
            StringBuilder strRootDir = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("General", "DefaultStorageDir", "", strRootDir, 255, Application.StartupPath + "/GameDesingerTools.ini");
            if (strRootDir.Length <= 0)
            {
                MessageBox.Show("您的工具选项中导出数据页中没有设置导出的根路径，请先为导出文件设置根目录!", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (this.checkedListBox1.Items.Count == 0)
                return;

            LuaExporter luaExporter = new LuaExporter();
            string exportLuaFile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\export.lua";
            string postExportLuaFile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\post_export.lua";
            string UiTableCfgFile = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName) + @"\UITableConfig.ini";
            luaExporter.InitLuaFile(exportLuaFile, postExportLuaFile, UiTableCfgFile);

            string strResult = "";
            for (int i = 0; i < checkedListBox1.Items.Count; ++i)
            {
                bool bRetCode = checkedListBox1.GetItemChecked(i);
                if (!bRetCode)
                    continue;

                DataRow row = m_ExportTable.Rows.Find(m_anTableID[i]);

                if (row == null)
                    return;

                string strTableName = row["tablename"].ToString().Trim();
                string strFilePath = row["filepath"].ToString().Trim();   // 相对路径
                if (strFilePath.Length > 0)
                {
                    StringBuilder sb = new StringBuilder(strRootDir.ToString());
                    if (strFilePath[0] != '\\' && sb[sb.Length - 1] != '\\')
                    {
                        sb.Append('\\');
                    }
                    strFilePath = sb.Append(strFilePath).ToString();          // 绝对路径
                }
                
                //这里开始作脚本扩展
                bool bExclusive = false;
                if (luaExporter.IsInterestedIn(strTableName, true))
                {
                    bExclusive = CallLuaExport(strTableName, ref strResult);
                }
                if (!bExclusive)
                {
                    bRetCode = ExportTab(strTableName, strFilePath);
                }

                // 导出过后有post-export的脚本扩展
                bool bSucPostExport = CallLuaPostExport(strTableName, strFilePath);

                if (bRetCode && bSucPostExport)
                {
                    strResult += string.Format("成功导出数据表{0}到{1}\r\n", strTableName, strFilePath);
                }
            }
            
            if (strResult != "")
            {                
                MessageBox.Show(strResult, "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            string strmsg = string.Format("结果为：{0}", strResult);
            log.Log(TimeLog.enumLogType.ltend, null, null, strmsg);
        }

        private void SelectAll_button_Click(object sender, EventArgs e)
        {
            for (int nIdnex = 0; nIdnex < checkedListBox1.Items.Count; ++nIdnex)
            {
                checkedListBox1.SetItemChecked(nIdnex, true);
            }
        }
        public void ShowMessage(string str)
        {
            MessageBox.Show(str, "信息", MessageBoxButtons.OK, MessageBoxIcon.Information); 
        }

        public bool GetDataTableRow(string tablename, LuaTable keys, ref LuaTable ltReturn)
        {
            string sql = string.Format("SELECT * FROM {0}", tablename);
            //DataTable tbl = Helper.GetDataTable(sql, m_conn);

            DataTable tbl = Helper.GetDataTableProxy(tablename, m_conn);

            if (tbl.PrimaryKey.Length != keys.Keys.Count)
                MessageBox.Show(string.Format("{0} —— 函数GetDataTableRow断言：{1}\r\n", DateTime.Now, "输入参数keys长度错误。"), "信息", MessageBoxButtons.OK, MessageBoxIcon.Error); 

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            for (int i = 0; i < KEYS.Length; i++)
            {
                KEYS[i] = ((LuaTable)keys)[i];
            }
            DataRow row = tbl.Rows.Find(KEYS);

            foreach (DataColumn col in tbl.Columns)
            {
                ltReturn[col.ColumnName] = row[col.ColumnName];
            }
            return true;
        }

/*        public string[] CloneValue(string[] strvalues)
        {
            if (strvalues != null)
                return (string[])(strvalues.Clone());
            return null;
        }*/
        private bool CallLuaExport(string strLuaFile, string strTableName, ref string fields, ref string values)
        {
            LuaEx lua = new LuaEx();
            lua["Conn"] = m_conn; // 注册sql连接
            lua["RootDir"] = Program.RootDir;
            lua.DoString("function trace(txt)\r\n    MainForm.LogEditBox.Visible = true\r\n    MainForm.LogEditBox.OutputBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n' \r\n end");
            lua.RegisterFunction("writefile", this, typeof(FileFolderHelper).GetMethod("WriteStringToFile"));
            lua.RegisterFunction("msgbox", this, typeof(TabExport).GetMethod("ShowMessage"));
            lua.RegisterFunction("GetDataTableRow", this, typeof(TabExport).GetMethod("GetDataTableRow"));
            //lua.RegisterFunction("clonevalues", this, typeof(TabExport).GetMethod("CloneValue"));            
            
            String luafile = strLuaFile;
            try
            {
                if (!File.Exists(luafile))
                    return false;
                try
                {
                    lua.DoFile(luafile);
                    LuaFunction fun = lua.GetFunction("onexport");
                    if (fun != null)
                    {
                        ImportInfoHelper helper = new ImportInfoHelper();
                        ArrayList feildlist = new ArrayList();
                        feildlist.AddRange(fields.Split(new char[] { '\t' }));
                        helper.Fields = feildlist;
                        helper.Values = values.Split(new string[] { "\r\n" }, StringSplitOptions.None);
                        object[] retobjs = fun.Call(m_strModlName, strTableName, Program.RootDir, helper);

                        if (retobjs != null && retobjs.GetLength(0) > 0)
                        {
                            if (retobjs[0] is bool)
                            {
                                StringBuilder strFields = new StringBuilder();
                                foreach (string strLine in feildlist)
                                {
                                    strFields.Append(strLine);
                                    strFields.Append('\t');
                                }
                                strFields.Remove(strFields.Length - 1, 1);
                                strFields.Append("\r\n");
                                fields = strFields.ToString();

                                StringBuilder strValues = new StringBuilder();
                                foreach (string strLine in helper.Values)
                                {
                                    strValues.Append(strLine);
                                    strValues.Append("\r\n");
                                }
                                values = strValues.ToString();

                                return (bool)retobjs[0];
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    ScriptDebugForm frm = Program.MainForm.DebugForm;
                    frm.OutputBox.Text += ex.Message + "\r\n";
                    frm.Show();
                    frm.BringToFront();
                }
            }
            finally
            {
                lua.Dispose();
            }
            return false;
        }

        private bool StrInStrArr(string str, string[] strArr)
        {
            foreach (string s in strArr)
            {
                if (s.ToLower() == str.ToLower())
                    return true;
            }
            return false;
        }

        private bool GenerateString(string strTableName, ref string strHeader, ref string strContent)
        {
            //sql connection
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT TOP 1 * FROM " + strTableName, m_conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable Table = ds.Tables[0];
            */

            DataTable Table = Helper.GetDataTableTopRowProxy(strTableName, m_conn);

            // 数据库自定义的order
           
            string sql_order = string.Format("SELECT export_order FROM sys_export_table_cfg WHERE tablename='{0}'", strTableName);
             /*
              * DataTable tbl_define_order = Helper.GetDataTable(sql_order, m_conn);
             */
            string filter = string.Format("tablename='{0}'", strTableName);
            DataTable tbl_define_order = Helper.GetDataTableProxy("sys_export_table_cfg", filter, null, m_conn);

            string strDefinedOrder = string.Empty;
            if (tbl_define_order.Rows.Count > 0)
            {
                strDefinedOrder = tbl_define_order.Rows[0]["export_order"].ToString().Trim();
            }

            // 导出时按照主键们来排序.
            if (Table.PrimaryKey.Length > 0)
            {
                string ordering = string.Empty;
                for (int i = 0; i < Table.PrimaryKey.Length; i++)
                {
                    ordering += ("[" + Table.PrimaryKey[i].ColumnName + "]");
                    if (i != Table.PrimaryKey.Length - 1)
                        ordering += ",";
                }
                if (strDefinedOrder.Length > 0)
                    ordering = strDefinedOrder;
                /* remoting
                adp = new SqlDataAdapter("SELECT * FROM " + strTableName + " ORDER BY " + ordering, m_conn);
                ds = new DataSet();
                adp.Fill(ds);
                Table = ds.Tables[0];
                */

                Table = Helper.GetDataTableProxy(strTableName, null, ordering, m_conn);
            }
            else
            {
                //Table = Helper.GetDataTable("SELECT * FROM " + strTableName, m_conn);
                Table = Helper.GetDataTableProxy(strTableName, m_conn);
            }

            // export ignore
            string strIgnore = string.Empty;
            string sql_ign = string.Format("SELECT export_ignore FROM sys_export_table_cfg WHERE tablename='{0}'", strTableName);
            filter = string.Format("tablename='{0}'", strTableName);

            //DataTable tbl_ign = Helper.GetDataTable(sql_ign, m_conn);

            DataTable tbl_ign = Helper.GetDataTableProxy("sys_export_table_cfg", filter, null, m_conn);
            if (tbl_ign.Rows.Count > 0)
            {
                strIgnore = tbl_ign.Rows[0]["export_ignore"].ToString().Trim();
            }
            string[] strIgnores = strIgnore.Split(new char[] {','});

            // string formatting
            StringBuilder strHeaderBuf = new StringBuilder();
            StringBuilder strContentBuf = new StringBuilder();

            bool bHasTitle = true;
            /* remoting
            string sql = string.Format("SELECT * FROM sys_export_table_cfg WHERE tablename='{0}'", strTableName);
            DataTable tbl_cfg = Helper.GetDataTable(sql, m_conn);
            */
            DataTable tbl_cfg = tbl_ign;
            if (tbl_cfg.Rows.Count > 0)
            {
                if (tbl_cfg.Rows[0]["hastitle"] != DBNull.Value)
                    bHasTitle = Convert.ToBoolean(tbl_cfg.Rows[0]["hastitle"]);
            }

            if (bHasTitle)
            {
                foreach (System.Data.DataColumn col in Table.Columns)
                {
                    if (!StrInStrArr(col.ColumnName, strIgnores))
                    {
                        strHeaderBuf.Append(col.ColumnName);
                        strHeaderBuf.Append("\t");
                    }
                }
                strHeaderBuf.Remove(strHeaderBuf.Length - 1, 1);
                strHeaderBuf.Append("\r\n");
            }

            foreach (System.Data.DataRow row in Table.Rows)
            {
                foreach (System.Data.DataColumn col in Table.Columns)
                {
                    if (StrInStrArr(col.ColumnName, strIgnores))
                        continue;

                    string item = row[col.ColumnName].ToString();

                    /*while (item.Contains("\n"))
                    {
                        item = item.Remove(item.IndexOf('\n'), 1);
                    }*/
                    if (item.Contains("\n"))
                        item = item.Replace("\n", string.Empty);

                    Type type = row[col.ColumnName].GetType();
                    if (type.Name == "Boolean")
                    {
                        item = item.ToLower() == "true" ? "1" : "0";
                    }
                    strContentBuf.Append(item); // it works even if item==""
                    strContentBuf.Append("\t");
                }
                strContentBuf.Remove(strContentBuf.Length - 1, 1);
                strContentBuf.Append("\r\n");
            }

            strHeader = strHeaderBuf.ToString();
            strContent = strContentBuf.ToString();
            return true;
        }

        public bool ExportTab(string strTableName, string strDestFile)
        {
            string strHeader = string.Empty;
            string strContent = string.Empty;
            if (!GenerateString(strTableName, ref strHeader, ref strContent))
                return false;

            try
            {
                StringBuilder strFinal = new StringBuilder();
                strFinal.Append(strHeader);
                strFinal.Append(strContent);
                
                FileFolderHelper.StringToFile(strFinal.ToString(), strDestFile);
                return true;
            }
            catch (Exception ex)
            {
                string s = string.Format("导出 tab 文件:\r\n    {0}\r\n时出现错误。请检查文件名，磁盘空间等。\r\n\r\n错误讯息为:{1}", strDestFile, ex.Message);
                System.Windows.Forms.MessageBox.Show(s, "Error");
                return false;
            }
        }

        private void TabExport_FormClosed(object sender, FormClosedEventArgs e)
        {
            log.Log(TimeLog.enumLogType.ltend, null, null, null);
            log.Clear();
        }

        private void btOpenDictionary_Click(object sender, EventArgs e)
        {
            StringBuilder strRootDir = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("General", "DefaultStorageDir", "", strRootDir, 255, Application.StartupPath + "/GameDesingerTools.ini");
            if (strRootDir.Length <= 0)
            {
                MessageBox.Show("您的工具选项中导出数据页中没有设置导出的根路径，请先为导出文件设置根目录!", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (this.checkedListBox1.Items.Count == 0)
                return;

            for (int i = 0; i < checkedListBox1.Items.Count; ++i)
            {
                bool bRetCode = checkedListBox1.GetItemChecked(i);
                if (!bRetCode)
                    continue;

                DataRow row = m_ExportTable.Rows.Find(m_anTableID[i]);

                if (row == null)
                    return;

                string strFilePath = row["filepath"].ToString().Trim();   // 相对路径
                if (strFilePath.Length > 0)
                {
                    StringBuilder sb = new StringBuilder(strRootDir.ToString());
                    if (strFilePath[0] != '\\' && sb[sb.Length - 1] != '\\')
                    {
                        sb.Append('\\');
                    }
                    strFilePath = sb.Append(strFilePath).ToString();          // 绝对路径                    
                    strFilePath = strFilePath.Replace('/', '\\').Replace("\\\\", "\\");            //去掉多余的\\        
                    System.Diagnostics.Process.Start("explorer.exe", @"/select," + strFilePath);
                }
            }
                
        }

        private void btOpenExportedFile_Click(object sender, EventArgs e)
        {
            StringBuilder strRootDir = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("General", "DefaultStorageDir", "", strRootDir, 255, Application.StartupPath + "/GameDesingerTools.ini");
            if (strRootDir.Length <= 0)
            {
                MessageBox.Show("您的工具选项中导出数据页中没有设置导出的根路径，请先为导出文件设置根目录!", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (this.checkedListBox1.Items.Count == 0)
                return;

            for (int i = 0; i < checkedListBox1.Items.Count; ++i)
            {
                bool bRetCode = checkedListBox1.GetItemChecked(i);
                if (!bRetCode)
                    continue;

                DataRow row = m_ExportTable.Rows.Find(m_anTableID[i]);

                if (row == null)
                    return;
 
                string strFilePath = row["filepath"].ToString().Trim();   // 相对路径
                if (strFilePath.Length > 0)
                {
                    StringBuilder sb = new StringBuilder(strRootDir.ToString());
                    if (strFilePath[0] != '\\' && sb[sb.Length - 1] != '\\')
                    {
                        sb.Append('\\');
                    }
                    strFilePath = sb.Append(strFilePath).ToString();          // 绝对路径
                    int nIndex = strFilePath.LastIndexOf('\\');
                    strFilePath = strFilePath.Replace('/', '\\').Replace("\\\\", "\\"); ;//去掉多余的\\
                    System.Diagnostics.Process.Start(strFilePath);
                }
            }
        }
    }

    public class LuaExporter
    {
        private string m_exportLuaContent = string.Empty;
        private string m_postExportLuaContent = string.Empty;
        private string m_UiTableCfgContent = string.Empty;

        public LuaExporter() {}

        public void InitLuaFile(string exportLuaFileName, string postExportLuaFileName, string uiTableCfgFileName)
        {
            m_exportLuaContent = FileFolderHelper.FileToString(exportLuaFileName);
            m_postExportLuaContent = FileFolderHelper.FileToString(postExportLuaFileName);
            m_UiTableCfgContent = FileFolderHelper.FileToString(uiTableCfgFileName);
        }

        public bool IsInterestedIn(string tblname, bool export)
        {
            bool b = export ? m_exportLuaContent.IndexOf("'" + tblname + "'") != -1 : m_postExportLuaContent.IndexOf("\"" + tblname + "\"") != -1;
            if (!b)
                b = export ? m_exportLuaContent.IndexOf("'" + tblname.ToLower() + "'") != -1 : m_postExportLuaContent.IndexOf("\"" + tblname.ToLower() + "\"") != -1;

            // 是否UITableConfig.ini中是感兴趣的
            if (!b && export)
                b = m_UiTableCfgContent.IndexOf(tblname) != -1;

            return b;
        }
    }
}