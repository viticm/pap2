using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace GameLuaEditor
{
    public partial class SettingForm : Form
    {
        // 更新脚本字体用的回调
        public delegate void UpdateScriptFont(Font font);
        private UpdateScriptFont updateScriptFont;

        // 更新脚本字体颜色用的回调
        public delegate void UpdateScriptForeColor(Color color);
        private UpdateScriptForeColor updateScriptForeColor;

        /// <summary>
        /// 构造函数
        /// </summary>
        public SettingForm()
        {
            InitializeComponent();            
            Init();
        }

        /// <summary>
        /// 更新脚本字体用的回调
        /// </summary>
        public UpdateScriptFont CurrentUpdateScriptFont
        {
            set
            {
                updateScriptFont = value;
            }
        }

        /// <summary>
        /// 更新脚本字体颜色用的回调
        /// </summary>
        public UpdateScriptForeColor CurrentUpdateScriptForeColor
        {
            set
            {
                updateScriptForeColor = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            txtPath.Text = Helper.ClientPath;
            serverPathBox.Text = Helper.ServerPath;
            messageDelayBox.Text = Helper.ReceiveMessageDelay.ToString();
            messageRetryTimeBox.Text = Helper.ReceiveMessageRetryTime.ToString();
            breakPointWaitCircleBox.Text = Helper.BreakPointWaitCircle.ToString();
            serverIPBox.Text = Helper.ServerIP;
            fontPreviewLabel.Font = Helper.ScriptFont;
            fontPreviewLabel.ForeColor = Helper.ScriptForeColor;
        }

        /// <summary>
        /// 选择文件夹
        /// </summary>
        /// <param name="description">窗体描述</param>
        /// <returns>文件夹路径</returns>
        private string ViewFolder(string description)
        {
            string folderPath = "";

            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.Description = description;

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                folderPath = folderBrowserDialog.SelectedPath;
            }

            return folderPath;
        }        

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnOK_Click(object sender, EventArgs e)
        {
            if (txtPath.Text == "")
            {
                MessageBox.Show("客户端路径不能为空！", "配置设置", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            Helper.ClientPath = txtPath.Text;            
            
            // 保存脚本编辑器的配置文件
            string fileName = Path.Combine(Application.StartupPath, "LuaEditor.ini");
            if (!File.Exists(fileName))
            {
                File.Create(fileName).Close();
            }
            
            if (serverPathBox.Text != "")
            {
                Helper.ServerPath = serverPathBox.Text;
                IniIO.IniWriteValue("General", "ServerFolder", Helper.ServerPath, fileName);
            }

            if (messageDelayBox.Text != "")
            {
                Helper.ReceiveMessageDelay = int.Parse(messageDelayBox.Text);
                IniIO.IniWriteValue("General", "ReceiveMessageDelay", Helper.ReceiveMessageDelay.ToString(), fileName);
            }

            if (messageRetryTimeBox.Text != "")
            {
                Helper.ReceiveMessageRetryTime = int.Parse(messageRetryTimeBox.Text);
                IniIO.IniWriteValue("General", "ReceiveMessageRetryTime", Helper.ReceiveMessageRetryTime.ToString(), fileName);
            }

            if (breakPointWaitCircleBox.Text != "")
            {
                Helper.BreakPointWaitCircle = int.Parse(breakPointWaitCircleBox.Text);
                IniIO.IniWriteValue("General", "BreakPointWaitCircle", Helper.BreakPointWaitCircle.ToString(), fileName);
            }

            if (serverIPBox.Text != "")
            {
                Helper.ServerIP = serverIPBox.Text;
                IniIO.IniWriteValue("General", "ServerIP", Helper.ServerIP, fileName);
            }

            Helper.ScriptFont = fontPreviewLabel.Font;
            Helper.ScriptForeColor = fontPreviewLabel.ForeColor;
            string fontName = fontPreviewLabel.Font.Name;            
            string fontSize = fontPreviewLabel.Font.Size.ToString();
            string fontColor = fontPreviewLabel.ForeColor.Name;

            IniIO.IniWriteValue("General", "ScriptFontName", fontName, fileName);
            IniIO.IniWriteValue("General", "ScriptFontSize", fontSize, fileName);
            IniIO.IniWriteValue("General", "ScriptForeColor", fontColor, fileName);
            IniIO.IniWriteValue("General", "RootDir", Helper.ClientPath, "./GameDesingerTools.ini");

            // 更新字体
            updateScriptFont(Helper.ScriptFont);

            // 更新字体颜色
            updateScriptForeColor(Helper.ScriptForeColor);

            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnEditor_Click(object sender, EventArgs e)
        {
            string strFolder    = Application.StartupPath.TrimEnd(new char[] { '\\' });
            string strCfg       = Helper.FileToString(strFolder + @"\Plugins\LuaCheck\setting.cfg");
            string strUserCfg   = Helper.FileToString(strFolder + @"\Plugins\LuaCheck\_setting.cfg");

            //strCfg = "哈哈,int,123,|是否允许开启,list,false;true,|";
            //strUserCfg = "456,true";

            string[] astrUserCfg = strUserCfg.Split(new string[] { "," },  StringSplitOptions.RemoveEmptyEntries);
            string[] astrCfg = strCfg.Split(new string[] { "|" },  StringSplitOptions.RemoveEmptyEntries);

            //读取用户配置,并改写strCfg
            if(astrUserCfg.Length == astrCfg.Length)
            {
                strCfg = "";
                for(int i = 0; i < astrCfg.Length; i++)
                {
                    string strItem = astrCfg[i];
                    string[] asItem = strItem.Split(new char[] { ',' });
                    if (asItem.Length != 4) return;
                    if(asItem[1] == "list")
                    {
                        string strList = asItem[2].Trim(new char[] { ';' });
                        strList = ";" + strList + ";";
                        strList = astrUserCfg[i] + strList.Replace(";" + astrUserCfg[i] + ";", ";");
                        asItem[2] = strList;
                    }
                    else
                    {
                        asItem[2] = astrUserCfg[i];
                    }
                    astrCfg[i] = string.Join(",", asItem) + ",";
                }
                strCfg = string.Join("|", astrCfg) + "|";
            }

            string[] asp1 = { "|" };
            ModelForm mf = new ModelForm(strCfg.Split(asp1, StringSplitOptions.RemoveEmptyEntries));
            if (mf.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            List<string> list = mf.InputList;
            Helper.GameLuaEditorSetting = string.Join(",", list.ToArray());
            Helper.WriteStringToFile(Helper.GameLuaEditorSetting, strFolder + @"\Plugins\LuaCheck\_setting.cfg");
        }

        private void btnCancel_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.Alt && e.Control && e.KeyCode == Keys.I)
            {
                ImportForm frmImp = new ImportForm();
                frmImp.ShowDialog();
            }            
        }

        /// <summary>
        /// 浏览客户端目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string folderPath = ViewFolder("请选择客户端文件夹所在目录");

            if (folderPath != "")
            {
                txtPath.Text = folderPath;
            }
        }

        /// <summary>
        /// 浏览服务端目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            string folderPath = ViewFolder("请选择服务端文件夹所在目录");

            if (folderPath != "")
            {
                serverPathBox.Text = folderPath;
            }
        }

        /// <summary>
        /// 挂接调试文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAttachDebugFile_Click(object sender, EventArgs e)
        {
            string serverFolder = serverPathBox.Text;
            string clientFolder = txtPath.Text;

            if (clientFolder == "")
            {
                MessageBox.Show("请先选择客户端所在目录！", "挂接调试文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (serverFolder == "")
            {
                MessageBox.Show("请先选择服务端所在目录！", "挂接调试文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            List<string> srcFileNameList = new List<string>();
            List<string> destFileNameList = new List<string>();

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Engine_Lua5.dll"));
            destFileNameList.Add(Path.Combine(serverFolder, "Engine_Lua5.dll"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Engine_Lua5D.dll"));
            destFileNameList.Add(Path.Combine(serverFolder, "Engine_Lua5D.dll"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Lua5D.dll"));
            destFileNameList.Add(Path.Combine(serverFolder, "Lua5D.dll"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\udpsocket.dll"));
            destFileNameList.Add(Path.Combine(serverFolder, "udpsocket.dll"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\ldb.lua"));
            destFileNameList.Add(Path.Combine(clientFolder, @"scripts\ldb.lua"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\udpsocket.dll"));
            destFileNameList.Add(Path.Combine(serverFolder, "udpsocket.dll"));

            srcFileNameList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\udpsocket.dll"));
            destFileNameList.Add(Path.Combine(clientFolder, "udpsocket.dll"));

            for (int i = 0; i < srcFileNameList.Count; i++)
            {
                if (File.Exists(srcFileNameList[i]))
                {                    
                    File.Copy(srcFileNameList[i], destFileNameList[i], true);                    
                }
                else
                {
                    MessageBox.Show(string.Format("文件{0}不存在，请先运行UpdateProgram.exe获取更新文件！", srcFileNameList[i]), "挂接调试文件",
                                                  MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }

            MessageBox.Show("调试文件挂接完成！", "挂接调试文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /// <summary>
        /// 选择字体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSelectFont_Click(object sender, EventArgs e)
        {
            FontDialog fontDialog = new FontDialog();
            fontDialog.Font = fontPreviewLabel.Font;

            if (fontDialog.ShowDialog() == DialogResult.OK)
            {
                fontPreviewLabel.Font = fontDialog.Font;
            }
        }

        /// <summary>
        /// 选择颜色
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            ColorDialog colorDialog = new ColorDialog();
            colorDialog.Color = fontPreviewLabel.ForeColor;

            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                fontPreviewLabel.ForeColor = colorDialog.Color;
            }
        }
    }
}