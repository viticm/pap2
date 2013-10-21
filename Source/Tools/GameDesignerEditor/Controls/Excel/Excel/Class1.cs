using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.Runtime.InteropServices;
using LuaInterface;
using Excel;
using DevComponents.DotNetBar;
using System.Data.SqlClient;
using System.Data;

namespace Excel
{
    public class Class1
    {
        Excel.Application excelApp = null;
        Workbook workbook = null;
        bool CopyLogics = false;
        object[] CopiedKeys = null;
        SqlConnection Conn = null;

        public Class1(string strXL, object extraMenuItem, SqlConnection conn)
        {
            Conn = conn;

            try
            {
                excelApp = new Excel.ApplicationClass();
                workbook = excelApp.Workbooks.Open(strXL, 0, false, 5, "", "", false,
                                Excel.XlPlatform.xlWindows, "", true,
                                false, 0, true, false, false);
            }
            catch(Exception ex)
            {
                MessageBox.Show("在打开Excel文件{0}时产生异常: " + ex.ToString());
            }     
        }

        ~Class1()
        {
            try
            {
                workbook.Close(false, Missing.Value, Missing.Value);
               // Marshal.ReleaseComObject(workbook);
                workbook = null;
                if (excelApp.Workbooks.Count == 0)
                {
                    //Marshal.ReleaseComObject(excelApp.Workbooks);
                    excelApp.Quit();
                   // Marshal.ReleaseComObject(excelApp);
                    killexcel(excelApp);
                    excelApp = null;
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("在释放系统资源时产生异常: " + ex.ToString());
            }     

//             if (workbook != null)
//             {
//                 workbook.Close(false, Missing.Value, Missing.Value);
//                 Marshal.ReleaseComObject(workbook);
//                 workbook = null;
//             }
//             Marshal.ReleaseComObject(excelApp.Workbooks);
//             excelApp.Quit();
//             Marshal.ReleaseComObject(excelApp);
//             excelApp = null;
        }

        // 运行excel的vba宏
        public object RunExcelMacro(string strMacroName, object args)
        {
            LuaTable ltArgs = args as LuaTable;
            if (ltArgs == null)
            {
                MessageBox.Show("调用RunExcelMacro(string strMacroName, object args)时参数为空！");
                return null;
            }

            object[] oArgArr = new object[ltArgs.Values.Count];
            for (int i = 0; i < oArgArr.Length; i++)
            {
                oArgArr[i] = ltArgs[i];
            }

            object[] ArgArr30 = new object[30];
            for (int i = 0; i < oArgArr.Length; i++)
            {
                ArgArr30[i] = oArgArr[i];
            }
            for (int i = oArgArr.Length; i < ArgArr30.Length; i++)
            {
                ArgArr30[i] = Missing.Value;
            }

            if(excelApp == null) // 未初始化成功
            {
                MessageBox.Show("运行Excel的vba宏失败!\n\n请检查是否已在机器上装有Excel.");
                return null; // 脚本已经对null值的处理 
            }

            object result_macro = null;
            try
            {
                result_macro = excelApp.Run(strMacroName,
                ArgArr30[0],  ArgArr30[1],  ArgArr30[2],  ArgArr30[3],  ArgArr30[4],  ArgArr30[5],
                ArgArr30[6],  ArgArr30[7],  ArgArr30[8],  ArgArr30[9],  ArgArr30[10], ArgArr30[11],
                ArgArr30[12], ArgArr30[13], ArgArr30[14], ArgArr30[15], ArgArr30[16], ArgArr30[17],
                ArgArr30[18], ArgArr30[19], ArgArr30[20], ArgArr30[21], ArgArr30[22], ArgArr30[23],
                ArgArr30[24], ArgArr30[25], ArgArr30[26], ArgArr30[27], ArgArr30[28], ArgArr30[29]);
            }
            catch (Exception ex)
            {
                MessageBox.Show("宏运行错误,请不要保存所作修改！");
            }

            return (result_macro as string[]);
#if false
            Excel.ApplicationClass oExcel = new Excel.ApplicationClass();
            oExcel.Visible = true;
            Excel.Workbooks oBooks = oExcel.Workbooks;
            Excel._Workbook oBook = null;
            string strXL = "C:/Documents and Settings/kuangsihao/桌面/剑网3-NPC数值.xlsm";
            oBook = oBooks.Open(strXL, oMissing, oMissing,
                oMissing, oMissing, oMissing, oMissing, oMissing, oMissing,
                oMissing, oMissing, oMissing, oMissing, oMissing, oMissing);
            RunMacro(oExcel, new Object[] { "矩形1_Click" });

            private void RunMacro(object oApp, object[] oRunArgs)
            {
                oApp.GetType().InvokeMember("Run",
                    System.Reflection.BindingFlags.Default |
                    System.Reflection.BindingFlags.InvokeMethod,
                    null, oApp, oRunArgs);
            }
#endif
        }

        public void CopyNpcLogics(object[] keys)
        {
            CopyLogics = true;
            CopiedKeys = keys;
        }

        public void CopyNpcRepresent(object[] keys)
        {
            CopyLogics = false;
            CopiedKeys = keys;
        }

        public void UpdateAllAIType()
        {
            Conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = "UPDATE NpcTemplate SET AIType = ID";
                cmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                string err = string.Format("在更新表时发生错误。\r\n\r\n错误信息：{1}", ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();
        }

        public bool PasteNpc(object[] des_keys)
        {
            string strLogic = string.Format("您选择了从 [{0}] 号复制逻辑数据到 [{1}] 号。\r\n\r\n[{2}] 号记录中，除了ID、Name、RepresentID1 - RepresentID10 之外的所有字段都将被重写。\r\n\r\n确定吗？", CopiedKeys[0], des_keys[0], des_keys[0]);
            string strRepre = string.Format("您选择了从 [{0}] 号复制表现数据到 [{1}] 号。\r\n\r\n[{2}] 号记录中，将分配新的表现ID。\r\n\r\n确定吗？", CopiedKeys[0], des_keys[0], des_keys[0]);
            DialogResult result = MessageBox.Show(CopyLogics ? strLogic : strRepre, "提示", MessageBoxButtons.YesNo);
            if (result != DialogResult.Yes)
                return false;

            SqlDataAdapter adp_tmplt = new SqlDataAdapter("SELECT * FROM NpcTemplate", Conn);
            DataSet ds_tmplt = new DataSet();
            adp_tmplt.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp_tmplt.Fill(ds_tmplt);
            System.Data.DataTable tbl_tmplt = ds_tmplt.Tables[0];

            SqlDataAdapter adp_rep = new SqlDataAdapter("SELECT * FROM npc", Conn);
            DataSet ds_rep = new DataSet();
            adp_rep.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp_rep.Fill(ds_rep);
            System.Data.DataTable tbl_rep = ds_rep.Tables[0];

            DataRow des_row = tbl_tmplt.Rows.Find(des_keys);
            DataRow src_row = tbl_tmplt.Rows.Find(CopiedKeys);
            if (des_row == null || src_row == null)
                return false;

            if (CopyLogics)
            {
                foreach (DataColumn col in tbl_tmplt.Columns)
                {
                    bool bNeedToCopy = true;
                    string colname = col.ColumnName.ToLower();
                    if (colname == "id" || colname == "name" || colname == "title" || colname.StartsWith("representid"))
                    {
                        bNeedToCopy = false;
                    }

                    if (bNeedToCopy)
                    {
                        des_row[col.ColumnName] = src_row[col.ColumnName];
                    }
                }

                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp_tmplt);
                adp_tmplt.UpdateCommand = cmdBuilder.GetUpdateCommand();
                int val = adp_tmplt.Update(tbl_tmplt);
                tbl_tmplt.AcceptChanges();
            }
            else
            {
                for (int i = 1; i <= 10; i++)
                {
                    string colname = "RepresentID" + i.ToString();
                    if (src_row[colname].ToString() != string.Empty && src_row[colname].ToString() != "0")
                    {
                        object[] repkeys = new object[1];
                        repkeys[0] = Convert.ToInt32(src_row[colname]);
                        DataRow src_row_rep = tbl_rep.Rows.Find(repkeys);

                        //System.Data.DataTable t = GetDataTable("SELECT MAX(RepresentID) FROM npc", Conn);
                        //int newid = Convert.ToInt32(t.Rows[0][0]) + 1;
                        int newid = GetUnusedRepresentID();

                        DataRow newrow_rep = tbl_rep.NewRow();
                        newrow_rep.BeginEdit();
                        newrow_rep["RepresentID"] = newid;

                        foreach (DataColumn col in tbl_rep.Columns)
                        {
                            if (col.ColumnName.ToLower() != "representid")
                            {
                                newrow_rep[col.ColumnName] = src_row_rep[col.ColumnName];
                            }
                        }
                        newrow_rep.EndEdit();
                        tbl_rep.Rows.Add(newrow_rep);

                        SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp_rep);
                        adp_rep.InsertCommand = cmdBuilder.GetInsertCommand();
                        int val = adp_rep.Update(tbl_rep);
                        tbl_rep.AcceptChanges();

                        // 写模板表
                        des_row[colname] = newid;
                    }
                    else
                    {
                        des_row[colname] = 0;
                    }
                }
                SqlCommandBuilder cmdBuilder2 = new SqlCommandBuilder(adp_tmplt);
                adp_tmplt.UpdateCommand = cmdBuilder2.GetUpdateCommand();
                int val2 = adp_tmplt.Update(tbl_tmplt);
                tbl_tmplt.AcceptChanges();
            }
            return true;
        }

        private int GetUnusedRepresentID()
        {
            for (int i = 0; ; i++)
            {
                System.Data.DataTable tbl = GetDataTable("SELECT RepresentID FROM npc WHERE RepresentID = " + i.ToString(), Conn);
                if (tbl.Rows.Count == 0)
                {
                    return i;
                }
            }
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

        [DllImport("user32.dll")]
        private static extern int GetWindowThreadProcessId(IntPtr hwnd, out int id);
        public void killexcel(Excel.Application xlapp)
        {
            try
            {
                IntPtr app = new IntPtr(xlapp.Hwnd);
                int processid;
                GetWindowThreadProcessId(app, out processid);
                System.Diagnostics.Process.GetProcessById(processid).Kill();
            }
            catch(Exception ex)
            { }
        }

    }
}
