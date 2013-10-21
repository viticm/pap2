
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
using System.Windows.Forms;

namespace Fireball.Win32
{

	#region params

	public class NativeMessageArgs : EventArgs
	{
		public Message Message;
		public bool Cancel;
	}

	public delegate void NativeMessageHandler(object s, NativeMessageArgs e);

	#endregion

	public class NativeSubclasser : NativeWindow
	{
		public event NativeMessageHandler Message = null;

		protected virtual void OnMessage(NativeMessageArgs e)
		{
			if (Message != null)
				Message(this, e);
		}

		public NativeSubclasser()
		{
		}

		public NativeSubclasser(Control Target)
		{
			this.AssignHandle(Target.Handle);
			Target.HandleCreated += new EventHandler(this.Handle_Created);
			Target.HandleDestroyed += new EventHandler(this.Handle_Destroyed);
		}

		private void Handle_Created(object o, EventArgs e)
		{
			this.AssignHandle(((Control) o).Handle);
		}

		private void Handle_Destroyed(object o, EventArgs e)
		{
			this.ReleaseHandle();
		}

		public NativeSubclasser(IntPtr hWnd)
		{
			this.AssignHandle(hWnd);
		}

		public void Detatch()
		{
			//	this.ReleaseHandle ();
		}

		protected override void WndProc(ref Message m)
		{
			try
			{
				NativeMessageArgs e = new NativeMessageArgs();
				e.Message = m;
				e.Cancel = false;

				OnMessage(e);

				if (!e.Cancel)
					base.WndProc(ref m);
			}
			catch (Exception x)
			{
				Console.WriteLine(x.Message);
			}
		}
	}
}