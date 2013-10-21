using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Windows.Forms;
using FlowChartEditor.ButtonGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class ConditionGraphElement:SlotContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        /// <param name="size">图元的大小</param>
        public ConditionGraphElement(Point location, Size size):base(location, size)
        {
            inSlotCount = 1;
            outSlotCount = 3;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        public override string TypeString
        {
            get
            {
                return "ConditionNode";
            }
        }

        /// <summary>
        /// 初始化结点的插槽
        /// </summary>
        /// <param name="size">图元的大小</param>
        protected override void InitSlot(Size size)
        {
            Helper helper = Helper.GetHelper();
            int x1 = location.X + (int)(size.Width / 2);
            int y1 = location.Y;            
            int x2 = location.X;
            int y2 = location.Y + (int)(size.Height / 2);
            int x3 = location.X + size.Width;
            int y3 = y2;
            int x4 = location.X + (int)(size.Width / 2);
            int y4 = location.Y + size.Height;

            SlotGraphElement slot1 = new SlotGraphElement(this, new Point(x1, y1), new Size(6, 6));
            SlotGraphElement slot2 = new SlotGraphElement(this, new Point(x2, y2), new Size(6, 6));
            SlotGraphElement slot3 = new SlotGraphElement(this, new Point(x3, y3), new Size(6, 6));
            SlotGraphElement slot4 = new SlotGraphElement(this, new Point(x4, y4), new Size(6, 6));

            slot1.Name = "连接插槽";         
            slot1.IsInSlot = true;

            slot2.Name = "连接插槽";
            slot2.IsOutSlot = true;
            slot2.CanDelete = false;
            slot2.Priority = 101;
            ConnectButton button1 = new ConnectButton(slot2, new Point(x2 - 6, y2 + 25), new Size(12, 8));
            button1.Name = "连接按钮";
            slot2.BindingConnectButton = button1;

            slot3.Name = "连接插槽";
            slot3.IsOutSlot = true;
            slot3.CanDelete = false;
            slot3.Priority = 100;
            ConnectButton button2 = new ConnectButton(slot3, new Point(x3 - 6, y3 + 25), new Size(12, 8));
            button2.Name = "连接按钮";
            slot3.BindingConnectButton = button2;

            slot4.Name = "连接插槽";
            slot4.IsOutSlot = true;
            slot4.CanDelete = false;

            ConnectButton button3 = new ConnectButton(slot4, new Point(x4 - 6, y4 + 25), new Size(12, 8));
            button3.Name = "连接按钮";
            slot4.BindingConnectButton = button3;

            inSlotList.Add(slot1);
            outSlotList.Add(slot2);
            outSlotList.Add(slot3);
            outSlotList.Add(slot4);
            buttonList.Add(button1);
            buttonList.Add(button2);
            buttonList.Add(button3);
            slotList.Add(slot1);
            slotList.Add(slot2);
            slotList.Add(slot3);
            slotList.Add(slot4);

            // 重设插槽优先级
            ResetSlotProperty(inSlotList);
            ResetSlotProperty(outSlotList);
        }

        /// <summary>
        /// 调整出口插槽链表
        /// </summary>
        /// <param name="n">插槽数量</param>
        protected override void AdjustOutSlotList(int n)
        {
            int newX;
            int newY;
            Point newPoint = Point.Empty;

            if (n == outSlotList.Count) // 不需要调整插槽
            {
                newX = location.X;
                newY = (int)(location.Y + elementSize.Height / 2);
                newPoint.X = newX;
                newPoint.Y = newY;
                outSlotList[0].Location = newPoint;
                outSlotList[0].BindingConnectButton.Location = new Point(newX - 6, newY + 25);
                if (outSlotList[0].Binded) // 调整绑定的连接线控制点
                {
                    outSlotList[0].BindingConnector.Location = newPoint;
                }

                newX = location.X + elementSize.Width;
                newY = (int)(location.Y + elementSize.Height / 2);
                newPoint.X = newX;
                newPoint.Y = newY;
                outSlotList[1].Location = newPoint;
                outSlotList[1].BindingConnectButton.Location = new Point(newX - 6, newY + 25);
                if (outSlotList[1].Binded) // 调整绑定的连接线控制点
                {
                    outSlotList[1].BindingConnector.Location = newPoint;
                }

                // 调整插糟的坐标
                for (int i = 2; i < outSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i - 2 + 1) / (n - 2 + 1));
                    newY = location.Y + elementSize.Height;
                    newPoint.X = newX;
                    newPoint.Y = newY;
                    outSlotList[i].Location = newPoint;
                    outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, newY + 25);
                    if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        outSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else if (n < outSlotList.Count) // 需要减少插槽数量
            {
                int deleteCount = outSlotList.Count - n;
                List<SlotGraphElement> deleteSlotList = new List<SlotGraphElement>();

                for (int i = 0; i < deleteCount; i++)
                {
                    deleteSlotList.Add(outSlotList[outSlotList.Count - 1 - i]);
                }

                foreach (SlotGraphElement slot in deleteSlotList)
                {
                    if (slot.Binded)
                    {
                        slot.UnBind();
                    }

                    buttonList.Remove(slot.BindingConnectButton);
                    slot.BindingConnectButton = null;
                    outSlotList.Remove(slot);
                    slotList.Remove(slot);
                }

                // 调整插糟的坐标
                for (int i = 2; i < outSlotList.Count; i++)
                {
                    newX = (int)(location.X + elementSize.Width * (i - 2 + 1) / (n - 2 + 1));
                    newY = location.Y + elementSize.Height;
                    newPoint.X = newX;
                    newPoint.Y = newY;
                    outSlotList[i].Location = newPoint;
                    outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, newY + 25);
                    if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                    {
                        outSlotList[i].BindingConnector.Location = newPoint;
                    }
                }
            }
            else // 需要增加插槽数量
            {
                Helper helper = Helper.GetHelper();
                int oldCount = outSlotList.Count;
                SlotGraphElement newSlot;

                // 添加插槽并调整插槽的坐标
                for (int i = 2; i < n; i++)
                {
                    if (i < oldCount)
                    {
                        newX = (int)(location.X + elementSize.Width * (i - 2 + 1) / (n - 2 + 1));
                        newY = location.Y + elementSize.Height;
                        newPoint.X = newX;
                        newPoint.Y = newY;
                        outSlotList[i].Location = newPoint;
                        outSlotList[i].BindingConnectButton.Location = new Point(newX - 6, newY + 25);
                        if (outSlotList[i].Binded) // 调整绑定的连接线控制点
                        {
                            outSlotList[i].BindingConnector.Location = newPoint;
                        }
                    }
                    else
                    {
                        newX = (int)(location.X + elementSize.Width * (i - 2 + 1) / (n - 2 + 1));
                        newY = location.Y + elementSize.Height;
                        newPoint.X = newX;
                        newPoint.Y = newY;
                        newSlot = new SlotGraphElement(this, newPoint, new Size(6, 6));
                        newSlot.Name = "连接插槽";
                        newSlot.IsOutSlot = true;
                        newSlot.CanDelete = true;
                        newSlot.Refresh();

                        ConnectButton button = new ConnectButton(newSlot, new Point(newX - 6, location.Y + elementSize.Height + 25),
                            new Size(12, 8));
                        button.Name = "连接按钮";
                        button.Refresh();
                        newSlot.BindingConnectButton = button;

                        outSlotList.Add(newSlot);
                        buttonList.Add(button);
                        slotList.Add(newSlot);
                    }
                }
            }

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
            if (textWidth * 2 < textRectangle.Width - 12)
            {
                if (textWidth * 2 < minSize.Width - 12)
                {
                    newWidth = minSize.Width;
                }
                else
                {
                    newWidth = (int)(textWidth * 2 + 12);
                }
            }
            else
            {
                newWidth = (int)(textWidth * 2 + 12);
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

                if (newHeight > minSize.Height * 2)
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
                currentWidth = textSize.Width * 2;
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
                            currentWidth = textSize.Width * 2;
                            j++;
                        }

                        j--;
                        newData.Add(newLine);
                        line = "    " + line.Substring(line.Length - j, j);
                        textSize = g.MeasureString(line, graphSetting.GraphElementTextFont);
                        currentWidth = textSize.Width * 2;
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
            int textY = (int)(textRectangle.Y + (textRectangle.Height - textHeight) / 2);
            textLocation.X = textX;
            textLocation.Y = textY;
        }

        /// <summary>
        /// 刷新图元的图片
        /// </summary>
        protected override void RefreshImages()
        {
            if (normalImage == null)
            {
                normalImage = graphSetting.ConditionNodeNormalImage;
            }

            if (selectedImage == null)
            {
                selectedImage = graphSetting.ConditionNodeSelectedImage;
            }

            if (activatedImage == null)
            {
                activatedImage = graphSetting.ConditionNodeActivatedImage;
            }

            if (shadowImage == null)
            {
                shadowImage = graphSetting.ConditionNodeShadowImage;
            }

            if (abbreviateImage == null)
            {
                abbreviateImage = graphSetting.ConditionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override SlotContainer Clone()
        {
            ConditionGraphElement conditionGraphElement = new ConditionGraphElement(location, elementSize);
            CopyData(this, conditionGraphElement);             

            return conditionGraphElement;
        }
    }
}
