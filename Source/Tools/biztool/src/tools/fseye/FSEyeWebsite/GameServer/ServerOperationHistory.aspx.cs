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
using FSEye;
using FSEye.DAL;
using FSEye.Security;
using Resources;

public partial class GameServer_ServerOperationHistory : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerOperationHistory, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }      
        
        if (IsPostBack == false)
        {          
            CreateCheckBoxListOperation();
            SelectedServerGroupChanged(null,null);

            DateTime now = DateTime.Now;
            EndDate.SelectedDate = now;
            StartDate.SelectedDate = now.AddDays(-1);

             if (Page.Request.Params["SerialNum"] != null)
            {
                QueryByWhatRadioButtonList.SelectedValue = "operationId";
                QueryByWhatRadioButtonList_OnSelectedIndexChanged(null, null);
                OperationIdTextBox.Text = Page.Request.Params["SerialNum"];
                ButtonQuery_Click(null, null);
            }
            
        }
    }

    protected void ButtonQuery_Click(object sender, EventArgs e)
    {
        //对GameServerManager的读权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }
        
        //按流水号查询的情况（相对简明）
        if (QueryByWhatRadioButtonList.SelectedValue == "operationId")
        {
            Int64 operationId = Int64.Parse(OperationIdTextBox.Text);

            if (operationId <= 0)
            {
                LabelOpMsg.Text = "不合法的操作流水号 -- 必须为正整数";
                LabelOpMsg.Visible = true;
                ResultDataGrid.Visible = false;
                return;
            }

            DbFilter operationIdFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationId, operationId);

            DbFilter finalFilter = operationIdFilter;

            //按照操作结果checkboxlist的勾选情况生成最终的finalfilter
            //这里将两项都勾和两项都不勾作为相同的情况处理---当作都勾
            if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == true) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == false))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1);
                finalFilter = FilterFactory.CreateAndFilter(operationIdFilter, resultFilter);
            }
            else if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == false) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == true))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0);
                finalFilter = FilterFactory.CreateAndFilter(operationIdFilter, resultFilter);
            }
            //这里之所以需要再加一个else，是为了限定只返回operation_result字段的值为0或1的记录，
            //这样就过滤掉了存储最大流水号的那条特殊记录(因为其operation_result字段的值为-1)
            else
            {
                DbFilter resultFilter = FilterFactory.CreateOrFilter(FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1), FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0));
                finalFilter = FilterFactory.CreateAndFilter(operationIdFilter, resultFilter);
            }

            DataTable dt = QueryAndReturnResultDataTable(finalFilter);

            if (dt == null)
            {
                LabelOpMsg.Text = "没有查到满足条件的记录";
                LabelOpMsg.Visible = true;
                ResultDataGrid.Visible = false;
                return;
            }

            DataView dv = new DataView(dt);
            ResultDataGrid.DataSource = dv;
            ResultDataGrid.DataBind();
            ResultDataGrid.Visible = true;
            LabelOpMsg.Visible = false;
        }
        
        //按操作对象查询的情况(复杂)
        else if (QueryByWhatRadioButtonList.SelectedValue == "target")
        {
            if (ServerGroupDropDownList.SelectedServerGroup == null)
            {
                LabelOpMsg.Text = "无操作对象";
                LabelOpMsg.Visible = true;
                ResultDataGrid.Visible = false;
                return;
            }

            DbFilter finalFilter = null;

            //操作对象的filter
            if (ServerDropDownList.SelectedValue == "thisServerGroup")
            {
                int serverGroupId = ServerGroupDropDownList.SelectedServerGroup.Id;
                finalFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldTargetId, serverGroupId);
            }
            else
            {
                int serverId = int.Parse(ServerDropDownList.SelectedValue);
                finalFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldTargetId, serverId);
            }

            //平台用户的filter
            if (UserRadioButtonList.SelectedValue == "name")
            {
                if (UserNameTextBox.Text != String.Empty)
                {
                    FSEye.Security.User user = AdminServer.TheInstance.SecurityManager.GetUser(UserNameTextBox.Text);
                    if (user == null)
                    {
                        LabelOpMsg.Text = "指定的平台用户不存在";
                        LabelOpMsg.Visible = true;
                        ResultDataGrid.Visible = false;
                        return;
                    }
                    DbFilter userFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldUserId, user.SecurityObject.Id);
                    finalFilter = FilterFactory.CreateAndFilter(finalFilter, userFilter);
                }
            }
            else if (UserRadioButtonList.SelectedValue == "id")
            {
                if (UserIdTextBox.Text != String.Empty)
                {
                    int userId = int.Parse(UserIdTextBox.Text);
                    FSEye.Security.User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
                    if (user == null)
                    {
                        LabelOpMsg.Text = "指定的平台用户不存在";
                        LabelOpMsg.Visible = true;
                        ResultDataGrid.Visible = false;
                        return;
                    }
                    DbFilter userFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldUserId, userId);
                    finalFilter = FilterFactory.CreateAndFilter(finalFilter, userFilter);
                }
            }

            //操作类型的filter
            if (CheckBoxListOperation.SelectedIndex != -1)
            {
                DbFilter operationTypeFilter = null;
                foreach (ListItem item in CheckBoxListOperation.Items)
                {
                    if (item.Selected == true)
                    {
                        int opTypeInt = int.Parse(item.Value);

                        DbFilter singleTypeFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationType, opTypeInt);

                        if (operationTypeFilter == null)
                        {
                            operationTypeFilter = singleTypeFilter;
                        }
                        else
                        {
                            operationTypeFilter = FilterFactory.CreateOrFilter(operationTypeFilter, singleTypeFilter);
                        }
                    }
                }

                if (operationTypeFilter != null)
                {
                    finalFilter = FilterFactory.CreateAndFilter(finalFilter, operationTypeFilter);
                }
            }

            //操作结果的filter
            if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == true) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == false))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1);
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, resultFilter);
            }
            else if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == false) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == true))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0);
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, resultFilter);
            }
            else
            {
                DbFilter resultFilter = FilterFactory.CreateOrFilter(FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1), FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, resultFilter);
            }

            //时间的filter
            DateTime smallerDateTime;
            DateTime biggerDateTime;
            if ((StartDate.Selected == true) && (EndDate.Selected == true))
            {
                if (StartDate.SelectedDate <= EndDate.SelectedDate)
                {
                    smallerDateTime = StartDate.SelectedDate;
                    biggerDateTime = EndDate.SelectedDate;
                }
                else
                {
                    smallerDateTime = EndDate.SelectedDate;
                    biggerDateTime = StartDate.SelectedDate;
                }

                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }
            else if ((StartDate.Selected == true) && (EndDate.Selected == false))
            {
                DateTime now = DateTime.Now;

                if (StartDate.SelectedDate <= now)
                {
                    smallerDateTime = StartDate.SelectedDate;
                    biggerDateTime = now;
                }
                else
                {
                    smallerDateTime = now;
                    biggerDateTime = StartDate.SelectedDate;
                }
                
                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }
            else if ((StartDate.Selected == false) && (EndDate.Selected == true))
            {
                DateTime now = DateTime.Now;

                if (EndDate.SelectedDate <= now)
                {
                    smallerDateTime = EndDate.SelectedDate;
                    biggerDateTime = now;
                }
                else
                {
                    smallerDateTime = now;
                    biggerDateTime = EndDate.SelectedDate;
                }

                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }

            //finalfilter生成完毕，可以查询了
            DataTable dt = QueryAndReturnResultDataTable(finalFilter);

            if (dt == null)
            {
                LabelOpMsg.Text = "没有查到满足条件的记录";
                LabelOpMsg.Visible = true;
                ResultDataGrid.Visible = false;
                return;
            }

            DataView dv = new DataView(dt);
            ResultDataGrid.DataSource = dv;
            ResultDataGrid.DataBind();
            ResultDataGrid.Visible = true;
            LabelOpMsg.Visible = false;
        }

        else if (QueryByWhatRadioButtonList.SelectedValue == "time")
        {            
            DbFilter finalFilter;
            
            //操作结果的filter
            if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == true) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == false))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1);
                finalFilter = resultFilter;
            }
            else if ((resultSucessCheckBoxList.Items.FindByValue("1").Selected == false) && (resultSucessCheckBoxList.Items.FindByValue("0").Selected == true))
            {
                DbFilter resultFilter = FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0);
                finalFilter = resultFilter;
            }
            else
            {
                DbFilter resultFilter = FilterFactory.CreateOrFilter(FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 1), FilterFactory.CreateEqualFilter(TableString.ServerOperationLogFieldOperationResult, 0));
                finalFilter = resultFilter;
            }            
            
            //时间的filter
            DateTime smallerDateTime;
            DateTime biggerDateTime;
            if ((StartDate.Selected == true) && (EndDate.Selected == true))
            {
                if (StartDate.SelectedDate <= EndDate.SelectedDate)
                {
                    smallerDateTime = StartDate.SelectedDate;
                    biggerDateTime = EndDate.SelectedDate;
                }
                else
                {
                    smallerDateTime = EndDate.SelectedDate;
                    biggerDateTime = StartDate.SelectedDate;
                }

                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }
            else if ((StartDate.Selected == true) && (EndDate.Selected == false))
            {
                DateTime now = DateTime.Now;

                if (StartDate.SelectedDate <= now)
                {
                    smallerDateTime = StartDate.SelectedDate;
                    biggerDateTime = now;
                }
                else
                {
                    smallerDateTime = now;
                    biggerDateTime = StartDate.SelectedDate;
                }

                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }
            else if ((StartDate.Selected == false) && (EndDate.Selected == true))
            {
                DateTime now = DateTime.Now;

                if (EndDate.SelectedDate <= now)
                {
                    smallerDateTime = EndDate.SelectedDate;
                    biggerDateTime = now;
                }
                else
                {
                    smallerDateTime = now;
                    biggerDateTime = EndDate.SelectedDate;
                }

                DbFilter timeFilter = FilterFactory.CreateAndFilter(FilterFactory.CreateLargerEqualFilter(TableString.ServerOperationLogFieldLogTime, smallerDateTime), FilterFactory.CreateLesserEqualFilter(TableString.ServerOperationLogFieldLogTime, biggerDateTime));
                finalFilter = FilterFactory.CreateAndFilter(finalFilter, timeFilter);
            }

            //finalfilter生成完毕，可以查询了
            DataTable dt = QueryAndReturnResultDataTable(finalFilter);

            if (dt == null)
            {
                LabelOpMsg.Text = "没有查到满足条件的记录";
                LabelOpMsg.Visible = true;
                ResultDataGrid.Visible = false;
                return;
            }

            DataView dv = new DataView(dt);
            ResultDataGrid.DataSource = dv;
            ResultDataGrid.DataBind();
            ResultDataGrid.Visible = true;
            LabelOpMsg.Visible = false;
        }
    }


    //按照filter来查询数据库，并对结果处理，将各个int值转化为对应的可读的string信息，
    //并且使其结构于页面中的ResultDataGrid一致。
    //将处理后生成的datatable返回，ResultDataGrid只须与其绑定即可。
    private DataTable QueryAndReturnResultDataTable(DbFilter filter)
    {
        //开始查询了
        using (IBlazeDatabase db = DbFactory.GetDatabase())
        {
            IBlazeTable table = db.GetTable(TableString.ServerOperationLogTableName);
            DataSet ds = new DataSet();
            table.Get(ds, filter);
            DataTable dt = ds.Tables[TableString.ServerOperationLogTableName];

            if (dt.Rows.Count == 0)
            {
                return null;
            }

            //按照log_time升序进行排序
            dt.DefaultView.Sort = TableString.ServerOperationLogFieldLogTime + " ASC";

            DataTable finalDataTable = new DataTable();
            finalDataTable.Columns.Add(new DataColumn("operation_id", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("user_name", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("target_type", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("target_name", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("operation_type", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("operation_result", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("description", typeof(String)));
            finalDataTable.Columns.Add(new DataColumn("log_time", typeof(DateTime)));


            foreach (DataRow row in dt.Rows)
            {
                //新的datarow，要加入到finalDataTable中去的
                DataRow newRow = finalDataTable.NewRow();

                //设置显示结果中的操作流水号
                Int64 opId = (Int64)row[TableString.ServerOperationLogFieldOperationId];
                if (opId <= 0)
                {
                    newRow["operation_id"] = "不合法的操作流水号:必须为正整数";
                }
                else
                {
                    newRow["operation_id"] = opId.ToString();
                }

                //设置平台用户名
                int userId = (int)row[TableString.ServerOperationLogFieldUserId];
                FSEye.Security.User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
                if (user == null)
                {
                    newRow["user_name"] = "无此用户";
                }
                else
                {
                    newRow["user_name"] = user.UserName;
                }

                //设置操作对象类型和操作对象名称
                //targetId = -2表示对多个组开始进行操作，多个组名存在description字段中
                int isServerGroup = (int)row[TableString.ServerOperationLogFieldServerOrGroup];
                int targetId = (int)row[TableString.ServerOperationLogFieldTargetId];
                if (isServerGroup == 1)
                {
                    newRow["target_type"] = "服务器组";
                    if (targetId == -2)
                    {
                        newRow["target_name"] = "";
                    }
                    else
                    {
                        ServerGroup serverGroup = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(targetId);
                        if (serverGroup == null)
                        {
                            newRow["target_name"] = "无此服务器组";
                        }
                        else
                        {
                            newRow["target_name"] = serverGroup.Name;
                        }
                    }
                }
                else if (isServerGroup == 0)
                {
                    newRow["target_type"] = "服务器";
                    GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(targetId);
                    if (server == null)
                    {
                        newRow["target_name"] = "无此服务器";
                    }
                    else
                    {
                        newRow["target_name"] = server.Name;
                    }
                }
                else
                {
                    newRow["target_type"] = "不合法的对象类型";
                    newRow["target_name"] = "不合法的操作对象";
                }

                //设置操作类型
                int opTypeInt = (int)row[TableString.ServerOperationLogFieldOperationType];
                GameServer.ServerOperationType opTypeEnum = (GameServer.ServerOperationType)Enum.Parse(typeof(GameServer.ServerOperationType), opTypeInt.ToString());
                switch (opTypeEnum)
                {
                    case GameServer.ServerOperationType.Start:
                        newRow["operation_type"] = "启动";
                        break;
                    case GameServer.ServerOperationType.Close:
                        newRow["operation_type"] = "关闭";
                        break;
                    case GameServer.ServerOperationType.Update:
                        newRow["operation_type"] = "更新";
                        break;
                    case GameServer.ServerOperationType.Download:
                        newRow["operation_type"] = "下载更新包";
                        break;
                    case GameServer.ServerOperationType.UpdateConfigFile:
                        newRow["operation_type"] = "上传配置文件";
                        break;
                    default:
                        newRow["operation_type"] = "未知操作类型";
                        break;
                }

                //设置操作结果
                int resultSuccess = (int)row[TableString.ServerOperationLogFieldOperationResult];
                if (resultSuccess == 1)
                {
                    newRow["operation_result"] = "成功";
                }
                else if (resultSuccess == 0)
                {
                    newRow["operation_result"] = "失败";                    
                }
                else
                {
                    newRow["operation_result"] = "不合法的操作结果";
                }

                //设置操作描述
                newRow["description"] = (String)row[TableString.ServerOperationLogFieldDescription];


                //设置日志记录时间
                newRow["log_time"] = (DateTime)row[TableString.ServerOperationLogFieldLogTime];

                //将newrow加入到finaldatatable中
                finalDataTable.Rows.Add(newRow);
            }

            return finalDataTable;
        }

    }

    protected void QueryByWhatRadioButtonList_OnSelectedIndexChanged(object sender, EventArgs e)
    {
        string value = QueryByWhatRadioButtonList.SelectedValue;

        if (value == "target")
        {
            OperationTargetTableHeaderRow.Visible = true;
            OperationIdTableHeaderRow.Visible = false;
            UserTableHeaderRow.Visible = true;
            OperationTypeTableHeaderRow.Visible = true;
            StartTimeTableHeaderRow.Visible = true;
            EndTimeTableHeaderRow.Visible = true;
        }
        else if (value == "operationId")
        {
            OperationTargetTableHeaderRow.Visible = false;
            OperationIdTableHeaderRow.Visible = true;
            UserTableHeaderRow.Visible = false;
            OperationTypeTableHeaderRow.Visible = false;
            StartTimeTableHeaderRow.Visible = false;
            EndTimeTableHeaderRow.Visible = false;
        }
        else if (value == "time")
        {
            OperationTargetTableHeaderRow.Visible = false;
            OperationIdTableHeaderRow.Visible = false;
            UserTableHeaderRow.Visible = false;
            OperationTypeTableHeaderRow.Visible = false;
            StartTimeTableHeaderRow.Visible = true;
            EndTimeTableHeaderRow.Visible = true;
        }

        LabelOpMsg.Visible = false;
        ResultDataGrid.Visible = false;
    }

    protected void UserRadioButtonList_OnSelectedIndexChanged(object sender, EventArgs e)
    {
        if (UserRadioButtonList.SelectedValue == "name")
        {
            UserNameTextBox.Visible = true;
            UserIdTextBox.Visible = false;
        }
        else if (UserRadioButtonList.SelectedValue == "id")
        {
            UserNameTextBox.Visible = false;
            UserIdTextBox.Visible = true;
        }

        UserNameTextBox.Text = String.Empty;
        UserIdTextBox.Text = String.Empty;
    }

    protected void SelectedServerGroupChanged(object sender, EventArgs e)
    {
        ServerDropDownList.Items.Clear();
        
        ListItem item = new ListItem("本服务器组", "thisServerGroup");
        ServerDropDownList.Items.Add(item);

        string text = String.Empty;
        string value = String.Empty;

        foreach (GameServer server in ServerGroupDropDownList.SelectedServerGroup.List)
        {
            text = server.Name;
            value = server.Id.ToString();
            item = new ListItem(text, value);
            ServerDropDownList.Items.Add(item);
        }
    }



    private void CreateCheckBoxListOperation()
    {
        string text = String.Empty;

        foreach (GameServer.ServerOperationType operationType in Enum.GetValues(typeof(GameServer.ServerOperationType)))
        {
            switch (operationType)
            {
                case GameServer.ServerOperationType.Start:
                    text = "启动";
                    break;
                case GameServer.ServerOperationType.Close:
                    text = "关闭";
                    break;
                case GameServer.ServerOperationType.Update:
                    text = "更新";
                    break;
                case GameServer.ServerOperationType.Download:
                    text = "下载";
                    break;
                case GameServer.ServerOperationType.UpdateGuard:
                    text = "更新Guard";
                    break;
                
                default:
                    break;
            }

            ListItem item = new ListItem(text, ((int)operationType).ToString());
            item.Selected = true;
            CheckBoxListOperation.Items.Add(item);
        }
    }
}
