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
using MySql.Data.MySqlClient;
using Resources;
using FSEye.Security;
using FSEye;

public partial class GameMaster_OperationQuery : BasePage
{
    const int DefaultRecordPerPage = 50;

    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterOperationQuery, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            CreateCheckBoxListOperation();

            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;            
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

        LabelOpMsg.Visible = false;
        PanelResult.Visible = false;
    }

    private void CreateCheckBoxListOperation()
    {
        foreach (GMUtil.GMOperation op in Enum.GetValues(typeof(GMUtil.GMOperation)))
        {
            string text = string.Empty;
            switch(op)
            {
                case GMUtil.GMOperation.ChatDisable:
                    text = StringDef.ChatDisable;
                    break;
                case GMUtil.GMOperation.ChatEnable:
                    text = StringDef.ChatEnable;
                    break;
                case GMUtil.GMOperation.FreezeAccount:
                    text = StringDef.Freeze+StringDef.Account;
                    break;
                case GMUtil.GMOperation.FreezeRole:
                    text = StringDef.Freeze+StringDef.Role;
                    break;
                case GMUtil.GMOperation.SendMail:
                    text = StringDef.SendMail;
                    break;
                case GMUtil.GMOperation.SendMessage:
                    text = StringDef.SendMessage;
                    break;
                case GMUtil.GMOperation.UnfreezeAccount:
                    text = StringDef.Unfreeze+StringDef.Account;
                    break;
                case GMUtil.GMOperation.UnfreezeRole:
                    text = StringDef.Unfreeze+StringDef.Role;
                    break;
                case GMUtil.GMOperation.ExeSysCmd:
                    text = StringDef.ExecuteGMCommand;
                    break;
                case GMUtil.GMOperation.CreateIBShopScript:
                    text = StringDef.CreateIBShopConfigScript;
                    break;
                case GMUtil.GMOperation.DeleteIBShopScript:
                    text = StringDef.DeleteIBShopConfigScript;
                    break;
                default:
                    break;
            }
            ListItem item = new ListItem(text, ((int)op).ToString());
            item.Selected = true;
            CheckBoxListOperation.Items.Add(item);
        }        
    }

    private ICollection CreateDataSource(DataTable table)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;
            dataTable.Columns.Add("User", typeof(String));
            dataTable.Columns.Add("Operation", typeof(String));
            dataTable.Columns.Add("Target", typeof(String));
            dataTable.Columns.Add("ServerName", typeof(String));
            dataTable.Columns.Add("Description", typeof(String));
            dataTable.Columns.Add("LogTime", typeof(DateTime));
            dataTable.Columns.Add("ServerId", typeof(Int32));
            DataTableReader reader = new DataTableReader(table);
            foreach (DataRow row in table.Rows)
            {
                dataRow = dataTable.NewRow();
                int userId = (int)row.ItemArray[1];
                User user = TheAdminServer.SecurityManager.GetUser(userId);
                if (user != null)
                    dataRow[0] = user.RealName;
                else
                    dataRow[0] = string.Format("[{0}]", userId);
                GMUtil.GMOperation operation = (GMUtil.GMOperation)row.ItemArray[2];
                string text = string.Empty;
                switch (operation)
                {
                    case GMUtil.GMOperation.ChatDisable:
                        text = StringDef.ChatDisable;
                        break;
                    case GMUtil.GMOperation.ChatEnable:
                        text = StringDef.ChatEnable;
                        break;
                    case GMUtil.GMOperation.FreezeAccount:
                        text = StringDef.Freeze + StringDef.Account;
                        break;
                    case GMUtil.GMOperation.FreezeRole:
                        text = StringDef.Freeze + StringDef.Role;
                        break;
                    case GMUtil.GMOperation.SendMail:
                        text = StringDef.SendMail;
                        break;
                    case GMUtil.GMOperation.SendMessage:
                        text = StringDef.SendMessage;
                        break;
                    case GMUtil.GMOperation.UnfreezeAccount:
                        text = StringDef.Unfreeze + StringDef.Account;
                        break;
                    case GMUtil.GMOperation.UnfreezeRole:
                        text = StringDef.Unfreeze + StringDef.Role;
                        break;
                    case GMUtil.GMOperation.ExeSysCmd:
                        text = StringDef.ExecuteGMCommand;
                        break;
                    case GMUtil.GMOperation.CreateIBShopScript:
                        text = StringDef.CreateIBShopConfigScript;
                        break;
                    case GMUtil.GMOperation.DeleteIBShopScript:
                        text = StringDef.DeleteIBShopConfigScript;
                        break;
                    default:
                        break;
                }
                dataRow[1] = text;
                dataRow[2] = Encoding.Default.GetString((byte[])row.ItemArray[3]);
                int serverId = (int)row.ItemArray[4];
                dataRow[6] = serverId;
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
                if (server != null)
                    dataRow[3] = server.Name;
                else
                    dataRow[3] = StringDef.NotAvailable;
                dataRow[4] = Encoding.Default.GetString((byte[])row.ItemArray[5]);
                dataRow[5] = (DateTime)row.ItemArray[6];

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

    protected void ButtonQuery_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }
        
        Query(0);
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

    void Query(int offset)
    {
        ArrayList paramList = new ArrayList();
        StringBuilder whereStatement = new StringBuilder();
        whereStatement.Append("WHERE ");

        string ope = TextBoxOperator.Text.Trim();
        if (ope.Length != 0)
        {
            switch (RadioButtonListOperator.SelectedIndex)
            {
                case 0:
                    whereStatement.AppendFormat("{0}='{1}' AND ", TableString.GMOpLogFieldUserId, ope);
                    break;
                case 1:
                    User user = TheAdminServer.SecurityManager.GetUser(ope);
                    if (user != null)
                    {
                        whereStatement.AppendFormat("{0}='{1}' AND ", TableString.GMOpLogFieldUserId, user.SecurityObject.Id);
                    }
                    else
                    {
                        whereStatement.AppendFormat("{0}='{1}' AND ", TableString.GMOpLogFieldUserId, ope);
                    }
                    break;
            }
        }

        switch (RadioButtonListTarget.SelectedIndex)
        {
            case 1:
                whereStatement.AppendFormat("{0}={1} AND ", TableString.GMOpLogFieldTargetType, (int)GMUtil.GMOpTargetType.Account);
                break;
            case 2:
                whereStatement.AppendFormat("{0}={1} AND ", TableString.GMOpLogFieldTargetType, (int)GMUtil.GMOpTargetType.Role);
                break;
            default:
                break;
        }

        string target = TextBoxTarget.Text.Trim();
        if(target.Length!=0)
        {            
            MySqlParameter param = new MySqlParameter("?target", MySqlDbType.Blob);
            if(CheckBoxExactlyMatch.Checked)
            {
                whereStatement.AppendFormat("{0}=?target AND ", TableString.GMOpLogFieldTarget);
                param.Value = Encoding.Default.GetBytes("target");
            }
            else 
            {
                whereStatement.AppendFormat("{0} LIKE ?target AND ", TableString.GMOpLogFieldTarget);
                param.Value = Encoding.Default.GetBytes("%" + target + "%");
            }
            
            paramList.Add(param);
        }

        if (ServerDropDownList.Enabled)
            whereStatement.AppendFormat("{0}={1} AND ", TableString.GMOpLogFieldServerId, ServerDropDownList.SelectedServerId);

        if(CheckBoxListOperation.SelectedIndex!=-1)
        {
            StringBuilder operationStr = new StringBuilder();
            foreach (ListItem item in CheckBoxListOperation.Items)
            {
                if (item.Selected)
                    operationStr.AppendFormat("{0}={1} OR ", TableString.GMOpLogFieldOperation, int.Parse(item.Value));
            }
            operationStr.Remove(operationStr.Length - 3, 3);
            whereStatement.Append(string.Format("({0}) AND ", operationStr.ToString()));
        }

        if (StartDate.Selected)
            whereStatement.Append(string.Format("{0}>='{1}' AND ", TableString.GMOpLogFieldLogTime, StartDate.SelectedDate.ToString("yyyy-MM-dd HH:mm:ss")));
        if (EndDate.Selected)
            whereStatement.Append(string.Format("{0}<'{1}' AND ", TableString.GMOpLogFieldLogTime, EndDate.SelectedDate.ToString("yyyy-MM-dd HH:mm:ss")));

        if (whereStatement.Length != 6)
            whereStatement.Remove(whereStatement.Length - 4, 4);
        else
            whereStatement.Remove(0, whereStatement.Length);

        int limitCount = _recordPerPage;
        string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

        string sqlStatement = string.Format("SELECT * FROM {0} {1} {2}", TableString.GMOpLogTableName, whereStatement.Length == 0 ? string.Empty : whereStatement.ToString(),limitStatement);

        //为动画效果而做延迟
        System.Threading.Thread.Sleep(500);

        DataTable table = LogSystem.TheInstance.QueryGMOperationLog(sqlStatement, paramList.ToArray());
        if (table != null && table.Rows.Count != 0)
        {
            PanelResult.Visible = true;

            DataGridResult.DataSource = CreateDataSource(table);
            DataGridResult.DataBind();

            ViewState[WebConfig.SessionQueryLogOffset] = offset;

            ButtonPreviousPage.Enabled = (offset > 0);
            ButtonFirstPage.Enabled = (offset > 0);
            ButtonNextPage.Enabled = (table.Rows.Count >= limitCount);
        }
        else
        {
            LabelOpMsg.Text = StringDef.NoMatchingRecord;
            LabelOpMsg.Visible = true;
            return;
        }        
    }

    protected void RadioButtonListTarget_SelectedIndexChanged(object sender, EventArgs e)
    {
        switch (int.Parse(RadioButtonListTarget.SelectedValue))
        {
            case (int)GMUtil.GMOpTargetType.Role:
                ServerGroupDropDownList.Enabled = true;
                ServerDropDownList.Enabled = true; 
                break;
            case (int)GMUtil.GMOpTargetType.Account:
            default:
                ServerGroupDropDownList.Enabled = false;
                ServerDropDownList.Enabled = false;
                break;
        }
    }

    protected void SelectedServerGroupChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}
