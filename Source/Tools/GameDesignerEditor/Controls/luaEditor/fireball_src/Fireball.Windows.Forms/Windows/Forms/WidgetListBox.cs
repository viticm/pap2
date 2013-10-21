//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE

using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Fireball.Drawing;
using Fireball.Win32;

namespace Fireball.Windows.Forms
{
	[ToolboxItem(true)]
	public class WidgetListBox : ListBox
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
        private ControlBorderStyle borderStyle;

		private Color borderColor = Color.Black;
		private Container components = null;
		private bool RunOnce = true;

		public event EventHandler Load = null;


		public WidgetListBox() : base()
		{
			SetStyle(ControlStyles.EnableNotifyMessage, true);
            this.BorderStyle = ControlBorderStyle.FixedSingle;
		}

		[Browsable(false)]
		public Size WindowSize
		{
			get
			{
                RECTAPI s = new RECTAPI();
                NativeUser32Api.GetWindowRect(this.Handle, ref s);
				return new Size(s.Width, s.Height);
			}
		}

		[Category("Appearance - Borders"), Description("The border color")]
		[DefaultValue(typeof (Color), "Black")]
		public Color BorderColor
		{
			get { return borderColor; }

			set
			{
				borderColor = value;
				this.Refresh();
				this.Invalidate();
				UpdateStyles();
			}
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		private const int WS_EX_CLIENTEDGE = unchecked((int) 0x00000200);
		private const int WS_BORDER = unchecked((int) 0x00800000);

		protected override CreateParams CreateParams
		{
			get
			{
				CreateParams cp = base.CreateParams;

                if (BorderStyle == ControlBorderStyle.None)
					return cp;

				cp.ExStyle &= (~WS_EX_CLIENTEDGE);
				cp.Style &= (~WS_BORDER);

				return cp;
			}
		}

		[Browsable(true),
			EditorBrowsable(EditorBrowsableState.Always)]
		[Category("Appearance - Borders"), Description("The border style")]
        [DefaultValue(ControlBorderStyle.None)]
        public new ControlBorderStyle BorderStyle
		{
			get { return borderStyle; }
			set
			{
				if (borderStyle != value)
				{
					if (!Enum.IsDefined(typeof (BorderStyle), value))
					{
						throw new InvalidEnumArgumentException("value", (int) value, typeof (BorderStyle));
					}
					borderStyle = value;
					UpdateStyles();
					this.Refresh();
				}
			}
		}


		[Browsable(false), EditorBrowsable(EditorBrowsableState.Never), Obsolete("Do not use!", true)]
		public override Image BackgroundImage
		{
			get { return base.BackgroundImage; }
			set { base.BackgroundImage = value; }
		}


		[Browsable(false)]
		public int ClientWidth
		{
			get { return this.WindowSize.Width - (this.BorderWidth*2); }
		}

		[Browsable(false)]
		public int ClientHeight
		{
			get { return this.WindowSize.Height - (this.BorderWidth*2); }
		}

		[Browsable(false)]
		public int BorderWidth
		{
			get
			{
				switch (this.borderStyle)
				{
                    case ControlBorderStyle.None:
						{
							return 0;
						}
                    case ControlBorderStyle.Sunken:
						{
							return 2;
						}
                    case ControlBorderStyle.SunkenThin:
						{
							return 1;
						}
                    case ControlBorderStyle.Raised:
						{
							return 2;
						}

                    case ControlBorderStyle.Etched:
						{
							return 2;
						}
                    case ControlBorderStyle.Bump:
						{
							return 6;
						}
                    case ControlBorderStyle.FixedSingle:
						{
							return 1;
						}
                    case ControlBorderStyle.FixedDouble:
						{
							return 2;
						}
                    case ControlBorderStyle.RaisedThin:
						{
							return 1;
						}
                    case ControlBorderStyle.Dotted:
						{
							return 1;
						}
                    case ControlBorderStyle.Dashed:
						{
							return 1;
						}
				}


				return this.Height;
			}
		}

		protected virtual void OnLoad(EventArgs e)
		{
			if (Load != null)
				Load(this, e);
			this.Refresh();
		}

		protected override unsafe void WndProc(ref Message m)
		{
			if (m.Msg == (int) WindowMessage.WM_NCPAINT)
			{
				try
				{
					RenderBorder();
				}
				catch
				{
				}
				base.WndProc(ref m);
			}
			else if (m.Msg == (int) WindowMessage.WM_SHOWWINDOW)
			{
				if (RunOnce)
				{
					RunOnce = false;
					OnLoad(null);
					base.WndProc(ref m);
					UpdateStyles();
				}
				else
				{
					UpdateStyles();
					base.WndProc(ref m);
				}

			}
			else if (m.Msg == (int) WindowMessage.WM_NCCREATE)
			{
				base.WndProc(ref m);
			}
			else if (m.Msg == (int) WindowMessage.WM_NCCALCSIZE)
			{
				if (m.WParam == (IntPtr) 0)
				{
                    RECTAPI* pRC = (RECTAPI*)m.LParam;
					//pRC->left -=3;
					base.WndProc(ref m);
				}
				else if (m.WParam == (IntPtr) 1)
				{
					_NCCALCSIZE_PARAMS* pNCP = (_NCCALCSIZE_PARAMS*) m.LParam;

					base.WndProc(ref m);

					int t = pNCP->NewRect.Top + this.BorderWidth;
					int l = pNCP->NewRect.Left + this.BorderWidth;
					int b = pNCP->NewRect.Bottom - this.BorderWidth;
					int r = pNCP->NewRect.Right - this.BorderWidth;


					pNCP->NewRect.Top = t;
					pNCP->NewRect.Left = l;
					pNCP->NewRect.Right = r;
					pNCP->NewRect.Bottom = b;
				}
			}
			else
			{
				base.WndProc(ref m);
			}
		}

		private void RenderBorder()
		{
			IntPtr hdc = NativeUser32Api.GetWindowDC(this.Handle);
            RECTAPI s = new RECTAPI();
            NativeUser32Api.GetWindowRect(this.Handle, ref s);

			using (Graphics g = Graphics.FromHdc(hdc))
			{
                DrawingTools.DrawBorder((ControlBorderStyle)(int)this.BorderStyle, this.BorderColor, g, new Rectangle(0, 0, s.Width, s.Height));
			}
            NativeUser32Api.ReleaseDC(this.Handle, hdc);
		}


		protected override void OnEnter(EventArgs e)
		{
			base.OnEnter(e);
		}


	}
}