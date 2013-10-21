namespace GuideConfig
{
    partial class SortForm
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
            this.recordListBox = new System.Windows.Forms.ListBox();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bMoveUp = new DevComponents.DotNetBar.ButtonX();
            this.bMoveDown = new DevComponents.DotNetBar.ButtonX();
            this.SuspendLayout();
            // 
            // itemListBox
            // 
            this.recordListBox.FormattingEnabled = true;
            this.recordListBox.ItemHeight = 12;
            this.recordListBox.Location = new System.Drawing.Point(12, 12);
            this.recordListBox.Name = "itemListBox";
            this.recordListBox.ScrollAlwaysVisible = true;
            this.recordListBox.Size = new System.Drawing.Size(374, 184);
            this.recordListBox.TabIndex = 0;
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(240, 220);
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
            this.bCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bCancel.Location = new System.Drawing.Point(316, 220);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 2;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bMoveUp
            // 
            this.bMoveUp.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bMoveUp.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bMoveUp.Location = new System.Drawing.Point(12, 220);
            this.bMoveUp.Name = "bMoveUp";
            this.bMoveUp.Size = new System.Drawing.Size(70, 20);
            this.bMoveUp.TabIndex = 3;
            this.bMoveUp.Text = "上移";
            this.bMoveUp.Click += new System.EventHandler(this.bMoveUp_Click);
            // 
            // bMoveDown
            // 
            this.bMoveDown.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bMoveDown.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bMoveDown.Location = new System.Drawing.Point(88, 220);
            this.bMoveDown.Name = "bMoveDown";
            this.bMoveDown.Size = new System.Drawing.Size(70, 20);
            this.bMoveDown.TabIndex = 4;
            this.bMoveDown.Text = "下移";
            this.bMoveDown.Click += new System.EventHandler(this.bMoveDown_Click);
            // 
            // SortForm
            // 
            this.AcceptButton = this.bOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bCancel;
            this.ClientSize = new System.Drawing.Size(398, 260);
            this.Controls.Add(this.bMoveDown);
            this.Controls.Add(this.bMoveUp);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bOK);
            this.Controls.Add(this.recordListBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SortForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "排列显示顺序";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox recordListBox;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bMoveUp;
        private DevComponents.DotNetBar.ButtonX bMoveDown;
    }
}