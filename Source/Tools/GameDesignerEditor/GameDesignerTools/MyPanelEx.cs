using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using atlaxLib;

namespace GameDesingerTools
{
    class MyPanelEx : PanelEx
    {
        public BaseForm bf_parent;

        protected override void WndProc(ref Message msg)
        {
            try
            {
                if (BaseForm.K3DEngine != null)
                {
                    string name = bf_parent.ModelName + "[" + bf_parent.tabControl.SelectedTabIndex.ToString() + "]";

                    tagMSG tmsg = new tagMSG();
                    tmsg.hWnd = (IntPtr)msg.HWnd;
                    tmsg.lParam = (int)(msg.LParam);
                    tmsg.message = Convert.ToUInt32(msg.Msg);
                    tmsg.wParam = (uint)(msg.WParam);

                    BaseForm.K3DEngine.OnWndMsg(name, ref tmsg);
                }
                base.WndProc(ref msg);
            }
            catch (Exception)
            {

            }
        }
    };
}
