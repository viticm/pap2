using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;

namespace ScriptViewer
{
    public partial class frmHistory : Form
    {
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
        
        public string strRetCode = null;

        public frmHistory()
        {
            InitializeComponent();
            this.listBox1.SelectionMode = SelectionMode.MultiSimple;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Visible = false;
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItems.Count == 1)
            {
                this.buttonX1.Enabled = true;
                this.buttonX3.Enabled = false;
                hisItem hi = listBox1.SelectedItem as hisItem;
                this.strRetCode = hi.strCode;
            }
            else if (this.listBox1.SelectedItems.Count == 2)
            {
                this.buttonX1.Enabled = false;
                this.buttonX3.Enabled = true;
            }
            else
            {
                this.buttonX1.Enabled = false;
                this.buttonX3.Enabled = false;               
            }
        }

        private void buttonX3_Click(object sender, EventArgs e)
        {
            string strPath = Application.StartupPath;
            string strFileNameRight = "ScriptRight.lua";
            string strFileNameLeft = "ScriptLeft.lua";
            string strFileRight = strPath + "\\" + strFileNameRight;
            string strFileLeft = strPath + "\\" + strFileNameLeft;
            StringBuilder strbComparerDir = new StringBuilder("", 255);
            hisItem ItemLeft = this.listBox1.SelectedItems[0] as hisItem;
            hisItem ItemRight = this.listBox1.SelectedItems[1] as hisItem;
            StringToFile(ItemLeft.Code, strFileLeft, Encoding.GetEncoding("gb2312"));
            StringToFile(ItemRight.Code, strFileRight, Encoding.GetEncoding("gb2312"));

            GetPrivateProfileString("Advance", "ComparerDir", "", strbComparerDir, 255, strPath + "\\GameDesingerTools.ini");
            if (strbComparerDir.ToString() == "")
            {
                MessageBox.Show("请确定在GameDesingerTools.ini中设置 Beyond Compare 相应路径！");
            }
            else
            {
                try
                {
                    string strArgs = "\"" + strFileLeft + "\" \"" + strFileRight + "\"";
                    Process.Start(strbComparerDir.ToString(), strArgs);
                }
                catch (Exception ex)
                {
                    throw new Exception("启动 Beyond Compare 失败，请确定已经安装并且路径正确！");
                }
            }
        }

        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                CreatePath(strFileName);
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }
        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }
    }

    public class hisItem
    {
        public hisItem(int id, string logtime, string strCode, string reason, string author)
        {
            this.id = id;
            this.logtime = logtime;
            this.strCode = strCode;
            this.reason = reason;
            this.strAuthor = author;
        }
        public hisItem(string strCode)
        {
            this.id = -1;
            this.logtime = "";
            this.strCode = strCode;
        }
        public string Code
        {
            get { return strCode; }
        }
        public override string ToString()
        {
            if (this.id == -1)
                return "当前代码";
            else
            {
                DateTime d1 = DateTime.Parse(this.logtime);
                DateTime d2 = DateTime.Now;
                TimeSpan ts = d2 - d1;
                string tag = "";
                if (d1.Date == d2.Date) tag = "[今天]　　　";
                else if (ts.Days < 7) tag = "[一周以内]　";
                else if (ts.Days < 30) tag = "[一个月内]　";
                else if (ts.Days < 180) tag = "[半年以内]　";
                else tag = "[半年以上]　";
                return tag + " [" + reason + "]   " + this.logtime + "      " + this.strAuthor;
            }
        }
        public int id = 0;
        public string logtime = "";
        public string strCode = "";
        public string reason = "";
        public string strAuthor = "";
    };
}