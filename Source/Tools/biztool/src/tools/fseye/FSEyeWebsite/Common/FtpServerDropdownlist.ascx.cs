//////////////////////////////////////////////////////////////////////
//
//      Kingsoft 
//
//      Created_datetime : 2008
//      File_base        : GameServer
//      File_ext         : cs
//      Author           : huangbiao
//      Description      : FTP server 列表下拉控件
//
//////////////////////////////////////////////////////////////////////
using FSEye;
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

public partial class Common_FtpServerDropdownlist : System.Web.UI.UserControl
{
    /// <summary>
	/// 选中服务器组改变事件
	/// </summary>
    public event EventHandler SelectFTPChanged;

    public bool AutoPostBack
    {
        get { return FtpServerList.AutoPostBack; }
        set { FtpServerList.AutoPostBack = value; }
    }

    public bool Enabled
    {
        get { return FtpServerList.Enabled; }
        set { FtpServerList.Enabled = value; }
    }

    public int SelectFtp
    {
        get {
            if (FtpServerList.SelectedValue == null || FtpServerList.SelectedValue == string.Empty)
                return -1;
            else
                return int.Parse(FtpServerList.SelectedValue); 
        }
        set
        { FtpServerList.SelectedValue = value.ToString(); }
        //    foreach (ListItem item in FtpServerList.Items)
        //    {
        //        if (item.Value == value.ToString()) item.Selected = true;
        //    }
        //}
    }

    #region 事件&代理
    public delegate void SelectedIndexChangedEventHandler(object sender, EventArgs e);
    #endregion

    protected void Page_Init(object sender, EventArgs e)
    {
        if (!IsPostBack)
            ListFtp();
    }

    private void ListFtp()
    {
        FtpServerList.Items.Clear();
        FtpServerList.Items.Add(new ListItem(StringDef.None, "-1"));
        foreach (FTPServer server in AdminServer.TheInstance.FTPClient.FTPServerList)
        {
            ListItem item = new ListItem("[" + server.ID + "]" + server.Literal, server.ID.ToString());
            FtpServerList.Items.Add(item);
        }
        //if (FtpServerList.Items.Count > 0) FtpServerList.Items[0].Selected = true;
    }

    public void ListFtpByType(FTPServer.FTPServerType type)
    {
        FtpServerList.Items.Clear();
        FtpServerList.Items.Add(new ListItem(StringDef.None, "-1"));
        foreach (FTPServer server in AdminServer.TheInstance.FTPClient.FTPServerList)
        {
            if (server.FtpServerType == type)
            {
                ListItem item = new ListItem("[" + server.ID + "]" + server.Literal, server.ID.ToString());
                FtpServerList.Items.Add(item);
            }
        }
    }

    protected void SelectedIndexChanged_Click(object sender, EventArgs e)
    {
        if (this.SelectFTPChanged != null)
        {
            this.SelectFTPChanged(sender, e);
        }
    }
}
