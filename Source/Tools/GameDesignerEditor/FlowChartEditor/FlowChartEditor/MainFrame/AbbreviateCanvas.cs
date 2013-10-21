using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using GraphElementLibrary;

namespace FlowChartEditor
{
    public class AbbreviateCanvas:Panel
    {
        private Point visibleRegionLocation; // 可视区域的位置
        private Color frameColor = Color.RoyalBlue; // 可视区域普通状态的颜色
        private Color selectedColor = Color.Red; // 可视区域被选中状态的颜色
        private Color activatedColor = Color.DarkOrange; // 可视区域被激活状态的颜色
        private bool selected = false; // 可视区域是否被选中
        private bool activated = false; // 可视区域是否被激活
        private bool tracking = false; // 是否正在拖动可视区域
        private bool paintCanvas = false; // 是否绘制绘图板
        private Point lastMovePoint; // 上一个移动的位置
        private Point regionLocation = Point.Empty; // 绘制的当前区域的位置
        private Size regionSize = Size.Empty; // 绘制的当前区域的大小
        private Canvas currentCanvas; // 当前对应的绘图板
        private Point tooltipLocation; // 提示框的位置
        private StringFormat stringFormat = new StringFormat(); // 字体格式对象
        private float horizontalScale; // 绘图板水平缩放参数
        private float verticalScale; // 绘图板垂直缩放参数
        private float wlRate; // 绘图板宽高比
        private Size validOffset = new Size(0, 0); // 有效区域偏移
        private Point validRegionLocaiton = new Point(0, 0); // 有效区域位置

        Brush frameBrush = Brushes.Black;
        Brush backgroundBrush = Brushes.LightYellow;
        Brush textBrush = Brushes.Black;
        Pen framePen = new Pen(Color.RoyalBlue); // 边框画笔

        private Rectangle validRegion = Rectangle.Empty; // 有效绘图区域

        protected GraphSetting graphSetting = GraphSetting.GetGraphSetting(); // 绘图参数配置对象

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
            stringFormat.Alignment = StringAlignment.Center;
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
        /// 绘制方法
        /// </summary>
        /// <param name="e">绘制参数</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            if (paintCanvas && Visible) // 检查是否可以绘制绘图板
            {               
                // 获取绘图对象
                Graphics g = e.Graphics;            
                // 设置绘图参数
                g.PixelOffsetMode = PixelOffsetMode.HighSpeed;                

                Helper helper = Helper.GetHelper();                

                if (graphSetting.CanvasScale != 1.0f) // 绘图板在缩放状态
                {
                    SizeF textSize = g.MeasureString("在缩放状态下无法查看缩略图", graphSetting.GraphElementTextFont);
                    int textX = (int)((this.Size.Width - textSize.Width) / 2);
                    int textY = (int)((this.Size.Height - textSize.Height) / 2);
                    g.DrawString("在缩放状态下无法查看缩略图", graphSetting.GraphElementTextFont, Brushes.Black, textX, textY);                    
                }
                else
                {
                    // 计算出当前有效区域                          
                    horizontalScale = (float)this.Size.Width / currentCanvas.AutoScrollMinSize.Width;
                    verticalScale = (float)this.Size.Height / currentCanvas.AutoScrollMinSize.Height;
                    wlRate = (float)currentCanvas.AutoScrollMinSize.Width / currentCanvas.AutoScrollMinSize.Height;                    

                    if (horizontalScale < verticalScale) // 水平优先
                    {
                        validRegion.Width = this.Size.Width;
                        validRegion.Height = (int)(this.Size.Width / wlRate);
                        validRegionLocaiton.X = 0;
                        validRegionLocaiton.Y = (int)((this.Size.Height - validRegion.Height) / 2);
                        validRegion.Location = validRegionLocaiton;
                    }
                    else // 垂直优先
                    {
                        validRegion.Height = this.Size.Height;
                        validRegion.Width = (int)(this.Size.Height * wlRate);
                        validRegionLocaiton.X = (int)((this.Size.Width - validRegion.Width) / 2);
                        validRegionLocaiton.Y = 0;
                        validRegion.Location = validRegionLocaiton;
                    }

                    horizontalScale = (float)validRegion.Width / currentCanvas.AutoScrollMinSize.Width;
                    verticalScale = (float)validRegion.Height / currentCanvas.AutoScrollMinSize.Height;

                    validOffset.Width = (int)((this.Size.Width - validRegion.Width) / (2 * horizontalScale));
                    validOffset.Height = (int)((this.Size.Height - validRegion.Height) / (2 * verticalScale));                    

                    regionLocation.X = -currentCanvas.AutoScrollPosition.X;
                    regionLocation.Y = -currentCanvas.AutoScrollPosition.Y;
                    regionSize.Width = currentCanvas.ClientSize.Width;
                    regionSize.Height = currentCanvas.ClientSize.Height;

                    // 坐标转换
                    g.ScaleTransform(horizontalScale, verticalScale);

                    // 坐标平移
                    g.TranslateTransform(validOffset.Width, validOffset.Height);

                    // 绘制图元                    
                    foreach (GraphElement graphElement in currentCanvas.GraphElementList)
                    {
                        graphElement.AbbreviativePaint(g);
                    }                    
                                                            
                    // 绘制可视区域
                    Color penColor = frameColor;
                    
                    if(activated)
                    {
                        penColor = activatedColor;
                    }
                    else if(selected)
                    {
                        penColor = selectedColor;
                    }

                    framePen.Color = penColor;                                                           
                    Rectangle regionRectangle = new Rectangle(regionLocation, currentCanvas.ClientSize);
                    g.DrawRectangle(framePen, regionRectangle);

                    // 绘制提示文本
                    if (activated)
                    {   
                        // 恢复转换后的坐标
                        g.TranslateTransform(-validOffset.Width, -validOffset.Height);
                        g.ScaleTransform(1 / horizontalScale, 1 / verticalScale);

                        SizeF tooltipSize = g.MeasureString("鼠标拖动移动当前绘图区域", graphSetting.GraphElementTextFont);
                        int textX = tooltipLocation.X + 5;
                        int textY = tooltipLocation.Y + 5;
                        int width = (int)(tooltipSize.Width + 10);
                        int height = (int)(tooltipSize.Height + 10);
                        Rectangle toolTipRectangle = new Rectangle(tooltipLocation, new Size(width, height));                        

                        g.FillRectangle(backgroundBrush, toolTipRectangle);
                        g.DrawRectangle(new Pen(Color.Black), toolTipRectangle);
                        g.DrawString("鼠标拖动移动当前绘图区域", graphSetting.GraphElementTextFont, textBrush, new PointF(textX, textY));
                    } 
                }                                                               
            }            
        }

        /// <summary>
        /// 绘制背景
        /// </summary>
        /// <param name="e">绘制参数</param>
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            base.OnPaintBackground(e);

            // 获取绘图对象
            Graphics g = e.Graphics;
            g.Clear(Color.Gray);
            g.FillRectangle(Brushes.White, validRegion);
        }

        /// <summary>
        /// 当前点是否在可视区域内
        /// </summary>
        /// <param name="p">当前点位置</param>
        /// <returns>是否在可视区域内</returns>
        private bool IsInRegion(Point p)
        {                       
            Rectangle regionRectangle = new Rectangle(new Point(-currentCanvas.AutoScrollPosition.X, -currentCanvas.AutoScrollPosition.Y), 
                currentCanvas.ClientSize);
            // 先平移再放缩
            Point point = new Point(p.X - validOffset.Width, p.Y - validOffset.Height);            
            point.X = (int)(p.X / horizontalScale);
            point.Y = (int)(p.Y / verticalScale);
            bool result = regionRectangle.Contains(point);

            return result;
        }

        /// <summary>
        /// 移动可视区域
        /// </summary>
        /// <param name="p"></param>
        public void MoveRegion(Point p)
        {
            activated = false;

            if (currentCanvas != null && (graphSetting.CanvasScale == 1.0f))
            {
                if (tracking) // 可视绘图区域
                {
                    int horizontalValue = (int)((p.X - lastMovePoint.X) / horizontalScale);
                    int verticalValue = (int)((p.Y - lastMovePoint.Y) / verticalScale);                                                   
                    currentCanvas.Visible = false; // 避免闪烁

                    if (regionLocation.X + horizontalValue >= 0 &&
                        regionLocation.X + regionSize.Width + horizontalValue <= currentCanvas.AutoScrollMinSize.Width &&
                        currentCanvas.HorizontalScroll.Value + horizontalValue >= currentCanvas.HorizontalScroll.Minimum &&
                        currentCanvas.HorizontalScroll.Value + horizontalValue <= currentCanvas.HorizontalScroll.Maximum)
                    {
                        currentCanvas.HorizontalScroll.Value += horizontalValue;
                    }
                    else
                    {
                        if (horizontalValue > 0) // 向右移动可视区域
                        {
                            currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Maximum;
                        }
                        else // 向左移动可视区域
                        {
                            currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Minimum;
                        }
                    }

                    if (regionLocation.Y + verticalValue >= 0 &&
                        regionLocation.Y + regionSize.Height + verticalValue <= currentCanvas.AutoScrollMinSize.Height &&
                        currentCanvas.VerticalScroll.Value + verticalValue >= currentCanvas.VerticalScroll.Minimum &&
                        currentCanvas.VerticalScroll.Value + verticalValue <= currentCanvas.VerticalScroll.Maximum)
                    {
                        currentCanvas.VerticalScroll.Value += verticalValue;
                    }
                    else
                    {
                        if (verticalValue > 0) // 向下移动可视区域
                        {
                            currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Maximum;
                        }
                        else // 向上移动可视区域
                        {
                            currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Minimum;
                        }
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

                    Invalidate();
                }
            }    
        }

        /// <summary>
        /// 切换可视区域的位置
        /// </summary>
        /// <param name="p">当前点位置</param>
        private void ChangeVisibleRegionLocation(Point p)
        {
            if (currentCanvas != null && (graphSetting.CanvasScale == 1.0f))
            {
                int horizontalValue = (int)((p.X - validRegion.X) / horizontalScale - regionLocation.X - regionSize.Width / 2);
                int verticalValue = (int)((p.Y - validRegion.Y) / verticalScale - regionLocation.Y - regionSize.Height / 2);
                currentCanvas.Visible = false; // 避免闪烁

                if (regionLocation.X + horizontalValue >= 0 &&
                    regionLocation.X + regionSize.Width + horizontalValue <= currentCanvas.AutoScrollMinSize.Width &&
                    currentCanvas.HorizontalScroll.Value + horizontalValue >= currentCanvas.HorizontalScroll.Minimum &&
                    currentCanvas.HorizontalScroll.Value + horizontalValue <= currentCanvas.HorizontalScroll.Maximum)
                {
                    currentCanvas.HorizontalScroll.Value += horizontalValue;
                }
                else
                {
                    if (horizontalValue > 0) // 向右移动可视区域
                    {
                        currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Maximum;
                    }
                    else // 向左移动可视区域
                    {
                        currentCanvas.HorizontalScroll.Value = currentCanvas.HorizontalScroll.Minimum;
                    }
                }

                if (regionLocation.Y + verticalValue >= 0 &&
                    regionLocation.Y + regionSize.Height + verticalValue <= currentCanvas.AutoScrollMinSize.Height &&
                    currentCanvas.VerticalScroll.Value + verticalValue >= currentCanvas.VerticalScroll.Minimum &&
                    currentCanvas.VerticalScroll.Value + verticalValue <= currentCanvas.VerticalScroll.Maximum)
                {
                    currentCanvas.VerticalScroll.Value += verticalValue;
                }
                else
                {
                    if (verticalValue > 0) // 向下移动可视区域
                    {
                        currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Maximum;
                    }
                    else // 向上移动可视区域
                    {
                        currentCanvas.VerticalScroll.Value = currentCanvas.VerticalScroll.Minimum;
                    }
                }

                selected = true;
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
            if(currentCanvas != null && (graphSetting.CanvasScale == 1.0f))
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
                    if (validRegion.Contains(p))
                    {
                        ChangeVisibleRegionLocation(p);
                    }                    
                }

                Invalidate();
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
