using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using GraphElementLibrary;

namespace FlowChartEditor
{
    class TextGraphElement:SlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="id">图元的大小</param>
        public TextGraphElement(Point location, Size size)
            : base(location, size)
        {
            inSlotCount = 0;
            outSlotCount = 0;
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
                AdjustResizeControlerList(); // 调整缩放控制点链表
            }

            get
            {
                return elementSize;
            }            
        }


        /// <summary>
        /// 初始化插槽
        /// </summary>
        /// <param name="size">图元的大小</param>
        protected override void InitSlot(Size size)
        {
            
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
        /// 增加出口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public override SlotGraphElement AddOutSlot()
        {
            return null;
        }

        /// <summary>
        /// 删除入口插槽
        /// </summary>
        /// <param name="slot">要删除的插槽</param>
        public override void RemoveInSlot(SlotGraphElement slot)
        {

        }

        /// <summary>
        /// 删除出口插槽
        /// </summary>
        /// <param name="slot">要删除的插槽</param>
        public override void RemoveOutSlot(SlotGraphElement slot)
        {

        }
    }
}
