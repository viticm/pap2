using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Reflection;
using System.Collections;
using System.Diagnostics;
using System.Data;

using LuaInterface;
using DevComponents.DotNetBar.Controls;

namespace FlowChartEditor
{
    public class Helper
    {
        private static Helper helper; // 唯一的私有实例             

        private Graphics graphicsObject; // 绘图对象
        private TextBoxX logBox; // 日志输出文本框

        private string outputDir = ""; // 流程图的输出路径
        private List<int> latestDiagramList = new List<int>(); // 最近打开的绘图
        private string encodeInfo = "gb2312"; // 编码

        private string defaultChart = "AI图"; // 默认绘图
        private const string configFileName = "config.lua"; // 配置文件名称
        private List<string> diagramNameList = new List<string>(); // 绘图名称链表        
        private const int maxDiagramCount = 10; // 最多绘图类型数量
        private Dictionary<string, LogicBaseManager> logicManagerDictionary = new Dictionary<string, LogicBaseManager>(); // 逻辑管理器容器 
        private Dictionary<string, string> logicManagerDisplayNameDictionary = new Dictionary<string, string>();
        private List<string> updateFileList = new List<string>(); // 需要更新的文件链表

        private LogManager.LogManager logManager = LogManager.LogManager.GetLogManager(); // 日志管理器

        /// <summary>
        /// 构造函数
        /// </summary>
        private Helper()
        {

        }

        /// <summary>
        /// 获取辅助类的实例
        /// </summary>
        public static Helper GetHelper()
        {
            if(helper == null)
            {
                helper = new Helper();
            }

            return helper;
        }

        /// <summary>
        /// 设置流程图参数
        /// </summary>
        /// <param name="args">参数哈希表</param>
        public void SetArguments(Hashtable args)
        {           
            outputDir = args["OutPutDir"] as string;
            latestDiagramList = args["LatestDiagramList"] as List<int>;

            if (latestDiagramList == null)
            {
                latestDiagramList = new List<int>();
            }
        }

        /// <summary>
        /// 获取流程图参数
        /// </summary>
        /// <returns>参数哈希表</returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();
            args["OutPutDir"] = outputDir;            
            args["LatestDiagramList"] = latestDiagramList;

            return args;
        }

        /// <summary>
        /// 默认绘图分类
        /// </summary>
        public string DefaultChart
        {
            get
            {
                return defaultChart;              
            }
        }

        /// <summary>
        /// 最近打开的绘图链表s
        /// </summary>
        public List<int> LatestDiagramList
        {
            get
            {
                return latestDiagramList;
            }
        }

        /// <summary>
        /// 加载逻辑管理器
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>逻辑管理器</returns>
        private LogicBaseManager LoadLogicManager(string filePath, string className)
        {
            LogicBaseManager logicBaseManager = null;

            try
            {
                string fullPath = Path.Combine(Application.StartupPath, @"Plugins\FlowChart");
                fullPath = Path.Combine(fullPath, filePath);
                Assembly assembly = Assembly.LoadFrom(fullPath);
                logicBaseManager = assembly.CreateInstance(className) as LogicBaseManager;
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("加载逻辑文件 {0} 时产生异常：\r\n{1}", filePath, ex.Message), "初始化逻辑",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return logicBaseManager;
        }



        /// <summary>
        /// 初始化逻辑管理器
        /// </summary>
        public void InitLogicManager()
        {
            string filePath = Path.Combine(Application.StartupPath, configFileName);
            
            if (File.Exists(filePath))
            {
                Lua lua = new Lua();

                lua.DoFile(filePath);
                LuaTable configTable = lua.GetTable("Config") as LuaTable;

                string[] diagramNameArray = new string[maxDiagramCount];

                foreach (object key in configTable.Keys)
                {
                    LuaTable logicManagerTable = configTable[key] as LuaTable;

                    if (logicManagerTable != null)
                    {
                        // 读取逻辑管理器信息
                        string logicManagerFilePath = logicManagerTable["LogicManagerFilePath"] as string;
                        string logicManagerClassName = logicManagerTable["LogicManagerClassName"] as string;
                        LogicBaseManager logicManager = null;

                        if (!string.IsNullOrEmpty(logicManagerFilePath) && !string.IsNullOrEmpty(logicManagerClassName))
                        {
                            logicManager = LoadLogicManager(logicManagerFilePath, logicManagerClassName);
                        }
                        else
                        {
                            logicManager = new LogicBaseManager();
                        }

                        // 读取显示名称
                        string logicManagerDisplayName = logicManagerTable["DisplayName"] as string;

                        if (!string.IsNullOrEmpty(logicManagerDisplayName))
                        {
                            logicManager.DisplayName = logicManagerDisplayName;
                        }                        

                        // 读取脚本生成插件信息
                        string compilerFilePath = logicManagerTable["CompilerFilePath"] as string;
                        string compilerClassName = logicManagerTable["CompilerClassName"] as string;

                        if (!string.IsNullOrEmpty(compilerFilePath) && !string.IsNullOrEmpty(compilerClassName))
                        {
                            logicManager.InitCompiler(compilerFilePath, compilerClassName);
                        }

                        // 检查是否是默认分类
                        int index = (int)(double)logicManagerTable["Index"];
                        diagramNameArray[index - 1] = logicManagerDisplayName;                        

                        // 读取数据元信息
                        LuaTable dataElementGroupTable = logicManagerTable["DataElement"] as LuaTable;

                        if (dataElementGroupTable != null)
                        {
                            foreach (object graphType in dataElementGroupTable.Keys)
                            {
                                LuaTable dataElementConfigTable = dataElementGroupTable[graphType] as LuaTable;

                                if (dataElementConfigTable != null)
                                {
                                    string dataElementFilePath = dataElementConfigTable["FilePath"] as string;
                                    string dataElementClassName = dataElementConfigTable["ClassName"] as string;

                                    logicManager.RegistDataElementInfo(graphType as string, dataElementFilePath, dataElementClassName);
                                }                                
                            }
                        }

                        logicManagerDictionary[logicManager.DisplayName] = logicManager;
                    }                    
                }

                foreach (string displayName in diagramNameArray)
                {
                    if (displayName != null)
                    {
                        diagramNameList.Add(displayName);
                    }
                }                
            }            
        }

        /// <summary>
        /// 日志输出文本框
        /// </summary>
        public TextBoxX LogBox
        {
            set
            {
                logBox = value;
            }
        }

        /// <summary>
        /// 流程图的输入路径属性
        /// </summary>
        public string OutputDir
        {
            set
            {
                outputDir = value;
            }

            get
            {
                return outputDir;
            }
        }

        /// <summary>
        /// 绘图名称链表
        /// </summary>
        public List<string> DiagramNameList
        {
            get
            {
                return diagramNameList;
            }
        }

        /// <summary>
        /// 绘图对象
        /// </summary>
        public Graphics GraphicsObject
        {
            get
            {
                if(graphicsObject == null)
                {
                    Bitmap bmp = new Bitmap(1, 1);
                    graphicsObject = Graphics.FromImage(bmp);
                }

                return graphicsObject;
            }
        }
        
        /// <summary>
        /// 读取图片文件
        /// </summary>
        /// <param name="resourceName">资源名称</param>
        /// <returns>图片</returns>
        public Image LoadImageFromResource(string resourceName)
        {
            Image image = null;
            Assembly assembly = Assembly.GetExecutingAssembly();
            byte[] resourceBuffer = FlowChartEditor.Properties.Resources.ResourceManager.GetObject(resourceName) as byte[];

            if (resourceBuffer != null)
            {
                Stream imageStream = new MemoryStream(resourceBuffer);
                image = Image.FromStream(imageStream);
            }                     

            return image;
        }

        /// <summary>
        /// 获取当前用户名
        /// </summary>
        /// <returns>当前用户名</returns>
        public string GetHostName()
        {
            string hostName = Dns.GetHostName();

            return hostName;
        }

        /// <summary>
        /// 获取当前用户网络地址
        /// </summary>
        /// <returns>当前用户网络地址</returns>
        public string GetHostAddress()
        {            
            string ip = Dns.GetHostEntry(Dns.GetHostName()).AddressList[0].ToString();;

            return ip;
        }

        /// <summary>
        /// 获取当前用户所有网络地址
        /// </summary>
        /// <returns>当前用户所有网络地址</returns>
        public List<string> GetHostAddresses()
        {
            List<string> addressList = new List<string>();

            foreach (IPAddress ipAddress in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
            {
                addressList.Add(ipAddress.ToString());
            }

            return addressList;
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
                    StreamReader streamReader = new StreamReader(fileName, Encoding.GetEncoding(encodeInfo));
                    content = streamReader.ReadToEnd();
                    streamReader.Close();
                }
                catch (IOException ex)
                {
                    logManager.RecordLog("写文件", ex.Message, LogManager.LogLevel.Warning);
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

                StreamWriter streamWriter = new StreamWriter(fileName, false, Encoding.GetEncoding(encodeInfo));
                streamWriter.Write(content);
                streamWriter.Close();
            }
            catch (IOException ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.Message, fileName));
                success = false;
            }

            return success;
        }        

        /// <summary>
        /// 输出调试文本
        /// </summary>
        /// <param name="text">调试文本</param>
        public void PrintText(string text)
        {
            logBox.AppendText(string.Format("{0}\r\n", text));
        }

        /// <summary>
        /// 记录最近打开的绘图ID
        /// </summary>
        /// <param name="id">最近打开的绘图ID</param>
        public void RecordLatestDiagram(int id)
        {
            if (latestDiagramList.Count < 5)
            {
                if (!latestDiagramList.Contains(id))
                {
                    latestDiagramList.Add(id);
                }
            }
            else
            {
                if (!latestDiagramList.Contains(id))
                {
                    latestDiagramList.RemoveAt(0);
                    latestDiagramList.Insert(0, id);
                }                                
            }
        }

        /// <summary>
        /// 获取逻辑管理器实例
        /// </summary>
        /// <param name="chartMode">当前编辑模式</param>
        /// <returns>逻辑管理器实例</returns>
        public LogicBaseManager GetLogicManager(string chartMode)
        {
            LogicBaseManager logicManager = logicManagerDictionary[chartMode];            

            return logicManager;
        }

        /// <summary>
        /// 转换数据表数值
        /// </summary>
        /// <param name="value">旧数值</param>
        /// <returns>新数值</returns>
        private string ConvertValue(string value)
        {
            string newValue = value;

            switch (value)
            {
                case "True":
                    {
                        newValue = "1";
                        break;
                    }
                case "False":
                    {
                        newValue = "0";
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            return newValue;
        }

        /// <summary>
        /// 读取数据表
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <returns>数据表</returns>
        public DataTable ReadDataTable(string fileName)
        {
            DataTable table = null;
            string content = ReadFile(fileName);

            if (content != null)
            {
                table = new DataTable();
                List<string> fieldList = new List<string>();

                bool firstLine = true;
                string[] lines = content.Split(new char[] { '\n' });

                foreach (string s in lines)
                {
                    string line = s.TrimEnd(new char[] { '\r' });

                    if (line != "") // 跳过空的数据行
                    {
                        string[] data = line.Split(new char[] { '\t' });

                        if (firstLine)
                        {
                            firstLine = false;

                            for (int i = 0; i < data.Length; i++)
                            {
                                DataColumn dataColumn = new DataColumn(data[i]);
                                table.Columns.Add(dataColumn);
                                fieldList.Add(data[i]);
                            }
                        }
                        else
                        {
                            DataRow dataRow = table.NewRow();

                            for (int i = 0; i < data.Length; i++)
                            {
                                dataRow[i] = data[i];
                            }

                            table.Rows.Add(dataRow);
                        }
                    }
                }
            }            

            return table;
        }

        /// <summary>
        /// 保存数据表内容
        /// </summary>
        /// <param name="infoTable">数据表</param>
        /// <param name="fileName">文件名</param>
        public void SaveDataTable(DataTable infoTable, string fileName)
        {
            StringBuilder content = new StringBuilder();
            StringBuilder headLine = new StringBuilder();

            foreach (DataColumn dataColumn in infoTable.Columns)
            {
                headLine.Append(string.Format("{0}\t", dataColumn.ColumnName));
            }

            headLine.Remove(headLine.Length - 1, 1);
            content.AppendLine(headLine.ToString());

            foreach (DataRow dataRow in infoTable.Rows)
            {
                StringBuilder line = new StringBuilder();

                foreach (DataColumn dataColumn in infoTable.Columns)
                {
                    line.Append(string.Format("{0}\t", ConvertValue(dataRow[dataColumn].ToString())));
                }

                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }

            SaveFile(fileName, content.ToString());
        }

        /// <summary>
        /// 检查是否需要更新
        /// </summary>
        /// <returns>是否需要更新</returns>
        public bool CheckRequireUpdate()
        {
            return false;

            bool requireUpdate = false;
            string ftpFolder = "ftp://changan/newtools/";
            string toolFolder = Application.StartupPath;

            List<string> fileList = new List<string>();
            fileList.Add("FlowChartEditor.exe");
            fileList.Add(@"Plugins/FlowChart/ActionNode.dll");
            fileList.Add(@"Plugins/FlowChart/AIChartLogicManager.dll");
            fileList.Add(@"Plugins/FlowChart/AIDiagramPlugin.dll");
            fileList.Add(@"Plugins/FlowChart/BaseExp.dll");
            fileList.Add(@"Plugins/FlowChart/CodeCompiler.dll");
            fileList.Add(@"Plugins/FlowChart/CodeProvider.dll");
            fileList.Add(@"Plugins/FlowChart/Compiler_Sword3.dll");
            fileList.Add(@"Plugins/FlowChart/Compiler_Sword3_AI.dll");
            fileList.Add(@"Plugins/FlowChart/ConditionLine.dll");
            fileList.Add(@"Plugins/FlowChart/ConditionNode.dll");
            fileList.Add(@"Plugins/FlowChart/ConstSelector.dll");
            fileList.Add(@"Plugins/FlowChart/CoordinateEditor.dll");
            fileList.Add(@"Plugins/FlowChart/DataElementLibrary.dll");
            fileList.Add(@"Plugins/FlowChart/DataInteractor.dll");
            fileList.Add(@"Plugins/FlowChart/DBManager.dll");
            fileList.Add(@"Plugins/FlowChart/EventNode.dll");
            fileList.Add(@"Plugins/FlowChart/FlowChartDataLibrary.dll");
            fileList.Add(@"Plugins/FlowChart/FlowChartLogicManager.dll");
            fileList.Add(@"Plugins/FlowChart/FlowChartVariable.dll");
            fileList.Add(@"Plugins/FlowChart/GraphElementLibrary.dll");
            fileList.Add(@"Plugins/FlowChart/PatrolIO.dll");
            fileList.Add(@"Plugins/FlowChart/QuestSelector.dll");
            fileList.Add(@"Plugins/FlowChart/TextEditor.dll");
            fileList.Add(@"Plugins/FlowChart/VarManager.dll");            

            try
            {
                foreach (string fileName in fileList)
                {
                    string ftpFileName = string.Format("{0}{1}", ftpFolder, fileName);
                    string toolFileName = Path.Combine(toolFolder, fileName);

                    FtpWebRequest ftp = FtpWebRequest.Create(new Uri(ftpFileName)) as FtpWebRequest;
                    ftp.UseBinary = true;
                    ftp.KeepAlive = false;                

                    FileInfo fi = new FileInfo(toolFileName);

                    if (fi.Exists) // 本地文件已经存在
                    {
                        ftp.Method = WebRequestMethods.Ftp.GetDateTimestamp;
                        FtpWebResponse response = ftp.GetResponse() as FtpWebResponse;
                        DateTime newEditTime = response.LastModified;
                        response.Close();

                        DateTime localEditTime = fi.LastWriteTime;

                        if (newEditTime > localEditTime) // 需要更新文件
                        {
                            requireUpdate = true;
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                logManager.RecordLog("检查更新", ex.Message, LogManager.LogLevel.Warning);
            }            

            return requireUpdate;
        }
    }
}
