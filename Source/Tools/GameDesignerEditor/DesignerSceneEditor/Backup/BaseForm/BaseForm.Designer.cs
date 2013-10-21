namespace DesignerSceneEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BaseForm));
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.SideCtrlBar = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer2 = new DevComponents.DotNetBar.PanelDockContainer();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treeView1 = new MWControlSuite.MWTreeView();
            this.checkShowAll = new System.Windows.Forms.CheckBox();
            this.panelDockContainer1 = new DevComponents.DotNetBar.PanelDockContainer();
            this.panelEx2 = new DevComponents.DotNetBar.PanelEx();
            this.wayPointSetParameterPanel = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.bAutoAttackPathNpc = new DevComponents.DotNetBar.ButtonX();
            this.patrolPathOrderBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.patrolPathWalkSpeedBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.patrolPathNameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX7 = new DevComponents.DotNetBar.LabelX();
            this.labelX9 = new DevComponents.DotNetBar.LabelX();
            this.labelX8 = new DevComponents.DotNetBar.LabelX();
            this.wayPointParameterPanel = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.wayPointScriptBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonX18 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX17 = new DevComponents.DotNetBar.ButtonX();
            this.labelX13 = new DevComponents.DotNetBar.LabelX();
            this.cWayPointRun = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cWayPointAnimationLoop = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.wayPointStayAnimationBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.wayPointStayDirectionBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.wayPointStayFrameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX12 = new DevComponents.DotNetBar.LabelX();
            this.labelX10 = new DevComponents.DotNetBar.LabelX();
            this.labelX11 = new DevComponents.DotNetBar.LabelX();
            this.expandableSplitter1 = new DevComponents.DotNetBar.ExpandableSplitter();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.buttonX4 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.patrolTree = new DevComponents.AdvTree.AdvTree();
            this.node1 = new DevComponents.AdvTree.Node();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.panelDockContainer4 = new DevComponents.DotNetBar.PanelDockContainer();
            this.panelEx4 = new DevComponents.DotNetBar.PanelEx();
            this.dataGridViewX2 = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.expandableSplitter2 = new DevComponents.DotNetBar.ExpandableSplitter();
            this.panelEx3 = new DevComponents.DotNetBar.PanelEx();
            this.buttonX9 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX8 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX7 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX6 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX5 = new DevComponents.DotNetBar.ButtonX();
            this.trafficTree = new DevComponents.AdvTree.AdvTree();
            this.node2 = new DevComponents.AdvTree.Node();
            this.nodeConnector2 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.panelDockContainer6 = new DevComponents.DotNetBar.PanelDockContainer();
            this.panelEx5 = new DevComponents.DotNetBar.PanelEx();
            this.groupPanel3 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.buttonX21 = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel2 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.bReplaceAllNpcTemplate = new DevComponents.DotNetBar.ButtonX();
            this.bReplaceNpcTemplate = new DevComponents.DotNetBar.ButtonX();
            this.buttonX11 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX10 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.npcTree = new DevComponents.AdvTree.AdvTree();
            this.node3 = new DevComponents.AdvTree.Node();
            this.nodeConnector3 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle3 = new DevComponents.DotNetBar.ElementStyle();
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.buttonX20 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX19 = new DevComponents.DotNetBar.ButtonX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.previewCanvas1 = new DesignerSceneEditor.PreviewCanvas();
            this.panelDockContainer3 = new DevComponents.DotNetBar.PanelDockContainer();
            this.groupBoxBrushParams = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.sliderBrushHeight = new DevComponents.DotNetBar.Controls.Slider();
            this.sliderDisplayWidth = new DevComponents.DotNetBar.Controls.Slider();
            this.sliderBrushRadius = new DevComponents.DotNetBar.Controls.Slider();
            this.radioEcllipse = new System.Windows.Forms.RadioButton();
            this.radioRect = new System.Windows.Forms.RadioButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.checkBoxShowPoly = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkShowLogicCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkShowTerrainCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkShowItemCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkShowSelectedCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonX13 = new DevComponents.DotNetBar.ButtonX();
            this.buttonBuildPoly = new DevComponents.DotNetBar.ButtonX();
            this.buttonX15 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX14 = new DevComponents.DotNetBar.ButtonX();
            this.groupboxPoly = new System.Windows.Forms.GroupBox();
            this.colorPicker = new DevComponents.DotNetBar.PanelEx();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.checkBoxBlock = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.buttonX16 = new DevComponents.DotNetBar.ButtonX();
            this.buttonOK = new DevComponents.DotNetBar.ButtonX();
            this.buttonClearScript = new DevComponents.DotNetBar.ButtonX();
            this.checkBoxRideHorse = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.buttonBrowseScript = new DevComponents.DotNetBar.ButtonX();
            this.checkBoxRest = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.textLogicName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.checkBoxPut = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.textBoxHeight = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.checkBoxStall = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBoxIndoor = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.textBoxScript = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.advTreePoly = new DevComponents.AdvTree.AdvTree();
            this.node5 = new DevComponents.AdvTree.Node();
            this.nodeConnector5 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle5 = new DevComponents.DotNetBar.ElementStyle();
            this.panelDockContainer5 = new DevComponents.DotNetBar.PanelDockContainer();
            this.panelEx6 = new DevComponents.DotNetBar.PanelEx();
            this.groupPanel7 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.buttonX22 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX27 = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel6 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.buttonX12 = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel5 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.bReplaceAllDoodadTemplate = new DevComponents.DotNetBar.ButtonX();
            this.bReplaceDoodadTemplate = new DevComponents.DotNetBar.ButtonX();
            this.buttonX24 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX23 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX2 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.doodadTree = new DevComponents.AdvTree.AdvTree();
            this.node4 = new DevComponents.AdvTree.Node();
            this.nodeConnector4 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle4 = new DevComponents.DotNetBar.ElementStyle();
            this.groupPanel4 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.buttonX26 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX25 = new DevComponents.DotNetBar.ButtonX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.previewCanvas2 = new DesignerSceneEditor.PreviewCanvas();
            this.dockContainerItem2 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem1 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem4 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem6 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem7 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem3 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite8 = new DevComponents.DotNetBar.DockSite();
            this.dockSite5 = new DevComponents.DotNetBar.DockSite();
            this.dockSite6 = new DevComponents.DotNetBar.DockSite();
            this.dockSite7 = new DevComponents.DotNetBar.DockSite();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.dockContainerItem9 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockContainerItem5 = new DevComponents.DotNetBar.DockContainerItem();
            this.tmrRender = new System.Windows.Forms.Timer(this.components);
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
            this.cmScriptOp = new DevComponents.DotNetBar.ButtonItem();
            this.bShowLuaCode = new DevComponents.DotNetBar.ButtonItem();
            this.bShowMetainfo = new DevComponents.DotNetBar.ButtonItem();
            this.bEditFile = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteRecord = new DevComponents.DotNetBar.ButtonItem();
            this.bClearContent = new DevComponents.DotNetBar.ButtonItem();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.cmRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonJmpToTree = new DevComponents.DotNetBar.ButtonItem();
            this.buttonDelSelObj = new DevComponents.DotNetBar.ButtonItem();
            this.buttonProp = new DevComponents.DotNetBar.ButtonItem();
            this.dockContainerItem8 = new DevComponents.DotNetBar.DockContainerItem();
            this.PanelView = new DesignerSceneEditor.MyPanel();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.dockSite1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.SideCtrlBar)).BeginInit();
            this.SideCtrlBar.SuspendLayout();
            this.panelDockContainer2.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panelDockContainer1.SuspendLayout();
            this.panelEx2.SuspendLayout();
            this.wayPointSetParameterPanel.SuspendLayout();
            this.wayPointParameterPanel.SuspendLayout();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.patrolTree)).BeginInit();
            this.panelDockContainer4.SuspendLayout();
            this.panelEx4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewX2)).BeginInit();
            this.panelEx3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trafficTree)).BeginInit();
            this.panelDockContainer6.SuspendLayout();
            this.panelEx5.SuspendLayout();
            this.groupPanel3.SuspendLayout();
            this.groupPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.npcTree)).BeginInit();
            this.groupPanel1.SuspendLayout();
            this.panelDockContainer3.SuspendLayout();
            this.groupBoxBrushParams.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupboxPoly.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.advTreePoly)).BeginInit();
            this.panelDockContainer5.SuspendLayout();
            this.panelEx6.SuspendLayout();
            this.groupPanel7.SuspendLayout();
            this.groupPanel6.SuspendLayout();
            this.groupPanel5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.doodadTree)).BeginInit();
            this.groupPanel4.SuspendLayout();
            this.PanelView.SuspendLayout();
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
            this.dotNetBarManager1.ParentForm = null;
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
            this.dockSite4.Location = new System.Drawing.Point(245, 846);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(547, 0);
            this.dockSite4.TabIndex = 7;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Controls.Add(this.SideCtrlBar);
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.SideCtrlBar, 242, 846)))}, DevComponents.DotNetBar.eOrientation.Vertical);
            this.dockSite1.Location = new System.Drawing.Point(0, 0);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(245, 846);
            this.dockSite1.TabIndex = 4;
            this.dockSite1.TabStop = false;
            // 
            // SideCtrlBar
            // 
            this.SideCtrlBar.AccessibleDescription = "DotNetBar Bar (SideCtrlBar)";
            this.SideCtrlBar.AccessibleName = "DotNetBar Bar";
            this.SideCtrlBar.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.SideCtrlBar.AutoSyncBarCaption = true;
            this.SideCtrlBar.Controls.Add(this.panelDockContainer2);
            this.SideCtrlBar.Controls.Add(this.panelDockContainer1);
            this.SideCtrlBar.Controls.Add(this.panelDockContainer4);
            this.SideCtrlBar.Controls.Add(this.panelDockContainer6);
            this.SideCtrlBar.Controls.Add(this.panelDockContainer3);
            this.SideCtrlBar.Controls.Add(this.panelDockContainer5);
            this.SideCtrlBar.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.SideCtrlBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem2,
            this.dockContainerItem1,
            this.dockContainerItem4,
            this.dockContainerItem6,
            this.dockContainerItem7,
            this.dockContainerItem3});
            this.SideCtrlBar.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.SideCtrlBar.Location = new System.Drawing.Point(0, 0);
            this.SideCtrlBar.Name = "SideCtrlBar";
            this.SideCtrlBar.SelectedDockTab = 0;
            this.SideCtrlBar.Size = new System.Drawing.Size(242, 846);
            this.SideCtrlBar.Stretch = true;
            this.SideCtrlBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.SideCtrlBar.TabIndex = 1;
            this.SideCtrlBar.TabStop = false;
            this.SideCtrlBar.Text = "别名面板";
            // 
            // panelDockContainer2
            // 
            this.panelDockContainer2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer2.Controls.Add(this.splitContainer1);
            this.panelDockContainer2.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer2.Name = "panelDockContainer2";
            this.panelDockContainer2.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer2.Style.GradientAngle = 90;
            this.panelDockContainer2.TabIndex = 0;
            // 
            // splitContainer1
            // 
            this.splitContainer1.BackColor = System.Drawing.Color.Transparent;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeView1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.checkShowAll);
            this.splitContainer1.Size = new System.Drawing.Size(236, 795);
            this.splitContainer1.SplitterDistance = 668;
            this.splitContainer1.TabIndex = 1;
            // 
            // treeView1
            // 
            this.treeView1.CheckedNodes = ((System.Collections.Hashtable)(resources.GetObject("treeView1.CheckedNodes")));
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.RubberbandGradientBlend = new MWControlSuite.MWRubberbandGradientBlend[0];
            this.treeView1.RubberbandGradientColorBlend = new MWControlSuite.MWRubberbandGradientColorBlend[0];
            this.treeView1.SelNodes = ((System.Collections.Hashtable)(resources.GetObject("treeView1.SelNodes")));
            this.treeView1.Size = new System.Drawing.Size(236, 668);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            this.treeView1.AfterSelNodesRemove += new MWControlSuite.MWPropertyEventHandler(this.treeView1_AfterSelNodesRemove);
            this.treeView1.AfterSelNodesAdd += new MWControlSuite.MWPropertyEventHandler(this.treeView1_AfterSelNodesAdd);
            // 
            // checkShowAll
            // 
            this.checkShowAll.AutoSize = true;
            this.checkShowAll.Checked = true;
            this.checkShowAll.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkShowAll.Location = new System.Drawing.Point(24, 21);
            this.checkShowAll.Name = "checkShowAll";
            this.checkShowAll.Size = new System.Drawing.Size(138, 16);
            this.checkShowAll.TabIndex = 0;
            this.checkShowAll.Text = "显示所有npc和doodad";
            this.checkShowAll.UseVisualStyleBackColor = true;
            this.checkShowAll.CheckedChanged += new System.EventHandler(this.checkShowAll_CheckedChanged);
            // 
            // panelDockContainer1
            // 
            this.panelDockContainer1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer1.Controls.Add(this.panelEx2);
            this.panelDockContainer1.Controls.Add(this.expandableSplitter1);
            this.panelDockContainer1.Controls.Add(this.panelEx1);
            this.panelDockContainer1.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer1.Name = "panelDockContainer1";
            this.panelDockContainer1.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer1.Style.GradientAngle = 90;
            this.panelDockContainer1.TabIndex = 6;
            // 
            // panelEx2
            // 
            this.panelEx2.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx2.Controls.Add(this.wayPointSetParameterPanel);
            this.panelEx2.Controls.Add(this.wayPointParameterPanel);
            this.panelEx2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx2.Location = new System.Drawing.Point(0, 354);
            this.panelEx2.Name = "panelEx2";
            this.panelEx2.Size = new System.Drawing.Size(236, 441);
            this.panelEx2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx2.Style.GradientAngle = 90;
            this.panelEx2.TabIndex = 2;
            // 
            // wayPointSetParameterPanel
            // 
            this.wayPointSetParameterPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.wayPointSetParameterPanel.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.wayPointSetParameterPanel.Controls.Add(this.bAutoAttackPathNpc);
            this.wayPointSetParameterPanel.Controls.Add(this.patrolPathOrderBox);
            this.wayPointSetParameterPanel.Controls.Add(this.patrolPathWalkSpeedBox);
            this.wayPointSetParameterPanel.Controls.Add(this.patrolPathNameBox);
            this.wayPointSetParameterPanel.Controls.Add(this.labelX7);
            this.wayPointSetParameterPanel.Controls.Add(this.labelX9);
            this.wayPointSetParameterPanel.Controls.Add(this.labelX8);
            this.wayPointSetParameterPanel.Location = new System.Drawing.Point(11, 16);
            this.wayPointSetParameterPanel.Name = "wayPointSetParameterPanel";
            this.wayPointSetParameterPanel.Size = new System.Drawing.Size(216, 167);
            // 
            // 
            // 
            this.wayPointSetParameterPanel.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.wayPointSetParameterPanel.Style.BackColorGradientAngle = 90;
            this.wayPointSetParameterPanel.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.wayPointSetParameterPanel.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointSetParameterPanel.Style.BorderBottomWidth = 1;
            this.wayPointSetParameterPanel.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.wayPointSetParameterPanel.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointSetParameterPanel.Style.BorderLeftWidth = 1;
            this.wayPointSetParameterPanel.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointSetParameterPanel.Style.BorderRightWidth = 1;
            this.wayPointSetParameterPanel.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointSetParameterPanel.Style.BorderTopWidth = 1;
            this.wayPointSetParameterPanel.Style.CornerDiameter = 4;
            this.wayPointSetParameterPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.wayPointSetParameterPanel.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.wayPointSetParameterPanel.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.wayPointSetParameterPanel.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.wayPointSetParameterPanel.TabIndex = 6;
            this.wayPointSetParameterPanel.Text = "路径参数";
            // 
            // bAutoAttackPathNpc
            // 
            this.bAutoAttackPathNpc.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAutoAttackPathNpc.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAutoAttackPathNpc.Location = new System.Drawing.Point(4, 108);
            this.bAutoAttackPathNpc.Name = "bAutoAttackPathNpc";
            this.bAutoAttackPathNpc.Size = new System.Drawing.Size(130, 20);
            this.bAutoAttackPathNpc.TabIndex = 6;
            this.bAutoAttackPathNpc.Text = "自动为关联的NPC列队";
            this.bAutoAttackPathNpc.Click += new System.EventHandler(this.bAutoAttackPathNpc_Click);
            // 
            // patrolPathOrderBox
            // 
            this.patrolPathOrderBox.DisplayMember = "Text";
            this.patrolPathOrderBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.patrolPathOrderBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.patrolPathOrderBox.FormattingEnabled = true;
            this.patrolPathOrderBox.ItemHeight = 15;
            this.patrolPathOrderBox.Location = new System.Drawing.Point(101, 68);
            this.patrolPathOrderBox.Name = "patrolPathOrderBox";
            this.patrolPathOrderBox.Size = new System.Drawing.Size(100, 21);
            this.patrolPathOrderBox.TabIndex = 5;
            this.patrolPathOrderBox.SelectedIndexChanged += new System.EventHandler(this.patrolPathOrderBox_SelectedIndexChanged);
            // 
            // patrolPathWalkSpeedBox
            // 
            // 
            // 
            // 
            this.patrolPathWalkSpeedBox.Border.Class = "TextBoxBorder";
            this.patrolPathWalkSpeedBox.Location = new System.Drawing.Point(101, 41);
            this.patrolPathWalkSpeedBox.Name = "patrolPathWalkSpeedBox";
            this.patrolPathWalkSpeedBox.Size = new System.Drawing.Size(100, 21);
            this.patrolPathWalkSpeedBox.TabIndex = 4;
            this.patrolPathWalkSpeedBox.TextChanged += new System.EventHandler(this.patrolPathWalkSpeedBox_TextChanged);
            // 
            // patrolPathNameBox
            // 
            // 
            // 
            // 
            this.patrolPathNameBox.Border.Class = "TextBoxBorder";
            this.patrolPathNameBox.Location = new System.Drawing.Point(101, 14);
            this.patrolPathNameBox.Name = "patrolPathNameBox";
            this.patrolPathNameBox.Size = new System.Drawing.Size(100, 21);
            this.patrolPathNameBox.TabIndex = 3;
            this.patrolPathNameBox.TextChanged += new System.EventHandler(this.patrolPathNameBox_TextChanged);
            // 
            // labelX7
            // 
            this.labelX7.Location = new System.Drawing.Point(8, 14);
            this.labelX7.Name = "labelX7";
            this.labelX7.Size = new System.Drawing.Size(80, 20);
            this.labelX7.TabIndex = 0;
            this.labelX7.Text = "路径名称";
            // 
            // labelX9
            // 
            this.labelX9.Location = new System.Drawing.Point(8, 68);
            this.labelX9.Name = "labelX9";
            this.labelX9.Size = new System.Drawing.Size(80, 20);
            this.labelX9.TabIndex = 2;
            this.labelX9.Text = "巡逻阵型";
            // 
            // labelX8
            // 
            this.labelX8.Location = new System.Drawing.Point(8, 41);
            this.labelX8.Name = "labelX8";
            this.labelX8.Size = new System.Drawing.Size(80, 20);
            this.labelX8.TabIndex = 1;
            this.labelX8.Text = "行走速度";
            // 
            // wayPointParameterPanel
            // 
            this.wayPointParameterPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.wayPointParameterPanel.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.wayPointParameterPanel.Controls.Add(this.wayPointScriptBox);
            this.wayPointParameterPanel.Controls.Add(this.buttonX18);
            this.wayPointParameterPanel.Controls.Add(this.buttonX17);
            this.wayPointParameterPanel.Controls.Add(this.labelX13);
            this.wayPointParameterPanel.Controls.Add(this.cWayPointRun);
            this.wayPointParameterPanel.Controls.Add(this.cWayPointAnimationLoop);
            this.wayPointParameterPanel.Controls.Add(this.wayPointStayAnimationBox);
            this.wayPointParameterPanel.Controls.Add(this.wayPointStayDirectionBox);
            this.wayPointParameterPanel.Controls.Add(this.wayPointStayFrameBox);
            this.wayPointParameterPanel.Controls.Add(this.labelX12);
            this.wayPointParameterPanel.Controls.Add(this.labelX10);
            this.wayPointParameterPanel.Controls.Add(this.labelX11);
            this.wayPointParameterPanel.Location = new System.Drawing.Point(11, 203);
            this.wayPointParameterPanel.Name = "wayPointParameterPanel";
            this.wayPointParameterPanel.Size = new System.Drawing.Size(216, 225);
            // 
            // 
            // 
            this.wayPointParameterPanel.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.wayPointParameterPanel.Style.BackColorGradientAngle = 90;
            this.wayPointParameterPanel.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.wayPointParameterPanel.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointParameterPanel.Style.BorderBottomWidth = 1;
            this.wayPointParameterPanel.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.wayPointParameterPanel.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointParameterPanel.Style.BorderLeftWidth = 1;
            this.wayPointParameterPanel.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointParameterPanel.Style.BorderRightWidth = 1;
            this.wayPointParameterPanel.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.wayPointParameterPanel.Style.BorderTopWidth = 1;
            this.wayPointParameterPanel.Style.CornerDiameter = 4;
            this.wayPointParameterPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.wayPointParameterPanel.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.wayPointParameterPanel.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.wayPointParameterPanel.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.wayPointParameterPanel.TabIndex = 5;
            this.wayPointParameterPanel.Text = "路径点参数";
            // 
            // wayPointScriptBox
            // 
            // 
            // 
            // 
            this.wayPointScriptBox.Border.Class = "TextBoxBorder";
            this.wayPointScriptBox.Location = new System.Drawing.Point(7, 173);
            this.wayPointScriptBox.Name = "wayPointScriptBox";
            this.wayPointScriptBox.Size = new System.Drawing.Size(194, 21);
            this.wayPointScriptBox.TabIndex = 14;
            this.wayPointScriptBox.TextChanged += new System.EventHandler(this.wayPointScriptBox_TextChanged);
            // 
            // buttonX18
            // 
            this.buttonX18.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX18.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX18.Location = new System.Drawing.Point(141, 142);
            this.buttonX18.Name = "buttonX18";
            this.buttonX18.Size = new System.Drawing.Size(60, 20);
            this.buttonX18.TabIndex = 13;
            this.buttonX18.Text = "清除脚本";
            this.buttonX18.Click += new System.EventHandler(this.buttonX18_Click);
            // 
            // buttonX17
            // 
            this.buttonX17.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX17.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX17.Location = new System.Drawing.Point(75, 142);
            this.buttonX17.Name = "buttonX17";
            this.buttonX17.Size = new System.Drawing.Size(60, 20);
            this.buttonX17.TabIndex = 12;
            this.buttonX17.Text = "浏览脚本";
            this.buttonX17.Click += new System.EventHandler(this.buttonX17_Click);
            // 
            // labelX13
            // 
            this.labelX13.Location = new System.Drawing.Point(7, 142);
            this.labelX13.Name = "labelX13";
            this.labelX13.Size = new System.Drawing.Size(30, 20);
            this.labelX13.TabIndex = 11;
            this.labelX13.Text = "脚本";
            // 
            // cWayPointRun
            // 
            this.cWayPointRun.Location = new System.Drawing.Point(101, 110);
            this.cWayPointRun.Name = "cWayPointRun";
            this.cWayPointRun.Size = new System.Drawing.Size(70, 20);
            this.cWayPointRun.TabIndex = 10;
            this.cWayPointRun.Text = "跑步";
            this.cWayPointRun.CheckedChanged += new System.EventHandler(this.cWayPointRun_CheckedChanged);
            // 
            // cWayPointAnimationLoop
            // 
            this.cWayPointAnimationLoop.Checked = true;
            this.cWayPointAnimationLoop.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cWayPointAnimationLoop.CheckValue = "Y";
            this.cWayPointAnimationLoop.Location = new System.Drawing.Point(9, 110);
            this.cWayPointAnimationLoop.Name = "cWayPointAnimationLoop";
            this.cWayPointAnimationLoop.Size = new System.Drawing.Size(80, 20);
            this.cWayPointAnimationLoop.TabIndex = 9;
            this.cWayPointAnimationLoop.Text = "动作循环";
            this.cWayPointAnimationLoop.CheckedChanged += new System.EventHandler(this.cWayPointAnimationLoop_CheckedChanged);
            // 
            // wayPointStayAnimationBox
            // 
            // 
            // 
            // 
            this.wayPointStayAnimationBox.Border.Class = "TextBoxBorder";
            this.wayPointStayAnimationBox.Location = new System.Drawing.Point(101, 67);
            this.wayPointStayAnimationBox.Name = "wayPointStayAnimationBox";
            this.wayPointStayAnimationBox.Size = new System.Drawing.Size(100, 21);
            this.wayPointStayAnimationBox.TabIndex = 8;
            this.wayPointStayAnimationBox.TextChanged += new System.EventHandler(this.wayPointStayAnimationBox_TextChanged);
            // 
            // wayPointStayDirectionBox
            // 
            // 
            // 
            // 
            this.wayPointStayDirectionBox.Border.Class = "TextBoxBorder";
            this.wayPointStayDirectionBox.Location = new System.Drawing.Point(101, 40);
            this.wayPointStayDirectionBox.Name = "wayPointStayDirectionBox";
            this.wayPointStayDirectionBox.Size = new System.Drawing.Size(100, 21);
            this.wayPointStayDirectionBox.TabIndex = 7;
            this.wayPointStayDirectionBox.TextChanged += new System.EventHandler(this.wayPointStayDirectionBox_TextChanged);
            // 
            // wayPointStayFrameBox
            // 
            // 
            // 
            // 
            this.wayPointStayFrameBox.Border.Class = "TextBoxBorder";
            this.wayPointStayFrameBox.Location = new System.Drawing.Point(101, 13);
            this.wayPointStayFrameBox.Name = "wayPointStayFrameBox";
            this.wayPointStayFrameBox.Size = new System.Drawing.Size(100, 21);
            this.wayPointStayFrameBox.TabIndex = 6;
            this.wayPointStayFrameBox.TextChanged += new System.EventHandler(this.wayPointStayFrameBox_TextChanged);
            // 
            // labelX12
            // 
            this.labelX12.Location = new System.Drawing.Point(7, 67);
            this.labelX12.Name = "labelX12";
            this.labelX12.Size = new System.Drawing.Size(80, 20);
            this.labelX12.TabIndex = 5;
            this.labelX12.Text = "停留动作";
            // 
            // labelX10
            // 
            this.labelX10.Location = new System.Drawing.Point(7, 13);
            this.labelX10.Name = "labelX10";
            this.labelX10.Size = new System.Drawing.Size(80, 20);
            this.labelX10.TabIndex = 3;
            this.labelX10.Text = "停留帧数";
            // 
            // labelX11
            // 
            this.labelX11.Location = new System.Drawing.Point(7, 39);
            this.labelX11.Name = "labelX11";
            this.labelX11.Size = new System.Drawing.Size(80, 20);
            this.labelX11.TabIndex = 4;
            this.labelX11.Text = "停留方向";
            // 
            // expandableSplitter1
            // 
            this.expandableSplitter1.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter1.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter1.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.expandableSplitter1.Dock = System.Windows.Forms.DockStyle.Top;
            this.expandableSplitter1.ExpandFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter1.ExpandFillColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter1.ExpandLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter1.ExpandLineColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter1.GripDarkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter1.GripDarkColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter1.GripLightColor = System.Drawing.Color.FromArgb(((int)(((byte)(227)))), ((int)(((byte)(239)))), ((int)(((byte)(255)))));
            this.expandableSplitter1.GripLightColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.expandableSplitter1.HotBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(151)))), ((int)(((byte)(61)))));
            this.expandableSplitter1.HotBackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(184)))), ((int)(((byte)(94)))));
            this.expandableSplitter1.HotBackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemPressedBackground2;
            this.expandableSplitter1.HotBackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemPressedBackground;
            this.expandableSplitter1.HotExpandFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter1.HotExpandFillColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter1.HotExpandLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter1.HotExpandLineColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter1.HotGripDarkColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter1.HotGripDarkColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter1.HotGripLightColor = System.Drawing.Color.FromArgb(((int)(((byte)(227)))), ((int)(((byte)(239)))), ((int)(((byte)(255)))));
            this.expandableSplitter1.HotGripLightColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.expandableSplitter1.Location = new System.Drawing.Point(0, 344);
            this.expandableSplitter1.Name = "expandableSplitter1";
            this.expandableSplitter1.Size = new System.Drawing.Size(236, 10);
            this.expandableSplitter1.Style = DevComponents.DotNetBar.eSplitterStyle.Office2007;
            this.expandableSplitter1.TabIndex = 1;
            this.expandableSplitter1.TabStop = false;
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.buttonX4);
            this.panelEx1.Controls.Add(this.buttonX3);
            this.panelEx1.Controls.Add(this.buttonX2);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Controls.Add(this.patrolTree);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(236, 344);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // buttonX4
            // 
            this.buttonX4.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX4.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX4.Location = new System.Drawing.Point(11, 304);
            this.buttonX4.Name = "buttonX4";
            this.buttonX4.Size = new System.Drawing.Size(70, 20);
            this.buttonX4.TabIndex = 4;
            this.buttonX4.Text = "添加路径点";
            this.buttonX4.Click += new System.EventHandler(this.buttonX4_Click);
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX3.Location = new System.Drawing.Point(157, 275);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(70, 20);
            this.buttonX3.TabIndex = 3;
            this.buttonX3.Text = "删除路径";
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.Location = new System.Drawing.Point(11, 275);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 2;
            this.buttonX2.Text = "添加路径";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(157, 304);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 1;
            this.buttonX1.Text = "刷新";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // patrolTree
            // 
            this.patrolTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.patrolTree.AllowDrop = true;
            this.patrolTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.patrolTree.BackgroundStyle.Class = "TreeBorderKey";
            this.patrolTree.DragDropEnabled = false;
            this.patrolTree.HotTracking = true;
            this.patrolTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.patrolTree.Location = new System.Drawing.Point(10, 11);
            this.patrolTree.Name = "patrolTree";
            this.patrolTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node1,
            this.node1});
            this.patrolTree.NodesConnector = this.nodeConnector1;
            this.patrolTree.NodeStyle = this.elementStyle1;
            this.patrolTree.PathSeparator = ";";
            this.patrolTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.patrolTree.Size = new System.Drawing.Size(217, 253);
            this.patrolTree.Styles.Add(this.elementStyle1);
            this.patrolTree.Styles.Add(this.elementStyle1);
            this.patrolTree.SuspendPaint = false;
            this.patrolTree.TabIndex = 0;
            this.patrolTree.Text = "patrolTree";
            this.patrolTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTree1_AfterNodeSelect);
            // 
            // node1
            // 
            this.node1.Expanded = true;
            this.node1.Name = "node1";
            this.node1.Text = "node1";
            // 
            // nodeConnector1
            // 
            this.nodeConnector1.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle1
            // 
            this.elementStyle1.Name = "elementStyle1";
            this.elementStyle1.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // panelDockContainer4
            // 
            this.panelDockContainer4.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer4.Controls.Add(this.panelEx4);
            this.panelDockContainer4.Controls.Add(this.expandableSplitter2);
            this.panelDockContainer4.Controls.Add(this.panelEx3);
            this.panelDockContainer4.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer4.Name = "panelDockContainer4";
            this.panelDockContainer4.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer4.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer4.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer4.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer4.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer4.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer4.Style.GradientAngle = 90;
            this.panelDockContainer4.TabIndex = 8;
            // 
            // panelEx4
            // 
            this.panelEx4.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx4.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx4.Controls.Add(this.dataGridViewX2);
            this.panelEx4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx4.Location = new System.Drawing.Point(0, 381);
            this.panelEx4.Name = "panelEx4";
            this.panelEx4.Size = new System.Drawing.Size(236, 414);
            this.panelEx4.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx4.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx4.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx4.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx4.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx4.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx4.Style.GradientAngle = 90;
            this.panelEx4.TabIndex = 2;
            // 
            // dataGridViewX2
            // 
            this.dataGridViewX2.AllowUserToAddRows = false;
            this.dataGridViewX2.AllowUserToDeleteRows = false;
            this.dataGridViewX2.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.dataGridViewX2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dataGridViewX2.DefaultCellStyle = dataGridViewCellStyle1;
            this.dataGridViewX2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridViewX2.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.dataGridViewX2.Location = new System.Drawing.Point(0, 0);
            this.dataGridViewX2.MultiSelect = false;
            this.dataGridViewX2.Name = "dataGridViewX2";
            this.dataGridViewX2.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.AutoSizeToDisplayedHeaders;
            this.dataGridViewX2.RowTemplate.Height = 23;
            this.dataGridViewX2.Size = new System.Drawing.Size(236, 414);
            this.dataGridViewX2.TabIndex = 0;
            this.dataGridViewX2.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewX2_CellValueChanged);
            this.dataGridViewX2.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridViewX2_CellMouseDown);
            // 
            // expandableSplitter2
            // 
            this.expandableSplitter2.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter2.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter2.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.expandableSplitter2.Dock = System.Windows.Forms.DockStyle.Top;
            this.expandableSplitter2.ExpandFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter2.ExpandFillColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter2.ExpandLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter2.ExpandLineColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter2.GripDarkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter2.GripDarkColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter2.GripLightColor = System.Drawing.Color.FromArgb(((int)(((byte)(227)))), ((int)(((byte)(239)))), ((int)(((byte)(255)))));
            this.expandableSplitter2.GripLightColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.expandableSplitter2.HotBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(252)))), ((int)(((byte)(151)))), ((int)(((byte)(61)))));
            this.expandableSplitter2.HotBackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(184)))), ((int)(((byte)(94)))));
            this.expandableSplitter2.HotBackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemPressedBackground2;
            this.expandableSplitter2.HotBackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemPressedBackground;
            this.expandableSplitter2.HotExpandFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter2.HotExpandFillColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter2.HotExpandLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.expandableSplitter2.HotExpandLineColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandableSplitter2.HotGripDarkColor = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter2.HotGripDarkColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter2.HotGripLightColor = System.Drawing.Color.FromArgb(((int)(((byte)(227)))), ((int)(((byte)(239)))), ((int)(((byte)(255)))));
            this.expandableSplitter2.HotGripLightColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.expandableSplitter2.Location = new System.Drawing.Point(0, 371);
            this.expandableSplitter2.Name = "expandableSplitter2";
            this.expandableSplitter2.Size = new System.Drawing.Size(236, 10);
            this.expandableSplitter2.Style = DevComponents.DotNetBar.eSplitterStyle.Office2007;
            this.expandableSplitter2.TabIndex = 1;
            this.expandableSplitter2.TabStop = false;
            // 
            // panelEx3
            // 
            this.panelEx3.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx3.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx3.Controls.Add(this.buttonX9);
            this.panelEx3.Controls.Add(this.buttonX8);
            this.panelEx3.Controls.Add(this.buttonX7);
            this.panelEx3.Controls.Add(this.buttonX6);
            this.panelEx3.Controls.Add(this.buttonX5);
            this.panelEx3.Controls.Add(this.trafficTree);
            this.panelEx3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelEx3.Location = new System.Drawing.Point(0, 0);
            this.panelEx3.Name = "panelEx3";
            this.panelEx3.Size = new System.Drawing.Size(236, 371);
            this.panelEx3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx3.Style.GradientAngle = 90;
            this.panelEx3.TabIndex = 0;
            // 
            // buttonX9
            // 
            this.buttonX9.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX9.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX9.Location = new System.Drawing.Point(104, 335);
            this.buttonX9.Name = "buttonX9";
            this.buttonX9.Size = new System.Drawing.Size(120, 20);
            this.buttonX9.TabIndex = 5;
            this.buttonX9.Text = "删除交通路径及小点";
            this.buttonX9.Click += new System.EventHandler(this.buttonX9_Click);
            // 
            // buttonX8
            // 
            this.buttonX8.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX8.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX8.Location = new System.Drawing.Point(10, 335);
            this.buttonX8.Name = "buttonX8";
            this.buttonX8.Size = new System.Drawing.Size(80, 20);
            this.buttonX8.TabIndex = 4;
            this.buttonX8.Text = "点前添加小点";
            this.buttonX8.Click += new System.EventHandler(this.buttonX8_Click);
            // 
            // buttonX7
            // 
            this.buttonX7.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX7.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX7.Location = new System.Drawing.Point(104, 301);
            this.buttonX7.Name = "buttonX7";
            this.buttonX7.Size = new System.Drawing.Size(120, 20);
            this.buttonX7.TabIndex = 3;
            this.buttonX7.Text = "添加交通路径及小点";
            this.buttonX7.Click += new System.EventHandler(this.buttonX7_Click);
            // 
            // buttonX6
            // 
            this.buttonX6.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX6.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX6.Location = new System.Drawing.Point(10, 301);
            this.buttonX6.Name = "buttonX6";
            this.buttonX6.Size = new System.Drawing.Size(80, 20);
            this.buttonX6.TabIndex = 2;
            this.buttonX6.Text = "添加大交通点";
            this.buttonX6.Click += new System.EventHandler(this.buttonX6_Click);
            // 
            // buttonX5
            // 
            this.buttonX5.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX5.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX5.Location = new System.Drawing.Point(11, 267);
            this.buttonX5.Name = "buttonX5";
            this.buttonX5.Size = new System.Drawing.Size(70, 20);
            this.buttonX5.TabIndex = 1;
            this.buttonX5.Text = "刷新";
            this.buttonX5.Click += new System.EventHandler(this.buttonX5_Click);
            // 
            // trafficTree
            // 
            this.trafficTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.trafficTree.AllowDrop = true;
            this.trafficTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.trafficTree.BackgroundStyle.Class = "TreeBorderKey";
            this.trafficTree.DragDropEnabled = false;
            this.trafficTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.trafficTree.Location = new System.Drawing.Point(11, 11);
            this.trafficTree.Name = "trafficTree";
            this.trafficTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node2,
            this.node2});
            this.trafficTree.NodesConnector = this.nodeConnector2;
            this.trafficTree.NodeStyle = this.elementStyle2;
            this.trafficTree.PathSeparator = ";";
            this.trafficTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.trafficTree.Size = new System.Drawing.Size(213, 240);
            this.trafficTree.Styles.Add(this.elementStyle2);
            this.trafficTree.Styles.Add(this.elementStyle2);
            this.trafficTree.SuspendPaint = false;
            this.trafficTree.TabIndex = 0;
            this.trafficTree.Text = "trafficTree";
            this.trafficTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTree2_AfterNodeSelect);
            // 
            // node2
            // 
            this.node2.Expanded = true;
            this.node2.Name = "node2";
            this.node2.Text = "node2";
            // 
            // nodeConnector2
            // 
            this.nodeConnector2.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle2
            // 
            this.elementStyle2.Name = "elementStyle2";
            this.elementStyle2.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // panelDockContainer6
            // 
            this.panelDockContainer6.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer6.Controls.Add(this.panelEx5);
            this.panelDockContainer6.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer6.Name = "panelDockContainer6";
            this.panelDockContainer6.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer6.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer6.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer6.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer6.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer6.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer6.Style.GradientAngle = 90;
            this.panelDockContainer6.TabIndex = 12;
            // 
            // panelEx5
            // 
            this.panelEx5.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx5.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx5.Controls.Add(this.groupPanel3);
            this.panelEx5.Controls.Add(this.groupPanel2);
            this.panelEx5.Controls.Add(this.groupPanel1);
            this.panelEx5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx5.Location = new System.Drawing.Point(0, 0);
            this.panelEx5.Name = "panelEx5";
            this.panelEx5.Size = new System.Drawing.Size(236, 795);
            this.panelEx5.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx5.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx5.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx5.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx5.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx5.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx5.Style.GradientAngle = 90;
            this.panelEx5.TabIndex = 0;
            // 
            // groupPanel3
            // 
            this.groupPanel3.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel3.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel3.Controls.Add(this.buttonX21);
            this.groupPanel3.DrawTitleBox = false;
            this.groupPanel3.Location = new System.Drawing.Point(10, 714);
            this.groupPanel3.Name = "groupPanel3";
            this.groupPanel3.Size = new System.Drawing.Size(217, 64);
            // 
            // 
            // 
            this.groupPanel3.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel3.Style.BackColorGradientAngle = 90;
            this.groupPanel3.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel3.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel3.Style.BorderBottomWidth = 1;
            this.groupPanel3.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel3.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel3.Style.BorderLeftWidth = 1;
            this.groupPanel3.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel3.Style.BorderRightWidth = 1;
            this.groupPanel3.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel3.Style.BorderTopWidth = 1;
            this.groupPanel3.Style.CornerDiameter = 4;
            this.groupPanel3.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel3.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel3.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel3.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel3.TabIndex = 2;
            this.groupPanel3.Text = "其他功能";
            // 
            // buttonX21
            // 
            this.buttonX21.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX21.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX21.Location = new System.Drawing.Point(68, 6);
            this.buttonX21.Name = "buttonX21";
            this.buttonX21.Size = new System.Drawing.Size(70, 20);
            this.buttonX21.TabIndex = 0;
            this.buttonX21.Text = "管理分组";
            this.buttonX21.Click += new System.EventHandler(this.buttonX21_Click);
            // 
            // groupPanel2
            // 
            this.groupPanel2.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel2.Controls.Add(this.bReplaceAllNpcTemplate);
            this.groupPanel2.Controls.Add(this.bReplaceNpcTemplate);
            this.groupPanel2.Controls.Add(this.buttonX11);
            this.groupPanel2.Controls.Add(this.buttonX10);
            this.groupPanel2.Controls.Add(this.textBoxX1);
            this.groupPanel2.Controls.Add(this.npcTree);
            this.groupPanel2.DrawTitleBox = false;
            this.groupPanel2.Location = new System.Drawing.Point(11, 328);
            this.groupPanel2.Name = "groupPanel2";
            this.groupPanel2.Size = new System.Drawing.Size(217, 365);
            // 
            // 
            // 
            this.groupPanel2.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel2.Style.BackColorGradientAngle = 90;
            this.groupPanel2.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel2.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderBottomWidth = 1;
            this.groupPanel2.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel2.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderLeftWidth = 1;
            this.groupPanel2.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderRightWidth = 1;
            this.groupPanel2.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderTopWidth = 1;
            this.groupPanel2.Style.CornerDiameter = 4;
            this.groupPanel2.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel2.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel2.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel2.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel2.TabIndex = 1;
            this.groupPanel2.Text = "资源列表";
            // 
            // bReplaceAllNpcTemplate
            // 
            this.bReplaceAllNpcTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bReplaceAllNpcTemplate.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bReplaceAllNpcTemplate.Location = new System.Drawing.Point(106, 38);
            this.bReplaceAllNpcTemplate.Name = "bReplaceAllNpcTemplate";
            this.bReplaceAllNpcTemplate.Size = new System.Drawing.Size(100, 20);
            this.bReplaceAllNpcTemplate.TabIndex = 5;
            this.bReplaceAllNpcTemplate.Text = "全场景替换模板";
            this.bReplaceAllNpcTemplate.Click += new System.EventHandler(this.bReplaceAllNpcTemplate_Click);
            // 
            // bReplaceNpcTemplate
            // 
            this.bReplaceNpcTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bReplaceNpcTemplate.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bReplaceNpcTemplate.Location = new System.Drawing.Point(30, 38);
            this.bReplaceNpcTemplate.Name = "bReplaceNpcTemplate";
            this.bReplaceNpcTemplate.Size = new System.Drawing.Size(70, 20);
            this.bReplaceNpcTemplate.TabIndex = 4;
            this.bReplaceNpcTemplate.Text = "替换模板";
            this.bReplaceNpcTemplate.Click += new System.EventHandler(this.bReplaceNpcTemplate_Click);
            // 
            // buttonX11
            // 
            this.buttonX11.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX11.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX11.Location = new System.Drawing.Point(156, 3);
            this.buttonX11.Name = "buttonX11";
            this.buttonX11.Size = new System.Drawing.Size(50, 20);
            this.buttonX11.TabIndex = 3;
            this.buttonX11.Text = "刷新";
            this.buttonX11.Click += new System.EventHandler(this.buttonX11_Click);
            // 
            // buttonX10
            // 
            this.buttonX10.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX10.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX10.Location = new System.Drawing.Point(100, 3);
            this.buttonX10.Name = "buttonX10";
            this.buttonX10.Size = new System.Drawing.Size(50, 20);
            this.buttonX10.TabIndex = 2;
            this.buttonX10.Text = "搜索";
            this.buttonX10.Click += new System.EventHandler(this.buttonX10_Click);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(4, 3);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(80, 21);
            this.textBoxX1.TabIndex = 1;
            // 
            // npcTree
            // 
            this.npcTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.npcTree.AllowDrop = true;
            this.npcTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.npcTree.BackgroundStyle.Class = "TreeBorderKey";
            this.npcTree.DragDropEnabled = false;
            this.npcTree.HotTracking = true;
            this.npcTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.npcTree.Location = new System.Drawing.Point(4, 72);
            this.npcTree.Name = "npcTree";
            this.npcTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node3,
            this.node3});
            this.npcTree.NodesConnector = this.nodeConnector3;
            this.npcTree.NodeStyle = this.elementStyle3;
            this.npcTree.PathSeparator = ";";
            this.npcTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.npcTree.Size = new System.Drawing.Size(202, 261);
            this.npcTree.Styles.Add(this.elementStyle3);
            this.npcTree.Styles.Add(this.elementStyle3);
            this.npcTree.SuspendPaint = false;
            this.npcTree.TabIndex = 0;
            this.npcTree.Text = "npcTree";
            this.npcTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.npcTree_AfterNodeSelect);
            // 
            // node3
            // 
            this.node3.Expanded = true;
            this.node3.Name = "node3";
            this.node3.Text = "node3";
            // 
            // nodeConnector3
            // 
            this.nodeConnector3.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle3
            // 
            this.elementStyle3.Name = "elementStyle3";
            this.elementStyle3.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.buttonX20);
            this.groupPanel1.Controls.Add(this.buttonX19);
            this.groupPanel1.Controls.Add(this.labelX1);
            this.groupPanel1.Controls.Add(this.previewCanvas1);
            this.groupPanel1.DrawTitleBox = false;
            this.groupPanel1.Location = new System.Drawing.Point(11, 12);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(216, 296);
            // 
            // 
            // 
            this.groupPanel1.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel1.Style.BackColorGradientAngle = 90;
            this.groupPanel1.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel1.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderBottomWidth = 1;
            this.groupPanel1.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel1.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderLeftWidth = 1;
            this.groupPanel1.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderRightWidth = 1;
            this.groupPanel1.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderTopWidth = 1;
            this.groupPanel1.Style.CornerDiameter = 4;
            this.groupPanel1.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel1.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel1.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel1.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel1.TabIndex = 0;
            this.groupPanel1.Text = "预览窗口";
            // 
            // buttonX20
            // 
            this.buttonX20.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX20.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX20.Enabled = false;
            this.buttonX20.Location = new System.Drawing.Point(181, 239);
            this.buttonX20.Name = "buttonX20";
            this.buttonX20.Size = new System.Drawing.Size(25, 20);
            this.buttonX20.TabIndex = 3;
            this.buttonX20.Text = ">>";
            this.buttonX20.Click += new System.EventHandler(this.buttonX20_Click);
            // 
            // buttonX19
            // 
            this.buttonX19.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX19.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX19.Enabled = false;
            this.buttonX19.Location = new System.Drawing.Point(4, 239);
            this.buttonX19.Name = "buttonX19";
            this.buttonX19.Size = new System.Drawing.Size(25, 20);
            this.buttonX19.TabIndex = 2;
            this.buttonX19.Text = "<<";
            this.buttonX19.Click += new System.EventHandler(this.buttonX19_Click);
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(44, 239);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(120, 20);
            this.labelX1.TabIndex = 1;
            this.labelX1.Text = "无动作";
            this.labelX1.TextAlignment = System.Drawing.StringAlignment.Center;
            // 
            // previewCanvas1
            // 
            this.previewCanvas1.Location = new System.Drawing.Point(4, 3);
            this.previewCanvas1.Name = "previewCanvas1";
            this.previewCanvas1.Size = new System.Drawing.Size(202, 225);
            this.previewCanvas1.TabIndex = 0;
            // 
            // panelDockContainer3
            // 
            this.panelDockContainer3.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer3.Controls.Add(this.groupBoxBrushParams);
            this.panelDockContainer3.Controls.Add(this.groupBox2);
            this.panelDockContainer3.Controls.Add(this.groupBox1);
            this.panelDockContainer3.Controls.Add(this.groupboxPoly);
            this.panelDockContainer3.Controls.Add(this.advTreePoly);
            this.panelDockContainer3.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer3.Name = "panelDockContainer3";
            this.panelDockContainer3.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer3.Style.GradientAngle = 90;
            this.panelDockContainer3.TabIndex = 15;
            // 
            // groupBoxBrushParams
            // 
            this.groupBoxBrushParams.BackColor = System.Drawing.Color.Transparent;
            this.groupBoxBrushParams.Controls.Add(this.label3);
            this.groupBoxBrushParams.Controls.Add(this.label2);
            this.groupBoxBrushParams.Controls.Add(this.label1);
            this.groupBoxBrushParams.Controls.Add(this.sliderBrushHeight);
            this.groupBoxBrushParams.Controls.Add(this.sliderDisplayWidth);
            this.groupBoxBrushParams.Controls.Add(this.sliderBrushRadius);
            this.groupBoxBrushParams.Controls.Add(this.radioEcllipse);
            this.groupBoxBrushParams.Controls.Add(this.radioRect);
            this.groupBoxBrushParams.Location = new System.Drawing.Point(3, 651);
            this.groupBoxBrushParams.Name = "groupBoxBrushParams";
            this.groupBoxBrushParams.Size = new System.Drawing.Size(230, 117);
            this.groupBoxBrushParams.TabIndex = 19;
            this.groupBoxBrushParams.TabStop = false;
            this.groupBoxBrushParams.Text = "笔刷参数";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 92);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "高度";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 69);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "视野";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 45);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "半径";
            // 
            // sliderBrushHeight
            // 
            this.sliderBrushHeight.Location = new System.Drawing.Point(52, 88);
            this.sliderBrushHeight.Maximum = 20;
            this.sliderBrushHeight.Minimum = 1;
            this.sliderBrushHeight.Name = "sliderBrushHeight";
            this.sliderBrushHeight.Size = new System.Drawing.Size(165, 23);
            this.sliderBrushHeight.TabIndex = 1;
            this.sliderBrushHeight.Text = "1";
            this.sliderBrushHeight.Value = 1;
            this.sliderBrushHeight.ValueChanged += new System.EventHandler(this.LogicBrushParamsChanged);
            // 
            // sliderDisplayWidth
            // 
            this.sliderDisplayWidth.Location = new System.Drawing.Point(52, 65);
            this.sliderDisplayWidth.Maximum = 64;
            this.sliderDisplayWidth.Minimum = 1;
            this.sliderDisplayWidth.Name = "sliderDisplayWidth";
            this.sliderDisplayWidth.Size = new System.Drawing.Size(165, 23);
            this.sliderDisplayWidth.TabIndex = 1;
            this.sliderDisplayWidth.Text = "1";
            this.sliderDisplayWidth.Value = 1;
            this.sliderDisplayWidth.ValueChanged += new System.EventHandler(this.LogicBrushParamsChanged);
            // 
            // sliderBrushRadius
            // 
            this.sliderBrushRadius.Location = new System.Drawing.Point(52, 42);
            this.sliderBrushRadius.Minimum = 1;
            this.sliderBrushRadius.Name = "sliderBrushRadius";
            this.sliderBrushRadius.Size = new System.Drawing.Size(165, 19);
            this.sliderBrushRadius.TabIndex = 1;
            this.sliderBrushRadius.Text = "5";
            this.sliderBrushRadius.Value = 5;
            this.sliderBrushRadius.ValueChanged += new System.EventHandler(this.LogicBrushParamsChanged);
            // 
            // radioEcllipse
            // 
            this.radioEcllipse.AutoSize = true;
            this.radioEcllipse.Location = new System.Drawing.Point(124, 20);
            this.radioEcllipse.Name = "radioEcllipse";
            this.radioEcllipse.Size = new System.Drawing.Size(47, 16);
            this.radioEcllipse.TabIndex = 0;
            this.radioEcllipse.Text = "圆形";
            this.radioEcllipse.UseVisualStyleBackColor = true;
            this.radioEcllipse.CheckedChanged += new System.EventHandler(this.LogicBrushParamsChanged);
            // 
            // radioRect
            // 
            this.radioRect.AutoSize = true;
            this.radioRect.Checked = true;
            this.radioRect.Location = new System.Drawing.Point(10, 20);
            this.radioRect.Name = "radioRect";
            this.radioRect.Size = new System.Drawing.Size(47, 16);
            this.radioRect.TabIndex = 0;
            this.radioRect.TabStop = true;
            this.radioRect.Text = "方形";
            this.radioRect.UseVisualStyleBackColor = true;
            this.radioRect.CheckedChanged += new System.EventHandler(this.LogicBrushParamsChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.Color.Transparent;
            this.groupBox2.Controls.Add(this.checkBoxShowPoly);
            this.groupBox2.Controls.Add(this.checkShowLogicCell);
            this.groupBox2.Controls.Add(this.checkShowTerrainCell);
            this.groupBox2.Controls.Add(this.checkShowItemCell);
            this.groupBox2.Controls.Add(this.checkShowSelectedCell);
            this.groupBox2.Location = new System.Drawing.Point(3, 523);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(230, 123);
            this.groupBox2.TabIndex = 18;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "显示参数";
            // 
            // checkBoxShowPoly
            // 
            this.checkBoxShowPoly.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxShowPoly.Checked = true;
            this.checkBoxShowPoly.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxShowPoly.CheckValue = "Y";
            this.checkBoxShowPoly.Location = new System.Drawing.Point(6, 14);
            this.checkBoxShowPoly.Name = "checkBoxShowPoly";
            this.checkBoxShowPoly.Size = new System.Drawing.Size(165, 23);
            this.checkBoxShowPoly.TabIndex = 15;
            this.checkBoxShowPoly.Text = "显示多边形";
            this.checkBoxShowPoly.CheckedChanged += new System.EventHandler(this.checkBoxX1_CheckedChanged);
            // 
            // checkShowLogicCell
            // 
            this.checkShowLogicCell.BackColor = System.Drawing.Color.Transparent;
            this.checkShowLogicCell.Location = new System.Drawing.Point(6, 35);
            this.checkShowLogicCell.Name = "checkShowLogicCell";
            this.checkShowLogicCell.Size = new System.Drawing.Size(165, 23);
            this.checkShowLogicCell.TabIndex = 15;
            this.checkShowLogicCell.Text = "显示逻辑障碍";
            this.checkShowLogicCell.CheckedChanged += new System.EventHandler(this.LogicDisplayParamsChanged);
            // 
            // checkShowTerrainCell
            // 
            this.checkShowTerrainCell.BackColor = System.Drawing.Color.Transparent;
            this.checkShowTerrainCell.Enabled = false;
            this.checkShowTerrainCell.Location = new System.Drawing.Point(37, 56);
            this.checkShowTerrainCell.Name = "checkShowTerrainCell";
            this.checkShowTerrainCell.Size = new System.Drawing.Size(151, 23);
            this.checkShowTerrainCell.TabIndex = 15;
            this.checkShowTerrainCell.Text = "显示地表障碍";
            this.checkShowTerrainCell.CheckedChanged += new System.EventHandler(this.LogicDisplayParamsChanged);
            // 
            // checkShowItemCell
            // 
            this.checkShowItemCell.BackColor = System.Drawing.Color.Transparent;
            this.checkShowItemCell.Enabled = false;
            this.checkShowItemCell.Location = new System.Drawing.Point(37, 76);
            this.checkShowItemCell.Name = "checkShowItemCell";
            this.checkShowItemCell.Size = new System.Drawing.Size(151, 23);
            this.checkShowItemCell.TabIndex = 15;
            this.checkShowItemCell.Text = "显示物件障碍";
            this.checkShowItemCell.CheckedChanged += new System.EventHandler(this.LogicDisplayParamsChanged);
            // 
            // checkShowSelectedCell
            // 
            this.checkShowSelectedCell.BackColor = System.Drawing.Color.Transparent;
            this.checkShowSelectedCell.Enabled = false;
            this.checkShowSelectedCell.Location = new System.Drawing.Point(37, 96);
            this.checkShowSelectedCell.Name = "checkShowSelectedCell";
            this.checkShowSelectedCell.Size = new System.Drawing.Size(168, 23);
            this.checkShowSelectedCell.TabIndex = 15;
            this.checkShowSelectedCell.Text = "显示选中物件障碍";
            this.checkShowSelectedCell.CheckedChanged += new System.EventHandler(this.LogicDisplayParamsChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.buttonX13);
            this.groupBox1.Controls.Add(this.buttonBuildPoly);
            this.groupBox1.Controls.Add(this.buttonX15);
            this.groupBox1.Controls.Add(this.buttonX14);
            this.groupBox1.Location = new System.Drawing.Point(3, 401);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(230, 116);
            this.groupBox1.TabIndex = 17;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "添加";
            // 
            // buttonX13
            // 
            this.buttonX13.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX13.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX13.Location = new System.Drawing.Point(13, 21);
            this.buttonX13.Name = "buttonX13";
            this.buttonX13.Size = new System.Drawing.Size(206, 23);
            this.buttonX13.TabIndex = 12;
            this.buttonX13.Text = "新建逻辑数据";
            this.buttonX13.Click += new System.EventHandler(this.buttonX13_Click);
            // 
            // buttonBuildPoly
            // 
            this.buttonBuildPoly.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonBuildPoly.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonBuildPoly.Location = new System.Drawing.Point(149, 51);
            this.buttonBuildPoly.Name = "buttonBuildPoly";
            this.buttonBuildPoly.Size = new System.Drawing.Size(70, 23);
            this.buttonBuildPoly.TabIndex = 12;
            this.buttonBuildPoly.Text = "多边形";
            this.buttonBuildPoly.Click += new System.EventHandler(this.buttonBuildPoly_Click);
            // 
            // buttonX15
            // 
            this.buttonX15.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX15.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX15.Location = new System.Drawing.Point(13, 51);
            this.buttonX15.Name = "buttonX15";
            this.buttonX15.Size = new System.Drawing.Size(128, 23);
            this.buttonX15.TabIndex = 12;
            this.buttonX15.Text = "新建多边形并添加顶点";
            this.buttonX15.Click += new System.EventHandler(this.buttonX15_Click);
            // 
            // buttonX14
            // 
            this.buttonX14.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX14.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX14.Location = new System.Drawing.Point(13, 81);
            this.buttonX14.Name = "buttonX14";
            this.buttonX14.Size = new System.Drawing.Size(206, 23);
            this.buttonX14.TabIndex = 11;
            this.buttonX14.Text = "顶点添加结束并转换为逻辑区域";
            this.buttonX14.Click += new System.EventHandler(this.buttonX14_Click);
            // 
            // groupboxPoly
            // 
            this.groupboxPoly.BackColor = System.Drawing.Color.Transparent;
            this.groupboxPoly.Controls.Add(this.colorPicker);
            this.groupboxPoly.Controls.Add(this.labelX6);
            this.groupboxPoly.Controls.Add(this.labelX5);
            this.groupboxPoly.Controls.Add(this.labelX4);
            this.groupboxPoly.Controls.Add(this.checkBoxBlock);
            this.groupboxPoly.Controls.Add(this.labelX3);
            this.groupboxPoly.Controls.Add(this.buttonX16);
            this.groupboxPoly.Controls.Add(this.buttonOK);
            this.groupboxPoly.Controls.Add(this.buttonClearScript);
            this.groupboxPoly.Controls.Add(this.checkBoxRideHorse);
            this.groupboxPoly.Controls.Add(this.buttonBrowseScript);
            this.groupboxPoly.Controls.Add(this.checkBoxRest);
            this.groupboxPoly.Controls.Add(this.textLogicName);
            this.groupboxPoly.Controls.Add(this.checkBoxPut);
            this.groupboxPoly.Controls.Add(this.textBoxHeight);
            this.groupboxPoly.Controls.Add(this.checkBoxStall);
            this.groupboxPoly.Controls.Add(this.checkBoxIndoor);
            this.groupboxPoly.Controls.Add(this.textBoxScript);
            this.groupboxPoly.Location = new System.Drawing.Point(3, 226);
            this.groupboxPoly.Name = "groupboxPoly";
            this.groupboxPoly.Size = new System.Drawing.Size(230, 173);
            this.groupboxPoly.TabIndex = 1;
            this.groupboxPoly.TabStop = false;
            this.groupboxPoly.Text = "多边形属性";
            // 
            // colorPicker
            // 
            this.colorPicker.CanvasColor = System.Drawing.SystemColors.Control;
            this.colorPicker.Location = new System.Drawing.Point(44, 140);
            this.colorPicker.Name = "colorPicker";
            this.colorPicker.Size = new System.Drawing.Size(43, 23);
            this.colorPicker.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.colorPicker.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.colorPicker.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.colorPicker.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.colorPicker.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.colorPicker.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.colorPicker.Style.GradientAngle = 90;
            this.colorPicker.TabIndex = 31;
            this.colorPicker.Click += new System.EventHandler(this.colorPicker_Click);
            // 
            // labelX6
            // 
            this.labelX6.BackColor = System.Drawing.Color.Transparent;
            this.labelX6.Location = new System.Drawing.Point(12, 140);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(29, 23);
            this.labelX6.TabIndex = 29;
            this.labelX6.Text = "颜色";
            // 
            // labelX5
            // 
            this.labelX5.BackColor = System.Drawing.Color.Transparent;
            this.labelX5.Location = new System.Drawing.Point(12, 113);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(29, 23);
            this.labelX5.TabIndex = 29;
            this.labelX5.Text = "名字";
            // 
            // labelX4
            // 
            this.labelX4.BackColor = System.Drawing.Color.Transparent;
            this.labelX4.Location = new System.Drawing.Point(124, 113);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(32, 23);
            this.labelX4.TabIndex = 29;
            this.labelX4.Text = "高度";
            // 
            // checkBoxBlock
            // 
            this.checkBoxBlock.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxBlock.Location = new System.Drawing.Point(15, 18);
            this.checkBoxBlock.Name = "checkBoxBlock";
            this.checkBoxBlock.Size = new System.Drawing.Size(75, 23);
            this.checkBoxBlock.TabIndex = 23;
            this.checkBoxBlock.Text = "障碍";
            // 
            // labelX3
            // 
            this.labelX3.BackColor = System.Drawing.Color.Transparent;
            this.labelX3.Location = new System.Drawing.Point(12, 83);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(29, 23);
            this.labelX3.TabIndex = 30;
            this.labelX3.Text = "脚本";
            // 
            // buttonX16
            // 
            this.buttonX16.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX16.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX16.ForeColor = System.Drawing.SystemColors.ControlText;
            this.buttonX16.Location = new System.Drawing.Point(161, 140);
            this.buttonX16.Name = "buttonX16";
            this.buttonX16.Size = new System.Drawing.Size(62, 23);
            this.buttonX16.TabIndex = 13;
            this.buttonX16.Text = "Ⅹ 删除";
            this.buttonX16.Click += new System.EventHandler(this.buttonDel_Click);
            // 
            // buttonOK
            // 
            this.buttonOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonOK.ForeColor = System.Drawing.SystemColors.ControlText;
            this.buttonOK.Location = new System.Drawing.Point(93, 140);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(62, 23);
            this.buttonOK.TabIndex = 13;
            this.buttonOK.Text = "√ 确定";
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonClearScript
            // 
            this.buttonClearScript.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonClearScript.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonClearScript.Location = new System.Drawing.Point(192, 83);
            this.buttonClearScript.Name = "buttonClearScript";
            this.buttonClearScript.Size = new System.Drawing.Size(26, 23);
            this.buttonClearScript.TabIndex = 19;
            this.buttonClearScript.Text = "×";
            // 
            // checkBoxRideHorse
            // 
            this.checkBoxRideHorse.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxRideHorse.Location = new System.Drawing.Point(136, 58);
            this.checkBoxRideHorse.Name = "checkBoxRideHorse";
            this.checkBoxRideHorse.Size = new System.Drawing.Size(75, 23);
            this.checkBoxRideHorse.TabIndex = 27;
            this.checkBoxRideHorse.Text = "下马区域";
            // 
            // buttonBrowseScript
            // 
            this.buttonBrowseScript.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonBrowseScript.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonBrowseScript.Location = new System.Drawing.Point(162, 83);
            this.buttonBrowseScript.Name = "buttonBrowseScript";
            this.buttonBrowseScript.Size = new System.Drawing.Size(28, 23);
            this.buttonBrowseScript.TabIndex = 20;
            this.buttonBrowseScript.Text = "...";
            // 
            // checkBoxRest
            // 
            this.checkBoxRest.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxRest.Location = new System.Drawing.Point(15, 59);
            this.checkBoxRest.Name = "checkBoxRest";
            this.checkBoxRest.Size = new System.Drawing.Size(75, 23);
            this.checkBoxRest.TabIndex = 28;
            this.checkBoxRest.Text = "休息区域";
            // 
            // textLogicName
            // 
            // 
            // 
            // 
            this.textLogicName.Border.Class = "TextBoxBorder";
            this.textLogicName.Location = new System.Drawing.Point(44, 113);
            this.textLogicName.Name = "textLogicName";
            this.textLogicName.Size = new System.Drawing.Size(73, 21);
            this.textLogicName.TabIndex = 21;
            // 
            // checkBoxPut
            // 
            this.checkBoxPut.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxPut.Location = new System.Drawing.Point(136, 17);
            this.checkBoxPut.Name = "checkBoxPut";
            this.checkBoxPut.Size = new System.Drawing.Size(75, 23);
            this.checkBoxPut.TabIndex = 26;
            this.checkBoxPut.Text = "可摆放物件";
            // 
            // textBoxHeight
            // 
            // 
            // 
            // 
            this.textBoxHeight.Border.Class = "TextBoxBorder";
            this.textBoxHeight.Location = new System.Drawing.Point(154, 113);
            this.textBoxHeight.Name = "textBoxHeight";
            this.textBoxHeight.Size = new System.Drawing.Size(67, 21);
            this.textBoxHeight.TabIndex = 21;
            // 
            // checkBoxStall
            // 
            this.checkBoxStall.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxStall.Location = new System.Drawing.Point(15, 39);
            this.checkBoxStall.Name = "checkBoxStall";
            this.checkBoxStall.Size = new System.Drawing.Size(75, 23);
            this.checkBoxStall.TabIndex = 24;
            this.checkBoxStall.Text = "可摆摊";
            // 
            // checkBoxIndoor
            // 
            this.checkBoxIndoor.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxIndoor.Location = new System.Drawing.Point(136, 38);
            this.checkBoxIndoor.Name = "checkBoxIndoor";
            this.checkBoxIndoor.Size = new System.Drawing.Size(75, 23);
            this.checkBoxIndoor.TabIndex = 25;
            this.checkBoxIndoor.Text = "室内区域";
            // 
            // textBoxScript
            // 
            // 
            // 
            // 
            this.textBoxScript.Border.Class = "TextBoxBorder";
            this.textBoxScript.Location = new System.Drawing.Point(44, 84);
            this.textBoxScript.Name = "textBoxScript";
            this.textBoxScript.Size = new System.Drawing.Size(111, 21);
            this.textBoxScript.TabIndex = 22;
            // 
            // advTreePoly
            // 
            this.advTreePoly.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.advTreePoly.AllowDrop = true;
            this.advTreePoly.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.advTreePoly.BackgroundStyle.Class = "TreeBorderKey";
            this.advTreePoly.Dock = System.Windows.Forms.DockStyle.Top;
            this.advTreePoly.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.advTreePoly.Location = new System.Drawing.Point(0, 0);
            this.advTreePoly.Name = "advTreePoly";
            this.advTreePoly.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node5,
            this.node5});
            this.advTreePoly.NodesConnector = this.nodeConnector5;
            this.advTreePoly.NodeStyle = this.elementStyle5;
            this.advTreePoly.PathSeparator = ";";
            this.advTreePoly.Size = new System.Drawing.Size(236, 220);
            this.advTreePoly.Styles.Add(this.elementStyle5);
            this.advTreePoly.Styles.Add(this.elementStyle5);
            this.advTreePoly.SuspendPaint = false;
            this.advTreePoly.TabIndex = 16;
            this.advTreePoly.Text = "advTree1";
            this.advTreePoly.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTreePoly_AfterNodeSelect);
            // 
            // node5
            // 
            this.node5.Expanded = true;
            this.node5.Name = "node5";
            this.node5.Text = "node5";
            // 
            // nodeConnector5
            // 
            this.nodeConnector5.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle5
            // 
            this.elementStyle5.Name = "elementStyle5";
            this.elementStyle5.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // panelDockContainer5
            // 
            this.panelDockContainer5.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer5.Controls.Add(this.panelEx6);
            this.panelDockContainer5.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer5.Name = "panelDockContainer5";
            this.panelDockContainer5.Size = new System.Drawing.Size(236, 795);
            this.panelDockContainer5.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer5.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer5.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer5.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer5.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer5.Style.GradientAngle = 90;
            this.panelDockContainer5.TabIndex = 13;
            // 
            // panelEx6
            // 
            this.panelEx6.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx6.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx6.Controls.Add(this.groupPanel7);
            this.panelEx6.Controls.Add(this.buttonX27);
            this.panelEx6.Controls.Add(this.groupPanel6);
            this.panelEx6.Controls.Add(this.groupPanel5);
            this.panelEx6.Controls.Add(this.groupPanel4);
            this.panelEx6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx6.Location = new System.Drawing.Point(0, 0);
            this.panelEx6.Name = "panelEx6";
            this.panelEx6.Size = new System.Drawing.Size(236, 795);
            this.panelEx6.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx6.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx6.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx6.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx6.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx6.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx6.Style.GradientAngle = 90;
            this.panelEx6.TabIndex = 0;
            // 
            // groupPanel7
            // 
            this.groupPanel7.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel7.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel7.Controls.Add(this.buttonX22);
            this.groupPanel7.DrawTitleBox = false;
            this.groupPanel7.Location = new System.Drawing.Point(-71, 1037);
            this.groupPanel7.Name = "groupPanel7";
            this.groupPanel7.Size = new System.Drawing.Size(218, 60);
            // 
            // 
            // 
            this.groupPanel7.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel7.Style.BackColorGradientAngle = 90;
            this.groupPanel7.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel7.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel7.Style.BorderBottomWidth = 1;
            this.groupPanel7.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel7.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel7.Style.BorderLeftWidth = 1;
            this.groupPanel7.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel7.Style.BorderRightWidth = 1;
            this.groupPanel7.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel7.Style.BorderTopWidth = 1;
            this.groupPanel7.Style.CornerDiameter = 4;
            this.groupPanel7.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel7.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel7.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel7.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel7.TabIndex = 4;
            this.groupPanel7.Text = "其他功能";
            // 
            // buttonX22
            // 
            this.buttonX22.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX22.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX22.Location = new System.Drawing.Point(66, 3);
            this.buttonX22.Name = "buttonX22";
            this.buttonX22.Size = new System.Drawing.Size(80, 20);
            this.buttonX22.TabIndex = 0;
            this.buttonX22.Text = "管理分组";
            // 
            // buttonX27
            // 
            this.buttonX27.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX27.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX27.Location = new System.Drawing.Point(9, 324);
            this.buttonX27.Name = "buttonX27";
            this.buttonX27.Size = new System.Drawing.Size(80, 20);
            this.buttonX27.TabIndex = 3;
            this.buttonX27.Text = "重生组检查";
            this.buttonX27.Click += new System.EventHandler(this.buttonX27_Click);
            // 
            // groupPanel6
            // 
            this.groupPanel6.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel6.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel6.Controls.Add(this.buttonX12);
            this.groupPanel6.DrawTitleBox = false;
            this.groupPanel6.Location = new System.Drawing.Point(9, 718);
            this.groupPanel6.Name = "groupPanel6";
            this.groupPanel6.Size = new System.Drawing.Size(218, 60);
            // 
            // 
            // 
            this.groupPanel6.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel6.Style.BackColorGradientAngle = 90;
            this.groupPanel6.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel6.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel6.Style.BorderBottomWidth = 1;
            this.groupPanel6.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel6.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel6.Style.BorderLeftWidth = 1;
            this.groupPanel6.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel6.Style.BorderRightWidth = 1;
            this.groupPanel6.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel6.Style.BorderTopWidth = 1;
            this.groupPanel6.Style.CornerDiameter = 4;
            this.groupPanel6.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel6.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel6.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel6.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel6.TabIndex = 2;
            this.groupPanel6.Text = "其他功能";
            // 
            // buttonX12
            // 
            this.buttonX12.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX12.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX12.Location = new System.Drawing.Point(66, 3);
            this.buttonX12.Name = "buttonX12";
            this.buttonX12.Size = new System.Drawing.Size(80, 20);
            this.buttonX12.TabIndex = 0;
            this.buttonX12.Text = "管理分组";
            this.buttonX12.Click += new System.EventHandler(this.ManageGroup);
            // 
            // groupPanel5
            // 
            this.groupPanel5.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel5.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel5.Controls.Add(this.bReplaceAllDoodadTemplate);
            this.groupPanel5.Controls.Add(this.bReplaceDoodadTemplate);
            this.groupPanel5.Controls.Add(this.buttonX24);
            this.groupPanel5.Controls.Add(this.buttonX23);
            this.groupPanel5.Controls.Add(this.textBoxX2);
            this.groupPanel5.Controls.Add(this.doodadTree);
            this.groupPanel5.DrawTitleBox = false;
            this.groupPanel5.Location = new System.Drawing.Point(10, 357);
            this.groupPanel5.Name = "groupPanel5";
            this.groupPanel5.Size = new System.Drawing.Size(217, 336);
            // 
            // 
            // 
            this.groupPanel5.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel5.Style.BackColorGradientAngle = 90;
            this.groupPanel5.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel5.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel5.Style.BorderBottomWidth = 1;
            this.groupPanel5.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel5.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel5.Style.BorderLeftWidth = 1;
            this.groupPanel5.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel5.Style.BorderRightWidth = 1;
            this.groupPanel5.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel5.Style.BorderTopWidth = 1;
            this.groupPanel5.Style.CornerDiameter = 4;
            this.groupPanel5.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel5.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel5.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel5.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel5.TabIndex = 1;
            this.groupPanel5.Text = "资源列表";
            // 
            // bReplaceAllDoodadTemplate
            // 
            this.bReplaceAllDoodadTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bReplaceAllDoodadTemplate.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bReplaceAllDoodadTemplate.Location = new System.Drawing.Point(105, 33);
            this.bReplaceAllDoodadTemplate.Name = "bReplaceAllDoodadTemplate";
            this.bReplaceAllDoodadTemplate.Size = new System.Drawing.Size(100, 20);
            this.bReplaceAllDoodadTemplate.TabIndex = 5;
            this.bReplaceAllDoodadTemplate.Text = "全场景替换模板";
            this.bReplaceAllDoodadTemplate.Click += new System.EventHandler(this.bReplaceAllDoodadTemplate_Click);
            // 
            // bReplaceDoodadTemplate
            // 
            this.bReplaceDoodadTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bReplaceDoodadTemplate.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bReplaceDoodadTemplate.Location = new System.Drawing.Point(29, 33);
            this.bReplaceDoodadTemplate.Name = "bReplaceDoodadTemplate";
            this.bReplaceDoodadTemplate.Size = new System.Drawing.Size(70, 20);
            this.bReplaceDoodadTemplate.TabIndex = 4;
            this.bReplaceDoodadTemplate.Text = "替换模板";
            this.bReplaceDoodadTemplate.Click += new System.EventHandler(this.bReplaceDoodadTemplate_Click);
            // 
            // buttonX24
            // 
            this.buttonX24.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX24.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX24.Location = new System.Drawing.Point(155, 2);
            this.buttonX24.Name = "buttonX24";
            this.buttonX24.Size = new System.Drawing.Size(50, 20);
            this.buttonX24.TabIndex = 3;
            this.buttonX24.Text = "刷新";
            this.buttonX24.Click += new System.EventHandler(this.buttonX24_Click);
            // 
            // buttonX23
            // 
            this.buttonX23.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX23.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX23.Location = new System.Drawing.Point(98, 2);
            this.buttonX23.Name = "buttonX23";
            this.buttonX23.Size = new System.Drawing.Size(50, 20);
            this.buttonX23.TabIndex = 2;
            this.buttonX23.Text = "搜索";
            this.buttonX23.Click += new System.EventHandler(this.buttonX23_Click);
            // 
            // textBoxX2
            // 
            // 
            // 
            // 
            this.textBoxX2.Border.Class = "TextBoxBorder";
            this.textBoxX2.Location = new System.Drawing.Point(5, 2);
            this.textBoxX2.Name = "textBoxX2";
            this.textBoxX2.Size = new System.Drawing.Size(80, 21);
            this.textBoxX2.TabIndex = 1;
            // 
            // doodadTree
            // 
            this.doodadTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.doodadTree.AllowDrop = true;
            this.doodadTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.doodadTree.BackgroundStyle.Class = "TreeBorderKey";
            this.doodadTree.DragDropEnabled = false;
            this.doodadTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.doodadTree.Location = new System.Drawing.Point(5, 59);
            this.doodadTree.Name = "doodadTree";
            this.doodadTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node4,
            this.node4});
            this.doodadTree.NodesConnector = this.nodeConnector4;
            this.doodadTree.NodeStyle = this.elementStyle4;
            this.doodadTree.PathSeparator = ";";
            this.doodadTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.doodadTree.Size = new System.Drawing.Size(200, 242);
            this.doodadTree.Styles.Add(this.elementStyle4);
            this.doodadTree.Styles.Add(this.elementStyle4);
            this.doodadTree.SuspendPaint = false;
            this.doodadTree.TabIndex = 0;
            this.doodadTree.Text = "advTree1";
            this.doodadTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.doodadTree_AfterNodeSelect);
            // 
            // node4
            // 
            this.node4.Expanded = true;
            this.node4.Name = "node4";
            this.node4.Text = "node4";
            // 
            // nodeConnector4
            // 
            this.nodeConnector4.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle4
            // 
            this.elementStyle4.Name = "elementStyle4";
            this.elementStyle4.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // groupPanel4
            // 
            this.groupPanel4.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel4.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel4.Controls.Add(this.buttonX26);
            this.groupPanel4.Controls.Add(this.buttonX25);
            this.groupPanel4.Controls.Add(this.labelX2);
            this.groupPanel4.Controls.Add(this.previewCanvas2);
            this.groupPanel4.DrawTitleBox = false;
            this.groupPanel4.Location = new System.Drawing.Point(9, 11);
            this.groupPanel4.Name = "groupPanel4";
            this.groupPanel4.Size = new System.Drawing.Size(218, 291);
            // 
            // 
            // 
            this.groupPanel4.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel4.Style.BackColorGradientAngle = 90;
            this.groupPanel4.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel4.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel4.Style.BorderBottomWidth = 1;
            this.groupPanel4.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel4.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel4.Style.BorderLeftWidth = 1;
            this.groupPanel4.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel4.Style.BorderRightWidth = 1;
            this.groupPanel4.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel4.Style.BorderTopWidth = 1;
            this.groupPanel4.Style.CornerDiameter = 4;
            this.groupPanel4.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel4.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel4.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel4.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel4.TabIndex = 0;
            this.groupPanel4.Text = "预览窗口";
            // 
            // buttonX26
            // 
            this.buttonX26.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX26.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX26.Enabled = false;
            this.buttonX26.Location = new System.Drawing.Point(181, 232);
            this.buttonX26.Name = "buttonX26";
            this.buttonX26.Size = new System.Drawing.Size(25, 20);
            this.buttonX26.TabIndex = 3;
            this.buttonX26.Text = ">>";
            this.buttonX26.Click += new System.EventHandler(this.buttonX26_Click);
            // 
            // buttonX25
            // 
            this.buttonX25.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX25.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX25.Enabled = false;
            this.buttonX25.Location = new System.Drawing.Point(6, 232);
            this.buttonX25.Name = "buttonX25";
            this.buttonX25.Size = new System.Drawing.Size(25, 20);
            this.buttonX25.TabIndex = 2;
            this.buttonX25.Text = "<<";
            this.buttonX25.Click += new System.EventHandler(this.buttonX25_Click);
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(46, 232);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(120, 20);
            this.labelX2.TabIndex = 1;
            this.labelX2.Text = "无模型";
            this.labelX2.TextAlignment = System.Drawing.StringAlignment.Center;
            // 
            // previewCanvas2
            // 
            this.previewCanvas2.Location = new System.Drawing.Point(6, 1);
            this.previewCanvas2.Name = "previewCanvas2";
            this.previewCanvas2.Size = new System.Drawing.Size(200, 218);
            this.previewCanvas2.TabIndex = 0;
            // 
            // dockContainerItem2
            // 
            this.dockContainerItem2.Control = this.panelDockContainer2;
            this.dockContainerItem2.Name = "dockContainerItem2";
            this.dockContainerItem2.Text = "别名面板";
            // 
            // dockContainerItem1
            // 
            this.dockContainerItem1.Control = this.panelDockContainer1;
            this.dockContainerItem1.Name = "dockContainerItem1";
            this.dockContainerItem1.Text = "路径点";
            // 
            // dockContainerItem4
            // 
            this.dockContainerItem4.Control = this.panelDockContainer4;
            this.dockContainerItem4.Name = "dockContainerItem4";
            this.dockContainerItem4.Text = "交通点";
            // 
            // dockContainerItem6
            // 
            this.dockContainerItem6.Control = this.panelDockContainer6;
            this.dockContainerItem6.Name = "dockContainerItem6";
            this.dockContainerItem6.Text = "Npc面板";
            // 
            // dockContainerItem7
            // 
            this.dockContainerItem7.Control = this.panelDockContainer5;
            this.dockContainerItem7.Name = "dockContainerItem7";
            this.dockContainerItem7.Text = "Doodad面板";
            // 
            // dockContainerItem3
            // 
            this.dockContainerItem3.Control = this.panelDockContainer3;
            this.dockContainerItem3.Name = "dockContainerItem3";
            this.dockContainerItem3.Text = "多边形编辑面板";
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(792, 0);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 846);
            this.dockSite2.TabIndex = 5;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 846);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(792, 0);
            this.dockSite8.TabIndex = 12;
            this.dockSite8.TabStop = false;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 0);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 846);
            this.dockSite5.TabIndex = 9;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(792, 0);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 846);
            this.dockSite6.TabIndex = 10;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(792, 0);
            this.dockSite7.TabIndex = 11;
            this.dockSite7.TabStop = false;
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(245, 0);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(547, 0);
            this.dockSite3.TabIndex = 6;
            this.dockSite3.TabStop = false;
            // 
            // dockContainerItem9
            // 
            this.dockContainerItem9.Name = "dockContainerItem9";
            this.dockContainerItem9.Text = "dockContainerItem9";
            // 
            // dockContainerItem5
            // 
            this.dockContainerItem5.Name = "dockContainerItem5";
            this.dockContainerItem5.Text = "测试预览";
            // 
            // tmrRender
            // 
            this.tmrRender.Interval = 30;
            this.tmrRender.Tick += new System.EventHandler(this.tmrRender_Tick);
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
            this.buttonDelete});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonAddSameLevel
            // 
            this.buttonAddSameLevel.ImagePaddingHorizontal = 8;
            this.buttonAddSameLevel.Name = "buttonAddSameLevel";
            this.buttonAddSameLevel.Text = "添加本层";
            // 
            // buttonAddSub
            // 
            this.buttonAddSub.ImagePaddingHorizontal = 8;
            this.buttonAddSub.Name = "buttonAddSub";
            this.buttonAddSub.Text = "添加子层";
            // 
            // buttonRename
            // 
            this.buttonRename.BeginGroup = true;
            this.buttonRename.ImagePaddingHorizontal = 8;
            this.buttonRename.Name = "buttonRename";
            this.buttonRename.Text = "重命名";
            // 
            // buttonCopy
            // 
            this.buttonCopy.ImagePaddingHorizontal = 8;
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Text = "复制";
            // 
            // buttonPaste
            // 
            this.buttonPaste.ImagePaddingHorizontal = 8;
            this.buttonPaste.Name = "buttonPaste";
            this.buttonPaste.Text = "粘贴";
            // 
            // buttonDelete
            // 
            this.buttonDelete.BeginGroup = true;
            this.buttonDelete.ImagePaddingHorizontal = 8;
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Text = "删除";
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
            this.bClearContent});
            this.cmScriptOp.Text = "cmScriptOp";
            // 
            // bShowLuaCode
            // 
            this.bShowLuaCode.ImagePaddingHorizontal = 8;
            this.bShowLuaCode.Name = "bShowLuaCode";
            this.bShowLuaCode.Text = "显示代码(&S)";
            // 
            // bShowMetainfo
            // 
            this.bShowMetainfo.BeginGroup = true;
            this.bShowMetainfo.ImagePaddingHorizontal = 8;
            this.bShowMetainfo.Name = "bShowMetainfo";
            this.bShowMetainfo.Text = "编辑元信息(&X)";
            // 
            // bEditFile
            // 
            this.bEditFile.ImagePaddingHorizontal = 8;
            this.bEditFile.Name = "bEditFile";
            this.bEditFile.Text = "编辑文件(&E)";
            // 
            // bDeleteRecord
            // 
            this.bDeleteRecord.BeginGroup = true;
            this.bDeleteRecord.ImagePaddingHorizontal = 8;
            this.bDeleteRecord.Name = "bDeleteRecord";
            this.bDeleteRecord.Text = "删除从表记录(&D)";
            // 
            // bClearContent
            // 
            this.bClearContent.ImagePaddingHorizontal = 8;
            this.bClearContent.Name = "bClearContent";
            this.bClearContent.Text = "清空";
            // 
            // cmRightClick
            // 
            this.cmRightClick.AutoExpandOnClick = true;
            this.cmRightClick.ImagePaddingHorizontal = 8;
            this.cmRightClick.Name = "cmRightClick";
            this.cmRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonJmpToTree,
            this.buttonDelSelObj,
            this.buttonProp});
            this.cmRightClick.Text = "cmRightClick";
            // 
            // buttonJmpToTree
            // 
            this.buttonJmpToTree.ImagePaddingHorizontal = 8;
            this.buttonJmpToTree.Name = "buttonJmpToTree";
            this.buttonJmpToTree.Text = "转到逻辑树(&T)";
            this.buttonJmpToTree.Click += new System.EventHandler(this.buttonJmpToTree_Click);
            // 
            // buttonDelSelObj
            // 
            this.buttonDelSelObj.ImagePaddingHorizontal = 8;
            this.buttonDelSelObj.Name = "buttonDelSelObj";
            this.buttonDelSelObj.Text = "删除(&D)";
            this.buttonDelSelObj.Click += new System.EventHandler(this.buttonDelSelObj_Click);
            // 
            // buttonProp
            // 
            this.buttonProp.ImagePaddingHorizontal = 8;
            this.buttonProp.Name = "buttonProp";
            this.buttonProp.Text = "属性(&R)";
            this.buttonProp.Click += new System.EventHandler(this.buttonProp_Click);
            // 
            // dockContainerItem8
            // 
            this.dockContainerItem8.Name = "dockContainerItem8";
            this.dockContainerItem8.Text = "dockContainerItem8";
            // 
            // PanelView
            // 
            this.PanelView.Controls.Add(this.contextMenuBar1);
            this.PanelView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PanelView.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.PanelView.Location = new System.Drawing.Point(245, 0);
            this.PanelView.Name = "PanelView";
            this.PanelView.Size = new System.Drawing.Size(547, 846);
            this.PanelView.TabIndex = 13;
            this.PanelView.TabStop = true;
            this.PanelView.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PanelView_MouseClick);
            this.PanelView.MouseDown += new System.Windows.Forms.MouseEventHandler(this.PanelView_MouseDown);
            this.PanelView.Resize += new System.EventHandler(this.PanelView_Resize);
            this.PanelView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.PanelView_MouseUp);
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmRightClick});
            this.contextMenuBar1.Location = new System.Drawing.Point(175, 526);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(240, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 0;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // BaseForm
            // 
            this.ClientSize = new System.Drawing.Size(792, 846);
            this.Controls.Add(this.PanelView);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.Name = "BaseForm";
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.BaseForm_KeyUp);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.BaseForm_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.BaseForm_KeyDown);
            this.dockSite1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.SideCtrlBar)).EndInit();
            this.SideCtrlBar.ResumeLayout(false);
            this.panelDockContainer2.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            this.splitContainer1.ResumeLayout(false);
            this.panelDockContainer1.ResumeLayout(false);
            this.panelEx2.ResumeLayout(false);
            this.wayPointSetParameterPanel.ResumeLayout(false);
            this.wayPointParameterPanel.ResumeLayout(false);
            this.panelEx1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.patrolTree)).EndInit();
            this.panelDockContainer4.ResumeLayout(false);
            this.panelEx4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewX2)).EndInit();
            this.panelEx3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trafficTree)).EndInit();
            this.panelDockContainer6.ResumeLayout(false);
            this.panelEx5.ResumeLayout(false);
            this.groupPanel3.ResumeLayout(false);
            this.groupPanel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.npcTree)).EndInit();
            this.groupPanel1.ResumeLayout(false);
            this.panelDockContainer3.ResumeLayout(false);
            this.groupBoxBrushParams.ResumeLayout(false);
            this.groupBoxBrushParams.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupboxPoly.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.advTreePoly)).EndInit();
            this.panelDockContainer5.ResumeLayout(false);
            this.panelEx6.ResumeLayout(false);
            this.groupPanel7.ResumeLayout(false);
            this.groupPanel6.ResumeLayout(false);
            this.groupPanel5.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.doodadTree)).EndInit();
            this.groupPanel4.ResumeLayout(false);
            this.PanelView.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        public DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
        private DevComponents.DotNetBar.DockSite dockSite4;
        private DevComponents.DotNetBar.DockSite dockSite1;
        private DevComponents.DotNetBar.DockSite dockSite2;
        private DevComponents.DotNetBar.DockSite dockSite3;
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.DockSite dockSite5;
        private DevComponents.DotNetBar.DockSite dockSite6;
        private DevComponents.DotNetBar.DockSite dockSite7;
        private DevComponents.DotNetBar.Bar SideCtrlBar;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer2;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem2;
        private System.Windows.Forms.Timer tmrRender;
        //private DevComponents.DotNetBar.TabControlPanel BasePanel;
        private DevComponents.DotNetBar.ButtonItem bTabContext;
        private DevComponents.DotNetBar.ButtonItem cmdContextSave;
        private DevComponents.DotNetBar.ButtonItem cmdContextClose;
        private DevComponents.DotNetBar.ButtonItem bTabColor;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonAddSameLevel;
        private DevComponents.DotNetBar.ButtonItem buttonAddSub;
        private DevComponents.DotNetBar.ButtonItem buttonRename;
        private DevComponents.DotNetBar.ButtonItem buttonCopy;
        private DevComponents.DotNetBar.ButtonItem buttonPaste;
        private DevComponents.DotNetBar.ButtonItem buttonDelete;
        private DevComponents.DotNetBar.ButtonItem cmScriptOp;
        private DevComponents.DotNetBar.ButtonItem bShowLuaCode;
        private DevComponents.DotNetBar.ButtonItem bShowMetainfo;
        private DevComponents.DotNetBar.ButtonItem bEditFile;
        private DevComponents.DotNetBar.ButtonItem bDeleteRecord;
        private DevComponents.DotNetBar.ButtonItem bClearContent;
        // private MyPanel PanelView;
        private MWControlSuite.MWTreeView treeView1;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer1;
        private DevComponents.DotNetBar.PanelEx panelEx2;
        private DevComponents.DotNetBar.ExpandableSplitter expandableSplitter1;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem1;
        private DevComponents.DotNetBar.ButtonX buttonX4;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.AdvTree.AdvTree patrolTree;
        private DevComponents.AdvTree.Node node1;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer4;
        private DevComponents.DotNetBar.PanelEx panelEx4;
        private DevComponents.DotNetBar.ExpandableSplitter expandableSplitter2;
        private DevComponents.DotNetBar.PanelEx panelEx3;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem4;
        private DevComponents.DotNetBar.Controls.DataGridViewX dataGridViewX2;
        private DevComponents.AdvTree.AdvTree trafficTree;
        private DevComponents.AdvTree.Node node2;
        private DevComponents.AdvTree.NodeConnector nodeConnector2;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.DotNetBar.ButtonX buttonX9;
        private DevComponents.DotNetBar.ButtonX buttonX8;
        private DevComponents.DotNetBar.ButtonX buttonX7;
        private DevComponents.DotNetBar.ButtonX buttonX6;
        private DevComponents.DotNetBar.ButtonX buttonX5;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem5;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer6;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem6;
        private DevComponents.DotNetBar.PanelEx panelEx5;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel2;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.ButtonX buttonX11;
        private DevComponents.DotNetBar.ButtonX buttonX10;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.AdvTree.AdvTree npcTree;
        private DevComponents.AdvTree.Node node3;
        private DevComponents.AdvTree.NodeConnector nodeConnector3;
        private DevComponents.DotNetBar.ElementStyle elementStyle3;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel3;
        private PreviewCanvas previewCanvas1;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.ButtonX buttonX20;
        private DevComponents.DotNetBar.ButtonX buttonX19;
        private DevComponents.DotNetBar.ButtonX buttonX21;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer5;
        private DevComponents.DotNetBar.PanelEx panelEx6;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel4;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem7;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel5;
        private DevComponents.AdvTree.AdvTree doodadTree;
        private DevComponents.AdvTree.Node node4;
        private DevComponents.AdvTree.NodeConnector nodeConnector4;
        private DevComponents.DotNetBar.ElementStyle elementStyle4;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel6;
        private DevComponents.DotNetBar.ButtonX buttonX24;
        private DevComponents.DotNetBar.ButtonX buttonX23;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX2;
        private PreviewCanvas previewCanvas2;
        private DevComponents.DotNetBar.ButtonX buttonX26;
        private DevComponents.DotNetBar.ButtonX buttonX25;
        private DevComponents.DotNetBar.LabelX labelX2;
        private MyPanel PanelView;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer3;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem3;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxShowPoly;
        private DevComponents.DotNetBar.ButtonX buttonX14;
        private DevComponents.DotNetBar.ButtonX buttonX15;
        private DevComponents.AdvTree.AdvTree advTreePoly;
        private DevComponents.AdvTree.Node node5;
        private DevComponents.AdvTree.NodeConnector nodeConnector5;
        private DevComponents.DotNetBar.ElementStyle elementStyle5;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private DevComponents.DotNetBar.ButtonX buttonOK;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem cmRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonProp;
        private DevComponents.DotNetBar.ButtonItem buttonJmpToTree;
        private DevComponents.DotNetBar.ButtonItem buttonDelSelObj;
        private System.Windows.Forms.GroupBox groupboxPoly;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxBlock;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.ButtonX buttonClearScript;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxRideHorse;
        private DevComponents.DotNetBar.ButtonX buttonBrowseScript;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxRest;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxPut;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxHeight;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxStall;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxIndoor;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxScript;
        private DevComponents.DotNetBar.ButtonX buttonX12;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.Controls.TextBoxX textLogicName;
        private DevComponents.DotNetBar.ButtonX buttonX13;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkShowSelectedCell;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkShowItemCell;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkShowTerrainCell;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkShowLogicCell;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private DevComponents.DotNetBar.LabelX labelX6;
        private DevComponents.DotNetBar.PanelEx colorPicker;
        private System.Windows.Forms.GroupBox groupBoxBrushParams;
        private DevComponents.DotNetBar.Controls.Slider sliderBrushHeight;
        private DevComponents.DotNetBar.Controls.Slider sliderDisplayWidth;
        private System.Windows.Forms.RadioButton radioEcllipse;
        private System.Windows.Forms.RadioButton radioRect;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private DevComponents.DotNetBar.Controls.Slider sliderBrushRadius;
        private DevComponents.DotNetBar.ButtonX buttonX16;
        private DevComponents.DotNetBar.LabelX labelX9;
        private DevComponents.DotNetBar.LabelX labelX8;
        private DevComponents.DotNetBar.LabelX labelX7;
        private DevComponents.DotNetBar.Controls.GroupPanel wayPointSetParameterPanel;
        private DevComponents.DotNetBar.Controls.GroupPanel wayPointParameterPanel;
        private DevComponents.DotNetBar.LabelX labelX11;
        private DevComponents.DotNetBar.LabelX labelX10;
        private DevComponents.DotNetBar.ButtonX bAutoAttackPathNpc;
        private DevComponents.DotNetBar.Controls.ComboBoxEx patrolPathOrderBox;
        private DevComponents.DotNetBar.Controls.TextBoxX patrolPathWalkSpeedBox;
        private DevComponents.DotNetBar.Controls.TextBoxX patrolPathNameBox;
        private DevComponents.DotNetBar.Controls.TextBoxX wayPointStayDirectionBox;
        private DevComponents.DotNetBar.Controls.TextBoxX wayPointStayFrameBox;
        private DevComponents.DotNetBar.LabelX labelX12;
        private DevComponents.DotNetBar.Controls.TextBoxX wayPointStayAnimationBox;
        private DevComponents.DotNetBar.LabelX labelX13;
        private DevComponents.DotNetBar.Controls.CheckBoxX cWayPointRun;
        private DevComponents.DotNetBar.Controls.CheckBoxX cWayPointAnimationLoop;
        private DevComponents.DotNetBar.Controls.TextBoxX wayPointScriptBox;
        private DevComponents.DotNetBar.ButtonX buttonX18;
        private DevComponents.DotNetBar.ButtonX buttonX17;
        private DevComponents.DotNetBar.ButtonX buttonBuildPoly;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.CheckBox checkShowAll;
        private DevComponents.DotNetBar.ButtonX bReplaceNpcTemplate;
        private DevComponents.DotNetBar.ButtonX bReplaceAllNpcTemplate;
        private DevComponents.DotNetBar.ButtonX bReplaceAllDoodadTemplate;
        private DevComponents.DotNetBar.ButtonX bReplaceDoodadTemplate;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel7;
        private DevComponents.DotNetBar.ButtonX buttonX22;
        private DevComponents.DotNetBar.ButtonX buttonX27;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem8;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem9;
    }
}
