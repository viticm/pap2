using System;
using System.Data;
using System.IO;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using FSEye;

public partial class GameServer_ShowGuardConfig : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (IsPostBack == false)
        {
            guardConfigTextBox.Width = 600;
            guardConfigTextBox.Height = 600;
            
            if (Request.Params[WebConfig.ParamServerId] != null)
            {
                int serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
                GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (server != null)
                {
                    try
                    {
                        using (MemoryStream configStream = new MemoryStream())
                        {
                            server.LordConfig.WriteGuardConfig(configStream);
                            guardConfigTextBox.Text = SystemConfig.Current.DefaultEncoding.GetString(configStream.ToArray()); //Encoding.ASCII.GetString(configStream.ToArray());
                        }
                    }
                    catch (Exception ex)
                    {
                        guardConfigTextBox.Text = ex.Message;
                    }
                }
                else
                {
                    guardConfigTextBox.Text = "无此服务器";
                }
            }
        }
    }
}
