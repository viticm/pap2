/*
/* EASYCrypt.h: interface for the CEASYCrypt class.
/* by Cooler liuyujun@263.net
/* modified : chenqianjiang  2004-09-10
*/


#ifndef _EASYCRYPT__H
#define _EASYCRYPT__H



extern "C" int Encrypt(size_t dwEncryptDataLen, unsigned char *pEncryptData, unsigned *puCryptSign);

extern "C" int Decrypt(size_t dwDecryptDataLen, unsigned char *pDecryptData, unsigned *puCryptSign);

#endif /* !_EASYCRYPT__H */
