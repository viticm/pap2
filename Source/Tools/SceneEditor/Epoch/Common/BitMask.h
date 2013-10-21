// File: BitMask.h
// Desc:

#ifndef BITMASK_H
#define BITMASK_H

#include"DisableWarns.h"

#define MakeBitMask(uPos, uValue) (0xFFFFFFFF & ((uValue) << (uPos)))  // value is either 0 or 1

namespace Common
{

inline unsigned int SetBit(unsigned int uOrigin, unsigned int uPos, unsigned int uValue)  // value is either 0 or 1
{
	unsigned char ucOriginBits[32];
	unsigned int i = 0;
	for(; i < 32; ++i)
	{
		if(i == uPos)
		{
			ucOriginBits[i] = uValue;
		}
		else
		{
			ucOriginBits[i] = ((uOrigin >> i) & MakeBitMask(0, 1));
		}
	}

	unsigned int ret = 0;
	for(i = 0; i < 32; ++i)
	{
		ret |= (ucOriginBits[i] << i);
	}
	
	return ret;
}

inline bool TestBit(unsigned int uOrigin, unsigned int uPos, unsigned int uValue)
{
	return !((uOrigin & MakeBitMask(uPos, 1)) ^ MakeBitMask(uPos, uValue));
}

}  // namespace

#endif