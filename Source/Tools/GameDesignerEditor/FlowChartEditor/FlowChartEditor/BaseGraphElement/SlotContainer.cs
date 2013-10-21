using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.ComponentModel;

using FlowChartEditor.ButtonGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class SlotContainer:GraphElement
    {
        protected List<SlotGraphElement> inSlotList = new List<SlotGraphElement>(); // 入口插槽链表
        protected List<SlotGraphElement> outSlotList = new List<SlotGraphElement>(); // 出口插槽链表
        protected List<SlotGraphElement> slotList = new List<SlotGraphElement>(); // 插槽链表
        protected List<ButtonGraphElement.BaseButton> buttonList = new List<ButtonGraphElement.BaseButton>(); // 按钮链表
        protected List<ResizeControler> resizeControlerList = new List<ResizeControler>(); // 缩放控制点链表
        protected AddButton addButton; // 添加按钮
        protected int inSlotCount; // 入口插槽的数量
        protected int outSlotCount; // 出口插槽的数量
        protected bool resizing = false; // 是否正在缩放状态
        protected RemarkGraphElement remarkNode; // 插槽容器对应的注释结点        
        protected Point textLocation = Point.Empty; // 绘制文本的位置   
        protected bool requireEvent = false; // 是否需要入口事件
        protected bool isEntrance = false; // 是否是入口结点

        [NonSerialized]
        protected Image normalImage; // 普通状态下的图片
        [NonSerialized]
        protected Image selectedImage; // 被选中状态下的图片
        [NonSerialized]
        protected Image activatedImage; // 被激活状态下的图片
        [NonSerialized]
        protected Image shadowImage; // 阴影图片
        [NonSerialized]
        protected Image abbreviateImage; // 缩略的图片

        [NonSerialized]
        protected Rectangle textRectangle = Rectangle.Empty; // 图元当前区域
        [NonSerialized]
        protected Rectangle shadowRectangle = Rectangle.Empty; // 图元阴影区域

        [NonSerialized]
        protected GraphSetting graphSetting; // 绘图参数配置对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public SlotContainer(Point location, Size elementSize)
                            : base(location)
        {
            this.elementSize = elementSize;
            this.minSize = elementSize;
        }

        /// <summary>
        /// 是否是入口结点
        /// </summary>
        [Browsable(false)]
        public virtual bool IsEntrance
        {
            set
            {
                isEntrance = value;
            }

            get
            {
                return isEntrance;
            }
        }

        /// <summary>
        /// 表现阴影效果的图片属性
        /// </summary>
        [Browsable(false)]
        public virtual Image ShadowImage
        {
            set
            {
                shadowImage = value;
            }
        }

        /// <summary>
        /// 普通状态下的图片属性
        /// </summary>
        [Browsable(false)]
        public virtual Image NormalImage
        {
            set
            {
                normalImage = value;
            }
        }

        /// <summary>
        /// 被选中状态下的图片属性
        /// </summary>
        [Browsable(false)]
        public virtual Image SelectedImage
        {
            set
            {
                selectedImage = value;
            }
        }

        /// <summary>
        /// 被激活状态下的图片属性
        /// </summary>
        [Browsable(false)]
        public virtual Image ActivatedImage
        {
            set
            {
                activatedImage = value;
            }
        }

        /// <summary>
        /// 图元的区域矩形
        /// </summary>
        [Browsable(false)]
        public virtual Rectangle TextRectangle
        {
            get
            {
                return textRectangle;
            }
        }

        /// <summary>
        /// 是否需要入口事件
        /// </summary>
        [Browsable(false)]
        public virtual bool RequireEvent
        {
            set
            {
                requireEvent = value;
            }

            get
            {
                return requireEvent;
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
                return invalidRectangle;
            }
        }

        /// <summary>
        /// 图元的显示文本属性
        /// </summary>
        [Browsable(false)]
        public override string Text
        {            
            set
            {
                displayText = value;
                text = value;
            }

            get
            {
                return text;
            }
        }

        /// <summary>
        /// 图元的注释属性
        /// </summary>
        [Browsable(false)]
        public override string Remark
        {
            get
            {
                return remark;
            }

            set
            {
                remark = value;

                if (remarkNode != null) // 改变注释结点的文本
                {
                    remarkNode.Text = value;
                }
            }
        }

        /// <summary>
        /// 插槽容器对应的注释结点属性
        /// </summary>
        [Browsable(false)]
        public virtual RemarkGraphElement RemarkNode
        {
            set
            {
                remarkNode = value;
            }

            get
            {
                return remarkNode;
            }
        }

        /// <summary>
        /// 是否正在缩放状态属性
        /// </summary>
        [Browsable(false)]
        public bool Resizing
        {
            set
            {
                resizing = value;
            }

            get
            {
                return resizing;
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
                shadowRectangle.Location = value + new Size(4, 4);
                invalidRectangle.Location = value + new Size(-10, -10);
            }

            get
            {
                return location;
            }
        }

        /// <summary>
        /// 图元大小属性
        /// </summary>
        [Browsable(false)]
        public override Size ElementSize
        {
            set
            {
                elementSize = value;
                textRectangle.Size = value;
                shadowRectangle.Size = value;
                invalidRectangle.Size = value + new Size(20, 45);
                AdjustText(); // 调整显示文本
                AdjustInSlotList(inSlotCount); // 调整入口插槽链表
                AdjustOutSlotList(outSlotCount); // 调整出口插槽链表
                AdjustResizeControlerList(); // 调整缩放控制点链表
                AdjustButtonList(); // 调整按钮链表                
            }

            get
            {
                return elementSize;
            }
        }

        /// <summary>
        /// 入口插糟数量属性
        /// </summary>
        [Browsable(false)]
        public virtual int InSlotCount
        {
            set
            {
                AdjustInSlotList(value);
                inSlotCount = value;                
            }

            get
            {
                return inSlotCount;
            }
        }

        /// <summary>
        /// 初始化图元
        /// </summary>
        public void Init()
        {
            InitSlot(elementSize); // 初始化结点的插槽
            InitResizeControler(elementSize); // 初始化结点的缩放控制点
            InitButton(); // 初始化按钮

            // 刷新数据
            Refresh();
        }

        /// <summary>
        /// 初始化按钮
        /// </summary>
        protected virtual void InitButton()
        {
            Helper helper = Helper.GetHelper();
            int x = location.X + elementSize.Width - 12;
            int y = location.Y + elementSize.Height + 22;
            AddButton button = new AddButton(this, new Point(x, y), new Size(12, 12));
            button.Name = "添加插槽按钮";
            this.addButton = button;
            buttonList.Add(button);
        }

        /// <summary>
        /// 调整按钮链表
        /// </summary>
        protected virtual void AdjustButtonList()
        {
            if (addButton != null)
            {
                int x = location.X + elementSize.Width - 12;
                int y = location.Y + elementSize.Height + 22;
                addButton.Location = new Point(x, y);
            }            
        }

        /// <summary>
        /// 调整缩放控制点链表
        /// </summary>
        protected virtual void AdjustResizeControlerList()
        {
            int index = 0;

            // 调整左上角的缩放控制点的坐标
            int newX = location.X - 10;
            int newY = location.Y - 10;
            Point newPoint = new Point(newX, newY);
            ResizeControler resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化左边的缩放控制点
            newX = location.X - 10;
            newY = location.Y + (int)(elementSize.Height / 2);
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化左下角的缩放控制点
            newX = location.X - 10;
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化下边的缩放控制点
            newX = location.X + (int)(elementSize.Width / 2);
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化右下角的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化右边的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y + (int)(elementSize.Height / 2);
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];
            resizeControler.Location = newPoint;
            index++;

            // 初始化右上角的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y - 10;
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];            
            resizeControler.Location = newPoint;            
            index++;

            // 初始化上边的缩放控制点
            newX = location.X + (int)(elementSize.Width / 2);
            newY = location.Y - 10;
            newPoint = new Point(newX, newY);
            resizeControler = resizeControlerList[index];            
            resizeControler.Location = newPoint;            
        }

        /// <summary>
        /// 出口插糟数量属性
        /// </summary>
        [Browsable(false)]
        public virtual int OutSlotCount
        {
            set
            {
                AdjustOutSlotList(value);
                outSlotCount = value;                
            }

            get
            {
                return outSlotCount;
            }
        }

        /// <summary>
        /// 初始化结点的缩放控制点
        /// </summary>
        /// <param name="size">当前图元的大小</param>
        protected virtual void InitResizeControler(Size size)
        {
            // 初始化左上角的缩放控制点
            int newX = location.X - 10;
            int newY = location.Y - 10;
            Point newPoint = new Point(newX, newY);
            ResizeControler newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Up_Left;
            resizeControlerList.Add(newControler);

            // 初始化左边的缩放控制点
            newX = location.X - 10;
            newY = location.Y + (int)(elementSize.Height / 2);
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Left;
            resizeControlerList.Add(newControler);

            // 初始化左下角的缩放控制点
            newX = location.X - 10;
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Down_Left;
            resizeControlerList.Add(newControler); 

            // 初始化下边的缩放控制点
            newX = location.X + (int)(elementSize.Width / 2);
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Down;
            resizeControlerList.Add(newControler); 

            // 初始化右下角的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y + elementSize.Height + 10;
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Down_Right;
            resizeControlerList.Add(newControler); 

            // 初始化右边的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y + (int)(elementSize.Height / 2);
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Right;
            resizeControlerList.Add(newControler); 

            // 初始化右上角的缩放控制点
            newX = location.X + elementSize.Width + 10;
            newY = location.Y - 10;
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Up_Right;
            resizeControlerList.Add(newControler); 

            // 初始化上边的缩放控制点
            newX = location.X + (int)(elementSize.Width / 2);
            newY = location.Y - 10;
            newPoint = new Point(newX, newY);
            newControler = new ResizeControler(this, newPoint, new Size(4, 4));
            newControler.CurrentDirection = Direction.Up;
            resizeControlerList.Add(newControler); 
        }

        /// <summary>
        /// 初始化结点的插槽
        /// </summary>
        /// <param name="Size">当前图元的大小</param>
        protected virtual void InitSlot(Size size)
        {
            int x1 = location.X + (int)(size.Width / 2);
            int y1 = location.Y;
            int x2 = x1;
            int y2 = location.Y + size.Height;

            SlotGraphElement slot1 = new SlotGraphElement(this, new Point(x1, y1), new Size(6, 6));
            SlotGraphElement slot2 = new SlotGraphElement(this, new Point(x2, y2), new Size(6, 6));

            slot1.Name = "连接插槽";        
            slot1.IsInSlot = true;

            slot2.Name = "连接插槽";
            slot2.IsOutSlot = true;

            ConnectButton button = new ConnectButton(slot2, new Point(x2 - 6, y2 + 25), new Size(12, 8));
            button.Name = "连接按钮";
            slot2.BindingConnectButton = button;      

            inSlotList.Add(slot1);
            outSlotList.Add(slot2);
            buttonList.Add(button);
            slotList.Add(slot1);
            slotList.Add(slot2);

            // 重设插槽优先级
            ResetSlotProperty(inSlotList);
            ResetSlotProperty(outSlotList);
        }

        /// <summary>
        /// 重设插槽优先级
        /// </summary>
        protected virtual void ResetSlotProperty(List<SlotGraphElement> slotGraphElementList)
        {
            int property = 0;

            foreach (SlotGraphElement slot in slotGraphElementList)
            {
                slot.Priority = property;
                property++;
            }
        }

        /// <summary>
        /// 调整入口插槽链表
        /// </summary>
        /// <param name="n">插槽数量</param>
        protected virtual void AdjustInSlotList(int n)
        {
            if (n == inSlotList.Count) // 不需要调整插槽
            {
                int newX;
                Point newPoint;

                // 调整插糟的坐标
                for (int i = 0; i < inSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                    newPoint = new Point(newX, location.Y);
                    inSlotList[i].Location = newPoint;

                    if (inSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        inSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else if (n < inSlotList.Count) // 需要减少插槽数量
            {
                int deleteCount = inSlotList.Count - n;
                int newX;
                Point newPoint;
                List<SlotGraphElement> deleteSlotList = new List<SlotGraphElement>();

                for (int i = 0; i < deleteCount; i++)
                {
                    deleteSlotList.Add(inSlotList[inSlotList.Count - 1 - i]);
                }

                foreach (SlotGraphElement slot in deleteSlotList)
                {
                    if (slot.Binded)
                    {
                        slot.UnBind();
                    }

                    inSlotList.Remove(slot);
                    slotList.Remove(slot);
                }

                // 调整插糟的坐标
                for (int i = 0; i < inSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                    newPoint = new Point(newX, location.Y);
                    inSlotList[i].Location = newPoint;

                    if (inSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        inSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else // 需要增加插槽数量
            {
                int oldCount = inSlotList.Count;
                SlotGraphElement newSlot;
                Point newPoint;
                int newX;

                // 添加插槽并调整插槽的坐标
                for (int i = 0; i < n; i++)
                {
                    if (i < oldCount)
                    {
                        newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                        newPoint = new Point(newX, location.Y);
                        inSlotList[i].Location = newPoint;

                        if (inSlotList[i].Binded) // 调整绑定的连接线控制点
                        {
                            inSlotList[i].BindingConnector.Location = newPoint;
                        }
                    }
                    else
                    {
                        newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                        newPoint = new Point(newX, location.Y);
                        newSlot = new SlotGraphElement(this, newPoint, new Size(6, 6));
                        newSlot.Name = "连接插槽";
                        newSlot.IsInSlot = true;
                        newSlot.CanDelete = true;
                        newSlot.Refresh();

                        inSlotList.Add(newSlot);
                        slotList.Add(newSlot);
                    }
                }
            }

            ResetSlotProperty(inSlotList);
        }

        /// <summary>
        /// 调整出口插槽链表
        /// </summary>
        /// <param name="n">插槽数量</param>
        protected virtual void AdjustOutSlotList(int n)
        {
            if (n == outSlotList.Count) // 不需要调整插槽
            {
                int newX;
                Point newPoint;

                // 调整插糟的坐标
                for (int i = 0; i < outSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                    newPoint = new Point(newX, location.Y + elementSize.Height);
                    outSlotList[i].Location = newPoint;
                    outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, location.Y + elementSize.Height + 25);

                    if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        outSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else if (n < outSlotList.Count) // 需要减少插槽数量
            {
                int deleteCount = outSlotList.Count - n;
                int newX;
                Point newPoint;
                List<SlotGraphElement> deleteSlotList = new List<SlotGraphElement>();

                for (int i = 0; i < deleteCount; i++)
                {
                    deleteSlotList.Add(outSlotList[outSlotList.Count - 1 - i]);
                }

                foreach (SlotGraphElement slot in deleteSlotList)
                {
                    if (slot.Binded)
                    {
                        slot.UnBind();
                    }

                    buttonList.Remove(slot.BindingConnectButton);
                    slot.BindingConnectButton = null;
                    outSlotList.Remove(slot);
                    slotList.Remove(slot);
                }

                // 调整插糟的坐标
                for (int i = 0; i < outSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                    newPoint = new Point(newX, location.Y + elementSize.Height);
                    outSlotList[i].Location = newPoint;
                    outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, location.Y + elementSize.Height + 25);

                    if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        outSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else // 需要增加插槽数量
            {
                Helper helper = Helper.GetHelper();
                int oldCount = outSlotList.Count;
                SlotGraphElement newSlot;
                Point newPoint;
                int newX;

                // 添加插槽并调整插槽的坐标
                for (int i = 0; i < n; i++)
                {
                    if (i < oldCount)
                    {
                        newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                        newPoint = new Point(newX, location.Y + elementSize.Height);
                        outSlotList[i].Location = newPoint;
                        outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, location.Y + elementSize.Height + 25);

                        if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                        {
                            outSlotList[i].BindingConnector.Location = newPoint;
                        }
                    }
                    else
                    {
                        newX = (int)(location.X + elementSize.Width * (i + 1) / (n + 1));
                        newPoint = new Point(newX, location.Y + elementSize.Height);
                        newSlot = new SlotGraphElement(this, newPoint, new Size(6, 6));
                        newSlot.Name = "连接插槽";
                        newSlot.IsOutSlot = true;
                        newSlot.CanDelete = true;
                        newSlot.Refresh();

                        ConnectButton button = new ConnectButton(newSlot, new Point(newX - 6, location.Y + elementSize.Height + 25),
                            new Size(12, 8));
                        button.Name = "连接按钮";
                        button.Refresh();
                        newSlot.BindingConnectButton = button;

                        outSlotList.Add(newSlot);
                        buttonList.Add(button);
                        slotList.Add(newSlot);
                    }
                }
            }

            ResetSlotProperty(outSlotList);
        }

        /// <summary>
        /// 获取与连接线绑定的入口插槽
        /// </summary>
        /// <param name="line">连接线</param>
        /// <returns>入口插槽</returns>
        public SlotGraphElement GetBindedInSlot(ConnectorContainer line)
        {
            SlotGraphElement slotGraphElement = null;

            foreach (SlotGraphElement slot in inSlotList)
            {
                if (slot.Binded && slot.BindingConnector.Line == line)
                {
                    slotGraphElement = slot;
                    break;
                }
            }

            return slotGraphElement;
        }

        /// <summary>
        /// 获取与连接线绑定的入口插槽的索引
        /// </summary>
        /// <param name="line">连接线</param>
        /// <returns>索引</returns>
        public int GetBindedInSlotIndex(ConnectorContainer line)
        {
            int index = -1;
            SlotGraphElement slot = GetBindedInSlot(line);

            if (slot != null)
            {
                index = slot.Priority;
            }

            return index;
        }

        /// <summary>
        /// 获取与连接线绑定的出口插槽
        /// </summary>
        /// <param name="line">连接线</param>
        /// <returns>出口插槽</returns>
        public SlotGraphElement GetBindedOutSlot(ConnectorContainer line)
        {
            SlotGraphElement slotGraphElement = null;

            foreach (SlotGraphElement slot in outSlotList)
            {
                if (slot.Binded && slot.BindingConnector.Line == line)
                {
                    slotGraphElement = slot;
                    break;
                }
            }

            return slotGraphElement;
        }

        /// <summary>
        /// 获取与连接线绑定的出口插槽的索引
        /// </summary>
        /// <param name="line">连接线</param>
        /// <returns>索引</returns>
        public int GetBindedOutSlotIndex(ConnectorContainer line)
        {
            int index = -1;
            SlotGraphElement slot = GetBindedOutSlot(line);

            if (slot != null)
            {
                index = slot.Priority;
            }

            return index;
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
            bool result  = r.Contains(textRectangle);
            return result;            
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected virtual void RefreshImages()
        {
            
        }

        /// <summary>
        /// 自动调整图元的大小
        /// </summary>
        public virtual void AdjustElementSize()
        {
            Graphics g = Helper.GetHelper().GraphicsObject;                
            Rectangle textRectangle = new Rectangle(location, elementSize);                
            SizeF textSize = g.MeasureString(text, graphSetting.GraphElementTextFont);
            float textWidth = textSize.Width;
            float textHeight = textSize.Height;
            int newWidth = elementSize.Width;
            int newHeight = elementSize.Height;

            // measurestring不准，预留两个像素的偏移量
            if (textWidth < textRectangle.Width - 12)
            {
                if (textWidth < minSize.Width - 12)
                {
                    newWidth = minSize.Width;
                }
                else
                {
                    newWidth = (int)(textWidth + 12);
                }
            }
            else
            {   
                newWidth = (int)(textWidth + 12);                                         
            }

            if (textHeight < textRectangle.Height - 12)
            {
                if (textHeight < minSize.Height - 12)
                {
                    newHeight = minSize.Height;
                }
                else
                {
                    newHeight = (int)(textHeight + 12);
                }
            }
            else
            {
                newHeight = (int)(textHeight + 12);
            }

            // 调整图元大小
            if (newWidth != elementSize.Width || newHeight != elementSize.Height)
            {
                if(newWidth > minSize.Width * 2)
                {
                    newWidth = (int)(minSize.Width * 1.5); 
                }

                if (newHeight > minSize.Height * 2)
                {
                    newHeight = (int)(minSize.Height * 1.5);
                }

                ElementSize = new Size(newWidth, newHeight);                               
            }
        }

        /// <summary>
        /// 调整图元文本
        /// </summary>
        public virtual void AdjustText()
        {
            Graphics g = Helper.GetHelper().GraphicsObject;
            SizeF textSize = g.MeasureString(text, graphSetting.GraphElementTextFont);
            float textWidth = 0;
            float textHeight = textSize.Height;

            string[] data = text.Split(new char[] { '\n' });
            List<string> newData = new List<string>();
            displayText = "";
            float currentWidth;

            for (int i = 0; i < data.Length; i++)
            {
                string line = data[i];
                string newLine = line;
                textSize = g.MeasureString(line, graphSetting.GraphElementTextFont);
                currentWidth = textSize.Width;
                if (currentWidth > textRectangle.Width - 10) // 文本超出有效宽度
                {
                    int j = 2;

                    while (currentWidth > textRectangle.Width - 10)
                    {
                        j = 2;

                        while (currentWidth > textRectangle.Width - 10)
                        {
                            newLine = line.Substring(0, line.Length - j);
                            textSize = g.MeasureString(newLine, graphSetting.GraphElementTextFont);
                            currentWidth = textSize.Width;
                            j++;
                        }

                        j--;
                        newData.Add(newLine);
                        line = "    " + line.Substring(line.Length - j, j);
                        textSize = g.MeasureString(line, graphSetting.GraphElementTextFont);
                        currentWidth = textSize.Width;
                    }                    
                }

                newData.Add(line);
            }

            foreach(string s in newData)
            {
                displayText += s + "\n";
            }
            displayText = displayText.TrimEnd(new char[] { '\n' });
            textSize = g.MeasureString(displayText, graphSetting.GraphElementTextFont);
            textWidth = textSize.Width;
            textHeight = textSize.Height;

            if(textHeight + 10 > elementSize.Height) // 调整高度
            {
                elementSize.Height = (int)(textHeight + 10);
                textRectangle.Size = new Size(textRectangle.Width, elementSize.Height);
                shadowRectangle.Size = new Size(textRectangle.Width, elementSize.Height);
                invalidRectangle.Size = new Size(textRectangle.Width + 20, elementSize.Height + 45);
            }
            int textX = (int)(textRectangle.X + (textRectangle.Width - textWidth) / 2);
            int textY = (int)(textRectangle.Y + (textRectangle.Height - textHeight) / 2);
            textLocation.X = textX;
            textLocation.Y = textY;
        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {
            // 初始化绘图数据管理对象
            graphSetting = GraphSetting.GetGraphSetting();

            // 刷新表现文本
            displayText = text;

            // 刷新逻辑数据
            textRectangle.Location = location;
            textRectangle.Size = elementSize;
            shadowRectangle.Location = location + new Size(4, 4);
            shadowRectangle.Size = elementSize;
            invalidRectangle = new Rectangle(location + new Size(-10, -10), elementSize + new Size(20, 45));
            inSlotCount = inSlotList.Count;
            outSlotCount = outSlotList.Count;
           
            requireEvent = true;

            // 刷新图片
            RefreshImages();

            // 刷新插槽
            foreach(SlotGraphElement slot in slotList)
            {
                slot.Refresh();
            }

            // 重设插槽优先级
            ResetSlotProperty(inSlotList);
            ResetSlotProperty(outSlotList);

            // 刷新按钮
            foreach(ButtonGraphElement.BaseButton button in buttonList)
            {
                button.Refresh();
            }

            // 刷新缩放控制点
            foreach(ResizeControler resizeControler in resizeControlerList)
            {
                resizeControler.Refresh();
            }

            // 刷新注释结点
            if(remarkNode != null)
            {
                remarkNode.Refresh();
            }
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
                bool sizeChanged = (this.elementSize != graphSetting.AIActionNodeElementSize);

                if (moving) // 移动时只绘制矩形框
                {
                    // 绘制图片
                    if (sizeChanged)
                    {
                        g.DrawImage(normalImage, textRectangle);
                    }
                    else
                    {
                        g.DrawImage(normalImage, location);
                    }
                }
                else
                {
                    // 绘制图片阴影                    
                    if (sizeChanged)
                    {
                        g.DrawImage(shadowImage, shadowRectangle);
                    }
                    else
                    {
                        g.DrawImage(shadowImage, shadowRectangle.Location);
                    }                    

                    // 绘制图片                       
                    Image image;

                    if (activated) // 当前图元被激活
                    {
                        image = activatedImage;                        
                    }
                    else if (selected) // 当前图元被选中
                    {
                        image = selectedImage;
                    }
                    else // 当前图元没有被选中和激活
                    {
                        image = normalImage;
                    }                    

                    // 绘制图片                    
                    if (sizeChanged)
                    {
                        g.DrawImage(image, textRectangle);
                    }
                    else
                    {
                        g.DrawImage(image, location);
                    }
                    

                    // 绘制文本
                    if (showText)
                    {
                        if (selected)
                        {
                            g.DrawString(displayText, graphSetting.GraphElementTextBoldFont, graphSetting.GraphElementTextSelectedBrush, textLocation);
                        }
                        else
                        {
                            g.DrawString(displayText, graphSetting.GraphElementTextFont, graphSetting.GraphElementTextBrush, textLocation);
                        }
                        
                    }

                    // 绘制ID
                    if (showID)
                    {
                        g.DrawString(id.ToString(), graphSetting.GraphElementTextBoldFont, Brushes.Black, location.X - 50, location.Y);
                    }

                    // 绘制插糟
                    foreach (SlotGraphElement slotGraphElement in slotList)
                    {
                        slotGraphElement.Paint(g);
                    }

                    // 绘制缩放控制点
                    if (resizing)
                    {
                        foreach (ResizeControler resizeControler in resizeControlerList)
                        {
                            resizeControler.Paint(g);
                        }

                        // 绘制按钮
                        if (addButton != null)
                        {
                            addButton.Paint(g);
                        }
                    }

                    // 绘制注释结点
                    if (showRemark && remarkNode != null)
                    {
                        remarkNode.Paint(g);
                    }
                }
            }
        }

        /// <summary>
        /// 绘制图元的缩略效果
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void AbbreviativePaint(Graphics g)
        {
            g.DrawImage(abbreviateImage, textRectangle);
        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="s">移动的相对位置</param>
        public override void Move(Size s)
        {
            location += s;
            textRectangle.Location += s;
            shadowRectangle.Location += s;
            invalidRectangle.Location += s;

            // 移动文本位置
            textLocation += s;

            // 移动结点上的插槽
            foreach (SlotGraphElement slotGraphElement in slotList)
            {
                slotGraphElement.Move(s);
            }

            // 移动结点上的缩放控制点
            foreach(ResizeControler resizeControler in resizeControlerList)
            {
                resizeControler.Move(s);        
            }

            // 移动结点上的按钮
            if(addButton != null)
            {
                addButton.Move(s);
            }
         
            // 移动结点的注释图元
            if(remarkNode != null)
            {
                remarkNode.Move(s);
            }
        }

        /// <summary>
        /// 移动图元边框
        /// </summary>
        /// <param name="locationDistancement">顶点位移</param>
        /// <param name="elementSizeDistancement">大小位移</param>
        public virtual void MoveBorder(Size locationDistancement, Size elementSizeDistancement)
        {
            this.location += locationDistancement;
            textRectangle.Location += locationDistancement;
            shadowRectangle.Location += locationDistancement;
            invalidRectangle.Location += locationDistancement;
            this.ElementSize += elementSizeDistancement;
        }

        /// <summary>
        /// 获取插槽链表
        /// </summary>
        /// <returns>插槽链表</returns>
        public virtual List<SlotGraphElement> GetSlotList()
        {
            return slotList;
        }

        /// <summary>
        /// 获取入口插槽链表
        /// </summary>
        /// <returns>入口插槽链表</returns>
        public virtual List<SlotGraphElement> GetInSlotList()
        {
            return inSlotList;
        }

        /// <summary>
        /// 获取出口插槽链表
        /// </summary>
        /// <returns>出口插槽链表 </returns>
        public virtual List<SlotGraphElement> GetOutSlotList()
        {
            return outSlotList;
        }

        /// <summary>
        /// 获取指定区域内的缩放控制点
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>缩放控制点</returns>
        public virtual ResizeControler GetResizeControlerInRegion(Point p)
        {
            ResizeControler resizeControler = null;

            foreach (ResizeControler controler in resizeControlerList)
            {
                if (controler.IsInRegion(p))
                {
                    resizeControler = controler;
                    break;
                }
            }

            return resizeControler;
        }

        /// <summary>
        /// 获取缩放控制点链表
        /// </summary>
        /// <returns>缩放控制点链表</returns>
        public virtual List<ResizeControler> GetResizeControlerList()
        {
            return resizeControlerList;
        }

        /// <summary>
        /// 获取区域内的按钮
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>按钮</returns>
        public BaseButton GetButtonInRegion(Point p)
        {
            BaseButton button = null;

            foreach (BaseButton baseButton in buttonList)
            {
                if (baseButton.IsInRegion(p))
                {
                    button = baseButton;
                    break;
                }
            }

            return button;
        }

        /// <summary>
        /// 获取按钮链表
        /// </summary>
        /// <returns>按钮链表</returns>
        public virtual List<ButtonGraphElement.BaseButton> GetButtonList()
        {
            return buttonList;
        }

        /// <summary>
        /// 取消绑定连接线
        /// </summary>
        /// <param name="line">连接线</param>
        public virtual void UnBindLine(ConnectorContainer line)
        {
            SlotGraphElement deleteSlot = null;

            foreach(SlotGraphElement slot in slotList)
            {
                if(slot.Binded && ((slot.BindingConnector == line.HeadConnector) || slot.BindingConnector == line.TailConnector))
                {
                    slot.UnBind();
                    deleteSlot = slot;
                    break;
                }
            }

            if (deleteSlot != null && deleteSlot.CanDelete) // 插槽可以删除
            {
                if(deleteSlot.IsInSlot || outSlotCount > 1)
                {
                    RemoveSlot(deleteSlot);
                }                
            }
        }

        /// <summary>
        /// 增加入口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public virtual SlotGraphElement AddInSlot()
        {            
            InSlotCount = inSlotCount + 1;
            SlotGraphElement newSlot = inSlotList[inSlotCount - 1];            

            return newSlot;
        }

        /// <summary>
        /// 获取入口插槽
        /// </summary>
        /// <returns>入口插槽</returns>
        public virtual SlotGraphElement GetInSlot()
        {
            bool find = false; // 是否已经找到没有绑定连接线控制点的插槽
            int priority = -1;
            SlotGraphElement newSlot = null;            

            foreach (SlotGraphElement slot in inSlotList)
            {
                if (!slot.Binded)
                {
                    if(slot.Priority < priority || priority == -1)
                    {
                        newSlot = slot;
                        priority = slot.Priority;
                    }                    
                    find = true;
                }
            }

            if (!find) // 没有找到没有绑定连接线控制点的插槽，则新建一个入口插槽
            {
                InSlotCount = inSlotCount + 1;
                newSlot = inSlotList[inSlotCount - 1];
            }

            return newSlot;
        }

        /// <summary>
        /// 绑定连入的连接线
        /// </summary>
        /// <param name="line">连接线</param>
        public virtual void BindInLine(ConnectorContainer line)
        {
            SlotGraphElement slot = GetInSlot();

            if (slot != null)
            {
                slot.Bind(line.HeadConnector);
            }
        }

        /// <summary>
        /// 绑定连入的连接线
        /// </summary>
        /// <param name="line">连接线</param>
        /// <param name="index">索引</param>
        public virtual void BindInLine(ConnectorContainer line, int index)
        {            
            if (index >= 0 && index < inSlotCount)
            {
                SlotGraphElement slot = inSlotList[index];

                if (!slot.Binded)
                {
                    slot.Bind(line.HeadConnector);
                }
            }
        }

        /// <summary>
        /// 绑定连出的连接线
        /// </summary>
        /// <param name="line">连接线</param>
        public virtual void BindOutLine(ConnectorContainer line)
        {
            SlotGraphElement slot = GetOutSlot();

            if (slot != null)
            {
                slot.Bind(line.TailConnector);
            }
        }

        /// <summary>
        /// 绑定连出的连接线
        /// </summary>
        /// <param name="line">连接线</param>
        /// <param name="index">索引</param>
        public virtual void BindOutLine(ConnectorContainer line, int index)
        {
            if (index >= 0 && index < outSlotCount)
            {
                SlotGraphElement slot = outSlotList[index];

                if (!slot.Binded)
                {
                    slot.Bind(line.TailConnector);
                }
            }
        }

        /// <summary>
        /// 刷新连接的连接线的显示
        /// </summary>
        /// <param name="lineVisible">是否显示连接线</param>
        public void RefreshRelevateLine(bool lineVisible)
        {
            foreach (SlotGraphElement slot in slotList)
            {
                if (slot.Binded)
                {
                    slot.BindingConnector.Line.Visible = lineVisible; // 移动中隐藏连接线
                }
            }
        }

        /// <summary>
        /// 获取在指定位置的插槽
        /// </summary>
        /// <param name="p">指定位置</param>
        /// <returns>插槽</returns>
        public SlotGraphElement GetSlotInRegion(Point p)
        {
            SlotGraphElement slotGraphElement = null;

            foreach (SlotGraphElement slot in slotList) // 遍历检查容器里的插槽是否可被激活
            {
                if (slot.IsInRegion(p))
                {
                    slotGraphElement = slot;
                    break;
                }
            }

            return slotGraphElement;
        }

        /// <summary>
        /// 解除连接所有的连接线
        /// </summary>
        public void UnBind()
        {
            foreach (SlotGraphElement slot in slotList)
            {
                if (slot.Binded)
                {
                    slot.UnBind();
                }
            }
        }

        /// <summary>
        /// 获取连接的连接线
        /// </summary>
        /// <returns>连接线链表</returns>
        public List<ConnectorContainer> GetConnectedLine()
        {
            List<ConnectorContainer> lineList = new List<ConnectorContainer>();

            foreach (SlotGraphElement slot in slotList)
            {
                if (slot.Binded)
                {
                    ConnectorContainer line = slot.BindingConnector.Line;

                    if (!lineList.Contains(line))
                    {
                        lineList.Add(line);
                    }
                }
            }

            return lineList;
        }

        /// <summary>
        /// 增加出口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public virtual SlotGraphElement AddOutSlot()
        {
            OutSlotCount = outSlotCount + 1;
            SlotGraphElement newSlot = outSlotList[outSlotCount - 1];

            return newSlot;
        }

        /// <summary>
        /// 获取出口插槽
        /// </summary>
        /// <returns>出口插槽</returns>
        public virtual SlotGraphElement GetOutSlot()
        {
            bool find = false; // 是否已经找到没有绑定连接线控制点的插槽
            int priority = -1;
            SlotGraphElement newSlot = null;

            foreach (SlotGraphElement slot in outSlotList)
            {
                if (!slot.Binded)
                {
                    if(slot.Priority < priority || priority == -1)
                    {
                        newSlot = slot;
                        priority = slot.Priority;
                    }
                    find = true;
                }
            }

            if (!find) // 没有找到没有绑定连接线控制点的插槽，则新建一个入口插槽
            {
                OutSlotCount = outSlotCount + 1;
                newSlot = outSlotList[outSlotCount - 1];
            }

            return newSlot;
        }

        /// <summary>
        /// 删除插槽
        /// </summary>
        /// <param name="slot">要删除的插槽</param>
        public virtual void RemoveSlot(SlotGraphElement slot)
        {
            if (slot.IsInSlot) // 入口插槽
            {
                RemoveInSlot(slot);
            }
            else // 出口插槽
            {
                RemoveOutSlot(slot);
            }
        }

        /// <summary>
        /// 锁定入口插槽
        /// </summary>
        public void LockInSlot()
        {
            foreach (SlotGraphElement slot in inSlotList)
            {
                slot.CanDelete = false;
            }
        }

        /// <summary>
        /// 锁定出口插槽
        /// </summary>
        public void LockOutSlot()
        {
            foreach (SlotGraphElement slot in outSlotList)
            {
                slot.CanDelete = false;
            }
        }        

        /// <summary>
        /// 删除入口插槽
        /// </summary>
        /// <param name="slot">要删除的插槽</param>
        protected virtual void RemoveInSlot(SlotGraphElement slot)
        {
            if(slot.Binded) // 解除绑定
            {
                slot.UnBind();
            }
            inSlotList.Remove(slot);
            slotList.Remove(slot);
            inSlotCount = inSlotList.Count;
            AdjustInSlotList(inSlotList.Count);
        }

        /// <summary>
        /// 删除出口插槽
        /// </summary>
        /// <param name="slot">要删除的插槽</param>
        protected virtual void RemoveOutSlot(SlotGraphElement slot)
        {
            if (slot.Binded) // 解除绑定
            {
                slot.UnBind();
            }
            outSlotList.Remove(slot);
            slotList.Remove(slot);
            buttonList.Remove(slot.BindingConnectButton);
            outSlotCount = outSlotList.Count;
            AdjustOutSlotList(outSlotList.Count);
        }

        /// <summary>
        /// 复制数据
        /// </summary>
        /// <param name="srcSlotContainer">源插槽容器</param>
        /// <param name="destSlotContainer">目标插槽容器</param>
        protected virtual void CopyData(SlotContainer srcSlotContainer, SlotContainer destSlotContainer)
        {
            destSlotContainer.Name = srcSlotContainer.Name;
            destSlotContainer.Remark = srcSlotContainer.Remark;
            destSlotContainer.Text = srcSlotContainer.Text;
            destSlotContainer.DisplayText = srcSlotContainer.DisplayText;
            destSlotContainer.TooltipText = srcSlotContainer.TooltipText;
            destSlotContainer.ShowText = srcSlotContainer.ShowText;

            // 初始化绘图参数
            destSlotContainer.Init();
            destSlotContainer.InSlotCount = srcSlotContainer.InSlotCount;
            destSlotContainer.OutSlotCount = srcSlotContainer.OutSlotCount;
            destSlotContainer.AdjustText();
            destSlotContainer.AdjustResizeControlerList();
            destSlotContainer.AdjustButtonList();
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public virtual SlotContainer Clone()
        {            
            return null;
        }
    }
}
