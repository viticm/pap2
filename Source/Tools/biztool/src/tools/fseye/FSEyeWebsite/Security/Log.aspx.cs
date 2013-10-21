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
using FSEye.Security;
using Resources;

public partial class Security_Log : BasePage
{
    const int DefaultRecordPerPage = 50;

    int _recordPerPage = DefaultRecordPerPage;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityLog, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			StartTime.SelectedDate = DateTime.Today;
			EndTime.SelectedDate = DateTime.Now;
			//DataTable dt = new DataTable("TestTable");
			//dt.Columns.Add("ID", typeof(int));
			//dt.Columns.Add("Desc", typeof(string));
			//for (int i = 0; i < 100; i++)
			//{
			//    dt.Rows.Add(i, "TestData");
			//}
			//DataSet ds = new DataSet();
			//ds.Tables.Add(dt);

			//GridViewLog.DataSource = ds;
			//GridViewLog.DataBind();
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

	protected void ButtonQuery_Click(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityLog, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
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
		int operatorId = 0;
		int operation = 0;
		int targetId = 0;
		string description = TextBoxDescription.Text;

		try
		{
            //通过ID入Name查询
            operatorId = RadioButtonListOperator.SelectedIndex == 0 ? int.Parse(TextBoxOperator.Text) : 
                TheAdminServer.SecurityManager.GetUser(TextBoxOperator.Text).SecurityObject.Id;
		}
		catch (Exception)
		{
            //该用户不存在已被删除
		}

		try
		{
			if (CheckBoxRead.Checked)
				operation |= OpType.READ;

			if (CheckBoxWrite.Checked)
				operation |= OpType.WRITE;

			if (CheckBoxExecute.Checked)
				operation |= OpType.EXECUTE;
		}
		catch (Exception)
		{

		}

		try
		{
            if (RadioButtonListTarget.SelectedIndex == 0)
            {
                targetId = int.Parse(TextBoxTarget.Text);
            }
            else
            {
                foreach (FSEyeObject obj in TheAdminServer.SecurityManager.ObjectMap.Values)
                {
                    if (string.Equals(obj.Name, TextBoxTarget.Text, StringComparison.OrdinalIgnoreCase)) targetId = obj.Id;
                }
            }
		}
		catch (Exception)
		{
             //"该对象不存在或者已被删除"
		}


        DateTime startTime = StartTime.SelectedDate;
        DateTime endTime = EndTime.SelectedDate;

        //为动态效果做的延迟
        System.Threading.Thread.Sleep(500);

        DataTable table = LogSystem.TheInstance.QueryLog(operatorId, operation, targetId, description, startTime, endTime, offset, _recordPerPage);
		
		if (table != null && table.Rows.Count > 0)
		{
            PanelResult.Visible = true;

            ViewState[WebConfig.SessionQueryLogOffset] = offset;

            ButtonPreviousPage.Enabled = (offset > 0);
            ButtonFirstPage.Enabled = (offset > 0);
            ButtonNextPage.Enabled = (table.Rows.Count >= _recordPerPage);

            CreateDataTable(table);
		}
		else
		{
            PanelResult.Visible = false;
            LabelOpMsg.Text = StringDef.NoMatchingRecord;
		}
	}

    private void CreateDataTable(DataTable table)
    {
        foreach (DataRow dataRow in table.Rows)
        {
            TableRow row = new TableRow();
            TableCell cell;

            cell = new TableCell();
            int fsObjectId0 = int.Parse(dataRow[1].ToString());
            FSEyeObject fsObject0 = TheAdminServer.SecurityManager.Get(fsObjectId0);
            cell.Text = fsObject0 == null ? string.Format("{0}(ID)", fsObjectId0) : fsObject0.Name;
            row.Cells.Add(cell);

            cell = new TableCell();
            switch (int.Parse(dataRow[2].ToString()))
            {
                case OpType.READ:
                    cell.Text = "READ";
                    break;
                case OpType.WRITE:
                    cell.Text = "WRITE";
                    break;
                case OpType.EXECUTE:
                    cell.Text = "EXECUTE";
                    break;
            }
            row.Cells.Add(cell);

            cell = new TableCell();
            int fsObjectId = int.Parse(dataRow[3].ToString());
            FSEyeObject fsObject = TheAdminServer.SecurityManager.Get(fsObjectId);
            cell.Text = fsObject == null ? string.Format("{0}(ID)", fsObjectId) : fsObject.Name;
            row.Cells.Add(cell);

            cell = new TableCell();
            cell.Text = Encoding.Default.GetString(dataRow[4] as byte[]);
            row.Cells.Add(cell);

            cell = new TableCell();
            cell.Text = dataRow[5].ToString();
            row.Cells.Add(cell);

            TableLog.Rows.Add(row);
        }
    }
}