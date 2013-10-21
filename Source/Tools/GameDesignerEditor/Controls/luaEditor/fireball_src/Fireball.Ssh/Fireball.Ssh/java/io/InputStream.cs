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
using System.IO;

namespace Fireball.Ssh.java.io
{
	/// <summary>
	/// Summary description for InputStream.
	/// </summary>
	public abstract class InputStream : Stream
	{
		public override void WriteByte(byte value)
		{
		}

		public override void Write(byte[] buffer, int offset, int count)
		{
		}

		public override bool CanRead
		{
			get
			{
				return true;
			}
		}
		public override bool CanWrite
		{
			get
			{
				return false;
			}
		}
		public override bool CanSeek
		{
			get
			{
				return false;
			}
		}
		public override void Flush()
		{
			
		}
		public override long Length
		{
			get
			{
				return 0;
			}
		}
		public override long Position
		{
			get
			{
				return 0;
			}
			set
			{
			}
		}
		public override void SetLength(long value)
		{			
		}
		public override long Seek(long offset, SeekOrigin origin)
		{
			return 0;
		}
	}
}
