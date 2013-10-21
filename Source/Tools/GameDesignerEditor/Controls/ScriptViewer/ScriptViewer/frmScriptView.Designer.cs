namespace ScriptViewer
{
    partial class frmScriptView
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.bar2 = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer1 = new DevComponents.DotNetBar.PanelDockContainer();
            this.scriptTree1 = new ScriptViewer.scriptTree();
            this.dockContainerItem1 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite8 = new DevComponents.DotNetBar.DockSite();
            this.dockSite5 = new DevComponents.DotNetBar.DockSite();
            this.dockSite6 = new DevComponents.DotNetBar.DockSite();
            this.dockSite7 = new DevComponents.DotNetBar.DockSite();
            this.bar3 = new DevComponents.DotNetBar.Bar();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.btnSave = new DevComponents.DotNetBar.ButtonItem();
            this.btnSaveAll = new DevComponents.DotNetBar.ButtonItem();
            this.btnClose = new DevComponents.DotNetBar.ButtonItem();
            this.btnCloseAll = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.btnHistory = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemFind = new DevComponents.DotNetBar.ButtonItem();
            this.btnFind = new DevComponents.DotNetBar.ButtonItem();
            this.editButton = new DevComponents.DotNetBar.ButtonItem();
            this.fontButton = new DevComponents.DotNetBar.ButtonItem();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.tabStrip1 = new DevComponents.DotNetBar.TabStrip();
            this.controlContainerItem1 = new DevComponents.DotNetBar.ControlContainerItem();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.dockContainerItem2 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar2)).BeginInit();
            this.bar2.SuspendLayout();
            this.panelDockContainer1.SuspendLayout();
            this.dockSite7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.bar1.SuspendLayout();
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
            this.dotNetBarManager1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.dotNetBarManager1.ToolbarBottomDockSite = this.dockSite8;
            this.dotNetBarManager1.ToolbarLeftDockSite = this.dockSite5;
            this.dotNetBarManager1.ToolbarRightDockSite = this.dockSite6;
            this.dotNetBarManager1.ToolbarTopDockSite = this.dockSite7;
            this.dotNetBarManager1.TopDockSite = this.dockSite3;
            this.dotNetBarManager1.ItemClick += new System.EventHandler(this.dotNetBarManager1_ItemClick);
            // 
            // dockSite4
            // 
            this.dockSite4.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite4.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite4.Location = new System.Drawing.Point(0, 452);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(620, 0);
            this.dockSite4.TabIndex = 7;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Controls.Add(this.bar2);
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.bar2, 191, 399)))}, DevComponents.DotNetBar.eOrientation.Horizontal);
            this.dockSite1.Location = new System.Drawing.Point(0, 53);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(194, 399);
            this.dockSite1.TabIndex = 4;
            this.dockSite1.TabStop = false;
            // 
            // bar2
            // 
            this.bar2.AccessibleDescription = "DotNetBar Bar (bar2)";
            this.bar2.AccessibleName = "DotNetBar Bar";
            this.bar2.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.bar2.AutoSyncBarCaption = true;
            this.bar2.Controls.Add(this.panelDockContainer1);
            this.bar2.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.bar2.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem1});
            this.bar2.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.bar2.Location = new System.Drawing.Point(0, 0);
            this.bar2.Name = "bar2";
            this.bar2.Size = new System.Drawing.Size(191, 399);
            this.bar2.Stretch = true;
            this.bar2.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar2.TabIndex = 0;
            this.bar2.TabStop = false;
            this.bar2.Text = "全部脚本";
            // 
            // panelDockContainer1
            // 
            this.panelDockContainer1.Controls.Add(this.scriptTree1);
            this.panelDockContainer1.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer1.Name = "panelDockContainer1";
            this.panelDockContainer1.Size = new System.Drawing.Size(185, 373);
            this.panelDockContainer1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer1.Style.GradientAngle = 90;
            this.panelDockContainer1.TabIndex = 0;
            // 
            // scriptTree1
            // 
            this.scriptTree1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.scriptTree1.Location = new System.Drawing.Point(0, 0);
            this.scriptTree1.Name = "scriptTree1";
            this.scriptTree1.Size = new System.Drawing.Size(185, 373);
            this.scriptTree1.TabIndex = 1;
            this.scriptTree1.SelectedScriptNode += new ScriptViewer.scriptTree.treeEventHandler(this.scriptTree1_SelectedScriptNode);
            // 
            // dockContainerItem1
            // 
            this.dockContainerItem1.Control = this.panelDockContainer1;
            this.dockContainerItem1.Name = "dockContainerItem1";
            this.dockContainerItem1.Text = "全部脚本";
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(620, 53);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 399);
            this.dockSite2.TabIndex = 5;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 452);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(620, 0);
            this.dockSite8.TabIndex = 11;
            this.dockSite8.TabStop = false;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 53);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 399);
            this.dockSite5.TabIndex = 8;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(620, 53);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 399);
            this.dockSite6.TabIndex = 9;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Controls.Add(this.bar3);
            this.dockSite7.Controls.Add(this.bar1);
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(620, 53);
            this.dockSite7.TabIndex = 10;
            this.dockSite7.TabStop = false;
            // 
            // bar3
            // 
            this.bar3.AccessibleDescription = "DotNetBar Bar (bar3)";
            this.bar3.AccessibleName = "DotNetBar Bar";
            this.bar3.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.bar3.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.bar3.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem2,
            this.buttonItem1,
            this.buttonItemFind,
            this.editButton});
            this.bar3.Location = new System.Drawing.Point(0, 0);
            this.bar3.MenuBar = true;
            this.bar3.Name = "bar3";
            this.bar3.Size = new System.Drawing.Size(620, 24);
            this.bar3.Stretch = true;
            this.bar3.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar3.TabIndex = 1;
            this.bar3.TabStop = false;
            this.bar3.Text = "bar3";
            // 
            // buttonItem2
            // 
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnSave,
            this.btnSaveAll,
            this.btnClose,
            this.btnCloseAll});
            this.buttonItem2.Text = "文件(&F)";
            // 
            // btnSave
            // 
            this.btnSave.ImagePaddingHorizontal = 8;
            this.btnSave.Name = "btnSave";
            this.btnSave.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.btnSave.Text = "保存(&S)   ";
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnSaveAll
            // 
            this.btnSaveAll.ImagePaddingHorizontal = 8;
            this.btnSaveAll.Name = "btnSaveAll";
            this.btnSaveAll.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlShiftS);
            this.btnSaveAll.Text = "全部保存(&A)       ";
            this.btnSaveAll.Click += new System.EventHandler(this.btnSaveAll_Click);
            // 
            // btnClose
            // 
            this.btnClose.BeginGroup = true;
            this.btnClose.ImagePaddingHorizontal = 8;
            this.btnClose.Name = "btnClose";
            this.btnClose.Text = "关闭当前(&C)";
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // btnCloseAll
            // 
            this.btnCloseAll.ImagePaddingHorizontal = 8;
            this.btnCloseAll.Name = "btnCloseAll";
            this.btnCloseAll.Text = "全部关闭(&X)";
            this.btnCloseAll.Click += new System.EventHandler(this.btnCloseAll_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnHistory});
            this.buttonItem1.Text = "历史(&H)";
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // btnHistory
            // 
            this.btnHistory.ImagePaddingHorizontal = 8;
            this.btnHistory.Name = "btnHistory";
            this.btnHistory.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F4);
            this.btnHistory.Text = "查看历史(&E)...  ";
            this.btnHistory.Click += new System.EventHandler(this.btnHistory_Click);
            // 
            // buttonItemFind
            // 
            this.buttonItemFind.ImagePaddingHorizontal = 8;
            this.buttonItemFind.Name = "buttonItemFind";
            this.buttonItemFind.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnFind});
            this.buttonItemFind.Text = "查找(&S)";
            // 
            // btnFind
            // 
            this.btnFind.ImagePaddingHorizontal = 8;
            this.btnFind.Name = "btnFind";
            this.btnFind.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F2);
            this.btnFind.Text = "查找(&F)...  ";
            this.btnFind.Click += new System.EventHandler(this.btnFind_Click);
            // 
            // editButton
            // 
            this.editButton.ImagePaddingHorizontal = 8;
            this.editButton.Name = "editButton";
            this.editButton.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.fontButton});
            this.editButton.Text = "编辑(&E)";
            // 
            // fontButton
            // 
            this.fontButton.ImagePaddingHorizontal = 8;
            this.fontButton.Name = "fontButton";
            this.fontButton.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F4);
            this.fontButton.Text = "字体";
            this.fontButton.Click += new System.EventHandler(this.fontButton_Click);
            // 
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.bar1.Controls.Add(this.tabStrip1);
            this.bar1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.bar1.DockLine = 1;
            this.bar1.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.controlContainerItem1});
            this.bar1.Location = new System.Drawing.Point(0, 26);
            this.bar1.MenuBar = true;
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(620, 27);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 0;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // tabStrip1
            // 
            this.tabStrip1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabStrip1.AutoSelectAttachedControl = true;
            this.tabStrip1.CanReorderTabs = true;
            this.tabStrip1.CloseButtonOnTabsAlwaysDisplayed = false;
            this.tabStrip1.CloseButtonPosition = DevComponents.DotNetBar.eTabCloseButtonPosition.Right;
            this.tabStrip1.CloseButtonVisible = true;
            this.tabStrip1.Location = new System.Drawing.Point(3, 2);
            this.tabStrip1.MdiForm = this;
            this.tabStrip1.MdiTabbedDocuments = true;
            this.tabStrip1.Name = "tabStrip1";
            this.tabStrip1.SelectedTab = null;
            this.tabStrip1.SelectedTabFont = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.tabStrip1.Size = new System.Drawing.Size(614, 23);
            this.tabStrip1.Style = DevComponents.DotNetBar.eTabStripStyle.Office2007Document;
            this.tabStrip1.TabAlignment = DevComponents.DotNetBar.eTabStripAlignment.Top;
            this.tabStrip1.TabIndex = 12;
            this.tabStrip1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabStrip1.Text = "tabStrip1";
            this.tabStrip1.SelectedTabChanged += new DevComponents.DotNetBar.TabStrip.SelectedTabChangedEventHandler(this.tabStrip1_SelectedTabChanged);
            // 
            // controlContainerItem1
            // 
            this.controlContainerItem1.AllowItemResize = false;
            this.controlContainerItem1.Control = this.tabStrip1;
            this.controlContainerItem1.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleAlways;
            this.controlContainerItem1.Name = "controlContainerItem1";
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(0, 53);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(620, 0);
            this.dockSite3.TabIndex = 6;
            this.dockSite3.TabStop = false;
            // 
            // tabItem1
            // 
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "tabItem1";
            // 
            // dockContainerItem2
            // 
            this.dockContainerItem2.Name = "dockContainerItem2";
            this.dockContainerItem2.Text = "dockContainerItem2";
            // 
            // frmScriptView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(620, 452);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.IsMdiContainer = true;
            this.Name = "frmScriptView";
            this.Text = "脚本编辑器";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmScriptView_FormClosing);
            this.dockSite1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar2)).EndInit();
            this.bar2.ResumeLayout(false);
            this.panelDockContainer1.ResumeLayout(false);
            this.dockSite7.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.bar1.ResumeLayout(false);
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
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.Bar bar2;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer1;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem1;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.TabStrip tabStrip1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem2;
        private DevComponents.DotNetBar.Bar bar3;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private DevComponents.DotNetBar.ControlContainerItem controlContainerItem1;
        private DevComponents.DotNetBar.ButtonItem btnClose;
        private DevComponents.DotNetBar.ButtonItem btnCloseAll;
        private DevComponents.DotNetBar.ButtonItem btnSave;
        private DevComponents.DotNetBar.ButtonItem btnSaveAll;
        public scriptTree scriptTree1;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem btnHistory;
        private DevComponents.DotNetBar.ButtonItem buttonItemFind;
        private DevComponents.DotNetBar.ButtonItem btnFind;
        private DevComponents.DotNetBar.ButtonItem editButton;
        private DevComponents.DotNetBar.ButtonItem fontButton;
    }
}

