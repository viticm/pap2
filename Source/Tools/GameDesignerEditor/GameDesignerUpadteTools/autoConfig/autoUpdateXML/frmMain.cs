using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace autoUpdateXML
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void btnGet_Click(object sender, EventArgs e)
        {
            try
            {
                XmlDocument xd = new XmlDocument();
                xd.LoadXml(txtInfo.Text);
                XmlNodeList xnl = xd.GetElementsByTagName("filelist");
                xnl[0].InnerXml = codeMake();
                txtInfo.Text = FormatXml(xd.InnerXml);
                label3.Text = "更新完毕~~";
            }
            catch
            {
                MessageBox.Show("先读取服务器上的代码请");
                label3.Text = "更新失败!";
            }
        }

        private string codeMake()
        {
            string strOut = "";
            strOut += "<item name=\"";
            strOut += @"\\changan\newtools\UpdateProgram.exe";
            strOut += "\" size=\"\"><value /></item>";
            string sp = "";

            foreach(string strFileName in Directory.GetFiles(Application.StartupPath,"*",SearchOption.AllDirectories))
            {
                if(strFileName.IndexOf("\\_") != -1)
                {
                    continue;
                }

                //排除文件列表
                if (strFileName.ToLower() == Path.Combine(Application.StartupPath,"UpdateProgram.exe").ToLower()
                 || strFileName.ToLower() == Path.Combine(Application.StartupPath, "autoUpdateXML.exe").ToLower()
                 || strFileName.ToLower() == Path.Combine(Application.StartupPath, "update.xml").ToLower()
                 || strFileName.ToLower() == Path.Combine(Application.StartupPath, "local.xml").ToLower()
                 || strFileName.ToLower() == Path.Combine(Application.StartupPath, "GameDesingerTools.exe.config").ToLower()
                 || strFileName.ToLower() == Path.Combine(Application.StartupPath, "GameDesingerTools.ini").ToLower()
                    )
                {
                    sp += strFileName + "\n";
                    continue;
                }
                strOut += "<item name=\"";
                strOut += strFileName.Replace(Application.StartupPath, @"\\changan\newtools");
                strOut += "\" size=\"\"><value /></item>";
            }
            MessageBox.Show("排除文件(不包含“_”开头文件)：\n\n" + sp);
            return strOut;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            label3.Text = "正在保存...";
            if (txtInfo.Text == "")
            {
                MessageBox.Show("文件空~");
                return;
            }
            if (File.Exists("update.xml"))
            {
                File.Delete("update.xml");
            }
            FileStream fs = File.OpenWrite("update.xml");
            StreamWriter sw = new StreamWriter(fs);
            sw.Write(txtInfo.Text);
            sw.Close();
            fs.Close();
            label3.Text = "";
            MessageBox.Show("update.xml已保存！");
        }

        private void btnDownload_Click(object sender, EventArgs e)
        {
            label3.Text = "读取中...";
            if (File.Exists("update.xml"))
            {
                File.Delete("update.xml");
            }
            File.Copy(@"\\changan\newtools\update.xml", "update.xml");
            string stext = File.ReadAllText("update.xml");
            txtInfo.Text = stext;
            label3.Text = "读取完成";
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private string FormatXml(string sUnformattedXml)
        {
            XmlDocument xd = new XmlDocument();
            xd.LoadXml(sUnformattedXml);
            StringBuilder sb = new StringBuilder();
            StringWriter sw = new StringWriter(sb);
            XmlTextWriter xtw = null;
            try
            {
                xtw = new XmlTextWriter(sw);
                xtw.Formatting = Formatting.Indented;
                xtw.Indentation = 4;
                xtw.IndentChar = ' ';

                xd.WriteTo(xtw);
            }
            finally
            {
                if (xtw != null)
                    xtw.Close();
            }
            return sb.ToString();
        }

        private void btnHelp_Click(object sender, EventArgs e)
        {
            string info = "\t\t\t帮助\n";
            info += "使用方法：\n";
            info += "1. 点[读取服务器代码]，从服务器上获取update.xml文件\n";
            info += "2. 点[更新filelist]，更新filelist区域\n";
            info += "3. 点[保存XML]，保存update.xml在当前目录下\n";
            info += "\n注意事项：\n";
            info += "1. 新filelist是根据当前目录得到的，所以需要在正确目录内运行\n";
            info += "2. 只会更新filelist, 其他区域不会动\n";
            MessageBox.Show(info);
        }

    }
}