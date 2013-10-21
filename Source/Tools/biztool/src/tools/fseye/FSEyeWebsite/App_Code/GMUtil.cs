using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Collections;
using System.Text;
using Resources;
using FSEye.FS2Lib;
using FSEye.PlugIn;
using FSEye.Security;
using FSEye;
using System.Threading;

/// <summary>
/// GM工具类，主要的GM操作以静态方法的方式保存在这个类中。
/// </summary>
public class GMUtil
{
    public GMUtil()
    {
    }

    /// <summary>
    /// 玩家禁言及解禁言
    /// </summary>
    /// <param name="userId">FSEye用户</param>
    /// <param name="server">服务器</param>
    /// <param name="roleName">角色名</param>
    /// <param name="chatEnable">禁言标志位 true为解禁言 false为禁言</param>
    /// <returns></returns>
    public static bool SetRoleChatEnable(int userId, GameServer server, string roleName, bool chatEnable)
    {
        if (server == null || !server.IsConnected)
            return false;

        //先更新数据库然后发GM指令
        ArrayList paramList = new ArrayList();
        string sqlCmdText = string.Format("UPDATE {0} SET {1}='?' WHERE {2}='?';",
            FS2TableString.RolesfirstTableName,
            FS2TableString.RolesfirstFieldNoChatIn, 
            FS2TableString.RolesfirstFieldRoleName);
        paramList.Add(chatEnable ? "0" : "1");
        paramList.Add(roleName);

        SqlCommand sqlCmd = new SqlCommand(sqlCmdText, paramList.ToArray());
        SqlResult result = WebUtil.QueryGameServerDb(userId, server, sqlCmd);
        if (result != null && result.Success)
        {
            string gmCmd = string.Format("?gm ds SetChatFlag({0})", chatEnable ? 0 : 1);
            server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, gmCmd);

            FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(userId,
                chatEnable ? (int)GMOperation.ChatEnable : (int)GMOperation.ChatDisable,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                              AdminServer.TheInstance.SecurityManager.GetUser(userId).RealName,
                              server.Name,
                              roleName == null || roleName.Length == 0 ? string.Empty : string.Format("[{0}]", roleName),
                              chatEnable ? GMOperation.ChatEnable.ToString() : GMOperation.ChatDisable.ToString()),
                (int)GMOpTargetType.Role);
            return true;
        }
        else
        {
            return false;
        }        
    }

    public static bool SetRoleFrozen(int userId, GameServer server, string roleName, bool freeze)
    {
        if (server == null || !server.IsConnected)
            return false;

        //先更新数据库然后发GM指令
        ArrayList paramList = new ArrayList();
        string sqlCmdText = string.Format("UPDATE {0} SET {1}='?' WHERE {2}='?';",
            FS2TableString.RolesfirstTableName,
            FS2TableString.RolesfirstFieldNoLoginIn,
            FS2TableString.RolesfirstFieldRoleName);
        paramList.Add(freeze ? "1" : "0");
        paramList.Add(roleName);

        SqlCommand sqlCmd = new SqlCommand(sqlCmdText, paramList.ToArray());
        SqlResult result = WebUtil.QueryGameServerDb(userId, server, sqlCmd);
        if (result != null && result.Success)
        {
            //发GM指令设置Login标志位并把强制玩家下线
            string gmCmd = string.Format("?gm ds SetLoginFlag({0})", freeze ? 1 : 0);
            server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, gmCmd);

            if (freeze)
                server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, WebConfig.GMCommandKickPlayer);
            
            FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(userId,
                freeze ? (int)GMOperation.FreezeRole : (int)GMOperation.UnfreezeRole,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                              AdminServer.TheInstance.SecurityManager.GetUser(userId).RealName,
                              server.Name,
                              roleName == null || roleName.Length == 0 ? string.Empty : string.Format("[{0}]", roleName),
                              freeze ? StringDef.Freeze + StringDef.Role : StringDef.Unfreeze + StringDef.Role),
                (int)GMOpTargetType.Role);
            return true;
        }
        else
        {
            return false;
        }
    }

    /// <summary>
    /// 能过帐号名得到角色名(暂时还没有全服查找)
    /// </summary>
    /// <param name="userId">FSEye用户ID</param>
    /// <param name="server">服务器</param>
    /// <param name="account">帐号</param>
    /// <param name="selectOnline">是否在线</param>
    /// <returns></returns>
    public static IList GetRoleNameByAccount(int userId, GameServer server, string account, bool selectOnline)
    {
        if (server == null || !server.IsConnected)
            return null;
        ArrayList roleList = new ArrayList();
        StringBuilder cmdText = new StringBuilder();
        cmdText.AppendFormat("SELECT {0},{1} FROM {2} WHERE {3}='?'", 
            FS2TableString.RolesfirstFieldId,
            FS2TableString.RolesfirstFieldRoleName, 
            FS2TableString.RolesfirstTableName,
            FS2TableString.RolesfirstFieldAccountName);
        if (selectOnline)
            cmdText.AppendFormat(" AND {0}='1'", FS2TableString.RolesfirstFieldOnline);

        SqlCommand cmd = new SqlCommand(cmdText.ToString(), account);
        SqlResult result = WebUtil.QueryGameServerDb(userId, server, cmd);
        if (result != null && result.Success)
        {
            result.SetFieldType(new SqlDataType[]{
                            SqlDataType.Int32,
                            SqlDataType.String
                            });
            object[] record = null;
            while ((record = result.ReadRecord()) != null)
            {
                roleList.Add(record[1] as string);
            }
        }
        return roleList;
    }

    /// <summary>
    /// 通过在线帐号的网关信息获取其正在登录的GameServer(未完成)
    /// </summary>
    /// <param name="account"></param>
    /// <returns></returns>
    public static GameServer GetAccountOnlineServer(int userId,string account)
    {
        //return AdminServer.TheInstance.GameServerManager.GetGameServer(91);
        return null;
    }

    /// <summary>
    /// 冻结帐号
    /// </summary>
    /// <param name="userId">FSEye用户Id</param>
    /// <param name="account">帐号</param>
    /// <param name="freeze">冻结标志位 true为冻结 false为解冻结</param>
    /// <returns>FSEye.PaySysLib.OperationResult</returns>
    public static FSEye.PaySysLib.OperationResult SetAccountFrozen(int userId,string account,bool freeze)
    {
        FSEye.PaySysLib.OperationResult result;
        if(freeze)
            result =  AdminServer.TheInstance.PaySysAgent.FreezeAccount(account);
        else
            result = AdminServer.TheInstance.PaySysAgent.UnfreezeAccount(account);

        FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(userId,
            freeze ? (int)GMOperation.FreezeAccount : (int)GMOperation.UnfreezeAccount,
            account,
            -1,
            string.Format(StringDef.GMMessageAccountLog,
                          AdminServer.TheInstance.SecurityManager.GetUser(userId).RealName,
                          account,
                          freeze ? GMOperation.FreezeAccount.ToString() : GMOperation.UnfreezeAccount.ToString(),
                          result.ToString()),
            (int)GMOpTargetType.Account);
        return result;
    }

    /// <summary>
    /// 得到帐号网关信息
    /// </summary>
    /// <param name="userId">FSEye用户Id</param>
    /// <param name="account">帐号</param>
    /// <returns>网关信息</returns>
    public static string GetGatewayByAccount(int userId,string account)
    {
        return AdminServer.TheInstance.PaySysAgent.GetGatewayByAccount(account);
    }

    /// <summary>
    /// [GM指令]发邮件
    /// </summary>
    /// <param name="userId">FSEye用户Id</param>
    /// <param name="serverId">服务器Id</param>
    /// <param name="parameters">发邮件GM指令参数</param>
    public static bool SendMail(int userId,int serverId,object[] parameters)
    {
        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
        if (server != null && server.IsConnected)
        {
            StringBuilder mailParam = new StringBuilder();
            string roleName = string.Empty;
            if (parameters.Length == 5 || parameters.Length == 10)
            {
                roleName = parameters[0] as string;
                foreach (object param in parameters)
                {
                    mailParam.Append(param);
                    mailParam.Append(',');
                }
            }
            else
            {
                return false;
                //throw new Exception("Parameters' number not correct.");
            }

            string gmCmd = string.Format(WebConfig.GMCommandSendMail, mailParam.ToString(0, mailParam.Length - 1));
            bool gmCmdResult = server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, gmCmd);

            FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(userId,
                (int)GMOperation.SendMail,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                              roleName,
                              server.Name,
                              roleName.Length == 0 ? string.Empty : string.Format("[{0}]", roleName),
                              StringDef.SendMail + string.Format("[{0}]", gmCmdResult ? StringDef.Success : StringDef.Failure)),
                (int)GMOpTargetType.Role);

            return gmCmdResult;
        }
        return false;
    }

    /*  以前的版本，现在拆分成两个函数
     *  ExeGMCommand负责发送协议
     *  GetGMCommandResult负责按照server和userId取执行结果
     *  但完整保留以前的版本
    public static string ExeGMCommand(int userId, int serverId, string roleName, string gmCmd, bool needLog)
    {
        object outArg = null;

        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
        if ((server != null && server.IsConnected) == false)
            return null;

        bool result = server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, ref outArg, roleName, gmCmd);

        if (result)
        {
            if (server != null && needLog)
            {
                User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
                FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(
                    userId,
                    (int)GMOperation.ExeSysCmd,
                    roleName,
                    server.Id,
                    string.Format(StringDef.GMMessageLog,
                            user == null ? userId + "(ID)" : user.RealName,
                            server.Name,
                            roleName.Length == 0 ? string.Empty : string.Format("[{0}]", roleName),
                            string.Format("{0}[{1}][{2}]", StringDef.ExecuteGMCommand, gmCmd, result ? StringDef.Success : StringDef.Failure)),
                    roleName.Length == 0 ? (int)GMOpTargetType.System : (int)GMOpTargetType.Role);
            }

            uint sessionId = (uint)outArg;
            for (int i = 0; i < WebConfig.GetQueryResultRetryTimes; i++)
            {
                Thread.Sleep(WebConfig.GetQueryResultDelay);
                string GMResult = server.GetPlugInData(userId, LordControl.PlugInGuid, LordControl.DataKeyGMResult, sessionId) as string;
                return GMResult;
            }
        }

        return null;
    }
    */

    public static bool ExeGMCommand(int userId, int serverId, string roleName, bool isAccountName, string gmCmd, bool isExecutedByGameServer, ArrayList gameServerArrayList, bool needLog)
    {
        object outArg = null;

        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
        if ((server != null && server.IsConnected) == false)
            return false;

        bool result = server.DoPlugInAction(userId, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, ref outArg, roleName, isAccountName, gmCmd, isExecutedByGameServer, gameServerArrayList);

        if (server != null && needLog)
        {
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
            FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(
                userId,
                (int)GMOperation.ExeSysCmd,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                        user == null ? userId + "(ID)" : user.RealName,
                        server.Name,
                        roleName.Length == 0 ? string.Empty : string.Format("[{0}]", roleName),
                        string.Format("{0}[{1}][{2}]", StringDef.ExecuteGMCommand, gmCmd, result ? StringDef.Success : StringDef.Failure)),
                roleName.Length == 0 ? (int)GMOpTargetType.System : (int)GMOpTargetType.Role);
        }

        return result;
    }

    public static string GetGMCommandResult(GameServer server, int userId)
    {
        string GMResult = server.GetPlugInData(userId, LordControl.PlugInGuid, LordControl.DataKeyUserGMResult, userId) as string;
        
        return GMResult;
    }










    public enum GMOperation
    {
        FreezeRole = 0,
        UnfreezeRole,
        FreezeAccount,
        UnfreezeAccount,
        ChatEnable,
        ChatDisable,
        SendMail,
        SendMessage,
        ExeSysCmd,
        CreateIBShopScript,
        DeleteIBShopScript
    }

    public enum GMOpTargetType
    {
        Account = 0,
        Role,
        System,
        Unknow = 10
    }
}