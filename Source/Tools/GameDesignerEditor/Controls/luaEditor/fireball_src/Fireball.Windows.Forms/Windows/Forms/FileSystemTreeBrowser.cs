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

//Original stuff grabbed from http://codeproject.com/cs/miscctrl/ExplorerTreeView.asp


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using Fireball.Shell;
using Fireball.Drawing;

namespace Fireball.Windows.Forms
{
    public partial class FileSystemTreeBrowser : UserControl
    {
        public event EventHandler SelectedPathChanged;

        public FileSystemTreeBrowser()
        {
            InitializeComponent();

            // Set the TreeView image list to the system image list.
            SystemImageList.SetTVImageList(treeWnd.Handle);
            LoadRootNodes();

            treeWnd.AfterSelect += new TreeViewEventHandler(treeWnd_AfterSelect);
        }

        void treeWnd_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (SelectedPathChanged != null)
                SelectedPathChanged(this, EventArgs.Empty);
        }

        public ShellItem CurrentShellItem
        {
            get
            {
                return treeWnd.SelectedNode.Tag as ShellItem;
            }
        }

        /// <summary>
        /// Loads the root TreeView nodes.
        /// </summary>
        private void LoadRootNodes()
        {
            // Create the root shell item.
            ShellItem m_shDesktop = new ShellItem();

            // Create the root node.
            TreeNode tvwRoot = new TreeNode();
            tvwRoot.Text = m_shDesktop.DisplayName;
            tvwRoot.ImageIndex = m_shDesktop.IconIndex;
            tvwRoot.SelectedImageIndex = m_shDesktop.IconIndex;
            tvwRoot.Tag = m_shDesktop;

            // Now we need to add any children to the root node.
            ArrayList arrChildren = m_shDesktop.GetSubFolders();
            foreach (ShellItem shChild in arrChildren)
            {
                TreeNode tvwChild = new TreeNode();
                tvwChild.Text = shChild.DisplayName;
                tvwChild.ImageIndex = shChild.IconIndex;
                tvwChild.SelectedImageIndex = shChild.IconIndex;
                tvwChild.Tag = shChild;

                // If this is a folder item and has children then add a place holder node.
                if (shChild.IsFolder && shChild.HasSubFolder)
                    tvwChild.Nodes.Add("PH");
                tvwRoot.Nodes.Add(tvwChild);
            }

            // Add the root node to the tree.
            treeWnd.Nodes.Clear();
            treeWnd.Nodes.Add(tvwRoot);
            tvwRoot.Expand();
        }
    }

    class ExplorerTreeViewWnd : TreeView
    {
        protected override void OnBeforeExpand(TreeViewCancelEventArgs e)
        {
            // Remove the placeholder node.
            e.Node.Nodes.Clear();

            // We stored the ShellItem object in the node's Tag property - hah!
            ShellItem shNode = (ShellItem)e.Node.Tag;
            ArrayList arrSub = shNode.GetSubFolders();
            foreach (ShellItem shChild in arrSub)
            {
                TreeNode tvwChild = new TreeNode();
                tvwChild.Text = shChild.DisplayName;
                tvwChild.ImageIndex = shChild.IconIndex;
                tvwChild.SelectedImageIndex = shChild.IconIndex;
                tvwChild.Tag = shChild;

                // If this is a folder item and has children then add a place holder node.
                if (shChild.IsFolder && shChild.HasSubFolder)
                    tvwChild.Nodes.Add("PH");
                e.Node.Nodes.Add(tvwChild);
            }

            base.OnBeforeExpand(e);
        }
    }
}
