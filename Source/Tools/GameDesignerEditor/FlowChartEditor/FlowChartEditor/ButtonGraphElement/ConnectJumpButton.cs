using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace FlowChartEditor
{
    [Serializable]
    class ConnectJumpButton:ButtonGraphElement
    {
        private SlotGraphElement bindingSlot; // 绑定的插槽

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="bindingSlot">绑定的插槽</param>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public ConnectJumpButton(SlotGraphElement bindingSlot, Point location, Size elementSize)
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
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {
            RefreshImages();

            Rectangle rectangle = new Rectangle(location, elementSize);
            Rectangle hoverRectangle = new Rectangle(new Point(location.X, location.Y - 1),
                new Size(elementSize.Width + 3, elementSize.Height + 2));
            Image image;

            if (selected) // 图元被选中
            {
                image = selectedImage;
                g.DrawImage(image, hoverRectangle);
            }
            else if (activated) // 图元被激活
            {
                image = activatedImage;
                g.DrawImage(image, hoverRectangle);
            }
            else // 图元没有被选中和激活
            {
                image = normalImage;
                g.DrawImage(image, rectangle);
            }
        }
    }
}
