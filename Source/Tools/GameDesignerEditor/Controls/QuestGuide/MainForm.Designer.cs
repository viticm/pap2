namespace QuestGuide
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
            this.components = new System.ComponentModel.Container();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.dgvQuestInfo = new System.Windows.Forms.DataGridView();
            this.advTree1 = new DevComponents.AdvTree.AdvTree();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.nodeRoot = new DevComponents.AdvTree.Node();
            this.nodeAccept = new DevComponents.AdvTree.Node();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.nodeMake = new DevComponents.AdvTree.Node();
            this.nodeFinish = new DevComponents.AdvTree.Node();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.elementStyle3 = new DevComponents.DotNetBar.ElementStyle();
            this.elementStyle4 = new DevComponents.DotNetBar.ElementStyle();
            this.elementStyle5 = new DevComponents.DotNetBar.ElementStyle();
            this.barStatus = new DevComponents.DotNetBar.Bar();
            this.lbItemChoosed = new DevComponents.DotNetBar.LabelItem();
            this.lbXYorGrayIds = new DevComponents.DotNetBar.LabelItem();
            this.combxItemMapId = new DevComponents.DotNetBar.ComboBoxItem();
            this.btnDeleteQuestValue = new DevComponents.DotNetBar.ButtonItem();
            this.lbItemCursor = new DevComponents.DotNetBar.LabelItem();
            this.lbItemGray = new DevComponents.DotNetBar.LabelItem();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.panelEx1 = new QuestGuide.Canvas();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvQuestInfo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.advTree1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.barStatus)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.CanReorderTabs = true;
            this.tabControl1.Controls.Add(this.tabControlPanel1);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(895, 651);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.tabItem1);
            this.tabControl1.Text = "tabControl1";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.splitContainer1);
            this.tabControlPanel1.Controls.Add(this.barStatus);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(895, 625);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 1;
            this.tabControlPanel1.TabItem = this.tabItem1;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(1, 1);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.panelEx1);
            this.splitContainer1.Panel2.SizeChanged += new System.EventHandler(this.splitContainer1_Panel2_SizeChanged);
            this.splitContainer1.Size = new System.Drawing.Size(893, 597);
            this.splitContainer1.SplitterDistance = 238;
            this.splitContainer1.TabIndex = 2;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.dgvQuestInfo);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.advTree1);
            this.splitContainer2.Size = new System.Drawing.Size(238, 597);
            this.splitContainer2.SplitterDistance = 305;
            this.splitContainer2.TabIndex = 0;
            // 
            // dgvQuestInfo
            // 
            this.dgvQuestInfo.AllowUserToAddRows = false;
            this.dgvQuestInfo.AllowUserToDeleteRows = false;
            this.dgvQuestInfo.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dgvQuestInfo.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            this.dgvQuestInfo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvQuestInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dgvQuestInfo.Location = new System.Drawing.Point(0, 0);
            this.dgvQuestInfo.Name = "dgvQuestInfo";
            this.dgvQuestInfo.ReadOnly = true;
            this.dgvQuestInfo.RowHeadersVisible = false;
            this.dgvQuestInfo.RowTemplate.Height = 23;
            this.dgvQuestInfo.Size = new System.Drawing.Size(238, 305);
            this.dgvQuestInfo.TabIndex = 0;
            // 
            // advTree1
            // 
            this.advTree1.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.advTree1.AllowDrop = true;
            this.advTree1.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.advTree1.BackgroundStyle.Class = "TreeBorderKey";
            this.advTree1.ContextMenuStrip = this.contextMenuStrip1;
            this.advTree1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.advTree1.DragDropEnabled = false;
            this.advTree1.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.advTree1.Location = new System.Drawing.Point(0, 0);
            this.advTree1.Name = "advTree1";
            this.advTree1.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.nodeRoot});
            this.advTree1.NodesConnector = this.nodeConnector1;
            this.advTree1.NodeStyle = this.elementStyle1;
            this.advTree1.PathSeparator = ";";
            this.advTree1.Size = new System.Drawing.Size(238, 288);
            this.advTree1.Styles.Add(this.elementStyle1);
            this.advTree1.Styles.Add(this.elementStyle2);
            this.advTree1.Styles.Add(this.elementStyle3);
            this.advTree1.Styles.Add(this.elementStyle4);
            this.advTree1.Styles.Add(this.elementStyle5);
            this.advTree1.SuspendPaint = false;
            this.advTree1.TabIndex = 2;
            this.advTree1.NodeClick += new DevComponents.AdvTree.TreeNodeMouseEventHandler(this.advTree1_NodeClick);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // nodeRoot
            // 
            this.nodeRoot.Expanded = true;
            this.nodeRoot.Name = "nodeRoot";
            this.nodeRoot.Nodes.AddRange(new DevComponents.AdvTree.Node[] {
            this.nodeAccept,
            this.nodeMake,
            this.nodeFinish});
            this.nodeRoot.Text = "任务名称";
            // 
            // nodeAccept
            // 
            this.nodeAccept.Expanded = true;
            this.nodeAccept.Name = "nodeAccept";
            this.nodeAccept.Style = this.elementStyle1;
            this.nodeAccept.Text = "接任务";
            // 
            // elementStyle1
            // 
            this.elementStyle1.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle1.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle1.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle1.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle1.Name = "elementStyle1";
            this.elementStyle1.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // nodeMake
            // 
            this.nodeMake.Expanded = true;
            this.nodeMake.Name = "nodeMake";
            this.nodeMake.Style = this.elementStyle1;
            this.nodeMake.Text = "做任务";
            // 
            // nodeFinish
            // 
            this.nodeFinish.Expanded = true;
            this.nodeFinish.Name = "nodeFinish";
            this.nodeFinish.Style = this.elementStyle1;
            this.nodeFinish.Text = "交任务";
            // 
            // nodeConnector1
            // 
            this.nodeConnector1.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle2
            // 
            this.elementStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(248)))), ((int)(((byte)(242)))), ((int)(((byte)(226)))));
            this.elementStyle2.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(232)))), ((int)(((byte)(209)))), ((int)(((byte)(153)))));
            this.elementStyle2.BackColorGradientAngle = 90;
            this.elementStyle2.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle2.BorderBottomWidth = 1;
            this.elementStyle2.BorderColor = System.Drawing.Color.DarkGray;
            this.elementStyle2.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle2.BorderLeftWidth = 1;
            this.elementStyle2.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle2.BorderRightWidth = 1;
            this.elementStyle2.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle2.BorderTopWidth = 1;
            this.elementStyle2.CornerDiameter = 4;
            this.elementStyle2.Description = "Tan";
            this.elementStyle2.Name = "elementStyle2";
            this.elementStyle2.PaddingBottom = 1;
            this.elementStyle2.PaddingLeft = 1;
            this.elementStyle2.PaddingRight = 1;
            this.elementStyle2.PaddingTop = 1;
            this.elementStyle2.TextColor = System.Drawing.Color.Black;
            // 
            // elementStyle3
            // 
            this.elementStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(249)))), ((int)(((byte)(225)))), ((int)(((byte)(226)))));
            this.elementStyle3.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(238)))), ((int)(((byte)(149)))), ((int)(((byte)(151)))));
            this.elementStyle3.BackColorGradientAngle = 90;
            this.elementStyle3.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle3.BorderBottomWidth = 1;
            this.elementStyle3.BorderColor = System.Drawing.Color.DarkGray;
            this.elementStyle3.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle3.BorderLeftWidth = 1;
            this.elementStyle3.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle3.BorderRightWidth = 1;
            this.elementStyle3.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle3.BorderTopWidth = 1;
            this.elementStyle3.CornerDiameter = 4;
            this.elementStyle3.Description = "Red";
            this.elementStyle3.Name = "elementStyle3";
            this.elementStyle3.PaddingBottom = 1;
            this.elementStyle3.PaddingLeft = 1;
            this.elementStyle3.PaddingRight = 1;
            this.elementStyle3.PaddingTop = 1;
            this.elementStyle3.TextColor = System.Drawing.Color.Black;
            // 
            // elementStyle4
            // 
            this.elementStyle4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(221)))), ((int)(((byte)(230)))), ((int)(((byte)(247)))));
            this.elementStyle4.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(138)))), ((int)(((byte)(168)))), ((int)(((byte)(228)))));
            this.elementStyle4.BackColorGradientAngle = 90;
            this.elementStyle4.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle4.BorderBottomWidth = 1;
            this.elementStyle4.BorderColor = System.Drawing.Color.DarkGray;
            this.elementStyle4.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle4.BorderLeftWidth = 1;
            this.elementStyle4.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle4.BorderRightWidth = 1;
            this.elementStyle4.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle4.BorderTopWidth = 1;
            this.elementStyle4.CornerDiameter = 4;
            this.elementStyle4.Description = "Blue";
            this.elementStyle4.Name = "elementStyle4";
            this.elementStyle4.PaddingBottom = 1;
            this.elementStyle4.PaddingLeft = 1;
            this.elementStyle4.PaddingRight = 1;
            this.elementStyle4.PaddingTop = 1;
            this.elementStyle4.TextColor = System.Drawing.Color.Black;
            // 
            // elementStyle5
            // 
            this.elementStyle5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(244)))), ((int)(((byte)(213)))));
            this.elementStyle5.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(216)))), ((int)(((byte)(105)))));
            this.elementStyle5.BackColorGradientAngle = 90;
            this.elementStyle5.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle5.BorderBottomWidth = 1;
            this.elementStyle5.BorderColor = System.Drawing.Color.DarkGray;
            this.elementStyle5.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle5.BorderLeftWidth = 1;
            this.elementStyle5.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle5.BorderRightWidth = 1;
            this.elementStyle5.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.elementStyle5.BorderTopWidth = 1;
            this.elementStyle5.CornerDiameter = 4;
            this.elementStyle5.Description = "Yellow";
            this.elementStyle5.Name = "elementStyle5";
            this.elementStyle5.PaddingBottom = 1;
            this.elementStyle5.PaddingLeft = 1;
            this.elementStyle5.PaddingRight = 1;
            this.elementStyle5.PaddingTop = 1;
            this.elementStyle5.TextColor = System.Drawing.Color.Black;
            // 
            // barStatus
            // 
            this.barStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.barStatus.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.barStatus.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.lbItemChoosed,
            this.lbXYorGrayIds,
            this.combxItemMapId,
            this.btnDeleteQuestValue,
            this.lbItemCursor,
            this.lbItemGray});
            this.barStatus.Location = new System.Drawing.Point(1, 598);
            this.barStatus.Name = "barStatus";
            this.barStatus.Size = new System.Drawing.Size(893, 26);
            this.barStatus.Stretch = true;
            this.barStatus.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.barStatus.TabIndex = 0;
            this.barStatus.TabStop = false;
            this.barStatus.Text = "bar1";
            // 
            // lbItemChoosed
            // 
            this.lbItemChoosed.Name = "lbItemChoosed";
            this.lbItemChoosed.Text = "当前节点：";
            // 
            // lbXYorGrayIds
            // 
            this.lbXYorGrayIds.Name = "lbXYorGrayIds";
            this.lbXYorGrayIds.Text = "节点信息";
            // 
            // combxItemMapId
            // 
            this.combxItemMapId.ComboWidth = 90;
            this.combxItemMapId.DropDownHeight = 106;
            this.combxItemMapId.Enabled = false;
            this.combxItemMapId.Name = "combxItemMapId";
            this.combxItemMapId.Stretch = true;
            this.combxItemMapId.SelectedIndexChanged += new System.EventHandler(this.combxItemMapId_SelectedIndexChanged);
            // 
            // btnDeleteQuestValue
            // 
            this.btnDeleteQuestValue.Enabled = false;
            this.btnDeleteQuestValue.ImagePaddingHorizontal = 8;
            this.btnDeleteQuestValue.Name = "btnDeleteQuestValue";
            this.btnDeleteQuestValue.Text = "删除该任务变量";
            this.btnDeleteQuestValue.Click += new System.EventHandler(this.btnDeleteQuestValue_Click);
            // 
            // lbItemCursor
            // 
            this.lbItemCursor.ItemAlignment = DevComponents.DotNetBar.eItemAlignment.Far;
            this.lbItemCursor.Name = "lbItemCursor";
            this.lbItemCursor.Text = "(0, 0)";
            this.lbItemCursor.TextAlignment = System.Drawing.StringAlignment.Center;
            // 
            // lbItemGray
            // 
            this.lbItemGray.Name = "lbItemGray";
            this.lbItemGray.Text = "灰度：";
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "任务指引";
            // 
            // panelEx1
            // 
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(651, 597);
            this.panelEx1.TabIndex = 1;
            this.panelEx1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panelEx1_MouseMove);
            this.panelEx1.SizeChanged += new System.EventHandler(this.panelEx1_SizeChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(895, 651);
            this.Controls.Add(this.tabControl1);
            this.Name = "MainForm";
            this.Text = "MainForm";
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvQuestInfo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.advTree1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.barStatus)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.Bar barStatus;
        private Canvas panelEx1;
        private System.Windows.Forms.SplitContainer splitContainer1;

        private DevComponents.DotNetBar.LabelItem lbItemCursor;
        private DevComponents.DotNetBar.ComboBoxItem combxItemMapId;
        private DevComponents.DotNetBar.LabelItem lbItemChoosed;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private DevComponents.AdvTree.AdvTree advTree1;
        private DevComponents.AdvTree.Node nodeRoot;
        private DevComponents.AdvTree.Node nodeAccept;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.AdvTree.Node nodeMake;
        private DevComponents.AdvTree.Node nodeFinish;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.DotNetBar.ElementStyle elementStyle3;
        private DevComponents.DotNetBar.ElementStyle elementStyle4;
        private DevComponents.DotNetBar.ElementStyle elementStyle5;
        private System.Windows.Forms.DataGridView dgvQuestInfo;
        private DevComponents.DotNetBar.ButtonItem btnDeleteQuestValue;
        private DevComponents.DotNetBar.LabelItem lbXYorGrayIds;
        private DevComponents.DotNetBar.LabelItem lbItemGray;
    }
}