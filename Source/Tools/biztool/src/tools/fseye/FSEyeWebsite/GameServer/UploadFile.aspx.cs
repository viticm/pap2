using System;
using System.Data;
using System.Configuration;
using System.Collections.Generic;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Diagnostics;
using System.IO;
using System.Text;
using Brettle.Web.NeatUpload;
using Resources;
using FSEye;
using FSEye.Security;

public partial class GameServer_UploadFile : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerUploadFile, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			ListUploadedFiles();            
		}
        ClearMessage();
        LabelOpMsg.Text = string.Empty;
	}

	protected void ButtonUpload_Click(object sender, EventArgs e)
	{
        if (UploadFile1.HasFile)
        {
            UploadFile1.MoveTo(Path.Combine(SystemConfig.Current.AdminServerUploadFileRootPath, UploadFile1.FileName), MoveToOptions.Overwrite);
            LiteralFileName.Text = StringDef.FileName + StringDef.Colon + UploadFile1.FileName;
            LiteralMD5Hash.Text = StringDef.MD5HashCode + StringDef.Colon + WebUtil.GetHashCodeString(UploadFile1.Hash);
            ListUploadedFiles();
        }
	}

	private void ListUploadedFiles()
	{
		CheckBoxListUploadedFile.Items.Clear();
		FileInfo[] fileInfos;
		TheAdminServer.ListUploadedFiles(out fileInfos);
		long totleFileLength = 0;

		if (fileInfos != null && fileInfos.Length > 0)
		{
			LabelUploadedNoFile.Visible = false;
			foreach (FileInfo fi in fileInfos)
			{
				fi.Refresh();

				totleFileLength += fi.Length;
				StringBuilder fileInfoText = new StringBuilder();

				fileInfoText.Append(fi.Name);
				fileInfoText.Append(" ");

				fileInfoText.AppendFormat("[{0}]", WebUtil.BetterSizeText(fi.Length));
				fileInfoText.AppendFormat(" [{0}] ", fi.LastWriteTime);

                fileInfoText.Append(string.Format("[<a href = 'GameServerHandler.ashx?{0}={1}&{2}={3}' target='_blank'>", WebConfig.ParamOperation, "GetFSEyeFile", WebConfig.ParamFileName, Convert.ToBase64String(Encoding.Default.GetBytes(fi.Name))) + StringDef.Download + "</a>]");

				CheckBoxListUploadedFile.Items.Add(new ListItem(fileInfoText.ToString(),fi.Name));
			}
		}
		else
		{
			LabelUploadedNoFile.Visible = true;
		}

		string totleFileLengthText = WebUtil.BetterSizeText(totleFileLength);
		LabelUploadedFilesInfo.Text = String.Format(StringDef.UploadFileDirSummary, fileInfos.Length, totleFileLengthText);
	}
        protected void LinkButtonRefresh_Click(object sender, EventArgs e)
    {
        ListUploadedFiles();
    }

	protected void ButtonSelectAllFiles_Click(object sender, EventArgs e)
	{
		SetAllFiles(true);
	}

	protected void ButtonSelectNoFile_Click(object sender, EventArgs e)
	{
		SetAllFiles(false);
	}

	protected void ButtonDeleteSelectedFiles_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in CheckBoxListUploadedFile.Items)
		{
			if (item.Selected)
			{
				TheAdminServer.DeleteUploadedFile(item.Value);
			}
		}
		ListUploadedFiles();
	}

	private void SetAllFiles(bool select)
	{
		foreach (ListItem item in CheckBoxListUploadedFile.Items)
		{
			item.Selected = select;
		}
	}

    protected void ButtonMD5Files_Click(object sernder, EventArgs e)
    {
        ResultList.Visible = true;
        foreach (ListItem item in CheckBoxListUploadedFile.Items)
        {
            if (item.Selected)
            {
                TableRow row = new TableRow();
                TableCell[] cell = new TableCell[2];
                for (int i = 0; i <= 1; i++) cell[i] = new TableCell();

                cell[0].Text = item.Value;
                cell[1].Text = TheAdminServer.GetFileMD5(item.Value);
                
                row.Cells.AddRange(cell);
                ResultList.Rows.Add(row);
            }
        }
    }

   
}