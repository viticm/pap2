using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace sprViewer
{
    partial class frmTest
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.cbxSkillLevel = new System.Windows.Forms.ComboBox();
            this.labError = new System.Windows.Forms.Label();
            this.pb1 = new System.Windows.Forms.PictureBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb1)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ControlText;
            this.panel1.Controls.Add(this.cbxSkillLevel);
            this.panel1.Controls.Add(this.labError);
            this.panel1.Controls.Add(this.pb1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(469, 359);
            this.panel1.TabIndex = 0;
            this.panel1.SizeChanged += new System.EventHandler(this.panel1_SizeChanged);
            // 
            // cbxSkillLevel
            // 
            this.cbxSkillLevel.Font = new System.Drawing.Font("SimHei", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.cbxSkillLevel.FormattingEnabled = true;
            this.cbxSkillLevel.Location = new System.Drawing.Point(0, 3);
            this.cbxSkillLevel.Name = "cbxSkillLevel";
            this.cbxSkillLevel.Size = new System.Drawing.Size(41, 24);
            this.cbxSkillLevel.TabIndex = 3;
            this.cbxSkillLevel.Visible = false;
            this.cbxSkillLevel.SelectedIndexChanged += new System.EventHandler(this.cbxSkillLevel_SelectedIndexChanged);
            // 
            // labError
            // 
            this.labError.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.labError.AutoSize = true;
            this.labError.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.labError.Location = new System.Drawing.Point(176, 142);
            this.labError.Name = "labError";
            this.labError.Size = new System.Drawing.Size(53, 12);
            this.labError.TabIndex = 2;
            this.labError.Text = "无法显示";
            this.labError.Visible = false;
            // 
            // pb1
            // 
            this.pb1.Location = new System.Drawing.Point(126, 71);
            this.pb1.Name = "pb1";
            this.pb1.Size = new System.Drawing.Size(150, 149);
            this.pb1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pb1.TabIndex = 0;
            this.pb1.TabStop = false;
            // 
            // timer1
            // 
            this.timer1.Interval = 33;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timer2
            // 
            this.timer2.Enabled = true;
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // frmTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(469, 359);
            this.Controls.Add(this.panel1);
            this.Name = "frmTest";
            this.Text = "frmTest";
            this.Load += new System.EventHandler(this.frmTest_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.Panel panel1;
        public System.Windows.Forms.Timer timer1;
        public System.Windows.Forms.Label labError;
        public System.Windows.Forms.PictureBox pb1;
        private System.Windows.Forms.Timer timer2;
        private ComboBox cbxSkillLevel;
    }
}