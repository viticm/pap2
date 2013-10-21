using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using GraphElementLibrary;
using DevComponents.DotNetBar;

namespace FlowChartEditor
{
    public class Canvas:Panel
    {
        private List<GraphElement> graphElementList = new List<GraphElement>(); // 图元链表
        private Background background; // 背景

        private bool showAbbreviatGraphElement = false; // 是否显示缩略的图元
        private GraphElement abbreviatGraphElement; // 缩略的图元
        private GraphElement abbreviateLine; // 缩略图元的连接线

        private bool autoConnect = false; // 是否自动连接图元

        private bool multiSelect = false; // 是否处于多选图元阶段
        private bool showMultiSelectRegion = false; // 是否显示多选图元框选的区域
        private Rectangle multiSelectRectangle; // 多选绘制的多边形
        private Rectangle multiSelectRegion; // 多选图元框选的区域

        private Bitmap canvasAbbreviateBitmap; // 绘图板的缩略图
        private bool refreshAbbreviateBitmap = true; // 是否刷新绘图板的缩略图
        private AbbreviateCanvas abbreviateCanvas; // 缩略图绘图板

        private Point tooltipImageLocation; // 提示图片的位置
        private Size tooltipImageSize; // 提示图片的大小
        private Image tooltipImage; // 提示图片
        private bool showTooltipImage; // 显示提示图片

        private Rectangle tooltipTextRectangle; // 提示文本的背景矩形
        private Point tooltipTextLocation; // 提示文本的文本位置

        private Rectangle visibleRectangle = Rectangle.Empty; // 可视区域矩形
        private Rectangle tooltipImageRectangle = Rectangle.Empty; // 提示图片的绘制矩形

        private bool firstShow = true; // 是否是第一次显示
        private bool paintCanvas = true; // 是否刷新绘图板

        private bool showConflictRectangle = false; // 是否显示冲突区域
        private Rectangle conflictRectangle; // 冲突区域

        private Brush frameBrush = Brushes.Black;
        private Brush backgroundBrush;
        private Brush textBrush;
        private Pen multiSelectRegionPen;
        private Pen multiSelectPen;
        private Color multiSelectColor;
        private Brush multiSelectBrush;
        private Pen gridPen;

        /// <summary>
        /// 构造函数
        /// </summary>
        public Canvas()
        {
            // 设置双缓冲等绘制参数
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.ResizeRedraw, true);          
        }

        /// <summary>
        /// 释放绘图板的绘图资源
        /// </summary>
        public void Release()
        {
            backgroundBrush.Dispose();
            textBrush.Dispose();
            multiSelectRegionPen.Dispose();
            multiSelectPen.Dispose();
            multiSelectBrush.Dispose();
        }

        /// <summary>
        /// 重设绘图参数
        /// </summary>
        public void ResetArgs()
        {
            backgroundBrush = new SolidBrush(background.TooltipBackgroundColor);
            textBrush = new SolidBrush(background.ToolTipColor);
            multiSelectRegionPen = new Pen(background.MultiSelectRegionColor);
            multiSelectRegionPen.DashStyle = DashStyle.Dash;
            multiSelectPen = new Pen(background.MultiSelectColor);
            multiSelectColor = Color.FromArgb(127, background.MultiSelectBackgroundColor);
            multiSelectBrush = new SolidBrush(multiSelectColor);
            gridPen = new Pen(background.GridColor);            
        }

        /// <summary>
        /// 刷新缩略图绘图板
        /// </summary>
        public void RefreshAbbreviateCanvas()
        {
            if(abbreviateCanvas != null && abbreviateCanvas.Visible && refreshAbbreviateBitmap) // 需要刷新缩略图
            {
                abbreviateCanvas.PaintCanvas = false; // 避免使用释放的缩略图片资源的问题
                AbbreviateCanvas tempAbbreviateCanvas = abbreviateCanvas;
                abbreviateCanvas = null;

                if (canvasAbbreviateBitmap != null)
                {
                    canvasAbbreviateBitmap.Dispose(); // 释放图片资源
                }
                canvasAbbreviateBitmap = new Bitmap(background.CanvasSize.Width, background.CanvasSize.Height);
                Graphics g = Graphics.FromImage(canvasAbbreviateBitmap);
                g.Clear(Color.White);

                // 绘制图元
                foreach (GraphElement graphElement in graphElementList)
                {
                    graphElement.AbbreviativePaint(g);
                }

                refreshAbbreviateBitmap = false;
                g.Dispose(); // 释放绘图资源
                tempAbbreviateCanvas.AbbreciateImage = canvasAbbreviateBitmap;
                tempAbbreviateCanvas.Invalidate();
                abbreviateCanvas = tempAbbreviateCanvas;
                abbreviateCanvas.PaintCanvas = true; // 避免使用释放的缩略图片资源的问题
            }
        }

        /// <summary>
        /// 是否刷新绘图板
        /// </summary>
        public bool PaintCanvas
        {
            set
            {
                paintCanvas = value;
            }

            get
            {
                return paintCanvas;
            }
        }

        /// <summary>
        /// 是否显示冲突区域
        /// </summary>
        public bool ShowConflictRectangle
        {
            set
            {
                showConflictRectangle = value;
            }
        }

        /// <summary>
        /// 冲突区域
        /// </summary>
        public Rectangle ConflictRectangle
        {
            set
            {
                conflictRectangle = value;
            }
        }

        /// <summary>
        /// 提示文本的背景矩形属性
        /// </summary>
        public Rectangle TooltipTextRectangle
        {
            set
            {
                tooltipTextRectangle = value;
            }

            get
            {
                return tooltipTextRectangle;
            }
        }

        /// <summary>
        /// 提示文本的文字位置
        /// </summary>
        public Point TooltipTextLocation
        {
            set
            {
                tooltipTextLocation = value;
            }

            get
            {
                return tooltipTextLocation;
            }
        }

        /// <summary>
        /// 是否显示多选图元框选的区域
        /// </summary>
        public bool ShowMultiSelectRegion
        {
            set
            {
                showMultiSelectRegion = value;
            }
        }

        /// <summary>
        /// 多选图元框选的区域属性
        /// </summary>
        public Rectangle MultiSelectRegion
        {
            set
            {
                multiSelectRegion = value;
            }

            get
            {
                return multiSelectRegion;
            }
        }

        /// <summary>
        /// 缩略图绘图板
        /// </summary>
        public AbbreviateCanvas CurrentAbbreviateCanvas
        {
            set
            {
                abbreviateCanvas = value;
            }

            get
            {
                return abbreviateCanvas;
            }
        }

        /// <summary>
        /// 是否刷新绘图板的缩略图属性
        /// </summary>
        public bool RefreshAbbreviateBitmap
        {
            set
            {
                refreshAbbreviateBitmap = value;
            }

            get
            {
                return refreshAbbreviateBitmap;
            }
        }

        /// <summary>
        /// 是否处于多选图元阶段
        /// </summary>
        public bool MultiSelect
        {
            set
            {
                multiSelect = value;
            }
        }

        /// <summary>
        /// 多选绘制的多边形
        /// </summary>
        public Rectangle MultiSelectRectangle
        {
            set
            {
                multiSelectRectangle = value;
            }
        }

        /// <summary>
        /// 图元链表属性
        /// </summary>
        public List<GraphElement> GraphElementList
        {
            set
            {
                graphElementList = value;
                if (firstShow)
                {
                    refreshAbbreviateBitmap = true;
                    abbreviateCanvas.AbbreciateImage = GetBitMap();
                    abbreviateCanvas.VisibleRegionLocation = this.AutoScrollPosition;
                    abbreviateCanvas.VisibleRegionSize = this.ClientSize;
                    abbreviateCanvas.EntireRegionSize = background.CanvasSize;
                    abbreviateCanvas.Invalidate();
                    abbreviateCanvas.PaintCanvas = true;
                    firstShow = false;
                }
            }

            get
            {
                return graphElementList;
            }
        }

        /// <summary>
        /// 背景属性
        /// </summary>
        public Background CanvasBackgound
        {
            set
            {
                background = value;
                ResetArgs(); // 重设绘图参数
            }
        }

        /// <summary>
        /// 是否显示缩略的图元属性
        /// </summary>
        public bool ShowAbbreviatGraphElement
        {
            set
            {
                showAbbreviatGraphElement = value;
            }
        }

        /// <summary>
        /// 缩略的图元属性
        /// </summary>
        public GraphElement AbbreviatGraphElement
        {
            set
            {
                abbreviatGraphElement = value;
            }
        }

        /// <summary>
        /// 缩略的连接线属性
        /// </summary>
        public ConnectorContainer AbbreviateLine
        {
            set
            {
                abbreviateLine = value;
            }
        }

        /// <summary>
        /// 是否自动连接缩略图元属性
        /// </summary>
        public bool AutoConnect
        {
            set
            {
                autoConnect = value;
            }
        }

        /// <summary>
        /// 绘制的方法
        /// </summary>
        /// <param name="e">绘制事件参数</param>
        protected override void  OnPaint(PaintEventArgs e)
        {
 	        base.OnPaint(e);

            if(paintCanvas) // 绘制绘图板
            {
                // 获取绘图对象
                Graphics g = e.Graphics;
                visibleRectangle.Location = ClientRectangle.Location - new Size(AutoScrollPosition);
                visibleRectangle.Size = ClientSize;

                // 设置绘图参数
                g.SmoothingMode = SmoothingMode.HighQuality;
                g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
                g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit; // 提高文本绘制质量
                g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);

                // 绘制缩略图元的连接线
                if(showAbbreviatGraphElement && autoConnect)
                {
                    abbreviateLine.AbbreviativePaint(g);
                }

                // 绘制图元
                foreach(GraphElement graphElement in graphElementList)
                {                
                    Rectangle rectangle = new Rectangle(graphElement.Location, graphElement.ElementSize);
                    if(graphElement.Visible && (visibleRectangle.Contains(rectangle) || visibleRectangle.IntersectsWith(rectangle))) // 当图元在可视区域内再进行绘制
                    {
                        graphElement.Paint(g); 
                    }  
                }

                // 绘制缩略的图元
                if(showAbbreviatGraphElement)
                {                
                    abbreviatGraphElement.Paint(g);
                }

                // 绘制多选框
                if(multiSelect)
                {
                    g.FillRectangle(multiSelectBrush, multiSelectRectangle);
                    g.DrawRectangle(multiSelectPen, multiSelectRectangle);
                }

                // 绘制多选图元框选区域
                if(showMultiSelectRegion)
                {
                    g.DrawRectangle(multiSelectRegionPen, multiSelectRegion);
                }
                
                // 绘制缩略图控件
                if(abbreviateCanvas != null && abbreviateCanvas.Visible)
                {
                    abbreviateCanvas.AbbreciateImage = GetBitMap();
                    abbreviateCanvas.VisibleRegionLocation = this.AutoScrollPosition;
                    abbreviateCanvas.VisibleRegionSize = this.ClientSize;
                    abbreviateCanvas.EntireRegionSize = background.CanvasSize;
                    abbreviateCanvas.Invalidate();
                }

                // 绘制冲突区域
                if(showConflictRectangle)
                {
                    g.FillRectangle(multiSelectBrush, conflictRectangle);
                    showConflictRectangle = false;
                }

                // 绘制提示的图片
                if(showTooltipImage)
                {
                    g.DrawImage(tooltipImage, tooltipImageRectangle);
                    showTooltipImage = false; // 只显示一次
                }

                // 绘制提示文本
                if(background.ShowTooltipText)
                {                
                    g.FillRectangle(backgroundBrush, tooltipTextRectangle);
                    g.DrawRectangle(new Pen(Color.Black), tooltipTextRectangle);
                    g.DrawString(background.TooltipText, background.TooltipFont, textBrush, tooltipTextLocation);
                }
            }            
        }

        /// <summary>
        /// 显示提示图片
        /// </summary>
        /// <param name="image">提示图片</param>
        /// <param name="imageLocation">图片的位置</param>
        /// <param name="imageSize">图片的大小</param>
        public void SetTooltipImage(Image image, Point imageLocation, Size imageSize)
        {
            tooltipImage = image;
            tooltipImageLocation = imageLocation;
            tooltipImageSize = imageSize;
            tooltipImageRectangle.Location = imageLocation;
            tooltipImageRectangle.Size = imageSize;
            showTooltipImage = true;
        }

        /// <summary>
        /// 获取当前绘图板的缩略位图
        /// </summary>
        /// <returns>当前绘图板的缩略位图</returns>
        private Bitmap GetBitMap()
        {
            Bitmap bmp;

            if(refreshAbbreviateBitmap && !Helper.GetHelper().AutoRefreshAbbreviateCanvas) // 需要刷新绘图板缩略图
            {                
                bmp = new Bitmap(background.CanvasSize.Width, background.CanvasSize.Height);
                Graphics g = Graphics.FromImage(bmp);
                g.Clear(Color.White);

                // 绘制图元
                foreach (GraphElement graphElement in graphElementList)
                {
                    graphElement.AbbreviativePaint(g);
                }

                if (canvasAbbreviateBitmap != null)
                {
                    canvasAbbreviateBitmap.Dispose(); // 释放图片资源
                }
                canvasAbbreviateBitmap = bmp;
                refreshAbbreviateBitmap = false;
                g.Dispose(); // 释放绘图资源
            }
            else
            {
                bmp = canvasAbbreviateBitmap;
            }

            return bmp;
        }

        /// <summary>
        /// 绘制背景
        /// </summary>
        /// <param name="e">绘制事件参数</param>
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            base.OnPaintBackground(e);

            // 获取绘图对象
            Graphics g = e.Graphics;

            // 设置绘图参数
            g.SmoothingMode = SmoothingMode.HighQuality;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);

            if(background != null)
            {
                int gridWidth = background.GridSize.Width;
                int gridHeight = background.GridSize.Height;
                int canvasWidth = this.background.CanvasSize.Width;
                int canvasHeight = this.background.CanvasSize.Height;

                for (int i = 0; i <= canvasWidth; i += gridWidth)
                {
                    g.DrawLine(gridPen, i, 0, i, canvasHeight);
                }

                for (int i = 0; i <= canvasHeight; i +=gridHeight)
                {
                    g.DrawLine(gridPen, 0, i, canvasWidth, i);
                }
            }
        }
    }
}
