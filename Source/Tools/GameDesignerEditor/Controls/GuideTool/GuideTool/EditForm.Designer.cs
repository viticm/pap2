namespace GuideTool
{
    partial class EditForm
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
            this.tabPanel = new System.Windows.Forms.FlowLayoutPanel();
            this.bPreviousStep = new DevComponents.DotNetBar.ButtonX();
            this.bNextStep = new DevComponents.DotNetBar.ButtonX();
            this.bFinish = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.fieldPanel = new System.Windows.Forms.TableLayoutPanel();
            this.descriptionBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.SuspendLayout();
            // 
            // tabPanel
            // 
            this.tabPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.tabPanel.AutoScroll = true;
            this.tabPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tabPanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.tabPanel.Location = new System.Drawing.Point(12, 12);
            this.tabPanel.Name = "tabPanel";
            this.tabPanel.Size = new System.Drawing.Size(178, 427);
            this.tabPanel.TabIndex = 0;
            // 
            // bPreviousStep
            // 
            this.bPreviousStep.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bPreviousStep.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bPreviousStep.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bPreviousStep.Location = new System.Drawing.Point(314, 419);
            this.bPreviousStep.Name = "bPreviousStep";
            this.bPreviousStep.Size = new System.Drawing.Size(70, 20);
            this.bPreviousStep.TabIndex = 1;
            this.bPreviousStep.Text = "上一步";
            this.bPreviousStep.Click += new System.EventHandler(this.bPreviousStep_Click);
            // 
            // bNextStep
            // 
            this.bNextStep.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bNextStep.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bNextStep.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bNextStep.Location = new System.Drawing.Point(390, 419);
            this.bNextStep.Name = "bNextStep";
            this.bNextStep.Size = new System.Drawing.Size(70, 20);
            this.bNextStep.TabIndex = 2;
            this.bNextStep.Text = "下一步";
            this.bNextStep.Click += new System.EventHandler(this.bNextStep_Click);
            // 
            // bFinish
            // 
            this.bFinish.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bFinish.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bFinish.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bFinish.Location = new System.Drawing.Point(466, 419);
            this.bFinish.Name = "bFinish";
            this.bFinish.Size = new System.Drawing.Size(70, 20);
            this.bFinish.TabIndex = 3;
            this.bFinish.Text = "完成";
            this.bFinish.Click += new System.EventHandler(this.bFinish_Click);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.Location = new System.Drawing.Point(582, 419);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 4;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // fieldPanel
            // 
            this.fieldPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fieldPanel.ColumnCount = 3;
            this.fieldPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.fieldPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.fieldPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.fieldPanel.Location = new System.Drawing.Point(208, 12);
            this.fieldPanel.Name = "fieldPanel";
            this.fieldPanel.RowCount = 4;
            this.fieldPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.fieldPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.fieldPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.fieldPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.fieldPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.fieldPanel.Size = new System.Drawing.Size(444, 278);
            this.fieldPanel.TabIndex = 5;
            // 
            // descriptionBox
            // 
            this.descriptionBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.descriptionBox.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.descriptionBox.Border.Class = "TextBoxBorder";
            this.descriptionBox.Location = new System.Drawing.Point(208, 321);
            this.descriptionBox.Multiline = true;
            this.descriptionBox.Name = "descriptionBox";
            this.descriptionBox.ReadOnly = true;
            this.descriptionBox.Size = new System.Drawing.Size(444, 75);
            this.descriptionBox.TabIndex = 6;
            // 
            // EditForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(678, 464);
            this.Controls.Add(this.descriptionBox);
            this.Controls.Add(this.fieldPanel);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bFinish);
            this.Controls.Add(this.bNextStep);
            this.Controls.Add(this.bPreviousStep);
            this.Controls.Add(this.tabPanel);
            this.Name = "EditForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "编辑向导";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.FlowLayoutPanel tabPanel;
        private DevComponents.DotNetBar.ButtonX bPreviousStep;
        private DevComponents.DotNetBar.ButtonX bNextStep;
        private DevComponents.DotNetBar.ButtonX bFinish;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private System.Windows.Forms.TableLayoutPanel fieldPanel;
        private DevComponents.DotNetBar.Controls.TextBoxX descriptionBox;
    }
}