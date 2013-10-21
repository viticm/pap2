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
using Fireball.Windows.Forms;
using Fireball.CodeEditor;
using Fireball.CodeEditor.SyntaxFiles;
using System.IO;
using Fireball.Syntax;
using Fireball.Docking;
using FireEdit.Themes;
using System.Drawing;

namespace FireEdit.Docking
{
    public class FireEditorTabItem : FireEditDocumentWindow
    {
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ToolStripMenuItem mnuUndo;
        private System.Windows.Forms.ToolStripMenuItem mnuRedo;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem mnuCut;
        private System.Windows.Forms.ToolStripMenuItem mnuCopy;
        private System.Windows.Forms.ToolStripMenuItem mnuPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem mnuSelectAll;
        private CodeEditorControl _EditorControl = null;

        public event EventHandler FileSaved;

        public event EventHandler<LanguageChangedEventArgs> LanguageChanged;



        public FireEditorTabItem()
        {
            InitializeComponent();

            _EditorControl = new CodeEditorControl();
            _EditorControl.Dock = System.Windows.Forms.DockStyle.Fill;

            _EditorControl.ContextMenuStrip = contextMenuStrip1;

            _EditorControl.Name = "codeeditor";

            this.Controls.Add(_EditorControl);

            this.Text = "Blank*";

            _EditorControl.FileNameChanged += new EventHandler(_EditorControl_FileNameChanged);

            _EditorControl.FileSavedChanged += new EventHandler(_EditorControl_FileSaved);

            _EditorControl.FontName = "Courier New";

            _EditorControl.FontSize = 10;

            
      

            _EditorControl.LineNumberForeColor = Color.SteelBlue;
            _EditorControl.LineNumberBorderColor = Color.SteelBlue;
        }
 

        void _EditorControl_FileSaved(object sender, EventArgs e)
        {
            if (FileSaved != null)
                FileSaved(sender, e);
        }

        void _EditorControl_FileNameChanged(object sender, EventArgs e)
        {
            this.Text = Path.GetFileName(this._EditorControl.FileName);
        }

        public CodeEditorControl CodeEditor
        {
            get
            {
                return _EditorControl;
            }
        }

        public void Open(string filename)
        {
            this._EditorControl.Open(filename);

            CodeEditorSyntaxLoader.SetSyntax(this._EditorControl, filename);

            if (LanguageChanged != null)
                LanguageChanged(this,
                    new LanguageChangedEventArgs(this._EditorControl.Document.Parser.Language));

            this.ToolTipText = filename;
        }

        public void Save(string filename)
        {
            this._EditorControl.Save(filename);

            if (this._EditorControl.Document.Parser.Language == null)
            {
                CodeEditorSyntaxLoader.SetSyntax(this._EditorControl, filename);
                if (LanguageChanged != null)
                    LanguageChanged(this,
                        new LanguageChangedEventArgs(this._EditorControl.Document.Parser.Language));
            }
            this.ToolTipText = filename;
        }


        public void SetSyntaxLanguage(Language language)
        {
            this._EditorControl.Document.Parser.Init(language);

            if (LanguageChanged != null)
                LanguageChanged(this, 
                    new LanguageChangedEventArgs(language));
        }

        public void SetSyntaxLanguage(SyntaxLanguage syntaxLanguage)
        {
            CodeEditorSyntaxLoader.SetSyntax(this._EditorControl, syntaxLanguage);
            if (LanguageChanged != null)
                LanguageChanged(this,
                    new LanguageChangedEventArgs(this._EditorControl.Document.Parser.Language));
        }

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.mnuUndo = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRedo = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuCut = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSelectAll = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuUndo,
            this.mnuRedo,
            this.toolStripSeparator1,
            this.mnuCut,
            this.mnuCopy,
            this.mnuPaste,
            this.toolStripSeparator2,
            this.mnuSelectAll});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(129, 148);
            this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
            this.contextMenuStrip1.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.contextMenuStrip1_ItemClicked);
            // 
            // mnuUndo
            // 
            this.mnuUndo.Image = global::FireEdit.Properties.Resources.undo;
            this.mnuUndo.Name = "mnuUndo";
            this.mnuUndo.Size = new System.Drawing.Size(128, 22);
            this.mnuUndo.Text = "&Undo";
            // 
            // mnuRedo
            // 
            this.mnuRedo.Image = global::FireEdit.Properties.Resources.redo;
            this.mnuRedo.Name = "mnuRedo";
            this.mnuRedo.Size = new System.Drawing.Size(128, 22);
            this.mnuRedo.Text = "&Redo";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(125, 6);
            // 
            // mnuCut
            // 
            this.mnuCut.Image = global::FireEdit.Properties.Resources.cut;
            this.mnuCut.Name = "mnuCut";
            this.mnuCut.Size = new System.Drawing.Size(128, 22);
            this.mnuCut.Text = "&Cut";
            // 
            // mnuCopy
            // 
            this.mnuCopy.Image = global::FireEdit.Properties.Resources.copy;
            this.mnuCopy.Name = "mnuCopy";
            this.mnuCopy.Size = new System.Drawing.Size(128, 22);
            this.mnuCopy.Text = "&Copy";
            // 
            // mnuPaste
            // 
            this.mnuPaste.Image = global::FireEdit.Properties.Resources.paste;
            this.mnuPaste.Name = "mnuPaste";
            this.mnuPaste.Size = new System.Drawing.Size(128, 22);
            this.mnuPaste.Text = "&Paste";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(125, 6);
            // 
            // mnuSelectAll
            // 
            this.mnuSelectAll.Image = global::FireEdit.Properties.Resources.selection;
            this.mnuSelectAll.Name = "mnuSelectAll";
            this.mnuSelectAll.Size = new System.Drawing.Size(128, 22);
            this.mnuSelectAll.Text = "Select &All";
            // 
            // FireEditorTabItem
            // 
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Name = "FireEditorTabItem";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        private void contextMenuStrip1_ItemClicked(object sender, System.Windows.Forms.ToolStripItemClickedEventArgs e)
        {
            if (e.ClickedItem == mnuCopy)
                _EditorControl.Copy();
            else if(e.ClickedItem == mnuCut)
                _EditorControl.Cut();
            else if(e.ClickedItem == mnuPaste)
                _EditorControl.Paste();
            else if(e.ClickedItem == mnuUndo)
                _EditorControl.Undo();
            else if(e.ClickedItem == mnuRedo)
                _EditorControl.Redo();
            else if(e.ClickedItem == mnuSelectAll)
                _EditorControl.SelectAll();
        }

        private void contextMenuStrip1_Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            mnuCut.Enabled = _EditorControl.CanCopy;
            mnuCopy.Enabled = _EditorControl.CanCopy;
            mnuPaste.Enabled = _EditorControl.CanPaste;
            mnuRedo.Enabled = _EditorControl.CanRedo;
            mnuUndo.Enabled = _EditorControl.CanUndo;
            mnuSelectAll.Enabled = _EditorControl.CanSelect;
        }
    }

    public class LanguageChangedEventArgs : EventArgs
    {
        private Language _Language = null;

        public LanguageChangedEventArgs(Language language)
        {
            _Language = language;
        }

        public Language Language
        {
            get
            {
                return _Language;
            }
        }
    }
}
