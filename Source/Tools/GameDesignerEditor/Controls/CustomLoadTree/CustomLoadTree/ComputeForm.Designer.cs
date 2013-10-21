namespace CustomLoadTree
{
    partial class ComputeForm
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
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.buttonItem4 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem5 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem6 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.labelItem1 = new DevComponents.DotNetBar.LabelItem();
            this.labelItem2 = new DevComponents.DotNetBar.LabelItem();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.autoFillToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setDefaultToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hideColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.groupPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.bar1);
            this.groupPanel1.Controls.Add(this.dataGridView1);
            this.groupPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupPanel1.Location = new System.Drawing.Point(0, 0);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(824, 341);
            // 
            // 
            // 
            this.groupPanel1.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel1.Style.BackColorGradientAngle = 90;
            this.groupPanel1.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel1.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderBottomWidth = 1;
            this.groupPanel1.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel1.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderLeftWidth = 1;
            this.groupPanel1.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderRightWidth = 1;
            this.groupPanel1.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderTopWidth = 1;
            this.groupPanel1.Style.CornerDiameter = 4;
            this.groupPanel1.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel1.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel1.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel1.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel1.TabIndex = 1;
            // 
            // bar1
            // 
            this.bar1.Dock = System.Windows.Forms.DockStyle.Top;
            this.bar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItem4,
            this.buttonItem5,
            this.buttonItem6,
            this.buttonItem1,
            this.buttonItem2,
            this.labelItem1,
            this.labelItem2});
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(818, 25);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 2;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // buttonItem4
            // 
            this.buttonItem4.Name = "buttonItem4";
            this.buttonItem4.Text = "全部选中";
            this.buttonItem4.Click += new System.EventHandler(this.buttonItem4_Click);
            // 
            // buttonItem5
            // 
            this.buttonItem5.Name = "buttonItem5";
            this.buttonItem5.Text = "开始计算";
            this.buttonItem5.Click += new System.EventHandler(this.buttonItem5_Click);
            // 
            // buttonItem6
            // 
            this.buttonItem6.Name = "buttonItem6";
            this.buttonItem6.Text = "保存数据";
            this.buttonItem6.Click += new System.EventHandler(this.buttonItem6_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "设置参数";
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // buttonItem2
            // 
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.Text = "精简模式";
            this.buttonItem2.Click += new System.EventHandler(this.buttonItem2_Click);
            // 
            // labelItem1
            // 
            this.labelItem1.BackColor = System.Drawing.Color.LavenderBlush;
            this.labelItem1.BorderType = DevComponents.DotNetBar.eBorderType.None;
            this.labelItem1.Name = "labelItem1";
            this.labelItem1.Text = "只读";
            // 
            // labelItem2
            // 
            this.labelItem2.BackColor = System.Drawing.Color.LightYellow;
            this.labelItem2.BorderType = DevComponents.DotNetBar.eBorderType.None;
            this.labelItem2.Name = "labelItem2";
            this.labelItem2.Text = "可存";
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.AllowUserToOrderColumns = true;
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCells;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Location = new System.Drawing.Point(9, 34);
            this.dataGridView1.MultiSelect = false;
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.AutoSizeToAllHeaders;
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(800, 292);
            this.dataGridView1.TabIndex = 1;
            this.dataGridView1.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridView1_CellMouseDown);
            this.dataGridView1.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellClick);
            this.dataGridView1.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellValueChanged);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.autoFillToolStripMenuItem,
            this.setDefaultToolStripMenuItem,
            this.hideColumnToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(131, 70);
            // 
            // autoFillToolStripMenuItem
            // 
            this.autoFillToolStripMenuItem.Name = "autoFillToolStripMenuItem";
            this.autoFillToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.autoFillToolStripMenuItem.Text = "自动填充列";
            this.autoFillToolStripMenuItem.Click += new System.EventHandler(this.autoFillToolStripMenuItem_Click);
            // 
            // setDefaultToolStripMenuItem
            // 
            this.setDefaultToolStripMenuItem.Name = "setDefaultToolStripMenuItem";
            this.setDefaultToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.setDefaultToolStripMenuItem.Text = "回复默认值";
            this.setDefaultToolStripMenuItem.Click += new System.EventHandler(this.setDefaultToolStripMenuItem_Click);
            // 
            // hideColumnToolStripMenuItem
            // 
            this.hideColumnToolStripMenuItem.Name = "hideColumnToolStripMenuItem";
            this.hideColumnToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.hideColumnToolStripMenuItem.Text = "保存数值";
            this.hideColumnToolStripMenuItem.Click += new System.EventHandler(this.hideColumnToolStripMenuItem_Click);
            // 
            // ComputeForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(824, 341);
            this.Controls.Add(this.groupPanel1);
            this.Name = "ComputeForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "任务流量分析工具";
            this.groupPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private System.Windows.Forms.DataGridView dataGridView1;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.ButtonItem buttonItem4;
        private DevComponents.DotNetBar.ButtonItem buttonItem5;
        private DevComponents.DotNetBar.ButtonItem buttonItem6;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem autoFillToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setDefaultToolStripMenuItem;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.LabelItem labelItem1;
        private DevComponents.DotNetBar.LabelItem labelItem2;
        private System.Windows.Forms.ToolStripMenuItem hideColumnToolStripMenuItem;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
    }
}

