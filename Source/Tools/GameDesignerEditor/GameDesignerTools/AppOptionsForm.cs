using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace GameDesingerTools
{
    public partial class AppOptionsForm : Form
    {

        public string DataBase = string.Empty;
        private string[] mInfoCN = null;
        private string[] mInfoEN = null;

        public AppOptionsForm()
        {
            InitializeComponent();
            string strIniFile = Application.StartupPath + "/GameDesingerTools.ini";
            string strRemoteFile = Application.StartupPath + "/GameDesingerTools_remote.ini";
            StringBuilder sb = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("General", "DefaultStorageDir", "", sb, 255, strIniFile);
            textBoxX1.Text = sb.ToString();


            //FileFolderHelper.ReadIniFile("General", "RootDir", "", sb, 255, strIniFile);
            txtBoxRootDir.Text = Program.RootDir;
            //txtBoxConnStr.Text = Program.ConnetionString;
            textBox3DEnginePath.Text = Program.EngineDir;

            FileFolderHelper.ReadIniFile("Advance", "tmrRenderInterval", "50", sb, 255, strIniFile);
            textBoxX2.Text = sb.ToString();

            FileFolderHelper.ReadIniFile("Advance", "ComparerDir", "", sb, 255, strIniFile);
            this.textBoxXComparer.Text = sb.ToString();

            FileFolderHelper.ReadIniFile("Remote", "DBInfo", "", sb, 255, strRemoteFile);
            string dbinfo = sb.ToString().Trim();
            mInfoEN = dbinfo.Split(new char[] { ' ' });

            FileFolderHelper.ReadIniFile("Remote", "DBInfoCHN", "", sb, 255, strRemoteFile);
            string cnInfo = sb.ToString().Trim();
            mInfoCN = cnInfo.Split(new char[] { ' ' });
            comboBox1.Items.AddRange(mInfoCN);

            FileFolderHelper.ReadIniFile("Remote", "DataBase", "", sb, 255, strRemoteFile);
            string database = sb.ToString().Trim();
            if (database != string.Empty)
            {
                for (int index = 0; index < mInfoCN.Length; ++index)
                {
                    if (database == mInfoEN[index])
                    {
                        comboBox1.SelectedIndex = index;
                    }
                }
            }

            tabItem4.Parent.SelectedTab = tabItem4;
            
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            string formerConnectionString = Program.ConnetionString; // 旧的连接语句 
            string strIniFile = Application.StartupPath + "/GameDesingerTools.ini";
            string strRemoteFile = Application.StartupPath + "/GameDesingerTools_remote.ini";
            FileFolderHelper.WriteIniFile("General", "DefaultStorageDir", textBoxX1.Text, strIniFile);
            //FileFolderHelper.WriteIniFile("Browsing Base Folder", "Script", textBoxX2.Text, strIniFile);
            //FileFolderHelper.WriteIniFile("Browsing Base Folder", "Sfx", textBoxX3.Text, strIniFile);
            FileFolderHelper.WriteIniFile("Advance", "tmrRenderInterval", textBoxX2.Text, strIniFile);
            FileFolderHelper.WriteIniFile("Advance", "ComparerDir", textBoxXComparer.Text, strIniFile);
            int iInterval = 50;
            bool bSuc = int.TryParse(textBoxX2.Text, out iInterval);
            if (bSuc)
            {
                Program.m_3d_tmrRender = iInterval;
                if (Program.MainForm != null)
                    Program.MainForm.UpdateAllBaseFormTmrRenderInterval();
            }
            FileFolderHelper.WriteIniFile("General", "RootDir", txtBoxRootDir.Text, strIniFile);
            FileFolderHelper.WriteIniFile("General", "3DEnginePath", textBox3DEnginePath.Text, strIniFile);
            Program.RootDir = txtBoxRootDir.Text.Trim();
            Program.EngineDir = textBox3DEnginePath.Text.Trim();

            if (comboBox1.SelectedIndex < 0)
            {
                string message = "请选择数据库";
                MessageBox.Show(message);
                return;
            }
            int index = comboBox1.SelectedIndex;
            DataBase = mInfoEN[index];
            FileFolderHelper.WriteIniFile("Remote", "DataBase", DataBase, strRemoteFile);

            DialogResult = DialogResult.OK;
            Close();
        }

        void CheckConnString(string formerConnectionString)
        {
            if (Program.ConnetionString != formerConnectionString) // 连接语句被修改过
            {
                DialogResult result = MessageBox.Show("连接语句已经改变，是否立即重新启动程序", "重启程序提示", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    if (Helper.CloseAllWindows(Program.MainForm)) // 成功关闭所有窗体
                    {
                        Program.Restart = true;
                    }
                }
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            string DestFolder = FileFolderHelper.BrowserFolder("选择 skill.tab 的默认保存路径");
            if (DestFolder != "")
                textBoxX1.Text = DestFolder;
        }

        private void browRootDir_Click(object sender, EventArgs e)
        {
            string DestFolder = FileFolderHelper.BrowserFolder("选择设置文件的根路径");
            if (DestFolder != "")
                txtBoxRootDir.Text = DestFolder;
        }

        private void buttonX4_Click(object sender, EventArgs e)
        {
            string DestFolder = FileFolderHelper.BrowserFolder("选择KG3DEngine.dll所在路径");
            if (DestFolder != "")
                textBox3DEnginePath.Text = DestFolder;
        }

        private void buttonComparer_Click(object sender, EventArgs e)
        {
            string DestFolder = FileFolderHelper.BrowserFile(new string[] { "exe"});
            if (DestFolder != "")
                textBoxXComparer.Text = DestFolder;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = comboBox1.SelectedIndex;
            if (index < 0)
            {
                return;
            }


            label6.Text = mInfoEN[index];
        }
    }
}