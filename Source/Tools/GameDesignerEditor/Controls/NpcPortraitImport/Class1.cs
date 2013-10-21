using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;

namespace NpcPortraitImport
{
    public class Class1
    {
        private string m_strFileFullName = string.Empty;
        private string m_strTableName = string.Empty;
        private SqlConnection Conn = null;

        public Class1(string tablename, string filename, SqlConnection conn)
        {
            m_strTableName = tablename;
            m_strFileFullName = filename;
            Conn = conn;
        }

        public void ImportFile()
        {
            // get db table
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + m_strTableName, Conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];

            // clear
            tbl.BeginInit();
            tbl.Rows.Clear();

            // 获得另外一张表npc, 读取name 和 map
            SqlDataAdapter adp2 = new SqlDataAdapter("SELECT * FROM npc", Conn);
            adp2.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds2 = new DataSet();
            adp2.Fill(ds2);
            System.Data.DataTable tbl_npc = ds2.Tables[0];

            // import
            string fileContent = FileToString(m_strFileFullName);
            fileContent = fileContent.Trim();
            string[] arrContents = fileContent.Split(new string[] { "NpcPortraitCameraInfo", "[", "]", "{", "}", "=", ",", "\r\n", "\t", " " },
                StringSplitOptions.RemoveEmptyEntries);
            int arr_index = 0;
            for (int i = 0; i < arrContents.Length / 9; i++)
            {
                DataRow newRow = tbl.NewRow();
                newRow.BeginEdit();
                newRow["RepresentID"] = arrContents[arr_index++];
                newRow["PositionX"] = arrContents[arr_index++];
                newRow["PositionY"] = arrContents[arr_index++];
                newRow["PositionZ"] = arrContents[arr_index++];
                newRow["LookatX"] = arrContents[arr_index++];
                newRow["LookatY"] = arrContents[arr_index++];
                newRow["LookatZ"] = arrContents[arr_index++];
                newRow["Width"] = arrContents[arr_index++];
                newRow["Height"] = arrContents[arr_index++];
                DataRow r = tbl_npc.Rows.Find(newRow["RepresentID"]);
                newRow["Map"] = r["map"].ToString().Trim();
                newRow["Name"] = r["Model"].ToString().Trim();
                tbl.Rows.Add(newRow);
                newRow.EndEdit();
            }

            // upload
            tbl.EndInit();
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
        }

        private string FileToString(string strFileName)
        {
            return FileToString(strFileName, Encoding.GetEncoding("gb2312"));
        }
        private string FileToString(string strFileName, Encoding encoding)
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

            //StreamReader reader = new StreamReader(strFileName, encoding);
            string strContent = reader.ReadToEnd();
            reader.Close();
            return strContent;
        }
    }
}
