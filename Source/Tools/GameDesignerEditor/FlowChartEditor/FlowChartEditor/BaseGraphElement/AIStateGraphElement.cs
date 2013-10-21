using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.ComponentModel;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class AIStateGraphElement:AISlotContainer
    {        
        private bool isInitialState = false; // 是否是初态

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="elementSize">图元的大小</param>
        public AIStateGraphElement(Point location, Size elementSize):base(location, elementSize)
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
                return "AIStateNode";
            }
        }

        /// <summary>
        /// 是否是初态
        /// </summary>
        [Browsable(false)]
        public bool IsInitialState
        {
            set
            {
                isInitialState = value;
                isEntrance = value;                
            }

            get
            {
                return isInitialState;
            }
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {            
            base.Paint(g);

            // 绘制初态标记
            if (visible && isInitialState)
            {
                g.FillEllipse(Brushes.Black, location.X + elementSize.Width + 10, location.Y, 10, 10);
            }
        }

        /// <summary>
        /// 绘制图元的缩略效果
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void AbbreviativePaint(Graphics g)
        {
            // 绘制主框体
            g.DrawImage(abbreviateImage, textRectangle);
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.AIStateNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.AIStateNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.AIStateNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.AIStateNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.AIStateNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {            
            AIStateGraphElement aiStateGraphElement = new AIStateGraphElement(location, elementSize);
            CopyData(this, aiStateGraphElement);
            aiStateGraphElement.IsInitialState = isInitialState;

            return aiStateGraphElement;
        }
    }
}
