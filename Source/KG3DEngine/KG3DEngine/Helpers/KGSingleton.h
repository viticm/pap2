////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSingleton.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-10-31 16:07:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSINGLETON_H_
#define _INCLUDE_KGSINGLETON_H_

////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	template<class _Type>
	class TKGSimplerSingletonHolder
	{
	public:
		
	private:
		TKGSimplerSingletonHolder();
		~TKGSimplerSingletonHolder();
		TKGSimplerSingletonHolder(const TKGSimplerSingletonHolder&);
		void operator=(const TKGSimplerSingletonHolder&);
	};
};

#endif //_INCLUDE_KGSINGLETON_H_
