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

        AtlIEKG3DSceneSceneEditor m_pEditor = null;
        AtlIEKG3DScene m_pScene = null;
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


        public TopViewForm(AtlIEKG3DSceneSceneEditor pEditor, BaseForm baseForm)
        {
            InitializeComponent();
            this.GotFocus += new EventHandler(this.TopViewForm_GotFocus);
            m_pEditor = pEditor;
            m_BaseForm = baseForm;
        }

        public void Init()
        {
            string filename = "TopView";
            //Encoding encode = Encoding.GetEncoding(936);
            byte[] byteArr = EditorLayer.Encode936.GetBytes(filename);
            m_pEditor.AddOutputWindow(byteArr, (int)this.Handle, OUTPUTWND_SCENEEDITOR_TOPVIEW, 0, ref m_OutputWindowID);

            m_pEditor.GetScene(ref m_pScene);
            m_pScene.OnResizeWindow(0);

            _AtlVector3 vCameraPos = m_pEditor.GetCameraMainPos();
            m_pEditor.SetCameraTopPos(vCameraPos.x, vCameraPos.z);
        }

        public void UnInit()
        {
            m_pEditor.RemoveOutputWindow(m_OutputWindowID);
            m_pEditor.SetEditState(SCENESTATE_SELECT);
            m_OutputWindowID = -1;
            m_BaseForm.m_bTopView = false;
        }

        public void SetMyCurrentWindow()
        {
            if (m_pScene != null)
                m_pScene.SetCurrentOutputWindow(m_OutputWindowID);
        }

        private void TopViewForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            UnInit();
            this.Hide();
            e.Cancel = true;
        }

        private void TopViewForm_GotFocus(object sender, System.EventArgs e)
        {
            SetMyCurrentWindow();
        }

        private void TopViewForm_Resize(object sender, EventArgs e)
        {
            m_pScene.OnResizeWindow(0);
        }

        private void TopViewForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                _AtlVector3 vPos = new _AtlVector3();
                vPos.x = e.Location.X;
                vPos.y = 0;
                vPos.z = e.Location.Y;
                MousePosToScene(ref vPos);

                if (m_bSelecting)
                {
                    //m_pEditor.AddSelectRegionPoint(0, vPos);
                }
                if (m_bAddPolyVertex)
                {
                    m_pEditor.AddPolyVetexInTopView(ref vPos);
                }
            }

            else if (e.Button == MouseButtons.Middle)
            {
                m_MouseLDown = e.Location;
                m_CameraPos = m_pEditor.GetCameraTopPos();
            }

            else if (e.Button == MouseButtons.Right)
            {
                if (m_bAddPolyVertex)
                {
                    m_pEditor.EndAddPolyVertex(0);
                }
            }
        }

        private void TopViewForm_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                _AtlVector3 vPos = new _AtlVector3();
                vPos.x = e.Location.X;
                vPos.y = 0;
                vPos.z = e.Location.Y;
                MousePosToScene(ref vPos);

                if (m_bSelecting)
                {
                    //m_pEditor.AddSelectRegionPoint(1, vPos);
                    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
                    {
                        //m_pEditor.EndSelectRegion(1, 1);
                    }
                    else
                    {
                        //m_pEditor.EndSelectRegion(0, 1);
                    }
                }

            }
            else if (e.Button == MouseButtons.Middle)
            {
            }
            else if (e.Button == MouseButtons.Right)
            {
            }
        }

        private void TopViewForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                if (e.Location.Y < m_LastMousePos.Y)
                {
                    m_pEditor.SetCameraTopScale(EngineLayer.ATLBase.GetSceneEditorCameraTopScale() * 1.05f);
                }
                else
                {
                    float fScale = EngineLayer.ATLBase.GetSceneEditorCameraTopScale() * 0.95f;
                    if (fScale < 0.01f)
                        fScale = 0.01f;
                    m_pEditor.SetCameraTopScale(fScale);
                }
            }

            else if (e.Button == MouseButtons.Middle)
            {
                m_pEditor.SetCameraTopPos(m_CameraPos.x - (e.Location.X - m_MouseLDown.X) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale(),
                    m_CameraPos.z + (e.Location.Y - m_MouseLDown.Y) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale());
            }

            else if (e.Button == MouseButtons.Left)
            {
                if (m_bSelecting)
                {
                    _AtlVector3 vPos = new _AtlVector3();
                    vPos.x = e.Location.X;
                    vPos.y = 0;
                    vPos.z = e.Location.Y;
                    MousePosToScene(ref vPos);

                    //m_pEditor.AddSelectRegionPoint(1, vPos);
//                     if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
//                     {
//                         m_pEditor.EndSelectRegion(1, 0);
//                     }
//                     else
//                     {
//                         m_pEditor.EndSelectRegion(0, 0);
//                     }
                }
            }

            m_LastMousePos = e.Location;
        }

        private void TopViewForm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            m_pEditor.SetNPCPosition();
        }

        private void MousePosToScene(ref _AtlVector3 vPos)
        {
            float CX = 0.5f * (this.ClientRectangle.Right + this.ClientRectangle.Left);
            float CZ = 0.5f * (this.ClientRectangle.Bottom + this.ClientRectangle.Top);
            m_CameraPos = m_pEditor.GetCameraTopPos();
            vPos.x = m_CameraPos.x + (vPos.x - CX) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale();
            vPos.z = m_CameraPos.z - (vPos.z - CZ) * EngineLayer.ATLBase.GetSceneEditorCameraTopScale();
        }

    }
}