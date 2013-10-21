
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
using System.Runtime.InteropServices;

namespace Fireball.Win32
{
    public enum AlphaFlags : byte
    {
        AC_SRC_ALPHA = 1,
        AC_SRC_OVER = 0
    }

    public enum Cursors : uint
    {
        IDC_APPSTARTING = 0x7f8a,
        IDC_ARROW = 0x7f00,
        IDC_CROSS = 0x7f03,
        IDC_HAND = 0x7f89,
        IDC_HELP = 0x7f8b,
        IDC_IBEAM = 0x7f01,
        IDC_ICON = 0x7f81,
        IDC_NO = 0x7f88,
        IDC_SIZE = 0x7f80,
        IDC_SIZEALL = 0x7f86,
        IDC_SIZENESW = 0x7f83,
        IDC_SIZENS = 0x7f85,
        IDC_SIZENWSE = 0x7f82,
        IDC_SIZEWE = 0x7f84,
        IDC_UPARROW = 0x7f04,
        IDC_WAIT = 0x7f02
    }

    public enum HitTest
    {
        HTBORDER = 0x12,
        HTBOTTOM = 15,
        HTBOTTOMLEFT = 0x10,
        HTBOTTOMRIGHT = 0x11,
        HTCAPTION = 2,
        HTCLIENT = 1,
        HTCLOSE = 20,
        HTERROR = -2,
        HTGROWBOX = 4,
        HTHELP = 0x15,
        HTHSCROLL = 6,
        HTLEFT = 10,
        HTMAXBUTTON = 9,
        HTMENU = 5,
        HTMINBUTTON = 8,
        HTNOWHERE = 0,
        HTOBJECT = 0x13,
        HTREDUCE = 8,
        HTRIGHT = 11,
        HTSIZE = 4,
        HTSIZEFIRST = 10,
        HTSIZELAST = 0x11,
        HTSYSMENU = 3,
        HTTOP = 12,
        HTTOPLEFT = 13,
        HTTOPRIGHT = 14,
        HTTRANSPARENT = -1,
        HTVSCROLL = 7,
        HTZOOM = 9
    }

    #region Messages
    public enum WindowMessage
    {
        WM_ACTIVATE = 6,
        WM_ACTIVATEAPP = 0x1c,
        WM_AFXFIRST = 0x360,
        WM_AFXLAST = 0x37f,
        WM_APP = 0x8000,
        WM_ASKCBFORMATNAME = 780,
        WM_CANCELJOURNAL = 0x4b,
        WM_CANCELMODE = 0x1f,
        WM_CAPTURECHANGED = 0x215,
        WM_CHANGECBCHAIN = 0x30d,
        WM_CHAR = 0x102,
        WM_CHARTOITEM = 0x2f,
        WM_CHILDACTIVATE = 0x22,
        WM_CLEAR = 0x303,
        WM_CLOSE = 0x10,
        WM_COMMAND = 0x111,
        WM_COMMNOTIFY = 0x44,
        WM_COMPACTING = 0x41,
        WM_COMPAREITEM = 0x39,
        WM_CONTEXTMENU = 0x7b,
        WM_COPY = 0x301,
        WM_COPYDATA = 0x4a,
        WM_CREATE = 1,
        WM_CTLCOLORBTN = 0x135,
        WM_CTLCOLORDLG = 310,
        WM_CTLCOLOREDIT = 0x133,
        WM_CTLCOLORLISTBOX = 0x134,
        WM_CTLCOLORMSGBOX = 0x132,
        WM_CTLCOLORSCROLLBAR = 0x137,
        WM_CTLCOLORSTATIC = 0x138,
        WM_CUT = 0x300,
        WM_DEADCHAR = 0x103,
        WM_DELETEITEM = 0x2d,
        WM_DESTROY = 2,
        WM_DESTROYCLIPBOARD = 0x307,
        WM_DEVICECHANGE = 0x219,
        WM_DEVMODECHANGE = 0x1b,
        WM_DISPLAYCHANGE = 0x7e,
        WM_DRAWCLIPBOARD = 0x308,
        WM_DRAWITEM = 0x2b,
        WM_DROPFILES = 0x233,
        WM_ENABLE = 10,
        WM_ENDSESSION = 0x16,
        WM_ENTERIDLE = 0x121,
        WM_ENTERMENULOOP = 0x211,
        WM_ENTERSIZEMOVE = 0x231,
        WM_ERASEBKGND = 20,
        WM_EXITMENULOOP = 530,
        WM_EXITSIZEMOVE = 0x232,
        WM_FONTCHANGE = 0x1d,
        WM_GETDLGCODE = 0x87,
        WM_GETFONT = 0x31,
        WM_GETHOTKEY = 0x33,
        WM_GETICON = 0x7f,
        WM_GETMINMAXINFO = 0x24,
        WM_GETOBJECT = 0x3d,
        WM_GETTEXT = 13,
        WM_GETTEXTLENGTH = 14,
        WM_HANDHELDFIRST = 0x358,
        WM_HANDHELDLAST = 0x35f,
        WM_HELP = 0x53,
        WM_HOTKEY = 0x312,
        WM_HSCROLL = 0x114,
        WM_HSCROLLCLIPBOARD = 0x30e,
        WM_ICONERASEBKGND = 0x27,
        WM_IME_CHAR = 0x286,
        WM_IME_COMPOSITION = 0x10f,
        WM_IME_COMPOSITIONFULL = 0x284,
        WM_IME_CONTROL = 0x283,
        WM_IME_ENDCOMPOSITION = 270,
        WM_IME_KEYDOWN = 0x290,
        WM_IME_KEYLAST = 0x10f,
        WM_IME_KEYUP = 0x291,
        WM_IME_NOTIFY = 0x282,
        WM_IME_REQUEST = 0x288,
        WM_IME_SELECT = 0x285,
        WM_IME_SETCONTEXT = 0x281,
        WM_IME_STARTCOMPOSITION = 0x10d,
        WM_INITDIALOG = 0x110,
        WM_INITMENU = 0x116,
        WM_INITMENUPOPUP = 0x117,
        WM_INPUTLANGCHANGE = 0x51,
        WM_INPUTLANGCHANGEREQUEST = 80,
        WM_KEYDOWN = 0x100,
        WM_KEYLAST = 0x108,
        WM_KEYUP = 0x101,
        WM_KILLFOCUS = 8,
        WM_LBUTTONDBLCLK = 0x203,
        WM_LBUTTONDOWN = 0x201,
        WM_LBUTTONUP = 0x202,
        WM_MBUTTONDBLCLK = 0x209,
        WM_MBUTTONDOWN = 0x207,
        WM_MBUTTONUP = 520,
        WM_MDIACTIVATE = 0x222,
        WM_MDICASCADE = 0x227,
        WM_MDICREATE = 0x220,
        WM_MDIDESTROY = 0x221,
        WM_MDIGETACTIVE = 0x229,
        WM_MDIICONARRANGE = 0x228,
        WM_MDIMAXIMIZE = 0x225,
        WM_MDINEXT = 0x224,
        WM_MDIREFRESHMENU = 0x234,
        WM_MDIRESTORE = 0x223,
        WM_MDISETMENU = 560,
        WM_MDITILE = 550,
        WM_MEASUREITEM = 0x2c,
        WM_MENUCHAR = 0x120,
        WM_MENUCOMMAND = 0x126,
        WM_MENUDRAG = 0x123,
        WM_MENUGETOBJECT = 0x124,
        WM_MENURBUTTONUP = 290,
        WM_MENUSELECT = 0x11f,
        WM_MOUSEACTIVATE = 0x21,
        WM_MOUSEHOVER = 0x2a1,
        WM_MOUSELEAVE = 0x2a3,
        WM_MOUSEMOVE = 0x200,
        WM_MOUSEWHEEL = 0x20a,
        WM_MOVE = 3,
        WM_MOVING = 0x216,
        WM_NCACTIVATE = 0x86,
        WM_NCCALCSIZE = 0x83,
        WM_NCCREATE = 0x81,
        WM_NCDESTROY = 130,
        WM_NCHITTEST = 0x84,
        WM_NCLBUTTONDBLCLK = 0xa3,
        WM_NCLBUTTONDOWN = 0xa1,
        WM_NCLBUTTONUP = 0xa2,
        WM_NCMBUTTONDBLCLK = 0xa9,
        WM_NCMBUTTONDOWN = 0xa7,
        WM_NCMBUTTONUP = 0xa8,
        WM_NCMOUSEMOVE = 160,
        WM_NCPAINT = 0x85,
        WM_NCRBUTTONDBLCLK = 0xa6,
        WM_NCRBUTTONDOWN = 0xa4,
        WM_NCRBUTTONUP = 0xa5,
        WM_NCXBUTTONDOWN = 0xab,
        WM_NCXBUTTONUP = 0xac,
        WM_NEXTDLGCTL = 40,
        WM_NEXTMENU = 0x213,
        WM_NOTIFY = 0x4e,
        WM_NOTIFYFORMAT = 0x55,
        WM_NULL = 0,
        WM_PAINT = 15,
        WM_PAINTCLIPBOARD = 0x309,
        WM_PAINTICON = 0x26,
        WM_PALETTECHANGED = 0x311,
        WM_PALETTEISCHANGING = 0x310,
        WM_PARENTNOTIFY = 0x210,
        WM_PASTE = 770,
        WM_PENWINFIRST = 0x380,
        WM_PENWINLAST = 0x38f,
        WM_POWER = 0x48,
        WM_PRINT = 0x317,
        WM_PRINTCLIENT = 0x318,
        WM_QUERYDRAGICON = 0x37,
        WM_QUERYENDSESSION = 0x11,
        WM_QUERYNEWPALETTE = 0x30f,
        WM_QUERYOPEN = 0x13,
        WM_QUEUESYNC = 0x23,
        WM_QUIT = 0x12,
        WM_RBUTTONDBLCLK = 0x206,
        WM_RBUTTONDOWN = 0x204,
        WM_RBUTTONUP = 0x205,
        WM_RENDERALLFORMATS = 0x306,
        WM_RENDERFORMAT = 0x305,
        WM_SETCURSOR = 0x20,
        WM_SETFOCUS = 7,
        WM_SETFONT = 0x30,
        WM_SETHOTKEY = 50,
        WM_SETICON = 0x80,
        WM_SETREDRAW = 11,
        WM_SETTEXT = 12,
        WM_SETTINGCHANGE = 0x1a,
        WM_SHOWWINDOW = 0x18,
        WM_SIZE = 5,
        WM_SIZECLIPBOARD = 0x30b,
        WM_SIZING = 0x214,
        WM_SPOOLERSTATUS = 0x2a,
        WM_STYLECHANGED = 0x7d,
        WM_STYLECHANGING = 0x7c,
        WM_SYNCPAINT = 0x88,
        WM_SYSCHAR = 0x106,
        WM_SYSCOLORCHANGE = 0x15,
        WM_SYSCOMMAND = 0x112,
        WM_SYSDEADCHAR = 0x107,
        WM_SYSKEYDOWN = 260,
        WM_SYSKEYUP = 0x105,
        WM_TCARD = 0x52,
        WM_TIMECHANGE = 30,
        WM_TIMER = 0x113,
        WM_UNDO = 0x304,
        WM_UNINITMENUPOPUP = 0x125,
        WM_USER = 0x400,
        WM_USERCHANGED = 0x54,
        WM_VKEYTOITEM = 0x2e,
        WM_VSCROLL = 0x115,
        WM_VSCROLLCLIPBOARD = 0x30a,
        WM_WINDOWPOSCHANGED = 0x47,
        WM_WINDOWPOSCHANGING = 70,
        WM_WININICHANGE = 0x1a,
        WM_XBUTTONDBLCLK = 0x20d,
        WM_XBUTTONDOWN = 0x20b,
        WM_XBUTTONUP = 0x20c
    }

    public enum GDIRop
    {
        SrcCopy = 13369376,
        Blackness = 0, //to be implemented
        Whiteness = 0
    }

    public enum PeekMessageFlags
    {
        PM_NOREMOVE,
        PM_REMOVE,
        PM_NOYIELD
    }

    public enum SetWindowPosFlags : uint
    {
        SWP_ASYNCWINDOWPOS = 0x4000,
        SWP_DEFERERASE = 0x2000,
        SWP_DRAWFRAME = 0x20,
        SWP_FRAMECHANGED = 0x20,
        SWP_HIDEWINDOW = 0x80,
        SWP_NOACTIVATE = 0x10,
        SWP_NOCOPYBITS = 0x100,
        SWP_NOMOVE = 2,
        SWP_NOOWNERZORDER = 0x200,
        SWP_NOREDRAW = 8,
        SWP_NOREPOSITION = 0x200,
        SWP_NOSENDCHANGING = 0x400,
        SWP_NOSIZE = 1,
        SWP_NOZORDER = 4,
        SWP_SHOWWINDOW = 0x40
    }

    public enum ShowWindowStyles : short
    {
        SW_FORCEMINIMIZE = 11,
        SW_HIDE = 0,
        SW_MAX = 11,
        SW_MAXIMIZE = 3,
        SW_MINIMIZE = 6,
        SW_NORMAL = 1,
        SW_RESTORE = 9,
        SW_SHOW = 5,
        SW_SHOWDEFAULT = 10,
        SW_SHOWMAXIMIZED = 3,
        SW_SHOWMINIMIZED = 2,
        SW_SHOWMINNOACTIVE = 7,
        SW_SHOWNA = 8,
        SW_SHOWNOACTIVATE = 4,
        SW_SHOWNORMAL = 1
    }

    public enum SetWindowPosZ
    {
        HWND_BOTTOM = 1,
        HWND_NOTOPMOST = -2,
        HWND_TOP = 0,
        HWND_TOPMOST = -1
    }

    public enum UpdateLayeredWindowsFlags
    {
        ULW_ALPHA = 2,
        ULW_COLORKEY = 1,
        ULW_OPAQUE = 4
    }

    public enum VirtualKeys
    {
        VK_0 = 0x30,
        VK_1 = 0x31,
        VK_2 = 50,
        VK_3 = 0x33,
        VK_4 = 0x34,
        VK_5 = 0x35,
        VK_6 = 0x36,
        VK_7 = 0x37,
        VK_8 = 0x38,
        VK_9 = 0x39,
        VK_A = 0x41,
        VK_ADD = 0x6b,
        VK_APPS = 0x5d,
        VK_ATTN = 0xf6,
        VK_B = 0x42,
        VK_BACK = 8,
        VK_C = 0x43,
        VK_CANCEL = 3,
        VK_CAPITAL = 20,
        VK_CLEAR = 12,
        VK_CONTROL = 0x11,
        VK_CRSEL = 0xf7,
        VK_D = 0x44,
        VK_DECIMAL = 110,
        VK_DIVIDE = 0x6f,
        VK_DOWN = 40,
        VK_E = 0x45,
        VK_END = 0x23,
        VK_EREOF = 0xf9,
        VK_ESCAPE = 0x1b,
        VK_EXECUTE = 0x2b,
        VK_EXSEL = 0xf8,
        VK_F = 70,
        VK_G = 0x47,
        VK_H = 0x48,
        VK_HELP = 0x2f,
        VK_HOME = 0x24,
        VK_I = 0x49,
        VK_J = 0x4a,
        VK_K = 0x4b,
        VK_L = 0x4c,
        VK_LBUTTON = 1,
        VK_LCONTROL = 0xa2,
        VK_LEFT = 0x25,
        VK_LMENU = 0xa4,
        VK_LSHIFT = 160,
        VK_LWIN = 0x5b,
        VK_M = 0x4d,
        VK_MENU = 0x12,
        VK_MULTIPLY = 0x6a,
        VK_N = 0x4e,
        VK_NEXT = 0x22,
        VK_NONAME = 0xfc,
        VK_NUMPAD0 = 0x60,
        VK_NUMPAD1 = 0x61,
        VK_NUMPAD2 = 0x62,
        VK_NUMPAD3 = 0x63,
        VK_NUMPAD4 = 100,
        VK_NUMPAD5 = 0x65,
        VK_NUMPAD6 = 0x66,
        VK_NUMPAD7 = 0x67,
        VK_NUMPAD8 = 0x68,
        VK_NUMPAD9 = 0x69,
        VK_O = 0x4f,
        VK_OEM_CLEAR = 0xfe,
        VK_P = 80,
        VK_PA1 = 0xfd,
        VK_PLAY = 250,
        VK_PRIOR = 0x21,
        VK_Q = 0x51,
        VK_R = 0x52,
        VK_RCONTROL = 0xa3,
        VK_RETURN = 13,
        VK_RIGHT = 0x27,
        VK_RMENU = 0xa5,
        VK_RSHIFT = 0xa1,
        VK_RWIN = 0x5c,
        VK_S = 0x53,
        VK_SELECT = 0x29,
        VK_SEPARATOR = 0x6c,
        VK_SHIFT = 0x10,
        VK_SNAPSHOT = 0x2c,
        VK_SPACE = 0x20,
        VK_SUBTRACT = 0x6d,
        VK_T = 0x54,
        VK_TAB = 9,
        VK_U = 0x55,
        VK_UP = 0x26,
        VK_V = 0x56,
        VK_W = 0x57,
        VK_X = 0x58,
        VK_Y = 0x59,
        VK_Z = 90,
        VK_ZOOM = 0xfb
    }

    public enum WindowExStyles
    {
        WS_EX_ACCEPTFILES = 0x10,
        WS_EX_APPWINDOW = 0x40000,
        WS_EX_CLIENTEDGE = 0x200,
        WS_EX_CONTEXTHELP = 0x400,
        WS_EX_CONTROLPARENT = 0x10000,
        WS_EX_DLGMODALFRAME = 1,
        WS_EX_LAYERED = 0x80000,
        WS_EX_LEFT = 0,
        WS_EX_LEFTSCROLLBAR = 0x4000,
        WS_EX_LTRREADING = 0,
        WS_EX_MDICHILD = 0x40,
        WS_EX_NOPARENTNOTIFY = 4,
        WS_EX_OVERLAPPEDWINDOW = 0x300,
        WS_EX_PALETTEWINDOW = 0x188,
        WS_EX_RIGHT = 0x1000,
        WS_EX_RIGHTSCROLLBAR = 0,
        WS_EX_RTLREADING = 0x2000,
        WS_EX_STATICEDGE = 0x20000,
        WS_EX_TOOLWINDOW = 0x80,
        WS_EX_TOPMOST = 8,
        WS_EX_TRANSPARENT = 0x20,
        WS_EX_WINDOWEDGE = 0x100
    }

    public enum WindowStyles : uint
    {
        WS_BORDER = 0x800000,
        WS_CAPTION = 0xc00000,
        WS_CHILD = 0x40000000,
        WS_CHILDWINDOW = 0x40000000,
        WS_CLIPCHILDREN = 0x2000000,
        WS_CLIPSIBLINGS = 0x4000000,
        WS_DISABLED = 0x8000000,
        WS_DLGFRAME = 0x400000,
        WS_GROUP = 0x20000,
        WS_HSCROLL = 0x100000,
        WS_ICONIC = 0x20000000,
        WS_MAXIMIZE = 0x1000000,
        WS_MAXIMIZEBOX = 0x10000,
        WS_MINIMIZE = 0x20000000,
        WS_MINIMIZEBOX = 0x20000,
        WS_OVERLAPPED = 0,
        WS_OVERLAPPEDWINDOW = 0xcf0000,
        WS_POPUP = 0x80000000,
        WS_POPUPWINDOW = 0x80880000,
        WS_SIZEBOX = 0x40000,
        WS_SYSMENU = 0x80000,
        WS_TABSTOP = 0x10000,
        WS_THICKFRAME = 0x40000,
        WS_TILED = 0,
        WS_TILEDWINDOW = 0xcf0000,
        WS_VISIBLE = 0x10000000,
        WS_VSCROLL = 0x200000
    }

    #endregion

    public enum DrawTextFlags
    {
        DT_TOP = 0x00000000,
        DT_LEFT = 0x00000000,
        DT_CENTER = 0x00000001,
        DT_RIGHT = 0x00000002,
        DT_VCENTER = 0x00000004,
        DT_BOTTOM = 0x00000008,
        DT_WORDBREAK = 0x00000010,
        DT_SINGLELINE = 0x00000020,
        DT_EXPANDTABS = 0x00000040,
        DT_TABSTOP = 0x00000080,
        DT_NOCLIP = 0x00000100,
        DT_EXTERNALLEADING = 0x00000200,
        DT_CALCRECT = 0x00000400,
        DT_NOPREFIX = 0x00000800,
        DT_INTERNAL = 0x00001000,
        DT_EDITCONTROL = 0x00002000,
        DT_PATH_ELLIPSIS = 0x00004000,
        DT_END_ELLIPSIS = 0x00008000,
        DT_MODIFYSTRING = 0x00010000,
        DT_RTLREADING = 0x00020000,
        DT_WORD_ELLIPSIS = 0x00040000,
        DT_NOFULLWIDTHCHARBREAK = 0x00080000,
        DT_HIDEPREFIX = 0x00100000,
        DT_PREFIXONLY = 0x00200000,
    }

    public enum TextBoxNotifications
    {
        EN_SETFOCUS = 0x0100,
        EN_KILLFOCUS = 0x0200,
        EN_CHANGE = 0x0300,
        EN_UPDATE = 0x0400,
        EN_ERRSPACE = 0x0500,
        EN_MAXTEXT = 0x0501,
        EN_HSCROLL = 0x0601,
        EN_VSCROLL = 0x0602,
    }

    public enum TextBoxMessages
    {
        EM_GETSEL = 0x00B0,
        EM_LINEINDEX = 0x00BB,
        EM_LINEFROMCHAR = 0x00C9,
        EM_POSFROMCHAR = 0x00D6,
    }

    public enum TextBoxStyles
    {
        ES_LEFT = 0x0000,
        ES_CENTER = 0x0001,
        ES_RIGHT = 0x0002,
        ES_MULTILINE = 0x0004,
        ES_UPPERCASE = 0x0008,
        ES_LOWERCASE = 0x0010,
        ES_PASSWORD = 0x0020,
        ES_AUTOVSCROLL = 0x0040,
        ES_AUTOHSCROLL = 0x0080,
        ES_NOHIDESEL = 0x0100,
        ES_OEMCONVERT = 0x0400,
        ES_READONLY = 0x0800,
        ES_WANTRETURN = 0x1000,
    }

    [Flags()]
    public enum WMPrintFlags
    {
        PRF_CHECKVISIBLE = 0x00000001,
        PRF_NONCLIENT = 0x00000002,
        PRF_CLIENT = 0x00000004,
        PRF_ERASEBKGND = 0x00000008,
        PRF_CHILDREN = 0x00000010,
        PRF_OWNED = 0x0000020,
    }

    #region Shell Enumerations

    [Flags]
    public enum SHGNO : uint
    {
        SHGDN_NORMAL = 0x0000,                 // Default (display purpose)
        SHGDN_INFOLDER = 0x0001,               // Displayed under a folder (relative)
        SHGDN_FOREDITING = 0x1000,             // For in-place editing
        SHGDN_FORADDRESSBAR = 0x4000,          // UI friendly parsing name (remove ugly stuff)
        SHGDN_FORPARSING = 0x8000,             // Parsing name for ParseDisplayName()
    }

    [Flags]
    public enum SHCONTF : uint
    {
        SHCONTF_FOLDERS = 0x0020,              // Only want folders enumerated (SFGAO_FOLDER)
        SHCONTF_NONFOLDERS = 0x0040,           // Include non folders
        SHCONTF_INCLUDEHIDDEN = 0x0080,        // Show items normally hidden
        SHCONTF_INIT_ON_FIRST_NEXT = 0x0100,   // Allow EnumObject() to return before validating enum
        SHCONTF_NETPRINTERSRCH = 0x0200,       // Hint that client is looking for printers
        SHCONTF_SHAREABLE = 0x0400,            // Hint that client is looking sharable resources (remote shares)
        SHCONTF_STORAGE = 0x0800,              // Include all items with accessible storage and their ancestors
    }

    [Flags]
    public enum SFGAOF : uint
    {
        SFGAO_CANCOPY = 0x1,                   // Objects can be copied  (DROPEFFECT_COPY)
        SFGAO_CANMOVE = 0x2,                   // Objects can be moved   (DROPEFFECT_MOVE)
        SFGAO_CANLINK = 0x4,                   // Objects can be linked  (DROPEFFECT_LINK)
        SFGAO_STORAGE = 0x00000008,            // Supports BindToObject(IID_IStorage)
        SFGAO_CANRENAME = 0x00000010,          // Objects can be renamed
        SFGAO_CANDELETE = 0x00000020,          // Objects can be deleted
        SFGAO_HASPROPSHEET = 0x00000040,       // Objects have property sheets
        SFGAO_DROPTARGET = 0x00000100,         // Objects are drop target
        SFGAO_CAPABILITYMASK = 0x00000177,
        SFGAO_ENCRYPTED = 0x00002000,          // Object is encrypted (use alt color)
        SFGAO_ISSLOW = 0x00004000,             // 'Slow' object
        SFGAO_GHOSTED = 0x00008000,            // Ghosted icon
        SFGAO_LINK = 0x00010000,               // Shortcut (link)
        SFGAO_SHARE = 0x00020000,              // Shared
        SFGAO_READONLY = 0x00040000,           // Read-only
        SFGAO_HIDDEN = 0x00080000,             // Hidden object
        SFGAO_DISPLAYATTRMASK = 0x000FC000,
        SFGAO_FILESYSANCESTOR = 0x10000000,    // May contain children with SFGAO_FILESYSTEM
        SFGAO_FOLDER = 0x20000000,             // Support BindToObject(IID_IShellFolder)
        SFGAO_FILESYSTEM = 0x40000000,         // Is a win32 file system object (file/folder/root)
        SFGAO_HASSUBFOLDER = 0x80000000,       // May contain children with SFGAO_FOLDER
        SFGAO_CONTENTSMASK = 0x80000000,
        SFGAO_VALIDATE = 0x01000000,           // Invalidate cached information
        SFGAO_REMOVABLE = 0x02000000,          // Is this removeable media?
        SFGAO_COMPRESSED = 0x04000000,         // Object is compressed (use alt color)
        SFGAO_BROWSABLE = 0x08000000,          // Supports IShellFolder, but only implements CreateViewObject() (non-folder view)
        SFGAO_NONENUMERATED = 0x00100000,      // Is a non-enumerated object
        SFGAO_NEWCONTENT = 0x00200000,         // Should show bold in explorer tree
        SFGAO_CANMONIKER = 0x00400000,         // Defunct
        SFGAO_HASSTORAGE = 0x00400000,         // Defunct
        SFGAO_STREAM = 0x00400000,             // Supports BindToObject(IID_IStream)
        SFGAO_STORAGEANCESTOR = 0x00800000,    // May contain children with SFGAO_STORAGE or SFGAO_STREAM
        SFGAO_STORAGECAPMASK = 0x70C50008,     // For determining storage capabilities, ie for open/save semantics
    }

    [Flags]
    public enum STRRET : uint
    {
        STRRET_WSTR = 0,
        STRRET_OFFSET = 0x1,
        STRRET_CSTR = 0x2,
    }

    [Flags]
    public enum SHGFI
    {
        SHGFI_ICON = 0x000000100,
        SHGFI_DISPLAYNAME = 0x000000200,
        SHGFI_TYPENAME = 0x000000400,
        SHGFI_ATTRIBUTES = 0x000000800,
        SHGFI_ICONLOCATION = 0x000001000,
        SHGFI_EXETYPE = 0x000002000,
        SHGFI_SYSICONINDEX = 0x000004000,
        SHGFI_LINKOVERLAY = 0x000008000,
        SHGFI_SELECTED = 0x000010000,
        SHGFI_ATTR_SPECIFIED = 0x000020000,
        SHGFI_LARGEICON = 0x000000000,
        SHGFI_SMALLICON = 0x000000001,
        SHGFI_OPENICON = 0x000000002,
        SHGFI_SHELLICONSIZE = 0x000000004,
        SHGFI_PIDL = 0x000000008,
        SHGFI_USEFILEATTRIBUTES = 0x000000010,
        SHGFI_ADDOVERLAYS = 0x000000020,
        SHGFI_OVERLAYINDEX = 0x000000040
    }

    [Flags]
    public enum CSIDL : uint
    {
        CSIDL_DESKTOP = 0x0000,
        CSIDL_WINDOWS = 0x0024
    }

    #endregion
}
