using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using FlowChartEditor.BaseGraphElement;
using System.ComponentModel;

namespace FlowChartEditor.ButtonGraphElement
{
    [Serializable]
    public class AddButton:BaseButton
    {
        private SlotContainer slotContainer; // 添加按钮对应的插槽        

        public AddButton(SlotContainer slotContainer, Point location, Size elementSize)
            :base(location, elementSize)
        {
            this.slotContainer = slotContainer;
        }

        /// <summary>
        /// 绑定的插槽属性
        /// </summary>
        [Browsable(false)]
        public SlotContainer BindingSlotContainer
        {
            get
            {
                return slotContainer;
            }
        }

        /// <summary>
        /// 刷新图元数据
        /// </summary>
        public override void Refresh()
        {
            base.Refresh();

            normalImage = graphSetting.AddButtonNormalImage;
            activatedImage = graphSetting.AddButtonActivatedImage;
        }
    }
}
