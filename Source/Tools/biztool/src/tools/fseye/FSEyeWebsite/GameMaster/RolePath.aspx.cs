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

public partial class GameMaster_RolePath : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (IsPostBack == false)
        {
            if ((Request.QueryString["roleGUID"] != null) && (Request.QueryString["serverID"] != null))
            {
                roleGUIDTextBox.Text = Request.QueryString["roleGUID"];
                serverGroupDropDownList.SelectedValue = Request.QueryString["serverID"];
            }

            RecentLoginTime_end.SelectedDate = DateTime.Now;
            RecentLoginTime_start.SelectedDate = RecentLoginTime_end.SelectedDate.AddHours(-24);
        }
    }

    protected void submitButton_Click(object sender, EventArgs e)
    {
        ViewState["totalResults"] = 0;
        ViewState["resultsPerPage"] = 30;
        ViewState["currentPage"] = 0;
        
        QueryAndDisplay(true);
    }

    protected void ButtonNextPage_Click(object sender, EventArgs e)
    {
        QueryAndDisplay(false);
    }

    protected void ButtonPreviousPage_Click(object sender, EventArgs e)
    {
        int currentPage = (int)ViewState["currentPage"];
        currentPage -= 2;
        ViewState["currentPage"] = currentPage;

        QueryAndDisplay(false);
    }

    protected void ButtonFirstPage_Click(object sender, EventArgs e)
    {
        ViewState["currentPage"] = 0;

        QueryAndDisplay(false);
    }

    private void QueryAndDisplay(bool firstTimeQuery)
    {       
        string roleGUID = roleGUIDTextBox.Text;
        GameServer gameCenter = AdminServer.TheInstance.GameServerManager.GetGameServer(int.Parse(serverGroupDropDownList.SelectedValue));
        ArrayList paramList = new ArrayList();

        string queryString1 = " select logtime as time, '升级' as event, CONCAT('升到<', CAST(field2 as char(8)),'>级') as remark"
                              + " from log_3_1"
                              + " where field1='?'";


        string queryString2 = " select logtime as time, '加入门派' as event, CONCAT('加入<', (select field2 from faction where log_3_2.field3=faction.field1 and faction.field3=0), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark"
                              + " from log_3_2"
                              + " where field1='?'";


        string queryString3 = " select logtime as time, '出师' as event, CONCAT('出师自<', (select field2 from faction where log_3_3.field3=faction.field1 and faction.field3=0), '>，路线<', (select field2 from faction where log_3_3.field3=faction.field1 and log_3_3.field4=faction.field3), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark"
                              + " from log_3_3"
                              + " where field1='?'";

        string queryString4 = " select logtime as time, '加入家族' as event, CONCAT('加入<', (select field2 from kin where log_3_4.field3=kin.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark"
                              + " from log_3_4"
                              + " where field1='?'";

        string queryString5 = " select logtime as time, '退出家族' as event, CONCAT('退出<', (select field2 from kin where log_3_5.field3=kin.field1), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark"
                              + " from log_3_5"
                              + " where field1='?'";

        string queryString6 = " select logtime as time, '完成任务' as event, CONCAT('主任务:', CAST(field4 as char), ' 子任务:', CAST(field5 as char)) as remark"
                              + " from log_3_6"
                              + " where field1='?'";

        string queryString7 = " select logtime as time,"
                              + " (case field2"
                              + " when 0 then '下线'"
                              + " when 1 then '登录'"
                              + " when 2 then '开始离线托管'"
                              + " when 3 then '离线托管超时'"
                              + " when 4 then '离线托管踢号'"
                              + " else '未知事件'"
                              + " end) as event,"
                              + " field3 as remark"
                              + " from log_3_7"
                              + " where field1='?'";


                              //+ " union"
                              //+ " select logtime as time, '选择路线' as event, CONCAT('选择路线<', (select field2 from faction where log_3_8.field3=faction.field1 and log_3_8.field4=faction.field3), '>，当时为<', CAST(field2 as char(8)), '>级')   as remark"
                              //+ " from log_3_8"
                              //+ " where field1='?'"
                              //+ " and logtime between '?' and '?'"

        ArrayList queryStringsArrayList = new ArrayList();

        //根据eventTypeCheckBoxList的勾选情况来确定哪几个queryString被加入到最终的SQL语句中
        if (eventTypeCheckBoxList.SelectedIndex == -1)
        {
            LabelSuccess.Visible = false;
            LabelOpMsg.Text = "未选择事件类型 - 至少要选中一种事件";
            LabelOpMsg.Visible = true;
            resultDataGrid.Visible = false;
            resultPagingInfoPanel.Visible = false;
            return;
        }

        if (eventTypeCheckBoxList.Items.FindByValue("log_3_1").Selected == true)
        {
            queryStringsArrayList.Add(queryString1);
        }

        if (eventTypeCheckBoxList.Items.FindByValue("log_3_2,log_3_3,log_3_8").Selected == true)
        {
            queryStringsArrayList.Add(queryString2);
            queryStringsArrayList.Add(queryString3);            
        }

        if (eventTypeCheckBoxList.Items.FindByValue("log_3_4,log_3_5").Selected == true)
        {
            queryStringsArrayList.Add(queryString4);
            queryStringsArrayList.Add(queryString5);
        }

        if (eventTypeCheckBoxList.Items.FindByValue("log_3_6").Selected == true)
        {
            queryStringsArrayList.Add(queryString6);
        }

        if (eventTypeCheckBoxList.Items.FindByValue("log_3_7").Selected == true)
        {
            queryStringsArrayList.Add(queryString7);
        }


        //若两个时间的TextBox不是都为空
        //注意，TextBox为空时，SelectedDate属性的值为DateTime.MinValue
        if (!((RecentLoginTime_start.SelectedDate == DateTime.MinValue) && (RecentLoginTime_end.SelectedDate == DateTime.MinValue)))
        {
            DateTime time1;
            DateTime time2;

            if (RecentLoginTime_start.SelectedDate == DateTime.MinValue)
            {
                time1 = DateTime.Now;
            }
            else
            {
                time1 = RecentLoginTime_start.SelectedDate;
            }
            if (RecentLoginTime_end.SelectedDate == DateTime.MinValue)
            {
                time2 = DateTime.Now;
            }
            else
            {
                time2 = RecentLoginTime_end.SelectedDate;
            }

            for (int i = 0; i < queryStringsArrayList.Count; i++)
            {
                queryStringsArrayList[i] += " and logtime between '?' and '?'";

                paramList.Add(roleGUID);

                if (time1 <= time2)
                {
                    paramList.Add(time1.ToString("yyyy-MM-dd HH:mm:ss"));
                    paramList.Add(time2.ToString("yyyy-MM-dd HH:mm:ss"));
                }
                else
                {
                    paramList.Add(time2.ToString("yyyy-MM-dd HH:mm:ss"));
                    paramList.Add(time1.ToString("yyyy-MM-dd HH:mm:ss"));
                }
            }
        }
        else
        {
            for (int i = 0; i < queryStringsArrayList.Count; i++)
            {
                paramList.Add(roleGUID);
            }
        }

        string queryString = String.Join(" union all ", (string[])queryStringsArrayList.ToArray(typeof(String)));

        queryString += " order by Time DESC ";

        queryString = queryString.Insert(7, " SQL_CALC_FOUND_ROWS ");

        int currentPage = (int)ViewState["currentPage"];
        int resultsPerPage = (int)ViewState["resultsPerPage"];
        int offset = resultsPerPage * currentPage;
        queryString += " limit " + offset.ToString() + "," + resultsPerPage.ToString();
        currentPage++;
        ViewState["currentPage"] = currentPage;

        SqlCommand sqlcmd = new SqlCommand(queryString, paramList.ToArray());
        SqlResult queryResult = WebUtil.QueryGameServerDb(CurrentUser.Id, gameCenter, sqlcmd);      

        if (queryResult != null && queryResult.Success)
        {
            object[] record = null;
            
            DataTable dt = new DataTable();            
            dt.Columns.Add(new DataColumn("time", typeof(DateTime)));
            dt.Columns.Add(new DataColumn("event", typeof(String)));
            dt.Columns.Add(new DataColumn("remark", typeof(String)));
            DataRow dr;
            
            queryResult.SetFieldType
                (
                    new SqlDataType[] 
                    {
			            SqlDataType.DateTime,
			            SqlDataType.String,
			            SqlDataType.String,
                    }
                );
            
            while ((record = queryResult.ReadRecord()) != null)
            {
                dr = dt.NewRow();
                dr[0] = (DateTime)record[0];
                dr[1] = (String)record[1];
                dr[2] = (String)record[2];
                dt.Rows.Add(dr);                
            }

            DataView dv = new DataView(dt);

            if (dt.Rows.Count == 0)
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = "没有满足条件的记录";
                LabelOpMsg.Visible = true;
                resultDataGrid.Visible = false;
                resultPagingInfoPanel.Visible = false;
                return;
            }

            resultDataGrid.DataSource = dv;
            resultDataGrid.DataBind();
            resultDataGrid.Visible = true;

            LabelOpMsg.Visible = false;
            LabelSuccess.Text = "查询成功完成";
            LabelSuccess.Visible = true;

            //只有在首次查询时，也就是点击查询按钮时才去查结果的总数，otherwise，总数已经在ViewState["totalResults"]中了
            if (firstTimeQuery == true)
            {
                SqlCommand sqlcmdRowCount = new SqlCommand("select found_rows()", paramList.ToArray());
                SqlResult queryResultRowCount = WebUtil.QueryGameServerDb(CurrentUser.Id, gameCenter, sqlcmdRowCount);

                queryResultRowCount.SetFieldType(new SqlDataType[] { SqlDataType.String });

                object[] rowCountResult = queryResultRowCount.ReadRecord();

                ViewState["totalResults"] = rowCountResult[0];
            }

            int rowCount = int.Parse((string)ViewState["totalResults"]);
            resultInfoLabel.Text = "共有" + rowCount.ToString() + "条记录，每页" + resultsPerPage.ToString() + "条记录，当前页数：" + currentPage.ToString();

            //当前页数大于1时才允许点击“第一页”按钮和“上一页”按钮
            if (currentPage >= 2)
            {
                ButtonFirstPage.Enabled = true;
                ButtonPreviousPage.Enabled = true;
            }
            else
            {
                ButtonFirstPage.Enabled = false;
                ButtonPreviousPage.Enabled = false;
            }

            //当下一页还能有记录的时候才允许点击“下一页”按钮
            if ((resultsPerPage * currentPage) < rowCount)
            {
                ButtonNextPage.Enabled = true;
            }
            else
            {
                ButtonNextPage.Enabled = false;
            }

            resultPagingInfoPanel.Visible = true;

        }
        else
        {
            if (queryResult == null)
            {
                LabelSuccess.Visible = false;
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                LabelOpMsg.Visible = true;
                resultDataGrid.Visible = false;
                resultPagingInfoPanel.Visible = false;

            }
            else
            {
                LabelSuccess.Visible = false;               
                LabelOpMsg.Text = StringDef.OperationFail;
                LabelOpMsg.Visible = true;
                resultDataGrid.Visible = false;
                resultPagingInfoPanel.Visible = false;                                                        
            }
        }
        
    }
}
