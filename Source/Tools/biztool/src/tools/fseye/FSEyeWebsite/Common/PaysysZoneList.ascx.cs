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
using System.Collections.Generic;
using FSEye.PaySysLib;
using FSEye;

public partial class Common_PaysysZoneList : System.Web.UI.UserControl
{
    public string SelectedZone
    {
        get { return DropDownListZone.SelectedValue; }
    }

    protected void Page_Init(object sender, EventArgs e)
    {
        if(!IsPostBack)
        {
            IList<GatewayInfo> list = AdminServer.TheInstance.PaySysAgent.GatewayList;
            if (list != null && list.Count != 0)
            {
                string lastZoneName = string.Empty;
                foreach (GatewayInfo info in list)
                {
                    //去掉名字一样的
                    if (lastZoneName.Equals(info.ZoneName)) continue;
                    ListItem item = new ListItem(info.ZoneName, info.ZoneName);
                    DropDownListZone.Items.Add(item);
                    lastZoneName = info.ZoneName;
                }
            }
        }
    }
}
