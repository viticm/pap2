using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;
using System.Runtime.InteropServices;
using System.Diagnostics;
using AtlKG3DEngineProxyLib;


namespace DesignerSceneEditor
{
    public partial class TopViewForm : Form
    {
        [DllImport("user32.dll")]
        public static extern short GetAsyncKeyState(int vKey);

        public int m_OutputWindowID = -1;
        public bool m_bSelecting = false;
        public bool m_bAddPolyVertex = false;
        _AtlVector3 m_CameraPos;
        BaseForm m_BaseForm;
        Point m_MouseLDown;
        Point m_LastMousePos = new Point(0, 0);
        const int SCENESTATE_SELECT = 0;
        const int OUTPUTWND_SCENEEDITOR_TOPVIEW = 3;
        const int VK_CONTROL = 0x11;
        public SceneEditor SceneSceneEditor = null;
        private const int WM_MOUSEWHEEL = 0x020A;
        private const int SCENESTATE_NONE = 1000;
        private const int STATE_SELECTING = 1;
        private const int STATE_ADDPOLYVERTEX = 2;
        private const int STATE_SELECTLOGICREGION = 3;
        private const int STATE_NONE = 1000;
        private int m_editState = STATE_NONE;
        private int m_nRegionXStart = 0;
        private int m_nRegionZStart = 0;
        private int m_nCountRegionX = 0;
        private int m_nCountRegionZ = 0;
        private bool m_bAutoFillParam = false;


        public TopViewForm(BaseForm baseForm)
        {
            InitializeComponent();
            m_BaseForm = baseForm;
            SceneSceneEditor = m_BaseForm.SceneSceneEditor;
        }

        public void Init()
        {
            SceneSceneEditor.AddWindowing("TopView", (int)this.panel1.Handle, OUTPUTWND_SCENEEDITOR_TOPVIEW, ref m_OutputWindowID, 0);
            SceneSceneEditor.OnResizeWindow();
            SceneSceneEditor.UpdateTopPosAsMainPos();
        }

        protected override void WndProc(ref Message msg)
        {
            if (msg.Msg == WM_MOUSEWHEEL)
            {
                int k = 0;
				k |= (int)msg.WParam;
				float S = 1.0F;
				if (k > 0)
					S = 0.85F;
				else
					S = 1.15F;
                if (SceneSceneEditor != null)
				{
                    float fscale = SceneSceneEditor.GetCameraTopScale() * S;
                    fscale = fscale > 0.01f ? fscale : 0.01f;
                    SceneSceneEditor.SetCameraTopScale(fscale);
				}
            }

            base.WndProc(ref msg);
        }

        public void UnInit()
        {
            SceneSceneEditor.RemoveWindowing(m_OutputWindowID);
            m_OutputWindowID = -1;
        }

        public void SetMyCurrentWindow()
        {
            if (SceneSceneEditor != null)
                SceneSceneEditor.SetCurrentOutputWindow(m_OutputWindowID);
        }

        private void HideSelf()
        {
            this.Hide();
            this.UnInit();
            m_BaseForm.m_bTopView = false;
            
        }
        private void TopViewForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.T)
            {
                HideSelf();
            }
        }
        private void TopViewForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            HideSelf();
            e.Cancel = true;
        }

        private void panel1_Resize(object sender, EventArgs e)
        {
            SceneSceneEditor.OnResizeWindow();
        }

        private void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            SetMyCurrentWindow();

            if (e.Button == MouseButtons.Left)
            {
                switch (m_editState)
                {
                    case STATE_SELECTING:
                        {
                            //_AtlVector3 vPos = new _AtlVector3();
                            //vPos.x = e.Location.X;
                            //vPos.y = 0;
                            //vPos.z = e.Location.Y;
                            //MousePosToScene(ref vPos);
                            //SceneSceneEditor.AddSelectSceneBlockPoint
                            break;
                        }
                    case STATE_ADDPOLYVERTEX:
                        {
                            //float X, Z;
                            //D3DXVECTOR3 vPos;
                            //MousePosToScene(X, Z, point);
                            //vPos.x = X; vPos.z = Z; vPos.y = 0;
                            //m_lpSceneEditor->AddPolyVetexInTopView(vPos);
                            break;
                        }
                    case STATE_SELECTLOGICREGION:
                        {
                            _AtlVector3 vPos = new _AtlVector3();
                            vPos.x = e.Location.X;
                            vPos.y = 0;
                            vPos.z = e.Location.Y;
                            MousePosToScene(ref vPos);
                            int nX_Start, nZ_Start, nX_Count = 0, nZ_Count = 0;
                            nX_Start = (int)(vPos.x / 3200.0f) > 0 ? (int)(vPos.x / 3200.0f) : 0;
                            nZ_Start = (int)(vPos.z / 3200.0f) > 0 ? (int)(vPos.z / 3200.0f) : 0;
                            SceneSceneEditor.SetAppointLogicalRegion(nX_Start, nZ_Start, nX_Count, nZ_Count);
                            break;
                        }
                    default:
                        break;
                }
            }

            else if (e.Button == MouseButtons.Middle)
            {
                m_MouseLDown = e.Location;
                SceneSceneEditor.GetCameraTopPos(ref m_CameraPos);
            }

            else if (e.Button == MouseButtons.Right)
            {
                //if (m_bAddPolyVertex)
                //{
                //    m_pEditor.EndAddPolyVertex(0);
                //}
            }
        }

        private void panel1_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                switch (m_editState)
                {
                    case STATE_SELECTING:
                        break;
                    case STATE_ADDPOLYVERTEX:
                        break;
                    case STATE_SELECTLOGICREGION:
                        {
                            _AtlVector3 vPos = new _AtlVector3();
                            vPos.x = e.Location.X;
                            vPos.y = 0;
                            vPos.z = e.Location.Y;
                            MousePosToScene(ref vPos);

                            int nX_End, nZ_End;
                            int nX = 0, nZ = 0;
                            SceneSceneEditor.GetAppointLogicalRegion(ref m_nRegionXStart, ref m_nRegionZStart, ref m_nCountRegionX, ref m_nCountRegionZ);
                            nX = (int)(vPos.x / 3200.0f);
                            nZ = (int)(vPos.z / 3200.0f);
                            nX_End = m_nRegionXStart > nX ? m_nRegionXStart : nX;
                            nZ_End = m_nRegionZStart > nZ ? m_nRegionZStart : nZ;
                            m_nRegionXStart = m_nRegionXStart < nX ? m_nRegionXStart : nX;
                            m_nRegionZStart = m_nRegionZStart < nZ ? m_nRegionZStart : nZ;
                            m_nCountRegionX = nX_End - m_nRegionXStart + 1;
                            m_nCountRegionZ = nZ_End - m_nRegionZStart + 1;
                            SceneSceneEditor.SetAppointLogicalRegion(m_nRegionXStart, m_nRegionZStart, m_nCountRegionX, m_nCountRegionZ);
                            
                            // update UI
                            m_bAutoFillParam = true;
                            this.textXStart.Text = m_nRegionXStart.ToString();
                            this.textZStart.Text = m_nRegionZStart.ToString();
                            this.textXCount.Text = m_nCountRegionX.ToString();
                            this.textZCount.Text = m_nCountRegionZ.ToString();
                            m_bAutoFillParam = false;
                            break;
                        }
                    default:
                        break;
                }
            }
            else if (e.Button == MouseButtons.Middle)
            {
            }
            else if (e.Button == MouseButtons.Right)
            {
            }
        }

        private void panel1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            SceneSceneEditor.SetNPCPosition();
        }

        private void panel1_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                if (e.Location.Y < m_LastMousePos.Y)
                {
                    SceneSceneEditor.SetCameraTopScale(SceneSceneEditor.GetCameraTopScale() * 1.05f);
                }
                else
                {
                    float fScale = SceneSceneEditor.GetCameraTopScale() * 0.95f;
                    if (fScale < 0.01f)
                        fScale = 0.01f;
                    SceneSceneEditor.SetCameraTopScale(fScale);
                }
            }

            else if (e.Button == MouseButtons.Middle)
            {
                SceneSceneEditor.SetCameraTopPos(m_CameraPos.x - (e.Location.X - m_MouseLDown.X) * SceneSceneEditor.GetCameraTopScale(),
                    m_CameraPos.z + (e.Location.Y - m_MouseLDown.Y) * SceneSceneEditor.GetCameraTopScale());
            }

            else if (e.Button == MouseButtons.Left)
            {
                //if (m_bSelecting)
                //{
                //    _AtlVector3 vPos = new _AtlVector3();
                //    vPos.x = e.Location.X;
                //    vPos.y = 0;
                //    vPos.z = e.Location.Y;
                //    Z(ref vPos);

                //    //m_pEditor.AddSelectRegionPoint(1, vPos);
                //       if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
                //       {
                //           m_pEditor.EndSelectRegion(1, 0);
                //       }
                //       else
                //       {
                //           m_pEditor.EndSelectRegion(0, 0);
                //       }
                //}
            }

            m_LastMousePos = e.Location;
        }

        private void buttonX1_Click(object sender, EventArgs e) // 选择逻辑区域
        {
            SceneSceneEditor.SetEditState(SCENESTATE_NONE);
            SceneSceneEditor.EnableShowAppointLogicalRegion(1);
            this.checkShowAppoint.Checked = true;
            m_editState = STATE_SELECTLOGICREGION;
        }

        private void checkShowAppoint_CheckedChanged(object sender, EventArgs e)
        {
            SceneSceneEditor.EnableShowAppointLogicalRegion(this.checkShowAppoint.Checked ? 1 : 0);
        }

        private void AppointParamsChanged(object sender, EventArgs e)
        {
            if (m_bAutoFillParam)
                return;

            try
            {
                m_nRegionXStart = Int32.Parse(this.textXStart.Text);
                m_nRegionZStart = Int32.Parse(this.textZStart.Text);
                m_nCountRegionX = Int32.Parse(this.textXCount.Text);
                m_nCountRegionZ = Int32.Parse(this.textZCount.Text);
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("请重新输入正确的数字。\r\n" + ex.Message, "无效的输入", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.textXStart.Text = this.textZStart.Text = this.textXCount.Text = this.textZCount.Text = "0";
                return;
            }
            SceneSceneEditor.SetAppointLogicalRegion(m_nRegionXStart, m_nRegionZStart, m_nCountRegionX, m_nCountRegionZ);
        }


        private void MousePosToScene(ref _AtlVector3 vPos)
        {           
            float CX = 0.5f * (this.panel1.ClientRectangle.Right + this.panel1.ClientRectangle.Left);
            float CZ = 0.5f * (this.panel1.ClientRectangle.Bottom + this.panel1.ClientRectangle.Top);
            SceneSceneEditor.GetCameraTopPos(ref m_CameraPos);
            vPos.x = m_CameraPos.x + (vPos.x - CX) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale();
            vPos.z = m_CameraPos.z - (vPos.z - CZ) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale();
        }

    }
}