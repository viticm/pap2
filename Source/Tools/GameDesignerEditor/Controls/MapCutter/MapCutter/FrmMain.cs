using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;

namespace MapCutter
{
    public partial class FrmMain : Form
    {
        [DllImport("PsdRead.dll")]
        public static extern void ReadPsdInfo(string fileName, ref int layerCount, int[] layBuffer);
        [DllImport("kernel32.dll")]
        static extern void ExitProcess(uint exitCode);

        private string rootPath;
        private string strMapPath = "";
        private atlaxLib.Test3DEngine m_3DEngie = null;

        public FrmMain()
        {
            try
            {
                InitializeComponent();
                ImageCutter.OnCutting += new ImageCutter.OnCuttingHandler(ImageCutter_OnCutting);
                string strIniFileFullName = Application.StartupPath + "\\GameDesingerTools.ini";
                strMapPath = IniIO.IniReadValue("General", "RootDir", strIniFileFullName);
                if(strMapPath == null || strMapPath == "")
                {
                    MessageBox.Show("地图目录路径未设置!请把程序放到填表工具同一目录下!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                strMapPath = strMapPath.TrimEnd(new char[] { '\\' });
                rootPath = strMapPath;
                Panel p = new Panel();

                try
                {
                    Helper.RegComDll("atlax.dll");
                    m_3DEngie = new atlaxLib.Test3DEngine();
                    m_3DEngie.Init((int)p.Handle, strMapPath + "\\", Application.StartupPath);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("atlax初始化失败: " + ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                strMapPath += @"\data\source\maps\";
            }
            catch (Exception ex)
            {
                MessageBox.Show("frmMain初始化失败: " + ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
        }

        private void FrmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            try
            {
                ExitProcess(0);
            }
            catch (Exception)
            {
                ExitProcess(0);           	
            }
        }

        void ImageCutter_OnCutting(object sender, ImageCutter.CutterEventArgs e)
        {
            this.proc.Maximum = e.nAllFrame;
            this.proc.Value = e.nCurFrame;
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnSmallOpen_Click(object sender, EventArgs e)
        {
            CD1.FileName = "";
            CD1.Filter = "*.bmp|*.bmp";
            CD1.ShowDialog();
            if(CD1.FileName != "")
            {
                txtSmallPath.Text = CD1.FileName;
            }
        }

        private void btnMiddleOpen_Click(object sender, EventArgs e)
        {
            CD1.FileName = "";
            CD1.Filter = "*.psd|*.psd";
            CD1.ShowDialog();
            if (CD1.FileName != "")
            {
                txtMiddlePath.Text = CD1.FileName;
            }
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if(strMapPath == "")
            {
                MessageBox.Show("地图目录路径未设置!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if(tabControl1.SelectedTabIndex == 0)
            {
                //对于小地图来说
                if (txtSmallPath.Text == "" || txtSmallName.Text == "")
                {
                    return;
                }
                btnStart.Enabled = false;
                
                //移动图片到指定目录
                string strPreBmp = "";
                {
                    if (radioButton1.Checked)
                        strPreBmp = "0";
                    if (radioButton2.Checked)
                        strPreBmp = "1";
                }
                float fscale = 1.0f;
                {
                    if (checkBox4.Checked)
                        fscale = 0.6f;
                }
                string strCreatePath = "";
                {
                    strCreatePath = Path.Combine(this.strMapPath, this.txtSmallName.Text + "minimap");
                    if (!Directory.Exists(strCreatePath))
                        Directory.CreateDirectory(strCreatePath);
                }
                if(!File.Exists(txtSmallPath.Text))
                {
                    MessageBox.Show("找不到源文件" + txtSmallPath.Text, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                //开始切割并保存图形碎片
                proc.Value = 0;
                proc.Visible = true;
                Application.DoEvents();
                Image imgScale = ImageCutter.ImageCutAndSave(txtSmallPath.Text, 128, 128, fscale, strCreatePath, strPreBmp + "_{1}_{0}.jpg");
                
                //保存缩略整图
                // imgScale.Save(Path.Combine(strCreatePath, strPreBmp + ".bmp"), System.Drawing.Imaging.ImageFormat.Bmp);

                //写INI
                string strIniFileName = Path.Combine(strCreatePath, "config.ini");
                IniIO.IniWriteValue("config", "width", "128", strIniFileName);
                if (checkBox4.Checked)
                    IniIO.IniWriteValue("config", "scale", "0.012", strIniFileName);
                else
                    IniIO.IniWriteValue("config", "scale", "0.02", strIniFileName);

                proc.Value = 0;
                proc.Visible = false;
                
                //提示成功
                MessageBox.Show("生成完毕！" + strMapPath, "完毕", MessageBoxButtons.OK, MessageBoxIcon.Information);
                btnStart.Enabled = true;
            }
            else
            {
                //对于中地图来说
                if (txtMiddlePath.Text == "" || txtMiddleName.Text == "")
                {
                    return;
                }
                btnStart.Enabled = false;

                //读取PSD文件的2个图层
                int layCount = 0;
                int[] layBuffer =  new int[8];
                ReadPsdInfo(txtMiddlePath.Text, ref layCount, layBuffer);

                if (layBuffer == null) // 没有缓冲区数据信息，直接报错返回
                //test if(layCount != 2 || layBuffer == null)
                {
                    MessageBox.Show("读取psd失败!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    btnStart.Enabled = true;
                    return;
                }
                Rectangle rectBig = new Rectangle(layBuffer[1], layBuffer[0], layBuffer[3] - layBuffer[1], layBuffer[2] - layBuffer[0]);
                Rectangle rectSmall = Rectangle.Empty;
                if (layCount == 1) // 只有一层图层，rectBig和rectSmall重合
                {
                    rectSmall = new Rectangle(layBuffer[1], layBuffer[0], layBuffer[3] - layBuffer[1], layBuffer[2] - layBuffer[0]);
                }
                else if (layCount == 2)
                {
                    rectSmall = new Rectangle(layBuffer[1 + 4], layBuffer[0 + 4], layBuffer[3 + 4] - layBuffer[1 + 4], layBuffer[2 + 4] - layBuffer[0 + 4]);
                }
                else
                {
                    MessageBox.Show("输入psd文件图层太多，请检查！\r\n\r\n如果你不知道这是什么意思，请找RTX： kuangsihao 或者 zhouhengda");
                    btnStart.Enabled = true;
                    return;
                }

                //建立目录
                string strCreatePath = "";
                {
                    strCreatePath = Path.Combine(this.strMapPath, this.txtMiddleName.Text + "minimap");
                    if (!Directory.Exists(strCreatePath))
                        Directory.CreateDirectory(strCreatePath);
                }

                //计算
                float nScale = 0;
                float nStartX = 0;
                float nStartY = 0;

                int nScaleWidth = 0;   //W实际Region宽
                int nScaleHeight = 0;
                try
                {
                    // 直接从.Map文件读取region数目。
//                     string strMap = string.Format("{0}/data/source/maps/{1}/{1}.Map", rootPath, this.txtMiddleName.Text);
//                     FileStream fs = new FileStream(strMap, FileMode.Open, FileAccess.Read);
//                     BinaryReader r = new BinaryReader(fs);
//                     r.ReadBytes(32);
//                     nScaleWidth = r.ReadInt32();
//                     nScaleHeight = r.ReadInt32();
//                     fs.Close();



                    int ptr3DScene = 0;
                    string strMap = this.strMapPath + this.txtMiddleName.Text + "\\" + this.txtMiddleName.Text + ".Map";
                    m_3DEngie.LoadOneMap_NoOutputWindow(strMap, out ptr3DScene);
                    m_3DEngie.ReadMapTerrainInfo(ptr3DScene, out nScaleWidth, out nScaleHeight);
                    nScaleWidth /= 16;
                    nScaleHeight /= 16;
                }
                catch(Exception ex)
                {
                    MessageBox.Show("读取Map失败:" + ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    btnStart.Enabled = true;
                    return;
                }

                if (checkBoxFormat.Checked)
                {
                    string configfile = textBoxOld.Text;
                    if (configfile.Length == 0)
                    {
                        MessageBox.Show("请选择旧的config.ini文件。");
                        btnStart.Enabled = true;
                        return;
                    }

                    string strScale = IniIO.IniReadValue("middlemap0", "scale", configfile);
                    float fScale = (float)Convert.ToDouble(strScale);
                    string strWidth = IniIO.IniReadValue("middlemap0", "width", configfile);
                    float fWidth = (float)Convert.ToDouble(strWidth);
                    nScaleWidth = Convert.ToInt32(fWidth * 2 / 64 / 32 / fScale);
                }


                nScale = ((float)rectSmall.Width) / ((float)nScaleWidth / 2 * 64 * 32);
                int nX0 = rectSmall.Left;
                int nY0 = rectBig.Height - rectSmall.Height - rectSmall.Top;
                int nY0_____1 = rectBig.Bottom - rectSmall.Bottom;
                nStartX = (-nX0) / nScale;
                nStartY = (-nY0) / nScale;

                //写INI
                string strIniFileName = Path.Combine(strCreatePath, "config.ini");
                IniIO.IniWriteValue("middlemap0", "name", this.txtMiddleName.Text, strIniFileName);
                IniIO.IniWriteValue("middlemap0", "image", "middlemap.tga", strIniFileName);
                IniIO.IniWriteValue("middlemap0", "width", rectBig.Width.ToString(), strIniFileName);
                IniIO.IniWriteValue("middlemap0", "height", rectBig.Height.ToString(), strIniFileName);
                IniIO.IniWriteValue("middlemap0", "scale", nScale.ToString(), strIniFileName);
                IniIO.IniWriteValue("middlemap0", "startx", nStartX.ToString(), strIniFileName);
                IniIO.IniWriteValue("middlemap0", "starty", nStartY.ToString(), strIniFileName);
                IniIO.IniWriteValue("middlemap0", "copy", checkBox1.Checked ? "1" : "0", strIniFileName);
                IniIO.IniWriteValue("middlemap0", "fresherroom", checkBox2.Checked ? "1" : "0", strIniFileName);
                IniIO.IniWriteValue("middlemap0", "battlefield", checkBox3.Checked ? "1" : "0", strIniFileName);

                //提示成功
                MessageBox.Show("生成Ini文件完毕！\r\n\r\n请手工制作middlemap.tga！\r\n\r\n" + strMapPath, "完毕", 
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                btnStart.Enabled = true;
            }
        }

        private void btnMiddleOpen2_Click(object sender, EventArgs e)
        {
            CD1.FileName = "";
            CD1.Filter = "*.ini|*.ini";
            CD1.ShowDialog();
            if (CD1.FileName != "")
            {
                textBoxOld.Text = CD1.FileName;
            }
        }

        private void btnTransfer_Click(object sender, EventArgs e)
        {
            AreaTabFiller tabFileTransfer = new AreaTabFiller();
            string assistFilePath = tbxArtFile.Text.ToString();
            if (!File.Exists(assistFilePath))
            {
                MessageBox.Show("美术文件" + assistFilePath + "不存在");
                return;
            }
            try
            {
                tabFileTransfer.Init(this.rootPath, tbxMapName.Text.ToString(), assistFilePath);
                tabFileTransfer.Transfer();
                tabFileTransfer.Save();
                MessageBox.Show("恭喜！\r\n转换完毕", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception myex)
            {              
                MessageBox.Show(myex.ToString(), "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
        }

        private void btnBrower_Click(object sender, EventArgs e)
        {
            CD1.FileName = "";
            CD1.Filter = "tab文件(*.tab)|*.tab|所有文件(*.*)|*.*";
            CD1.ShowDialog();            
            if (CD1.FileName != "")
            {
                tbxArtFile.Text = CD1.FileName;              
            }
        }


    }
}