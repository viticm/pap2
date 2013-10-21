using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using GraphElementLibrary;

namespace FlowChartEditor.BaseGraphElement
{
    [Serializable]
    public class LineGraphElement: ConnectorContainer
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="tail">连接线尾部的位置</param>
        /// <param name="head">连接线头部的位置</param>
        /// <param name="id">连接线的id</param>
        public LineGraphElement(Point tail, Point head)
            : base(tail, head)
        {
            connectorCount = 2;
        }

        /// <summary>
        /// 描述字符串
        /// </summary>        
        public override string TypeString
        {
            get
            {
                return "ConditionLine";
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <returns>复制的图元</returns>
        public override ConnectorContainer Clone()
        {
            LineGraphElement newLine = new LineGraphElement(tailConnector.Location, headConnector.Location);
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
