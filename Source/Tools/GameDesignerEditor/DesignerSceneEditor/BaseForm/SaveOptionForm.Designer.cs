namespace DesignerSceneEditor
{
	partial class SaveOptionForm
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
            this.radioAll = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.checkComputeCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkAppointCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkClearCell = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.radioRest = new System.Windows.Forms.RadioButton();
            this.radioNotRest = new System.Windows.Forms.RadioButton();
            this.checkBoxRest = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.radioIniOnly = new System.Windows.Forms.RadioButton();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.checkSaveLogics = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.panelEx1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.radioAll);
            this.panelEx1.Controls.Add(this.groupBox1);
            this.panelEx1.Controls.Add(this.groupBox2);
            this.panelEx1.Controls.Add(this.groupBox3);
            this.panelEx1.Controls.Add(this.radioIniOnly);
            this.panelEx1.Controls.Add(this.bCancel);
            this.panelEx1.Controls.Add(this.bOK);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(480, 397);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.Color = System.Drawing.Color.LightSteelBlue;
            this.panelEx1.Style.BackColor2.Color = System.Drawing.Color.LightSteelBlue;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // radioAll
            // 
            this.radioAll.AutoSize = true;
            this.radioAll.Location = new System.Drawing.Point(27, 77);
            this.radioAll.Name = "radioAll";
            this.radioAll.Size = new System.Drawing.Size(143, 16);
            this.radioAll.TabIndex = 4;
            this.radioAll.TabStop = true;
            this.radioAll.Text = "保存编辑器和逻辑数据";
            this.radioAll.UseVisualStyleBackColor = true;
            this.radioAll.CheckedChanged += new System.EventHandler(this.radioAll_CheckedChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.checkComputeCell);
            this.groupBox1.Controls.Add(this.checkAppointCell);
            this.groupBox1.Controls.Add(this.checkClearCell);
            this.groupBox1.Location = new System.Drawing.Point(67, 161);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(370, 91);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            // 
            // checkComputeCell
            // 
            this.checkComputeCell.Location = new System.Drawing.Point(16, 16);
            this.checkComputeCell.Name = "checkComputeCell";
            this.checkComputeCell.Size = new System.Drawing.Size(137, 23);
            this.checkComputeCell.TabIndex = 2;
            this.checkComputeCell.Text = "计算障碍";
            this.checkComputeCell.CheckedChanged += new System.EventHandler(this.cbCalcColi_CheckedChanged);
            // 
            // checkAppointCell
            // 
            this.checkAppointCell.Enabled = false;
            this.checkAppointCell.Location = new System.Drawing.Point(48, 37);
            this.checkAppointCell.Name = "checkAppointCell";
            this.checkAppointCell.Size = new System.Drawing.Size(322, 23);
            this.checkAppointCell.TabIndex = 3;
            this.checkAppointCell.Text = "只计算指定cell的障碍(指定区域外的障碍信息不变)";
            // 
            // checkClearCell
            // 
            this.checkClearCell.Enabled = false;
            this.checkClearCell.Location = new System.Drawing.Point(48, 59);
            this.checkClearCell.Name = "checkClearCell";
            this.checkClearCell.Size = new System.Drawing.Size(322, 23);
            this.checkClearCell.TabIndex = 3;
            this.checkClearCell.Text = "计算时清除旧的障碍信息";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.radioRest);
            this.groupBox3.Controls.Add(this.radioNotRest);
            this.groupBox3.Controls.Add(this.checkBoxRest);
            this.groupBox3.Location = new System.Drawing.Point(67, 258);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(370, 75);
            this.groupBox3.TabIndex = 6;
            this.groupBox3.TabStop = false;
            // 
            // radioRest
            // 
            this.radioRest.AutoSize = true;
            this.radioRest.Location = new System.Drawing.Point(48, 49);
            this.radioRest.Name = "radioRest";
            this.radioRest.Size = new System.Drawing.Size(47, 16);
            this.radioRest.TabIndex = 5;
            this.radioRest.TabStop = true;
            this.radioRest.Text = "安全";
            this.radioRest.UseVisualStyleBackColor = true;
            // 
            // radioNotRest
            // 
            this.radioNotRest.AutoSize = true;
            this.radioNotRest.Location = new System.Drawing.Point(120, 49);
            this.radioNotRest.Name = "radioNotRest";
            this.radioNotRest.Size = new System.Drawing.Size(59, 16);
            this.radioNotRest.TabIndex = 5;
            this.radioNotRest.TabStop = true;
            this.radioNotRest.Text = "非安全";
            this.radioNotRest.UseVisualStyleBackColor = true;
            // 
            // checkBoxRest
            // 
            this.checkBoxRest.Location = new System.Drawing.Point(16, 20);
            this.checkBoxRest.Name = "checkBoxRest";
            this.checkBoxRest.Size = new System.Drawing.Size(255, 23);
            this.checkBoxRest.TabIndex = 2;
            this.checkBoxRest.Text = "更新整个场景安全(休息)区域信息";
            this.checkBoxRest.CheckedChanged += new System.EventHandler(this.cbRest_CheckedChanged);
            // 
            // radioIniOnly
            // 
            this.radioIniOnly.AutoSize = true;
            this.radioIniOnly.Location = new System.Drawing.Point(27, 25);
            this.radioIniOnly.Name = "radioIniOnly";
            this.radioIniOnly.Size = new System.Drawing.Size(203, 16);
            this.radioIniOnly.TabIndex = 4;
            this.radioIniOnly.TabStop = true;
            this.radioIniOnly.Text = "只保存编辑器数据(.Map.Logical)";
            this.radioIniOnly.UseVisualStyleBackColor = true;
            this.radioIniOnly.CheckedChanged += new System.EventHandler(this.radioIniOnly_CheckedChanged);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.Location = new System.Drawing.Point(362, 357);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(75, 23);
            this.bCancel.TabIndex = 1;
            this.bCancel.Text = "取消(&C)";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.Location = new System.Drawing.Point(273, 357);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(75, 23);
            this.bOK.TabIndex = 0;
            this.bOK.Text = "确定(&O)";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.checkSaveLogics);
            this.groupBox2.Location = new System.Drawing.Point(67, 99);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(370, 56);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            // 
            // checkSaveLogics
            // 
            this.checkSaveLogics.Location = new System.Drawing.Point(16, 20);
            this.checkSaveLogics.Name = "checkSaveLogics";
            this.checkSaveLogics.Size = new System.Drawing.Size(348, 23);
            this.checkSaveLogics.TabIndex = 2;
            this.checkSaveLogics.Text = "保存逻辑文件（整个场景的逻辑信息都有效）";
            // 
            // SaveOptionForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(480, 397);
            this.Controls.Add(this.panelEx1);
            this.Name = "SaveOptionForm";
            this.Text = "保存选项";
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		#endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkClearCell;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkComputeCell;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bOK;
        private System.Windows.Forms.RadioButton radioAll;
        private System.Windows.Forms.RadioButton radioIniOnly;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkAppointCell;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxRest;
        private System.Windows.Forms.RadioButton radioNotRest;
        private System.Windows.Forms.RadioButton radioRest;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkSaveLogics;
	}
}