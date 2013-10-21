#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using FireEdit;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using FireEdit.Docking;
using Fireball.Windows.Forms;
using System.Diagnostics;

namespace GemixEd
{
    public class GemixPlugin: FireEditPlugin
    {
        private FireEditApplication _Application = null;

        private ToolStripMenuItem _BuildMenuItem = null;
        private ToolStripMenuItem _ConfigureGemixItem = null;
        private ToolStripMenuItem _BuildFileItem = null;
        private ToolStripMenuItem _BuildFileRunItem = null;

        private XmlDocument _ConfigDocument = new XmlDocument();

        public override Fireball.Plugins.PluginInfo About
        {
            get { return null; }
        }

        public override bool Execute(Fireball.Plugins.PluginApplication application, params object[] parameters)
        {
            return this.Execute(application);
        }

        public override bool Execute(Fireball.Plugins.PluginApplication application)
        {

            _Application = ((FireEditApplication)application);

           ToolStripItem mnuEdit = _Application.MainForm.MainMenuStrip.Items.Find("mnuEdit", false)[0];

           int editMenuIndex = _Application.MainForm.MainMenuStrip.Items.IndexOf(mnuEdit);


           _BuildMenuItem = new ToolStripMenuItem("&Build");


           _Application.MainForm.MainMenuStrip.Items.Insert(editMenuIndex + 1, _BuildMenuItem);

           _Application.MainForm.Text = "FireEdit For Gemix";

           _ConfigureGemixItem = new ToolStripMenuItem("Configure GMX Compiler");
           _ConfigureGemixItem.Click += new EventHandler(_ConfigureGemixItem_Click);

           _BuildMenuItem.DropDownItems.Add(_ConfigureGemixItem);

           _BuildMenuItem.DropDownItems.Add("-");
           _BuildFileItem = new ToolStripMenuItem("Build PRG");
           _BuildFileRunItem = new ToolStripMenuItem("Build PRG/RUN");
           _BuildFileRunItem.Click += new EventHandler(_BuildFileRunItem_Click);

           _BuildFileItem.Click += new EventHandler(_BuildFileItem_Click);

           _BuildMenuItem.DropDownItems.Add(_BuildFileItem);
           _BuildMenuItem.DropDownItems.Add(_BuildFileRunItem);


           string configFile = Path.Combine(Application.StartupPath, "gemix.xml");


           _Application.MainForm.DockContainer.ActiveDocumentChanged += new EventHandler(DockContainer_ActiveDocumentChanged);

           if (File.Exists(configFile))
           {
               _ConfigDocument.Load(configFile);
           }
           else
           {
               _ConfigDocument.LoadXml("<gemix><compiler /></gemix>");
           }

            return true;
        }

        void _BuildFileRunItem_Click(object sender, EventArgs e)
        {
            string exeFile = BuildPRG();

            if (exeFile != null)
            {
                Process.Start(exeFile);
            }
        }

        void compiler_OutputReceived(object sender, EventArgs e)
        {
            //if (_Application.MainForm.OutputWindow.IsHidden)
            //{


                _Application.MainForm.OutputWindow.AppendOutput(sender.ToString());
            //}
        }

        void _BuildFileItem_Click(object sender, EventArgs e)
        {
            BuildPRG();
        }

        string BuildPRG()
        {
            _Application.MainForm.OutputWindow.Show(_Application.MainForm.DockContainer,
                 Fireball.Docking.DockState.DockBottom);

            _Application.MainForm.OutputWindow.Clear();

            GemixCompiler compiler = new GemixCompiler();
            compiler.OutputReceived += new EventHandler(compiler_OutputReceived);

            FireEditorTabItem editor = _Application.MainForm.CurrentEditorTabItem;

            _Application.MainForm.Save();

            string prgFile = editor.CodeEditor.FileName;

            string gmxCompiler = _ConfigDocument.SelectSingleNode("gemix/compiler").InnerText;

            compiler.Start(gmxCompiler, prgFile);

            string exeFile = Path.GetFileNameWithoutExtension(prgFile) + ".exe";

            return (File.Exists(exeFile) ? exeFile : null);
        }

        void DockContainer_ActiveDocumentChanged(object sender, EventArgs e)
        {
            if (_Application.MainForm.DockContainer.ActiveDocument is FireEditorTabItem)
            {
                FireEditorTabItem tab = (FireEditorTabItem)_Application.MainForm.DockContainer.ActiveDocument;

                if (tab.CodeEditor.Document.Parser.Language.Name == "Gemix")
                {
                    _BuildMenuItem.Enabled = true;
                }
                else
                {
                    _BuildMenuItem.Enabled = false;
                }
            }
            else
                _BuildMenuItem.Enabled = false;
        }

        void _ConfigureGemixItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog opf = new OpenFileDialog();

            opf.Filter = "Gemix Compiler|*.exe";

            if (opf.ShowDialog() == DialogResult.OK)
            {
                _ConfigDocument.SelectSingleNode("gemix/compiler").InnerText = opf.FileName;

                string configFile = Path.Combine(Application.StartupPath, "gemix.xml");

                _ConfigDocument.Save(configFile);
            }
        }



        public override string Name
        {
            get {
                return "GemixED";
            }
        }

        public override bool ShowMenuItem
        {
            get { return false; }
        }
    }
}
