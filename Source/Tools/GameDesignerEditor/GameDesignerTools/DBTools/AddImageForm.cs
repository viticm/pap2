using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;
using System.Drawing.Imaging;

namespace GameDesingerTools.DBTools
{
    public partial class AddImageForm : Form
    {
        private string fileName; // 文件名称
        private Image image; // 图标
        Image previewImage; // 预览的图标
        private TreeNode node; // 树结点
        private ModelEdit modelEditor; // 表元编辑器对象
        private bool readSuccess = false; // 是否读取图标成功
        private List<string> encodableTypeList = new List<string>(); // 可以进行编码设置的类型链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="image">要定义的图标</param>
        public AddImageForm(string fileName, TreeNode node, ModelEdit modelEditor)
        {
            this.fileName = fileName;
            this.node = node;
            this.modelEditor = modelEditor;
            InitializeComponent();
            comboBoxEx1.SelectedIndex = 2; // 默认为低图标质量
            comboBoxEx2.SelectedIndex = 1; // 默认为16位色深
            FillEncodableList();
            ShowImage();
        }

        /// <summary>
        /// 填充可以进行编码设置的类型链表
        /// </summary>
        private void FillEncodableList()
        {
            /*
             * GDI+内置的编码器支持的文件类型有： 
             * 1 *.bmp, *.dib, *.rle
             * 2 *.jpg, *.jpeg, *.jpe, *.jfif
             * 3 *.gif
             * 4 *.tif, *.tiff
             * 5 *.png
             */
            encodableTypeList.Add(".bmp");
            encodableTypeList.Add(".dib");
            encodableTypeList.Add(".rle");
            encodableTypeList.Add(".jpg");
            encodableTypeList.Add(".jpeg");
            encodableTypeList.Add(".jpe");
            encodableTypeList.Add(".jfif");
            encodableTypeList.Add(".gif");
            encodableTypeList.Add(".tif");
            encodableTypeList.Add(".tiff");
            encodableTypeList.Add(".png");
        }

        /// <summary>
        /// 显示图标
        /// </summary>
        private void ShowImage()
        {
            try
            {
                image = Image.FromFile(fileName);
                readSuccess = true;
            }
            catch(Exception ex)
            {
                MessageBox.Show("无法识别的图标文件! \n\n" + ex.ToString());
                readSuccess = false;
            }

            if(readSuccess && image != null)
            {
                if(!encodableTypeList.Contains(Path.GetExtension(fileName).ToLower())) // 不支持进行编码设置的类型
                {
                    checkBoxX1.Enabled = false;
                }

                Image.GetThumbnailImageAbort callb = new Image.GetThumbnailImageAbort(ThumbnailCallback); 
                Image newImage = image.GetThumbnailImage(24, 24, callb, new IntPtr());
                label1.Image = newImage;
                image.Dispose(); // 释放image所占资源
                image = newImage;
            }
            else
            {
                DialogResult = DialogResult.No;
                this.Close();
            }
        }

        /// <summary>
        /// 回调方法
        /// </summary>
        /// <returns>返回值</returns>
        private bool ThumbnailCallback()
        {
            return false;
        }

        /// <summary>
        /// 获取图片的类型
        /// </summary>
        /// <param name="extention">文件后缀</param>
        /// <returns>图片类型</returns>
        private ImageFormat GetImageFormat(string extention)
        {
            ImageFormat format = ImageFormat.Bmp;

            switch(extention)
            {
                case ".jpg":
                    {
                        format = ImageFormat.Jpeg;
                        break;
                    }
                case ".jpeg":
                    {
                        format = ImageFormat.Jpeg;
                        break;
                    }
                case ".bmp":
                    {
                        format = ImageFormat.Bmp;
                        break;
                    }
                case ".emf":
                    {
                        format = ImageFormat.Emf;
                        break;
                    }
                case ".exif":
                    {
                        format = ImageFormat.Exif;
                        break;
                    }
                case ".gif":
                    {
                        format = ImageFormat.Gif;
                        break;
                    }
                case ".ico":
                    {
                        format = ImageFormat.Icon;
                        break;
                    }
                case ".icon":
                    {
                        format = ImageFormat.Icon;
                        break;
                    }
                case ".png":
                    {
                        format = ImageFormat.Png;
                        break;
                    }
                case ".tiff":
                    {
                        format = ImageFormat.Tiff;
                        break;
                    }
                case ".wmf":
                    {
                        format = ImageFormat.Wmf;
                        break;
                    }
            }

            return format;
        }

        /// <summary>
        /// 获取图标的编码信息
        /// </summary>
        /// <param name="mimeType">编码类型</param>
        /// <returns>编码信息</returns>
        private ImageCodecInfo GetImageCodecInfo(string extention)
        {
            ImageCodecInfo imageCodecInfo = null;
            ImageCodecInfo[] encoders = ImageCodecInfo.GetImageEncoders();
            /*
             * GDI+内置的编码器支持的文件类型有： 
             * 1 *.bmp, *.dib, *.rle
             * 2 *.jpg, *.jpeg, *.jpe, *.jfif
             * 3 *.gif
             * 4 *.tif, *.tiff
             * 5 *.png
             */
            foreach(ImageCodecInfo i in encoders)
            {
                if(i.FilenameExtension.Contains(extention))
                {
                    imageCodecInfo = i;
                }
            }
            return imageCodecInfo;
        }

        /// <summary>
        /// 利用临时文件保存图标信息
        /// </summary>
        /// <returns>字符缓冲</returns>
        private byte[] SaveImageInFile()
        {
            int index = 0;
            string tempFileName = index.ToString() + Path.GetExtension(fileName);
            byte[] buffer = null;
            try
            {
                while(File.Exists(tempFileName)) // 文件已经存在
                {
                    index++;
                    tempFileName = index.ToString() + Path.GetExtension(fileName);
                }
                image.Save(tempFileName);
                FileInfo fi = new FileInfo(tempFileName);
                Stream s = fi.OpenRead();
                buffer = new byte[(int)fi.Length];
                s.Read(buffer, 0, buffer.Length);
                s.Close();
                fi.Delete();
            }
            catch(Exception ex)
            {
                MessageBox.Show("在保存修改后的图标临时文件时产生异常: " + ex.ToString());
            }
            return buffer;
        }

        /// <summary>
        /// 获取自定义的编码参数
        /// </summary>
        /// <returns>自定义的编码参数</returns>
        private EncoderParameters GetConfigEncoderParameters()
        {
            long imageQuality = 20L; // 图标质量
            switch (comboBoxEx1.SelectedIndex) // 读取图标质量
            {
                case 0: // 高质量
                    {
                        imageQuality = 100L;
                        break;
                    }
                case 1: // 中质量
                    {
                        imageQuality = 60L;
                        break;
                    }
                case 2: // 低质量
                    {
                        imageQuality = 20L;
                        break;
                    }
            }

            long imageColorDepth = 16L; // 图标色深
            switch (comboBoxEx2.SelectedIndex) // 读取图标色深
            {
                case 0: // 8位色深
                    {
                        imageColorDepth = 8L;
                        break;
                    }
                case 1: // 16位色深
                    {
                        imageColorDepth = 16L;
                        break;
                    }
                case 2: // 24位色深
                    {
                        imageColorDepth = 24L;
                        break;
                    }
                case 3: // 32位色深
                    {
                        imageColorDepth = 32L;
                        break;
                    }
            }

            ImageCodecInfo myImageCodecInfo = GetImageCodecInfo(Path.GetExtension(fileName).ToUpper());
            System.Drawing.Imaging.Encoder myEncoder1 = System.Drawing.Imaging.Encoder.Quality; 
            System.Drawing.Imaging.Encoder myEncoder2 = System.Drawing.Imaging.Encoder.ColorDepth;
            EncoderParameters myEncoderParameters = new EncoderParameters(2);
            EncoderParameter myEncoderParameter1 = new EncoderParameter(myEncoder1, imageQuality);
            EncoderParameter myEncoderParameter2 = new EncoderParameter(myEncoder2, imageColorDepth);
            myEncoderParameters.Param[0] = myEncoderParameter1;
            myEncoderParameters.Param[1] = myEncoderParameter2;
            return myEncoderParameters;
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        private void SaveData()
        {
            // 更新数据库
            try
            {                
                if(MainForm.conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    MainForm.conn.Open();
                }

                EncoderParameters myEncoderParameters; // 编码参数
                if(checkBoxX1.Checked == true) // 用户自定义编码参数
                {
                    myEncoderParameters = GetConfigEncoderParameters();
                }
                else // 采用默认的编码参数
                {
                    myEncoderParameters = new EncoderParameters(1);
                    System.Drawing.Imaging.Encoder myEncoder = System.Drawing.Imaging.Encoder.Quality;
                    EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 100L);
                    myEncoderParameters.Param[0] = myEncoderParameter;
                }

                MemoryStream ms = new MemoryStream();
                ImageCodecInfo myImageCodecInfo = GetImageCodecInfo(Path.GetExtension(fileName).ToUpper());

                byte[] imageData = null; // 图标的信息
                if(checkBoxX1.Checked && myImageCodecInfo != null) // 有效的编码信息
                {
                    image.Save(ms, myImageCodecInfo, myEncoderParameters);
                    imageData = ms.GetBuffer();
                    ms.Read(imageData, 0, imageData.Length);
                }
                else // 利用文件流重新读取 
                {
                    imageData = SaveImageInFile();
                }
                

                // 更新预览效果
                // Image previewImage = Image.FromStream(ms);
                // label1.Image = previewImage;
                /* remoting ???
                SqlCommand cmd = MainForm.conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE sys_modl_def SET iconid = {0}, ImageData = @imageData WHERE id = {1}", 
                    -1, (int)node.Tag);
                SqlParameter paramData = new SqlParameter("@imageData", SqlDbType.Image);
                paramData.Value = imageData;
                cmd.Parameters.Add(paramData);
                cmd.ExecuteNonQuery();
                 */

                string filter = string.Format("id = {0}", (int)node.Tag);
                string _sql = "select * from sys_modl_def where " + filter;

                DataTable tbl = Helper.GetDataTableProxy("sys_modl_def", filter, null, MainForm.conn);
                foreach (DataRow row in tbl.Rows)
                {
                    row["iconid"] = -1;
                    row["ImageData"] = imageData;
                }
                Helper.SaveTable(tbl, _sql, MainForm.conn);


                modelEditor.imglstIcons.Images.Add(fileName, image);
                node.ImageIndex = modelEditor.imglstIcons.Images.Count - 1;
                node.SelectedImageIndex = modelEditor.imglstIcons.Images.Count - 1;
                modelEditor.LoadModelDefTable();
                Program.MainForm.RefreshIcons(node.Tag.ToString(), image); // 刷新模块id对应的Button图标
            }
            catch(Exception ex)
            {
                MessageBox.Show("在更新图标文件数据库时产生异常: " + ex.ToString());
            }
            finally
            {
                if(MainForm.conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    MainForm.conn.Close();
                }
            }
        }

        /// <summary>
        /// 用户选择确定string sql = "select * from sys_modl_def where ";
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            SaveData();
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 用户选择自定义编码参数
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            if(checkBoxX1.Checked == true) // 已经选中
            {
                groupPanel2.Enabled = true;
                if(previewImage == null) // 没有生成预览图标
                {
                    EncoderParameters myEncoderParameters = GetConfigEncoderParameters(); // 编码参数
                    MemoryStream ms = new MemoryStream(); // 内存流
                    ImageCodecInfo myImageCodecInfo = GetImageCodecInfo(Path.GetExtension(fileName).ToUpper());
                    image.Save(ms, myImageCodecInfo, myEncoderParameters);
                    byte[] imageData = ms.GetBuffer(); // 图标的信息
                    ms.Read(imageData, 0, imageData.Length);
                    previewImage = Image.FromStream(ms); // 预览的图标
                    label1.Image = previewImage;
                }
                else
                {
                    label1.Image = previewImage;
                }       
            }
            else // 取消选中
            {
                groupPanel2.Enabled = false;
                label1.Image = image;
            }
        }

        /// <summary>
        /// 用户改变图标质量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(image != null && checkBoxX1.Checked)
            {
                EncoderParameters myEncoderParameters = GetConfigEncoderParameters(); // 编码参数
                MemoryStream ms = new MemoryStream(); // 内存流
                ImageCodecInfo myImageCodecInfo = GetImageCodecInfo(Path.GetExtension(fileName).ToUpper());
                image.Save(ms, myImageCodecInfo, myEncoderParameters);
                byte[] imageData = ms.GetBuffer(); // 图标的信息
                ms.Read(imageData, 0, imageData.Length);
                previewImage = Image.FromStream(ms); // 预览的图标
                label1.Image = previewImage;
            }
        }

        /// <summary>
        /// 用户改变图标色深
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comboBoxEx2_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (image != null && checkBoxX1.Checked)
            {
                EncoderParameters myEncoderParameters = GetConfigEncoderParameters(); // 编码参数
                MemoryStream ms = new MemoryStream(); // 内存流
                ImageCodecInfo myImageCodecInfo = GetImageCodecInfo(Path.GetExtension(fileName).ToUpper());
                image.Save(ms, myImageCodecInfo, myEncoderParameters);
                byte[] imageData = ms.GetBuffer(); // 图标的信息
                ms.Read(imageData, 0, imageData.Length);
                previewImage = Image.FromStream(ms); // 预览的图标
                label1.Image = previewImage;
            }         
        }
    }
}