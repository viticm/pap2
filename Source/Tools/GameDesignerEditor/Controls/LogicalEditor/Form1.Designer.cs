namespace LogicalEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.buttonSave = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonOpenMap = new DevComponents.DotNetBar.ButtonX();
            this.tree1 = new MWControlSuite.MWTreeView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.buttonFind = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.labelLoading = new System.Windows.Forms.Label();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.SuspendLayout();
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(441, 427);
            this.propertyGrid1.TabIndex = 1;
            this.propertyGrid1.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid1_PropertyValueChanged);
            // 
            // buttonSave
            // 
            this.buttonSave.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonSave.Location = new System.Drawing.Point(523, 476);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(75, 23);
            this.buttonSave.TabIndex = 2;
            this.buttonSave.Text = "保存(&S)";
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.Location = new System.Drawing.Point(604, 476);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(75, 23);
            this.buttonX2.TabIndex = 2;
            this.buttonX2.Text = "关闭(&C)";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonOpenMap
            // 
            this.buttonOpenMap.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOpenMap.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOpenMap.AutoExpandOnClick = true;
            this.buttonOpenMap.Location = new System.Drawing.Point(397, 476);
            this.buttonOpenMap.Name = "buttonOpenMap";
            this.buttonOpenMap.Size = new System.Drawing.Size(93, 23);
            this.buttonOpenMap.TabIndex = 2;
            this.buttonOpenMap.Text = "打开地图(&O)";
            // 
            // tree1
            // 
            this.tree1.BackColor = System.Drawing.SystemColors.Window;
            this.tree1.CheckedNodes = ((System.Collections.Hashtable)(resources.GetObject("tree1.CheckedNodes")));
            this.tree1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tree1.Location = new System.Drawing.Point(0, 0);
            this.tree1.Name = "tree1";
            this.tree1.RubberbandGradientBlend = new MWControlSuite.MWRubberbandGradientBlend[0];
            this.tree1.RubberbandGradientColorBlend = new MWControlSuite.MWRubberbandGradientColorBlend[0];
            this.tree1.SelNodes = ((System.Collections.Hashtable)(resources.GetObject("tree1.SelNodes")));
            this.tree1.Size = new System.Drawing.Size(222, 427);
            this.tree1.TabIndex = 4;
            this.tree1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tree1_AfterSelect);
            this.tree1.AfterSelNodesRemove += new MWControlSuite.MWPropertyEventHandler(this.tree1_AfterSelNodesRemove);
            this.tree1.AfterSelNodesAdd += new MWControlSuite.MWPropertyEventHandler(this.tree1_AfterSelNodesAdd);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(12, 31);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.tree1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer1.Size = new System.Drawing.Size(667, 427);
            this.splitContainer1.SplitterDistance = 222;
            this.splitContainer1.TabIndex = 5;
            // 
            // progressBar1
            // 
            this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.progressBar1.Location = new System.Drawing.Point(105, 476);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(275, 23);
            this.progressBar1.TabIndex = 6;
            this.progressBar1.Visible = false;
            // 
            // bar1
            // 
            this.bar1.Dock = System.Windows.Forms.DockStyle.Top;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonFind,
            this.buttonItem2});
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(694, 25);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 2;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // buttonFind
            // 
            this.buttonFind.ImagePaddingHorizontal = 8;
            this.buttonFind.Name = "buttonFind";
            this.buttonFind.Text = "查找";
            this.buttonFind.Click += new System.EventHandler(this.buttonFind_Click);
            // 
            // buttonItem2
            // 
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.Text = "buttonItem2";
            this.buttonItem2.Visible = false;
            // 
            // labelLoading
            // 
            this.labelLoading.AutoSize = true;
            this.labelLoading.Location = new System.Drawing.Point(12, 480);
            this.labelLoading.Name = "labelLoading";
            this.labelLoading.Size = new System.Drawing.Size(65, 12);
            this.labelLoading.TabIndex = 7;
            this.labelLoading.Text = "Loading...";
            this.labelLoading.Visible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(694, 511);
            this.Controls.Add(this.labelLoading);
            this.Controls.Add(this.bar1);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.buttonOpenMap);
            this.Name = "Form1";
            this.Text = "地图逻辑工具";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private DevComponents.DotNetBar.ButtonX buttonSave;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonOpenMap;
        private MWControlSuite.MWTreeView tree1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.ButtonItem buttonFind;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private System.Windows.Forms.Label labelLoading;
    }
}

