using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using FlowChartEditor.ButtonGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class InterfaceGraphElement : AISlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元位置</param>
        /// <param name="elementSize">图元大小</param>
        public InterfaceGraphElement(Point location, Size elementSize) : base(location, elementSize)
        {

        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        public override string TypeString
        {
            get
            {
                return "InterfaceNode";
            }
        }

        /// <summary>
        /// 获取连接的图元
        /// </summary>
        /// <returns>连接的图元</returns>
        public GraphElement GetConnectedOutGraphElement()
        {
            GraphElement graphElement = null;

            foreach (SlotGraphElement slot in outSlotList)
            {
                if (slot.Binded)
                {
                    ConnectorContainer line = slot.BindingConnector.Line;
                    graphElement = line.OutSlotContainer;
                }
            }

            return graphElement;
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.InterfaceNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.InterfaceNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.InterfaceNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.InterfaceNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.InterfaceNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 初始化按钮
        /// </summary>
        protected override void InitButton()
        {

        }

        /// <summary>
        /// 调整按钮链表
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
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            InterfaceGraphElement interfaceGraphElement = new InterfaceGraphElement(location, elementSize);
            CopyData(this, interfaceGraphElement);

            return interfaceGraphElement;
        }
    }
}
