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
using FSEye.FS2Lib;

public partial class Common_ItemIdSearch : System.Web.UI.UserControl
{
    public string ItemId
    {
        get { return TextBoxItemId.Text; }
    }

    public TextBox TextBoxItemName
    {
        get { return this.ItemName; }
    }

    public RadioButtonList RadioButtonListItemId
    {
        get { return this.ListItemId; }
    }

    protected void Page_Load(object sender, EventArgs e)
    {

    }

    protected void ButtonSearchItemId_Click(object sender, EventArgs e)
    {
        ListItemId.Items.Clear();
        string itemName = ItemName.Text;
        if (itemName == null || itemName.Trim().Length == 0)
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.ItemName);
            return;
        }
        IList items = FS2GameDataManager.TheInstance.GetItemId(itemName);
        if (items != null && items.Count != 0)
        {
            foreach (FS2ItemData item in items)
            {
                ListItem listItem = new ListItem(string.Format("{0}[{1}]", item.Name, item.TemplateId), item.TemplateId);
                ListItemId.Items.Add(listItem);
            }
            LabelSearchId.Visible = false;
        }
        else
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = StringDef.NoInfo;
        }
    }

    protected void SelectedItemIdChanged(object sender, EventArgs e)
    {
        if (ListItemId.SelectedIndex != -1)
            TextBoxItemId.Text = ListItemId.SelectedValue;
    }
}