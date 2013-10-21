using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace RecordeOperator
{
    class SliderCtrl : System.Windows.Forms.Panel
    {
        int    m_nCurPos;
        int    m_nMaxPos;
        int    m_nMinPos;
        float  m_fStep;
        Font   m_font;
        bool   m_bPaintNow;

        Rectangle m_rcChannel;
        Rectangle m_rcSlider;

        ChildForm m_ChildForm;
        Dictionary<int, int> m_Dictionary = new Dictionary<int,int>();
        int m_Length;
        List<List<int>> m_ModelList = new List<List<int>>();
        
        public SliderCtrl()
        {
        }

        public void Init()
        {
            m_nMinPos = 0;
            m_nMaxPos = 0;
            m_nCurPos = 0;
            m_fStep   = 0.0f;
            m_font    = new Font("宋体", 10, System.Drawing.FontStyle.Regular);

            m_bPaintNow = false;
            m_Dictionary.Clear();
            m_Length = 0;

            this.MouseDown += new MouseEventHandler(Slider_MouseDown);
            this.MouseMove += new MouseEventHandler(Slider_MouseMove);
            this.MouseUp   += new MouseEventHandler(Slider_MouseUp);
        }

        public void SetChildForm(ChildForm Child)
        {
            m_ChildForm = Child;
        }

        public bool SetRange(int nMin, int nMax)
        {
            bool bResult = false;
            if (nMax >= nMin)
            {
                m_nMaxPos = nMax;
                m_nMinPos = nMin;
                if (m_nCurPos < nMin) m_nCurPos = nMin;
                if (m_nCurPos > nMax) m_nCurPos = nMax;
                bResult = true;
            }
            return bResult;
        }

        public int GetPos()
        {
            return m_nCurPos;
        }

        public bool SetPos(int nPos)
        {
            bool bResult = false;
            if (nPos >= m_nMinPos && nPos <= m_nMaxPos)
            {
                m_nCurPos = nPos;
                bResult = true;
            }
            return bResult;
        }

        public void ListClear()
        {
            m_Length = 0;
            m_Dictionary.Clear();
            for (int i = 0; i < m_ModelList.Count; i++)
            {
                m_ModelList[i].Clear();
            }
            m_ModelList.Clear();
        }

        public void SetModelTime(int nTime, int nInfo)
        {
            if (nTime < 0)
                return;
            if (!m_Dictionary.ContainsKey(nTime))
            {
                m_Dictionary.Add(nTime, m_Length);
                List<int> ListInfo = new List<int>();
                ListInfo.Add(nInfo);
                m_ModelList.Add(ListInfo);
                m_Length++;
            }
            else
            {
                int nID = m_Dictionary[nTime];
                m_ModelList[nID].Add(nInfo);
            }
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            Size size = m_ChildForm.GetPanel3Size();
            Graphics graphics = pe.Graphics;
            Rectangle rectClient = new Rectangle(0, 0, size.Width, size.Height);
            int i = 0;

            // 设置字体居中显示
            string strInfo;
            StringFormat strFormat = new StringFormat();
            strFormat.Alignment = StringAlignment.Center;
            strFormat.LineAlignment = StringAlignment.Center;

            graphics.FillRectangle(new SolidBrush(Color.FromArgb(197, 197, 197)), rectClient);

            // channel
            m_rcChannel.X      = rectClient.X + 50;                      // left
            m_rcChannel.Width  = rectClient.Width - 100;   // right = rectClient.X + rectClient.Width
            m_rcChannel.Y      = rectClient.Y;                           // top 
            m_rcChannel.Height = 20;                                     // bottom = rectClient.Y + rectClient.Height
            Rectangle rectChannel = m_rcChannel;
            rectChannel.X     = rectClient.X;
            rectChannel.Width = rectClient.Width;
            graphics.FillRectangle(new SolidBrush(Color.FromArgb(207, 207, 207)), rectChannel);

            // line
            Pen pen = new Pen(Color.FromArgb(100, 100, 100), 1);
            pe.Graphics.DrawLine(pen, rectClient.X, m_rcChannel.Height, rectClient.Width, m_rcChannel.Height);

            // rect
            graphics.DrawLine(pen, rectClient.Left, rectClient.Top, rectClient.Left, rectClient.Bottom - 1);
            graphics.DrawLine(pen, rectClient.Left, rectClient.Bottom - 1, rectClient.Right - 1, rectClient.Bottom - 1);
            graphics.DrawLine(pen, rectClient.Left, rectClient.Top, rectClient.Right - 1, rectClient.Top);
            graphics.DrawLine(pen, rectClient.Right - 1, rectClient.Top, rectClient.Right - 1, rectClient.Bottom - 1);

            int nCnt = m_nMaxPos - m_nMinPos;
            if (nCnt > 0)
                m_fStep = (float)m_rcChannel.Width / nCnt;
            else
                m_fStep = 0;
            
            // point
            Rectangle rectPoint = new Rectangle();
            rectPoint.X      = (int)(m_rcChannel.X + m_nCurPos * m_fStep - 3);
            rectPoint.Width  = 6;
            rectPoint.Y      = (int)(m_rcChannel.Height);
            rectPoint.Height = 25;
            graphics.FillRectangle(new SolidBrush(Color.FromArgb(142, 173, 195)), rectPoint);
            DrawFrame(graphics, rectPoint, Color.FromArgb(100, 100, 100));

            m_rcSlider.X      = (int)(m_rcChannel.Left + (int)(m_nCurPos * m_fStep - 50));
            m_rcSlider.Width  = 100;
            m_rcSlider.Y      = m_rcChannel.Top;
            m_rcSlider.Height = m_rcChannel.Bottom - m_rcChannel.Top;

            for (i = 0; i <= nCnt; i++)
            {
                int nIndex  = (int)(m_rcChannel.Left + i * m_fStep);
                int nLength = 0;
                if (i % 10 != 0)
                {
                    nLength = 8;
                }
                else
                {
                    nLength = 18;
                    Rectangle rectText = new Rectangle();
                    rectText.X      = nIndex - 50;
                    rectText.Y      = m_rcChannel.Bottom + 15;
                    rectText.Width  = 100;
                    rectText.Height = 17;
                    strInfo  = i.ToString();
                    graphics.DrawString(strInfo, m_font, Brushes.Black, rectText, strFormat);
                }
                graphics.DrawLine(pen, nIndex, m_rcChannel.Bottom, nIndex, m_rcChannel.Bottom + nLength);
            }

            // slider
            Rectangle rc = m_rcSlider;
            rc.Width  -= 2;
            rc.Height -= 2;
            graphics.FillRectangle(new SolidBrush(Color.FromArgb(100, 100, 100)), m_rcSlider);
            graphics.FillRectangle(new SolidBrush(Color.FromArgb(238, 237, 230)), rc);

            strInfo = m_nCurPos.ToString();
            graphics.DrawString(strInfo, m_font, Brushes.Black, rc, strFormat);

            foreach(int nTime in m_Dictionary.Keys)
            {
                int n = (int)(m_rcChannel.Left + nTime * m_fStep);
                rc.X = n - 2;
                rc.Y = m_rcChannel.Bottom;
                rc.Width = 4;
                rc.Height = 10;
                graphics.FillRectangle(new SolidBrush(Color.FromArgb(0, 0, 255)), rc);
            }

            base.OnPaint(pe);
        }

        public void DrawFrame(Graphics graphics, Rectangle rect, Color color)
        {
            Pen pen = new Pen(color, 1);
            graphics.DrawLine(pen, rect.Left, rect.Top, rect.Left, rect.Bottom);
            graphics.DrawLine(pen, rect.Left, rect.Top, rect.Right, rect.Top);
            graphics.DrawLine(pen, rect.Left, rect.Bottom, rect.Right, rect.Bottom);
            graphics.DrawLine(pen, rect.Right, rect.Top, rect.Right, rect.Bottom);
        }

        public bool PointInRect(Point p, Rectangle rect)
        {
            if (p.X < rect.Left || p.X > rect.Right || p.Y < rect.Top || p.Y > rect.Bottom)
                return false;
            return true;
        }

        public void Slider_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (PointInRect(e.Location, m_rcSlider))
                {
                    m_bPaintNow = true;
                }
                else if (e.Location.Y >= m_rcChannel.Bottom && e.Location.Y <= m_rcChannel.Bottom + 10)
                {
                    int nIndex = e.Location.X - m_rcChannel.Left;
                    int nTime = (int)((nIndex + m_fStep * 0.5) / m_fStep);
                    int nTimeIndex = (int)(nTime * m_fStep);
                    if (nIndex >= nTimeIndex - 2 && nIndex <= nTimeIndex + 2)
                    {
                        if (m_Dictionary.ContainsKey(nTime))
                        {
                            int nID = m_Dictionary[nTime];
                            m_ChildForm.GetInfo(m_ModelList[nID]);
                        }
                    }
                }
            }
        }

        public void Slider_MouseUp(object sender, MouseEventArgs e)
        {
            m_bPaintNow = false;
        }

        public void Slider_MouseMove(object sender, MouseEventArgs e)
        {
            int playState = m_ChildForm.GetPlayState();
            if (playState == 0)
                return;
            int nIndex = 0;
            nIndex = e.Location.X - m_rcChannel.Left;
            int nTime = (int)((nIndex + m_fStep * 0.5) / m_fStep);
            if (nTime != m_nCurPos && m_bPaintNow)
            {
                SetPos(nTime);
                this.Invalidate();
                this.Update();
            }
        }
    }
}
