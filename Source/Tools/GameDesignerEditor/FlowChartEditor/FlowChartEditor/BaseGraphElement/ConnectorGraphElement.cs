using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.ComponentModel;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor
{
    [Serializable]
    public class ConnectorGraphElement:GraphElement
    {
        private bool isHeadPoint = false; // 是否是连接线的头结点
        private bool isTailPoint = false; // 是否是连接线的尾结点
        private bool binded = false; // 是否已经绑定插槽
        private ConnectorContainer line; // 连接线控制点所属的连接线

        [NonSerialized]
        Rectangle textRectangle = Rectangle.Empty;
        [NonSerialized]
        Rectangle activateRectangle = Rectangle.Empty;

        [NonSerialized]
        protected GraphSetting graphSetting; // 绘图参数配置对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="line">连接线控制点所属的连接线</param>
        /// <param name="location">连接点的位置</param>
        /// <param name="size">连接线的大小</param>
        public ConnectorGraphElement(ConnectorContainer line, Point location, Size size)
            : base(location)
        {
            this.line = line;
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
                activateRectangle.Location = value + new Size(-elementSize.Width, -elementSize.Height);

                if(line.ShowText) // 调整连接线的文本位置
                {
                    line.AdjustText(); 
                }
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
                invalidRectangle = new Rectangle(location + new Size(-elementSize.Width, -elementSize.Height), 
                                                 elementSize + new Size(elementSize.Width * 2, elementSize.Height * 2));

                return invalidRectangle;
            }
        }

        /// <summary>
        /// 是否是连接线头结点属性
        /// </summary>
        [Browsable(false)]
        public bool IsHeadPoint
        {
            set
            {
                isHeadPoint = value;
            }

            get
            {
                return isHeadPoint;
            }
        }

        /// <summary>
        /// 是否是连接线尾结点属性
        /// </summary>
        [Browsable(false)]
        public bool IsTailPoint
        {
            set
            {
                isTailPoint = value;
            }

            get
            {
                return isTailPoint;
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
        /// 是否已经绑定插槽属性
        /// </summary>
        [Browsable(false)]
        public bool Binded
        {
            set
            {
                binded = value;              
            }

            get
            {
                return binded;
            }
        }

        /// <summary>
        /// 连接线控制点所属的连接线属性
        /// </summary>
        [Browsable(false)]
        public ConnectorContainer Line
        {
            get
            {
                return line;
            }
        }

        /// <summary>
        /// 获取绑定的插槽
        /// </summary>
        /// <returns>绑定的插槽</returns>
        public SlotGraphElement GetBindingSlot()
        {
            SlotGraphElement slotGraphElement = null;
            SlotContainer slotContainer;

            if (isHeadPoint)
            {
                slotContainer = line.OutSlotContainer;

                if (slotContainer != null)
                {
                    foreach(SlotGraphElement slot in slotContainer.GetInSlotList())
                    {
                        if (slot.BindingConnector == this)
                        {
                            slotGraphElement = slot;
                            break;
                        }
                    }
                }
            }
            else if (isTailPoint)
            {
                slotContainer = line.InSlotContainer;

                if (slotContainer != null)
                {
                    foreach(SlotGraphElement slot in slotContainer.GetOutSlotList())
                    {
                        if (slot.BindingConnector == this)
                        {
                            slotGraphElement = slot;
                            break;
                        }
                    }
                }
            }

            return slotGraphElement;
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
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            bool result = activateRectangle.Contains(p);

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
                    g.FillRectangle(graphSetting.GraphElementActivatedBrush, textRectangle);
                }
                else if (selected) // 当前图元被选中
                {
                    g.FillRectangle(graphSetting.GraphElementSelectedBrush, textRectangle);
                }
                else if (binded) // 当前图元被绑定
                {
                    g.FillRectangle(graphSetting.GraphElementBindedBrush, textRectangle);
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
            this.location += s;
            textRectangle.Location += s;
            activateRectangle.Location += s;

            if (line.ShowText) // 调整连接线的文本位置
            {
                line.AdjustText();
            }
        }
    }
}
