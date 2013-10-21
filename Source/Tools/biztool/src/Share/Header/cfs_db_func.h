//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-8 13:28
//      File_base        : cfs_db_func
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : mysql database wrap global functions
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__DB___FUNC____H_____
#define _CFS__DB___FUNC____H_____

//////////////////////////////////////////////////////////////////////////
// Include region
#include <time.h>
#include "cfs_filelogs.h"

#define MAXSIZE_TIMESTRING				64

//////////////////////////////////////////////////////////////////////////
// cfs_db_func class define region

class cfs_db_func
{
public:
	cfs_db_func(){}
	virtual ~cfs_db_func(){}
	
	static void waitforframe(int nWaitTime/* in millisecond */);

	static const char *GetCurDateTimeWithString(char *pStringBuf = NULL);
	static const char *GetCurYearMonthWithString(char *pStringBuf = NULL);
	static unsigned int GetCurDateTimeWithUNIX();

	static void* db_malloc( unsigned int usize );
	static void db_free( void* p );
};

// Public inline
inline 
unsigned int cfs_db_func::GetCurDateTimeWithUNIX()
{
	return time(NULL);
}

#endif // _CFS__DB___FUNC____H_____
