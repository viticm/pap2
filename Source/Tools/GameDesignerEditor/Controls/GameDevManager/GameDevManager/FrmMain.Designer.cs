namespace GameDevManager
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.popViewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.popSetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.popExitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label4 = new System.Windows.Forms.Label();
            this.btnSetup = new System.Windows.Forms.Button();
            this.btn4Restart = new System.Windows.Forms.Button();
            this.btn3Restart = new System.Windows.Forms.Button();
            this.btn2Restart = new System.Windows.Forms.Button();
            this.btn1Stop = new System.Windows.Forms.Button();
            this.btn1Restart = new System.Windows.Forms.Button();
            this.cb3Top = new System.Windows.Forms.CheckBox();
            this.cb2Top = new System.Windows.Forms.CheckBox();
            this.cb1Top = new System.Windows.Forms.CheckBox();
            this.cb3View = new System.Windows.Forms.CheckBox();
            this.cb2View = new System.Windows.Forms.CheckBox();
            this.btn4Stop = new System.Windows.Forms.Button();
            this.cb1View = new System.Windows.Forms.CheckBox();
            this.btn3Stop = new System.Windows.Forms.Button();
            this.btn4Start = new System.Windows.Forms.Button();
            this.btn2Stop = new System.Windows.Forms.Button();
            this.btn3Start = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.btn2Start = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.btn1Start = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnOpenClientFolder = new System.Windows.Forms.Button();
            this.btnOpenDir = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.ContextMenuStrip = this.contextMenuStrip1;
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.btnSetup);
            this.groupBox1.Controls.Add(this.btn4Restart);
            this.groupBox1.Controls.Add(this.btn3Restart);
            this.groupBox1.Controls.Add(this.btn2Restart);
            this.groupBox1.Controls.Add(this.btn1Stop);
            this.groupBox1.Controls.Add(this.btn1Restart);
            this.groupBox1.Controls.Add(this.cb3Top);
            this.groupBox1.Controls.Add(this.cb2Top);
            this.groupBox1.Controls.Add(this.cb1Top);
            this.groupBox1.Controls.Add(this.cb3View);
            this.groupBox1.Controls.Add(this.cb2View);
            this.groupBox1.Controls.Add(this.btn4Stop);
            this.groupBox1.Controls.Add(this.cb1View);
            this.groupBox1.Controls.Add(this.btn3Stop);
            this.groupBox1.Controls.Add(this.btn4Start);
            this.groupBox1.Controls.Add(this.btn2Stop);
            this.groupBox1.Controls.Add(this.btn3Start);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.btn2Start);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.btn1Start);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(8, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(313, 150);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "服务器管理";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.popViewToolStripMenuItem,
            this.popSetupToolStripMenuItem,
            this.popExitToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(113, 70);
            this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
            // 
            // popViewToolStripMenuItem
            // 
            this.popViewToolStripMenuItem.Name = "popViewToolStripMenuItem";
            this.popViewToolStripMenuItem.Size = new System.Drawing.Size(112, 22);
            this.popViewToolStripMenuItem.Text = "显示";
            this.popViewToolStripMenuItem.Click += new System.EventHandler(this.viewToolStripMenuItem_Click);
            // 
            // popSetupToolStripMenuItem
            // 
            this.popSetupToolStripMenuItem.Name = "popSetupToolStripMenuItem";
            this.popSetupToolStripMenuItem.Size = new System.Drawing.Size(112, 22);
            this.popSetupToolStripMenuItem.Text = "设置...";
            this.popSetupToolStripMenuItem.Click += new System.EventHandler(this.设置ToolStripMenuItem_Click);
            // 
            // popExitToolStripMenuItem
            // 
            this.popExitToolStripMenuItem.Name = "popExitToolStripMenuItem";
            this.popExitToolStripMenuItem.Size = new System.Drawing.Size(112, 22);
            this.popExitToolStripMenuItem.Text = "退出";
            this.popExitToolStripMenuItem.Click += new System.EventHandler(this.tuicToolStripMenuItem_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(65, 12);
            this.label4.TabIndex = 19;
            this.label4.Text = "GameClient";
            // 
            // btnSetup
            // 
            this.btnSetup.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSetup.ContextMenuStrip = this.contextMenuStrip1;
            this.btnSetup.Location = new System.Drawing.Point(232, 119);
            this.btnSetup.Name = "btnSetup";
            this.btnSetup.Size = new System.Drawing.Size(75, 23);
            this.btnSetup.TabIndex = 18;
            this.btnSetup.Text = "设置...";
            this.btnSetup.UseVisualStyleBackColor = true;
            this.btnSetup.Click += new System.EventHandler(this.btnSetup_Click);
            // 
            // btn4Restart
            // 
            this.btn4Restart.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn4Restart.ForeColor = System.Drawing.Color.MediumBlue;
            this.btn4Restart.Location = new System.Drawing.Point(142, 92);
            this.btn4Restart.Name = "btn4Restart";
            this.btn4Restart.Size = new System.Drawing.Size(28, 23);
            this.btn4Restart.TabIndex = 17;
            this.btn4Restart.Text = "L";
            this.btn4Restart.UseVisualStyleBackColor = true;
            this.btn4Restart.Click += new System.EventHandler(this.btn4Restart_Click);
            // 
            // btn3Restart
            // 
            this.btn3Restart.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn3Restart.ForeColor = System.Drawing.Color.MediumBlue;
            this.btn3Restart.Location = new System.Drawing.Point(142, 66);
            this.btn3Restart.Name = "btn3Restart";
            this.btn3Restart.Size = new System.Drawing.Size(28, 23);
            this.btn3Restart.TabIndex = 17;
            this.btn3Restart.Text = "L";
            this.btn3Restart.UseVisualStyleBackColor = true;
            this.btn3Restart.Click += new System.EventHandler(this.btn3Restart_Click);
            // 
            // btn2Restart
            // 
            this.btn2Restart.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn2Restart.ForeColor = System.Drawing.Color.MediumBlue;
            this.btn2Restart.Location = new System.Drawing.Point(142, 40);
            this.btn2Restart.Name = "btn2Restart";
            this.btn2Restart.Size = new System.Drawing.Size(28, 23);
            this.btn2Restart.TabIndex = 16;
            this.btn2Restart.Text = "L";
            this.btn2Restart.UseVisualStyleBackColor = true;
            this.btn2Restart.Click += new System.EventHandler(this.btn2Restart_Click);
            // 
            // btn1Stop
            // 
            this.btn1Stop.Font = new System.Drawing.Font("Wingdings", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn1Stop.ForeColor = System.Drawing.Color.Red;
            this.btn1Stop.Location = new System.Drawing.Point(110, 15);
            this.btn1Stop.Name = "btn1Stop";
            this.btn1Stop.Size = new System.Drawing.Size(26, 23);
            this.btn1Stop.TabIndex = 6;
            this.btn1Stop.Text = "n";
            this.btn1Stop.UseVisualStyleBackColor = true;
            this.btn1Stop.Click += new System.EventHandler(this.btn1Stop_Click);
            // 
            // btn1Restart
            // 
            this.btn1Restart.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn1Restart.ForeColor = System.Drawing.Color.MediumBlue;
            this.btn1Restart.Location = new System.Drawing.Point(142, 15);
            this.btn1Restart.Name = "btn1Restart";
            this.btn1Restart.Size = new System.Drawing.Size(28, 23);
            this.btn1Restart.TabIndex = 15;
            this.btn1Restart.Text = "L";
            this.btn1Restart.UseVisualStyleBackColor = true;
            this.btn1Restart.Click += new System.EventHandler(this.btn1Restart_Click);
            // 
            // cb3Top
            // 
            this.cb3Top.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb3Top.AutoSize = true;
            this.cb3Top.Location = new System.Drawing.Point(243, 71);
            this.cb3Top.Name = "cb3Top";
            this.cb3Top.Size = new System.Drawing.Size(48, 16);
            this.cb3Top.TabIndex = 14;
            this.cb3Top.Text = "最上";
            this.cb3Top.UseVisualStyleBackColor = true;
            this.cb3Top.CheckedChanged += new System.EventHandler(this.cb3Top_CheckedChanged);
            // 
            // cb2Top
            // 
            this.cb2Top.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb2Top.AutoSize = true;
            this.cb2Top.Location = new System.Drawing.Point(243, 46);
            this.cb2Top.Name = "cb2Top";
            this.cb2Top.Size = new System.Drawing.Size(48, 16);
            this.cb2Top.TabIndex = 13;
            this.cb2Top.Text = "最上";
            this.cb2Top.UseVisualStyleBackColor = true;
            this.cb2Top.CheckedChanged += new System.EventHandler(this.cb2Top_CheckedChanged);
            // 
            // cb1Top
            // 
            this.cb1Top.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb1Top.AutoSize = true;
            this.cb1Top.Location = new System.Drawing.Point(243, 22);
            this.cb1Top.Name = "cb1Top";
            this.cb1Top.Size = new System.Drawing.Size(48, 16);
            this.cb1Top.TabIndex = 12;
            this.cb1Top.Text = "最上";
            this.cb1Top.UseVisualStyleBackColor = true;
            this.cb1Top.CheckedChanged += new System.EventHandler(this.checkBox4_CheckedChanged);
            // 
            // cb3View
            // 
            this.cb3View.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb3View.AutoSize = true;
            this.cb3View.Checked = true;
            this.cb3View.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cb3View.Location = new System.Drawing.Point(189, 71);
            this.cb3View.Name = "cb3View";
            this.cb3View.Size = new System.Drawing.Size(48, 16);
            this.cb3View.TabIndex = 11;
            this.cb3View.Text = "显示";
            this.cb3View.UseVisualStyleBackColor = true;
            this.cb3View.CheckedChanged += new System.EventHandler(this.cb3View_CheckedChanged);
            // 
            // cb2View
            // 
            this.cb2View.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb2View.AutoSize = true;
            this.cb2View.Location = new System.Drawing.Point(189, 47);
            this.cb2View.Name = "cb2View";
            this.cb2View.Size = new System.Drawing.Size(48, 16);
            this.cb2View.TabIndex = 10;
            this.cb2View.Text = "显示";
            this.cb2View.UseVisualStyleBackColor = true;
            this.cb2View.CheckedChanged += new System.EventHandler(this.cb2View_CheckedChanged);
            // 
            // btn4Stop
            // 
            this.btn4Stop.Font = new System.Drawing.Font("Wingdings", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn4Stop.ForeColor = System.Drawing.Color.Red;
            this.btn4Stop.Location = new System.Drawing.Point(111, 92);
            this.btn4Stop.Name = "btn4Stop";
            this.btn4Stop.Size = new System.Drawing.Size(26, 23);
            this.btn4Stop.TabIndex = 8;
            this.btn4Stop.Text = "n";
            this.btn4Stop.UseVisualStyleBackColor = true;
            this.btn4Stop.Click += new System.EventHandler(this.btn4Stop_Click);
            // 
            // cb1View
            // 
            this.cb1View.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb1View.AutoSize = true;
            this.cb1View.Location = new System.Drawing.Point(189, 21);
            this.cb1View.Name = "cb1View";
            this.cb1View.Size = new System.Drawing.Size(48, 16);
            this.cb1View.TabIndex = 9;
            this.cb1View.Text = "显示";
            this.cb1View.UseVisualStyleBackColor = true;
            this.cb1View.CheckedChanged += new System.EventHandler(this.cb1View_CheckedChanged);
            // 
            // btn3Stop
            // 
            this.btn3Stop.Font = new System.Drawing.Font("Wingdings", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn3Stop.ForeColor = System.Drawing.Color.Red;
            this.btn3Stop.Location = new System.Drawing.Point(111, 66);
            this.btn3Stop.Name = "btn3Stop";
            this.btn3Stop.Size = new System.Drawing.Size(26, 23);
            this.btn3Stop.TabIndex = 8;
            this.btn3Stop.Text = "n";
            this.btn3Stop.UseVisualStyleBackColor = true;
            this.btn3Stop.Click += new System.EventHandler(this.btn3Stop_Click);
            // 
            // btn4Start
            // 
            this.btn4Start.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn4Start.ForeColor = System.Drawing.Color.LimeGreen;
            this.btn4Start.Location = new System.Drawing.Point(78, 92);
            this.btn4Start.Name = "btn4Start";
            this.btn4Start.Size = new System.Drawing.Size(27, 23);
            this.btn4Start.TabIndex = 5;
            this.btn4Start.Text = "u";
            this.btn4Start.UseVisualStyleBackColor = true;
            this.btn4Start.Click += new System.EventHandler(this.btn4Start_Click);
            // 
            // btn2Stop
            // 
            this.btn2Stop.Font = new System.Drawing.Font("Wingdings", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn2Stop.ForeColor = System.Drawing.Color.Red;
            this.btn2Stop.Location = new System.Drawing.Point(111, 41);
            this.btn2Stop.Name = "btn2Stop";
            this.btn2Stop.Size = new System.Drawing.Size(26, 23);
            this.btn2Stop.TabIndex = 7;
            this.btn2Stop.Text = "n";
            this.btn2Stop.UseVisualStyleBackColor = true;
            this.btn2Stop.Click += new System.EventHandler(this.btn2Stop_Click);
            // 
            // btn3Start
            // 
            this.btn3Start.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn3Start.ForeColor = System.Drawing.Color.LimeGreen;
            this.btn3Start.Location = new System.Drawing.Point(78, 66);
            this.btn3Start.Name = "btn3Start";
            this.btn3Start.Size = new System.Drawing.Size(27, 23);
            this.btn3Start.TabIndex = 5;
            this.btn3Start.Text = "u";
            this.btn3Start.UseVisualStyleBackColor = true;
            this.btn3Start.Click += new System.EventHandler(this.btn3Start_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 72);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "GameServer";
            // 
            // btn2Start
            // 
            this.btn2Start.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn2Start.ForeColor = System.Drawing.Color.LimeGreen;
            this.btn2Start.Location = new System.Drawing.Point(78, 41);
            this.btn2Start.Name = "btn2Start";
            this.btn2Start.Size = new System.Drawing.Size(27, 23);
            this.btn2Start.TabIndex = 3;
            this.btn2Start.Text = "u";
            this.btn2Start.UseVisualStyleBackColor = true;
            this.btn2Start.Click += new System.EventHandler(this.btn2Start_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "GameCenter";
            // 
            // btn1Start
            // 
            this.btn1Start.Font = new System.Drawing.Font("Wingdings 3", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btn1Start.ForeColor = System.Drawing.Color.LimeGreen;
            this.btn1Start.Location = new System.Drawing.Point(78, 15);
            this.btn1Start.Name = "btn1Start";
            this.btn1Start.Size = new System.Drawing.Size(27, 23);
            this.btn1Start.TabIndex = 1;
            this.btn1Start.Text = "u";
            this.btn1Start.UseVisualStyleBackColor = true;
            this.btn1Start.Click += new System.EventHandler(this.btn1Start_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "Bishop";
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.BalloonTipText = "iiii";
            this.notifyIcon1.ContextMenuStrip = this.contextMenuStrip1;
            this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
            this.notifyIcon1.Text = "剑三服务器管理工具";
            this.notifyIcon1.Visible = true;
            this.notifyIcon1.DoubleClick += new System.EventHandler(this.notifyIcon1_DoubleClick);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.ContextMenuStrip = this.contextMenuStrip1;
            this.groupBox2.Controls.Add(this.btnOpenClientFolder);
            this.groupBox2.Controls.Add(this.btnOpenDir);
            this.groupBox2.Location = new System.Drawing.Point(327, 6);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(139, 150);
            this.groupBox2.TabIndex = 12;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "快捷命令";
            // 
            // btnOpenClientFolder
            // 
            this.btnOpenClientFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOpenClientFolder.Location = new System.Drawing.Point(7, 41);
            this.btnOpenClientFolder.Name = "btnOpenClientFolder";
            this.btnOpenClientFolder.Size = new System.Drawing.Size(127, 23);
            this.btnOpenClientFolder.TabIndex = 2;
            this.btnOpenClientFolder.Text = "打开Client目录";
            this.btnOpenClientFolder.UseVisualStyleBackColor = true;
            this.btnOpenClientFolder.Click += new System.EventHandler(this.btnOpenClientFolder_Click);
            // 
            // btnOpenDir
            // 
            this.btnOpenDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOpenDir.Location = new System.Drawing.Point(7, 15);
            this.btnOpenDir.Name = "btnOpenDir";
            this.btnOpenDir.Size = new System.Drawing.Size(127, 23);
            this.btnOpenDir.TabIndex = 0;
            this.btnOpenDir.Text = "打开Server目录";
            this.btnOpenDir.UseVisualStyleBackColor = true;
            this.btnOpenDir.Click += new System.EventHandler(this.btnOpenDir_Click);
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(474, 168);
            this.ContextMenuStrip = this.contextMenuStrip1;
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBox2);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FrmMain";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FrmMain_FormClosed);
            this.SizeChanged += new System.EventHandler(this.FrmMain_SizeChanged);
            this.Activated += new System.EventHandler(this.FrmMain_Activated);
            this.Move += new System.EventHandler(this.FrmMain_Move);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FrmMain_FormClosing);
            this.Load += new System.EventHandler(this.FrmMain_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.contextMenuStrip1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.Button btn3Stop;
        private System.Windows.Forms.Button btn2Stop;
        private System.Windows.Forms.Button btn1Stop;
        private System.Windows.Forms.Button btn3Start;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btn2Start;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btn1Start;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem popViewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem popSetupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem popExitToolStripMenuItem;
        private System.Windows.Forms.CheckBox cb3Top;
        private System.Windows.Forms.CheckBox cb2Top;
        private System.Windows.Forms.CheckBox cb1Top;
        private System.Windows.Forms.CheckBox cb3View;
        private System.Windows.Forms.CheckBox cb2View;
        private System.Windows.Forms.CheckBox cb1View;
        private System.Windows.Forms.Button btn3Restart;
        private System.Windows.Forms.Button btn2Restart;
        private System.Windows.Forms.Button btn1Restart;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnOpenDir;
        private System.Windows.Forms.Button btnSetup;
        private System.Windows.Forms.Button btnOpenClientFolder;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btn4Restart;
        private System.Windows.Forms.Button btn4Stop;
        private System.Windows.Forms.Button btn4Start;
    }
}

