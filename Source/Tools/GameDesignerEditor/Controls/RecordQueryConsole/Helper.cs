using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.IO;

namespace RecordQueryConsole
{
    class Helper
    {
        public static bool DataTableExport(DataTable dt, string FileFullPath)
        {
            if (!System.IO.Directory.Exists(Path.GetDirectoryName(FileFullPath)))
            {
                CreatePath(FileFullPath);
            }
            
            bool bResult = false;
            string fileExten = System.IO.Path.GetExtension(FileFullPath);
            if (fileExten == ".xml")
            {
                bResult = DataTableExportAsXml(dt, FileFullPath);
            }
            else if (fileExten == ".xlsx")
            {
                bResult = DataTableExportAsExcel(dt, FileFullPath);
            }
            else
            {
                bResult = false;
            }
            return bResult;
        }
        public static bool DataTableExportAsExcel(DataTable dt, string FileFullPath)
        {
            if (!System.IO.Directory.Exists(Path.GetDirectoryName(FileFullPath)))
            {
                CreatePath(FileFullPath);
            }
            
            Microsoft.Office.Interop.Excel.Application excelApplication = new Microsoft.Office.Interop.Excel.Application();
            excelApplication.EnableEvents = false;
            excelApplication.Application.DisplayAlerts = false;
            excelApplication.Workbooks.Add(true);
            Microsoft.Office.Interop.Excel.Worksheet myWorkSheet = (Microsoft.Office.Interop.Excel.Worksheet)excelApplication.ActiveSheet;
            excelApplication.Visible = false;
            int nRowIndex    = 0;
            int nColumnIndex = 0;
            int ColumnCount = dt.Columns.Count;
            int RowCount = dt.Rows.Count;
            object[,] strArr = new object[RowCount + 1, ColumnCount];

            foreach (DataColumn col in dt.Columns)
            {
                strArr[nRowIndex, nColumnIndex] = col.ColumnName;
                ++nColumnIndex;
            }
            ++nRowIndex;
            nColumnIndex = 0;
         

            foreach (DataRow row in dt.Rows)
            {
                for (int i = 0; i < ColumnCount; i++)
                {
                    strArr[nRowIndex, nColumnIndex] = row[i].ToString();
                    ++nColumnIndex;
                }
                ++nRowIndex;
                nColumnIndex = 0;
            }
            string strExcelMaxColumnIndex = GetExcelMaxColumnIndex(ColumnCount, RowCount + 1);
            Microsoft.Office.Interop.Excel.Range myRange = (Microsoft.Office.Interop.Excel.Range)myWorkSheet.get_Range("A1", strExcelMaxColumnIndex);
            myRange.get_Resize(RowCount + 1, ColumnCount);
            try
            {
                myRange.Value2 = strArr;
                myRange.Columns.AutoFit();
                myRange.Cells.HorizontalAlignment = Microsoft.Office.Interop.Excel.XlHAlign.xlHAlignCenter;

                myWorkSheet.SaveAs(FileFullPath, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);

            }
            catch (Exception ex)
            { 
                return false;
            }
           
            excelApplication.Quit();
            killexcel(excelApplication);
            GC.Collect();
            return true;
        }

        [DllImport("user32.dll")]
        private static extern int GetWindowThreadProcessId(IntPtr hwnd, out int id);
        public static void killexcel(Microsoft.Office.Interop.Excel.Application xlapp)
        {
            try
            {
                IntPtr app = new IntPtr(xlapp.Hwnd);
                int processid;
                GetWindowThreadProcessId(app, out processid);
                System.Diagnostics.Process.GetProcessById(processid).Kill();
            }
            catch
            { }
        }
        private static string GetExcelMaxColumnIndex(int nColumnCnt, int nRowCnt)
        {
            string strResult = string.Empty;
            char cOne = '\0';
            int ncolcnt = nColumnCnt;
            while (ncolcnt > 0)
            {
                cOne = (char)('A' + ncolcnt % 26 - 1);
                strResult = string.Format("{0}{1}", cOne, strResult);
                ncolcnt /= 26;
            }
            strResult = string.Format("{0}{1}", strResult, nRowCnt);
            return strResult;
        }

        public static bool DataTableExportAsXml(DataTable dt, string FileFullPath)
        {
            if (!System.IO.Directory.Exists(Path.GetDirectoryName(FileFullPath)))
            {
                CreatePath(FileFullPath);
            }           
            
            System.Xml.XmlTextWriter writer = new System.Xml.XmlTextWriter(FileFullPath, Encoding.UTF8);
            writer.Formatting = System.Xml.Formatting.Indented;            
            writer.WriteStartDocument();
            string str = string.Format("type=\"text/xsl\" href=\"{0}.xslt\"", Path.GetFileNameWithoutExtension(FileFullPath) );
            writer.WriteProcessingInstruction("xml-stylesheet", str);
            

            dt.DataSet.WriteXml(writer);
            writer.Close();

            return true;
        }

        public static DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
        public static bool IsLegalDataTimeString(string strChecked)
        {
            DateTime dtCheck;
            return DateTime.TryParse(strChecked, out dtCheck);
        }

        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
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

        /// <summary>
        /// 检查文件目录是否存在，不存在则创建，返回文件的绝对路径
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns></returns>
        public static string CreatePath(string fileName)
        {
            string FullPathRet = Path.GetFullPath(fileName);
            string path = Path.GetDirectoryName(FullPathRet); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
                return FullPathRet;
            }
            catch (IOException ex)
            {
                return "";
            }
        }

    }
}
