using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using GraphElementLibrary;

namespace FlowChartEditor
{
    [Serializable]
    public class ButtonGraphElement:GraphElement
    {
        [NonSerialized]
        protected Rectangle textRectangle = Rectangle.Empty;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        /// <param name="buttonType">按钮的类型</param>
        public ButtonGraphElement(Point location, Size elementSize):base(location)
        {
            this.elementSize = elementSize;
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
            }

            get
            {
                return location;
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
                invalidRectangle = new Rectangle(location, elementSize);
                return invalidRectangle;
            }
        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {
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
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            bool result = false;

            result = textRectangle.Contains(p);

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

            result = r.Contains(textRectangle);

            return result;
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        public virtual void RefreshImages()
        {
            if (this is AddButton) // 当前类型为添加按钮
            {
                if (normalImage == null)
                {
                    normalImage = Helper.GetHelper().AddButtonNormalImage;
                }

                if (activatedImage == null)
                {
                    activatedImage = Helper.GetHelper().AddButtonActivatedImage;
                }
            }
            else if (this is ConnectButton) // 当前类型为连接按钮
            {
                if (normalImage == null)
                {
                    normalImage = Helper.GetHelper().ConnectButtonNormalImage;
                }

                if (activatedImage == null)
                {
                    activatedImage = Helper.GetHelper().ConnectButtonActivatedImage;
                }               
            }            
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {
            Image image;

            if(selected) // 图元被选中
            {
                image = activatedImage;
                g.DrawImage(image, textRectangle);
            }
            else if(activated) // 图元被激活
            {
                image = activatedImage;
                g.DrawImage(image, textRectangle);
            }
            else // 图元没有被选中和激活
            {
                image = normalImage;
                g.DrawImage(image, textRectangle);
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
        }
    }
}
