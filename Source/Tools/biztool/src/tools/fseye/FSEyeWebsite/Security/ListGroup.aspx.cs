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

public partial class Security_ListGroup : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityListGroup, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		CreateTableGroup();
	}

	private void CreateTableGroup()
	{
		Group[] groups = AdminServer.TheInstance.SecurityManager.GetAllGroups();
		Group group = null;
				
		for (int i = 0; i < groups.Length; i++)
		{
			group = groups[i];

			TableRow row = new TableRow();
			TableCell cell = new TableCell();
			cell.CssClass = CssConfig.TableCellNormal;
			HyperLink linkDeleteGroup = new HyperLink();
			linkDeleteGroup.NavigateUrl = "DeleteGroup.aspx?groupId=" + group.SecurityObject.Id;
			linkDeleteGroup.Text = StringDef.Delete;
			linkDeleteGroup.SkinID = "SmallButton";
			cell.Controls.Add(linkDeleteGroup);
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.CssClass = CssConfig.TableCellNormal;
			cell.Text = (i + 1).ToString();
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.CssClass = CssConfig.TableCellNormal;
			cell.Text = group.SecurityObject.Id.ToString();
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.CssClass = CssConfig.TableCellNormal;
			HyperLink linkEditGroup = new HyperLink();
			linkEditGroup.NavigateUrl = "EditGroup.aspx?groupId=" + group.SecurityObject.Id;
			linkEditGroup.Text = group.SecurityObject.Name;
			linkEditGroup.SkinID = "PlainText";
			cell.Controls.Add(linkEditGroup);
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.CssClass = CssConfig.TableCellNormal;
			cell.Text = group.SecurityObject.Comment;
			row.Cells.Add(cell);

			TableGroup.Rows.Add(row);
		}
	}
}
