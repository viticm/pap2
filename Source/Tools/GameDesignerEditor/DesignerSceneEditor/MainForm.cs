using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;

namespace GameDesingerTools
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            mdiClient1.ControlAdded += new ControlEventHandler(MdiClientControlAddRemove);
            mdiClient1.ControlRemoved += new ControlEventHandler(MdiClientControlAddRemove);
        }
        #region "换肤相关"
        private void ChangeDotNetBarStyle(eDotNetBarStyle style)
        {
            cmdStyleOffice2007Blue.Checked = (style == eDotNetBarStyle.Office2007);
            cmdStyleOffice2003.Checked = (style == eDotNetBarStyle.Office2003);
            cmdStyleOfficeXP.Checked = (style == eDotNetBarStyle.OfficeXP);
            cmdStyleVS2005.Checked = (style == eDotNetBarStyle.VS2005);

            //explorerBar1.ColorScheme = new DevComponents.DotNetBar.ColorScheme(style);

            dotNetBarManager1.Style = style;
            if (style == eDotNetBarStyle.Office2003 || style == eDotNetBarStyle.OfficeXP || style == eDotNetBarStyle.VS2005)
            {
                if (dotNetBarManager1.IsThemeActive)
                {
                    //bThemes.Visible = true;
                    barStandard.RecalcLayout();
                }
            }
            else if (dotNetBarManager1.IsThemeActive)
            {
                //bThemes.Visible = false;
                barStandard.RecalcLayout();
            }

            if (style == eDotNetBarStyle.Office2003)
            {
                tabStrip1.Style = eTabStripStyle.OneNote;
            }
            else if (style == eDotNetBarStyle.Office2007)
            {
                tabStrip1.Style = eTabStripStyle.Office2007Document;
            }
            else if (style == eDotNetBarStyle.VS2005)
            {
                tabStrip1.Style = eTabStripStyle.VS2005;
            }
            else
            {
                tabStrip1.Style = eTabStripStyle.Flat;
            }

            // Status bar style
            bar1.Style = style;
        }
        private void EnableThemes(bool bEnable)
        {
            dotNetBarManager1.SuspendLayout = true;
            dotNetBarManager1.ThemeAware = bEnable;
            dotNetBarManager1.SuspendLayout = false;
            tabStrip1.ThemeAware = bEnable;

            this.Refresh();
        }
        private void cmdStyleOffice2007Blue_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.Office2007);
        }
        private void cmdStyleOfficeXP_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.OfficeXP);
        }
        private void cmdStyleVS2005_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.VS2005);
        }
        private void cmdStyleOffice2003_Click(object sender, EventArgs e)
        {
            ChangeDotNetBarStyle(eDotNetBarStyle.Office2003);
        }
        #endregion
        #region "ＴＡＢ页相关"
        private void SwitchWorkingMode(bool tabbedMdi)
        {
            if (tabbedMdi)
            {
                // Switch to Tabbed Mdi Interface
                dotNetBarManager1.MdiSystemItemVisible = false;
                tabStrip1.MdiTabbedDocuments = true;
                dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
            else
            {
                // Switch to standard Mdi Interface
                tabStrip1.Visible = false;
                dotNetBarManager1.MdiSystemItemVisible = true;
                tabStrip1.MdiTabbedDocuments = false;
                tabStrip1.MdiForm = null;
                SyncMdiTabbedBar();
            }
        }
        private void SyncMdiTabbedBar()
        {
            if (cmdTabbedMdi.Checked)
            {
                // Hide window list bar
                dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
            else
            {
                if (this.MdiChildren.Length > 0)
                {
                    dotNetBarManager1.Bars["windowlist"].Visible = true;
                    dotNetBarManager1.Bars["windowlist"].DockLine = 1;
                    dotNetBarManager1.Bars["windowlist"].RecalcLayout();
                }
                else
                    dotNetBarManager1.Bars["windowlist"].Visible = false;
            }
        }
        private void cmdTabbedMdi_Click(object sender, EventArgs e)
        {
            SwitchWorkingMode(!cmdTabbedMdi.Checked);
            cmdTabbedMdi.Checked = !cmdTabbedMdi.Checked;
        }
        #endregion

        private void MainForm_Load(object sender, EventArgs e)
        {
            tabStrip1.MdiForm = this;
        }
        private void cmdNewDocument_Click(object sender, EventArgs e)
        {
            foreach(Form frm in this.MdiChildren)
            {
                if (frm is SkillForm)
                {
                    frm.Activate();
                    return;
                }
            }
            SkillForm doc = new SkillForm();
            doc.MdiParent = this;
            doc.WindowState = FormWindowState.Maximized;
            doc.Show();
            doc.Update();
            //doc.Text = "New Document " + this.MdiChildren.Length.ToString();
        }
        private void buttonItem3_Click(object sender, EventArgs e)
        {
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is NPCForm)
                {
                    frm.Activate();
                    return;
                }
            }
            NPCForm doc = new NPCForm();
            doc.MdiParent = this;
            doc.WindowState = FormWindowState.Maximized;
            doc.Show();
            doc.Update();
        }
        private void MdiClientControlAddRemove(object sender, ControlEventArgs e)
        {
            if (e.Control is Form)
                SyncMdiTabbedBar();
        }


    }
}