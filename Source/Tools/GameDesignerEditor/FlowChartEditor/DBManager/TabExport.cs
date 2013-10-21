using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data.SqlClient;
using System.Data;
using System.Windows.Forms;
using System.Collections;

namespace DBManager
{
    public class TabExport
    {
        public static System.Data.SqlClient.SqlConnection m_conn = DBManager.DataBaseManager.GetDataBaseManager().Connection_Jx3web;
        DataTable m_ExportTable = new DataTable();
        public Hashtable WriteFileList = new Hashtable();

        public TabExport(List<string> exportTableList, string strOutDir)
        {
            this.WriteFileList = new Hashtable();
            string filePath;
            bool exportSuccess = false;

            foreach(string tableName in exportTableList)
            {
                switch(tableName)
                {
                    case "Other":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\item\\000\\Other.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "item_Custom_Armor":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\item\\000\\Custom_Armor.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "Custom_Trinket":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\item\\000\\Custom_Trinket.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "item_Custom_Weapon":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\item\\000\\Custom_Weapon.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "NpcTemplate":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\NpcTemplate.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "tbl_doodad":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\DoodadTemplate.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                    case "tbl_quests":
                        {
                            filePath = Path.Combine(strOutDir, "settings\\Quests.tab");
                            exportSuccess = ExportTab(tableName, filePath);

                            break;
                        }
                }

                if(!exportSuccess)
                {
                    break;
                }
            }           
        }

        private bool GenerateString(string strTableName, ref string strHeader, ref string strContent)
        {
            //sql connection
            SqlDataAdapter adp = new SqlDataAdapter("SELECT TOP 1 * FROM " + strTableName, m_conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable Table = ds.Tables[0];

            // 导出时按照主键们来排序.
            if (Table.PrimaryKey.Length > 0)
            {
                string ordering = string.Empty;
                for (int i = 0; i < Table.PrimaryKey.Length; i++)
                {
                    ordering += Table.PrimaryKey[i].ColumnName;
                    if (i != Table.PrimaryKey.Length - 1)
                        ordering += ",";
                }
                adp = new SqlDataAdapter("SELECT * FROM " + strTableName + " ORDER BY " + ordering, m_conn);
                ds = new DataSet();
                adp.Fill(ds);
                Table = ds.Tables[0];
            }
            else
            {
                Table =GetDataTable("SELECT * FROM " + strTableName, m_conn);
            }

            // string formatting
            StringBuilder strHeaderBuf = new StringBuilder();
            StringBuilder strContentBuf = new StringBuilder();

            bool bHasTitle = true;
            string sql = string.Format("SELECT * FROM sys_export_table_cfg WHERE tablename='{0}'", strTableName);
            DataTable tbl_cfg = GetDataTable(sql, m_conn);
            if (tbl_cfg.Rows.Count > 0)
            {
                if (tbl_cfg.Rows[0]["hastitle"] != DBNull.Value)
                    bHasTitle = Convert.ToBoolean(tbl_cfg.Rows[0]["hastitle"]);
            }

            if (bHasTitle)
            {
                foreach (System.Data.DataColumn col in Table.Columns)
                {
                    strHeaderBuf.Append(col.ColumnName);
                    strHeaderBuf.Append("\t");
                }
                strHeaderBuf.Remove(strHeaderBuf.Length - 1, 1);
                strHeaderBuf.Append("\r\n");
            }

            foreach (System.Data.DataRow row in Table.Rows)
            {
                foreach (System.Data.DataColumn col in Table.Columns)
                {
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
                StringToFile(strFinal.ToString(), strDestFile);
                return true;
            }
            catch (Exception ex)
            {
                string s = string.Format("导出 tab 文件:\r\n    {0}\r\n时出现错误。请检查文件名，磁盘空间等。\r\n\r\n错误讯息为:{1}", strDestFile, ex.Message);
                System.Windows.Forms.MessageBox.Show(s, "Error");
                return false;
            }
        }

        public void StringToFile(string strContent, string strFileName)
        {
            this.WriteFileList[strFileName] = strContent;
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
    }
}
