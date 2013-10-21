namespace CustomLoadTree
{
    partial class FindFrm
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.radioID = new System.Windows.Forms.RadioButton();
            this.radioName = new System.Windows.Forms.RadioButton();
            this.textBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.radioNPC = new System.Windows.Forms.RadioButton();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.radioDoodad = new System.Windows.Forms.RadioButton();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.radioItem = new System.Windows.Forms.RadioButton();
            this.buttonCancel = new DevComponents.DotNetBar.ButtonX();
            this.buttonGotoQuest = new DevComponents.DotNetBar.ButtonX();
            this.buttonFind = new DevComponents.DotNetBar.ButtonX();
            this.radioTrinket = new System.Windows.Forms.RadioButton();
            this.radioArmor = new System.Windows.Forms.RadioButton();
            this.radioWeapon = new System.Windows.Forms.RadioButton();
            this.panelEx1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.groupBox2);
            this.panelEx1.Controls.Add(this.groupBox1);
            this.panelEx1.Controls.Add(this.buttonCancel);
            this.panelEx1.Controls.Add(this.buttonGotoQuest);
            this.panelEx1.Controls.Add(this.buttonFind);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(879, 721);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.Color = System.Drawing.Color.LightSteelBlue;
            this.panelEx1.Style.BackColor2.Color = System.Drawing.Color.LightSteelBlue;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.radioID);
            this.groupBox2.Controls.Add(this.radioName);
            this.groupBox2.Controls.Add(this.textBox);
            this.groupBox2.Location = new System.Drawing.Point(12, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(278, 100);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "查找任务";
            // 
            // radioID
            // 
            this.radioID.AutoSize = true;
            this.radioID.Location = new System.Drawing.Point(17, 26);
            this.radioID.Name = "radioID";
            this.radioID.Size = new System.Drawing.Size(65, 16);
            this.radioID.TabIndex = 0;
            this.radioID.TabStop = true;
            this.radioID.Text = "任务 ID";
            this.radioID.UseVisualStyleBackColor = true;
            this.radioID.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // radioName
            // 
            this.radioName.AutoSize = true;
            this.radioName.Location = new System.Drawing.Point(114, 26);
            this.radioName.Name = "radioName";
            this.radioName.Size = new System.Drawing.Size(71, 16);
            this.radioName.TabIndex = 1;
            this.radioName.TabStop = true;
            this.radioName.Text = "任务名称";
            this.radioName.UseVisualStyleBackColor = true;
            this.radioName.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // textBox
            // 
            // 
            // 
            // 
            this.textBox.Border.Class = "TextBoxBorder";
            this.textBox.Font = new System.Drawing.Font("SimSun", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox.Location = new System.Drawing.Point(17, 51);
            this.textBox.Name = "textBox";
            this.textBox.Size = new System.Drawing.Size(241, 29);
            this.textBox.TabIndex = 2;
            this.textBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KeyDown);
            this.textBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.listBox1);
            this.groupBox1.Controls.Add(this.radioNPC);
            this.groupBox1.Controls.Add(this.textBoxX1);
            this.groupBox1.Controls.Add(this.radioDoodad);
            this.groupBox1.Controls.Add(this.buttonX1);
            this.groupBox1.Controls.Add(this.dataGridView1);
            this.groupBox1.Controls.Add(this.radioWeapon);
            this.groupBox1.Controls.Add(this.radioArmor);
            this.groupBox1.Controls.Add(this.radioTrinket);
            this.groupBox1.Controls.Add(this.radioItem);
            this.groupBox1.Location = new System.Drawing.Point(12, 136);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(855, 529);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "查找引用";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(21, 505);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(197, 12);
            this.label1.TabIndex = 10;
            this.label1.Text = "提示：双击任意一行来定位到任务。";
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(22, 324);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(812, 172);
            this.listBox1.TabIndex = 7;
            this.listBox1.DoubleClick += new System.EventHandler(this.listBox1_DoubleClick);
            // 
            // radioNPC
            // 
            this.radioNPC.AutoSize = true;
            this.radioNPC.Location = new System.Drawing.Point(22, 30);
            this.radioNPC.Name = "radioNPC";
            this.radioNPC.Size = new System.Drawing.Size(65, 16);
            this.radioNPC.TabIndex = 2;
            this.radioNPC.TabStop = true;
            this.radioNPC.Text = "NPC模板";
            this.radioNPC.UseVisualStyleBackColor = true;
            this.radioNPC.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Font = new System.Drawing.Font("SimSun", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBoxX1.Location = new System.Drawing.Point(22, 52);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(236, 29);
            this.textBoxX1.TabIndex = 2;
            this.textBoxX1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KeyDown);
            this.textBoxX1.TextChanged += new System.EventHandler(this.textBox_TextChanged);
            // 
            // radioDoodad
            // 
            this.radioDoodad.AutoSize = true;
            this.radioDoodad.Location = new System.Drawing.Point(119, 30);
            this.radioDoodad.Name = "radioDoodad";
            this.radioDoodad.Size = new System.Drawing.Size(83, 16);
            this.radioDoodad.TabIndex = 3;
            this.radioDoodad.TabStop = true;
            this.radioDoodad.Text = "Doodad模板";
            this.radioDoodad.UseVisualStyleBackColor = true;
            this.radioDoodad.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(261, 52);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(62, 29);
            this.buttonX1.TabIndex = 5;
            this.buttonX1.Text = "浏览(F4)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Location = new System.Drawing.Point(22, 96);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(812, 211);
            this.dataGridView1.TabIndex = 6;
            // 
            // radioItem
            // 
            this.radioItem.AutoSize = true;
            this.radioItem.Location = new System.Drawing.Point(226, 30);
            this.radioItem.Name = "radioItem";
            this.radioItem.Size = new System.Drawing.Size(71, 16);
            this.radioItem.TabIndex = 4;
            this.radioItem.TabStop = true;
            this.radioItem.Text = "Item模板";
            this.radioItem.UseVisualStyleBackColor = true;
            this.radioItem.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCancel.Location = new System.Drawing.Point(787, 682);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 4;
            this.buttonCancel.Text = "取消(&C)";
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonGotoQuest
            // 
            this.buttonGotoQuest.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonGotoQuest.Location = new System.Drawing.Point(581, 682);
            this.buttonGotoQuest.Name = "buttonGotoQuest";
            this.buttonGotoQuest.Size = new System.Drawing.Size(75, 23);
            this.buttonGotoQuest.TabIndex = 3;
            this.buttonGotoQuest.Text = "定位到任务";
            this.buttonGotoQuest.Visible = false;
            this.buttonGotoQuest.Click += new System.EventHandler(this.buttonGotoQuest_Click);
            // 
            // buttonFind
            // 
            this.buttonFind.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonFind.Location = new System.Drawing.Point(686, 682);
            this.buttonFind.Name = "buttonFind";
            this.buttonFind.Size = new System.Drawing.Size(75, 23);
            this.buttonFind.TabIndex = 3;
            this.buttonFind.Text = "查找(Enter)";
            this.buttonFind.Click += new System.EventHandler(this.buttonFind_Click);
            // 
            // radioTrinket
            // 
            this.radioTrinket.AutoSize = true;
            this.radioTrinket.Location = new System.Drawing.Point(312, 30);
            this.radioTrinket.Name = "radioTrinket";
            this.radioTrinket.Size = new System.Drawing.Size(89, 16);
            this.radioTrinket.TabIndex = 4;
            this.radioTrinket.TabStop = true;
            this.radioTrinket.Text = "Trinket模板";
            this.radioTrinket.UseVisualStyleBackColor = true;
            this.radioTrinket.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // radioArmor
            // 
            this.radioArmor.AutoSize = true;
            this.radioArmor.Location = new System.Drawing.Point(415, 30);
            this.radioArmor.Name = "radioArmor";
            this.radioArmor.Size = new System.Drawing.Size(77, 16);
            this.radioArmor.TabIndex = 4;
            this.radioArmor.TabStop = true;
            this.radioArmor.Text = "Armor模板";
            this.radioArmor.UseVisualStyleBackColor = true;
            this.radioArmor.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // radioWeapon
            // 
            this.radioWeapon.AutoSize = true;
            this.radioWeapon.Location = new System.Drawing.Point(509, 30);
            this.radioWeapon.Name = "radioWeapon";
            this.radioWeapon.Size = new System.Drawing.Size(83, 16);
            this.radioWeapon.TabIndex = 4;
            this.radioWeapon.TabStop = true;
            this.radioWeapon.Text = "Weapon模板";
            this.radioWeapon.UseVisualStyleBackColor = true;
            this.radioWeapon.CheckedChanged += new System.EventHandler(this.radio_CheckedChanged);
            // 
            // FindFrm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(879, 721);
            this.ControlBox = false;
            this.Controls.Add(this.panelEx1);
            this.Name = "FindFrm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "查找";
            this.TopMost = true;
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FindFrm_FormClosed);
            this.panelEx1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private System.Windows.Forms.RadioButton radioID;
        private DevComponents.DotNetBar.ButtonX buttonFind;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox;
        private System.Windows.Forms.RadioButton radioName;
        private DevComponents.DotNetBar.ButtonX buttonCancel;
        private System.Windows.Forms.RadioButton radioItem;
        private System.Windows.Forms.RadioButton radioDoodad;
        private System.Windows.Forms.RadioButton radioNPC;
        private System.Windows.Forms.DataGridView dataGridView1;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListBox listBox1;
        private DevComponents.DotNetBar.ButtonX buttonGotoQuest;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RadioButton radioWeapon;
        private System.Windows.Forms.RadioButton radioArmor;
        private System.Windows.Forms.RadioButton radioTrinket;
    }
}