using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace LogManager
{        
    [Flags]
    public enum LogLevel { Trace, Warning, Error }; // 日志等级

    public class LogManager
    {
        private static LogManager logManager = null;  // 日志管理器对象
        private List<LogInfo> logInfoList = new List<LogInfo>(); // 日志信息链表
        private string logEncode = "gb2312"; // 日志编码
        private string logFolder = @"log\FlowChartEditor"; // 日志文件夹

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private LogManager()
        {
            
        }

        /// <summary>
        /// 日志编码
        /// </summary>
        public string LogEncode
        {
            set
            {
                logEncode = value;
            }

            get
            {
                return logEncode;
            }
        }

        /// <summary>
        /// 日志文件夹
        /// </summary>
        public string LogFolder
        {
            set
            {
                logFolder = value;
            }

            get
            {
                return logFolder;
            }
        }

        /// <summary>
        /// 获取日志管理器对象实例
        /// </summary>
        /// <returns>日志管理器对象</returns>
        public static LogManager GetLogManager()
        {
            if (logManager == null)
            {
                logManager = new LogManager();
            }

            return logManager;
        }


        /// <summary>
        /// 记录日志
        /// </summary>
        /// <param name="logType">日志类型</param>
        /// <param name="message">日志消息</param>
        /// <param name="logLevel">日志等级</param>
        public void RecordLog(string logType, string message, LogLevel logLevel)
        {
            LogInfo logInfo = new LogInfo(logType, message, logLevel);
            logInfoList.Add(logInfo);
        }        

        /// <summary>
        /// 转换日期
        /// </summary>
        /// <param name="dateTime">当前日期</param>
        /// <returns>日期字符串</returns>
        public static string ConvertDateTime(DateTime dateTime)
        {
            return dateTime.ToString().Replace(" ", "-").Replace(":", "-");
        }

        /// <summary>
        /// 读取文件内容
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <returns>文件内容</returns>
        public string ReadFile(string fileName)
        {
            string content = null;

            if (File.Exists(fileName))
            {
                try
                {
                    StreamReader streamReader = new StreamReader(fileName, Encoding.GetEncoding(logEncode));
                    content = streamReader.ReadToEnd();
                    streamReader.Close();
                }
                catch (IOException ex)
                {
                    ;
                }                
            }

            return content;
        }

        /// <summary>
        /// 将文本保存到文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">文本内容</param>
        public bool SaveFile(string fileName, string content)
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

                StreamWriter streamWriter = new StreamWriter(fileName, false, Encoding.GetEncoding(logEncode));
                streamWriter.Write(content);
                streamWriter.Close();
            }
            catch (IOException ex)
            {                
                success = false;
            }

            return success;
        }

        /// <summary>
        /// 记录日志
        /// </summary>
        public void WriteLog()
        {
            LogLevel logLevel = LogLevel.Trace | LogLevel.Warning | LogLevel.Error;
            WriteLog("", logLevel, DateTime.MinValue, DateTime.MaxValue);
        }

        /// <summary>
        /// 写日志
        /// </summary>
        /// <param name="logType">日志分类</param>
        /// <param name="logLevel">日志等级</param>
        /// <param name="minTime">最小时间</param>
        /// <param name="maxTime">最大时间</param>
        public void WriteLog(string logType, LogLevel logLevel, DateTime minTime, DateTime maxTime)
        {
            StringBuilder logText = new StringBuilder();

            foreach (LogInfo logInfo in logInfoList)
            {
                if (((logInfo.RecordLogLevel & logLevel) == logInfo.RecordLogLevel)
                    && (logInfo.RecordTime >= minTime && logInfo.RecordTime <= maxTime)
                    && (logInfo.LogType == logType || string.IsNullOrEmpty(logType)))
                {
                    logText.AppendLine(logInfo.ToString());
                }
            }

            string fileName = Path.Combine(logFolder, string.Format("{0}.txt", ConvertDateTime(DateTime.Now)));
            SaveFile(fileName, logText.ToString());
        }

        /// <summary>
        /// 日志信息
        /// </summary>
        class LogInfo
        {
            private readonly string logType;
            private readonly string message;
            private readonly LogLevel logLevel;
            private DateTime recordTime;

            public LogInfo(string logType, string message, LogLevel logLevel)
            {
                this.logType = logType;
                this.message = message;
                this.logLevel = logLevel;
                recordTime = DateTime.Now;
            }

            public string LogType
            {
                get
                {
                    return logType;
                }
            }

            public string Message
            {
                get
                {
                    return message;
                }
            }

            public LogLevel RecordLogLevel
            {
                get
                {
                    return logLevel;
                }
            }

            public DateTime RecordTime
            {
                get
                {
                    return recordTime;
                }
            }

            public override string ToString()
            {
                string levelDescription = "Record";

                switch (logLevel)
                {
                    case LogLevel.Trace:
                        {
                            levelDescription = "Trace";
                            break;
                        }
                    case LogLevel.Warning:
                        {
                            levelDescription = "Warning";
                            break;
                        }
                    case LogLevel.Error:
                        {
                            levelDescription = "Error";
                            break;
                        }
                }

                string infoString = string.Format("{0} [{1}]: <{2}> {3}", LogManager.ConvertDateTime(DateTime.Now),
                                                  levelDescription, logType, message);

                return infoString;
            }
        }
    }
}
