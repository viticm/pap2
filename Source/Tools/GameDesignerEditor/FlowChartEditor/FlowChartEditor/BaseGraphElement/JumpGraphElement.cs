using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace FlowChartEditor
{
    [Serializable]
    class JumpGraphElement:SlotContainer
    {
        private List<SlotGraphElement> jumpSlotList = new List<SlotGraphElement>(); // 跳转插槽链表
        private int jumpSlotCount; // 跳转插槽数量

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="id">图元的大小</param>
        public JumpGraphElement(Point location, Size size)
            : base(location, size)
        {
            inSlotCount = 1;
            outSlotCount = 1;
            jumpSlotCount = 1;
        }        

        /// <summary>
        /// 跳转插槽数量属性
        /// </summary>
        public int JumpSlotCount
        {
            set
            {
                AdjustJumpSlotList(value);
                jumpSlotCount = value;
            }

            get
            {
                return jumpSlotCount;
            }
        }

        /// <summary>
        /// 图元大小属性
        /// </summary>
        [Browsable(true), Description("图元的大小"), Category("绘图属性")]
        public override Size ElementSize
        {
            set
            {
                elementSize = value;
                AdjustInSlotList(inSlotCount); // 调整入口插槽链表
                AdjustOutSlotList(outSlotCount); // 调整出口插槽链表
                AdjustJumpSlotList(jumpSlotCount); // 调整跳转插槽链表
                AdjustResizeControlerList(); // 调整缩放控制点链表
                AdjustButtonList(); // 调整按钮链表
            }

            get
            {
                return elementSize;
            }
        }

        /// <summary>
        /// 初始化结点的插槽
        /// </summary>
        /// <param name="Size">当前图元的大小</param>
        protected override void InitSlot(Size size)
        {
            int x1 = location.X + (int)(size.Width / 2) - 3;
            int y1 = location.Y - 3;
            int x2 = x1;
            int y2 = location.Y + size.Height;
            int x3 = location.X + size.Width - 3;
            int y3 = location.Y + (int)((size.Height - 20) / 2) - 3 + 20;

            SlotGraphElement slot1 = new SlotGraphElement(this, new Point(x1, y1), new Size(6, 6));
            SlotGraphElement slot2 = new SlotGraphElement(this, new Point(x2, y2), new Size(6, 6));
            SlotGraphElement slot3 = new SlotGraphElement(this, new Point(x3, y3), new Size(6, 6));

            slot1.Name = "连接插槽";
            slot1.FrameColor = Color.RoyalBlue;
            slot1.SelectedColor = Color.Red;
            slot1.ActivatedColor = Color.DarkOrange;
            slot1.BindedColor = Color.Lime;
            slot1.IsInSlot = true;
            slot1.IsOutSlot = false;
            slot1.IsJumpSlot = false;
            slot1.CanDelete = true;
            slot2.Name = "连接插槽";
            slot2.FrameColor = Color.RoyalBlue;
            slot2.SelectedColor = Color.Red;
            slot2.ActivatedColor = Color.DarkOrange;
            slot2.BindedColor = Color.Lime;
            slot2.IsInSlot = false;
            slot2.IsOutSlot = true;
            slot2.IsJumpSlot = false;
            slot2.CanDelete = true;
            slot3.Name = "连接插槽";
            slot3.FrameColor = Color.RoyalBlue;
            slot3.SelectedColor = Color.Red;
            slot3.ActivatedColor = Color.DarkOrange;
            slot3.BindedColor = Color.Lime;
            slot3.IsInSlot = false;
            slot3.IsOutSlot = false;
            slot3.IsJumpSlot = true;
            slot3.CanDelete = false;

            ConnectButton button = new ConnectButton(slot2, new Point(x2 - 6, y2 + 5), new Size(12, 18));
            button.Name = "连接按钮";
            button.NormalImage = connectButtonNormalImage;
            button.SelectedImage = connectButtonSelectedImage;
            button.ActivatedImage = connectButtonActivatedImage;
            slot2.BindingConnectButton = button;

            ConnectJumpButton jumpButton = new ConnectJumpButton(slot3, new Point(x3 + 5, y3 - 6), new Size(18, 12));
            jumpButton.Name = "连接按钮";
            jumpButton.NormalImage = connectJumpButtonNormalImage;
            jumpButton.SelectedImage = connectJumpButtonSelectedImage;
            jumpButton.ActivatedImage = connectJumpButtonActivatedImage;
            slot3.BindingConnectButton = jumpButton;

            inSlotList.Add(slot1);
            outSlotList.Add(slot2);
            jumpSlotList.Add(slot3);
            buttonList.Add(button);
            buttonList.Add(jumpButton);
            slotList.Add(slot1);
            slotList.Add(slot2);
            slotList.Add(slot3);
        }

        /// <summary>
        /// 调整插槽数量
        /// </summary>
        /// <param name="n">插槽数量</param>
        private void AdjustJumpSlotList(int n)
        {
            if (n == jumpSlotList.Count) // 不需要调整插槽
            {
                int newY;
                Point newPoint;

                // 调整插糟的坐标
                for (int i = 0; i < jumpSlotList.Count; i++)
                {
                    newY = (int)(location.Y + (elementSize.Height - 20) * (i + 1) / (n + 1) - 3 + 20);
                    newPoint = new Point(location.X + elementSize.Width - 3, newY);
                    jumpSlotList[i].Location = newPoint;
                    jumpSlotList[i].BindingConnectButton.Location = new Point(location.X + elementSize.Width + 5, newY - 6);
                    if (jumpSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        jumpSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else if (n < jumpSlotList.Count) // 需要减少插槽数量
            {
                int deleteCount = jumpSlotList.Count - n;
                int newY;
                Point newPoint;
                List<SlotGraphElement> deleteSlotList = new List<SlotGraphElement>();

                for (int i = 0; i < deleteCount; i++)
                {
                    deleteSlotList.Add(jumpSlotList[jumpSlotList.Count - 1 - i]);
                }

                foreach (SlotGraphElement slot in deleteSlotList)
                {
                    if (slot.Binded)
                    {
                        slot.UnBind();
                    }

                    jumpSlotList.Remove(slot);
                    slotList.Remove(slot);
                }

                // 调整插糟的坐标
                for (int i = 0; i < jumpSlotList.Count; i++)
                {
                    newY = (int)(location.Y + (elementSize.Height - 20) * (i + 1) / (n + 1) - 3 + 20);
                    newPoint = new Point(location.X + elementSize.Width - 3, newY);
                    jumpSlotList[i].Location = newPoint;
                    jumpSlotList[i].BindingConnectButton.Location = new Point(location.X + elementSize.Width + 5, newY - 6);
                    if (jumpSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        jumpSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else // 需要增加插槽数量
            {
                int oldCount = jumpSlotList.Count;
                SlotGraphElement newSlot;
                Point newPoint;
                int newY;

                // 添加插槽并调整插槽的坐标
                for (int i = 0; i < n; i++)
                {
                    if (i < oldCount)
                    {
                        newY = (int)(location.Y + (elementSize.Height - 20) * (i + 1) / (n + 1) - 3 + 20);
                        newPoint = new Point(location.X + elementSize.Width - 3, newY);
                        jumpSlotList[i].Location = newPoint;
                        jumpSlotList[i].BindingConnectButton.Location = new Point(location.X + elementSize.Width + 5, newY - 6);
                        if (jumpSlotList[i].Binded) // 调整绑定的连接线控制点
                        {
                            jumpSlotList[i].BindingConnector.Location = newPoint;
                        }
                    }
                    else
                    {
                        newY = (int)(location.Y + (elementSize.Height - 20) * (i + 1) / (n + 1) - 3 + 20);
                        newPoint = new Point(location.X + elementSize.Width - 3, newY);
                        newSlot = new SlotGraphElement(this, newPoint, new Size(6, 6));
                        newSlot.Name = "连接插槽";
                        newSlot.FrameColor = Color.RoyalBlue;
                        newSlot.SelectedColor = Color.Red;
                        newSlot.ActivatedColor = Color.DarkOrange;
                        newSlot.BindedColor = Color.Lime;
                        newSlot.IsInSlot = false;
                        newSlot.IsOutSlot = false;
                        newSlot.IsJumpSlot = true;
                        newSlot.CanDelete = false;

                        ConnectJumpButton button = new ConnectJumpButton(newSlot, new Point(location.X + elementSize.Width + 5, 
                            newY - 6), new Size(12, 18));
                        button.Name = "连接按钮";
                        button.NormalImage = connectJumpButtonNormalImage;
                        button.SelectedImage = connectJumpButtonSelectedImage;
                        button.ActivatedImage = connectJumpButtonActivatedImage;
                        newSlot.BindingConnectButton = button;

                        buttonList.Add(button);
                        jumpSlotList.Add(newSlot);
                        slotList.Add(newSlot);
                    }
                }
            }
        }

        /// <summary>
        /// 获取跳转插槽链表
        /// </summary>
        /// <returns>跳转插槽链表 </returns>
        public List<SlotGraphElement> GetJumpSlotList()
        {
            return jumpSlotList;
        }

        /// <summary>
        /// 增加跳转插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public SlotGraphElement AddJumpSlot()
        {            
            JumpSlotCount = jumpSlotCount + 1;
            SlotGraphElement newSlot = jumpSlotList[jumpSlotCount - 1];            

            return newSlot;
        }

        /// <summary>
        /// 获取跳转插槽
        /// </summary>
        /// <returns>跳转插槽</returns>
        public SlotGraphElement GetJumpSlot()
        {
            bool find = false; // 是否已经找到没有绑定连接线控制点的插槽
            SlotGraphElement newSlot = null;

            foreach (SlotGraphElement slot in jumpSlotList)
            {
                if (!slot.Binded)
                {
                    newSlot = slot;
                    find = true;
                    break;
                }
            }

            if (!find) // 没有找到没有绑定连接线控制点的插槽，则新建一个入口插槽
            {
                JumpSlotCount = jumpSlotCount + 1;
                newSlot = jumpSlotList[jumpSlotCount - 1];
            }

            return newSlot;
        }
    }
}
