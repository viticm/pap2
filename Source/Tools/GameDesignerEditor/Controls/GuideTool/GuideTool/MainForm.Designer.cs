namespace GuideTool
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
            this.guideTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.descriptionBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bStart = new DevComponents.DotNetBar.ButtonX();
            this.bExit = new DevComponents.DotNetBar.ButtonX();
            this.bLoadGuideInfo = new DevComponents.DotNetBar.ButtonX();
            ((System.ComponentModel.ISupportInitialize)(this.guideTree)).BeginInit();
            this.SuspendLayout();
            // 
            // guideTree
            // 
            this.guideTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.guideTree.AllowDrop = true;
            this.guideTree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.guideTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.guideTree.BackgroundStyle.Class = "TreeBorderKey";
            this.guideTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.guideTree.Location = new System.Drawing.Point(12, 12);
            this.guideTree.Name = "guideTree";
            this.guideTree.NodesConnector = this.nodeConnector1;
            this.guideTree.NodeStyle = this.elementStyle1;
            this.guideTree.PathSeparator = ";";
            this.guideTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.guideTree.Size = new System.Drawing.Size(522, 257);
            this.guideTree.Styles.Add(this.elementStyle1);
            this.guideTree.SuspendPaint = false;
            this.guideTree.TabIndex = 0;
            this.guideTree.Text = "advTree1";
            this.guideTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.guideTree_AfterNodeSelect);
            // 
            // nodeConnector1
            // 
            this.nodeConnector1.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle1
            // 
            this.elementStyle1.Name = "elementStyle1";
            this.elementStyle1.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // descriptionBox
            // 
            this.descriptionBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.descriptionBox.BackColor = System.Drawing.SystemColors.MenuBar;
            // 
            // 
            // 
            this.descriptionBox.Border.Class = "TextBoxBorder";
            this.descriptionBox.Location = new System.Drawing.Point(12, 275);
            this.descriptionBox.Multiline = true;
            this.descriptionBox.Name = "descriptionBox";
            this.descriptionBox.Size = new System.Drawing.Size(522, 76);
            this.descriptionBox.TabIndex = 1;
            // 
            // bStart
            // 
            this.bStart.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bStart.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bStart.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bStart.Location = new System.Drawing.Point(378, 374);
            this.bStart.Name = "bStart";
            this.bStart.Size = new System.Drawing.Size(70, 20);
            this.bStart.TabIndex = 2;
            this.bStart.Text = "开始";
            this.bStart.Click += new System.EventHandler(this.bStart_Click);
            // 
            // bExit
            // 
            this.bExit.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bExit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bExit.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bExit.Location = new System.Drawing.Point(464, 374);
            this.bExit.Name = "bExit";
            this.bExit.Size = new System.Drawing.Size(70, 20);
            this.bExit.TabIndex = 3;
            this.bExit.Text = "退出";
            this.bExit.Click += new System.EventHandler(this.bExit_Click);
            // 
            // bLoadGuideInfo
            // 
            this.bLoadGuideInfo.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bLoadGuideInfo.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bLoadGuideInfo.Location = new System.Drawing.Point(12, 374);
            this.bLoadGuideInfo.Name = "bLoadGuideInfo";
            this.bLoadGuideInfo.Size = new System.Drawing.Size(100, 20);
            this.bLoadGuideInfo.TabIndex = 4;
            this.bLoadGuideInfo.Text = "加载向导数据";
            this.bLoadGuideInfo.Click += new System.EventHandler(this.bLoadGuideInfo_Click);
            // 
            // MainForm
            // 
            this.AcceptButton = this.bStart;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bExit;
            this.ClientSize = new System.Drawing.Size(546, 417);
            this.Controls.Add(this.bLoadGuideInfo);
            this.Controls.Add(this.bExit);
            this.Controls.Add(this.bStart);
            this.Controls.Add(this.descriptionBox);
            this.Controls.Add(this.guideTree);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "向导选择";
            ((System.ComponentModel.ISupportInitialize)(this.guideTree)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.AdvTree.AdvTree guideTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.Controls.TextBoxX descriptionBox;
        private DevComponents.DotNetBar.ButtonX bStart;
        private DevComponents.DotNetBar.ButtonX bExit;
        private DevComponents.DotNetBar.ButtonX bLoadGuideInfo;
    }
}

