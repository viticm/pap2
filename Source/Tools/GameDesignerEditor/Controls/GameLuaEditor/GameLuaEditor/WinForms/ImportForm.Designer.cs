namespace GameLuaEditor
{
    partial class ImportForm
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.selectAllBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.coverScriptBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.scriptListBox = new System.Windows.Forms.CheckedListBox();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.selectAllBox);
            this.panelEx1.Controls.Add(this.bCancel);
            this.panelEx1.Controls.Add(this.bOK);
            this.panelEx1.Controls.Add(this.coverScriptBox);
            this.panelEx1.Controls.Add(this.scriptListBox);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(456, 454);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // selectAllBox
            // 
            this.selectAllBox.Checked = true;
            this.selectAllBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.selectAllBox.CheckValue = "Y";
            this.selectAllBox.Location = new System.Drawing.Point(12, 368);
            this.selectAllBox.Name = "selectAllBox";
            this.selectAllBox.Size = new System.Drawing.Size(75, 20);
            this.selectAllBox.TabIndex = 4;
            this.selectAllBox.Text = "全部选中";
            this.selectAllBox.CheckedChanged += new System.EventHandler(this.selectAllBox_CheckedChanged);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bCancel.Location = new System.Drawing.Point(374, 409);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 3;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(289, 409);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(70, 20);
            this.bOK.TabIndex = 2;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // coverScriptBox
            // 
            this.coverScriptBox.Location = new System.Drawing.Point(93, 368);
            this.coverScriptBox.Name = "coverScriptBox";
            this.coverScriptBox.Size = new System.Drawing.Size(150, 20);
            this.coverScriptBox.TabIndex = 1;
            this.coverScriptBox.Text = "覆盖已经存在的脚本";
            // 
            // scriptListBox
            // 
            this.scriptListBox.CheckOnClick = true;
            this.scriptListBox.FormattingEnabled = true;
            this.scriptListBox.Location = new System.Drawing.Point(12, 12);
            this.scriptListBox.Name = "scriptListBox";
            this.scriptListBox.ScrollAlwaysVisible = true;
            this.scriptListBox.Size = new System.Drawing.Size(432, 340);
            this.scriptListBox.TabIndex = 0;
            // 
            // ImportForm
            // 
            this.AcceptButton = this.bOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bCancel;
            this.ClientSize = new System.Drawing.Size(456, 454);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ImportForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "脚本导入配置";
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private System.Windows.Forms.CheckedListBox scriptListBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX coverScriptBox;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.Controls.CheckBoxX selectAllBox;
    }
}