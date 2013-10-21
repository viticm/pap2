using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using GraphElementLibrary;

namespace FlowChartEditor
{
    public class MultiSelectMark : GraphElement
    {
        private GraphSetting graphSetting = GraphSetting.GetGraphSetting();
        private bool showBackground = false; // 是否显示背景

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元位置</param>
        /// <param name="elementSize">图元大小</param>
        public MultiSelectMark(Point location, Size elementSize) : base(location)
        {
            this.elementSize = elementSize;

            // 默认不显示
            visible = false;
        }

        /// <summary>
        /// 是否显示背景
        /// </summary>
        public bool ShowBackground
        {
            set
            {
                showBackground = value;
            }

            get
            {
                return showBackground;
            }
        }

        /// <summary>
        /// 区域矩形
        /// </summary>
        public Rectangle RegionRectangle
        {
            get
            {
                return new Rectangle(location, elementSize);
            }
        }

        /// <summary>
        /// 调整多选标记
        /// </summary>
        /// <param name="location">多选标记的位置</param>
        /// <param name="elementSize">多选标记的大小</param>
        public void Adjust(Point location, Size elementSize)
        {
            this.location = location;
            this.elementSize = elementSize;
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
                // 绘制背景
                if (showBackground)
                {
                    g.FillRectangle(graphSetting.MultiSelectBackgroudBrush, location.X, location.Y, elementSize.Width, elementSize.Height);
                }
                
                // 绘制边框
                g.DrawRectangle(graphSetting.MultiSelectMarkPen, location.X, location.Y, elementSize.Width, elementSize.Height);
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
            this.location += s;
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
