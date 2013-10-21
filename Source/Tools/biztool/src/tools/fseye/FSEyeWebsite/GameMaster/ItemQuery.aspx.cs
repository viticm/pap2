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
using FSEye.Security;
using FSEye.FS2Lib;
using JX2Lib;
using FSEye;
using FSEye.PlugIn;

public partial class GameMaster_ItemQuery : BasePage
{
    
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (Request.Params[WebConfig.ParamRoleGUID] != null && Request.Params[WebConfig.serverID] != null && Request.Params[WebConfig.ParamcRoleName] != null)
            {
                TextBoxcRoleName.Text = Request.Params[WebConfig.ParamcRoleName].Trim();
                DropDownListGameCenter.SelectedValue = Request.Params[WebConfig.serverID].Trim();
            }
        }
    }

   
    protected void LinkButtonItemQuery_Click(object sender, EventArgs e)
    {
        if (Request.Params[WebConfig.ParamRoleGUID] != null && Request.Params[WebConfig.serverID] != null && Request.Params[WebConfig.ParamcRoleName] != null)
        {
            if ((TextBoxcRoleName.Text == Request.Params[WebConfig.ParamcRoleName]) && (DropDownListGameCenter.SelectedValue == Request.Params[WebConfig.serverID]))
            {
                BindData(Request.Params[WebConfig.ParamRoleGUID], int.Parse(Request.Params[WebConfig.serverID]), Request.Params[WebConfig.ParamcRoleName]);
            }
            else
            {
                itemquery();
            }
        }
        else
        {
            itemquery();
        }
    }

    private void itemquery()
    {
        string queryString;
        ArrayList paramList = new ArrayList();
        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(int.Parse(this.DropDownListGameCenter.SelectedValue));

        //获得RoleId
        queryString = "select cAccountName, cRoleId from trolebaseinfo where cRoleName = '" + this.TextBoxcRoleName.Text + "'";

        SqlCommand rolecmd = new SqlCommand(queryString, paramList.ToArray());
        SqlResult roleResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, rolecmd);
        object[] roleRecord = null;
        if (roleResult != null && roleResult.Success)
        {
            roleResult.SetFieldType(
                    new SqlDataType[] {
                        SqlDataType.String,//物品所属帐号名
						SqlDataType.String,//cRoleId
                        });

            if ((roleRecord = roleResult.ReadRecord()) != null)
            {
                string accountName = (string)roleRecord[0];
                string roleID = (string)roleRecord[1];
                if (roleID != null)
                {
                    BindData(roleID, server.Id, TextBoxcRoleName.Text);
                }
            }
            else
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = "不存在该角色";
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
                return;
            }
        }
        else
        {
            if (roleResult == null)
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
            }
            else
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = StringDef.OperationFail;
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
            }
        }





    }

    private void BindData(string roleID, int serverID, string roleName)
    {
        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverID);
        
        ArrayList paramList = new ArrayList();
        ArrayList items = new ArrayList();
        DataTable dt = new DataTable();
        dt.Columns.Add(new DataColumn("GoodsID", typeof(String)));
        dt.Columns.Add(new DataColumn("GoodsName", typeof(String)));
        dt.Columns.Add(new DataColumn("LogTime", typeof(String)));
        dt.Columns.Add(new DataColumn("EventName", typeof(String)));
        dt.Columns.Add(new DataColumn("Note", typeof(String)));
        dt.Columns.Add(new DataColumn("serverID", typeof(Int32)));
        string goodsName = this.TextBoxcItemName.Text;
         string beginTime="";
         string endTime="";
        if(RecentTime_start.SelectedDate!=DateTime.MaxValue&&RecentTime_end.SelectedDate!=DateTime.MinValue){
            beginTime = RecentTime_start.SelectedDate.ToString();
            endTime = RecentTime_end.SelectedDate.ToString();
        }
        else{
            beginTime=DateTime.MinValue.ToString();
            endTime=DateTime.MaxValue.ToString();
        }
        string queryString = "select distinct Field1 as GoodsID, Field30 as GoodsName "
        + " from log_4_1"
        + " where Field1 in"
        + " ( "
        + " select distinct Field1 as GoodsID"
        + " from log_4_1"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " and Field30 like CONCAT('%','" + goodsName + "','%')"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_2"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_3"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "' "
        + " union"
        + " select distinct Field1 as GoodsID"
        + " from log_4_4"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_5"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and (Field2 = '" + roleID + "' || Field3 = '" + roleID + "') "
        + " )"
        + " and Field30 like CONCAT('%','" + goodsName + "','%')"
        + " union "
        + "select distinct Field1 as GoodsID, Field30 as GoodsName "
        + " from log_4_6"
        + " where Field1 in"
        + " ( "
        + " select distinct Field1 as GoodsID"
        + " from log_4_6"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " and Field30 like CONCAT('%','" + goodsName + "','%')"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_2"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_3"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "' "
        + " union"
        + " select distinct Field1 as GoodsID"
        + " from log_4_4"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and Field2 = '" + roleID + "'"
        + " union "
        + " select distinct Field1 as GoodsID"
        + " from log_4_5"
        + " where logtime between '" + beginTime + "' and '" + endTime + "'"
        + " and (Field2 = '" + roleID + "' || Field3 = '" + roleID + "') "
        + " )"
        + " and Field30 like CONCAT('%','" + goodsName + "','%')";

        SqlCommand itemcmd = new SqlCommand(queryString, paramList.ToArray());

        SqlResult itemResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, itemcmd);


        if (itemResult != null && itemResult.Success)
        {

            itemResult.SetFieldType(
                    new SqlDataType[] {
                                SqlDataType.String,//物品GUID
						        SqlDataType.String,//物品名
                                //  SqlDataType.String,//logtime
                                //  SqlDataType.String,//事件
                                //SqlDataType.String,//物品描述
                                }
            );

            //做物品结果列表了

            object[] itemRecord = null;

            DataRow dr;

            while ((itemRecord = itemResult.ReadRecord()) != null)
            {
                dr = dt.NewRow();
                dr[0] = (String)itemRecord[0];
                dr[1] = (String)itemRecord[1];
                //dr[2] = (String)itemRecord[2];
                //dr[3] = (String)itemRecord[3];
                //dr[4] = (String)itemRecord[4];
                dr[5] = (Int32)server.Id;
                dt.Rows.Add(dr);
            }

            if (dt.Rows.Count == 0)
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = "没有满足条件的记录";
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
                return;                
            }

            DataGridItemResult.Enabled = true;
            DataGridItemResult.Visible = true;
            DataGridItemResult.DataSource = dt.DefaultView;
            DataGridItemResult.DataBind();
            
            LabelOpMsg.Visible = false;
            LabelSuccess.Text = "查询成功完成";
            LabelSuccess.Visible = true;
            ItemPanelResult.Visible = true;

        }
        else
        {
            if (itemResult == null)
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
            }
            else
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = StringDef.OperationFail;
                LabelOpMsg.Visible = true;
                ItemPanelResult.Visible = false;
            }

        }
    }

}
