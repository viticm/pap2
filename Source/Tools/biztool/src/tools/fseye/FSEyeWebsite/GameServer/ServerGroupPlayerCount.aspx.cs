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

public partial class GameServer_ServerGroupPlayerCount : System.Web.UI.Page
{
    int _groupId = 0;
    public int GetGroupId
    {
        get { return _groupId; }
    }
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            try
            {
                _groupId = int.Parse(Request.Params["groupId"]);
            }
            catch (Exception)
            {
                Response.Redirect("ServerOverviewPlayerCount.aspx");
            }
        }
    }
}
