namespace StandardNpcCreate
{
    partial class SourceIDSelectForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.editorIDRadioButton = new System.Windows.Forms.RadioButton();
            this.forceDetailRadioButton = new System.Windows.Forms.RadioButton();
            this.buttonX4 = new DevComponents.DotNetBar.ButtonX();
            this.searchConditionBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.editorTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.panelEx2 = new DevComponents.DotNetBar.PanelEx();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.modelDescriptionBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.descriptionLabel = new DevComponents.DotNetBar.LabelX();
            this.previewLabel = new DevComponents.DotNetBar.LabelX();
            this.editorView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.editorTree)).BeginInit();
            this.panelEx2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.editorView)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.editorIDRadioButton);
            this.panelEx1.Controls.Add(this.forceDetailRadioButton);
            this.panelEx1.Controls.Add(this.buttonX4);
            this.panelEx1.Controls.Add(this.searchConditionBox);
            this.panelEx1.Controls.Add(this.editorTree);
            this.panelEx1.Controls.Add(this.panelEx2);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(897, 485);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // editorIDRadioButton
            // 
            this.editorIDRadioButton.AutoSize = true;
            this.editorIDRadioButton.Location = new System.Drawing.Point(65, 446);
            this.editorIDRadioButton.Name = "editorIDRadioButton";
            this.editorIDRadioButton.Size = new System.Drawing.Size(47, 16);
            this.editorIDRadioButton.TabIndex = 12;
            this.editorIDRadioButton.Text = "编号";
            this.editorIDRadioButton.UseVisualStyleBackColor = true;
            this.editorIDRadioButton.CheckedChanged += new System.EventHandler(this.editorIDRadioButton_CheckedChanged);
            // 
            // forceDetailRadioButton
            // 
            this.forceDetailRadioButton.AutoSize = true;
            this.forceDetailRadioButton.Checked = true;
            this.forceDetailRadioButton.Location = new System.Drawing.Point(12, 446);
            this.forceDetailRadioButton.Name = "forceDetailRadioButton";
            this.forceDetailRadioButton.Size = new System.Drawing.Size(47, 16);
            this.forceDetailRadioButton.TabIndex = 11;
            this.forceDetailRadioButton.TabStop = true;
            this.forceDetailRadioButton.Text = "名称";
            this.forceDetailRadioButton.UseVisualStyleBackColor = true;
            this.forceDetailRadioButton.CheckedChanged += new System.EventHandler(this.forceDetailRadioButton_CheckedChanged);
            // 
            // buttonX4
            // 
            this.buttonX4.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX4.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX4.Location = new System.Drawing.Point(209, 446);
            this.buttonX4.Name = "buttonX4";
            this.buttonX4.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.F3);
            this.buttonX4.Size = new System.Drawing.Size(70, 20);
            this.buttonX4.TabIndex = 10;
            this.buttonX4.Text = "下一个(F3)";
            this.buttonX4.Click += new System.EventHandler(this.buttonX4_Click);
            // 
            // searchConditionBox
            // 
            // 
            // 
            // 
            this.searchConditionBox.Border.Class = "TextBoxBorder";
            this.searchConditionBox.Location = new System.Drawing.Point(3, 407);
            this.searchConditionBox.Name = "searchConditionBox";
            this.searchConditionBox.Size = new System.Drawing.Size(276, 21);
            this.searchConditionBox.TabIndex = 7;
            this.searchConditionBox.WatermarkText = "输入查找条件";
            // 
            // editorTree
            // 
            this.editorTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.editorTree.AllowDrop = true;
            this.editorTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.editorTree.BackgroundStyle.Class = "TreeBorderKey";
            this.editorTree.DragDropEnabled = false;
            this.editorTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.editorTree.Location = new System.Drawing.Point(0, 0);
            this.editorTree.Name = "editorTree";
            this.editorTree.NodesConnector = this.nodeConnector1;
            this.editorTree.NodeStyle = this.elementStyle1;
            this.editorTree.PathSeparator = ";";
            this.editorTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.editorTree.Size = new System.Drawing.Size(282, 386);
            this.editorTree.Styles.Add(this.elementStyle1);
            this.editorTree.SuspendPaint = false;
            this.editorTree.TabIndex = 4;
            this.editorTree.Text = "advTree1";
            this.editorTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTree1_AfterNodeSelect);
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
            // panelEx2
            // 
            this.panelEx2.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx2.Controls.Add(this.buttonX3);
            this.panelEx2.Controls.Add(this.modelDescriptionBox);
            this.panelEx2.Controls.Add(this.descriptionLabel);
            this.panelEx2.Controls.Add(this.previewLabel);
            this.panelEx2.Controls.Add(this.editorView);
            this.panelEx2.Controls.Add(this.buttonX1);
            this.panelEx2.Controls.Add(this.buttonX2);
            this.panelEx2.Location = new System.Drawing.Point(285, 0);
            this.panelEx2.Name = "panelEx2";
            this.panelEx2.Size = new System.Drawing.Size(611, 485);
            this.panelEx2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx2.Style.GradientAngle = 90;
            this.panelEx2.TabIndex = 6;
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX3.Location = new System.Drawing.Point(260, 446);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(90, 20);
            this.buttonX3.TabIndex = 7;
            this.buttonX3.Text = "更新模型描述";
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // modelDescriptionBox
            // 
            // 
            // 
            // 
            this.modelDescriptionBox.Border.Class = "TextBoxBorder";
            this.modelDescriptionBox.Location = new System.Drawing.Point(260, 341);
            this.modelDescriptionBox.Multiline = true;
            this.modelDescriptionBox.Name = "modelDescriptionBox";
            this.modelDescriptionBox.Size = new System.Drawing.Size(349, 87);
            this.modelDescriptionBox.TabIndex = 6;
            this.modelDescriptionBox.WatermarkText = "模型描述";
            // 
            // descriptionLabel
            // 
            this.descriptionLabel.Font = new System.Drawing.Font("SimSun", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.descriptionLabel.ForeColor = System.Drawing.Color.Red;
            this.descriptionLabel.Location = new System.Drawing.Point(6, 446);
            this.descriptionLabel.Name = "descriptionLabel";
            this.descriptionLabel.Size = new System.Drawing.Size(248, 20);
            this.descriptionLabel.TabIndex = 5;
            this.descriptionLabel.Text = "模型使用次数";
            // 
            // previewLabel
            // 
            this.previewLabel.Location = new System.Drawing.Point(6, 3);
            this.previewLabel.Name = "previewLabel";
            this.previewLabel.Size = new System.Drawing.Size(248, 425);
            this.previewLabel.TabIndex = 4;
            // 
            // editorView
            // 
            this.editorView.AllowUserToAddRows = false;
            this.editorView.AllowUserToDeleteRows = false;
            this.editorView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.editorView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.editorView.DefaultCellStyle = dataGridViewCellStyle1;
            this.editorView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.editorView.Location = new System.Drawing.Point(260, 3);
            this.editorView.MultiSelect = false;
            this.editorView.Name = "editorView";
            this.editorView.ReadOnly = true;
            this.editorView.RowTemplate.Height = 23;
            this.editorView.Size = new System.Drawing.Size(349, 332);
            this.editorView.TabIndex = 3;
            this.editorView.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridViewX1_CellMouseDown);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(444, 446);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 0;
            this.buttonX1.Text = "确定";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.Location = new System.Drawing.Point(529, 446);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 1;
            this.buttonX2.Text = "关闭";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // SourceIDSelectForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(897, 485);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SourceIDSelectForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "SourceID选择";
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.editorTree)).EndInit();
            this.panelEx2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.editorView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.Controls.DataGridViewX editorView;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.PanelEx panelEx2;
        private DevComponents.AdvTree.AdvTree editorTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.LabelX previewLabel;
        private DevComponents.DotNetBar.LabelX descriptionLabel;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.Controls.TextBoxX modelDescriptionBox;
        private DevComponents.DotNetBar.Controls.TextBoxX searchConditionBox;
        private DevComponents.DotNetBar.ButtonX buttonX4;
        private System.Windows.Forms.RadioButton editorIDRadioButton;
        private System.Windows.Forms.RadioButton forceDetailRadioButton;
    }
}