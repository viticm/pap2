#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

#ifndef interface
#define interface struct
#endif

interface IEncrypt
{
	virtual int AddRef() = 0;

	virtual void Release() = 0;

	virtual int Encode(
		unsigned int uSize,
		unsigned char* szBuffer,
		unsigned* pKey) = 0;

	virtual int Decode(
		unsigned int uSize,
		unsigned char* szBuffer,
		unsigned* pKey) = 0;
};

typedef struct _key {
	void* pN;
	int	nNLen;
	void* pE;
	int	nELen;
	void* pD;
	int	nDLen;
}MULTI_KEY;

interface IEncrypt2
{
	virtual int AddRef() = 0;
	virtual void Release() = 0;
	
	virtual int Encode(
		void* szBufferIn,
		int nSizeIn,
		void* szBufferOut,
		int& nSizeOut,
		MULTI_KEY Key ) = 0;
	
	virtual int Decode(
		void* szBufferIn,
		int nSizeIn,
		void* szBufferOut,
		int& nSizeOut,
		MULTI_KEY Key ) = 0;
};

IEncrypt* BlazeCreatePSEncrypt( );
IEncrypt* BlazeCreateKSGEncrypt( );
IEncrypt2* BlazeCreateRC4Encrypt( );
IEncrypt2* BlazeCreateRSAEncrypt( );
IEncrypt2* BlazeCreateBase64Encrypt( );

#endif
