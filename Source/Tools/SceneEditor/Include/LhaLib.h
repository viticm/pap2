//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KLhaLib.h
// Date:	2000.12.25
// Code:	Daniel Wang
// Desc:	Ver1.14	Source All chagned, 1995.01.14 N.Watazaki
//---------------------------------------------------------------------------
#ifndef KLhaLib_H
#define KLhaLib_H
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//---------------------------------------------------------------------------
void	lha_encode(unsigned char* pIn, 
				   unsigned long nIn,
				   unsigned char* pOut,
				   unsigned long* pnOut);
void	lha_decode(unsigned char* pIn,
				   unsigned long nIn,
				   unsigned char* pOut,
				   unsigned long* pnOut);
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif 
//---------------------------------------------------------------------------
#endif
