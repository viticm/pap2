using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using FlowChartEditor.ButtonGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class SlotGraphElement:GraphElement
    {
        private SlotContainer slotContainer; // 插槽所属容器
        private ConnectorGraphElement bindingConnector; // 绑定的连接线控制点
        private ButtonGraphElement.BaseButton bindingConnectButton; // 绑定的连接按钮
        private Color bindedColor = Color.Lime; // 绑定后显示的颜色
        private bool binded = false; // 插槽是否已经绑定连接线控制点
        private bool isInSlot = false; // 是否是入口插槽
        private bool isOutSlot = false; // 是否是出口插槽
        private bool canDelete = true; // 是否可以删除
        private int priority; // 插槽的优先级

        [NonSerialized]
        Rectangle textRectangle = Rectangle.Empty;
        [NonSerialized]
        Rectangle activateRectangle = Rectangle.Empty;

        [NonSerialized]
        protected GraphSetting graphSetting; // 绘图参数配置对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="slotContainner">插槽所属容器</param>
        /// <param name="location">插槽的位置</param>
        /// <param name="size">插槽的大小</param>
        /// <param name="id">插槽的id</param>
        public SlotGraphElement(SlotContainer slotContainer, Point location, Size size)
            : base(location)
        {
            this.slotContainer = slotContainer;
            this.elementSize = size;
        }

        /// <summary>
        /// 插槽的优先级属性
        /// </summary>
        [Browsable(false)]
        public int Priority
        {
            set
            {
                priority = value;
            }

            get
            {
                return priority;
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
            }

            get
            {
                return location;
            }
        }

        /// <summary>
        /// 是否可以删除属性
        /// </summary>
        [Browsable(false)]
        public bool CanDelete
        {
            set
            {
                canDelete = value;
            }

            get
            {
                return canDelete;
            }
        }

        /// <summary>
        /// 绑定的连接按钮属性
        /// </summary>
        [Browsable(false)]
        public ButtonGraphElement.BaseButton BindingConnectButton
        {
            set
            {
                bindingConnectButton = value;
            }

            get
            {
                return bindingConnectButton;
            }
        }

        /// <summary>
        /// 是否是出口插槽属性
        /// </summary>
        [Browsable(false)]
        public bool IsInSlot
        {
            set
            {
                isInSlot = value;
            }

            get
            {
                return isInSlot;
            }
        }

        /// <summary>
        /// 是否是入口插槽属性
        /// </summary>
        [Browsable(false)]
        public bool IsOutSlot
        {
            set
            {
                isOutSlot = value;
            }

            get
            {
                return isOutSlot;
            }
        }

        /// <summary>
        /// 绑定后显示的颜色属性
        /// </summary>
        [Browsable(true), DisplayName("绑定状态颜色"), Description("绑定后显示的颜色"), Category("绘图属性")]
        public Color BindedColor
        {
            set
            {
                bindedColor = value;
            }

            get
            {
                return bindedColor;
            }
        }

        /// <summary>
        /// 插槽所属容器属性
        /// </summary>
        [Browsable(false)]
        public SlotContainer SlotContainer
        {
            get
            {
                return slotContainer;
            }
        }

        /// <summary>
        /// 插槽大小属性
        /// </summary>
        [Browsable(false)]
        public override Size ElementSize
        {
            get
            {
                return elementSize;
            }
        }

        /// <summary>
        /// 插槽是否已经绑定连接线控制点属性
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
        /// 绑定的连接线控制点属性
        /// </summary>
        [Browsable(false)]
        public ConnectorGraphElement BindingConnector
        {
            get
            {
                return bindingConnector;
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
                if(slotContainer.Selected || slotContainer.Resizing || selected || activated) // 插槽容器或自己被选中和激活时才显示
                {
                    if (activated) // 当前图元被激活
                    {
                        g.FillRectangle(graphSetting.BackgroundBrush, activateRectangle);
                        g.DrawRectangle(graphSetting.GraphElementActivatedPen, activateRectangle);
                    }
                    else if(selected) // 当前图元被选中
                    {
                        g.FillRectangle(graphSetting.BackgroundBrush, activateRectangle);
                        g.DrawRectangle(graphSetting.GraphElementSelectedPen, activateRectangle);
                    }
                    else if(binded) // 当前图元被绑定
                    {
                        // 由连接线控制点来表现
                    }
                    else // 当前图元没有被选中，激活和绑定
                    {
                        g.FillRectangle(graphSetting.BackgroundBrush, textRectangle);
                        g.DrawRectangle(graphSetting.GraphElementNormalPen, textRectangle);
                    }

                    // 绘制绑定的连接线控制点
                    if (binded)
                    {
                        bindingConnector.Paint(g);
                    }
                }

                // 绘制连接按钮
                if (isOutSlot && !binded)
                {
                    if (slotContainer.Resizing && bindingConnectButton != null)
                    {
                        bindingConnectButton.Paint(g);
                    }                
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
            if(!selected) // 插槽被选中时不能移动
            {
                this.location += s;
                textRectangle.Location += s;
                activateRectangle.Location += s;

                if(bindingConnector != null) // 移动连接线控制点
                {
                    bindingConnector.Move(s);
                }

                if(bindingConnectButton != null) // 移动连接按钮
                {
                    bindingConnectButton.Move(s);
                }
            }
        }

        /// <summary>
        /// 绑定连接线控制点
        /// </summary>
        /// <param name="connector">连接线控制点</param>
        public void Bind(ConnectorGraphElement connector)
        {
            if(binded && connector != bindingConnector) // 已经绑定了连接线控制点
            {
                MessageBox.Show("该插槽已经绑定了连接线控制点！", "连接绑定检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else if(isInSlot && connector.IsTailPoint)
            {
                MessageBox.Show("入口插槽不能绑定出端连接线控制点！", "连接绑定检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else if(isOutSlot && connector.IsHeadPoint)
            {
                MessageBox.Show("出口插槽不能绑定了入端连接线控制点！", "连接绑定检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                bindingConnector = connector;
                bindingConnector.Binded = true;
                bindingConnector.Location = location;
                Binded = true;

                // 处理逻辑连接信息
                if (connector.IsHeadPoint) // 连接线控制点是头控制点
                {
                    connector.Line.OutSlotContainer = slotContainer;
                }
                else // 连接线控制点是尾控制点
                {
                    connector.Line.InSlotContainer = slotContainer;
                }
            }
        }

        /// <summary>
        /// 取消绑定连接线控制点
        /// </summary>
        public void UnBind()
        {
            // 处理逻辑连接信息
            if (bindingConnector.IsHeadPoint) // 连接线控制点是头控制点
            {
                bindingConnector.Line.OutSlotContainer = null;
            }
            else // 连接线控制点是尾控制点
            {
                bindingConnector.Line.InSlotContainer = null;
            }

            bindingConnector.Binded = false;
            bindingConnector = null;
            Binded = false;            
        }
    }
}
