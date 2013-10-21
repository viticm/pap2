
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

namespace Fireball.Win32
{
    public static class NativeShellApi
    {

        public const Int32 FILE_ATTRIBUTE_NORMAL = 0x80;
        public static Guid IID_IShellFolder = new Guid("000214E6-0000-0000-C000-000000000046");


        [DllImport("shlwapi.dll", SetLastError = true)]
        public static extern int SHAutoComplete(IntPtr hWnd, UInt32 flags);

        [DllImport("shell32.dll")]
        public static extern Int32 SHGetDesktopFolder(ref IShellFolder ppshf);

        [DllImport("shell32.dll")]
        public static extern IntPtr SHGetFileInfo(string pszPath, uint dwFileAttribs, out SHFILEINFO psfi, uint cbFileInfo, SHGFI uFlags);

        [DllImport("shell32.dll")]
        public static extern IntPtr SHGetFileInfo(IntPtr pIDL, uint dwFileAttributes, out SHFILEINFO psfi, uint cbFileInfo, SHGFI uFlags);

        [DllImport("shell32.dll")]
        public static extern Int32 SHGetSpecialFolderLocation(IntPtr hwndOwner, CSIDL nFolder, ref IntPtr ppidl);

        [DllImport("shell32.dll")]
        public static extern IntPtr ILCombine(IntPtr pIDLParent, IntPtr pIDLChild);

        [DllImport("shell32.dll")]
        public static extern Int32 SHGetPathFromIDList(IntPtr pIDL, StringBuilder strPath);
    }
}
