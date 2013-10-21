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

namespace Fireball.Windows.Forms
{
    partial class FileSystemTreeBrowser
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
            this.treeWnd = new ExplorerTreeViewWnd();
            this.SuspendLayout();
            // 
            // treeWnd
            // 
            this.treeWnd.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeWnd.Location = new System.Drawing.Point(0, 0);
            this.treeWnd.Name = "treeWnd";
            this.treeWnd.Size = new System.Drawing.Size(289, 454);
            this.treeWnd.TabIndex = 0;
            // 
            // ExplorerTreeView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.treeWnd);
            this.Name = "ExplorerTreeView";
            this.Size = new System.Drawing.Size(289, 454);
            this.ResumeLayout(false);

        }

        #endregion

        private ExplorerTreeViewWnd treeWnd;
    }
}
