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
using System.IO;
using FSEye.DAL;
using FSEye;
public partial class Common_ServerConfigTextBox : System.Web.UI.UserControl
{
   
    public string Text
    {
        set { ServerCfgTextBox.Text = value; }
        get { return ServerCfgTextBox.Text; }
    }
    protected void Page_Load(object sender, EventArgs e)
    {
        ServerCfgTextBox.Height = 600;
        ServerCfgTextBox.Width = 600;
    }


    public void getServerConfigTextBox(string severtypeurl){
        string url = severtypeurl;
        
        Encoding encoding = SystemConfig.Current.DefaultEncoding; 
        
        StreamReader file = new StreamReader(url, encoding);

        String str;
        String resultstr = "";
        while ((str = file.ReadLine()) != null)
        {
            resultstr += str + "\r\n";
        }
        ServerCfgTextBox.Text = resultstr;
    }
    protected void ServerConfigTextBox_TextChanged(object sender, EventArgs e)
    {
       

    }
   
}
