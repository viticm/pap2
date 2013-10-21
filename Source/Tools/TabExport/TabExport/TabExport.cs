using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.IO;

namespace TabExport
{
	class TabExporter
	{
		public static void Export(SqlConnection conn, string tableName, string outFile, Encoding enc)
		{	
			string select = "SELECT * FROM " + tableName;
			SqlDataAdapter da = new SqlDataAdapter(select, conn);
			DataSet ds = new DataSet();
			da.Fill(ds);

            // 导出时按照主键们来排序.
            DataTable Table = ds.Tables[0];
            if (Table.PrimaryKey.Length > 0)
            {
                string ordering = string.Empty;
                for (int i = 0; i < Table.PrimaryKey.Length; i++)
                {
                    ordering += Table.PrimaryKey[i].ColumnName;
                    if (i != Table.PrimaryKey.Length - 1)
                        ordering += ",";
                }
                da = new SqlDataAdapter("SELECT * FROM " + tableName + " ORDER BY " + ordering, conn);
                ds = new DataSet();
                da.Fill(ds);
            }

			using(FileStream fs = new FileStream(outFile, FileMode.Create, FileAccess.Write))
			using(StreamWriter sw = new StreamWriter(fs, enc))
			{
				StringBuilder sb = new StringBuilder();
				//export title line

				foreach(DataColumn col in ds.Tables[0].Columns) {
					sb.Append(col.ToString());
					sb.Append("\t");
				}
				if (sb.Length > 0) {
					sb.Remove(sb.Length - 1, 1);
					sw.WriteLine(sb);
				}

				//export each line
				foreach(DataRow row in ds.Tables[0].Rows) {
					sb.Length = 0;
					foreach(DataColumn col in ds.Tables[0].Columns) {
						object data = row[col];
						if (col.DataType == typeof(Boolean))
						{
							if (data.GetType() == typeof(System.DBNull))
							{
								sb.Append("");
							}
							else
							{
								sb.Append((bool)data ? 1 : 0);
							}
						}
						else
						{
							string s;
							if (data.GetType() == typeof(System.DBNull))
								s = "";
							else
								s = data.ToString();
							s = s.Replace("\n", "");
							s = s.Replace("\r", "");
							sb.Append(s);
						}
						sb.Append("\t");
					}
					if (sb.Length > 0) {
						sb.Remove(sb.Length - 1, 1);
						sw.WriteLine(sb);
					}
				}
			}
			
		}

	}
}
