namespace GameLuaEditor
{
    partial class MainForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.barFunction = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer2 = new DevComponents.DotNetBar.PanelDockContainer();
            this.windowControl = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.outputBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.outPutBar = new DevComponents.DotNetBar.Bar();
            this.bEnableServerDebugOutput = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteOutput = new DevComponents.DotNetBar.ButtonItem();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel3 = new DevComponents.DotNetBar.TabControlPanel();
            this.watchTree = new DevComponents.AdvTree.AdvTree();
            this.varNameColumn = new DevComponents.AdvTree.ColumnHeader();
            this.varValueColumn = new DevComponents.AdvTree.ColumnHeader();
            this.varFileNameColumn = new DevComponents.AdvTree.ColumnHeader();
            this.varStateColumn = new DevComponents.AdvTree.ColumnHeader();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.watchBar = new DevComponents.DotNetBar.Bar();
            this.bAddVar = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteVar = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteAllVar = new DevComponents.DotNetBar.ButtonItem();
            this.bRefreshVar = new DevComponents.DotNetBar.ButtonItem();
            this.tabItem4 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.breakPointView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.EnableBreakPoint = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.BreakPointCondition = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BreakPointFileName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BreakPointLineIndex = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BreakPointState = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.breakPointBar = new DevComponents.DotNetBar.Bar();
            this.bDeleteBreakPoint = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteAllBreakPoint = new DevComponents.DotNetBar.ButtonItem();
            this.tabItem3 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel4 = new DevComponents.DotNetBar.TabControlPanel();
            this.callStackView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.CallStackLevel = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CallStackFileName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CallStackLineIndex = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CallStackFunctionName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tabItem5 = new DevComponents.DotNetBar.TabItem(this.components);
            this.dockContainerItem1 = new DevComponents.DotNetBar.DockContainerItem();
            this.imageList2 = new System.Windows.Forms.ImageList(this.components);
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.bar2 = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer1 = new DevComponents.DotNetBar.PanelDockContainer();
            this.treePh = new System.Windows.Forms.TreeView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.脚本列表 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite8 = new DevComponents.DotNetBar.DockSite();
            this.bar4 = new DevComponents.DotNetBar.Bar();
            this.labelStatus = new DevComponents.DotNetBar.LabelItem();
            this.LDBName = new DevComponents.DotNetBar.LabelItem();
            this.LHostName = new DevComponents.DotNetBar.LabelItem();
            this.dockSite5 = new DevComponents.DotNetBar.DockSite();
            this.dockSite6 = new DevComponents.DotNetBar.DockSite();
            this.dockSite7 = new DevComponents.DotNetBar.DockSite();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.btnScript = new DevComponents.DotNetBar.ButtonItem();
            this.btnSave = new DevComponents.DotNetBar.ButtonItem();
            this.btnSaveAll = new DevComponents.DotNetBar.ButtonItem();
            this.btnClose = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.btnSetup = new DevComponents.DotNetBar.ButtonItem();
            this.bEdit = new DevComponents.DotNetBar.ButtonItem();
            this.btnHistory = new DevComponents.DotNetBar.ButtonItem();
            this.btnStringReplace = new DevComponents.DotNetBar.ButtonItem();
            this.btnCheck = new DevComponents.DotNetBar.ButtonItem();
            this.bClearLS = new DevComponents.DotNetBar.ButtonItem();
            this.btnFormat = new DevComponents.DotNetBar.ButtonItem();
            this.btnHistoryTotal = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem3 = new DevComponents.DotNetBar.ButtonItem();
            this.bViewGeneralToolBar = new DevComponents.DotNetBar.ButtonItem();
            this.bViewDebugToolBar = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem4 = new DevComponents.DotNetBar.ButtonItem();
            this.barGeneral = new DevComponents.DotNetBar.Bar();
            this.btnToolSave = new DevComponents.DotNetBar.ButtonItem();
            this.btnToolSaveAll = new DevComponents.DotNetBar.ButtonItem();
            this.btnToolSetup = new DevComponents.DotNetBar.ButtonItem();
            this.btnToolHistory = new DevComponents.DotNetBar.ButtonItem();
            this.btnToolReplace = new DevComponents.DotNetBar.ButtonItem();
            this.bunToolCheck = new DevComponents.DotNetBar.ButtonItem();
            this.btnToolFormat = new DevComponents.DotNetBar.ButtonItem();
            this.btnHistroyTotal = new DevComponents.DotNetBar.ButtonItem();
            this.barDebug = new DevComponents.DotNetBar.Bar();
            this.bSwitchBreakPoint = new DevComponents.DotNetBar.ButtonItem();
            this.bMouseGetWord = new DevComponents.DotNetBar.ButtonItem();
            this.bDetachDebugFile = new DevComponents.DotNetBar.ButtonItem();
            this.bStartDebug = new DevComponents.DotNetBar.ButtonItem();
            this.bStartDebugClient = new DevComponents.DotNetBar.ButtonItem();
            this.bStopDebug = new DevComponents.DotNetBar.ButtonItem();
            this.bStepOver = new DevComponents.DotNetBar.ButtonItem();
            this.bStepInto = new DevComponents.DotNetBar.ButtonItem();
            this.bContinueDebug = new DevComponents.DotNetBar.ButtonItem();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.tabStrip1 = new DevComponents.DotNetBar.TabStrip();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.popTreePh_Folder = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.cmdTreePhFolder_newFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFolder_newFile = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFolder_del = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFolder_rename = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cmdTreePhFolder_check = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFolder_export = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFolder_openlocal = new System.Windows.Forms.ToolStripMenuItem();
            this.bImportLocalData = new System.Windows.Forms.ToolStripMenuItem();
            this.bImportFile = new System.Windows.Forms.ToolStripMenuItem();
            this.bImportFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.popTreePh_File = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.cmdTreePhFile_reanme = new System.Windows.Forms.ToolStripMenuItem();
            this.cmdTreePhFile_del = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.cmdTreePhFile_export = new System.Windows.Forms.ToolStripMenuItem();
            this.popRight = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.btnReplace = new System.Windows.Forms.ToolStripMenuItem();
            this.popTreePh_LocalFileRoot = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.bLoadLocalFile = new System.Windows.Forms.ToolStripMenuItem();
            this.bLoadLocalFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.popTreePh_LocalFolder = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.bAddLocalFile = new System.Windows.Forms.ToolStripMenuItem();
            this.bAddAllLocalFile = new System.Windows.Forms.ToolStripMenuItem();
            this.bRemoveLocalFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.popTreePh_LocalFile = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.bRemoveLocalFile = new System.Windows.Forms.ToolStripMenuItem();
            this.bNewFileInFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.dockSite4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.barFunction)).BeginInit();
            this.barFunction.SuspendLayout();
            this.panelDockContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.windowControl)).BeginInit();
            this.windowControl.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.outPutBar)).BeginInit();
            this.tabControlPanel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.watchTree)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.watchBar)).BeginInit();
            this.tabControlPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.breakPointView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.breakPointBar)).BeginInit();
            this.tabControlPanel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.callStackView)).BeginInit();
            this.dockSite1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar2)).BeginInit();
            this.bar2.SuspendLayout();
            this.panelDockContainer1.SuspendLayout();
            this.dockSite8.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar4)).BeginInit();
            this.dockSite7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barGeneral)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barDebug)).BeginInit();
            this.popTreePh_Folder.SuspendLayout();
            this.popTreePh_File.SuspendLayout();
            this.popRight.SuspendLayout();
            this.popTreePh_LocalFileRoot.SuspendLayout();
            this.popTreePh_LocalFolder.SuspendLayout();
            this.popTreePh_LocalFile.SuspendLayout();
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
            this.dotNetBarManager1.Images = this.imageList2;
            this.dotNetBarManager1.ImagesMedium = this.imageList2;
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
            this.dockSite4.Controls.Add(this.barFunction);
            this.dockSite4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite4.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.barFunction, 886, 249)))}, DevComponents.DotNetBar.eOrientation.Vertical);
            this.dockSite4.Location = new System.Drawing.Point(0, 168);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(886, 252);
            this.dockSite4.TabIndex = 4;
            this.dockSite4.TabStop = false;
            // 
            // barFunction
            // 
            this.barFunction.AccessibleDescription = "DotNetBar Bar (barFunction)";
            this.barFunction.AccessibleName = "DotNetBar Bar";
            this.barFunction.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barFunction.AutoSyncBarCaption = true;
            this.barFunction.CloseSingleTab = true;
            this.barFunction.Controls.Add(this.panelDockContainer2);
            this.barFunction.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.barFunction.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockContainerItem1});
            this.barFunction.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.barFunction.Location = new System.Drawing.Point(0, 3);
            this.barFunction.Name = "barFunction";
            this.barFunction.Size = new System.Drawing.Size(886, 249);
            this.barFunction.Stretch = true;
            this.barFunction.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barFunction.TabIndex = 0;
            this.barFunction.TabStop = false;
            this.barFunction.Text = "功能面板";
            // 
            // panelDockContainer2
            // 
            this.panelDockContainer2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer2.Controls.Add(this.windowControl);
            this.panelDockContainer2.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer2.Name = "panelDockContainer2";
            this.panelDockContainer2.Size = new System.Drawing.Size(880, 223);
            this.panelDockContainer2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer2.Style.GradientAngle = 90;
            this.panelDockContainer2.TabIndex = 0;
            // 
            // windowControl
            // 
            this.windowControl.CanReorderTabs = true;
            this.windowControl.Controls.Add(this.tabControlPanel1);
            this.windowControl.Controls.Add(this.tabControlPanel3);
            this.windowControl.Controls.Add(this.tabControlPanel2);
            this.windowControl.Controls.Add(this.tabControlPanel4);
            this.windowControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.windowControl.Location = new System.Drawing.Point(0, 0);
            this.windowControl.Name = "windowControl";
            this.windowControl.SelectedTabFont = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.windowControl.SelectedTabIndex = 0;
            this.windowControl.Size = new System.Drawing.Size(880, 223);
            this.windowControl.TabIndex = 0;
            this.windowControl.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.windowControl.Tabs.Add(this.tabItem2);
            this.windowControl.Tabs.Add(this.tabItem3);
            this.windowControl.Tabs.Add(this.tabItem4);
            this.windowControl.Tabs.Add(this.tabItem5);
            this.windowControl.Text = "tabControl1";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.outputBox);
            this.tabControlPanel1.Controls.Add(this.outPutBar);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(880, 197);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 1;
            this.tabControlPanel1.TabItem = this.tabItem2;
            // 
            // outputBox
            // 
            // 
            // 
            // 
            this.outputBox.Border.Class = "TextBoxBorder";
            this.outputBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.outputBox.Location = new System.Drawing.Point(1, 26);
            this.outputBox.Multiline = true;
            this.outputBox.Name = "outputBox";
            this.outputBox.Size = new System.Drawing.Size(878, 170);
            this.outputBox.TabIndex = 2;
            // 
            // outPutBar
            // 
            this.outPutBar.Dock = System.Windows.Forms.DockStyle.Top;
            this.outPutBar.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.outPutBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bEnableServerDebugOutput,
            this.bDeleteOutput});
            this.outPutBar.Location = new System.Drawing.Point(1, 1);
            this.outPutBar.Name = "outPutBar";
            this.outPutBar.Size = new System.Drawing.Size(878, 25);
            this.outPutBar.Stretch = true;
            this.outPutBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.outPutBar.TabIndex = 1;
            this.outPutBar.TabStop = false;
            this.outPutBar.Text = "bar5";
            // 
            // bEnableServerDebugOutput
            // 
            this.bEnableServerDebugOutput.ImagePaddingHorizontal = 8;
            this.bEnableServerDebugOutput.Name = "bEnableServerDebugOutput";
            this.bEnableServerDebugOutput.Text = "开启服务端调试输出";
            this.bEnableServerDebugOutput.Click += new System.EventHandler(this.bEnableServerOutput_Click);
            // 
            // bDeleteOutput
            // 
            this.bDeleteOutput.ImagePaddingHorizontal = 8;
            this.bDeleteOutput.Name = "bDeleteOutput";
            this.bDeleteOutput.Text = "清空";
            this.bDeleteOutput.Click += new System.EventHandler(this.bDeleteOutput_Click);
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel1;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "输出";
            // 
            // tabControlPanel3
            // 
            this.tabControlPanel3.Controls.Add(this.watchTree);
            this.tabControlPanel3.Controls.Add(this.watchBar);
            this.tabControlPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel3.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel3.Name = "tabControlPanel3";
            this.tabControlPanel3.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel3.Size = new System.Drawing.Size(880, 197);
            this.tabControlPanel3.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel3.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel3.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel3.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel3.Style.GradientAngle = 90;
            this.tabControlPanel3.TabIndex = 3;
            this.tabControlPanel3.TabItem = this.tabItem4;
            // 
            // watchTree
            // 
            this.watchTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.watchTree.AllowDrop = true;
            this.watchTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.watchTree.BackgroundStyle.Class = "TreeBorderKey";
            this.watchTree.CellEdit = true;
            this.watchTree.Columns.Add(this.varNameColumn);
            this.watchTree.Columns.Add(this.varValueColumn);
            this.watchTree.Columns.Add(this.varFileNameColumn);
            this.watchTree.Columns.Add(this.varStateColumn);
            this.watchTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.watchTree.DragDropEnabled = false;
            this.watchTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.watchTree.Location = new System.Drawing.Point(1, 26);
            this.watchTree.Name = "watchTree";
            this.watchTree.NodesConnector = this.nodeConnector1;
            this.watchTree.NodeStyle = this.elementStyle1;
            this.watchTree.PathSeparator = ";";
            this.watchTree.Size = new System.Drawing.Size(878, 170);
            this.watchTree.Styles.Add(this.elementStyle1);
            this.watchTree.SuspendPaint = false;
            this.watchTree.TabIndex = 1;
            this.watchTree.DragDrop += new System.Windows.Forms.DragEventHandler(this.watchTree_DragDrop);
            this.watchTree.AfterCellEdit += new DevComponents.AdvTree.CellEditEventHandler(this.watchTree_AfterCellEdit);
            this.watchTree.DragEnter += new System.Windows.Forms.DragEventHandler(this.watchTree_DragEnter);
            // 
            // varNameColumn
            // 
            this.varNameColumn.Name = "varNameColumn";
            this.varNameColumn.Text = "变量名";
            this.varNameColumn.Width.Relative = 20;
            // 
            // varValueColumn
            // 
            this.varValueColumn.Name = "varValueColumn";
            this.varValueColumn.Text = "变量值";
            this.varValueColumn.Width.Relative = 40;
            // 
            // varFileNameColumn
            // 
            this.varFileNameColumn.Name = "varFileNameColumn";
            this.varFileNameColumn.Text = "文件名";
            this.varFileNameColumn.Width.Relative = 20;
            // 
            // varStateColumn
            // 
            this.varStateColumn.Name = "varStateColumn";
            this.varStateColumn.Text = "状态";
            this.varStateColumn.Width.Relative = 20;
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
            // watchBar
            // 
            this.watchBar.Dock = System.Windows.Forms.DockStyle.Top;
            this.watchBar.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.watchBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bAddVar,
            this.bDeleteVar,
            this.bDeleteAllVar,
            this.bRefreshVar});
            this.watchBar.Location = new System.Drawing.Point(1, 1);
            this.watchBar.Name = "watchBar";
            this.watchBar.Size = new System.Drawing.Size(878, 25);
            this.watchBar.Stretch = true;
            this.watchBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.watchBar.TabIndex = 0;
            this.watchBar.TabStop = false;
            this.watchBar.Text = "bar5";
            // 
            // bAddVar
            // 
            this.bAddVar.ImagePaddingHorizontal = 8;
            this.bAddVar.Name = "bAddVar";
            this.bAddVar.Text = "添加变量";
            this.bAddVar.Click += new System.EventHandler(this.bAddVar_Click);
            // 
            // bDeleteVar
            // 
            this.bDeleteVar.ImagePaddingHorizontal = 8;
            this.bDeleteVar.Name = "bDeleteVar";
            this.bDeleteVar.Text = "删除变量";
            this.bDeleteVar.Click += new System.EventHandler(this.bDeleteVar_Click);
            // 
            // bDeleteAllVar
            // 
            this.bDeleteAllVar.ImagePaddingHorizontal = 8;
            this.bDeleteAllVar.Name = "bDeleteAllVar";
            this.bDeleteAllVar.Text = "删除所有变量";
            this.bDeleteAllVar.Click += new System.EventHandler(this.bDeleteAllVar_Click);
            // 
            // bRefreshVar
            // 
            this.bRefreshVar.Enabled = false;
            this.bRefreshVar.ImagePaddingHorizontal = 8;
            this.bRefreshVar.Name = "bRefreshVar";
            this.bRefreshVar.Text = "刷新";
            this.bRefreshVar.Click += new System.EventHandler(this.bRefreshVar_Click);
            // 
            // tabItem4
            // 
            this.tabItem4.AttachedControl = this.tabControlPanel3;
            this.tabItem4.Name = "tabItem4";
            this.tabItem4.Text = "观察";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.breakPointView);
            this.tabControlPanel2.Controls.Add(this.breakPointBar);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(880, 197);
            this.tabControlPanel2.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel2.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel2.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel2.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel2.Style.GradientAngle = 90;
            this.tabControlPanel2.TabIndex = 2;
            this.tabControlPanel2.TabItem = this.tabItem3;
            // 
            // breakPointView
            // 
            this.breakPointView.AllowUserToAddRows = false;
            this.breakPointView.AllowUserToDeleteRows = false;
            this.breakPointView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.breakPointView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.breakPointView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.EnableBreakPoint,
            this.BreakPointCondition,
            this.BreakPointFileName,
            this.BreakPointLineIndex,
            this.BreakPointState});
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.breakPointView.DefaultCellStyle = dataGridViewCellStyle3;
            this.breakPointView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.breakPointView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.breakPointView.Location = new System.Drawing.Point(1, 26);
            this.breakPointView.MultiSelect = false;
            this.breakPointView.Name = "breakPointView";
            this.breakPointView.RowTemplate.Height = 23;
            this.breakPointView.Size = new System.Drawing.Size(878, 170);
            this.breakPointView.TabIndex = 2;
            // 
            // EnableBreakPoint
            // 
            this.EnableBreakPoint.HeaderText = "开启";
            this.EnableBreakPoint.Name = "EnableBreakPoint";
            this.EnableBreakPoint.Width = 35;
            // 
            // BreakPointCondition
            // 
            this.BreakPointCondition.HeaderText = "条件";
            this.BreakPointCondition.Name = "BreakPointCondition";
            this.BreakPointCondition.Width = 54;
            // 
            // BreakPointFileName
            // 
            this.BreakPointFileName.HeaderText = "文件名";
            this.BreakPointFileName.Name = "BreakPointFileName";
            this.BreakPointFileName.ReadOnly = true;
            this.BreakPointFileName.Resizable = System.Windows.Forms.DataGridViewTriState.True;
            this.BreakPointFileName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            this.BreakPointFileName.Width = 47;
            // 
            // BreakPointLineIndex
            // 
            this.BreakPointLineIndex.HeaderText = "行号";
            this.BreakPointLineIndex.Name = "BreakPointLineIndex";
            this.BreakPointLineIndex.ReadOnly = true;
            this.BreakPointLineIndex.Width = 54;
            // 
            // BreakPointState
            // 
            this.BreakPointState.HeaderText = "状态";
            this.BreakPointState.Name = "BreakPointState";
            this.BreakPointState.ReadOnly = true;
            this.BreakPointState.Width = 54;
            // 
            // breakPointBar
            // 
            this.breakPointBar.Dock = System.Windows.Forms.DockStyle.Top;
            this.breakPointBar.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.breakPointBar.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bDeleteBreakPoint,
            this.bDeleteAllBreakPoint});
            this.breakPointBar.Location = new System.Drawing.Point(1, 1);
            this.breakPointBar.Name = "breakPointBar";
            this.breakPointBar.Size = new System.Drawing.Size(878, 25);
            this.breakPointBar.Stretch = true;
            this.breakPointBar.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.breakPointBar.TabIndex = 1;
            this.breakPointBar.TabStop = false;
            this.breakPointBar.Text = "bar5";
            // 
            // bDeleteBreakPoint
            // 
            this.bDeleteBreakPoint.ImagePaddingHorizontal = 8;
            this.bDeleteBreakPoint.Name = "bDeleteBreakPoint";
            this.bDeleteBreakPoint.Text = "删除断点";
            this.bDeleteBreakPoint.Click += new System.EventHandler(this.bDeleteBreakPoint_Click);
            // 
            // bDeleteAllBreakPoint
            // 
            this.bDeleteAllBreakPoint.ImagePaddingHorizontal = 8;
            this.bDeleteAllBreakPoint.Name = "bDeleteAllBreakPoint";
            this.bDeleteAllBreakPoint.Text = "删除所有断点";
            this.bDeleteAllBreakPoint.Click += new System.EventHandler(this.bDeleteAllBreakPoint_Click);
            // 
            // tabItem3
            // 
            this.tabItem3.AttachedControl = this.tabControlPanel2;
            this.tabItem3.Name = "tabItem3";
            this.tabItem3.Text = "断点";
            // 
            // tabControlPanel4
            // 
            this.tabControlPanel4.Controls.Add(this.callStackView);
            this.tabControlPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel4.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel4.Name = "tabControlPanel4";
            this.tabControlPanel4.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel4.Size = new System.Drawing.Size(880, 197);
            this.tabControlPanel4.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel4.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel4.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel4.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel4.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel4.Style.GradientAngle = 90;
            this.tabControlPanel4.TabIndex = 4;
            this.tabControlPanel4.TabItem = this.tabItem5;
            // 
            // callStackView
            // 
            this.callStackView.AllowUserToAddRows = false;
            this.callStackView.AllowUserToDeleteRows = false;
            this.callStackView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.callStackView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.callStackView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.CallStackLevel,
            this.CallStackFileName,
            this.CallStackLineIndex,
            this.CallStackFunctionName});
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.callStackView.DefaultCellStyle = dataGridViewCellStyle4;
            this.callStackView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.callStackView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.callStackView.Location = new System.Drawing.Point(1, 1);
            this.callStackView.Name = "callStackView";
            this.callStackView.ReadOnly = true;
            this.callStackView.RowTemplate.Height = 23;
            this.callStackView.Size = new System.Drawing.Size(878, 195);
            this.callStackView.TabIndex = 0;
            this.callStackView.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.callStackView_CellMouseDoubleClick);
            // 
            // CallStackLevel
            // 
            this.CallStackLevel.HeaderText = "调用层级";
            this.CallStackLevel.Name = "CallStackLevel";
            this.CallStackLevel.ReadOnly = true;
            this.CallStackLevel.Width = 78;
            // 
            // CallStackFileName
            // 
            this.CallStackFileName.HeaderText = "文件名";
            this.CallStackFileName.Name = "CallStackFileName";
            this.CallStackFileName.ReadOnly = true;
            this.CallStackFileName.Width = 66;
            // 
            // CallStackLineIndex
            // 
            this.CallStackLineIndex.HeaderText = "行号";
            this.CallStackLineIndex.Name = "CallStackLineIndex";
            this.CallStackLineIndex.ReadOnly = true;
            this.CallStackLineIndex.Width = 54;
            // 
            // CallStackFunctionName
            // 
            this.CallStackFunctionName.HeaderText = "方法名";
            this.CallStackFunctionName.Name = "CallStackFunctionName";
            this.CallStackFunctionName.ReadOnly = true;
            this.CallStackFunctionName.Width = 66;
            // 
            // tabItem5
            // 
            this.tabItem5.AttachedControl = this.tabControlPanel4;
            this.tabItem5.Name = "tabItem5";
            this.tabItem5.Text = "调用栈";
            // 
            // dockContainerItem1
            // 
            this.dockContainerItem1.Control = this.panelDockContainer2;
            this.dockContainerItem1.Name = "dockContainerItem1";
            this.dockContainerItem1.Text = "功能面板";
            // 
            // imageList2
            // 
            this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
            this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList2.Images.SetKeyName(0, "8_10.png");
            this.imageList2.Images.SetKeyName(1, "9_10.png");
            this.imageList2.Images.SetKeyName(2, "15_7.png");
            this.imageList2.Images.SetKeyName(3, "8_17.png");
            this.imageList2.Images.SetKeyName(4, "3_19.png");
            this.imageList2.Images.SetKeyName(5, "13_28.png");
            this.imageList2.Images.SetKeyName(6, "15_27.png");
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Controls.Add(this.bar2);
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.bar2, 204, 87)))}, DevComponents.DotNetBar.eOrientation.Horizontal);
            this.dockSite1.Location = new System.Drawing.Point(0, 81);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(207, 87);
            this.dockSite1.TabIndex = 1;
            this.dockSite1.TabStop = false;
            // 
            // bar2
            // 
            this.bar2.AccessibleDescription = "DotNetBar Bar (bar2)";
            this.bar2.AccessibleName = "DotNetBar Bar";
            this.bar2.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.bar2.AutoSyncBarCaption = true;
            this.bar2.Controls.Add(this.panelDockContainer1);
            this.bar2.Dock = System.Windows.Forms.DockStyle.Left;
            this.bar2.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.bar2.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.脚本列表});
            this.bar2.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.bar2.Location = new System.Drawing.Point(0, 0);
            this.bar2.Name = "bar2";
            this.bar2.Size = new System.Drawing.Size(204, 87);
            this.bar2.Stretch = true;
            this.bar2.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar2.TabIndex = 0;
            this.bar2.TabStop = false;
            this.bar2.Text = "脚本列表";
            // 
            // panelDockContainer1
            // 
            this.panelDockContainer1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelDockContainer1.Controls.Add(this.treePh);
            this.panelDockContainer1.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer1.Name = "panelDockContainer1";
            this.panelDockContainer1.Size = new System.Drawing.Size(198, 61);
            this.panelDockContainer1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer1.Style.GradientAngle = 90;
            this.panelDockContainer1.TabIndex = 0;
            // 
            // treePh
            // 
            this.treePh.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treePh.HideSelection = false;
            this.treePh.ImageIndex = 0;
            this.treePh.ImageList = this.imageList1;
            this.treePh.LabelEdit = true;
            this.treePh.Location = new System.Drawing.Point(0, 0);
            this.treePh.Name = "treePh";
            this.treePh.SelectedImageIndex = 0;
            this.treePh.Size = new System.Drawing.Size(198, 61);
            this.treePh.TabIndex = 0;
            this.treePh.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treePh_NodeMouseDoubleClick);
            this.treePh.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.treePh_AfterLabelEdit);
            this.treePh.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treePh_NodeMouseClick);
            this.treePh.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treePh_KeyDown);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "folder");
            this.imageList1.Images.SetKeyName(1, "file");
            // 
            // 脚本列表
            // 
            this.脚本列表.Control = this.panelDockContainer1;
            this.脚本列表.Name = "脚本列表";
            this.脚本列表.Text = "脚本列表";
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(886, 81);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 87);
            this.dockSite2.TabIndex = 2;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Controls.Add(this.bar4);
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 420);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(886, 20);
            this.dockSite8.TabIndex = 8;
            this.dockSite8.TabStop = false;
            // 
            // bar4
            // 
            this.bar4.AccessibleDescription = "DotNetBar Bar (bar4)";
            this.bar4.AccessibleName = "DotNetBar Bar";
            this.bar4.AccessibleRole = System.Windows.Forms.AccessibleRole.StatusBar;
            this.bar4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.bar4.DockSide = DevComponents.DotNetBar.eDockSide.Bottom;
            this.bar4.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.ResizeHandle;
            this.bar4.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.labelStatus,
            this.LDBName,
            this.LHostName});
            this.bar4.ItemSpacing = 2;
            this.bar4.Location = new System.Drawing.Point(0, 1);
            this.bar4.Name = "bar4";
            this.bar4.Size = new System.Drawing.Size(886, 19);
            this.bar4.Stretch = true;
            this.bar4.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar4.TabIndex = 20;
            this.bar4.TabStop = false;
            this.bar4.Text = "barStatus";
            // 
            // labelStatus
            // 
            this.labelStatus.BorderType = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.PaddingLeft = 2;
            this.labelStatus.PaddingRight = 2;
            this.labelStatus.SingleLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.labelStatus.Stretch = true;
            // 
            // LDBName
            // 
            this.LDBName.BorderType = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.LDBName.Name = "LDBName";
            this.LDBName.PaddingLeft = 2;
            this.LDBName.PaddingRight = 2;
            this.LDBName.SingleLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.LDBName.TextAlignment = System.Drawing.StringAlignment.Center;
            this.LDBName.Width = 200;
            // 
            // LHostName
            // 
            this.LHostName.BorderType = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.LHostName.Name = "LHostName";
            this.LHostName.SingleLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.LHostName.TextAlignment = System.Drawing.StringAlignment.Center;
            this.LHostName.Width = 150;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 81);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 339);
            this.dockSite5.TabIndex = 5;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(886, 81);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 339);
            this.dockSite6.TabIndex = 6;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Controls.Add(this.bar1);
            this.dockSite7.Controls.Add(this.barGeneral);
            this.dockSite7.Controls.Add(this.barDebug);
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(886, 81);
            this.dockSite7.TabIndex = 7;
            this.dockSite7.TabStop = false;
            // 
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.bar1.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnScript,
            this.buttonItem2,
            this.bEdit,
            this.buttonItem3,
            this.buttonItem1});
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.MenuBar = true;
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(886, 24);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 0;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // btnScript
            // 
            this.btnScript.ImagePaddingHorizontal = 8;
            this.btnScript.Name = "btnScript";
            this.btnScript.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnSave,
            this.btnSaveAll,
            this.btnClose});
            this.btnScript.Text = "文件(&F)";
            // 
            // btnSave
            // 
            this.btnSave.ImagePaddingHorizontal = 8;
            this.btnSave.Name = "btnSave";
            this.btnSave.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.btnSave.Text = "保存(&S)";
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnSaveAll
            // 
            this.btnSaveAll.ImagePaddingHorizontal = 8;
            this.btnSaveAll.Name = "btnSaveAll";
            this.btnSaveAll.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlShiftS);
            this.btnSaveAll.Text = "全部保存(&A)";
            this.btnSaveAll.Click += new System.EventHandler(this.btnSaveAll_Click);
            // 
            // btnClose
            // 
            this.btnClose.ImagePaddingHorizontal = 8;
            this.btnClose.Name = "btnClose";
            this.btnClose.Text = "关闭(&X)";
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // buttonItem2
            // 
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnSetup});
            this.buttonItem2.Text = "设置(&S)";
            // 
            // btnSetup
            // 
            this.btnSetup.ImagePaddingHorizontal = 8;
            this.btnSetup.Name = "btnSetup";
            this.btnSetup.Text = "首选项(&P)";
            this.btnSetup.Click += new System.EventHandler(this.btnSetup_Click);
            // 
            // bEdit
            // 
            this.bEdit.ImagePaddingHorizontal = 8;
            this.bEdit.Name = "bEdit";
            this.bEdit.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnHistory,
            this.btnStringReplace,
            this.btnCheck,
            this.bClearLS,
            this.btnFormat,
            this.btnHistoryTotal});
            this.bEdit.Text = "编辑";
            // 
            // btnHistory
            // 
            this.btnHistory.ImagePaddingHorizontal = 8;
            this.btnHistory.Name = "btnHistory";
            this.btnHistory.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F4);
            this.btnHistory.Text = "查看历史记录";
            this.btnHistory.Click += new System.EventHandler(this.btnHistory_Click);
            // 
            // btnStringReplace
            // 
            this.btnStringReplace.ImagePaddingHorizontal = 8;
            this.btnStringReplace.Name = "btnStringReplace";
            this.btnStringReplace.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlH);
            this.btnStringReplace.Text = "字符串替换";
            this.btnStringReplace.Click += new System.EventHandler(this.btnStringReplace_Click);
            // 
            // btnCheck
            // 
            this.btnCheck.ImagePaddingHorizontal = 8;
            this.btnCheck.Name = "btnCheck";
            this.btnCheck.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F6);
            this.btnCheck.Text = "脚本检查(&C)";
            this.btnCheck.Click += new System.EventHandler(this.btnCheck_Click);
            // 
            // bClearLS
            // 
            this.bClearLS.ImagePaddingHorizontal = 8;
            this.bClearLS.Name = "bClearLS";
            this.bClearLS.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F7);
            this.bClearLS.Text = "LS表清理";
            this.bClearLS.Click += new System.EventHandler(this.bClearLS_Click);
            // 
            // btnFormat
            // 
            this.btnFormat.ImagePaddingHorizontal = 8;
            this.btnFormat.Name = "btnFormat";
            this.btnFormat.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F8);
            this.btnFormat.Text = "代码格式化(&F)";
            this.btnFormat.Click += new System.EventHandler(this.btnFormat_Click);
            // 
            // btnHistoryTotal
            // 
            this.btnHistoryTotal.ImagePaddingHorizontal = 8;
            this.btnHistoryTotal.Name = "btnHistoryTotal";
            this.btnHistoryTotal.Text = "查看历史操作总记录";
            this.btnHistoryTotal.Click += new System.EventHandler(this.buttonItem5_Click);
            // 
            // buttonItem3
            // 
            this.buttonItem3.ImagePaddingHorizontal = 8;
            this.buttonItem3.Name = "buttonItem3";
            this.buttonItem3.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bViewGeneralToolBar,
            this.bViewDebugToolBar});
            this.buttonItem3.Text = "工具(&T)";
            // 
            // bViewGeneralToolBar
            // 
            this.bViewGeneralToolBar.Checked = true;
            this.bViewGeneralToolBar.ImagePaddingHorizontal = 8;
            this.bViewGeneralToolBar.Name = "bViewGeneralToolBar";
            this.bViewGeneralToolBar.Text = "通用工具栏";
            this.bViewGeneralToolBar.Click += new System.EventHandler(this.bViewGeneralToolBar_Click);
            // 
            // bViewDebugToolBar
            // 
            this.bViewDebugToolBar.Checked = true;
            this.bViewDebugToolBar.ImagePaddingHorizontal = 8;
            this.bViewDebugToolBar.Name = "bViewDebugToolBar";
            this.bViewDebugToolBar.Text = "调试工具栏";
            this.bViewDebugToolBar.Click += new System.EventHandler(this.bViewDebugToolBar_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem4});
            this.buttonItem1.Text = "帮助(&H)";
            // 
            // buttonItem4
            // 
            this.buttonItem4.ImagePaddingHorizontal = 8;
            this.buttonItem4.Name = "buttonItem4";
            this.buttonItem4.Text = "版本历史(&S)";
            this.buttonItem4.Click += new System.EventHandler(this.buttonItem4_Click);
            // 
            // barGeneral
            // 
            this.barGeneral.AccessibleDescription = "DotNetBar Bar (barGeneral)";
            this.barGeneral.AccessibleName = "DotNetBar Bar";
            this.barGeneral.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.barGeneral.DisplayMoreItemsOnMenu = true;
            this.barGeneral.DockedBorderStyle = DevComponents.DotNetBar.eBorderType.Sunken;
            this.barGeneral.DockLine = 1;
            this.barGeneral.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barGeneral.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barGeneral.ImageSize = DevComponents.DotNetBar.eBarImageSize.Medium;
            this.barGeneral.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnToolSave,
            this.btnToolSaveAll,
            this.btnToolSetup,
            this.btnToolHistory,
            this.btnToolReplace,
            this.bunToolCheck,
            this.btnToolFormat,
            this.btnHistroyTotal});
            this.barGeneral.Location = new System.Drawing.Point(0, 25);
            this.barGeneral.MenuBar = true;
            this.barGeneral.Name = "barGeneral";
            this.barGeneral.Size = new System.Drawing.Size(261, 30);
            this.barGeneral.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barGeneral.TabIndex = 1;
            this.barGeneral.TabStop = false;
            this.barGeneral.Text = "bar3";
            // 
            // btnToolSave
            // 
            this.btnToolSave.ImageIndex = 0;
            this.btnToolSave.ImagePaddingHorizontal = 8;
            this.btnToolSave.Name = "btnToolSave";
            this.btnToolSave.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.btnToolSave.Text = "buttonItem4";
            this.btnToolSave.Tooltip = "保存";
            this.btnToolSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnToolSaveAll
            // 
            this.btnToolSaveAll.ImageIndex = 1;
            this.btnToolSaveAll.ImagePaddingHorizontal = 8;
            this.btnToolSaveAll.Name = "btnToolSaveAll";
            this.btnToolSaveAll.Text = "buttonItem5";
            this.btnToolSaveAll.Tooltip = "全部保存";
            this.btnToolSaveAll.Click += new System.EventHandler(this.btnSaveAll_Click);
            // 
            // btnToolSetup
            // 
            this.btnToolSetup.ImageIndex = 2;
            this.btnToolSetup.ImagePaddingHorizontal = 8;
            this.btnToolSetup.Name = "btnToolSetup";
            this.btnToolSetup.Text = "buttonItem7";
            this.btnToolSetup.Tooltip = "设置";
            this.btnToolSetup.Click += new System.EventHandler(this.btnSetup_Click);
            // 
            // btnToolHistory
            // 
            this.btnToolHistory.ImageIndex = 4;
            this.btnToolHistory.ImagePaddingHorizontal = 8;
            this.btnToolHistory.Name = "btnToolHistory";
            this.btnToolHistory.Text = "buttonItem4";
            this.btnToolHistory.Tooltip = "修改历史";
            this.btnToolHistory.Click += new System.EventHandler(this.btnHistory_Click);
            // 
            // btnToolReplace
            // 
            this.btnToolReplace.ImageIndex = 3;
            this.btnToolReplace.ImagePaddingHorizontal = 8;
            this.btnToolReplace.Name = "btnToolReplace";
            this.btnToolReplace.Tooltip = "字符串替换";
            this.btnToolReplace.Click += new System.EventHandler(this.btnStringReplace_Click);
            // 
            // bunToolCheck
            // 
            this.bunToolCheck.ImageIndex = 5;
            this.bunToolCheck.ImagePaddingHorizontal = 8;
            this.bunToolCheck.Name = "bunToolCheck";
            this.bunToolCheck.Text = "buttonItem1";
            this.bunToolCheck.Tooltip = "脚本检查";
            this.bunToolCheck.Click += new System.EventHandler(this.bunToolCheck_Click);
            // 
            // btnToolFormat
            // 
            this.btnToolFormat.ImageIndex = 6;
            this.btnToolFormat.ImagePaddingHorizontal = 8;
            this.btnToolFormat.Name = "btnToolFormat";
            this.btnToolFormat.Text = "buttonItem1";
            this.btnToolFormat.Tooltip = "代码格式化";
            this.btnToolFormat.Click += new System.EventHandler(this.btnToolFormat_Click);
            // 
            // btnHistroyTotal
            // 
            this.btnHistroyTotal.ImagePaddingHorizontal = 8;
            this.btnHistroyTotal.Name = "btnHistroyTotal";
            this.btnHistroyTotal.Text = "总历史记录";
            this.btnHistroyTotal.Click += new System.EventHandler(this.buttonItem5_Click);
            // 
            // barDebug
            // 
            this.barDebug.AccessibleDescription = "DotNetBar Bar (barDebug)";
            this.barDebug.AccessibleName = "DotNetBar Bar";
            this.barDebug.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barDebug.DockLine = 2;
            this.barDebug.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barDebug.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barDebug.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bSwitchBreakPoint,
            this.bMouseGetWord,
            this.bDetachDebugFile,
            this.bStartDebug,
            this.bStartDebugClient,
            this.bStopDebug,
            this.bStepOver,
            this.bStepInto,
            this.bContinueDebug});
            this.barDebug.Location = new System.Drawing.Point(0, 56);
            this.barDebug.Name = "barDebug";
            this.barDebug.Size = new System.Drawing.Size(867, 25);
            this.barDebug.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barDebug.TabIndex = 2;
            this.barDebug.TabStop = false;
            this.barDebug.Text = "bar5";
            // 
            // bSwitchBreakPoint
            // 
            this.bSwitchBreakPoint.ImagePaddingHorizontal = 8;
            this.bSwitchBreakPoint.Name = "bSwitchBreakPoint";
            this.bSwitchBreakPoint.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F9);
            this.bSwitchBreakPoint.Text = "设置/取消断点(<u>F9</u>)";
            this.bSwitchBreakPoint.Click += new System.EventHandler(this.bSwitchBreakPoint_Click);
            // 
            // bMouseGetWord
            // 
            this.bMouseGetWord.ImagePaddingHorizontal = 8;
            this.bMouseGetWord.Name = "bMouseGetWord";
            this.bMouseGetWord.Text = "关闭鼠标取词";
            this.bMouseGetWord.Click += new System.EventHandler(this.bMouseGetWord_Click);
            // 
            // bDetachDebugFile
            // 
            this.bDetachDebugFile.ImagePaddingHorizontal = 8;
            this.bDetachDebugFile.Name = "bDetachDebugFile";
            this.bDetachDebugFile.Text = "取消挂接脚本";
            this.bDetachDebugFile.Click += new System.EventHandler(this.bDetachDebugFile_Click);
            // 
            // bStartDebug
            // 
            this.bStartDebug.BeginGroup = true;
            this.bStartDebug.ImagePaddingHorizontal = 8;
            this.bStartDebug.Name = "bStartDebug";
            this.bStartDebug.Text = "开始调试(服务端脚本)";
            this.bStartDebug.Tooltip = "调试scripts目录下的脚本";
            this.bStartDebug.Click += new System.EventHandler(this.bStartDebug_Click);
            // 
            // bStartDebugClient
            // 
            this.bStartDebugClient.ImagePaddingHorizontal = 8;
            this.bStartDebugClient.Name = "bStartDebugClient";
            this.bStartDebugClient.Text = "开始调试(客户端脚本)";
            this.bStartDebugClient.Tooltip = "调试ui目录下的脚本";
            this.bStartDebugClient.Click += new System.EventHandler(this.bStartDebugClient_Click);
            // 
            // bStopDebug
            // 
            this.bStopDebug.ImagePaddingHorizontal = 8;
            this.bStopDebug.Name = "bStopDebug";
            this.bStopDebug.Text = "停止调试";
            this.bStopDebug.Click += new System.EventHandler(this.bStopDebug_Click);
            // 
            // bStepOver
            // 
            this.bStepOver.BeginGroup = true;
            this.bStepOver.Enabled = false;
            this.bStepOver.ImagePaddingHorizontal = 8;
            this.bStepOver.Name = "bStepOver";
            this.bStepOver.Text = "单步跳过(<u>F10</u>)";
            this.bStepOver.Click += new System.EventHandler(this.bStepOver_Click);
            // 
            // bStepInto
            // 
            this.bStepInto.Enabled = false;
            this.bStepInto.ImagePaddingHorizontal = 8;
            this.bStepInto.Name = "bStepInto";
            this.bStepInto.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F11);
            this.bStepInto.Text = "单步进入(<u>F11</u>)";
            this.bStepInto.Click += new System.EventHandler(this.bStepDebug_Click);
            // 
            // bContinueDebug
            // 
            this.bContinueDebug.Enabled = false;
            this.bContinueDebug.ImagePaddingHorizontal = 8;
            this.bContinueDebug.Name = "bContinueDebug";
            this.bContinueDebug.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F5);
            this.bContinueDebug.Text = "继续运行(<u>F5</u>)";
            this.bContinueDebug.Click += new System.EventHandler(this.bContinueDebug_Click);
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(0, 81);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(886, 0);
            this.dockSite3.TabIndex = 3;
            this.dockSite3.TabStop = false;
            // 
            // tabStrip1
            // 
            this.tabStrip1.AutoSelectAttachedControl = true;
            this.tabStrip1.CanReorderTabs = true;
            this.tabStrip1.CloseButtonOnTabsAlwaysDisplayed = false;
            this.tabStrip1.CloseButtonOnTabsVisible = true;
            this.tabStrip1.CloseButtonPosition = DevComponents.DotNetBar.eTabCloseButtonPosition.Right;
            this.tabStrip1.CloseButtonVisible = false;
            this.tabStrip1.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabStrip1.Location = new System.Drawing.Point(207, 81);
            this.tabStrip1.MdiTabbedDocuments = true;
            this.tabStrip1.Name = "tabStrip1";
            this.tabStrip1.SelectedTab = null;
            this.tabStrip1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabStrip1.Size = new System.Drawing.Size(679, 23);
            this.tabStrip1.Style = DevComponents.DotNetBar.eTabStripStyle.Office2007Document;
            this.tabStrip1.TabAlignment = DevComponents.DotNetBar.eTabStripAlignment.Top;
            this.tabStrip1.TabIndex = 10;
            this.tabStrip1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabStrip1.Text = "tabStrip1";
            // 
            // tabItem1
            // 
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "tabItem1";
            // 
            // popTreePh_Folder
            // 
            this.popTreePh_Folder.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmdTreePhFolder_newFolder,
            this.cmdTreePhFolder_newFile,
            this.cmdTreePhFolder_del,
            this.cmdTreePhFolder_rename,
            this.toolStripSeparator1,
            this.cmdTreePhFolder_check,
            this.cmdTreePhFolder_export,
            this.cmdTreePhFolder_openlocal,
            this.bImportLocalData});
            this.popTreePh_Folder.Name = "popTreePh";
            this.popTreePh_Folder.Size = new System.Drawing.Size(119, 186);
            // 
            // cmdTreePhFolder_newFolder
            // 
            this.cmdTreePhFolder_newFolder.Name = "cmdTreePhFolder_newFolder";
            this.cmdTreePhFolder_newFolder.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_newFolder.Text = "新建分类";
            this.cmdTreePhFolder_newFolder.Click += new System.EventHandler(this.cmdTreePhFolder_newFolder_Click);
            // 
            // cmdTreePhFolder_newFile
            // 
            this.cmdTreePhFolder_newFile.Name = "cmdTreePhFolder_newFile";
            this.cmdTreePhFolder_newFile.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_newFile.Text = "新建文件";
            this.cmdTreePhFolder_newFile.Click += new System.EventHandler(this.cmdTreePhFolder_newFile_Click);
            // 
            // cmdTreePhFolder_del
            // 
            this.cmdTreePhFolder_del.Name = "cmdTreePhFolder_del";
            this.cmdTreePhFolder_del.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_del.Text = "删除";
            this.cmdTreePhFolder_del.Click += new System.EventHandler(this.cmdTreePhFolder_del_Click);
            // 
            // cmdTreePhFolder_rename
            // 
            this.cmdTreePhFolder_rename.Name = "cmdTreePhFolder_rename";
            this.cmdTreePhFolder_rename.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_rename.Text = "重命名";
            this.cmdTreePhFolder_rename.Click += new System.EventHandler(this.cmdTreePhFolder_rename_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(115, 6);
            // 
            // cmdTreePhFolder_check
            // 
            this.cmdTreePhFolder_check.Name = "cmdTreePhFolder_check";
            this.cmdTreePhFolder_check.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_check.Text = "检查脚本";
            this.cmdTreePhFolder_check.Click += new System.EventHandler(this.cmdTreePhFolder_check_Click);
            // 
            // cmdTreePhFolder_export
            // 
            this.cmdTreePhFolder_export.Name = "cmdTreePhFolder_export";
            this.cmdTreePhFolder_export.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_export.Text = "导出脚本";
            this.cmdTreePhFolder_export.Click += new System.EventHandler(this.cmdTreePhFolder_export_Click);
            // 
            // cmdTreePhFolder_openlocal
            // 
            this.cmdTreePhFolder_openlocal.Name = "cmdTreePhFolder_openlocal";
            this.cmdTreePhFolder_openlocal.Size = new System.Drawing.Size(118, 22);
            this.cmdTreePhFolder_openlocal.Text = "打开目录";
            this.cmdTreePhFolder_openlocal.Click += new System.EventHandler(this.cmdTreePhFolder_openlocal_Click);
            // 
            // bImportLocalData
            // 
            this.bImportLocalData.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bImportFile,
            this.bImportFolder});
            this.bImportLocalData.Name = "bImportLocalData";
            this.bImportLocalData.Size = new System.Drawing.Size(118, 22);
            this.bImportLocalData.Text = "导入脚本";
            // 
            // bImportFile
            // 
            this.bImportFile.Name = "bImportFile";
            this.bImportFile.Size = new System.Drawing.Size(130, 22);
            this.bImportFile.Text = "单文件导入";
            this.bImportFile.Click += new System.EventHandler(this.bImportFile_Click);
            // 
            // bImportFolder
            // 
            this.bImportFolder.Name = "bImportFolder";
            this.bImportFolder.Size = new System.Drawing.Size(130, 22);
            this.bImportFolder.Text = "批量导入";
            this.bImportFolder.Click += new System.EventHandler(this.bImportFolder_Click);
            // 
            // popTreePh_File
            // 
            this.popTreePh_File.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bNewFileInFolder,
            this.cmdTreePhFile_reanme,
            this.cmdTreePhFile_del,
            this.toolStripSeparator2,
            this.cmdTreePhFile_export});
            this.popTreePh_File.Name = "popTreePh_File";
            this.popTreePh_File.Size = new System.Drawing.Size(179, 120);
            // 
            // cmdTreePhFile_reanme
            // 
            this.cmdTreePhFile_reanme.Name = "cmdTreePhFile_reanme";
            this.cmdTreePhFile_reanme.Size = new System.Drawing.Size(178, 22);
            this.cmdTreePhFile_reanme.Text = "重命名";
            this.cmdTreePhFile_reanme.Click += new System.EventHandler(this.cmdTreePhFile_reanme_Click);
            // 
            // cmdTreePhFile_del
            // 
            this.cmdTreePhFile_del.Name = "cmdTreePhFile_del";
            this.cmdTreePhFile_del.Size = new System.Drawing.Size(178, 22);
            this.cmdTreePhFile_del.Text = "删除";
            this.cmdTreePhFile_del.Click += new System.EventHandler(this.cmdTreePhFile_del_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(175, 6);
            // 
            // cmdTreePhFile_export
            // 
            this.cmdTreePhFile_export.Name = "cmdTreePhFile_export";
            this.cmdTreePhFile_export.Size = new System.Drawing.Size(178, 22);
            this.cmdTreePhFile_export.Text = "导出脚本";
            this.cmdTreePhFile_export.Click += new System.EventHandler(this.cmdTreePhFile_export_Click);
            // 
            // popRight
            // 
            this.popRight.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnReplace});
            this.popRight.Name = "popRight";
            this.popRight.Size = new System.Drawing.Size(149, 26);
            this.popRight.Opening += new System.ComponentModel.CancelEventHandler(this.popRight_Opening);
            // 
            // btnReplace
            // 
            this.btnReplace.Name = "btnReplace";
            this.btnReplace.Size = new System.Drawing.Size(148, 22);
            this.btnReplace.Text = "字符串替换(&Z)";
            this.btnReplace.Click += new System.EventHandler(this.btnStringReplace_Click);
            // 
            // popTreePh_LocalFileRoot
            // 
            this.popTreePh_LocalFileRoot.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bLoadLocalFile,
            this.bLoadLocalFolder});
            this.popTreePh_LocalFileRoot.Name = "popTreePh_LocalFile";
            this.popTreePh_LocalFileRoot.Size = new System.Drawing.Size(143, 48);
            // 
            // bLoadLocalFile
            // 
            this.bLoadLocalFile.Name = "bLoadLocalFile";
            this.bLoadLocalFile.Size = new System.Drawing.Size(142, 22);
            this.bLoadLocalFile.Text = "加载本地文件";
            this.bLoadLocalFile.Click += new System.EventHandler(this.bLoadLocalFile_Click);
            // 
            // bLoadLocalFolder
            // 
            this.bLoadLocalFolder.Name = "bLoadLocalFolder";
            this.bLoadLocalFolder.Size = new System.Drawing.Size(142, 22);
            this.bLoadLocalFolder.Text = "加载本地目录";
            this.bLoadLocalFolder.Click += new System.EventHandler(this.bLoadLocalFolder_Click);
            // 
            // popTreePh_LocalFolder
            // 
            this.popTreePh_LocalFolder.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bAddLocalFile,
            this.bAddAllLocalFile,
            this.bRemoveLocalFolder});
            this.popTreePh_LocalFolder.Name = "popTreePh_LocalFolder";
            this.popTreePh_LocalFolder.Size = new System.Drawing.Size(179, 70);
            // 
            // bAddLocalFile
            // 
            this.bAddLocalFile.Name = "bAddLocalFile";
            this.bAddLocalFile.Size = new System.Drawing.Size(178, 22);
            this.bAddLocalFile.Text = "添加文件";
            this.bAddLocalFile.Click += new System.EventHandler(this.bAddLocalFile_Click);
            // 
            // bAddAllLocalFile
            // 
            this.bAddAllLocalFile.Name = "bAddAllLocalFile";
            this.bAddAllLocalFile.Size = new System.Drawing.Size(178, 22);
            this.bAddAllLocalFile.Text = "添加目录下所有文件";
            this.bAddAllLocalFile.Click += new System.EventHandler(this.bAddAllLocalFile_Click);
            // 
            // bRemoveLocalFolder
            // 
            this.bRemoveLocalFolder.Name = "bRemoveLocalFolder";
            this.bRemoveLocalFolder.Size = new System.Drawing.Size(178, 22);
            this.bRemoveLocalFolder.Text = "移除目录";
            this.bRemoveLocalFolder.Click += new System.EventHandler(this.bRemoveLocalFolder_Click);
            // 
            // popTreePh_LocalFile
            // 
            this.popTreePh_LocalFile.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bRemoveLocalFile});
            this.popTreePh_LocalFile.Name = "popTreePh_LocalFile";
            this.popTreePh_LocalFile.Size = new System.Drawing.Size(119, 26);
            // 
            // bRemoveLocalFile
            // 
            this.bRemoveLocalFile.Name = "bRemoveLocalFile";
            this.bRemoveLocalFile.Size = new System.Drawing.Size(118, 22);
            this.bRemoveLocalFile.Text = "移除文件";
            this.bRemoveLocalFile.Click += new System.EventHandler(this.bRemoveLocalFile_Click);
            // 
            // bNewFileInFolder
            // 
            this.bNewFileInFolder.Name = "bNewFileInFolder";
            this.bNewFileInFolder.Size = new System.Drawing.Size(178, 22);
            this.bNewFileInFolder.Text = "在当前分类新建脚本";
            this.bNewFileInFolder.Click += new System.EventHandler(this.bNewFileInFolder_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(886, 440);
            this.Controls.Add(this.tabStrip1);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.IsMdiContainer = true;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "脚本编辑";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
            this.dockSite4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.barFunction)).EndInit();
            this.barFunction.ResumeLayout(false);
            this.panelDockContainer2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.windowControl)).EndInit();
            this.windowControl.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.outPutBar)).EndInit();
            this.tabControlPanel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.watchTree)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.watchBar)).EndInit();
            this.tabControlPanel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.breakPointView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.breakPointBar)).EndInit();
            this.tabControlPanel4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.callStackView)).EndInit();
            this.dockSite1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar2)).EndInit();
            this.bar2.ResumeLayout(false);
            this.panelDockContainer1.ResumeLayout(false);
            this.dockSite8.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar4)).EndInit();
            this.dockSite7.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barGeneral)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barDebug)).EndInit();
            this.popTreePh_Folder.ResumeLayout(false);
            this.popTreePh_File.ResumeLayout(false);
            this.popRight.ResumeLayout(false);
            this.popTreePh_LocalFileRoot.ResumeLayout(false);
            this.popTreePh_LocalFolder.ResumeLayout(false);
            this.popTreePh_LocalFile.ResumeLayout(false);
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
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.ButtonItem btnScript;
        private DevComponents.DotNetBar.ButtonItem btnSaveAll;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.ButtonItem btnSave;
        private DevComponents.DotNetBar.ButtonItem btnClose;
        private DevComponents.DotNetBar.ButtonItem btnSetup;
        private DevComponents.DotNetBar.Bar bar2;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer1;
        private DevComponents.DotNetBar.DockContainerItem 脚本列表;
        private DevComponents.DotNetBar.TabStrip tabStrip1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.ContextMenuStrip popTreePh_Folder;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_newFolder;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_newFile;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_rename;
        private DevComponents.DotNetBar.ButtonItem btnHistory;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_del;
        private System.Windows.Forms.ContextMenuStrip popTreePh_File;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFile_reanme;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFile_del;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_export;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFile_export;
        private DevComponents.DotNetBar.ButtonItem buttonItem3;
        private DevComponents.DotNetBar.ButtonItem btnStringReplace;
        private DevComponents.DotNetBar.Bar barGeneral;
        private DevComponents.DotNetBar.ButtonItem btnToolSave;
        private System.Windows.Forms.ImageList imageList2;
        private DevComponents.DotNetBar.ButtonItem btnToolSaveAll;
        private DevComponents.DotNetBar.ButtonItem btnToolSetup;
        public System.Windows.Forms.TreeView treePh;
        private DevComponents.DotNetBar.ButtonItem btnToolReplace;
        private DevComponents.DotNetBar.ButtonItem btnToolHistory;
        private System.Windows.Forms.ContextMenuStrip popRight;
        private System.Windows.Forms.ToolStripMenuItem btnReplace;
        private DevComponents.DotNetBar.Bar bar4;
        private DevComponents.DotNetBar.LabelItem labelStatus;
        internal DevComponents.DotNetBar.LabelItem LDBName;
        private DevComponents.DotNetBar.LabelItem LHostName;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_openlocal;
        private DevComponents.DotNetBar.ButtonItem btnCheck;
        private DevComponents.DotNetBar.ButtonItem bunToolCheck;
        private System.Windows.Forms.ToolStripMenuItem cmdTreePhFolder_check;
        private DevComponents.DotNetBar.ButtonItem bClearLS;
        private DevComponents.DotNetBar.ButtonItem btnToolFormat;
        private DevComponents.DotNetBar.ButtonItem btnFormat;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem buttonItem4;
        private DevComponents.DotNetBar.ButtonItem btnHistoryTotal;
        private DevComponents.DotNetBar.ButtonItem btnHistroyTotal;
        private DevComponents.DotNetBar.Bar barDebug;
        private DevComponents.DotNetBar.ButtonItem bStartDebug;
        private DevComponents.DotNetBar.ButtonItem bStopDebug;
        private DevComponents.DotNetBar.Bar barFunction;
        private DevComponents.DotNetBar.PanelDockContainer panelDockContainer2;
        private DevComponents.DotNetBar.TabControl windowControl;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.TabItem tabItem3;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel3;
        private DevComponents.DotNetBar.Bar watchBar;
        private DevComponents.DotNetBar.ButtonItem bDeleteVar;
        private DevComponents.DotNetBar.ButtonItem bDeleteAllVar;
        private DevComponents.DotNetBar.TabItem tabItem4;
        private DevComponents.DotNetBar.Bar breakPointBar;
        private DevComponents.DotNetBar.ButtonItem bDeleteBreakPoint;
        private DevComponents.DotNetBar.ButtonItem bRefreshVar;
        private DevComponents.DotNetBar.ButtonItem bDeleteAllBreakPoint;
        private DevComponents.DotNetBar.Controls.TextBoxX outputBox;
        private DevComponents.DotNetBar.Bar outPutBar;
        private DevComponents.DotNetBar.ButtonItem bDeleteOutput;
        private DevComponents.DotNetBar.Controls.DataGridViewX breakPointView;
        private DevComponents.DotNetBar.ButtonItem bStepInto;
        private DevComponents.DotNetBar.ButtonItem bContinueDebug;
        private DevComponents.DotNetBar.ButtonItem bAddVar;
        private System.Windows.Forms.DataGridViewCheckBoxColumn EnableBreakPoint;
        private System.Windows.Forms.DataGridViewTextBoxColumn BreakPointCondition;
        private System.Windows.Forms.DataGridViewTextBoxColumn BreakPointFileName;
        private System.Windows.Forms.DataGridViewTextBoxColumn BreakPointLineIndex;
        private System.Windows.Forms.DataGridViewTextBoxColumn BreakPointState;
        private DevComponents.DotNetBar.ButtonItem bSwitchBreakPoint;
        private DevComponents.DotNetBar.ButtonItem bStepOver;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel4;
        private DevComponents.DotNetBar.TabItem tabItem5;
        private DevComponents.DotNetBar.Controls.DataGridViewX callStackView;
        private System.Windows.Forms.DataGridViewTextBoxColumn CallStackLevel;
        private System.Windows.Forms.DataGridViewTextBoxColumn CallStackFileName;
        private System.Windows.Forms.DataGridViewTextBoxColumn CallStackLineIndex;
        private System.Windows.Forms.DataGridViewTextBoxColumn CallStackFunctionName;
        private DevComponents.DotNetBar.ButtonItem bEnableServerDebugOutput;
        private DevComponents.DotNetBar.ButtonItem bViewGeneralToolBar;
        private DevComponents.DotNetBar.ButtonItem bViewDebugToolBar;
        private DevComponents.DotNetBar.ButtonItem bMouseGetWord;
        private DevComponents.DotNetBar.ButtonItem bEdit;
        private System.Windows.Forms.ContextMenuStrip popTreePh_LocalFileRoot;
        private System.Windows.Forms.ToolStripMenuItem bLoadLocalFile;
        private System.Windows.Forms.ContextMenuStrip popTreePh_LocalFolder;
        private System.Windows.Forms.ToolStripMenuItem bRemoveLocalFolder;
        private System.Windows.Forms.ToolStripMenuItem bAddLocalFile;
        private System.Windows.Forms.ContextMenuStrip popTreePh_LocalFile;
        private System.Windows.Forms.ToolStripMenuItem bRemoveLocalFile;
        private DevComponents.DotNetBar.ButtonItem bStartDebugClient;
        private System.Windows.Forms.ToolStripMenuItem bAddAllLocalFile;
        private DevComponents.AdvTree.AdvTree watchTree;
        private DevComponents.AdvTree.ColumnHeader varNameColumn;
        private DevComponents.AdvTree.ColumnHeader varValueColumn;
        private DevComponents.AdvTree.ColumnHeader varFileNameColumn;
        private DevComponents.AdvTree.ColumnHeader varStateColumn;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private System.Windows.Forms.ToolStripMenuItem bLoadLocalFolder;
        private DevComponents.DotNetBar.ButtonItem bDetachDebugFile;
        private System.Windows.Forms.ToolStripMenuItem bImportLocalData;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem bImportFile;
        private System.Windows.Forms.ToolStripMenuItem bImportFolder;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem bNewFileInFolder;
    }
}

