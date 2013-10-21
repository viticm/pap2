using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;


namespace MapLogicalFileReader
{
    public class MapLogicalTableReader
    {
        private string TableName;//例如：npc,doodad
        private int RowCount;//个数
        private DataTable TheTable;
        string[] ColumnNames;
        private string MapLogicalFilePath;



        public MapLogicalTableReader( string filePath, string tableName, string[] readingColumns)
        {
            MapLogicalFilePath = filePath;
            TableName = tableName;
            ColumnNames = readingColumns;
        }
        public bool Init()
        {
            if (!Check())
            {
                return false;
            }
            CreateTable();
            ReadFileFillTable();

            return true;

        }
        private void CreateTable()
        {
            TheTable = new DataTable();
           
            foreach (string columnName in ColumnNames)
            {
                System.Type myDataType = System.Type.GetType("System.String");
                DataColumn newCol = new DataColumn(columnName, myDataType);
                TheTable.Columns.Add(newCol);
            }

        }
        private bool Check()
        { 

            if (!Helper.IsFileExists(MapLogicalFilePath))
            {
                return false;
            }
            if (TableName.Trim() == "")
            {
                return false;
            }
            if (ColumnNames == null)
            {
                return false;
            }
            foreach (string column in ColumnNames)
            {
                if (column.Trim() == "")
                {
                    return false;                 
                }
            }

            string content = Helper.FileToString(MapLogicalFilePath, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(MapLogicalFilePath, content);
            if (Helper.m_inis == null)
            {
                string strErr = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", MapLogicalFilePath);

                return false;
            }
            Helper.CloseIniFile();

            return true;
        }
        private bool ReadFileFillTable()
        {
            // 读文件
            string content = Helper.FileToString(MapLogicalFilePath, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(MapLogicalFilePath, content);           
            RowCount = Convert.ToInt32(Helper.GetIniString("MAIN", string.Format("Num{0}", TableName)));

            string section = string.Empty;
            string cellValue = string.Empty;
           
            for (int i = 0; i < RowCount; ++i)
            {
                DataRow newrow = TheTable.NewRow();
                section = string.Format("{0}{1}", TableName, i);
                foreach (string columnName in ColumnNames)
                {
                    cellValue = Helper.GetIniString(section, columnName);
                    newrow[columnName] = cellValue;
                }

                TheTable.Rows.Add(newrow);
            } 
            
            Helper.CloseIniFile();

            return true;

        }

        public DataRow[] Select(string filterColumn, string filterValue)
        {
            return TheTable.Select(string.Format("{0} = '{1}'", filterColumn, filterValue));
        }

    }
}
