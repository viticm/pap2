using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Collections.Generic;
using System.Text;
using Resources;
using FSEye.FS2Lib;
using FSEye;
using FSEye.Security;


public partial class GameMaster_RoleSkillInfo : BasePage
{
    int _serverId;
    int _roleId;

    protected void Page_Init(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterRoleSkillInfo, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (Request.Params[WebConfig.ParamServerId] != null)
        {
            _serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
            if (Request.Params[WebConfig.ParamRoleId] != null)
            {
                _roleId = int.Parse(Request.Params[WebConfig.ParamRoleId]);
            }

            if (!IsPostBack)
            {
                GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(_serverId);
                if (server == null) return;
                FS2RoleDataInfo info = Query(server, _roleId);
                if (info != null)
                {
                    LabelTitle.Text += string.Format("[{0}]", info.RoleName) + StringDef.SkillInfo;                    
                    RenderAll(info);
                    Session["RoleDataInfo"] = info;
                }
            }
        }
        else
        {
            Response.Redirect("QueryRole.aspx");
        }
        LabelOpMsg.Text = string.Empty;
    }

    protected FS2RoleDataInfo Query(GameServer server, int roleId)
    {
        FS2RoleDataInfo roleInfo = null;
        ArrayList paramList = new ArrayList();
        paramList.Add(roleId);
        string selectedFiled = string.Format("{0},{1}",
            FS2TableString.RolesfirstFieldSkillList, FS2TableString.RolesfirstFieldRoleName);
        string sqlCmdText = string.Format("SELECT {0} FROM {1} WHERE {2}='?';", selectedFiled,
            FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldId);
        SqlCommand cmd = new SqlCommand(sqlCmdText, paramList.ToArray());
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return null;
        }
        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
        if (result != null && result.Success)
        {
            result.SetFieldType(new SqlDataType[]{
                SqlDataType.Blob,
                SqlDataType.String
                });
            object[] record;

            byte[] skillData;

            roleInfo = new FS2RoleDataInfo();
            while ((record = result.ReadRecord()) != null)
            {
                if (record != null)
                {
                    skillData = (byte[])record[0];
                    if (skillData != null)
                    {
                        roleInfo.SkillList = FS2SkillDataInfo.Parse(skillData);
                    }
                    roleInfo.RoleName = record[1] as string;
                }
            }
        }
        return roleInfo;
    }

    private void RenderAll(FS2RoleDataInfo roleInfo)
    {
        StringBuilder content = new StringBuilder();
        content.Append("<table class='TN' >");
        for (int index = 0; index != roleInfo.SkillList.Count;++index )
        {
            FS2SkillDataInfo skillInfo = roleInfo.SkillList[index] as FS2SkillDataInfo;
            if (skillInfo == null) return;

            FS2SkillData skillData = FS2GameDataManager.TheInstance.GetSkillData(skillInfo.SkillId);
            if (skillData != null)
            {
                //31,32,33为普通攻击，忽略。
                if (skillData.TemplateId == 31 || skillData.TemplateId == 32 || skillData.TemplateId == 33) continue;
                //SkillSeries为0的是主技能，如异术、玄女等。
                if (skillData.SkillSeries == 0)
                {
                    content.AppendFormat("<tr>");
                    content.AppendFormat("<td class='TCH' align='center'>{0}</td>", skillData.Name);
                    IList<int> subSkills = FS2GameDataManager.TheInstance.GetSubSkill(skillData.TemplateId);
                    if (subSkills != null)
                    {
                        foreach (FS2SkillDataInfo info in roleInfo.SkillList)
                        {
                            if (subSkills.Contains(info.SkillId))
                            {
                                //技能列表中技能包含在此主技能中则显示
                                FS2SkillData subSkillData = FS2GameDataManager.TheInstance.GetSkillData(info.SkillId);
                                //Level用从RoleInfo里面解析出来的
                                if (subSkillData != null) content.Append(GameDataRender.RenderSkillCell(subSkillData.Name, info.SkillLevel.ToString(), string.Empty, string.Format("updateRoleSkillInfo(\"{0}\")", subSkillData.TemplateId)));
                            }
                        }
                    }
                    content.AppendFormat("</tr>");
                }
            }
        }
        content.Append("</table>");
        DivSkillInfo.InnerHtml = content.ToString();
    }
}