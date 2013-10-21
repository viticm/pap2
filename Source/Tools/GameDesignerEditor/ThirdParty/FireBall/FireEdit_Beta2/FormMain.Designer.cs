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

namespace FireEdit
{
    partial class FormMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.stFileLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.stLineLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.stColumLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.stKeyMod = new System.Windows.Forms.ToolStripStatusLabel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.mnuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuNew = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSave = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuPrint = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuRecentFiles = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator11 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEdit = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuUndo = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRedo = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuCut = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSelectAll = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator13 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSyntaxOptions = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuPlugins = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuConfigurePlugins = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator12 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.mainToolBar = new System.Windows.Forms.ToolStrip();
            this.btnNew = new System.Windows.Forms.ToolStripDropDownButton();
            this.btnOpen = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.btnSave = new System.Windows.Forms.ToolStripButton();
            this.btnSaveAs = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator10 = new System.Windows.Forms.ToolStripSeparator();
            this.btnPrint = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.btnCut = new System.Windows.Forms.ToolStripButton();
            this.btnCopy = new System.Windows.Forms.ToolStripButton();
            this.btnPaste = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.btnUndo = new System.Windows.Forms.ToolStripButton();
            this.btnRedo = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.btnFind = new System.Windows.Forms.ToolStripButton();
            this.btnFindNext = new System.Windows.Forms.ToolStripButton();
            this.btnReplace = new System.Windows.Forms.ToolStripButton();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.syntaxDocument1 = new Fireball.Syntax.SyntaxDocument(this.components);
            this.statusStrip1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.mainToolBar.SuspendLayout();
            this.toolStripContainer1.BottomToolStripPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.stFileLabel,
            this.stLineLabel,
            this.stColumLabel,
            this.stKeyMod});
            this.statusStrip1.Location = new System.Drawing.Point(0, 0);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.statusStrip1.Size = new System.Drawing.Size(460, 22);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // stFileLabel
            // 
            this.stFileLabel.DoubleClickEnabled = true;
            this.stFileLabel.Name = "stFileLabel";
            this.stFileLabel.Size = new System.Drawing.Size(445, 17);
            this.stFileLabel.Spring = true;
            this.stFileLabel.MouseEnter += new System.EventHandler(this.stFileLabel_MouseEnter);
            this.stFileLabel.MouseLeave += new System.EventHandler(this.stFileLabel_MouseLeave);
            this.stFileLabel.DoubleClick += new System.EventHandler(this.stFileLabel_DoubleClick);
            // 
            // stLineLabel
            // 
            this.stLineLabel.Name = "stLineLabel";
            this.stLineLabel.Size = new System.Drawing.Size(0, 17);
            // 
            // stColumLabel
            // 
            this.stColumLabel.Name = "stColumLabel";
            this.stColumLabel.Size = new System.Drawing.Size(0, 17);
            // 
            // stKeyMod
            // 
            this.stKeyMod.Name = "stKeyMod";
            this.stKeyMod.Size = new System.Drawing.Size(0, 17);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuFile,
            this.mnuEdit,
            this.mnuPlugins,
            this.mnuHelp});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(460, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // mnuFile
            // 
            this.mnuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuNew,
            this.toolStripSeparator1,
            this.mnuOpen,
            this.mnuSave,
            this.mnuSaveAs,
            this.toolStripSeparator2,
            this.mnuPrint,
            this.toolStripSeparator9,
            this.mnuRecentFiles,
            this.toolStripSeparator11,
            this.mnuExit});
            this.mnuFile.Name = "mnuFile";
            this.mnuFile.Size = new System.Drawing.Size(35, 20);
            this.mnuFile.Text = "&File";
            // 
            // mnuNew
            // 
            this.mnuNew.Image = global::FireEdit.Properties.Resources._new;
            this.mnuNew.Name = "mnuNew";
            this.mnuNew.Size = new System.Drawing.Size(143, 22);
            this.mnuNew.Text = "&New";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(140, 6);
            // 
            // mnuOpen
            // 
            this.mnuOpen.Image = global::FireEdit.Properties.Resources.open;
            this.mnuOpen.Name = "mnuOpen";
            this.mnuOpen.Size = new System.Drawing.Size(143, 22);
            this.mnuOpen.Text = "&Open";
            this.mnuOpen.Click += new System.EventHandler(this.mnuOpen_Click);
            // 
            // mnuSave
            // 
            this.mnuSave.Image = global::FireEdit.Properties.Resources.save;
            this.mnuSave.Name = "mnuSave";
            this.mnuSave.Size = new System.Drawing.Size(143, 22);
            this.mnuSave.Text = "&Save";
            this.mnuSave.Click += new System.EventHandler(this.mnuSave_Click);
            // 
            // mnuSaveAs
            // 
            this.mnuSaveAs.Image = global::FireEdit.Properties.Resources.save_as;
            this.mnuSaveAs.Name = "mnuSaveAs";
            this.mnuSaveAs.Size = new System.Drawing.Size(143, 22);
            this.mnuSaveAs.Text = "Save &As";
            this.mnuSaveAs.Click += new System.EventHandler(this.mnuSaveAs_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(140, 6);
            // 
            // mnuPrint
            // 
            this.mnuPrint.Image = global::FireEdit.Properties.Resources.printer;
            this.mnuPrint.Name = "mnuPrint";
            this.mnuPrint.Size = new System.Drawing.Size(143, 22);
            this.mnuPrint.Text = "&Print";
            this.mnuPrint.Click += new System.EventHandler(this.mnuPrint_Click);
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(140, 6);
            // 
            // mnuRecentFiles
            // 
            this.mnuRecentFiles.Name = "mnuRecentFiles";
            this.mnuRecentFiles.Size = new System.Drawing.Size(143, 22);
            this.mnuRecentFiles.Text = "Recent Files";
            // 
            // toolStripSeparator11
            // 
            this.toolStripSeparator11.Name = "toolStripSeparator11";
            this.toolStripSeparator11.Size = new System.Drawing.Size(140, 6);
            // 
            // mnuExit
            // 
            this.mnuExit.Name = "mnuExit";
            this.mnuExit.Size = new System.Drawing.Size(143, 22);
            this.mnuExit.Text = "&Exit";
            this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
            // 
            // mnuEdit
            // 
            this.mnuEdit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuUndo,
            this.mnuRedo,
            this.toolStripSeparator3,
            this.mnuCut,
            this.mnuCopy,
            this.mnuPaste,
            this.toolStripSeparator4,
            this.mnuSelectAll,
            this.toolStripSeparator13,
            this.mnuSyntaxOptions});
            this.mnuEdit.Name = "mnuEdit";
            this.mnuEdit.Size = new System.Drawing.Size(37, 20);
            this.mnuEdit.Text = "&Edit";
            this.mnuEdit.DropDownOpening += new System.EventHandler(this.mnuEdit_DropDownOpening);
            // 
            // mnuUndo
            // 
            this.mnuUndo.Image = global::FireEdit.Properties.Resources.undo;
            this.mnuUndo.Name = "mnuUndo";
            this.mnuUndo.Size = new System.Drawing.Size(159, 22);
            this.mnuUndo.Text = "&Undo";
            this.mnuUndo.Click += new System.EventHandler(this.mnuUndo_Click);
            // 
            // mnuRedo
            // 
            this.mnuRedo.Image = global::FireEdit.Properties.Resources.redo;
            this.mnuRedo.Name = "mnuRedo";
            this.mnuRedo.Size = new System.Drawing.Size(159, 22);
            this.mnuRedo.Text = "&Redo";
            this.mnuRedo.Click += new System.EventHandler(this.mnuRedo_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(156, 6);
            // 
            // mnuCut
            // 
            this.mnuCut.Image = global::FireEdit.Properties.Resources.cut;
            this.mnuCut.Name = "mnuCut";
            this.mnuCut.Size = new System.Drawing.Size(159, 22);
            this.mnuCut.Text = "&Taglia";
            this.mnuCut.Click += new System.EventHandler(this.mnuCut_Click);
            // 
            // mnuCopy
            // 
            this.mnuCopy.Image = global::FireEdit.Properties.Resources.copy;
            this.mnuCopy.Name = "mnuCopy";
            this.mnuCopy.Size = new System.Drawing.Size(159, 22);
            this.mnuCopy.Text = "&Copia";
            this.mnuCopy.Click += new System.EventHandler(this.mnuCopy_Click);
            // 
            // mnuPaste
            // 
            this.mnuPaste.Image = global::FireEdit.Properties.Resources.paste;
            this.mnuPaste.Name = "mnuPaste";
            this.mnuPaste.Size = new System.Drawing.Size(159, 22);
            this.mnuPaste.Text = "&Paste";
            this.mnuPaste.Click += new System.EventHandler(this.mnuPaste_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(156, 6);
            // 
            // mnuSelectAll
            // 
            this.mnuSelectAll.Image = global::FireEdit.Properties.Resources.selection;
            this.mnuSelectAll.Name = "mnuSelectAll";
            this.mnuSelectAll.Size = new System.Drawing.Size(159, 22);
            this.mnuSelectAll.Text = "&Select All";
            this.mnuSelectAll.Click += new System.EventHandler(this.mnuSelectAll_Click);
            // 
            // toolStripSeparator13
            // 
            this.toolStripSeparator13.Name = "toolStripSeparator13";
            this.toolStripSeparator13.Size = new System.Drawing.Size(156, 6);
            // 
            // mnuSyntaxOptions
            // 
            this.mnuSyntaxOptions.Name = "mnuSyntaxOptions";
            this.mnuSyntaxOptions.Size = new System.Drawing.Size(159, 22);
            this.mnuSyntaxOptions.Text = "Syntax &Options";
            this.mnuSyntaxOptions.Click += new System.EventHandler(this.mnuSyntaxOptions_Click);
            // 
            // mnuPlugins
            // 
            this.mnuPlugins.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuConfigurePlugins,
            this.toolStripSeparator12});
            this.mnuPlugins.Name = "mnuPlugins";
            this.mnuPlugins.Size = new System.Drawing.Size(52, 20);
            this.mnuPlugins.Text = "&Plugins";
            // 
            // mnuConfigurePlugins
            // 
            this.mnuConfigurePlugins.Name = "mnuConfigurePlugins";
            this.mnuConfigurePlugins.Size = new System.Drawing.Size(168, 22);
            this.mnuConfigurePlugins.Text = "&Configure Plugins";
            this.mnuConfigurePlugins.Click += new System.EventHandler(this.mnuConfigurePlugins_Click);
            // 
            // toolStripSeparator12
            // 
            this.toolStripSeparator12.Name = "toolStripSeparator12";
            this.toolStripSeparator12.Size = new System.Drawing.Size(165, 6);
            // 
            // mnuHelp
            // 
            this.mnuHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuAbout});
            this.mnuHelp.Name = "mnuHelp";
            this.mnuHelp.Size = new System.Drawing.Size(40, 20);
            this.mnuHelp.Text = "&Help";
            // 
            // mnuAbout
            // 
            this.mnuAbout.Name = "mnuAbout";
            this.mnuAbout.Size = new System.Drawing.Size(114, 22);
            this.mnuAbout.Text = "&About";
            this.mnuAbout.Click += new System.EventHandler(this.mnuAbout_Click);
            // 
            // mainToolBar
            // 
            this.mainToolBar.Dock = System.Windows.Forms.DockStyle.None;
            this.mainToolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnNew,
            this.btnOpen,
            this.toolStripSeparator5,
            this.btnSave,
            this.btnSaveAs,
            this.toolStripSeparator10,
            this.btnPrint,
            this.toolStripSeparator6,
            this.btnCut,
            this.btnCopy,
            this.btnPaste,
            this.toolStripSeparator7,
            this.btnUndo,
            this.btnRedo,
            this.toolStripSeparator8,
            this.btnFind,
            this.btnFindNext,
            this.btnReplace});
            this.mainToolBar.Location = new System.Drawing.Point(3, 24);
            this.mainToolBar.Name = "mainToolBar";
            this.mainToolBar.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.mainToolBar.Size = new System.Drawing.Size(378, 25);
            this.mainToolBar.TabIndex = 2;
            this.mainToolBar.Text = "toolStrip1";
            // 
            // btnNew
            // 
            this.btnNew.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnNew.Image = global::FireEdit.Properties.Resources._new;
            this.btnNew.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnNew.Name = "btnNew";
            this.btnNew.Size = new System.Drawing.Size(29, 22);
            this.btnNew.Text = "New File";
            // 
            // btnOpen
            // 
            this.btnOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnOpen.Image = global::FireEdit.Properties.Resources.open;
            this.btnOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(23, 22);
            this.btnOpen.Text = "Open File";
            this.btnOpen.Click += new System.EventHandler(this.btnOpen_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
            // 
            // btnSave
            // 
            this.btnSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSave.Image = global::FireEdit.Properties.Resources.save;
            this.btnSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(23, 22);
            this.btnSave.Text = "Save";
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnSaveAs
            // 
            this.btnSaveAs.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSaveAs.Image = global::FireEdit.Properties.Resources.save_as;
            this.btnSaveAs.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSaveAs.Name = "btnSaveAs";
            this.btnSaveAs.Size = new System.Drawing.Size(23, 22);
            this.btnSaveAs.Text = "Save As";
            this.btnSaveAs.Click += new System.EventHandler(this.btnSaveAs_Click);
            // 
            // toolStripSeparator10
            // 
            this.toolStripSeparator10.Name = "toolStripSeparator10";
            this.toolStripSeparator10.Size = new System.Drawing.Size(6, 25);
            // 
            // btnPrint
            // 
            this.btnPrint.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPrint.Image = global::FireEdit.Properties.Resources.printer;
            this.btnPrint.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnPrint.Name = "btnPrint";
            this.btnPrint.Size = new System.Drawing.Size(23, 22);
            this.btnPrint.Text = "Print";
            this.btnPrint.Click += new System.EventHandler(this.btnPrint_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(6, 25);
            // 
            // btnCut
            // 
            this.btnCut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnCut.Image = global::FireEdit.Properties.Resources.cut;
            this.btnCut.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCut.Name = "btnCut";
            this.btnCut.Size = new System.Drawing.Size(23, 22);
            this.btnCut.Text = "Cut";
            this.btnCut.Click += new System.EventHandler(this.btnCut_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnCopy.Image = global::FireEdit.Properties.Resources.copy;
            this.btnCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(23, 22);
            this.btnCopy.Text = "Copy";
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // btnPaste
            // 
            this.btnPaste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPaste.Enabled = false;
            this.btnPaste.Image = global::FireEdit.Properties.Resources.paste;
            this.btnPaste.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnPaste.Name = "btnPaste";
            this.btnPaste.Size = new System.Drawing.Size(23, 22);
            this.btnPaste.Text = "Paste";
            this.btnPaste.Click += new System.EventHandler(this.btnPaste_Click);
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(6, 25);
            // 
            // btnUndo
            // 
            this.btnUndo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnUndo.Image = global::FireEdit.Properties.Resources.undo;
            this.btnUndo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnUndo.Name = "btnUndo";
            this.btnUndo.Size = new System.Drawing.Size(23, 22);
            this.btnUndo.Text = "Undo";
            this.btnUndo.Click += new System.EventHandler(this.btnUndo_Click);
            // 
            // btnRedo
            // 
            this.btnRedo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRedo.Image = global::FireEdit.Properties.Resources.redo;
            this.btnRedo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnRedo.Name = "btnRedo";
            this.btnRedo.Size = new System.Drawing.Size(23, 22);
            this.btnRedo.Text = "Redo";
            this.btnRedo.Click += new System.EventHandler(this.btnRedo_Click);
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(6, 25);
            // 
            // btnFind
            // 
            this.btnFind.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnFind.Image = global::FireEdit.Properties.Resources.find;
            this.btnFind.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnFind.Name = "btnFind";
            this.btnFind.Size = new System.Drawing.Size(23, 22);
            this.btnFind.Text = "Find";
            this.btnFind.Click += new System.EventHandler(this.btnFind_Click);
            // 
            // btnFindNext
            // 
            this.btnFindNext.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnFindNext.Image = global::FireEdit.Properties.Resources.find_next;
            this.btnFindNext.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnFindNext.Name = "btnFindNext";
            this.btnFindNext.Size = new System.Drawing.Size(23, 22);
            this.btnFindNext.Text = "FindNext";
            this.btnFindNext.Click += new System.EventHandler(this.btnFindNext_Click);
            // 
            // btnReplace
            // 
            this.btnReplace.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnReplace.Image = global::FireEdit.Properties.Resources.replace;
            this.btnReplace.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnReplace.Name = "btnReplace";
            this.btnReplace.Size = new System.Drawing.Size(23, 22);
            this.btnReplace.Text = "Replace";
            this.btnReplace.Click += new System.EventHandler(this.btnReplace_Click);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.BottomToolStripPanel
            // 
            this.toolStripContainer1.BottomToolStripPanel.Controls.Add(this.statusStrip1);
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.AutoScroll = true;
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(460, 386);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(460, 457);
            this.toolStripContainer1.TabIndex = 3;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.menuStrip1);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.mainToolBar);
            // 
            // syntaxDocument1
            // 
            this.syntaxDocument1.Lines = new string[] {
        ""};
            this.syntaxDocument1.MaxUndoBufferSize = 1000;
            this.syntaxDocument1.Modified = false;
            this.syntaxDocument1.UndoStep = 0;
            // 
            // FormMain
            // 
            this.ClientSize = new System.Drawing.Size(460, 457);
            this.Controls.Add(this.toolStripContainer1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FormMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "FireEdit - [Blank]";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.mainToolBar.ResumeLayout(false);
            this.mainToolBar.PerformLayout();
            this.toolStripContainer1.BottomToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.BottomToolStripPanel.PerformLayout();
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStrip mainToolBar;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private Fireball.Syntax.SyntaxDocument syntaxDocument1;
        private System.Windows.Forms.ToolStripMenuItem mnuFile;
        private System.Windows.Forms.ToolStripMenuItem mnuNew;
        private System.Windows.Forms.ToolStripMenuItem mnuOpen;
        private System.Windows.Forms.ToolStripMenuItem mnuSave;
        private System.Windows.Forms.ToolStripMenuItem mnuSaveAs;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem mnuExit;
        private System.Windows.Forms.ToolStripMenuItem mnuEdit;
        private System.Windows.Forms.ToolStripMenuItem mnuUndo;
        private System.Windows.Forms.ToolStripMenuItem mnuRedo;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem mnuCut;
        private System.Windows.Forms.ToolStripMenuItem mnuCopy;
        private System.Windows.Forms.ToolStripMenuItem mnuPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem mnuSelectAll;
        private System.Windows.Forms.ToolStripButton btnOpen;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripButton btnSave;
        private System.Windows.Forms.ToolStripButton btnSaveAs;
        private System.Windows.Forms.ToolStripDropDownButton btnNew;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripButton btnCut;
        private System.Windows.Forms.ToolStripButton btnCopy;
        private System.Windows.Forms.ToolStripButton btnPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
        private System.Windows.Forms.ToolStripButton btnUndo;
        private System.Windows.Forms.ToolStripButton btnRedo;
        private System.Windows.Forms.ToolStripStatusLabel stFileLabel;
        private System.Windows.Forms.ToolStripStatusLabel stKeyMod;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
        private System.Windows.Forms.ToolStripButton btnFind;
        private System.Windows.Forms.ToolStripButton btnFindNext;
        private System.Windows.Forms.ToolStripButton btnReplace;
        private System.Windows.Forms.ToolStripStatusLabel stLineLabel;
        private System.Windows.Forms.ToolStripStatusLabel stColumLabel;
        private System.Windows.Forms.ToolStripMenuItem mnuPrint;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator9;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator10;
        private System.Windows.Forms.ToolStripButton btnPrint;
        private System.Windows.Forms.ToolStripMenuItem mnuHelp;
        private System.Windows.Forms.ToolStripMenuItem mnuAbout;
        private System.Windows.Forms.ToolStripMenuItem mnuRecentFiles;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator11;
        private System.Windows.Forms.ToolStripMenuItem mnuPlugins;
        private System.Windows.Forms.ToolStripMenuItem mnuConfigurePlugins;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator12;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator13;
        private System.Windows.Forms.ToolStripMenuItem mnuSyntaxOptions;
    }
}

