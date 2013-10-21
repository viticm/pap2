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
using Resources;
using FSEye.Security;
using FSEye;
using FSEye.FS2Lib;

public partial class GameInfo_TopPlayer : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoTopPlayer, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
        LabelOpMsg.Text = string.Empty;

        TableTopPlutocrat.Visible = false;
        TableTopPlayer.Visible = false;
	}

    protected void LinkButtonStatistic_Click(object sender, EventArgs e)
    {
        CreateTopPlayerList();
        CreateTopPlutocratList();
    }

	void CreateTopPlayerList()
	{
		try
		{			
			int serverId = ServerDropDownList.SelectedServerId;
			GameServer server = ServerDropDownList.SelectedGameServer;
            //需要在该游戏服务器的Execute权限
			//if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.EXECUTE, Session))
			//{
			//    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
			//}         
			if (server != null)
			{
                SqlCommand cmd = new SqlCommand("SELECT Id,AccountName,RoleName,RoleSex,RoleType,RoleLevel,SkillSeries FROM rolesfirst ORDER BY RoleLevel DESC LIMIT 10;");
                if (!server.IsConnected)
                {
                    LabelOpMsg.Text = StringDef.NoConnectionAlert;
                    return;
                }
                SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id,server, cmd);
                if (result != null && result.Success)
                {
                    result.SetFieldType(
                            new SqlDataType[] {
							SqlDataType.Int32,
							SqlDataType.String,
							SqlDataType.String,
							SqlDataType.SByte,
							SqlDataType.SByte,
							SqlDataType.SByte,
                            SqlDataType.Int32
						});

                    ArrayList roles = new ArrayList();
                    object[] record = null;
                    while ((record = result.ReadRecord()) != null)
                    {
                        FS2RoleDataInfo info = new FS2RoleDataInfo();
                        info.RoleId = (int)record[0];
                        info.AccountName = (string)record[1];
                        info.RoleName = (string)record[2];
                        info.RoleSex = (FS2RoleSex)((SByte)record[3]);
                        info.SkillSeries = (int)record[6];
                        switch ((SByte)record[4])
                        {
                            case (SByte)0:
                                if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Xuanfeng;
                                else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Xingtian;
                                else info.RoleType = FS2RoleType.Jiashi;
                                break;
                            case (SByte)1:
                                if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Zhenren;
                                else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Tianshi;
                                else info.RoleType = FS2RoleType.Daoshi;
                                break;
                            case (SByte)2:
                                if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Shoushi;
                                else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Yishi;
                                else info.RoleType = FS2RoleType.Yiren;
                                break;
                        }
                        info.RoleLevel = (SByte)record[5];

                        roles.Add(info);
                    }

                    FS2RoleDataInfo[] rolesInfo = (FS2RoleDataInfo[])roles.ToArray(typeof(FS2RoleDataInfo));
                    if (roles != null && rolesInfo.Length > 0)
                    {
                        FS2RoleDataInfo info = null;
                        for (int i = 0; i < rolesInfo.Length; i++)
                        {
                            info = rolesInfo[i];
                            TableRow row = new TableRow();

                            TableCell cell = new TableCell();
                            cell.Text = (i + 1).ToString();
                            row.Cells.Add(cell);

                            cell = new TableCell();
                            cell.Font.Bold = true;
                            cell.Text = info.RoleLevel.ToString();
                            row.Cells.Add(cell);

                            cell = new TableCell();
                            HyperLink link = new HyperLink();
                            link.Target = "_blank";
                            link.NavigateUrl = "~/GameMaster/RoleDetail.aspx?serverId=" + serverId + "&roleId=" + info.RoleId;
                            link.Text = info.RoleName;
                            link.SkinID = "PlainText";
                            cell.Controls.Add(link);
                            row.Cells.Add(cell);

                            cell = new TableCell();
                            cell.Text = info.AccountName;
                            row.Cells.Add(cell);

                            //cell = new TableCell();
                            //if (info.TongName != null && info.TongName.Length != 0)
                            //{
                            //    cell.Text = info.TongName;
                            //}
                            //else
                            //{
                            //    cell.Text = StringDef.None;
                            //}
                            //row.Cells.Add(cell);

                            cell = new TableCell();
                            Image sexImage = new Image();
                            switch (info.RoleSex)
                            {
                                case FS2RoleSex.Male:
                                    sexImage.ToolTip = StringDef.Male;
                                    sexImage.ImageUrl = "~/Image/male.gif";
                                    break;
                                case FS2RoleSex.Female:
                                    sexImage.ToolTip = StringDef.Female;
                                    sexImage.ImageUrl = "~/Image/female.gif";
                                    break;
                            }
                            cell.Controls.Add(sexImage);
                            row.Cells.Add(cell);

                            cell = new TableCell();
                            string classDescription = string.Empty;
                            switch (info.RoleType)
                            {
                                case FS2RoleType.Jiashi:
                                    classDescription = StringDef.Jiashi;
                                    break;
                                case FS2RoleType.Xuanfeng:
                                    classDescription = StringDef.XuanFeng;
                                    break;
                                case FS2RoleType.Xingtian:
                                    classDescription = StringDef.XingTian;
                                    break;
                                case FS2RoleType.Daoshi:
                                    classDescription = StringDef.Daoshi;
                                    break;
                                case FS2RoleType.Zhenren:
                                    classDescription = StringDef.ZhenRen;
                                    break;
                                case FS2RoleType.Tianshi:
                                    classDescription = StringDef.TianShi;
                                    break;
                                case FS2RoleType.Yiren:
                                    classDescription = StringDef.Yiren;
                                    break;
                                case FS2RoleType.Shoushi:
                                    classDescription = StringDef.ShouShi;
                                    break;
                                case FS2RoleType.Yishi:
                                    classDescription = StringDef.YiShi;
                                    break;
                            }
                            cell.Text = classDescription;
                            row.Cells.Add(cell);

                            TableTopPlayer.Rows.Add(row);
                        }

                        TableTopPlayer.Visible = true;

                    }
                }
                else
                {
                    if (result == null)
                        LabelOpMsg.Text = StringDef.QueryTimeOut;
                    else
                        LabelOpMsg.Text = StringDef.OperationFail;
                }
			}
		}
		catch (Exception ex)
		{
            LabelOpMsg.Text = ex.Message;
		}
	}

	void CreateTopPlutocratList()
	{
		try
		{
			int serverId = ServerDropDownList.SelectedServerId;
			GameServer server = ServerDropDownList.SelectedGameServer;
            //需要在该游戏服务器的Execute权限
            //if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.EXECUTE, Session))
            //{
            //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            //}
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            SqlCommand cmd = new SqlCommand("SELECT Id,AccountName,RoleName,RoleSex,RoleType,RoleLevel,Money,MoneyInBox,SkillSeries FROM rolesfirst ORDER BY Money+MoneyInBox DESC LIMIT 10;");
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id,server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(
                        new SqlDataType[] {
						SqlDataType.Int32,
						SqlDataType.String,
						SqlDataType.String,
						SqlDataType.SByte,
						SqlDataType.SByte,
						SqlDataType.SByte,
						SqlDataType.UInt32,
                        SqlDataType.UInt32,
                        SqlDataType.Int32
					});

                ArrayList roles = new ArrayList();
                object[] record = null;
                while ((record = result.ReadRecord()) != null)
                {
                    FS2RoleDataInfo info = new FS2RoleDataInfo();
                    info.RoleId = (int)record[0];
                    info.AccountName = (string)record[1];
                    info.RoleName = (string)record[2];
                    info.RoleSex = (FS2RoleSex)((SByte)record[3]);
                    info.SkillSeries = (int)record[8];
                    switch ((SByte)record[4])
                    {
                        case (SByte)0:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Xuanfeng;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Xingtian;
                            else info.RoleType = FS2RoleType.Jiashi;
                            break;
                        case (SByte)1:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Zhenren;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Tianshi;
                            else info.RoleType = FS2RoleType.Daoshi;
                            break;
                        case (SByte)2:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Shoushi;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Yishi;
                            else info.RoleType = FS2RoleType.Yiren;
                            break;
                    }

                    info.RoleLevel = (SByte)record[5];
                    info.Money = (uint)record[6];
                    info.MoneyInBox = (uint)record[7];

                    roles.Add(info);
                }

                FS2RoleDataInfo[] rolesInfo = (FS2RoleDataInfo[])roles.ToArray(typeof(FS2RoleDataInfo));
                if (roles != null && rolesInfo.Length > 0)
                {
                    FS2RoleDataInfo info = null;
                    for (int i = 0; i < rolesInfo.Length; i++)
                    {
                        info = rolesInfo[i];
                        TableRow row = new TableRow();

                        TableCell cell = new TableCell();
                        cell.Text = (i + 1).ToString();
                        row.Cells.Add(cell);

                        cell = new TableCell();
                        cell.Text = GameDataRender.RenderMoney(info.Money + info.MoneyInBox);
                        row.Cells.Add(cell);

                        cell = new TableCell();
                        HyperLink link = new HyperLink();
                        link.Target = "_blank";
                        link.NavigateUrl = "~/GameMaster/RoleDetail.aspx?serverId=" + serverId + "&roleId=" + info.RoleId;
                        link.Text = info.RoleName;
                        link.SkinID = "PlainText";
                        cell.Controls.Add(link);
                        row.Cells.Add(cell);

                        cell = new TableCell();
                        cell.Text = info.AccountName;
                        row.Cells.Add(cell);

                        //cell = new TableCell();
                        //if (info.TongName != null && info.TongName.Length != 0)
                        //{
                        //    cell.Text = info.TongName;
                        //}
                        //else
                        //{
                        //    cell.Text = StringDef.None;
                        //}
                        //row.Cells.Add(cell);

                        cell = new TableCell();
                        cell.Font.Bold = true;
                        cell.Text = info.RoleLevel.ToString();
                        row.Cells.Add(cell);

                        cell = new TableCell();
                        Image sexImage = new Image();
                        switch (info.RoleSex)
                        {
                            case FS2RoleSex.Male:
                                sexImage.ToolTip = StringDef.Male;
                                sexImage.ImageUrl = "~/Image/male.gif";
                                break;
                            case FS2RoleSex.Female:
                                sexImage.ToolTip = StringDef.Female;
                                sexImage.ImageUrl = "~/Image/female.gif";
                                break;
                        }
                        cell.Controls.Add(sexImage);
                        row.Cells.Add(cell);

                        cell = new TableCell();
                        string classDescription = string.Empty;
                        switch (info.RoleType)
                        {
                            case FS2RoleType.Jiashi:
                                classDescription = StringDef.Jiashi;
                                break;
                            case FS2RoleType.Xuanfeng:
                                classDescription = StringDef.XuanFeng;
                                break;
                            case FS2RoleType.Xingtian:
                                classDescription = StringDef.XingTian;
                                break;
                            case FS2RoleType.Daoshi:
                                classDescription = StringDef.Daoshi;
                                break;
                            case FS2RoleType.Zhenren:
                                classDescription = StringDef.ZhenRen;
                                break;
                            case FS2RoleType.Tianshi:
                                classDescription = StringDef.TianShi;
                                break;
                            case FS2RoleType.Yiren:
                                classDescription = StringDef.Yiren;
                                break;
                            case FS2RoleType.Shoushi:
                                classDescription = StringDef.ShouShi;
                                break;
                            case FS2RoleType.Yishi:
                                classDescription = StringDef.YiShi;
                                break;
                        }
                        cell.Text = classDescription;
                        row.Cells.Add(cell);

                        TableTopPlutocrat.Rows.Add(row);
                    }

                    TableTopPlutocrat.Visible = true;

                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
            }
		}
		catch (Exception ex)
		{
            LabelOpMsg.Text = ex.Message;
		}
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}