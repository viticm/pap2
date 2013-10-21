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
using FSEye;
using FSEye.Security;

public partial class GameServer_DatabaseInfo : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        //页面权限判断
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerDatabaseInfo, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamServerId] != null)
            {
                ViewState[WebConfig.ParamServerId] = int.Parse(Request.Params[WebConfig.ParamServerId]);
            }
            else
            {
                Response.Redirect("ServerList.aspx", true);
            }
            //服务器权限判断
            if (!WebUtil.CheckPrivilege((int)ViewState[WebConfig.ParamServerId], OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            ShowProcessList();
        }
    }

    private ICollection CreateVariableDataSource(SqlResult result)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("VariableName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("VariableValue", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
                SqlDataType.String,
                SqlDataType.String
            });

            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = record[0] as string;
                dataRow[1] = record[1] as string;

                dataTable.Rows.Add(dataRow);
            }
            ViewState["Variables"] = dataTable;
            return new DataView(dataTable);
        }
        catch (Exception)
        {
            return null;
        }
    }

    private ICollection CreateProcessListDataSource(SqlResult result)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("User", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Host", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Database", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Command", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Time", typeof(String)));
            dataTable.Columns.Add(new DataColumn("State", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Info", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
                SqlDataType.Int32,
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.String,
                SqlDataType.String
            });
            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = record[1] as string;
                dataRow[1] = record[2] as string;
                dataRow[2] = record[3] as string;
                dataRow[3] = record[4] as string;
                dataRow[4] = record[5] as string;
                dataRow[5] = record[6] as string;
                dataRow[6] = record[7] as string;

                dataTable.Rows.Add(dataRow);
            }
            ViewState["ProcessList"] = dataTable;
            return new DataView(dataTable);
        }
        catch (Exception)
        {
            return null;
        }
    }

    private ICollection CreateStateDataSource(SqlResult result)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("StateName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("StateValue", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
                SqlDataType.String,
                SqlDataType.String
            });

            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = record[0] as string;
                dataRow[1] = record[1] as string;

                dataTable.Rows.Add(dataRow);
            }
            ViewState["State"] = dataTable;
            return new DataView(dataTable);
        }
        catch (Exception)
        {
            return null;
        }
    }

    private ICollection CreateUserDataSource(SqlResult result)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("UserName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("UserValue", typeof(String)));

            result.SetFieldType(new SqlDataType[]{
                SqlDataType.String,
            });

            object[] record;
            while ((record = result.ReadRecord()) != null)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = record[0] as string;
                
                dataTable.Rows.Add(dataRow);
            }
            ViewState["User"] = dataTable;
            return new DataView(dataTable);
        }
        catch (Exception)
        {
            return null;
        }
    }

    private void ShowVariables()
    {
        try
        {
            int serverId = (int)ViewState[WebConfig.ParamServerId];
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
            string cmdText = "Show variables;";
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(cmdText, null));
            if (result != null && result.Success)
            {
                DataGridVariables.DataSource = CreateVariableDataSource(result);
                DataGridVariables.DataBind();
            }
            else
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
        }
        catch (Exception)
        {
        }
    }

    private void ShowProcessList()
    {
        try
        {
            int serverId = (int)ViewState[WebConfig.ParamServerId];
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
            string cmdText = "Show processlist;";
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(cmdText, null));
            if (result != null && result.Success)
            {
                DataGridProcessList.DataSource = CreateProcessListDataSource(result);
                DataGridProcessList.DataBind();
            }
            else
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
        }
        catch (Exception)
        {

        }
    }

    private void ShowUsers()
    {
        try
        {
            int serverId = (int)ViewState[WebConfig.ParamServerId];
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
            string cmdText = "SELECT current_user;";
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(cmdText, null));
            if (result != null && result.Success)
            {
                DataGridUser.DataSource = CreateUserDataSource(result);
                DataGridUser.DataBind();
            }
            else
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
        }
        catch (Exception)
        {
        }
    }

    private void ShowStatus()
    {
        try
        {
            int serverId = (int)ViewState[WebConfig.ParamServerId];
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
            string cmdText = "Show status;";
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(cmdText, null));
            if (result != null && result.Success)
            {
                DataGridState.DataSource = CreateStateDataSource(result);
                DataGridState.DataBind();
            }
            else
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
        }
        catch (Exception)
        {
        }
    }

    protected void DataGridProcessList_PageIndexChanged(Object sender, DataGridPageChangedEventArgs e)
    {
        // Set CurrentPageIndex to the page the user clicked.
        DataGridProcessList.CurrentPageIndex = e.NewPageIndex;
        DataGridProcessList.DataSource = new DataView(ViewState["ProcessList"] as DataTable);
        DataGridProcessList.DataBind();
    }

    protected void DataGridVariables_PageIndexChanged(Object sender, DataGridPageChangedEventArgs e)
    {
        DataGridVariables.CurrentPageIndex = e.NewPageIndex;
        DataGridVariables.DataSource = new DataView(ViewState["Variables"] as DataTable);
        DataGridVariables.DataBind();
    }

    protected void DataGridState_PageIndexChanged(Object sender, DataGridPageChangedEventArgs e)
    {
        DataGridState.CurrentPageIndex = e.NewPageIndex;
        DataGridState.DataSource = new DataView(ViewState["State"] as DataTable);
        DataGridState.DataBind();
    }

    protected void DataGridUser_PageIndexChanged(Object sender, DataGridPageChangedEventArgs e)
    {
        DataGridUser.CurrentPageIndex = e.NewPageIndex;
        DataGridUser.DataSource = new DataView(ViewState["User"] as DataTable);
        DataGridUser.DataBind();
    }

    protected void LinkButton_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            switch (lb.CommandName)
            {
                case "Process":
                    DataGridProcessList.Visible = !DataGridProcessList.Visible;
                    if (DataGridProcessList.Visible) ShowProcessList();                     
                    break;
                case "Variable":
                    DataGridVariables.Visible = !DataGridVariables.Visible ;
                    if (DataGridVariables.Visible) ShowVariables();
                    break;
                case "State":
                    DataGridState.Visible = !DataGridState.Visible;
                    if (DataGridState.Visible) ShowStatus();
                    break;
                case "User":
                    DataGridUser.Visible = !DataGridUser.Visible;
                    if (DataGridUser.Visible) ShowUsers();
                    break;
            }
        }
    }
}