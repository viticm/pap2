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
	struct BitEncoder
	{
		public const int kNumBitModelTotalBits = 11;
		public const uint kBitModelTotal = (1 << kNumBitModelTotalBits);
		const int kNumMoveBits = 5;
		const int kNumMoveReducingBits = 2;
		public const int kNumBitPriceShiftBits = 6;

		uint Prob;

		public void Init() { Prob = kBitModelTotal >> 1; }

		public void UpdateModel(uint symbol)
		{
			if (symbol == 0)
				Prob += (kBitModelTotal - Prob) >> kNumMoveBits;
			else
				Prob -= (Prob) >> kNumMoveBits;
		}

		public void Encode(Encoder encoder, uint symbol)
		{
			// encoder.EncodeBit(Prob, kNumBitModelTotalBits, symbol);
			// UpdateModel(symbol);
			uint newBound = (encoder.Range >> kNumBitModelTotalBits) * Prob;
			if (symbol == 0)
			{
				encoder.Range = newBound;
				Prob += (kBitModelTotal - Prob) >> kNumMoveBits;
			}
			else
			{
				encoder.Low += newBound;
				encoder.Range -= newBound;
				Prob -= (Prob) >> kNumMoveBits;
			}
			if (encoder.Range < Encoder.kTopValue)
			{
				encoder.Range <<= 8;
				encoder.ShiftLow();
			}
		}

		private static UInt32[] ProbPrices = new UInt32[kBitModelTotal >> kNumMoveReducingBits];

		static BitEncoder()
		{
			const int kNumBits = (kNumBitModelTotalBits - kNumMoveReducingBits);
			for (int i = kNumBits - 1; i >= 0; i--)
			{
				UInt32 start = (UInt32)1 << (kNumBits - i - 1);
				UInt32 end = (UInt32)1 << (kNumBits - i);
				for (UInt32 j = start; j < end; j++)
					ProbPrices[j] = ((UInt32)i << kNumBitPriceShiftBits) +
						(((end - j) << kNumBitPriceShiftBits) >> (kNumBits - i - 1));
			}
		}

		public uint GetPrice(uint symbol)
		{
			return ProbPrices[(((Prob - symbol) ^ ((-(int)symbol))) & (kBitModelTotal - 1)) >> kNumMoveReducingBits];
		}
	  public uint GetPrice0() { return ProbPrices[Prob >> kNumMoveReducingBits]; }
		public uint GetPrice1() { return ProbPrices[(kBitModelTotal - Prob) >> kNumMoveReducingBits]; }
	}

	struct BitDecoder
	{
		public const int kNumBitModelTotalBits = 11;
		public const uint kBitModelTotal = (1 << kNumBitModelTotalBits);
		const int kNumMoveBits = 5;

		uint Prob;

		public void UpdateModel(int numMoveBits, uint symbol)
		{
			if (symbol == 0)
				Prob += (kBitModelTotal - Prob) >> numMoveBits;
			else
				Prob -= (Prob) >> numMoveBits;
		}

		public void Init() { Prob = kBitModelTotal >> 1; }

		public uint Decode(RangeCoder.Decoder rangeDecoder)
		{
			uint newBound = (uint)(rangeDecoder.Range >> kNumBitModelTotalBits) * (uint)Prob;
			if (rangeDecoder.Code < newBound)
			{
				rangeDecoder.Range = newBound;
				Prob += (kBitModelTotal - Prob) >> kNumMoveBits;
				if (rangeDecoder.Range < Decoder.kTopValue)
				{
					rangeDecoder.Code = (rangeDecoder.Code << 8) | (byte)rangeDecoder.Stream.ReadByte();
					rangeDecoder.Range <<= 8;
				}
				return 0;
			}
			else
			{
				rangeDecoder.Range -= newBound;
				rangeDecoder.Code -= newBound;
				Prob -= (Prob) >> kNumMoveBits;
				if (rangeDecoder.Range < Decoder.kTopValue)
				{
					rangeDecoder.Code = (rangeDecoder.Code << 8) | (byte)rangeDecoder.Stream.ReadByte();
					rangeDecoder.Range <<= 8;
				}
				return 1;
			}
		}
	}
}
