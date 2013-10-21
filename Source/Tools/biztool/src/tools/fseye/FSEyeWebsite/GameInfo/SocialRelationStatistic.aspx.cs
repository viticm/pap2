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

public partial class GameInfo_SocialRelationStatistic : BasePage
{
    const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticSocialRelationStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
        else
        {
            try
            {
                _recordPerPage = int.Parse(ListBoxRecordPerPage.SelectedValue);
            }
            catch (Exception)
            {
                _recordPerPage = DefaultRecordPerPage;
            }
        }

        PanelResult.Visible = false;
        LabelOpMsg.Text = string.Empty;
    }

    protected void LinkButtonQuery_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    private ICollection CreateDataSource(SqlResult result)
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
            dataTable.Columns.Add(new DataColumn("ViewParent", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
            SqlDataType.String,
            SqlDataType.String,
            SqlDataType.Int32,
            SqlDataType.String,
            SqlDataType.Int32,
            SqlDataType.String
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
                dataRow[5] = ServerDropDownList.SelectedServerId;
                dataRow[6] = Server.UrlEncode(record[1] as string);
                string parentGuid = record[5] as string;
                if (parentGuid != null && parentGuid.Length != 0)
                {
                    dataRow[7] = string.Format("<a href='../GameInfo/ClanMemberStatistic.aspx?serverId={0}&nodeGuid={1}' target='_blank' >{2}</a>",
                        ServerDropDownList.SelectedServerId,
                        parentGuid,
                        StringDef.OrganizationInfo);
                }
                else
                {
                    dataRow[7] = StringDef.NotAvailable;
                }

                dataTable.Rows.Add(dataRow);
            }
            return new DataView(dataTable);

        }
        catch (Exception)
        {
            return null;
        }
    }

    void Query(int offset)
    {
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
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
            ViewState[WebConfig.ParamServerId] = server.Id;

            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();
            string cmdText = "SELECT NodeName,LeaderName,LayerId,NodeGUID,ChildCount,ParentGUID FROM socialrelation {0} {1}";

            searchCondition.AppendFormat("WHERE {0}='1' ",FS2TableString.SocialrelationFieldTemplateId);
            if (DropDownListType.SelectedIndex != 0) 
                searchCondition.AppendFormat(" AND {0}='{1}'", FS2TableString.SocialrelationFieldLayerId, DropDownListType.SelectedValue);

            string name = TextBoxName.Text.Trim();
            if (name.Length > 0)
            {
                if (CheckBoxName.Checked)
                {
                    searchCondition.AppendFormat(" AND {0}='?'", FS2TableString.SocialrelationFieldNodeName);
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0} LIKE '%?%'", FS2TableString.SocialrelationFieldNodeName);
                }
                paramList.Add(name);
            }

            string leader = TextBoxLeaderName.Text.Trim();
            if (leader.Length > 0)
            {
                if (CheckBoxLeaderName.Checked)
                {
                    searchCondition.AppendFormat(" AND {0}='?'", FS2TableString.SocialrelationFieldLeaderName);
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0} LIKE '%?%'", FS2TableString.SocialrelationFieldLeaderName);
                }
                paramList.Add(leader);
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

            SqlCommand cmd = new SqlCommand(string.Format(cmdText, searchCondition.ToString(),limitStatement),paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                DataGridResult.DataSource = CreateDataSource(result);
                int rowCount = (DataGridResult.DataSource as DataView).Table.Rows.Count;
                if (rowCount == 0)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                    PanelResult.Visible = false;
                    return;
                }
                else
                {
                    ViewState[WebConfig.SessionQueryLogOffset] = offset;

                    ButtonPreviousPage.Enabled = (offset > 0);
                    ButtonFirstPage.Enabled = (offset > 0);
                    ButtonNextPage.Enabled = (rowCount >= limitCount);

                    PanelResult.Visible = true;
                    DataGridResult.DataBind();
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

    protected void ButtonPage_Click(object sender, EventArgs e)
    {
        Button bt = sender as Button;
        if (bt != null)
        {
            int offset = (int)ViewState[WebConfig.SessionQueryLogOffset];
            switch (bt.CommandName)
            {
                case "FirstPage":
                    offset = 0;
                    break;
                case "PreviousPage":
                    offset -= _recordPerPage;
                    break;
                case "NextPage":
                    offset += _recordPerPage;
                    break;
            }
            Query(offset);
        }
    }

    //void QueryChild(int serverId,string NodeGUID)
    //{
    //    try
    //    {
    //        GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
    //        if (server == null)
    //        {
    //            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
    //            return;
    //        }
    //        if (!server.IsConnected)
    //        {
    //            LabelOpMsg.Text = StringDef.NoConnectionAlert;
    //            return;
    //        }            

    //        ArrayList paramList = new ArrayList();
    //        StringBuilder searchCondition = new StringBuilder();
    //        string cmdText = "SELECT NodeName,LeaderName,LayerId,NodeGUID,ChildCount FROM socialrelation {0}";

    //        searchCondition.AppendFormat("WHERE {0}='1' AND {1}='?'", FS2TableString.SocialrelationFieldTemplateId,FS2TableString.SocialrelationFieldParentGUID);
    //        paramList.Add(NodeGUID);

    //        SqlCommand cmd = new SqlCommand(string.Format(cmdText, searchCondition.ToString()), paramList.ToArray());
    //        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
    //        if (result != null && result.Success)
    //        {
    //            DataGridResult.DataSource = CreateDataSource(result);
    //            if ((DataGridResult.DataSource as DataView).Table.Rows.Count == 0)
    //            {
    //                LabelOpMsg.Text = StringDef.NoMatchingRecord;
    //                DataGridResult.Visible = false;
    //                return;
    //            }
    //            else
    //            {
    //                DataGridResult.DataBind();
    //                DataGridResult.Visible = true;
    //            }
    //        }
    //        else
    //        {
    //            LabelOpMsg.Text = StringDef.Query + StringDef.Failure;
    //            DataGridResult.Visible = false;
    //        }
    //    }
    //    catch (Exception ex)
    //    {
    //        LabelOpMsg.Text = ex.Message;
    //        DataGridResult.Visible = false;
    //    }
    //}

    //protected void DataGridResult_ItemCommand(object sender, DataGridCommandEventArgs e)
    //{
    //    switch (e.CommandName)
    //    {
    //        case "MemberInfo":
    //            if (e.Item.Cells[1].Text.Equals(StringDef.Clan))
    //            {
    //                //氏族处理
    //                int serverId = (int)ViewState[WebConfig.ParamServerId];
    //                string nodeGuid = e.Item.Cells[5].Text;
    //                Response.Redirect(string.Format("~/GameInfo/ClanMemberStatistic.aspx?{0}={1}&{2}={3}", WebConfig.ParamServerId, serverId, WebConfig.ParamNodeGUID, nodeGuid), false);
    //            }
    //            else
    //            {
    //                //诸侯和国家处理
    //                int serverId = (int)ViewState[WebConfig.ParamServerId];
    //                string nodeGuid = e.Item.Cells[5].Text;
    //                LabelResult.Text = e.Item.Cells[1].Text + string.Format("[{0}]", e.Item.Cells[0].Text) + StringDef.MemberInfo;
    //                QueryChild(serverId, nodeGuid);                    
    //            }
    //            break;
    //    }
    //}
}