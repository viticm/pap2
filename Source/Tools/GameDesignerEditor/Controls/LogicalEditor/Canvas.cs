using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;

namespace LogicalEditor
{
    public class Canvas : Panel
    {
        private Brush blueBrush = Brushes.Blue; // 蓝色的刷子
        private Brush yellowBrush = Brushes.Yellow; // 黄色的刷子
        private Pen redPen = new Pen(Color.Red, 4); // 红色的笔
        private Pen greenPen = new Pen(Color.Green, 1); // 绿色的笔
        private Font timesFont = new Font("Times New Roman", 8, FontStyle.Bold); // 显示文字的字体
        private List<Point> markPointList = new List<Point>(); // 标记的点链表
        private List<Point> multiPointList = new List<Point>(); // 多区域标记的点链表
        private Bitmap bmp; // 背景图片
        Size littileSize = new Size(1, 1); // 1个像素的宽度
        public Image image; // 要绘制的图片
        private string text; // 要显示的文字
        private float textX; // 要显示的文字的横坐标
        private float textY; // 要显示的文字的纵坐标
        private bool blueString = true; // 用蓝色的刷子显示文字
        public int canvasWidth = 600; // 画布的宽度
        public int canvasHeight = 480; // 画布的高度
        public int m_lenDirEnd = 24; // 方向的长度

        public float scale = 1.0f; // 缩放
        public float start_adjust_x = 0, start_adjust_y = 0; // 用于中键移动。起始点

        public Hashtable m_ODIs = new Hashtable(); // ObjDrawInfo 的hash

        /// <summary>
        /// 构造函数
        /// </summary>
        public Canvas()
        {
            // 设置双缓冲
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer |
                    ControlStyles.ResizeRedraw |
                    ControlStyles.AllPaintingInWmPaint, true);
        }

        /// <summary>
        /// 画布的宽度属性
        /// </summary>
        public int CanvasWidth
        {
            get
            {
                return canvasWidth;
            }
        }

        /// <summary>
        /// 画布的高度属性
        /// </summary>
        public int CanvasHeight
        {
            get
            {
                return canvasHeight;
            }
        }

        // 选中点的 ODI
        public void AddODI(string key, ObjDrawInfo odi)
        {
            if (!m_ODIs.ContainsKey(key))
                m_ODIs.Add(key, odi);
        }
        public void RemoveODI(string key)
        {
            if (m_ODIs.ContainsKey(key))
                m_ODIs.Remove(key);
        }

        public PointF GetDirEndAdjustment(int nDir)
        {
            double angel = (double)nDir / 256 * Math.PI * 2;
            return new PointF((float)(m_lenDirEnd * Math.Cos(angel)), (float)(-m_lenDirEnd * Math.Sin(angel)));
        }

        public PointF GetODIClientPos(ObjDrawInfo odi)
        {
            float startscalex = ((1.0f - scale) * image.Width / 2 + start_adjust_x) / image.Width;
            float startscaley = ((1.0f - scale) * image.Height / 2 + start_adjust_y) / image.Height;
            float x = (odi.pfPos.X - startscalex) * canvasWidth / scale;
            float y = (odi.pfPos.Y - startscaley) * canvasHeight / scale;
            return new PointF(x, y);
        }

        /// <summary>
        /// 重写的绘图方法
        /// </summary>
        /// <param name="e">绘图事件参数</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics dc = e.Graphics;
            if (canvasWidth == 0 || canvasHeight == 0)
                return;

            Bitmap bufferBMP = new Bitmap(canvasWidth, canvasHeight); // 绘图的缓冲
            Graphics g = Graphics.FromImage(bufferBMP);
            g.Clear(this.BackColor);
            g.SmoothingMode =  SmoothingMode.HighQuality; //高质量
            g.PixelOffsetMode = PixelOffsetMode.HighQuality; //高像素偏移质量

            if (image != null) // 绘制图片
            {
                RectangleF destRect = new RectangleF(0, 0, canvasWidth, canvasHeight);
                RectangleF srcRect = new RectangleF((1.0f - scale) * image.Width / 2 + start_adjust_x, (1.0f - scale) * image.Height / 2 + start_adjust_y, image.Width * scale, image.Height * scale);
                g.DrawImage(image, destRect, srcRect, GraphicsUnit.Pixel);

                foreach (DictionaryEntry entry in m_ODIs)
                {
                    ObjDrawInfo odi = entry.Value as ObjDrawInfo;
                    PointF pointf = GetODIClientPos(odi);
                    g.DrawEllipse(redPen, pointf.X - 3, pointf.Y - 3, 7, 7);
                    int nDir = (int)(odi.nDir);
                    if ((this.Parent as Preview).m_ShowDir && nDir != -1)
                    {
                        PointF adjustment = GetDirEndAdjustment(nDir);
                        g.DrawLine(redPen, pointf.X, pointf.Y, pointf.X + adjustment.X, pointf.Y + adjustment.Y);
                    }
                    if ((this.Parent as Preview).m_ShowObjName)
                    {
                        LogicObj curObj = odi.lObj;
                        string strODI = string.Format("{0}[{1}]", curObj.tyPe.ToLower().EndsWith("point") ? "Point" : (curObj.tyPe.ToLower().EndsWith("vertex") ? "Vertex" : curObj.values[0]), curObj.inDex);
                        g.DrawString(strODI, timesFont, yellowBrush, pointf.X + 3, pointf.Y + 7);
                    }
                }
            }

            // 从缓冲中绘制最终图像
            dc.DrawImage(bufferBMP, 0, 0); // g.DrawLine(greenPen, p, p + littileSize);
        }

        /// <summary>
        /// 重设画布的尺寸
        /// </summary>
        /// <param name="width">宽度</param>
        /// <param name="height">高度</param>
        public void ResetSize(int width, int height)
        {
            this.canvasWidth = width;
            this.canvasHeight = height;
            this.Refresh();
        }

        /// <summary>
        /// 绘制图片
        /// </summary>
        /// <param name="image">图片</param>
        public void PaintPicture(Image img)
        {
            this.image = img;
            bmp = new Bitmap(this.image);
            this.Refresh();
        }

        /// <summary>
        /// 绘制文字
        /// </summary>
        /// <param name="text">文字内容</param>
        /// <param name="x">文字的横坐标</param>
        /// <param name="y">文字的纵坐标</param>
        /// <param name="blueString">文字是否用蓝色显示</param>
        public void PaintString(string text, int x, int y, bool blueString)
        {
            this.text = text;
            this.blueString = blueString;

            if (x > canvasWidth - 100)
            {
                textX = canvasWidth - 100;
            }
            else
            {
                textX = x;
            }

            if (y > canvasHeight - 50)
            {
                textY = canvasHeight - 50;
            }
            else
            {
                textY = y;
            }

            this.Refresh();
        }

        /// <summary>
        /// 标记选中的区域
        /// </summary>
        /// <param name="value">灰度参数</param>
        public void MarkArea(int value)
        {
            markPointList.Clear();
            Color color;
            int x;
            int y;

            for (int i = 1; i < canvasWidth; i += 5)
            {
                for (int j = 1; j < canvasHeight; j += 5)
                {
                    x = (int)((double)i * image.Width / canvasWidth);
                    y = (int)((double)j * image.Height / canvasHeight);
                    if ((x < bmp.Width) && (y < bmp.Height))
                    {
                        color = bmp.GetPixel(x, y);
                        if (color.B == value)
                        {
                            markPointList.Add(new Point(i, j));
                        }
                    }
                }
            }

            this.Refresh();
        }

        /// <summary>
        /// 绘制多标记区域
        /// </summary>
        /// <param name="values">区域的灰度链表</param>
        public void MultiMarkArea(List<int> values)
        {
            multiPointList.Clear();
            Color color;
            int x;
            int y;

            for (int i = 1; i < canvasWidth; i += 5)
            {
                for (int j = 1; j < canvasHeight; j += 5)
                {
                    x = (int)((double)i * image.Width / canvasWidth);
                    y = (int)((double)j * image.Height / canvasHeight);
                    if ((x < bmp.Width) && (y < bmp.Height))
                    {
                        color = bmp.GetPixel(x, y);
                        if (values.Contains(color.B))
                        {
                            multiPointList.Add(new Point(i, j));
                        }
                    }
                }
            }

            this.Refresh();
        }

        /// <summary>
        /// 清理绘图面板
        /// </summary>
        public void ClearPaint()
        {
            markPointList.Clear();
            multiPointList.Clear();
            text = null;
            this.Refresh();
        }
    }
}
