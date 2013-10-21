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
using FSEye.FS2Lib;
using FSEye;
using FSEye.Security;

public partial class GameInfo_ItemFlow : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (IsPostBack == false)
        {
            if ((Request.QueryString["itemGUID"] != null) && (Request.QueryString["serverID"] != null))
            {
                TextBoxInputItemGuid.Text = Request.QueryString["itemGUID"];
                serverGroupDropDownList.SelectedValue = Request.QueryString["serverID"];
            }

            RecentLoginTime_end.SelectedDate = DateTime.Now;
            RecentLoginTime_start.SelectedDate = RecentLoginTime_end.SelectedDate.AddHours(-24);
        }
    }

    protected void LinkButtonQuery_Click(object sender, EventArgs e)
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
        
        string itemGUID = TextBoxInputItemGuid.Text;
        GameServer gameCenter = AdminServer.TheInstance.GameServerManager.GetGameServer(int.Parse(serverGroupDropDownList.SelectedValue));
        ArrayList paramList = new ArrayList();
        
        string queryString1 = " select logtime as Time,"
                            + " (case Field3"
                            + " when 1 then '生成:系统产生'"
                            + " when 2 then '生成:合成'"
                            + " when 3 then '生成:NPC商店购买'"
                            + " when 4 then '生成:奇珍阁购买'"
                            + " when 5 then '生成:NPC掉落'"
                            + " when 6 then '生成:手工拆分'"
                            + " when 7 then '生成:贩卖拆分'"                            
                            + " when 8 then '生成:脚本(任务)'"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " Field30 as GoodsName, Field2 as userGUID, (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_1.Field2 limit 1) as userName, Field31 as ItemDetails"
                            + " from log_4_1"
                            + " where Field1 = '?'";

        string queryString2 = " select logtime as Time,"
                            + " (case Field3"
                            + " when 1 then '销毁:系统销毁'"
                            + " when 2 then '销毁:系统回收(如丢弃在地上等)'"
                            + " when 3 then '销毁:有效期至'"
                            + " when 4 then '销毁:使用消耗'"
                            + " when 5 then '销毁:商品叠加'"
                            + " when 6 then '销毁:卖给NPC'"
                            + " when 7 then '销毁:脚本删除(任务)'"                            
                            + " when 8 then '销毁:装备磨损'"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " (select Field30 from log_4_1 where log_4_1.field1 = log_4_2.field1 limit 1) as GoodsName,"
                            + " Field2 as userGUID,"
                            + " (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_2.Field2 limit 1) as userName,"
                            + " (select Field31 from log_4_1 where log_4_1.field1 = log_4_2.field1 limit 1) as ItemDetails"
                            + " from log_4_2"
                            + " where Field1 = '?'";

        string queryString3 = " select logtime as Time,"
                            + " (case Field3"
                            + " when 1 then CONCAT('丢弃:丢弃在地上',' 地点:',(select mapName from map where mapid = Field4 limit 1  ),'  坐标: ','X:',CAST(Field5 as char(12)),' Y:',CAST(Field6 as char(12)))"
                            + " when 2 then CONCAT('邮寄',' 地点:',(select mapName from map where mapid = Field4 limit 1  ),'  坐标: ','X:',CAST(Field5 as char(12)),' Y:',CAST(Field6 as char(12)))"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " (select Field30 from log_4_1 where log_4_1.field1 = log_4_3.field1 limit 1) as GoodsName,"
                            + " Field2 as userGUID,"
                            + " (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_3.Field2 limit 1) as userName,"
                            + " (select Field31 from log_4_1 where log_4_1.field1 = log_4_3.field1 limit 1) as ItemDetails"
                            + " from log_4_3"
                            + " where Field1 = '?'";

        string queryString4 = " select logtime as Time,"
                            + " (case Field3"
                            + " when 1 then CONCAT('捡起',' 地点:',(select mapName from map where mapid = Field4 limit 1  ),'  坐标: ','X:',CAST(Field5 as char(12)),' Y:',CAST(Field6 as char(12)))"
                            + " when 2 then CONCAT('获取自邮件附件',' 地点:',(select mapName from map where mapid = Field4 limit 1  ),'  坐标: ','X:',CAST(Field5 as char(12)),' Y:',CAST(Field6 as char(12)))"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " (select Field30 from log_4_1 where log_4_1.field1 = log_4_4.field1 limit 1) as GoodsName,"
                            + " Field2 as userGUID,"
                            + " (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_4.Field2 limit 1) as userName,"
                            + " (select Field31 from log_4_1 where log_4_1.field1 = log_4_4.field1 limit 1) as ItemDetails"
                            + " from log_4_4"
                            + " where Field1 = '?'";

        string queryString5 = " select logtime as Time,"
                            + " (case Field4"
                            + " when 1 then CONCAT('所有者变更:玩家交易',' 买家<',(select cRoleName from trolebaseinfo where cRoleId =Field2 limit 1),'>坐标:',(select mapName from map where mapid = Field5 limit 1),' X:',CAST(Field6 as char(12)),' Y:',CAST(Field7 as char(12)),' 卖家<',(select cRoleName from trolebaseinfo where cRoleId =Field3 limit 1),'>坐标:',(select mapName from map where mapid = Field8 limit 1),' X:',CAST(Field9 as char(12)),' Y:',CAST(Field10 as char(12)))"
                            + " when 2 then CONCAT('所有者变更:收购',' 买家<',(select cRoleName from trolebaseinfo where cRoleId =Field2 limit 1),'>坐标:',(select mapName from map where mapid = Field5 limit 1),' X:',CAST(Field6 as char(12)),' Y:',CAST(Field7 as char(12)),' 卖家<',(select cRoleName from trolebaseinfo where cRoleId =Field3 limit 1),'>坐标:',(select mapName from map where mapid = Field8 limit 1),' X:',CAST(Field9 as char(12)),' Y:',CAST(Field10 as char(12)))"
                            + " when 3 then CONCAT('所有者变更:贩卖',' 买家<',(select cRoleName from trolebaseinfo where cRoleId =Field2 limit 1),'>坐标:',(select mapName from map where mapid = Field5 limit 1),' X:',CAST(Field6 as char(12)),' Y:',CAST(Field7 as char(12)),' 卖家<',(select cRoleName from trolebaseinfo where cRoleId =Field3 limit 1),'>坐标:',(select mapName from map where mapid = Field8 limit 1),' X:',CAST(Field9 as char(12)),' Y:',CAST(Field10 as char(12)))"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " (select Field30 from log_4_1 where log_4_1.field1 = log_4_5.field1 limit 1) as GoodsName,"
                            + " Field2 as userGUID,"
                            + " (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_5.Field2 limit 1) as userName,"
                            + " (select Field31 from log_4_1 where log_4_1.field1 = log_4_5.field1 limit 1) as ItemDetails"
                            + " from log_4_5"
                            + " where Field1 = '?'";

        string queryString6 = " select logtime as Time,"
                            + " (case field3"
                            + " when 1 then (case field15"
                            + " when 0 then '属性改变：玄晶剥离'"
                            + " else CONCAT('属性改变：强化装备至 +', field15)"
                            + " end)"
                            + " else '未知事件'"
                            + " end) as eventname,"
                            + " Field30 as GoodsName, Field2 as userGUID, (select cRoleName from trolebaseinfo where trolebaseinfo.cRoleId = log_4_6.Field2 limit 1) as userName, Field31 as ItemDetails"
                            + " from log_4_6"
                            + " where field1 = '?'";

        ArrayList queryStringsArrayList = new ArrayList();

        if (eventTypeCheckBoxList.Items.FindByValue("log_4_1").Selected == true)
            queryStringsArrayList.Add(queryString1);
        
        if (eventTypeCheckBoxList.Items.FindByValue("log_4_2").Selected == true)
            queryStringsArrayList.Add(queryString2);

        if (eventTypeCheckBoxList.Items.FindByValue("log_4_3").Selected == true)
            queryStringsArrayList.Add(queryString3);
        
        if (eventTypeCheckBoxList.Items.FindByValue("log_4_4").Selected == true)
            queryStringsArrayList.Add(queryString4);
        
        if (eventTypeCheckBoxList.Items.FindByValue("log_4_5").Selected == true)
            queryStringsArrayList.Add(queryString5);

        if (eventTypeCheckBoxList.Items.FindByValue("log_4_6").Selected == true)
            queryStringsArrayList.Add(queryString6);
        

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

                paramList.Add(itemGUID);

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
                paramList.Add(itemGUID);
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
            dt.Columns.Add(new DataColumn("itemName", typeof(String)));
            dt.Columns.Add(new DataColumn("ownerID", typeof(String)));
            dt.Columns.Add(new DataColumn("ownerRoleName", typeof(String)));
            dt.Columns.Add(new DataColumn("itemDetails", typeof(String)));
            dt.Columns.Add(new DataColumn("gameCenterID", typeof(String)));

            DataRow dr;
            
            queryResult.SetFieldType
                (
                    new SqlDataType[] 
                    {
                        SqlDataType.DateTime,
                        SqlDataType.String,
                        SqlDataType.String,
                        SqlDataType.String,
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
                dr[3] = (String)record[3];
                dr[4] = (String)record[4];
                dr[5] = (String)record[5];
                dr[6] = serverGroupDropDownList.SelectedValue;
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