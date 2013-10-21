
// BufSocket.h: interface for the CBufSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFSOCKET_H__ABC62098_FF8C_412F_BE5B_820881B1E334__INCLUDED_)
#define AFX_BUFSOCKET_H__ABC62098_FF8C_412F_BE5B_820881B1E334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 包含头文件
#include "winsock2.h"

// 宏定义
#define Min(a, b)               (((a) < (b)) ? (a): (b))
#define INext(i, n)             ((((i) + 1) < (n)) ? ((i) + 1): 0)

#undef	IN
#define IN
#undef	OUT
#define OUT

// 常量
CONST INT SOCKET_SUCCESS					= 0;		// SOCKET函数成功返回
CONST INT SOCKET_FAIL						= (-1);		// SOCKET函数失败
CONST INT SOCKET_BUFFER_TOO_SMALL			= 1;		// 缓冲区太小
CONST INT INVALID_CHAR						= 0x100;	// 无效接收字符(正常字符是单字节的)

static CONST INT DEFAULT_BUFFER_SIZE			= 1024 * 10;		// 缺省的缓冲区大小
static CONST INT DEFAULT_SHUTDOWN_RECV_TIMEOUT	= 4*1000;	// 优雅关闭时延迟4秒
static CONST INT DEFAULT_BLOCKED_SNDRCV_SLEEP	= 5;		// 5毫秒(发生阻塞时等待[睡眠]时间)
static CONST INT DEFAULT_CONN_TIMEOUT			= 20*1000;	// 20秒 连接超时
static CONST INT DEFAULT_SEND_TIMEOUT			= 20*1000;	// 20秒 发送超时
static CONST INT DEFAULT_RECV_TIMEOUT			= 20*1000;	// 20秒 接收超时 
static CONST INT DEFAULT_ACCEPT_TIMEOUT			= 20*1000;	// 20秒 等待超时 


#pragma pack(push, 1)

// 数据结构(缓冲区循环使用)
typedef struct _tagBufSocketData
{
    SOCKET			hSocket;			// SOCKET
    INT             nBufferSize;		// 数据缓冲大小
    LPSTR           pszBuffer;			// 数据缓冲
	LPSTR			pszBufferRecv;		// Recv数据缓冲
    INT             nBytesInBuffer;		// 缓冲区已有数据长度(字节)
    INT             nReadIndex;			// 可以读取的下一缓冲区位置
    INT             nBufferIndex;		// 可以使用的下一缓冲区位置	
} BSD,*PBSD;

#pragma pack(pop)


class CBufSocket  
{
public:
	CBufSocket();
	virtual ~CBufSocket();//是基类,有继承类

public:
	// 初始化及释放动态链接库
	static BOOL		InitLibrary( INT nMajorVer = 2 ,INT nMinorVer = 2 );
	static VOID		CleanupLibrary();
	static DWORD	GetIP(LPCSTR pszName,BOOL bFixNtDNS  = FALSE); // Used to Fix NT DNS Problem	

public:

	//seawind
	HANDLE m_hStopEvent;

	INT		GetLastError(VOID);	// 获取错误

	BOOL	Create(	INT		nAddressFamily	= AF_INET, 
					INT		nType			= SOCK_STREAM, 
					INT		nProtocol		= 0,
					INT		nBufferSize		= DEFAULT_BUFFER_SIZE,
					SOCKET	hSocket			= INVALID_SOCKET,
					DWORD	dwFlags			= WSA_FLAG_OVERLAPPED);

	VOID	Close(	BOOL bHardClose = FALSE );
	INT		Recv( OUT CHAR *pszBuffer, INT nBufferSize, DWORD dwTimeout = DEFAULT_RECV_TIMEOUT);
	INT		Send(CHAR CONST * pszBuffer, INT nBufferSize, DWORD dwTimeout = DEFAULT_SEND_TIMEOUT);
	
	BOOL	Connect(CHAR CONST * pszServer, INT nPort,DWORD dwTimeout = DEFAULT_CONN_TIMEOUT,BOOL bFixNtDNS = FALSE);
	INT		BSDGetChar(DWORD dwTimeout = DEFAULT_RECV_TIMEOUT);
	INT		BSDGetString(OUT CHAR *pszBuffer, INT nBufferSize,DWORD dwTimeout = DEFAULT_RECV_TIMEOUT);
	INT		BSDGetData(OUT CHAR *pszBuffer, INT nBufferSize,DWORD dwTimeout = DEFAULT_RECV_TIMEOUT);
	
	SOCKET	GetSocket();
	BOOL	GetSockName(OUT CHAR *pszIP,OUT INT& nPort);

	// Add in 1.1 <
	BOOL	Listen(INT nConnections = 5);
	BOOL	Bind(INT nPort = 0);
	SOCKET	Accept(struct sockaddr * pSocketAddr, LPINT nAddrLen,DWORD dwTimeout = DEFAULT_ACCEPT_TIMEOUT);
	SOCKET	Accept(struct sockaddr * pSocketAddr, LPINT nAddrLen,HANDLE hEndEvent,DWORD dwTimeout = DEFAULT_ACCEPT_TIMEOUT);
	// Add in 1.1 >

	// Add in 1.2 <
	BOOL	RecvFrom(OUT LPSTR pszFrom,OUT INT& nFromPort,OUT CHAR *pszBuffer,IN INT nBufferSize,IN DWORD dwTimeout = DEFAULT_RECV_TIMEOUT);
	BOOL	SendTo(LPCSTR pszTo,INT nToPort,LPCSTR pszBuffer, INT nBufferSize, DWORD dwTimeout = DEFAULT_SEND_TIMEOUT);
	BOOL	SetSockOpt( INT nOptionName, LPCVOID lpOptionValue, INT nOptionLen, INT nLevel = SOL_SOCKET );
	BOOL	GetSockOpt( INT nOptionName, LPVOID lpOptionValue, LPINT lpOptionLen, INT nLevel = SOL_SOCKET );
	// Add in 1.2 >
	

private:
	VOID	SetLastError(INT nErrorCode);
	BOOL	SetSocketOption(SOCKET hSocket);
	BOOL	Connect(SOCKET hSocket, CONST struct sockaddr * pSocketAddr, INT iAddrLen,DWORD dwTimeout);
	INT     RecvOnce(SOCKET hSocket, OUT CHAR *pszBuffer, INT nBufferSize);
	INT		RecvData(SOCKET hSocket, OUT CHAR *pszBuffer, INT nBufferSize, DWORD dwTimeout);
	INT     SendOnce(SOCKET hSocket, CHAR CONST * pszBuffer, INT nBufferSize);
	INT		SendData(SOCKET hSocket, CHAR CONST * pszBuffer, INT nBufferSize, DWORD dwTimeout);
	INT		BSDReadData(DWORD dwTimeout);

	// Add in 1.1 <
	BOOL	Bind(SOCKET hSocket, CONST struct sockaddr * pSocketAddr, INT nAddrLen);
	BOOL    Block(SOCKET hSocket, BOOL bBlock);
	// Add in 1.1 >

	// Add in 1.2 <
	INT		RecvFrom(SOCKET hSocket, struct sockaddr * pFrom, INT nFromlen,CHAR *pszBuffer, INT nBufferSize, DWORD dwTimeout);
	INT		SendTo(SOCKET hSocket, CONST struct sockaddr * pTo,INT nToLen,CHAR CONST * pszBuffer, INT nBufferSize, DWORD dwTimeout);
	// Add in 1.2 >

private:
	SOCKET	m_hSocket;
	PBSD	m_pBSD;
	INT		m_nErrorCode;	//错误代号
};

#endif // !defined(AFX_BUFSOCKET_H__ABC62098_FF8C_412F_BE5B_820881B1E334__INCLUDED_)
