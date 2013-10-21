#region License

//Copyright (C) <2005>  <Sebastian Faltoni> sebastian@dotnetfireball.org

//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using FireEdit;
using System.Windows.Forms;
using FireEdit.Docking;

namespace Fireball.Plugin
{
    public class HtmlPlugin : FireEditPlugin
    {
        public override bool Execute(Fireball.Plugins.PluginApplication application)
        {
            return this.Execute(application, new object[0]);
        }

        public override bool Execute(Fireball.Plugins.PluginApplication application, params object[] parameters)
        {

            ((FireEditApplication)application).MainForm.NewCodeEditor += new EventHandler<NewCodeEditorEventArgs>(MainForm_NewCodeEditor);
            return true;
        }

        void MainForm_NewCodeEditor(object sender, NewCodeEditorEventArgs e)
        {
            e.FireEditorTabItem.LanguageChanged += new EventHandler<LanguageChangedEventArgs>(FireEditorTabItem_LanguageChanged);
        }

        void FireEditorTabItem_LanguageChanged(object sender, LanguageChangedEventArgs e)
        {
            if (e.Language != null && e.Language.Name == "HTML")
            {
                FireEditorTabItem tabItem = (FireEditorTabItem)sender;

                ToolStrip myToolStrip = new ToolStrip();

                myToolStrip.Name = "switchBar";

                myToolStrip.Dock = DockStyle.Bottom;

                tabItem.Controls.Add(myToolStrip);

                tabItem.BringToFront();


                ToolStripButton btnCode = new ToolStripButton();

                btnCode.Checked = true;

                btnCode.Text = "Html Code";

                btnCode.CheckOnClick = true;

                myToolStrip.Items.Add(btnCode);

                btnCode.Click += new EventHandler(btnCode_Click);


                ToolStripButton btnPreview = new ToolStripButton();

                btnPreview.Text = "Html Preview";

                btnPreview.CheckOnClick = true;

                myToolStrip.Items.Add(btnPreview);

                btnPreview.Click += new EventHandler(btnPreview_Click);


                WebBrowser browser = new WebBrowser();

                browser.Name = "preview";

                browser.Dock = DockStyle.Fill;

                browser.Visible = false;


                tabItem.Controls.Add(browser);
            }
        }

        void btnPreview_Click(object sender, EventArgs e)
        {
            ToolStripButton button = (ToolStripButton)sender;

            FireEditorTabItem tabItem = (FireEditorTabItem)button.Owner.Parent;

            if (button.Checked == true)
                ((ToolStripButton)button.Owner.Items[0]).Checked = false;

            if (!tabItem.CodeEditor.Saved)
            {
                MessageBox.Show("The file need to be saved before show a preview!");
                ((ToolStripButton)button.Owner.Items[0]).Checked = true;
               ((ToolStripButton)button.Owner.Items[1]).Checked = false;
                return;
            }

            tabItem.Controls["preview"].Visible = true;
            tabItem.Controls["preview"].BringToFront();

            tabItem.Controls["codeeditor"].Visible = false;



            ((WebBrowser)tabItem.Controls["preview"]).Navigate(tabItem.CodeEditor.FileName);

        }

        void btnCode_Click(object sender, EventArgs e)
        {
            ToolStripButton button = (ToolStripButton)sender;

            FireEditorTabItem tabItem = (FireEditorTabItem)button.Owner.Parent;

            if (button.Checked == true)
                ((ToolStripButton)button.Owner.Items[1]).Checked = false;

            tabItem.Controls["preview"].Visible = false;

            tabItem.Controls["codeeditor"].Visible = true;
            tabItem.Controls["codeeditor"].BringToFront();

        }

        public override Fireball.Plugins.PluginInfo About
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override bool ShowMenuItem
        {
            get { return false; }
        }

        public override string Name
        {
            get {
                return "HtmlPlugin";
            }
        }
    }
}