using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace LogicalEditor
{
    public partial class Preview : Form
    {
        private Image m_image = null;
        public Canvas m_panel1 = null;
        private int old_mouse_x = -1, old_mouse_y = -1; // 用于算鼠标 delta
        private Form1 m_parentFrm = null;
        private bool m_ShiftPress = false;
        private ObjDrawInfo m_mouseUnderODI = null;
        private bool m_mouseUnderIsDir = false;
        private ObjDrawInfo m_mouseGrabODI = null;
        private bool m_mouseGrabIsDir = false;
        private float mouse_map_x = -1.0f, mouse_map_y = -1.0f; // 当前鼠标处在 map 的坐标 单位厘米
        private int m_startDragDirX = -1, m_startDragDirY = -1; // 鼠标在dirend处按下左键时的位置。
        private bool m_mmbPress = false;
        public bool m_ShowDir = false;
        public bool m_ShowObjName = false;


        public Preview(Form1 parentFrm)
        {
            InitializeComponent();
            m_panel1 = panel1;
            m_parentFrm = parentFrm;
        }
        public void LoadPicture(string strFile)
        {
            Paloma.TargaImage tgaImage = new Paloma.TargaImage(strFile);
            Image image = tgaImage.Image;
            panel1.PaintPicture(image);
            panel1.ClearPaint();
            m_image = image;
            panel1_SizeChanged(null, null);
        }

        private void panel1_SizeChanged(object sender, EventArgs e)
        {
            int newWidth = panel1.Width;
            int newHeight = panel1.Height;
            panel1.ClearPaint();
            panel1.ResetSize(newWidth, newHeight);
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            if (e.Delta > 0)
                this.m_panel1.scale *= 0.9f;
            else
                this.m_panel1.scale /= 0.9f;

            if (this.m_panel1.scale > 1.0f)
                this.m_panel1.scale = 1.0f;
            //if (this.m_panel1.scale < 0.1f)
            //    this.m_panel1.scale = 0.1f;

            ShowScaleInfo();
            ShowMouseObjInfo();
            this.Refresh();
            base.OnMouseWheel(e);
        }

        private void ShowScaleInfo()
        {
            double dScale = (double)1 / this.m_panel1.scale;
            string strScale = string.Empty;
            for (int i = 0; i < 28 - this.statusLabel2.Text.Trim().Length; i++)
                strScale += " ";
            strScale = string.Format("{0}缩放: {1}", strScale, dScale >= 10 ? dScale.ToString("00.00") : dScale.ToString("0.00"));
            this.statusLabel4.Text = strScale;
        }

        private void ShowMouseObjInfo()
        {
            string strPre = string.Empty;
            for (int i = 0; i < 12 - this.statusLabel4.Text.Trim().Length; i++)
                strPre += " ";
            if (m_mouseGrabODI != null)
            {
                LogicObj curObj = m_mouseGrabODI.lObj;
                //string strODI = string.Format("{0}对象: {1}[{2}]", strPre, curObj.values[0], curObj.inDex);
                string strODI = string.Format("{0}对象: {1}[{2}]", strPre, curObj.tyPe.ToLower().EndsWith("point") ? "Point" : (curObj.tyPe.ToLower().EndsWith("vertex") ? "Vertex" : curObj.values[0]), curObj.inDex);
                this.statusLabel3.Text = strODI;
            }
            else if (m_mouseUnderODI != null)
            {
                LogicObj curObj = m_mouseUnderODI.lObj;
                //string strODI = string.Format("{0}对象: {1}[{2}]", strPre, curObj.values[0], curObj.inDex);
                string strODI = string.Format("{0}对象: {1}[{2}]", strPre, curObj.tyPe.ToLower().EndsWith("point") ? "Point" : (curObj.tyPe.ToLower().EndsWith("vertex") ? "Vertex" : curObj.values[0]), curObj.inDex);
                this.statusLabel3.Text = strODI;
            }
            else 
            {
                this.statusLabel3.Text = strPre + "对象: N/A";
            }
        }

        private void ShowMousePosInfo(int mouse_x, int mouse_y)
        {
            // 鼠标位置
            string strMousePos = string.Format("鼠标 ({0}, {1})", mouse_x, mouse_y);
            this.statusLabel1.Text = strMousePos;

            // 地图位置
            if (this.m_panel1.image != null)
            {
                float screenscalex = (float)mouse_x / this.m_panel1.CanvasWidth;
                float startx = (1.0f - this.m_panel1.scale) / 2 * this.m_panel1.image.Width + this.m_panel1.start_adjust_x;
                float displayx = startx + screenscalex * this.m_panel1.image.Width * this.m_panel1.scale;
                displayx = displayx * m_parentFrm.curMapLogicWidthTotal / this.m_panel1.image.Width + m_parentFrm.curLogicStartX;
                double d = Convert.ToDouble(displayx);
                d = Math.Round(d, 2);
                mouse_map_x = (float)d;

                float screenscaley = (float)mouse_y / this.m_panel1.CanvasHeight;
                float starty = (1.0f - this.m_panel1.scale) / 2 * this.m_panel1.image.Height + this.m_panel1.start_adjust_y;
                float displayy = starty + screenscaley * this.m_panel1.image.Height * this.m_panel1.scale;
                displayy = m_parentFrm.curMapLogicHeightTotal - displayy * m_parentFrm.curMapLogicHeightTotal / this.m_panel1.image.Height + m_parentFrm.curLogicStartY;
                d = Convert.ToDouble(displayy);
                d = Math.Round(d, 2);
                mouse_map_y = (float)d;
            }

            string strMapPos = string.Empty;
            for (int i = 0; i < 17 - this.statusLabel1.Text.Length; i++)
                strMapPos += " ";
            strMapPos = string.Format("{0}地图 ({1}, {2})", strMapPos, mouse_map_x, mouse_map_y);
            this.statusLabel2.Text = strMapPos;

            // 比例尺信息
            ShowScaleInfo();
            // 当前鼠标下的 ODI 信息
            ShowMouseObjInfo();
        }

        private Point ODI2ClientPoint(ObjDrawInfo odi)
        {
            float startscalex = ((1.0f - this.m_panel1.scale) * this.m_panel1.image.Width / 2 + this.m_panel1.start_adjust_x) / this.m_panel1.image.Width;
            float startscaley = ((1.0f - this.m_panel1.scale) * this.m_panel1.image.Height / 2 + this.m_panel1.start_adjust_y) / this.m_panel1.image.Height;

            int mousex = Convert.ToInt32((odi.pfPos.X - startscalex) * this.m_panel1.canvasWidth / this.m_panel1.scale);
            int mousey = Convert.ToInt32((odi.pfPos.Y - startscaley) * this.m_panel1.canvasHeight / this.m_panel1.scale);

            return new Point(mousex, mousey);
        }

        private ObjDrawInfo GetMouseODI(int mousex, int mousey)
        {
            if (this.m_panel1.image == null) return null;

            Point curMousePoint = new Point(mousex, mousey);
            ObjDrawInfo odiRet = null;

            foreach (DictionaryEntry entry in this.m_panel1.m_ODIs)
            {
                ObjDrawInfo odi = entry.Value as ObjDrawInfo;
                Point pointObj = ODI2ClientPoint(odi);

                if (Helper.PointsAreNear(pointObj, curMousePoint))
                {
                    odiRet = odi;
                    m_mouseUnderIsDir = false;
                    break;
                }

                if (m_ShowDir)
                {
                    int nDir = (int)(odi.nDir);
                    if (nDir != -1)
                    {
                        PointF adjustment = m_panel1.GetDirEndAdjustment(nDir);
                        Point pointDirEnd = new Point(Convert.ToInt32(pointObj.X + adjustment.X), Convert.ToInt32(pointObj.Y + adjustment.Y));
                        if (Helper.PointsAreNear(pointDirEnd, curMousePoint))
                        {
                            odiRet = odi;
                            m_mouseUnderIsDir = true;
                        }
                    }
                }
            }

            return odiRet;   
        }

        private void AdjustObjLogicalPos(LogicObj obj, PointF pfClientRateAdjust)
        {
            if (m_parentFrm.htObjScenePosIndices.ContainsKey(obj.tyPe))
            {
                double x = 0.0, y = 0.0;
                float height = 0.0f;
                // 场景pos
                if (m_parentFrm.htObjScenePosIndices[obj.tyPe] != null)
                {
                    int posIndex = Convert.ToInt32(m_parentFrm.htObjScenePosIndices[obj.tyPe]);
                    string vPosition = obj.values[posIndex];
                    string[] fields = vPosition.Split(new char[] { ',' });
                    x = Convert.ToDouble(fields[0]);
                    y = Convert.ToDouble(fields[2]);
                    x += pfClientRateAdjust.X * m_parentFrm.curMapLogicWidthTotal * m_panel1.scale;
                    y -= pfClientRateAdjust.Y * m_parentFrm.curMapLogicHeightTotal * m_panel1.scale;
                    x = Math.Round(x, 2);
                    y = Math.Round(y, 2);
                    height = 0.0f;
                    Program.Engine.GetHeight((int)m_parentFrm.htScenePtr[m_parentFrm.m_curMapName], (float)x, (float)y, out height);
                    vPosition = string.Format("{0},{1},{2}", x, height, y);
                    obj.values[posIndex] = vPosition;
                    obj.ForcePreviewChange(true, posIndex);
                }
                
                // 逻辑xyz
                if (m_parentFrm.htObjLogicNXIndices[obj.tyPe] != null)
                {
                    float logic_x_start = (float)(Convert.ToDouble((m_parentFrm.maps[m_parentFrm.m_curMapName] as LO_Map).MAIN.values[11]));
                    float logic_z_start = (float)(Convert.ToDouble((m_parentFrm.maps[m_parentFrm.m_curMapName] as LO_Map).MAIN.values[12]));
                    int lX = 0, lY = 0, lZ = 0;
                    Helper.SceneToLogical(ref lX, ref lY, ref lZ, (float)x, height, (float)y, 0.0f, logic_x_start, logic_z_start);
                    int nXIndex = Convert.ToInt32(m_parentFrm.htObjLogicNXIndices[obj.tyPe]);
                    obj.values[nXIndex] = lX.ToString();
                    obj.values[nXIndex + 1] = lY.ToString();
                    obj.values[nXIndex + 2] = lZ.ToString();

                    obj.ForcePreviewChange(true, nXIndex);
                    obj.ForcePreviewChange(true, nXIndex + 1);
                    obj.ForcePreviewChange(true, nXIndex + 2);
                }

                obj.ForceInhandChange(true);
                this.m_parentFrm.ForceMapChanged();
                this.m_parentFrm.Pg.Refresh();
            }
        }

        private void AdjustObjLogicalDir(LogicObj obj, int nDir)
        {
            if (m_parentFrm.htObjLogicDirIndices.ContainsKey(obj.tyPe) && m_parentFrm.htObjLogicRotationIndices.ContainsKey(obj.tyPe))
            {
                int dirIndex = Convert.ToInt32(m_parentFrm.htObjLogicDirIndices[obj.tyPe]);
                int oldDir = Convert.ToInt32(obj.values[dirIndex]);
                int newDir = nDir;
                obj.values[dirIndex] = newDir.ToString();
                obj.fields_changed[dirIndex] = true;

                int rotationIndex = Convert.ToInt32(m_parentFrm.htObjLogicRotationIndices[obj.tyPe]);
                float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
                Program.Engine.QuaternionRotationByNDir(ref x, ref y, ref z, ref w, nDir);
                double xx = x, yy = y, zz = z, ww = w;
                xx = Math.Round(xx, 6);
                yy = Math.Round(yy, 6);
                zz = Math.Round(zz, 6);
                ww = Math.Round(ww, 6);
                string newRotation = string.Format("{0},{1},{2},{3}", xx, yy, zz, ww);
                obj.values[rotationIndex] = newRotation;

                obj.ForcePreviewChange(true, rotationIndex);
                obj.ForceInhandChange(true);
                this.m_parentFrm.ForceMapChanged();
                this.m_parentFrm.Pg.Refresh();
            }
        }

        private void panel1_MouseMove(object sender, MouseEventArgs e)
        {
            int deltax = 0;
            int deltay = 0;
            if (e.Button != MouseButtons.None)
            {
                if (old_mouse_x != -1)
                {
                    deltax = e.X - old_mouse_x;
                }
                old_mouse_x = e.X;

                if (old_mouse_y != -1)
                {
                    deltay = e.Y - old_mouse_y;
                }
                old_mouse_y = e.Y;
            }

            bool bNeedRefresh = false;
            if (e.Button == MouseButtons.Middle)
            {
                float deltaxf = deltax / (float)this.m_panel1.CanvasWidth;
                float deltayf = deltay / (float)this.m_panel1.CanvasHeight;

                this.m_panel1.start_adjust_x -= deltaxf * this.m_panel1.image.Width * this.m_panel1.scale;
                this.m_panel1.start_adjust_y -= deltayf * this.m_panel1.image.Height * this.m_panel1.scale;

                bNeedRefresh = true;
            }

            ShowMousePosInfo(e.X, e.Y);

            if (m_mouseGrabODI == null)
            {
                m_mouseUnderODI = GetMouseODI(e.X, e.Y);
                if (m_mouseUnderODI != null)
                    this.m_panel1.Cursor = Cursors.Hand;
                else if (!m_mmbPress)
                    this.m_panel1.Cursor = Cursors.Default;
            }
            else
            {
                if (m_mouseGrabIsDir)
                {
                    int delx = e.X - m_startDragDirX;
                    int dely = m_startDragDirY - e.Y;

                    if (delx == 0)
                    {
                        m_mouseGrabODI.nDir = dely >= 0 ? 64 : 192;
                    }
                    else
                    {
                        if (delx > 0)
                        {
                            if (dely >= 0)
                            {
                                double fdiv = (double)dely / delx;
                                double angel = Math.Atan(fdiv);
                                m_mouseGrabODI.nDir = Convert.ToInt32(angel * 256 / 2 / Math.PI);
                            }
                            else
                            {
                                double fdiv = (double)(-dely) / delx;
                                double angel = Math.PI * 2 - Math.Atan(fdiv);
                                m_mouseGrabODI.nDir = Convert.ToInt32(angel * 256 / 2 / Math.PI);
                            }
                        }
                        else
                        {
                            if (dely >= 0)
                            {
                                double fdiv = (double)dely / (-delx);
                                double angel = Math.PI - Math.Atan(fdiv);
                                m_mouseGrabODI.nDir = Convert.ToInt32(angel * 256 / 2 / Math.PI);
                            }
                            else
                            {
                                double fdiv = (double)(-dely) / (-delx);
                                double angel = Math.PI + Math.Atan(fdiv);
                                m_mouseGrabODI.nDir = Convert.ToInt32(angel * 256 / 2 / Math.PI);
                            }
                        }

                        if (m_mouseGrabODI.nDir >= 256)
                            m_mouseGrabODI.nDir = 0;
                    }

                    bNeedRefresh = true;

                    // 更新内存obj:
                    AdjustObjLogicalDir(m_mouseGrabODI.lObj, m_mouseGrabODI.nDir);
                }
                else
                {
                    float fAdjustX = (float)deltax / this.m_panel1.CanvasWidth;
                    float fAdjustY = (float)deltay / this.m_panel1.CanvasHeight;

                    m_mouseGrabODI.pfPos.X += fAdjustX * this.m_panel1.scale;
                    m_mouseGrabODI.pfPos.Y += fAdjustY * this.m_panel1.scale;
                    bNeedRefresh = true;

                    // 更新内存obj: 加入落在地图上时，刷新此对象的 vPosition, x, y, z等
                    AdjustObjLogicalPos(m_mouseGrabODI.lObj, new PointF(fAdjustX, fAdjustY));
                }
            }

            if (bNeedRefresh)
                this.Refresh();
        }

        private void panel1_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.None)
            {
                this.old_mouse_x = -1;
                this.old_mouse_y = -1;
            }

            if (e.Button == MouseButtons.Middle)
            {
                m_mmbPress = false;
                this.m_panel1.Cursor = Cursors.Default;
            }
            else if (e.Button == MouseButtons.Left)
            {
                m_mouseGrabODI = null;
                m_startDragDirX = -1;
                m_startDragDirY = -1;
            }
        }

        private void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Middle)
            {
                m_mmbPress = true;
                this.m_panel1.Cursor = Cursors.Hand;
            }
            else if (e.Button == MouseButtons.Left)
            {
                m_mouseGrabODI = m_mouseUnderODI;
                m_mouseGrabIsDir = m_mouseUnderIsDir;
                if (m_mouseGrabODI != null && m_mouseGrabIsDir)
                {
                    PointF grabPointF = m_panel1.GetODIClientPos(m_mouseGrabODI);
                    m_startDragDirX = Convert.ToInt32(grabPointF.X);
                    m_startDragDirY = Convert.ToInt32(grabPointF.Y);
                }

                if (this.m_ShiftPress)
                {
                    string strResult = string.Format("{0},{1}", mouse_map_x, mouse_map_y);
                    Clipboard.SetText(strResult);
                }
            }
        }

        private void Preview_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.ShiftKey)
            {
                m_ShiftPress = true;
            }
        }

        private void Preview_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.ShiftKey)
            {
                m_ShiftPress = false;
            }
        }

        private void checkShowDir_CheckedChanged(object sender, DevComponents.DotNetBar.CheckBoxChangeEventArgs e)
        {
            m_ShowDir = (sender as DevComponents.DotNetBar.CheckBoxItem).Checked;
            this.Refresh();
        }

        private void checkShowObjName_CheckedChanged(object sender, DevComponents.DotNetBar.CheckBoxChangeEventArgs e)
        {
            m_ShowObjName = (sender as DevComponents.DotNetBar.CheckBoxItem).Checked;
            this.Refresh();
        }

        private void Preview_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
        }

        private void panel1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left) // 双击切换scale
            {
                this.m_panel1.scale = 1.0f;
                this.m_panel1.start_adjust_x = 0.0f;
                this.m_panel1.start_adjust_y = 0.0f;
                this.Refresh();
            }
        }

    }
}