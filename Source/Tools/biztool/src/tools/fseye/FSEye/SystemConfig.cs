using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using FSEye.Cryptography;

namespace FSEye
{
	public class SystemConfig
	{
		const string XmlElementSystemConfig = "SystemConfig";
		
		const string XmlElementConfig = "Config";

		const string XmlAttribute = "attribute";

		const string XmlValue = "value";

		ushort _maxConnectionCount;

        string _adminServerListenIP;

		int _adminServerListenPort;

		int _secondaryAdminServerListenPort;

		string _adminServerUploadFileRootPath;

        string _serverListFileDir;

        string _serverListFileName;

        string _pakFileName;

        string _serverListCRCFileName;

        int _serverListServerGroupMaxPlayer;

		string _dbConnectionString;

        string dbDefaultIP;

        string defaultGameDbName;

		string _defaultDbName;

		Encoding _defaultEncoding = Encoding.Default;

		string _defaultNewLine = Environment.NewLine;

		string _logDir = string.Empty;

		bool _logEnabled = true;

		int _listenForConfigGuardReturnPort;

		int _configGuardPort;

		bool _needGuardAuthorization = true;

		string _gameSettingDir;

		long _guardAuthorizationTimeout = 5000;

		string _paySysDbConnectionString;

		bool _enablePaysys = false;

		bool _preloadGameData = false;

        string _smsUrl = "";

        string _smsCommand = "";

        string websiteVersion = "0-0-0-0";
        string _groupPlayerCountDir;
        string _groupPlayerCountFileName;
        int _groupPlayerCountKickInterval;

		static SystemConfig g_currentConfig = new SystemConfig();

		public static SystemConfig Current
		{
			get	{ return g_currentConfig; }
			set { g_currentConfig = value; }
		}

		public static SystemConfig LoadFromXml(Stream stream, bool isEncrypted)
		{
			if (stream == null)
			{
				throw new ArgumentNullException("stream");
			}

			using (XmlTextReader reader = new XmlTextReader(stream))
			{
				if (reader != null)
				{
					SystemConfig config = new SystemConfig();
					while (reader.Read())
					{
						if (reader.NodeType == XmlNodeType.Element && reader.Name == XmlElementConfig)
						{
							reader.MoveToAttribute(XmlAttribute);
							string attribute = reader.ReadContentAsString();
							switch (attribute)
							{
								case "MaxConnectionCount":
									reader.MoveToAttribute(XmlValue);
									config._maxConnectionCount = (ushort)reader.ReadContentAsInt();
									break;
                                case "AdminServerListenIP":
                                    reader.MoveToAttribute(XmlValue);
                                    config._adminServerListenIP = reader.ReadContentAsString();
                                    break;
								case "AdminServerListenPort":
									reader.MoveToAttribute(XmlValue);
									config._adminServerListenPort = reader.ReadContentAsInt();
									break;                                
								case "SecondaryAdminServerListenPort":
									reader.MoveToAttribute(XmlValue);
									config._secondaryAdminServerListenPort = reader.ReadContentAsInt();
									break;
								case "AdminServerUploadFileRootPath":
									reader.MoveToAttribute(XmlValue);
									config._adminServerUploadFileRootPath = reader.ReadContentAsString();

                                    if (!config._adminServerUploadFileRootPath.EndsWith(@"\")) config._adminServerUploadFileRootPath += @"\";

                                    //若该文件夹不存在，则创建它
                                    if (Directory.Exists(config._adminServerUploadFileRootPath) == false)
                                    {
                                        Directory.CreateDirectory(config._adminServerUploadFileRootPath);
                                    }

									break;
								case "DbConnectionString":
									{
										reader.MoveToAttribute(XmlValue);
										string readConnectionString = reader.ReadContentAsString();
										if (isEncrypted)
										{
											try
											{
												config._dbConnectionString = CryptographyManager.TheInstance.PredefinedDecrypt(readConnectionString, Encoding.Default);
											}
											catch (Exception ex)
											{
												throw new Exception("Decrypt SystemConfig Entry \"DbConnectionString\" Failed.", ex);
											}
										}
										else
										{
											config._dbConnectionString = readConnectionString;
										}
									}
									break;
								case "DefaultDbName":
									reader.MoveToAttribute(XmlValue);
									config._defaultDbName = reader.ReadContentAsString();
									break;
								case "DefaultEncoding":
									reader.MoveToAttribute(XmlValue);
									config._defaultEncoding = Encoding.GetEncoding(reader.ReadContentAsString());
									break;
								case "DefaultNewLine":
									{
										reader.MoveToAttribute(XmlValue);
										string newLine = reader.ReadContentAsString();
										if (newLine == @"\n")
										{
											config._defaultNewLine = "\n";
										}
										else if (newLine == @"\r\n")
										{
											config._defaultNewLine = "\r\n";
										}
									}
									break;
								case "LogDir":
									reader.MoveToAttribute(XmlValue);
									config._logDir = reader.ReadContentAsString();

                                    if (!config._logDir.EndsWith(@"\")) config._logDir+=@"\";

                                    //若该文件夹不存在，则创建它
                                    if (Directory.Exists(config._logDir) == false)
                                    {
                                        Directory.CreateDirectory(config._logDir);
                                    }

									break;
								case "LogEnabled":
									reader.MoveToAttribute(XmlValue);
									config._logEnabled = reader.ReadContentAsBoolean();
									break;
								case "ListenForConfigGuardReturnPort":
									reader.MoveToAttribute(XmlValue);
									config._listenForConfigGuardReturnPort = reader.ReadContentAsInt();
									break;
								case "ConfigGuardPort":
									reader.MoveToAttribute(XmlValue);
									config._configGuardPort = reader.ReadContentAsInt();
									break;
								case "NeedGuardAuthorization":
									reader.MoveToAttribute(XmlValue);
									config._needGuardAuthorization = reader.ReadContentAsBoolean();
									break;
								case "GameSettingDir":
									reader.MoveToAttribute(XmlValue);
                                    config._gameSettingDir = reader.ReadContentAsString();

                                    if (!config._gameSettingDir.EndsWith(@"\")) config._gameSettingDir += @"\";

                                    //若该文件夹不存在，则创建它
                                    if (Directory.Exists(config._gameSettingDir) == false)
                                    {
                                        Directory.CreateDirectory(config._gameSettingDir);
                                    }
									
									break;
								case "GuardAuthorizationTimeout":
									reader.MoveToAttribute(XmlValue);
									config._guardAuthorizationTimeout = reader.ReadContentAsLong();
									break;
                                case "EnablePaysys":
                                    reader.MoveToAttribute(XmlValue);
                                    config._enablePaysys = reader.ReadContentAsBoolean();
                                    break;
                                case "DBDefaultIP":
                                    reader.MoveToAttribute(XmlValue);
                                    config.dbDefaultIP = reader.ReadContentAsString();
                                    break;
                                case "DefaultGameDbName":
                                    reader.MoveToAttribute(XmlValue);
                                    config.defaultGameDbName = reader.ReadContentAsString();
                                    break;
								case "PaySysDbConnectionString":
									{
										reader.MoveToAttribute(XmlValue);
										string readConnectionString = reader.ReadContentAsString();
                                        if (config._enablePaysys)
                                        {
                                            if (isEncrypted)
                                            {
                                                try
                                                {
                                                    config._paySysDbConnectionString = CryptographyManager.TheInstance.PredefinedDecrypt(readConnectionString, Encoding.Default);
                                                }
                                                catch (Exception ex)
                                                {
                                                    throw new Exception("Decrypt SystemConfig Entry \"PaySysDbConnectionString\" Failed.", ex);
                                                }
                                            }
                                            else
                                            {
                                                config._paySysDbConnectionString = readConnectionString;
                                            }
                                        }										
									}
									break;
								case "PreloadGameData":
									reader.MoveToAttribute(XmlValue);
									config._preloadGameData = reader.ReadContentAsBoolean();
									break;
                                case "ServerListFileDir":
                                    reader.MoveToAttribute(XmlValue);
                                    config._serverListFileDir = reader.ReadContentAsString();
                                    
                                    if (!config._serverListFileDir.EndsWith(@"\")) config._serverListFileDir += @"\";

                                    //若该文件夹不存在，则创建它
                                    if (Directory.Exists(config._serverListFileDir) == false)
                                    {
                                        Directory.CreateDirectory(config._serverListFileDir);
                                    }

                                    break;
                                case "ServerListFileName":
                                    reader.MoveToAttribute(XmlValue);
                                    config._serverListFileName = reader.ReadContentAsString();
                                    break;
                                case "PakFileName":
                                    reader.MoveToAttribute(XmlValue);
                                    config._pakFileName = reader.ReadContentAsString();
                                    break;
                                case "ServerListCRCFileName":
                                    reader.MoveToAttribute(XmlValue);
                                    config._serverListCRCFileName = reader.ReadContentAsString();
                                    break;
                                case "ServerListServerGroupMaxPlayer":
                                    reader.MoveToAttribute(XmlValue);
                                    config._serverListServerGroupMaxPlayer = reader.ReadContentAsInt();
                                    break;
                                case "SMSUrl":
                                    reader.MoveToAttribute(XmlValue);
                                    config._smsUrl = reader.ReadContentAsString();
                                    break;
                                case "SMSCommand":
                                    reader.MoveToAttribute(XmlValue);
                                    config._smsCommand = reader.ReadContentAsString();
                                    break;
                                case "WebSiteVersion":
                                    reader.MoveToAttribute(XmlValue);
                                    config.websiteVersion = reader.ReadContentAsString();
                                    break;
                                case "GroupPlayerCountDir":
                                    reader.MoveToAttribute(XmlValue);
                                    config._groupPlayerCountDir = reader.ReadContentAsString();
                                    if (!config._groupPlayerCountDir.EndsWith(@"\")) config._groupPlayerCountDir += @"\";

                                    //若该文件夹不存在，则创建它
                                    if (Directory.Exists(config._groupPlayerCountDir) == false)
                                    {
                                        Directory.CreateDirectory(config._groupPlayerCountDir);
                                    }
                                    break;
                                case "GroupPlayerCountFileName":
                                    reader.MoveToAttribute(XmlValue);
                                    config._groupPlayerCountFileName = reader.ReadContentAsString();
                                    break;
                                case "GroupPlayerCountKickInterval":
                                    reader.MoveToAttribute(XmlValue);
                                    config._groupPlayerCountKickInterval = reader.ReadContentAsInt();
                                    break;
								default:
									throw new Exception("Unknown attribute '" + attribute + "' found in xml stream.");
							}
						}
					}
					return config;
				}
			}

			return null;
		}

		public SystemConfig()
		{
		}

		/// <summary>
		/// 最多连接数量
		/// </summary>
		/// <remarks>0表示无限制</remarks>
		public ushort MaxConnectionCount
		{
			get { return _maxConnectionCount; }
		}

        public string AdminServerListenIP
        {
            get { return _adminServerListenIP; }
        }

		/// <summary>
		/// 管理服务器监听端口（首选）
		/// </summary>
		public int AdminServerListenPort
		{
			get { return _adminServerListenPort; }
		}

        /// <summary>
        /// 运营平台IP（首选）
        /// </summary>
        public string DBDefaultIP
        {
            get { return dbDefaultIP; }
        }

        /// <summary>
        /// 短信网关(WEB)
        /// </summary>
        public string SMSUrl
        {
            get { return _smsUrl; }
        }

        /// <summary>
        /// 短信网关认证
        /// </summary>
        public string SMSCommand
        {
            get { return _smsCommand; }
        }

		/// <summary>
		/// 管理服务器监听端口（次选）
		/// </summary>
		public int SecondaryAdminServerListenPort
		{
			get { return _secondaryAdminServerListenPort; }
		}

		/// <summary>
		/// 监听配置Guard确认端口
		/// </summary>
		public int ListenForConfigGuardReturnPort
		{
			get { return _listenForConfigGuardReturnPort; }
		}

		/// <summary>
		/// 配置Guard端口
		/// </summary>
		public int ConfigGuardPort
		{
			get { return _configGuardPort; }
		}

		/// <summary>
		/// 管理服务器上传文件根路径
		/// </summary>
		public string AdminServerUploadFileRootPath
		{
			get { return _adminServerUploadFileRootPath; }
		}

		/// <summary>
		/// 数据库连接字符串
		/// </summary>
		public string DbConnectionString
		{
			get { return _dbConnectionString; }
		}

		/// <summary>
		/// 默认数据库名
		/// </summary>
		public string DefaultDbName
		{
			get { return _defaultDbName; }
		}

        public string DefaultGameDbName
		{
			get { return defaultGameDbName; }
		}

       

		/// <summary>
		/// 默认编码
		/// </summary>
		public Encoding DefaultEncoding
		{
			get { return _defaultEncoding; }
		}

		/// <summary>
		/// 默认换行符号
		/// </summary>
		public string DefaultNewLine
		{
			get { return _defaultNewLine; }
		}

		/// <summary>
		/// 日志目录
		/// </summary>
		public string LogDir
		{
			get { return _logDir; }
		}

		/// <summary>
		/// 启用日志
		/// </summary>
		public bool LogEnabled
		{
			get { return _logEnabled; }
		}

		/// <summary>
		/// 需要Guard验证
		/// </summary>
		public bool NeedGuardAuthorization
		{
			get { return _needGuardAuthorization; }
		}

		/// <summary>
		/// 游戏设定目录
		/// </summary>
		public string GameSettingDir
		{
			get { return _gameSettingDir; }
		}

		/// <summary>
		/// Guard验证超时
		/// </summary>
		public long GuardAuthorizationTimeout
		{
			get { return _guardAuthorizationTimeout; }
		}

		/// <summary>
		/// PaySys数据库连接字符串
		/// </summary>
		public string PaySysDbConnectionString
		{
			get { return _paySysDbConnectionString; }
		}

		/// <summary>
		/// 启用Paysys
		/// </summary>
		public bool EnablePaysys
		{
			get { return _enablePaysys; }
		}

		/// <summary>
		/// 预先载入游戏数据
		/// </summary>
		public bool PreloadGameData
		{
			get { return _preloadGameData; }
		}

        /// <summary>
        /// 服务器列表文件保存目录
        /// </summary>
        public string ServerListFileDir
        {
            get { return _serverListFileDir; }
        }

        /// <summary>
        /// 服务器列表文件名称
        /// </summary>
        public string ServerListFileName
        {
            get { return _serverListFileName; }
        }

        /// <summary>
        /// pak包名称
        /// </summary>
        public string PakFileName
        {
            get { return _pakFileName; }
        }

        /// <summary>
        /// 服务器列表CRC文件名称(存储对服务器列表文件的CRC校验码)
        /// </summary>
        public string ServerListCRCFileName
        {
            get { return _serverListCRCFileName; }
        }

        /// <summary>
        /// 服务器列表单个区服最大总人数
        /// </summary>
        public int ServerListServerGroupMaxPlayer
        {
            get { return _serverListServerGroupMaxPlayer; }
        }
        public string WebSiteVersion
        {
            get { return websiteVersion; }
        }
        public string GroupPlayerCountFileName
        {
            get { return _groupPlayerCountFileName; }
        }
        public string GroupPlayerCountDir
        {
            get { return _groupPlayerCountDir; }
        }
        public int GroupPlayerCountKickInterval
        {
            get { return _groupPlayerCountKickInterval; }
        }
        

	}

	/// <summary>
	/// 预定义的安全对象
	/// </summary>
	public class PredefinedSecurityObjects
	{
		public const string GameServerList = "GameServerList";//游戏服务器列表
		public const string ServerInfo = "ServerInfo";//服务器信息
			public const string BasicInfo = "BasicInfo";//基本信息
			public const string Cpu = "Cpu";//CPU
			public const string Memory = "Memory";//内存
			public const string NetCard = "NetCard";//网卡
			public const string Disk = "Disk";//磁盘
			public const string Process = "Process";//进程
			public const string PlugIn = "PlugIn";//插件
			public const string FMTask = "FMTask";//文件管理任务		
		public const string Console = "Console";//控制台

		public const string Users = "Users";//用户
		public const string Groups = "Groups";//组
	}
}
