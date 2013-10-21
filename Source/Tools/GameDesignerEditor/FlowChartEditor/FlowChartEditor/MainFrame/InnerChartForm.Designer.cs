namespace FlowChartEditor
{
    partial class InnerChartForm
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
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.innerChartBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.lDescription = new DevComponents.DotNetBar.LabelX();
            this.bView = new DevComponents.DotNetBar.ButtonX();
            this.SuspendLayout();
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(200, 115);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(70, 20);
            this.bOK.TabIndex = 1;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.Location = new System.Drawing.Point(287, 115);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 2;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // innerChartBox
            // 
            this.innerChartBox.DisplayMember = "Text";
            this.innerChartBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.innerChartBox.FormattingEnabled = true;
            this.innerChartBox.ItemHeight = 15;
            this.innerChartBox.Location = new System.Drawing.Point(107, 22);
            this.innerChartBox.Name = "innerChartBox";
            this.innerChartBox.Size = new System.Drawing.Size(250, 21);
            this.innerChartBox.TabIndex = 3;
            this.innerChartBox.SelectedIndexChanged += new System.EventHandler(this.innerChartBox_SelectedIndexChanged);
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(12, 22);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(75, 20);
            this.labelX1.TabIndex = 4;
            this.labelX1.Text = "子绘图";
            // 
            // lDescription
            // 
            this.lDescription.Location = new System.Drawing.Point(12, 62);
            this.lDescription.Name = "lDescription";
            this.lDescription.Size = new System.Drawing.Size(345, 20);
            this.lDescription.TabIndex = 5;
            this.lDescription.Text = "子绘图描述";
            // 
            // bView
            // 
            this.bView.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bView.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bView.Location = new System.Drawing.Point(12, 115);
            this.bView.Name = "bView";
            this.bView.Size = new System.Drawing.Size(70, 20);
            this.bView.TabIndex = 6;
            this.bView.Text = "查看";
            this.bView.Click += new System.EventHandler(this.bView_Click);
            // 
            // InnerChartForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(374, 155);
            this.Controls.Add(this.bView);
            this.Controls.Add(this.lDescription);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.innerChartBox);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bOK);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InnerChartForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "子绘图";
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.Controls.ComboBoxEx innerChartBox;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX lDescription;
        private DevComponents.DotNetBar.ButtonX bView;
    }
}