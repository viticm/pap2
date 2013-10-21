//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : GameServer
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 游戏服务器--文件列表页面
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
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;

public partial class GameServer_FileList : BasePage
{
    DataTable Store;    
    protected void Page_Load(object sender, EventArgs e)
    {
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerListFile, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            TextBoxPath.Text =  GameServer.FilePathDelimiter + GameServer.UploadFileDir + GameServer.FilePathDelimiter;
            ///DataGridResult.Items[0].Attributes.Add();
            /////ServerDropDownList.SelectedGameServer.GuardDir +
        }

        if (ViewState["Store"] != null) Store = ViewState["Store"] as DataTable;
        LabelOpMsg.Text = string.Empty;
    }

    protected void ListFile_Click(object sender,EventArgs e)
    {
        ListFile();
    }

    private void ListFile()
    {
        GameServer server = ServerDropDownList.SelectedGameServer;
        if (server != null)
        {
            //加入权限检查
            if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return;
            }
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            if (!server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRefreshUploadDir, TextBoxPath.Text))
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
            DataGridResult.DataSource = CreateDataSource(LoadFileInfo());
            if (DataGridResult.DataSource != null && (DataGridResult.DataSource as ICollection).Count != 0)
            {
                DataGridResult.Visible = true;
                DataGridResult.DataBind();
            }
            else
            {
                DataGridResult.Visible = false;
                LabelOpMsg.Text = StringDef.NoFiles;
                return;
            }
        }
        else
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
        }
    }

    /// <summary>
    /// 判断有无重名
    /// </summary>
    /// <param name="newFileName">新文件名</param>
    protected bool IsNamed(String newFileName)
    {
        foreach (DataGridItem item in DataGridResult.Items)
        {
            string fileName = item.Cells[3].Text;
            if(DataGridResult.EditItemIndex == item.ItemIndex)continue;
            if (newFileName.Equals(fileName, StringComparison.OrdinalIgnoreCase)) return true;
        }
        return false;
    }

    private ICollection CreateDataSource(IList fileList)
    {
        if(fileList==null||fileList.Count==0)return null;
        DataTable dataTable = new DataTable();
        DataRow dataRow;

        dataTable.Columns.Add(new DataColumn("FileName", typeof(String)));        
        dataTable.Columns.Add(new DataColumn("FileSize", typeof(String)));
        dataTable.Columns.Add(new DataColumn("ModifiedTime", typeof(String)));
        dataTable.Columns.Add(new DataColumn("FileAttribute", typeof(String)));

        foreach (GuardFile file in fileList)
        {
            dataRow = dataTable.NewRow();
            dataRow[0] = file.fileName;
            dataRow[1] = file.fileSize;
            dataRow[2] = file.modifideTime;
            dataRow[3] = file.attribute;

            dataTable.Rows.Add(dataRow);
        }
        DataView dataView = new DataView(dataTable);
        ViewState["Store"] = dataTable;
        ViewState[WebConfig.ParamServerId] = ServerDropDownList.SelectedServerId;
        ViewState["Path"] = TextBoxPath.Text;
        return dataView;
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void ServerIndexChanged(object sender, EventArgs e)
    {
        TextBoxPath.Text = ServerDropDownList.SelectedGameServer.GuardDir + GameServer.FilePathDelimiter + GameServer.UploadFileDir + GameServer.FilePathDelimiter;
        DataGridResult.Visible = false;
    }

    protected void DataGridResutl_Rename(Object sender, DataGridCommandEventArgs e)
    {
        DataGridResult.EditItemIndex = e.Item.ItemIndex;
        DataGridResult.DataSource = new DataView(Store);
        DataGridResult.DataBind();
    }

    protected void DataGridResult_Update(Object sender, DataGridCommandEventArgs e)
    {
        TextBox textBoxFileName = e.Item.Cells[4].Controls[0] as TextBox;
        if (textBoxFileName == null) return;
        string newFileName = textBoxFileName.Text;
        if (IsNamed(newFileName))
        {
            LabelOpMsg.Text = StringDef.FileNameUsed;
            return;
        }
        string path = ViewState["Path"] as string;       
        if (path[path.Length - 1].ToString() != GameServer.FilePathDelimiter)
            path += GameServer.FilePathDelimiter;
        string oldFileName = (ViewState["Store"] as DataTable).Rows[e.Item.ItemIndex].ItemArray[0] as string;
        GameServer server = TheAdminServer.GameServerManager.GetGameServer((int)ViewState[WebConfig.ParamServerId]);
        if (server != null)
        {
            if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return;
            }

            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRenemeUploadDirFile, path + oldFileName, path + newFileName))
            {
                //LabelOpMsg.Text = StringDef.OperationSucceed;
                DataGridResult.EditItemIndex = -1;
                System.Threading.Thread.Sleep(100);
                ListFile();
            }
            else LabelOpMsg.Text = StringDef.OperationFail;
        }
    }

    protected void DataGridResutl_Cancel(Object sender, DataGridCommandEventArgs e)
    {
        DataGridResult.EditItemIndex = -1;
        DataGridResult.DataSource = new DataView(Store);
        DataGridResult.DataBind();
    }

    protected void DataGridResult_Delete(Object sender, DataGridCommandEventArgs e)
    {
        string fileName = e.Item.Cells[4].Text;
        if (fileName != null && fileName.Length != 0)
        {
            GameServer server = TheAdminServer.GameServerManager.GetGameServer((int)ViewState[WebConfig.ParamServerId]);
            if (server != null)
            {
                if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                    return;
                }

                string path = ViewState["Path"] as string;
                if (path == null) return;
                if (path[path.Length - 1].ToString() != GameServer.FilePathDelimiter)
                    path += GameServer.FilePathDelimiter;
                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyDeleteUploadDirFile, path + fileName))
                {
                    //LabelOpMsg.Text = StringDef.OperationSucceed;
                    System.Threading.Thread.Sleep(100);
                    ListFile();
                }
                else LabelOpMsg.Text = StringDef.OperationFail;
            }
        }
    }

    protected void DataGridResult_ItemCommand(object sender, DataGridCommandEventArgs e)
    {
        switch (((LinkButton)e.CommandSource).CommandName)
        {
            case "Download":
                GameServer server = TheAdminServer.GameServerManager.GetGameServer((int)ViewState[WebConfig.ParamServerId]);
                if(server != null)
                {
                    if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                    {
                        LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                        return;
                    }
                    string fileName = e.Item.Cells[4].Text;
                    CheckBox check = e.Item.Cells[3].FindControl("CheckBoxOverWrite") as CheckBox;
                    if (check == null) return;
                    string path = ViewState["Path"] as string;
                    if (path[path.Length - 1].ToString() != GameServer.FilePathDelimiter)
                        path += GameServer.FilePathDelimiter;
                    server.AddFMTask(new FileDownloadTask(path + fileName, fileName, check.Checked));
                    LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.AddToFileList;
                }                
                break;
            default:
                break;
        }
    }

    protected void DataGridResult_ItemDataBound(object sender, DataGridItemEventArgs e)
    {
        if (e.Item.Cells[0].Controls.Count != 0)
        {
            LinkButton lb = e.Item.Cells[0].Controls[0] as LinkButton;
            if (lb != null)
                lb.Attributes.Add("onclick",
                    string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgDeleteConfirm, StringDef.File)));
        }
    }

    private IList LoadFileInfo()
    {
        GameServer server = ServerDropDownList.SelectedGameServer;
        string fileInfoString = string.Empty;
        int tryTimes = 0;
        while(fileInfoString == string.Empty)
        {
            //10秒超时
            ++tryTimes;
            if (tryTimes == 10) break;
            System.Threading.Thread.Sleep(1000);
            fileInfoString = server.GetPlugInData(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyUploadDirInfo) as string;
        }
        if (fileInfoString == string.Empty)
        {
            //获取文件列表失败
            LabelOpMsg.Text = StringDef.OperationFail;
            return null;
        }
        
        ArrayList fileList = new ArrayList();
        if (fileInfoString != null && fileInfoString.Length!=0)
        {
            string[] fileInfo = fileInfoString.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            for (int index = 1;index!=fileInfo.Length;++index)
            {
                if (fileInfo[index].Length == 0) continue;
                try
                {
                    GuardFile file = new GuardFile();
                    string[] fileUnits = fileInfo[index].Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    file.attribute = fileUnits[0];
                    file.fileSize = fileUnits[2];
                    file.modifideTime = fileUnits[3] + ' ' + fileUnits[4] + ' ' + fileUnits[5];

                    //文件名有空格的情况
                    for (int nameIndex = 6; nameIndex < fileUnits.Length; ++nameIndex)
                        file.fileName += fileUnits[nameIndex]+' ';
                    file.fileName = file.fileName.Substring(0, file.fileName.Length - 1);
                    
                    fileList.Add(file);
                }
                catch (Exception)
                {
                    continue;
                }
            }
        }
        return fileList;
    }

    //string oldPath = _server.GuardDir + GameServer.FilePathDelimiter + GameServer.UploadFileDir + GameServer.FilePathDelimiter
}

public class GuardFile
{
    public string fileName = string.Empty;
    public string fileSize = string.Empty;
    public string modifideTime = string.Empty;
    public string attribute = string.Empty;
    
    public GuardFile()
    { }
}