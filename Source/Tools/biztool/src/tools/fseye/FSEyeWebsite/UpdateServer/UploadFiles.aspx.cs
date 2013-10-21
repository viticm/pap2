//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : UpdateManagement
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 更新管理中的上传更新文件到更新服务器
//
//////////////////////////////////////////////////////////////////////

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
using FSEye;
using System.IO;
using Resources;
using FSEye.Security;
using FSEye.WebControls;

public partial class UpDateManagement_UpLoadFiles : BasePage
{
    static FileInfo[] fileInfos;
    protected void Page_Load(object sender, EventArgs e)
    {
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUploadFiles, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (!IsPostBack)
        {
            ListUploadedFiles();
            ListFTPServerType();
            ListFTPServer();
        }

        if (ViewState["Uploading"] != null)
        {
            bool uploading = (bool)ViewState["Uploading"];
            if (uploading)
                ShowUploadingStatus(null, null);
        }
    }

    protected void ButtonUploadToUpdateServer_Click(object sender, EventArgs e)
    {
        ArrayList fileList = new ArrayList();

        bool enableTimer = true;
        ShowUploadingStatusTimer.Enabled = false;

        ClearLabelMessage();

        if (CheckBoxListUploadedFile.SelectedIndex == -1)
        {
            ShowLabelMessage("没有选择上传的文件", MessageType.Failure);
            enableTimer = false;
            return;
        }

        if (FTPServerCheckBoxList.SelectedIndex == -1)
        {
            ShowLabelMessage("没有选择更新服务器", MessageType.Failure);
            enableTimer = false;
            return;
        }

        try
        {
            //需要有对此页面的执行权限
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUploadFiles, OpType.EXECUTE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            //禁用掉上传按钮，用户在上传过程中不允许再次上传
            ButtonUploadToUpdateServer.Enabled = false;

            for (int index = 0; index != fileInfos.Length; ++index)
            {
                ListItem item = CheckBoxListUploadedFile.Items[index] as ListItem;
                if (item.Selected) fileList.Add(fileInfos[index]);
            }

            //任务开始前清掉所有更新服务器的已完成任务信息
            foreach (ListItem serverItem in FTPServerCheckBoxList.Items)
            {
                FTPServer server = TheAdminServer.FTPClient.GetFTPServer(int.Parse(serverItem.Value));
                server.UploadedFiles.Clear();
                server.UploadedFilesStateTable.Clear();
            }

            foreach (ListItem serverItem in FTPServerCheckBoxList.Items)
            {
                if (serverItem.Selected)
                {
                    FTPServer server = TheAdminServer.FTPClient.GetFTPServer(int.Parse(serverItem.Value));                                        

                    if (server.UploadFiles.Count != 0)
                    {
                        server.AddToUploadFileList(fileList, OverWriteChoice.SelectedIndex == 0 ? false : true);
                        ShowLabelMessage(StringDef.AddToUploadFileList, MessageType.Success);
                    }
                    else
                    {
                        server.AddToUploadFileList(fileList, OverWriteChoice.SelectedIndex == 0 ? false : true);
                        if (server.UploadFile(server.UploadFiles))
                        {
                            ShowLabelMessage(StringDef.OperationSucceed, MessageType.Success);
                        }
                        else ShowLabelMessage(StringDef.OperationFail, MessageType.Failure);
                    }
                }
            }
        }
        catch (Exception ee)
        {
            ShowLabelMessage(ee.Message, MessageType.Failure);
        }
        finally
        {
            //启动timer，去定时刷新上传进度信息
            if (enableTimer)
            {
                if (ShowUploadingStatusTimer.Enabled == false)
                {
                    ShowUploadingStatus(null, null);
                    ShowUploadingStatusTimer.Enabled = true;

                    ViewState["Uploading"] = true;
                }
            }
        }
    }

    private void ListUploadedFiles()
    {
        CheckBoxListUploadedFile.Items.Clear();        
        TheAdminServer.ListUploadedFiles(out fileInfos);
        long totleFileLength = 0;
        if (fileInfos != null && fileInfos.Length > 0)
        {
            LabelUploadedNoFile.Visible = false;
            foreach(FileInfo file in fileInfos)
            {                
                totleFileLength += file.Length;
                CheckBoxListUploadedFile.Items.Add(new ListItem(file.Name,file.FullName));
            }
        }
        else
        {
            LabelUploadedNoFile.Visible = true;
        }
        string totleFileLengthText = string.Empty;
        if (totleFileLength < 1024)
        {
            totleFileLengthText = Decimal.Round((decimal)totleFileLength,2).ToString() + "B";
        }
        else if (totleFileLength < 1024 * 1024)
        {
            totleFileLengthText = Decimal.Round((decimal)totleFileLength / (decimal)1024,2).ToString() + "K";
        }
        else
        {
            totleFileLengthText = Decimal.Round(totleFileLength / (decimal)1024 / (decimal)1024,2) + "M";
        }
        LabelUploadedFilesInfo.Text = String.Format(StringDef.UploadFileDirSummary,fileInfos.Length,totleFileLengthText);
    }

    private void ListFTPServerType()
    {
        string text = String.Empty;

        foreach (FTPServer.FTPServerType serverType in Enum.GetValues(typeof(FTPServer.FTPServerType)))
        {
            switch (serverType)
            {
                case FTPServer.FTPServerType.ServerUpdate:
                    text = "服务端更新FTP";
                    break;
                case FTPServer.FTPServerType.ClientUpdate:
                    text = "客户端更新FTP";
                    break;
                case FTPServer.FTPServerType.ClientServerListFile:
                    text = "客户端ServerList更新FTP";
                    break;

                default:
                    break;
            }

            ListItem item = new ListItem(text, ((int)serverType).ToString());
            if (serverType == FTPServer.FTPServerType.ServerUpdate)
            {
                item.Selected = true;
                CheckBoxListFTPServerType.Items.Add(item);
            }
        }
    }

    private void ListFTPServer()
    {
        FTPServerCheckBoxList.Items.Clear();

        if (TheAdminServer.FTPClient.FTPServerList.Count == 0)
        {
            NoUpdateServer.Visible = true;
            return;
        }
        else
        {
            NoUpdateServer.Visible = false;
            foreach (FTPServer server in TheAdminServer.FTPClient.FTPServerList)
            {
                bool showServer = false;
                foreach (ListItem item in CheckBoxListFTPServerType.Items)
                {
                    if ((item.Selected == true) && (((int)server.FtpServerType).ToString() == item.Value))
                    {
                        showServer = true;
                        break;
                    }
                }

                if (showServer == true)
                {
                    string href = @"<a href = 'UpdateServerState.aspx?serverId=" + server.ID + "' style='text-decoration:none;'>" + server.Literal + "</a>";
                    ListItem item = new ListItem("[" + server.ID + "]" + href, server.ID.ToString());
                    FTPServerCheckBoxList.Items.Add(item);
                }
            }
            FTPServerCheckBoxList.ToolTip = StringDef.ClickToViewState;
        }
    }

    protected void CheckBoxListFTPServerType_OnSelectedIndexChanged(object sender, EventArgs e)
    {
        ListFTPServer();
    }

    protected void ButtonSelectAllFiles_Click(object sender, EventArgs e)
    {
        SetAllFiles(true);
    }

    protected void ButtonSelectNoFile_Click(object sender, EventArgs e)
    {
        SetAllFiles(false);
    }

    protected void ButtonSelectAllServers_Click(object sender, EventArgs e)
    {
        SetAllServers(true);
    }

    protected void ButtonSelectNoServer_Click(object sender, EventArgs e)
    {
        SetAllServers(false);
    }

    private void SetAllFiles(bool select)
    {
        foreach (ListItem item in CheckBoxListUploadedFile.Items)
        {
            item.Selected = select;
        }
    }

    private void SetAllServers(bool select)
    {
        foreach (ListItem item in FTPServerCheckBoxList.Items)
        {
            item.Selected = select;
        }
    }

    protected void ShowUploadingStatusTimer_OnTick(object sender, EventArgs e)
    {
        //什么也不做，因为函数ShowUploadingStatus已经在Page_Load的时候调用过了
    }

    protected void ShowUploadingStatus(object sender, EventArgs e)
    {
        TableHeaderRow header = new TableHeaderRow();
        TableCell[] head = new TableCell[6];
        for (int i = 0; i != 6; ++i)
            head[i] = new TableCell();
        head[0].Width = new Unit(30f, UnitType.Percentage);
        head[0].Text = StringDef.UpdateServer;
        head[1].Width = new Unit(30f, UnitType.Percentage);
        head[1].Text = StringDef.FileName;
        head[2].Width = new Unit(10f, UnitType.Percentage);
        head[2].Text = StringDef.Size;
        head[3].Width = new Unit(200, UnitType.Pixel);
        head[3].Text = StringDef.Process;
        head[4].Width = new Unit(10f, UnitType.Percentage);
        head[4].Text = StringDef.Option;
        head[5].Width = new Unit(10f, UnitType.Percentage);
        head[5].Text = StringDef.Operation;

        header.Cells.AddRange(head);
        ShowUploadingStatusTable.Rows.Add(header);
        
        foreach (ListItem serverItem in FTPServerCheckBoxList.Items)
        {
            FTPServer ftpServer = TheAdminServer.FTPClient.GetFTPServer(int.Parse(serverItem.Value));

            if (ftpServer.UploadedFiles.Count != 0)
            {
                foreach (FileInfo uploadedFile in ftpServer.UploadedFiles)
                {
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[5];
                    for (int i = 0; i != 5; ++i)
                        cell[i] = new TableCell();

                    cell[0].HorizontalAlign = HorizontalAlign.Center;
                    cell[0].Text = ftpServer.Literal + "(" + ftpServer.Address + ")";

                    cell[1].HorizontalAlign = HorizontalAlign.Center;
                    cell[1].Text = uploadedFile.Name;

                    cell[2].Text = GetFileLengthText(uploadedFile.Length);
                    cell[2].HorizontalAlign = HorizontalAlign.Center;

                    ProgressBar percentBar = new ProgressBar();
                    percentBar.Width = new Unit(200, UnitType.Pixel);
                    percentBar.Height = new Unit(20, UnitType.Pixel);
                    percentBar.Orientation = Orientation.Horizontal;
                    percentBar.CompletedCssClass = "ProgressBarCompleted";
                    percentBar.BorderWidth = new Unit(1);
                    percentBar.Height = new Unit(20, UnitType.Pixel);
                    percentBar.Progress = 1.0f;
                    cell[3].Controls.Add(percentBar);
                    cell[3].EnableTheming = false;

                    cell[4].Text = (bool)ftpServer.UploadedFilesStateTable[uploadedFile] ? StringDef.OverWrite : StringDef.Append;
                    cell[4].HorizontalAlign = HorizontalAlign.Center;

                    row.Cells.AddRange(cell);
                    ShowUploadingStatusTable.Rows.Add(row);
                }
            }

            if (ftpServer.UploadFiles.Count != 0)
            {
                for (int index = 0; index != ftpServer.UploadFiles.Count; ++index)
                {
                    FileInfo file = ftpServer.UploadFiles[index] as FileInfo;
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[6];
                    for (int i = 0; i != 6; ++i)
                        cell[i] = new TableCell();

                    cell[0].HorizontalAlign = HorizontalAlign.Center;
                    cell[0].Text = ftpServer.Literal + "(" + ftpServer.Address + ")";

                    cell[1].HorizontalAlign = HorizontalAlign.Center;
                    cell[1].Text = file.Name;

                    cell[2].Text = GetFileLengthText(file.Length);
                    cell[2].HorizontalAlign = HorizontalAlign.Center;

                    ProgressBar percentBar = new ProgressBar();
                    percentBar.Width = new Unit(200, UnitType.Pixel);
                    percentBar.Height = new Unit(20, UnitType.Pixel);
                    percentBar.Orientation = Orientation.Horizontal;
                    percentBar.ProcessedCssClass = "ProgressBarProcessed";
                    percentBar.IndicatorCssClass = "ProgressBarIndicator";
                    percentBar.CompletedCssClass = "ProgressBarCompleted";
                    percentBar.BorderWidth = new Unit(1);
                    percentBar.Height = new Unit(20, UnitType.Pixel);
                    percentBar.Progress = index == 0 ? ftpServer.CurrentProgress : 0;
                    cell[3].Controls.Add(percentBar);
                    cell[3].EnableTheming = false;

                    cell[4].Text = (bool)ftpServer.UploadFilesStateTable[file] ? StringDef.OverWrite : StringDef.Append;
                    cell[4].HorizontalAlign = HorizontalAlign.Center;

                    LinkButton link = new LinkButton();
                    link.OnClientClick = "return window.confirm('是否要取消上传？');";
                    link.Width = new Unit(80f, UnitType.Percentage);
                    link.CommandName = ftpServer.ID.ToString();
                    link.ID = "CancelUploadFile" + link.CommandName + index.ToString();                    
                    link.Click += new EventHandler(CancelUploadFile_Click);
                    link.Text = "取消上传";
                    cell[5].Controls.Add(link);

                    row.Cells.AddRange(cell);
                    ShowUploadingStatusTable.Rows.Add(row);
                }
            }
        }
    }

    protected void CancelUploadFile_Click(object sender, EventArgs e)
    {
        //需要对该页面有执行权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUploadFiles, OpType.EXECUTE, Session))
        {
            ShowLabelMessage("取消上传失败：没有足够的权限", MessageType.Failure);
            return;
        }

        LinkButton link = sender as LinkButton;

        int serverId = int.Parse(link.CommandName);
        FTPServer server = TheAdminServer.FTPClient.GetFTPServer(serverId);

        string cancel = "CancelUploadFile";

        int delId = int.Parse(link.ID.Substring(cancel.Length + link.CommandName.Length));

        //delId == 0说明这是正在上传的任务
        if (delId == 0)
        {            
            server.Abort = true;
        }
        //delId != 0说明这是在队列里等待上传的任务
        else
        {
            //先清掉UploadFilesStateTable里的记录
            server.UploadFilesStateTable.Remove(server.UploadFiles[delId]);

            server.UploadFiles.Remove(server.UploadFiles[delId]);
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

    public void ShowLabelMessage(string message, MessageType type)
    {
        Label messageLabel = UpdatePanel2.FindControl("LabelOpResultMessage") as Label;
        
        if (messageLabel != null)
        {
            messageLabel.Visible = true;
            messageLabel.Text = message;

            string cssClass = CssConfig.MessageNormal;
            switch (type)
            {
                case MessageType.Success:
                    cssClass = CssConfig.MessageSuccess;
                    break;
                case MessageType.Failure:
                    cssClass = CssConfig.MessageFailure;
                    break;
                case MessageType.Attention:
                    cssClass = CssConfig.MessageAttention;
                    break;
                case MessageType.Normal:
                    cssClass = CssConfig.MessageNormal;
                    break;
            }
            messageLabel.CssClass = cssClass;
        }
    }

    private void ClearLabelMessage()
    {
        Label messageLabel = UpdatePanel2.FindControl("LabelOpResultMessage") as Label;
        if (messageLabel != null) messageLabel.Visible = false;
    }
}
