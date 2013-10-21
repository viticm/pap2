namespace Fireball.Windows.Forms.Dialogs
{
	partial class WizardForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WizardForm));
            this.panel1 = new System.Windows.Forms.Panel();
            this.lblTitle = new System.Windows.Forms.Label();
            this.picLogo = new System.Windows.Forms.PictureBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.btnAnnulla = new System.Windows.Forms.Button();
            this.btnBack = new System.Windows.Forms.Button();
            this.btnNext = new System.Windows.Forms.Button();
            this.panelSteps = new System.Windows.Forms.Panel();
            this.leftPanel = new System.Windows.Forms.PictureBox();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picLogo)).BeginInit();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.leftPanel)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.White;
            this.panel1.Controls.Add(this.lblTitle);
            this.panel1.Controls.Add(this.picLogo);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(481, 71);
            this.panel1.TabIndex = 0;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // lblTitle
            // 
            this.lblTitle.AutoSize = true;
            this.lblTitle.BackColor = System.Drawing.Color.Transparent;
            this.lblTitle.Font = new System.Drawing.Font("Verdana", 26.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTitle.ForeColor = System.Drawing.Color.MediumSeaGreen;
            this.lblTitle.Location = new System.Drawing.Point(75, 11);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(130, 42);
            this.lblTitle.TabIndex = 1;
            this.lblTitle.Text = "Wizard";
            // 
            // picLogo
            // 
            this.picLogo.BackColor = System.Drawing.Color.Transparent;
            this.picLogo.Image = ((System.Drawing.Image)(resources.GetObject("picLogo.Image")));
            this.picLogo.Location = new System.Drawing.Point(13, 13);
            this.picLogo.Name = "picLogo";
            this.picLogo.Size = new System.Drawing.Size(42, 40);
            this.picLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.picLogo.TabIndex = 0;
            this.picLogo.TabStop = false;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.btnAnnulla);
            this.panel3.Controls.Add(this.btnBack);
            this.panel3.Controls.Add(this.btnNext);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(0, 336);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(481, 41);
            this.panel3.TabIndex = 2;
            this.panel3.Paint += new System.Windows.Forms.PaintEventHandler(this.panel3_Paint);
            // 
            // btnAnnulla
            // 
            this.btnAnnulla.Location = new System.Drawing.Point(3, 6);
            this.btnAnnulla.Name = "btnAnnulla";
            this.btnAnnulla.Size = new System.Drawing.Size(75, 27);
            this.btnAnnulla.TabIndex = 2;
            this.btnAnnulla.Text = "Annulla";
            this.btnAnnulla.Click += new System.EventHandler(this.btnAnnulla_Click);
            // 
            // btnBack
            // 
            this.btnBack.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnBack.Location = new System.Drawing.Point(311, 7);
            this.btnBack.Name = "btnBack";
            this.btnBack.Size = new System.Drawing.Size(75, 27);
            this.btnBack.TabIndex = 1;
            this.btnBack.Text = "Precedente";
            this.btnBack.Click += new System.EventHandler(this.btnBack_Click);
            // 
            // btnNext
            // 
            this.btnNext.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnNext.Location = new System.Drawing.Point(392, 7);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(75, 27);
            this.btnNext.TabIndex = 0;
            this.btnNext.Text = "Successivo";
            this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // panelSteps
            // 
            this.panelSteps.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelSteps.Location = new System.Drawing.Point(153, 71);
            this.panelSteps.Name = "panelSteps";
            this.panelSteps.Padding = new System.Windows.Forms.Padding(1);
            this.panelSteps.Size = new System.Drawing.Size(328, 265);
            this.panelSteps.TabIndex = 3;
            this.panelSteps.Paint += new System.Windows.Forms.PaintEventHandler(this.panelSteps_Paint);
            // 
            // leftPanel
            // 
            this.leftPanel.BackColor = System.Drawing.Color.CornflowerBlue;
            this.leftPanel.Dock = System.Windows.Forms.DockStyle.Left;
            this.leftPanel.Image = ((System.Drawing.Image)(resources.GetObject("leftPanel.Image")));
            this.leftPanel.Location = new System.Drawing.Point(0, 71);
            this.leftPanel.Name = "leftPanel";
            this.leftPanel.Size = new System.Drawing.Size(153, 265);
            this.leftPanel.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.leftPanel.TabIndex = 4;
            this.leftPanel.TabStop = false;
            // 
            // WizardForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(481, 377);
            this.Controls.Add(this.panelSteps);
            this.Controls.Add(this.leftPanel);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panel3);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "WizardForm";
            this.Text = "Wizard";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picLogo)).EndInit();
            this.panel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.leftPanel)).EndInit();
            this.ResumeLayout(false);

		}

		#endregion

		protected System.Windows.Forms.Panel panel1;
		protected System.Windows.Forms.Panel panel3;
		protected System.Windows.Forms.Button btnNext;
		protected System.Windows.Forms.Button btnBack;
		protected System.Windows.Forms.Button btnAnnulla;
		protected System.Windows.Forms.Panel panelSteps;
		protected System.Windows.Forms.PictureBox picLogo;
		protected System.Windows.Forms.PictureBox leftPanel;
		protected System.Windows.Forms.Label lblTitle;

	}
}