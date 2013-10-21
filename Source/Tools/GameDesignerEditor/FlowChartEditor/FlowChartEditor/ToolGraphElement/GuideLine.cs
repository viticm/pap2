using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using GraphElementLibrary;

namespace FlowChartEditor
{
    public class GuideLine : GraphElement
    {
        private List<Point[]> guideLineList = new List<Point[]>(); // 调整线链表 
        private GraphSetting graphSetting = GraphSetting.GetGraphSetting();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="location">图元的位置</param>
        public GuideLine(Point location) : base(location)
        {

        }

        /// <summary>
        /// 添加调整线
        /// </summary>
        /// <param name="pointArray">调整线点数组</param>
        public void AddGuideLine(Point[] pointArray)
        {
            guideLineList.Add(pointArray);
        }

        /// <summary>
        /// 清理调整线链表
        /// </summary>        
        public void ClearGuideLineList()
        {
            guideLineList.Clear();
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
            if (visible && graphSetting.CanvasScale == 1.0f)
            {                
                foreach (Point[] points in guideLineList)
                {
                    g.DrawLine(graphSetting.AdjustLinePen, points[0], points[1]);
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
