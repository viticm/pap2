//  Copyright (c) 2006, Gustavo Franco
//  Email:  gustavo_franco@hotmail.com
//  All rights reserved.

//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:

//  Redistributions of source code must retain the above copyright notice, 
//  this list of conditions and the following disclaimer. 
//  Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation 
//  and/or other materials provided with the distribution. 

//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE. IT CAN BE DISTRIBUTED FREE OF CHARGE AS LONG AS THIS HEADER 
//  REMAINS UNCHANGED.

using System;
using System.IO;
using System.Data;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Imaging;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using CustomControls.Controls;
using IconBrowser;

namespace CustomControls
{
    //[Author("Franco, Gustavo")]
    public partial class FormOpenFileDialog : OpenFileDialogEx
    {
        /*[DllImport("UiTexTGALoader.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern bool Render1(System.IntPtr hWnd);

        [DllImport("UiTexTGALoader.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int GetCurrenIconID1();

        [DllImport("UiTexTGALoader.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern Rectangle GetCurrenIconRect1();

        [DllImport("UiTexTGALoader.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern bool Setup1(System.IntPtr hWnd, string lpFileName);

        [DllImport("UiTexTGALoader.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void Release1();        
        */
        private string curFilename;
        private Rectangle IconRect, InitIconRect;
        int IconID = -1;
        private DllHelper dll;

        ParameterCache[] m_paraminfo = new ParameterCache[6];

        #region Constructors
        public FormOpenFileDialog()
        {
            InitializeComponent();
            
            dll = new DllHelper();
            dll.LoadDll(AppDomain.CurrentDomain.BaseDirectory + "UiTexTGALoader.dll");
            Render(pbxPreview.Handle);
        }
        #endregion        
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
        }
        private bool Render(System.IntPtr hWnd)
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
        private bool Setup(System.IntPtr hWnd, string lpFileName)
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
        private Rectangle GetCurrenIconRect()
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

        private int GetCurrenIconID()
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

        private void Release()
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

        public void UnInit()
        {
            Release();
            dll.UnLoadDll();
        }

        #region Overrides
        public override void OnFileNameChanged(string filePath)
        {
            if (//filePath.ToLower().EndsWith(".bmp") ||
                //filePath.ToLower().EndsWith(".jpg") ||
                //filePath.ToLower().EndsWith(".png") ||
                //filePath.ToLower().EndsWith(".tif") ||
                //filePath.ToLower().EndsWith(".gif"))
                filePath.ToLower().EndsWith(".uitex"))
            {
                //if (pbxPreview.Image != null)
                //    pbxPreview.Image.Dispose();

                try
                {
                    FileInfo fi = new FileInfo(filePath);
                    lblSizeValue.Text = fi.Length.ToString() + " Bytes";// Math.Round((fi.Length / 1024.0), 2).ToString() + "KB";
                    lblIDValue.Text = string.Empty;
                    lblRect.Text = string.Empty;

                    curFilename = filePath;
                    bool b = Setup(pbxPreview.Handle, filePath);
                    if (!InitRect.IsEmpty)
                        SetCurrenIconRect(InitRect);
                    Render(pbxPreview.Handle);
                    this.InitIconRect.Width = 0;
                    this.InitIconRect.Height = 0;
                    this.IconRect.Width = 0;
                    this.IconRect.Height = 0;
                }
                catch(Exception)// ex)
                {}
            }
            else
            {
                //if (pbxPreview.Image != null)
                //    pbxPreview.Image.Dispose();
                //pbxPreview.Image = null;
            }
        }
        //目录切换，需要清除目前显示的图片内容
        public override void OnFolderNameChanged(string folderName)
        {
            //if (pbxPreview.Image != null)
            //    pbxPreview.Image.Dispose();
            //pbxPreview.Image = null;
            //lblSizeValue.Text   = string.Empty;
            //lblRect.Text = string.Empty;
            //lblIDValue.Text = string.Empty;
            
            lblIDValue.Text = string.Empty;
            lblRect.Text = string.Empty;
            bool b = Setup(pbxPreview.Handle, null);
            Render(pbxPreview.Handle);
            pbxPreview.Invalidate();
        }

        public override void OnClosingDialog(CancelEventArgs e)
        {
            if (pbxPreview.Image != null)
                pbxPreview.Image.Dispose();
            if (IconID < 0)
                e.Cancel = true;
        }
        #endregion

        #region Private Methods
        private string GetColorsCountFromImage(Image image)
        {
            switch(image.PixelFormat)
            {
                case PixelFormat.Format16bppArgb1555:
                case PixelFormat.Format16bppGrayScale:
                case PixelFormat.Format16bppRgb555:
                case PixelFormat.Format16bppRgb565:
                    return "16 bits (65536 colors)";
                case PixelFormat.Format1bppIndexed:
                    return "1 bit (Black & White)";
                case PixelFormat.Format24bppRgb:
                    return "24 bits (True Colors)";
                case PixelFormat.Format32bppArgb:
                case PixelFormat.Format32bppPArgb:
                case PixelFormat.Format32bppRgb:
                    return "32 bits (Alpha Channel)";
                case PixelFormat.Format4bppIndexed:
                    return "4 bits (16 colors)";
                case PixelFormat.Format8bppIndexed:
                    return "8 bits (256 colors)";
            }
            return string.Empty;
        }

        private string GetFormatFromImage(Image image)
        {
            if (image.RawFormat.Equals(ImageFormat.Bmp))
                return "BMP";
            else if (image.RawFormat.Equals(ImageFormat.Gif))
                return "GIF";
            else if (image.RawFormat.Equals(ImageFormat.Jpeg))
                return "JPG";
            else if (image.RawFormat.Equals(ImageFormat.Png))
                return "PNG";
            else if (image.RawFormat.Equals(ImageFormat.Tiff))
                return "TIFF";
            return string.Empty;
        }
        #endregion


        private void pbxPreview_MouseClick(object sender, MouseEventArgs e)
        {
            IconRect = GetCurrenIconRect();
            IconID = GetCurrenIconID();
            lblIDValue.Text = IconID.ToString();
            lblRect.Text = IconRect.ToString();
            SetCurrenIconRect(IconRect);
            Render(pbxPreview.Handle);
        }
        
        private void pbxPreview_MouseMove(object sender, MouseEventArgs e)
        {
            //Render(pbxPreview.Handle);
            dll.InvokeLastMethod();
        }

        private void pbxPreview_Paint(object sender, PaintEventArgs e)
        {
            //Render(pbxPreview.Handle);
            dll.InvokeLastMethod();
        }
    }
    class ParameterCache
    {
        public object[] Parameters;
        public Type[] ParameterTypes;
        public ModePass[] themode;
        public Type Type_Return;
    };
}