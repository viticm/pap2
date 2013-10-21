using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace MapCutter
{
    public static class ImageCutter
    {
        private static TGAHelper tgaHelper = new TGAHelper();

        public delegate void OnCuttingHandler(object sender, CutterEventArgs e);
        public static event OnCuttingHandler OnCutting;                

        public static Image ImageCutAndSave(string strImageFileName, int nWidth, int nHeight, float fscale, string strOutFolder, string strFormat)
        {
            //strFormat = "{0}_{1}.jpg"   表示 x_y.jpg
            Image imgsrc = Image.FromFile(strImageFileName);

            /*
            if (fscale != 1.0f)
                imgsrc = imgsrc.GetThumbnailImage((int)(imgsrc.Width * fscale + 0.5f), (int)(imgsrc.Height * fscale + 0.5f), null, IntPtr.Zero);

            int nCurFrame = 0;
            for (int i = 0, ii = 0; i < imgsrc.Width; i += nWidth, ii++)
            {
                for (int j = 0, jj = 0; j < imgsrc.Height; j += nHeight, jj++)
                {
                    // 触发切割过程事件
                    CutterEventArgs e = new CutterEventArgs();
                    e.nCurFrame = ++nCurFrame;
                    e.nAllFrame = (
                                   imgsrc.Width % nWidth == 0
                                   ? imgsrc.Width / nWidth
                                   : imgsrc.Width / nWidth + 1
                                   ) * (
                                   imgsrc.Height % nHeight == 0
                                   ? imgsrc.Height / nHeight
                                   : imgsrc.Height / nHeight + 1
                                   );
                    if (e.nAllFrame < e.nCurFrame)
                        e.nAllFrame = e.nCurFrame;
                    OnCutting(null, e);

                    // 裁减图片
                    Image timg = new Bitmap(nWidth, nHeight);
                    Graphics g = Graphics.FromImage(timg);
                    g.DrawImage(imgsrc, new Rectangle(0, 0, nWidth, nHeight), new Rectangle(i, imgsrc.Height - j - nHeight, nWidth, nHeight), GraphicsUnit.Pixel);
                    string fname = string.Format(strFormat, ii, jj);
                    fname = Path.Combine(strOutFolder, fname);
                    timg.Save(fname, ImageFormat.Jpeg);      
                    tgaHelper.SaveMinimapBlock(strOutFolder, 0, ii, jj, timg as Bitmap);              
                }
            }            
            */

            FileInfo fi = new FileInfo(strImageFileName);

            /* 默认切割的layer参数为0，不再根据文件名来定
            string fileName = fi.Name.Replace(".bmp", "");
            int layer = int.Parse(fileName);
            */

            int layer = 0;
            tgaHelper.CutImage(strImageFileName, strOutFolder, layer, nWidth, true);

            return imgsrc;
        }

        public class CutterEventArgs:EventArgs
        {
            public int nCurFrame;
            public int nAllFrame;
        };
    }
}
