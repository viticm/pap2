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
    [Serializable]
    public class ActionGraphElement:SlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="id">图元的大小</param>
        public ActionGraphElement(Point location, Size size)
            : base(location, size)
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
                return "ActionNode";
            }
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.ActionNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.ActionNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.ActionNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.ActionNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.ActionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            ActionGraphElement actionGraphElement = new ActionGraphElement(location, elementSize);
            CopyData(this, actionGraphElement);                          

            return actionGraphElement;
        }
    }
}
