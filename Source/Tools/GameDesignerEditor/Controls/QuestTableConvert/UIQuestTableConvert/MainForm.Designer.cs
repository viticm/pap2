namespace UIQuestTableConvert
{
    partial class MainForm
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
            this.fileNameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bBrowseFile = new DevComponents.DotNetBar.ButtonX();
            this.bConvert = new DevComponents.DotNetBar.ButtonX();
            this.bExit = new DevComponents.DotNetBar.ButtonX();
            this.folderNameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bBrowseFolder = new DevComponents.DotNetBar.ButtonX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.SuspendLayout();
            // 
            // fileNameBox
            // 
            // 
            // 
            // 
            this.fileNameBox.Border.Class = "TextBoxBorder";
            this.fileNameBox.Location = new System.Drawing.Point(12, 38);
            this.fileNameBox.Name = "fileNameBox";
            this.fileNameBox.Size = new System.Drawing.Size(300, 21);
            this.fileNameBox.TabIndex = 0;
            this.fileNameBox.WatermarkText = "任务表文件路径";
            // 
            // bBrowseFile
            // 
            this.bBrowseFile.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bBrowseFile.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bBrowseFile.Location = new System.Drawing.Point(318, 38);
            this.bBrowseFile.Name = "bBrowseFile";
            this.bBrowseFile.Size = new System.Drawing.Size(70, 20);
            this.bBrowseFile.TabIndex = 1;
            this.bBrowseFile.Text = "浏览";
            this.bBrowseFile.Click += new System.EventHandler(this.bBrowse_Click);
            // 
            // bConvert
            // 
            this.bConvert.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bConvert.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bConvert.Location = new System.Drawing.Point(242, 143);
            this.bConvert.Name = "bConvert";
            this.bConvert.Size = new System.Drawing.Size(70, 20);
            this.bConvert.TabIndex = 2;
            this.bConvert.Text = "开始拆分";
            this.bConvert.Click += new System.EventHandler(this.bConvert_Click);
            // 
            // bExit
            // 
            this.bExit.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bExit.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bExit.Location = new System.Drawing.Point(318, 143);
            this.bExit.Name = "bExit";
            this.bExit.Size = new System.Drawing.Size(70, 20);
            this.bExit.TabIndex = 3;
            this.bExit.Text = "退出";
            this.bExit.Click += new System.EventHandler(this.bExit_Click);
            // 
            // folderNameBox
            // 
            // 
            // 
            // 
            this.folderNameBox.Border.Class = "TextBoxBorder";
            this.folderNameBox.Location = new System.Drawing.Point(12, 101);
            this.folderNameBox.Name = "folderNameBox";
            this.folderNameBox.Size = new System.Drawing.Size(300, 21);
            this.folderNameBox.TabIndex = 4;
            this.folderNameBox.WatermarkText = "文件输出目录";
            // 
            // bBrowseFolder
            // 
            this.bBrowseFolder.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bBrowseFolder.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bBrowseFolder.Location = new System.Drawing.Point(318, 101);
            this.bBrowseFolder.Name = "bBrowseFolder";
            this.bBrowseFolder.Size = new System.Drawing.Size(70, 20);
            this.bBrowseFolder.TabIndex = 5;
            this.bBrowseFolder.Text = "浏览";
            this.bBrowseFolder.Click += new System.EventHandler(this.bBrowseFolder_Click);
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(12, 12);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(100, 20);
            this.labelX1.TabIndex = 6;
            this.labelX1.Text = "任务数据表路径";
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(12, 75);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(100, 20);
            this.labelX2.TabIndex = 7;
            this.labelX2.Text = "文件输出目录";
            // 
            // MainForm
            // 
            this.AcceptButton = this.bBrowseFile;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bExit;
            this.ClientSize = new System.Drawing.Size(403, 185);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.bBrowseFolder);
            this.Controls.Add(this.folderNameBox);
            this.Controls.Add(this.bExit);
            this.Controls.Add(this.bConvert);
            this.Controls.Add(this.bBrowseFile);
            this.Controls.Add(this.fileNameBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "任务表拆分工具";
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.TextBoxX fileNameBox;
        private DevComponents.DotNetBar.ButtonX bBrowseFile;
        private DevComponents.DotNetBar.ButtonX bConvert;
        private DevComponents.DotNetBar.ButtonX bExit;
        private DevComponents.DotNetBar.Controls.TextBoxX folderNameBox;
        private DevComponents.DotNetBar.ButtonX bBrowseFolder;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX2;
    }
}

