using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;
using System.Windows.Forms;
using System.Data.SqlClient;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	public class PreviewCanvas : Panel
	{
        private bool beginInit = false; // 是否开始初始化 
        private string previewType = "NPC"; // 预览类型

        /// <summary>
        /// 构造函数
        /// </summary>
        public PreviewCanvas()
        {

        }

        /// <summary>
        /// 预览类型
        /// </summary>
        public string PreviewType
        {
            set
            {
                previewType = value;
            }
        }

        /// <summary>
        /// 是否开始初始化
        /// </summary>
        public bool BeginInit
        {
            set
            {
                beginInit = value;
            }
        }

        /// <summary>
        /// 处理面板大小改变事件
        /// </summary>
        /// <param name="eventargs">事件参数</param>
        protected override void OnResize(EventArgs eventargs)
        {
            base.OnResize(eventargs);
            if(EngineLayer.ATLBase != null)
            {
                EngineLayer.ATLBase.OnResizeNpcWindow();
                EngineLayer.ATLBase.OnResizeDoodadWindow();
            }
        }

        /// <summary>
        /// 接收系统消息
        /// </summary>
        /// <param name="msg">系统消息</param>
        protected override void WndProc(ref Message msg)
        {
            if (beginInit && EngineLayer.ATLBase != null)
            {
                int ret = 0;
                tagMSG tmsg = new tagMSG();
                tmsg.hwnd = (IntPtr)msg.HWnd;
                tmsg.lParam = (int)(msg.LParam);
                tmsg.message = Convert.ToUInt32(msg.Msg);
                tmsg.wParam = (uint)(msg.WParam);
                
                EngineLayer.ATLBase.OnWndMsg(ref tmsg, ref ret, previewType);
            }
            base.WndProc(ref msg);            
        }
	}
}
