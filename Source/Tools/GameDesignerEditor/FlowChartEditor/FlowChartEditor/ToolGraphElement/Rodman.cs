using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using GraphElementLibrary;

namespace FlowChartEditor
{
    public class Rodman : GraphElement
    {
        private Rectangle borderRectangle; // 边框矩形
        private RodmanType rodmanType = RodmanType.None; // 当前游标类型

        private GraphSetting graphSetting = GraphSetting.GetGraphSetting();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元位置</param>
        /// <param name="elementSize">图元大小</param>
        public Rodman(Point location, Size elementSize) : base(location)
        {
            this.elementSize = elementSize;
        }

        /// <summary>
        /// 当前游标类型
        /// </summary>
        public RodmanType CurrentRodmanType
        {
            set
            {
                rodmanType = value;
            }

            get
            {
                return rodmanType;
            }
        }

        /// <summary>
        /// 边框矩形
        /// </summary>
        public Rectangle BorderRectangle
        {
            set
            {
                borderRectangle = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        /// <param name="location">位置</param>
        /// <param name="borderSize">边框矩形</param>
        public void Init(Point location, Rectangle borderRectangle)
        {
            this.location = location;
            this.borderRectangle = borderRectangle;
        }        

        /// <summary>
        /// 绘制图元
        /// </summary>
        /// <param name="g">绘图对象</param>
        public override void Paint(Graphics g)
        {
            // 缩放状态就不要显示了
            if (visible && graphSetting.CanvasScale == 1.0f)
            {
                switch (rodmanType)
                {
                    case RodmanType.None:
                        {
                            break;
                        }
                    case RodmanType.Horizonal:
                        {
                            g.DrawLine(graphSetting.AdjustLinePen, location.X, borderRectangle.Top, location.X, borderRectangle.Bottom);
                            g.FillRectangle(graphSetting.RodmanBackgroundBrush, borderRectangle.Location.X, borderRectangle.Location.Y, borderRectangle.Width, (int)(elementSize.Width / 2));

                            if (activated)
                            {
                                g.FillRectangle(graphSetting.RodmanActivatedBrush, location.X - (int)(elementSize.Width / 2), borderRectangle.Top, elementSize.Width, elementSize.Height);
                            }
                            else
                            {
                                g.FillRectangle(graphSetting.RodmanSelectedBrush, location.X - (int)(elementSize.Width / 2), borderRectangle.Top, elementSize.Width, elementSize.Height);
                            }

                            break;
                        }
                    case RodmanType.Vertical:
                        {
                            g.DrawLine(graphSetting.AdjustLinePen, borderRectangle.Left, location.Y, borderRectangle.Right, location.Y);
                            g.FillRectangle(graphSetting.RodmanBackgroundBrush, borderRectangle.Location.X, borderRectangle.Location.Y, (int)(elementSize.Width / 2), borderRectangle.Height);

                            if (activated)
                            {
                                g.FillRectangle(graphSetting.RodmanActivatedBrush, borderRectangle.Left, location.Y - (int)(elementSize.Width / 2), elementSize.Height, elementSize.Width);
                            }
                            else
                            {
                                g.FillRectangle(graphSetting.RodmanSelectedBrush, borderRectangle.Left, location.Y - (int)(elementSize.Width / 2), elementSize.Height, elementSize.Width);
                            }

                            break;
                        }
                }
            }            
        }

        /// <summary>
        /// 绘制缩略图元
        /// </summary>
        /// <param name="g">绘图对象</param>
        public override void AbbreviativePaint(Graphics g)
        {
            
        }

        /// <summary>
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            bool inRegion = false;

            if (p.Y - borderRectangle.Top < elementSize.Height)
            {
                this.rodmanType = RodmanType.Horizonal;
                inRegion = true;
            }
            else if (p.X - borderRectangle.Left < elementSize.Height)
            {
                this.rodmanType = RodmanType.Vertical;
                inRegion = true;
            }
            else
            {
                this.rodmanType = RodmanType.None;
            }

            return inRegion;
        }

        /// <summary>
        /// 判断图元是否在某个矩形内
        /// </summary>
        /// <param name="r">当前矩形</param>
        /// <returns>图元是否在当前矩形内</returns>
        public override bool IsInRegion(Rectangle r)
        {
            return false;
        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="s">移动的相对位置</param>
        public override void Move(Size s)
        {
            this.location += s;
        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {

        }

        /// <summary>
        /// 释放图元的绘图资源
        /// </summary>
        public override void Release()
        {

        }
    }
}
