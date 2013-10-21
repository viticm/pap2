namespace GreyHoundEditor
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
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.bFile = new DevComponents.DotNetBar.ButtonItem();
            this.bOpen = new DevComponents.DotNetBar.ButtonItem();
            this.bClose = new DevComponents.DotNetBar.ButtonItem();
            this.bData = new DevComponents.DotNetBar.ButtonItem();
            this.bRead = new DevComponents.DotNetBar.ButtonItem();
            this.bSave = new DevComponents.DotNetBar.ButtonItem();
            this.bView = new DevComponents.DotNetBar.ButtonItem();
            this.bPanel = new DevComponents.DotNetBar.ButtonItem();
            this.bClear = new DevComponents.DotNetBar.ButtonItem();
            this.bMark = new DevComponents.DotNetBar.ButtonItem();
            this.dockSite3 = new DevComponents.DotNetBar.DockSite();
            this.canvas = new GreyHoundEditor.Canvas();
            this.dockSite7.SuspendLayout();
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
            this.dotNetBarManager1.ParentForm = this;
            this.dotNetBarManager1.RightDockSite = this.dockSite2;
            this.dotNetBarManager1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
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
            this.dockSite4.Location = new System.Drawing.Point(0, 506);
            this.dockSite4.Name = "dockSite4";
            this.dockSite4.Size = new System.Drawing.Size(602, 0);
            this.dockSite4.TabIndex = 4;
            this.dockSite4.TabStop = false;
            // 
            // dockSite1
            // 
            this.dockSite1.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite1.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite1.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite1.Location = new System.Drawing.Point(0, 25);
            this.dockSite1.Name = "dockSite1";
            this.dockSite1.Size = new System.Drawing.Size(0, 481);
            this.dockSite1.TabIndex = 1;
            this.dockSite1.TabStop = false;
            // 
            // dockSite2
            // 
            this.dockSite2.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite2.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite2.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite2.Location = new System.Drawing.Point(602, 25);
            this.dockSite2.Name = "dockSite2";
            this.dockSite2.Size = new System.Drawing.Size(0, 481);
            this.dockSite2.TabIndex = 2;
            this.dockSite2.TabStop = false;
            // 
            // dockSite8
            // 
            this.dockSite8.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite8.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dockSite8.Location = new System.Drawing.Point(0, 506);
            this.dockSite8.Name = "dockSite8";
            this.dockSite8.Size = new System.Drawing.Size(602, 0);
            this.dockSite8.TabIndex = 8;
            this.dockSite8.TabStop = false;
            // 
            // dockSite5
            // 
            this.dockSite5.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite5.Dock = System.Windows.Forms.DockStyle.Left;
            this.dockSite5.Location = new System.Drawing.Point(0, 25);
            this.dockSite5.Name = "dockSite5";
            this.dockSite5.Size = new System.Drawing.Size(0, 481);
            this.dockSite5.TabIndex = 5;
            this.dockSite5.TabStop = false;
            // 
            // dockSite6
            // 
            this.dockSite6.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite6.Dock = System.Windows.Forms.DockStyle.Right;
            this.dockSite6.Location = new System.Drawing.Point(602, 25);
            this.dockSite6.Name = "dockSite6";
            this.dockSite6.Size = new System.Drawing.Size(0, 481);
            this.dockSite6.TabIndex = 6;
            this.dockSite6.TabStop = false;
            // 
            // dockSite7
            // 
            this.dockSite7.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite7.Controls.Add(this.bar1);
            this.dockSite7.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite7.Location = new System.Drawing.Point(0, 0);
            this.dockSite7.Name = "dockSite7";
            this.dockSite7.Size = new System.Drawing.Size(602, 25);
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
            this.bFile,
            this.bData,
            this.bPanel});
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.MenuBar = true;
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(602, 24);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 0;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // bFile
            // 
            this.bFile.Name = "bFile";
            this.bFile.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bOpen,
            this.bClose});
            this.bFile.Text = "文件";
            // 
            // bOpen
            // 
            this.bOpen.Name = "bOpen";
            this.bOpen.Text = "打开";
            this.bOpen.Click += new System.EventHandler(this.bOpen_Click);
            // 
            // bClose
            // 
            this.bClose.Name = "bClose";
            this.bClose.Text = "关闭";
            this.bClose.Click += new System.EventHandler(this.bClose_Click);
            // 
            // bData
            // 
            this.bData.Name = "bData";
            this.bData.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bRead,
            this.bSave,
            this.bView});
            this.bData.Text = "数据";
            // 
            // bRead
            // 
            this.bRead.Name = "bRead";
            this.bRead.Text = "读取";
            this.bRead.Click += new System.EventHandler(this.bRead_Click);
            // 
            // bSave
            // 
            this.bSave.Name = "bSave";
            this.bSave.Text = "保存";
            this.bSave.Click += new System.EventHandler(this.bSave_Click);
            // 
            // bView
            // 
            this.bView.Name = "bView";
            this.bView.Text = "预览";
            this.bView.Click += new System.EventHandler(this.bView_Click);
            // 
            // bPanel
            // 
            this.bPanel.Name = "bPanel";
            this.bPanel.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bClear,
            this.bMark});
            this.bPanel.Text = "面板";
            // 
            // bClear
            // 
            this.bClear.Name = "bClear";
            this.bClear.Text = "清理标记";
            this.bClear.Click += new System.EventHandler(this.bClear_Click);
            // 
            // bMark
            // 
            this.bMark.Name = "bMark";
            this.bMark.Text = "已配置区域";
            this.bMark.Click += new System.EventHandler(this.bMark_Click);
            // 
            // dockSite3
            // 
            this.dockSite3.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
            this.dockSite3.Dock = System.Windows.Forms.DockStyle.Top;
            this.dockSite3.DocumentDockContainer = new DevComponents.DotNetBar.DocumentDockContainer();
            this.dockSite3.Location = new System.Drawing.Point(0, 25);
            this.dockSite3.Name = "dockSite3";
            this.dockSite3.Size = new System.Drawing.Size(602, 0);
            this.dockSite3.TabIndex = 3;
            this.dockSite3.TabStop = false;
            // 
            // canvas
            // 
            this.canvas.Dock = System.Windows.Forms.DockStyle.Fill;
            this.canvas.Location = new System.Drawing.Point(0, 25);
            this.canvas.Name = "canvas";
            this.canvas.Size = new System.Drawing.Size(602, 481);
            this.canvas.TabIndex = 0;
            this.canvas.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseDoubleClick);
            this.canvas.MouseClick += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseClick);
            this.canvas.SizeChanged += new System.EventHandler(this.canvas_SizeChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(602, 506);
            this.Controls.Add(this.canvas);
            this.Controls.Add(this.dockSite1);
            this.Controls.Add(this.dockSite2);
            this.Controls.Add(this.dockSite3);
            this.Controls.Add(this.dockSite4);
            this.Controls.Add(this.dockSite5);
            this.Controls.Add(this.dockSite6);
            this.Controls.Add(this.dockSite7);
            this.Controls.Add(this.dockSite8);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "灰度图填表工具";
            this.dockSite7.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private Canvas canvas;
        private DevComponents.DotNetBar.DotNetBarManager dotNetBarManager1;
        private DevComponents.DotNetBar.DockSite dockSite4;
        private DevComponents.DotNetBar.DockSite dockSite1;
        private DevComponents.DotNetBar.DockSite dockSite2;
        private DevComponents.DotNetBar.DockSite dockSite3;
        private DevComponents.DotNetBar.DockSite dockSite5;
        private DevComponents.DotNetBar.DockSite dockSite6;
        private DevComponents.DotNetBar.DockSite dockSite7;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.ButtonItem bFile;
        private DevComponents.DotNetBar.ButtonItem bOpen;
        private DevComponents.DotNetBar.ButtonItem bSave;
        private DevComponents.DotNetBar.ButtonItem bClose;
        private DevComponents.DotNetBar.DockSite dockSite8;
        private DevComponents.DotNetBar.ButtonItem bData;
        private DevComponents.DotNetBar.ButtonItem bView;
        private DevComponents.DotNetBar.ButtonItem bRead;
        private DevComponents.DotNetBar.ButtonItem bPanel;
        private DevComponents.DotNetBar.ButtonItem bClear;
        private DevComponents.DotNetBar.ButtonItem bMark;


    }
}

