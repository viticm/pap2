using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;

namespace GraphElementLibrary
{
    [Serializable]
    public abstract class GraphElement
    {
        protected Point location; // 图元位置
        protected string name = ""; // 图元的名称
        protected int id = 0; // 图元的id  
        protected Size elementSize; // 图元大小属性
        protected Size minSize; // 图元的最小大小

        [NonSerialized]
        protected Rectangle invalidRectangle; // 图元的刷新区域      

        [NonSerialized]
        protected object tag; // 附属数据              

        protected string remark = ""; // 图元的注释
        protected string text = ""; // 图元的文本

        [NonSerialized]
        protected string displayText = ""; // 图元的显示文本

        [NonSerialized]
        protected string tooltipText = ""; // 图元的注释文本

        protected bool selected = false; // 是否已被选中
        protected bool activated = false; // 是否已被激活
        protected bool enable = false; // 是否有效
        protected bool showText = false; // 是否显示文本
        protected bool showRemark = false; // 是否显示注释
        protected bool moving = false; // 图元是否正在移动
        protected bool showID = false; // 是否显示图元的id
        protected bool visible = true; // 是否显示图元        

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元位置</param>
        /// <param name="id">图元的id</param>
        protected GraphElement(Point location)
        {
            this.location = location;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        [Browsable(false)]
        public virtual string TypeString
        {
            get
            {
                return "";
            }
        }

        /// <summary>
        /// 附属数据
        /// </summary>
        [Browsable(false)]
        public virtual object Tag
        {
            set
            {
                tag = value;
            }

            get
            {
                return tag;
            }
        }

        /// <summary>
        /// 图元是否可见
        /// </summary>
        [Browsable(false)]
        public virtual bool Visible
        {
            set
            {
                visible = value;
            }

            get
            {
                return visible;
            }
        }

        /// <summary>
        /// 图元的提示文本
        /// </summary>
        [Browsable(false)]
        public virtual string TooltipText
        {
            set
            {
                tooltipText = value;
            }

            get
            {
                return tooltipText;
            }
        }

        /// <summary>
        /// 是否显示ID属性
        /// </summary>
        [Browsable(false)]
        public virtual bool ShowID
        {
            set
            {
                showID = value;
            }

            get
            {
                return showID;
            }
        }

        /// <summary>
        /// 图元是否正在移动属性
        /// </summary>
        [Browsable(false)]
        public virtual bool Moving
        {
            set
            {
                moving = value;
            }

            get
            {
                return moving;
            }
        }

        /// <summary>
        /// 图元的刷新区域属性
        /// </summary>
        [Browsable(false)]
        public virtual Rectangle InvalidRectangle
        {
            get
            {
                return invalidRectangle;
            }
        }

        /// <summary>
        /// 图元的显示文本属性
        /// </summary>
        [Browsable(false)]
        public virtual string DisplayText
        {
            set
            {
                displayText = value;
            }

            get
            {
                return displayText;
            }
        }

        /// <summary>
        /// 图元的注释属性
        /// </summary>
        [Browsable(false)]
        public virtual string Remark
        {
            set
            {
                remark = value;
            }

            get
            {
                return remark;
            }
        }

        /// <summary>
        /// 是否显示注释属性
        /// </summary>
        [Browsable(false)]
        public virtual bool ShowRemark
        {
            set
            {
                showRemark = value;
            }

            get
            {
                return showRemark;
            }
        }

        

        /// <summary>
        /// 图元是否有效属性
        /// </summary>
        [Browsable(false)]
        public virtual bool Enable
        {
            set
            {
                enable = value;
            }

            get
            {
                return enable;
            }
        }

        /// <summary>
        /// 图元的名称属性
        /// </summary>
        [Browsable(false)]
        public virtual string Name
        {
            set
            {
                name = value;
            }

            get
            {
                return name;
            }
        }

        /// <summary>
        /// 图元的文本属性
        /// </summary>
        [Browsable(false)]
        public virtual string Text
        {
            set
            {
                text = value;
            }

            get
            {
                return text;
            }
        }

        /// <summary>
        /// 是否显示文本属性
        /// </summary>
        [Browsable(false)]
        public virtual bool ShowText
        {
            set
            {
                showText = value;
            }

            get
            {
                return showText;
            }
        }

        /// <summary>
        /// 图元的最小大小属性
        /// </summary>
        [Browsable(false)]
        public virtual Size MinSize
        {
            set
            {
                minSize = value;
            }

            get
            {
                return minSize;
            }
        }        

        /// <summary>
        /// 图元位置属性
        /// </summary>
        [Browsable(false)]
        public virtual Point Location
        {
            set
            {
                location = value;
            }

            get
            {
                return location;
            }
        }

        /// <summary>
        /// 图元id属性
        /// </summary>
        [Browsable(false)]
        public virtual int ID
        {
            set
            {
                id = value;
            }

            get
            {
                return id;
            }
        }        

        /// <summary>
        /// 图元是否选中属性
        /// </summary>
        [Browsable(false)]
        public virtual bool Selected
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
        public virtual bool Activated
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
        /// 图元大小属性
        /// </summary>
        [Browsable(false)]
        public virtual Size ElementSize
        {
            set
            {
                elementSize = value;
            }

            get
            {
                return elementSize;
            }
        }

        /// <summary>
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public abstract bool IsInRegion(Point p);

        /// <summary>
        /// 判断图元是否在某个矩形内
        /// </summary>
        /// <param name="r">当前矩形</param>
        /// <returns>图元是否在当前矩形内</returns>
        public abstract bool IsInRegion(Rectangle r);

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public abstract void Paint(Graphics g);

        /// <summary>
        /// 绘制图元的缩略效果
        /// </summary>
        /// <param name="g">图形对象</param>
        public abstract void AbbreviativePaint(Graphics g);

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="s">移动的相对位置</param>
        public abstract void Move(Size s);

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public abstract void Refresh();

        /// <summary>
        /// 释放图元的绘图资源
        /// </summary>
        public abstract void Release();
    }
}
