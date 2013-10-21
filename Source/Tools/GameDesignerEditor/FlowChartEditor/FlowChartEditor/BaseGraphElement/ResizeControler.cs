using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class ResizeControler:GraphElement
    {
        private Direction direction; // 缩放控制点的方向
        private SlotContainer slotContainer; // 连接线控制点所属的连接线

        [NonSerialized]
        private Rectangle textRectangle = Rectangle.Empty;
        [NonSerialized]
        private Rectangle activateRectangle = Rectangle.Empty;

        [NonSerialized]
        protected GraphSetting graphSetting; // 绘图参数配置对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="line">缩放控制点所属的插槽容器</param>
        /// <param name="location">缩放控制点的位置</param>
        /// <param name="size">缩放控制点的大小</param>
        public ResizeControler(SlotContainer slotContainer, Point location, Size size)
            : base(location)
        {
            this.slotContainer = slotContainer;
            this.location = location;
            this.elementSize = size;
        }

        /// <summary>
        /// 图元的位置属性
        /// </summary>
        [Browsable(false)]
        public override Point Location
        {
            set
            {
                location = value;
                textRectangle.Location = value + new Size(-(int)(elementSize.Width / 2), -(int)(elementSize.Height / 2));
                activateRectangle.Location = value + new Size(-elementSize.Width / 2, -elementSize.Height / 2);
            }

            get
            {
                return location;
            }
        }

        /// <summary>
        /// 图元是否被选中属性
        /// </summary>
        [Browsable(false)]
        public override bool Selected
        {
            set
            {
                selected = value;               
            }

            get
            {
                return selected;
            }
        }

        /// <summary>
        /// 图元是否被激活属性
        /// </summary>
        [Browsable(false)]
        public override bool Activated
        {
            set
            {
                activated = value;               
            }

            get
            {
                return activated;
            }
        }

        /// <summary>
        /// 图元的刷新区域属性
        /// </summary>
        [Browsable(false)]
        public override Rectangle InvalidRectangle
        {
            get
            {
                invalidRectangle = new Rectangle(location + new Size(-elementSize.Width, -elementSize.Height), 
                    new Size(elementSize.Width * 2, elementSize.Height * 2));

                return base.InvalidRectangle;
            }           
        }

        /// <summary>
        /// 缩放控制点的方向属性
        /// </summary>
        [Browsable(false)]
        public Direction CurrentDirection
        {
            set
            {
                direction = value;
            }

            get
            {
                return direction;
            }
        }

        /// <summary>
        /// 缩放控制点所属的插槽容器属性
        /// </summary>
        [Browsable(false)]
        public SlotContainer Owner
        {
            get
            {
                return slotContainer;
            }
        }

        /// <summary>
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            bool result = false;

            Rectangle rectangle = new Rectangle(new Point(location.X - elementSize.Width,
                                                location.Y - elementSize.Height), new Size(elementSize.Width * 2, elementSize.Height * 2));
            result = rectangle.Contains(p);

            return result;
        }

        /// <summary>
        /// 判断图元是否在某个矩形内
        /// </summary>
        /// <param name="r">当前矩形</param>
        /// <returns>图元是否在当前矩形内</returns>
        public override bool IsInRegion(Rectangle r)
        {
            bool result = false;

            Rectangle rectangle = new Rectangle(location, elementSize);
            result = r.Contains(rectangle);

            return result;
        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {
            // 初始化绘图数据管理对象
            graphSetting = GraphSetting.GetGraphSetting();

            // 刷新逻辑数据
            textRectangle.Location = location + new Size(-(int)(elementSize.Width / 2), -(int)(elementSize.Height / 2));
            textRectangle.Size = elementSize;
            activateRectangle.Location = location + new Size(-elementSize.Width, -elementSize.Height);
            activateRectangle.Size = elementSize + elementSize;           
        }

        /// <summary>
        /// 释放图元的绘图资源
        /// </summary>
        public override void Release()
        {
            
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {           
            if (visible)
            {
                // 绘制边框色            
                if (activated) // 当前图元被激活
                {
                    g.FillRectangle(graphSetting.GraphElementActivatedBrush, activateRectangle);
                }
                else if(selected) // 当前图元被选中
                {
                    g.FillRectangle(graphSetting.GraphElementSelectedBrush, activateRectangle);
                }
                else // 当前图元没有被选中
                {
                    g.FillRectangle(graphSetting.GraphElementNormalBrush, textRectangle);
                }
            }            
        }

        /// <summary>
        /// 绘制缩略的图元
        /// </summary>
        /// <param name="g">绘图对象</param>
        public override void AbbreviativePaint(Graphics g)
        {
            
        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="p">移动的相对位置</param>
        public override void Move(Size s)
        {
            if(selected) // 缩放控制点在被选中状态，改变它对应的插槽容器的大小
            {
                int newX;
                int newY;
                Size locationDistancement = new Size(0, 0);
                Size elementSizeDistancement = new Size(0, 0);
                bool resizeSlotContainer = false; // 是否缩放插槽容器

                switch (direction)
                {
                    case Direction.Up:
                        {
                            locationDistancement = new Size(0, s.Height);
                            elementSizeDistancement = new Size(0, -s.Height);  

                            // 保证调整后图元的大小大于其最小大小               
                            if((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X;
                                newY = location.Y + s.Height;                            
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(0, s.Height);
                                activateRectangle.Location += new Size(0, s.Height);
                                resizeSlotContainer = true;
                            }                            

                            break;
                        }
                    case Direction.Up_Left:
                        {
                            locationDistancement = new Size(s.Width, s.Height);
                            elementSizeDistancement = new Size(-s.Width, -s.Height);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y + s.Height;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, s.Height);
                                activateRectangle.Location += new Size(s.Width, s.Height);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Left:
                        {
                            locationDistancement = new Size(s.Width, 0);
                            elementSizeDistancement = new Size(-s.Width, 0);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, 0);
                                activateRectangle.Location += new Size(s.Width, 0);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Down_Left:
                        {
                            locationDistancement = new Size(s.Width, 0);
                            elementSizeDistancement = new Size(-s.Width, s.Height);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y + s.Height;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, s.Height);
                                activateRectangle.Location += new Size(s.Width, s.Height);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Down:
                        {
                            elementSizeDistancement = new Size(0, s.Height);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X;
                                newY = location.Y + s.Height;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(0, s.Height);
                                activateRectangle.Location += new Size(0, s.Height);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Down_Right:
                        {
                            elementSizeDistancement = new Size(s.Width, s.Height);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y + s.Height;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, s.Height);
                                activateRectangle.Location += new Size(s.Width, s.Height);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Right:
                        {
                            elementSizeDistancement = new Size(s.Width, 0);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, 0);
                                activateRectangle.Location += new Size(s.Width, 0);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                    case Direction.Up_Right:
                        {
                            locationDistancement = new Size(0, s.Height);
                            elementSizeDistancement = new Size(s.Width, -s.Height);

                            // 保证调整后图元的大小大于其最小大小               
                            if ((slotContainer.ElementSize.Width + elementSizeDistancement.Width >= slotContainer.MinSize.Width) &&
                                (slotContainer.ElementSize.Height + elementSizeDistancement.Height >= slotContainer.MinSize.Height))
                            {
                                newX = location.X + s.Width;
                                newY = location.Y + s.Height;                                
                                location = new Point(newX, newY);
                                textRectangle.Location += new Size(s.Width, s.Height);
                                activateRectangle.Location += new Size(s.Width, s.Height);
                                resizeSlotContainer = true;
                            }

                            break;
                        }
                }

                // 调整缩放控制点对应的插槽容器的大小
                if(resizeSlotContainer)
                {
                    slotContainer.MoveBorder(locationDistancement, elementSizeDistancement);
                }                
            }
            else // 在非选中状态下，则直接移动缩放控制点即可
            {
                this.location += s;
                textRectangle.Location += s;
                activateRectangle.Location += s;
            }
        }
    }
}
