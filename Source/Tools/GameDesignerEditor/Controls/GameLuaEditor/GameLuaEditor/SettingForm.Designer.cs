namespace GameLuaEditor
{
    partial class SettingForm
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
            this.btnEditor = new DevComponents.DotNetBar.ButtonX();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.fontPreviewLabel = new DevComponents.DotNetBar.LabelX();
            this.bSelectFont = new DevComponents.DotNetBar.ButtonX();
            this.labelX7 = new DevComponents.DotNetBar.LabelX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.txtPath = new System.Windows.Forms.TextBox();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.serverIPBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.messageRetryTimeBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.breakPointWaitCircleBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.bAttachDebugFile = new DevComponents.DotNetBar.ButtonX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.messageDelayBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.serverPathBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.btnOK = new DevComponents.DotNetBar.ButtonX();
            this.btnCancel = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.tabControlPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.btnEditor);
            this.panelEx1.Controls.Add(this.tabControl1);
            this.panelEx1.Controls.Add(this.btnOK);
            this.panelEx1.Controls.Add(this.btnCancel);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(422, 261);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            this.panelEx1.Text = "panelEx1";
            // 
            // btnEditor
            // 
            this.btnEditor.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnEditor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnEditor.Location = new System.Drawing.Point(14, 229);
            this.btnEditor.Name = "btnEditor";
            this.btnEditor.Size = new System.Drawing.Size(127, 23);
            this.btnEditor.TabIndex = 3;
            this.btnEditor.Text = "脚本编辑器设定(&S)";
            this.btnEditor.Click += new System.EventHandler(this.btnEditor_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.BackColor = System.Drawing.Color.Transparent;
            this.tabControl1.CanReorderTabs = true;
            this.tabControl1.Controls.Add(this.tabControlPanel2);
            this.tabControl1.Controls.Add(this.tabControlPanel1);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(422, 209);
            this.tabControl1.TabIndex = 3;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.tabItem1);
            this.tabControl1.Tabs.Add(this.tabItem2);
            this.tabControl1.Text = "tabControl1";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.buttonX3);
            this.tabControlPanel1.Controls.Add(this.fontPreviewLabel);
            this.tabControlPanel1.Controls.Add(this.bSelectFont);
            this.tabControlPanel1.Controls.Add(this.labelX7);
            this.tabControlPanel1.Controls.Add(this.buttonX1);
            this.tabControlPanel1.Controls.Add(this.labelX2);
            this.tabControlPanel1.Controls.Add(this.txtPath);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(422, 183);
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
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX3.Location = new System.Drawing.Point(336, 95);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(70, 20);
            this.buttonX3.TabIndex = 10;
            this.buttonX3.Text = "选择颜色";
            this.buttonX3.Visible = false;
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // fontPreviewLabel
            // 
            this.fontPreviewLabel.BackColor = System.Drawing.Color.Transparent;
            this.fontPreviewLabel.Location = new System.Drawing.Point(10, 95);
            this.fontPreviewLabel.Name = "fontPreviewLabel";
            this.fontPreviewLabel.Size = new System.Drawing.Size(200, 30);
            this.fontPreviewLabel.TabIndex = 9;
            this.fontPreviewLabel.Text = "脚本字体预览";
            // 
            // bSelectFont
            // 
            this.bSelectFont.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bSelectFont.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bSelectFont.Location = new System.Drawing.Point(259, 95);
            this.bSelectFont.Name = "bSelectFont";
            this.bSelectFont.Size = new System.Drawing.Size(70, 20);
            this.bSelectFont.TabIndex = 8;
            this.bSelectFont.Text = "选择字体";
            this.bSelectFont.Click += new System.EventHandler(this.bSelectFont_Click);
            // 
            // labelX7
            // 
            this.labelX7.BackColor = System.Drawing.Color.Transparent;
            this.labelX7.Location = new System.Drawing.Point(10, 69);
            this.labelX7.Name = "labelX7";
            this.labelX7.Size = new System.Drawing.Size(70, 20);
            this.labelX7.TabIndex = 7;
            this.labelX7.Text = "脚本字体";
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(356, 36);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(50, 20);
            this.buttonX1.TabIndex = 6;
            this.buttonX1.Text = "浏览";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // labelX2
            // 
            this.labelX2.BackColor = System.Drawing.Color.Transparent;
            this.labelX2.Location = new System.Drawing.Point(10, 10);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(70, 20);
            this.labelX2.TabIndex = 5;
            this.labelX2.Text = "客户端路径";
            // 
            // txtPath
            // 
            this.txtPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtPath.Location = new System.Drawing.Point(10, 36);
            this.txtPath.Name = "txtPath";
            this.txtPath.Size = new System.Drawing.Size(340, 21);
            this.txtPath.TabIndex = 0;
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "常规";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.serverIPBox);
            this.tabControlPanel2.Controls.Add(this.labelX6);
            this.tabControlPanel2.Controls.Add(this.messageRetryTimeBox);
            this.tabControlPanel2.Controls.Add(this.labelX5);
            this.tabControlPanel2.Controls.Add(this.breakPointWaitCircleBox);
            this.tabControlPanel2.Controls.Add(this.labelX4);
            this.tabControlPanel2.Controls.Add(this.bAttachDebugFile);
            this.tabControlPanel2.Controls.Add(this.labelX3);
            this.tabControlPanel2.Controls.Add(this.messageDelayBox);
            this.tabControlPanel2.Controls.Add(this.buttonX2);
            this.tabControlPanel2.Controls.Add(this.serverPathBox);
            this.tabControlPanel2.Controls.Add(this.labelX1);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(422, 183);
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
            // serverIPBox
            // 
            // 
            // 
            // 
            this.serverIPBox.Border.Class = "TextBoxBorder";
            this.serverIPBox.Location = new System.Drawing.Point(86, 36);
            this.serverIPBox.Name = "serverIPBox";
            this.serverIPBox.Size = new System.Drawing.Size(150, 21);
            this.serverIPBox.TabIndex = 17;
            // 
            // labelX6
            // 
            this.labelX6.BackColor = System.Drawing.Color.Transparent;
            this.labelX6.Location = new System.Drawing.Point(10, 36);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(70, 20);
            this.labelX6.TabIndex = 16;
            this.labelX6.Text = "服务端IP";
            // 
            // messageRetryTimeBox
            // 
            // 
            // 
            // 
            this.messageRetryTimeBox.Border.Class = "TextBoxBorder";
            this.messageRetryTimeBox.Location = new System.Drawing.Point(176, 107);
            this.messageRetryTimeBox.Name = "messageRetryTimeBox";
            this.messageRetryTimeBox.Size = new System.Drawing.Size(100, 21);
            this.messageRetryTimeBox.TabIndex = 15;
            this.messageRetryTimeBox.Text = "2";
            // 
            // labelX5
            // 
            this.labelX5.BackColor = System.Drawing.Color.Transparent;
            this.labelX5.Location = new System.Drawing.Point(10, 107);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(160, 20);
            this.labelX5.TabIndex = 14;
            this.labelX5.Text = "接收调试消息超时重试次数";
            // 
            // breakPointWaitCircleBox
            // 
            // 
            // 
            // 
            this.breakPointWaitCircleBox.Border.Class = "TextBoxBorder";
            this.breakPointWaitCircleBox.Location = new System.Drawing.Point(176, 145);
            this.breakPointWaitCircleBox.Name = "breakPointWaitCircleBox";
            this.breakPointWaitCircleBox.Size = new System.Drawing.Size(100, 21);
            this.breakPointWaitCircleBox.TabIndex = 13;
            this.breakPointWaitCircleBox.Text = "512";
            // 
            // labelX4
            // 
            this.labelX4.BackColor = System.Drawing.Color.Transparent;
            this.labelX4.Location = new System.Drawing.Point(10, 145);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(160, 20);
            this.labelX4.TabIndex = 12;
            this.labelX4.Text = "断点触发检查间隔（毫秒）";
            // 
            // bAttachDebugFile
            // 
            this.bAttachDebugFile.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAttachDebugFile.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAttachDebugFile.Location = new System.Drawing.Point(310, 10);
            this.bAttachDebugFile.Name = "bAttachDebugFile";
            this.bAttachDebugFile.Size = new System.Drawing.Size(100, 20);
            this.bAttachDebugFile.TabIndex = 11;
            this.bAttachDebugFile.Text = "挂接调试文件";
            this.bAttachDebugFile.Click += new System.EventHandler(this.bAttachDebugFile_Click);
            // 
            // labelX3
            // 
            this.labelX3.BackColor = System.Drawing.Color.Transparent;
            this.labelX3.Location = new System.Drawing.Point(10, 72);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(160, 20);
            this.labelX3.TabIndex = 10;
            this.labelX3.Text = "接收调试消息延迟（毫秒）";
            // 
            // messageDelayBox
            // 
            // 
            // 
            // 
            this.messageDelayBox.Border.Class = "TextBoxBorder";
            this.messageDelayBox.Location = new System.Drawing.Point(176, 72);
            this.messageDelayBox.Name = "messageDelayBox";
            this.messageDelayBox.Size = new System.Drawing.Size(100, 21);
            this.messageDelayBox.TabIndex = 9;
            this.messageDelayBox.Text = "128";
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.Location = new System.Drawing.Point(242, 10);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(50, 20);
            this.buttonX2.TabIndex = 8;
            this.buttonX2.Text = "浏览";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // serverPathBox
            // 
            // 
            // 
            // 
            this.serverPathBox.Border.Class = "TextBoxBorder";
            this.serverPathBox.Location = new System.Drawing.Point(86, 10);
            this.serverPathBox.Name = "serverPathBox";
            this.serverPathBox.Size = new System.Drawing.Size(150, 21);
            this.serverPathBox.TabIndex = 6;
            // 
            // labelX1
            // 
            this.labelX1.BackColor = System.Drawing.Color.Transparent;
            this.labelX1.Location = new System.Drawing.Point(10, 10);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(70, 20);
            this.labelX1.TabIndex = 5;
            this.labelX1.Text = "服务端路径";
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel2;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "调试";
            // 
            // btnOK
            // 
            this.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(254, 229);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 2;
            this.btnOK.Text = "确定(&O)";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(335, 229);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "取消(&C)";
            this.btnCancel.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.btnCancel_PreviewKeyDown);
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // SettingForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(422, 261);
            this.Controls.Add(this.panelEx1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SettingForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "系统设置";
            this.panelEx1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            this.tabControlPanel1.PerformLayout();
            this.tabControlPanel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnOK;
        private DevComponents.DotNetBar.ButtonX btnCancel;
        private System.Windows.Forms.TextBox txtPath;
        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.ButtonX btnEditor;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.Controls.TextBoxX messageDelayBox;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.Controls.TextBoxX serverPathBox;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.Controls.TextBoxX breakPointWaitCircleBox;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.ButtonX bAttachDebugFile;
        private DevComponents.DotNetBar.Controls.TextBoxX messageRetryTimeBox;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.Controls.TextBoxX serverIPBox;
        private DevComponents.DotNetBar.LabelX labelX6;
        private DevComponents.DotNetBar.ButtonX bSelectFont;
        private DevComponents.DotNetBar.LabelX labelX7;
        private DevComponents.DotNetBar.LabelX fontPreviewLabel;
        private DevComponents.DotNetBar.ButtonX buttonX3;
    }
}