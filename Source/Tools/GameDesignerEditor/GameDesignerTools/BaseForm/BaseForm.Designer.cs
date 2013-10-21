namespace GameDesingerTools
{
    partial class BaseForm
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
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Node0", -2, -2);
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Node1");
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Node4");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("Node6");
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("Node5", new System.Windows.Forms.TreeNode[] {
            treeNode4});
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("Node2", new System.Windows.Forms.TreeNode[] {
            treeNode3,
            treeNode5});
            System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode("Node3");
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer1 = new DevComponents.DotNetBar.PanelDockContainer();
            this.baseTree = new System.Windows.Forms.TreeView();
            this.dockContainerItem1 = new DevComponents.DotNetBar.DockContainerItem();
            this.PreviewBar = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer2 = new DevComponents.DotNetBar.PanelDockContainer();
            this.PreviewPanel = new DevComponents.DotNetBar.PanelEx();
            this.dockContainerItem2 = new DevComponents.DotNetBar.DockContainerItem();
            this.PreviewPluginBar = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer3 = new DevComponents.DotNetBar.PanelDockContainer();
            this.ContainerPanel = new DevComponents.DotNetBar.PanelEx();
            this.dockContainerItem3 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite8 = new DevComponents.DotNetBar.DockSite();
            this.dockSite5 = new DevComponents.DotNetBar.DockSite();
            this.dockSite6 = new DevComponents.DotNetBar.DockSite();
            this.dockSite7 = new DevComponents.DotNetBar.DockSite();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.BasePanel = new DevComponents.DotNetBar.TabControlPanel();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.bTabContext = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextSave = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextClose = new DevComponents.DotNetBar.ButtonItem();
            this.bTabColor = new DevComponents.DotNetBar.ButtonItem();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSameLevel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSub = new DevComponents.DotNetBar.ButtonItem();
            this.buttonRename = new DevComponents.DotNetBar.ButtonItem();
            this.buttonCopy = new DevComponents.DotNetBar.ButtonItem();
            this.buttonPaste = new DevComponents.DotNetBar.ButtonItem();
            this.buttonDelete = new DevComponents.DotNetBar.ButtonItem();
            this.buttonExpandAll = new DevComponents.DotNetBar.ButtonItem();
            this.buttonReloadTree = new DevComponents.DotNetBar.ButtonItem();
            this.cmScriptOp = new DevComponents.DotNetBar.ButtonItem();
            this.bShowLuaCode = new DevComponents.DotNetBar.ButtonItem();
            this.bShowMetainfo = new DevComponents.DotNetBar.ButtonItem();
            this.bEditFile = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteRecord = new DevComponents.DotNetBar.ButtonItem();
            this.bAddRecord = new DevComponents.DotNetBar.ButtonItem();
            this.bClear = new DevComponents.DotNetBar.ButtonItem();
            this.bClearContent = new DevComponents.DotNetBar.ButtonItem();
            this.bViewLog = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.BasePg = new System.Windows.Forms.PropertyGrid();
            this.BaseTabItem = new DevComponents.DotNetBar.TabItem(this.components);
            this.tmrRender = new System.Windows.Forms.Timer(this.components);
            this.dockSite1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.bar1.SuspendLayout();
            this.panelDockContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PreviewBar)).BeginInit();
            this.PreviewBar.SuspendLayout();
            this.panelDockContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PreviewPluginBar)).BeginInit();
            this.PreviewPluginBar.SuspendLayout();
            this.panelDockContainer3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.BasePanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
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
            this.dockSite4.Location = new System.Drawing.Point(185, 542);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(568, 0);
            this.dockSite4.TabIndex = 7;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Controls.Add(this.bar1);
            this.dockSite1.Controls.Add(this.PreviewBar);
            this.dockSite1.Controls.Add(this.PreviewPluginBar);
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.bar1, 182, 189))),
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.PreviewPluginBar, 182, 207))),
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.PreviewBar, 182, 140)))}, DevComponents.DotNetBar.eOrientation.Vertical);
            this.dockSite1.Location = new System.Drawing.Point(0, 0);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(185, 542);
            this.dockSite1.TabIndex = 4;
            this.dockSite1.TabStop = false;
            // 
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.bar1.CanCustomize = false;
            this.bar1.Controls.Add(this.panelDockContainer1);
            this.bar1.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem1});
            this.bar1.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(182, 189);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 0;
            this.bar1.TabNavigation = true;
            this.bar1.TabStop = false;
            // 
            // panelDockContainer1
            // 
            this.panelDockContainer1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer1.Controls.Add(this.baseTree);
            this.panelDockContainer1.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer1.Name = "panelDockContainer1";
            this.panelDockContainer1.Size = new System.Drawing.Size(176, 163);
            this.panelDockContainer1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer1.Style.GradientAngle = 90;
            this.panelDockContainer1.TabIndex = 0;
            this.panelDockContainer1.Text = "记录内容";
            // 
            // baseTree
            // 
            this.baseTree.AllowDrop = true;
            this.baseTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.baseTree.HideSelection = false;
            this.baseTree.Location = new System.Drawing.Point(0, 0);
            this.baseTree.Name = "baseTree";
            treeNode1.ImageIndex = -2;
            treeNode1.Name = "Node0";
            treeNode1.SelectedImageIndex = -2;
            treeNode1.Text = "Node0";
            treeNode2.Name = "Node1";
            treeNode2.Text = "Node1";
            treeNode3.Name = "Node4";
            treeNode3.Text = "Node4";
            treeNode4.Name = "Node6";
            treeNode4.Text = "Node6";
            treeNode5.Name = "Node5";
            treeNode5.Text = "Node5";
            treeNode6.Name = "Node2";
            treeNode6.Text = "Node2";
            treeNode7.Name = "Node3";
            treeNode7.Text = "Node3";
            this.baseTree.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode6,
            treeNode7});
            this.baseTree.Size = new System.Drawing.Size(176, 163);
            this.baseTree.TabIndex = 1;
            this.baseTree.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.Tree_AfterLabelEdit);
            this.baseTree.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.baseTree_BeforeExpand);
            this.baseTree.DragDrop += new System.Windows.Forms.DragEventHandler(this.baseTree_DragDrop);
            this.baseTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.baseTree_AfterSelect);
            this.baseTree.DragEnter += new System.Windows.Forms.DragEventHandler(this.baseTree_DragEnter);
            this.baseTree.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.TreeNodeClick);
            this.baseTree.BeforeLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.baseTree_BeforeLabelEdit);
            this.baseTree.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.baseTree_BeforeSelect);
            this.baseTree.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.baseTree_ItemDrag);
            // 
            // dockContainerItem1
            // 
            this.dockContainerItem1.Control = this.panelDockContainer1;
            this.dockContainerItem1.Name = "dockContainerItem1";
            this.dockContainerItem1.Text = "记录内容";
            // 
            // PreviewBar
            // 
            this.PreviewBar.AccessibleDescription = "DotNetBar Bar (PreviewBar)";
            this.PreviewBar.AccessibleName = "DotNetBar Bar";
            this.PreviewBar.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.PreviewBar.AutoSyncBarCaption = true;
            this.PreviewBar.Controls.Add(this.panelDockContainer2);
            this.PreviewBar.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.PreviewBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem2});
            this.PreviewBar.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.PreviewBar.Location = new System.Drawing.Point(0, 402);
            this.PreviewBar.Name = "PreviewBar";
            this.PreviewBar.Size = new System.Drawing.Size(182, 140);
            this.PreviewBar.Stretch = true;
            this.PreviewBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.PreviewBar.TabIndex = 1;
            this.PreviewBar.TabStop = false;
            this.PreviewBar.Text = "预览";
            this.PreviewBar.Visible = false;
            // 
            // panelDockContainer2
            // 
            this.panelDockContainer2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer2.Controls.Add(this.PreviewPanel);
            this.panelDockContainer2.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer2.Name = "panelDockContainer2";
            this.panelDockContainer2.Size = new System.Drawing.Size(176, 114);
            this.panelDockContainer2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer2.Style.GradientAngle = 90;
            this.panelDockContainer2.TabIndex = 0;
            // 
            // PreviewPanel
            // 
            this.PreviewPanel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.PreviewPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.PreviewPanel.CausesValidation = false;
            this.PreviewPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PreviewPanel.Location = new System.Drawing.Point(0, 0);
            this.PreviewPanel.Name = "PreviewPanel";
            this.PreviewPanel.Size = new System.Drawing.Size(176, 114);
            this.PreviewPanel.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.PreviewPanel.Style.BackColor1.Alpha = ((byte)(0));
            this.PreviewPanel.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.PreviewPanel.Style.BackColor2.Alpha = ((byte)(0));
            this.PreviewPanel.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.PreviewPanel.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.PreviewPanel.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.PreviewPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.PreviewPanel.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.PreviewPanel.Style.GradientAngle = 90;
            this.PreviewPanel.TabIndex = 0;
            this.PreviewPanel.Visible = false;
            this.PreviewPanel.DoubleClick += new System.EventHandler(this.PreviewPanel_DoubleClick);
            this.PreviewPanel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.PreviewPanel_MouseMove);
            this.PreviewPanel.Resize += new System.EventHandler(this.PreviewPanel_Resize);
            // 
            // dockContainerItem2
            // 
            this.dockContainerItem2.Control = this.panelDockContainer2;
            this.dockContainerItem2.Name = "dockContainerItem2";
            this.dockContainerItem2.Text = "预览";
            // 
            // PreviewPluginBar
            // 
            this.PreviewPluginBar.AccessibleDescription = "DotNetBar Bar (PreviewPluginBar)";
            this.PreviewPluginBar.AccessibleName = "DotNetBar Bar";
            this.PreviewPluginBar.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.PreviewPluginBar.AutoSyncBarCaption = true;
            this.PreviewPluginBar.Controls.Add(this.panelDockContainer3);
            this.PreviewPluginBar.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.PreviewPluginBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem3});
            this.PreviewPluginBar.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.PreviewPluginBar.Location = new System.Drawing.Point(0, 192);
            this.PreviewPluginBar.Name = "PreviewPluginBar";
            this.PreviewPluginBar.Size = new System.Drawing.Size(182, 207);
            this.PreviewPluginBar.Stretch = true;
            this.PreviewPluginBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.PreviewPluginBar.TabIndex = 2;
            this.PreviewPluginBar.TabStop = false;
            this.PreviewPluginBar.Text = "预览插件";
            this.PreviewPluginBar.Visible = false;
            // 
            // panelDockContainer3
            // 
            this.panelDockContainer3.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer3.Controls.Add(this.ContainerPanel);
            this.panelDockContainer3.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer3.Name = "panelDockContainer3";
            this.panelDockContainer3.Size = new System.Drawing.Size(176, 181);
            this.panelDockContainer3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer3.Style.GradientAngle = 90;
            this.panelDockContainer3.TabIndex = 0;
            // 
            // ContainerPanel
            // 
            this.ContainerPanel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ContainerPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.ContainerPanel.CausesValidation = false;
            this.ContainerPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ContainerPanel.Location = new System.Drawing.Point(0, 0);
            this.ContainerPanel.Name = "ContainerPanel";
            this.ContainerPanel.Size = new System.Drawing.Size(176, 181);
            this.ContainerPanel.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.ContainerPanel.Style.BackColor1.Alpha = ((byte)(0));
            this.ContainerPanel.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.ContainerPanel.Style.BackColor2.Alpha = ((byte)(0));
            this.ContainerPanel.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.ContainerPanel.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.ContainerPanel.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.ContainerPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.ContainerPanel.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.ContainerPanel.Style.GradientAngle = 90;
            this.ContainerPanel.TabIndex = 0;
            this.ContainerPanel.Visible = false;
            // 
            // dockContainerItem3
            // 
            this.dockContainerItem3.Control = this.panelDockContainer3;
            this.dockContainerItem3.Name = "dockContainerItem3";
            this.dockContainerItem3.Text = "预览插件";
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(753, 0);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 542);
            this.dockSite2.TabIndex = 5;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 542);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(753, 0);
            this.dockSite8.TabIndex = 12;
            this.dockSite8.TabStop = false;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 0);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 542);
            this.dockSite5.TabIndex = 9;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(753, 0);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 542);
            this.dockSite6.TabIndex = 10;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(753, 0);
            this.dockSite7.TabIndex = 11;
            this.dockSite7.TabStop = false;
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(185, 0);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(568, 0);
            this.dockSite3.TabIndex = 6;
            this.dockSite3.TabStop = false;
            // 
            // tabControl1
            // 
            this.tabControl1.CanReorderTabs = false;
            this.tabControl1.Controls.Add(this.BasePanel);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(185, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(568, 542);
            this.tabControl1.TabIndex = 8;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.BaseTabItem);
            this.tabControl1.Text = "tabControl1";
            this.tabControl1.SelectedTabChanged += new DevComponents.DotNetBar.TabStrip.SelectedTabChangedEventHandler(this.tabControl1_SelectedTabChanged);
            // 
            // BasePanel
            // 
            this.BasePanel.Controls.Add(this.contextMenuBar1);
            this.BasePanel.Controls.Add(this.BasePg);
            this.BasePanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BasePanel.Location = new System.Drawing.Point(0, 26);
            this.BasePanel.Name = "BasePanel";
            this.BasePanel.Padding = new System.Windows.Forms.Padding(1);
            this.BasePanel.Size = new System.Drawing.Size(568, 516);
            this.BasePanel.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.BasePanel.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.BasePanel.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.BasePanel.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.BasePanel.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.BasePanel.Style.GradientAngle = 90;
            this.BasePanel.TabIndex = 1;
            this.BasePanel.TabItem = this.BaseTabItem;
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bTabContext,
            this.cmNodeRightClick,
            this.cmScriptOp});
            this.contextMenuBar1.Location = new System.Drawing.Point(205, 459);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(263, 47);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.contextMenuBar1.TabIndex = 31;
            this.contextMenuBar1.TabStop = false;
            // 
            // bTabContext
            // 
            this.bTabContext.AutoExpandOnClick = true;
            this.bTabContext.GlobalName = "bTabContext";
            this.bTabContext.ImagePaddingHorizontal = 8;
            this.bTabContext.Name = "bTabContext";
            this.bTabContext.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmdContextSave,
            this.cmdContextClose,
            this.bTabColor});
            this.bTabContext.Text = "bTabContext";
            // 
            // cmdContextSave
            // 
            this.cmdContextSave.GlobalItem = false;
            this.cmdContextSave.ImageIndex = 17;
            this.cmdContextSave.ImagePaddingHorizontal = 8;
            this.cmdContextSave.Name = "cmdContextSave";
            this.cmdContextSave.Text = "保存";
            this.cmdContextSave.Tooltip = "保存活动文档";
            // 
            // cmdContextClose
            // 
            this.cmdContextClose.Enabled = false;
            this.cmdContextClose.GlobalItem = false;
            this.cmdContextClose.ImagePaddingHorizontal = 8;
            this.cmdContextClose.Name = "cmdContextClose";
            this.cmdContextClose.Text = "关闭";
            this.cmdContextClose.Tooltip = "关闭活动文档";
            // 
            // bTabColor
            // 
            this.bTabColor.BeginGroup = true;
            this.bTabColor.GlobalName = "bTabColor";
            this.bTabColor.ImagePaddingHorizontal = 8;
            this.bTabColor.Name = "bTabColor";
            this.bTabColor.PopupType = DevComponents.DotNetBar.ePopupType.Container;
            this.bTabColor.Text = "更改标签背景色";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.ImagePaddingHorizontal = 8;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonAddSameLevel,
            this.buttonAddSub,
            this.buttonRename,
            this.buttonCopy,
            this.buttonPaste,
            this.buttonDelete,
            this.buttonExpandAll,
            this.buttonReloadTree});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonAddSameLevel
            // 
            this.buttonAddSameLevel.ImagePaddingHorizontal = 8;
            this.buttonAddSameLevel.Name = "buttonAddSameLevel";
            this.buttonAddSameLevel.Text = "添加本层";
            this.buttonAddSameLevel.Click += new System.EventHandler(this.buttonAddSameLevel_Click);
            // 
            // buttonAddSub
            // 
            this.buttonAddSub.ImagePaddingHorizontal = 8;
            this.buttonAddSub.Name = "buttonAddSub";
            this.buttonAddSub.Text = "添加子层";
            this.buttonAddSub.Click += new System.EventHandler(this.buttonAddSubLevel_Click);
            // 
            // buttonRename
            // 
            this.buttonRename.BeginGroup = true;
            this.buttonRename.ImagePaddingHorizontal = 8;
            this.buttonRename.Name = "buttonRename";
            this.buttonRename.Text = "重命名";
            this.buttonRename.Click += new System.EventHandler(this.buttonRename_Click);
            // 
            // buttonCopy
            // 
            this.buttonCopy.ImagePaddingHorizontal = 8;
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Text = "复制";
            this.buttonCopy.Click += new System.EventHandler(this.buttonCopy_Click);
            // 
            // buttonPaste
            // 
            this.buttonPaste.ImagePaddingHorizontal = 8;
            this.buttonPaste.Name = "buttonPaste";
            this.buttonPaste.Text = "粘贴";
            this.buttonPaste.Click += new System.EventHandler(this.buttonPaste_Click);
            // 
            // buttonDelete
            // 
            this.buttonDelete.BeginGroup = true;
            this.buttonDelete.ImagePaddingHorizontal = 8;
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Text = "删除";
            this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
            // 
            // buttonExpandAll
            // 
            this.buttonExpandAll.BeginGroup = true;
            this.buttonExpandAll.ImagePaddingHorizontal = 8;
            this.buttonExpandAll.Name = "buttonExpandAll";
            this.buttonExpandAll.Text = "展开";
            this.buttonExpandAll.Click += new System.EventHandler(this.buttonExpandAll_Click);
            // 
            // buttonReloadTree
            // 
            this.buttonReloadTree.BeginGroup = true;
            this.buttonReloadTree.ImagePaddingHorizontal = 8;
            this.buttonReloadTree.Name = "buttonReloadTree";
            this.buttonReloadTree.Text = "刷新树";
            this.buttonReloadTree.Click += new System.EventHandler(this.buttonReloadTree_Click);
            // 
            // cmScriptOp
            // 
            this.cmScriptOp.AutoExpandOnClick = true;
            this.cmScriptOp.ImagePaddingHorizontal = 8;
            this.cmScriptOp.Name = "cmScriptOp";
            this.cmScriptOp.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bShowLuaCode,
            this.bShowMetainfo,
            this.bEditFile,
            this.bDeleteRecord,
            this.bAddRecord,
            this.bClear,
            this.bClearContent,
            this.bViewLog,
            this.buttonItem1});
            this.cmScriptOp.Text = "cmScriptOp";
            this.cmScriptOp.PopupOpen += new DevComponents.DotNetBar.DotNetBarManager.PopupOpenEventHandler(this.cmScriptOp_PopupOpen);
            // 
            // bShowLuaCode
            // 
            this.bShowLuaCode.ImagePaddingHorizontal = 8;
            this.bShowLuaCode.Name = "bShowLuaCode";
            this.bShowLuaCode.Text = "显示代码(&S)";
            this.bShowLuaCode.Click += new System.EventHandler(this.buttonShowLuaCode_Click);
            // 
            // bShowMetainfo
            // 
            this.bShowMetainfo.BeginGroup = true;
            this.bShowMetainfo.ImagePaddingHorizontal = 8;
            this.bShowMetainfo.Name = "bShowMetainfo";
            this.bShowMetainfo.Text = "编辑元信息(&X)";
            this.bShowMetainfo.Click += new System.EventHandler(this.bShowMetainfo_Click);
            // 
            // bEditFile
            // 
            this.bEditFile.ImagePaddingHorizontal = 8;
            this.bEditFile.Name = "bEditFile";
            this.bEditFile.Text = "编辑文件(&E)";
            this.bEditFile.Click += new System.EventHandler(this.bEditFile_Click);
            // 
            // bDeleteRecord
            // 
            this.bDeleteRecord.BeginGroup = true;
            this.bDeleteRecord.ImagePaddingHorizontal = 8;
            this.bDeleteRecord.Name = "bDeleteRecord";
            this.bDeleteRecord.Text = "删除从表记录(&D)";
            this.bDeleteRecord.Click += new System.EventHandler(this.bDeleteRecord_Click);
            // 
            // bAddRecord
            // 
            this.bAddRecord.ImagePaddingHorizontal = 8;
            this.bAddRecord.Name = "bAddRecord";
            this.bAddRecord.Text = "新建记录";
            this.bAddRecord.Click += new System.EventHandler(this.bAddRecord_Click);
            // 
            // bClear
            // 
            this.bClear.ImagePaddingHorizontal = 8;
            this.bClear.Name = "bClear";
            this.bClear.Text = "置空";
            this.bClear.Click += new System.EventHandler(this.bClear_Click);
            // 
            // bClearContent
            // 
            this.bClearContent.BeginGroup = true;
            this.bClearContent.ImagePaddingHorizontal = 8;
            this.bClearContent.Name = "bClearContent";
            this.bClearContent.Text = "测试用";
            this.bClearContent.Click += new System.EventHandler(this.bClearContent_Click);
            // 
            // bViewLog
            // 
            this.bViewLog.ImagePaddingHorizontal = 8;
            this.bViewLog.Name = "bViewLog";
            this.bViewLog.Text = "查看修改日志";
            this.bViewLog.Click += new System.EventHandler(this.bViewLog_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "修改";
            this.buttonItem1.Visible = false;
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // BasePg
            // 
            this.BasePg.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(225)))), ((int)(((byte)(242)))), ((int)(((byte)(255)))));
            this.BasePg.CommandsDisabledLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.BasePg.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BasePg.LineColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.BasePg.Location = new System.Drawing.Point(1, 1);
            this.BasePg.Name = "BasePg";
            this.BasePg.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.BasePg.Size = new System.Drawing.Size(566, 514);
            this.BasePg.TabIndex = 29;
            this.BasePg.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.BasePg_PropertyValueChanged);
            // 
            // BaseTabItem
            // 
            this.BaseTabItem.AttachedControl = this.BasePanel;
            this.BaseTabItem.Name = "BaseTabItem";
            this.BaseTabItem.Text = "BaseItem(not visible)";
            this.BaseTabItem.Visible = false;
            // 
            // tmrRender
            // 
            this.tmrRender.Interval = 50;
            this.tmrRender.Tick += new System.EventHandler(this.tmrRender_Tick);
            // 
            // BaseForm
            // 
            this.ClientSize = new System.Drawing.Size(753, 542);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.KeyPreview = true;
            this.Name = "BaseForm";
            this.Load += new System.EventHandler(this.Form_Load);
            this.Activated += new System.EventHandler(this.BaseForm_Activated);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.BaseForm_FormClosed);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.BaseForm_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.BaseForm_KeyDown);
            this.dockSite1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.bar1.ResumeLayout(false);
            this.panelDockContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PreviewBar)).EndInit();
            this.PreviewBar.ResumeLayout(false);
            this.panelDockContainer2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PreviewPluginBar)).EndInit();
            this.PreviewPluginBar.ResumeLayout(false);
            this.panelDockContainer3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.BasePanel.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        public DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
        private DevComponents.DotNetBar.DockSite dockSite4;
        private DevComponents.DotNetBar.DockSite dockSite1;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer1;
        private System.Windows.Forms.TreeView baseTree;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem1;
        private DevComponents.DotNetBar.DockSite dockSite2;
        private DevComponents.DotNetBar.DockSite dockSite3;
        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.DockSite dockSite5;
        private DevComponents.DotNetBar.DockSite dockSite6;
        private DevComponents.DotNetBar.DockSite dockSite7;
        private DevComponents.DotNetBar.TabControlPanel BasePanel;
        private DevComponents.DotNetBar.TabItem BaseTabItem;
        private System.Windows.Forms.PropertyGrid BasePg;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem bTabContext;
        private DevComponents.DotNetBar.ButtonItem cmdContextSave;
        private DevComponents.DotNetBar.ButtonItem cmdContextClose;
        private DevComponents.DotNetBar.ButtonItem bTabColor;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonRename;
        private DevComponents.DotNetBar.ButtonItem buttonDelete;
        private DevComponents.DotNetBar.ButtonItem cmScriptOp;
        private DevComponents.DotNetBar.ButtonItem bShowLuaCode;
        private DevComponents.DotNetBar.ButtonItem bEditFile;
        private DevComponents.DotNetBar.ButtonItem bShowMetainfo;
        private DevComponents.DotNetBar.ButtonItem buttonAddSub;
        private DevComponents.DotNetBar.ButtonItem buttonAddSameLevel;
        private DevComponents.DotNetBar.Bar PreviewBar;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer2;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem2;
        private DevComponents.DotNetBar.PanelEx PreviewPanel;
        private System.Windows.Forms.Timer tmrRender;
        private DevComponents.DotNetBar.ButtonItem bDeleteRecord;
        private DevComponents.DotNetBar.ButtonItem buttonCopy;
        private DevComponents.DotNetBar.ButtonItem buttonPaste;
        private DevComponents.DotNetBar.Bar PreviewPluginBar;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer3;
        private DevComponents.DotNetBar.PanelEx ContainerPanel;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem3;
        private DevComponents.DotNetBar.ButtonItem bClearContent;
        private DevComponents.DotNetBar.ButtonItem bAddRecord;
        private DevComponents.DotNetBar.ButtonItem bViewLog;
        private DevComponents.DotNetBar.ButtonItem buttonExpandAll;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem bClear;
        private DevComponents.DotNetBar.ButtonItem buttonReloadTree;
    }
}
