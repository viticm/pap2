#region Copyright

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

#endregion

using System;

namespace Fireball.Drawing.GDI
{
	/// <summary>
	/// Summary description for GDIObject.
	/// </summary>
	public abstract class GDIObject : IDisposable
	{
		protected bool IsCreated = false;

		protected virtual void Destroy()
		{
			IsCreated = false;
			MemHandler.Remove(this);
		}

		protected virtual void Create()
		{
			IsCreated = true;
			MemHandler.Add(this);
		}

		#region Implementation of IDisposable

		public void Dispose()
		{
			this.Destroy();
		}

		#endregion
	}
}