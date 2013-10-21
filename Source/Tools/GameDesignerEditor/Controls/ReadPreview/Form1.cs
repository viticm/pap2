using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Data.SqlClient;
using System.Diagnostics;
//
//richtextbox的高度是固定的，行数，列数->客户区大小-> richtextbox label textbox button的位置
//客户区和richtextbox的高度决定了图片的绘画位置，位置是居中的。
//没有写图片大小改变客户区大小，这时可以改变行数和列数来显示大的图片
//
namespace ReadPreview
{
    public partial class Form1 : Form
    {
        /// <summary>
        /// 阅读书籍预览
        /// </summary>
        /// <param name="rtbxstr">书籍内容或者图片编号</param>
        /// <param name="Showtype">预览样式：1横排，2竖排，3竖排图片</param>
        /// <param name="ClientDir">客户端路径</param>
        public Form1(string rtbxstr, int Showtype, string ClientDir, SqlConnection conn)
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer |
                    ControlStyles.ResizeRedraw |
                    ControlStyles.AllPaintingInWmPaint, true);
            richTextBox1.Width = ClientRectangle.Width;
            richTextBox1.Text = rtbxstr;
            m_Font = new Font("宋体", 10.5f);
        }
        int m_nRowsCnt;
        int m_nColumnsCnt;
        Font m_Font;
        int m_nFaceType;
        SqlConnection m_conn;
        string m_strIcoPath;

        int m_nVLineSpace = 25;//竖排时行间距
        int m_nHLineSpace = 17;//横排时行间距

        int m_nDrawWidth = 620;//绘画区宽度
        int m_nDrawHeight = 335;//绘画区高度

        Image m_Image;
        //Image m_TLImage;
        //Image m_CTImage;
        //Image m_TRImage;
        //Image m_BLImage;
        //Image m_CBImage;
        //Image m_BRImage;
        Image m_bkImagePic;
        Image m_bkImageBook;
        Image m_bkImageBookH;

        const int HOFFSET = 50;//内容显示区与客户区的横方向差距
        const int VOFFSET = 70;//
        

        const int BOTTONY = 23;//23是下面label textbox button的大约高度


        public bool Init(string rtbxstr, int Showtype, string ClientDir, SqlConnection conn)
        {
            if(Showtype > 3 || Showtype < 1)
            {
                MessageBox.Show("Make sure type = 1,2 or 3");
                return false;
            }
            if (Showtype == 1)
            {
                m_nRowsCnt = 17;
                m_nColumnsCnt = 17;
            }
            else if (Showtype == 2)
            {
                m_nRowsCnt = 19;
                m_nColumnsCnt = 20;
            }
            else if (Showtype == 3)
            {
                m_nRowsCnt = 38;
                m_nColumnsCnt = 20;
            }//if (Showtype == 1)

            UpdateLineSpace();
            m_nFaceType = Showtype;
            RowCnttbx.Text = m_nRowsCnt.ToString();
            ColCnttbx.Text = m_nColumnsCnt.ToString(); 
            richTextBox1.Text = rtbxstr;
            m_conn = conn;
           
            m_strIcoPath = ClientDir + "\\ui\\Image\\icon";
            if (m_nFaceType == 3)
            {
                InitImage();               
            }
            InitBkImage();
            return true;
        }
        protected void InitImage()
        {
            int nFrame = 0;
            string IcoName = GetIcoName(ref nFrame);
            if (IcoName == "")
            {
               System.Diagnostics.Debug.Print("未能从数据库里读取相应的文件名");
               MessageBox.Show("没有与当前Contents对应的图片文件");
                return;
            }
            m_Image = uitex2img(m_strIcoPath, m_strIcoPath + "\\" + IcoName, nFrame);
            if (m_Image == null)
            {
                MessageBox.Show("没有与当前Contents对应的图片文件");
            }
        }

        protected void InitBkImage()
        {
//             int nFrame = 0;
//             string strUitexPath = m_strIcoPath.Replace("icon", "QuestPanel");
//             m_TLImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleTL.UITex", nFrame);
//             m_CTImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleCT.UITex", nFrame);
//             m_TRImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleTR.UITex", nFrame);
//             m_BLImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleBL.UITex", nFrame);
//             m_CBImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleCB.UITex", nFrame);
//             m_BRImage = uitex2img(strUitexPath, strUitexPath + "\\LargeScaleBR.UITex", nFrame);

            string strBkPic = Path.Combine(Application.StartupPath, "Design/pic.png");
            m_bkImagePic = Image.FromFile(strBkPic);
            string strBkBook = Path.Combine(Application.StartupPath, "Design/book.png");
            m_bkImageBook = Image.FromFile(strBkBook);
            string strBkBookH = Path.Combine(Application.StartupPath, "Design/book-h.png");
            m_bkImageBookH = Image.FromFile(strBkBookH);
        }
        private void Form1_Paint_1(object sender, PaintEventArgs e)
        {
            Bitmap bufferBMP = new Bitmap(ClientRectangle.Width, richTextBox1.Location.Y); // 绘图的缓冲
            Graphics g = Graphics.FromImage(bufferBMP);
            g.Clear(this.BackColor);
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality; //高质量
            g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality; //高像素偏移质量            

            DrawBkGround(g);
           
            if (1==m_nFaceType)
            {
                DrawHorizontal(g);
                
            }
            else if (2==m_nFaceType)
            {
                DrawVertical(g);
            }
            else if (3 == m_nFaceType && m_Image !=null)
            {
                int x = (ClientRectangle.Width - m_Image.Width) / 2;
                int y = (ClientRectangle.Height - m_Image.Height - richTextBox1.Height) / 2 - BOTTONY;
                g.DrawImage(m_Image,x,y);
            }
            e.Graphics.DrawImage(bufferBMP, 0, 0);
        }

        /// <summary>
        ///竖排画文本 
        /// </summary>
        /// <param name="g"></param>
        protected void DrawVertical(Graphics g)
        {
            StringFormat drawFormat = new StringFormat(); 
            drawFormat.FormatFlags = StringFormatFlags.DirectionRightToLeft | StringFormatFlags.DirectionVertical;
            string strOneLine = "";
            float x = ClientRectangle.Width - HOFFSET;
            float y = VOFFSET;
            int nStartIndex = 0;
            string strAll = richTextBox1.Text;
            int nCnt = 0;
            while(nStartIndex < strAll.Length && nCnt < m_nColumnsCnt)
            {
                strOneLine = GetNextLine(strAll, ref nStartIndex, m_nRowsCnt);
                g.DrawString(strOneLine, m_Font, Brushes.Black, x, y, drawFormat);
                x -= m_nVLineSpace;
                nCnt++;
            }
            drawFormat.Dispose(); 

        }

        protected void DrawHorizontal(Graphics g)
        {
            string strOneLine = "";
            float x = HOFFSET;
            float y = VOFFSET;
            int nStartIndex = 0;
            string strAll = richTextBox1.Text;
            int nCnt = 0;
            while (nStartIndex < strAll.Length && nCnt < m_nRowsCnt)
            {
                strOneLine = GetNextLine(strAll, ref nStartIndex, m_nColumnsCnt);
                g.DrawString(strOneLine, m_Font, Brushes.Black, x, y);
                y += m_nHLineSpace;
                nCnt++;
            }
        }
        /// <summary>
        /// 从索引nStartIndex开始获取strAll里可以竖排绘画的下一行
        /// 例如：长度满行数的字符串，或者以换行结尾的字符串
        /// </summary>
        /// <param name="strAll"></param>
        /// <param name="FullLength">满FullLength为一行</param>
        /// <param name="nStartIndex"></param>
        /// <returns></returns>
        protected string GetNextLine(string strAll, ref int nStartIndex, int nFullLength)
        {
            int nFirstRetrunIndex = strAll.IndexOf('\n', nStartIndex);
            if (nFirstRetrunIndex == -1)//如果没有换行，就等于最后一个字符的索引+1（以包含最后一个字符）
            {
                nFirstRetrunIndex = strAll.Length;
            }
            string strTemp = "";
            if (nFullLength < (nFirstRetrunIndex - nStartIndex))
            {
                strTemp = strAll.Substring(nStartIndex, nFullLength);
                nStartIndex += nFullLength;
            }                
            else
            {
                strTemp = strAll.Substring(nStartIndex, nFirstRetrunIndex - nStartIndex);
                nStartIndex = nFirstRetrunIndex + 1;
            }
            return strTemp;

        }
  
        protected void DrawBkGround(Graphics g)
        {
            switch (m_nFaceType)
            {
                case 1://横版
                    {
                        Rectangle r = new Rectangle(0, 0, ClientRectangle.Width, richTextBox1.Location.Y);
                        g.DrawImage(m_bkImageBookH, r);
                    }
                    break;

                case 2://竖版
                    {
                        Rectangle r = new Rectangle(0, 0, ClientRectangle.Width, richTextBox1.Location.Y);
                        g.DrawImage(m_bkImageBook, r);
                    }
                    break;

                case 3://图片
                    {
                        Rectangle r = new Rectangle(0, 0, ClientRectangle.Width, richTextBox1.Location.Y);
                        g.DrawImage(m_bkImagePic, r);
                    }
                    break;
                default:
                    break;
            }

            //DrawBlackGroundPart(m_TLImage, 1, g);
            //DrawBlackGroundPart(m_CTImage, 2, g);            
            //DrawBlackGroundPart(m_BLImage, 4, g);
            //DrawBlackGroundPart(m_CBImage, 5, g);
            //if (1==m_nFaceType)//横排只画一半
            //{
            //    return;
            //}
            //DrawBlackGroundPart(m_TRImage, 3, g);
            //DrawBlackGroundPart(m_BRImage, 6, g);
        }
        protected void DrawBlackGroundPart(Image theImage, int nPos, Graphics g)
        {            
            if (theImage != null)
            {
               
                Rectangle r = new Rectangle(0, 0, ClientRectangle.Width / 3 + 1, richTextBox1.Location.Y/2+1);
                if (1==m_nFaceType) //横排只画一半背景
                {
                    r.Width = ClientRectangle.Width * 2 / 3 + 1;
                }
                switch(nPos)
                {
                    case 1:
                        r.X = 0;
                        r.Y = 0;
                        break;
                    case 2:
                        if (m_nFaceType == 1)
                        {
                            r.X = ClientRectangle.Width * 2 / 3;
                        }
                        else
                        {
                            r.X = ClientRectangle.Width / 3;
                        }                        
                        r.Y = 0;
                        break;
                    case 3:
                        r.X = (ClientRectangle.Width*2)/3;
                        r.Y = 0;
                        break;
                    case 4:
                        r.X = 0;
                        r.Y = richTextBox1.Location.Y/2;
                        break;
                    case 5:
                        if (m_nFaceType == 1)
                        {
                            r.X = ClientRectangle.Width * 2 / 3;
                        }
                        else
                        {
                            r.X = ClientRectangle.Width / 3;
                        }   
                        r.Y = richTextBox1.Location.Y/2;
                        break;
                    case 6:
                        r.X = (ClientRectangle.Width*2)/3;
                        r.Y = richTextBox1.Location.Y/2;
                        break;
                }
                g.DrawImage(theImage, r);
                
            }
            
        }
        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            if (m_nFaceType == 3)
            {
                InitImage();
            }
            this.Invalidate();
        }       

        private void Form1_ClientSizeChanged(object sender, EventArgs e)
        {

            richTextBox1.Width = ClientRectangle.Width;
            richTextBox1.Location = new Point(0, ClientRectangle.Height - richTextBox1.Height - BOTTONY);
            int y = ClientRectangle.Height - BOTTONY;
            this.label1.Location = new Point(0, y);
            this.ColCnttbx.Location = new Point(50, y);
            this.label2.Location = new Point(135, y);
            this.RowCnttbx.Location = new Point(180, y);
            this.OKbtn.Location = new Point(270, y);
            this.Cancelbtn.Location = new Point(330, y);
            this.Invalidate();
        }

        /// <summary>
        /// 更新行间距和窗口大小
        /// </summary>
        protected void UpdateLineSpace()
        {
            if (m_nColumnsCnt > 0)
            {
                m_nVLineSpace = m_nDrawWidth / m_nColumnsCnt;
            }
            if (1 == m_nFaceType)
            {
                this.Width = this.Width - ClientRectangle.Width + m_nDrawWidth + HOFFSET;
            }
            else
            {
                this.Width = this.Width - ClientRectangle.Width + m_nDrawWidth + HOFFSET * 2;
            }            
            if (m_nRowsCnt > 0)
            {
                m_nHLineSpace = m_nDrawHeight / m_nRowsCnt;
            }
            this.Height = this.Height - ClientRectangle.Height + m_nDrawHeight + richTextBox1.Height + BOTTONY + VOFFSET * 2;
        }

        protected void ChgDrawWidth()
        {
            Graphics g = this.CreateGraphics();
            string strMeasure = new string('测',m_nColumnsCnt + 2);
            SizeF stringSize = new SizeF();
            stringSize = g.MeasureString(strMeasure, m_Font);
            m_nDrawWidth = (int)stringSize.Width;

            if (g != null)
            {
                g.Dispose();
            }
        }
        protected void ChgDrawHeight()
        {
            Graphics g = this.CreateGraphics();
            string strMeasure = new string('测', m_nRowsCnt);
            SizeF stringSize = new SizeF();
            stringSize = g.MeasureString(strMeasure, m_Font);
            m_nDrawHeight = (int)stringSize.Width;

            if (g != null)
            {
                g.Dispose();
            }
        }
        private void ColCnttbx_TextChanged(object sender, EventArgs e)
        {
            if (ColCnttbx.Text == "")
            {
                ColCnttbx.Text = "0";
            }
            m_nColumnsCnt = Convert.ToInt32(ColCnttbx.Text);
            if (1 == m_nFaceType)
            {
                ChgDrawWidth();
            }

            UpdateLineSpace();
            this.Invalidate();

        }

        private void RowCnttbx_TextChanged(object sender, EventArgs e)
        {
            if (RowCnttbx.Text == "")
            {
                RowCnttbx.Text = "0";
            }
            m_nRowsCnt = Convert.ToInt32(RowCnttbx.Text);
            if (2 == m_nFaceType)
            {
                ChgDrawHeight();
            }
            UpdateLineSpace();
            this.Invalidate();

        }

        private void ColCnttbx_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != 8 && e.KeyChar < 48 || e.KeyChar > 57)
            {
                e.KeyChar = '\0';
            }

        }
        private void RowCnttbx_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != 8 && e.KeyChar < 48 || e.KeyChar > 57)
            {
                e.KeyChar = '\0';
            }
        }
        public string GetText()
        {
            return richTextBox1.Text;
        }       

        private void OKbtn_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;

        }

        private void Cancelbtn_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void richTextBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == ' ')
            {
                e.KeyChar = '　';
            }
        }
      
        /// <summary>
        /// 从数据库读取FileName       
        /// </summary>        
        ///        
        protected string GetIcoName(ref int nFrame)
        {
            string IcoName = "";
            try
            {
                if (m_conn.State == ConnectionState.Closed)
                {
                    m_conn.Open();
                }
                SqlCommand cmd = new SqlCommand("select FileName, Frame from icon where ID = " + richTextBox1.Text, m_conn);
                SqlDataReader reader = cmd.ExecuteReader();
                if (reader.Read())
                {
                    IcoName = reader[0].ToString();
                    nFrame = reader.GetInt32(1);
                }
                m_conn.Close();
            }
            catch(SqlException e)
            {              
                System.Diagnostics.Debug.Print("读取数据库icon失败"+e.ToString());
            }         
            return IcoName;   

        }  
        
        //以下是从spViewer里拷贝过来的
        /// <summary>
        /// UITex转Img
        /// </summary>
        /// <param name="strIconDir">UITex的TGA所在目录，一般和UITex目录相同</param>
        /// <param name="filename">完整UITex文件名（含路径）</param>
        /// <param name="frame">第几帧</param>
        /// <returns></returns>
        public static Image uitex2img(string strIconDir, string filename, int frame)
        {
            //读取UITex文件，分析出当前帧坐标和长宽
            BinaryReader b = new BinaryReader(File.Open(filename, FileMode.Open));
            /*读文件名*/
            byte[] tb = new byte[64];
            b.BaseStream.Seek(0x18, System.IO.SeekOrigin.Begin);

            tb = b.ReadBytes(64);

            filename = System.Text.Encoding.Default.GetString(tb);
            filename = strIconDir + "\\" + filename.Substring(0, filename.IndexOf('\0'));

            b.BaseStream.Seek(0x0c, System.IO.SeekOrigin.Begin);
            /*帧总数*/
            int allframe = b.ReadInt32();
            if (frame > allframe - 1)
            {
                b.Close();
                return (null);
            }
            /*读帧坐标、高宽*/
            b.BaseStream.Seek(0x58 + frame * 0x14, SeekOrigin.Begin);
            int dwInfo = b.ReadInt32();
            int nLeft = b.ReadInt32();
            int nTop = b.ReadInt32();
            int nWidth = b.ReadInt32();
            int nHeight = b.ReadInt32();
            b.Close();

            //裁剪图片
            Image timg = new Bitmap(nWidth, nHeight);
            Graphics ee = Graphics.FromImage(timg);
            Point p1 = new Point(0, 0);
            Point pWidth = new Point(nWidth, 0);
            Point pHeight = new Point(0, nHeight);
            Point[] destPara1 = { p1, pWidth, pHeight };
            Rectangle srcRect = new Rectangle(nLeft, nTop, nWidth, nHeight);
            ee.DrawImage(tga2img(filename), destPara1, srcRect, GraphicsUnit.Pixel);
            return (timg);
        }

        /// <summary>
        /// TGA转Image
        /// </summary>
        /// <param name="tgaFilename">TGA完整文件名</param>
        /// <returns>Image</returns>
        public static Image tga2img(string tgaFilename)
        {
            //读取TGA文件，分析出长宽
            BinaryReader b = new BinaryReader(File.Open(tgaFilename, FileMode.Open));
            b.BaseStream.Seek(0x0C, System.IO.SeekOrigin.Begin);
            int nWidth = b.ReadInt16();
            int nHeight = b.ReadInt16();
            b.Close();

            //送tgaPreview输出jpg文件
            string tmpFilename = Path.GetTempFileName();
            Panel p = new Panel();
            tgaPreview.Init(p.Handle);
            tgaPreview.SetFileNames(p.Handle, tgaFilename + "\r\n", (UInt16)nWidth, (UInt16)nHeight);
            tgaPreview.Render(p.Handle);
            tgaPreview.FrameMove(p.Handle);
            tgaPreview.ExportToFile(p.Handle, (UInt16)nWidth, (UInt16)nHeight, tmpFilename);
            tgaPreview.UnInit(p.Handle);

            //载入jpg文件
            FileStream fs = new FileStream(tmpFilename, FileMode.Open, FileAccess.Read);
            int byteLength = (int)fs.Length;
            byte[] wf = new byte[byteLength];
            fs.Read(wf, 0, byteLength);
            fs.Close();
            Image img = Image.FromStream(new MemoryStream(wf));  
            File.Delete(tmpFilename);
            return(img);
        }

      
        
    }; 

/// <summary>
    /// tgaPreview.dll 引用
    /// </summary>
    public static class tgaPreview
    {
        [DllImport("TGAPreview.dll")]
        public static extern void Init(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void UnInit(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void SetFileNames(IntPtr hRenderWnd, string Filenames, UInt16 nWidth, UInt16 nHeight);

        [DllImport("TGAPreview.dll")]
        public static extern void OnResize(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void FrameMove(IntPtr hRenderWnd);  //可调整，每秒10帧，就每秒调用10次

        [DllImport("TGAPreview.dll")]
        public static extern void Render(IntPtr hRenderWnd);     //每秒30次调用

        [DllImport("TGAPreview.dll")]
        public static extern bool ExportToFile(IntPtr hRenderWnd, UInt16 nWidth, UInt16 nHeight, string Filenames);
    };

}