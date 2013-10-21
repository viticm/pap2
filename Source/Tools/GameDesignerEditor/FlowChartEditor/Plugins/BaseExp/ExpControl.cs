using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Diagnostics;

namespace BaseExp
{
    public partial class ExpControl : FlowLayoutPanel
    {
        public VarExp VExp = null;              //任意类型

        private GameEvent m_eventDefine;
        private ActionExp m_actionExp;
        private bool isReady = false;
        private bool m_showThisEvent = true;

        //获取是否全蓝
        public bool GetReady
        {
            get { return isReady; }
        }

        public ActionExp ActionExp
        {
            get { return this.m_actionExp; }
        }

        public ExpControl()
        {
            InitializeComponent();
        }

        //根据事件显示标签
        public void SetComboText(GameEvent gameEvent)
        {
            Trace.Assert(gameEvent != null);

            //造一个假的ActionExp
            ActionExp aexp = new ActionExp();
            aexp.API = gameEvent.GetEventAPI();

            //继续显示
            SetComboText(aexp, gameEvent);
            this.m_showThisEvent = false;
        }

        //根据表达式显示标签
        public void SetComboText(ActionExp expAction, GameEvent gameEvent)
        {
            Trace.Assert(expAction != null);
            Trace.Assert(gameEvent != null);
            this.m_eventDefine = gameEvent;
            this.m_actionExp = expAction;
            GameAPI api = expAction.API;
            this.Controls.Clear();
            isReady = true;
            m_actionExp.strText = "";
            m_actionExp.strFullText = "";

            string strComboText = "}" + api.combText;
            string[] tmp = strComboText.Split(new string[] { "{" }, StringSplitOptions.None);
            for (int i = -1; i < tmp.Length - 1; i++)
            {
                string[] strcs = tmp[i + 1].Split(new string[] { "}" }, StringSplitOptions.None);

                //添加热文本
                if (strcs[0] != "")
                {
                    LinkLabel link = new LinkLabel();
                    link.Margin = new System.Windows.Forms.Padding(0);
                    link.AutoSize = true;
                    link.Tag = new Hashtable();
                    int nReali = Convert.ToInt32(strcs[0].ToString()) - 1;
                    
                    TypeDefine typed = api.ArgList[nReali].ArgType;
                    if(typed.isEnum || typed.DBID == FixedValueProvider.TYPE_STRING_ID)
                    {
                        //右键菜单
                        ConstExp[] exps = ExpManager.ScanConstantFromHistory(typed);
                        if (exps.Length > 0)
                        {
                            link.ContextMenu = new ContextMenu();
                            for(int it = 0; it < exps.Length; it++)
                            {
                                MenuItem mi = new MenuItem(exps[it].ToString());
                                mi.Tag = new object[] { nReali, exps[it] };
                                mi.Click += new EventHandler(mi_Click);
                                link.ContextMenu.MenuItems.Add(mi);
                            }
                        }
                    }
                    
                    if (/*api.ArgValues.Count <= i || */api.ArgValues[nReali] == null)
                    {

                        link.Text = "<" + api.ArgList[nReali].ArgName + ">";
                        link.LinkColor = Color.Red;
                        (link.Tag as Hashtable)["exp"] = ExpManager.CreateNewExp(api.ArgList[nReali].ArgType);
                        (link.Tag as Hashtable)["pos"] = nReali;
                        isReady = false;
                    }
                    else
                    {
                        link.Text = api.ArgValues[nReali].ToString();
                        link.LinkColor = Color.Blue;
                        (link.Tag as Hashtable)["exp"] = api.ArgValues[nReali];
                        (link.Tag as Hashtable)["pos"] = nReali;
                    }

                    link.MouseClick += new MouseEventHandler(link_MouseClick);
                    link.PreviewKeyDown += new PreviewKeyDownEventHandler(link_PreviewKeyDown);
                    this.Controls.Add(link);

                    //追加到显示内容区域
                    //if(link.Text.Length > 25)
                    //    m_actionExp.strText += link.Text.Substring(0,23) + "...";
                    //else
                    //    m_actionExp.strText += link.Text + "";
                    m_actionExp.strText += link.Text;
                    m_actionExp.strFullText += link.Text;
                }

                //正常文本
                if (strcs[1] != "")
                {
                    Label l = new Label();
                    l.Margin = new System.Windows.Forms.Padding(0);
                    l.AutoSize = true;
                    l.Text = strcs[1];
                    this.Controls.Add(l);
                    m_actionExp.strText += l.Text + "";
                    m_actionExp.strFullText += l.Text;
                    l.Text = l.Text.Replace("\r\n", "");
                    
                }
            }

            //添加换行
            Control br = new Label();
            br.Width = this.Width - 23;
            br.Height = 0;
            this.Controls.Add(br);

            //添加Tip注释
            Control ltip = new Label();
            ltip.Text = "\r\n" + expAction.API.strTip;
            ltip.ForeColor = Color.Gray;
            ltip.AutoSize = true;
            this.Controls.Add(ltip);

            //专门处理比较API
            if(expAction.API.DBID == FixedValueProvider.COMPARE_API_ID)
            {
                LinkLabel left = this.Controls[0] as LinkLabel;
                LinkLabel right = this.Controls[2] as LinkLabel;
                Hashtable htleft = left.Tag as Hashtable;
                Hashtable htright = right.Tag as Hashtable;
                if (left.LinkColor == Color.Red)
                {
                    //左值类型清空
                    htleft["exp"] = ExpManager.CreateUnknownExp();

                    //右侧变灰
                    right.LinkColor = Color.Gray;
                }
                else
                {
                    //右侧的类型跟左侧走
                    (htright["exp"] as Exp).ReturnType = (htleft["exp"] as Exp).ReturnType;

                    //右侧变红
                    if ((htright["exp"] as Exp).strText == null)
                    {
                        right.LinkColor = Color.Red;
                    }
                }
            }

        }

        //左击链接
        void link_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left) return;
            LinkLabel link = sender as LinkLabel;
            if (link.LinkColor == Color.Gray) return;           //灰色不响应
            Hashtable ht = link.Tag as Hashtable;
            Exp expEdit = ht["exp"] as Exp;
            int nAsPos = Convert.ToInt32(ht["pos"].ToString());
            TypeDefine require_retType = null;
            if (expEdit != null) require_retType = expEdit.ReturnType;
            ExpForm expForm = new ExpForm(expEdit, this.m_eventDefine, m_showThisEvent, require_retType, this.VExp);
            //校准坐标
            expForm.Location = PointToScreen(new Point(link.Location.X, link.Location.Y + link.Height));
            int tx = expForm.Location.X;
            int ty = expForm.Location.Y;
            if (tx + expForm.Width > Screen.PrimaryScreen.Bounds.Width)
                tx = tx - expForm.Width + link.Width;
            if (ty + expForm.Height > Screen.PrimaryScreen.Bounds.Height)
                ty = ty - expForm.Height - link.Height;
            expForm.Location = new Point(tx, ty);
            //弹出窗口
            DialogResult dr = expForm.ShowDialog();
            if(dr == DialogResult.OK)
            {
                ExpManager.ScanConstantIntoHistory(expForm.RusultExp);          //把常数添加至历史
                ModifyExp(nAsPos, expForm.RusultExp);
            }            
        }

        //选中菜单
        void mi_Click(object sender, EventArgs e)
        {
            MenuItem mi = sender as MenuItem;
            object[] objs = mi.Tag as object[];
            int nAsPos = Convert.ToInt32(objs[0].ToString());
            ConstExp exp = objs[1] as ConstExp;
            ModifyExp(nAsPos, exp);
        }

        void link_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                link_MouseClick(sender, new MouseEventArgs(MouseButtons.Left, 0, 0, 0, 0));
        }

        private void ModifyExp(int nAsPos, Exp exp)
        {
            this.m_actionExp.API.ArgValues[nAsPos] = exp;
            this.SetComboText(this.m_actionExp,this.m_eventDefine);
            //自动放置焦点
            int n = 0;
            foreach(Control c in this.Controls)
            {
                if (c is LinkLabel)
                {
                    n++;
                    if (n > nAsPos)
                    {
                        (c as LinkLabel).Focus();
                        break;
                    }
                }
            }  
        }
    }
}
