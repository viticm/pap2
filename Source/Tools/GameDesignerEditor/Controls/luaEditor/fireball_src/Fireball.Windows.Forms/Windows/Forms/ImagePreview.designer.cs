namespace Fireball.Windows.Forms
{
    partial class ImagePreview
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ImagePreview));
            this.toolStripPreview = new System.Windows.Forms.ToolStrip();
            this.tbZoomPlus = new System.Windows.Forms.ToolStripButton();
            this.tbZoomMinus = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.tbZoom100 = new System.Windows.Forms.ToolStripButton();
            this.tbZoomFit = new System.Windows.Forms.ToolStripButton();
            this.picturePreview = new Fireball.Windows.Forms.PicturePreview();
            this.toolStripPreview.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStripPreview
            // 
            this.toolStripPreview.BackColor = System.Drawing.SystemColors.ControlLight;
            this.toolStripPreview.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStripPreview.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tbZoomPlus,
            this.tbZoomMinus,
            this.toolStripSeparator1,
            this.tbZoom100,
            this.tbZoomFit});
            this.toolStripPreview.Location = new System.Drawing.Point(0, 361);
            this.toolStripPreview.Name = "toolStripPreview";
            this.toolStripPreview.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.toolStripPreview.Size = new System.Drawing.Size(342, 25);
            this.toolStripPreview.Stretch = true;
            this.toolStripPreview.TabIndex = 0;
            this.toolStripPreview.Text = "toolStrip1";
            // 
            // tbZoomPlus
            // 
            this.tbZoomPlus.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbZoomPlus.Image = ((System.Drawing.Image)(resources.GetObject("tbZoomPlus.Image")));
            this.tbZoomPlus.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tbZoomPlus.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbZoomPlus.Name = "tbZoomPlus";
            this.tbZoomPlus.Text = "Zoom +";
            this.tbZoomPlus.Click += new System.EventHandler(this.tbZoomPlus_Click);
            // 
            // tbZoomMinus
            // 
            this.tbZoomMinus.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbZoomMinus.Image = ((System.Drawing.Image)(resources.GetObject("tbZoomMinus.Image")));
            this.tbZoomMinus.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tbZoomMinus.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbZoomMinus.Name = "tbZoomMinus";
            this.tbZoomMinus.Text = "Zoom -";
            this.tbZoomMinus.Click += new System.EventHandler(this.tbZoomMinus_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            // 
            // tbZoom100
            // 
            this.tbZoom100.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbZoom100.Image = ((System.Drawing.Image)(resources.GetObject("tbZoom100.Image")));
            this.tbZoom100.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tbZoom100.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbZoom100.Name = "tbZoom100";
            this.tbZoom100.Text = "Zoom 100%";
            this.tbZoom100.Click += new System.EventHandler(this.tbZoom100_Click);
            // 
            // tbZoomFit
            // 
            this.tbZoomFit.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbZoomFit.Image = ((System.Drawing.Image)(resources.GetObject("tbZoomFit.Image")));
            this.tbZoomFit.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbZoomFit.Name = "tbZoomFit";
            this.tbZoomFit.Text = "Zoom tutto";
            this.tbZoomFit.Visible = false;
            this.tbZoomFit.Click += new System.EventHandler(this.tbZoomFit_Click);
            // 
            // picturePreview
            // 
            this.picturePreview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.picturePreview.Image = null;
            this.picturePreview.Info = null;
            this.picturePreview.Location = new System.Drawing.Point(0, 0);
            this.picturePreview.Name = "picturePreview";
            this.picturePreview.Size = new System.Drawing.Size(342, 361);
            this.picturePreview.TabIndex = 1;
            this.picturePreview.Text = "picturePreview1";
            this.picturePreview.Zoom = 1F;
            // 
            // ImagePreview
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.picturePreview);
            this.Controls.Add(this.toolStripPreview);
            this.Name = "ImagePreview";
            this.Size = new System.Drawing.Size(342, 386);
            this.toolStripPreview.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStripPreview;
        private System.Windows.Forms.ToolStripButton tbZoomPlus;
        private System.Windows.Forms.ToolStripButton tbZoomMinus;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton tbZoom100;
        private System.Windows.Forms.ToolStripButton tbZoomFit;
        private PicturePreview picturePreview;
    }
}
