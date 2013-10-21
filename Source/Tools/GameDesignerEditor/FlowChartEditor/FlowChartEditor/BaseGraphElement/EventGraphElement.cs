using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using FlowChartEditor.ButtonGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class EventGraphElement:SlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="id">图元的大小</param>
        public EventGraphElement(Point location, Size size)
            : base(location, size)
        {
            inSlotCount = 0;
            outSlotCount = 1;
            enable = true;
            isEntrance = true;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        public override string TypeString
        {
            get
            {
                return "EventNode";
            }
        }

        /// <summary>
        /// 入口插糟数量属性
        /// </summary>
        [Browsable(false)]
        public override int InSlotCount
        {
            get
            {
                return inSlotCount;
            }
        }

        /// <summary>
        /// 初始化结点的插槽
        /// </summary>
        /// <param name="size">当前图元的大小</param>
        protected override void InitSlot(Size size)
        {
            Helper helper = Helper.GetHelper();
            int x1 = location.X + (int)(size.Width / 2);
            int y1 = location.Y + size.Height;

            SlotGraphElement slot1 = new SlotGraphElement(this, new Point(x1, y1), new Size(6, 6));
            slot1.Name = "连接插槽";
            slot1.IsOutSlot = true;
            slot1.CanDelete = false;

            ConnectButton button = new ConnectButton(slot1, new Point(x1 - 6, y1 + 25), new Size(12, 8));
            button.Name = "连接按钮";
            slot1.BindingConnectButton = button;  

            outSlotList.Add(slot1);
            buttonList.Add(button);
            slotList.Add(slot1);

            // 重设插槽优先级
            ResetSlotProperty(inSlotList);
            ResetSlotProperty(outSlotList);
        }

        /// <summary>
        /// 自动调整图元的大小
        /// </summary>
        public override void AdjustElementSize()
        {
            Graphics g = Helper.GetHelper().GraphicsObject;            
            Rectangle textRectangle = new Rectangle(location, elementSize);
            SizeF textSize = g.MeasureString(text, graphSetting.GraphElementTextFont);
            float textWidth = textSize.Width;
            float textHeight = textSize.Height;
            int newWidth = elementSize.Width;
            int newHeight = elementSize.Height;

            // measurestring不准，预留两个像素的偏移量
            if (textWidth < textRectangle.Width - 12)
            {
                if (textWidth < minSize.Width - 12)
                {
                    newWidth = minSize.Width;
                }
                else
                {
                    newWidth = (int)(textWidth + 12);
                }
            }
            else
            {
                newWidth = (int)(textWidth + 12);
            }

            if (textHeight * 2 < textRectangle.Height - 12)
            {
                if (textHeight * 2 < minSize.Height - 12)
                {
                    newHeight = minSize.Height;
                }
                else
                {
                    newHeight = (int)(textHeight * 2 + 12);
                }
            }
            else
            {
                newHeight = (int)(textHeight * 2 + 12);
            }

            // 调整图元大小
            if (newWidth != elementSize.Width || newHeight != elementSize.Height)
            {
                if (newWidth > minSize.Width * 2)
                {
                    newWidth = (int)(minSize.Width * 1.5);
                    
                }

                if(newHeight > minSize.Height * 2)
                {
                    newHeight = (int)(minSize.Height * 1.5);
                }

                ElementSize = new Size(newWidth, newHeight);
            }
        }

        /// <summary>
        /// 调整图元文本
        /// </summary>
        public override void AdjustText()
        {
            Graphics g = Helper.GetHelper().GraphicsObject;
            SizeF textSize = g.MeasureString(text, graphSetting.GraphElementTextFont);
            float textWidth = 0;
            float textHeight = textSize.Height;

            string[] data = text.Split(new char[] { '\n' });
            List<string> newData = new List<string>();
            displayText = "";
            float currentWidth;

            for (int i = 0; i < data.Length; i++)
            {
                string line = data[i];
                string newLine = line;
                textSize = g.MeasureString(line, graphSetting.GraphElementTextFont);
                currentWidth = textSize.Width;
                if (currentWidth > textRectangle.Width - 10) // 文本超出有效宽度
                {
                    int j = 2;

                    while (currentWidth > textRectangle.Width - 10)
                    {
                        j = 2;

                        while (currentWidth > textRectangle.Width - 10)
                        {
                            newLine = line.Substring(0, line.Length - j);
                            textSize = g.MeasureString(newLine, graphSetting.GraphElementTextFont);
                            currentWidth = textSize.Width;
                            j++;
                        }

                        j--;
                        newData.Add(newLine);
                        line = "    " + line.Substring(line.Length - j, j);
                        textSize = g.MeasureString(line, graphSetting.GraphElementTextFont);
                        currentWidth = textSize.Width;
                    }
                }

                newData.Add(line);
            }

            foreach (string s in newData)
            {
                displayText += s + "\n";
            }
            displayText = displayText.TrimEnd(new char[] { '\n' });
            textSize = g.MeasureString(displayText, graphSetting.GraphElementTextFont);
            textWidth = textSize.Width;
            textHeight = textSize.Height;

            if (textHeight * 2 + 10 > elementSize.Height) // 调整高度
            {
                elementSize.Height = (int)(textHeight * 2 + 10);
                textRectangle.Size = new Size(textRectangle.Width, elementSize.Height);
                shadowRectangle.Size = new Size(textRectangle.Width, elementSize.Height);
                invalidRectangle.Size = new Size(textRectangle.Width + 20, elementSize.Height + 45);
            }

            int textX = (int)(textRectangle.X + (textRectangle.Width - textWidth) / 2);
            int textY = (int)(textRectangle.Y + ((float)textRectangle.Height / 2 - textHeight) / 2);
            textLocation.X = textX;
            textLocation.Y = textY;
        }

        /// <summary>
        /// 调整入口插槽
        /// </summary>
        /// <param name="n">插槽数量</param>
        protected override void AdjustInSlotList(int n)
        {

        }

        /// <summary>
        /// 获取未绑定的入口插槽
        /// </summary>
        /// <returns>入口插槽</returns>
        public override SlotGraphElement GetInSlot()
        {
            return null;
        }

        /// <summary>
        /// 增加入口插槽
        /// </summary>
        /// <returns>新增的插槽</returns>
        public override SlotGraphElement AddInSlot()
        {
            MessageBox.Show("事件结点上不能新增入口插槽！", "连接图元", MessageBoxButtons.OK, MessageBoxIcon.Information);
            return null;
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.EventNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.EventNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.EventNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.EventNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.EventNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            EventGraphElement eventGraphElement = new EventGraphElement(location, elementSize);
            CopyData(this, eventGraphElement);            

            return eventGraphElement;
        }
    }
}
