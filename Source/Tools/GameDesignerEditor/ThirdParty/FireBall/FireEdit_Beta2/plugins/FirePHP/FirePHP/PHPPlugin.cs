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
using Fireball.Windows.Forms;
using Fireball.Syntax;
using System.Diagnostics;
using System.Collections;
using System.Resources;
using System.IO;
using FireEdit.Docking;
using Fireball.Docking;
using System.Windows.Forms;

namespace FirePHP
{
    public class PHPPlugin :FireEditPlugin
    {
        private Hashtable _FunctionsHashTable = null;

        private PHPToolBar phpToolBar = null;        

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
            _FunctionsHashTable = new Hashtable();

            Stream  stream = typeof(PHPPlugin).Assembly.GetManifestResourceStream("FirePHP.phpfunctions.ini");

            StreamReader reader = new StreamReader(stream);

            while (!reader.EndOfStream)
            {
                string line = reader.ReadLine();

                if (line != null)
                {
                    string functionName = line.Substring(0, line.IndexOf('='));
                    string desc = line.Substring(line.IndexOf('=') + 1);

                    _FunctionsHashTable.Add(functionName, desc);
                }
            }

            FireEditApplication editor = (FireEditApplication)application;

            editor.MainForm.NewCodeEditor += new EventHandler<NewCodeEditorEventArgs>(MainForm_NewCodeEditor);
            editor.MainForm.DockContainer.ActiveDocumentChanged += new EventHandler(DockContainer_ActiveDocumentChanged);

            phpToolBar = new PHPToolBar();

            phpToolBar.Enabled = false;

            phpToolBar.Name = "PHPToolBar";

            phpToolBar.Location = new System.Drawing.Point(editor.MainForm.MainToolStrip.Location.X + editor.MainForm.MainToolStrip.Width,
                editor.MainForm.MainToolStrip.Location.Y);

            editor.MainForm.TopToolStripPanel.Controls.Add(phpToolBar);


            ToolStripMenuItem mnuPHPHelp = new ToolStripMenuItem("PHP Docs");

            mnuPHPHelp.Click += new EventHandler(mnuPHPHelp_Click);

            ToolStripMenuItem mnuHelp = (ToolStripMenuItem)editor.MainForm.MainMenuStrip.Items.Find("mnuHelp", false)[0];

            mnuHelp.DropDownItems.Add("-");
            mnuHelp.DropDownItems.Add(mnuPHPHelp);


            return true;
        }

        void mnuPHPHelp_Click(object sender, EventArgs e)
        {
            try
            {
                Process.Start(Application.StartupPath + "\\docs\\php_manual_it.chm");
            }
            catch
            {

            }
        }

        void DockContainer_ActiveDocumentChanged(object sender, EventArgs e)
        {
            DockContainer container = (DockContainer)sender;

            if (container.ActiveDocument is FireEditorTabItem)
            {
                this.CheckForToolbarState((FireEditorTabItem)container.ActiveDocument);
            }
        }

        void CheckForToolbarState(FireEditorTabItem tabItem)
        { 
            if (tabItem.CodeEditor.Document.Parser.Language.Name == "PHP")
            {
                phpToolBar.Enabled = true;
            }
            else
            {
                phpToolBar.Enabled = false;
            }
        }

        void MainForm_NewCodeEditor(object sender, NewCodeEditorEventArgs e)
        {
            e.FireEditorTabItem.LanguageChanged += new EventHandler<FireEdit.Docking.LanguageChangedEventArgs>(FireEditorTabItem_LanguageChanged);            
        }

        void FireEditorTabItem_LanguageChanged(object sender, FireEdit.Docking.LanguageChangedEventArgs e)
        {
            FireEditorTabItem tabItem = (FireEditorTabItem)sender;


            if (e.Language.Name == "PHP")
            {
                tabItem.CodeEditor.KeyPress += new System.Windows.Forms.KeyPressEventHandler(CodeEditor_KeyPress);
                tabItem.CodeEditor.KeyDown += new System.Windows.Forms.KeyEventHandler(CodeEditor_KeyDown);
                tabItem.CodeEditor.CaretChange += new EventHandler(CodeEditor_CaretChange);
                tabItem.CodeEditor.InfoTipSelectedIndexChanged += new EventHandler(CodeEditor_InfoTipSelectedIndexChanged);
            }

            if (sender is FireEditorTabItem)
            {
                this.CheckForToolbarState((FireEditorTabItem)sender);
            }
        }

        void CodeEditor_InfoTipSelectedIndexChanged(object sender, EventArgs e)
        {
            CodeEditorControl editor = (CodeEditorControl)sender;

            string cleaned = currentTip.Substring(0, currentTip.IndexOf("-"));

            StringBuilder sb = new StringBuilder();

            sb.Append(cleaned.Substring(0, cleaned.IndexOf("(") + 1));

            string onlyArgs = cleaned.Substring(cleaned.IndexOf("(") + 1);
            onlyArgs = onlyArgs.Substring(0, onlyArgs.IndexOf(")") - 1);

            string[] args = onlyArgs.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

            for (int i = 0; i < args.Length; i++)
            {
                if (editor.InfoTipSelectedIndex-1 == i)
                {
                    sb.Append("<b>");
                }
                sb.Append(args[i]);

                if (editor.InfoTipSelectedIndex-1 == i)
                {
                    sb.Append("</b>");
                }

                if (i == args.Length-1)
                {
                    sb.Append(",");
                }
            }

            sb.Append(")<br>");
            sb.Append("<font color=green>");
            sb.Append(currentTip.Substring(currentTip.IndexOf("-") + 1).Trim());
            sb.Append("</font>");

            editor.InfoTipText = sb.ToString();
        }


        private string currentTip = null;

        void CodeEditor_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            CodeEditorControl editor = (CodeEditorControl)sender;
            if (e.KeyChar == ')')
            {
                wordEnteredInfo = null;
                editor.InfoTipVisible = false;

            }
            else if (e.KeyChar == '(')
            {
                // Word word = editor.Caret.CurrentWord;
                editor.Caret.CurrentRow.Parse(true);
                Word word = editor.Caret.CurrentWord;

                if (word != null)
                {
                    
                    if (word.Pattern == null || word.Pattern.Parent == null) 
                     
                    {
                        if (editor.Caret.CurrentWord.Index == 0)
                            return;
                        word = editor.Caret.CurrentRow[editor.Caret.CurrentWord.Index - 1];
                    }
                    if (word.Pattern == null || word.Pattern.Parent == null)
                    {
                        return;
                    }

                    if (word.Pattern.Parent.Style.Name == "PHP Functions")

                        if (word != wordEnteredInfo && _FunctionsHashTable.ContainsKey(word.Text))
                        {
                            if (editor.Caret.Position.X < word.Column)
                                return;

                            wordEnteredInfo = word;

                            currentTip = _FunctionsHashTable[word.Text].ToString();
                            editor.InfoTipText = currentTip;
                            editor.InfoTipVisible = true;
                            editor.InfoTipPosition = editor.Caret.Position;
                            editor.InfoTipCount = currentTip.Split(new char[] { ',' },
                                StringSplitOptions.RemoveEmptyEntries).Length;
                            editor.InfoTipSelectedIndex = 1;
                        }
                }

                if (wordEnteredInfo != null)
                {

                    if (wordEnteredInfo.Row.Text.IndexOf(wordEnteredInfo.Text) == -1)
                    {
                        editor.InfoTipVisible = false;
                        wordEnteredInfo = null;
                    }
                }
                else
                    editor.InfoTipVisible = false;
            }
            else if ((wordEnteredInfo != null && editor.Caret.CurrentWord == null)
           || (wordEnteredInfo != null && wordEnteredInfo.Column > editor.Caret.CurrentWord.Column))
                editor.InfoTipVisible = false;//this is executed when you move before the ( or deleted it
        }

        void CodeEditor_CaretChange(object sender, EventArgs e)
        {
            CodeEditorControl control = (CodeEditorControl)sender;

            if (wordEnteredInfo != null && control.Caret.CurrentWord != null)
            {
                string row = wordEnteredInfo.Row.Text;

                int endWord = row.IndexOf(wordEnteredInfo.Text) + wordEnteredInfo.Text.Length- 1;

                int dotC = 1;


                int count = control.Caret.CurrentWord.Column;

                for (int i = endWord+1; i < count; i++)
                {
                    string currentChar = row[i].ToString();

                    if (currentChar == ",")
                        dotC++;
                }
                if (dotC > control.InfoTipCount)
                    control.InfoTipSelectedIndex = control.InfoTipCount;
                else
                    control.InfoTipSelectedIndex = dotC;
            }
            //else if (wordEnteredInfo != null)
            //{
            //    if (wordEnteredInfo.Row != control.Caret.CurrentRow)
            //    {
            //        control.InfoTipVisible = false;
            //        wordEnteredInfo = null;
            //    }
            //}
        }

        private Word wordEnteredInfo = null;

     

        void CodeEditor_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            CodeEditorControl editor = (CodeEditorControl)sender;

            if (e.KeyCode == System.Windows.Forms.Keys.Return)
            {
                editor.InfoTipVisible = false;
            }
        }

        public override string Name
        {
            get {
                return "FirePHP";
            }
        }

        public override bool ShowMenuItem
        {
            get {
                return false;
            }
        }
    }
}
