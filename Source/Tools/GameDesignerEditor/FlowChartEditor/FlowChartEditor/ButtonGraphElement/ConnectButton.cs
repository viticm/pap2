using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor.ButtonGraphElement
{
    [Serializable]
    public class ConnectButton : BaseButton
    {
        private SlotGraphElement bindingSlot; // 绑定的插槽

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="bindingSlot">绑定的插槽</param>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public ConnectButton(SlotGraphElement bindingSlot, Point location, Size elementSize)
            :base(location, elementSize)
        {
            this.bindingSlot = bindingSlot;
        }

        /// <summary>
        /// 绑定的插槽属性
        /// </summary>
        public SlotGraphElement BindingSlot
        {
            get
            {
                return bindingSlot;
            }
        }

        /// <summary>
        /// 刷新图元数据
        /// </summary>
        public override void Refresh()
        {
            base.Refresh();

            normalImage = graphSetting.ConnectButtonNormalImage;
            activatedImage = graphSetting.ConnectButtonActivatedImage;
        }
    }
}
