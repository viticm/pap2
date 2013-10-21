namespace FlowChartEditor
{
    partial class InnerChartReferenceForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.innerChartTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.infoContainer = new System.Windows.Forms.SplitContainer();
            this.bSearchReference = new DevComponents.DotNetBar.ButtonX();
            this.bClose = new DevComponents.DotNetBar.ButtonX();
            this.bViewNode = new DevComponents.DotNetBar.ButtonX();
            this.diagramInfoView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            ((System.ComponentModel.ISupportInitialize)(this.innerChartTree)).BeginInit();
            this.infoContainer.Panel1.SuspendLayout();
            this.infoContainer.Panel2.SuspendLayout();
            this.infoContainer.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.diagramInfoView)).BeginInit();
            this.SuspendLayout();
            // 
            // innerChartTree
            // 
            this.innerChartTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.innerChartTree.AllowDrop = true;
            this.innerChartTree.BackColor = System.Drawing.SystemColors.Window;
            this.innerChartTree.Dock = System.Windows.Forms.DockStyle.Fill;
            this.innerChartTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.innerChartTree.Location = new System.Drawing.Point(0, 0);
            this.innerChartTree.Name = "innerChartTree";
            this.innerChartTree.NodesConnector = this.nodeConnector1;
            this.innerChartTree.NodeStyle = this.elementStyle1;
            this.innerChartTree.PathSeparator = ";";
            this.innerChartTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.innerChartTree.Size = new System.Drawing.Size(211, 432);
            this.innerChartTree.Styles.Add(this.elementStyle1);
            this.innerChartTree.SuspendPaint = false;
            this.innerChartTree.TabIndex = 0;
            this.innerChartTree.Text = "advTree1";
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
            // infoContainer
            // 
            this.infoContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.infoContainer.Location = new System.Drawing.Point(0, 0);
            this.infoContainer.Name = "infoContainer";
            // 
            // infoContainer.Panel1
            // 
            this.infoContainer.Panel1.Controls.Add(this.innerChartTree);
            // 
            // infoContainer.Panel2
            // 
            this.infoContainer.Panel2.Controls.Add(this.bSearchReference);
            this.infoContainer.Panel2.Controls.Add(this.bClose);
            this.infoContainer.Panel2.Controls.Add(this.bViewNode);
            this.infoContainer.Panel2.Controls.Add(this.diagramInfoView);
            this.infoContainer.Size = new System.Drawing.Size(634, 432);
            this.infoContainer.SplitterDistance = 211;
            this.infoContainer.TabIndex = 1;
            // 
            // bSearchReference
            // 
            this.bSearchReference.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bSearchReference.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bSearchReference.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bSearchReference.Location = new System.Drawing.Point(14, 391);
            this.bSearchReference.Name = "bSearchReference";
            this.bSearchReference.Size = new System.Drawing.Size(70, 20);
            this.bSearchReference.TabIndex = 3;
            this.bSearchReference.Text = "查找引用";
            this.bSearchReference.Click += new System.EventHandler(this.bSearchReference_Click);
            // 
            // bClose
            // 
            this.bClose.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bClose.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bClose.Location = new System.Drawing.Point(337, 391);
            this.bClose.Name = "bClose";
            this.bClose.Size = new System.Drawing.Size(70, 20);
            this.bClose.TabIndex = 2;
            this.bClose.Text = "关闭";
            this.bClose.Click += new System.EventHandler(this.bClose_Click);
            // 
            // bViewNode
            // 
            this.bViewNode.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bViewNode.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bViewNode.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bViewNode.Location = new System.Drawing.Point(261, 391);
            this.bViewNode.Name = "bViewNode";
            this.bViewNode.Size = new System.Drawing.Size(70, 20);
            this.bViewNode.TabIndex = 1;
            this.bViewNode.Text = "查看绘图";
            this.bViewNode.Click += new System.EventHandler(this.bViewNode_Click);
            // 
            // diagramInfoView
            // 
            this.diagramInfoView.AllowUserToAddRows = false;
            this.diagramInfoView.AllowUserToDeleteRows = false;
            this.diagramInfoView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.diagramInfoView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.diagramInfoView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.diagramInfoView.DefaultCellStyle = dataGridViewCellStyle2;
            this.diagramInfoView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.diagramInfoView.Location = new System.Drawing.Point(3, 3);
            this.diagramInfoView.MultiSelect = false;
            this.diagramInfoView.Name = "diagramInfoView";
            this.diagramInfoView.ReadOnly = true;
            this.diagramInfoView.RowTemplate.Height = 23;
            this.diagramInfoView.Size = new System.Drawing.Size(413, 353);
            this.diagramInfoView.TabIndex = 0;
            // 
            // InnerChartReferenceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bClose;
            this.ClientSize = new System.Drawing.Size(634, 432);
            this.Controls.Add(this.infoContainer);
            this.Name = "InnerChartReferenceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "子绘图引用关系";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.InnerChartReferenceForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.innerChartTree)).EndInit();
            this.infoContainer.Panel1.ResumeLayout(false);
            this.infoContainer.Panel2.ResumeLayout(false);
            this.infoContainer.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.diagramInfoView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.AdvTree.AdvTree innerChartTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private System.Windows.Forms.SplitContainer infoContainer;
        private DevComponents.DotNetBar.ButtonX bViewNode;
        private DevComponents.DotNetBar.Controls.DataGridViewX diagramInfoView;
        private DevComponents.DotNetBar.ButtonX bClose;
        private DevComponents.DotNetBar.ButtonX bSearchReference;
    }
}