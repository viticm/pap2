using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.Specialized;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
//#if _DEBUG_1


namespace CustomLoadTree_Activity
{
    public enum EmfToWmfBitsFlags
    {

        // Use the default conversion
        EmfToWmfBitsFlagsDefault = 0x00000000,

        // Embedded the source of the EMF metafiel within the resulting WMF
        // metafile
        EmfToWmfBitsFlagsEmbedEmf = 0x00000001,

        // Place a 22-byte header in the resulting WMF file.  The header is
        // required for the metafile to be considered placeable.
        EmfToWmfBitsFlagsIncludePlaceable = 0x00000002,

        // Don't simulate clipping by using the XOR operator.
        EmfToWmfBitsFlagsNoXORClip = 0x00000004
    };



    class RTFHelper
    {
        /* RTF HEADER
        * ----------
        * 
        * \rtf[N]  - For text to be considered to be RTF, it must be enclosed in this tag.
        *      rtf1 is used because the RichTextBox conforms to RTF Specification
        *      version 1.
        * \ansi  - The character set.
        * \ansicpg[N] - Specifies that unicode characters might be embedded. ansicpg1252
        *      is the default used by Windows.
        * \deff[N]  - The default font. \deff0 means the default font is the first font
        *      found.
        * \deflang[N] - The default language. \deflang1033 specifies US English.
        * */
        private const string RTF_HEADER = @"{\rtf1\ansi\ansicpg1252\deff0\deflang1033";

        /* RTF DOCUMENT AREA
        * -----------------
        * 
        * \viewkind[N] - The type of view or zoom level.  \viewkind4 specifies normal view.
        * \uc[N]  - The number of bytes corresponding to a Unicode character.
        * \pard  - Resets to default paragraph properties
        * \cf[N]  - Foreground color.  \cf1 refers to the color at index 1 in
        *      the color table
        * \f[N]  - Font number. \f0 refers to the font at index 0 in the font
        *      table.
        * \fs[N]  - Font size in half-points.
        * */
        private const string RTF_DOCUMENT_PRE = @"\viewkind4\uc1\pard\cf1\f0\fs20";
        private const string RTF_DOCUMENT_POST = @"\cf0\fs17}";
        private const string RTF_IMAGE_POST = @"}";

        // Allows the x-coordinates and y-coordinates of the metafile to be adjusted
        // independently
        private const int MM_ANISOTROPIC = 8;

        // The number of twips in an inch
        // For more information, see GetImagePrefix() method.
        private const int TWIPS_PER_INCH = 1440;

        // The number of hundredths of millimeters (0.01 mm) in an inch
        // For more information, see GetImagePrefix() method.
        private const int HMM_PER_INCH = 2540;

        // The horizontal resolution at which the control is being displayed
        private static float xDpi;

        // The vertical resolution at which the control is being displayed
        private static float yDpi;


        public static string GetRTFFromTgaFile(string strFileName, Control CurControl, int nFrame)
        {
            int nIndexPos = strFileName.LastIndexOf(".");
            string ExpandName = strFileName.Substring(nIndexPos, strFileName.Length - nIndexPos).ToLower() ;

            Image _image;
            if (ExpandName == ".uitex")
            {
                int nFileNamePos = strFileName.LastIndexOf("\\");
                _image = uitex2img(strFileName.Substring(0, nFileNamePos), strFileName, nFrame);
            }
            else if (ExpandName == ".tga")
            {
                Paloma.TargaImage tgaImage = new Paloma.TargaImage(strFileName);
                _image = tgaImage.Image;
            }
            else
            {
                MessageBox.Show("错误的文件扩展名。");
                return string.Empty;
            }
            
            StringBuilder _rtf = new StringBuilder();

            using (Graphics _graphics = CurControl.CreateGraphics())
            {
                xDpi = _graphics.DpiX;
                yDpi = _graphics.DpiY;
            }
            _rtf.Append(RTF_HEADER);
            _rtf.Append(GetFontTable());
            _rtf.Append(GetImagePrefix(_image));
            _rtf.Append(GetRtfImage(_image, CurControl));
            _rtf.Append(RTF_IMAGE_POST);

            return _rtf.ToString();
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
            return (img);
        }


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

        private static string GetImagePrefix(Image _image) 
        {

           StringBuilder _rtf = new StringBuilder();

           // Calculate the current width of the image in (0.01)mm
           int picw = (int)Math.Round((_image.Width / xDpi) * HMM_PER_INCH);

           // Calculate the current height of the image in (0.01)mm
           int pich = (int)Math.Round((_image.Height / yDpi) * HMM_PER_INCH);

           // Calculate the target width of the image in twips
           int picwgoal = (int)Math.Round((_image.Width / xDpi) * TWIPS_PER_INCH);

           // Calculate the target height of the image in twips
           int pichgoal = (int)Math.Round((_image.Height / yDpi) * TWIPS_PER_INCH);

           // Append values to RTF string
           _rtf.Append(@"{\pict\wmetafile8");
           _rtf.Append(@"\picw");
           _rtf.Append(picw);
           _rtf.Append(@"\pich");
           _rtf.Append(pich);
           _rtf.Append(@"\picwgoal");
           _rtf.Append(picwgoal);
           _rtf.Append(@"\pichgoal");
           _rtf.Append(pichgoal);
           _rtf.Append(" ");

           return _rtf.ToString();
        }

        [DllImportAttribute("gdiplus.dll")]
        private static extern uint GdipEmfToWmfBits(IntPtr _hEmf, uint _bufferSize,
         byte[] _buffer, int _mappingMode, EmfToWmfBitsFlags _flags);


        private static string GetRtfImage(Image _image, Control CurControl) 
        {
            StringBuilder _rtf = null;

            // Used to store the enhanced metafile
            MemoryStream _stream = null;

            // Used to create the metafile and draw the image
            Graphics _graphics = null;

            // The enhanced metafile
            Metafile _metaFile = null;

            // Handle to the device context used to create the metafile
            IntPtr _hdc;

            try 
            {
                _rtf = new StringBuilder();
                _stream = new MemoryStream();

                // Get a graphics context from the RichTextBox
                using (_graphics = CurControl.CreateGraphics()) 
                {

                    // Get the device context from the graphics context
                    _hdc = _graphics.GetHdc();

                    // Create a new Enhanced Metafile from the device context
                    _metaFile = new Metafile(_stream, _hdc);

                    // Release the device context
                    _graphics.ReleaseHdc(_hdc);
                }
            
                using(_graphics = Graphics.FromImage(_metaFile)) 
                {

                    // Draw the image on the Enhanced Metafile
                    _graphics.DrawImage(_image, new Rectangle(0, 0, _image.Width, _image.Height));
                }
                IntPtr _hEmf = _metaFile.GetHenhmetafile();

                // A call to EmfToWmfBits with a null buffer return the size of the
                // buffer need to store the WMF bits.  Use this to get the buffer
                // size.
                uint _bufferSize = GdipEmfToWmfBits(_hEmf, 0, null, MM_ANISOTROPIC,
                EmfToWmfBitsFlags.EmfToWmfBitsFlagsDefault);

                // Create an array to hold the bits
                byte[] _buffer = new byte[_bufferSize];

                // A call to EmfToWmfBits with a valid buffer copies the bits into the
                // buffer an returns the number of bits in the WMF.  
                uint _convertedSize = GdipEmfToWmfBits(_hEmf, _bufferSize, _buffer, MM_ANISOTROPIC,
                EmfToWmfBitsFlags.EmfToWmfBitsFlagsDefault);

                // Append the bits to the RTF string
                for(int i = 0; i < _buffer.Length; ++i) 
                {
                    _rtf.Append(String.Format("{0:X2}", _buffer[i]));
                }

                return _rtf.ToString();
            }
            finally 
            {
                if(_graphics != null)
                _graphics.Dispose();
                if(_metaFile != null)
                _metaFile.Dispose();
                if(_stream != null)
                _stream.Close();
            }
        }

        /// <summary>
        /// Creates a font table from a font object.  When an Insert or Append 
        /// operation is performed a font is either specified or the default font
        /// is used.  In any case, on any Insert or Append, only one font is used,
        /// thus the font table will always contain a single font.  The font table
        /// should have the form ...
        /// 
        /// {\fonttbl{\f0\[FAMILY]\fcharset0 [FONT_NAME];}
        /// </summary>
        /// <param name="_font"></param>
        /// <returns></returns>
        private static string GetFontTable() 
        {

            StringBuilder _fontTable = new StringBuilder();

            _fontTable.Append(@"{\fonttbl{\f0");
            _fontTable.Append(@"\");
            _fontTable.Append("宋体");
            _fontTable.Append(@"\fcharset0 ");
            _fontTable.Append("宋体");
            _fontTable.Append(@";}}");

            return _fontTable.ToString();
        }
    }
}
//#endif