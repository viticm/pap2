using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class InnerChart : AISlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元位置</param>
        /// <param name="size">图元大小</param>
        public InnerChart (Point location, Size size): base(location, size)
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
                return "InnerChart";
            }
        }

        /// <summary>
        /// 获取有接口选项的连接线
        /// </summary>
        /// <param name="infoString">接口选项</param>
        /// <returns>连接线</returns>
        public ConnectorContainer GetConnectedLine(string infoString)
        {
            ConnectorContainer line = null;

            foreach (SlotGraphElement slot in outSlotList)
            {
                if (slot.Binded)
                {
                    ConnectorContainer connectorContainer = slot.BindingConnector.Line;

                    if (connectorContainer.Text == infoString)
                    {
                        line = connectorContainer;
                        break;
                    }
                }
            }

            return line;
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
        /// 获取入口插槽
        /// </summary>
        /// <returns>入口插槽</returns>
        public override SlotGraphElement GetInSlot()
        {
            int priority = -1;
            SlotGraphElement newSlot = null;

            foreach (SlotGraphElement slot in inSlotList)
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
        /// 增加入口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public override SlotGraphElement AddInSlot()
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
                normalImage = graphSetting.AIActionNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.AIActionNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.AIActionNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.AIActionNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.AIActionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            InnerChart innerChart = new InnerChart(location, elementSize);
            CopyData(this, innerChart);

            return innerChart;
        }
    }
}
