namespace CustomLoadTree_CareerEvent
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
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Node1");
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemAdd = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemAddChild = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemDel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemRename = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem4 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem5 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem15 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemLevel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemID = new DevComponents.DotNetBar.ButtonItem();
            this.bMarkQuest = new DevComponents.DotNetBar.ButtonItem();
            this.bCancelMark = new DevComponents.DotNetBar.ButtonItem();
            this.button3 = new System.Windows.Forms.Button();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(3, 3);
            this.treeView1.Name = "treeView1";
            treeNode1.Name = "Node1";
            treeNode1.Text = "Node1";
            this.treeView1.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode1});
            this.treeView1.Size = new System.Drawing.Size(203, 492);
            this.treeView1.TabIndex = 0;
            this.treeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.OnTreeNodeClicked);
            // 
            // splitContainer1
            // 
            this.splitContainer1.AllowDrop = true;
            this.splitContainer1.Location = new System.Drawing.Point(12, 12);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.button3);
            this.splitContainer1.Panel1.Controls.Add(this.button2);
            this.splitContainer1.Panel1.Controls.Add(this.button1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.treeView1);
            this.splitContainer1.Size = new System.Drawing.Size(211, 536);
            this.splitContainer1.SplitterDistance = 34;
            this.splitContainer1.TabIndex = 1;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(141, 3);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(67, 23);
            this.button2.TabIndex = 1;
            this.button2.Text = "展开";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(0, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(68, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "查找";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmNodeRightClick});
            this.contextMenuBar1.Location = new System.Drawing.Point(273, 268);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(219, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 2;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.ImagePaddingHorizontal = 8;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItemAdd,
            this.buttonItemAddChild,
            this.buttonItemDel,
            this.buttonItemRename});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonItemAdd
            // 
            this.buttonItemAdd.ImagePaddingHorizontal = 8;
            this.buttonItemAdd.Name = "buttonItemAdd";
            this.buttonItemAdd.Text = "添加";
            this.buttonItemAdd.Click += new System.EventHandler(this.buttonItemAdd_Click);
            // 
            // buttonItemAddChild
            // 
            this.buttonItemAddChild.BeginGroup = true;
            this.buttonItemAddChild.ImagePaddingHorizontal = 8;
            this.buttonItemAddChild.Name = "buttonItemAddChild";
            this.buttonItemAddChild.Text = "添加子节点";
            this.buttonItemAddChild.Click += new System.EventHandler(this.buttonItemAddChild_Click);
            // 
            // buttonItemDel
            // 
            this.buttonItemDel.ImagePaddingHorizontal = 8;
            this.buttonItemDel.Name = "buttonItemDel";
            this.buttonItemDel.Text = "删除";
            this.buttonItemDel.Click += new System.EventHandler(this.buttonItemDel_Click);
            // 
            // buttonItemRename
            // 
            this.buttonItemRename.ImagePaddingHorizontal = 8;
            this.buttonItemRename.Name = "buttonItemRename";
            this.buttonItemRename.Text = "重命名";
            // 
            // buttonItem2
            // 
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.Text = "任务";
            // 
            // buttonItem1
            // 
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "分区";
            // 
            // buttonItem4
            // 
            this.buttonItem4.ImagePaddingHorizontal = 8;
            this.buttonItem4.Name = "buttonItem4";
            this.buttonItem4.Text = "完成其一";
            // 
            // buttonItem5
            // 
            this.buttonItem5.ImagePaddingHorizontal = 8;
            this.buttonItem5.Name = "buttonItem5";
            this.buttonItem5.Text = "全部完成";
            // 
            // buttonItem15
            // 
            this.buttonItem15.ImagePaddingHorizontal = 8;
            this.buttonItem15.Name = "buttonItem15";
            this.buttonItem15.Text = "buttonItem15";
            // 
            // buttonItemLevel
            // 
            this.buttonItemLevel.ImagePaddingHorizontal = 8;
            this.buttonItemLevel.Name = "buttonItemLevel";
            this.buttonItemLevel.Text = "任务等级";
            // 
            // buttonItemID
            // 
            this.buttonItemID.ImagePaddingHorizontal = 8;
            this.buttonItemID.Name = "buttonItemID";
            this.buttonItemID.Text = "任务ID";
            // 
            // bMarkQuest
            // 
            this.bMarkQuest.ForeColor = System.Drawing.Color.Black;
            this.bMarkQuest.ImagePaddingHorizontal = 8;
            this.bMarkQuest.Name = "bMarkQuest";
            this.bMarkQuest.Text = "标记任务 ●";
            // 
            // bCancelMark
            // 
            this.bCancelMark.ImagePaddingHorizontal = 8;
            this.bCancelMark.Name = "bCancelMark";
            this.bCancelMark.Text = "取消标记";
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(74, 3);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(61, 23);
            this.button3.TabIndex = 2;
            this.button3.Text = "刷新";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(765, 560);
            this.Controls.Add(this.contextMenuBar1);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.ButtonItem buttonItem4;
        private DevComponents.DotNetBar.ButtonItem buttonItem5;
        private DevComponents.DotNetBar.ButtonItem buttonItem15;
        private DevComponents.DotNetBar.ButtonItem buttonItemAddChild;
        private DevComponents.DotNetBar.ButtonItem buttonItemDel;
        private DevComponents.DotNetBar.ButtonItem buttonItemLevel;
        private DevComponents.DotNetBar.ButtonItem buttonItemID;
        private DevComponents.DotNetBar.ButtonItem bMarkQuest;
        private DevComponents.DotNetBar.ButtonItem bCancelMark;
        private DevComponents.DotNetBar.ButtonItem buttonItemRename;
        private DevComponents.DotNetBar.ButtonItem buttonItemAdd;
        private System.Windows.Forms.Button button3;
    }
}