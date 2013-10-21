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

public partial class Common_DateCalendar : System.Web.UI.UserControl
{
    public DateTime SelectedDate
    {
        set
        {
            DateTextBox.Text = value.ToString("yyyy-MM-dd");
            DateCalendarSelect.SelectedDate = value;
        }
        get {
            try
            {
                return DateTime.Parse(DateTextBox.Text);
            }
            catch (Exception e)
            {
                throw e;
            }            
        }
    }

    public bool Enable
    {
        set { TableTest.Enabled = value; }
        get { return TableTest.Enabled; }
    }

    public bool CheckBoxOn
    {
        set
        {
            if (value)
            {
                DateCheck.Visible = true;
                DateLabel.Visible = false;
                Set(false);
            }
            else
            {
                DateCheck.Visible = false;
                DateLabel.Visible = true;
            }
        }
        get { return DateCheck.Visible; }
    }

    public Unit FirstCellWidth
    {
        set {
            TableTest.Rows[0].Cells[0].Width = value;
        }
        get{
            return TableTest.Rows[0].Cells[0].Width ;
        }
    }


    public string DateLabelText
    {
        set {
            DateCheck.Text = value; 
            DateLabel.Text = value;
        }
        get { return DateLabel.Text; }
    }

    public bool Selected
    {
        set { 
            DateCheck.Checked = value;
            if (value)
            {
                Set(true);
            }
            else
            {
                Set(false);
            }
        }
        get { return DateCheck.Checked; }
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        //this.SelectedDate = DateTime.Today;
    }

    protected void Select(object sender, EventArgs e)
    {
        DateCalendarSelect.Visible = !DateCalendarSelect.Visible;
    }

    protected void SelectedChanged(object sender, EventArgs e)
    {
        DateTextBox.Text = DateCalendarSelect.SelectedDate.ToString("yyyy-MM-dd");
        DateCalendarSelect.Visible = false;
    }

    protected void Check_Click(object sender, EventArgs e)
    {
        if (DateCheck.Checked)
        {
            Set(true);
        }else{
            Set(false);
        }
    }

    private void Set(bool enable)
    {
        DateTextBox.Enabled = enable;
        ButtonSelect.Enabled = enable;
    }
}
