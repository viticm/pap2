using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Security.Permissions;

namespace Fireball.Win32
{
	public class Win32Window
    {
        #region Api declarations

        private const int SRCCOPY = 0x00CC0020;

        private const int GWL_EXSTYLE = -20;
        private const int WS_EX_TOOLWINDOW = 0x00000080;
        private const int WS_EX_APPWINDOW = 0x00040000;

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern bool SetForegroundWindow(IntPtr window);

        [DllImport("user32.dll")]
        private static extern int SetFocus(IntPtr window);

        [DllImport("user32.dll")]
        static extern bool BringWindowToTop(IntPtr window);

        [DllImport("user32.dll")]
        static extern IntPtr FindWindowEx(IntPtr parent, IntPtr childAfter, string className, string windowName);

        [DllImport("user32.dll", EntryPoint = "FindWindow")]
        static extern IntPtr FindWindowWin32(string className, string windowName);

        [DllImport("user32.dll")]
        static extern int SendMessage(IntPtr window, int message, int wparam, int lparam);

        [DllImport("user32.dll")]
        static extern int PostMessage(IntPtr window, int message, int wparam, int lparam);

        [DllImport("user32.dll")]
        static extern IntPtr GetParent(IntPtr window);

        [DllImport("user32.dll")]
        static extern IntPtr GetDesktopWindow();

        [DllImport("user32.dll")]
        static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll")]
        static extern IntPtr GetLastActivePopup(IntPtr window);

        [DllImport("user32.dll")]
        static extern int GetWindowText(
            IntPtr window,
            [In][Out] StringBuilder text,
            int copyCount);

        [DllImport("user32.dll")]
        static extern bool SetWindowText(
            IntPtr window,
            [MarshalAs(UnmanagedType.LPTStr)]
			string text);

        [DllImport("user32.dll")]
        static extern int GetWindowTextLength(IntPtr window);

        [DllImport("user32.dll")]
        static extern int SetWindowLong(
            IntPtr window,
            int index,
            int value);

        [DllImport("user32.dll")]
        static extern int GetWindowLong(
            IntPtr window,
            int index);

        // BOOL CALLBACK EnumWindowsProc(
        //				  HWND hwnd,      // handle to parent window
        //				  LPARAM lParam   // application-defined value
        //		
        delegate bool EnumWindowsProc(
            IntPtr window, int i);

        // BOOL EnumWindows(
        //	WNDENUMPROC lpEnumFunc,  // callback function
        //	LPARAM lParam            // application-defined value
        //	);
        [DllImport("user32.dll")]
        static extern bool EnumChildWindows(
            IntPtr window, EnumWindowsProc callback, int i);

        [DllImport("user32.dll")]
        static extern bool EnumThreadWindows(
            int threadId, EnumWindowsProc callback, int i);

        [DllImport("user32.dll")]
        static extern bool EnumWindows(EnumWindowsProc callback, int i);

        [DllImport("user32.dll")]
        static extern int GetWindowThreadProcessId(IntPtr window, ref int processId);

        [DllImport("user32.dll")]
        static extern int GetWindowThreadProcessId(IntPtr window, IntPtr ptr);

        [DllImport("user32.dll")]
        static extern bool GetWindowPlacement(IntPtr window, ref WindowPlacement position);

        [DllImport("user32.dll")]
        static extern bool SetWindowPlacement(IntPtr window, ref WindowPlacement position);

        [DllImport("user32.dll")]
        static extern bool IsChild(IntPtr parent, IntPtr window);

        [DllImport("user32.dll")]
        static extern bool IsIconic(IntPtr window);

        [DllImport("user32.dll")]
        static extern bool IsZoomed(IntPtr window);

        [DllImport("user32.dll")]
        private static extern IntPtr GetWindowDC(IntPtr hwnd);

        [DllImport("user32.dll")]
        private static extern int ReleaseDC(IntPtr hwnd, IntPtr dc);

        [DllImport("user32.dll")]
        private static extern bool GetWindowRect(IntPtr hwnd, ref RECTAPI rectangle);

        [DllImport("user32.dll")]
        private static extern bool GetClientRect(IntPtr hwnd, ref RECTAPI rectangle);

        [DllImport("gdi32.dll")]
        private static extern UInt64 BitBlt
               (IntPtr hDestDC, int x, int y, int nWidth, int nHeight,
                IntPtr hSrcDC, int xSrc, int ySrc, System.Int32 dwRop);

        [DllImport("user32.dll")]
        private static extern bool GetWindowInfo(IntPtr hwnd, ref WindowInfo info);

        #endregion

        #region Static members

        private static ArrayList _topLevelWindows = null;

        private static bool EnumerateTopLevelProc(IntPtr window, int i)
        {
            _topLevelWindows.Add(new Win32Window(window));
            return (true);
        }
        private static bool EnumerateThreadProc(IntPtr window, int i)
        {
            _topLevelWindows.Add(new Win32Window(window));
            return (true);
        }

        public static ArrayList TopLevelWindows
        {
            get
            {
                _topLevelWindows = new ArrayList();
                EnumWindows(new EnumWindowsProc(EnumerateTopLevelProc), 0);
                ArrayList top = _topLevelWindows;
                _topLevelWindows = null;
                return top;
            }
        }
        public static Win32Window DesktopWindow
        {
            get
            {
                return new Win32Window(GetDesktopWindow());
            }
        }
        public static Win32Window ForegroundWindow
        {
            get
            {
                return new Win32Window(GetForegroundWindow());
            }
        }

        public static ArrayList GetThreadWindows(int threadId)
        {
            _topLevelWindows = new ArrayList();
            EnumThreadWindows(threadId, new EnumWindowsProc(EnumerateThreadProc), 0);
            ArrayList windows = _topLevelWindows;
            _topLevelWindows = null;
            return windows;
        }
        /// <summary>
        /// Find a window by name or class
        /// </summary>
        /// <param name="className">Name of the class, or null</param>
        /// <param name="windowName">Name of the window, or null</param>
        /// <returns></returns
        public static Win32Window FindWindow(string className, string windowName)
        {
            return new Win32Window(FindWindowWin32(className, windowName));
        }
        private static Win32Window FindWindowHwnd(string sWild)
        {
            string wname = sWild.ToUpper();
            Win32Window wnd = Win32Window.FindWindow("", wname);
            return wnd;
        }
        public static bool IsChild(Win32Window parent, Win32Window window)
        {
            return IsChild(parent._window, window._window);
        }
        public static Image DesktopAsBitmap
        {
            get
            {
                Image myImage = new Bitmap(Screen.PrimaryScreen.Bounds.Width,
                    Screen.PrimaryScreen.Bounds.Height);
                Graphics gr1 = Graphics.FromImage(myImage);
                IntPtr dc1 = gr1.GetHdc();
                IntPtr desktopWindow = GetDesktopWindow();
                IntPtr dc2 = GetWindowDC(desktopWindow);
                BitBlt(dc1, 0, 0, Screen.PrimaryScreen.Bounds.Width,
                    Screen.PrimaryScreen.Bounds.Height, dc2, 0, 0, SRCCOPY);
                ReleaseDC(desktopWindow, dc2);
                gr1.ReleaseHdc(dc1);
                gr1.Dispose();
                return myImage;
            }
        }

        #endregion

        #region Istance members

        private IntPtr _window;
		private ArrayList _windowList = null;

        private bool EnumerateChildProc(IntPtr window, int i)
        {
            _windowList.Add(new Win32Window(window));
            return (true);
        }

        /// <summary>
        /// Create a Win32Window 
        /// </summary>
        /// <param name="window">handle of a window</param>
        public Win32Window(IntPtr window)
        {
            this._window = window;
        }

		public IntPtr Window
		{
			get
			{
				return _window;
			}
		}
		public bool IsNull
		{
			get
			{
				return _window == IntPtr.Zero;
			}
		}
		public ArrayList Children
		{
			get
			{
				_windowList = new ArrayList();
				EnumChildWindows(_window, new EnumWindowsProc(EnumerateChildProc), 0);
				ArrayList children = _windowList;
				_windowList = null;
				return children;
			}
        }
        public Win32Window Parent
        {
            get
            {
                return new Win32Window(GetParent(_window));
            }
        }
        /// <summary>
        /// Get the last (topmost) active popup
        /// </summary>
        public Win32Window LastActivePopup
        {
            get
            {
                IntPtr popup = GetLastActivePopup(_window);
                if (popup == _window)
                    return new Win32Window(IntPtr.Zero);
                else
                    return new Win32Window(popup);
            }
        }
        public string Text
        {
            get
            {
                int length = GetWindowTextLength(_window);
                StringBuilder sb = new StringBuilder(length + 1);
                GetWindowText(_window, sb, sb.Capacity);
                return sb.ToString();
            }
            set
            {
                SetWindowText(_window, value);
            }
        }
        public int ThreadId
        {
            get
            {
                return GetWindowThreadProcessId(_window, IntPtr.Zero);
            }
        }
        public int ProcessId
        {
            get
            {
                int processId = 0;
                GetWindowThreadProcessId(_window, ref processId);
                return processId;
            }
        }
        public WindowPlacement WindowPlacement
        {
            get
            {
                WindowPlacement placement = new WindowPlacement();
                GetWindowPlacement(_window, ref placement);
                return placement;
            }
            set
            {
                SetWindowPlacement(_window, ref value);
            }
        }
        public bool Minimized
        {
            get
            {
                return IsIconic(_window);
            }
        }
        public bool Maximized
        {
            get
            {
                return IsZoomed(_window);
            }
        }

        public void BringWindowToTop()
        {
            BringWindowToTop(_window);
        }
        /// <summary>
        /// Find a child of this window
        /// </summary>
        /// <param name="className">Name of the class, or null</param>
        /// <param name="windowName">Name of the window, or null</param>
        /// <returns></returns>
        public Win32Window FindChild(string className, string windowName)
        {
            return new Win32Window(
                FindWindowEx(_window, IntPtr.Zero, className, windowName));
        }
        public int SendMessage(int message, int wparam, int lparam)
        {
            return SendMessage(_window, message, wparam, lparam);
        }
        public int PostMessage(int message, int wparam, int lparam)
        {
            return PostMessage(_window, message, wparam, lparam);
        }
        public int GetWindowLong(int index)
        {
            return GetWindowLong(_window, index);
        }
        public int SetWindowLong(int index, int value)
        {
            return SetWindowLong(_window, index, value);
        }
        public void MakeToolWindow()
        {
            int windowStyle = GetWindowLong(GWL_EXSTYLE);
            SetWindowLong(GWL_EXSTYLE, windowStyle | WS_EX_TOOLWINDOW);
        }
        public /*unsafe*/ Image WindowAsBitmap
        {
            get
            {
                if (IsNull)
                    return null;

                this.BringWindowToTop();
                System.Threading.Thread.Sleep(500);

                RECTAPI rect = new RECTAPI();
                if (!GetWindowRect(_window, ref rect))
                    return null;

                WindowInfo windowInfo = new WindowInfo();
                windowInfo.Size = Marshal.SizeOf(typeof(WindowInfo));
                if (!GetWindowInfo(_window, ref windowInfo))
                    return null;

                Image myImage = new Bitmap(rect.Width, rect.Height);
                Graphics gr1 = Graphics.FromImage(myImage);
                IntPtr dc1 = gr1.GetHdc();
                IntPtr dc2 = GetWindowDC(_window);
                BitBlt(dc1, 0, 0, rect.Width, rect.Height, dc2, 0, 0, SRCCOPY);
                ReleaseDC(_window, dc2);
                gr1.ReleaseHdc(dc1);
                gr1.Dispose();
                return myImage;

            }
        }

        public bool SetAsForegroundWindow()
        {
            return SetForegroundWindow(_window);
        }

        public /*unsafe*/ Image WindowClientAsBitmap
        {
            get
            {
                if (IsNull)
                    return null;

                this.BringWindowToTop();
                System.Threading.Thread.Sleep(500);

                RECTAPI rect = new RECTAPI();
                if (!GetClientRect(_window, ref rect))
                    return null;

                WindowInfo windowInfo = new WindowInfo();
                windowInfo.Size = Marshal.SizeOf(typeof(WindowInfo));
                if (!GetWindowInfo(_window, ref windowInfo))
                    return null;

                int xOffset = windowInfo.ClientRect.X - windowInfo.WindowRect.X;
                int yOffset = windowInfo.ClientRect.Y - windowInfo.WindowRect.Y;

                Image myImage = new Bitmap(rect.Width, rect.Height);
                Graphics gr1 = Graphics.FromImage(myImage);
                IntPtr dc1 = gr1.GetHdc();
                IntPtr dc2 = GetWindowDC(_window);
                BitBlt(dc1, 0, 0, rect.Width, rect.Height, dc2, xOffset, yOffset, SRCCOPY);
                gr1.ReleaseHdc(dc1);
                return myImage;
            }
        }
        public void Focus()
        {
            /*UIPermission _permission = new UIPermission(UIPermissionWindow.AllWindows);
            _permission.Demand();*/
            //SetFocus(_window);
            SetForegroundWindow(_window);
        }

        #endregion

	}
}


