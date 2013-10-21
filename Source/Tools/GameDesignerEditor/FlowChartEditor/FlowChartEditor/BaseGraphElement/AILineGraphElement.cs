using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class AILineGraphElement:ConnectorContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="tail">连接线尾结点的位置</param>
        /// <param name="head">连接线头结点的位置</param>
        public AILineGraphElement(Point tail, Point head):base(tail, head)
        {
            connectorCount = 2;
            enable = true;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>
        public override string TypeString
        {
            get
            {
                return "AILine";
            }
        }

        /// <summary>
        /// 刷新图元的数据
        /// </summary>
        public override void Refresh()
        {
            base.Refresh();
            requireEvent = false;
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override ConnectorContainer Clone()
        {
            AILineGraphElement newLine = new AILineGraphElement(tailConnector.Location, headConnector.Location);
            newLine.Text = text;
            newLine.DisplayText = displayText;
            newLine.TooltipText = tooltipText;
            newLine.ShowText = showText;
            newLine.ShowID = showID;

            newLine.Init();
            newLine.Refresh();

            newLine.ConnectorCount = connectorCount;
            List<ConnectorGraphElement> list = newLine.GetConnectorList();

            for (int i = 0; i < connectorList.Count; i++)
            {
                list[i].Location = connectorList[i].Location;
            }

            return newLine;
        }
    }
}
