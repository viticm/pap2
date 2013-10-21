////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGBitSet.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-1-28 10:11:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGBitSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(KG_ENABLE_TDD)
namespace KG_CUSTOM_HELPERS
{
	KG_TEST_BEGIN(KGBitSet)
	{
		//Black Box 1: Clean Condition
		{
			KGBitSet<32> BitSet;
			_ASSERTE(! BitSet.Any());
			_ASSERTE(BitSet.None());
			{
				_ASSERTE(! BitSet.Is<0>());
				_ASSERTE(! BitSet.At<0>());
				_ASSERTE(! BitSet.Test<0>());
				_ASSERTE(! BitSet.Is<2>());
				_ASSERTE(! BitSet.Is<16>());
				_ASSERTE(! BitSet.Is<31>());
			}
		}
		//Unit1 : //Set, Reset
		{
			KGBitSet<32> BitSet;

			BitSet.SetTrue<15>();
			_ASSERTE(BitSet.Is<15>());

			{
				_ASSERTE(! BitSet.Is<0>());
				_ASSERTE(! BitSet.At<0>());
				_ASSERTE(! BitSet.Test<0>());
				_ASSERTE(! BitSet.Is<2>());
				_ASSERTE(! BitSet.Is<16>());
				_ASSERTE(! BitSet.Is<31>());
			}

			BitSet.SetFalse<15>();
			_ASSERTE(! BitSet.Is<15>());

			{
				_ASSERTE(! BitSet.Is<0>());
				_ASSERTE(! BitSet.At<0>());
				_ASSERTE(! BitSet.Test<0>());
				_ASSERTE(! BitSet.Is<2>());
				_ASSERTE(! BitSet.Is<16>());
				_ASSERTE(! BitSet.Is<31>());
			}
		}
		//Unit2 //Set Version2
		{
			KGBitSet<32> BitSet;

			BitSet.Set<13>(TRUE);
			_ASSERTE(BitSet.Is<13>());

			{
				_ASSERTE(! BitSet.Is<0>());
				_ASSERTE(! BitSet.At<0>());
				_ASSERTE(! BitSet.Test<0>());
				_ASSERTE(! BitSet.Is<2>());
				_ASSERTE(! BitSet.Is<16>());
				_ASSERTE(! BitSet.Is<31>());
			}

			BitSet.Set<13>(FALSE);
			_ASSERTE(! BitSet.Is<13>());

			{
				_ASSERTE(! BitSet.Is<0>());
				_ASSERTE(! BitSet.At<0>());
				_ASSERTE(! BitSet.Test<0>());
				_ASSERTE(! BitSet.Is<2>());
				_ASSERTE(! BitSet.Is<16>());
				_ASSERTE(! BitSet.Is<31>());
			}
		}

#if 0
		//Should Failed to be Compiled
		{
			KGBitSet<33> BitSet;
		}
#endif
#if 0
		{
			KGBitSet<32> BitSet;
			BitSet.Is<33>();	
			BitSet.SetTrue<33>();
			BitSet.SetFalse<33>();
		}
#endif

		//_KG3D_DEBUG_REPORT("KGBitSet All Runtime Tests Passed, Congratulations!\n");
	}
	KG_TEST_END(KGBitSet)
};
#endif
////////////////////////////////////////////////////////////////////////////////

