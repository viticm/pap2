namespace DesignerSceneEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.buttonSave = new DevComponents.DotNetBar.ButtonItem();
            this.barStandard = new DevComponents.DotNetBar.Bar();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.buttonCopy = new DevComponents.DotNetBar.ButtonItem();
            this.buttonDelete = new DevComponents.DotNetBar.ButtonItem();
            this.buttonOpen = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem4 = new DevComponents.DotNetBar.ButtonItem();
            this.item_139 = new DevComponents.DotNetBar.CustomizeItem();
            this.tabStrip1 = new DevComponents.DotNetBar.TabStrip();
            this.dockSearchResults = new DevComponents.DotNetBar.DockContainerItem();
            this.labelStatus = new DevComponents.DotNetBar.LabelItem();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.itemContainer1 = new DevComponents.DotNetBar.ItemContainer();
            this.labelItem1 = new DevComponents.DotNetBar.LabelItem();
            this.cameraLocationBox = new DevComponents.DotNetBar.TextBoxItem();
            this.buttonItem3 = new DevComponents.DotNetBar.ButtonItem();
            this.sliderItem1 = new DevComponents.DotNetBar.SliderItem();
            this.checkBoxItem1 = new DevComponents.DotNetBar.CheckBoxItem();
            this.checkShowPoly = new DevComponents.DotNetBar.CheckBoxItem();
            this.checkToggleShowProfile = new DevComponents.DotNetBar.CheckBoxItem();
            this.labelCameraDis = new DevComponents.DotNetBar.LabelItem();
            this.textBoxCameraDis = new DevComponents.DotNetBar.TextBoxItem();
            this.barRightDockSite = new DevComponents.DotNetBar.DockSite();
            this.barTopDockSite = new DevComponents.DotNetBar.DockSite();
            this.barBottomDockSite = new DevComponents.DotNetBar.DockSite();
            this.dockContainerItem1 = new DevComponents.DotNetBar.DockContainerItem();
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.barLeftDockSite = new DevComponents.DotNetBar.DockSite();
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.mainmenu = new DevComponents.DotNetBar.Bar();
            this.bFile = new DevComponents.DotNetBar.ButtonItem();
            this.bOpen = new DevComponents.DotNetBar.ButtonItem();
            this.bSave = new DevComponents.DotNetBar.ButtonItem();
            this.bExit = new DevComponents.DotNetBar.ButtonItem();
            this.bEdit = new DevComponents.DotNetBar.ButtonItem();
            this.bCopy = new DevComponents.DotNetBar.ButtonItem();
            this.bDelete = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.CreateImageBut = new DevComponents.DotNetBar.ButtonItem();
            this.barTool = new DevComponents.DotNetBar.Bar();
            this.buttonFullScreen = new DevComponents.DotNetBar.ButtonItem();
            this.bFiltSelectObject = new DevComponents.DotNetBar.ButtonItem();
            this.bPMode = new DevComponents.DotNetBar.ButtonItem();
            this.bTopView = new DevComponents.DotNetBar.ButtonItem();
            this.lObjectOperateion = new DevComponents.DotNetBar.LabelItem();
            this.buttonSelect = new DevComponents.DotNetBar.ButtonItem();
            this.buttonMove = new DevComponents.DotNetBar.ButtonItem();
            this.buttonRotate = new DevComponents.DotNetBar.ButtonItem();
            this.buttonScale = new DevComponents.DotNetBar.ButtonItem();
            this.comboItem1 = new DevComponents.Editors.ComboItem();
            this.comboItem2 = new DevComponents.Editors.ComboItem();
            this.comboItem3 = new DevComponents.Editors.ComboItem();
            this.comboItem4 = new DevComponents.Editors.ComboItem();
            this.mdiClient1 = new System.Windows.Forms.MdiClient();
            this.RegionSplitBut = new DevComponents.DotNetBar.ButtonItem();
            ((System.ComponentModel.ISupportInitialize)(this.barStandard)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.dockSite3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mainmenu)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barTool)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonSave
            // 
            this.buttonSave.GlobalName = "buttonSave";
            this.buttonSave.ImageIndex = 25;
            this.buttonSave.ImagePaddingHorizontal = 8;
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonSave.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.buttonSave.Text = "保存";
            this.buttonSave.Tooltip = "保存";
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // barStandard
            // 
            this.barStandard.AccessibleDescription = "DotNetBar Bar (barStandard)";
            this.barStandard.AccessibleName = "DotNetBar Bar";
            this.barStandard.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barStandard.CanHide = true;
            this.barStandard.DockLine = 1;
            this.barStandard.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barStandard.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barStandard.Images = this.imageList1;
            this.barStandard.ImagesLarge = this.imageList1;
            this.barStandard.ImagesMedium = this.imageList1;
            this.barStandard.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonCopy,
            this.buttonDelete,
            this.buttonSave,
            this.buttonOpen,
            this.item_139});
            this.barStandard.Location = new System.Drawing.Point(0, 25);
            this.barStandard.Margin = new System.Windows.Forms.Padding(1);
            this.barStandard.Name = "barStandard";
            this.barStandard.Size = new System.Drawing.Size(164, 33);
            this.barStandard.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barStandard.TabIndex = 1;
            this.barStandard.TabStop = false;
            this.barStandard.Text = "标准";
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Magenta;
            this.imageList1.Images.SetKeyName(0, "");
            this.imageList1.Images.SetKeyName(1, "");
            this.imageList1.Images.SetKeyName(2, "");
            this.imageList1.Images.SetKeyName(3, "");
            this.imageList1.Images.SetKeyName(4, "");
            this.imageList1.Images.SetKeyName(5, "");
            this.imageList1.Images.SetKeyName(6, "");
            this.imageList1.Images.SetKeyName(7, "");
            this.imageList1.Images.SetKeyName(8, "");
            this.imageList1.Images.SetKeyName(9, "");
            this.imageList1.Images.SetKeyName(10, "");
            this.imageList1.Images.SetKeyName(11, "");
            this.imageList1.Images.SetKeyName(12, "");
            this.imageList1.Images.SetKeyName(13, "");
            this.imageList1.Images.SetKeyName(14, "");
            this.imageList1.Images.SetKeyName(15, "");
            this.imageList1.Images.SetKeyName(16, "");
            this.imageList1.Images.SetKeyName(17, "");
            this.imageList1.Images.SetKeyName(18, "");
            this.imageList1.Images.SetKeyName(19, "NPC.ico");
            this.imageList1.Images.SetKeyName(20, "skill.ico");
            this.imageList1.Images.SetKeyName(21, "Save.ico");
            this.imageList1.Images.SetKeyName(22, "Close.ico");
            this.imageList1.Images.SetKeyName(23, "Admin tools.ico");
            this.imageList1.Images.SetKeyName(24, "open.ico");
            this.imageList1.Images.SetKeyName(25, "Project3.ico");
            this.imageList1.Images.SetKeyName(26, "Notepad 1.ico");
            this.imageList1.Images.SetKeyName(27, "kcmsystem.ico");
            this.imageList1.Images.SetKeyName(28, "lin_agt_wrench.ico");
            this.imageList1.Images.SetKeyName(29, "package_utilities.ico");
            this.imageList1.Images.SetKeyName(30, "start_here.ico");
            this.imageList1.Images.SetKeyName(31, "gimp.ico");
            this.imageList1.Images.SetKeyName(32, "open.ico");
            this.imageList1.Images.SetKeyName(33, "ico00009.ico");
            this.imageList1.Images.SetKeyName(34, "demoDoc.ico");
            this.imageList1.Images.SetKeyName(35, "gray.ico");
            this.imageList1.Images.SetKeyName(36, "no1.ico");
            this.imageList1.Images.SetKeyName(37, "ok.ico");
            this.imageList1.Images.SetKeyName(38, "r1.ico");
            this.imageList1.Images.SetKeyName(39, "b1.ico");
            this.imageList1.Images.SetKeyName(40, "canc.ico");
            this.imageList1.Images.SetKeyName(41, "demo.ico");
            this.imageList1.Images.SetKeyName(42, "startme.ico");
            // 
            // buttonCopy
            // 
            this.buttonCopy.ImageIndex = 1;
            this.buttonCopy.ImagePaddingHorizontal = 8;
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Text = "复制";
            this.buttonCopy.Tooltip = "复制选中的对象";
            this.buttonCopy.Click += new System.EventHandler(this.bCopy_Click);
            // 
            // buttonDelete
            // 
            this.buttonDelete.ImageIndex = 40;
            this.buttonDelete.ImagePaddingHorizontal = 8;
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Text = "删除";
            this.buttonDelete.Tooltip = "删除选中的对象";
            this.buttonDelete.Click += new System.EventHandler(this.bDelete_Click);
            // 
            // buttonOpen
            // 
            this.buttonOpen.GlobalName = "buttonOpen";
            this.buttonOpen.ImageIndex = 24;
            this.buttonOpen.ImagePaddingHorizontal = 8;
            this.buttonOpen.Name = "buttonOpen";
            this.buttonOpen.SplitButton = true;
            this.buttonOpen.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem4});
            this.buttonOpen.Text = "打开";
            this.buttonOpen.Tooltip = "打开";
            this.buttonOpen.Click += new System.EventHandler(this.buttonOpen_Click);
            // 
            // buttonItem4
            // 
            this.buttonItem4.ImagePaddingHorizontal = 8;
            this.buttonItem4.Name = "buttonItem4";
            this.buttonItem4.Text = "New Item";
            // 
            // item_139
            // 
            this.item_139.AutoCollapseOnClick = true;
            this.item_139.CanCustomize = true;
            this.item_139.GlobalName = "item_139";
            this.item_139.Name = "item_139";
            this.item_139.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.item_139.Text = "&Add or Remove Buttons";
            this.item_139.Tooltip = "Bar Options";
            // 
            // tabStrip1
            // 
            this.tabStrip1.AutoSelectAttachedControl = true;
            this.tabStrip1.CanReorderTabs = true;
            this.tabStrip1.CloseButtonOnTabsVisible = true;
            this.tabStrip1.CloseButtonVisible = false;
            this.tabStrip1.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabStrip1.Location = new System.Drawing.Point(0, 58);
            this.tabStrip1.MdiTabbedDocuments = true;
            this.tabStrip1.Name = "tabStrip1";
            this.tabStrip1.SelectedTab = null;
            this.tabStrip1.SelectedTabFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabStrip1.Size = new System.Drawing.Size(1103, 28);
            this.tabStrip1.Style = DevComponents.DotNetBar.eTabStripStyle.Office2007Dock;
            this.tabStrip1.TabAlignment = DevComponents.DotNetBar.eTabStripAlignment.Top;
            this.tabStrip1.TabIndex = 18;
            this.tabStrip1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabStrip1.Text = "tabStrip1";
            this.tabStrip1.Visible = false;
            // 
            // dockSearchResults
            // 
            this.dockSearchResults.DefaultFloatingSize = new System.Drawing.Size(256, 196);
            this.dockSearchResults.GlobalItem = true;
            this.dockSearchResults.GlobalName = "dockSearchResults";
            this.dockSearchResults.Image = ((System.Drawing.Image)(resources.GetObject("dockSearchResults.Image")));
            this.dockSearchResults.Name = "dockSearchResults";
            this.dockSearchResults.Text = "Search Results";
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
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.StatusBar;
            this.bar1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.bar1.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.ResizeHandle;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.labelStatus,
            this.itemContainer1,
            this.sliderItem1,
            this.checkBoxItem1,
            this.checkShowPoly,
            this.checkToggleShowProfile,
            this.labelCameraDis,
            this.textBoxCameraDis});
            this.bar1.ItemSpacing = 2;
            this.bar1.Location = new System.Drawing.Point(0, 460);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(1103, 25);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 19;
            this.bar1.TabStop = false;
            this.bar1.Text = "barStatus";
            // 
            // itemContainer1
            // 
            this.itemContainer1.Name = "itemContainer1";
            this.itemContainer1.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.labelItem1,
            this.cameraLocationBox,
            this.buttonItem3});
            // 
            // labelItem1
            // 
            this.labelItem1.Name = "labelItem1";
            this.labelItem1.Text = "摄像机焦点位置";
            // 
            // cameraLocationBox
            // 
            this.cameraLocationBox.Caption = "textBoxItem2";
            this.cameraLocationBox.Name = "cameraLocationBox";
            this.cameraLocationBox.TextBoxWidth = 150;
            this.cameraLocationBox.WatermarkColor = System.Drawing.SystemColors.GrayText;
            this.cameraLocationBox.WatermarkText = "0,0,0";
            // 
            // buttonItem3
            // 
            this.buttonItem3.ImagePaddingHorizontal = 8;
            this.buttonItem3.Name = "buttonItem3";
            this.buttonItem3.Text = "确定";
            this.buttonItem3.Click += new System.EventHandler(this.buttonItem3_Click);
            // 
            // sliderItem1
            // 
            this.sliderItem1.LabelWidth = 100;
            this.sliderItem1.Maximum = 20;
            this.sliderItem1.Minimum = 1;
            this.sliderItem1.Name = "sliderItem1";
            this.sliderItem1.Text = "摄像机移动速度:";
            this.sliderItem1.Value = 6;
            this.sliderItem1.Width = 200;
            this.sliderItem1.ValueChanged += new System.EventHandler(this.sliderItem1_ValueChanged);
            // 
            // checkBoxItem1
            // 
            this.checkBoxItem1.Name = "checkBoxItem1";
            this.checkBoxItem1.Text = "绘制region边界";
            this.checkBoxItem1.CheckedChanged += new DevComponents.DotNetBar.CheckBoxChangeEventHandler(this.checkBoxItem1_CheckedChanged);
            // 
            // checkShowPoly
            // 
            this.checkShowPoly.Checked = true;
            this.checkShowPoly.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkShowPoly.Name = "checkShowPoly";
            this.checkShowPoly.Text = "绘制多边形";
            this.checkShowPoly.CheckedChanged += new DevComponents.DotNetBar.CheckBoxChangeEventHandler(this.checkShowPoly_CheckedChanged);
            // 
            // checkToggleShowProfile
            // 
            this.checkToggleShowProfile.Name = "checkToggleShowProfile";
            this.checkToggleShowProfile.Text = "显示调试信息";
            this.checkToggleShowProfile.CheckedChanged += new DevComponents.DotNetBar.CheckBoxChangeEventHandler(this.checkToggleShowProfile_CheckedChanged);
            // 
            // labelCameraDis
            // 
            this.labelCameraDis.Name = "labelCameraDis";
            this.labelCameraDis.Text = "摄像机视野";
            // 
            // textBoxCameraDis
            // 
            this.textBoxCameraDis.Name = "textBoxCameraDis";
            this.textBoxCameraDis.WatermarkColor = System.Drawing.SystemColors.GrayText;
            this.textBoxCameraDis.InputTextChanged += new DevComponents.DotNetBar.TextBoxItem.TextChangedEventHandler(this.textBoxCameraDis_InputTextChanged);
            // 
            // barRightDockSite
            // 
            this.barRightDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barRightDockSite.Dock = System.Windows.Forms.DockStyle.Right;
            this.barRightDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barRightDockSite.Location = new System.Drawing.Point(1103, 58);
            this.barRightDockSite.Name = "barRightDockSite";
            this.barRightDockSite.Size = new System.Drawing.Size(0, 402);
            this.barRightDockSite.TabIndex = 16;
            this.barRightDockSite.TabStop = false;
            // 
            // barTopDockSite
            // 
            this.barTopDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barTopDockSite.Dock = System.Windows.Forms.DockStyle.Top;
            this.barTopDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barTopDockSite.Location = new System.Drawing.Point(0, 58);
            this.barTopDockSite.Name = "barTopDockSite";
            this.barTopDockSite.Size = new System.Drawing.Size(1103, 0);
            this.barTopDockSite.TabIndex = 13;
            this.barTopDockSite.TabStop = false;
            // 
            // barBottomDockSite
            // 
            this.barBottomDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barBottomDockSite.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.barBottomDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barBottomDockSite.Location = new System.Drawing.Point(0, 460);
            this.barBottomDockSite.Name = "barBottomDockSite";
            this.barBottomDockSite.Size = new System.Drawing.Size(1103, 0);
            this.barBottomDockSite.TabIndex = 14;
            this.barBottomDockSite.TabStop = false;
            // 
            // dockContainerItem1
            // 
            this.dockContainerItem1.Name = "dockContainerItem1";
            this.dockContainerItem1.Text = "日志";
            // 
            // dotNetBarManager1
            // 
            this.dotNetBarManager1.BottomDockSite = this.barBottomDockSite;
            this.dotNetBarManager1.DefinitionName = "";
            this.dotNetBarManager1.Images = this.imageList1;
            this.dotNetBarManager1.ImagesMedium = this.imageList1;
            this.dotNetBarManager1.LeftDockSite = this.barLeftDockSite;
            this.dotNetBarManager1.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.dotNetBarManager1.MdiSystemItemVisible = false;
            this.dotNetBarManager1.ParentForm = this;
            this.dotNetBarManager1.RightDockSite = this.barRightDockSite;
            this.dotNetBarManager1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.dotNetBarManager1.ToolbarBottomDockSite = this.dockSite4;
            this.dotNetBarManager1.ToolbarLeftDockSite = this.dockSite1;
            this.dotNetBarManager1.ToolbarRightDockSite = this.dockSite2;
            this.dotNetBarManager1.ToolbarTopDockSite = this.dockSite3;
            this.dotNetBarManager1.TopDockSite = this.barTopDockSite;
            this.dotNetBarManager1.ItemClick += new System.EventHandler(this.dotNetBarManager1_ItemClick);
            // 
            // barLeftDockSite
            // 
            this.barLeftDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barLeftDockSite.Dock = System.Windows.Forms.DockStyle.Left;
            this.barLeftDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barLeftDockSite.Location = new System.Drawing.Point(0, 58);
            this.barLeftDockSite.Name = "barLeftDockSite";
            this.barLeftDockSite.Size = new System.Drawing.Size(0, 402);
            this.barLeftDockSite.TabIndex = 15;
            this.barLeftDockSite.TabStop = false;
            // 
            // dockSite4
            // 
            this.dockSite4.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite4.Location = new System.Drawing.Point(0, 460);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(1103, 0);
            this.dockSite4.TabIndex = 23;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.Location = new System.Drawing.Point(0, 58);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(0, 402);
            this.dockSite1.TabIndex = 20;
            this.dockSite1.TabStop = false;
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.Location = new System.Drawing.Point(1103, 58);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 402);
            this.dockSite2.TabIndex = 21;
            this.dockSite2.TabStop = false;
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Controls.Add(this.mainmenu);
            this.dockSite3.Controls.Add(this.barStandard);
            this.dockSite3.Controls.Add(this.barTool);
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.Location = new System.Drawing.Point(0, 0);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(1103, 58);
            this.dockSite3.TabIndex = 22;
            this.dockSite3.TabStop = false;
            // 
            // mainmenu
            // 
            this.mainmenu.AccessibleDescription = "DotNetBar Bar (mainmenu)";
            this.mainmenu.AccessibleName = "DotNetBar Bar";
            this.mainmenu.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.mainmenu.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.mainmenu.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bFile,
            this.bEdit,
            this.buttonItem1});
            this.mainmenu.Location = new System.Drawing.Point(0, 0);
            this.mainmenu.LockDockPosition = true;
            this.mainmenu.MenuBar = true;
            this.mainmenu.Name = "mainmenu";
            this.mainmenu.Size = new System.Drawing.Size(1103, 24);
            this.mainmenu.Stretch = true;
            this.mainmenu.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.mainmenu.TabIndex = 0;
            this.mainmenu.TabStop = false;
            this.mainmenu.Text = "主菜单";
            // 
            // bFile
            // 
            this.bFile.ImagePaddingHorizontal = 8;
            this.bFile.Name = "bFile";
            this.bFile.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bOpen,
            this.bSave,
            this.bExit});
            this.bFile.Text = "文件";
            // 
            // bOpen
            // 
            this.bOpen.ImagePaddingHorizontal = 8;
            this.bOpen.Name = "bOpen";
            this.bOpen.Text = "打开";
            this.bOpen.Click += new System.EventHandler(this.buttonOpen_Click);
            // 
            // bSave
            // 
            this.bSave.ImagePaddingHorizontal = 8;
            this.bSave.Name = "bSave";
            this.bSave.Text = "保存";
            this.bSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // bExit
            // 
            this.bExit.ImagePaddingHorizontal = 8;
            this.bExit.Name = "bExit";
            this.bExit.Text = "退出";
            this.bExit.Click += new System.EventHandler(this.bExit_Click);
            // 
            // bEdit
            // 
            this.bEdit.ImagePaddingHorizontal = 8;
            this.bEdit.Name = "bEdit";
            this.bEdit.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bCopy,
            this.bDelete});
            this.bEdit.Text = "编辑";
            // 
            // bCopy
            // 
            this.bCopy.ImagePaddingHorizontal = 8;
            this.bCopy.Name = "bCopy";
            this.bCopy.Text = "复制选中对象";
            this.bCopy.Click += new System.EventHandler(this.bCopy_Click);
            // 
            // bDelete
            // 
            this.bDelete.ImagePaddingHorizontal = 8;
            this.bDelete.Name = "bDelete";
            this.bDelete.Text = "删除选中对象";
            this.bDelete.Click += new System.EventHandler(this.bDelete_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.CreateImageBut,
            this.RegionSplitBut});
            this.buttonItem1.Text = "工具";
            // 
            // CreateImageBut
            // 
            this.CreateImageBut.ImagePaddingHorizontal = 8;
            this.CreateImageBut.Name = "CreateImageBut";
            this.CreateImageBut.Text = "生成场景缩略图";
            this.CreateImageBut.Click += new System.EventHandler(this.CreateImageBut_Click);
            // 
            // barTool
            // 
            this.barTool.AccessibleDescription = "DotNetBar Bar (barTool)";
            this.barTool.AccessibleName = "DotNetBar Bar";
            this.barTool.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barTool.DockLine = 1;
            this.barTool.DockOffset = 164;
            this.barTool.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barTool.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barTool.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonFullScreen,
            this.bFiltSelectObject,
            this.bPMode,
            this.bTopView,
            this.lObjectOperateion,
            this.buttonSelect,
            this.buttonMove,
            this.buttonRotate,
            this.buttonScale});
            this.barTool.Location = new System.Drawing.Point(166, 25);
            this.barTool.Name = "barTool";
            this.barTool.Size = new System.Drawing.Size(386, 33);
            this.barTool.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.barTool.TabIndex = 2;
            this.barTool.TabStop = false;
            this.barTool.Text = "bar2";
            // 
            // buttonFullScreen
            // 
            this.buttonFullScreen.ImageIndex = 38;
            this.buttonFullScreen.ImagePaddingHorizontal = 8;
            this.buttonFullScreen.Name = "buttonFullScreen";
            this.buttonFullScreen.Text = "切换全屏显示";
            this.buttonFullScreen.Tooltip = "切换全屏显示";
            this.buttonFullScreen.Click += new System.EventHandler(this.buttonFullScreen_Click);
            // 
            // bFiltSelectObject
            // 
            this.bFiltSelectObject.ImagePaddingHorizontal = 8;
            this.bFiltSelectObject.Name = "bFiltSelectObject";
            this.bFiltSelectObject.Text = "选择过滤项";
            this.bFiltSelectObject.Click += new System.EventHandler(this.bFiltSelectObject_Click);
            // 
            // bPMode
            // 
            this.bPMode.ImageIndex = 33;
            this.bPMode.ImagePaddingHorizontal = 8;
            this.bPMode.Name = "bPMode";
            this.bPMode.Text = "P模式";
            this.bPMode.Tooltip = "P模式";
            this.bPMode.Click += new System.EventHandler(this.bPMode_Click);
            // 
            // bTopView
            // 
            this.bTopView.ImagePaddingHorizontal = 8;
            this.bTopView.Name = "bTopView";
            this.bTopView.Text = "顶视图";
            this.bTopView.Click += new System.EventHandler(this.bTopView_Click);
            // 
            // lObjectOperateion
            // 
            this.lObjectOperateion.Name = "lObjectOperateion";
            this.lObjectOperateion.Text = "对象操作";
            // 
            // buttonSelect
            // 
            this.buttonSelect.ImageIndex = 42;
            this.buttonSelect.ImagePaddingHorizontal = 8;
            this.buttonSelect.Name = "buttonSelect";
            this.buttonSelect.Text = "选择";
            this.buttonSelect.Tooltip = "选择";
            this.buttonSelect.Click += new System.EventHandler(this.buttonSelect_Click);
            // 
            // buttonMove
            // 
            this.buttonMove.ImagePaddingHorizontal = 8;
            this.buttonMove.Name = "buttonMove";
            this.buttonMove.Text = "移动";
            this.buttonMove.Tooltip = "移动";
            this.buttonMove.Click += new System.EventHandler(this.buttonMove_Click);
            // 
            // buttonRotate
            // 
            this.buttonRotate.ImagePaddingHorizontal = 8;
            this.buttonRotate.Name = "buttonRotate";
            this.buttonRotate.Text = "旋转";
            this.buttonRotate.Tooltip = "旋转";
            this.buttonRotate.Click += new System.EventHandler(this.buttonRotate_Click);
            // 
            // buttonScale
            // 
            this.buttonScale.ImagePaddingHorizontal = 8;
            this.buttonScale.Name = "buttonScale";
            this.buttonScale.Text = "缩放";
            this.buttonScale.Tooltip = "缩放";
            this.buttonScale.Click += new System.EventHandler(this.buttonScale_Click);
            // 
            // comboItem1
            // 
            this.comboItem1.Text = "选择";
            // 
            // comboItem2
            // 
            this.comboItem2.Text = "移动";
            // 
            // comboItem3
            // 
            this.comboItem3.Text = "旋转";
            // 
            // comboItem4
            // 
            this.comboItem4.Text = "缩放";
            // 
            // mdiClient1
            // 
            this.mdiClient1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mdiClient1.Location = new System.Drawing.Point(0, 86);
            this.mdiClient1.Name = "mdiClient1";
            this.mdiClient1.Size = new System.Drawing.Size(1103, 374);
            this.mdiClient1.TabIndex = 17;
            // 
            // RegionSplitBut
            // 
            this.RegionSplitBut.ImagePaddingHorizontal = 8;
            this.RegionSplitBut.Name = "RegionSplitBut";
            this.RegionSplitBut.Text = "区域分块";
            this.RegionSplitBut.Click += new System.EventHandler(this.RegionSplitBut_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1103, 485);
            this.Controls.Add(this.tabStrip1);
            this.Controls.Add(this.barRightDockSite);
            this.Controls.Add(this.barTopDockSite);
            this.Controls.Add(this.barBottomDockSite);
            this.Controls.Add(this.barLeftDockSite);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.bar1);
            this.Controls.Add(this.mdiClient1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.Name = "MainForm";
            this.Text = "剑网3 策划场景编辑器";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.MdiChildActivate += new System.EventHandler(this.MainForm_MdiChildActivate);
            ((System.ComponentModel.ISupportInitialize)(this.barStandard)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.dockSite3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.mainmenu)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barTool)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonItem buttonSave;
        private DevComponents.DotNetBar.Bar barStandard;
        private DevComponents.DotNetBar.CustomizeItem item_139;
        private DevComponents.DotNetBar.TabStrip tabStrip1;
        private DevComponents.DotNetBar.DockContainerItem dockSearchResults;
        private DevComponents.DotNetBar.LabelItem labelStatus;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.DockSite barRightDockSite;
        private DevComponents.DotNetBar.DockSite barTopDockSite;
        private DevComponents.DotNetBar.DockSite barBottomDockSite;
        public DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
        private DevComponents.DotNetBar.DockSite barLeftDockSite;
        private DevComponents.DotNetBar.DockSite dockSite4;
        private DevComponents.DotNetBar.DockSite dockSite1;
        private DevComponents.DotNetBar.DockSite dockSite2;
        private DevComponents.DotNetBar.DockSite dockSite3;
        private System.Windows.Forms.MdiClient mdiClient1;
        private DevComponents.DotNetBar.DockContainerItem dockContainerItem1;
        private DevComponents.DotNetBar.ButtonItem buttonOpen;
        public System.Windows.Forms.ImageList imageList1;
        private DevComponents.DotNetBar.SliderItem sliderItem1;
        private DevComponents.DotNetBar.ItemContainer itemContainer1;
        private DevComponents.DotNetBar.LabelItem labelItem1;
        private DevComponents.DotNetBar.TextBoxItem cameraLocationBox;
        private DevComponents.DotNetBar.ButtonItem buttonItem3;
        private DevComponents.DotNetBar.CheckBoxItem checkBoxItem1;
        private DevComponents.DotNetBar.LabelItem labelCameraDis;
        private DevComponents.DotNetBar.TextBoxItem textBoxCameraDis;
        private DevComponents.DotNetBar.ButtonItem buttonItem4;
        private DevComponents.DotNetBar.Bar mainmenu;
        private DevComponents.DotNetBar.ButtonItem bFile;
        private DevComponents.DotNetBar.ButtonItem bOpen;
        private DevComponents.DotNetBar.ButtonItem bSave;
        private DevComponents.DotNetBar.ButtonItem bExit;
        private DevComponents.DotNetBar.Bar barTool;
        private DevComponents.DotNetBar.ButtonItem bPMode;
        private DevComponents.DotNetBar.ButtonItem bTopView;
        private DevComponents.DotNetBar.LabelItem lObjectOperateion;
        private DevComponents.Editors.ComboItem comboItem1;
        private DevComponents.Editors.ComboItem comboItem2;
        private DevComponents.Editors.ComboItem comboItem3;
        private DevComponents.Editors.ComboItem comboItem4;
        private DevComponents.DotNetBar.ButtonItem bFiltSelectObject;
        private DevComponents.DotNetBar.ButtonItem buttonCopy;
        private DevComponents.DotNetBar.ButtonItem buttonDelete;
        private DevComponents.DotNetBar.ButtonItem bEdit;
        private DevComponents.DotNetBar.ButtonItem bCopy;
        private DevComponents.DotNetBar.ButtonItem bDelete;
        private DevComponents.DotNetBar.CheckBoxItem checkShowPoly;
        private DevComponents.DotNetBar.CheckBoxItem checkToggleShowProfile;
        private DevComponents.DotNetBar.ButtonItem buttonFullScreen;
        private DevComponents.DotNetBar.ButtonItem buttonSelect;
        private DevComponents.DotNetBar.ButtonItem buttonMove;
        private DevComponents.DotNetBar.ButtonItem buttonRotate;
        private DevComponents.DotNetBar.ButtonItem buttonScale;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem CreateImageBut;
        private DevComponents.DotNetBar.ButtonItem RegionSplitBut;
    }
}

