#region Licences
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
#endregion Licences


using System;
using System.Threading;
using Threading = System.Threading;

namespace Fireball.Ssh.java.lang
{
	/// <summary>
	/// Summary description for Thread.
	/// </summary>
	public class Thread
	{
		Threading.Thread t;

		public Thread(Threading.Thread t)
		{
			this.t=t;
		}
		public Thread(ThreadStart ts):this(new Threading.Thread(ts))
		{
		}

		public Thread(Runnable r):this(new ThreadStart(r.run))
		{
		}

		public void setName(string name)
		{
			t.Name=name;
		}

		public void start()
		{
			t.Start();
		}

		public void yield()
		{
		}

		public void interrupt()
		{
			t.Interrupt();
		}

		public void notifyAll()
		{
			Monitor.PulseAll(this);
		}

		public static void Sleep(int t)
		{
			Threading.Thread.Sleep(t);
		}

		public static void sleep(int t)
		{
			Sleep(t);
		}

		public static Thread currentThread()
		{
			return new Thread( Threading.Thread.CurrentThread );
		}
	}
}
