namespace MapCutter
{
    partial class FrmMain
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.proc = new DevComponents.DotNetBar.Controls.ProgressBarX();
            this.btnClose = new DevComponents.DotNetBar.ButtonX();
            this.btnStart = new DevComponents.DotNetBar.ButtonX();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel4 = new DevComponents.DotNetBar.TabControlPanel();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.btnBrower = new DevComponents.DotNetBar.ButtonX();
            this.tbxArtFile = new System.Windows.Forms.TextBox();
            this.btnTransfer = new DevComponents.DotNetBar.ButtonX();
            this.tbxMapName = new System.Windows.Forms.TextBox();
            this.tabItem3 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.checkBox4 = new System.Windows.Forms.CheckBox();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.txtSmallPath = new System.Windows.Forms.TextBox();
            this.txtSmallName = new System.Windows.Forms.TextBox();
            this.btnSmallOpen = new DevComponents.DotNetBar.ButtonX();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.小地图编辑 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.checkBoxFormat = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBox3 = new System.Windows.Forms.CheckBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.btnMiddleOpen = new DevComponents.DotNetBar.ButtonX();
            this.textBoxOld = new System.Windows.Forms.TextBox();
            this.txtMiddlePath = new System.Windows.Forms.TextBox();
            this.txtMiddleName = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel3 = new DevComponents.DotNetBar.TabControlPanel();
            this.canvas = new MapCutter.Canvas();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.bFile = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.bOpen = new DevComponents.DotNetBar.ButtonItem();
            this.bClose = new DevComponents.DotNetBar.ButtonItem();
            this.bData = new DevComponents.DotNetBar.ButtonItem();
            this.bRead = new DevComponents.DotNetBar.ButtonItem();
            this.bSave = new DevComponents.DotNetBar.ButtonItem();
            this.bView = new DevComponents.DotNetBar.ButtonItem();
            this.bPanel = new DevComponents.DotNetBar.ButtonItem();
            this.bClear = new DevComponents.DotNetBar.ButtonItem();
            this.bMark = new DevComponents.DotNetBar.ButtonItem();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.CD1 = new System.Windows.Forms.OpenFileDialog();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabControlPanel4.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.tabControlPanel2.SuspendLayout();
            this.tabControlPanel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.proc);
            this.panelEx1.Controls.Add(this.btnClose);
            this.panelEx1.Controls.Add(this.btnStart);
            this.panelEx1.Controls.Add(this.tabControl1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(569, 432);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            this.panelEx1.Text = "panelEx1";
            // 
            // proc
            // 
            this.proc.Location = new System.Drawing.Point(8, 397);
            this.proc.Name = "proc";
            this.proc.Size = new System.Drawing.Size(393, 23);
            this.proc.TabIndex = 3;
            this.proc.Text = "progressBarX1";
            this.proc.Visible = false;
            // 
            // btnClose
            // 
            this.btnClose.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClose.Location = new System.Drawing.Point(488, 397);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(75, 23);
            this.btnClose.TabIndex = 2;
            this.btnClose.Text = "放弃";
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // btnStart
            // 
            this.btnStart.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnStart.Location = new System.Drawing.Point(407, 397);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 1;
            this.btnStart.Text = "开始";
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.BackColor = System.Drawing.Color.Transparent;
            this.tabControl1.CanReorderTabs = true;
            this.tabControl1.Controls.Add(this.tabControlPanel4);
            this.tabControl1.Controls.Add(this.tabControlPanel1);
            this.tabControl1.Controls.Add(this.tabControlPanel2);
            this.tabControl1.Controls.Add(this.tabControlPanel3);
            this.tabControl1.Location = new System.Drawing.Point(8, 9);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(554, 378);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.小地图编辑);
            this.tabControl1.Tabs.Add(this.tabItem1);
            this.tabControl1.Tabs.Add(this.tabItem2);
            this.tabControl1.Tabs.Add(this.tabItem3);
            this.tabControl1.Text = "tabControl1";
            // 
            // tabControlPanel4
            // 
            this.tabControlPanel4.Controls.Add(this.label8);
            this.tabControlPanel4.Controls.Add(this.label7);
            this.tabControlPanel4.Controls.Add(this.btnBrower);
            this.tabControlPanel4.Controls.Add(this.tbxArtFile);
            this.tabControlPanel4.Controls.Add(this.btnTransfer);
            this.tabControlPanel4.Controls.Add(this.tbxMapName);
            this.tabControlPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel4.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel4.Name = "tabControlPanel4";
            this.tabControlPanel4.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel4.Size = new System.Drawing.Size(554, 352);
            this.tabControlPanel4.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel4.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel4.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel4.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel4.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel4.Style.GradientAngle = 90;
            this.tabControlPanel4.TabIndex = 4;
            this.tabControlPanel4.TabItem = this.tabItem3;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.BackColor = System.Drawing.Color.Transparent;
            this.label8.Location = new System.Drawing.Point(41, 59);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(89, 12);
            this.label8.TabIndex = 15;
            this.label8.Text = "对应场景名称：";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.BackColor = System.Drawing.Color.Transparent;
            this.label7.Location = new System.Drawing.Point(41, 143);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 12);
            this.label7.TabIndex = 14;
            this.label7.Text = "美术文件：";
            // 
            // btnBrower
            // 
            this.btnBrower.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnBrower.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnBrower.Location = new System.Drawing.Point(399, 171);
            this.btnBrower.Name = "btnBrower";
            this.btnBrower.Size = new System.Drawing.Size(64, 20);
            this.btnBrower.TabIndex = 13;
            this.btnBrower.Text = "浏览";
            this.btnBrower.Click += new System.EventHandler(this.btnBrower_Click);
            // 
            // tbxArtFile
            // 
            this.tbxArtFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbxArtFile.Location = new System.Drawing.Point(41, 170);
            this.tbxArtFile.Name = "tbxArtFile";
            this.tbxArtFile.Size = new System.Drawing.Size(326, 21);
            this.tbxArtFile.TabIndex = 12;
            // 
            // btnTransfer
            // 
            this.btnTransfer.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnTransfer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnTransfer.Location = new System.Drawing.Point(303, 235);
            this.btnTransfer.Name = "btnTransfer";
            this.btnTransfer.Size = new System.Drawing.Size(64, 20);
            this.btnTransfer.TabIndex = 11;
            this.btnTransfer.Text = "转换";
            this.btnTransfer.Click += new System.EventHandler(this.btnTransfer_Click);
            // 
            // tbxMapName
            // 
            this.tbxMapName.Location = new System.Drawing.Point(41, 88);
            this.tbxMapName.Name = "tbxMapName";
            this.tbxMapName.Size = new System.Drawing.Size(221, 21);
            this.tbxMapName.TabIndex = 10;
            this.tbxMapName.Text = "少林";
            // 
            // tabItem3
            // 
            this.tabItem3.AttachedControl = this.tabControlPanel4;
            this.tabItem3.Name = "tabItem3";
            this.tabItem3.Text = "美术坐标到逻辑坐标";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.checkBox4);
            this.tabControlPanel1.Controls.Add(this.radioButton2);
            this.tabControlPanel1.Controls.Add(this.radioButton1);
            this.tabControlPanel1.Controls.Add(this.txtSmallPath);
            this.tabControlPanel1.Controls.Add(this.txtSmallName);
            this.tabControlPanel1.Controls.Add(this.btnSmallOpen);
            this.tabControlPanel1.Controls.Add(this.label3);
            this.tabControlPanel1.Controls.Add(this.label2);
            this.tabControlPanel1.Controls.Add(this.label1);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(554, 352);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 1;
            this.tabControlPanel1.TabItem = this.小地图编辑;
            // 
            // checkBox4
            // 
            this.checkBox4.AutoSize = true;
            this.checkBox4.BackColor = System.Drawing.Color.Transparent;
            this.checkBox4.Location = new System.Drawing.Point(192, 222);
            this.checkBox4.Name = "checkBox4";
            this.checkBox4.Size = new System.Drawing.Size(48, 16);
            this.checkBox4.TabIndex = 17;
            this.checkBox4.Text = "副本";
            this.checkBox4.UseVisualStyleBackColor = false;
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.BackColor = System.Drawing.Color.Transparent;
            this.radioButton2.Location = new System.Drawing.Point(127, 222);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(47, 16);
            this.radioButton2.TabIndex = 7;
            this.radioButton2.Text = "洞穴";
            this.radioButton2.UseVisualStyleBackColor = false;
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.BackColor = System.Drawing.Color.Transparent;
            this.radioButton1.Checked = true;
            this.radioButton1.Location = new System.Drawing.Point(50, 222);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(71, 16);
            this.radioButton1.TabIndex = 7;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "户外场景";
            this.radioButton1.UseVisualStyleBackColor = false;
            // 
            // txtSmallPath
            // 
            this.txtSmallPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSmallPath.Location = new System.Drawing.Point(50, 56);
            this.txtSmallPath.Name = "txtSmallPath";
            this.txtSmallPath.Size = new System.Drawing.Size(419, 21);
            this.txtSmallPath.TabIndex = 4;
            // 
            // txtSmallName
            // 
            this.txtSmallName.Location = new System.Drawing.Point(50, 135);
            this.txtSmallName.Name = "txtSmallName";
            this.txtSmallName.Size = new System.Drawing.Size(221, 21);
            this.txtSmallName.TabIndex = 4;
            this.txtSmallName.Text = "少林";
            // 
            // btnSmallOpen
            // 
            this.btnSmallOpen.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnSmallOpen.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSmallOpen.Location = new System.Drawing.Point(475, 57);
            this.btnSmallOpen.Name = "btnSmallOpen";
            this.btnSmallOpen.Size = new System.Drawing.Size(64, 20);
            this.btnSmallOpen.TabIndex = 2;
            this.btnSmallOpen.Text = "浏览";
            this.btnSmallOpen.Click += new System.EventHandler(this.btnSmallOpen_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(18, 184);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 0;
            this.label3.Text = "地图属性：";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(18, 109);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 12);
            this.label2.TabIndex = 0;
            this.label2.Text = "对应场景名称：";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(18, 33);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(125, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "选择你要生成的地图：";
            // 
            // 小地图编辑
            // 
            this.小地图编辑.AttachedControl = this.tabControlPanel1;
            this.小地图编辑.Name = "小地图编辑";
            this.小地图编辑.Text = "小地图编辑";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.checkBoxFormat);
            this.tabControlPanel2.Controls.Add(this.checkBox3);
            this.tabControlPanel2.Controls.Add(this.checkBox2);
            this.tabControlPanel2.Controls.Add(this.checkBox1);
            this.tabControlPanel2.Controls.Add(this.buttonX1);
            this.tabControlPanel2.Controls.Add(this.btnMiddleOpen);
            this.tabControlPanel2.Controls.Add(this.textBoxOld);
            this.tabControlPanel2.Controls.Add(this.txtMiddlePath);
            this.tabControlPanel2.Controls.Add(this.txtMiddleName);
            this.tabControlPanel2.Controls.Add(this.label6);
            this.tabControlPanel2.Controls.Add(this.label5);
            this.tabControlPanel2.Controls.Add(this.label10);
            this.tabControlPanel2.Controls.Add(this.label11);
            this.tabControlPanel2.Controls.Add(this.label4);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(554, 352);
            this.tabControlPanel2.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel2.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel2.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel2.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel2.Style.GradientAngle = 90;
            this.tabControlPanel2.TabIndex = 2;
            this.tabControlPanel2.TabItem = this.tabItem1;
            // 
            // checkBoxFormat
            // 
            this.checkBoxFormat.BackColor = System.Drawing.Color.Transparent;
            this.checkBoxFormat.Location = new System.Drawing.Point(50, 246);
            this.checkBoxFormat.Name = "checkBoxFormat";
            this.checkBoxFormat.Size = new System.Drawing.Size(499, 23);
            this.checkBoxFormat.TabIndex = 17;
            this.checkBoxFormat.Text = "是新旧过渡场景（图片为旧格式，场景map为新格式，详情密zhouhengda,kuangsihao）";
            // 
            // checkBox3
            // 
            this.checkBox3.AutoSize = true;
            this.checkBox3.BackColor = System.Drawing.Color.Transparent;
            this.checkBox3.Location = new System.Drawing.Point(170, 186);
            this.checkBox3.Name = "checkBox3";
            this.checkBox3.Size = new System.Drawing.Size(48, 16);
            this.checkBox3.TabIndex = 16;
            this.checkBox3.Text = "战场";
            this.checkBox3.UseVisualStyleBackColor = false;
            // 
            // checkBox2
            // 
            this.checkBox2.AutoSize = true;
            this.checkBox2.BackColor = System.Drawing.Color.Transparent;
            this.checkBox2.Location = new System.Drawing.Point(104, 186);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(60, 16);
            this.checkBox2.TabIndex = 16;
            this.checkBox2.Text = "新手村";
            this.checkBox2.UseVisualStyleBackColor = false;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.BackColor = System.Drawing.Color.Transparent;
            this.checkBox1.Location = new System.Drawing.Point(50, 186);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(48, 16);
            this.checkBox1.TabIndex = 16;
            this.checkBox1.Text = "副本";
            this.checkBox1.UseVisualStyleBackColor = false;
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.Location = new System.Drawing.Point(475, 276);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(64, 20);
            this.buttonX1.TabIndex = 4;
            this.buttonX1.Text = "浏览";
            this.buttonX1.Click += new System.EventHandler(this.btnMiddleOpen2_Click);
            // 
            // btnMiddleOpen
            // 
            this.btnMiddleOpen.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnMiddleOpen.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMiddleOpen.Location = new System.Drawing.Point(475, 57);
            this.btnMiddleOpen.Name = "btnMiddleOpen";
            this.btnMiddleOpen.Size = new System.Drawing.Size(64, 20);
            this.btnMiddleOpen.TabIndex = 4;
            this.btnMiddleOpen.Text = "浏览";
            this.btnMiddleOpen.Click += new System.EventHandler(this.btnMiddleOpen_Click);
            // 
            // textBoxOld
            // 
            this.textBoxOld.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOld.Location = new System.Drawing.Point(143, 275);
            this.textBoxOld.Name = "textBoxOld";
            this.textBoxOld.Size = new System.Drawing.Size(326, 21);
            this.textBoxOld.TabIndex = 9;
            // 
            // txtMiddlePath
            // 
            this.txtMiddlePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtMiddlePath.Location = new System.Drawing.Point(50, 56);
            this.txtMiddlePath.Name = "txtMiddlePath";
            this.txtMiddlePath.Size = new System.Drawing.Size(419, 21);
            this.txtMiddlePath.TabIndex = 9;
            // 
            // txtMiddleName
            // 
            this.txtMiddleName.Location = new System.Drawing.Point(50, 117);
            this.txtMiddleName.Name = "txtMiddleName";
            this.txtMiddleName.Size = new System.Drawing.Size(221, 21);
            this.txtMiddleName.TabIndex = 9;
            this.txtMiddleName.Text = "少林";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.BackColor = System.Drawing.Color.Transparent;
            this.label6.Location = new System.Drawing.Point(48, 278);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(77, 12);
            this.label6.TabIndex = 7;
            this.label6.Text = "旧配置文件：";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.Location = new System.Drawing.Point(18, 226);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(65, 12);
            this.label5.TabIndex = 7;
            this.label5.Text = "新旧格式：";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.BackColor = System.Drawing.Color.Transparent;
            this.label10.Location = new System.Drawing.Point(18, 162);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(65, 12);
            this.label10.TabIndex = 7;
            this.label10.Text = "地图属性：";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.BackColor = System.Drawing.Color.Transparent;
            this.label11.Location = new System.Drawing.Point(18, 91);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(89, 12);
            this.label11.TabIndex = 8;
            this.label11.Text = "对应场景名称：";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(18, 33);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(125, 12);
            this.label4.TabIndex = 1;
            this.label4.Text = "选择你要生成的地图：";
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel2;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "中地图编辑器";
            // 
            // tabControlPanel3
            // 
            this.tabControlPanel3.Controls.Add(this.canvas);
            this.tabControlPanel3.Controls.Add(this.bar1);
            this.tabControlPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel3.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel3.Name = "tabControlPanel3";
            this.tabControlPanel3.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel3.Size = new System.Drawing.Size(554, 352);
            this.tabControlPanel3.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel3.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel3.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel3.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel3.Style.GradientAngle = 90;
            this.tabControlPanel3.TabIndex = 3;
            this.tabControlPanel3.TabItem = this.tabItem2;
            // 
            // canvas
            // 
            this.canvas.Dock = System.Windows.Forms.DockStyle.Fill;
            this.canvas.Location = new System.Drawing.Point(1, 25);
            this.canvas.Name = "canvas";
            this.canvas.Size = new System.Drawing.Size(552, 326);
            this.canvas.TabIndex = 2;
            this.canvas.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseDoubleClick);
            this.canvas.MouseClick += new System.Windows.Forms.MouseEventHandler(this.canvas_MouseClick);
            this.canvas.SizeChanged += new System.EventHandler(this.panel1_SizeChanged);
            // 
            // bar1
            // 
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = System.Windows.Forms.AccessibleRole.MenuBar;
            this.bar1.Dock = System.Windows.Forms.DockStyle.Top;
            this.bar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bFile,
            this.bData,
            this.bPanel});
            this.bar1.Location = new System.Drawing.Point(1, 1);
            this.bar1.MenuBar = true;
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(552, 24);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 1;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // bFile
            // 
            this.bFile.ImagePaddingHorizontal = 8;
            this.bFile.Name = "bFile";
            this.bFile.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem1,
            this.bOpen,
            this.bClose});
            this.bFile.Text = "文件";
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "打开地图";
            this.buttonItem1.Visible = false;
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // bOpen
            // 
            this.bOpen.ImagePaddingHorizontal = 8;
            this.bOpen.Name = "bOpen";
            this.bOpen.Text = "打开";
            this.bOpen.Click += new System.EventHandler(this.bOpen_Click);
            // 
            // bClose
            // 
            this.bClose.BeginGroup = true;
            this.bClose.ImagePaddingHorizontal = 8;
            this.bClose.Name = "bClose";
            this.bClose.Text = "关闭";
            this.bClose.Click += new System.EventHandler(this.bClose_Click);
            // 
            // bData
            // 
            this.bData.ImagePaddingHorizontal = 8;
            this.bData.Name = "bData";
            this.bData.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bRead,
            this.bSave,
            this.bView});
            this.bData.Text = "数据";
            // 
            // bRead
            // 
            this.bRead.ImagePaddingHorizontal = 8;
            this.bRead.Name = "bRead";
            this.bRead.Text = "读取";
            this.bRead.Click += new System.EventHandler(this.bRead_Click);
            // 
            // bSave
            // 
            this.bSave.ImagePaddingHorizontal = 8;
            this.bSave.Name = "bSave";
            this.bSave.Text = "保存";
            this.bSave.Click += new System.EventHandler(this.bSave_Click);
            // 
            // bView
            // 
            this.bView.ImagePaddingHorizontal = 8;
            this.bView.Name = "bView";
            this.bView.Text = "预览";
            this.bView.Click += new System.EventHandler(this.bView_Click);
            // 
            // bPanel
            // 
            this.bPanel.ImagePaddingHorizontal = 8;
            this.bPanel.Name = "bPanel";
            this.bPanel.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bClear,
            this.bMark});
            this.bPanel.Text = "面板";
            // 
            // bClear
            // 
            this.bClear.ImagePaddingHorizontal = 8;
            this.bClear.Name = "bClear";
            this.bClear.Text = "清理标记";
            this.bClear.Click += new System.EventHandler(this.bClear_Click);
            // 
            // bMark
            // 
            this.bMark.ImagePaddingHorizontal = 8;
            this.bMark.Name = "bMark";
            this.bMark.Text = "已配置区域";
            this.bMark.Click += new System.EventHandler(this.bMark_Click);
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel3;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "灰度图制作";
            // 
            // CD1
            // 
            this.CD1.FileName = "openFileDialog1";
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(569, 432);
            this.Controls.Add(this.panelEx1);
            this.Name = "FrmMain";
            this.Text = "中·小地图编辑器";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FrmMain_FormClosed);
            this.panelEx1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabControlPanel4.ResumeLayout(false);
            this.tabControlPanel4.PerformLayout();
            this.tabControlPanel1.ResumeLayout(false);
            this.tabControlPanel1.PerformLayout();
            this.tabControlPanel2.ResumeLayout(false);
            this.tabControlPanel2.PerformLayout();
            this.tabControlPanel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnClose;
        private DevComponents.DotNetBar.ButtonX btnStart;
        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem 小地图编辑;
        private DevComponents.DotNetBar.ButtonX btnSmallOpen;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtSmallName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private System.Windows.Forms.Label label4;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.ButtonX btnMiddleOpen;
        private System.Windows.Forms.TextBox txtMiddleName;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.TextBox txtSmallPath;
        private DevComponents.DotNetBar.Controls.ProgressBarX proc;
        private System.Windows.Forms.TextBox txtMiddlePath;
        private System.Windows.Forms.OpenFileDialog CD1;
        private System.Windows.Forms.CheckBox checkBox3;
        private System.Windows.Forms.CheckBox checkBox2;
        private System.Windows.Forms.CheckBox checkBox1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel3;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.ButtonItem bFile;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem bOpen;
        private DevComponents.DotNetBar.ButtonItem bClose;
        private DevComponents.DotNetBar.ButtonItem bData;
        private DevComponents.DotNetBar.ButtonItem bRead;
        private DevComponents.DotNetBar.ButtonItem bSave;
        private DevComponents.DotNetBar.ButtonItem bView;
        private DevComponents.DotNetBar.ButtonItem bPanel;
        private DevComponents.DotNetBar.ButtonItem bClear;
        private DevComponents.DotNetBar.ButtonItem bMark;
        private Canvas canvas;
        private System.Windows.Forms.CheckBox checkBox4;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxFormat;
        private System.Windows.Forms.Label label5;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private System.Windows.Forms.TextBox textBoxOld;
        private System.Windows.Forms.Label label6;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel4;
        private DevComponents.DotNetBar.ButtonX btnBrower;
        private System.Windows.Forms.TextBox tbxArtFile;
        private DevComponents.DotNetBar.ButtonX btnTransfer;
        private System.Windows.Forms.TextBox tbxMapName;
        private DevComponents.DotNetBar.TabItem tabItem3;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
    }
}

