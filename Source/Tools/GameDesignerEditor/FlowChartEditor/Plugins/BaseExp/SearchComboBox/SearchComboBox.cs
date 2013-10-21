using System;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Collections;
using System.Text;

namespace BaseExp.SearchComboBox
{
    /// <summary>
    /// SearchCombBox 的摘要说明。
    /// </summary>
    public class SearchComboBox : System.Windows.Forms.ComboBox
    {
        private ArrayList m_arrlist_All = new ArrayList();

        [DllImport("user32.dll")]
        private static extern IntPtr FindWindowExA(IntPtr hWnd1, IntPtr hWnd2, string lpsz1, string lpsz2);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hwnd, int wMsg, int length, 
            [MarshalAs(UnmanagedType.LPTStr)]StringBuilder lParam);

        private const int WM_GETTEXT = 0xd;
//         private FilterType m_filter = FilterType.ShouZiMu;
// 
//         public FilterType Filter
//         {
//             get { return this.Filter; }
//             set { this.Filter = value; }
//         }

        //为了解决Window的bug,当Items数目为0，并且下拉菜单处于下拉状态，获取Text会出问题。
        //所以只好手工帮助它修正这个bug，FindWindow，再从comboBox里面的那个Edit中取其Text
        public override string Text
        {
            get 
            {
                if (this.Items.Count > 0)
                    return base.Text;
                else
                {
                    IntPtr ip = FindWindowExA(this.Handle, IntPtr.Zero, null, null);
                    if(ip != IntPtr.Zero)
                    {
                        StringBuilder sb = new StringBuilder(256);
                        SendMessage(ip, WM_GETTEXT, 255, sb);
                        return sb.ToString();
                    }
                    return "";
                }
            }
            set { base.Text = value; }
        }


        protected override void OnEnter(EventArgs e)
        {
            if (this.DropDownStyle != ComboBoxStyle.DropDown) return;
            foreach (object i in this.Items)
            {
                string strPy = "";
                //if (this.m_filter == FilterType.ShouZiMu)
                    strPy = Chinese2Spell.GetHeadOfChs(i.ToString());
                //else if (this.m_filter == FilterType.QuanPin)
                //    strPy = Chinese2Spell.ChsString2Spell(i.ToString());
                strPy = strPy.Replace(" ", "").ToLower();
                m_arrlist_All.Add(new object[] { i, strPy });
            }
            base.OnEnter(e);
        }

        protected override void OnLeave(EventArgs e)
        {
            if (this.DropDownStyle != ComboBoxStyle.DropDown) return;
            foreach (object[] i in m_arrlist_All)
            {
                object i1 = i[0];
                if(!Items.Contains(i1)) Items.Add(i1);
            }
            this.m_arrlist_All.Clear();            
            base.OnLeave(e);
        }


        protected override void OnTextUpdate(EventArgs e)
        {
            if (this.DropDownStyle != ComboBoxStyle.DropDown) return;
            foreach (object[] i in m_arrlist_All)
            {
                object i1 = i[0];   //内容对象
                object i2 = i[1];   //拼音字符串
                bool bShow = true;
                if (this.Text.Length > 0)
                {
                    string[] strKeys = this.Text.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string strKey in strKeys)
                    {
                        if (i2.ToString().IndexOf(strKey) == -1)
                        {
                            bShow = false;
                            break;
                        }
                    }
                }
                if (bShow)
                {
                    if(!Items.Contains(i1)) Items.Add(i1);
                }   
                else
                {
                    if (Items.Contains(i1)) Items.Remove(i1);
                }
            }
            if (this.Items.Count >　0)
            {
                this.DroppedDown = true;
            }
            this.Cursor = Cursors.Arrow;
            base.OnTextUpdate(e);
        }
    }

//     public enum FilterType
//     {
//         ShouZiMu = 0,
//         QuanPin = 1
//     }
}
