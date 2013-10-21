using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor
{
    public class Tooltip : GraphElement
    {
        private GraphSetting graphSetting = GraphSetting.GetGraphSetting();        
        private enum TipFormat { Normal, Bold, UnderLine, Split };

        private const string normalStyleString = "<normal>";
        private const string boldStyleString = "<bold>";
        private const string italicStyleString = "<underline>";
        private const string splitStyleString = "<split>";

        private const int charWidth = 16;        
        private const int tipInterval = 5;
        private const int textInterval = 5;
        private Size graphElementInterval = new Size(10, 10);

        private string[] lineSpliter = new string[] { "\r\n" };                
        private List<string> lineList = new List<string>();
        private List<TipFormat> formatList = new List<TipFormat>();
        
        private Rectangle visibleRectangle;
        private Rectangle graphElementRectangle = Rectangle.Empty;
        private Point textLocation = Point.Empty;
        private Size textSize = Size.Empty;

        private GraphElement activatedGraphElement = null; 

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        public Tooltip(Point location) : base(location)
        {
            
        }

        /// <summary>
        /// 调整图元
        /// </summary>
        /// <param name="text">图元的文本</param>
        /// <param name="graphElementRectangle">图元区域</param>
        /// <param name="visibleRectable">当前可视区域</param>
        public void Adjust(string text, GraphElement graphElement, Rectangle visibleRectangle, Point mouseLocation)
        {
            this.text = text;            
            this.visibleRectangle = visibleRectangle;

            if (this.activatedGraphElement != graphElement)
            {
                this.activatedGraphElement = graphElement;
                Init(graphElement, mouseLocation);
            }            
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init(GraphElement graphElement, Point mouseLocation)
        {
            // 计算图元区域
            if (graphElement is SlotContainer)
            {
                SlotContainer slotContainer = graphElement as SlotContainer;
                graphElementRectangle = slotContainer.TextRectangle;
            }
            else if (graphElement is ConnectorContainer)
            {
                graphElementRectangle.Location = mouseLocation;
                graphElementRectangle.Size = graphElementInterval;
            }
            else
            {
                graphElementRectangle.Location = graphElement.Location;
                graphElementRectangle.Size = graphElement.ElementSize + graphElementInterval;
            }

            // 计算文字大小
            lineList.Clear();
            formatList.Clear();

            elementSize = Size.Empty;
            string[] lines = text.Split(lineSpliter, StringSplitOptions.RemoveEmptyEntries);
            Graphics g = Helper.GetHelper().GraphicsObject;
            
            textSize.Height = lines.Length * charWidth;
            int currentWidth = 0;            
            SizeF textSizeF;
            string displayLine;

            foreach (string line in lines)
            {                
                if (line.StartsWith(boldStyleString))
                {
                    displayLine = line.Replace(boldStyleString, "");
                    textSizeF = g.MeasureString(displayLine, graphSetting.GraphElementTextBoldFont);
                    currentWidth = (int)textSizeF.Width + 1;
                    
                    lineList.Add(displayLine);
                    formatList.Add(TipFormat.Bold);
                }
                else if (line.StartsWith(italicStyleString))
                {
                    displayLine = line.Replace(italicStyleString, "");
                    textSizeF = g.MeasureString(displayLine, graphSetting.GraphElementTextItalicFont);
                    currentWidth = (int)textSizeF.Width + 1;
                    
                    lineList.Add(displayLine);
                    formatList.Add(TipFormat.UnderLine);
                }
                else if (line.StartsWith(splitStyleString))
                {
                    currentWidth = 0;

                    displayLine = line.Replace(splitStyleString, "");
                    lineList.Add(displayLine);
                    formatList.Add(TipFormat.Split);
                }
                else
                {
                    displayLine = line;
                    textSizeF = g.MeasureString(displayLine, graphSetting.GraphElementTextFont);
                    currentWidth = (int)textSizeF.Width + 1;
                    
                    lineList.Add(displayLine);
                    formatList.Add(TipFormat.Normal);
                }

                if (currentWidth > textSize.Width)
                {
                    textSize.Width = currentWidth;
                }
            }

            elementSize.Width = textSize.Width + textInterval * 2;
            elementSize.Height = textSize.Height + textInterval * 2;

            // 计算文字位置
            if (graphElementRectangle.Right + elementSize.Width > visibleRectangle.Right)
            {
                location.X = graphElementRectangle.Left - elementSize.Width - tipInterval;
                location.Y = graphElementRectangle.Top;
            }
            else
            {
                location.X = graphElementRectangle.Right + tipInterval;
                location.Y = graphElementRectangle.Top;
            }

            textLocation.X = location.X + textInterval;
            textLocation.Y = location.Y + textInterval;
        }

        /// <summary>
        /// 判断某个点是否在该图元的区域内
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>该点是否在图元的区域内</returns>
        public override bool IsInRegion(Point p)
        {
            return false;
        }

        /// <summary>
        /// 判断图元是否在某个矩形内
        /// </summary>
        /// <param name="r">当前矩形</param>
        /// <returns>图元是否在当前矩形内</returns>
        public override bool IsInRegion(Rectangle r)
        {
            return false;
        }

        /// <summary>
        /// 绘制图元自身
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void Paint(Graphics g)
        {
            if (visible)
            {
                // g.ScaleTransform(1 / graphSetting.CanvasScale, 1 / graphSetting.CanvasScale); // 恢复缩放

                g.FillRectangle(graphSetting.TooltipBackgroundBrush, location.X, location.Y, elementSize.Width, elementSize.Height);
                g.DrawRectangle(graphSetting.GraphElementNormalPen, location.X, location.Y, elementSize.Width, elementSize.Height);

                for (int i = 0; i < lineList.Count; i++)
                {
                    string line = lineList[i];
                    TipFormat lineFormat = formatList[i];                    

                    switch (lineFormat)
                    {
                        case TipFormat.Normal:
                            {
                                g.DrawString(line, graphSetting.GraphElementTextFont, graphSetting.LineTextBrush, textLocation.X, textLocation.Y + i * charWidth);
                                break;
                            }
                        case TipFormat.Bold:
                            {
                                g.DrawString(line, graphSetting.GraphElementTextBoldFont, graphSetting.LineTextBrush, textLocation.X, textLocation.Y + i * charWidth);
                                break;
                            }
                        case TipFormat.UnderLine:
                            {
                                g.DrawString(line, graphSetting.GraphElementTextUnderLineFont, graphSetting.LineTextBrush, textLocation.X, textLocation.Y + i * charWidth);
                                break;
                            }
                        case TipFormat.Split:
                            {
                                g.DrawLine(graphSetting.GraphElementNormalPen, location.X, textLocation.Y + i * charWidth, location.X + elementSize.Width, textLocation.Y + i * charWidth);
                                break;
                            }
                    }
                }                                
            }            
        }

        /// <summary>
        /// 绘制图元的缩略效果
        /// </summary>
        /// <param name="g">图形对象</param>
        public override void AbbreviativePaint(Graphics g)
        {

        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="s">移动的相对位置</param>
        public override void Move(Size s)
        {

        }

        /// <summary>
        /// 刷新图元绘制参数
        /// </summary>
        public override void Refresh()
        {

        }

        /// <summary>
        /// 释放图元的绘图资源
        /// </summary>
        public override void Release()
        {

        }
    }
}
