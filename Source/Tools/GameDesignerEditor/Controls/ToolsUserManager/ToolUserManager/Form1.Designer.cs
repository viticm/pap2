namespace ToolUserManager
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
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.btnRevoke = new DevComponents.DotNetBar.ButtonX();
            this.btnDelete = new DevComponents.DotNetBar.ButtonX();
            this.btnGrant = new DevComponents.DotNetBar.ButtonX();
            this.btnAdd = new DevComponents.DotNetBar.ButtonX();
            this.tboxIP = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.combxUserName = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.combxDBs = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.groupPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.btnRevoke);
            this.groupPanel1.Controls.Add(this.btnDelete);
            this.groupPanel1.Controls.Add(this.btnGrant);
            this.groupPanel1.Controls.Add(this.btnAdd);
            this.groupPanel1.Controls.Add(this.tboxIP);
            this.groupPanel1.Controls.Add(this.combxUserName);
            this.groupPanel1.Controls.Add(this.combxDBs);
            this.groupPanel1.Location = new System.Drawing.Point(1, 4);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(480, 370);
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
            this.groupPanel1.TabIndex = 0;
            this.groupPanel1.Text = "管理者";
            // 
            // btnRevoke
            // 
            this.btnRevoke.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnRevoke.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnRevoke.Location = new System.Drawing.Point(253, 217);
            this.btnRevoke.Name = "btnRevoke";
            this.btnRevoke.Size = new System.Drawing.Size(75, 23);
            this.btnRevoke.TabIndex = 5;
            this.btnRevoke.Text = "收回权限";
            this.btnRevoke.Click += new System.EventHandler(this.btnRevoke_Click);
            // 
            // btnDelete
            // 
            this.btnDelete.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnDelete.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnDelete.Location = new System.Drawing.Point(253, 169);
            this.btnDelete.Name = "btnDelete";
            this.btnDelete.Size = new System.Drawing.Size(75, 23);
            this.btnDelete.TabIndex = 5;
            this.btnDelete.Text = "删除";
            this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
            // 
            // btnGrant
            // 
            this.btnGrant.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnGrant.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnGrant.Location = new System.Drawing.Point(103, 217);
            this.btnGrant.Name = "btnGrant";
            this.btnGrant.Size = new System.Drawing.Size(75, 23);
            this.btnGrant.TabIndex = 5;
            this.btnGrant.Text = "开通权限";
            this.btnGrant.Click += new System.EventHandler(this.btnGrant_Click);
            // 
            // btnAdd
            // 
            this.btnAdd.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnAdd.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnAdd.Location = new System.Drawing.Point(103, 169);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 5;
            this.btnAdd.Text = "添加";
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // tboxIP
            // 
            // 
            // 
            // 
            this.tboxIP.Border.Class = "TextBoxBorder";
            this.tboxIP.Location = new System.Drawing.Point(257, 104);
            this.tboxIP.Name = "tboxIP";
            this.tboxIP.Size = new System.Drawing.Size(100, 21);
            this.tboxIP.TabIndex = 4;
            this.tboxIP.Text = "192.168.";
            // 
            // combxUserName
            // 
            this.combxUserName.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.combxUserName.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.combxUserName.DisplayMember = "Text";
            this.combxUserName.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.combxUserName.FormattingEnabled = true;
            this.combxUserName.ItemHeight = 15;
            this.combxUserName.Location = new System.Drawing.Point(103, 104);
            this.combxUserName.Name = "combxUserName";
            this.combxUserName.Size = new System.Drawing.Size(121, 21);
            this.combxUserName.TabIndex = 3;
            this.combxUserName.Text = "用户id";
            this.combxUserName.TextChanged += new System.EventHandler(this.combxUserName_TextChanged);
            // 
            // combxDBs
            // 
            this.combxDBs.DisplayMember = "Text";
            this.combxDBs.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.combxDBs.FormattingEnabled = true;
            this.combxDBs.ItemHeight = 15;
            this.combxDBs.Location = new System.Drawing.Point(103, 26);
            this.combxDBs.Name = "combxDBs";
            this.combxDBs.Size = new System.Drawing.Size(121, 21);
            this.combxDBs.TabIndex = 3;
            this.combxDBs.Text = "请选择数据库";
            this.combxDBs.SelectedIndexChanged += new System.EventHandler(this.combxDBs_SelectedIndexChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(485, 377);
            this.Controls.Add(this.groupPanel1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "工具用户管理";
            this.groupPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.Controls.ComboBoxEx combxUserName;
        private DevComponents.DotNetBar.Controls.ComboBoxEx combxDBs;
        private DevComponents.DotNetBar.Controls.TextBoxX tboxIP;
        private DevComponents.DotNetBar.ButtonX btnDelete;
        private DevComponents.DotNetBar.ButtonX btnAdd;
        private DevComponents.DotNetBar.ButtonX btnRevoke;
        private DevComponents.DotNetBar.ButtonX btnGrant;

    }
}

