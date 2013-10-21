namespace RoleBrowseTest
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
            if (m_3DEngie != null)
            {
                m_3DEngie.UnloadFile("");
                m_3DEngie.UnInit();
                m_3DEngie = null;
            }
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
            this.label1 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.Player_comboBox = new System.Windows.Forms.ComboBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.PlayerPathText = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.Part_comboBox = new System.Windows.Forms.ComboBox();
            this.PartlistBox = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.Socket_comboBox = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.Item_textBox = new System.Windows.Forms.TextBox();
            this.Item_listBox = new System.Windows.Forms.ListBox();
            this.ItemsClearbutton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.MultiItems_radioButton = new System.Windows.Forms.RadioButton();
            this.SingleItems_radioButton = new System.Windows.Forms.RadioButton();
            this.Ani_listBox = new System.Windows.Forms.ListBox();
            this.Save_button = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.comboBox_Face = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.comboBox_Type = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(36, 47);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "模型：";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(28, 289);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(364, 335);
            this.pictureBox1.TabIndex = 2;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            // 
            // Player_comboBox
            // 
            this.Player_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Player_comboBox.FormattingEnabled = true;
            this.Player_comboBox.Location = new System.Drawing.Point(83, 44);
            this.Player_comboBox.Name = "Player_comboBox";
            this.Player_comboBox.Size = new System.Drawing.Size(103, 20);
            this.Player_comboBox.TabIndex = 3;
            this.Player_comboBox.SelectedIndexChanged += new System.EventHandler(this.comboBox_Player_SelectedIndexChanged);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 33;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // PlayerPathText
            // 
            this.PlayerPathText.Location = new System.Drawing.Point(97, 249);
            this.PlayerPathText.Name = "PlayerPathText";
            this.PlayerPathText.Size = new System.Drawing.Size(295, 21);
            this.PlayerPathText.TabIndex = 5;
            this.PlayerPathText.Text = "E:\\work\\Sword3\\Sword3-Products\\trunk\\client\\data\\public\\PlayerCheckTool";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(26, 252);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 6;
            this.label2.Text = "人物路径：";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(36, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 7;
            this.label3.Text = "部件：";
            // 
            // Part_comboBox
            // 
            this.Part_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Part_comboBox.FormattingEnabled = true;
            this.Part_comboBox.Items.AddRange(new object[] {
            "头",
            "身体",
            "腿",
            "手",
            "腰带",
            "脸"});
            this.Part_comboBox.Location = new System.Drawing.Point(83, 75);
            this.Part_comboBox.Name = "Part_comboBox";
            this.Part_comboBox.Size = new System.Drawing.Size(103, 20);
            this.Part_comboBox.TabIndex = 8;
            this.Part_comboBox.SelectedIndexChanged += new System.EventHandler(this.comboBox_Part_SelectedIndexChanged);
            // 
            // PartlistBox
            // 
            this.PartlistBox.FormattingEnabled = true;
            this.PartlistBox.ItemHeight = 12;
            this.PartlistBox.Location = new System.Drawing.Point(201, 78);
            this.PartlistBox.Name = "PartlistBox";
            this.PartlistBox.Size = new System.Drawing.Size(177, 160);
            this.PartlistBox.TabIndex = 9;
            this.PartlistBox.SelectedIndexChanged += new System.EventHandler(this.PartlistBox_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(36, 108);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(41, 12);
            this.label4.TabIndex = 10;
            this.label4.Text = "插槽：";
            // 
            // Socket_comboBox
            // 
            this.Socket_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Socket_comboBox.FormattingEnabled = true;
            this.Socket_comboBox.Items.AddRange(new object[] {
            "帽子",
            "腰（背部中）",
            "腰（背部低）",
            "右肩",
            "左肩",
            "右腰",
            "左腰",
            "右手",
            "左手"});
            this.Socket_comboBox.Location = new System.Drawing.Point(83, 105);
            this.Socket_comboBox.Name = "Socket_comboBox";
            this.Socket_comboBox.Size = new System.Drawing.Size(103, 20);
            this.Socket_comboBox.TabIndex = 11;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(401, 302);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "动作：";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(201, 43);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(41, 26);
            this.button1.TabIndex = 14;
            this.button1.Text = "刷新";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(387, 19);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(65, 12);
            this.label6.TabIndex = 15;
            this.label6.Text = "道具路径：";
            // 
            // Item_textBox
            // 
            this.Item_textBox.Location = new System.Drawing.Point(448, 15);
            this.Item_textBox.Name = "Item_textBox";
            this.Item_textBox.Size = new System.Drawing.Size(295, 21);
            this.Item_textBox.TabIndex = 16;
            this.Item_textBox.Text = "E:\\work\\Sword3\\Sword3-Products\\trunk\\client\\data\\source\\item";
            // 
            // Item_listBox
            // 
            this.Item_listBox.FormattingEnabled = true;
            this.Item_listBox.ItemHeight = 12;
            this.Item_listBox.Location = new System.Drawing.Point(450, 44);
            this.Item_listBox.Name = "Item_listBox";
            this.Item_listBox.Size = new System.Drawing.Size(292, 244);
            this.Item_listBox.TabIndex = 17;
            this.Item_listBox.SelectedIndexChanged += new System.EventHandler(this.Item_listBox_SelectedIndexChanged);
            // 
            // ItemsClearbutton
            // 
            this.ItemsClearbutton.Location = new System.Drawing.Point(248, 43);
            this.ItemsClearbutton.Name = "ItemsClearbutton";
            this.ItemsClearbutton.Size = new System.Drawing.Size(64, 26);
            this.ItemsClearbutton.TabIndex = 18;
            this.ItemsClearbutton.Text = "清除道具";
            this.ItemsClearbutton.UseVisualStyleBackColor = true;
            this.ItemsClearbutton.Click += new System.EventHandler(this.ItemsClearbutton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.MultiItems_radioButton);
            this.groupBox1.Controls.Add(this.SingleItems_radioButton);
            this.groupBox1.Location = new System.Drawing.Point(38, 173);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(148, 68);
            this.groupBox1.TabIndex = 19;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "道具绑定模式：";
            // 
            // MultiItems_radioButton
            // 
            this.MultiItems_radioButton.AutoSize = true;
            this.MultiItems_radioButton.Location = new System.Drawing.Point(6, 42);
            this.MultiItems_radioButton.Name = "MultiItems_radioButton";
            this.MultiItems_radioButton.Size = new System.Drawing.Size(131, 16);
            this.MultiItems_radioButton.TabIndex = 1;
            this.MultiItems_radioButton.TabStop = true;
            this.MultiItems_radioButton.Text = "同时可绑定多个道具";
            this.MultiItems_radioButton.UseVisualStyleBackColor = true;
            // 
            // SingleItems_radioButton
            // 
            this.SingleItems_radioButton.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.SingleItems_radioButton.AutoSize = true;
            this.SingleItems_radioButton.Checked = true;
            this.SingleItems_radioButton.Location = new System.Drawing.Point(6, 20);
            this.SingleItems_radioButton.Name = "SingleItems_radioButton";
            this.SingleItems_radioButton.Size = new System.Drawing.Size(107, 16);
            this.SingleItems_radioButton.TabIndex = 0;
            this.SingleItems_radioButton.TabStop = true;
            this.SingleItems_radioButton.Text = "只绑定一个道具";
            this.SingleItems_radioButton.UseVisualStyleBackColor = true;
            this.SingleItems_radioButton.CheckedChanged += new System.EventHandler(this.SingleItems_radioButton_CheckedChanged);
            // 
            // Ani_listBox
            // 
            this.Ani_listBox.FormattingEnabled = true;
            this.Ani_listBox.ItemHeight = 12;
            this.Ani_listBox.Location = new System.Drawing.Point(450, 298);
            this.Ani_listBox.Name = "Ani_listBox";
            this.Ani_listBox.Size = new System.Drawing.Size(294, 292);
            this.Ani_listBox.TabIndex = 20;
            this.Ani_listBox.SelectedIndexChanged += new System.EventHandler(this.Ani_listBox_SelectedIndexChanged);
            // 
            // Save_button
            // 
            this.Save_button.Location = new System.Drawing.Point(321, 43);
            this.Save_button.Name = "Save_button";
            this.Save_button.Size = new System.Drawing.Size(57, 26);
            this.Save_button.TabIndex = 21;
            this.Save_button.Text = "保存";
            this.Save_button.UseVisualStyleBackColor = true;
            this.Save_button.Click += new System.EventHandler(this.Save_button_Click);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.Filter = "MDL文件(*.mdl)|*.mdl";
            // 
            // comboBox_Face
            // 
            this.comboBox_Face.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Face.FormattingEnabled = true;
            this.comboBox_Face.Location = new System.Drawing.Point(83, 131);
            this.comboBox_Face.Name = "comboBox_Face";
            this.comboBox_Face.Size = new System.Drawing.Size(103, 20);
            this.comboBox_Face.TabIndex = 23;
            this.comboBox_Face.SelectedIndexChanged += new System.EventHandler(this.comboBox_Face_SelectedIndexChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(36, 134);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(29, 12);
            this.label7.TabIndex = 22;
            this.label7.Text = "脸：";
            // 
            // comboBox_Type
            // 
            this.comboBox_Type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Type.FormattingEnabled = true;
            this.comboBox_Type.Items.AddRange(new object[] {
            "主角模型",
            "NPC",
            "Doodad",
            "其他"});
            this.comboBox_Type.Location = new System.Drawing.Point(83, 12);
            this.comboBox_Type.Name = "comboBox_Type";
            this.comboBox_Type.Size = new System.Drawing.Size(103, 20);
            this.comboBox_Type.TabIndex = 25;
            this.comboBox_Type.SelectedIndexChanged += new System.EventHandler(this.comboBox_Type_SelectedIndexChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(36, 15);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(41, 12);
            this.label8.TabIndex = 24;
            this.label8.Text = "类型：";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(753, 663);
            this.Controls.Add(this.comboBox_Type);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.comboBox_Face);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.Save_button);
            this.Controls.Add(this.Ani_listBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.ItemsClearbutton);
            this.Controls.Add(this.Item_listBox);
            this.Controls.Add(this.Item_textBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.Socket_comboBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.PartlistBox);
            this.Controls.Add(this.Part_comboBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.PlayerPathText);
            this.Controls.Add(this.Player_comboBox);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "角色检查工具";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ComboBox Player_comboBox;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.TextBox PlayerPathText;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox Part_comboBox;
        private System.Windows.Forms.ListBox PartlistBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox Socket_comboBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox Item_textBox;
        private System.Windows.Forms.ListBox Item_listBox;
        private System.Windows.Forms.Button ItemsClearbutton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton SingleItems_radioButton;
        private System.Windows.Forms.RadioButton MultiItems_radioButton;
        private System.Windows.Forms.ListBox Ani_listBox;
        private System.Windows.Forms.Button Save_button;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.ComboBox comboBox_Face;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox comboBox_Type;
        private System.Windows.Forms.Label label8;
    }
}

