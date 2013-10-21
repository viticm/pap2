using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlowChartEditor
{
    [Serializable]
    public class Background
    {
        private Color backgroundColor = Color.White; // 背景颜色
        private Size gridSize = new Size(25, 25); // 标记格大小
        private Size canvasSize = new Size(1200, 1600); // 画布大小                  
        private Size baseSize = new Size(1200, 1600); // 绘图板的基础大小
        private Size minSize = new Size(1200, 1600); // 绘图板的最小大小        

        [NonSerialized]
        private bool debugMode = false; // 是否在调试模式                

        /// <summary>
        /// 是否在调试模式
        /// </summary>
        [Browsable(false)]
        public bool DebugMode
        {
            set
            {
                debugMode = value;
            }

            get
            {
                return debugMode;
            }
        }

        /// <summary>
        /// 绘图板的最小大小
        /// </summary>
        [Browsable(false)]
        public Size MinSize
        {
            set
            {
                minSize = value;
            }

            get
            {
                return minSize;
            }
        }
       
        /// <summary>
        /// 绘图板的基础大小
        /// </summary>
        [Browsable(false)]
        public Size BaseSize
        {
            set
            {
                baseSize = value;
            }

            get
            {
                return baseSize;
            }
        }        

        /// <summary>
        /// 背景颜色属性
        /// </summary>
        [Browsable(false)]
        public Color BackgroundColor
        {
            set
            {
                backgroundColor = value;
            }

            get
            {
                return backgroundColor;
            }
        }

        /// <summary>
        /// 标记格大小属性
        /// </summary>
        [Browsable(false)]
        public Size GridSize
        {
            set
            {
                gridSize = value;
            }

            get
            {
                return gridSize;
            }
        }

        /// <summary>
        /// 画布大小属性
        /// </summary>
        [Browsable(false)]
        public Size CanvasSize
        {
            set
            {
                canvasSize = value;
            }

            get
            {
                return canvasSize;
            }
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Refresh()
        {
            minSize = new Size(1200, 1600);
        }
    }
}
