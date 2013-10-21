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

public partial class Common_ScriptCalendar : System.Web.UI.UserControl
{
    public string CheckBoxToolTip
    {
        get { return CheckBoxSelect.ToolTip; }
        set { CheckBoxSelect.ToolTip = value; }
    }

    public DateTime SelectedDate
    {
        get
        {
            DateTime date = DateTime.MinValue;
            try
            {                
                date = DateTime.Parse(TextBoxDate.Text);
            }
            catch (Exception)
            {
            }
            return date;
        }
        set
        {
            TextBoxDate.Text = value.ToString("yyyy-MM-dd HH:mm:ss");
            calendarButtonExtender.SelectedDate = value;
        }
    }

    public bool ShowCheckBox
    {
        set
        {
            CheckBoxSelect.Visible = value;
            TextBoxDate.Enabled = !value;
            ButtonDate.Enabled = !value;
        }
        get { return CheckBoxSelect.Visible; }
    }

    public bool Enabled
    {
        set
        {
            TextBoxDate.Enabled = value;
            ButtonDate.Enabled = value;
        }
        get { return TextBoxDate.Enabled; }
    }

    public bool Selected
    {
        get { return CheckBoxSelect.Checked; }
    }

    protected void Page_Load(object sender, EventArgs e)
    {
    }

    protected void CheckBoxSelect_CheckedChanged(object sender, EventArgs e)
    {
        TextBoxDate.Enabled = CheckBoxSelect.Checked;
        ButtonDate.Enabled = CheckBoxSelect.Checked;        
    }

    public bool TextBoxReadOnly
    {
        set
        {
            TextBoxDate.ReadOnly = value;
        }
        get
        {
            return TextBoxDate.ReadOnly;
        }
    }
}
