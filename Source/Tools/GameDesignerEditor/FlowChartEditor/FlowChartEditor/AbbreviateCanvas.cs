using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using DevComponents.DotNetBar;

namespace FlowChartEditor
{
    public class AbbreviateCanvas:Panel
    {
        private Image abbreviateImage; // 缩略图
        private Size visibleRegionSize; // 可视区域的大小
        private Size entireRegionSize; // 整个区域的大小
        private Point visibleRegionLocation; // 可视区域的位置
        private Color frameColor = Color.RoyalBlue; // 可视区域普通状态的颜色
        private Color selectedColor = Color.Red; // 可视区域被选中状态的颜色
        private Color activatedColor = Color.DarkOrange; // 可视区域被激活状态的颜色
        private bool selected = false; // 可视区域是否被选中
        private bool activated = false; // 可视区域是否被激活
        private bool tracking = false; // 是否正在拖动可视区域
        private bool paintCanvas = false; // 是否绘制绘图板
        private Point lastMovePoint; // 上一个移动的位置
        private Point regionLocation; // 绘制的当前区域的位置
        private Size regionSize; // 绘制的当前区域的大小
        private Canvas currentCanvas; // 当前对应的绘图板
        private Point tooltipLocation; // 提示框的位置

        /// <summary>
        /// 构造函数
        /// </summary>
        public AbbreviateCanvas()
        {
            // 设置双缓冲等绘制参数
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
        }

        /// <summary>
        /// 是否绘制绘图板
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
        /// 当前对应的绘图板属性
        /// </summary>
        public Canvas CurrentCanvas
        {
            set
            {
                currentCanvas = value;
            }
        }
        
        /// <summary>
        /// 整个区域的大小属性
        /// </summary>
        public Size EntireRegionSize
        {
            set
            {
                entireRegionSize = value;
            }
        }

        /// <summary>
        /// 可视区域的位置属性
        /// </summary>
        public Point VisibleRegionLocation
        {
            set
            {
                visibleRegionLocation = value;                
            }
        }

        /// <summary>
        /// 可视区域的大小属性
        /// </summary>
        public Size VisibleRegionSize
        {
            set
            {
                visibleRegionSize = value;
            }
        }

        /// <summary>
        /// 缩略图属性
        /// </summary>
        public Image AbbreciateImage
        {
            set
            {
                abbreviateImage = value;
            }
        }

        /// <summary>
        /// 绘制方法
        /// </summary>
        /// <param name="e">绘制参数</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            if(paintCanvas && abbreviateImage != null) // 检查是否可以绘制绘图板
            {               
                // 获取绘图对象
                Graphics g = e.Graphics;            
                // 设置绘图参数
                g.PixelOffsetMode = PixelOffsetMode.HighSpeed;

                Rectangle imageRectangle = new Rectangle(new Point(0, 0), this.Size);
                g.DrawImage(abbreviateImage, imageRectangle);

                // 绘制可视区域
                Color penColor = frameColor;
                if(selected)
                {
                    penColor = selectedColor;
                }
                else if(activated)
                {
                    penColor = activatedColor;
                }
                Pen framePen = new Pen(penColor);
                int regionWidth = (int)(visibleRegionSize.Width * this.Size.Width / entireRegionSize.Width);
                int regionHeight = (int)(visibleRegionSize.Height * this.Size.Height / entireRegionSize.Height);
                int regionX = (int)(-visibleRegionLocation.X * this.Size.Width / entireRegionSize.Width);
                int regionY = (int)(-visibleRegionLocation.Y * this.Size.Height / entireRegionSize.Height);
                if(!tracking) // 拖动时已经自动更新位置
                {
                    regionLocation = new Point(regionX, regionY);
                }                                
                regionSize = new Size(regionWidth, regionHeight);
                Rectangle regionRectangle = new Rectangle(regionLocation, regionSize);
                g.DrawRectangle(framePen, regionRectangle);

                // 绘制提示文本
                if(activated)
                {
                    Helper helper = Helper.GetHelper();
                    SizeF tooltipSize = g.MeasureString("鼠标拖动移动当前绘图区域", helper.TooltipFont);
                    int textX = tooltipLocation.X + 5;
                    int textY = tooltipLocation.Y + 5;
                    int width = (int)(tooltipSize.Width + 10);
                    int height = (int)(tooltipSize.Height + 10);
                    Rectangle toolTipRectangle = new Rectangle(tooltipLocation, new Size(width, height));

                    Brush frameBrush = Brushes.Black;
                    Brush backgroundBrush = Brushes.LightYellow;
                    Brush textBrush = Brushes.Black;

                    g.FillRectangle(backgroundBrush, toolTipRectangle);
                    g.DrawRectangle(new Pen(Color.Black), toolTipRectangle);
                    g.DrawString("鼠标拖动移动当前绘图区域", helper.TooltipFont, textBrush, new PointF(textX, textY));
                }                                                
            }            
        }

        /// <summary>
        /// 当前点是否在可视区域内
        /// </summary>
        /// <param name="p">当前点位置</param>
        /// <returns>是否在可视区域内</returns>
        private bool IsInRegion(Point p)
        {
            int regionWidth = (int)(visibleRegionSize.Width * this.Size.Width / entireRegionSize.Width);
            int regionHeight = (int)(visibleRegionSize.Height * this.Size.Height / entireRegionSize.Height);
            int regionX = (int)(-visibleRegionLocation.X * this.Size.Width / entireRegionSize.Width);
            int regionY = (int)(-visibleRegionLocation.Y * this.Size.Height / entireRegionSize.Height);
            Rectangle regionRectangle = new Rectangle(new Point(regionX, regionY), new Size(regionWidth, regionHeight));
            bool result = regionRectangle.Contains(p);

            return result;
        }

        /// <summary>
        /// 移动可视区域
        /// </summary>
        /// <param name="p"></param>
        public void MoveRegion(Point p)
        {
            activated = false;

            if (currentCanvas != null)
            {
                if (tracking) // 可视绘图区域
                {

                    int horizontalValue = p.X - lastMovePoint.X;
                    int verticalValue = p.Y - lastMovePoint.Y;
                    int horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                    int verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);
                    bool changeLocation = false; // 是否改变可视区域位置                               

                    currentCanvas.Visible = false; // 避免闪烁

                    if (regionLocation.X + horizontalValue >= 0 &&
                        regionLocation.X + regionSize.Width + horizontalValue <= this.Size.Width &&
                        currentCanvas.HorizontalScroll.Value + horizontalScrollValue >= currentCanvas.HorizontalScroll.Minimum &&
                        currentCanvas.HorizontalScroll.Value + horizontalScrollValue <= currentCanvas.HorizontalScroll.Maximum)
                    {
                        currentCanvas.HorizontalScroll.Value += horizontalScrollValue;
                        changeLocation = true;
                    }
                    else
                    {
                        if (horizontalValue > 0) // 向右移动可视区域
                        {
                            horizontalValue = this.Size.Width - regionSize.Width - regionLocation.X;
                            horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                            currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Maximum;
                        }
                        else // 向左移动可视区域
                        {
                            horizontalValue = -regionLocation.X;
                            horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                            currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Minimum;
                        }
                    }

                    if (regionLocation.Y + verticalValue >= 0 &&
                        regionLocation.Y + regionSize.Height + verticalValue <= this.Size.Height &&
                        currentCanvas.VerticalScroll.Value + verticalScrollValue >= currentCanvas.VerticalScroll.Minimum &&
                        currentCanvas.VerticalScroll.Value + verticalScrollValue <= currentCanvas.VerticalScroll.Maximum)
                    {
                        currentCanvas.VerticalScroll.Value += verticalScrollValue;
                        changeLocation = true;
                    }
                    else
                    {
                        if (verticalValue > 0) // 向下移动可视区域
                        {
                            verticalValue = this.Size.Height - regionSize.Height - regionLocation.Y;
                            verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);
                            currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Maximum;
                        }
                        else // 向上移动可视区域
                        {
                            verticalValue = -regionLocation.Y;
                            verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);
                            currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Minimum;
                        }
                    }

                    if (changeLocation)
                    {
                        regionLocation += new Size(horizontalValue, verticalValue);
                    }

                    lastMovePoint = p;
                    currentCanvas.Visible = true;
                    this.Refresh();
                    currentCanvas.Refresh();
                }
                else
                {
                    if (IsInRegion(p)) // 激活可视区域
                    {
                        activated = true;
                        tooltipLocation = p;
                    }

                    Invalidate(ClientRectangle);
                }
            }    
        }

        /// <summary>
        /// 切换可视区域的位置
        /// </summary>
        /// <param name="p">当前点位置</param>
        private void ChangeVisibleRegionLocation(Point p)
        {
            if (currentCanvas != null)
            {
                int horizontalValue = p.X - regionLocation.X - (int)(regionSize.Width / 2);
                int verticalValue = p.Y - regionLocation.Y - (int)(regionSize.Height / 2);
                int horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                int verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);

                currentCanvas.Visible = false; // 避免闪烁

                if (regionLocation.X + horizontalValue >= 0 &&
                    regionLocation.X + regionSize.Width + horizontalValue <= this.Size.Width &&
                    currentCanvas.HorizontalScroll.Value + horizontalScrollValue >= currentCanvas.HorizontalScroll.Minimum &&
                    currentCanvas.HorizontalScroll.Value + horizontalScrollValue <= currentCanvas.HorizontalScroll.Maximum)
                {                    
                    currentCanvas.HorizontalScroll.Value += horizontalScrollValue;
                }
                else
                {
                    if (horizontalValue > 0) // 向右移动可视区域
                    {
                        horizontalValue = this.Size.Width - regionSize.Width - regionLocation.X;
                        horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                        currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Maximum;
                    }
                    else // 向左移动可视区域
                    {
                        horizontalValue = -regionLocation.X;
                        horizontalScrollValue = (int)(horizontalValue * entireRegionSize.Width / this.Size.Width);
                        currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Minimum;
                    }
                }

                if (regionLocation.Y + verticalValue >= 0 &&
                    regionLocation.Y + regionSize.Height + verticalValue <= this.Size.Height &&
                    currentCanvas.VerticalScroll.Value + verticalScrollValue >= currentCanvas.VerticalScroll.Minimum &&
                    currentCanvas.VerticalScroll.Value + verticalScrollValue <= currentCanvas.VerticalScroll.Maximum)
                {
                    currentCanvas.VerticalScroll.Value += verticalScrollValue;
                }
                else
                {
                    if (verticalValue > 0) // 向下移动可视区域
                    {
                        verticalValue = this.Size.Height - regionSize.Height - regionLocation.Y;
                        verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);
                        currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Maximum;
                    }
                    else // 向上移动可视区域
                    {
                        verticalValue = -regionLocation.Y;
                        verticalScrollValue = (int)(verticalValue * entireRegionSize.Height / this.Size.Height);
                        currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Minimum;
                    }
                }
                
                selected = true;
                regionLocation += new Size(horizontalValue, verticalValue);
                lastMovePoint = p;
                currentCanvas.Visible = true;
                this.Invalidate();
                currentCanvas.Invalidate();
            }
        }

        /// <summary>
        /// 选中可视区域
        /// </summary>
        /// <param name="p">当前点位置</param>
        public void SelectRegion(Point p)
        {
            if(currentCanvas != null)
            {
                if(IsInRegion(p))
                {
                    selected = true;
                    tracking = true;
                    lastMovePoint = p;
                }
                else
                {
                    selected = false;

                    // 移动可视区域
                    ChangeVisibleRegionLocation(p);
                }

                Invalidate(new Rectangle(regionLocation, regionSize));
            }            
        }

        /// <summary>
        /// 结束移动
        /// </summary>
        /// <param name="p">当前点位置</param>
        public void EndMove(Point p)
        {
            tracking = false;
        }

        /// <summary>
        /// 结束移动
        /// </summary>
        public void EndMove()
        {
            tracking = false;
            activated = false;
            selected = false;
        }
    }
}
