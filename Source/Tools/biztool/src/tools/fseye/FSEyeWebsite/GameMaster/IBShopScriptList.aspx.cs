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
using System.IO;
using Resources;
using FSEye;

public partial class GameMaster_IBShopScriptList : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if(!IsPostBack)
        {
            CreateScriptList();
        }
    }

    private void CreateScriptList()
    {
        IList<IBShopScript> scriptList = TheAdminServer.IBShopManager.GetScript();
        Session["ScriptList"] = scriptList;
        if (scriptList != null && scriptList.Count != 0)
        {
            DataGridScript.DataSource = CreateDataSource(scriptList);
            DataGridScript.DataBind();
        }
        else
        {
            LabelOpMsg.Text = StringDef.NoInfo;
        }
    }

    private ICollection CreateDataSource(IList<IBShopScript> scriptList)
    {
        if (scriptList != null && scriptList.Count != 0)
        {
            try
            {
                DataTable dataTable = new DataTable();
                DataRow dataRow;

                dataTable.Columns.Add(new DataColumn("ID", typeof(Int32)));
                dataTable.Columns.Add(new DataColumn("TimeStamp", typeof(String)));

                foreach (IBShopScript script in scriptList)
                {
                    dataRow = dataTable.NewRow();
                    dataRow[0] = script.ID;
                    dataRow[1] = script.TimeStamp.ToString("yyyy-MM-dd HH:mm:ss");

                    dataTable.Rows.Add(dataRow);
                }
                DataView dataView = new DataView(dataTable);
                dataView.Sort = "ID ASC";
                return dataView;
            }
            catch (Exception)
            {
                return null;
            }
        }
        return null;
    }

    protected void LinkButton_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;

        if (lb != null)
        {
            DataGridItem dataGridItem = lb.Parent.Parent as DataGridItem;
            if (dataGridItem != null)
            {
                int scriptId = int.Parse(dataGridItem.Cells[0].Text);
                switch (lb.CommandName)
                {
                    case "Delete":
                        if (TheAdminServer.IBShopManager.DeleteScript(scriptId))
                        {
                            FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(
                                CurrentUser.Id,
                                (int)GMUtil.GMOperation.DeleteIBShopScript,
                                scriptId.ToString(),
                                -1,
                                string.Format(StringDef.GMMessageIBShopScript, CurrentUser.RealName, StringDef.Delete, scriptId.ToString()),
                                (int)GMUtil.GMOpTargetType.System
                            );
                            CreateScriptList();
                        }
                        else
                        {
                            LabelOpMsg.Text = StringDef.OperationFail;
                            return;
                        }
                        break;
                    case "View":
                        Response.Redirect(string.Format("IBShop.aspx?{0}={1}", WebConfig.ParamScriptId, scriptId));
                        break;
                    case "Create":
                        IBShopScript script = GetScript(scriptId);
                        if (script != null)
                        {
                            TextWriter writer = null;
                            try
                            {
                                string fileName = string.Format("IBShop_{0}.sql", script.TimeStamp.ToString("yyyyMMddHHmm"));
                                FileStream fileStream = new FileStream(SystemConfig.Current.AdminServerUploadFileRootPath + fileName, 
                                    FileMode.OpenOrCreate, 
                                    FileAccess.Write, 
                                    FileShare.Read
                                    );
                                fileStream.Position = 0;
                                writer = new StreamWriter(fileStream);                                
                                writer.Write(script.Script);
                                writer.Flush();
                                LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + string.Format(StringDef.MsgCreateFileSuccess, fileName);
                            }
                            catch (Exception ex)
                            {
                                LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
                            }
                            finally
                            {
                                if (writer != null)
                                    writer.Close();
                            }
                        }
                        else
                        {
                            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.CreateScriptFile;
                        }
                        break;
                }
            }
        }
    }

    private IBShopScript GetScript(int scriptId)
    {
        if (Session["ScriptList"] != null)
        {
            IList<IBShopScript> scriptList = Session["ScriptList"] as IList<IBShopScript>;
            foreach (IBShopScript script in scriptList)
            {
                if (script.ID == scriptId)
                {
                    return script;
                }
            }
        }
        IList<IBShopScript> scriptListFromDb = TheAdminServer.IBShopManager.GetScript(scriptId);
        if (scriptListFromDb != null && scriptListFromDb.Count != 0)
        {
            return scriptListFromDb[0];
        }
        return null;
    }
}