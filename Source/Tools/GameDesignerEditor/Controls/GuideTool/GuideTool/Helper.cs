using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Xml;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Windows.Forms;

namespace GuideTool
{
    class Helper
    {
        private static IFormatter formatter = new BinaryFormatter();

        /// <summary>
        /// 输出异常信息
        /// </summary>
        /// <param name="ex">当前异常</param>
        public static void PrintExceptionInfo(Exception ex)
        {
            MessageBox.Show(string.Format("当前操作产生异常：{0}", ex.Message, "错误",
                            MessageBoxButtons.OK, MessageBoxIcon.Error));
        }

        /// <summary>
        /// 读取文件内容
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <returns>文件内容</returns>
        public static byte[] ReadFile(string fileName)
        {            
            byte[] buffer = null;

            if (File.Exists(fileName))
            {
                try
                {
                    buffer = File.ReadAllBytes(fileName);
                }
                catch (IOException ex)
                {
                    PrintExceptionInfo(ex);
                }
            }

            return buffer;
        }

        /// <summary>
        /// 将文本保存到文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">文件内容</param>
        public static bool SaveFile(string fileName, byte[] content)
        {
            bool success = true;

            try
            {
                FileInfo fi = new FileInfo(fileName);

                if (!fi.Exists) // 文件不存在
                {
                    string folderName = fi.DirectoryName; // 获取目录

                    if (!string.IsNullOrEmpty(folderName) && !Directory.Exists(folderName)) // 目录不存在
                    {
                        Directory.CreateDirectory(folderName); // 先创建目录，再创建文件
                    }

                    fi.Create().Close();
                }

                File.WriteAllBytes(fileName, content);
            }
            catch (IOException ex)
            {                
                success = false;
                PrintExceptionInfo(ex);
            }

            return success;
        }        

        /// <summary>
        /// 反序列化数据
        /// </summary>
        /// <param name="buffer">当前数据</param>
        /// <returns>反序列化后的数据</returns>
        public static object UnseralizeData(byte[] buffer)
        {
            MemoryStream memoryStream = new MemoryStream(buffer);
            object data = formatter.Deserialize(memoryStream);
            memoryStream.Close();

            return data;
        }

        /// <summary>
        /// 序列化数据
        /// </summary>
        /// <param name="data">当前数据</param>
        /// <returns>序列化后的数据</returns>
        public static byte[] SeralizeData(object data)
        {
            MemoryStream memoryStream = new MemoryStream();
            formatter.Serialize(memoryStream, data);
            byte[] buffer = new byte[memoryStream.Length];
            memoryStream.Seek(0, SeekOrigin.Begin);
            memoryStream.Read(buffer, 0, (int)memoryStream.Length);
            memoryStream.Close();

            return buffer;
        }

        /// <summary>
        /// 获取下拉框选项信息
        /// </summary>
        /// <param name="infoString">选项字符串描述</param>
        /// <returns>选项信息</returns>
        public static string[] GetListItemInfo(string infoString)
        {
            int index = infoString.LastIndexOf("(");
            string itemName = infoString.Substring(0, index - 1);
            string itemValue = infoString.Substring(index + 1, infoString.Length - index - 2);

            return new string[] { itemName, itemValue };
        }

        /// <summary>
        /// 当前值是否是空值或者数据库空值
        /// </summary>
        /// <param name="value">当前值</param>
        /// <returns>是否是空值或者数据库空值</returns>
        public static bool IsNullOrDBNull(object value)
        {
            bool isNullOrDBNull = ((value == null) || (value is DBNull));
            return isNullOrDBNull;
        }

        /// <summary>
        /// 获取数字的类型
        /// </summary>
        /// <param name="typeData">类型描述</param>
        /// <returns>数字的类型</returns>
        public static string GetNumberType(string typeData)
        {
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.LoadXml(typeData);
            XmlNode typeNode = xmlDocument["typeData"];
            string numberType = typeNode.Attributes["numberType"].Value;

            return numberType;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <param name="conn">sql连接</param>
        /// <returns>数据表</returns>
        public static DataTable GetDataTable(string sqlString, SqlConnection conn)
        {
            DataTable table = null;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            SqlDataAdapter adapter = new SqlDataAdapter(sqlString, conn);
            adapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet);
            table = dataSet.Tables[0];

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            return table;
        }
    }
}
