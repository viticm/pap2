using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using System.Data;
using System.Drawing.Drawing2D;

namespace VarManager
{
    class Canvas:Panel
    {
        private DataTable table; // 变量数据表
        private Font textFont = new Font("Sim Sun", 10, FontStyle.Bold);
        private Pen borderPen = new Pen(Color.Blue);
        private Brush textBrush = Brushes.Black;
        private Brush backGroundBrush = Brushes.Green;
        private Brush varBrush = Brushes.Red;
        private Hashtable varTable = new Hashtable();
        private List<object[]> dataList = new List<object[]>();
        private bool initFinished = false;
        private int maxStringLength = 0;

        /// <summary>
        /// 构造函数
        /// </summary>
        public Canvas()
        {
            // 设置双缓冲等绘制参数
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
        }

        /// <summary>
        /// 测算字符串的长度
        /// </summary>
        /// <param name="text">当前字符串</param>
        /// <param name="font">文本字体</param>
        /// <returns>字符串的长度</returns>
        private int MeasureStringLength(string text, Font font)
        {
            Bitmap bmp = new Bitmap(1, 1);
            Graphics g = Graphics.FromImage(bmp);
            SizeF textSize = g.MeasureString(text, font);
            float textWidth = textSize.Width;
            int stringLength = (int)textWidth;
            bmp.Dispose();

            return stringLength;
        }

        /// <summary>
        /// 初始化参数
        /// </summary>
        /// <param name="table">数据表</param>
        public void Init(DataTable table)
        {
            this.table = table;
            List<string> sceneList = new List<string>();
            string varName;
            int stringLength;
            object[] args;

            foreach(DataRow row in table.Rows)
            {
                string bindType = row["BindType"].ToString();
                switch(bindType)
                {
                    case "场景":
                        {
                            string sceneName = row["BindObject"].ToString();
                            if(!sceneList.Contains(sceneName))
                            {
                                sceneList.Add(sceneName);
                            }
                            
                            break;
                        }
                    case "玩家":
                        {
                            break;
                        }
                }
            }

            int index = 0;
            Point point;
            Size size;

            foreach(string s in sceneList)
            {
                varName = string.Format("场景变量 [{0}]", s);
                stringLength = MeasureStringLength(varName, textFont);
                if (stringLength > maxStringLength)
                {
                    maxStringLength = stringLength;
                } 
                point = new Point(5, index * 50 + 5);
                size = new Size(5120, 30);
                args = new object[3];
                args[0] = varName;
                args[1] = point;
                args[2] = size;
                dataList.Add(args);
                varTable[s] = point;
                index++;
            }

            varName = "玩家变量";
            stringLength = MeasureStringLength(varName, textFont);
            if (stringLength > maxStringLength)
            {
                maxStringLength = stringLength;
            }
            point = new Point(5, index * 50 + 5);
            size = new Size(640, 30);
            args = new object[3];
            args[0] = varName;
            args[1] = point;
            args[2] = size;
            dataList.Add(args);
            varTable[varName] = point;
            index++;            

            this.AutoScrollMinSize = new Size(10 + maxStringLength + 5120 + 50, index * 50);

            initFinished = true;
        }

        /// <summary>
        /// 绘制空间数据
        /// </summary>
        private void PaintSpaceData(Graphics g)
        {
            Point location = Point.Empty;
            Size elementSize = Size.Empty;
            Rectangle rectangle = Rectangle.Empty;

            foreach(DataRow row in table.Rows)
            {
                string bindType = row["BindType"].ToString();
                int startIndex = int.Parse(row["StartIndex"].ToString());
                int length = int.Parse(row["Length"].ToString());

                switch(bindType)
                {
                    case "场景":
                        {
                            string bindObject = row["BindObject"].ToString();                            
                            Point point = (Point)varTable[bindObject];
                            location.X = 10 + maxStringLength + startIndex * 5;
                            location.Y = point.Y;
                            elementSize.Width = length * 5;
                            elementSize.Height = 30;
                            rectangle.Location = location;
                            rectangle.Size = elementSize;
                            g.FillRectangle(varBrush, rectangle);

                            break;
                        }
                    case "玩家":
                        {
                            Point point = (Point)varTable["玩家变量"];
                            location.X = 10 + maxStringLength + startIndex * 5;
                            location.Y = point.Y;
                            elementSize.Width = length * 5;
                            elementSize.Height = 30;
                            rectangle.Location = location;
                            rectangle.Size = elementSize;
                            g.FillRectangle(varBrush, rectangle);

                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 绘制绘图板
        /// </summary>
        /// <param name="e">绘制事件参数</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            // 获取绘图对象
            Graphics g = e.Graphics;

            // 设置绘图参数
            g.SmoothingMode = SmoothingMode.HighQuality;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);   
            
            if(initFinished) // 绘制变量空间数据
            {
                PaintSpaceData(g);
            }
        }

        /// <summary>
        /// 绘制背景
        /// </summary>
        /// <param name="e">绘制事件参数</param>
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            base.OnPaintBackground(e);

            // 获取绘图对象
            Graphics g = e.Graphics;

            // 设置绘图参数
            g.SmoothingMode = SmoothingMode.HighQuality;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.TranslateTransform(this.AutoScrollPosition.X, this.AutoScrollPosition.Y);
            
            if(initFinished)
            {
                g.Clear(Color.White);
                foreach(object[] args in dataList)
                {
                    string varName = args[0] as string;
                    Point point = (Point)args[1];
                    Size size = (Size)args[2];
                    Rectangle rectangle = new Rectangle(new Point(10 + maxStringLength, point.Y), size);
                    g.DrawString(varName, textFont, textBrush, 5, point.Y);
                    g.FillRectangle(backGroundBrush, rectangle);                    
                }
            }
        }
    }
}
