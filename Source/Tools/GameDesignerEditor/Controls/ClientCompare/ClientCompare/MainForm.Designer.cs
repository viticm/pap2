namespace ClientCompare
{
    partial class MainForm
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
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite8 = new DevComponents.DotNetBar.DockSite();
            this.dockSite5 = new DevComponents.DotNetBar.DockSite();
            this.dockSite6 = new DevComponents.DotNetBar.DockSite();
            this.dockSite7 = new DevComponents.DotNetBar.DockSite();
            this.barMenu = new DevComponents.DotNetBar.Bar();
            this.bFile = new DevComponents.DotNetBar.ButtonItem();
            this.bOpenFolder1 = new DevComponents.DotNetBar.ButtonItem();
            this.bOpenFolder2 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonGenAllDiff = new DevComponents.DotNetBar.ButtonItem();
            this.bEdit = new DevComponents.DotNetBar.ButtonItem();
            this.bSet = new DevComponents.DotNetBar.ButtonItem();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.compareContainer = new System.Windows.Forms.SplitContainer();
            this.textPathLeft = new System.Windows.Forms.TextBox();
            this.leftView = new DevComponents.DotNetBar.Controls.ListViewEx();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.MenuItemDiff = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuItemExtern = new System.Windows.Forms.ToolStripMenuItem();
            this.textPathRight = new System.Windows.Forms.TextBox();
            this.rightView = new DevComponents.DotNetBar.Controls.ListViewEx();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.labelStatus = new DevComponents.DotNetBar.LabelItem();
            this.dockSite7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.barMenu)).BeginInit();
            this.compareContainer.Panel1.SuspendLayout();
            this.compareContainer.Panel2.SuspendLayout();
            this.compareContainer.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.SuspendLayout();
            // 
            // dotNetBarManager1
            // 
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.F1);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlC);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlA);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlV);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlX);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlZ);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlY);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.Del);
            this.dotNetBarManager1.AutoDispatchShortcuts.Add(DevComponents.DotNetBar.eShortcut.Ins);
            this.dotNetBarManager1.BottomDockSite = this.dockSite4;
            this.dotNetBarManager1.DefinitionName = "";
            this.dotNetBarManager1.EnableFullSizeDock = false;
            this.dotNetBarManager1.LeftDockSite = this.dockSite1;
            this.dotNetBarManager1.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.dotNetBarManager1.ParentForm = this;
            this.dotNetBarManager1.RightDockSite = this.dockSite2;
            this.dotNetBarManager1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.dotNetBarManager1.ToolbarBottomDockSite = this.dockSite8;
            this.dotNetBarManager1.ToolbarLeftDockSite = this.dockSite5;
            this.dotNetBarManager1.ToolbarRightDockSite = this.dockSite6;
            this.dotNetBarManager1.ToolbarTopDockSite = this.dockSite7;
            this.dotNetBarManager1.TopDockSite = this.dockSite3;
            // 
            // dockSite4
            // 
            this.dockSite4.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite4.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite4.Location = new System.Drawing.Point(0, 471);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(765, 0);
            this.dockSite4.TabIndex = 3;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite1.Location = new System.Drawing.Point(0, 25);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(0, 446);
            this.dockSite1.TabIndex = 0;
            this.dockSite1.TabStop = false;
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(765, 25);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 446);
            this.dockSite2.TabIndex = 1;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 471);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(765, 0);
            this.dockSite8.TabIndex = 7;
            this.dockSite8.TabStop = false;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 25);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 446);
            this.dockSite5.TabIndex = 4;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(765, 25);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 446);
            this.dockSite6.TabIndex = 5;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Controls.Add(this.barMenu);
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(765, 25);
            this.dockSite7.TabIndex = 6;
            this.dockSite7.TabStop = false;
            // 
            // barMenu
            // 
            this.barMenu.AccessibleDescription = "DotNetBar Bar (barMenu)";
            this.barMenu.AccessibleName = "DotNetBar Bar";
            this.barMenu.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.barMenu.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barMenu.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bFile,
            this.bEdit});
            this.barMenu.Location = new System.Drawing.Point(0, 0);
            this.barMenu.MenuBar = true;
            this.barMenu.Name = "barMenu";
            this.barMenu.Size = new System.Drawing.Size(765, 24);
            this.barMenu.Stretch = true;
            this.barMenu.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barMenu.TabIndex = 0;
            this.barMenu.TabStop = false;
            this.barMenu.Text = "bar1";
            // 
            // bFile
            // 
            this.bFile.ImagePaddingHorizontal = 8;
            this.bFile.Name = "bFile";
            this.bFile.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bOpenFolder1,
            this.bOpenFolder2,
            this.buttonItem1,
            this.buttonGenAllDiff});
            this.bFile.Text = "文件";
            this.bFile.Click += new System.EventHandler(this.bFile_Click);
            // 
            // bOpenFolder1
            // 
            this.bOpenFolder1.ImagePaddingHorizontal = 8;
            this.bOpenFolder1.Name = "bOpenFolder1";
            this.bOpenFolder1.Text = "打开源对比目录   (F1)";
            this.bOpenFolder1.Click += new System.EventHandler(this.bOpenFolder1_Click);
            // 
            // bOpenFolder2
            // 
            this.bOpenFolder2.ImagePaddingHorizontal = 8;
            this.bOpenFolder2.Name = "bOpenFolder2";
            this.bOpenFolder2.Text = "打开目标对比目录 (F2)";
            this.bOpenFolder2.Click += new System.EventHandler(this.bOpenFolder2_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "复制差异文件";
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // buttonGenAllDiff
            // 
            this.buttonGenAllDiff.ImagePaddingHorizontal = 8;
            this.buttonGenAllDiff.Name = "buttonGenAllDiff";
            this.buttonGenAllDiff.Text = "批量生成差异结果";
            this.buttonGenAllDiff.Click += new System.EventHandler(this.buttonGenAllDiff_Click);
            // 
            // bEdit
            // 
            this.bEdit.ImagePaddingHorizontal = 8;
            this.bEdit.Name = "bEdit";
            this.bEdit.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bSet});
            this.bEdit.Text = "工具";
            // 
            // bSet
            // 
            this.bSet.ImagePaddingHorizontal = 8;
            this.bSet.Name = "bSet";
            this.bSet.Text = "配置";
            this.bSet.Click += new System.EventHandler(this.bSet_Click);
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(0, 25);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(765, 0);
            this.dockSite3.TabIndex = 2;
            this.dockSite3.TabStop = false;
            // 
            // compareContainer
            // 
            this.compareContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.compareContainer.Location = new System.Drawing.Point(0, 25);
            this.compareContainer.Name = "compareContainer";
            // 
            // compareContainer.Panel1
            // 
            this.compareContainer.Panel1.Controls.Add(this.textPathLeft);
            this.compareContainer.Panel1.Controls.Add(this.leftView);
            // 
            // compareContainer.Panel2
            // 
            this.compareContainer.Panel2.Controls.Add(this.textPathRight);
            this.compareContainer.Panel2.Controls.Add(this.rightView);
            this.compareContainer.Size = new System.Drawing.Size(765, 419);
            this.compareContainer.SplitterDistance = 380;
            this.compareContainer.TabIndex = 8;
            this.compareContainer.Resize += new System.EventHandler(this.compareContainer_Resize);
            // 
            // textPathLeft
            // 
            this.textPathLeft.Dock = System.Windows.Forms.DockStyle.Top;
            this.textPathLeft.Location = new System.Drawing.Point(0, 0);
            this.textPathLeft.Name = "textPathLeft";
            this.textPathLeft.Size = new System.Drawing.Size(380, 21);
            this.textPathLeft.TabIndex = 1;
            // 
            // leftView
            // 
            this.leftView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.leftView.Border.Class = "ListViewBorder";
            this.leftView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader5});
            this.leftView.ContextMenuStrip = this.contextMenuStrip1;
            this.leftView.FullRowSelect = true;
            this.leftView.Location = new System.Drawing.Point(3, 27);
            this.leftView.MultiSelect = false;
            this.leftView.Name = "leftView";
            this.leftView.Size = new System.Drawing.Size(374, 392);
            this.leftView.TabIndex = 0;
            this.leftView.UseCompatibleStateImageBehavior = false;
            this.leftView.View = System.Windows.Forms.View.Details;
            this.leftView.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.leftView_MouseDoubleClick);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "名称";
            this.columnHeader1.Width = 150;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "大小";
            this.columnHeader2.Width = 100;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "修改时间";
            this.columnHeader5.Width = 150;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.MenuItemDiff,
            this.MenuItemExtern});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(161, 48);
            // 
            // MenuItemDiff
            // 
            this.MenuItemDiff.Name = "MenuItemDiff";
            this.MenuItemDiff.Size = new System.Drawing.Size(160, 22);
            this.MenuItemDiff.Text = "查看差异(&D)";
            this.MenuItemDiff.Click += new System.EventHandler(this.MenuItemDiff_Click);
            // 
            // MenuItemExtern
            // 
            this.MenuItemExtern.Name = "MenuItemExtern";
            this.MenuItemExtern.Size = new System.Drawing.Size(160, 22);
            this.MenuItemExtern.Text = "使用外部程序(&E)";
            this.MenuItemExtern.Click += new System.EventHandler(this.MenuItemExtern_Click);
            // 
            // textPathRight
            // 
            this.textPathRight.Dock = System.Windows.Forms.DockStyle.Top;
            this.textPathRight.Location = new System.Drawing.Point(0, 0);
            this.textPathRight.Name = "textPathRight";
            this.textPathRight.Size = new System.Drawing.Size(381, 21);
            this.textPathRight.TabIndex = 1;
            this.textPathRight.TextChanged += new System.EventHandler(this.textBox2_TextChanged);
            // 
            // rightView
            // 
            this.rightView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.rightView.Border.Class = "ListViewBorder";
            this.rightView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader3,
            this.columnHeader4,
            this.columnHeader6});
            this.rightView.FullRowSelect = true;
            this.rightView.Location = new System.Drawing.Point(3, 27);
            this.rightView.MultiSelect = false;
            this.rightView.Name = "rightView";
            this.rightView.Size = new System.Drawing.Size(375, 392);
            this.rightView.TabIndex = 0;
            this.rightView.UseCompatibleStateImageBehavior = false;
            this.rightView.View = System.Windows.Forms.View.Details;
            this.rightView.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.rightView_MouseDoubleClick);
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "名称";
            this.columnHeader3.Width = 150;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "大小";
            this.columnHeader4.Width = 100;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "修改时间";
            this.columnHeader6.Width = 150;
            // 
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.bar1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.labelStatus});
            this.bar1.Location = new System.Drawing.Point(0, 455);
            this.bar1.MenuBar = true;
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(765, 16);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 9;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // labelStatus
            // 
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Text = "labelItem1";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(765, 471);
            this.Controls.Add(this.bar1);
            this.Controls.Add(this.compareContainer);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.KeyPreview = true;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "客户端差异比较";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
            this.dockSite7.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.barMenu)).EndInit();
            this.compareContainer.Panel1.ResumeLayout(false);
            this.compareContainer.Panel1.PerformLayout();
            this.compareContainer.Panel2.ResumeLayout(false);
            this.compareContainer.Panel2.PerformLayout();
            this.compareContainer.ResumeLayout(false);
            this.contextMenuStrip1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
        private DevComponents.DotNetBar.DockSite dockSite4;
        private DevComponents.DotNetBar.DockSite dockSite1;
        private DevComponents.DotNetBar.DockSite dockSite2;
        private DevComponents.DotNetBar.DockSite dockSite3;
        private DevComponents.DotNetBar.DockSite dockSite5;
        private DevComponents.DotNetBar.DockSite dockSite6;
        private DevComponents.DotNetBar.DockSite dockSite7;
        private DevComponents.DotNetBar.Bar barMenu;
        private DevComponents.DotNetBar.ButtonItem bFile;
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.ButtonItem bOpenFolder1;
        private DevComponents.DotNetBar.ButtonItem bOpenFolder2;
        private DevComponents.DotNetBar.ButtonItem bEdit;
        private DevComponents.DotNetBar.ButtonItem bSet;
        private System.Windows.Forms.SplitContainer compareContainer;
        private DevComponents.DotNetBar.Controls.ListViewEx leftView;
        private DevComponents.DotNetBar.Controls.ListViewEx rightView;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.LabelItem labelStatus;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem MenuItemDiff;
        private System.Windows.Forms.ToolStripMenuItem MenuItemExtern;
        private DevComponents.DotNetBar.ButtonItem buttonGenAllDiff;
        private System.Windows.Forms.TextBox textPathLeft;
        private System.Windows.Forms.TextBox textPathRight;
    }
}