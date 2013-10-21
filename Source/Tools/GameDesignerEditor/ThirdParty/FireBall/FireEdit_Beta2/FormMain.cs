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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Fireball.Windows.Forms.CodeEditor;
using System.IO;
using Fireball.CodeEditor.SyntaxFiles;
using System.Collections;
using System.Diagnostics;
using Fireball.Syntax;
using System.Xml;
using Fireball.Windows.Forms;
using Fireball.Plugins.Dialogs;
using FireEdit.Dialogs;
using Fireball.Docking;
using FireEdit.Docking;
using Fireball.Collections.Generic;

namespace FireEdit
{
    public partial class FormMain : Form
    {
        private DockContainer _DockContainer = null;
        public event EventHandler<NewCodeEditorEventArgs> NewCodeEditor;
        private FireEditOutputWindow _OutputWindow = null;

        static LanguageList _Languages = null;

        public static LanguageList Languages
        {
            get
            {
                return _Languages;
            }
        }

        public FormMain()
        {
            InitializeComponent();

            SyntaxLoader.UserCustomStyles = Path.Combine(Application.StartupPath, "conf");

            SyntaxLoader.UseUserCustomStyles = true;

            if (Directory.Exists(SyntaxLoader.UserCustomStyles) == false)
                Directory.CreateDirectory(SyntaxLoader.UserCustomStyles);

            _DockContainer = new DockContainer();

            toolStripContainer1.ContentPanel.Controls.Add(_DockContainer);

            _DockContainer.Dock = DockStyle.Fill;

            _DockContainer.DocumentStyle = DocumentStyles.DockingWindow;

            _DockContainer.ActiveDocumentChanged+=new EventHandler(_DockContainer_ActiveDocumentChanged);

            _DockContainer.ActiveContentChanged+=new EventHandler(_DockContainer_ActiveContentChanged);

            _Languages = new LanguageList();

            LoadSyntaxes();

            mnuNew.DropDownItems.AddRange(GetNewFileItems());

            btnNew.DropDownItems.AddRange(GetNewFileItems());

            this.LoadRecents();

            this.FormClosing += new FormClosingEventHandler(FormMain_FormClosing);

            //if (File.Exists(Application.StartupPath + @"\dockconfig.xml"))
            //    _DockContainer.LoadFromXml(Application.StartupPath + @"\dockconfig.xml", null);

            ((ToolStripProfessionalRenderer)mainToolBar.Renderer).ColorTable.UseSystemColors = true;

            this.Text = "FireEdit";


            _OutputWindow = new FireEditOutputWindow();

            _OutputWindow.Icon = Icon.FromHandle(Properties.Resources.Output.GetHicon());  
        }


        private void LoadSyntaxes()
        {
            string path = Path.Combine(Application.StartupPath, "syns");

            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            string[] files = Directory.GetFiles(path, "*.syn");

            foreach (string file in files)
            {
                Language lang = Language.FromSyntaxFile(file);

                _Languages.Add(lang);
            }

            SyntaxLanguage[] langs = (SyntaxLanguage[])Enum.GetValues(typeof(SyntaxLanguage));

            foreach (SyntaxLanguage lang in langs)
            {
                Language lng = CodeEditorSyntaxLoader.GetLanguageFrom(lang);

                _Languages.Add(lng);
            }
        }


        /// <summary>
        /// The Default FireEdit OutputWindow
        /// </summary>
        public FireEditOutputWindow OutputWindow
        {
            get
            {
                return _OutputWindow;
            }
        }

        /// <summary>
        /// DockContainer for FireEditDockableWindow
        /// </summary>
        public DockContainer DockContainer
        {
            get
            {
                return _DockContainer;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            AddDocument(); 
        }


        void _DockContainer_ActiveContentChanged(object sender, EventArgs e)
        {
           // this.CheckForItemState();
        }


        void _DockContainer_ActiveDocumentChanged(object sender, EventArgs e)
        {
            this.CheckInsMode();

            FireEditDockableWindow tabItem = (FireEditDockableWindow)_DockContainer.ActiveDocument;

            this.CheckForItemState();

            if (tabItem is FireEditorTabItem)
            {
                FireEditorTabItem item = (FireEditorTabItem)tabItem;

                btnSave.Enabled = !item.CodeEditor.Saved;
            }
            else
            {
                DisableMainToolBarButtons();
            }

            
        }

        void DisableMainToolBarButtons()
        {
            foreach (ToolStripItem current in mainToolBar.Items)
            {
                current.Enabled = false;
            }

            btnNew.Enabled = true;
            btnOpen.Enabled = true;
        }

        public ToolStripMenuItem PluginsMenuItem
        {
            get
            {
                return mnuPlugins;
            }
        }

        void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.SaveRecentFiles();

            //_DockContainer.SaveAsXml(Application.StartupPath + @"\dockconfig.xml");
        }

        void Caret_Change(object sender, EventArgs e)
        {
            this.UpateLinesStatusInfo();
        }

        private void UpateLinesStatusInfo()
        {
            stColumLabel.Text = "Col: " + this.CurrentEditor.Caret.Position.X;

            stLineLabel.Text = "Ln: " + this.CurrentEditor.Caret.CurrentRow.Index;
        }

        private void mnuOpen_Click(object sender, EventArgs e)
        {
            this.Open();
        }

        private void mnuSave_Click(object sender, EventArgs e)
        {
            this.Save();
        }

        private void mnuSaveAs_Click(object sender, EventArgs e)
        {
            this.SaveAs();
        }

        private void mnuExit_Click(object sender, EventArgs e)
        {
            this.Close();
            Application.Exit();
        }

        void LoadRecents()
        {
            string file = Application.StartupPath;

            file = Path.Combine(file, "recents.xml");

            if (!File.Exists(file))
                return;

            XmlTextReader xreader = new XmlTextReader(file);

            int index = 1;

            while (xreader.Read())
            {
                if (xreader.IsStartElement("file"))
                {
                    string path = xreader.ReadString();

                    ToolStripMenuItem item = new ToolStripMenuItem(index.ToString() + ": " + Path.GetFileName(path));

                    item.Tag = path;

                    item.Click += new EventHandler(item_Click);

                    mnuRecentFiles.DropDownItems.Add(item);

                    index++;
                }
            }

            xreader.Close();
        }

        void AddRecent(string filename)
        {
            for (int i = 0; i < mnuRecentFiles.DropDownItems.Count; i++)
            {
                ToolStripMenuItem current = (ToolStripMenuItem)mnuRecentFiles.DropDownItems[i];

                if (current.Tag.ToString() == filename)
                {
                    mnuRecentFiles.DropDownItems.Remove(current);
                    break;
                }
            }

            if (mnuRecentFiles.DropDownItems.Count > 7)
            {
                for (int i = mnuRecentFiles.DropDownItems.Count-1; i > 7; i--)
                {
                    mnuRecentFiles.DropDownItems.RemoveAt(i);
                }
            }

            ToolStripMenuItem item = new ToolStripMenuItem("1: " + Path.GetFileName(filename));
            item.Tag = filename;
            item.Click+=new EventHandler(item_Click);

            mnuRecentFiles.DropDownItems.Insert(0, item);

            for (int i = 1; i < mnuRecentFiles.DropDownItems.Count; i++)
            {
                ToolStripMenuItem current = (ToolStripMenuItem)mnuRecentFiles.DropDownItems[i];

                current.Text = (i+1).ToString() + ": " + Path.GetFileName(current.Tag.ToString());
            }
        }

        void SaveRecentFiles()
        {
            string file = Application.StartupPath;

            file = Path.Combine(file, "recents.xml");

            XmlTextWriter xwr = new XmlTextWriter(file, Encoding.UTF8);

            xwr.WriteStartElement("files");

            for (int i = 0; i < mnuRecentFiles.DropDownItems.Count; i++)
            {
                ToolStripMenuItem current = (ToolStripMenuItem)mnuRecentFiles.DropDownItems[i];

                xwr.WriteStartElement("file");

                xwr.WriteString(current.Tag.ToString());

                xwr.WriteEndElement();
            }

            xwr.WriteEndElement();

            xwr.Flush();

            xwr.Close();
        }

        void item_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            this.OpenFile(item.Tag.ToString());
        }

        public void Save()
        {
            if (this.CurrentEditorTabItem.CodeEditor.FileName == null ||
                this.CurrentEditorTabItem.CodeEditor.FileName == string.Empty)
                this.SaveAs();
            else
            {
                this.CurrentEditorTabItem.CodeEditor.Save();
            }
            SetAllFileNameLabel();
        }

        public void Open()
        {
            OpenFileDialog opf = new OpenFileDialog();

            opf.Filter = GetDialogFileFilters();

            opf.FilterIndex = filterCount;

            if (opf.ShowDialog() == DialogResult.OK)
            {
                this.OpenFile(opf.FileName);
            }            
        }


        public FireEditorTabItem CurrentEditorTabItem
        {
            get
            {
                return this._DockContainer.ActiveDocument as FireEditorTabItem;
            }
        }

        public ToolStripPanel TopToolStripPanel
        {
            get
            {
                return toolStripContainer1.TopToolStripPanel;
            }
        }

        public void OpenFile(string filename)
        {
            FireEditorTabItem tabItem = this.AddDocument();

            tabItem.Open(filename);

            AddRecent(filename);

            SetAllFileNameLabel();

            tabItem.Activate();
        }


        public FireEditorTabItem AddDocument()
        {
            FireEditorTabItem tabItem = new FireEditorTabItem();

            tabItem.Show(_DockContainer, DockState.Document);

            tabItem.Activate();

            tabItem.FileSaved += new EventHandler(tabItem_FileSaved);

            tabItem.FormClosing+=new FormClosingEventHandler(tabItem_FormClosing);

            if (NewCodeEditor != null)
                NewCodeEditor(this, new NewCodeEditorEventArgs(ref tabItem));


            tabItem.CodeEditor.TextChanged += new EventHandler(editorControl_TextChanged);

            return tabItem;
        }


        void tabItem_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (sender is FireEditorTabItem)
            {
                FireEditorTabItem item = (FireEditorTabItem)sender;

                if (item.CodeEditor.Saved == false)
                {
                    DialogResult rt = MessageBox.Show("File unsaved do you want to save it before close?", Application.ProductName, MessageBoxButtons.YesNoCancel);

                    if (rt == DialogResult.Yes)
                    {
                        this.Save();
                    }
                    else if (rt == DialogResult.Cancel)
                    {
                        e.Cancel = true;
                    }
                }
            }
        }

        void tabItem_FileSaved(object sender, EventArgs e)
        {
            if (this.CurrentEditorTabItem != null)
                if (sender == this.CurrentEditor)
                    btnSave.Enabled = !this.CurrentEditor.Saved;
        }

        int filterCount = 0;

        private string GetDialogFileFilters()
        {
            Dictionary<string, Language>.Enumerator langs = (Dictionary<string, Language>.Enumerator)CodeEditorSyntaxLoader.LanguageList.ListLanguages();

            StringBuilder sb = new StringBuilder();

            int i = 0;

            while (langs.MoveNext())
            {
                if (i != 0)
                    sb.Append("|");

                sb.Append(langs.Current.Value.FileDialogFilters);
            }

            filterCount = i + 2;
            sb.Append("|All File|*.*");

            return sb.ToString();
        }

        public void SaveAs()
        {
            SaveFileDialog svd = new SaveFileDialog();

            svd.Filter = GetDialogFileFilters();

            if (svd.ShowDialog() == DialogResult.OK)
            {
                this.CurrentEditorTabItem.Save(svd.FileName);
                AddRecent(this.CurrentEditor.FileName);
                SetAllFileNameLabel();
            }            
        }

        private void mnuUndo_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Undo();
        }

        private void mnuRedo_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Redo();
        }

        private void mnuCut_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Cut();
        }

        private void mnuCopy_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Copy();
        }

        private void mnuPaste_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Paste();
        }

        private void mnuSelectAll_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.SelectAll();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            this.Save();
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            this.Open();
        }

        private void btnSaveAs_Click(object sender, EventArgs e)
        {
            this.SaveAs();
        }

        private void CheckForItemState()
        {
            if (this.CurrentEditorTabItem == null)
            {
                mnuUndo.Enabled = false;
                mnuRedo.Enabled = false;

                mnuPaste.Enabled = false;
                mnuCut.Enabled = false;
                mnuCopy.Enabled = false;

                mnuSelectAll.Enabled = false;

                btnCut.Enabled = false;
                btnCopy.Enabled = false;
                btnPaste.Enabled = false;

                btnRedo.Enabled = false;
                btnUndo.Enabled = false;
                btnSave.Enabled = false;
                btnSaveAs.Enabled = false;

                btnPrint.Enabled = false;
                mnuPrint.Enabled = false;

                mnuSave.Enabled = false;
                mnuSaveAs.Enabled = false;

                btnFind.Enabled = false;
                btnFindNext.Enabled = false;
                btnReplace.Enabled = false;

                return;
            }

            mnuUndo.Enabled = this.CurrentEditor.CanUndo;
            mnuRedo.Enabled = this.CurrentEditor.CanRedo;

            btnSave.Enabled = this.CurrentEditor.Saved;
            btnSaveAs.Enabled = true;
            btnPrint.Enabled = true;
            mnuPrint.Enabled = true;
            mnuSave.Enabled = this.CurrentEditor.Saved;
            mnuSaveAs.Enabled = true;

            btnFind.Enabled = true;
            btnFindNext.Enabled = true;
            btnReplace.Enabled = true;

            mnuPaste.Enabled = this.CurrentEditor.CanPaste;
            mnuCut.Enabled = this.CurrentEditor.CanCopy;
            mnuCopy.Enabled = this.CurrentEditor.CanCopy;

            mnuSelectAll.Enabled = this.CurrentEditor.CanSelect;

            btnCut.Enabled = mnuCut.Enabled;
            btnCopy.Enabled = mnuCopy.Enabled;
            btnPaste.Enabled = mnuPaste.Enabled;

            btnRedo.Enabled = mnuRedo.Enabled;
            btnUndo.Enabled = mnuUndo.Enabled;
        }

        void SetAllFileNameLabel()
        {
            stFileLabel.Text = Path.GetFileName(this.CurrentEditor.FileName);
            stFileLabel.ToolTipText = this.CurrentEditor.FileName;            
        }

        private void mnuEdit_DropDownOpening(object sender, EventArgs e)
        {
            this.CheckForItemState();
        }

        private void btnCut_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Cut();
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Copy();

            EditViewControl v = this.CurrentEditor.ActiveViewControl;
        }

        private void btnPaste_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Paste();
        }

        private void btnUndo_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Undo();
        }

        private void btnRedo_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.Redo();
        }

        private void editorControl_TextChanged(object sender, EventArgs e)
        {
            this.CheckForItemState();
        }

        private ToolStripMenuItem[] GetNewFileItems()
        {
             

            int i = 0;

            ArrayList list = new ArrayList();

            ToolStripMenuItem othersParent = null;

            Dictionary<string, Language>.Enumerator langs = (Dictionary<string, Language>.Enumerator)FormMain.Languages.ListLanguages();

            while(langs.MoveNext())
            {
                ToolStripMenuItem currentItem = new ToolStripMenuItem(langs.Current.Value.Name);

                currentItem.Tag = langs.Current.Value;

                currentItem.Click += new EventHandler(NewMenuItems_Click);

                if (i < 10)
                {
                    list.Add(currentItem);
                }
                else
                {
                    othersParent.DropDownItems.Add(currentItem);
                }

                i++;

                if ((i % 10) == 0)
                {
                    othersParent = new ToolStripMenuItem("Others");

                    list.Add(othersParent);
                }
            }

            ToolStripMenuItem[] items = new ToolStripMenuItem[list.Count];

            list.CopyTo(items);

            return items;
        }

        void NewMenuItems_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = sender as ToolStripMenuItem;

            FireEditorTabItem tabItem = AddDocument();

            tabItem.CodeEditor.Document.Text = "";

            tabItem.SetSyntaxLanguage((Language)item.Tag);

            tabItem.Activate();
        }

        private void stFileLabel_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                Process.Start(Path.GetDirectoryName(this.CurrentEditor.FileName));
            }
            catch
            {

            }
        }

        private void stFileLabel_MouseEnter(object sender, EventArgs e)
        {
            statusStrip1.Cursor = Cursors.Hand;
        }

        private void stFileLabel_MouseLeave(object sender, EventArgs e)
        {
            statusStrip1.Cursor = Cursors.Default;
        }

        private void btnFind_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.ShowFind();
        }

        private void btnFindNext_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.FindNext();
        }

        private void btnReplace_Click(object sender, EventArgs e)
        {
            this.CurrentEditor.ShowReplace();
        }

        private void editorControl_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Insert)
            {
                this.CheckInsMode();
            }
        }

        void CheckInsMode()
        {
            if (this.CurrentEditor == null)
                return;

            if (this.CurrentEditor.OverWrite)
                stKeyMod.Text = "OVR";
            else
                stKeyMod.Text = "INS";
        }


        private void mnuPrint_Click(object sender, EventArgs e)
        {
            this.Print();
        }


        void Print()
        {
            SourceCodePrintDocument printDoc = new SourceCodePrintDocument(this.CurrentEditor.Document);

            PrintDialog pdlg = new PrintDialog();

            pdlg.Document = printDoc;

            if (pdlg.ShowDialog(this) == DialogResult.OK)
                pdlg.Document.Print();
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            this.Print();
        }

        private void mnuAbout_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Copyright (c) dotnetfireball.org 2005/2006");
        }


        /// <summary>
        /// The current editor showed on FireEdit
        /// </summary>
        public CodeEditorControl CurrentEditor
        {
            get
            {
                if (this._DockContainer.ActiveDocument == null)
                    return null;

                return ((FireEditorTabItem)this._DockContainer.ActiveDocument).CodeEditor;
            }
        }

        /// <summary>
        /// The Default Toolbar with common command of FireEdit
        /// </summary>
        public ToolStrip MainToolStrip
        {
            get
            {
                return mainToolBar;
            }
        }

        /// <summary>
        /// Copy the text from the current document
        /// </summary>
        public void CopyText()
        {
            this.CurrentEditor.Copy();
        }

        /// <summary>
        /// Cut the text from the current document
        /// </summary>
        public void CutText()
        {
            this.CurrentEditor.Cut();
        }

        /// <summary>
        /// Paste the text on the current document
        /// </summary>
        public void PasteText()
        {
            this.CurrentEditor.Paste();
        }


        /// <summary>
        /// The Main Status strip of FireEdit Form
        /// </summary>
        public StatusStrip MainStatusStrip
        {
            get
            {
                return statusStrip1;
            }
        }

        private void mnuConfigurePlugins_Click(object sender, EventArgs e)
        {
            PluginConfigurationDialog dlg = new PluginConfigurationDialog();

            dlg.ShowDialog(this);
        }

        private void mnuSyntaxOptions_Click(object sender, EventArgs e)
        {
            FireEditLanguageOptions opt = new FireEditLanguageOptions();

            opt.ShowDialog();
        }

    }
}
