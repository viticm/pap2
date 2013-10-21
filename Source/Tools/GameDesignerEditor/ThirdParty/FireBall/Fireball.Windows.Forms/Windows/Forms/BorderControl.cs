//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE
using System;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
	public class ControlBorderPainter : NativeWindow
	{
		public ControlBorderPainter(IntPtr Handle)
		{
			this.AssignHandle(Handle);
		}

		protected override void WndProc(ref Message m)
		{
		}
	}
}