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

// IMatchFinder.cs

using System;

namespace Fireball.IO.Compression.SevenZip.Compression.LZ
{
	interface IInWindowStream
	{
		void SetStream(System.IO.Stream inStream);
		void Init();
		void ReleaseStream();
		Byte GetIndexByte(Int32 index);
		UInt32 GetMatchLen(Int32 index, UInt32 distance, UInt32 limit);
		UInt32 GetNumAvailableBytes();
	}

	interface IMatchFinder : IInWindowStream
	{
		void Create(UInt32 historySize, UInt32 keepAddBufferBefore,
				UInt32 matchMaxLen, UInt32 keepAddBufferAfter);
		UInt32 GetMatches(UInt32[] distances);
		void Skip(UInt32 num);
	}
}
