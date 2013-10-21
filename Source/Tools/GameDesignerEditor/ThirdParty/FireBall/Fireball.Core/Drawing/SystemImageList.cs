#region License

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

// Original stuff grabbed from http://codeproject.com/cs/miscctrl/ExplorerTreeView.asp

#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Win32;
using System.Runtime.InteropServices;



namespace Fireball.Drawing
{
    public class SystemImageList
    {
        #region Member Variables

        // Used to store the handle of the system image list.
        private static IntPtr m_pImgHandle = IntPtr.Zero;

        // Flags whether we've already retrieved the image list handle or not.
        private static Boolean m_bImgInit = false;

        #endregion

        #region Constants

        // TreeView message constants.
        private const UInt32 TVSIL_NORMAL = 0;
        private const UInt32 TVM_SETIMAGELIST = 4361;

        #endregion

        #region Private Methods

        /// <summary>
        /// Retrieves the handle of the system image list.
        /// </summary>
        private static void InitImageList()
        {
            // Only initialize once.
            if (m_bImgInit)
                throw new Exception("The system image list handle has already been retrieved.");

            // Retrieve the info for a fake file so we can get the image list handle.
            SHFILEINFO shInfo = new SHFILEINFO();
            SHGFI dwAttribs =
                SHGFI.SHGFI_USEFILEATTRIBUTES |
                SHGFI.SHGFI_SMALLICON |
                SHGFI.SHGFI_SYSICONINDEX;
            m_pImgHandle = NativeShellApi.SHGetFileInfo(".txt", NativeShellApi.FILE_ATTRIBUTE_NORMAL, out shInfo, (uint)Marshal.SizeOf(shInfo), dwAttribs);

            // Make sure we got the handle.
            if (m_pImgHandle.Equals(IntPtr.Zero))
                throw new Exception("Unable to retrieve system image list handle.");

            // Only allow one initialization.
            m_bImgInit = true;
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Sets the image list for the TreeView to the system image list.
        /// </summary>
        /// <param name="tvwHandle">The window handle of the TreeView control</param>
        public static void SetTVImageList(IntPtr tvwHandle)
        {
            InitImageList();
            Int32 hRes =NativeUser32Api.SendMessage(tvwHandle, TVM_SETIMAGELIST, TVSIL_NORMAL, m_pImgHandle);
            if (hRes != 0)
                Marshal.ThrowExceptionForHR(hRes);
        }

        #endregion
    }
}
