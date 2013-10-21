namespace FlowChartEditor
{
    partial class InterfaceNodeForm
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
            this.nameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bInput = new System.Windows.Forms.RadioButton();
            this.bOutput = new System.Windows.Forms.RadioButton();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.SuspendLayout();
            // 
            // nameBox
            // 
            // 
            // 
            // 
            this.nameBox.Border.Class = "TextBoxBorder";
            this.nameBox.Location = new System.Drawing.Point(68, 20);
            this.nameBox.Name = "nameBox";
            this.nameBox.Size = new System.Drawing.Size(150, 21);
            this.nameBox.TabIndex = 0;
            // 
            // bInput
            // 
            this.bInput.AutoSize = true;
            this.bInput.Checked = true;
            this.bInput.Location = new System.Drawing.Point(68, 55);
            this.bInput.Name = "bInput";
            this.bInput.Size = new System.Drawing.Size(47, 16);
            this.bInput.TabIndex = 1;
            this.bInput.TabStop = true;
            this.bInput.Text = "输入";
            this.bInput.UseVisualStyleBackColor = true;
            // 
            // bOutput
            // 
            this.bOutput.AutoSize = true;
            this.bOutput.Location = new System.Drawing.Point(121, 55);
            this.bOutput.Name = "bOutput";
            this.bOutput.Size = new System.Drawing.Size(47, 16);
            this.bOutput.TabIndex = 2;
            this.bOutput.Text = "输出";
            this.bOutput.UseVisualStyleBackColor = true;
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(68, 108);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 3;
            this.buttonX1.Text = "确定";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.Location = new System.Drawing.Point(148, 108);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 4;
            this.buttonX2.Text = "取消";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(12, 20);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(50, 20);
            this.labelX1.TabIndex = 5;
            this.labelX1.Text = "名称";
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(12, 55);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(50, 20);
            this.labelX2.TabIndex = 6;
            this.labelX2.Text = "类型";
            // 
            // InterfaceNodeForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(234, 148);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.Controls.Add(this.bOutput);
            this.Controls.Add(this.bInput);
            this.Controls.Add(this.nameBox);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InterfaceNodeForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "接口";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.Controls.TextBoxX nameBox;
        private System.Windows.Forms.RadioButton bInput;
        private System.Windows.Forms.RadioButton bOutput;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX2;
    }
}