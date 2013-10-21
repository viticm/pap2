using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security;
using System.Data.SqlClient;
using DevComponents.DotNetBar;
using System.Collections;
using System.Diagnostics;


namespace NotesManager
{
    public partial class frmMain : Form
    {
        private SqlDb sd = new SqlDb();
        private string keyword;
        public const string WEB_URL = "http://changan/eweb/";

        public frmMain()
        {
            InitializeComponent();
        }

        public frmMain(SqlConnection conn)
        {
            sd.SetConnection(conn);
            InitializeComponent();
            //this.keyword = keyword;
            //Application.DoEvents();
            //initTab();
        }

        //先删除原有tab页，再加载所有页面
        public void reloadAll(string keyword)
        {
            this.keyword = keyword;
            
            foreach(TabItem ti in tabControl1.Tabs )
            {
                ti.AttachedControl.Visible = false;
            }
            tabControl1.Tabs.Clear();
            initTab();
            tabControl1.Refresh();
        }

        //创建所有标签们
        private void initTab()
        {
            //查询数据库并列举出所有tab
            SqlDataReader reader = sd.ExeSqlDr(string.Format("select * from sys_notes where keyword = '{0}' order by sort", this.keyword));
            string title = "";
            while(reader.Read())
            {
                Application.DoEvents();
                title = reader["tabname"].ToString().Trim();
                newTab(title, WEB_URL + "document.asp?id=" + reader["id"].ToString(), reader["id"].ToString().Trim());
            }
            reader.Close();
            sd.CloseCnn();
            tabControl1.Tag = tabControl1.Tabs.Count;

            if(tabControl1.Tabs.Count == 0)
            {
                Label ctrl_lb = new Label();
                ctrl_lb.AutoSize = true;
                ctrl_lb.Location = new System.Drawing.Point(50, 50);
                ctrl_lb.Text = "当前没有任何标签。如需添加，请单击右键，选择“新标签”";
                tabControl1.Controls.Add(ctrl_lb);
            }

            editToolStripMenuItem.Enabled = delToolStripMenuItem.Enabled = (0 != tabControl1.Tabs.Count);
        }

        //创建新tab控件
        private TabItem newTab(string title, string url, string id)
        {
            TabItem ti = new TabItem();
            TabControlPanel panel = new TabControlPanel();
            WebBrowser tb = new WebBrowser();

            panel.Dock = DockStyle.Fill;
            panel.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            panel.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            panel.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            panel.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            panel.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right) | DevComponents.DotNetBar.eBorderSide.Bottom)));
            panel.Style.GradientAngle = 90;

            tb.Tag = id;
            tb.Dock = DockStyle.Fill;
            tb.ScrollBarsEnabled = false;
            tb.Navigate(url);
            tb.Visible = true;
            panel.Controls.Add(tb);
            panel.Tag = tb;
            tabControl1.Controls.Add(panel);

            ti.Text = title;
            ti.Tag = tb;
            panel.TabItem = ti;
            ti.AttachedControl = panel;
            tabControl1.Tabs.Add(ti);

            return (ti);
        }

        //添加新标签
        private void newToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            string title = "";
            title = frmInput.show_out();
            if (title == "") return;
            string sql = string.Format("select * from sys_notes where keyword='{0}' and tabname='{1}'", keyword, title);
            if( 1 == sd.ExeIsExist(sql))
            {
                MessageBox.Show("名称重复!", "错误", MessageBoxButtons.OK , MessageBoxIcon.Error);
                return;
            }
            //添加到数据库
            sql = string.Format("insert into sys_notes(keyword,tabname,author,createtime,modifyby,modifytime,htmlcode) values('{0}','{1}','{2}',getdate(),'{2}',getdate(),'{3}')", keyword, title, GetHostName(), "\r\n<STYLE>p{margin:0} </STYLE>");
            sd.ExeNoQuery(sql);

            //获取插入的数据的id
            sql = string.Format("select * from sys_notes where keyword='{0}' and tabname='{1}'", keyword, title);
            SqlDataReader reader = sd.ExeSqlDr(sql);
            string id = "";

            if (reader.Read())
            {
                id = reader["id"].ToString();
                //添加标签
                TabItem ti = newTab(title, WEB_URL + "document.asp?id=" + id, id);
                tabControl1.SelectedTab = ti;
            }
            reader.Close();
            sd.CloseCnn();
            editToolStripMenuItem.Enabled = delToolStripMenuItem.Enabled = (0 != tabControl1.Tabs.Count);
        }

        //删除标签
        private void delToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确认删除[" + tabControl1.SelectedTab.Text + "]？", "警告", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.Yes)
            {
                //从数据库删除
                string sql = string.Format("delete from sys_notes where id={0}", ((WebBrowser)tabControl1.SelectedTab.Tag).Tag);
                sd.ExeNoQuery(sql);

                //删除tab页
                tabControl1.SelectedPanel.Controls.Clear();
                tabControl1.Tabs.Remove(tabControl1.SelectedTab);
            }
            editToolStripMenuItem.Enabled = delToolStripMenuItem.Enabled = (0 != tabControl1.Tabs.Count);
        }
        
        
        //修改标签
        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string title = "";
            for (; ; )
            {
                title = frmInput.show_out(tabControl1.SelectedTab.Text);
                if (title == tabControl1.SelectedTab.Text)
                {
                    break;
                }
                string sql = string.Format("select * from sys_notes where keyword='{0}' and tabname='{1}'", keyword, title);
                if( 1 == sd.ExeIsExist(sql))
                {
                    MessageBox.Show("标签名称有重复!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    continue;
                }
                //改数据库
                sql = string.Format("update sys_notes set tabname='{0}',modifyby='{1}',modifytime=getdate() where id={2}",
                                            title,
                                            GetHostName(),
                                            ((WebBrowser)tabControl1.SelectedTab.Tag).Tag
                                            );
                sd.ExeNoQuery(sql);
                
                //改标签
                tabControl1.SelectedTab.Text = title;

                break;
            }
                

        }

        /************************************************************************/
        /* 看内容是否被更改过
        /************************************************************************/
        public Boolean hasChanged()
        {
            int tabIndex = 0;
            foreach ( TabItem ti in tabControl1.Tabs)
            {
                WebBrowser wb = (WebBrowser)ti.AttachedControl.Tag;
                while (wb.IsBusy || wb.ReadyState != WebBrowserReadyState.Complete)
                {
                    Application.DoEvents();
                }
                string oldneirong = "";
                string newneirong = "";

                
                try
                {
                    //获取旧内容
                    HtmlElement element = wb.Document.GetElementById("neirong");
                    if (element != null)
                        oldneirong = element.GetAttribute("value");

                    //获取新内容
                    object newContent = wb.Document.InvokeScript("getstring");
                    if (newContent != null)
                        newneirong = newContent.ToString();
                }
                catch (Exception e1)
                {
                    newneirong = null;
                    oldneirong = null;
                }

                //看看内容是否改变
                string sql = "";
                if ( oldneirong != newneirong )
                {
                    return (true);
                }

                //看看标签页顺序是否改变
                sql = string.Format("select * from sys_notes where sort={0} and id={1}",tabIndex++, ((WebBrowser)ti.Tag).Tag);
                if( 0 == sd.ExeIsExist(sql))
                {
                    return (true);
                }
                
                
            }

            //看看标签页数，是不是少了
            if( tabControl1.Tag != null && (int)tabControl1.Tag != tabControl1.Tabs.Count)
            {
                return (true);
            }

            //既然什么都没变，就返回false吧
            return (false);
        }


        /************************************************************************/
        /*  保存内容至数据库
        /************************************************************************/
        public void saveAll()
        {
            //保存所有标签
            int tabIndex = 0;
            foreach ( TabItem ti in tabControl1.Tabs)
            {
                WebBrowser wb = (WebBrowser)ti.AttachedControl.Tag;
                string oldneirong = "";
                string newneirong = "";

                //获取旧内容
                try
                {
                    oldneirong = wb.Document.GetElementById("neirong").GetAttribute("value");
                }
                catch(Exception e1)
                {
                    e1.ToString();
                    oldneirong = null;
                }

                //获取新内容
                try
                {
                    newneirong = wb.Document.InvokeScript("getstring").ToString();
                }
                catch (Exception e1)
                {
                    e1.ToString();
                    newneirong = null;
                }
                
                //更新数据库内容
                string sql = "";
                if (newneirong != null || oldneirong != newneirong )
                {
                     sql = string.Format("update sys_notes set htmlcode='{0}',modifyby='{1}',modifytime=getdate() where id={2}",
                                                newneirong.Replace("'", "''"),
                                                GetHostName(),
                                                ((WebBrowser)ti.Tag).Tag
                                                );
                    int retNo = sd.ExeNoQuery(sql);

                    //更新浏览器旧值
                    wb.Document.GetElementById("neirong").SetAttribute("value", newneirong);
                }



                //更新排列顺序
                sql = string.Format("update sys_notes set sort={0} where id={1}",
                                tabIndex++,
                                ((WebBrowser)ti.Tag).Tag
                                );
                sd.ExeNoQuery(sql);
            }
            tabControl1.Tag = tabControl1.Tabs.Count;
        }


        /************************************************************************/
        /* 刷新所有页面
        /************************************************************************/
        public void reloadAll()
        {
            foreach (TabItem ti in tabControl1.Tabs)
            {
                Application.DoEvents();
                WebBrowser wb = (WebBrowser)ti.AttachedControl.Tag;
                wb.Document.Window.History.Go(0);
            }
        }
        
        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            MessageBox.Show(hasChanged().ToString());
            saveAll();
        }

        public static string GetHostName()
        {
            return System.Net.Dns.GetHostName();
        }

        private void tabControl1_MouseDown(object sender, MouseEventArgs e)
        {
            if ( e.Button == MouseButtons.Right)
            {
                
            }
        }

    }
}