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

using System;

namespace Fireball.IO.Compression.SevenZip.Compression.RangeCoder
{
	struct BitTreeEncoder
	{
		BitEncoder[] Models;
		int NumBitLevels;

		public BitTreeEncoder(int numBitLevels)
		{
			NumBitLevels = numBitLevels;
			Models = new BitEncoder[1 << numBitLevels];
		}

		public void Init()
		{
			for (uint i = 1; i < (1 << NumBitLevels); i++)
				Models[i].Init();
		}

		public void Encode(Encoder rangeEncoder, UInt32 symbol)
		{
			UInt32 m = 1;
			for (int bitIndex = NumBitLevels; bitIndex > 0; )
			{
				bitIndex--;
				UInt32 bit = (symbol >> bitIndex) & 1;
				Models[m].Encode(rangeEncoder, bit);
				m = (m << 1) | bit;
			}
		}

		public void ReverseEncode(Encoder rangeEncoder, UInt32 symbol)
		{
			UInt32 m = 1;
			for (UInt32 i = 0; i < NumBitLevels; i++)
			{
				UInt32 bit = symbol & 1;
				Models[m].Encode(rangeEncoder, bit);
				m = (m << 1) | bit;
				symbol >>= 1;
			}
		}

		public UInt32 GetPrice(UInt32 symbol)
		{
			UInt32 price = 0;
			UInt32 m = 1;
			for (int bitIndex = NumBitLevels; bitIndex > 0; )
			{
				bitIndex--;
				UInt32 bit = (symbol >> bitIndex) & 1;
				price += Models[m].GetPrice(bit);
				m = (m << 1) + bit;
			}
			return price;
		}

		public UInt32 ReverseGetPrice(UInt32 symbol)
		{
			UInt32 price = 0;
			UInt32 m = 1;
			for (int i = NumBitLevels; i > 0; i--)
			{
				UInt32 bit = symbol & 1;
				symbol >>= 1;
				price += Models[m].GetPrice(bit);
				m = (m << 1) | bit;
			}
			return price;
		}

		public static UInt32 ReverseGetPrice(BitEncoder[] Models, UInt32 startIndex,
			int NumBitLevels, UInt32 symbol)
		{
			UInt32 price = 0;
			UInt32 m = 1;
			for (int i = NumBitLevels; i > 0; i--)
			{
				UInt32 bit = symbol & 1;
				symbol >>= 1;
				price += Models[startIndex + m].GetPrice(bit);
				m = (m << 1) | bit;
			}
			return price;
		}

		public static void ReverseEncode(BitEncoder[] Models, UInt32 startIndex,
			Encoder rangeEncoder, int NumBitLevels, UInt32 symbol)
		{
			UInt32 m = 1;
			for (int i = 0; i < NumBitLevels; i++)
			{
				UInt32 bit = symbol & 1;
				Models[startIndex + m].Encode(rangeEncoder, bit);
				m = (m << 1) | bit;
				symbol >>= 1;
			}
		}
	}

	struct BitTreeDecoder
	{
		BitDecoder[] Models;
		int NumBitLevels;

		public BitTreeDecoder(int numBitLevels)
		{
			NumBitLevels = numBitLevels;
			Models = new BitDecoder[1 << numBitLevels];
		}

		public void Init()
		{
			for (uint i = 1; i < (1 << NumBitLevels); i++)
				Models[i].Init();
		}

		public uint Decode(RangeCoder.Decoder rangeDecoder)
		{
			uint m = 1;
			for (int bitIndex = NumBitLevels; bitIndex > 0; bitIndex--)
				m = (m << 1) + Models[m].Decode(rangeDecoder);
			return m - ((uint)1 << NumBitLevels);
		}

		public uint ReverseDecode(RangeCoder.Decoder rangeDecoder)
		{
			uint m = 1;
			uint symbol = 0;
			for (int bitIndex = 0; bitIndex < NumBitLevels; bitIndex++)
			{
				uint bit = Models[m].Decode(rangeDecoder);
				m <<= 1;
				m += bit;
				symbol |= (bit << bitIndex);
			}
			return symbol;
		}

		public static uint ReverseDecode(BitDecoder[] Models, UInt32 startIndex,
			RangeCoder.Decoder rangeDecoder, int NumBitLevels)
		{
			uint m = 1;
			uint symbol = 0;
			for (int bitIndex = 0; bitIndex < NumBitLevels; bitIndex++)
			{
				uint bit = Models[startIndex + m].Decode(rangeDecoder);
				m <<= 1;
				m += bit;
				symbol |= (bit << bitIndex);
			}
			return symbol;
		}
	}
}
