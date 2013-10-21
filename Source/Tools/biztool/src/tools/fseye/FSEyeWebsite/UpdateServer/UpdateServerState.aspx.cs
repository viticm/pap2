////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : UpdateManagement
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 更新管理--查看更新服务器文件页面
//                         包括更新服务器上文件的列表,上传文件列表及进度
//                         和文件删除功能.
//
/////////////////////////////////////////////////////////////////////////

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
using System.IO;
using Resources;
using FSEye;
using FSEye.WebControls;
using FSEye.Security;
using System.Threading;
using System.Text;

public partial class UpDateManagement_UpdateServerState : BasePage
{
    public const string ParamPath = "Path";
    const int RefreshServerFilesDelayTime = 1000;
    const int DeleteServerFileDelayTime = 333;

    int _updateServerId = 0;
    FTPServer server;
    protected void Page_Load(object sender, EventArgs e)
    {
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUpdateServerState, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (Request.Params["serverId"] != null && Request.Params["serverId"].Length != 0)
        {
            _updateServerId = int.Parse(Request.Params["serverId"]);
            server = AdminServer.TheInstance.FTPClient.GetFTPServer(_updateServerId);
        }

        Message.Text = String.Empty;  

        if (!IsPostBack)
        {
            ViewState[ParamPath] = String.Empty;
            ListServerFiles();
            Thread.Sleep(RefreshServerFilesDelayTime);
            GetServerFileList();
        }
   
        ListUploadFiles();
    }

    private void ListServerFiles()
    {
        try
        {
            if (server != null && server.UploadFiles != null)
            {
                string path = String.Empty;
                if (ViewState[ParamPath] != null)
                {
                    path = ViewState[ParamPath] as string;
                }
                server.ListFile(path);
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void GetServerFileList()
    {
        try
        {
            if (server != null)
            {
                DataGridServerFileList.DataSource = CreateDataSourceServerFiles(server.ServerFiles);
                DataGridServerFileList.DataBind();

                LiteralPath.Text = server.CurrentPath;
                Message.Text += server.Result + "<br />";
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private ICollection CreateDataSourceServerFiles(IList fileList)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Size", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Type", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Modified", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Attribute", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Owner", typeof(String)));

            if (fileList != null)
            {
                foreach (ServerFileInfo info in fileList)
                {
                    dataRow = dataTable.NewRow();
                    dataRow[0] = info.Name;
                    dataRow[1] = WebUtil.BetterSizeText(info.Size);
                    dataRow[2] = info.Directory ? StringDef.Directory : StringDef.File;
                    dataRow[3] = info.ModifyTime;
                    dataRow[4] = info.Attribute;
                    dataRow[5] = info.Owner;

                    dataTable.Rows.Add(dataRow);
                }
            }

            dataTable.DefaultView.Sort = "Type, Name";

            return dataTable.DefaultView;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());

            return null;
        }
    }

    protected void LinkButtonRefresh_Click(object sender, EventArgs e)
    {
        ListServerFiles();
        Thread.Sleep(RefreshServerFilesDelayTime);
        GetServerFileList();
    }

    protected void LinkButtonViewDir_Click(object sender, EventArgs e)
    {
        try
        {
            LinkButton lb = sender as LinkButton;
            if (lb != null)
            {
                DataGridItem item = lb.Parent.Parent as DataGridItem;
                if (item != null)
                {
                    if (item.Cells[7].Text.Equals(StringDef.Directory))
                    {
                        if (ViewState[ParamPath] != null)
                        {
                            string path = ViewState[ParamPath] as string;
                            path += lb.Text + WebConfig.UpdateServerPathDelimiter;
                            ViewState[ParamPath] = path;
                        }
                        else
                        {
                            ViewState[ParamPath] = WebConfig.UpdateServerPathDelimiter;
                        }
                        ListServerFiles();
                        Thread.Sleep(RefreshServerFilesDelayTime);
                        GetServerFileList();
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void LinkButtonUpFolder_Click(object sender, EventArgs e)
    {
        try
        {
            if (ViewState[ParamPath] != null)
            {
                string path = ViewState[ParamPath] as string;
                if (!path.Equals(WebConfig.UpdateServerPathDelimiter))
                {
                    string pathTrim = path.TrimEnd('/');
                    path = pathTrim.Substring(0, pathTrim.LastIndexOf('/') + 1);
                }

                ViewState[ParamPath] = path;
                ListServerFiles();
                Thread.Sleep(RefreshServerFilesDelayTime);
                GetServerFileList();
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void LinkButtonDeleteServerFile_Click(object sender, EventArgs e)
    {
        try
        {
            if (server != null)
            {
                //需要对该页面有执行权限
                if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUpdateServerState, OpType.EXECUTE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
                LinkButton lb = sender as LinkButton;
                if (lb != null)
                {
                    string path = ViewState[ParamPath] as string;
                    StringBuilder msgBuilder = new StringBuilder();
                    foreach (DataGridItem item in DataGridServerFileList.Items)
                    {
                        CheckBox checkBox = item.Cells[0].FindControl("CheckBoxServerFile") as CheckBox;
                        if (checkBox != null && checkBox.Checked)
                        {
                            LinkButton linkButtonFile = item.Cells[1].FindControl("LinkButtonViewDir") as LinkButton;
                            if (linkButtonFile != null)
                            {
                                string fileName = linkButtonFile.Text;
                                if (fileName != null && fileName.Length != 0)
                                {
                                    ServerFileInfo file = server.GetServerFileInfo(fileName);
                                    if (file != null)
                                    {
                                        if (file.Directory)
                                        {
                                            if (server.RemoveDir(server.RootUri + path + file.Name + WebConfig.UpdateServerPathDelimiter))
                                            {
                                                Thread.Sleep(DeleteServerFileDelayTime);
                                                msgBuilder.Append(string.Format("{0}[{1}]<br />", server.Result, file.Name));
                                            }
                                            else
                                            {
                                                msgBuilder.Append(string.Format("[删除目录[{0}]失败]{1}<br />", file.Name, server.Result));
                                            }
                                        }
                                        else
                                        {
                                            if (server.DeleteFile(path + fileName))
                                            {
                                                Thread.Sleep(DeleteServerFileDelayTime);
                                                msgBuilder.Append(string.Format("{0}[{1}]<br />", server.Result, file.Name));
                                            }
                                            else
                                            {
                                                msgBuilder.Append(string.Format("[删除文件[{0}]失败]{1}<br />", file.Name, server.Result));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Message.Text += msgBuilder.Length == 0 ? StringDef.OperationSucceed : msgBuilder.ToString();
                    Message.Text += "<br />";
                    ListServerFiles();
                    Thread.Sleep(RefreshServerFilesDelayTime);
                    GetServerFileList();
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void CheckBoxSelectAll_CheckedChanged(object sender, EventArgs e)
    {
        CheckBox checkBox = sender as CheckBox;
        if (checkBox != null)
        {
            bool check = checkBox.Checked;
            foreach (DataGridItem item in DataGridServerFileList.Items)
            {
                CheckBox checkBoxItem = item.FindControl("CheckBoxServerFile") as CheckBox;
                if (checkBoxItem != null)
                {
                    checkBoxItem.Checked = check;
                }
            }
        }
    }

    protected void Timer2_Tick2(object sender, EventArgs e)
    {
        MessageUploadFile.Text = string.Empty;
    }

    private void ListUploadFiles()
    {
        if (server.UploadFiles.Count != 0)
        {
            TableHeaderRow header = new TableHeaderRow();
            //header.CssClass = "TableRowHeader";
            TableCell[] head = new TableCell[5];
            for (int i = 0; i != 5; ++i) head[i] = new TableCell();
            head[0].Width = new Unit(10f, UnitType.Percentage);
            head[0].Text = StringDef.Operation;
            head[1].Width = new Unit(35f, UnitType.Percentage);
            head[1].Text = StringDef.FileName;
            head[2].Width = new Unit(20f, UnitType.Percentage);
            head[2].Text = StringDef.Option;            
            head[3].Width = new Unit(20f, UnitType.Percentage);
            head[3].Text = StringDef.Size;
            //head[4].Width = new Unit(20f, UnitType.Percentage);
            head[4].Width = new Unit(150,UnitType.Pixel);
            head[4].Text = StringDef.Process;
            header.Cells.AddRange(head);
            UploadFileList.Rows.Add(header);

            for (int index = 0; index != server.UploadFiles.Count; ++index)
            {
                FileInfo file = server.UploadFiles[index] as FileInfo;
                TableRow row = new TableRow();                
                TableCell[] cell = new TableCell[5];
                for (int i = 0; i != 5; ++i) cell[i] = new TableCell();
                LinkButton link = new LinkButton();
                link.OnClientClick = "return removeConfirm();";
                link.Width = new Unit(80f, UnitType.Percentage);
                link.ID = "UploadFile" + index.ToString();
                link.Click += new EventHandler(UploadFileDelete_Click);
                link.Text = StringDef.Delete;
                cell[0].Controls.Add(link);
                cell[0].HorizontalAlign = HorizontalAlign.Center;
                cell[0].Controls.Add(link);
                cell[1].HorizontalAlign = HorizontalAlign.Left;
                cell[1].Text = file.Name;
                cell[1].EnableTheming = false;
                cell[2].Text = (bool)server.UploadFilesStateTable[file] ? StringDef.OverWrite : StringDef.Append;
                cell[2].HorizontalAlign = HorizontalAlign.Center;
                cell[3].Text = GetFileLengthText(file.Length);
                cell[3].HorizontalAlign = HorizontalAlign.Center;
                ProgressBar percentBar = new ProgressBar();
                percentBar.Width = new Unit(150, UnitType.Pixel);
                percentBar.Height = new Unit(20, UnitType.Pixel);
                percentBar.Orientation = Orientation.Horizontal;
                //percentBar.CssClass = "ProgressBarBorder";
                percentBar.ProcessedCssClass = "ProgressBarProcessed";
                percentBar.IndicatorCssClass = "ProgressBarIndicator";
                percentBar.CompletedCssClass = "ProgressBarCompleted";
                percentBar.BorderWidth = new Unit(1);
                percentBar.Height = new Unit(20, UnitType.Pixel);
                percentBar.Progress = index==0?server.CurrentProgress:server.GetFileProcess(file);
                cell[4].Controls.Add(percentBar);
                cell[4].EnableTheming = false;
                //cell[4].VerticalAlign = VerticalAlign.Middle;
                //cell[4].HorizontalAlign = HorizontalAlign.Left;
                row.Cells.AddRange(cell);
                UploadFileList.Rows.Add(row);
            }
        }
        else
        {
            TableRow row = new TableRow();
            TableCell cell = new TableCell();
            cell.Text = StringDef.NoUploadFiles;
            row.Cells.Add(cell);
            UploadFileList.Rows.Add(row);
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

    protected void UploadFileDelete_Click(object sender, EventArgs e)
    {
        //需要对该页面有执行权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUpdateServerState, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
        LinkButton link = sender as LinkButton;
        int delId = int.Parse(link.ID.Substring(10));
        if (delId == 0)
        {
            //Abort
            server.Abort = true;
            //ShowMessage(StringDef.Processing, MessageType.Success);
        }
        else
        {
            //先清掉UploadFilesStateTable里的记录
            server.UploadFilesStateTable.Remove(server.UploadFiles[delId]);
            
            server.UploadFiles.Remove(server.UploadFiles[delId]);
            //ShowMessage(StringDef.Processing, MessageType.Success);
        }
        MessageUploadFile.Text = StringDef.Processing;
    }
}