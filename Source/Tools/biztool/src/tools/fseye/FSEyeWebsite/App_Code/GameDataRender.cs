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
using System.Collections.Generic;
using System.Text;
using Resources;
using FSEye.FS2Lib;
using FSEye.PlugIn;
using FSEye;


/// <summary>
/// Summary description for GameDataRender
/// </summary>
public class GameDataRender
{
    public const string Global = "global";
    public const string System = "system";
    public const string ImagePathPrefix = "../Resources/Images/FS2Objects/";

    public static string RenderItemCell(string count, string bgImg, string mouseOverEvent,string clickEvent)
    {        
        if (bgImg == string.Empty) bgImg = "../Resources/Images/FS2Objects/itemImg.gif";
        else bgImg = ImagePathPrefix + bgImg;
        string styleStr = string.Format("style='background-image:url(\"{0}\");'", bgImg);
        StringBuilder itemHtml = new StringBuilder();
        itemHtml.AppendFormat("<td width='38px' align='center' class='ItemCell' onmouseover='{0}' {1} {3}>{2}</td>",
            mouseOverEvent, styleStr, count == "1" ? string.Empty : count,clickEvent);
        return itemHtml.ToString();
    }

    public static string RenderSkillCell(string name, string level, string skillImg, string mouseOverEvent)
    {
        if (skillImg == string.Empty) skillImg = "../Resources/Images/FS2Objects/skillImg.gif";
        else skillImg = ImagePathPrefix + "skill/" + skillImg;
        StringBuilder skillHtml = new StringBuilder();
        skillHtml.Append("<table width='100%' height='100%' background='../Resources/Images/FS2Objects/skillCell.gif'><th rowSpan='2'>");
        skillHtml.AppendFormat("<img src={0} width='30px' align='left' onmouseover='{1}' style='padding-left:1px;' />", skillImg, mouseOverEvent);
        skillHtml.AppendFormat("</th><td align='left'>{0}</td>", name);
        skillHtml.AppendFormat("<tr><td align='left'>{0}:{1}</td></tr></table>", StringDef.Level, level);
        return string.Format("<td width='101px'>{0}</td>", skillHtml.ToString());
    }

    public static string RenderMoney(uint money)
    {
        StringBuilder stringBuilder = new StringBuilder();
        uint gold = money / 10000;
        if (gold != 0)
            stringBuilder.AppendFormat("{0}<img src='../Image/Gold.gif' />", gold);
        uint silver = money % 10000 / 100;
        if (gold != 0 || silver != 0)
            stringBuilder.AppendFormat("{0}<img src='../Image/Silver.gif' img>", silver);
        uint copper = money % 100;
        stringBuilder.AppendFormat("{0}<img src='../Image/Copper.gif' />", copper);
        return stringBuilder.ToString();
    }

    /// <summary>
    /// 公聊消息解析
    /// </summary>
    /// <param name="msgQueue">消息队列</param>
    /// <param name="keepOriginal">原始数据/解析数据开关</param>
    /// <returns></returns>
    public static string RenderChatMsg(Queue<LordControl.GameMessage> msgQueue, bool keepOriginal, bool showTime, int serverId)
    {
        StringBuilder chatHtml = new StringBuilder();
        lock (msgQueue)
        {
            foreach (LordControl.GameMessage msg in msgQueue)
            {
                //Util.DebugLog(string.Format("Received Message:[{0}] to [{1}] [{2}]", msg.Sender, msg.Receiver, msg.Message));
                string originalData = null;
                int subIndex = 0;

                if (showTime)
                {
                    chatHtml.AppendFormat("[{0}]", msg.Time.ToString("HH:mm:ss"));
                }

                string textCss = string.Empty;
                switch (msg.Sender)
                {
                    case System:
                        chatHtml.AppendFormat("<span class='{0}'>[{1}]:</span>",
                            CssConfig.ChatMsgSys,
                            StringDef.System
                        );
                        textCss = CssConfig.ChatMsgSys;
                        break;
                    default:
                        switch (msg.Receiver.ToLower())
                        {
                            case Global:
                                string roleStr = string.Format("<a href='../GameMaster/RoleDetail.aspx?{0}={1}&{2}={3}' target='_blank' class={4}>{5}</a>",
                                    WebConfig.ParamServerId, serverId, WebConfig.ParamRoleName, HttpUtility.UrlEncode(msg.Sender), CssConfig.ChatMsgRole, msg.Sender);
                                chatHtml.AppendFormat("[{0}][{1}]:", StringDef.World, roleStr);
                                break;
                            default:
                                continue;
                        }
                        break;
                }
                
                if (keepOriginal)
                {
                    chatHtml.AppendFormat("{0}", msg.Message.Replace("<", "&lt;"));
                }
                else
                {
                    try
                    {
                        //解析Message
                        int index = 0;
                        while (index < msg.Message.Length)
                        {
                            if (msg.Message[index] == '<')
                            {
                                subIndex = msg.Message.IndexOf('>', index);
                                originalData = msg.Message.Substring(index + 3, subIndex - index - 3);
                                switch (msg.Message[index + 1])
                                {
                                    case 'N':
                                        chatHtml.Append(RenderChatText(originalData,textCss));
                                        break;
                                    case 'I':
                                        chatHtml.Append(RenderChatItem(originalData));
                                        break;
                                    case 'F':
                                        chatHtml.Append(RenderChatFace(originalData));
                                        break;
                                    case 'P':
                                        chatHtml.Append(RenderChatPlace(originalData));
                                        break;
                                    default:
                                        break;
                                }
                                index = subIndex + 1;
                            }
                            else
                            {
                                //没有以'<'开头则默认为文字
                                chatHtml.Append(RenderChatText(msg.Message[index].ToString(), textCss));
                                ++index;
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        chatHtml.Append(e.Message);
                    }
                }
                chatHtml.Append("<br />");
            }
        }
        return chatHtml.Length == 0 ? string.Empty : chatHtml.ToString();
    }

    public static string RenderChatItem(string originalData)
    {
        try
        {
            string[] datas = originalData.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);
            int hashId = int.Parse(datas[0]);

            int kind = (hashId & 0x7ff00000) >> 20;
            int special = (hashId & 0xffc00) >> 10;
            int detail = hashId & 0x3ff;
            int min = int.Parse(datas[1]);

            string returnStr = string.Empty;
            FS2ItemData item = FS2GameDataManager.TheInstance.GetItemData(string.Concat(kind, ',', special, ',', detail, ',', min));
            if (item == null)
            {
                returnStr = datas[3];
            }
            else
            {
                string colorStr = string.Empty;
                switch (item.Quality)
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
                returnStr = string.Format("<span onclick='{0}' style='color:{1};cursor:hand;'>{2}</span>",
                                            string.Format("updateRoleItemInfo(\"{0}\")", item.TemplateId),
                                            colorStr,
                                            datas[3]);
            }

            return returnStr;
        }
        catch (Exception)
        {
            return originalData;
        }
    }

    public static string RenderChatFace(string originalData)
    {
        //先用默认表情
        string returnStr = string.Format("<img src='{0}'></img>", ImagePathPrefix + "Face/default.gif");
        //string returnStr = string.Format("<img src='{0}'></img>", ImagePathPrefix + "Face/" + originalData + ".gif");
        return returnStr;
    }

    public static string RenderChatText(string originalData,string textCss)
    {
        //替换ASCII中的1和2为'<'和'>'。
        originalData = originalData.Replace((char)1, '<');
        originalData = originalData.Replace((char)2, '>');
        originalData = originalData.Replace("<", "&lt;");
        if (textCss != null && textCss.Length != 0)
            return string.Format("<span class='{0}'>{1}</span>", textCss, originalData);
        else
            return originalData;
    }

    public static string RenderChatPlace(string originalData)
    {
        string[] datas = originalData.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);
        if (datas.Length > 3)
            return datas[3];
        else
            return originalData;
    }
}