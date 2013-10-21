using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Collections;
using System.Drawing;

namespace ClientCompare
{
    class IconManager
    {
        private ImageList largeIconImageList = new ImageList();
        private ImageList smallIconImageList = new ImageList();
        private Hashtable largeIconTable = new Hashtable();
        private Hashtable smallIconTable = new Hashtable();

        [StructLayout(LayoutKind.Sequential)]
        public struct FileInfoStruct
        {
            public IntPtr hIcon;
            public int iIcon;
            public int dwAttributes;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string szDisplayName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
            public string szTypeName;            
        }

        public enum FileInfoFlags
        {
            SHGFI_ICON  = 0x000000100 ,  //  get icon 
            SHGFI_DISPLAYNAME  = 0x000000200 ,  //  get display name 
            SHGFI_TYPENAME  = 0x000000400 ,  //  get type name 
            SHGFI_ATTRIBUTES  = 0x000000800 ,  //  get attributes 
            SHGFI_ICONLOCATION  = 0x000001000 ,  //  get icon location 
            SHGFI_EXETYPE  = 0x000002000 ,  //  return exe type 
            SHGFI_SYSICONINDEX  = 0x000004000 ,  //  get system icon index 
            SHGFI_LINKOVERLAY  = 0x000008000 ,  //  put a link overlay on icon  SHGFI_SELECTED  = 0x000010000 ,  //  show icon in selected state 
            SHGFI_ATTR_SPECIFIED  = 0x000020000 ,  //  get only specified attributes 
            SHGFI_LARGEICON  = 0x000000000 ,  //  get large icon 
            SHGFI_SMALLICON  = 0x000000001 ,  //  get small icon 
            SHGFI_OPENICON  = 0x000000002 ,  //  get open icon 
            SHGFI_SHELLICONSIZE  = 0x000000004 ,  //  get shell size icon 
            SHGFI_PIDL  = 0x000000008 ,  //  pszPath is a pidl 
            SHGFI_USEFILEATTRIBUTES  = 0x000000010 ,  //  use passed dwFileAttribute 
            SHGFI_ADDOVERLAYS  = 0x000000020 ,  //  apply the appropriate overlays 
            SHGFI_OVERLAYINDEX  = 0x000000040   //  Get the index of the overlay 
        }

        public enum FileAttributeFlags
        {
            FILE_ATTRIBUTE_READONLY = 0x00000001 ,
            FILE_ATTRIBUTE_HIDDEN = 0x00000002 ,
            FILE_ATTRIBUTE_SYSTEM = 0x00000004 ,
            FILE_ATTRIBUTE_DIRECTORY = 0x00000010 ,
            FILE_ATTRIBUTE_ARCHIVE = 0x00000020 ,
            FILE_ATTRIBUTE_DEVICE = 0x00000040 ,
            FILE_ATTRIBUTE_NORMAL = 0x00000080 ,
            FILE_ATTRIBUTE_TEMPORARY = 0x00000100 ,
            FILE_ATTRIBUTE_SPARSE_FILE = 0x00000200 ,
            FILE_ATTRIBUTE_REPARSE_POINT = 0x00000400 ,
            FILE_ATTRIBUTE_COMPRESSED = 0x00000800 ,
            FILE_ATTRIBUTE_OFFLINE = 0x00001000 ,
            FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000 ,
            FILE_ATTRIBUTE_ENCRYPTED = 0x00004000
        }

        [DllImport("shell32.dll ", EntryPoint = "SHGetFileInfo")]
        public static extern int GetFileInfo(string pszPath, int dwFileAttributes, ref FileInfoStruct psfi, int cbFileInfo, int uFlags); 
                
        public IconManager()
        {
            Init();            
        }

        public ImageList LargeIconImageList
        {
            get
            {
                return largeIconImageList;
            }
        }

        public ImageList SmallIconImageList
        {
            get
            {
                return smallIconImageList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        public void Init()
        {
            // 初始化图片链表
            smallIconImageList.ImageSize = new Size(16, 16);
            smallIconImageList.ColorDepth = ColorDepth.Depth32Bit;

            largeIconImageList.ImageSize = new Size(24, 24);
            largeIconImageList.ColorDepth = ColorDepth.Depth32Bit;

            largeIconImageList.Images.Add(Properties.Resources.folderIcon);
            largeIconImageList.Images.Add(Properties.Resources.fileIcon);
            smallIconImageList.Images.Add(Properties.Resources.folderIcon);
            smallIconImageList.Images.Add(Properties.Resources.fileIcon);
        }

        public static Icon GetLargeIcon(string pFilePath)
        {
            Icon icon = null;

            FileInfoStruct info  = new FileInfoStruct();
            GetFileInfo(pFilePath, 0, ref info, Marshal.SizeOf(info), (int)(FileInfoFlags.SHGFI_ICON | FileInfoFlags.SHGFI_LARGEICON));

            try
            {
                icon = Icon.FromHandle(info.hIcon);
            }
            catch
            {
                ;
            }

            return icon;            
        }

        public static Icon GetSmallIcon(string pFilePath)
        {
            Icon icon = null;

            FileInfoStruct info = new FileInfoStruct();
            GetFileInfo(pFilePath, 0, ref info, Marshal.SizeOf(info), (int)(FileInfoFlags.SHGFI_ICON | FileInfoFlags.SHGFI_SMALLICON));
            try
            {
                icon = Icon.FromHandle(info.hIcon);
            }
            catch
            {
                ;
            }

            return icon;
        }

        public int AddFolderIcon(string folderName, IconType iconType)
        {
            int imageIndex = -1;
            DirectoryInfo di = new DirectoryInfo(folderName);

            if (di.Exists)
            {
                switch (iconType)
                {
                    case IconType.Large:
                        {
                            if (largeIconTable[di.Extension] != null)
                            {
                                imageIndex = int.Parse(largeIconTable[di.Extension].ToString());
                            }
                            else
                            {
                                Icon icon = GetLargeIcon(folderName);

                                if (icon != null)
                                {
                                    largeIconImageList.Images.Add(icon.ToBitmap());
                                    imageIndex = largeIconImageList.Images.Count - 1;
                                    largeIconTable[di.Extension] = imageIndex.ToString();
                                }
                                else
                                {
                                    imageIndex = 0;
                                }
                            }

                            break;
                        }
                    case IconType.Small:
                        {
                            if (smallIconTable[di.Extension] != null)
                            {
                                imageIndex = int.Parse(smallIconTable[di.Extension].ToString());
                            }
                            else
                            {
                                Icon icon = GetSmallIcon(folderName);

                                if (icon != null)
                                {
                                    smallIconImageList.Images.Add(icon.ToBitmap());
                                    imageIndex = smallIconImageList.Images.Count - 1;
                                    smallIconTable[di.Extension] = imageIndex.ToString();
                                }
                                else
                                {
                                    imageIndex = 0;
                                }
                            }

                            break;
                        }
                }
            }

            return imageIndex;
        }

        public int AddFileIcon(string fileName, IconType iconType)
        {
            int imageIndex = -1;
            FileInfo fi = new FileInfo(fileName);

            if (fi.Exists)
            {
                switch(iconType)
                {
                    case IconType.Large:
                        {
                            if (largeIconTable[fi.Extension] != null)
                            {
                                imageIndex = int.Parse(largeIconTable[fi.Extension].ToString());
                            }
                            else
                            {
                                Icon icon = GetLargeIcon(fileName);

                                if (icon != null)
                                {
                                    largeIconImageList.Images.Add(icon);
                                    imageIndex = largeIconImageList.Images.Count - 1;
                                    largeIconTable[fi.Extension] = imageIndex.ToString();
                                }
                                else
                                {
                                    imageIndex = 1;
                                }
                            }

                            break;
                        }
                    case IconType.Small:
                        {
                            if (smallIconTable[fi.Extension] != null)
                            {
                                imageIndex = int.Parse(smallIconTable[fi.Extension].ToString());
                            }
                            else
                            {
                                Icon icon = GetSmallIcon(fileName);

                                if (icon != null)
                                {
                                    smallIconImageList.Images.Add(icon);
                                    imageIndex = smallIconImageList.Images.Count - 1;
                                    smallIconTable[fi.Extension] = imageIndex.ToString();
                                }
                                else
                                {
                                    imageIndex = 1;
                                }
                            }

                            break;
                        }
                }
            }

            return imageIndex;                            
        }
        
    }
}
