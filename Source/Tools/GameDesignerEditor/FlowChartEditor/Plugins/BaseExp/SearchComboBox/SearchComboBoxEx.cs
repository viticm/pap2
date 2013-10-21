using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Reflection;
using System.Drawing;
using System.IO;

namespace BaseExp.SearchComboBox
{
    public class SearchComboBoxEx : DevComponents.DotNetBar.Controls.ComboBoxEx
    {
        private ArrayList m_arrlist_All = new ArrayList();
        ArrayList itemObjectList = new ArrayList(); // 下拉框内容对象列表
        static Random rnd = new Random(); // 随机数对象 

        [DllImport("user32.dll")]
        private static extern IntPtr FindWindowExA(IntPtr hWnd1, IntPtr hWnd2, string lpsz1, string lpsz2);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hwnd, int wMsg, int length,
            [MarshalAs(UnmanagedType.LPTStr)]StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessageA", CharSet = CharSet.Auto)]
        private static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, int lParam);

        private const int WM_GETTEXT = 0xd;
        private const int WM_KEYDOWN = 0x100;

        /// <summary>
        /// 构造函数
        /// </summary>
        public SearchComboBoxEx()
        {
            this.DrawMode = DrawMode.OwnerDrawFixed;
            this.DisplayMember = "Text";
            this.Images = StandardImageProvider.ImgList;
        }

        /// <summary>
        /// 为了解决Window的bug,当Items数目为0，并且下拉菜单处于下拉状态，获取Text会出问题。
        /// 所以只好手工帮助它修正这个bug，FindWindow，再从comboBox里面的那个Edit中取其Text
        /// </summary>
        public override string Text
        {
            get
            {
                if (base.Items.Count > 0)
                {
                    return base.Text;
                }
                else
                {
                    IntPtr ip = FindWindowExA(this.Handle, IntPtr.Zero, null, null);

                    if (ip != IntPtr.Zero)
                    {
                        StringBuilder sb = new StringBuilder(256);
                        SendMessage(ip, WM_GETTEXT, 255, sb);

                        return sb.ToString();
                    }

                    return "";
                }
            }

            set 
            { 
                base.Text = value; 
            }
        }


        protected override void OnEnter(EventArgs e)
        {
            foreach (object i in base.Items)
            {
                string strPy = "";
                strPy = Chinese2Spell.GetHeadOfChs(i.ToString());
                strPy = strPy.Replace(" ", "").ToLower();
                m_arrlist_All.Add(new object[] { i, strPy });
            }

            base.OnEnter(e);
        }

        protected override void OnLeave(EventArgs e)
        {
            foreach (object[] i in m_arrlist_All)
            {
                object i1 = i[0];
                if (!base.Items.Contains(i1)) base.Items.Add(i1);
            }
            this.m_arrlist_All.Clear();

            base.OnLeave(e);
        }

        protected override void OnTextUpdate(EventArgs e)
        {
            string oldText = this.Text;

            List<object> list = new List<object>();
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
                    if (!base.Items.Contains(i1)) list.Add(i1); //Items.Add(i1);
                }
                else
                {
                    if (base.Items.Contains(i1)) base.Items.Remove(i1);
             
                }
            }
            base.Items.AddRange(list.ToArray());
            if (base.Items.Count > 0)
            {
                this.DroppedDown = true;
            }
            this.Cursor = System.Windows.Forms.Cursors.Arrow;
            base.OnTextUpdate(e);
            
            // 太恶心
            if (this.Text != oldText)
            {
                string report = string.Format("old text: {0}\r\nnew text:{1}", oldText, this.Text);
                System.Diagnostics.Debug.WriteLine(report);
                
                this.Text = oldText;
                IntPtr hwnd = FindWindowExA(this.Handle, IntPtr.Zero, null, null);
                if (hwnd != IntPtr.Zero)
                {
                    int vk_end = 0x23;
                    SendMessage(hwnd, WM_KEYDOWN, vk_end, 0);
                }
            }
        }

        /// <summary>
        /// 批量添加下拉框内容
        /// </summary>
        /// <param name="objs">添加对象集合</param>
        public void Items_AddRange(object[] objs)
        {
            foreach(object o in objs)
            {
                Items_Add(o);
            }
        }        

        /// <summary>
        /// 添加下拉框内容
        /// </summary>
        /// <param name="obj">添加对象</param>
        public void Items_Add(object obj)
        {
            DevComponents.Editors.ComboItem item = new DevComponents.Editors.ComboItem();
 
            item.ImageIndex = rnd.Next(400);
            item.Tag = obj;
            item.Text = obj.ToString();
            base.Items.Add(item);
            itemObjectList.Add(obj);
        }

        /// <summary>
        /// 添加下拉框内容
        /// </summary>
        /// <param name="obj">添加对象</param>
        /// <param name="imageIndex">图标编号</param>
        public void Items_Add(object obj, int imageIndex)
        {
            DevComponents.Editors.ComboItem item = new DevComponents.Editors.ComboItem();

            item.ImageIndex = imageIndex;
            item.Tag = obj;
            item.Text = obj.ToString();
            base.Items.Add(item);
            itemObjectList.Add(obj);
        }

        /// <summary>
        /// 下拉框条目列表
        /// </summary>
        public ArrayList Items_All
        {
            get
            {
                return itemObjectList;
            }
        }

        /// <summary>
        /// 覆盖Items属性
        /// </summary>
        public new ComboBox.ObjectCollection Items
        {
            set 
            { 
                ; 
            }
        }

        /// <summary>
        /// 下拉框标签数量
        /// </summary>
        public int Items_Count
        {
            get
            {
                return base.Items.Count;
            }
        }

        /// <summary>
        /// 选中的下拉框标签
        /// </summary>
        public object Items_SelectedItem 
        { 
            get
            {
                if (base.SelectedItem is DevComponents.Editors.ComboItem)
                {
                    return (base.SelectedItem as DevComponents.Editors.ComboItem).Tag;
                }
                else
                {
                    return null;
                }
            }

            set
            {
                foreach (DevComponents.Editors.ComboItem item in base.Items)
                {
                    if (item.Tag == value)
                    {
                        this.SelectedItem = item;
                        break;
                    }
                }
            }
        }
    }

    /// <summary>
    /// 提供图标源
    /// </summary>
    public class StandardImageProvider
    {
        private static ImageList imglist = null;

        public static ImageList ImgList
        {
            get
            {
                if (imglist == null)
                {
                    imglist = new ImageList();
                    Assembly asm = Assembly.GetExecutingAssembly();

                    foreach (string resName in asm.GetManifestResourceNames())
                    {
                        if (resName.EndsWith(".png"))
                        {
                            imglist.Images.Add(Image.FromStream(asm.GetManifestResourceStream(resName)));
                        }
                    }

                }

                return imglist;
            }
        }

        private StandardImageProvider()
        {
 
        }
    }
}
