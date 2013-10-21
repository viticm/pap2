<%@ WebHandler Language="C#" Class="GameMasterHandler" %>

using System;
using System.Web;
using System.Text;
using System.Web.SessionState;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using Resources;
using FSEye;
using FSEye.FS2Lib;
using FSEye.PlugIn;

public class GameMasterHandler : IHttpHandler, IRequiresSessionState
{
	/* AJAX请求的操作 */
	const string OperationExecuteGMCmd = "ExecuteGMCmd";    
	const string OperationGetRoleDataSql = "GetRoleDataSql";
    const string OperationGetRoleItemInfo = "GetRoleItemInfo";
    const string OperationGetRoleSkillInfo = "GetRoleSkillInfo";
    const string OperationUpdateChatMsg = "UpdateChatMsg";
    const string OperationLoadSetting = "LoadSetting";
    const string OperationLoadIbItem = "LoadIbItem";
    const string OperationLoadItemStyle = "LoadItemStyle";
    const string OperationLoadItem = "LoadItem";
        
    public void ProcessRequest (HttpContext context)
	{
		if (!AdminServer.TheInstance.IsRunning)
		{
			context.Response.ContentType = "text/plain";
			context.Response.Output.Write(StringDef.ServiceUnavaiable);
			return;
		}
		
		//进行权限检查
        if (context.Session[WebConfig.SessionCurrentUser] == null)
        {
            //Response.Redirect(WebConfig.PageLogin, true);
            return;
        }
        UserInfo userInfo = context.Session[WebConfig.SessionCurrentUser] as UserInfo;
        if (userInfo == null) return;
        if (!AdminServer.TheInstance.SecurityManager.ValidateLoginUser(userInfo.Id, userInfo.LoginId))
        {
            context.Session.Remove(WebConfig.SessionCurrentUser);
            return;
        }
        
		if (context.Request.Params[WebConfig.ParamOperation] != null)
		{
			context.Response.ContentType = "text/plain";
			string op = context.Request.Params[WebConfig.ParamOperation];
			switch (op)
			{
				case OperationExecuteGMCmd:
					ProcessExecuteGMCmd(context);
					break;
				case OperationGetRoleDataSql:
					ProcessOperationGetRoleDataSql(context);
					break;
                case OperationGetRoleItemInfo:
                    ProcessOperationGetRoleItemInfo(context);
                    break;
                case OperationGetRoleSkillInfo:
                    ProcessOperationGetRoleSkillInfo(context);
                    break;
                case OperationUpdateChatMsg:
                    ProcessOperationUpdateChatMsg(context);
                    break;
                case OperationLoadSetting:
                    ProcessOpeartionLoadSetting(context);
                    break;
                case OperationLoadIbItem:
                    ProcessOperationLoadIbItem(context);
                    break;
                case OperationLoadItemStyle:
                    ProcessOperationLoadItemStyle(context);
                    break;
                case OperationLoadItem:
                    ProcessOperationLoadItem(context);
                    break;
				default:
					//TODO 记录错误
					break;
			}
		}
		else
		{
			//TODO 记录错误
		}
    }
 
    public bool IsReusable
	{
        get {
            return true;
        }
    }
	
	private void ProcessExecuteGMCmd(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null && context.Request.Params[WebConfig.ParamCommand] != null)
		{
			int serverId = 0;
			try
			{
				serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			}
			catch (FormatException)
			{
				//TODO
			}
			
			string command = context.Request.Params[WebConfig.ParamCommand];
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

			StringBuilder outputHtml = new StringBuilder();

			outputHtml.Append("server:");
			outputHtml.Append(serverId);
			outputHtml.Append("; cmd:");
			outputHtml.Append(command);

			context.Response.Output.Write(outputHtml.ToString());
		}
	}

    private void ProcessOperationGetRoleDataSql(HttpContext context)
	{
		int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
		int roleId = int.Parse(context.Request.Params[WebConfig.ParamRoleId]);
        
		GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
		if (server == null)
			return;
		
		string queryStatement = string.Format("SELECT * FROM {0} WHERE {1}={2}",
			FS2TableString.RolesfirstTableName,
			FS2TableString.RolesfirstFieldId,
			roleId);

        UserInfo userInfo = context.Session[WebConfig.SessionCurrentUser] as UserInfo;
        if (userInfo == null) return;

        if (!server.IsConnected)return;
        
        SqlResult result = WebUtil.QueryGameServerDb(userInfo.Id, server, new SqlCommand(queryStatement));
		if (result != null && result.Success)
		{
			result.SetFieldType(new SqlDataType[] {
				SqlDataType.Int32,
				SqlDataType.Int32,
				SqlDataType.String,
				SqlDataType.String,
				SqlDataType.String,
				SqlDataType.UInt16,
				SqlDataType.UInt16,
				SqlDataType.UInt16,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.DateTime,
				SqlDataType.DateTime,
				SqlDataType.DateTime,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.UInt16,
				SqlDataType.Int32,
				SqlDataType.Int32,
				SqlDataType.Int32,
				SqlDataType.Int32,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.Blob,
				SqlDataType.String,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.UInt32,
				SqlDataType.DateTime
			});

			object[] record = result.ReadRecord();
			if (record != null)
			{
				StringBuilder valueText = new StringBuilder();
				foreach (object fieldValue in record)
				{
					string text = SqlCommand.ValueToSqlString(fieldValue, SystemConfig.Current.DefaultEncoding);
					if (text != null && text.Length > 0)
						valueText.AppendFormat("{0},", text);
					else
						valueText.AppendFormat("'',");
				}
				valueText.Length = valueText.Length - 1;

				string cmd = string.Format("INSERT INTO {0} VALUES({1});",
					FS2TableString.RolesfirstTableName,
					valueText.ToString());

				context.Response.Clear();
				context.Response.ClearHeaders();
				context.Response.ClearContent();
				context.Response.HeaderEncoding = SystemConfig.Current.DefaultEncoding;
				context.Response.ContentEncoding = SystemConfig.Current.DefaultEncoding;
                context.Response.AppendHeader("Content-disposition", string.Format("attachment; filename={0}", HttpUtility.UrlEncode(string.Format("{0}_{1}_{2}.sql", server.Name, record[4], DateTime.Now.ToString("yyyy-MM-dd_HH:mm:ss")))));
				context.Response.ContentType = "application/octet-stream";
				context.Response.Output.Write(cmd);
				context.Response.End();
			}
		}
	}

    private void ProcessOperationGetRoleItemInfo(HttpContext context)
    {
        string itemTemplateId = context.Request.Params[WebConfig.ParamItemTemplateId];
        StringBuilder outputHtml = new StringBuilder();

        FS2ItemData itemData = FS2GameDataManager.TheInstance.GetItemData(itemTemplateId);
        if (itemData != null)
        {
			string imgStr = string.Format("<img src='{0}{1}_man_large_normal.jpg' />", GameDataRender.ImagePathPrefix, itemData.Image);
			
            string colorStr = string.Empty;
            switch (itemData.Quality)
            {
                case FS2ItemQuality.White:
                    colorStr = "White";
                    break;
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
            
            string itemPartStr = string.Empty;
            string qualityLevelStr = string.Empty;
            string requireProfs = string.Empty;
            string buffStr = string.Empty;
            if (itemData.Kind == 0)
            {
                switch (itemData.SpecialKind)
                {
                    case 0:
                        itemPartStr = "武器";
                        break;
                    case 2:
                        itemPartStr = "衣服";
                        break;
                    case 3:
                        itemPartStr = "戒指";
                        break;
                    case 4:
                        itemPartStr = "玉佩";
                        break;
                    case 5:
                        itemPartStr = "靴子";
                        break;
                    case 6:
                        itemPartStr = "护肩";
                        break;
                    case 7:
                        itemPartStr = "头盔";
                        break;
                    case 8:
                        itemPartStr = "护腕";
                        break;
                    case 9:
                        itemPartStr = "坠饰";
                        break;
                    case 10:
                        itemPartStr = "法宝";
                        break;
                    default:
                        break;
                }
                switch (itemData.QualityLevel)
                {
                    case FS2ItemQualityLevel.Low:
                        qualityLevelStr = "[初级]";
                        break;
                    case FS2ItemQualityLevel.Mid:
                        qualityLevelStr = "[中级]";
                        break;
                    case FS2ItemQualityLevel.High:
                        qualityLevelStr = "[高级]";
                        break;
                }

                if (itemData.RequireProfDesc != null)
                {
                    StringBuilder sbRequireProf = new StringBuilder();
                    foreach (FS2RoleType type in itemData.RequireProfDesc)
                    {
                        switch (type)
                        {
                            case FS2RoleType.Xuanfeng:
                                sbRequireProf.Append(StringDef.XuanFeng);
                                sbRequireProf.Append(' ');
                                break;
                            case FS2RoleType.Xingtian:
                                sbRequireProf.Append(StringDef.XingTian);
                                sbRequireProf.Append(' ');
                                break;
                            case FS2RoleType.Tianshi:
                                sbRequireProf.Append(StringDef.TianShi);
                                sbRequireProf.Append(' ');
                                break;
                            case FS2RoleType.Zhenren:
                                sbRequireProf.Append(StringDef.ZhenRen);
                                sbRequireProf.Append(' ');
                                break;
                            case FS2RoleType.Shoushi:
                                sbRequireProf.Append(StringDef.ShouShi);
                                sbRequireProf.Append(' ');
                                break;
                            case FS2RoleType.Yishi:
                                sbRequireProf.Append(StringDef.YiShi);
                                sbRequireProf.Append(' ');
                                break;                                
                        }
                    }
                    if (sbRequireProf.Length != 0) requireProfs = sbRequireProf.ToString();
                }

                FS2BuffData buff = FS2GameDataManager.TheInstance.GetBuff(itemData.Buff1Id);
                if (buff != null)
                {
                    buffStr = buff.Description.Replace("\\n", "<br />");
                }
            }

            outputHtml.Append("<table width='230px' border='0' background='../Resources/Images/FS2Objects/tooltipBack.gif'>");
			outputHtml.AppendFormat("<tr><td>{0}</td></tr>", imgStr);
            outputHtml.AppendFormat("<tr><td style='color:{0};' class='ItemTitle'>{1}<span class='ItemQuality'>{2}</span></td></tr>",
                colorStr, itemData.Name, qualityLevelStr);
            outputHtml.AppendFormat("<tr><td class='ItemDescInfo'>{0}<br /></td></tr>", itemData.Literal);
            if (itemPartStr.Length != 0)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}<br /><br /></td></tr>", itemPartStr);
            if (buffStr.Length != 0)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}<br /><br /></td></tr>", buffStr);
            if (itemData.Kind == 0 && !itemData.Repair)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}</td></tr>", StringDef.MsgCannotRepair);
            if (!itemData.Exchange)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}</td></tr>", StringDef.MsgCannotExchange);
            if (!itemData.Abandon)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}</td></tr>", StringDef.MsgCannotAbandon);
            
            outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}：{1}</td></tr>", StringDef.Weight, itemData.Weight);
            if (itemData.RequireLevel != 0) outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}：{1}</td></tr>", StringDef.RequireLevel, itemData.RequireLevel);
            if (requireProfs.Length != 0)
                outputHtml.AppendFormat("<tr><td class='ItemOtherInfo'>{0}：{1}</td></tr>", StringDef.RequireProfession, requireProfs);
            outputHtml.Append("</table>");
        }
        else
        {
            outputHtml.Append("<table width='250px' border='0' class='tooltip' background='../Resources/Images/FS2Objects/tooltipBack.gif'>");
            outputHtml.AppendFormat("<tr><td style='color:Yellow; text-align:center; font-size:14px;'>{0}</td></tr>", "没有对应的物品信息");
            outputHtml.Append("</table>");
        }
        context.Response.Output.Write(outputHtml.ToString());
    }
    
    private void ProcessOperationGetRoleSkillInfo(HttpContext context)
    {
        int skillTemplateId = int.Parse(context.Request.Params[WebConfig.ParamSkillTemplateId]);
        StringBuilder outputHtml = new StringBuilder();

        FS2SkillData skillData = FS2GameDataManager.TheInstance.GetSkillData(skillTemplateId);
        if (skillData != null)
        {
            outputHtml.Append("<table border='0' width='300px' background='../Resources/Images/FS2Objects/tooltipBack.gif'>");
            outputHtml.AppendFormat("<tr><td class='SkillInfo'>{0}</td><td align='right' class='SkillInfo' style='color:Gray'>{1}</td></tr>", skillData.Name, StringDef.Level + skillData.Level);
            outputHtml.AppendFormat("<tr><td colspan='2' class='SkillInfo'>{0}</td></tr>", skillData.Description);
            outputHtml.Append("</table>");
        }
        else
        {
            outputHtml.Append("<table width='300px' border='0' class='tooltip' background='../Resources/Images/FS2Objects/tooltipBack.gif'>");
            outputHtml.AppendFormat("<tr><td style='color:Yellow; text-align:center; font-size:14px;'>{0}</td></tr>", "没有对应的技能信息");
            outputHtml.Append("</table>");
        }
        context.Response.Output.Write(outputHtml.ToString());
    }

    private void ProcessOperationUpdateChatMsg(HttpContext context)
    {
        if (context.Request.Params[WebConfig.ParamServerId] != null)
        {
            int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
            bool original = bool.Parse(context.Request.Params["original"]);
            bool showTime = bool.Parse(context.Request.Params["showTime"]);
            if (server != null)
            {
                Queue<LordControl.GameMessage> gameMessageQueue = server.GetPlugInData((context.Session[WebConfig.SessionCurrentUser] as UserInfo).Id, LordControl.PlugInGuid, LordControl.DataKeyGameMessageQueue) as Queue<LordControl.GameMessage>;
                if (gameMessageQueue != null && gameMessageQueue.Count != 0)
                {
                    context.Response.Output.Write(GameDataRender.RenderChatMsg(gameMessageQueue, original, showTime, server.Id));
                }
            }
        }
    }

    private void ProcessOpeartionLoadSetting(HttpContext context)
    {
        if (context.Session["OriginalIbConfig"] != null)
        {
            IbConfig config = context.Session["OriginalIbConfig"] as IbConfig;
            config.CreateXml(context.Response.Output);
            context.Response.ContentEncoding = Encoding.UTF8;
            context.Response.ContentType = "text/xml";
        }
    }
    
    private void ProcessOperationLoadIbItem(HttpContext context)
    {
        if (context.Session["OriginalIbConfig"] != null)
        {
            IbConfig config = context.Session["OriginalIbConfig"] as IbConfig;
            context.Response.Output.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
            context.Response.Output.WriteLine("<itemList>");
            foreach (IbItem item in config.ItemList)
            {
                item.CreateXml(context.Response.Output);
            }
            context.Response.Output.WriteLine("</itemList>");
            context.Response.ContentEncoding = Encoding.UTF8;
            context.Response.ContentType = "text/xml";
        }
    }

    private void ProcessOperationLoadItemStyle(HttpContext context)
    {
        if (context.Session["OriginalIbConfig"] != null)
        {
            IbConfig config = context.Session["OriginalIbConfig"] as IbConfig;
            context.Response.Output.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
            context.Response.Output.WriteLine("<itemStyleList>");
            foreach (IbItemStyle itemStyle in config.ItemStyleList)
            {
                itemStyle.CreateXml(context.Response.Output);
            }
            context.Response.Output.WriteLine("</itemStyleList>");
            context.Response.ContentEncoding = Encoding.UTF8;
            context.Response.ContentType = "text/xml";
        }        
    }

    private void ProcessOperationLoadItem(HttpContext context)
    {
        context.Response.Output.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
        context.Response.Output.WriteLine("<itemList>");
        Hashtable itemTable = FS2GameDataManager.TheInstance.ItemHashtable;
        foreach (FS2ItemData item in itemTable.Values)
        {
            context.Response.Output.Write("<item templateId=\"{0}\" name=\"{1}\" />",
                item.TemplateId,
                Convert.ToBase64String(Encoding.UTF8.GetBytes(item.Name))
            );
        }
        context.Response.Output.WriteLine("</itemList>");
        context.Response.ContentEncoding = Encoding.UTF8;
        context.Response.ContentType = "text/xml";
    }
}