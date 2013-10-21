using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class AISlotContainer:SlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public AISlotContainer(Point location, Size elementSize):
            base(location, elementSize)
        {
            enable = true;
        }

        /// <summary>
        /// 刷新图元的数据
        /// </summary>
        public override void Refresh()
        {
            base.Refresh();
            requireEvent = false;
        }
    }
}
