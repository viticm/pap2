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
using System.Text;
using Resources;
using FSEye;
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_ClanMemberStatistic : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticClanMemberStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamServerId] != null && Request.Params[WebConfig.ParamNodeGUID] != null)
            {
                int serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
                string nodeGuid = Request.Params[WebConfig.ParamNodeGUID];
                FileNode(serverId, nodeGuid);
                //Query(serverId, nodeGuid);
            }
        }

        LabelOpMsg.Text = string.Empty;
    }

    ICollection CreateDataSource(IList roleList, int serverId)
    {
        if (roleList == null || roleList.Count == 0) return null;
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;
            dataTable.Columns.Add(new DataColumn("RoleId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("RoleName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("RoleAccount", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Level", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("Sex", typeof(String)));
            dataTable.Columns.Add(new DataColumn("RoleClass", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ServerId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("NoChatInGame", typeof(String)));
            dataTable.Columns.Add(new DataColumn("NoLoginGame", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Online", typeof(String)));

            foreach (FS2RoleDataInfo roleInfo in roleList)
            {
                dataRow = dataTable.NewRow();

                dataRow[0] = roleInfo.RoleId;
                dataRow[1] = roleInfo.RoleName;
                dataRow[2] = roleInfo.AccountName;
                dataRow[3] = roleInfo.RoleLevel;
                dataRow[4] = roleInfo.RoleSex == FS2RoleSex.Male ? StringDef.Male : StringDef.Female;
                string classDescription = string.Empty;
                switch (roleInfo.RoleType)
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
                dataRow[5] = classDescription;
                dataRow[6] = serverId;
                dataRow[7] = roleInfo.NoChatIn == 0 ? StringDef.No : StringDef.Yes;
                dataRow[8] = roleInfo.NoLoginIn == 0 ? StringDef.No : StringDef.Yes;
                dataRow[9] = roleInfo.Online == 0 ? StringDef.No : StringDef.Yes;

                dataTable.Rows.Add(dataRow);
            }

            DataView dataView = new DataView(dataTable);
            return dataView;
        }
        catch (Exception)
        {
            return null;
        }
    }

    private ICollection CreateDataSource(SqlResult result, int serverId)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;
            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Type", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Leader", typeof(String)));
            dataTable.Columns.Add(new DataColumn("NodeGUID", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ChildCount", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("ServerId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("RoleNameEncoded", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
            SqlDataType.String,
            SqlDataType.String,
            SqlDataType.Int32,
            SqlDataType.String,
            SqlDataType.Int32
            });

            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                dataRow = dataTable.NewRow();

                dataRow[0] = record[0] as string;
                string typeText = string.Empty;
                switch ((int)record[2])
                {
                    case 2:
                        typeText = StringDef.Clan;
                        break;
                    case 3:
                        typeText = StringDef.Seigneur;
                        break;
                    case 4:
                        typeText = StringDef.Nation;
                        break;
                }
                dataRow[1] = typeText;
                dataRow[2] = record[1] as string;
                dataRow[3] = record[3] as string;
                dataRow[4] = (int)record[4];
                dataRow[5] = serverId;
                dataRow[6] = Server.UrlEncode(record[1] as string);

                dataTable.Rows.Add(dataRow);
            }
            return new DataView(dataTable);

        }
        catch (Exception)
        {
            return null;
        }
    }

    private void FileNode(int serverId,string nodeGuid)
    {
        GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
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

        ArrayList paramList = new ArrayList();
        string cmdText = "SELECT NodeName,LeaderName,LayerId,NodeGUID,ChildCount FROM socialrelation WHERE {0}='?'";
        paramList.Add(nodeGuid);

        SqlCommand cmd = new SqlCommand(string.Format(cmdText, FS2TableString.SocialrelationFieldNodeGUID), paramList.ToArray());
        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
        if (result != null && result.Success)
        {
            result.SetFieldType(new SqlDataType[]{
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.Int32,
                SqlDataType.String,
                SqlDataType.Int32            
            });

            object[] record;
            SocialRelationInfo socialRelationInfo = null;
            while ((record = result.ReadRecord()) != null)
            {
                socialRelationInfo = new SocialRelationInfo();
                
                socialRelationInfo.NodeName = record[0] as string;
                socialRelationInfo.LeaderName = record[1] as string;
                socialRelationInfo.LayerId = (int)record[2];
                socialRelationInfo.NodeGUID = record[3] as string;
                socialRelationInfo.ChildCount = (int)record[4];
            }
            if (socialRelationInfo != null)
            {
                switch (socialRelationInfo.LayerId)
                {
                    case 2:
                        //氏族显示成员(Role)
                        LabelTitle.Text = string.Format("{0}[{1}] {2}:{3}", StringDef.Clan, socialRelationInfo.NodeName, StringDef.MemberCount, socialRelationInfo.ChildCount);
                        QueryRole(serverId, nodeGuid);
                        break;
                    case 3:
                    case 4:
                        //诸侯和国家
                        LabelTitle.Text = string.Format("{0}[{1}] {2}:{3}", socialRelationInfo.LayerId == 3 ? StringDef.Seigneur : StringDef.Nation, socialRelationInfo.NodeName, StringDef.MemberCount, socialRelationInfo.ChildCount);
                        QueryNode(serverId, nodeGuid);
                        break;
                }
            }
        }
    }

    void QueryNode(int serverId, string nodeGuid)
    {
        try
        {
            GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
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

            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();
            string cmdText = "SELECT NodeName,LeaderName,LayerId,NodeGUID,ChildCount FROM socialrelation {0}";

            searchCondition.AppendFormat("WHERE {0}='1' AND {1}='?'", FS2TableString.SocialrelationFieldTemplateId, FS2TableString.SocialrelationFieldParentGUID);
            paramList.Add(nodeGuid);

            SqlCommand cmd = new SqlCommand(string.Format(cmdText, searchCondition.ToString()), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                DataGridResult.DataSource = CreateDataSource(result, serverId);
                if ((DataGridResult.DataSource as DataView).Table.Rows.Count == 0)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                    DataGridResult.Visible = false;
                    return;
                }
                else
                {                    
                    DataGridResult.DataBind();
                    DataGridResult.Visible = true;
                }
            }
            else
            {
                LabelOpMsg.Text = StringDef.Query + StringDef.Failure;
                DataGridResult.Visible = false;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            DataGridResult.Visible = false;
        }
    }

    void QueryRole(int serverId, string NodeGUID)
    {
        try
        {
            GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
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

            ArrayList paramList = new ArrayList();
            string cmdText = "SELECT Id,AccountName,RoleName,RoleSex,RoleType,RoleLevel,SkillSeries,NoChatIn,NoLoginIn,Online FROM rolesfirst WHERE {0}='?' ORDER BY RoleName";
            paramList.Add(NodeGUID);

            SqlCommand cmd = new SqlCommand(string.Format(cmdText, FS2TableString.RolesfirstFieldTongGUID), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);

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
                        SqlDataType.Int32,
                        SqlDataType.Int32,
                        SqlDataType.Int32,
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
                    info.RoleSex = (FS2RoleSex)(SByte)record[3];
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
                    info.NoChatIn = (int)record[7];
                    info.NoLoginIn = (int)record[8];
                    info.Online = (int)record[9];

                    roles.Add(info);
                }

                DataGridRole.DataSource = CreateDataSource(roles, server.Id);
                DataGridRole.DataBind();

                if (roles == null || roles.Count != 0)
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;                
            }
            else
            {
                LabelOpMsg.Text = StringDef.Query + StringDef.Failure;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
        }
    }
}

public class SocialRelationInfo
{
    public string NodeGUID;

    public int LayerId;

    public string NodeName;

    public string LeaderName;

    public int ChildCount;

    public SocialRelationInfo()
    { }
}