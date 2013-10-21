namespace CustomLoadTree
{
    partial class Form1
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
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Node1");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("阿斯顿否", -2, -2, new System.Windows.Forms.TreeNode[] {
            treeNode1});
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Node3");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("Node5");
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("Node4", new System.Windows.Forms.TreeNode[] {
            treeNode4});
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("Node2", new System.Windows.Forms.TreeNode[] {
            treeNode3,
            treeNode5});
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem9 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem4 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem5 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem15 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem10 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem13 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem3 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem6 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem7 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem8 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem11 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemLevel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemID = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem12 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem14 = new DevComponents.DotNetBar.ButtonItem();
            this.bMultiEdit = new DevComponents.DotNetBar.ButtonItem();
            this.bMark = new DevComponents.DotNetBar.ButtonItem();
            this.bMarkQuest = new DevComponents.DotNetBar.ButtonItem();
            this.bCancelMark = new DevComponents.DotNetBar.ButtonItem();
            this.QuestStatbtn = new DevComponents.DotNetBar.ButtonItem();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.AllowDrop = true;
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.HideSelection = false;
            this.treeView1.HotTracking = true;
            this.treeView1.ImageIndex = 0;
            this.treeView1.ImageList = this.imageList1;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            treeNode1.ForeColor = System.Drawing.Color.Teal;
            treeNode1.Name = "Node1";
            treeNode1.Text = "Node1";
            treeNode2.ForeColor = System.Drawing.Color.Maroon;
            treeNode2.ImageIndex = -2;
            treeNode2.Name = "Node0";
            treeNode2.SelectedImageIndex = -2;
            treeNode2.Text = "阿斯顿否";
            treeNode3.ForeColor = System.Drawing.Color.Teal;
            treeNode3.Name = "Node3";
            treeNode3.Text = "Node3";
            treeNode4.ForeColor = System.Drawing.Color.Blue;
            treeNode4.Name = "Node5";
            treeNode4.Text = "Node5";
            treeNode5.ForeColor = System.Drawing.Color.Teal;
            treeNode5.Name = "Node4";
            treeNode5.Text = "Node4";
            treeNode6.ForeColor = System.Drawing.Color.Maroon;
            treeNode6.ImageIndex = -2;
            treeNode6.Name = "Node2";
            treeNode6.Text = "Node2";
            this.treeView1.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode2,
            treeNode6});
            this.treeView1.SelectedImageIndex = 0;
            this.treeView1.ShowNodeToolTips = true;
            this.treeView1.Size = new System.Drawing.Size(282, 632);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.treeView1_AfterLabelEdit);
            this.treeView1.NodeMouseHover += new System.Windows.Forms.TreeNodeMouseHoverEventHandler(this.treeView1_NodeMouseHover);
            this.treeView1.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView1_DragDrop);
            this.treeView1.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView1_DragEnter);
            this.treeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_NodeMouseClick);
            this.treeView1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView1_KeyDown);
            this.treeView1.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.baseTree_ItemDrag);
            this.treeView1.DragOver += new System.Windows.Forms.DragEventHandler(this.treeView1_DragOver);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "1_前置用不可接任务_2.bmp");
            this.imageList1.Images.SetKeyName(1, "2_脚本任务_3.bmp");
            this.imageList1.Images.SetKeyName(2, "3_采集任务.bmp");
            this.imageList1.Images.SetKeyName(3, "3_收集任务_2.bmp");
            this.imageList1.Images.SetKeyName(4, "4_杀怪任务.bmp");
            this.imageList1.Images.SetKeyName(5, "5_送道具任务.bmp");
            this.imageList1.Images.SetKeyName(6, "6_对话任务.bmp");
            this.imageList1.Images.SetKeyName(7, "X_计时任务.bmp");
            this.imageList1.Images.SetKeyName(8, "前置任务集合.bmp");
            this.imageList1.Images.SetKeyName(9, "场景分类_3.bmp");
            this.imageList1.Images.SetKeyName(10, "后续任务.bmp");
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmNodeRightClick});
            this.contextMenuBar1.Location = new System.Drawing.Point(317, 124);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(219, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 1;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.ImagePaddingHorizontal = 8;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem9,
            this.buttonItem10,
            this.buttonItem13,
            this.buttonItem3,
            this.buttonItem6,
            this.buttonItem7,
            this.buttonItem8,
            this.buttonItem11,
            this.buttonItem12,
            this.buttonItem14,
            this.bMultiEdit,
            this.bMark,
            this.QuestStatbtn});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonItem9
            // 
            this.buttonItem9.ImagePaddingHorizontal = 8;
            this.buttonItem9.Name = "buttonItem9";
            this.buttonItem9.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem2,
            this.buttonItem1,
            this.buttonItem4,
            this.buttonItem5,
            this.buttonItem15});
            this.buttonItem9.Text = "添加";
            // 
            // buttonItem2
            // 
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.Text = "任务";
            this.buttonItem2.Click += new System.EventHandler(this.buttonItem2_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "分区";
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // buttonItem4
            // 
            this.buttonItem4.ImagePaddingHorizontal = 8;
            this.buttonItem4.Name = "buttonItem4";
            this.buttonItem4.Text = "完成其一";
            this.buttonItem4.Click += new System.EventHandler(this.buttonItem4_Click);
            // 
            // buttonItem5
            // 
            this.buttonItem5.ImagePaddingHorizontal = 8;
            this.buttonItem5.Name = "buttonItem5";
            this.buttonItem5.Text = "全部完成";
            this.buttonItem5.Click += new System.EventHandler(this.buttonItem5_Click);
            // 
            // buttonItem15
            // 
            this.buttonItem15.ImagePaddingHorizontal = 8;
            this.buttonItem15.Name = "buttonItem15";
            this.buttonItem15.Text = "buttonItem15";
            // 
            // buttonItem10
            // 
            this.buttonItem10.ImagePaddingHorizontal = 8;
            this.buttonItem10.Name = "buttonItem10";
            this.buttonItem10.Text = "设为直接后续";
            this.buttonItem10.Click += new System.EventHandler(this.buttonItem10_Click);
            // 
            // buttonItem13
            // 
            this.buttonItem13.ImagePaddingHorizontal = 8;
            this.buttonItem13.Name = "buttonItem13";
            this.buttonItem13.Text = "设为同期任务";
            this.buttonItem13.Click += new System.EventHandler(this.buttonItem13_Click);
            // 
            // buttonItem3
            // 
            this.buttonItem3.BeginGroup = true;
            this.buttonItem3.ImagePaddingHorizontal = 8;
            this.buttonItem3.Name = "buttonItem3";
            this.buttonItem3.Text = "重命名";
            this.buttonItem3.Click += new System.EventHandler(this.buttonItem3_Click);
            // 
            // buttonItem6
            // 
            this.buttonItem6.ImagePaddingHorizontal = 8;
            this.buttonItem6.Name = "buttonItem6";
            this.buttonItem6.Text = "复制";
            this.buttonItem6.Click += new System.EventHandler(this.buttonItem6_Click);
            // 
            // buttonItem7
            // 
            this.buttonItem7.ImagePaddingHorizontal = 8;
            this.buttonItem7.Name = "buttonItem7";
            this.buttonItem7.Text = "粘贴";
            this.buttonItem7.Click += new System.EventHandler(this.buttonItem7_Click);
            // 
            // buttonItem8
            // 
            this.buttonItem8.ImagePaddingHorizontal = 8;
            this.buttonItem8.Name = "buttonItem8";
            this.buttonItem8.Text = "删除";
            this.buttonItem8.Click += new System.EventHandler(this.buttonItem8_Click);
            // 
            // buttonItem11
            // 
            this.buttonItem11.BeginGroup = true;
            this.buttonItem11.ImagePaddingHorizontal = 8;
            this.buttonItem11.Name = "buttonItem11";
            this.buttonItem11.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItemLevel,
            this.buttonItemID});
            this.buttonItem11.Text = "显示信息";
            // 
            // buttonItemLevel
            // 
            this.buttonItemLevel.ImagePaddingHorizontal = 8;
            this.buttonItemLevel.Name = "buttonItemLevel";
            this.buttonItemLevel.Text = "任务等级";
            this.buttonItemLevel.Click += new System.EventHandler(this.buttonItemLevel_Click);
            // 
            // buttonItemID
            // 
            this.buttonItemID.ImagePaddingHorizontal = 8;
            this.buttonItemID.Name = "buttonItemID";
            this.buttonItemID.Text = "任务ID";
            this.buttonItemID.Click += new System.EventHandler(this.buttonItemID_Click);
            // 
            // buttonItem12
            // 
            this.buttonItem12.ImagePaddingHorizontal = 8;
            this.buttonItem12.Name = "buttonItem12";
            this.buttonItem12.Text = "刷新";
            this.buttonItem12.Click += new System.EventHandler(this.buttonItem12_Click);
            // 
            // buttonItem14
            // 
            this.buttonItem14.ImagePaddingHorizontal = 8;
            this.buttonItem14.Name = "buttonItem14";
            this.buttonItem14.Text = "流量分析";
            this.buttonItem14.Click += new System.EventHandler(this.buttonItem14_Click);
            // 
            // bMultiEdit
            // 
            this.bMultiEdit.ImagePaddingHorizontal = 8;
            this.bMultiEdit.Name = "bMultiEdit";
            this.bMultiEdit.Text = "批量编辑";
            this.bMultiEdit.Click += new System.EventHandler(this.bMultiEdit_Click);
            // 
            // bMark
            // 
            this.bMark.ImagePaddingHorizontal = 8;
            this.bMark.Name = "bMark";
            this.bMark.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bMarkQuest,
            this.bCancelMark});
            this.bMark.Text = "标记";
            // 
            // bMarkQuest
            // 
            this.bMarkQuest.ForeColor = System.Drawing.Color.Black;
            this.bMarkQuest.ImagePaddingHorizontal = 8;
            this.bMarkQuest.Name = "bMarkQuest";
            this.bMarkQuest.Text = "标记任务 ●";
            this.bMarkQuest.Click += new System.EventHandler(this.bMarkQuest_Click);
            // 
            // bCancelMark
            // 
            this.bCancelMark.ImagePaddingHorizontal = 8;
            this.bCancelMark.Name = "bCancelMark";
            this.bCancelMark.Text = "取消标记";
            this.bCancelMark.Click += new System.EventHandler(this.bCancelMark_Click);
            // 
            // QuestStatbtn
            // 
            this.QuestStatbtn.ImagePaddingHorizontal = 8;
            this.QuestStatbtn.Name = "QuestStatbtn";
            this.QuestStatbtn.Text = "任务数量统计";
            this.QuestStatbtn.Click += new System.EventHandler(this.QuestStatbtn_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Location = new System.Drawing.Point(12, 12);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.splitContainer1.Panel1.Controls.Add(this.buttonX3);
            this.splitContainer1.Panel1.Controls.Add(this.buttonX2);
            this.splitContainer1.Panel1.Controls.Add(this.buttonX1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.treeView1);
            this.splitContainer1.Size = new System.Drawing.Size(282, 661);
            this.splitContainer1.SplitterDistance = 25;
            this.splitContainer1.TabIndex = 2;
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.Location = new System.Drawing.Point(113, 3);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(49, 23);
            this.buttonX3.TabIndex = 3;
            this.buttonX3.Text = "展开";
            this.buttonX3.Tooltip = "F3";
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Location = new System.Drawing.Point(58, 3);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(49, 23);
            this.buttonX2.TabIndex = 2;
            this.buttonX2.Text = "跳转";
            this.buttonX2.Tooltip = "F4";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Location = new System.Drawing.Point(3, 3);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(49, 23);
            this.buttonX1.TabIndex = 1;
            this.buttonX1.Text = "查找";
            this.buttonX1.Tooltip = "Ctrl+F";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(703, 685);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.contextMenuBar1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private DevComponents.DotNetBar.ButtonItem buttonItem4;
        private DevComponents.DotNetBar.ButtonItem buttonItem5;
        private DevComponents.DotNetBar.ButtonItem buttonItem6;
        private DevComponents.DotNetBar.ButtonItem buttonItem7;
        private DevComponents.DotNetBar.ButtonItem buttonItem8;
        private DevComponents.DotNetBar.ButtonItem buttonItem9;
        private System.Windows.Forms.ImageList imageList1;
        private DevComponents.DotNetBar.ButtonItem buttonItem3;
        private DevComponents.DotNetBar.ButtonItem buttonItem10;
        private DevComponents.DotNetBar.ButtonItem buttonItem11;
        private DevComponents.DotNetBar.ButtonItem buttonItemLevel;
        private DevComponents.DotNetBar.ButtonItem buttonItemID;
        private DevComponents.DotNetBar.ButtonItem buttonItem12;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonItem buttonItem13;
        private DevComponents.DotNetBar.ButtonItem buttonItem14;
        private DevComponents.DotNetBar.ButtonItem bMultiEdit;
        private DevComponents.DotNetBar.ButtonItem bMark;
        private DevComponents.DotNetBar.ButtonItem bMarkQuest;
        private DevComponents.DotNetBar.ButtonItem bCancelMark;
        private DevComponents.DotNetBar.ButtonItem buttonItem15;
        private DevComponents.DotNetBar.ButtonItem QuestStatbtn;
    }
}