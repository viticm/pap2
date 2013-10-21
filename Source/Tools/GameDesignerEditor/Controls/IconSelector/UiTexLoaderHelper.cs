using System;
using System.Collections.Generic;
using System.Text;
using IconBrowser;
using System.Drawing;

namespace IconSelector
{
    class ParameterCache
    {
        public object[] Parameters;
        public Type[] ParameterTypes;
        public ModePass[] themode;
        public Type Type_Return;
    };
    class UiTexLoaderHelper
    {
        private DllHelper dll;
        private ParameterCache[] m_paraminfo = new ParameterCache[9];
        private Rectangle InitIconRect, IconRect;
        private int IconID;

        public UiTexLoaderHelper()
        {
            dll = new DllHelper();
            dll.LoadDll(AppDomain.CurrentDomain.BaseDirectory + "UiTexTGALoader.dll");
        }
        #region "dll call helper"
        public Rectangle InitRect
        {
            get { return InitIconRect; }
            set { InitIconRect = value; }
        }
        public Rectangle Rect
        {
            get { return IconRect; }
            set { IconRect = value; }
        }
        public int ID
        {
            get { return IconID; }
            set { IconID = value; }
        }
        public bool Render(System.IntPtr hWnd)
        {
            ParameterCache paraminfo = m_paraminfo[0];
            if (m_paraminfo[0] == null)
            {
                paraminfo = m_paraminfo[0] = new ParameterCache();
                paraminfo.Parameters = new object[] { hWnd }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(System.IntPtr) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(bool);
            }
            object o = dll.Invoke("Render", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return Convert.ToBoolean(o);
        }
        public bool Setup(System.IntPtr hWnd, string lpFileName)
        {
            ParameterCache paraminfo = m_paraminfo[1];
            if (m_paraminfo[1] == null)
            {
                paraminfo = m_paraminfo[1] = new ParameterCache();
                paraminfo.Parameters = new object[] { hWnd, lpFileName };
                paraminfo.ParameterTypes = new Type[] { typeof(System.IntPtr), typeof(string) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue, ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(bool);
            }
            else
                paraminfo.Parameters[1] = lpFileName;

            object o = dll.Invoke("Setup", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            bool bRet = Convert.ToBoolean(o);
            return bRet;
        }
        public Rectangle GetCurrenIconRect()
        {
            ParameterCache paraminfo = m_paraminfo[2];
            if (m_paraminfo[2] == null)
            {
                paraminfo = m_paraminfo[2] = new ParameterCache();
                paraminfo.Parameters = new object[] { 0 }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(Rectangle);
            }
            object o = dll.Invoke("GetCurrenIconRect", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return (Rectangle)o;
        }

        public int GetCurrenIconID()
        {
            ParameterCache paraminfo = m_paraminfo[3];
            if (m_paraminfo[3] == null)
            {
                paraminfo = m_paraminfo[3] = new ParameterCache();
                paraminfo.Parameters = new object[] { 0 }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(int);
            }
            return Convert.ToInt32(dll.Invoke("GetCurrenIconID", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return));
        }

        public void Release()
        {
            ParameterCache paraminfo = m_paraminfo[4];
            if (m_paraminfo[4] == null)
            {
                paraminfo = m_paraminfo[4] = new ParameterCache();
                paraminfo.Parameters = new object[] { 0 }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(bool);
            }
            dll.Invoke("Release", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
        }

        public void SetCurrenIconRect(Rectangle rect)
        {
            ParameterCache paraminfo = m_paraminfo[5];
            if (m_paraminfo[5] == null)
            {
                paraminfo = m_paraminfo[5] = new ParameterCache();
                paraminfo.Parameters = new object[] { rect }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(Rectangle) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(void);
            }
            else
                paraminfo.Parameters[0] = rect;

            object o = dll.Invoke("SetOldIconRect", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return;
        }
        public Size GetCurrentImageSize()
        {
            ParameterCache paraminfo = m_paraminfo[6];
            if (m_paraminfo[6] == null)
            {
                paraminfo = m_paraminfo[6] = new ParameterCache();
                paraminfo.Parameters = new object[] { 0 }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(Size);
            }

            object o = dll.Invoke("GetCurrentImageSize", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return (Size)o;
        }
        public void SetRenderFrame(int nFrame)
        {
            ParameterCache paraminfo = m_paraminfo[7];
            if (paraminfo == null)
            {
                paraminfo = m_paraminfo[7] = new ParameterCache();
                paraminfo.Parameters = new object[] { nFrame }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(void);
            }

            object o = dll.Invoke("SetRenderFrame", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return;
        }

        public Size GetCurrentFrameSize()
        {
            ParameterCache paraminfo = m_paraminfo[8];
            if (paraminfo == null)
            {
                paraminfo = m_paraminfo[8] = new ParameterCache();
                paraminfo.Parameters = new object[] { 0 }; // 实参为 0 
                paraminfo.ParameterTypes = new Type[] { typeof(int) }; // 实参类型为 int 
                paraminfo.themode = new ModePass[] { ModePass.ByValue }; // 传送方式为值传             
                paraminfo.Type_Return = typeof(Size);
            }

            object o = dll.Invoke("GetCurrentFrameSize", paraminfo.Parameters, paraminfo.ParameterTypes, paraminfo.themode, paraminfo.Type_Return);
            return (Size)o;
        }

        public void UnInit()
        {
            Release();
            dll.UnLoadDll();
        }
        #endregion
    }
}
