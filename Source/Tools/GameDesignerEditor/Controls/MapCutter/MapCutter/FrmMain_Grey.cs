using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Collections;

namespace MapCutter
{
    public partial class FrmMain
    {  
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        [DllImport("user32.dll")]
        static extern void LockWindowUpdate(IntPtr handle);

        private string currentPath; // 当前路径
        private Image image; // 当前显示的图片
        private Hashtable dataTable = new Hashtable(); // 保存id对应数据的哈希表
        private List<string> itemList = new List<string>(); // 保存背景音乐下拉框选项的链表

        /// <summary>
        /// 读取文件
        /// </summary>
        /// <param name="fileName">文件全路径</param>
        private void LoadPicture(string fileName)
        {
            Image image = Image.FromFile(fileName);
            canvas.PaintPicture(image);
            canvas.ClearPaint();
            this.image = image;
            panel1_SizeChanged(null, null);
        }

        /// <summary>
        /// 打开文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bOpen_Click(object sender, EventArgs e)
        {
            string fileName; // 文件名    
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "读取数据文件";
            openFileDialog.InitialDirectory = Environment.SpecialFolder.MyComputer.ToString();
            openFileDialog.Filter = "灰度图文件(*.bmp)|*.bmp"; // 定义文件过滤
            openFileDialog.InitialDirectory = rootPath;
            openFileDialog.FilterIndex = 0;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
                if ((fileName != null) && (fileName != ""))
                {
                    this.currentPath = Path.GetFullPath(fileName);
                    this.Text = "灰度图填表工具 " + fileName;
                    LoadPicture(fileName);
                }
            }
        }

        /// <summary>
        /// 关闭文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// 读取数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bRead_Click(object sender, EventArgs e)
        {
            string fileName; // 文件名
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = Environment.SpecialFolder.MyComputer.ToString();
            openFileDialog.Filter = "数据文件(*.tab)|*.tab"; // 定义文件过滤
            openFileDialog.RestoreDirectory = true;
            openFileDialog.FilterIndex = 0;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
                if ((fileName != null) && (fileName != ""))
                {
                    this.currentPath = Path.GetFullPath(fileName);
                    dataTable.Clear(); // 清空旧数据
                    itemList.Clear(); // 清空旧数据
                    ReadFile(fileName);
                }
            }
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bSave_Click(object sender, EventArgs e)
        {
            string fileName = Path.Combine(currentPath, "area.tab");
            WriteFile(fileName);
        }

        /// <summary>
        /// 预览数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bView_Click(object sender, EventArgs e)
        {
            PreViewForm pForm = new PreViewForm();
            pForm.ShowData(dataTable);
            pForm.ShowDialog();
        }

        /// <summary>
        /// 清理标记
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bClear_Click(object sender, EventArgs e)
        {
            canvas.ClearPaint();
        }

        /// <summary>
        /// 显示已配置区域
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bMark_Click(object sender, EventArgs e)
        {
            List<int> markedIDList = new List<int>();
            foreach (string s in dataTable.Keys)
            {
                markedIDList.Add(int.Parse(s));
            }
            canvas.MultiMarkArea(markedIDList);
        }

        /// <summary>
        /// 鼠标双击图片
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (image != null)
            {
                int x = (int)((double)e.X * image.Width / canvas.Width);
                int y = (int)((double)e.Y * image.Height / canvas.Height);
                Bitmap bmp = new Bitmap(image);
                if ((x < bmp.Width) && (y < bmp.Height))
                {
                    Color pixelColor = bmp.GetPixel(x, y);
                    string id = GetID(pixelColor);
                    if (id != null)
                    {
                        string[] data = dataTable[id] as string[];
                        SettingForm sForm;

                        if (data != null)
                        {
                            sForm = new SettingForm(data[0], data[1], data[3], itemList);
                        }
                        else
                        {
                            sForm = new SettingForm(id, itemList);
                        }

                        sForm.RootPath = rootPath;
                        DialogResult result = sForm.ShowDialog();
                        if (result == DialogResult.OK)
                        {
                            if (data != null) // 数据已经初始化过，进行值重设
                            {
                                data[1] = sForm.AreaName;
                                data[2] = "0";
                                data[3] = sForm.BackgroundMusic;
                            }
                            else // 初始化数据
                            {
                                data = new string[4];
                                data[0] = id;
                                data[1] = sForm.AreaName;
                                data[2] = "0";
                                data[3] = sForm.BackgroundMusic;
                                dataTable[id] = data;
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        private void WriteFile(string fileName)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Title = "保存数据文件";
            saveFileDialog.FileName = fileName;
            saveFileDialog.Filter = "数据文件(*.tab)|*.tab"; // 定义文件过滤
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = saveFileDialog.FileName;
                if ((fileName != null) && (fileName != ""))
                {
                    string[] data;
                    string value;

                    try
                    {
                        FileInfo fi = new FileInfo(fileName);
                        StreamWriter sw;

                        if (!fi.Exists) // 文件不存在
                        {
                            string path = Path.GetDirectoryName(fileName); // 获取目录
                            if (!Directory.Exists(path)) // 目录不存在
                            {
                                Directory.CreateDirectory(path); // 先创建目录，再创建文件
                            }
                            FileStream fs = fi.Create();
                            fs.Close();
                            sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                        }
                        else
                        {
                            sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                        }

                        // 写入文件
                        sw.WriteLine("id\tname\tmiddlemap\tbackgroundmusic\t");
                        foreach (string s in dataTable.Keys)
                        {
                            data = dataTable[s] as string[];
                            if (data != null)
                            {
                                value = string.Format("{0}\t{1}\t{2}\t{3}\t", data[0], data[1], data[2], data[3]);
                                sw.WriteLine(value);
                            }
                        }

                        sw.Close();
                        MessageBox.Show(string.Format("文件{0}保存成功!", fileName));
                    }
                    catch (IOException ex)
                    {
                        MessageBox.Show("在保存灰度图配置文件时产生IO异常: " + ex.ToString());
                    }
                }
            }
        }

        /// <summary>
        /// 获取颜色对应的id
        /// </summary>
        /// <param name="color">颜色对象</param>
        /// <returns>对应的id</returns>
        private string GetID(Color color)
        {
            string id = null;

            id = ((int)color.B).ToString();

            return id;
        }

        /// <summary>
        /// 从文件读出数据
        /// </summary>
        /// <param name="fileName"></param>
        private void ReadFile(string fileName)
        {
            try
            {
                StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                string value = sr.ReadLine();
                string[] data;
                bool firstLine = true;

                while (value != null)
                {
                    if (firstLine) // 跳过第一行
                    {
                        firstLine = false;
                    }
                    else
                    {
                        data = value.Split(new char[] { '\t' });
                        dataTable[data[0]] = data;
                        if (!itemList.Contains(data[3])) // 添加背景音乐下拉框内容
                        {
                            itemList.Add(data[3]);
                        }
                    }

                    value = sr.ReadLine();
                }
                sr.Close();
                MessageBox.Show(string.Format("文件{0}读取成功!", fileName));
            }
            catch (IOException ex)
            {
                MessageBox.Show("在读取灰度图配置文件时产生IO异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 鼠标单击图片
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseClick(object sender, MouseEventArgs e)
        {
            LockWindowUpdate(canvas.Handle);

            if (e.Button == MouseButtons.Left) // 左键点击
            {
                if (image != null)
                {
                    int x = (int)((double)e.X * image.Width / canvas.Width);
                    int y = (int)((double)e.Y * image.Height / canvas.Height);
                    Bitmap bmp = new Bitmap(image);
                    bool blueString = true;

                    if ((x < bmp.Width) && (y < bmp.Height))
                    {
                        Color pixelColor = bmp.GetPixel(x, y);
                        string id = GetID(pixelColor);
                        if (id != null) // 计算出对应的id
                        {
                            if (pixelColor.B < 127) // 深色背景换用浅色文本显示
                            {
                                blueString = false;
                            }
                            string[] information = dataTable[id] as string[];
                            if (information != null) // 已经有该id的信息，直接显示
                            {
                                string path = information[3];
                                canvas.PaintString(string.Format("ID: {0}\n区域名称: {1}\n音乐文件: {2}", id, information[1], Path.GetFileName(path)), e.X, e.Y, blueString);
                            }
                            else // 没有该id的信息
                            {
                                canvas.PaintString(string.Format("ID: {0}\n区域名称: 无\n音乐文件: 无", id), e.X, e.Y, blueString);
                            }
                        }
                        else
                        {
                            canvas.PaintString("ID: 无\n区域名称: 无\n音乐文件: 无", e.X, e.Y, blueString);
                        }

                        canvas.MarkArea(pixelColor.B);
                    }
                }
            }
            else // 右键或别的键点击
            {
                canvas.ClearPaint();
            }

            LockWindowUpdate(IntPtr.Zero);
        }

        private void buttonItem1_Click(object sender, EventArgs e)
        {
            string strIniFileFullName = Directory.GetCurrentDirectory() + "\\GameDesingerTools.ini";
            string strEnginePath = IniReadValue("General", "3DEnginePath", strIniFileFullName);
            if (strEnginePath == null || strEnginePath == "")
            {
                MessageBox.Show("地图目录路径未设置!\r\n请把程序放到填表工具同一目录下!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string fileName; // 文件名
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "读取地图";
            openFileDialog.InitialDirectory = strEnginePath + "/data/source/maps/";
            openFileDialog.Filter = "地图文件(*.Map)|*.map|全部文件(*.*)|*.*"; // 定义文件过滤
            openFileDialog.RestoreDirectory = true;
            openFileDialog.FilterIndex = 0;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
                if ((fileName != null) && (fileName != ""))
                {
//                     MapFrm mapform = new MapFrm(strEnginePath, fileName);
//                     mapform.Show();
                }
            }
        }

        private string IniReadValue(string Section, string Key, string filepath) // 对ini文件进行读操作的函数
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, "", temp, 255, filepath);
            return temp.ToString();
        }

        /// <summary>
        /// 画布的大小改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void panel1_SizeChanged(object sender, EventArgs e)
        {
            int newWidth = canvas.Width;
            int newHeight = canvas.Height;
            canvas.ClearPaint();
            canvas.ResetSize(newWidth, newHeight);
        }
    }
}
