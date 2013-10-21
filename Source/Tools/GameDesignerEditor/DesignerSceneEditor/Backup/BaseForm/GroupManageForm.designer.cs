namespace DesignerSceneEditor
{
	partial class GroupManageForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.reviveTree = new DevComponents.AdvTree.AdvTree();
            this.node1 = new DevComponents.AdvTree.Node();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel4 = new DevComponents.DotNetBar.TabControlPanel();
            this.aiParameterTree = new DevComponents.AdvTree.AdvTree();
            this.node5 = new DevComponents.AdvTree.Node();
            this.nodeConnector4 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle4 = new DevComponents.DotNetBar.ElementStyle();
            this.tabItem4 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel3 = new DevComponents.DotNetBar.TabControlPanel();
            this.randomTree = new DevComponents.AdvTree.AdvTree();
            this.node4 = new DevComponents.AdvTree.Node();
            this.nodeConnector3 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle3 = new DevComponents.DotNetBar.ElementStyle();
            this.tabItem3 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.aiTree = new DevComponents.AdvTree.AdvTree();
            this.node3 = new DevComponents.AdvTree.Node();
            this.nodeConnector2 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.expandableSplitter1 = new DevComponents.DotNetBar.ExpandableSplitter();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.bUpdateGroup = new DevComponents.DotNetBar.ButtonX();
            this.bDeleteGroup = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel2 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.npcView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.groupView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.bTreeMenu = new DevComponents.DotNetBar.ButtonItem();
            this.bNewTreeNode = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteTreeNode = new DevComponents.DotNetBar.ButtonItem();
            this.node2 = new DevComponents.AdvTree.Node();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.reviveTree)).BeginInit();
            this.tabControlPanel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.aiParameterTree)).BeginInit();
            this.tabControlPanel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.randomTree)).BeginInit();
            this.tabControlPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.aiTree)).BeginInit();
            this.panelEx1.SuspendLayout();
            this.groupPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.npcView)).BeginInit();
            this.groupPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.groupView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.CanReorderTabs = false;
            this.tabControl1.Controls.Add(this.tabControlPanel1);
            this.tabControl1.Controls.Add(this.tabControlPanel4);
            this.tabControl1.Controls.Add(this.tabControlPanel3);
            this.tabControl1.Controls.Add(this.tabControlPanel2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Left;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(323, 639);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.tabItem1);
            this.tabControl1.Tabs.Add(this.tabItem2);
            this.tabControl1.Tabs.Add(this.tabItem3);
            this.tabControl1.Tabs.Add(this.tabItem4);
            this.tabControl1.Text = "tabControl1";
            this.tabControl1.SelectedTabChanged += new DevComponents.DotNetBar.TabStrip.SelectedTabChangedEventHandler(this.tabControl1_SelectedTabChanged);
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.reviveTree);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(323, 613);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 1;
            this.tabControlPanel1.TabItem = this.tabItem1;
            // 
            // reviveTree
            // 
            this.reviveTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.reviveTree.AllowDrop = true;
            this.reviveTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.reviveTree.BackgroundStyle.Class = "TreeBorderKey";
            this.reviveTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.reviveTree.DragDropEnabled = false;
            this.reviveTree.HotTracking = true;
            this.reviveTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.reviveTree.Location = new System.Drawing.Point(1, 1);
            this.reviveTree.Name = "reviveTree";
            this.reviveTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node1});
            this.reviveTree.NodesConnector = this.nodeConnector1;
            this.reviveTree.NodeStyle = this.elementStyle1;
            this.reviveTree.PathSeparator = ";";
            this.reviveTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.reviveTree.Size = new System.Drawing.Size(321, 611);
            this.reviveTree.Styles.Add(this.elementStyle1);
            this.reviveTree.SuspendPaint = false;
            this.reviveTree.TabIndex = 0;
            this.reviveTree.Text = "advTree1";
            this.reviveTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.ShowGroupMember);
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
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "重生组";
            // 
            // tabControlPanel4
            // 
            this.tabControlPanel4.Controls.Add(this.aiParameterTree);
            this.tabControlPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel4.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel4.Name = "tabControlPanel4";
            this.tabControlPanel4.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel4.Size = new System.Drawing.Size(323, 613);
            this.tabControlPanel4.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel4.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel4.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel4.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel4.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel4.Style.GradientAngle = 90;
            this.tabControlPanel4.TabIndex = 4;
            this.tabControlPanel4.TabItem = this.tabItem4;
            // 
            // aiParameterTree
            // 
            this.aiParameterTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.aiParameterTree.AllowDrop = true;
            this.aiParameterTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.aiParameterTree.BackgroundStyle.Class = "TreeBorderKey";
            this.aiParameterTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.aiParameterTree.DragDropEnabled = false;
            this.aiParameterTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.aiParameterTree.Location = new System.Drawing.Point(1, 1);
            this.aiParameterTree.Name = "aiParameterTree";
            this.aiParameterTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node5});
            this.aiParameterTree.NodesConnector = this.nodeConnector4;
            this.aiParameterTree.NodeStyle = this.elementStyle4;
            this.aiParameterTree.PathSeparator = ";";
            this.aiParameterTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.aiParameterTree.Size = new System.Drawing.Size(321, 611);
            this.aiParameterTree.Styles.Add(this.elementStyle4);
            this.aiParameterTree.SuspendPaint = false;
            this.aiParameterTree.TabIndex = 0;
            this.aiParameterTree.Text = "advTree1";
            this.aiParameterTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.ShowGroupMember);
            // 
            // node5
            // 
            this.node5.Expanded = true;
            this.node5.Name = "node5";
            this.node5.Text = "node5";
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
            // tabItem4
            // 
            this.tabItem4.AttachedControl = this.tabControlPanel4;
            this.tabItem4.Name = "tabItem4";
            this.tabItem4.Text = "AI参数组";
            // 
            // tabControlPanel3
            // 
            this.tabControlPanel3.Controls.Add(this.randomTree);
            this.tabControlPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel3.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel3.Name = "tabControlPanel3";
            this.tabControlPanel3.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel3.Size = new System.Drawing.Size(323, 613);
            this.tabControlPanel3.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel3.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel3.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel3.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel3.Style.GradientAngle = 90;
            this.tabControlPanel3.TabIndex = 3;
            this.tabControlPanel3.TabItem = this.tabItem3;
            // 
            // randomTree
            // 
            this.randomTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.randomTree.AllowDrop = true;
            this.randomTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.randomTree.BackgroundStyle.Class = "TreeBorderKey";
            this.randomTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.randomTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.randomTree.Location = new System.Drawing.Point(1, 1);
            this.randomTree.Name = "randomTree";
            this.randomTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node4});
            this.randomTree.NodesConnector = this.nodeConnector3;
            this.randomTree.NodeStyle = this.elementStyle3;
            this.randomTree.PathSeparator = ";";
            this.randomTree.Size = new System.Drawing.Size(321, 611);
            this.randomTree.Styles.Add(this.elementStyle3);
            this.randomTree.SuspendPaint = false;
            this.randomTree.TabIndex = 0;
            this.randomTree.Text = "advTree1";
            this.randomTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.ShowGroupMember);
            // 
            // node4
            // 
            this.node4.Expanded = true;
            this.node4.Name = "node4";
            this.node4.Text = "node4";
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
            // tabItem3
            // 
            this.tabItem3.AttachedControl = this.tabControlPanel3;
            this.tabItem3.Name = "tabItem3";
            this.tabItem3.Text = "随机组";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.aiTree);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(323, 613);
            this.tabControlPanel2.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel2.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel2.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel2.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel2.Style.GradientAngle = 90;
            this.tabControlPanel2.TabIndex = 2;
            this.tabControlPanel2.TabItem = this.tabItem2;
            // 
            // aiTree
            // 
            this.aiTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.aiTree.AllowDrop = true;
            this.aiTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.aiTree.BackgroundStyle.Class = "TreeBorderKey";
            this.aiTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.aiTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.aiTree.Location = new System.Drawing.Point(1, 1);
            this.aiTree.Name = "aiTree";
            this.aiTree.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.node3});
            this.aiTree.NodesConnector = this.nodeConnector2;
            this.aiTree.NodeStyle = this.elementStyle2;
            this.aiTree.PathSeparator = ";";
            this.aiTree.Size = new System.Drawing.Size(321, 611);
            this.aiTree.Styles.Add(this.elementStyle2);
            this.aiTree.SuspendPaint = false;
            this.aiTree.TabIndex = 0;
            this.aiTree.Text = "advTree1";
            this.aiTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.ShowGroupMember);
            // 
            // node3
            // 
            this.node3.Expanded = true;
            this.node3.Name = "node3";
            this.node3.Text = "node3";
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
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel2;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "仇恨组";
            // 
            // expandableSplitter1
            // 
            this.expandableSplitter1.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(101)))), ((int)(((byte)(147)))), ((int)(((byte)(207)))));
            this.expandableSplitter1.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandableSplitter1.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
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
            this.expandableSplitter1.Location = new System.Drawing.Point(323, 0);
            this.expandableSplitter1.Name = "expandableSplitter1";
            this.expandableSplitter1.Size = new System.Drawing.Size(10, 639);
            this.expandableSplitter1.Style = DevComponents.DotNetBar.eSplitterStyle.Office2007;
            this.expandableSplitter1.TabIndex = 1;
            this.expandableSplitter1.TabStop = false;
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.bUpdateGroup);
            this.panelEx1.Controls.Add(this.bDeleteGroup);
            this.panelEx1.Controls.Add(this.groupPanel2);
            this.panelEx1.Controls.Add(this.groupPanel1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(333, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(429, 639);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 2;
            // 
            // bUpdateGroup
            // 
            this.bUpdateGroup.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bUpdateGroup.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bUpdateGroup.Location = new System.Drawing.Point(317, 598);
            this.bUpdateGroup.Name = "bUpdateGroup";
            this.bUpdateGroup.Size = new System.Drawing.Size(100, 20);
            this.bUpdateGroup.TabIndex = 6;
            this.bUpdateGroup.Text = "更新分组数据";
            this.bUpdateGroup.Click += new System.EventHandler(this.bUpdateGroup_Click);
            // 
            // bDeleteGroup
            // 
            this.bDeleteGroup.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bDeleteGroup.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bDeleteGroup.Location = new System.Drawing.Point(211, 598);
            this.bDeleteGroup.Name = "bDeleteGroup";
            this.bDeleteGroup.Size = new System.Drawing.Size(100, 20);
            this.bDeleteGroup.TabIndex = 5;
            this.bDeleteGroup.Text = "删除分组";
            this.bDeleteGroup.Click += new System.EventHandler(this.bDeleteGroup_Click);
            // 
            // groupPanel2
            // 
            this.groupPanel2.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel2.Controls.Add(this.npcView);
            this.groupPanel2.Location = new System.Drawing.Point(10, 320);
            this.groupPanel2.Name = "groupPanel2";
            this.groupPanel2.Size = new System.Drawing.Size(407, 260);
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
            this.groupPanel2.TabIndex = 4;
            this.groupPanel2.Text = "分组成员";
            // 
            // npcView
            // 
            this.npcView.AllowUserToAddRows = false;
            this.npcView.AllowUserToDeleteRows = false;
            this.npcView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.npcView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.npcView.DefaultCellStyle = dataGridViewCellStyle1;
            this.npcView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.npcView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.npcView.Location = new System.Drawing.Point(0, 0);
            this.npcView.Name = "npcView";
            this.npcView.ReadOnly = true;
            this.npcView.RowTemplate.Height = 23;
            this.npcView.Size = new System.Drawing.Size(401, 236);
            this.npcView.TabIndex = 0;
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.groupView);
            this.groupPanel1.Location = new System.Drawing.Point(10, 12);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(407, 291);
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
            this.groupPanel1.TabIndex = 3;
            this.groupPanel1.Text = "分组数据";
            // 
            // groupView
            // 
            this.groupView.AllowUserToAddRows = false;
            this.groupView.AllowUserToDeleteRows = false;
            this.groupView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.groupView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.groupView.DefaultCellStyle = dataGridViewCellStyle2;
            this.groupView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnF2;
            this.groupView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.groupView.Location = new System.Drawing.Point(0, 0);
            this.groupView.Name = "groupView";
            this.groupView.RowTemplate.Height = 23;
            this.groupView.Size = new System.Drawing.Size(401, 267);
            this.groupView.TabIndex = 2;
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bTreeMenu});
            this.contextMenuBar1.Location = new System.Drawing.Point(6, 3);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(90, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 0;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // bTreeMenu
            // 
            this.bTreeMenu.AutoExpandOnClick = true;
            this.bTreeMenu.ImagePaddingHorizontal = 8;
            this.bTreeMenu.Name = "bTreeMenu";
            this.bTreeMenu.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bNewTreeNode,
            this.bDeleteTreeNode});
            this.bTreeMenu.Text = "bTreeMenu";
            // 
            // bNewTreeNode
            // 
            this.bNewTreeNode.ImagePaddingHorizontal = 8;
            this.bNewTreeNode.Name = "bNewTreeNode";
            this.bNewTreeNode.Text = "新建";
            // 
            // bDeleteTreeNode
            // 
            this.bDeleteTreeNode.ImagePaddingHorizontal = 8;
            this.bDeleteTreeNode.Name = "bDeleteTreeNode";
            this.bDeleteTreeNode.Text = "删除";
            // 
            // node2
            // 
            this.node2.Expanded = true;
            this.node2.HostedControl = this.contextMenuBar1;
            this.node2.Name = "node2";
            this.node2.Text = "contextMenuBar1";
            // 
            // GroupManageForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(762, 639);
            this.Controls.Add(this.panelEx1);
            this.Controls.Add(this.expandableSplitter1);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "GroupManageForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "分组管理";
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.reviveTree)).EndInit();
            this.tabControlPanel4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.aiParameterTree)).EndInit();
            this.tabControlPanel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.randomTree)).EndInit();
            this.tabControlPanel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.aiTree)).EndInit();
            this.panelEx1.ResumeLayout(false);
            this.groupPanel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.npcView)).EndInit();
            this.groupPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.groupView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

		}

		#endregion

        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.ExpandableSplitter expandableSplitter1;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.AdvTree.AdvTree reviveTree;
        private DevComponents.AdvTree.Node node1;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem bTreeMenu;
        private DevComponents.DotNetBar.ButtonItem bNewTreeNode;
        private DevComponents.DotNetBar.ButtonItem bDeleteTreeNode;
        private DevComponents.DotNetBar.Controls.DataGridViewX groupView;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel3;
        private DevComponents.DotNetBar.TabItem tabItem3;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel2;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.Controls.DataGridViewX npcView;
        private DevComponents.AdvTree.Node node2;
        private DevComponents.AdvTree.AdvTree randomTree;
        private DevComponents.AdvTree.Node node4;
        private DevComponents.AdvTree.NodeConnector nodeConnector3;
        private DevComponents.DotNetBar.ElementStyle elementStyle3;
        private DevComponents.AdvTree.AdvTree aiTree;
        private DevComponents.AdvTree.Node node3;
        private DevComponents.AdvTree.NodeConnector nodeConnector2;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel4;
        private DevComponents.DotNetBar.TabItem tabItem4;
        private DevComponents.AdvTree.AdvTree aiParameterTree;
        private DevComponents.AdvTree.Node node5;
        private DevComponents.AdvTree.NodeConnector nodeConnector4;
        private DevComponents.DotNetBar.ElementStyle elementStyle4;
        private DevComponents.DotNetBar.ButtonX bUpdateGroup;
        private DevComponents.DotNetBar.ButtonX bDeleteGroup;
	}
}