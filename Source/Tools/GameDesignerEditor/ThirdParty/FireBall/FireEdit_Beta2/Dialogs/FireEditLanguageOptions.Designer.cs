#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#endregion License

namespace FireEdit.Dialogs
{
    partial class FireEditLanguageOptions
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
            this.cmbLanguages = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.lstStyles = new System.Windows.Forms.ListBox();
            this.propGrid = new System.Windows.Forms.PropertyGrid();
            this.SuspendLayout();
            // 
            // cmbLanguages
            // 
            this.cmbLanguages.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLanguages.FormattingEnabled = true;
            this.cmbLanguages.Location = new System.Drawing.Point(12, 30);
            this.cmbLanguages.Name = "cmbLanguages";
            this.cmbLanguages.Size = new System.Drawing.Size(315, 21);
            this.cmbLanguages.TabIndex = 0;
            this.cmbLanguages.SelectedIndexChanged += new System.EventHandler(this.cmbLanguages_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Language";
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(86, 261);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 3;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(167, 261);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // lstStyles
            // 
            this.lstStyles.FormattingEnabled = true;
            this.lstStyles.Location = new System.Drawing.Point(12, 67);
            this.lstStyles.Name = "lstStyles";
            this.lstStyles.Size = new System.Drawing.Size(115, 173);
            this.lstStyles.TabIndex = 5;
            this.lstStyles.SelectedIndexChanged += new System.EventHandler(this.lstStyles_SelectedIndexChanged);
            // 
            // propGrid
            // 
            this.propGrid.HelpVisible = false;
            this.propGrid.Location = new System.Drawing.Point(134, 67);
            this.propGrid.Name = "propGrid";
            this.propGrid.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.propGrid.Size = new System.Drawing.Size(193, 173);
            this.propGrid.TabIndex = 6;
            this.propGrid.ToolbarVisible = false;
            this.propGrid.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propGrid_PropertyValueChanged);
            // 
            // FireEditLanguageOptions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(339, 290);
            this.Controls.Add(this.propGrid);
            this.Controls.Add(this.lstStyles);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmbLanguages);
            this.MaximizeBox = false;
            this.Name = "FireEditLanguageOptions";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Syntax Options";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox cmbLanguages;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.ListBox lstStyles;
        private System.Windows.Forms.PropertyGrid propGrid;
    }
}
