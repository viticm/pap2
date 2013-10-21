namespace DesignerSceneEditor{
	partial class InputForm
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
this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
this.panelEx1.SuspendLayout();
this.SuspendLayout();
// 
// panelEx1
// 
this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
this.panelEx1.Controls.Add(this.buttonX2);
this.panelEx1.Controls.Add(this.buttonX1);
this.panelEx1.Controls.Add(this.textBoxX1);
this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
this.panelEx1.Location = new System.Drawing.Point(0, 0);
this.panelEx1.Name = "panelEx1";
this.panelEx1.Size = new System.Drawing.Size(251, 92);
this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
this.panelEx1.Style.GradientAngle = 90;
this.panelEx1.TabIndex = 0;
// 
// textBoxX1
// 
// 
// 
// 
this.textBoxX1.Border.Class = "TextBoxBorder";
this.textBoxX1.Location = new System.Drawing.Point(12, 12);
this.textBoxX1.Name = "textBoxX1";
this.textBoxX1.Size = new System.Drawing.Size(227, 21);
this.textBoxX1.TabIndex = 0;
// 
// buttonX1
// 
this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
this.buttonX1.Location = new System.Drawing.Point(84, 55);
this.buttonX1.Name = "buttonX1";
this.buttonX1.Size = new System.Drawing.Size(70, 20);
this.buttonX1.TabIndex = 1;
this.buttonX1.Text = "确定";
this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
// 
// buttonX2
// 
this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
this.buttonX2.Location = new System.Drawing.Point(169, 55);
this.buttonX2.Name = "buttonX2";
this.buttonX2.Size = new System.Drawing.Size(70, 20);
this.buttonX2.TabIndex = 2;
this.buttonX2.Text = "取消";
this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
// 
// InputForm
// 
this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
this.ClientSize = new System.Drawing.Size(251, 92);
this.Controls.Add(this.panelEx1);
this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
this.MaximizeBox = false;
this.MinimizeBox = false;
this.Name = "InputForm";
this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
this.Text = "输入文本";
this.panelEx1.ResumeLayout(false);
this.ResumeLayout(false);

		}

		#endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
	}
}