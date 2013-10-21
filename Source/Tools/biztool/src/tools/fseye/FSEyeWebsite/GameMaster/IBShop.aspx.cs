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
using System.Text;
using System.IO;
using System.Xml;
using Resources;
using FSEye.FS2Lib;
using FSEye;

public partial class GameMaster_IBShop : BasePage
{
    public string XMLResultClientId
    {
        get { return XMLResult.ClientID; }
    }

    public string IbItemXMLResultClientId
    {
        get { return IbItemXMLResult.ClientID; }
    }

    public string IbItemStyleXMLResultClientId
    {
        get { return IbItemStyleXMLResult.ClientID; }
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamScriptId] != null)
            {
                //ÐÞ¸Ä
                int scriptId = int.Parse(Request.Params[WebConfig.ParamScriptId]);
                IbConfig config = CreateIbConfig(scriptId);
                if (config != null)
                {
                    Session["OriginalIbConfig"] = config;
                }
                else
                {
                    Response.Redirect("IBShopScriptList.aspx");
                }
                Session[WebConfig.OpType] = WebConfig.OpTypeModify;
            }
            else
            {
                //ÐÂÔö
                IbConfig config = new IbConfig();
                Session["OriginalIbConfig"] = config;
                Session[WebConfig.OpType] = WebConfig.OpTypeAdd;
            }
        }

        LabelOpMsg.Text = string.Empty;
    }

    private IbConfig CreateIbConfig(int scriptId)
    {
        IList<IBShopScript> scriptList = TheAdminServer.IBShopManager.GetScript(scriptId);
        if (scriptList != null && scriptList.Count != 0)
        {
            StringReader reader = new StringReader(scriptList[0].Script);
            IbConfig.XmlSchemaFile = SystemConfig.Current.GameSettingDir + "ibshop\\ibshop.xsd";
            IbConfig config = IbConfig.ParseSqlScript(reader);
            return config;
        }
        return null;
    }

    protected void SaveIbConfigSetting(object sender, EventArgs e)
    {
        if (Session["OriginalIbConfig"] != null)
        {
            TextWriter writer = null;
            try
            {
                string xmlSettingText = XMLResult.Value;
                string ibItemXmlSettingText = IbItemXMLResult.Value;
                string ibItemStyleXmlSettingText = IbItemStyleXMLResult.Value;
                IbConfig originalConfig = Session["OriginalIbConfig"] as IbConfig;

                UpdateIbItemInfo(originalConfig, ibItemXmlSettingText);
                UpdateIbItemStyleInfo(originalConfig, ibItemStyleXmlSettingText);

                IbConfig newConfig = IbConfig.ParseXml(originalConfig, new StringReader(xmlSettingText));
                newConfig.Order();

                writer = new StringWriter();
                newConfig.CreateSqlScript(writer);
                writer.Flush();
                long scriptId = -1;
                if (TheAdminServer.IBShopManager.SaveScript(writer.ToString(), out scriptId))
                {
                    LabelOpMsg.Text = StringDef.OperationSucceed;
                    FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(CurrentUser.Id,
                        (int)GMUtil.GMOperation.CreateIBShopScript,
                        scriptId.ToString(),
                        -1,
                        string.Format(StringDef.GMMessageIBShopScript, CurrentUser.RealName, StringDef.Create, scriptId.ToString()),
                        (int)GMUtil.GMOpTargetType.System);
                }
                else
                {
                    LabelOpMsg.Text = StringDef.OperationFail;
                    return;
                }
            }
            catch (Exception ex)
            {
                LabelOpMsg.Text = ex.Message;
            }
            finally
            {
                if (writer != null)
                {
                    writer.Close();
                }
            }
        }
    }

    private void UpdateIbItemInfo(IbConfig originalConfig, string ibItemInfo)
    {
        TextReader reader = new StringReader(ibItemInfo);
        XmlDataDocument doc = new XmlDataDocument();
        doc.DataSet.ReadXmlSchema(SystemConfig.Current.GameSettingDir + "ibshop\\ibItem.xsd");
        doc.Load(reader);

        originalConfig.ItemList.Clear();
        DataTable itemTable = doc.DataSet.Tables[0];
        foreach (DataRow row in itemTable.Rows)
        {
            IbItem item = new IbItem(
                (int)row[1],
                (int)row[2],
                (int)row[3],
                (int)row[4],
                (int)row[5],
                (int)row[6],
                (int)row[7],
                (int)row[8]
            );
            item.Id = (int)row[0];
            originalConfig.ItemList.Add(item);
        }
    }

    private void UpdateIbItemStyleInfo(IbConfig originalConfig, string ibItemStyleInfo)
    {
        TextReader reader = new StringReader(ibItemStyleInfo);
        XmlDataDocument doc = new XmlDataDocument();
        doc.DataSet.ReadXmlSchema(SystemConfig.Current.GameSettingDir + "ibshop\\ibItemStyle.xsd");
        doc.Load(reader);

        originalConfig.ItemStyleList.Clear();
        DataTable itemStyleTable = doc.DataSet.Tables[0];
        foreach (DataRow row in itemStyleTable.Rows)
        {
            IbItemStyle itemStyle = new IbItemStyle(Encoding.UTF8.GetString(Convert.FromBase64String(row[1] as string)));
            itemStyle.Id = (int)row[0];

            originalConfig.ItemStyleList.Add(itemStyle);
        }
    }
}