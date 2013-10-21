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
using FSEye.Security;

public partial class UpdateServer_MD5 : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerMD5, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
    }

    protected void buttonCreatMD5_Click(object sender, EventArgs e)
    {
        textboxRandomMD5.Text = System.Web.Security.FormsAuthentication.HashPasswordForStoringInConfigFile(Guid.NewGuid().ToString(), "MD5");         
    }

    protected void buttonMD5_Click(object sender, EventArgs e)
    {
        try
        {
            FileStream get_file = new FileStream(textboxFile.Text.Trim(), FileMode.Open, FileAccess.Read, FileShare.Read);
            byte[] pdfFile = new byte[get_file.Length];
            get_file.Read(pdfFile, 0, (int)get_file.Length);//将文件流读取到Buffer中
            get_file.Close();

            textboxRandomMD5.Text = MD5Buffer(pdfFile, 0, pdfFile.Length);//对Buffer中的字节内容算MD5
        }
        catch (Exception ex)
        {
            textboxRandomMD5.Text = ex.Message;
        }
    }

    private string MD5Buffer(byte[] pdfFile, int index, int count)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider get_md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] hash_byte = get_md5.ComputeHash(pdfFile, index, count);

        string result = System.BitConverter.ToString(hash_byte);
        result = result.Replace("-", "");
        return result;
    }

    private string MD5String(string str)
    {
        byte[] MD5Source = System.Text.Encoding.ASCII.GetBytes(str);
        return MD5Buffer(MD5Source, 0, MD5Source.Length);

    }

}
