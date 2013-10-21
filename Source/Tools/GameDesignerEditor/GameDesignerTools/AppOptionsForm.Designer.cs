namespace GameDesingerTools
{
    partial class AppOptionsForm
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
            this.panelEx3 = new DevComponents.DotNetBar.PanelEx();
            this.buttonCancel = new DevComponents.DotNetBar.ButtonX();
            this.buttonOK = new DevComponents.DotNetBar.ButtonX();
            this.tabControl2 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel4 = new DevComponents.DotNetBar.TabControlPanel();
            this.label6 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.tabItem4 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel3 = new DevComponents.DotNetBar.TabControlPanel();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonX4 = new DevComponents.DotNetBar.ButtonX();
            this.textBox3DEnginePath = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.设置文件的根目录 = new System.Windows.Forms.Label();
            this.browRootDir = new DevComponents.DotNetBar.ButtonX();
            this.txtBoxRootDir = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.tabItem3 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.label4 = new System.Windows.Forms.Label();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.buttonComparer = new DevComponents.DotNetBar.ButtonX();
            this.label1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.textBoxXComparer = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.textBoxX2 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.panelEx3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl2)).BeginInit();
            this.tabControl2.SuspendLayout();
            this.tabControlPanel4.SuspendLayout();
            this.tabControlPanel3.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.tabControlPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx3
            // 
            this.panelEx3.Controls.Add(this.buttonCancel);
            this.panelEx3.Controls.Add(this.buttonOK);
            this.panelEx3.Controls.Add(this.tabControl2);
            this.panelEx3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx3.Location = new System.Drawing.Point(0, 0);
            this.panelEx3.Name = "panelEx3";
            this.panelEx3.Size = new System.Drawing.Size(394, 248);
            this.panelEx3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx3.Style.GradientAngle = 90;
            this.panelEx3.StyleMouseDown.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.StyleMouseOver.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.TabIndex = 4;
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(286, 211);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 1;
            this.buttonCancel.Text = "取消(&C)";
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonOK
            // 
            this.buttonOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOK.Location = new System.Drawing.Point(191, 211);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 0;
            this.buttonOK.Text = "确定(&O)";
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // tabControl2
            // 
            this.tabControl2.BackColor = System.Drawing.Color.Transparent;
            this.tabControl2.CanReorderTabs = true;
            this.tabControl2.Controls.Add(this.tabControlPanel4);
            this.tabControl2.Controls.Add(this.tabControlPanel3);
            this.tabControl2.Controls.Add(this.tabControlPanel1);
            this.tabControl2.Controls.Add(this.tabControlPanel2);
            this.tabControl2.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabControl2.Location = new System.Drawing.Point(0, 0);
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl2.SelectedTabIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size(394, 192);
            this.tabControl2.TabIndex = 0;
            this.tabControl2.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl2.Tabs.Add(this.tabItem3);
            this.tabControl2.Tabs.Add(this.tabItem1);
            this.tabControl2.Tabs.Add(this.tabItem2);
            this.tabControl2.Tabs.Add(this.tabItem4);
            this.tabControl2.Text = "tabControl2";
            // 
            // tabControlPanel4
            // 
            this.tabControlPanel4.Controls.Add(this.label6);
            this.tabControlPanel4.Controls.Add(this.label3);
            this.tabControlPanel4.Controls.Add(this.comboBox1);
            this.tabControlPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel4.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel4.Name = "tabControlPanel4";
            this.tabControlPanel4.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel4.Size = new System.Drawing.Size(394, 166);
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
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.BackColor = System.Drawing.Color.Transparent;
            this.label6.Location = new System.Drawing.Point(92, 136);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(17, 12);
            this.label6.TabIndex = 2;
            this.label6.Text = "  ";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(92, 43);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(125, 12);
            this.label3.TabIndex = 1;
            this.label3.Text = "请选择你使用的数据库";
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(94, 77);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(193, 20);
            this.comboBox1.TabIndex = 0;
            this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // tabItem4
            // 
            this.tabItem4.AttachedControl = this.tabControlPanel4;
            this.tabItem4.Name = "tabItem4";
            this.tabItem4.Text = "数据库";
            // 
            // tabControlPanel3
            // 
            this.tabControlPanel3.Controls.Add(this.label2);
            this.tabControlPanel3.Controls.Add(this.buttonX4);
            this.tabControlPanel3.Controls.Add(this.textBox3DEnginePath);
            this.tabControlPanel3.Controls.Add(this.设置文件的根目录);
            this.tabControlPanel3.Controls.Add(this.browRootDir);
            this.tabControlPanel3.Controls.Add(this.txtBoxRootDir);
            this.tabControlPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel3.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel3.Name = "tabControlPanel3";
            this.tabControlPanel3.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel3.Size = new System.Drawing.Size(394, 166);
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
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(33, 64);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "设置3D引擎路径:";
            // 
            // buttonX4
            // 
            this.buttonX4.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX4.Location = new System.Drawing.Point(320, 80);
            this.buttonX4.Name = "buttonX4";
            this.buttonX4.Size = new System.Drawing.Size(42, 23);
            this.buttonX4.TabIndex = 3;
            this.buttonX4.Text = "...";
            this.buttonX4.Click += new System.EventHandler(this.buttonX4_Click);
            // 
            // textBox3DEnginePath
            // 
            // 
            // 
            // 
            this.textBox3DEnginePath.Border.Class = "TextBoxBorder";
            this.textBox3DEnginePath.Location = new System.Drawing.Point(35, 82);
            this.textBox3DEnginePath.Name = "textBox3DEnginePath";
            this.textBox3DEnginePath.Size = new System.Drawing.Size(279, 21);
            this.textBox3DEnginePath.TabIndex = 2;
            // 
            // 设置文件的根目录
            // 
            this.设置文件的根目录.AutoSize = true;
            this.设置文件的根目录.BackColor = System.Drawing.Color.Transparent;
            this.设置文件的根目录.Location = new System.Drawing.Point(32, 14);
            this.设置文件的根目录.Name = "设置文件的根目录";
            this.设置文件的根目录.Size = new System.Drawing.Size(107, 12);
            this.设置文件的根目录.TabIndex = 1;
            this.设置文件的根目录.Text = "设置文件的根路径:";
            // 
            // browRootDir
            // 
            this.browRootDir.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.browRootDir.Location = new System.Drawing.Point(319, 30);
            this.browRootDir.Name = "browRootDir";
            this.browRootDir.Size = new System.Drawing.Size(42, 23);
            this.browRootDir.TabIndex = 1;
            this.browRootDir.Text = "...";
            this.browRootDir.Click += new System.EventHandler(this.browRootDir_Click);
            // 
            // txtBoxRootDir
            // 
            // 
            // 
            // 
            this.txtBoxRootDir.Border.Class = "TextBoxBorder";
            this.txtBoxRootDir.Location = new System.Drawing.Point(34, 32);
            this.txtBoxRootDir.Name = "txtBoxRootDir";
            this.txtBoxRootDir.Size = new System.Drawing.Size(279, 21);
            this.txtBoxRootDir.TabIndex = 0;
            // 
            // tabItem3
            // 
            this.tabItem3.AttachedControl = this.tabControlPanel3;
            this.tabItem3.Name = "tabItem3";
            this.tabItem3.Text = "常规";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.label4);
            this.tabControlPanel1.Controls.Add(this.buttonX1);
            this.tabControlPanel1.Controls.Add(this.textBoxX1);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(394, 166);
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
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(32, 18);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(131, 12);
            this.label4.TabIndex = 2;
            this.label4.Text = "导出数据的保存根路径:";
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Location = new System.Drawing.Point(319, 34);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(42, 23);
            this.buttonX1.TabIndex = 1;
            this.buttonX1.Text = "...";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(34, 36);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(279, 21);
            this.textBoxX1.TabIndex = 0;
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "导出数据";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.buttonComparer);
            this.tabControlPanel2.Controls.Add(this.label1);
            this.tabControlPanel2.Controls.Add(this.label5);
            this.tabControlPanel2.Controls.Add(this.textBoxXComparer);
            this.tabControlPanel2.Controls.Add(this.textBoxX2);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(394, 166);
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
            // buttonComparer
            // 
            this.buttonComparer.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonComparer.Location = new System.Drawing.Point(331, 105);
            this.buttonComparer.Name = "buttonComparer";
            this.buttonComparer.Size = new System.Drawing.Size(42, 23);
            this.buttonComparer.TabIndex = 7;
            this.buttonComparer.Text = "...";
            this.buttonComparer.Click += new System.EventHandler(this.buttonComparer_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(32, 80);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(113, 12);
            this.label1.TabIndex = 5;
            this.label1.Text = "Beyond Compare路径";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.Location = new System.Drawing.Point(32, 18);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(119, 12);
            this.label5.TabIndex = 5;
            this.label5.Text = "tmrRender的Interval";
            // 
            // textBoxXComparer
            // 
            // 
            // 
            // 
            this.textBoxXComparer.Border.Class = "TextBoxBorder";
            this.textBoxXComparer.Location = new System.Drawing.Point(34, 105);
            this.textBoxXComparer.Name = "textBoxXComparer";
            this.textBoxXComparer.Size = new System.Drawing.Size(279, 21);
            this.textBoxXComparer.TabIndex = 3;
            // 
            // textBoxX2
            // 
            // 
            // 
            // 
            this.textBoxX2.Border.Class = "TextBoxBorder";
            this.textBoxX2.Location = new System.Drawing.Point(34, 36);
            this.textBoxX2.Name = "textBoxX2";
            this.textBoxX2.Size = new System.Drawing.Size(117, 21);
            this.textBoxX2.TabIndex = 3;
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel2;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "高级";
            // 
            // AppOptionsForm
            // 
            this.AcceptButton = this.buttonOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonCancel;
            this.ClientSize = new System.Drawing.Size(394, 248);
            this.Controls.Add(this.panelEx3);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AppOptionsForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "选项";
            this.TopMost = true;
            this.panelEx3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl2)).EndInit();
            this.tabControl2.ResumeLayout(false);
            this.tabControlPanel4.ResumeLayout(false);
            this.tabControlPanel4.PerformLayout();
            this.tabControlPanel3.ResumeLayout(false);
            this.tabControlPanel3.PerformLayout();
            this.tabControlPanel1.ResumeLayout(false);
            this.tabControlPanel1.PerformLayout();
            this.tabControlPanel2.ResumeLayout(false);
            this.tabControlPanel2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public DevComponents.DotNetBar.DotNetBarManager dotNetBarMgr;
        private DevComponents.DotNetBar.PanelEx panelEx3;
        private DevComponents.DotNetBar.TabControl tabControl2;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.ButtonX buttonCancel;
        private DevComponents.DotNetBar.ButtonX buttonOK;
        private System.Windows.Forms.Label label5;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX2;
        private System.Windows.Forms.Label label4;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel3;
        private System.Windows.Forms.Label 设置文件的根目录;
        private DevComponents.DotNetBar.ButtonX browRootDir;
        private DevComponents.DotNetBar.Controls.TextBoxX txtBoxRootDir;
        private DevComponents.DotNetBar.TabItem tabItem3;
        private System.Windows.Forms.Label label2;
        private DevComponents.DotNetBar.ButtonX buttonX4;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox3DEnginePath;
        private System.Windows.Forms.Label label1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxXComparer;
        private DevComponents.DotNetBar.ButtonX buttonComparer;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox comboBox1;
        private DevComponents.DotNetBar.TabItem tabItem4;
        private System.Windows.Forms.Label label6;
    }
}