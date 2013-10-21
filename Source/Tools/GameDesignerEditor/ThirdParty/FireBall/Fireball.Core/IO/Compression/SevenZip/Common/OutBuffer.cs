#region Licenze

//    Copyright (C) 2004  Sebastian Faltoni <sebastian@dotnetfireball.net>
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
//
//    NB: Original Code Grabbed From sevenzip lzma sdk http://www.7-zip.org/
#endregion

// OutBuffer.cs

namespace Fireball.IO.Compression.SevenZip.Buffer
{
	public class OutBuffer
	{
		byte[] m_Buffer;
		uint m_Pos;
		uint m_BufferSize;
		System.IO.Stream m_Stream;
		ulong m_ProcessedSize;

		public OutBuffer(uint bufferSize)
		{
			m_Buffer = new byte[bufferSize];
			m_BufferSize = bufferSize;
		}

		public void SetStream(System.IO.Stream stream) { m_Stream = stream; }
		public void FlushStream() { m_Stream.Flush(); }
		public void CloseStream() { m_Stream.Close(); }
		public void ReleaseStream() { m_Stream = null; }

		public void Init()
		{
			m_ProcessedSize = 0;
			m_Pos = 0;
		}

		public void WriteByte(byte b)
		{
			m_Buffer[m_Pos++] = b;
			if (m_Pos >= m_BufferSize)
				FlushData();
		}

		public void FlushData()
		{
			if (m_Pos == 0)
				return;
			m_Stream.Write(m_Buffer, 0, (int)m_Pos);
			m_Pos = 0;
		}

		public ulong GetProcessedSize() { return m_ProcessedSize + m_Pos; }
	}
}
