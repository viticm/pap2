namespace ConstSelector
{
    partial class NpcNickForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NpcNickForm));
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.advTree1 = new DevComponents.AdvTree.AdvTree();
            this.columnHeader1 = new DevComponents.AdvTree.ColumnHeader();
            this.columnHeader2 = new DevComponents.AdvTree.ColumnHeader();
            this.columnHeader3 = new DevComponents.AdvTree.ColumnHeader();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            ((System.ComponentModel.ISupportInitialize)(this.advTree1)).BeginInit();
            this.SuspendLayout();
            // 
            // nodeConnector1
            // 
            this.nodeConnector1.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // advTree1
            // 
            this.advTree1.AllowDrop = true;
            this.advTree1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.advTree1.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.advTree1.BackgroundStyle.Class = "TreeBorderKey";
            this.advTree1.Columns.Add(this.columnHeader1);
            this.advTree1.Columns.Add(this.columnHeader2);
            this.advTree1.Columns.Add(this.columnHeader3);
            this.advTree1.ExpandButtonType = DevComponents.AdvTree.eExpandButtonType.Triangle;
            this.advTree1.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.advTree1.Location = new System.Drawing.Point(12, 12);
            this.advTree1.Name = "advTree1";
            this.advTree1.NodesConnector = this.nodeConnector1;
            this.advTree1.NodeStyle = this.elementStyle1;
            this.advTree1.NodeStyleMouseOver = this.elementStyle2;
            this.advTree1.PathSeparator = ";";
            this.advTree1.Size = new System.Drawing.Size(572, 351);
            this.advTree1.Styles.Add(this.elementStyle1);
            this.advTree1.Styles.Add(this.elementStyle2);
            this.advTree1.SuspendPaint = false;
            this.advTree1.TabIndex = 6;
            this.advTree1.Text = "advTree1";
            // 
            // columnHeader1
            // 
            this.columnHeader1.Name = "columnHeader1";
            this.columnHeader1.Text = "别名";
            this.columnHeader1.Width.Absolute = 200;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Name = "columnHeader2";
            this.columnHeader2.Text = "类型";
            this.columnHeader2.Width.Absolute = 180;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Name = "columnHeader3";
            this.columnHeader3.Text = "位置";
            this.columnHeader3.Width.Absolute = 150;
            // 
            // elementStyle1
            // 
            this.elementStyle1.Name = "elementStyle1";
            this.elementStyle1.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle2
            // 
            this.elementStyle2.BackColor = System.Drawing.Color.White;
            this.elementStyle2.BackColor2 = System.Drawing.Color.FromArgb(((int)(((byte)(228)))), ((int)(((byte)(228)))), ((int)(((byte)(240)))));
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
            this.elementStyle2.Description = "Gray";
            this.elementStyle2.Name = "elementStyle2";
            this.elementStyle2.PaddingBottom = 1;
            this.elementStyle2.PaddingLeft = 1;
            this.elementStyle2.PaddingRight = 1;
            this.elementStyle2.PaddingTop = 1;
            this.elementStyle2.TextColor = System.Drawing.Color.Black;
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(509, 369);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(75, 23);
            this.buttonX2.TabIndex = 8;
            this.buttonX2.Text = "取消(&C)";
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "3_14.png");
            this.imageList1.Images.SetKeyName(1, "2_17.png");
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(428, 369);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(75, 23);
            this.buttonX1.TabIndex = 7;
            this.buttonX1.Text = "确定(&O)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // NpcNickForm
            // 
            this.ClientSize = new System.Drawing.Size(592, 404);
            this.Controls.Add(this.advTree1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.Name = "NpcNickForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "NPC别名选择器";
            ((System.ComponentModel.ISupportInitialize)(this.advTree1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.AdvTree.AdvTree advTree1;
        private DevComponents.AdvTree.ColumnHeader columnHeader1;
        private DevComponents.AdvTree.ColumnHeader columnHeader2;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private System.Windows.Forms.ImageList imageList1;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.AdvTree.ColumnHeader columnHeader3;
    }
}
