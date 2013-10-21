namespace DesignerSceneEditor
{
	partial class ObjectSelectMaskForm
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
            this.checkboxAll = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cTrafficPoint = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cPoly = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cDoodad = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cRefreshPoint = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cWayPoint = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.cNpc = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.checkboxAll);
            this.panelEx1.Controls.Add(this.cTrafficPoint);
            this.panelEx1.Controls.Add(this.cPoly);
            this.panelEx1.Controls.Add(this.cDoodad);
            this.panelEx1.Controls.Add(this.cRefreshPoint);
            this.panelEx1.Controls.Add(this.cWayPoint);
            this.panelEx1.Controls.Add(this.cNpc);
            this.panelEx1.Controls.Add(this.buttonX2);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(235, 145);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // checkboxAll
            // 
            this.checkboxAll.Location = new System.Drawing.Point(12, 90);
            this.checkboxAll.Name = "checkboxAll";
            this.checkboxAll.Size = new System.Drawing.Size(116, 20);
            this.checkboxAll.TabIndex = 7;
            this.checkboxAll.Text = "其他非逻辑对象";
            // 
            // cTrafficPoint
            // 
            this.cTrafficPoint.Checked = true;
            this.cTrafficPoint.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cTrafficPoint.CheckValue = "Y";
            this.cTrafficPoint.Location = new System.Drawing.Point(118, 64);
            this.cTrafficPoint.Name = "cTrafficPoint";
            this.cTrafficPoint.Size = new System.Drawing.Size(100, 20);
            this.cTrafficPoint.TabIndex = 7;
            this.cTrafficPoint.Text = "交通点";
            // 
            // cPoly
            // 
            this.cPoly.Checked = true;
            this.cPoly.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cPoly.CheckValue = "Y";
            this.cPoly.Location = new System.Drawing.Point(118, 38);
            this.cPoly.Name = "cPoly";
            this.cPoly.Size = new System.Drawing.Size(100, 20);
            this.cPoly.TabIndex = 6;
            this.cPoly.Text = "多边形区域";
            // 
            // cDoodad
            // 
            this.cDoodad.Checked = true;
            this.cDoodad.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cDoodad.CheckValue = "Y";
            this.cDoodad.Location = new System.Drawing.Point(118, 12);
            this.cDoodad.Name = "cDoodad";
            this.cDoodad.Size = new System.Drawing.Size(100, 20);
            this.cDoodad.TabIndex = 5;
            this.cDoodad.Text = "Doodad";
            // 
            // cRefreshPoint
            // 
            this.cRefreshPoint.Checked = true;
            this.cRefreshPoint.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cRefreshPoint.CheckValue = "Y";
            this.cRefreshPoint.Location = new System.Drawing.Point(12, 64);
            this.cRefreshPoint.Name = "cRefreshPoint";
            this.cRefreshPoint.Size = new System.Drawing.Size(100, 20);
            this.cRefreshPoint.TabIndex = 4;
            this.cRefreshPoint.Text = "刷新点";
            // 
            // cWayPoint
            // 
            this.cWayPoint.Checked = true;
            this.cWayPoint.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cWayPoint.CheckValue = "Y";
            this.cWayPoint.Location = new System.Drawing.Point(12, 38);
            this.cWayPoint.Name = "cWayPoint";
            this.cWayPoint.Size = new System.Drawing.Size(100, 20);
            this.cWayPoint.TabIndex = 3;
            this.cWayPoint.Text = "路径点";
            // 
            // cNpc
            // 
            this.cNpc.Checked = true;
            this.cNpc.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cNpc.CheckValue = "Y";
            this.cNpc.Location = new System.Drawing.Point(12, 12);
            this.cNpc.Name = "cNpc";
            this.cNpc.Size = new System.Drawing.Size(100, 20);
            this.cNpc.TabIndex = 2;
            this.cNpc.Text = "NPC";
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(148, 113);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 1;
            this.buttonX2.Text = "取消";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(71, 113);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 0;
            this.buttonX1.Text = "确定";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // ObjectSelectMaskForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(235, 145);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ObjectSelectMaskForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "对象选择过滤";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ObjectSelectMaskForm_FormClosing);
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		#endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.Controls.CheckBoxX cTrafficPoint;
        private DevComponents.DotNetBar.Controls.CheckBoxX cPoly;
        private DevComponents.DotNetBar.Controls.CheckBoxX cDoodad;
        private DevComponents.DotNetBar.Controls.CheckBoxX cRefreshPoint;
        private DevComponents.DotNetBar.Controls.CheckBoxX cWayPoint;
        private DevComponents.DotNetBar.Controls.CheckBoxX cNpc;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkboxAll;
	}
}