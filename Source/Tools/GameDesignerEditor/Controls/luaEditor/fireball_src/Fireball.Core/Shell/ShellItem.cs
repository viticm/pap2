
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
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


using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Fireball.Win32;
using System.Collections;

namespace Fireball.Shell
{
    //Original stuff grabbed from http://codeproject.com/cs/miscctrl/ExplorerTreeView.asp

    public class ShellItem
    {
        #region Private Member Variables

        // Sets a flag specifying whether or not we've got the IShellFolder interface for the Desktop.
        private static Boolean m_bHaveRootShell = false;

        #endregion

        #region Constructor

        /// <summary>
        /// Constructor. Creates the ShellItem object for the Desktop.
        /// </summary>
        public ShellItem()
        {
            // new ShellItem() can only be called once.
            if (m_bHaveRootShell)
                throw new Exception("The Desktop shell item already exists so cannot be created again.");

            // Obtain the root IShellFolder interface.
            int hRes = NativeShellApi.SHGetDesktopFolder(ref m_shRootShell);
            if (hRes != 0)
                Marshal.ThrowExceptionForHR(hRes);

            // Now get the PIDL for the Desktop shell item.
            hRes = NativeShellApi.SHGetSpecialFolderLocation(IntPtr.Zero, CSIDL.CSIDL_DESKTOP, ref m_pIDL);
            if (hRes != 0)
                Marshal.ThrowExceptionForHR(hRes);

            // Now retrieve some attributes for the root shell item.
            SHFILEINFO shInfo = new SHFILEINFO();
            NativeShellApi.SHGetFileInfo(m_pIDL, 0, out shInfo, (uint)Marshal.SizeOf(shInfo),
                SHGFI.SHGFI_DISPLAYNAME |
                SHGFI.SHGFI_PIDL |
                SHGFI.SHGFI_SMALLICON |
                SHGFI.SHGFI_SYSICONINDEX
            );

            // Set the arributes to object properties.
            DisplayName = shInfo.szDisplayName;
            IconIndex = shInfo.iIcon;
            IsFolder = true;
            HasSubFolder = true;
            Path = GetPath();

            // Internal with no set{} mutator.
            m_shShellFolder = RootShellFolder;
            m_bHaveRootShell = true;
        }

        /// <summary>
        /// Constructor. Create a sub-item shell item object.
        /// </summary>
        /// <param name="shDesktop">IShellFolder interface of the Desktop</param>
        /// <param name="pIDL">The fully qualified PIDL for this shell item</param>
        /// <param name="shParent">The ShellItem object for this item's parent</param>
        public ShellItem(IShellFolder shDesktop, IntPtr pIDL, ShellItem shParent)
        {
            // We need the Desktop shell item to exist first.
            if (m_bHaveRootShell == false)
                throw new Exception("The root shell item must be created before creating a sub-item");

            // Create the FQ PIDL for this new item.
            m_pIDL = NativeShellApi.ILCombine(shParent.PIDL, pIDL);

            // Get the properties of this item.
            SFGAOF uFlags = SFGAOF.SFGAO_FOLDER | SFGAOF.SFGAO_HASSUBFOLDER;

            // Here we get some basic attributes.
            shDesktop.GetAttributesOf(1, out m_pIDL, out uFlags);
            IsFolder = Convert.ToBoolean(uFlags & SFGAOF.SFGAO_FOLDER);
            HasSubFolder = Convert.ToBoolean(uFlags & SFGAOF.SFGAO_HASSUBFOLDER);

            // Now we want to get extended attributes such as the icon index etc.
            SHFILEINFO shInfo = new SHFILEINFO();
            SHGFI vFlags =
                SHGFI.SHGFI_SMALLICON |
                SHGFI.SHGFI_SYSICONINDEX |
                SHGFI.SHGFI_PIDL |
                SHGFI.SHGFI_DISPLAYNAME;
            NativeShellApi.SHGetFileInfo(m_pIDL, 0, out shInfo, (uint)Marshal.SizeOf(shInfo), vFlags);
            DisplayName = shInfo.szDisplayName;
            IconIndex = shInfo.iIcon;
            Path = GetPath();

            // Create the IShellFolder interface for this item.
            if (IsFolder)
            {
                uint hRes = shParent.m_shShellFolder.BindToObject(pIDL, IntPtr.Zero, ref NativeShellApi.IID_IShellFolder, out m_shShellFolder);
                if (hRes != 0)
                    Marshal.ThrowExceptionForHR((int)hRes);
            }
        }

        #endregion

        #region Destructor

        ~ShellItem()
        {
            // Release the IShellFolder interface of this shell item.
            if (m_shShellFolder != null)
                Marshal.ReleaseComObject(m_shShellFolder);

            // Free the PIDL too.
            if (!m_pIDL.Equals(IntPtr.Zero))
                Marshal.FreeCoTaskMem(m_pIDL);

            GC.SuppressFinalize(this);
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Gets the system path for this shell item.
        /// </summary>
        /// <returns>A path string.</returns>
        public string GetPath()
        {
            StringBuilder strBuffer = new StringBuilder(256);
            NativeShellApi.SHGetPathFromIDList(
                m_pIDL,
                strBuffer
            );
            return strBuffer.ToString();
        }

        /// <summary>
        /// Retrieves an array of ShellItem objects for sub-folders of this shell item.
        /// </summary>
        /// <returns>ArrayList of ShellItem objects.</returns>
        public ArrayList GetSubFolders()
        {
            // Make sure we have a folder.
            if (IsFolder == false)
                throw new Exception("Unable to retrieve sub-folders for a non-folder.");

            ArrayList arrChildren = new ArrayList();
            try
            {
                // Get the IEnumIDList interface pointer.
                IEnumIDList pEnum = null;
                uint hRes = ShellFolder.EnumObjects(IntPtr.Zero, SHCONTF.SHCONTF_FOLDERS, out pEnum);
                if (hRes != 0)
                    Marshal.ThrowExceptionForHR((int)hRes);

                IntPtr pIDL = IntPtr.Zero;
                Int32 iGot = 0;

                // Grab the first enumeration.
                pEnum.Next(1, out pIDL, out iGot);

                // Then continue with all the rest.
                while (!pIDL.Equals(IntPtr.Zero) && iGot == 1)
                {
                    // Create the new ShellItem object.
                    arrChildren.Add(new ShellItem(m_shRootShell, pIDL, this));

                    // Free the PIDL and reset counters.
                    Marshal.FreeCoTaskMem(pIDL);
                    pIDL = IntPtr.Zero;
                    iGot = 0;

                    // Grab the next item.
                    pEnum.Next(1, out pIDL, out iGot);
                }

                // Free the interface pointer.
                if (pEnum != null)
                    Marshal.ReleaseComObject(pEnum);
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message, "Error:",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error
                );
            }

            return arrChildren;
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets or set the display name for this shell item.
        /// </summary>
        public string DisplayName
        {
            get { return m_strDisplayName; }
            set { m_strDisplayName = value; }
        }
        string m_strDisplayName = "";

        /// <summary>
        /// Gets or sets the system image list icon index for this shell item.
        /// </summary>
        public Int32 IconIndex
        {
            get { return m_iIconIndex; }
            set { m_iIconIndex = value; }
        }
        Int32 m_iIconIndex = -1;

        /// <summary>
        /// Gets the IShellFolder interface of the Desktop.
        /// </summary>
        public IShellFolder RootShellFolder
        {
            get { return m_shRootShell; }
        }
        static IShellFolder m_shRootShell = null;

        /// <summary>
        /// Gets the IShellFolder interface of this shell item.
        /// </summary>
        public IShellFolder ShellFolder
        {
            get { return m_shShellFolder; }
        }

        IShellFolder m_shShellFolder = null;

        /// <summary>
        /// Gets the fully qualified PIDL for this shell item.
        /// </summary>
        public IntPtr PIDL
        {
            get { return m_pIDL; }
        }
        IntPtr m_pIDL = IntPtr.Zero;

        /// <summary>
        /// Gets or sets a boolean indicating whether this shell item is a folder.
        /// </summary>
        public bool IsFolder
        {
            get { return m_bIsFolder; }
            set { m_bIsFolder = value; }
        }
        bool m_bIsFolder = false;

        /// <summary>
        /// Gets or sets a boolean indicating whether this shell item has any sub-folders.
        /// </summary>
        public bool HasSubFolder
        {
            get { return m_bHasSubFolder; }
            set { m_bHasSubFolder = value; }
        }
        bool m_bHasSubFolder = false;

        /// <summary>
        /// Gets or sets the system path for this shell item.
        /// </summary>
        public string Path
        {
            get { return m_strPath; }
            set { m_strPath = value; }
        }
        string m_strPath = "";

        #endregion
    }
}
