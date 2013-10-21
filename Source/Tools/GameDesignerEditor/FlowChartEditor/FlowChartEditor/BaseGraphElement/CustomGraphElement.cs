using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Imaging;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    class CustomGraphElement: SlotContainer 
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="id">图元的大小</param>
        public CustomGraphElement(Point location, Size size)
            : base(location, size)
        {
            inSlotCount = 1;
            outSlotCount = 1;
        }        
    }
}
