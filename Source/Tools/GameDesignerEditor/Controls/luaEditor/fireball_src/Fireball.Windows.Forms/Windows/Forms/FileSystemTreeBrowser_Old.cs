//Copyright (C) 2004/2005 Sebastian Faltoni
//
//This library is free software; you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.
//
//This library is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this library; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US

using System;
using System.IO;
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Fireball.Drawing;

namespace Fireball.Windows.Forms
{
	public delegate void FileSystemEntryInvokedHandler(FileSystemTreeBrowser.FileSystemEntryType entryType, string path);

	public class FileSystemTreeBrowser  : Widget
	{

		TreeView m_SysTree = null;
		bool m_ShowFiles = true;
		bool m_ShowMessage = true;

		public enum FileSystemEntryType
		{
			Directory = 0,
			Drive = 1,
			File = 2
		}

		public event FileSystemEntryInvokedHandler FileSystemEntryInvoked;



		public FileSystemTreeBrowser()
		{
            m_SysTree = new TreeView();

			this.Controls.Add(m_SysTree);

			m_SysTree.Dock = DockStyle.Fill;

			m_SysTree.ImageList = new ImageList();

			m_SysTree.ImageList.ColorDepth = ColorDepth.Depth32Bit;

			m_SysTree.ItemHeight = 18;

			m_SysTree.HideSelection = false;

			m_SysTree.ImageList.ImageSize = new System.Drawing.Size(16, 16);

			m_SysTree.ShowRootLines = false;

			m_SysTree.Font = new Font("Tahoma", 8);

			m_SysTree.BeforeCollapse += new TreeViewCancelEventHandler(m_SysTree_BeforeCollapse);

			m_SysTree.BeforeExpand += new TreeViewCancelEventHandler(m_SysTree_BeforeExpand);

			m_SysTree.DoubleClick += new EventHandler(m_SysTree_DoubleClick);

			Populate();
		}

		void m_SysTree_DoubleClick(object sender, EventArgs e)
		{
			if (FileSystemEntryInvoked != null)
			{
				FileSystemEntryInvoked(CurrentEntryType, CurrentEntryPath);
			}
		}

		public FileSystemEntryType CurrentEntryType
		{
			get
			{
				if (m_SysTree.SelectedNode is DriveNode)
				{
					return FileSystemEntryType.Drive;
				}
				else if(m_SysTree.SelectedNode is FileSystemNode)
				{
					if (((FileSystemNode)m_SysTree.SelectedNode).IsFolder)
						return FileSystemEntryType.Directory;
					
				}
				return FileSystemEntryType.File;
			}
		}

		public string CurrentEntryPath
		{
			get
			{
				return ((FileSystemNode)m_SysTree.SelectedNode).Path;
			}
		}

		void m_SysTree_BeforeExpand(object sender, TreeViewCancelEventArgs e)
		{
			FileSystemNode node = e.Node as FileSystemNode;

			if (node != null)
			{
				if (!node.IsDesktop)
				{
					if (node.Nodes.Count == 1 && (node.CanHaveChilds))
					{
						node.Nodes.Clear();

						string path = node.Path;

						string[] dirs = Directory.GetDirectories(path);

						foreach (string dir in dirs)
						{
							string dirName = GetDirName(dir);

							DirectoryInfo infoDir = new DirectoryInfo(dir);

							bool noAccess = infoDir.GetAccessControl().AreAccessRulesProtected;

							if (noAccess) continue;

							FileSystemNode newDir = new FileSystemNode(dirName,
								dir, true, "folder", m_ShowFiles);

							node.Nodes.Add(newDir);
						}

						if (m_ShowFiles)
						{
							string[] files = Directory.GetFiles(path);

							foreach (string file in files)
							{
								string dirName = GetDirName(file);

								FileSystemNode newDir = new FileSystemNode(dirName,
									file, false, GetIconForFile(file), m_ShowFiles);

								node.Nodes.Add(newDir);
							}
						}
					}
					else
					{
						if (node is DriveNode)
						{
							DriveNode drvnode = node as DriveNode;

							if (!drvnode.IsReady)
							{
								e.Cancel = true;

								if (m_ShowMessage)
									MessageBox.Show(drvnode.Path +  " Is Not Ready","Device Error:",
										MessageBoxButtons.OK,MessageBoxIcon.Exclamation);
								return;
							}

						}
					}
				}
			}
		}

		string GetIconForFile(string path)
		{
			string ext = Path.GetExtension(path);

			int index = m_SysTree.ImageList.Images.IndexOfKey(ext);

			if (index != -1)
			{
				return ext;
			}
			else
			{
				Icon icon = Icon.ExtractAssociatedIcon(path);

				m_SysTree.ImageList.Images.Add(ext, icon.ToBitmap());

				return ext;
			}
		}

		string GetDirName(string path)
		{
			int lastSlash = path.LastIndexOfAny(new char[] { '\\', '/' });

			string pt = path.Substring(lastSlash + 1);

			if (pt.EndsWith("\\") || pt.EndsWith("/"))
				return pt.Substring(0, pt.Length - 1);

			return pt;
		}

		void m_SysTree_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
		{
			FileSystemNode node = e.Node as FileSystemNode;

			if (node != null)
			{
				if (node.IsDesktop)
					e.Cancel = true;
			}
		}

		void Populate()
		{
			string Desktop = System.Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);

			FileSystemNode desk = new FileSystemNode("Desktop", Desktop, false, "desktop", m_ShowFiles);			
		
			desk.IsDesktop = true;

			m_SysTree.Nodes.Add(desk);

			string mydocs = System.Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

			FileSystemNode docs = new FileSystemNode("MyDocuments", mydocs, true, "mydocs", m_ShowFiles);

			desk.Nodes.Add(docs);

		//	int index = 3;
//
			//Icon ico = Icon.ExtractAssociatedIcon("c:\\windows\\Explorer.exe",index);

			//Bitmap bmp = ico.ToBitmap();

			m_SysTree.ImageList.Images.Add("desktop", Properties.Resources.desktop);

		//	ico = Icon.ExtractAssociatedIcon("c:\\windows\\Explorer.exe", index);

			m_SysTree.ImageList.Images.Add("mydocs", Properties.Resources.folder_document);
			m_SysTree.ImageList.Images.Add("folder", Properties.Resources.folder);

			m_SysTree.ImageList.Images.Add("hd", Properties.Resources.diskdrive);
			m_SysTree.ImageList.Images.Add("cdrom", Properties.Resources.cd);
			m_SysTree.ImageList.Images.Add("network", Properties.Resources.harddisk_network);

			DriveInfo[] drives = DriveInfo.GetDrives();

			foreach (DriveInfo drive in drives)
			{
				desk.Nodes.Add(new DriveNode(drive));
			}
			

			desk.Expand();
		}

		class DriveNode : FileSystemNode
		{
			DriveInfo m_Info = null;

			public DriveNode(DriveInfo info)
			{
				m_Info = info;
				this.Nodes.Add("fake");

				if (info.DriveType == DriveType.CDRom)
				{
					this.ImageKey = "cdrom";
				}
				else if (info.DriveType == DriveType.Fixed)
				{
					this.ImageKey = "hd";
				}
				else if (info.DriveType == DriveType.Network)
				{
					this.ImageKey = "network";
				}
				else if (info.DriveType == DriveType.Ram)
				{
					this.ImageKey = "ram";
				}
			

				this.SelectedImageKey = this.ImageKey;

				try
				{
					this.Text = info.RootDirectory.FullName + " " + info.VolumeLabel;

					if(this.Text == string.Empty)
						this.Text = info.RootDirectory.FullName;
				}
				catch
				{
					this.Text = info.RootDirectory.FullName;
				}
				
			}

			public virtual bool IsReady
			{
				get
				{
					return m_Info.IsReady;
				}
			}

			public override bool IsDesktop
			{
				get
				{
					return false;
				}
				set
				{
					//NOP
				}
			}

			public override string Path
			{
				get
				{
					return m_Info.RootDirectory.FullName;
				}
				set
				{
					//NOP
				}
			}


			public override bool CanHaveChilds
			{
				get
				{
					if (m_Info.IsReady && ((Directory.GetFiles(this.Path).Length > 0)

						|| (Directory.GetDirectories(this.Path).Length > 0)))
					{
						return true;
					}
					return false;
				}
				set
				{
					throw new Exception("The method or operation is not implemented.");
				}
			}

			public override bool IsFolder
			{
				get
				{
					return true;
				}
				set
				{
					//NOP
				}
			}
		}

		class FileSystemNode : TreeNode
		{
			string m_Path = null;

			bool m_IsFolder = false;
			bool m_IsDesktop = false;
			bool m_CanHaveChilds = false;
			bool m_ShowFiles = false;

			public FileSystemNode()
			{

			}


			public FileSystemNode(string name, string path, bool isFolder, 
				string imgKey, bool showFiles)
			{
				this.Text = name;
				m_Path = path;
				m_IsFolder = isFolder;

				this.ImageKey = imgKey;

				this.SelectedImageKey = this.ImageKey;

				m_ShowFiles = showFiles;

				if (isFolder)
				{				
					if (showFiles && (Directory.GetFiles(path).Length > 0))
					{
						m_CanHaveChilds = true;
						this.Nodes.Add("fake");
					}
					else
					{
						if (Directory.GetDirectories(path).Length > 0)
						{
							m_CanHaveChilds = true;
							this.Nodes.Add("fake");
						}
					}
				}
			}

			public virtual bool CanHaveChilds
			{
				get { return m_CanHaveChilds; }
				set { m_CanHaveChilds = value; }
			}

			public virtual bool IsDesktop
			{
				get { return m_IsDesktop; }
				set { m_IsDesktop = value; }
			}

			public virtual string Path
			{
				get { return m_Path; }
				set { m_Path = value; }
			}

			public virtual bool IsFolder
			{
				get { return m_IsFolder; }
				set { m_IsFolder = value; }
			}

		}
	}
}
