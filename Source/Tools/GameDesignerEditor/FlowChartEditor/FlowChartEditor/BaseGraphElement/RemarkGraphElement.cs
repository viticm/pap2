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
    public class RemarkGraphElement:GraphElement
    {
        protected GraphElement owner; // 注释所属的图元
        protected Point headPoint = Point.Empty; // 连接线头结点位置
        protected Point tailPoint = Point.Empty; // 连接线尾结点位置
        protected Point textLocation = Point.Empty; // 绘制文本的位置

        [NonSerialized]
        Rectangle textRectangle = Rectangle.Empty;

        [NonSerialized]
        protected GraphSetting graphSetting; // 绘图参数配置对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="owner">注释所属的图元</param>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public RemarkGraphElement(GraphElement owner, Point location, Size elementSize):
            base(location)
        {
            this.owner = owner;
            this.location = location;
            this.elementSize = elementSize;
        }

        /// <summary>
        /// 图元的刷新区域属性
        /// </summary>
        [Browsable(false)]
        public override Rectangle InvalidRectangle
        {
            get
            {
                invalidRectangle = new Rectangle(location, elementSize);

                return invalidRectangle;
            }
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
                textRectangle.Location = value;
                AdjustLine(); // 调整连接线
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
        /// 图元是否激活属性
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
        /// 注释所属的图元属性
        /// </summary>
        [Browsable(false)]
        public virtual GraphElement Owner
        {
            set
            {
                owner = value;
            }

            get
            {
                return owner;
            }
        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {
            // 初始化绘图数据管理对象
            graphSetting = GraphSetting.GetGraphSetting();

            // 刷新逻辑数据
            textRectangle.Location = location;
            textRectangle.Size = elementSize;    
        }

        /// <summary>
        /// 释放图元的绘图资源
        /// </summary>
        public override void Release()
        {

        }

        /// <summary>
        /// 自动调整图元的大小
        /// </summary>
        public virtual void AdjustElementSize()
        {
            Graphics g = Helper.GetHelper().GraphicsObject;
            SizeF textSize = g.MeasureString(text, graphSetting.GraphElementTextFont);
            float textWidth = textSize.Width;
            float textHeight = textSize.Height;
            int newWidth = (int)(textWidth + 10);
            int newHeight = (int)(textHeight + 10);
            textLocation = location + new Size(5, 5);

            // 调整图元大小
            if (newWidth != elementSize.Width || newHeight != elementSize.Height)
            {
                elementSize = new Size(newWidth, newHeight);
                textRectangle.Size = elementSize;
            }
        }

        /// <summary>
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            bool result = textRectangle.Contains(p);

            return result;
        }

        /// <summary>
        /// 判断图元是否在某个矩形内
        /// </summary>
        /// <param name="r">当前矩形</param>
        /// <returns>图元是否在当前矩形内</returns>
        public override bool IsInRegion(Rectangle r)
        {
            bool result = r.Contains(textRectangle);

            return result;
        }

        /// <summary>
        /// 调整连接线
        /// </summary>
        public void AdjustLine()
        {
            int ownerX = (int)(owner.Location.X + owner.ElementSize.Width / 2);
            int ownerY = (int)(owner.Location.Y + owner.ElementSize.Height / 2);
            int x = location.X;
            int y = location.Y;
            int lineX1;
            int lineY1;
            int lineX2;
            int lineY2;

            if(ownerX >= x)
            {
                if(ownerY >= y) // 注释在所属图元的左上方
                {
                    lineX1 = location.X + elementSize.Width;
                    lineY1 = location.Y + elementSize.Height;
                    lineX2 = owner.Location.X;
                    lineY2 = owner.Location.Y;
                }
                else // 注释在所属图元的左下方
                {
                    lineX1 = location.X + elementSize.Width;
                    lineY1 = location.Y;
                    lineX2 = owner.Location.X;
                    lineY2 = owner.Location.Y + owner.ElementSize.Height;
                }
            }
            else
            {
                if(ownerY >= y) // 注释在所属图元的右上方
                {
                    lineX1 = location.X;
                    lineY1 = location.Y + elementSize.Height;
                    lineX2 = owner.Location.X + owner.ElementSize.Width;
                    lineY2 = owner.Location.Y;
                }
                else // 注释在所属图元的右下方
                {
                    lineX1 = location.X;
                    lineY1 = location.Y;
                    lineX2 = owner.Location.X + owner.ElementSize.Width;
                    lineY2 = owner.Location.Y + owner.ElementSize.Height;
                }
            }

            headPoint.X = lineX1;
            headPoint.Y = lineY1;
            tailPoint.X = lineX2;
            tailPoint.Y = lineY2;
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {
            if (visible)
            {
                // 绘制背景
                g.FillRectangle(graphSetting.RemarkBackgroundBrush, textRectangle);          

                // 绘制边框
                g.DrawRectangle(graphSetting.GraphElementBorderPen, textRectangle);

                // 绘制连接线           
                g.DrawLine(graphSetting.LineNormalPen, headPoint, tailPoint);
                
                //绘制文本
                g.DrawString(text, graphSetting.GraphElementTextFont, graphSetting.GraphElementTextBrush, textLocation);
            }            
        }

        /// <summary>
        /// 绘制图元的缩略效果
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void AbbreviativePaint(Graphics g)
        {

        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="s">移动的相对位置</param>
        public override void Move(Size s)
        {
            this.location += s;
            textRectangle.Location += s;
            textLocation += s;
            AdjustLine();
        }
    }
}
