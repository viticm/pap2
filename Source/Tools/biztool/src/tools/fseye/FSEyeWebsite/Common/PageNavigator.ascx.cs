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

public partial class Common_PageNavigator : System.Web.UI.UserControl
{
	/// <summary>
	/// Ò³Êý
	/// </summary>
	public int PageCount
	{
		get { return (int)ViewState["PageCount"]; }
		set
		{
			ViewState["PageCount"] = value;
			CreatePageList();
		}
	}

	//Ò³ÐòºÅ
	public int PageIndex
	{
		get { return (int)ViewState["PageIndex"]; }
		set
		{
			ViewState["PageIndex"] = value;
			RefreshState();
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!IsPostBack)
		{
			PageIndex = 0;
		}
	}

	void RefreshState()
	{
		ButtonPerviousPage.Enabled = (PageIndex > 0);
		ButtonNextPage.Enabled = (PageIndex + 1 < PageCount);
		ButtonFirstPage.Enabled = (PageIndex > 0);
		ButtonLastPage.Enabled = (PageIndex + 1 < PageCount);

		DropDownListPage.SelectedIndex = PageIndex;
	}

	void CreatePageList()
	{
		DropDownListPage.Items.Clear();
		for (int i = 0; i < PageCount; i++)
		{
			DropDownListPage.Items.Add((i + 1).ToString());
		}
	}

	protected void ButtonNextPage_Click(object sender, EventArgs e)
	{
		if (PageIndex + 1 < PageCount)
			PageIndex++;
	}

	protected void ButtonFirstPage_Click(object sender, EventArgs e)
	{
		if (PageCount > 0)
			PageIndex = 0;
	}

	protected void ButtonPreviousPage_Click(object sender, EventArgs e)
	{
		if (PageIndex > 0)
			PageIndex--;
	}

	protected void ButtonLastPage_Click(object sender, EventArgs e)
	{
		if (PageCount > 0)
			PageIndex = PageCount - 1;
	}

	protected void DropDownListPage_SelectedIndexChanged(object sender, EventArgs e)
	{
		PageIndex = DropDownListPage.SelectedIndex;
	}
}
