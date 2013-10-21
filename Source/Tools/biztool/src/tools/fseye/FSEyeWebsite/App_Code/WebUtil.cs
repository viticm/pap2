//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3
//      File_base        : Util
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : Web应用工具相关
//
//////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Threading;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.SessionState;
using System.Collections;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using System.Collections.Generic;
using System.Drawing;
using System.Xml;
using Resources;
using FSEye.Security;
using FSEye.PlugIn;
using FSEye.DAL;
using FSEye.FS2Lib;
using FSEye;


/// <summary>
/// 工具类
/// </summary>
public class WebUtil
{
	private static Random g_random = new Random();
    private static string ShowTablesString = "show tables in {0}";

	/// <summary>
	/// 创建随机名称
	/// </summary>
	/// <param name="prefix">前缀</param>
	/// <param name="suffix">后缀</param>
	public static string CreateRandomName(string prefix, string suffix)
	{
		return 
			(prefix != null ? prefix : string.Empty)
			+ "{" + Guid.NewGuid().ToString() + "}"//g_random.Next(WebConfig.RandomMin, WebConfig.RandomMax)
			+ (suffix != null ? suffix : string.Empty);
	}

	/// <summary>
	/// 查询游戏数据库
	/// </summary>
	/// <param name="server">游戏服务器</param>
	/// <param name="cmd">SQL命令</param>
	/// <returns>查询结果</returns>
	/// <remarks>这个函数是同步的（会阻塞）</remarks>
	public static SqlResult QueryGameServerDb(int userId,GameServer server, SqlCommand cmd)
	{
        object outArg = null;
        //简单判断
        if (!server.IsConnected) return null;
        if (server.DoPlugInAction(userId, GameServerDb.PlugInGuid, GameServerDb.ActionKeyQuery, ref outArg, "xxx", cmd))
		{
			uint sessionId = (uint)outArg;
			for (int i = 0; i < WebConfig.GetQueryResultRetryTimes; i++)
			{
				Thread.Sleep(WebConfig.GetQueryResultDelay);
                SqlResult result = server.GetPlugInData(userId, GameServerDb.PlugInGuid, GameServerDb.DataKeyQueryResult, sessionId) as SqlResult;
				if (result != null)
				{
					return result;
				}
			}
		}

		return null;
	}

    /// <summary>
    /// 判断数据库里面存不存在给定的表
    /// </summary>
    /// <param name="server">游戏服务器</param>
    /// <param name="cmd">SQL命令</param>
    /// <param name="tableName">表名</param>
    /// <param name="databaseName">数据库名</param>
    /// <returns></returns>
    public static bool IsTableExist(int userId, GameServer server, string tableName, string databaseName)
    {
        SqlCommand cmd = new SqlCommand(string.Format(ShowTablesString, databaseName),string.Empty);
        SqlResult result = QueryGameServerDb(userId, server, cmd);
        if (result.ResultState == SqlResult.State.Done && result.Success)
        {
            result.SetFieldType(new SqlDataType[] {
                                SqlDataType.String
                                });
            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                string tempName = record[0] as string;
                if (string.Equals(tableName, tempName)) return true;
            }
        }
        return false;
    }


    /// <summary>
    /// 得到日志表名
    /// </summary>
    /// <param name="userId"></param>
    /// <param name="server"></param>
    /// <returns></returns>
    public static ArrayList GetLogTables(int userId, GameServer server)
    {
        SqlCommand cmd = new SqlCommand(string.Format(ShowTablesString, FS2TableString.FS2DatabaseName), string.Empty);
        SqlResult result = QueryGameServerDb(userId, server, cmd);
        ArrayList results = new ArrayList();
        if (result.ResultState == SqlResult.State.Done && result.Success)
        {
            result.SetFieldType(new SqlDataType[] {
                                SqlDataType.String
                                });
            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                string tableName = record[0] as string;
                if (tableName.Contains(FS2TableString.LogTableNamePrefix))
                    results.Add(tableName);
            }
            return results;
        }
        return null;
    }

	/// <summary>
	/// 通过角色名查找角色的GUID
	/// </summary>
	/// <param name="server">游戏服务器</param>
	/// <param name="roleName">角色名</param>
	/// <returns>角色的GUID，或者null（没有找到）</returns>
	public static string FindRoleGuidByName(int userId,GameServer server, string roleName)
	{
		if (roleName != null && roleName.Length > 0)
		{
			string cmdText = string.Format("SELECT {0} FROM {1} WHERE {2}='?'",
				FS2TableString.RolesfirstFieldGUID,
				FS2TableString.RolesfirstTableName,
				FS2TableString.RolesfirstFieldRoleName);
            
			SqlResult findRoleGuidResult = QueryGameServerDb(userId,server, new SqlCommand(cmdText, roleName));
			if (findRoleGuidResult != null && findRoleGuidResult.Success)
			{
				findRoleGuidResult.SetFieldType(new SqlDataType[] { SqlDataType.String });
				object[] record = findRoleGuidResult.ReadRecord();
				if (record != null)
				{
					return record[0] as string;
				}
			}
		}

		return null;
	}

	/// <summary>
	/// 权限检查
	/// </summary>
	/// <param name="objId">请求访问的对象的编号</param>
	/// <param name="opType">访问类型</param>
	/// <param name="session">会话</param>
	public static bool CheckPrivilege(int objId, int opType, HttpSessionState session)
	{
		FSEyeObject checkObj = AdminServer.TheInstance.SecurityManager.Get(objId);
		return CheckPrivilege(checkObj, opType, session);
	}

	/// <summary>
	/// 权限检查
	/// </summary>
	/// <param name="fullName">请求访问的对象的完整路径</param>
	/// <param name="opType">访问类型</param>
	/// <param name="session">会话</param>
	public static bool CheckPrivilege(string fullName, int opType, HttpSessionState session)
	{
		if (fullName == null)
		{
			throw new ArgumentNullException("fullName");
		}

		FSEyeObject checkObj = AdminServer.TheInstance.SecurityManager.Get(fullName);
		return CheckPrivilege(checkObj, opType, session);
	}

	/// <summary>
	/// 权限检查
	/// </summary>
	/// <param name="checkObj">请求访问的对象</param>
	/// <param name="opType">访问类型</param>
	/// <param name="session">会话</param>
	public static bool CheckPrivilege(FSEyeObject checkObj, int opType, HttpSessionState session)
	{
        //加入异常处理
        try
        {
            //return true;

            if (checkObj == null)
            {
                throw new ArgumentNullException("checkObj");
            }
            if (session == null)
            {
                throw new ArgumentNullException("session");
            }

            object obj = null;
            if ((obj = session["CurrentUser"]) != null)
            {
                int id = (obj as UserInfo).Id;
                User user = AdminServer.TheInstance.SecurityManager.GetLoginUser(id);
                if (user == null)
                {
                    throw new Exception("login user with id=\"" + id + "\" can not be found");
                }

                if (checkObj.CheckPrivilege(user, opType) == CheckPrivilegeResult.Allow)
                {
                    return true;
                }
            }
        }
        catch (Exception)
        {            
            return false;
        }

		return false;
	}

	/// <summary>
	/// 得到用户
	/// </summary>
	public static User GetUser(HttpSessionState session)
	{
		object obj = null;
		if ((obj = session["CurrentUser"]) != null)
		{
			int id = (obj as UserInfo).Id;
			return AdminServer.TheInstance.SecurityManager.GetLoginUser(id);
		}

		return null;
	}

	/// <summary>
	/// 得到Guard任务的名字
	/// </summary>
	/// <param name="task">任务</param>	
	public static string GetGuardTaskName(GameServerControl.GuardTask task)
	{
		switch (task)
		{
			case GameServerControl.GuardTask.Unknown:
				return StringDef.Unknown;
			case GameServerControl.GuardTask.Idle:
				return StringDef.Idle;
			case GameServerControl.GuardTask.DoNormalSysCmd:
				return StringDef.DoNormalSysCmd;
			case GameServerControl.GuardTask.StartGameServer:
				return StringDef.StartGame;
			case GameServerControl.GuardTask.UpdateGameServer:
				return StringDef.UpdateGame;
			case GameServerControl.GuardTask.RefreshUploadDirFileInfo:
				return StringDef.RefreshUploadDirFileInfo;
			default:
				return string.Empty;
		}
	}

	/// <summary>
	/// 更好的大小文本
	/// </summary>	
	public static string BetterSizeText(long size)
	{
		if (size < 1024)
		{
			return size + "Byte";
		}
		else if (size < 1048576)
		{
			return Decimal.Round((decimal)size / 1024,2) + "KB";
		}
		else
		{
			return Decimal.Round((decimal)size / 1048576,2) + "MB";
		}
	}

	public static void ShowMessage(Label label, string message, MessageType type)
	{
		if (label == null)
			throw new ArgumentNullException("control");

		label.Visible = true;
		label.Text = message;

		string cssClass = CssConfig.MessageNormal;
		switch (type)
		{
			case MessageType.Success:
				label.SkinID = "MessageSuccess";
				//cssClass = CssConfig.MessageSuccess;
				break;
			case MessageType.Failure:
				label.SkinID = "MessageFailure";
				//cssClass = CssConfig.MessageFailure;
				break;
			case MessageType.Attention:
				label.SkinID = "MessageAttention";
				//cssClass = CssConfig.MessageAttention;
				break;
			case MessageType.Normal:
				label.SkinID = "MessageNormal";
				//cssClass = CssConfig.MessageNormal;
				break;
		}
		label.CssClass = cssClass;
	}

	public static void ValidateValueString(string valueString)
	{
		if (!BlazeDbUtil.ValidateValueString(valueString))
			throw new Exception("Invalidate value string exception: \"" + valueString + "\"");
	}

    /// <summary>
    /// 根据时间判断然后取需要的日志表
    /// </summary>
    /// <param name="start">查询开始日期</param>
    /// <param name="end">查询结束日期</param>
    /// <returns></returns>
    public static string GetLogTableName(int userId, GameServer server, DateTime start, DateTime end)
    {
        StringBuilder table = new StringBuilder();
        ArrayList tables = new ArrayList();
        ArrayList tableNameList = GetLogTables(userId, server);

        if (start != DateTime.MinValue && end != DateTime.MinValue)
        {
            foreach (string tableName in tableNameList)
            {
                //日志表名格式logsyyyyMM
                DateTime tableDate = new DateTime(int.Parse(tableName.Substring(4, 4)), int.Parse(tableName.Substring(8, 2)), 1);
                if (tableDate.Year > start.Year)
                {
                    if (tableDate.Year < end.Year) tables.Add(tableName);
                    else if (tableDate.Year == end.Year && tableDate.Month <= end.Month) tables.Add(tableName);
                }
                else if (tableDate.Year == start.Year && tableDate.Month >= start.Month)
                {
                    if (tableDate.Year < end.Year) tables.Add(tableName);
                    else if (tableDate.Year == end.Year && tableDate.Month <= end.Month) tables.Add(tableName);
                }
            }
        }
        else if (start != DateTime.MinValue)
        {
            foreach (string tableName in tableNameList)
            {

                //日志表名格式logsyyyyMM
                DateTime tableDate = new DateTime(int.Parse(tableName.Substring(4, 4)), int.Parse(tableName.Substring(8, 2)), 1);
                if (tableDate.Year > start.Year) tables.Add(tableName);
                else if (tableDate.Year == start.Year && tableDate.Month >= start.Month) tables.Add(tableName);
            }
        }
        else if (end != DateTime.MinValue)
        {
            foreach (string tableName in tableNameList)
            {

                //日志表名格式logsyyyyMM
                DateTime tableDate = new DateTime(int.Parse(tableName.Substring(4, 4)), int.Parse(tableName.Substring(8, 2)), 1);
                if (tableDate.Year < end.Year) tables.Add(tableName);
                else if (tableDate.Year == end.Year && tableDate.Month <= end.Month) tables.Add(tableName);
            }
        }
        else
        {
            tables = tableNameList;
        }

        //写成Mysql能用的Sql语句
        if (tables.Count == 0)
        {
            throw new Exception(StringDef.MessageNoLogTable);
        }
        else if (tables.Count == 1)
        {
            string tbName = tables[0] as string;
            table.Append(tbName);
        }
        else
        {
            foreach (string tableName in tables)
            {
                table.Append(string.Format("Select * from {0}", tableName));
                table.Append(" UNION ");
            }
            if (table.Length != 0) return string.Format("({0}) AS AliasTableName", table.ToString(0, table.Length - 6));
        }
        return table.ToString();
    }

    public static void AddTableNameToCmd(int userId, GameServer server, string cmdString,ArrayList paramList, DateTime start, DateTime end,out string resultString,out ArrayList resultParamList)
    {
        ArrayList resultList = new ArrayList();
        //先查再UNION
        ArrayList logTables = WebUtil.GetLogTables(userId, server);
        StringBuilder cmdBuilder = new StringBuilder();
        for (DateTime temp = start; (end.Year - temp.Year) * 12 + (end.Month - temp.Month) >= 0; temp = temp.AddMonths(1))
        {
            if (logTables.Contains(string.Concat("logs", temp.ToString("yyyyMM"))))
            {
                cmdBuilder.Append('(');
                cmdBuilder.AppendFormat(cmdString, string.Concat("logs", temp.ToString("yyyyMM")));
                cmdBuilder.Append(')');
                cmdBuilder.Append(" UNION ALL ");
                if (paramList.Count != 0)
                {
                    foreach (string param in paramList)
                    {
                        resultList.Add(param);
                    }
                }
            }
        }

        if (cmdBuilder.Length == 0)
        {
            resultString = string.Empty;
            resultParamList = null;
        }
        else
        {
            resultString = cmdBuilder.ToString(0, cmdBuilder.Length - 11);
            resultParamList = resultList;
        }
    }

    public static void AddTableNameToCmd(int userId, GameServer server, string cmdString, DateTime start, DateTime end, out string resultString)
    {
        ArrayList logTables = WebUtil.GetLogTables(userId, server);
        StringBuilder cmdBuilder = new StringBuilder();
        for (DateTime temp = start; (end.Year - temp.Year) * 12 + (end.Month - temp.Month) >= 0; temp = temp.AddMonths(1))
        {
            if (logTables.Contains(string.Concat("logs", temp.ToString("yyyyMM"))))
            {
                cmdBuilder.Append('(');
                cmdBuilder.AppendFormat(cmdString, string.Concat("logs", temp.ToString("yyyyMM")));
                cmdBuilder.Append(')');
                cmdBuilder.Append(" UNION ALL ");        
            }
        }

        if (cmdBuilder.Length == 0)
        {
            resultString = string.Empty;
        }
        else
        {
            resultString = cmdBuilder.ToString(0, cmdBuilder.Length - 11);
        }
    }

    public static bool ExportScheduledTask(string fileName, IList taskList)
    {
        FileStream fileStream = null;
        bool bRet = false;
        try
        {
            fileStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write);
            fileStream.SetLength(0L);
            XmlTextWriter writer = new XmlTextWriter(fileStream, Encoding.Default);            
            IFormatter formatter = new BinaryFormatter();
            MemoryStream taskDataStream;

            //声明
            writer.WriteStartDocument();
            writer.WriteWhitespace("\n");

            writer.WriteStartElement("TaskList");
            foreach (ScheduledTaskUnit taskUnit in taskList)
            {
                taskDataStream = new MemoryStream();
                formatter.Serialize(taskDataStream, taskUnit.Task);

                writer.WriteWhitespace("\n\t");
                writer.WriteStartElement("Task");
                writer.WriteWhitespace("\n\t\t");
                
                writer.WriteElementString("Name", taskUnit.Task.SecurityObject.Name);
                writer.WriteWhitespace("\n\t\t");

                writer.WriteElementString("Comment", taskUnit.Task.SecurityObject.Comment);
                writer.WriteWhitespace("\n\t\t");

                writer.WriteStartElement("Data");
                writer.WriteBinHex(taskDataStream.ToArray(), 0, (int)taskDataStream.Length);
                writer.WriteEndElement();
                writer.WriteWhitespace("\n\t");

                writer.WriteEndElement();
            }

            writer.WriteWhitespace("\n");
            writer.WriteEndElement();

            writer.Flush();            
        }
        catch (Exception)
        {
            bRet = false;
        }
        finally
        {
            if (fileStream != null) fileStream.Close();
        }
        return bRet;
    }

    public static bool ImportScheduledTask(string fileName)
    {
        FileStream fileStream = null;
        bool bRet = false;
        try
        {
            int buffSize = 512;
            byte[] taskData;
            fileStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Read);
            XmlTextReader reader = new XmlTextReader(fileStream);
                        
            reader.MoveToElement();
            
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name.Equals("Task"))
                {
                    //读Name
                    reader.Read();                    
                    reader.MoveToContent();
                    string name = reader.ReadElementContentAsString();
                    //读Comment
                    reader.Read();
                    reader.MoveToContent();
                    string comment = reader.ReadElementContentAsString();
                    //读Data
                    reader.Read();
                    reader.MoveToContent();
                    
                    int realLength = 0;
                    taskData = new byte[buffSize];                    
                    byte[] tempBuff = new byte[buffSize];
                    int readCount = reader.ReadElementContentAsBinHex(tempBuff, 0, buffSize);
                    Array.Copy(tempBuff, 0, taskData, realLength, readCount);
                    realLength += readCount;
                    while (tempBuff.Length == readCount)
                    {
                        Array.Resize<byte>(ref taskData, taskData.Length + buffSize);
                        readCount = reader.ReadElementContentAsBinHex(tempBuff, 0, buffSize);
                        Array.Copy(tempBuff, 0, taskData, realLength, readCount);
                        realLength += readCount;
                    }
                    Array.Resize<byte>(ref taskData, realLength);
                    MemoryStream taskDataStream = new MemoryStream(taskData);
                    IFormatter formatter = new BinaryFormatter();
                    IScheduledTask task = (IScheduledTask)formatter.Deserialize(taskDataStream);
                    AdminServer.TheInstance.ScheduledTaskManager.Add(task, name, comment);
                }
            }
            bRet = true;
        }
        catch (Exception)
        {
            bRet = false;
        }
        finally
        {
            if (fileStream != null) fileStream.Close();
        }
        return bRet;
    }

    public static bool ExportStrategy(string fileName, IList strategyList)
    {
        FileStream fileStream = null;
        bool bRet = false;
        try
        {
            fileStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write);
            fileStream.SetLength(0L);
            XmlTextWriter writer = new XmlTextWriter(fileStream, Encoding.Default);
            byte[] strategyBytes;

            //声明
            writer.WriteStartDocument();
            writer.WriteWhitespace("\n");

            writer.WriteStartElement("StrategyList");
            foreach (Strategy strategy in strategyList)
            {                
                writer.WriteWhitespace("\n\t");
                writer.WriteStartElement("Strategy");
                writer.WriteWhitespace("\n\t\t");

                writer.WriteElementString("Name", strategy.SecurityObject.Name);
                writer.WriteWhitespace("\n\t\t");

                writer.WriteElementString("Comment", strategy.SecurityObject.Comment);
                writer.WriteWhitespace("\n\t\t");

                writer.WriteElementString("Event", ((int)strategy.Event).ToString());
                writer.WriteWhitespace("\n\t\t");

                writer.WriteStartElement("Automation");
                strategyBytes = AdminServer.TheInstance.AutomationManager.Save(strategy.Automation);
                writer.WriteBinHex(strategyBytes, 0, strategyBytes.Length);
                writer.WriteEndElement();
                writer.WriteWhitespace("\n\t\t");

                writer.WriteElementString("Enable", strategy.Enabled ? "1" : "0");
                writer.WriteWhitespace("\n\t");

                writer.WriteEndElement();
                
            }
            writer.WriteWhitespace("\n");
            writer.WriteEndElement();

            writer.Flush();
            bRet = true;
        }
        catch (Exception)
        {
            bRet = false;
        }
        finally
        {
            if (fileStream != null) fileStream.Close();
        }
        return bRet;
    }

    public static bool ImportStrategy(string fileName)
    {
        FileStream fileStream = null;
        bool bRet = false;
        try
        {
            int buffSize = 512;
            byte[] automationData;
            fileStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Read);
            XmlTextReader reader = new XmlTextReader(fileStream);
            
            reader.MoveToElement();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name.Equals("Strategy"))
                {                    
                    //读Name
                    reader.Read();
                    reader.MoveToContent();
                    string name = reader.ReadElementContentAsString();
                    //读Comment
                    reader.Read();
                    reader.MoveToContent();
                    string comment = reader.ReadElementContentAsString();

                    Strategy strategyToAdd = new Strategy();
                    //读Event
                    reader.Read();
                    reader.MoveToContent();
                    strategyToAdd.Event = (FSEyeEvent)reader.ReadElementContentAsInt();
                    //读Automation
                    reader.Read();
                    reader.MoveToContent();                    
                    int realLength = 0;
                    automationData = new byte[buffSize];
                    byte[] tempBuff = new byte[buffSize];
                    int readCount = reader.ReadElementContentAsBinHex(tempBuff, 0, buffSize);
                    Array.Copy(tempBuff, 0, automationData, realLength, readCount);
                    realLength +=  readCount;
                    while (tempBuff.Length == readCount)
                    {
                        Array.Resize<byte>(ref automationData, automationData.Length + buffSize);
                        readCount = reader.ReadElementContentAsBinHex(tempBuff, 0, buffSize);
                        Array.Copy(tempBuff, 0, automationData, realLength, readCount);
                        realLength += readCount;                        
                    }
                    Array.Resize<byte>(ref automationData, realLength);
                    strategyToAdd.Automation = AdminServer.TheInstance.AutomationManager.Load(automationData);
                    //读Enable
                    reader.Read();
                    reader.MoveToContent();
                    strategyToAdd.Enabled = reader.ReadElementContentAsInt() == 1 ? true : false;                    
                    AdminServer.TheInstance.StrategyManager.Add(strategyToAdd, name, comment);
                }
            }
            bRet = true;
        }
        catch (Exception)
        {
            bRet = false;
        }
        finally
        {
            if (fileStream != null) fileStream.Close();
        }
        return bRet;
    }    
    public static Color GetRandomColor()
    {
        Random random = new Random();
        Color color = Color.FromArgb(random.Next(0, 255), random.Next(0, 255), random.Next(0, 255));
        return color;
    }

    public static string GetColorWebStyle(Color color)
    {
        return string.Format("#{0:X2}{1:X2}{2:X2}", color.R, color.G, color.B);
    }

    public static string GetHashCodeString(byte[] hashCode)
    {
        StringBuilder hashCodeBuilder = new StringBuilder();

        foreach (byte code in hashCode)
        {
            hashCodeBuilder.Append(code.ToString("x2"));
        }

        return hashCodeBuilder.ToString();
    }

    public static string GetItemQualityWebStyleColor(FS2ItemData item, bool isInTip)
    {
        string colorStr = isInTip ? "White" : "Black";
        if (item != null)
        {
            switch (item.Quality)
            {
                case FS2ItemQuality.Blue:
                    colorStr = "#3165FF";
                    break;
                case FS2ItemQuality.Green:
                    colorStr = "#00FB00";
                    break;
                case FS2ItemQuality.Yellow:
                    colorStr = "Yellow";
                    break;
                case FS2ItemQuality.Orange:
                    colorStr = "#F8952C";
                    break;
            }
        }
        return colorStr;
    }
}