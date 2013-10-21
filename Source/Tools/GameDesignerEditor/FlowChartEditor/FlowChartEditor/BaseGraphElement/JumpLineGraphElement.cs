using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlowChartEditor
{
    [Serializable]
    class JumpLineGraphElement:ConnectorContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="tail">连接线尾部的位置</param>
        /// <param name="head">连接线头部的位置</param>
        /// <param name="id">连接线的id</param>
        public JumpLineGraphElement(Point tail, Point head)
            : base(tail, head)
        {
            connectorCount = 2;
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {
            // 绘制边框色
            Pen framePen;
            Pen frameArrawPen;
            //Pen shadowPen;
            //Pen shadowArrawPen;
            //Color shadowColor;

            if (activated) // 当前图元被激活
            {
                framePen = new Pen(activatedColor, 1);
                frameArrawPen = new Pen(activatedColor, 1);
                //shadowColor = Color.FromArgb(127, activatedColor);
                //shadowPen = new Pen(shadowColor, 2);
                //shadowArrawPen = new Pen(shadowColor, 2);
            }
            else if (selected) // 当前图元被选中
            {
                framePen = new Pen(selectedColor, 1);
                frameArrawPen = new Pen(SelectedColor, 1);
                //shadowColor = Color.FromArgb(127, selectedColor);
                //shadowPen = new Pen(shadowColor, 2);
                //shadowArrawPen = new Pen(shadowColor, 2);
            } 
            else // 当前图元没有被选中和激活
            {
                if(enable) // 图元的数据有效
                {
                    framePen = new Pen(frameColor, 1);
                    frameArrawPen = new Pen(frameColor, 1);
                }
                else
                {
                    framePen = new Pen(disableColor, 1);
                    frameArrawPen = new Pen(disableColor, 1);
                }                
                // shadowColor = Color.FromArgb(127, frameColor);
                // shadowPen = new Pen(shadowColor, 2);
                // shadowArrawPen = new Pen(shadowColor, 2);
            }

            frameArrawPen.CustomEndCap = new AdjustableArrowCap((float)(frameArrawPen.Width * 3), (float)(frameArrawPen.Width * 4), true);
            frameArrawPen.DashStyle = DashStyle.Dash;
            framePen.DashStyle = DashStyle.Dash;
            // shadowArrawPen.CustomEndCap = new AdjustableArrowCap((float)(shadowArrawPen.Width * 3), (float)(shadowArrawPen.Width * 4), true);

            int index = 0;
            ConnectorGraphElement head;
            ConnectorGraphElement tail;
            Size shadowSize = new Size(2, 0);

            while (index + 1 < connectorList.Count)
            {
                head = connectorList[index];
                tail = connectorList[index + 1];

                if (index + 1 == connectorList.Count - 1) // 绘制带箭头的尾线
                {
                    g.DrawLine(frameArrawPen, head.Location, tail.Location);
                    // g.DrawLine(shadowArrawPen, head.Location + shadowSize, tail.Location + shadowSize); // 绘制阴影
                }
                else // 绘制不带箭头的线
                {
                    g.DrawLine(framePen, head.Location, tail.Location);
                    // g.DrawLine(shadowPen, head.Location + shadowSize, tail.Location + shadowSize); // 绘制阴影
                }

                index++;
            }

            // 绘制连接点
            foreach (ConnectorGraphElement c in connectorList)
            {
                c.Paint(g);
            }

            // 绘制文本
            if (showText)
            {
                int textX;
                int textY;

                if (connectorList.Count == 2) // 容器只有两个连接线控制点
                {
                    ConnectorGraphElement connector1 = connectorList[0];
                    ConnectorGraphElement connector2 = connectorList[1];
                    textX = (int)((connector1.Location.X + connector2.Location.X) / 2 + 10);
                    textY = (int)((connector1.Location.Y + connector2.Location.Y) / 2 + 10);
                }
                else
                {
                    ConnectorGraphElement connector = connectorList[1];
                    textX = connector.Location.X + 10;
                    textY = connector.Location.Y;
                }

                Brush textBrush = new SolidBrush(textColor);
                g.DrawString(text, textFont, textBrush, textX, textY);
            }
        }
    }
}
