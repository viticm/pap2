namespace DesignerSceneEditor
{
	partial class ThumbnailForm
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
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.CancelBut = new DevComponents.DotNetBar.ButtonX();
            this.SaveThumbnailBut = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX2 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.checkBoxX1 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.groupPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupPanel1
            // 
            this.groupPanel1.BackColor = System.Drawing.Color.Transparent;
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.CancelBut);
            this.groupPanel1.Controls.Add(this.SaveThumbnailBut);
            this.groupPanel1.Controls.Add(this.textBoxX2);
            this.groupPanel1.Controls.Add(this.textBoxX1);
            this.groupPanel1.Controls.Add(this.labelX2);
            this.groupPanel1.Controls.Add(this.checkBoxX1);
            this.groupPanel1.Controls.Add(this.labelX1);
            this.groupPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupPanel1.Location = new System.Drawing.Point(0, 0);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(365, 185);
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
            this.groupPanel1.TabIndex = 0;
            // 
            // CancelBut
            // 
            this.CancelBut.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.CancelBut.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.CancelBut.Location = new System.Drawing.Point(251, 147);
            this.CancelBut.Name = "CancelBut";
            this.CancelBut.Size = new System.Drawing.Size(75, 23);
            this.CancelBut.TabIndex = 6;
            this.CancelBut.Text = "关闭";
            this.CancelBut.Click += new System.EventHandler(this.CancelBut_Click);
            // 
            // SaveThumbnailBut
            // 
            this.SaveThumbnailBut.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.SaveThumbnailBut.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.SaveThumbnailBut.Location = new System.Drawing.Point(133, 147);
            this.SaveThumbnailBut.Name = "SaveThumbnailBut";
            this.SaveThumbnailBut.Size = new System.Drawing.Size(75, 23);
            this.SaveThumbnailBut.TabIndex = 5;
            this.SaveThumbnailBut.Text = "保存";
            this.SaveThumbnailBut.Click += new System.EventHandler(this.SaveThumbnailBut_Click);
            // 
            // textBoxX2
            // 
            // 
            // 
            // 
            this.textBoxX2.Border.Class = "TextBoxBorder";
            this.textBoxX2.Location = new System.Drawing.Point(251, 84);
            this.textBoxX2.Name = "textBoxX2";
            this.textBoxX2.Size = new System.Drawing.Size(71, 21);
            this.textBoxX2.TabIndex = 4;
            this.textBoxX2.Text = "1";
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(251, 20);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(71, 21);
            this.textBoxX1.TabIndex = 3;
            this.textBoxX1.Text = "2";
            // 
            // labelX2
            // 
            this.labelX2.BackColor = System.Drawing.Color.Transparent;
            this.labelX2.Location = new System.Drawing.Point(122, 84);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(99, 23);
            this.labelX2.TabIndex = 2;
            this.labelX2.Text = "树叶缩放比例";
            // 
            // checkBoxX1
            // 
            this.checkBoxX1.Checked = true;
            this.checkBoxX1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxX1.CheckValue = "Y";
            this.checkBoxX1.Location = new System.Drawing.Point(30, 84);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new System.Drawing.Size(86, 23);
            this.checkBoxX1.TabIndex = 1;
            this.checkBoxX1.Text = "是否渲染树";
            this.checkBoxX1.CheckedChanged += new System.EventHandler(this.checkBoxX1_CheckedChanged);
            // 
            // labelX1
            // 
            this.labelX1.BackColor = System.Drawing.Color.Transparent;
            this.labelX1.Location = new System.Drawing.Point(30, 20);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(191, 23);
            this.labelX1.TabIndex = 0;
            this.labelX1.Text = "单个Cell所占像素（不能大于2）";
            // 
            // ThumbnailForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(365, 185);
            this.Controls.Add(this.groupPanel1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ThumbnailForm";
            this.Text = "生成场景缩略图";
            this.groupPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		#endregion

        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.ButtonX CancelBut;
        private DevComponents.DotNetBar.ButtonX SaveThumbnailBut;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX2;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX1;
        private DevComponents.DotNetBar.LabelX labelX1;
	}
}