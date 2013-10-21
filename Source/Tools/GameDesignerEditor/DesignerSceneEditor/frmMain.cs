using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using DevComponents.DotNetBar;

namespace Notepad
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
    public class frmMain : System.Windows.Forms.Form//DevComponents.DotNetBar.Office2007RibbonForm
	{
		#region Private Form Variables
		public DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
		private DevComponents.DotNetBar.DockSite barTopDockSite;
		private DevComponents.DotNetBar.DockSite barBottomDockSite;
		private DevComponents.DotNetBar.DockSite barLeftDockSite;
		private DevComponents.DotNetBar.DockSite barRightDockSite;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.MdiClient mdiClient1;
		private System.Windows.Forms.OpenFileDialog openFileDialog1;
		private System.Windows.Forms.SaveFileDialog saveFileDialog1;
		private System.Windows.Forms.Timer timer1;
		private DevComponents.DotNetBar.TabStrip tabStrip1;
		private System.ComponentModel.IContainer components;
		private BalloonSearch m_Search=null;
		private Balloon m_InfoBalloon=null;
		private DevComponents.DotNetBar.Bar bar1;
		private DevComponents.DotNetBar.LabelItem labelStatus;
		internal DevComponents.DotNetBar.LabelItem labelPosition;
		private DevComponents.DotNetBar.ProgressBarItem itemProgressBar;
		private DevComponents.DotNetBar.DockSite dockSite1;
		private DevComponents.DotNetBar.DockSite dockSite2;
		private DevComponents.DotNetBar.DockSite dockSite3;
		private DevComponents.DotNetBar.DockSite dockSite4;
		private DevComponents.DotNetBar.Bar mainmenu;
		private DevComponents.DotNetBar.ButtonItem bFile;
		private DevComponents.DotNetBar.ButtonItem NewDocument;
		private DevComponents.DotNetBar.ButtonItem buttonItem1;
		private DevComponents.DotNetBar.ButtonItem buttonItem2;
		private DevComponents.DotNetBar.ButtonItem PrintPreview;
		private DevComponents.DotNetBar.ButtonItem Print;
		private DevComponents.DotNetBar.ButtonItem ExitApplication;
		private DevComponents.DotNetBar.ButtonItem bEdit;
		private DevComponents.DotNetBar.ButtonItem bUndo;
		public DevComponents.DotNetBar.ButtonItem bCut;
		public DevComponents.DotNetBar.ButtonItem bCopy;
		public DevComponents.DotNetBar.ButtonItem bPaste;
		public DevComponents.DotNetBar.ButtonItem bDelete;
		public DevComponents.DotNetBar.ButtonItem bSelectAll;
		public DevComponents.DotNetBar.ButtonItem Find;
		public DevComponents.DotNetBar.ButtonItem bFindNext;
		public DevComponents.DotNetBar.ButtonItem bReplace;
		private DevComponents.DotNetBar.ButtonItem bFormat;
		public DevComponents.DotNetBar.ButtonItem bBold;
		public DevComponents.DotNetBar.ButtonItem bItalic;
		public DevComponents.DotNetBar.ButtonItem bUnderline;
		public DevComponents.DotNetBar.ButtonItem bStrikethrough;
		public DevComponents.DotNetBar.ButtonItem bAlignLeft;
		public DevComponents.DotNetBar.ButtonItem bAlignCenter;
		public DevComponents.DotNetBar.ButtonItem bAlignRight;
		private DevComponents.DotNetBar.ButtonItem bWindow;
		private DevComponents.DotNetBar.ButtonItem WindowNew;
		private DevComponents.DotNetBar.ButtonItem WindowArrangeAll;
		private DevComponents.DotNetBar.MdiWindowListItem item_236;
		private DevComponents.DotNetBar.ButtonItem bHelp;
		private DevComponents.DotNetBar.ButtonItem bAbout;
		private DevComponents.DotNetBar.ButtonItem bChangeStyle;
		private DevComponents.DotNetBar.ButtonItem bThemes;
		private DevComponents.DotNetBar.CustomizeItem item_139;
		private DevComponents.DotNetBar.ButtonItem buttonItem9;
		private DevComponents.DotNetBar.ButtonItem buttonItem10;
		private DevComponents.DotNetBar.ButtonItem buttonItem11;
		private DevComponents.DotNetBar.ButtonItem buttonItem12;
		private DevComponents.DotNetBar.CustomizeItem item_140;
		private DevComponents.DotNetBar.ButtonItem buttonItem14;
		private DevComponents.DotNetBar.ButtonItem buttonItem15;
		private DevComponents.DotNetBar.ButtonItem buttonItem16;
		private DevComponents.DotNetBar.ButtonItem buttonItem17;
		private DevComponents.DotNetBar.ButtonItem buttonItem18;
		private DevComponents.DotNetBar.ButtonItem buttonItem19;
		private DevComponents.DotNetBar.ButtonItem buttonItem20;
		private DevComponents.DotNetBar.CustomizeItem item_141;
		private DevComponents.DotNetBar.Bar barTaskList;
		private DevComponents.DotNetBar.DockContainerItem dockTaskList;
		private DevComponents.DotNetBar.DockContainerItem dockSearchResults;
		private DevComponents.DotNetBar.PanelDockContainer panelDockContainer1;
		private DevComponents.DotNetBar.PanelDockContainer panelDockContainer2;
		private DevComponents.DotNetBar.Bar windowlist;
		private DevComponents.DotNetBar.MdiWindowListItem item_162;
		private DevComponents.DotNetBar.Bar barTaskPane;
		private DevComponents.DotNetBar.DockContainerItem TaskPane1;
		private DevComponents.DotNetBar.DockContainerItem TaskPane2;
		private DevComponents.DotNetBar.PanelDockContainer panelDockContainer3;
		private DevComponents.DotNetBar.PanelDockContainer panelDockContainer4;
		private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
		private DevComponents.DotNetBar.ButtonItem bEditPopup;
		private DevComponents.DotNetBar.ButtonItem buttonItem22;
		private DevComponents.DotNetBar.ButtonItem buttonItem23;
		private DevComponents.DotNetBar.ButtonItem buttonItem24;
		private DevComponents.DotNetBar.ButtonItem buttonItem25;
		private DevComponents.DotNetBar.ButtonItem bTaskListMenu;
		private DevComponents.DotNetBar.ButtonItem item_477;
		private DevComponents.DotNetBar.ButtonItem item_504;
		private DevComponents.DotNetBar.ButtonItem item_531;
		private DevComponents.DotNetBar.ButtonItem bTabContext;
		private DevComponents.DotNetBar.ButtonItem bTabColor;
		private DevComponents.DotNetBar.ButtonItem bDockContext;
		private System.Windows.Forms.ListView listView1;
		private System.Windows.Forms.ColumnHeader columnHeader1;
		private System.Windows.Forms.ColumnHeader columnHeader2;
		private System.Windows.Forms.TextBox textBox1;
		private DevComponents.DotNetBar.ButtonItem buttonItem26;
		private DevComponents.DotNetBar.ExplorerBarGroupItem explorerBarGroupItem2;
		private DevComponents.DotNetBar.ButtonItem buttonItem27;
		private DevComponents.DotNetBar.ButtonItem buttonItem28;
		private DevComponents.DotNetBar.ButtonItem buttonItem29;
		private DevComponents.DotNetBar.ButtonItem buttonItem30;
		private DevComponents.DotNetBar.ButtonItem buttonItem31;
		private DevComponents.DotNetBar.ExplorerBarGroupItem explorerBarGroupItem1;
		private DevComponents.DotNetBar.ButtonItem buttonItem32;
		private DevComponents.DotNetBar.ExplorerBar explorerBar1;
		private System.Windows.Forms.ListBox listBox1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.TextBox textBox2;
		private System.Windows.Forms.Label label1;
		private DevComponents.DotNetBar.ButtonItem cmdNewDocument;
		private DevComponents.DotNetBar.ButtonItem cmdOpenDocument;
		private DevComponents.DotNetBar.ButtonItem cmdSaveDocument;
		private DevComponents.DotNetBar.ButtonItem cmdPrintPreview;
		private DevComponents.DotNetBar.ButtonItem cmdPrint;
		private DevComponents.DotNetBar.ButtonItem cmdSaveDocumentAs;
		private DevComponents.DotNetBar.ButtonItem cmdPageSetup;
		private DevComponents.DotNetBar.ButtonItem cmdCloseDocument;
		private DevComponents.DotNetBar.ButtonItem cmdStyleOffice2003;
		private DevComponents.DotNetBar.ButtonItem cmdStyleOffice2007Blue;
		private DevComponents.DotNetBar.ButtonItem cmdStyleOfficeXP;
		private DevComponents.DotNetBar.ButtonItem cmdStyleVS2005;
		private DevComponents.DotNetBar.ButtonItem cmdTabbedMdi;
		private DevComponents.DotNetBar.Bar barStandard;
		private DevComponents.DotNetBar.Bar barEdit;
		private DevComponents.DotNetBar.Bar barFormat;
		private DevComponents.DotNetBar.ButtonItem bFind;
		private DevComponents.DotNetBar.ColorPickerDropDown colorPickerDropDown1;
		public DevComponents.DotNetBar.ColorPickerDropDown bTextColor;
		private DevComponents.DotNetBar.ButtonItem cmdContextSave;
		private DevComponents.DotNetBar.ButtonItem cmdContextClose;
		private System.Windows.Forms.Timer timerInfoBallon;
		#endregion

		#region Form Constructor, Dispose
		public frmMain()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			// When new forms are added to the MDI Client the event handler
			// will sync the bottom bar which if displayed shows active MDI Children
			mdiClient1.ControlAdded+=new ControlEventHandler(MdiClientControlAddRemove);
			mdiClient1.ControlRemoved+=new ControlEventHandler(MdiClientControlAddRemove);

			// Assign optional "Customize" menu to the bar that contains this item
			barTaskList.CustomizeMenu=bTaskListMenu;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}
		#endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
            "!",
            "Description of the task number 1"}, -1, System.Drawing.SystemColors.WindowText, System.Drawing.Color.WhiteSmoke, new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
            "",
            "Task number 2 "}, -1, System.Drawing.SystemColors.WindowText, System.Drawing.Color.WhiteSmoke, new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0))));
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.mdiClient1 = new System.Windows.Forms.MdiClient();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.barLeftDockSite = new DevComponents.DotNetBar.DockSite();
            this.barTopDockSite = new DevComponents.DotNetBar.DockSite();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.dotNetBarManager1 = new DevComponents.DotNetBar.DotNetBarManager(this.components);
            this.barBottomDockSite = new DevComponents.DotNetBar.DockSite();
            this.barTaskList = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer1 = new DevComponents.DotNetBar.PanelDockContainer();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.panelDockContainer2 = new DevComponents.DotNetBar.PanelDockContainer();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.dockTaskList = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSearchResults = new DevComponents.DotNetBar.DockContainerItem();
            this.barRightDockSite = new DevComponents.DotNetBar.DockSite();
            this.barTaskPane = new DevComponents.DotNetBar.Bar();
            this.panelDockContainer4 = new DevComponents.DotNetBar.PanelDockContainer();
            this.explorerBar1 = new DevComponents.DotNetBar.ExplorerBar();
            this.explorerBarGroupItem1 = new DevComponents.DotNetBar.ExplorerBarGroupItem();
            this.buttonItem32 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem31 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem26 = new DevComponents.DotNetBar.ButtonItem();
            this.explorerBarGroupItem2 = new DevComponents.DotNetBar.ExplorerBarGroupItem();
            this.buttonItem27 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem28 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem29 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem30 = new DevComponents.DotNetBar.ButtonItem();
            this.panelDockContainer3 = new DevComponents.DotNetBar.PanelDockContainer();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.TaskPane1 = new DevComponents.DotNetBar.DockContainerItem();
            this.TaskPane2 = new DevComponents.DotNetBar.DockContainerItem();
            this.dockSite4 = new DevComponents.DotNetBar.DockSite();
            this.windowlist = new DevComponents.DotNetBar.Bar();
            this.item_162 = new DevComponents.DotNetBar.MdiWindowListItem();
            this.dockSite1 = new DevComponents.DotNetBar.DockSite();
            this.dockSite2 = new DevComponents.DotNetBar.DockSite();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.mainmenu = new DevComponents.DotNetBar.Bar();
            this.bFile = new DevComponents.DotNetBar.ButtonItem();
            this.NewDocument = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.cmdCloseDocument = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.cmdSaveDocumentAs = new DevComponents.DotNetBar.ButtonItem();
            this.cmdPageSetup = new DevComponents.DotNetBar.ButtonItem();
            this.PrintPreview = new DevComponents.DotNetBar.ButtonItem();
            this.Print = new DevComponents.DotNetBar.ButtonItem();
            this.ExitApplication = new DevComponents.DotNetBar.ButtonItem();
            this.bEdit = new DevComponents.DotNetBar.ButtonItem();
            this.bUndo = new DevComponents.DotNetBar.ButtonItem();
            this.bCut = new DevComponents.DotNetBar.ButtonItem();
            this.bCopy = new DevComponents.DotNetBar.ButtonItem();
            this.bPaste = new DevComponents.DotNetBar.ButtonItem();
            this.bDelete = new DevComponents.DotNetBar.ButtonItem();
            this.bSelectAll = new DevComponents.DotNetBar.ButtonItem();
            this.Find = new DevComponents.DotNetBar.ButtonItem();
            this.bFindNext = new DevComponents.DotNetBar.ButtonItem();
            this.bReplace = new DevComponents.DotNetBar.ButtonItem();
            this.bFormat = new DevComponents.DotNetBar.ButtonItem();
            this.bBold = new DevComponents.DotNetBar.ButtonItem();
            this.bItalic = new DevComponents.DotNetBar.ButtonItem();
            this.bUnderline = new DevComponents.DotNetBar.ButtonItem();
            this.bStrikethrough = new DevComponents.DotNetBar.ButtonItem();
            this.bAlignLeft = new DevComponents.DotNetBar.ButtonItem();
            this.bAlignCenter = new DevComponents.DotNetBar.ButtonItem();
            this.bAlignRight = new DevComponents.DotNetBar.ButtonItem();
            this.bTextColor = new DevComponents.DotNetBar.ColorPickerDropDown();
            this.bWindow = new DevComponents.DotNetBar.ButtonItem();
            this.WindowNew = new DevComponents.DotNetBar.ButtonItem();
            this.WindowArrangeAll = new DevComponents.DotNetBar.ButtonItem();
            this.item_236 = new DevComponents.DotNetBar.MdiWindowListItem();
            this.bHelp = new DevComponents.DotNetBar.ButtonItem();
            this.bAbout = new DevComponents.DotNetBar.ButtonItem();
            this.bChangeStyle = new DevComponents.DotNetBar.ButtonItem();
            this.cmdStyleOffice2003 = new DevComponents.DotNetBar.ButtonItem();
            this.cmdStyleVS2005 = new DevComponents.DotNetBar.ButtonItem();
            this.cmdStyleOfficeXP = new DevComponents.DotNetBar.ButtonItem();
            this.cmdStyleOffice2007Blue = new DevComponents.DotNetBar.ButtonItem();
            this.barStandard = new DevComponents.DotNetBar.Bar();
            this.cmdNewDocument = new DevComponents.DotNetBar.ButtonItem();
            this.cmdOpenDocument = new DevComponents.DotNetBar.ButtonItem();
            this.cmdSaveDocument = new DevComponents.DotNetBar.ButtonItem();
            this.cmdPrintPreview = new DevComponents.DotNetBar.ButtonItem();
            this.cmdPrint = new DevComponents.DotNetBar.ButtonItem();
            this.bThemes = new DevComponents.DotNetBar.ButtonItem();
            this.cmdTabbedMdi = new DevComponents.DotNetBar.ButtonItem();
            this.item_139 = new DevComponents.DotNetBar.CustomizeItem();
            this.barEdit = new DevComponents.DotNetBar.Bar();
            this.buttonItem9 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem10 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem11 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem12 = new DevComponents.DotNetBar.ButtonItem();
            this.bFind = new DevComponents.DotNetBar.ButtonItem();
            this.item_140 = new DevComponents.DotNetBar.CustomizeItem();
            this.barFormat = new DevComponents.DotNetBar.Bar();
            this.buttonItem14 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem15 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem16 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem17 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem18 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem19 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem20 = new DevComponents.DotNetBar.ButtonItem();
            this.colorPickerDropDown1 = new DevComponents.DotNetBar.ColorPickerDropDown();
            this.item_141 = new DevComponents.DotNetBar.CustomizeItem();
            this.tabStrip1 = new DevComponents.DotNetBar.TabStrip();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timerInfoBallon = new System.Windows.Forms.Timer(this.components);
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.labelStatus = new DevComponents.DotNetBar.LabelItem();
            this.labelPosition = new DevComponents.DotNetBar.LabelItem();
            this.itemProgressBar = new DevComponents.DotNetBar.ProgressBarItem();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.bDockContext = new DevComponents.DotNetBar.ButtonItem();
            this.bTabContext = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextSave = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextClose = new DevComponents.DotNetBar.ButtonItem();
            this.bTabColor = new DevComponents.DotNetBar.ButtonItem();
            this.bEditPopup = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem22 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem23 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem24 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem25 = new DevComponents.DotNetBar.ButtonItem();
            this.bTaskListMenu = new DevComponents.DotNetBar.ButtonItem();
            this.item_477 = new DevComponents.DotNetBar.ButtonItem();
            this.item_504 = new DevComponents.DotNetBar.ButtonItem();
            this.item_531 = new DevComponents.DotNetBar.ButtonItem();
            this.barBottomDockSite.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.barTaskList)).BeginInit();
            this.barTaskList.SuspendLayout();
            this.panelDockContainer1.SuspendLayout();
            this.panelDockContainer2.SuspendLayout();
            this.barRightDockSite.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.barTaskPane)).BeginInit();
            this.barTaskPane.SuspendLayout();
            this.panelDockContainer4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.explorerBar1)).BeginInit();
            this.panelDockContainer3.SuspendLayout();
            this.dockSite4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.windowlist)).BeginInit();
            this.dockSite3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mainmenu)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barStandard)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barEdit)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barFormat)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.DefaultExt = "*.rtf";
            this.openFileDialog1.Filter = "Text Files (*.txt)|*.txt|RTF Files (*.rtf)|*.rtf|All Files(*.*)|*.*";
            this.openFileDialog1.FilterIndex = 2;
            this.openFileDialog1.Title = "Open File";
            // 
            // mdiClient1
            // 
            this.mdiClient1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mdiClient1.Location = new System.Drawing.Point(0, 108);
            this.mdiClient1.Name = "mdiClient1";
            this.mdiClient1.Size = new System.Drawing.Size(473, 244);
            this.mdiClient1.TabIndex = 5;
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
            this.imageList1.Images.SetKeyName(19, "");
            this.imageList1.Images.SetKeyName(20, "");
            this.imageList1.Images.SetKeyName(21, "");
            this.imageList1.Images.SetKeyName(22, "");
            this.imageList1.Images.SetKeyName(23, "");
            // 
            // barLeftDockSite
            // 
            this.barLeftDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barLeftDockSite.Dock = System.Windows.Forms.DockStyle.Left;
            this.barLeftDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barLeftDockSite.Location = new System.Drawing.Point(0, 80);
            this.barLeftDockSite.Name = "barLeftDockSite";
            this.barLeftDockSite.Size = new System.Drawing.Size(0, 272);
            this.barLeftDockSite.TabIndex = 3;
            this.barLeftDockSite.TabStop = false;
            // 
            // barTopDockSite
            // 
            this.barTopDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barTopDockSite.Dock = System.Windows.Forms.DockStyle.Top;
            this.barTopDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.barTopDockSite.Location = new System.Drawing.Point(0, 80);
            this.barTopDockSite.Name = "barTopDockSite";
            this.barTopDockSite.Size = new System.Drawing.Size(704, 0);
            this.barTopDockSite.TabIndex = 1;
            this.barTopDockSite.TabStop = false;
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.DefaultExt = "*.rtf";
            this.saveFileDialog1.FileName = "doc1";
            this.saveFileDialog1.Filter = "Text Files (*.txt)|*.txt|RTF Files (*.rtf)|*.rtf|All Files(*.*)|*.*";
            this.saveFileDialog1.FilterIndex = 2;
            this.saveFileDialog1.Title = "Save File";
            // 
            // dotNetBarManager1
            // 
            this.dotNetBarManager1.BottomDockSite = this.barBottomDockSite;
            this.dotNetBarManager1.DefinitionName = "";
            this.dotNetBarManager1.Images = this.imageList1;
            this.dotNetBarManager1.LeftDockSite = this.barLeftDockSite;
            this.dotNetBarManager1.MdiSystemItemVisible = false;
            this.dotNetBarManager1.ParentForm = this;
            this.dotNetBarManager1.RightDockSite = this.barRightDockSite;
            this.dotNetBarManager1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.dotNetBarManager1.ToolbarBottomDockSite = this.dockSite4;
            this.dotNetBarManager1.ToolbarLeftDockSite = this.dockSite1;
            this.dotNetBarManager1.ToolbarRightDockSite = this.dockSite2;
            this.dotNetBarManager1.ToolbarTopDockSite = this.dockSite3;
            this.dotNetBarManager1.TopDockSite = this.barTopDockSite;
            this.dotNetBarManager1.BarClosing += new DevComponents.DotNetBar.DotNetBarManager.BarClosingEventHandler(this.dotNetBarManager1_BarClosing);
            this.dotNetBarManager1.ItemClick += new System.EventHandler(this.BarItemClick);
            this.dotNetBarManager1.PopupContainerLoad += new System.EventHandler(this.LoadPopup);
            this.dotNetBarManager1.MouseLeave += new System.EventHandler(this.dotNetBarManager1_MouseLeave);
            this.dotNetBarManager1.MouseEnter += new System.EventHandler(this.dotNetBarManager1_MouseEnter);
            this.dotNetBarManager1.PopupContainerUnload += new System.EventHandler(this.UnloadPopup);
            this.dotNetBarManager1.DockTabChange += new DevComponents.DotNetBar.DotNetBarManager.DockTabChangeEventHandler(this.dotNetBarManager1_DockTabChange);
            // 
            // barBottomDockSite
            // 
            this.barBottomDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barBottomDockSite.Controls.Add(this.barTaskList);
            this.barBottomDockSite.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.barBottomDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.barTaskList, 704, 128)))}, DevComponents.DotNetBar.eOrientation.Vertical);
            this.barBottomDockSite.Location = new System.Drawing.Point(0, 352);
            this.barBottomDockSite.Name = "barBottomDockSite";
            this.barBottomDockSite.Size = new System.Drawing.Size(704, 131);
            this.barBottomDockSite.TabIndex = 2;
            this.barBottomDockSite.TabStop = false;
            // 
            // barTaskList
            // 
            this.barTaskList.AccessibleDescription = "DotNetBar Bar (barTaskList)";
            this.barTaskList.AccessibleName = "DotNetBar Bar";
            this.barTaskList.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barTaskList.CanCustomize = false;
            this.barTaskList.CanHide = true;
            this.contextMenuBar1.SetContextMenuEx(this.barTaskList, this.bDockContext);
            this.barTaskList.Controls.Add(this.panelDockContainer1);
            this.barTaskList.Controls.Add(this.panelDockContainer2);
            this.barTaskList.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Caption;
            this.barTaskList.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.dockTaskList,
            this.dockSearchResults});
            this.barTaskList.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.barTaskList.Location = new System.Drawing.Point(0, 3);
            this.barTaskList.Name = "barTaskList";
            this.barTaskList.SelectedDockTab = 0;
            this.barTaskList.Size = new System.Drawing.Size(704, 128);
            this.barTaskList.Stretch = true;
            this.barTaskList.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barTaskList.TabIndex = 0;
            this.barTaskList.TabNavigation = true;
            this.barTaskList.TabStop = false;
            this.barTaskList.Text = "Dockable Window";
            // 
            // panelDockContainer1
            // 
            this.panelDockContainer1.Controls.Add(this.listView1);
            this.panelDockContainer1.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer1.Name = "panelDockContainer1";
            this.panelDockContainer1.Size = new System.Drawing.Size(698, 77);
            this.panelDockContainer1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer1.Style.GradientAngle = 90;
            this.panelDockContainer1.TabIndex = 1;
            // 
            // listView1
            // 
            this.listView1.BackColor = System.Drawing.SystemColors.Window;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.HideSelection = false;
            this.listView1.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2});
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(698, 77);
            this.listView1.TabIndex = 2;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "!";
            this.columnHeader1.Width = 18;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Description";
            this.columnHeader2.Width = 110;
            // 
            // panelDockContainer2
            // 
            this.panelDockContainer2.Controls.Add(this.textBox1);
            this.panelDockContainer2.Location = new System.Drawing.Point(3, 23);
            this.panelDockContainer2.Name = "panelDockContainer2";
            this.panelDockContainer2.Size = new System.Drawing.Size(698, 77);
            this.panelDockContainer2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer2.Style.GradientAngle = 90;
            this.panelDockContainer2.TabIndex = 2;
            // 
            // textBox1
            // 
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Location = new System.Drawing.Point(0, 0);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(698, 16);
            this.textBox1.TabIndex = 0;
            // 
            // dockTaskList
            // 
            this.dockTaskList.Control = this.panelDockContainer1;
            this.dockTaskList.DefaultFloatingSize = new System.Drawing.Size(256, 196);
            this.dockTaskList.GlobalItem = true;
            this.dockTaskList.GlobalName = "dockTaskList";
            this.dockTaskList.Image = ((System.Drawing.Image)(resources.GetObject("dockTaskList.Image")));
            this.dockTaskList.Name = "dockTaskList";
            this.dockTaskList.Text = "Task ListView";
            // 
            // dockSearchResults
            // 
            this.dockSearchResults.Control = this.panelDockContainer2;
            this.dockSearchResults.DefaultFloatingSize = new System.Drawing.Size(256, 196);
            this.dockSearchResults.GlobalItem = true;
            this.dockSearchResults.GlobalName = "dockSearchResults";
            this.dockSearchResults.Image = ((System.Drawing.Image)(resources.GetObject("dockSearchResults.Image")));
            this.dockSearchResults.Name = "dockSearchResults";
            this.dockSearchResults.Text = "Search Results";
            // 
            // barRightDockSite
            // 
            this.barRightDockSite.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.barRightDockSite.Controls.Add(this.barTaskPane);
            this.barRightDockSite.Dock = System.Windows.Forms.DockStyle.Right;
            this.barRightDockSite.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer(new DevComponents.DotNetBar.DocumentBaseContainer[] {
            ((DevComponents.DotNetBar.DocumentBaseContainer)(new DevComponents.DotNetBar.DocumentBarContainer(this.barTaskPane, 228, 272)))}, DevComponents.DotNetBar.eOrientation.Horizontal);
            this.barRightDockSite.Location = new System.Drawing.Point(473, 80);
            this.barRightDockSite.Name = "barRightDockSite";
            this.barRightDockSite.Size = new System.Drawing.Size(231, 272);
            this.barRightDockSite.TabIndex = 4;
            this.barRightDockSite.TabStop = false;
            // 
            // barTaskPane
            // 
            this.barTaskPane.AccessibleDescription = "DotNetBar Bar (barTaskPane)";
            this.barTaskPane.AccessibleName = "DotNetBar Bar";
            this.barTaskPane.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barTaskPane.AutoSyncBarCaption = true;
            this.barTaskPane.Controls.Add(this.panelDockContainer4);
            this.barTaskPane.Controls.Add(this.panelDockContainer3);
            this.barTaskPane.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.CaptionTaskPane;
            this.barTaskPane.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.TaskPane1,
            this.TaskPane2});
            this.barTaskPane.LayoutType = DevComponents.DotNetBar.eLayoutType.DockContainer;
            this.barTaskPane.Location = new System.Drawing.Point(3, 0);
            this.barTaskPane.Name = "barTaskPane";
            this.barTaskPane.SelectedDockTab = 0;
            this.barTaskPane.Size = new System.Drawing.Size(228, 272);
            this.barTaskPane.Stretch = true;
            this.barTaskPane.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barTaskPane.TabIndex = 0;
            this.barTaskPane.TabStop = false;
            this.barTaskPane.Text = "Getting Started";
            // 
            // panelDockContainer4
            // 
            this.panelDockContainer4.Controls.Add(this.explorerBar1);
            this.panelDockContainer4.Location = new System.Drawing.Point(3, 26);
            this.panelDockContainer4.Name = "panelDockContainer4";
            this.panelDockContainer4.Size = new System.Drawing.Size(222, 243);
            this.panelDockContainer4.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer4.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer4.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer4.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer4.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer4.Style.GradientAngle = 90;
            this.panelDockContainer4.TabIndex = 2;
            // 
            // explorerBar1
            // 
            this.explorerBar1.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.explorerBar1.BackColor = System.Drawing.SystemColors.Control;
            // 
            // 
            // 
            this.explorerBar1.BackStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.explorerBar1.BackStyle.BackColorGradientAngle = 90;
            this.explorerBar1.BackStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.explorerBar1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.explorerBar1.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            this.explorerBar1.GroupImages = null;
            this.explorerBar1.Groups.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.explorerBarGroupItem1,
            this.explorerBarGroupItem2});
            this.explorerBar1.Images = null;
            this.explorerBar1.Location = new System.Drawing.Point(0, 0);
            this.explorerBar1.Name = "explorerBar1";
            this.explorerBar1.Size = new System.Drawing.Size(222, 243);
            this.explorerBar1.TabIndex = 4;
            this.explorerBar1.Text = "explorerBar1";
            // 
            // explorerBarGroupItem1
            // 
            // 
            // 
            // 
            this.explorerBarGroupItem1.BackStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.explorerBarGroupItem1.BackStyle.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem1.BackStyle.BorderColor = System.Drawing.Color.White;
            this.explorerBarGroupItem1.BackStyle.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem1.BackStyle.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem1.BackStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem1.ExpandBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(174)))), ((int)(((byte)(182)))), ((int)(((byte)(216)))));
            this.explorerBarGroupItem1.ExpandButtonVisible = false;
            this.explorerBarGroupItem1.Expanded = true;
            this.explorerBarGroupItem1.ExpandForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(60)))), ((int)(((byte)(165)))));
            this.explorerBarGroupItem1.ExpandHotBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(174)))), ((int)(((byte)(182)))), ((int)(((byte)(216)))));
            this.explorerBarGroupItem1.ExpandHotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.explorerBarGroupItem1.HeaderExpands = false;
            this.explorerBarGroupItem1.Name = "explorerBarGroupItem1";
            this.explorerBarGroupItem1.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem32,
            this.buttonItem31,
            this.buttonItem26});
            this.explorerBarGroupItem1.Text = "Assistance";
            // 
            // 
            // 
            this.explorerBarGroupItem1.TitleHotStyle.BackColor = System.Drawing.Color.White;
            this.explorerBarGroupItem1.TitleHotStyle.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(199)))), ((int)(((byte)(211)))), ((int)(((byte)(247)))));
            this.explorerBarGroupItem1.TitleHotStyle.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.explorerBarGroupItem1.TitleHotStyle.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            // 
            // 
            // 
            this.explorerBarGroupItem1.TitleStyle.BackColor = System.Drawing.Color.White;
            this.explorerBarGroupItem1.TitleStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.explorerBarGroupItem1.TitleStyle.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.explorerBarGroupItem1.TitleStyle.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            // 
            // buttonItem32
            // 
            this.buttonItem32.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem32.HotFontUnderline = true;
            this.buttonItem32.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem32.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem32.Name = "buttonItem32";
            this.buttonItem32.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem32.Text = " Connect to DevComponents Online";
            // 
            // buttonItem31
            // 
            this.buttonItem31.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem31.HotFontUnderline = true;
            this.buttonItem31.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem31.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem31.Name = "buttonItem31";
            this.buttonItem31.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem31.Text = " Get latest news about using this product";
            // 
            // buttonItem26
            // 
            this.buttonItem26.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem26.HotFontUnderline = true;
            this.buttonItem26.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem26.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem26.Name = "buttonItem26";
            this.buttonItem26.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem26.Text = " Automatically update this list from web";
            // 
            // explorerBarGroupItem2
            // 
            // 
            // 
            // 
            this.explorerBarGroupItem2.BackStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.explorerBarGroupItem2.BackStyle.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem2.BackStyle.BorderColor = System.Drawing.Color.White;
            this.explorerBarGroupItem2.BackStyle.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem2.BackStyle.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem2.BackStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.explorerBarGroupItem2.ExpandBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(174)))), ((int)(((byte)(182)))), ((int)(((byte)(216)))));
            this.explorerBarGroupItem2.ExpandButtonVisible = false;
            this.explorerBarGroupItem2.Expanded = true;
            this.explorerBarGroupItem2.ExpandForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(60)))), ((int)(((byte)(165)))));
            this.explorerBarGroupItem2.ExpandHotBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(174)))), ((int)(((byte)(182)))), ((int)(((byte)(216)))));
            this.explorerBarGroupItem2.ExpandHotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.explorerBarGroupItem2.HeaderExpands = false;
            this.explorerBarGroupItem2.Name = "explorerBarGroupItem2";
            this.explorerBarGroupItem2.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem27,
            this.buttonItem28,
            this.buttonItem29,
            this.buttonItem30});
            this.explorerBarGroupItem2.Text = "Open";
            // 
            // 
            // 
            this.explorerBarGroupItem2.TitleHotStyle.BackColor = System.Drawing.Color.White;
            this.explorerBarGroupItem2.TitleHotStyle.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(199)))), ((int)(((byte)(211)))), ((int)(((byte)(247)))));
            this.explorerBarGroupItem2.TitleHotStyle.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.explorerBarGroupItem2.TitleHotStyle.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            // 
            // 
            // 
            this.explorerBarGroupItem2.TitleStyle.BackColor = System.Drawing.Color.White;
            this.explorerBarGroupItem2.TitleStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.explorerBarGroupItem2.TitleStyle.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.World);
            this.explorerBarGroupItem2.TitleStyle.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            // 
            // buttonItem27
            // 
            this.buttonItem27.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem27.HotFontUnderline = true;
            this.buttonItem27.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem27.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem27.Name = "buttonItem27";
            this.buttonItem27.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem27.Text = "Document1.doc";
            // 
            // buttonItem28
            // 
            this.buttonItem28.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem28.HotFontUnderline = true;
            this.buttonItem28.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem28.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem28.Name = "buttonItem28";
            this.buttonItem28.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem28.Text = "My Document 2.doc";
            // 
            // buttonItem29
            // 
            this.buttonItem29.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem29.HotFontUnderline = true;
            this.buttonItem29.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem29.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem29.Name = "buttonItem29";
            this.buttonItem29.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem29.Text = "Report.doc";
            // 
            // buttonItem30
            // 
            this.buttonItem30.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(33)))), ((int)(((byte)(93)))), ((int)(((byte)(198)))));
            this.buttonItem30.HotFontUnderline = true;
            this.buttonItem30.HotForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(66)))), ((int)(((byte)(142)))), ((int)(((byte)(255)))));
            this.buttonItem30.HotTrackingStyle = DevComponents.DotNetBar.eHotTrackingStyle.None;
            this.buttonItem30.Name = "buttonItem30";
            this.buttonItem30.PopupSide = DevComponents.DotNetBar.ePopupSide.Left;
            this.buttonItem30.Text = "End of year account.doc";
            // 
            // panelDockContainer3
            // 
            this.panelDockContainer3.Controls.Add(this.listBox1);
            this.panelDockContainer3.Controls.Add(this.label2);
            this.panelDockContainer3.Controls.Add(this.button1);
            this.panelDockContainer3.Controls.Add(this.textBox2);
            this.panelDockContainer3.Controls.Add(this.label1);
            this.panelDockContainer3.Location = new System.Drawing.Point(3, 26);
            this.panelDockContainer3.Name = "panelDockContainer3";
            this.panelDockContainer3.Size = new System.Drawing.Size(222, 243);
            this.panelDockContainer3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelDockContainer3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelDockContainer3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelDockContainer3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelDockContainer3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelDockContainer3.Style.GradientAngle = 90;
            this.panelDockContainer3.TabIndex = 1;
            // 
            // listBox1
            // 
            this.listBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.listBox1.IntegralHeight = false;
            this.listBox1.ItemHeight = 17;
            this.listBox1.Location = new System.Drawing.Point(8, 78);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(205, 163);
            this.listBox1.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(8, 60);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(96, 18);
            this.label2.TabIndex = 8;
            this.label2.Text = "Search results:";
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.BackColor = System.Drawing.Color.Transparent;
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button1.ForeColor = System.Drawing.Color.Transparent;
            this.button1.Image = ((System.Drawing.Image)(resources.GetObject("button1.Image")));
            this.button1.Location = new System.Drawing.Point(184, 26);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(25, 22);
            this.button1.TabIndex = 7;
            this.button1.UseVisualStyleBackColor = false;
            // 
            // textBox2
            // 
            this.textBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.textBox2.Location = new System.Drawing.Point(8, 26);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(169, 23);
            this.textBox2.TabIndex = 6;
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(8, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(96, 17);
            this.label1.TabIndex = 5;
            this.label1.Text = "Search for:";
            // 
            // TaskPane1
            // 
            this.TaskPane1.Control = this.panelDockContainer4;
            this.TaskPane1.DefaultFloatingSize = new System.Drawing.Size(193, 290);
            this.TaskPane1.GlobalItem = true;
            this.TaskPane1.GlobalName = "TaskPane1";
            this.TaskPane1.Name = "TaskPane1";
            this.TaskPane1.Text = "Getting Started";
            // 
            // TaskPane2
            // 
            this.TaskPane2.Control = this.panelDockContainer3;
            this.TaskPane2.DefaultFloatingSize = new System.Drawing.Size(193, 290);
            this.TaskPane2.GlobalItem = true;
            this.TaskPane2.GlobalName = "TaskPane2";
            this.TaskPane2.Name = "TaskPane2";
            this.TaskPane2.Text = "Research";
            // 
            // dockSite4
            // 
            this.dockSite4.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite4.Controls.Add(this.windowlist);
            this.dockSite4.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite4.Location = new System.Drawing.Point(0, 483);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(704, 28);
            this.dockSite4.TabIndex = 11;
            this.dockSite4.TabStop = false;
            // 
            // windowlist
            // 
            this.windowlist.AccessibleDescription = "Open Windows (windowlist)";
            this.windowlist.AccessibleName = "Open Windows";
            this.windowlist.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.windowlist.CanDockLeft = false;
            this.windowlist.CanDockRight = false;
            this.windowlist.CanDockTop = false;
            this.windowlist.DockSide = DevComponents.DotNetBar.eDockSide.Bottom;
            this.windowlist.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.item_162});
            this.windowlist.Location = new System.Drawing.Point(0, 0);
            this.windowlist.Name = "windowlist";
            this.windowlist.Size = new System.Drawing.Size(704, 27);
            this.windowlist.Stretch = true;
            this.windowlist.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.windowlist.TabIndex = 0;
            this.windowlist.TabStop = false;
            this.windowlist.Text = "Open Windows";
            this.windowlist.Visible = false;
            // 
            // item_162
            // 
            this.item_162.GlobalName = "item_162";
            this.item_162.Name = "item_162";
            this.item_162.ShowWindowIcons = true;
            this.item_162.Text = "MDI Window List";
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.Location = new System.Drawing.Point(0, 80);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(0, 403);
            this.dockSite1.TabIndex = 8;
            this.dockSite1.TabStop = false;
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.Location = new System.Drawing.Point(704, 80);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 403);
            this.dockSite2.TabIndex = 9;
            this.dockSite2.TabStop = false;
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Controls.Add(this.mainmenu);
            this.dockSite3.Controls.Add(this.barStandard);
            this.dockSite3.Controls.Add(this.barEdit);
            this.dockSite3.Controls.Add(this.barFormat);
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.Location = new System.Drawing.Point(0, 0);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(704, 80);
            this.dockSite3.TabIndex = 10;
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
            this.bFormat,
            this.bWindow,
            this.bHelp,
            this.bChangeStyle});
            this.mainmenu.Location = new System.Drawing.Point(0, 0);
            this.mainmenu.LockDockPosition = true;
            this.mainmenu.MenuBar = true;
            this.mainmenu.Name = "mainmenu";
            this.mainmenu.Size = new System.Drawing.Size(704, 26);
            this.mainmenu.Stretch = true;
            this.mainmenu.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.mainmenu.TabIndex = 0;
            this.mainmenu.TabStop = false;
            this.mainmenu.Text = "Main Menu";
            // 
            // bFile
            // 
            this.bFile.Category = "Main Menu";
            this.bFile.GlobalName = "bFile";
            this.bFile.Name = "bFile";
            this.bFile.PersonalizedMenus = DevComponents.DotNetBar.ePersonalizedMenus.Both;
            this.bFile.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.NewDocument,
            this.buttonItem1,
            this.cmdCloseDocument,
            this.buttonItem2,
            this.cmdSaveDocumentAs,
            this.cmdPageSetup,
            this.PrintPreview,
            this.Print,
            this.ExitApplication});
            this.bFile.Text = "&File";
            // 
            // NewDocument
            // 
            this.NewDocument.Category = "File";
            this.NewDocument.GlobalName = "NewDocument";
            this.NewDocument.ImageIndex = 10;
            this.NewDocument.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.NewDocument.Name = "NewDocument";
            this.NewDocument.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.NewDocument.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlN);
            this.NewDocument.Text = "&New";
            this.NewDocument.Tooltip = "Create new document";
            this.NewDocument.Click += new System.EventHandler(this.cmdNewDocument_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.Category = "File";
            this.buttonItem1.GlobalName = "OpenDocument";
            this.buttonItem1.ImageIndex = 11;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem1.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlO);
            this.buttonItem1.Text = "&Open";
            this.buttonItem1.Tooltip = "Open existing document";
            this.buttonItem1.Click += new System.EventHandler(this.cmdOpenDocument_Click);
            // 
            // cmdCloseDocument
            // 
            this.cmdCloseDocument.Category = "File";
            this.cmdCloseDocument.Enabled = false;
            this.cmdCloseDocument.GlobalName = "CloseDocument";
            this.cmdCloseDocument.ImageIndex = 3;
            this.cmdCloseDocument.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.cmdCloseDocument.Name = "cmdCloseDocument";
            this.cmdCloseDocument.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdCloseDocument.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlC);
            this.cmdCloseDocument.Text = "&Close";
            this.cmdCloseDocument.Tooltip = "Close active document";
            this.cmdCloseDocument.Click += new System.EventHandler(this.cmdCloseDocument_Click);
            // 
            // buttonItem2
            // 
            this.buttonItem2.BeginGroup = true;
            this.buttonItem2.Category = "File";
            this.buttonItem2.GlobalName = "SaveDocument";
            this.buttonItem2.ImageIndex = 17;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem2.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.buttonItem2.Text = "&Save";
            this.buttonItem2.Tooltip = "Save active document";
            this.buttonItem2.Click += new System.EventHandler(this.cmdSaveDocument_Click);
            // 
            // cmdSaveDocumentAs
            // 
            this.cmdSaveDocumentAs.Category = "File";
            this.cmdSaveDocumentAs.GlobalName = "SaveDocumentAs";
            this.cmdSaveDocumentAs.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.cmdSaveDocumentAs.Name = "cmdSaveDocumentAs";
            this.cmdSaveDocumentAs.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdSaveDocumentAs.Text = "Save &As...";
            this.cmdSaveDocumentAs.Click += new System.EventHandler(this.cmdSaveDocumentAs_Click);
            // 
            // cmdPageSetup
            // 
            this.cmdPageSetup.BeginGroup = true;
            this.cmdPageSetup.Category = "File";
            this.cmdPageSetup.GlobalName = "PageSetup";
            this.cmdPageSetup.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.cmdPageSetup.Name = "cmdPageSetup";
            this.cmdPageSetup.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdPageSetup.Text = "Page &Setup...";
            this.cmdPageSetup.Click += new System.EventHandler(this.cmdPageSetup_Click);
            // 
            // PrintPreview
            // 
            this.PrintPreview.Category = "File";
            this.PrintPreview.GlobalName = "PrintPreview";
            this.PrintPreview.ImageIndex = 14;
            this.PrintPreview.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.PrintPreview.Name = "PrintPreview";
            this.PrintPreview.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.PrintPreview.Text = "Print Pre&view";
            this.PrintPreview.Click += new System.EventHandler(this.cmdPrintPreview_Click);
            // 
            // Print
            // 
            this.Print.Category = "File";
            this.Print.GlobalName = "Print";
            this.Print.ImageIndex = 13;
            this.Print.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.Print.Name = "Print";
            this.Print.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.Print.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlP);
            this.Print.Text = "&Print";
            this.Print.Click += new System.EventHandler(this.cmdPrint_Click);
            // 
            // ExitApplication
            // 
            this.ExitApplication.BeginGroup = true;
            this.ExitApplication.Category = "File";
            this.ExitApplication.GlobalName = "ExitApplication";
            this.ExitApplication.Name = "ExitApplication";
            this.ExitApplication.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.ExitApplication.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlX);
            this.ExitApplication.Text = "E&xit";
            this.ExitApplication.Click += new System.EventHandler(this.cmdExitApplication_Click);
            // 
            // bEdit
            // 
            this.bEdit.Category = "Main Menu";
            this.bEdit.GlobalName = "bEdit";
            this.bEdit.Name = "bEdit";
            this.bEdit.PersonalizedMenus = DevComponents.DotNetBar.ePersonalizedMenus.Both;
            this.bEdit.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bUndo,
            this.bCut,
            this.bCopy,
            this.bPaste,
            this.bDelete,
            this.bSelectAll,
            this.Find,
            this.bFindNext,
            this.bReplace});
            this.bEdit.Text = "&Edit";
            // 
            // bUndo
            // 
            this.bUndo.Category = "Edit";
            this.bUndo.Enabled = false;
            this.bUndo.GlobalName = "bUndo";
            this.bUndo.ImageIndex = 19;
            this.bUndo.Name = "bUndo";
            this.bUndo.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bUndo.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlZ);
            this.bUndo.Text = "&Undo";
            // 
            // bCut
            // 
            this.bCut.BeginGroup = true;
            this.bCut.Category = "Edit";
            this.bCut.Enabled = false;
            this.bCut.GlobalName = "bCut";
            this.bCut.ImageIndex = 5;
            this.bCut.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bCut.Name = "bCut";
            this.bCut.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bCut.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlX);
            this.bCut.Text = "Cu&t";
            // 
            // bCopy
            // 
            this.bCopy.Category = "Edit";
            this.bCopy.Enabled = false;
            this.bCopy.GlobalName = "bCopy";
            this.bCopy.ImageIndex = 4;
            this.bCopy.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bCopy.Name = "bCopy";
            this.bCopy.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bCopy.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlC);
            this.bCopy.Text = "&Copy";
            // 
            // bPaste
            // 
            this.bPaste.Category = "Edit";
            this.bPaste.Enabled = false;
            this.bPaste.GlobalName = "bPaste";
            this.bPaste.ImageIndex = 12;
            this.bPaste.Name = "bPaste";
            this.bPaste.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bPaste.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlV);
            this.bPaste.Text = "&Paste";
            // 
            // bDelete
            // 
            this.bDelete.Category = "Edit";
            this.bDelete.Enabled = false;
            this.bDelete.GlobalName = "bDelete";
            this.bDelete.ImageIndex = 2;
            this.bDelete.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bDelete.Name = "bDelete";
            this.bDelete.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bDelete.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlD);
            this.bDelete.Text = "&Delete";
            // 
            // bSelectAll
            // 
            this.bSelectAll.BeginGroup = true;
            this.bSelectAll.Category = "Edit";
            this.bSelectAll.Enabled = false;
            this.bSelectAll.GlobalName = "bSelectAll";
            this.bSelectAll.Name = "bSelectAll";
            this.bSelectAll.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bSelectAll.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlA);
            this.bSelectAll.Text = "Select &All";
            // 
            // Find
            // 
            this.Find.BeginGroup = true;
            this.Find.Category = "Edit";
            this.Find.Enabled = false;
            this.Find.GlobalName = "Find";
            this.Find.ImageIndex = 7;
            this.Find.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.Find.Name = "Find";
            this.Find.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.Find.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlF);
            this.Find.Text = "&Find...";
            // 
            // bFindNext
            // 
            this.bFindNext.Category = "Edit";
            this.bFindNext.Enabled = false;
            this.bFindNext.GlobalName = "bFindNext";
            this.bFindNext.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bFindNext.Name = "bFindNext";
            this.bFindNext.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bFindNext.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F3);
            this.bFindNext.Text = "Find &Next";
            // 
            // bReplace
            // 
            this.bReplace.Category = "Edit";
            this.bReplace.Enabled = false;
            this.bReplace.GlobalName = "bReplace";
            this.bReplace.ImageIndex = 15;
            this.bReplace.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bReplace.Name = "bReplace";
            this.bReplace.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bReplace.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlH);
            this.bReplace.Text = "&Replace...";
            // 
            // bFormat
            // 
            this.bFormat.Category = "Main Menu";
            this.bFormat.GlobalName = "bFormat";
            this.bFormat.Name = "bFormat";
            this.bFormat.PersonalizedMenus = DevComponents.DotNetBar.ePersonalizedMenus.Both;
            this.bFormat.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bBold,
            this.bItalic,
            this.bUnderline,
            this.bStrikethrough,
            this.bAlignLeft,
            this.bAlignCenter,
            this.bAlignRight,
            this.bTextColor});
            this.bFormat.Text = "F&ormat";
            // 
            // bBold
            // 
            this.bBold.Category = "Format";
            this.bBold.Enabled = false;
            this.bBold.GlobalName = "bBold";
            this.bBold.ImageIndex = 0;
            this.bBold.Name = "bBold";
            this.bBold.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bBold.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlB);
            this.bBold.Text = "&Bold";
            // 
            // bItalic
            // 
            this.bItalic.Category = "Format";
            this.bItalic.Enabled = false;
            this.bItalic.GlobalName = "bItalic";
            this.bItalic.ImageIndex = 8;
            this.bItalic.Name = "bItalic";
            this.bItalic.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bItalic.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlI);
            this.bItalic.Text = "&Italic";
            // 
            // bUnderline
            // 
            this.bUnderline.Category = "Format";
            this.bUnderline.Enabled = false;
            this.bUnderline.GlobalName = "bUnderline";
            this.bUnderline.ImageIndex = 18;
            this.bUnderline.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bUnderline.Name = "bUnderline";
            this.bUnderline.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bUnderline.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlU);
            this.bUnderline.Text = "&Underline";
            // 
            // bStrikethrough
            // 
            this.bStrikethrough.Category = "Format";
            this.bStrikethrough.Enabled = false;
            this.bStrikethrough.GlobalName = "bStrikethrough";
            this.bStrikethrough.ImageIndex = 20;
            this.bStrikethrough.Name = "bStrikethrough";
            this.bStrikethrough.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bStrikethrough.Text = "&Strikethrough";
            // 
            // bAlignLeft
            // 
            this.bAlignLeft.BeginGroup = true;
            this.bAlignLeft.Category = "Format";
            this.bAlignLeft.Enabled = false;
            this.bAlignLeft.GlobalName = "bAlignLeft";
            this.bAlignLeft.ImageIndex = 9;
            this.bAlignLeft.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bAlignLeft.Name = "bAlignLeft";
            this.bAlignLeft.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bAlignLeft.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlL);
            this.bAlignLeft.Text = "Align &Left";
            // 
            // bAlignCenter
            // 
            this.bAlignCenter.Category = "Format";
            this.bAlignCenter.Enabled = false;
            this.bAlignCenter.GlobalName = "bAlignCenter";
            this.bAlignCenter.ImageIndex = 1;
            this.bAlignCenter.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bAlignCenter.Name = "bAlignCenter";
            this.bAlignCenter.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bAlignCenter.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlM);
            this.bAlignCenter.Text = "Align &Center";
            // 
            // bAlignRight
            // 
            this.bAlignRight.Category = "Format";
            this.bAlignRight.Enabled = false;
            this.bAlignRight.GlobalName = "bAlignRight";
            this.bAlignRight.ImageIndex = 16;
            this.bAlignRight.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bAlignRight.Name = "bAlignRight";
            this.bAlignRight.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bAlignRight.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlR);
            this.bAlignRight.Text = "Align &Right";
            // 
            // bTextColor
            // 
            this.bTextColor.Enabled = false;
            this.bTextColor.GlobalName = "bTextColor";
            this.bTextColor.ImageIndex = 21;
            this.bTextColor.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.bTextColor.Name = "bTextColor";
            this.bTextColor.Text = "&Text Color";
            // 
            // bWindow
            // 
            this.bWindow.Category = "Main Menu";
            this.bWindow.GlobalName = "bWindow";
            this.bWindow.Name = "bWindow";
            this.bWindow.PersonalizedMenus = DevComponents.DotNetBar.ePersonalizedMenus.Both;
            this.bWindow.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.WindowNew,
            this.WindowArrangeAll,
            this.item_236});
            this.bWindow.Text = "&Window";
            // 
            // WindowNew
            // 
            this.WindowNew.Category = "Window";
            this.WindowNew.GlobalName = "WindowNew";
            this.WindowNew.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.WindowNew.Name = "WindowNew";
            this.WindowNew.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.WindowNew.Text = "&New Window";
            this.WindowNew.Click += new System.EventHandler(this.cmdNewDocument_Click);
            // 
            // WindowArrangeAll
            // 
            this.WindowArrangeAll.Category = "Window";
            this.WindowArrangeAll.GlobalName = "WindowArrangeAll";
            this.WindowArrangeAll.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.WindowArrangeAll.Name = "WindowArrangeAll";
            this.WindowArrangeAll.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.WindowArrangeAll.Text = "&Arrange All";
            this.WindowArrangeAll.Click += new System.EventHandler(this.cmdWindowArrangeAll_Click);
            // 
            // item_236
            // 
            this.item_236.BeginGroup = true;
            this.item_236.GlobalName = "item_236";
            this.item_236.Name = "item_236";
            this.item_236.ShowWindowIcons = true;
            this.item_236.Text = "MDI Window List";
            // 
            // bHelp
            // 
            this.bHelp.Category = "Main Menu";
            this.bHelp.GlobalName = "bHelp";
            this.bHelp.Name = "bHelp";
            this.bHelp.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bAbout});
            this.bHelp.Text = "&Help";
            // 
            // bAbout
            // 
            this.bAbout.Category = "Help";
            this.bAbout.GlobalName = "bAbout";
            this.bAbout.Name = "bAbout";
            this.bAbout.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bAbout.Text = "&About...";
            // 
            // bChangeStyle
            // 
            this.bChangeStyle.ButtonStyle = DevComponents.DotNetBar.eButtonStyle.ImageAndText;
            this.bChangeStyle.GlobalName = "bChangeStyle";
            this.bChangeStyle.Image = ((System.Drawing.Image)(resources.GetObject("bChangeStyle.Image")));
            this.bChangeStyle.ImagePosition = DevComponents.DotNetBar.eImagePosition.Right;
            this.bChangeStyle.ItemAlignment = DevComponents.DotNetBar.eItemAlignment.Far;
            this.bChangeStyle.Name = "bChangeStyle";
            this.bChangeStyle.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmdStyleOffice2003,
            this.cmdStyleVS2005,
            this.cmdStyleOfficeXP,
            this.cmdStyleOffice2007Blue});
            this.bChangeStyle.Text = "Style";
            // 
            // cmdStyleOffice2003
            // 
            this.cmdStyleOffice2003.GlobalName = "StyleOffice2003";
            this.cmdStyleOffice2003.Name = "cmdStyleOffice2003";
            this.cmdStyleOffice2003.Text = "Office 2003";
            this.cmdStyleOffice2003.Click += new System.EventHandler(this.cmdStyleOffice2003_Click);
            // 
            // cmdStyleVS2005
            // 
            this.cmdStyleVS2005.GlobalName = "StyleVS2005";
            this.cmdStyleVS2005.Name = "cmdStyleVS2005";
            this.cmdStyleVS2005.Text = "VS 2005";
            this.cmdStyleVS2005.Click += new System.EventHandler(this.cmdStyleVS2005_Click);
            // 
            // cmdStyleOfficeXP
            // 
            this.cmdStyleOfficeXP.GlobalName = "StyleOfficeXP";
            this.cmdStyleOfficeXP.Name = "cmdStyleOfficeXP";
            this.cmdStyleOfficeXP.Text = "Office XP";
            this.cmdStyleOfficeXP.Click += new System.EventHandler(this.cmdStyleOfficeXP_Click);
            // 
            // cmdStyleOffice2007Blue
            // 
            this.cmdStyleOffice2007Blue.GlobalName = "StyleOffice2007Blue";
            this.cmdStyleOffice2007Blue.Name = "cmdStyleOffice2007Blue";
            this.cmdStyleOffice2007Blue.Text = "Office 2007 <font color=\"Blue\"><b>Blue</b></font>";
            this.cmdStyleOffice2007Blue.Click += new System.EventHandler(this.cmdStyleOffice2007Blue_Click);
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
            this.barStandard.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmdNewDocument,
            this.cmdOpenDocument,
            this.cmdSaveDocument,
            this.cmdPrintPreview,
            this.cmdPrint,
            this.bThemes,
            this.cmdTabbedMdi,
            this.item_139});
            this.barStandard.Location = new System.Drawing.Point(0, 27);
            this.barStandard.Name = "barStandard";
            this.barStandard.Size = new System.Drawing.Size(365, 27);
            this.barStandard.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barStandard.TabIndex = 1;
            this.barStandard.TabStop = false;
            this.barStandard.Text = "Standard";
            // 
            // cmdNewDocument
            // 
            this.cmdNewDocument.GlobalName = "NewDocument";
            this.cmdNewDocument.ImageIndex = 10;
            this.cmdNewDocument.Name = "cmdNewDocument";
            this.cmdNewDocument.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdNewDocument.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlN);
            this.cmdNewDocument.Text = "&New";
            this.cmdNewDocument.Tooltip = "Create new document";
            this.cmdNewDocument.Click += new System.EventHandler(this.cmdNewDocument_Click);
            // 
            // cmdOpenDocument
            // 
            this.cmdOpenDocument.GlobalName = "OpenDocument";
            this.cmdOpenDocument.ImageIndex = 11;
            this.cmdOpenDocument.Name = "cmdOpenDocument";
            this.cmdOpenDocument.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdOpenDocument.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlO);
            this.cmdOpenDocument.Text = "&Open";
            this.cmdOpenDocument.Tooltip = "Open existing document";
            this.cmdOpenDocument.Click += new System.EventHandler(this.cmdOpenDocument_Click);
            // 
            // cmdSaveDocument
            // 
            this.cmdSaveDocument.GlobalName = "SaveDocument";
            this.cmdSaveDocument.ImageIndex = 17;
            this.cmdSaveDocument.Name = "cmdSaveDocument";
            this.cmdSaveDocument.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdSaveDocument.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.cmdSaveDocument.Text = "&Save";
            this.cmdSaveDocument.Tooltip = "Save active document";
            this.cmdSaveDocument.Click += new System.EventHandler(this.cmdSaveDocument_Click);
            // 
            // cmdPrintPreview
            // 
            this.cmdPrintPreview.BeginGroup = true;
            this.cmdPrintPreview.GlobalName = "PrintPreview";
            this.cmdPrintPreview.ImageIndex = 14;
            this.cmdPrintPreview.Name = "cmdPrintPreview";
            this.cmdPrintPreview.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdPrintPreview.Text = "Print Pre&view";
            this.cmdPrintPreview.Tooltip = "Display print preview";
            this.cmdPrintPreview.Click += new System.EventHandler(this.cmdPrintPreview_Click);
            // 
            // cmdPrint
            // 
            this.cmdPrint.GlobalName = "Print";
            this.cmdPrint.ImageIndex = 13;
            this.cmdPrint.Name = "cmdPrint";
            this.cmdPrint.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.cmdPrint.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlP);
            this.cmdPrint.Text = "&Print";
            this.cmdPrint.Tooltip = "Print active document";
            this.cmdPrint.Click += new System.EventHandler(this.cmdPrint_Click);
            // 
            // bThemes
            // 
            this.bThemes.BeginGroup = true;
            this.bThemes.ButtonStyle = DevComponents.DotNetBar.eButtonStyle.ImageAndText;
            this.bThemes.Category = "Commands";
            this.bThemes.GlobalName = "bThemes";
            this.bThemes.Image = ((System.Drawing.Image)(resources.GetObject("bThemes.Image")));
            this.bThemes.Name = "bThemes";
            this.bThemes.Text = "Enable Themes";
            this.bThemes.Tooltip = "Enable DotNetBar Theme Support";
            // 
            // cmdTabbedMdi
            // 
            this.cmdTabbedMdi.BeginGroup = true;
            this.cmdTabbedMdi.Category = "Commands";
            this.cmdTabbedMdi.Checked = true;
            this.cmdTabbedMdi.GlobalName = "TabbedMdi";
            this.cmdTabbedMdi.Name = "cmdTabbedMdi";
            this.cmdTabbedMdi.Text = "Tabbed Mdi";
            this.cmdTabbedMdi.Click += new System.EventHandler(this.cmdTabbedMdi_Click);
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
            // barEdit
            // 
            this.barEdit.AccessibleDescription = "DotNetBar Bar (barEdit)";
            this.barEdit.AccessibleName = "DotNetBar Bar";
            this.barEdit.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barEdit.CanHide = true;
            this.barEdit.DockLine = 1;
            this.barEdit.DockOffset = 224;
            this.barEdit.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barEdit.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barEdit.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem9,
            this.buttonItem10,
            this.buttonItem11,
            this.buttonItem12,
            this.bFind,
            this.item_140});
            this.barEdit.Location = new System.Drawing.Point(367, 27);
            this.barEdit.Name = "barEdit";
            this.barEdit.Size = new System.Drawing.Size(159, 25);
            this.barEdit.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barEdit.TabIndex = 2;
            this.barEdit.TabStop = false;
            this.barEdit.Text = "Edit";
            // 
            // buttonItem9
            // 
            this.buttonItem9.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem9.DisabledImage")));
            this.buttonItem9.Enabled = false;
            this.buttonItem9.GlobalName = "bUndo";
            this.buttonItem9.ImageIndex = 19;
            this.buttonItem9.Name = "buttonItem9";
            this.buttonItem9.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem9.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlZ);
            this.buttonItem9.Text = "&Undo";
            this.buttonItem9.Tooltip = "Undo last action";
            // 
            // buttonItem10
            // 
            this.buttonItem10.BeginGroup = true;
            this.buttonItem10.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem10.DisabledImage")));
            this.buttonItem10.Enabled = false;
            this.buttonItem10.GlobalName = "bCut";
            this.buttonItem10.ImageIndex = 5;
            this.buttonItem10.Name = "buttonItem10";
            this.buttonItem10.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem10.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlX);
            this.buttonItem10.Text = "Cu&t";
            this.buttonItem10.Tooltip = "Cut selected text";
            // 
            // buttonItem11
            // 
            this.buttonItem11.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem11.DisabledImage")));
            this.buttonItem11.Enabled = false;
            this.buttonItem11.GlobalName = "bCopy";
            this.buttonItem11.ImageIndex = 4;
            this.buttonItem11.Name = "buttonItem11";
            this.buttonItem11.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem11.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlC);
            this.buttonItem11.Text = "&Copy";
            this.buttonItem11.Tooltip = "Copy selected text";
            // 
            // buttonItem12
            // 
            this.buttonItem12.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem12.DisabledImage")));
            this.buttonItem12.Enabled = false;
            this.buttonItem12.GlobalName = "bPaste";
            this.buttonItem12.ImageIndex = 12;
            this.buttonItem12.Name = "buttonItem12";
            this.buttonItem12.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem12.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlV);
            this.buttonItem12.Text = "&Paste";
            this.buttonItem12.Tooltip = "Paste text from clipboard";
            // 
            // bFind
            // 
            this.bFind.BeginGroup = true;
            this.bFind.DisabledImage = ((System.Drawing.Image)(resources.GetObject("bFind.DisabledImage")));
            this.bFind.Enabled = false;
            this.bFind.GlobalName = "Find";
            this.bFind.ImageIndex = 7;
            this.bFind.Name = "bFind";
            this.bFind.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bFind.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlF);
            this.bFind.Text = "&Find...";
            this.bFind.Tooltip = "Find text in active document";
            // 
            // item_140
            // 
            this.item_140.AutoCollapseOnClick = true;
            this.item_140.CanCustomize = true;
            this.item_140.GlobalName = "item_140";
            this.item_140.Name = "item_140";
            this.item_140.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.item_140.Text = "&Add or Remove Buttons";
            this.item_140.Tooltip = "Bar Options";
            // 
            // barFormat
            // 
            this.barFormat.AccessibleDescription = "DotNetBar Bar (barFormat)";
            this.barFormat.AccessibleName = "DotNetBar Bar";
            this.barFormat.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.barFormat.CanHide = true;
            this.barFormat.DockLine = 2;
            this.barFormat.DockSide = DevComponents.DotNetBar.eDockSide.Top;
            this.barFormat.GrabHandleStyle = DevComponents.DotNetBar.eGrabHandleStyle.Office2003;
            this.barFormat.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem14,
            this.buttonItem15,
            this.buttonItem16,
            this.buttonItem17,
            this.buttonItem18,
            this.buttonItem19,
            this.buttonItem20,
            this.colorPickerDropDown1,
            this.item_141});
            this.barFormat.Location = new System.Drawing.Point(0, 55);
            this.barFormat.Name = "barFormat";
            this.barFormat.Size = new System.Drawing.Size(247, 25);
            this.barFormat.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barFormat.TabIndex = 3;
            this.barFormat.TabStop = false;
            this.barFormat.Text = "Format";
            // 
            // buttonItem14
            // 
            this.buttonItem14.Category = "Format";
            this.buttonItem14.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem14.DisabledImage")));
            this.buttonItem14.Enabled = false;
            this.buttonItem14.GlobalName = "bBold";
            this.buttonItem14.ImageIndex = 0;
            this.buttonItem14.Name = "buttonItem14";
            this.buttonItem14.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem14.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlB);
            this.buttonItem14.Text = "&Bold";
            this.buttonItem14.Tooltip = "Bold";
            // 
            // buttonItem15
            // 
            this.buttonItem15.Category = "Format";
            this.buttonItem15.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem15.DisabledImage")));
            this.buttonItem15.Enabled = false;
            this.buttonItem15.GlobalName = "bItalic";
            this.buttonItem15.ImageIndex = 8;
            this.buttonItem15.Name = "buttonItem15";
            this.buttonItem15.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem15.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlI);
            this.buttonItem15.Text = "&Italic";
            this.buttonItem15.Tooltip = "Italic";
            // 
            // buttonItem16
            // 
            this.buttonItem16.Category = "Format";
            this.buttonItem16.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem16.DisabledImage")));
            this.buttonItem16.Enabled = false;
            this.buttonItem16.GlobalName = "bUnderline";
            this.buttonItem16.ImageIndex = 18;
            this.buttonItem16.Name = "buttonItem16";
            this.buttonItem16.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem16.Text = "&Underline";
            this.buttonItem16.Tooltip = "Underline";
            // 
            // buttonItem17
            // 
            this.buttonItem17.Category = "Format";
            this.buttonItem17.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem17.DisabledImage")));
            this.buttonItem17.Enabled = false;
            this.buttonItem17.GlobalName = "bStrikethrough";
            this.buttonItem17.ImageIndex = 20;
            this.buttonItem17.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleIfRecentlyUsed;
            this.buttonItem17.Name = "buttonItem17";
            this.buttonItem17.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem17.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlT);
            this.buttonItem17.Text = "&Strikethrough";
            // 
            // buttonItem18
            // 
            this.buttonItem18.BeginGroup = true;
            this.buttonItem18.Category = "Format";
            this.buttonItem18.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem18.DisabledImage")));
            this.buttonItem18.Enabled = false;
            this.buttonItem18.GlobalName = "bAlignLeft";
            this.buttonItem18.ImageIndex = 9;
            this.buttonItem18.Name = "buttonItem18";
            this.buttonItem18.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem18.Text = "Align &Left";
            this.buttonItem18.Tooltip = "Align Left";
            // 
            // buttonItem19
            // 
            this.buttonItem19.Category = "Format";
            this.buttonItem19.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem19.DisabledImage")));
            this.buttonItem19.Enabled = false;
            this.buttonItem19.GlobalName = "bAlignCenter";
            this.buttonItem19.ImageIndex = 1;
            this.buttonItem19.Name = "buttonItem19";
            this.buttonItem19.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem19.Text = "Align &Center";
            this.buttonItem19.Tooltip = "Align Center";
            // 
            // buttonItem20
            // 
            this.buttonItem20.Category = "Format";
            this.buttonItem20.DisabledImage = ((System.Drawing.Image)(resources.GetObject("buttonItem20.DisabledImage")));
            this.buttonItem20.Enabled = false;
            this.buttonItem20.GlobalName = "bAlignRight";
            this.buttonItem20.ImageIndex = 16;
            this.buttonItem20.Name = "buttonItem20";
            this.buttonItem20.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem20.Text = "Align &Right";
            this.buttonItem20.Tooltip = "Align Right";
            // 
            // colorPickerDropDown1
            // 
            this.colorPickerDropDown1.Enabled = false;
            this.colorPickerDropDown1.GlobalName = "bTextColor";
            this.colorPickerDropDown1.ImageIndex = 21;
            this.colorPickerDropDown1.Name = "colorPickerDropDown1";
            this.colorPickerDropDown1.Text = "colorPickerDropDown1";
            // 
            // item_141
            // 
            this.item_141.AutoCollapseOnClick = true;
            this.item_141.CanCustomize = true;
            this.item_141.GlobalName = "item_141";
            this.item_141.Name = "item_141";
            this.item_141.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.item_141.Text = "&Add or Remove Buttons";
            this.item_141.Tooltip = "Bar Options";
            // 
            // tabStrip1
            // 
            this.tabStrip1.AutoSelectAttachedControl = true;
            this.tabStrip1.CanReorderTabs = true;
            this.tabStrip1.CloseButtonOnTabsVisible = true;
            this.tabStrip1.CloseButtonVisible = false;
            this.contextMenuBar1.SetContextMenuEx(this.tabStrip1, this.bTabContext);
            this.tabStrip1.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabStrip1.Location = new System.Drawing.Point(0, 80);
            this.tabStrip1.MdiTabbedDocuments = true;
            this.tabStrip1.Name = "tabStrip1";
            this.tabStrip1.SelectedTab = null;
            this.tabStrip1.SelectedTabFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabStrip1.Size = new System.Drawing.Size(473, 28);
            this.tabStrip1.Style = DevComponents.DotNetBar.eTabStripStyle.OneNote;
            this.tabStrip1.TabAlignment = DevComponents.DotNetBar.eTabStripAlignment.Top;
            this.tabStrip1.TabIndex = 6;
            this.tabStrip1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabStrip1.Text = "tabStrip1";
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timerInfoBallon
            // 
            this.timerInfoBallon.Interval = 3000;
            this.timerInfoBallon.Tick += new System.EventHandler(this.timerInfoBallon_Tick);
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
            this.labelPosition,
            this.itemProgressBar});
            this.bar1.ItemSpacing = 2;
            this.bar1.Location = new System.Drawing.Point(0, 511);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(704, 23);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 7;
            this.bar1.TabStop = false;
            this.bar1.Text = "barStatus";
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
            // labelPosition
            // 
            this.labelPosition.BorderType = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.labelPosition.Name = "labelPosition";
            this.labelPosition.PaddingLeft = 2;
            this.labelPosition.PaddingRight = 2;
            this.labelPosition.SingleLineColor = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.labelPosition.Width = 100;
            // 
            // itemProgressBar
            // 
            this.itemProgressBar.ChunkColor = System.Drawing.Color.FromArgb(((int)(((byte)(254)))), ((int)(((byte)(142)))), ((int)(((byte)(75)))));
            this.itemProgressBar.ChunkColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(207)))), ((int)(((byte)(139)))));
            this.itemProgressBar.ChunkGradientAngle = 90F;
            this.itemProgressBar.MenuVisibility = DevComponents.DotNetBar.eMenuVisibility.VisibleAlways;
            this.itemProgressBar.Name = "itemProgressBar";
            this.itemProgressBar.RecentlyUsed = false;
            this.itemProgressBar.Text = "progressBarItem1";
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.Images = this.imageList1;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bEditPopup,
            this.bTaskListMenu,
            this.bTabContext,
            this.bDockContext});
            this.contextMenuBar1.Location = new System.Drawing.Point(211, 129);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(394, 51);
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 12;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.PopupOpen += new DevComponents.DotNetBar.DotNetBarManager.PopupOpenEventHandler(this.contextMenuBar1_PopupOpen);
            this.contextMenuBar1.PopupContainerUnload += new System.EventHandler(this.UnloadPopup);
            this.contextMenuBar1.PopupContainerLoad += new System.EventHandler(this.LoadPopup);
            // 
            // bDockContext
            // 
            this.bDockContext.AutoExpandOnClick = true;
            this.bDockContext.GlobalName = "bDockContext";
            this.bDockContext.Name = "bDockContext";
            this.bDockContext.Text = "bDockContext";
            // 
            // bTabContext
            // 
            this.bTabContext.AutoExpandOnClick = true;
            this.bTabContext.GlobalName = "bTabContext";
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
            this.cmdContextSave.Name = "cmdContextSave";
            this.cmdContextSave.Text = "Save";
            this.cmdContextSave.Tooltip = "Save active document";
            this.cmdContextSave.Click += new System.EventHandler(this.cmdSaveDocument_Click);
            // 
            // cmdContextClose
            // 
            this.cmdContextClose.Enabled = false;
            this.cmdContextClose.GlobalItem = false;
            this.cmdContextClose.Name = "cmdContextClose";
            this.cmdContextClose.Text = "Close";
            this.cmdContextClose.Tooltip = "Close active document";
            this.cmdContextClose.Click += new System.EventHandler(this.cmdCloseDocument_Click);
            // 
            // bTabColor
            // 
            this.bTabColor.BeginGroup = true;
            this.bTabColor.GlobalName = "bTabColor";
            this.bTabColor.Name = "bTabColor";
            this.bTabColor.PopupType = DevComponents.DotNetBar.ePopupType.Container;
            this.bTabColor.Text = "Change Tab Color";
            // 
            // bEditPopup
            // 
            this.bEditPopup.AutoExpandOnClick = true;
            this.bEditPopup.GlobalName = "bEditPopup";
            this.bEditPopup.Name = "bEditPopup";
            this.bEditPopup.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.bEditPopup.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem22,
            this.buttonItem23,
            this.buttonItem24,
            this.buttonItem25});
            this.bEditPopup.Text = "bEditPopup";
            // 
            // buttonItem22
            // 
            this.buttonItem22.BeginGroup = true;
            this.buttonItem22.GlobalName = "bCut";
            this.buttonItem22.ImageIndex = 5;
            this.buttonItem22.Name = "buttonItem22";
            this.buttonItem22.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem22.Text = "Cu&t";
            this.buttonItem22.Click += new System.EventHandler(this.DocumentContextMenuCommand);
            // 
            // buttonItem23
            // 
            this.buttonItem23.GlobalName = "bCopy";
            this.buttonItem23.ImageIndex = 4;
            this.buttonItem23.Name = "buttonItem23";
            this.buttonItem23.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem23.Text = "&Copy";
            this.buttonItem23.Click += new System.EventHandler(this.DocumentContextMenuCommand);
            // 
            // buttonItem24
            // 
            this.buttonItem24.GlobalName = "bPaste";
            this.buttonItem24.ImageIndex = 12;
            this.buttonItem24.Name = "buttonItem24";
            this.buttonItem24.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem24.Text = "&Paste";
            this.buttonItem24.Click += new System.EventHandler(this.DocumentContextMenuCommand);
            // 
            // buttonItem25
            // 
            this.buttonItem25.BeginGroup = true;
            this.buttonItem25.GlobalName = "bSelectAll";
            this.buttonItem25.Name = "buttonItem25";
            this.buttonItem25.PopupAnimation = DevComponents.DotNetBar.ePopupAnimation.SystemDefault;
            this.buttonItem25.Text = "Select &All";
            this.buttonItem25.Click += new System.EventHandler(this.DocumentContextMenuCommand);
            // 
            // bTaskListMenu
            // 
            this.bTaskListMenu.AutoExpandOnClick = true;
            this.bTaskListMenu.GlobalName = "bTaskListMenu";
            this.bTaskListMenu.Name = "bTaskListMenu";
            this.bTaskListMenu.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.item_477,
            this.item_504,
            this.item_531});
            this.bTaskListMenu.Text = "bTaskListMenu";
            // 
            // item_477
            // 
            this.item_477.GlobalName = "item_477";
            this.item_477.Name = "item_477";
            this.item_477.Text = "Show All Tasks";
            // 
            // item_504
            // 
            this.item_504.GlobalName = "item_504";
            this.item_504.Name = "item_504";
            this.item_504.Text = "Show High Importance only";
            // 
            // item_531
            // 
            this.item_531.BeginGroup = true;
            this.item_531.GlobalName = "item_531";
            this.item_531.Name = "item_531";
            this.item_531.Text = "Hide";
            // 
            // frmMain
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(704, 534);
            this.Controls.Add(this.contextMenuBar1);
            this.Controls.Add(this.tabStrip1);
            this.Controls.Add(this.barLeftDockSite);
            this.Controls.Add(this.barRightDockSite);
            this.Controls.Add(this.barTopDockSite);
            this.Controls.Add(this.barBottomDockSite);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.bar1);
            this.Controls.Add(this.mdiClient1);
            this.IsMdiContainer = true;
            this.Name = "frmMain";
            this.Text = "DotNetBar Notepad Sample";
            this.MdiChildActivate += new System.EventHandler(this.MdiChildActivated);
            this.Move += new System.EventHandler(this.frmMain_Move);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.barBottomDockSite.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.barTaskList)).EndInit();
            this.barTaskList.ResumeLayout(false);
            this.panelDockContainer1.ResumeLayout(false);
            this.panelDockContainer2.ResumeLayout(false);
            this.panelDockContainer2.PerformLayout();
            this.barRightDockSite.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.barTaskPane)).EndInit();
            this.barTaskPane.ResumeLayout(false);
            this.panelDockContainer4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.explorerBar1)).EndInit();
            this.panelDockContainer3.ResumeLayout(false);
            this.panelDockContainer3.PerformLayout();
            this.dockSite4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.windowlist)).EndInit();
            this.dockSite3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.mainmenu)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barStandard)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barEdit)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barFormat)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

		}
		#endregion

		#region AppCreation
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmMain());
		}
		#endregion

		#region Document Command Handling
		public void EditContextMenu()
		{
			bEditPopup.Displayed=false;
			bEditPopup.PopupMenu(Control.MousePosition);
		}

		private void BarItemClick(object sender, EventArgs e)
		{
			BaseItem item=sender as BaseItem;
			frmDocument activedocument=this.ActiveMdiChild as frmDocument;

			// Activate the form
			if(item.Name == "window_list")
				((Form)item.Tag).Activate();
			else if(item == bThemes)
				EnableThemes(bThemes);
			else if(item.GlobalName == bTextColor.GlobalName && activedocument!=null)
				activedocument.ExecuteCommand(item.GlobalName, ((ColorPickerDropDown)item).SelectedColor);
			else if(activedocument!=null)
			{
				// Pass command to the active document
				// Note the usage of GlobalName property! Since same command can be found on both menus and toolbars, for example Bold
				// you have to give two different names through Name property to these two instances. However, you can and should
				// give them same GlobalName. GlobalName will ensure that properties set on one instance are replicated to all
				// objects with the same GlobalName. You can read more about Global Items feature in help file.
				activedocument.ExecuteCommand(item.GlobalName,null);
			}
		}

		private void CreateNewDocument()
		{
			frmDocument doc=new frmDocument();
			doc.MdiParent=this;
			doc.WindowState=FormWindowState.Maximized;
			doc.Show();
			doc.Update();
			doc.Text="New Document "+this.MdiChildren.Length.ToString();
		}

		/// <summary>
		/// Verifies current context and enables/disables menu items...
		/// </summary>
		private void EnableFileItems()
		{
			// Accessing items through the Items collection and setting the properties on them
			// will propagate certain properties to all items with the same name...
			bool enabled = true;
			if(this.ActiveMdiChild==null)
			{
				enabled = false;
			}

			cmdSaveDocument.Enabled = enabled;
			cmdSaveDocumentAs.Enabled = enabled;
			cmdPrint.Enabled = enabled;
			cmdPrintPreview.Enabled = enabled;
			cmdPageSetup.Enabled = enabled;
			cmdCloseDocument.Enabled = enabled;
		}

		private void MdiChildActivated(object sender, System.EventArgs e)
		{
			EnableFileItems();
		}

		private void OpenDocument()
		{
			openFileDialog1.FileName="";
			openFileDialog1.ShowDialog();
			if(openFileDialog1.FileName=="")
				return;
			frmDocument doc=new frmDocument();
			doc.Text=openFileDialog1.FileName;
			doc.MdiParent=this;
			doc.Show();
			doc.OpenFile(openFileDialog1.FileName);
		}

		/// <summary>
		/// Save active document
		/// </summary>
		private void SaveDocument()
		{
			if(this.ActiveMdiChild==null)
				return;
			frmDocument doc=this.ActiveMdiChild as frmDocument;
			if(doc==null || !doc.DocumentChanged)
				return;
            SaveDocument(doc);
		}
		internal void SaveDocument(frmDocument doc)
		{
			if(doc.FileName=="")
			{
				if(saveFileDialog1.ShowDialog()!=DialogResult.OK)
					return;
				if(saveFileDialog1.FileName=="")
					return;
				doc.FileName=saveFileDialog1.FileName;
			}
			if(doc.FileName!="")
				doc.SaveFile();
		}
		private void SaveDocumentAs()
		{
			if(this.ActiveMdiChild==null)
				return;
			frmDocument doc=this.ActiveMdiChild as frmDocument;
			if(doc==null || !doc.DocumentChanged)
				return;
			if(doc.FileName=="")
			{
				SaveDocument(doc);
				return;
			}

			saveFileDialog1.ShowDialog();
			if(saveFileDialog1.FileName=="")
				return;
			doc.FileName=saveFileDialog1.FileName;
			doc.SaveFile();
		}

		private void LoadPopup(object sender, System.EventArgs e)
		{
			ButtonItem item=sender as ButtonItem;
			if(item == bTabColor)
			{
				DevComponents.DotNetBar.PopupContainerControl container=item.PopupContainerControl as PopupContainerControl;
				ColorPicker clr=new ColorPicker();
				container.Controls.Add(clr);
				if(cmdStyleOffice2003.Checked)
				{
					clr.BackColor=dotNetBarManager1.Bars[0].ColorScheme.BarBackground2;
					clr.tabStrip1.Style=eTabStripStyle.Office2003;
				}
				clr.Location=container.ClientRectangle.Location;
				container.ClientSize=clr.Size;
			}
		}

		private void UnloadPopup(object sender, System.EventArgs e)
		{
			ButtonItem item=sender as ButtonItem;
			if(item.Name=="bTabColor")
			{
				DevComponents.DotNetBar.PopupContainerControl container=item.PopupContainerControl as PopupContainerControl;
				ColorPicker clr=container.Controls[0] as ColorPicker;
				if(clr.SelectedColor!=Color.Empty)
				{
					tabStrip1.ColorScheme.TabBackground=ControlPaint.LightLight(clr.SelectedColor);
					tabStrip1.ColorScheme.TabBackground2=clr.SelectedColor;
					tabStrip1.Refresh();
				}
				// Close popup menu, since it is not closed when Popup Container is closed...
				item.Parent.Expanded=false;
			}
		}

		private void ChangeDotNetBarStyle(eDotNetBarStyle style)
		{
			cmdStyleOffice2007Blue.Checked = (style==eDotNetBarStyle.Office2007);
			cmdStyleOffice2003.Checked = (style==eDotNetBarStyle.Office2003);
			cmdStyleOfficeXP.Checked = (style==eDotNetBarStyle.OfficeXP);
			cmdStyleVS2005.Checked = (style==eDotNetBarStyle.VS2005);
			
			explorerBar1.ColorScheme=new DevComponents.DotNetBar.ColorScheme(style);

            dotNetBarManager1.Style=style;
			if(style==eDotNetBarStyle.Office2003 || style==eDotNetBarStyle.OfficeXP || style==eDotNetBarStyle.VS2005)
			{
				if(dotNetBarManager1.IsThemeActive)
				{
					bThemes.Visible=true;
					barStandard.RecalcLayout();
				}
			}
			else if(dotNetBarManager1.IsThemeActive)
			{
				bThemes.Visible=false;
				barStandard.RecalcLayout();
			}

			if(style==eDotNetBarStyle.Office2003)
			{
				tabStrip1.Style=eTabStripStyle.OneNote;
			}
			else if(style == eDotNetBarStyle.Office2007)
			{
				tabStrip1.Style=eTabStripStyle.Office2007Document;
			}
			else if(style==eDotNetBarStyle.VS2005)
			{
				tabStrip1.Style=eTabStripStyle.VS2005;
			}
			else
			{
				tabStrip1.Style=eTabStripStyle.Flat;
			}

			// Status bar style
			bar1.Style=style;
		}

		private void TaskPaneShowAtStartup(object sender, EventArgs e)
		{
			MessageBox.Show("This item is here for demonstration purposes only and code should be added to save the state of it.");
		}

		private void dotNetBarManager1_MouseEnter(object sender, System.EventArgs e)
		{
			// Sync Status-bar with the item tooltip...
			BaseItem item=sender as BaseItem;
			if(item==null)
				return;
            labelStatus.Text=item.Tooltip;
			// Disable timer stop flashing on Style button...
			if(item==bChangeStyle && timer1.Enabled)
			{
				timer1.Enabled=false;
				bChangeStyle.ForeColor=Color.Empty;
			}
		}

		private void dotNetBarManager1_MouseLeave(object sender, System.EventArgs e)
		{
            labelStatus.Text="";		
		}

		private void frmMain_Load(object sender, System.EventArgs e)
		{
			ChangeDotNetBarStyle(eDotNetBarStyle.Office2003);

			EnableFileItems();

			// This makes sure that Theme support is enabled by the end-user as well as present on this OS
			if(dotNetBarManager1.IsThemeActive)
			{
				bThemes.Visible=true;
				timer1.Enabled=true;
			}

			// Neccessary for Automatic Tab-Strip Mdi support
			tabStrip1.MdiForm=this;

			timerInfoBallon.Enabled=true;
		}

		private void timer1_Tick(object sender, System.EventArgs e)
		{
			// Flash Item to attract attention...
			if(bChangeStyle.ForeColor==Color.Red)
				bChangeStyle.ForeColor=Color.Empty;
			else
				bChangeStyle.ForeColor=Color.Red;

			ProgressBarItem progress=itemProgressBar;
			if(progress.Value==progress.Maximum)
				progress.Value=progress.Minimum;
			else
				progress.Increment(10);
		}

		private void EnableThemes(ButtonItem item)
		{
			bool bEnable=!item.Checked;
			item.Checked=bEnable;
			if(bEnable)
				item.Text="Disable Themes";
			else
				item.Text="Enable Themes";

			dotNetBarManager1.SuspendLayout=true;
			dotNetBarManager1.ThemeAware=bEnable;
			dotNetBarManager1.SuspendLayout=false;
			tabStrip1.ThemeAware=bEnable;

			this.Refresh();
		}

		private void SwitchWorkingMode(bool tabbedMdi)
		{
			if(tabbedMdi)
			{
				// Switch to Tabbed Mdi Interface
				dotNetBarManager1.MdiSystemItemVisible=false;
				tabStrip1.MdiTabbedDocuments=true;
				dotNetBarManager1.Bars["windowlist"].Visible=false;
			}
			else
			{
				// Switch to standard Mdi Interface
				tabStrip1.Visible=false;
				dotNetBarManager1.MdiSystemItemVisible=true;
				tabStrip1.MdiTabbedDocuments=false;
                tabStrip1.MdiForm=null;
				SyncMdiTabbedBar();
			}
		}

		private void MdiClientControlAddRemove(object sender, ControlEventArgs e)
		{
			if(e.Control is Form)
				SyncMdiTabbedBar();
		}
		private void SyncMdiTabbedBar()
		{
			if(cmdTabbedMdi.Checked)
			{
				// Hide window list bar
				dotNetBarManager1.Bars["windowlist"].Visible=false;
			}
			else
			{
				if(this.MdiChildren.Length>0)
				{
					dotNetBarManager1.Bars["windowlist"].Visible=true;
					dotNetBarManager1.Bars["windowlist"].DockLine=1;
					dotNetBarManager1.Bars["windowlist"].RecalcLayout();
				}
				else
					dotNetBarManager1.Bars["windowlist"].Visible=false;
			}
		}

		private TabItem TabItemFromPoint(Point pClient)
		{
			if(!tabStrip1.ClientRectangle.Contains(pClient))
				return null;
			foreach(TabItem tab in tabStrip1.Tabs)
			{
				if(tab.Visible && tab.DisplayRectangle.Contains(pClient))
					return tab;
			}

			return null;
		}

		private void DockContextClick(object sender, EventArgs e)
		{
			ButtonItem item=sender as ButtonItem;
			if(item==null)
				return;

			Bar bar=barTaskList;
			if(item.Name=="bc_tabnavigation")
			{
				bar.TabNavigation=!item.Checked;
				bar.DockTabControl.Refresh();
				return;
			}			
			
			if(bar.Items.Contains(item.Name))
				BarUtilities.SetDockContainerVisible((DockContainerItem)bar.Items[item.Name],!item.Checked);

			bar.RecalcLayout();
		}

		private void timerInfoBallon_Tick(object sender, System.EventArgs e)
		{
			timerInfoBallon.Enabled=false;
			m_InfoBalloon=new Balloon();
			m_InfoBalloon.CaptionImage=new Bitmap(typeof(frmMain),"Info.png");
			m_InfoBalloon.CaptionText="New DotNetBar Feature Alert...";
			m_InfoBalloon.Text="DotNetBar now includes Balloon control to help you convey your information effectively to the end-user.\nCheck out Find command implementation in this sample for an example of highly customized Balloon control.";
			m_InfoBalloon.AutoResize();
			m_InfoBalloon.AutoCloseTimeOut=10;
			m_InfoBalloon.Owner=this;
			m_InfoBalloon.Show(bFind,false); // Show below the Find toolbar button
		}

		private void frmMain_Move(object sender, System.EventArgs e)
		{
			this.CloseInfoBalloon();
			this.CloseSearch();
		}

		private void CloseInfoBalloon()
		{
			if(m_InfoBalloon!=null)
			{
				m_InfoBalloon.Close();
				m_InfoBalloon.Dispose();
				m_InfoBalloon=null;
			}
		}

		private void CloseSearch()
		{
			if(m_Search!=null)
			{
				m_Search.Close();
				m_Search.Dispose();
				m_Search=null;
			}
		}

		public void SearchActiveDocument(string text)
		{
			frmDocument activedocument=this.ActiveMdiChild as frmDocument;
			if(activedocument!=null)
				activedocument.ExecuteCommand("Find",text);
		}

		private void dotNetBarManager1_DockTabChange(object sender, DevComponents.DotNetBar.DockTabChangeEventArgs e)
		{
			Bar bar=sender as Bar;
			if(bar==null || e.NewTab==null)
				return;
			bar.Text=e.NewTab.Text;
		}

		private void dotNetBarManager1_BarClosing(object sender, DevComponents.DotNetBar.BarClosingEventArgs e)
		{
			Bar bar=sender as Bar;
			if(bar==null || bar != barTaskList)
				return;
			if(bar.VisibleItemCount > 1)
			{
				bar.Items[bar.SelectedDockTab].Visible=false;
				bar.RecalcLayout();
				e.Cancel=true;
			}
		}

		private void cmdNewDocument_Click(object sender, System.EventArgs e)
		{
			CreateNewDocument();
		}

		private void cmdOpenDocument_Click(object sender, System.EventArgs e)
		{
			OpenDocument();
		}

		private void cmdSaveDocument_Click(object sender, System.EventArgs e)
		{
			SaveDocument();
		}

		private void cmdCloseDocument_Click(object sender, System.EventArgs e)
		{
			frmDocument activedocument=this.ActiveMdiChild as frmDocument;
			if(activedocument!=null)
				activedocument.Close();
		}

		private void cmdPrintPreview_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show("Not implemented yet.");
		}

		private void cmdPrint_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show("Not implemented yet.");
		}

		private void cmdPageSetup_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show("Not implemented yet.");
		}

		private void cmdExitApplication_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}
		
		private void cmdWindowArrangeAll_Click(object sender, System.EventArgs e)
		{
			this.LayoutMdi(MdiLayout.Cascade);
		}

		private void cmdSaveDocumentAs_Click(object sender, System.EventArgs e)
		{
			SaveDocumentAs();
		}

		private void cmdStyleOffice2003_Click(object sender, System.EventArgs e)
		{
			ChangeDotNetBarStyle(eDotNetBarStyle.Office2003);
		}

		private void cmdStyleOffice2007Blue_Click(object sender, System.EventArgs e)
		{
			ChangeDotNetBarStyle(eDotNetBarStyle.Office2007);
		}

		private void cmdStyleOfficeXP_Click(object sender, System.EventArgs e)
		{
			ChangeDotNetBarStyle(eDotNetBarStyle.OfficeXP);
		}

		private void cmdStyleVS2005_Click(object sender, System.EventArgs e)
		{
			ChangeDotNetBarStyle(eDotNetBarStyle.VS2005);
		}

		private void cmdTabbedMdi_Click(object sender, System.EventArgs e)
		{
			SwitchWorkingMode(!cmdTabbedMdi.Checked);
			cmdTabbedMdi.Checked = !cmdTabbedMdi.Checked;
		}

		private void cmdFind_Click(object sender, System.EventArgs e)
		{
			if(m_Search==null || m_Search.IsDisposed)
			{
				m_Search=new BalloonSearch();
				m_Search.Owner=this;
				m_Search.Show(sender as BaseItem,true);
			}
		}

		private void DocumentContextMenuCommand(object sender, System.EventArgs e)
		{
			BaseItem item=sender as BaseItem;
			frmDocument activedocument=this.ActiveMdiChild as frmDocument;
			if(activedocument!=null)
				activedocument.ExecuteCommand(item.GlobalName, null);
		}

		private void contextMenuBar1_PopupOpen(object sender, DevComponents.DotNetBar.PopupOpenEventArgs e)
		{
			BaseItem item=sender as BaseItem;
			if(item==null)
				return;
			if(item == bTabContext)
			{
				// Enable/Disable the document related menu items based on where user right-clicked...
				TabItem tab=TabItemFromPoint(tabStrip1.PointToClient(Control.MousePosition));
				// If no tab is under the mouse disable document related items...
				if(tab==null)
				{
					cmdContextClose.Enabled = false;
					cmdContextSave.Enabled = false;
				}
				else
				{
					cmdContextClose.Enabled = true;
					cmdContextSave.Enabled = true;
					// Make sure that tab under the mouse is active tab
					tabStrip1.SelectedTab=tab;
				}
			}
			else if(item.Name=="bDockContext")
			{
				// Display our context menu only if user clicks on the bar caption or tab strip control
				if(!barTaskList.GrabHandleRect.Contains(barTaskList.PointToClient(Control.MousePosition)) && 
					barTaskList.DockTabControl!=null && !barTaskList.DockTabControl.Bounds.Contains(barTaskList.PointToClient(Control.MousePosition)))
				{
					e.Cancel=true;
					return;
				}
				item.SubItems.Clear();
				ButtonItem contextItem=null;
				foreach(BaseItem dockItem in barTaskList.Items)
				{
					contextItem=new ButtonItem(dockItem.Name,dockItem.Text);
					contextItem.GlobalItem=false;
					contextItem.Click+=new EventHandler(this.DockContextClick);
					item.SubItems.Add(contextItem);
					contextItem.Checked=dockItem.Visible;
				}
				contextItem=new ButtonItem("bc_tabnavigation","Tab Navigation");
				contextItem.Checked=barTaskList.TabNavigation;
				contextItem.BeginGroup=true;
				contextItem.Click+=new EventHandler(this.DockContextClick);
				item.SubItems.Add(contextItem);
				PopupItem popup=item as PopupItem;
			}
		}
		#endregion
	}
}

