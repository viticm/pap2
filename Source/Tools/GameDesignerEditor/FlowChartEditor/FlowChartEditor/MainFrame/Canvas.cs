using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DevComponents.DotNetBar;

namespace FlowChartEditor
{
    public class Canvas:Panel
    {
        private List<GraphElement> graphElementList = new List<GraphElement>(); // 图元链表        
        private Background background; // 背景

        private GraphElement abbreviatGraphElement; // 缩略的图元
        private ConnectorContainer abbreviateLine; // 缩略图元的连接线
        private AbbreviateCanvas abbreviateCanvas; // 缩略图绘图板
        
        private Rectangle visibleRectangle = Rectangle.Empty; // 可视区域矩形
        private bool paintCanvas = true; // 是否刷新绘图板

        private Rectangle tempRectangle = Rectangle.Empty; // 临时矩形    
        private Size tempSize = Size.Empty; // 临时尺寸
        private Point tempPoint = Point.Empty; // 临时点
        private Point currentPoint; // 当前鼠标所在点        

        private Tooltip tooltipGraphElement = new Tooltip(Point.Empty); // 提示框对象
        private Rodman rodman = new Rodman(Point.Empty, new Size(10, 25)); // 游标对象
        private MultiSelectMark multiSelectMark = new MultiSelectMark(Point.Empty, Size.Empty); // 多选标记
        private GuideLine guideLine = new GuideLine(Point.Empty); // 调整线对象

        protected GraphSetting graphSetting = GraphSetting.GetGraphSetting(); // 绘图参数配置对象              

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
        /// 调整线对象
        /// </summary>
        public GuideLine CurrentGuideLine
        {
            get
            {
                return guideLine;
            }
        }

        /// <summary>
        /// 提示框对象
        /// </summary>
        public Tooltip TooltipGraphElement
        {
            get
            {
                return tooltipGraphElement;
            }
        }

        /// <summary>
        /// 多选标记
        /// </summary>
        public MultiSelectMark CurrentMultiSelectMark
        {
            get
            {
                return multiSelectMark;
            }
        }

        /// <summary>
        /// 游标对象
        /// </summary>
        public Rodman CurrentRodman
        {
            set
            {
                rodman = value;
            }

            get
            {
                return rodman;
            }
        }

        /// <summary>
        /// 可视区域矩形
        /// </summary>
        public Rectangle VisibleRectangle
        {
            get
            {
                return visibleRectangle;
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

                if (abbreviateCanvas != null)
                {
                    abbreviateCanvas.PaintCanvas = value;
                }
            }

            get
            {
                return paintCanvas;
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
        /// 图元链表属性
        /// </summary>
        public List<GraphElement> GraphElementList
        {
            set
            {
                graphElementList = value;
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
            }

            get
            {
                return background;
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

            get
            {
                return abbreviatGraphElement;
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

            get
            {
                return abbreviateLine;
            }
        }

        /// <summary>
        /// 绘制的方法
        /// </summary>
        /// <param name="e">绘制事件参数</param>
        protected override void  OnPaint(PaintEventArgs e)
        {
 	        base.OnPaint(e);

            if (paintCanvas) // 绘制绘图板
            {                
                // 获取绘图对象
                Graphics g = e.Graphics;                              

                // 设置绘图参数
                g.SmoothingMode = SmoothingMode.HighQuality;
                g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
                // g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit; // 提高文本绘制质量
                g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);

                // 缩放绘图板
                g.ScaleTransform(graphSetting.CanvasScale, graphSetting.CanvasScale);                                        

                // 绘制图元
                PaintGraghElement(g);   
                
                // 绘制缩略的图元
                PaintAbbreviateGraphElement(g);

                // 绘制多选框
                multiSelectMark.Paint(g);                                           

                // 绘制调整线
                guideLine.Paint(g);

                // 绘制游标
                rodman.Paint(g);                

                // 绘制调试信息
                PaintDebugMessage(g);

                // 绘制提示框
                tooltipGraphElement.Paint(g);
            }            
        }

        /// <summary>
        /// 绘制缩略的图元
        /// </summary>
        /// <param name="g">绘图对象</param>
        private void PaintAbbreviateGraphElement(Graphics g)
        {
            // 绘制缩略的图元
            if (abbreviatGraphElement != null)
            {
                abbreviatGraphElement.Paint(g);
            }

            // 绘制缩略图元的连接线
            if (abbreviateLine != null && (abbreviateLine.Location.X > 0 && abbreviateLine.Location.Y > 0))
            {
                abbreviateLine.AbbreviativePaint(g);
            }        
        }

        /// <summary>
        /// 绘制调试信息
        /// </summary>
        /// <param name="g">绘图对象</param>
        private void PaintDebugMessage(Graphics g)
        {
            if (background != null && background.DebugMode)
            {
                g.DrawString(currentPoint.ToString(), graphSetting.GraphElementTextFont, graphSetting.LineTextBrush, currentPoint.X - 100, currentPoint.Y);
            }
        }

        /// <summary>
        /// 绘制图元
        /// </summary>
        /// <param name="g">绘图对象</param>
        private void PaintGraghElement(Graphics g)
        {
            List<ConnectorContainer> textLineList = new List<ConnectorContainer>(); // 绘制文本的连接线链表

            foreach (GraphElement graphElement in graphElementList)
            {
                tempPoint.X = (int)(graphElement.Location.X * graphSetting.CanvasScale);
                tempPoint.Y = (int)(graphElement.Location.Y * graphSetting.CanvasScale);
                tempSize.Width = (int)(graphElement.ElementSize.Width * graphSetting.CanvasScale);
                tempSize.Height = (int)(graphElement.ElementSize.Height * graphSetting.CanvasScale);
                tempRectangle.Location = tempPoint;
                tempRectangle.Size = tempSize;

                if ((visibleRectangle.Contains(tempRectangle) ||
                    visibleRectangle.IntersectsWith(tempRectangle))) // 当图元在可视区域内再进行绘制
                {
                    graphElement.ShowID = background.DebugMode; // 显示调试信息
                    graphElement.Paint(g);

                    if (graphElement is ConnectorContainer && graphElement.Visible)
                    {
                        textLineList.Add(graphElement as ConnectorContainer);
                    }
                }
            }

            // 连接线的文本单独绘制
            foreach (ConnectorContainer line in textLineList)
            {
                if (line.ShowText)
                {
                    line.PaintText(g);
                }
            }
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
            g.SmoothingMode = SmoothingMode.HighSpeed;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);

            if (background != null)
            {                                
                int gridWidth = background.GridSize.Width;
                int gridHeight = background.GridSize.Height;
                int canvasWidth = this.background.CanvasSize.Width;
                int canvasHeight = this.background.CanvasSize.Height;

                // 当前鼠标所在点
                currentPoint = PointToClient(Cursor.Position) - (Size)AutoScrollPosition;

                // 计算当前可视区域    
                visibleRectangle.Location = ClientRectangle.Location - (Size)AutoScrollPosition;
                visibleRectangle.Size = ClientSize;                

                int leftX = (int)Math.Floor((float)visibleRectangle.Left / gridWidth) * gridWidth;
                int topY = (int)Math.Floor((float)visibleRectangle.Top / gridHeight) * gridHeight;

                if (canvasWidth < visibleRectangle.Size.Width
                    || canvasHeight < visibleRectangle.Size.Height)
                {
                    for (int i = 0; i < canvasWidth; i += gridWidth)
                    {
                        g.DrawLine(graphSetting.GridLinePen, i, 0, i, canvasHeight);
                    }

                    for (int i = 0; i < canvasHeight; i += gridHeight)
                    {
                        g.DrawLine(graphSetting.GridLinePen, 0, i, canvasWidth, i);
                    } 

                    // 绘制边框
                    g.DrawLine(graphSetting.LineNormalPen, 0, 0, canvasWidth, 0);
                    g.DrawLine(graphSetting.LineNormalPen, 0, 0, 0, canvasHeight);
                    g.DrawLine(graphSetting.LineNormalPen, canvasWidth, 0, canvasWidth, canvasHeight);
                    g.DrawLine(graphSetting.LineNormalPen, 0, canvasHeight, canvasWidth, canvasHeight);
                }
                else
                {
                    for (int i = leftX; i < visibleRectangle.Right; i += gridWidth)
                    {
                        g.DrawLine(graphSetting.GridLinePen, i, visibleRectangle.Top, i, visibleRectangle.Bottom);
                    }

                    for (int i = topY; i < visibleRectangle.Bottom; i += gridHeight)
                    {
                        g.DrawLine(graphSetting.GridLinePen, visibleRectangle.Left, i, visibleRectangle.Right, i);
                    } 
                }            
            }
        }
    }
}
