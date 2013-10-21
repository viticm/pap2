using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Drawing;

namespace sprViewer
{
    public class Helper
    {
        private static Helper helper;
        private SqlConnection conn;
        private DataTable iconCacheTable;
        private MemoryStream memortStream;
        private StringBuilder logText = new StringBuilder();

        private Helper()
        {

        }

        public string LogText
        {
            get
            {
                return logText.ToString();
            }
        }

        public SqlConnection CurrentSqlConnection
        {
            set
            {
                conn = value;
            }

            get
            {
                return conn;
            }
        }

        public static Helper GetHelper()
        {
            if (helper == null)
            {
                helper = new Helper();
            }

            return helper;
        }

        public void InitLogText()
        {
            logText = new StringBuilder();
        }

        public void InitCacheTable()
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT FileName, FileType, LastWriteTime FROM ItemIconCache";
                SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                iconCacheTable = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                logText.AppendLine(string.Format("初始化图片Cache数据表错误：{0}", ex.Message));
            }
            finally
            {
            	if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }            
        }

        public void UpdateIcon(string fileName, string fileType, string imagePath, DateTime lastWriteTime)
        {
            if (File.Exists(imagePath))
            {                
                try
                {
                    FileStream fileStream = new FileStream(imagePath, FileMode.Open);
                    byte[] buffer = new byte[fileStream.Length];
                    fileStream.Read(buffer, 0, buffer.Length);
                    fileStream.Close();

                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    SqlCommand cmd = conn.CreateCommand();

                    string sqlString = string.Format("SELECT LastWriteTime FROM ItemIconCache WHERE FileName = '{0}' AND FileType = '{1}'",
                                                                 fileName, fileType);
                    cmd.CommandText = sqlString;
                    object executeResult = cmd.ExecuteScalar();

                    if (executeResult == null)
                    {
                        sqlString = string.Format("INSERT INTO ItemIconCache(FileName, FileType, LastWriteTime, Data) VALUES ('{0}', '{1}', '{2}', @data)",
                                                  fileName, fileType, lastWriteTime.ToString());
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE ItemIconCache SET LastWriteTime = '{0}', Data = @data WHERE FileName = '{1}' AND FileType = '{2}'",
                                                  lastWriteTime.ToString(), fileName, fileType);
                    }

                    cmd.CommandText = sqlString;
                    cmd.Parameters.Add("@data", SqlDbType.Image).Value = buffer;
                    cmd.ExecuteNonQuery();
                }
                catch(Exception ex)
                {
                    logText.AppendLine(string.Format("数据库更新图片文件{0}错误：{1}", fileName, ex.Message));
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                } 
            }                       
        }

        public int RequireUpdateCache(string fileName, string fileType, DateTime currentDateTime)
        {
            int equalCacheTime = 1;

            DataRow[] rows = iconCacheTable.Select(string.Format("FileName = '{0}' AND FileType = '{1}'",
                                                   fileName, fileType));
            if (rows.Length > 0)
            {
                DateTime lastWriteTime = ((DateTime)rows[0]["LastWriteTime"]);
 
                if (lastWriteTime.ToString() == currentDateTime.ToString())
                {
                    equalCacheTime = 0;                   
                }
                else
                {
                    lastWriteTime = lastWriteTime.AddSeconds(1); // 故意增加1秒，让DateTime比较条件成立

                    if (lastWriteTime > currentDateTime)
                    {
                        equalCacheTime = -1;
                    }
                }
            }

            return equalCacheTime;
        }

        public void RecordLog(string recordText)
        {
            logText.AppendLine(recordText);
        }

        public Image GetImageFromCache(string fileName, string fileType)
        {
            Image image = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Data FROM ItemIconCache WHERE FileName = '{0}' AND FileType = '{1}'", 
                                                 fileName, fileType);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    byte[] imageData = (byte[])executeResult;
                    memortStream = new MemoryStream();
                    memortStream.Write(imageData, 0, imageData.Length);
                    image = Image.FromStream(memortStream);
                }
            }
            catch(Exception ex)
            {
                logText.AppendLine(string.Format("数据库读取图片文件{0}错误：{1}", fileName, ex.Message));
            }
            finally
            {
            	if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return image;
        }

        public void CloseImageMemoryStream()
        {
            if (memortStream != null)
            {
                memortStream.Close();
            }
        }
    }
}
