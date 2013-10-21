using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace AITypeAutoCreate
{
    class Helper
    {
        private static Helper helper;
        private StringBuilder logText = new StringBuilder();

        private Helper()
        {

        }

        public static Helper GetHelper()
        {
            if (helper == null)
            {
                helper = new Helper();
            }

            return helper;
        }

        public string LogText
        {
            get
            {
                return logText.ToString();
            }
        }

        public void RecordLogText(string log)
        {
            string recordText = string.Format("{0} —— {1}", DateTime.Now, log);
            logText.AppendLine(recordText);
            Console.WriteLine(recordText);
        }

        public string GetFileContent(string fileName)
        {
            string content = null;

            try
            {
                StreamReader streamReader = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                content = streamReader.ReadToEnd();
                streamReader.Close();
            }
            catch (IOException ex)
            {
                RecordLogText(string.Format("对文件{0}进行IO操作时产生异常：" + ex.Message, fileName));   
            }

            return content;
        }

        /// <summary>
        /// 保存文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">数据内容</param>
        public void SaveData(string fileName, string content)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    string path = Path.GetDirectoryName(fileName); // 获取目录
                    if (!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                RecordLogText(string.Format("对文件{0}进行IO操作时产生异常：" + ex.Message, fileName));                
            }
        }
    }
}
