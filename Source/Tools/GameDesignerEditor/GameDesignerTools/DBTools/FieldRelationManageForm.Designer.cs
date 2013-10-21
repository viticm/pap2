namespace GameDesingerTools
{
    partial class FieldRelationForm
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
            this.bRemoveField = new DevComponents.DotNetBar.ButtonX();
            this.bAddField = new DevComponents.DotNetBar.ButtonX();
            this.bAddRelation = new DevComponents.DotNetBar.ButtonX();
            this.bRemoveRelation = new DevComponents.DotNetBar.ButtonX();
            this.keyFieldTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector2 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.bSearch = new DevComponents.DotNetBar.ButtonX();
            this.searchFieldBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.relationTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.relationContainer = new System.Windows.Forms.SplitContainer();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.keyFieldTree)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.relationTree)).BeginInit();
            this.splitContainer1.SuspendLayout();
            this.relationContainer.Panel1.SuspendLayout();
            this.relationContainer.Panel2.SuspendLayout();
            this.relationContainer.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.relationContainer);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(652, 479);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // bRemoveField
            // 
            this.bRemoveField.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRemoveField.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bRemoveField.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRemoveField.Location = new System.Drawing.Point(117, 404);
            this.bRemoveField.Name = "bRemoveField";
            this.bRemoveField.Size = new System.Drawing.Size(100, 20);
            this.bRemoveField.TabIndex = 10;
            this.bRemoveField.Text = "删除关联字段";
            this.bRemoveField.Click += new System.EventHandler(this.bRemoveField_Click);
            // 
            // bAddField
            // 
            this.bAddField.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAddField.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bAddField.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAddField.Location = new System.Drawing.Point(11, 404);
            this.bAddField.Name = "bAddField";
            this.bAddField.Size = new System.Drawing.Size(100, 20);
            this.bAddField.TabIndex = 9;
            this.bAddField.Text = "添加关联字段";
            this.bAddField.Click += new System.EventHandler(this.bAddField_Click);
            // 
            // bAddRelation
            // 
            this.bAddRelation.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAddRelation.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bAddRelation.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAddRelation.Location = new System.Drawing.Point(12, 404);
            this.bAddRelation.Name = "bAddRelation";
            this.bAddRelation.Size = new System.Drawing.Size(100, 20);
            this.bAddRelation.TabIndex = 8;
            this.bAddRelation.Text = "添加关联关系";
            this.bAddRelation.Click += new System.EventHandler(this.bAddRelation_Click);
            // 
            // bRemoveRelation
            // 
            this.bRemoveRelation.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRemoveRelation.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bRemoveRelation.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRemoveRelation.Location = new System.Drawing.Point(118, 404);
            this.bRemoveRelation.Name = "bRemoveRelation";
            this.bRemoveRelation.Size = new System.Drawing.Size(100, 20);
            this.bRemoveRelation.TabIndex = 7;
            this.bRemoveRelation.Text = "删除关联关系";
            this.bRemoveRelation.Click += new System.EventHandler(this.bRemoveRelation_Click);
            // 
            // keyFieldTree
            // 
            this.keyFieldTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.keyFieldTree.AllowDrop = true;
            this.keyFieldTree.AllowUserToResizeColumns = false;
            this.keyFieldTree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.keyFieldTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.keyFieldTree.BackgroundStyle.Class = "TreeBorderKey";
            this.keyFieldTree.DragDropEnabled = false;
            this.keyFieldTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.keyFieldTree.Location = new System.Drawing.Point(11, 37);
            this.keyFieldTree.Name = "keyFieldTree";
            this.keyFieldTree.NodesConnector = this.nodeConnector2;
            this.keyFieldTree.NodeStyle = this.elementStyle2;
            this.keyFieldTree.PathSeparator = ";";
            this.keyFieldTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.keyFieldTree.Size = new System.Drawing.Size(300, 350);
            this.keyFieldTree.Styles.Add(this.elementStyle2);
            this.keyFieldTree.SuspendPaint = false;
            this.keyFieldTree.TabIndex = 6;
            this.keyFieldTree.Text = "advTree1";
            this.keyFieldTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.keyFieldTree_AfterNodeSelect);
            // 
            // nodeConnector2
            // 
            this.nodeConnector2.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle2
            // 
            this.elementStyle2.Name = "elementStyle2";
            this.elementStyle2.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // bSearch
            // 
            this.bSearch.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bSearch.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.bSearch.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bSearch.Location = new System.Drawing.Point(217, 440);
            this.bSearch.Name = "bSearch";
            this.bSearch.Size = new System.Drawing.Size(50, 20);
            this.bSearch.TabIndex = 5;
            this.bSearch.Text = "查找";
            this.bSearch.Click += new System.EventHandler(this.bSearch_Click);
            // 
            // searchFieldBox
            // 
            this.searchFieldBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            // 
            // 
            // 
            this.searchFieldBox.Border.Class = "TextBoxBorder";
            this.searchFieldBox.Location = new System.Drawing.Point(11, 440);
            this.searchFieldBox.Name = "searchFieldBox";
            this.searchFieldBox.Size = new System.Drawing.Size(200, 21);
            this.searchFieldBox.TabIndex = 4;
            this.searchFieldBox.WatermarkText = "输入查找字段值";
            // 
            // relationTree
            // 
            this.relationTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.relationTree.AllowDrop = true;
            this.relationTree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.relationTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.relationTree.BackgroundStyle.Class = "TreeBorderKey";
            this.relationTree.DragDropEnabled = false;
            this.relationTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.relationTree.Location = new System.Drawing.Point(12, 37);
            this.relationTree.Name = "relationTree";
            this.relationTree.NodesConnector = this.nodeConnector1;
            this.relationTree.NodeStyle = this.elementStyle1;
            this.relationTree.PathSeparator = ";";
            this.relationTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.relationTree.Size = new System.Drawing.Size(300, 350);
            this.relationTree.Styles.Add(this.elementStyle1);
            this.relationTree.SuspendPaint = false;
            this.relationTree.TabIndex = 3;
            this.relationTree.Text = "advTree1";
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
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(12, 11);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(70, 20);
            this.labelX2.TabIndex = 2;
            this.labelX2.Text = "关联关系";
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(11, 11);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(70, 20);
            this.labelX1.TabIndex = 1;
            this.labelX1.Text = "关联字段";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Location = new System.Drawing.Point(190, 535);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Size = new System.Drawing.Size(54, 116);
            this.splitContainer1.SplitterDistance = 25;
            this.splitContainer1.TabIndex = 11;
            // 
            // relationContainer
            // 
            this.relationContainer.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.relationContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.relationContainer.Location = new System.Drawing.Point(0, 0);
            this.relationContainer.Name = "relationContainer";
            // 
            // relationContainer.Panel1
            // 
            this.relationContainer.Panel1.Controls.Add(this.keyFieldTree);
            this.relationContainer.Panel1.Controls.Add(this.bRemoveField);
            this.relationContainer.Panel1.Controls.Add(this.labelX1);
            this.relationContainer.Panel1.Controls.Add(this.bSearch);
            this.relationContainer.Panel1.Controls.Add(this.bAddField);
            this.relationContainer.Panel1.Controls.Add(this.searchFieldBox);
            // 
            // relationContainer.Panel2
            // 
            this.relationContainer.Panel2.Controls.Add(this.bRemoveRelation);
            this.relationContainer.Panel2.Controls.Add(this.relationTree);
            this.relationContainer.Panel2.Controls.Add(this.bAddRelation);
            this.relationContainer.Panel2.Controls.Add(this.labelX2);
            this.relationContainer.Size = new System.Drawing.Size(652, 479);
            this.relationContainer.SplitterDistance = 323;
            this.relationContainer.TabIndex = 11;
            this.relationContainer.SizeChanged += new System.EventHandler(this.relationContainer_SizeChanged);
            // 
            // FieldRelationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(652, 479);
            this.Controls.Add(this.panelEx1);
            this.Name = "FieldRelationForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "字段关联关系";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FieldRelationForm_FormClosing);
            this.panelEx1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.keyFieldTree)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.relationTree)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.relationContainer.Panel1.ResumeLayout(false);
            this.relationContainer.Panel2.ResumeLayout(false);
            this.relationContainer.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.AdvTree.AdvTree relationTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.ButtonX bSearch;
        private DevComponents.DotNetBar.Controls.TextBoxX searchFieldBox;
        private DevComponents.AdvTree.AdvTree keyFieldTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector2;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.DotNetBar.ButtonX bAddRelation;
        private DevComponents.DotNetBar.ButtonX bRemoveRelation;
        private DevComponents.DotNetBar.ButtonX bAddField;
        private DevComponents.DotNetBar.ButtonX bRemoveField;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer relationContainer;
    }
}