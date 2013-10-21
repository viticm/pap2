using System;

namespace Fireball.Windows.Forms.IntelliMouse
{
	public class ScrollEventArgs : EventArgs
	{
		public int DeltaX = 0;
		public int DeltaY = 0;
	}

	public delegate void ScrollEventHandler(object sender, ScrollEventArgs e);
}