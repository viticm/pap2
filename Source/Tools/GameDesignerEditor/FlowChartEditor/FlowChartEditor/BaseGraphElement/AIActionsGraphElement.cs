using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using FlowChartEditor.ButtonGraphElement;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class AIActionsGraphElement:AISlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="locaiton">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public AIActionsGraphElement(Point locaiton, Size elementSize):base(locaiton, elementSize)
        {
            inSlotCount = 1;
            outSlotCount = 1;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        public override string TypeString
        {
            get
            {
                return "AIActionsNode";
            }
        }

        /// <summary>
        /// 初始化结点的插槽
        /// </summary>
        /// <param name="Size">当前图元的大小</param>
        protected override void InitSlot(Size size)
        {
            Helper helper = Helper.GetHelper();

            int x1 = location.X + (int)(size.Width / 2);
            int y1 = location.Y;
            int x2 = x1;
            int y2 = location.Y + size.Height;

            SlotGraphElement slot1 = new SlotGraphElement(this, new Point(x1, y1), new Size(6, 6));
            SlotGraphElement slot2 = new SlotGraphElement(this, new Point(x2, y2), new Size(6, 6));

            slot1.Name = "连接插槽";
            slot1.IsInSlot = true;
            slot1.CanDelete = false;

            slot2.Name = "连接插槽";
            slot2.IsOutSlot = true;
            slot2.CanDelete = false;

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
        /// 初始化按钮
        /// </summary>
        protected override void InitButton()
        {

        }

        /// <summary>
        /// 调整状态链表
        /// </summary>
        protected override void AdjustButtonList()
        {
            
        }

        /// <summary>
        /// 获取出口插槽
        /// </summary>
        /// <returns>出口插槽</returns>
        public override SlotGraphElement GetOutSlot()
        {
            int priority = -1;
            SlotGraphElement newSlot = null;

            foreach (SlotGraphElement slot in outSlotList)
            {
                if (!slot.Binded)
                {
                    if (slot.Priority < priority || priority == -1)
                    {
                        newSlot = slot;
                        priority = slot.Priority;
                    }
                }
            }

            return newSlot;
        }

        /// <summary>
        /// 增加出口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public override SlotGraphElement AddOutSlot()
        {
            return null;
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.AIActionsNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.AIActionsNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.AIActionsNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.AIActionsNodeShadowImage;
            }

            if(abbreviateImage == null)
            {
                abbreviateImage = graphSetting.AIActionsNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            AIActionsGraphElement aiActionsGraphElement = new AIActionsGraphElement(location, elementSize);
            CopyData(this, aiActionsGraphElement);

            return aiActionsGraphElement;
        }
    }
}
