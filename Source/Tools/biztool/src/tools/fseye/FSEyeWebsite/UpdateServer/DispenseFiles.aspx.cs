using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Net;
using System.IO;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using Resources;
using FSEye;
using FSEye.Security;   //TODO:这个类没有序列化，需要重写 huangbiao 2008.3
using FSEye.PlugIn;

public partial class UpdateServer_DispenseFiles : BasePage
{
    FTPServer ftpServer;

    protected void Page_Load(object sender, EventArgs e)
    {
        ////if (ViewState["serverId"] != null)
        ////{
        ////    _updateServerId = int.Parse(ViewState["serverId"].ToString());
        ////    ftpServer = AdminServer.TheInstance.FTPClient.GetFTPServer(_updateServerId);
        ////    ////需要对该UpdateServer有Read的权限
        ////    //if (!WebUtil.CheckPrivilege(ftpServer.SecurityObject, OpType.READ, Session))
        ////    //{
        ////    //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        ////    //}
        ////}

        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerDispenseFiles, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }     


        if (!Page.IsPostBack)
        {
            LabelOpMsg.Text = "";
            ListFTPServer();
        }
        ////ListServerFiles();
    }



    private void ListFTPServer()
    {
        string strFtp="";
        //if (Request.Params["strFtp"] != null && Request.Params["strFtp"].Length != 0)
        //{
        //    strFtp = Request.Params["strFtp"].ToString();
        //}
        if (ViewState["serverId"] != null && ViewState["serverId"].ToString().Length != 0)
        {
            strFtp = ViewState["serverId"].ToString();
        }
        if (TheAdminServer.FTPClient.FTPServerList.Count == 0)
        {
        }
        else
        {
            foreach (FTPServer server in TheAdminServer.FTPClient.FTPServerList)
            {
                ListItem item = new ListItem("[" + server.ID + "]" + server.Literal, server.ID.ToString());
                DropDownListFTPServer.Items.Add(item);
                if (item.Value == strFtp)
                    item.Selected = true;
            }
        }
    }

    protected void ListServerFiles()
    {
        if (ftpServer != null)
        {
            if (ftpServer.Result != null)
            {
                if (ftpServer.Result.Trim() != "")
                {
                    Timer1.Enabled = false;
                    LabelOpMsg.Text = ftpServer.Result;
                    ftpServer.Result = "";
                }
            }

            if (ftpServer.ServerFiles.Count != 0)
            {
                TableHeaderRow header = new TableHeaderRow();
                TableHeaderCell[] head = new TableHeaderCell[7];
                for (int i = 0; i != 7; ++i) head[i] = new TableHeaderCell();
                head[0].Text = StringDef.Operation;
                head[1].Width = new Unit(30f, UnitType.Percentage);
                head[1].Text = StringDef.FileName;// "文件名";
                head[2].Width = new Unit(10f, UnitType.Percentage);
                head[2].Text = StringDef.Size;// "大小";
                head[3].Width = new Unit(10f, UnitType.Percentage);
                head[3].Text = StringDef.Type;// "类型";
                head[4].Width = new Unit(20f, UnitType.Percentage);
                head[4].Text = StringDef.ModifiedTime;// "修改时间";
                head[5].Width = new Unit(10f, UnitType.Percentage);
                head[5].Text = StringDef.Attribute;// "属性";
                head[6].Width = new Unit(10f, UnitType.Percentage);
                head[6].Text = StringDef.Owner;// "所有者";
                header.Cells.AddRange(head);
                ServerFileList.Rows.Add(header);

                int id = 0;
                foreach (ServerFileInfo file in ftpServer.ServerFiles)
                {
                    if (file.Name.Equals(".") || file.Name.Equals("..")) continue;
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[7];
                    for (int i = 0; i != 7; ++i) cell[i] = new TableCell();
                    LinkButton link = new LinkButton();
                    link.OnClientClick = string.Format("return deleteConfirm('{0}');", string.Format(StringDef.MsgDeleteConfirm, StringDef.File));
                    link.SkinID = "PlainText";
                    link.ID = "ServerFile" + id.ToString();
                    link.Click += new EventHandler(ServerFileDelete_Click);
                    link.Text = StringDef.Delete;
                    cell[0].Controls.Add(link);


                    //if (file.Directory)
                    //{
                    Image img = new Image();
                    img.ImageUrl = file.Directory ? "~/Image/Folder.jpg" : "~/Image/File.jpg";
                        img.Height = new Unit(15, UnitType.Pixel);
                        cell[1].VerticalAlign = VerticalAlign.Bottom;
                        cell[1].Controls.Add(img);
                        LinkButton linkFolder = new LinkButton();
                        linkFolder.SkinID = "PlainText";
                        linkFolder.ID = "LinkFolder" + id.ToString();
                        linkFolder.OnClientClick = file.Directory ? "LinkButtonClick('" + file.Name + "/')" : "LinkButtonClick('" + file.Name + "')"; 
                        linkFolder.Text = file.Name;
                        cell[1].Controls.Add(linkFolder);
                    //}
                    //else
                    //{
                    //    Image img = new Image();
                    //    //img.ImageUrl = file.Directory ? "~/Image/Folder.jpg" : "~/Image/File.jpg";
                    //    img.ImageUrl = "~/Image/File.jpg";
                    //    img.Height = new Unit(15, UnitType.Pixel);
                    //    cell[1].VerticalAlign = VerticalAlign.Bottom;
                    //    cell[1].Controls.Add(img);
                    //    Literal name = new Literal();
                    //    //name.Text = file.Directory ? "<a href='DispenseFiles.aspx?strFtp=" + ftpServer.ID + "&strPath=" + file.Name + "'>" + file.Name + "</a>" : file.Name;
                    //    name.Text = file.Name;
                    //    cell[1].Controls.Add(name);
                    //}


                    cell[1].HorizontalAlign = HorizontalAlign.Left;

                    cell[2].HorizontalAlign = HorizontalAlign.Center;
                    cell[2].Text = GetFileLengthText(file.Size);
                    cell[3].HorizontalAlign = HorizontalAlign.Center;
                    cell[3].Text = file.Directory ? StringDef.Folder : StringDef.File;
                    cell[4].HorizontalAlign = HorizontalAlign.Right;
                    cell[4].Text = file.ModifyTime;
                    cell[5].HorizontalAlign = HorizontalAlign.Center;
                    cell[5].Text = file.Attribute;
                    cell[6].HorizontalAlign = HorizontalAlign.Center;
                    cell[6].Text = file.Owner;
                    row.Cells.AddRange(cell);
                    ServerFileList.Rows.Add(row);
                    id++;
                }
            }
        }
    }

    protected void Timer1_Tick1(object sender, EventArgs e)
    {        
        int iNum = 0;
        if (ViewState["timeout"] != null)
            iNum = int.Parse(ViewState["timeout"].ToString());
        if (iNum == 0)
        {
            Timer1.Enabled = false;
            LabelOpMsg.Text = StringDef.QueryTimeOut;
        }
        else
        {            
            //if ((iNum % 5) == 0 && LabelOpMsg.Text != "ok")
            //    ftpServer.ListFile(txtHiddenFolderName.Text.Trim());
            ViewState["timeout"] = int.Parse(ViewState["timeout"].ToString()) - 1;
        }   

        ftpServer = AdminServer.TheInstance.FTPClient.GetFTPServer(int.Parse(DropDownListFTPServer.SelectedValue));
        ftpServer.ListFile(txtHiddenFolderName.Text.Trim());
        ListServerFiles();
    }

    protected void LinkFolder_Click()
    {
        if (ftpServer != null)
        {
            LabelOpMsg.Text = StringDef.Waiting;
            ftpServer.ListFile(txtHiddenFolderName.Text.Trim());
            Timer1.Enabled = true;
        }
    }

    protected void ServerFileDelete_Click(object sender, EventArgs e)
    {
        //需要对该页面有执行权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerDispenseFiles, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
        
        LinkButton link = sender as LinkButton;
        int delId = int.Parse(link.ID.Substring(10));
        string fileName = (ServerFileList.Rows[delId + 1].Cells[1].Controls[1] as Literal).Text;
        //string fileName = ServerFileList.Rows[delId + 1].Cells[1].Text;
        ServerFileInfo file = ftpServer.GetServerFileInfo(fileName);
        if (ftpServer != null && file != null)
        {

            if (file.Directory)
            {
                if (ftpServer.RemoveDir(ftpServer.RootUri + file.Name + "/")) LabelOpMsg.Text = StringDef.Processing;
                else LabelOpMsg.Text = (StringDef.OperationFail + ftpServer.Result);
            }
            else
            {
                LabelOpMsg.Text = StringDef.Processing;
                if (ftpServer.DeleteFile(fileName)) LabelOpMsg.Text = StringDef.Processing;
                else LabelOpMsg.Text = (StringDef.OperationFail + ftpServer.Result);
            }

            ftpServer.ListFile();
        }
    }

    private string GetFileLengthText(long length)
    {
        string fileLengthText = string.Empty;
        if (length < 1024)
        {
            fileLengthText = Decimal.Round((decimal)length, 2).ToString() + " bytes";
        }
        else if (length < 1024 * 1024)
        {
            fileLengthText = Decimal.Round((decimal)length / (decimal)1024, 2).ToString() + " KB";
        }
        else
        {
            fileLengthText = Decimal.Round((decimal)length / (decimal)1024 / (decimal)1024, 2).ToString() + " MB";
        }
        return fileLengthText;
    }

    protected void ButtonRefresh_Click(object sender, EventArgs e)
    {
        //这是原来的显示方式
        ////LabelOpMsg.Text = StringDef.Waiting;
        //////TextBoxRefresh.Text = 0; //表示点了一次刷新按钮
        ////ViewState["timeout"] = 10;
        ////ViewState["serverId"] = DropDownListFTPServer.SelectedValue;
        
        ////Timer1.Enabled = true;
        //////ListServerFiles(); 
        LabelOpMsg.Text = StringDef.Waiting;
        if (DropDownListFTPServer.SelectedValue != "")
        {
            ftpServer = AdminServer.TheInstance.FTPClient.GetFTPServer(int.Parse(DropDownListFTPServer.SelectedValue));
            string folder = txtHiddenFolderName.Text.Trim();
            if (folder.EndsWith(@"/")==false && folder.Length > 0 )
            {
                folder = folder.Substring(0, folder.LastIndexOf("/") + 1);
                txtHiddenFolderName.Text = folder;
            }
            ftpServer.ServerFiles.Clear();
            ftpServer.Result = "";
            ftpServer.ListFile(folder);
            ListServerFiles();
            Timer1.Enabled = true;
        }
    }

    protected void ButtonUpFolder_Click(object sender, EventArgs e)
    {
        LabelOpMsg.Text = "";
        string txtFolder = txtHiddenFolderName.Text.Trim();
        if (txtFolder.Length > 0)
        {
            LabelOpMsg.Text = StringDef.Waiting;
            if (txtFolder.EndsWith("/"))
            {
                //最后是目录
                txtFolder = txtFolder.Substring(0, txtFolder.Length - 1);
            }
            else
            {
                //最后是文件
                txtFolder = txtFolder.Substring(0, txtFolder.LastIndexOf("/"));
            }
                
            txtHiddenFolderName.Text = txtFolder.Substring(0, txtFolder.LastIndexOf("/") == -1 ? 0 : txtFolder.LastIndexOf("/") + 1);
            ViewState["timeout"] = 10;

            ftpServer = AdminServer.TheInstance.FTPClient.GetFTPServer(int.Parse(DropDownListFTPServer.SelectedValue));
            ftpServer.ListFile(txtHiddenFolderName.Text);
            Timer1.Enabled = true;
            //ListServerFiles();
        }
    }

    protected void buttonNextStep_Click(object sender, EventArgs e)
    {
        if (txtHiddenFolderName.Text.EndsWith(".tar.gz"))
        {
            Page.Response.Redirect("DispenseFiles2.aspx?serverId=" + DropDownListFTPServer.SelectedValue+ "&updatePak=" + txtHiddenFolderName.Text);
        }
        else
        {
            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone ,StringDef.PatchName + "*.tar.gz");
        }
    }
    protected void txtHiddenFolderName_TextChanged(object sender, EventArgs e)
    {
        //if (txtHiddenFolderName.Text.Trim().EndsWith("/"))
        //{
        //    LinkFolder_Click();
        //    ftpServer.ServerFiles.Clear();
        //}       
    }
}
