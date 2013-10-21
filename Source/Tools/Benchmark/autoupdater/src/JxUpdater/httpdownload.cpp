///////////////////////////////////////////////////////////////////////////////
// HttpDownload.cpp: implementation of the CHttpDownload class.
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "httpdownload.h"
#include "sockspacket.h"

#define READ_BUFFER_SIZE	(10 * 1024)				// 缓冲大小 10K
#define DEFAULT_SAVE_DIR	_T("C:\\")				// 缺省保存目录
#define DEFAULT_SAVE_FILE	_T("Unknown.htm")		// 缺省保存文件名
#define DEFAULT_USERAGENT	_T("KAVUpdate/1.0")		// 缺省的UserAgent
#define Check(a,b)			if((a))return (b);		

// 用于BASE64编码、解码的静态常量
CString CHttpDownload::m_strBase64TAB = _T( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );
INT		CHttpDownload::m_nBase64Mask[]= { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

////////////////////////////////////////////////////////////////////////////////
//						构造函数
////////////////////////////////////////////////////////////////////////////////
CHttpDownload::CHttpDownload()
{
	m_strDownloadUrl	= _T("");
	m_strSavePath		= _T("");
	m_strTempSavePath	= _T("");
	m_bSaveResponse		= TRUE;	//是否需要保存返回

	// 停止下载
	m_bStopDownload		= FALSE;
	m_hStopEvent		= NULL;

	// 强制重新下载(不管已有的文件是否与远程文件相同)
	m_bForceDownload	= FALSE;

	// 是否支持断点续传(假定不支持)
	m_bSupportResume	= FALSE;

	// 文件以及下载大小
	m_dwFileSize			= 0;	// 文件总的大小	
	m_dwFileDownloadedSize	= 0;	// 文件总共已经下载的大小

	m_dwDownloadSize	= 0;		// 本次Request需要下载的大小
	m_dwDownloadedSize	= 0;		// 本次Request已经下载的大小

	m_dwHeaderSize		= 0;		// HTTP协议头的长度
	m_strHeader			= _T("");	// HTTP协议头

	// Referer
	m_strReferer		= _T("");
	
	// UserAgent
	m_strUserAgent		= DEFAULT_USERAGENT;

	// 超时TIMEOUT	连接超时、发送超时、接收超时(单位：毫秒)
	m_dwConnTimeout		= HTTP_CONN_TIMEOUT;	
	m_dwRecvTimeout		= HTTP_RECV_TIMEOUT;
	m_dwSendTimeout		= HTTP_SEND_TIMEOUT;

	// 重试机制
	m_nRetryType		= HTTP_RETRY_NONE;	//重试类型(0:不重试 1:重试一定次数 2:总是重试)
	m_nRetryTimes		= 0;				//重试次数
	m_nRetryDelay		= 0;				//重试延迟(单位：毫秒)
	m_nRetryMax			= 0;				//重试最大次数

	// 错误处理
	m_nErrorCount		= 0;			//错误次数
	m_strError			= _T("");		//错误信息
	m_dwErrorCode		= 0;			//错误代码

	// 向其他窗口发送消息
	m_bNotify			= FALSE;		// 是否向外发送通知消息	
	m_hNotifyWnd		= NULL;			// 被通知的窗口
	m_nNotifyMessage	= 0;			// 被通知的消息

	// 是否进行验证 : Request-Header: Authorization
	m_bAuthorization	= FALSE;
	m_strUsername		= _T("");
	m_strPassword		= _T("");

	// 是否使用代理 
	m_bProxy			= FALSE;
	m_strProxyServer	= _T("");
	m_nProxyPort		= 0;
	m_nProxyType		= HTTP_PROXY_NONE;
	
	// 代理是否需要验证: Request-Header: Proxy-Authorization
	m_bProxyAuthorization = FALSE;
	m_strProxyUsername 	= _T("");
	m_strProxyPassword	= _T("");

	// 下载过程中所用的变量
	m_strServer			= _T("");
	m_strObject			= _T("");
	m_strFileName		= _T("");
	m_nPort				= DEFAULT_HTTP_PORT ;
	m_nVerb				= HTTP_VERB_GET;
	m_strData			= _T("");

	// 是否使用Cookie
	m_strCookie			= _T("");
	m_bUseIECookie		= FALSE;
	
}


////////////////////////////////////////////////////////////////////////////////
//		析构函数
////////////////////////////////////////////////////////////////////////////////
CHttpDownload::~CHttpDownload()
{
	CloseSocket();
	if( m_hStopEvent != NULL )
	{
		CloseHandle( m_hStopEvent );
		m_hStopEvent = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL CreateSocket()
//	用  途：创建SOCKET
//	对全局变量的影响：无
//	参  数：无
//	返回值：BOOL
//		TRUE : 成功创建SOCKET
//		FALSE: 创建SOCKET失败
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::CreateSocket()
{
	CloseSocket();
	return m_cBufSocket.Create(AF_INET,SOCK_STREAM,0,READ_BUFFER_SIZE);
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID CloseSocket()
//	用  途：关闭SOCKET
//	对全局变量的影响：无
//	参  数：无
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::CloseSocket()
{
	m_cBufSocket.Close(TRUE);	
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT Download(
//				LPCTSTR lpszDownloadUrl,
//				LPCTSTR lpszSavePath	/*= NULL*/,
//				BOOL bForceDownload		/*= FALSE*/,
//				BOOL bSaveResponse		/*= TRUE*/,
//				INT nVerb				/*= HTTP_VERB_GET*/,
//				LPCTSTR lpszData		/*= NULL*/,
//				LPTSTR lpszFtpURL		/*= NULL*/) 
//	用  途：下载入口，调用此函数来开始下载过程
//	对全局变量的影响：无
//	参  数：
//		lpszDownloadUrl : 待下载的网络URL
//		lpszSavePath    : 本地保存路径(可以是全路径或者仅仅是目录，目录需以\结尾)	
//		bForceDownload  : 是否强制下载，不管文件是否与远程一致
//		bSaveResponse   : 是否保存服务器的返回
//		nVerb           : HTTP命令
//		lpszData        : 要在发送请求时传给服务器的数据(用于POST)
//		lpszFtpURL		: 如果重定向到FTP,此指针用来保存FTP的URL
//	返回值：INT
//		HTTP_RESULT_STOP		: 用户停止下载(调用了StopDownload函数)
//		HTTP_RESULT_FAIL		: 下载失败
//		HTTP_RESULT_SUCCESS		: 下载成功
//		HTTP_RESULT_SAMEAS		: 本地已有完全一样的文件，不用再下载
//		HTTP_RESULT_REDIRECT_FTP: 重定向到FTP
////////////////////////////////////////////////////////////////////////////////
INT CHttpDownload::Download(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSavePath /*= NULL*/,BOOL bForceDownload /*= FALSE*/,BOOL bSaveResponse /*= TRUE*/,INT nVerb /*= HTTP_VERB_GET*/,LPCTSTR lpszData /*= NULL*/,LPTSTR lpszFtpURL /*= NULL*/ )
{
	m_bStopDownload	  = FALSE;
	m_bForceDownload  = bForceDownload;
	m_nRetryTimes	  = 0;
	m_nErrorCount	  = 0;
	m_strTempSavePath = _T("");
	
	m_bSaveResponse	  = bSaveResponse;
	m_nVerb			  = nVerb;
	m_strData		  = _T("");
	if( lpszData != NULL )
		m_strData	+=	lpszData;
	

	// 检验要下载的URL是否为空
	m_strDownloadUrl = lpszDownloadUrl;
	m_strDownloadUrl.TrimLeft();
	m_strDownloadUrl.TrimRight();
	if( m_strDownloadUrl.IsEmpty() )
		return HTTP_RESULT_FAIL;

	// 检验要下载的URL是否有效
	if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort))
	{
		// 在前面加上"http://"再试
		m_strDownloadUrl = _T("http://") + m_strDownloadUrl;
		if ( !ParseURL(m_strDownloadUrl,m_strServer, m_strObject, m_nPort) )
		{
			//TRACE1(_T("Failed to parse the URL: %s\n"), m_strDownloadUrl);
			return HTTP_RESULT_FAIL;
		}
	}

	// 检查本地保存路径
	m_strSavePath =  lpszSavePath;
	m_strSavePath.TrimLeft();
	m_strSavePath.TrimRight();
	if( m_strSavePath.IsEmpty() )
		m_strSavePath = DEFAULT_SAVE_DIR;
	
	if( m_strSavePath.Right(1) != '\\' )	// 只有路径，没有文件名
	{
		m_strTempSavePath =  m_strSavePath;
		m_strTempSavePath += ".tmp";
	}

	// 创建停止事件
	if( m_hStopEvent == NULL )
	{
		m_hStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		if( m_hStopEvent == NULL )
			return HTTP_RESULT_FAIL;

		//seawind
		m_cBufSocket.m_hStopEvent = m_hStopEvent;
	}
	ResetEvent( m_hStopEvent );

	
	// 设置下载数据
	m_dwDownloadedSize		= 0;
	m_dwFileDownloadedSize	= 0;
	m_dwFileSize			= 0;
	m_dwDownloadSize		= 0;

	BOOL bSendOnce = TRUE;		// 用于控制向hWndNotify窗口发送消息
	BOOL bSendRequestAgain = TRUE;
	
	while( bSendRequestAgain )
	{
		INT nRequestRet = SendRequest( nVerb,lpszFtpURL ) ;

		//seawind
		//if (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
		//	return HTTP_RESULT_STOP;
		Check( m_bStopDownload, HTTP_RESULT_STOP);

		switch(nRequestRet)
		{
		case HTTP_REQUEST_SUCCESS:
			// 需要保存否?
			if( !m_bSaveResponse )
			{
				//关闭SOCKET
				CloseSocket();
				return HTTP_REQUEST_SUCCESS;				
			}
			break;
		case HTTP_REQUEST_STOP:
			return HTTP_RESULT_STOP;
			break;
		case HTTP_REQUEST_FAIL:
			return HTTP_RESULT_FAIL;
			break;
		case HTTP_REQUEST_REDIRECT_FTP:
			return HTTP_RESULT_REDIRECT_FTP;
			break;
		case HTTP_REQUEST_ERROR:

			Check( m_bStopDownload, HTTP_RESULT_STOP);	// 是否应该停止下载

			switch( m_nRetryType )
			{
			case HTTP_RETRY_NONE:
				return HTTP_RESULT_FAIL;
				break;
			case HTTP_RETRY_ALWAYS:
				if( m_nRetryDelay > 0 )
				{
					//为了让等待时也能退出，不能使用Sleep函数
					if( WaitForSingleObject(m_hStopEvent,m_nRetryDelay) == WAIT_OBJECT_0 )
						return HTTP_RESULT_STOP;
				}
				continue;
				break;
			case HTTP_RETRY_TIMES:
				if( m_nRetryTimes > m_nRetryMax )
					return HTTP_RESULT_FAIL;
				m_nRetryTimes++;
			
				if( m_nRetryDelay > 0 )
				{
					//为了让等待时也能退出，不能使用Sleep函数
					if( WaitForSingleObject(m_hStopEvent,m_nRetryDelay) == WAIT_OBJECT_0 )
						return HTTP_RESULT_STOP;
				}
				continue;
				break;
			default:
				return HTTP_RESULT_FAIL;
				break;
			}
			break;
		default:
			return HTTP_RESULT_FAIL;
			break;
		}

		if( m_strSavePath.Right(1) == '\\' )
			m_strSavePath += m_strFileName;

		if( !m_bForceDownload ) // 非强制下载，检查Last-Modified
		{
			CFileStatus fileStatus;
			if (CFile::GetStatus(m_strSavePath,fileStatus))
			{
				// 可能会存在1秒的误差
				if (( fileStatus.m_mtime - m_TimeLastModified <=2 && m_TimeLastModified-fileStatus.m_mtime<=2 ) && (DWORD)fileStatus.m_size == m_dwFileSize )
					return HTTP_RESULT_SAMEAS;
			}
		}

		CFile fileDown;
		if(! fileDown.Open(m_strTempSavePath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyWrite) )
			return HTTP_RESULT_FAIL;	

		// 应该判断一下是否支持断点续传
		if( m_bSupportResume )
		{
			try
			{
				fileDown.SeekToEnd();
			}
			catch(CFileException* e)                                         
			{
				e->Delete();
				return HTTP_RESULT_FAIL;
			}	
		}

		// 发送消息
 		if( bSendOnce && m_bNotify )
 		{
 			HTTPDOWNLOADSTATUS DownloadStatus;
 			
 			DownloadStatus.dwFileSize  = m_dwFileSize;
 			DownloadStatus.strFileName = m_strFileName;
 			DownloadStatus.dwFileDownloadedSize  = m_dwFileDownloadedSize;
// 
// 			DownloadStatus.nStatusType = HTTP_STATUS_FILESIZE;			
// 			if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
// 				::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_HTTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);
// 			else
// 				return HTTP_RESULT_STOP;
// 
// 			DownloadStatus.nStatusType = HTTP_STATUS_FILENAME;
// 			if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
// 				::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_HTTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);	
// 			else
// 				return HTTP_RESULT_STOP;
// 		
 			DownloadStatus.nStatusType = HTTP_STATUS_FILEDOWNLOADEDSIZE;
 			if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
 				::SendMessage(m_hNotifyWnd,m_nNotifyMessage, HTTP_STATUS_FILEDOWNLOADEDSIZE, (LPARAM)&DownloadStatus);	
 			else
 				return HTTP_RESULT_STOP;
 			
 			bSendOnce = FALSE;
 		}

		m_dwDownloadedSize = 0;
		// 现在开始读取数据
		CHAR szReadBuf[READ_BUFFER_SIZE+1];

		/////////////////////////////////////////////
		// 如果m_dwDownloadSize = 0 (说明大小未知)
		if( m_dwDownloadSize == 0 )
			m_dwDownloadSize = 0xFFFFFFFF;
		////////////////////////////////////////////
		bSendRequestAgain = FALSE;
		do
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);            
			}

			// 是否应该停止下载
			Check( m_bStopDownload, HTTP_RESULT_STOP);
			ZeroMemory(szReadBuf,READ_BUFFER_SIZE+1);
			INT nRet = 	m_cBufSocket.BSDGetData(szReadBuf,READ_BUFFER_SIZE,m_dwRecvTimeout);

			//seawind
			Check( m_bStopDownload, HTTP_RESULT_STOP);

			if (nRet <= 0)
			{
				////////////////////////////////////////
				if( m_dwDownloadSize == 0xFFFFFFFF )
					break;
				///////////////////////////////////////
				m_nErrorCount++;
				bSendRequestAgain = TRUE;
				break;// 跳出内层循环
			}

			// 将数据写入文件
			try
			{
				fileDown.Write(szReadBuf,nRet);
			}
			catch(CFileException* e)
			{
				e->Delete();
				bSendRequestAgain = TRUE;
				break;// 跳出内层循环
			}

			m_dwDownloadedSize		+= nRet;
			m_dwFileDownloadedSize	+= nRet;

			// 通知消息
			if( m_bNotify )
			{
				HTTPDOWNLOADSTATUS DownloadStatus;
				DownloadStatus.nStatusType			= HTTP_STATUS_FILEDOWNLOADEDSIZE;
				DownloadStatus.dwFileDownloadedSize = m_dwFileDownloadedSize;
				DownloadStatus.dwFileSize			= m_dwFileSize;
				DownloadStatus.strFileName			= m_strFileName;
				
				if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
					::SendMessage(m_hNotifyWnd,m_nNotifyMessage, HTTP_STATUS_FILEDOWNLOADEDSIZE, (LPARAM)&DownloadStatus);
				else
					return HTTP_RESULT_STOP;
			}

		}while(m_dwDownloadedSize < m_dwDownloadSize);

		// 关闭文件
		fileDown.Close();

	} // WHILE LOOP
	
	//关闭SOCKET
	CloseSocket();

	// 文件改名
	//首先将已有的文件删除
	::DeleteFile(m_strSavePath);

	//再将新下载的文件改名
	::MoveFile(m_strTempSavePath, m_strSavePath);
    
    //再将新下载的文件的时间改回去
    CFileStatus fileStatus;
    
    fileStatus.m_size = 0;
    fileStatus.m_attribute = 0;
    
    try
    {
        CFile::GetStatus(m_strSavePath, fileStatus);
        
        fileStatus.m_mtime = m_TimeLastModified;
        CFile::SetStatus(m_strSavePath,fileStatus);
    }
    catch(CFileException *e)
    {
        e->Delete();
    }
    
    return HTTP_RESULT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT SendRequest(
//				INT nVerb /*= HTTP_VERB_GET*/,
//				LPTSTR lpszFtpURL/* = NULL*/ ) 
//	用  途：发送HTTP请求
//	对全局变量的影响：无
//	参  数：
//		nVerb      : HTTP命令
//		lpszFtpURL : 如果重定向到FTP,此指针用来保存FTP的URL
//	返回值：INT
//		HTTP_REQUEST_STOP			: 用户终止
//		HTTP_REQUEST_ERROR			: 发生一般网络错误，可以重试
//		HTTP_REQUEST_SUCCESS		: 请求成功
//		HTTP_REQUEST_FAIL			: 发生验证错误，不用重试
//		HTTP_REQUEST_REDIRECT_FTP	: 请求被重定向到FTP
////////////////////////////////////////////////////////////////////////////////
INT	CHttpDownload::SendRequest(INT nVerb /*= HTTP_VERB_GET*/,LPTSTR lpszFtpURL/* = NULL*/)
{
	// 判断HTTP命令是否合法
	if( nVerb < HTTP_VERB_MIN || nVerb > HTTP_VERB_MAX )
		return HTTP_REQUEST_FAIL;

	// 保存HTTP命令
	m_nVerb = nVerb;

	while (TRUE)
	{
		CString			strSend,strAuth,strHeader,strVerb,strCookie,strRange,strText;
		INT				nRet;
		CHAR			szReadBuf[1025];
		DWORD			dwContentLength,dwStatusCode;

		strSend		= _T("");
		strAuth		= _T("");
		strHeader	= _T("");
		strVerb		= _T("");
		strCookie	= _T("");
		strRange	= _T("");
		strText		= _T("");

		m_dwFileDownloadedSize = 0;			// 文件已经下载大小
		m_dwDownloadSize	   = 0;			// 要下载大小
		strVerb = HTTP_VERB_STR[m_nVerb];	// HTTP命令

		///////////////////////////////////////
		// 目前的版本中，此信息并没有用
		m_strHeader		= _T("");
		m_dwHeaderSize	= 0;
		//////////////////////////////////////
	
		Check( !CreateSocket(), HTTP_REQUEST_FAIL);		// 创建SOCKET
		Check( m_bStopDownload, HTTP_REQUEST_STOP);		// 是否应该停止下载

		// 建立连接
		nRet = MakeConnection( &m_cBufSocket,m_strServer,m_nPort);
		Check( nRet != HTTP_REQUEST_SUCCESS, nRet);
		Check( m_bStopDownload, HTTP_REQUEST_STOP);		// 是否应该停止下载

		if( m_nProxyType == HTTP_PROXY_HTTPGET )		// HTTP_GET代理
		{
			strSend  = strVerb  + m_strDownloadUrl + " HTTP/1.1\r\n";
			if( m_bProxyAuthorization )
			{
				strAuth = _T("");
				Base64Encode(m_strProxyUsername+":"+m_strProxyPassword,strAuth);
				strSend += "Proxy-Authorization: Basic "+strAuth+"\r\n";
			}
		}
		else	// 没有Proxy或者不是HTTP_GET代理
			strSend  = strVerb  + m_strObject + " HTTP/1.1\r\n";
		
		// 访问的页面是否是受保护的
		if( m_bAuthorization )
		{
			strAuth = _T("");
			Base64Encode(m_strUsername+":"+m_strPassword,strAuth);
			strSend += "Authorization: Basic "+strAuth+"\r\n";
		}

		strSend += "Host: " + m_strServer + "\r\n";
		strSend += "Accept: */*\r\n";
		strSend += "Pragma: no-cache\r\n"; 
		strSend += "Cache-Control: no-cache\r\n";
		strSend += "User-Agent: " + m_strUserAgent + "\r\n";
		if( !m_strReferer.IsEmpty() )
			strSend += "Referer: " + m_strReferer + "\r\n";
		strSend += "Connection: close\r\n";

		// 判断是否使用Cookie
		if( m_bUseIECookie )
		{
			// 获取IE的Cookie
			if( GetIECookie(strCookie,m_strDownloadUrl) )
				m_strCookie = strCookie;
			else
				m_strCookie = _T("");
		}
		
		// Cookie
		if( !m_strCookie.IsEmpty() )
			strSend += "Cookie: " + m_strCookie + "\r\n";
		
		// 是否保存服务器的返回
		if( m_bSaveResponse )
		{
			// 判断m_strSavePath是否为路径
			GetFileName();
			if( m_strSavePath.Right(1) == '\\' )
			{
				m_strTempSavePath = m_strSavePath;
				m_strTempSavePath += m_strFileName;
				m_strTempSavePath += ".tmp";
			}

			// 查看文件已经下载的长度
			CFileStatus fileDownStatus;
			if (CFile::GetStatus(m_strTempSavePath,fileDownStatus) && !m_bForceDownload )
			{
				m_dwFileDownloadedSize = (DWORD)fileDownStatus.m_size;
				if (m_dwFileDownloadedSize > 0)
				{
					strRange.Format(_T("Range: bytes=%d-\r\n"),m_dwFileDownloadedSize );
				}
			}
			strSend += strRange;
		}

		// Post?
		switch( m_nVerb )
		{
		case HTTP_VERB_GET:
		case HTTP_VERB_HEAD:
		//	strSend += strRange;
			break;
		case HTTP_VERB_POST:
			strSend += "Content-Type: application/x-www-form-urlencoded\r\n";
			strText.Format("Content-Length: %d\r\n", m_strData.GetLength() );
			strSend += strText;
			break;
		case HTTP_VERB_PUT:
		default:
			break;
		}

		//必须要加一个空行，否则Http服务器将不会应答
		strSend += "\r\n";

		// Post Data?
		switch( m_nVerb )
		{
		case HTTP_VERB_GET:
		case HTTP_VERB_HEAD:
			break;
		case HTTP_VERB_POST:
		case HTTP_VERB_PUT:
			strSend += m_strData;
			break;
		default:
			break;
		}

		//发送请求
		nRet = 	m_cBufSocket.Send((LPCTSTR)strSend,strSend.GetLength(),m_dwSendTimeout);
		if( nRet < strSend.GetLength() )
		{
			if ( m_cBufSocket.GetLastError() == WSAETIMEDOUT)	// 超时
				continue;
			else	// 其他错误，可能是网络断了，等待一段时间后重试
				return HTTP_REQUEST_ERROR;
		}

		// 接收HTTP头
		BOOL bAgain = TRUE;
		while( bAgain )
		{
			bAgain = FALSE;
			strHeader.Empty();
			while( TRUE )
			{
				Check(m_bStopDownload,HTTP_REQUEST_STOP);
				ZeroMemory(szReadBuf,1025);
				nRet = m_cBufSocket.BSDGetString(szReadBuf,1024,m_dwRecvTimeout);

				//seawind
				Check(m_bStopDownload,HTTP_REQUEST_STOP);

				Check( nRet == SOCKET_FAIL ,HTTP_REQUEST_ERROR);
				
				if( szReadBuf[0] == '\0' ) // We have encountered "\r\n\r\n"
					break; 
				
				strHeader += szReadBuf;
				if( nRet == SOCKET_SUCCESS )
					strHeader += "\r\n";
			}
			
			///////////////////////////////////////
			// 目前的版本中，此信息并没有用
			m_strHeader		= strHeader;
			m_dwHeaderSize	= m_strHeader.GetLength();
			//////////////////////////////////////
			nRet = GetInfo(strHeader,dwContentLength,dwStatusCode,m_TimeLastModified,strCookie,lpszFtpURL);
			switch ( nRet )
			{
			case HTTP_FAIL:
				return HTTP_REQUEST_FAIL;
				break;
			case HTTP_REDIRECT:
				m_nVerb = HTTP_VERB_GET;
				continue;
				break;
			case HTTP_REDIRECT_FTP:
				return HTTP_REQUEST_REDIRECT_FTP;
				break;
			case HTTP_SUCCESS:
				
				if( dwStatusCode >= 100  && dwStatusCode <200 )
				{
					bAgain = TRUE;
					continue;	// Inner Loop
				}
				
				if( m_bSaveResponse )
				{
					m_dwDownloadSize = dwContentLength;
					// 应该判断一下服务器是否支持断点续传
					if( strRange.IsEmpty() )
					{
						m_dwFileSize = dwContentLength; // 整个文件的长度
						
						//seawind
						m_bSupportResume = FALSE;
					}
					else
					{
						if ( dwStatusCode == 206 )	//支持断点续传
						{
							m_dwFileSize = (dwContentLength == 0 )?0:(m_dwFileDownloadedSize +dwContentLength);
							m_bSupportResume = TRUE;
						}
						else						//不支持断点续传
						{
							m_bSupportResume = FALSE;
							m_dwFileDownloadedSize	= 0; //不支持断点续传，此值要设为0
							m_dwFileSize = dwContentLength;
						}
					}// strRange
				}// m_bSaveResponse
				return HTTP_REQUEST_SUCCESS;
				break;
			default:
				return HTTP_REQUEST_FAIL;
				break;
			}// SWITCH
			
		} // WHILE bAgain
		
	}// WHILE LOOP

	return HTTP_REQUEST_SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetProxy(
//				LPCTSTR lpszProxyServer,
//				INT nProxyPort,
//				BOOL bProxy/*=TRUE*/,
//				BOOL bProxyAuthorization /*= FALSE*/,
//				LPCTSTR lpszProxyUsername /*= NULL*/,
//				LPCTSTR lpszProxyPassword /*= NULL*/,
//				INT nProxyType /*= HTTP_PROXY_HTTPGET*/ ) 
//	用  途：设置代理及代理认证方式
//	对全局变量的影响：无
//	参  数：
//		lpszProxyServer     : 代理服务器的地址(IP地址或者域名)
//		nProxyPort          : 代理服务器的端口
//		bProxy              : 是否使用代理(该值为FALSE的时候，前两个参数将被忽略)
//		bProxyAuthorization : 代理是否需要验证
//		lpszProxyUsername   : 代理服务器验证的用户名
//		lpszProxyPassword   : 代理服务器验证的口令
//		nProxyType          : 代理服务器的类型
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetProxy(LPCTSTR lpszProxyServer,INT nProxyPort,BOOL bProxy/*=TRUE*/,BOOL bProxyAuthorization /*= FALSE*/,LPCTSTR lpszProxyUsername /*= NULL*/,LPCTSTR lpszProxyPassword /*= NULL*/,INT nProxyType /*= HTTP_PROXY_HTTPGET*/)
{
	if( bProxy && nProxyType == HTTP_PROXY_USEIE )	// 使用IE的Proxy设置
	{
		if( !GetIEProxy(m_strProxyServer,m_nProxyPort,m_nProxyType) )
		{
			m_bProxy				= FALSE;
			m_bProxyAuthorization	= FALSE;
			m_nProxyPort			= 0;
			m_nProxyType			= HTTP_PROXY_NONE;
			m_strProxyServer		= _T("");
			m_strProxyUsername		= _T("");
			m_strProxyPassword		= _T("");
		}
		else
		{
			m_bProxy				= TRUE;
			if( lpszProxyUsername != NULL )
			{
				m_bProxyAuthorization	= TRUE;
				m_strProxyUsername		= lpszProxyUsername;
				m_strProxyPassword		= lpszProxyPassword;
			}
			else
			{
				m_bProxyAuthorization	= FALSE;
				m_strProxyUsername		= _T("");
				m_strProxyPassword		= _T("");
			}
			
		}
		return;	// 返回
	}

	if( bProxy && lpszProxyServer != NULL)
	{
		m_bProxy			= TRUE;
		m_strProxyServer	= lpszProxyServer;
		m_nProxyPort		= nProxyPort;
		m_nProxyType		= nProxyType;

		if( bProxyAuthorization && lpszProxyUsername != NULL)
		{
			m_bProxyAuthorization	= TRUE;
			m_strProxyUsername		= lpszProxyUsername;
			m_strProxyPassword		= lpszProxyPassword;
		}
		else
		{
			m_bProxyAuthorization	= FALSE;
			m_strProxyUsername		= _T("");
			m_strProxyPassword		= _T("");
		}
	}
	else
	{
		m_bProxy				= FALSE;
		m_bProxyAuthorization	= FALSE;
		m_nProxyPort			= 0;
		m_nProxyType			= HTTP_PROXY_NONE;
		m_strProxyServer		= _T("");
		m_strProxyUsername		= _T("");
		m_strProxyPassword		= _T("");
	}
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetAuthorization(
//				LPCTSTR lpszUsername,
//				LPCTSTR lpszPassword,
//				BOOL bAuthorization/* = TRUE*/ ) 
//	用  途：设置WWW认证信息(访问被保护的页面时需要)
//	对全局变量的影响：无
//	参  数：
//		lpszUsername   : 访问页面的用户名
//		lpszPassword   : 口令
//		bAuthorization : 是否需要验证(此值为FALSE时，前两个参数将被忽略)
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization/* = TRUE*/ )
{
	if( bAuthorization && lpszUsername != NULL )
	{
		m_bAuthorization = TRUE;
		m_strUsername	 = lpszUsername;
		m_strPassword	 = lpszPassword;
	}
	else
	{
		m_bAuthorization = FALSE;
		m_strUsername	 = _T("");
		m_strPassword	 = _T("");
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetNotifyWnd(
//				HWND hNotifyWnd,
//				INT nNotifyMsg,
//				BOOL bNotify /*= TRUE */ ) 
//	用  途：设置是否需要发送消息给调用窗口
//	对全局变量的影响：无
//	参  数：
//		hNotifyWnd : 接收消息的窗口
//		nNotifyMsg : 消息ID
//		bNotify    : 是否发送消息(此值为FALSE时，前两个参数将被忽略)
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetNotifyWnd(HWND hNotifyWnd, INT nNotifyMsg, BOOL bNotify /*= TRUE */)
{
	if( bNotify && (hNotifyWnd != NULL) && ::IsWindow(hNotifyWnd) )
	{
		m_bNotify			= TRUE;
		m_hNotifyWnd		= hNotifyWnd;
		m_nNotifyMessage	= nNotifyMsg;
	}
	else
	{
		m_bNotify			= FALSE;
		m_hNotifyWnd		= NULL;
		m_nNotifyMessage	= 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetTimeout(
//				DWORD dwSendTimeout,
//				DWORD dwRecvTimeout,
//				DWORD dwConnTimeout ) 
//	用  途：设置超时
//	对全局变量的影响：无
//	参  数：
//		dwSendTimeout : 发送超时
//		dwRecvTimeout : 接收超时
//		dwConnTimeout : 连接超时
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetTimeout(DWORD dwSendTimeout, DWORD dwRecvTimeout, DWORD dwConnTimeout)
{
	if( dwSendTimeout > 0 )
		m_dwSendTimeout = dwSendTimeout;
	
	if( dwRecvTimeout > 0 )
		m_dwRecvTimeout = dwRecvTimeout;

	if( dwConnTimeout > 0 )
		m_dwConnTimeout = dwConnTimeout;


}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetUserAgent( LPCTSTR lpszUserAgent ) 
//	用  途：设置UserAgent
//	对全局变量的影响：无
//	参  数：
//		lpszUserAgent : 新的UserAgent(例如：Mozilla/4.0 (compatible; MSIE 5.0; Windows 98) )
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetUserAgent(LPCTSTR lpszUserAgent)
{
	if( lpszUserAgent == NULL )
		m_strUserAgent = DEFAULT_USERAGENT;
	else
		m_strUserAgent = lpszUserAgent;	

	if( m_strUserAgent.IsEmpty())
		m_strUserAgent = DEFAULT_USERAGENT;
	
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetReferer( LPCTSTR lpszReferer ) 
//	用  途：设置Referer(提交者)
//	对全局变量的影响：无
//	参  数：
//		lpszReferer :  新的Referer
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetReferer(LPCTSTR lpszReferer)
{
	if( lpszReferer != NULL )
		m_strReferer = lpszReferer;
	else
		m_strReferer = _T("");
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID SetRetry(
//				INT nRetryType,
//				INT nRetryDelay,
//				INT nRetryMax ) 
//	用  途：设置重试的机制
//	对全局变量的影响：无
//	参  数：
//		nRetryType  :  重试方式
//			HTTP_RETRY_NONE  - 不重试
//			HTTP_RETRY_TIMES - 重试一定次数
//			HTTP_RETRY_ALWAYS- 总是重试(可能陷入死循环)
//		nRetryDelay : 重试延迟
//		nRetryMax   : 最大重试次数(nRetryType为HTTP_RETRY_TIMES时，此值才有意义)
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetRetry(INT nRetryType, INT nRetryDelay, INT nRetryMax)
{
	m_nRetryType  = nRetryType;
	m_nRetryDelay = nRetryDelay;
	m_nRetryMax	  = nRetryMax;
	
	// 检查一下m_nRetryMax,如果为0，设为缺省值
	//if( (HTTP_RETRY_TIMES == m_nRetryType) && (0 == m_nRetryMax) )
	//	m_nRetryMax = DEFAULT_HTTP_RETRY_MAX;
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetDownloadFileStatus(
//				IN LPCTSTR lpszDownloadUrl,
//				OUT DWORD &dwFileSize,
//				OUT CTime &FileTime ) 
//	用  途：获取下载文件的状态
//	对全局变量的影响：无
//	参  数：
//		lpszDownloadUrl : 文件的URL
//		dwFileSize      : 用于保存文件的大小
//		FileTime        : 用于保存文件的修改时间
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::GetDownloadFileStatus(IN LPCTSTR lpszDownloadUrl,OUT DWORD &dwFileSize, OUT CTime &FileTime)
{
	// 检验要下载的URL是否为空
	m_strDownloadUrl = lpszDownloadUrl;
	m_strDownloadUrl.TrimLeft();
	m_strDownloadUrl.TrimRight();
	if( m_strDownloadUrl.IsEmpty() )
		return FALSE;

	// 检验要下载的URL是否有效
	if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort))
	{
		// 在前面加上"http://"再试
		m_strDownloadUrl = _T("http://") + m_strDownloadUrl;
		if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort) )
		{
			//TRACE1(_T("Failed to parse the URL: %s\n"), m_strDownloadUrl);
			return FALSE;
		}
	}

	m_strSavePath		= "|";
	m_strTempSavePath	= "|";
	m_bStopDownload		= FALSE;

	if ( SendRequest() !=  HTTP_REQUEST_SUCCESS )
		return FALSE;

	dwFileSize  = m_dwDownloadSize;
	FileTime	= m_TimeLastModified;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL ParseURL(
//				IN LPCTSTR lpszURL,
//				OUT CString &strServer,
//				OUT CString &strObject,
//				OUT INT& nPort ) 
//	用  途：从URL里面拆分出Server和Object来
//	对全局变量的影响：无
//	参  数：
//		lpszURL   : 待拆分的URL
//		strServer : 服务器地址
//		strObject : 服务器上的目标文件
//		nPort     : 服务器的端口
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::ParseURL(IN LPCTSTR lpszURL, OUT CString &strServer, OUT CString &strObject,OUT INT& nPort)
{
	CString strURL(lpszURL);
	strURL.TrimLeft();
	strURL.TrimRight();
	
	// 清除数据
	strServer = _T("");
	strObject = _T("");
	nPort	  = 0;

	INT nPos = strURL.Find("://");
	if( nPos == -1 )
		return FALSE;

	// 进一步验证是否为http://
	CString strTemp = strURL.Left( nPos+lstrlen("://") );
	strTemp.MakeLower();
	if( strTemp.Compare("http://") != 0 )
		return FALSE;

	strURL = strURL.Mid( strTemp.GetLength() );
	nPos = strURL.Find('/');
	if ( nPos == -1 )
		return FALSE;

	strObject = strURL.Mid(nPos);
	strTemp   = strURL.Left(nPos);
	
	///////////////////////////////////////////////////////////////
	/// 注意：并没有考虑URL中有用户名和口令的情形和最后有#的情形
	/// 例如：http://abc@def:www.yahoo.com:81/index.html#link1
	/// 
	//////////////////////////////////////////////////////////////

	// 查找是否有端口号
	nPos = strTemp.Find(":");
	if( nPos == -1 )
	{
		strServer = strTemp;
		nPort	  = DEFAULT_HTTP_PORT;
	}
	else
	{
		strServer = strTemp.Left( nPos );
		strTemp	  = strTemp.Mid( nPos+1 );
		nPort	  = (INT)_ttoi((LPCTSTR)strTemp);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT GetInfo(
//				IN LPCTSTR lpszHeader,
//				OUT DWORD& dwContentLength,
//				OUT DWORD& dwStatusCode,
//				OUT CTime& TimeLastModified,
//				OUT CString& strCookie,
//				LPTSTR lpszFtpURL ) 
//	用  途：从返回的HTTP协议头里获得必要的信息
//	对全局变量的影响：无
//	参  数：
//		lpszHeader       : HTTP协议头
//		dwContentLength  : content-length字段的值
//		dwStatusCode     : 状态码
//		TimeLastModified : last-modified字段的值
//		strCookie        : 用来保存Cookie
//		lpszFtpURL       : 用来保存重定向到FTP的URL
//	返回值：INT
//		HTTP_FAIL		: 不合法的HTTP协议头，失败
//		HTTP_REDIRECT	: 重定向
//		HTTP_SUCCESS	: 成功
////////////////////////////////////////////////////////////////////////////////
INT CHttpDownload::GetInfo(IN LPCTSTR lpszHeader,OUT DWORD& dwContentLength,OUT DWORD& dwStatusCode,OUT CTime& TimeLastModified,OUT CString& strCookie,LPTSTR lpszFtpURL)
{
	dwContentLength = 0;
	dwStatusCode	= 0;
	TimeLastModified= CTime::GetCurrentTime();
	strCookie		= _T("");

	CString strHeader(lpszHeader);
	strHeader.MakeLower();

	//拆分出HTTP应答的头信息的第一行
	INT nPos = strHeader.Find("\r\n");
	if (nPos == -1)
		return HTTP_FAIL;

	// 获得返回码: Status Code
	CString strStatusLine = strHeader.Left(nPos);
	strStatusLine.TrimLeft();
	strStatusLine.TrimRight();

	nPos = strStatusLine.Find(' ');
	if( nPos == -1 )
		return HTTP_FAIL;
	strStatusLine = strStatusLine.Mid(nPos+1);
	nPos = strStatusLine.Find(' ');
	if( nPos == -1 )
		return HTTP_FAIL;
	strStatusLine = strStatusLine.Left(nPos);
	dwStatusCode = (DWORD)_ttoi((LPCTSTR)strStatusLine);

	// 检查返回码
	if( dwStatusCode >= 300 && dwStatusCode < 400 ) //首先检测一下服务器的应答是否为重定向
	{
		nPos = strHeader.Find("location:");
		if (nPos == -1)
			return HTTP_FAIL;

		CString strRedirectURL = strHeader.Mid(nPos + strlen("location:"));
		nPos = strRedirectURL.Find("\r\n");
		if (nPos == -1)
			return HTTP_FAIL;

		strRedirectURL = strRedirectURL.Left(nPos);
		strRedirectURL.TrimLeft();
		strRedirectURL.TrimRight();
		
		// 设置Referer
		m_strReferer = m_strDownloadUrl;

		// 判断是否重定向到其他的服务器
		// 从版本1.7开始，支持重定向到FTP服务器
		if( strRedirectURL.Mid(0,6) == _T("ftp://") )
		{
			if( lpszFtpURL != NULL )
				memcpy( lpszFtpURL,(LPCTSTR)strRedirectURL,strRedirectURL.GetLength() );

			return HTTP_REDIRECT_FTP;
		}

		if( strRedirectURL.Mid(0,7) == _T("http://") )
		{
			m_strDownloadUrl = strRedirectURL;
			// 检验要下载的URL是否有效
			if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort))
				return HTTP_FAIL;
			return HTTP_REDIRECT;
		}

		// 重定向到本服务器的其他地方
		strRedirectURL.Replace("\\","/");
		
		// 是相对于根目录
		if( strRedirectURL[0] == '/' )
		{
			m_strObject = strRedirectURL;
			return HTTP_REDIRECT;
		}
		
		// 是相对当前目录
		INT nParentDirCount = 0;
		nPos = strRedirectURL.Find("../");
		while (nPos != -1)
		{
			strRedirectURL = strRedirectURL.Mid(nPos+3);
			nParentDirCount++;
			nPos = strRedirectURL.Find("../");
		}
		for (INT i=0; i<=nParentDirCount; i++)
		{
			nPos = m_strDownloadUrl.ReverseFind('/');
			if (nPos != -1)
				m_strDownloadUrl = m_strDownloadUrl.Left(nPos);
		}
		m_strDownloadUrl = m_strDownloadUrl+"/"+strRedirectURL;

		if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort))
			return HTTP_FAIL;
		return HTTP_REDIRECT;
	}

	// 发生错误，不用重试(不管是服务器或者是客户端的)
	if( dwStatusCode >=400 )
		return HTTP_FAIL;
		
	// 获取ContentLength(如果没有，就返回0，表示长度未知)
	nPos = strHeader.Find("content-length:");
	if( nPos != -1 )
	{
		CString strConentLen = strHeader.Mid(nPos + strlen("content-length:"));	
		nPos = strConentLen.Find("\r\n");
		if (nPos != -1)
		{
			strConentLen = strConentLen.Left(nPos);	
			strConentLen.TrimLeft();
			strConentLen.TrimRight();

			// Content-Length:
			dwContentLength = (DWORD) _ttoi( (LPCTSTR)strConentLen );
		}
	}

	// 获取Last-Modified:
	nPos = strHeader.Find("last-modified:");
	if (nPos != -1)
	{
		CString strTime = strHeader.Mid(nPos + strlen("last-modified:"));
		nPos = strTime.Find("\r\n");
		if (nPos != -1)
		{
			strTime = strTime.Left(nPos);
			strTime.TrimLeft();
			strTime.TrimRight();
			TimeLastModified = GetTime(strTime);
		}
	}

	// 获取Cookie
	nPos = strHeader.Find("set-cookie:");
	if (nPos != -1)
	{
		CString strText = strHeader.Mid(nPos + strlen("set-cookie:"));
		nPos = strText.Find("\r\n");
		if (nPos != -1)
		{
			strCookie = strText.Left(nPos);
			strCookie.TrimLeft();
			strCookie.TrimRight();
			
			INT nPos1 = strCookie.Find("expires=");
			INT nPos2 = strCookie.Find("domain=");
			INT nPos3 = strCookie.Find("path=");
			
			nPos = -1;
			if( nPos1 != -1 )
				nPos = nPos1;

			if( nPos2 != -1 )
				nPos = ( nPos == -1 )? nPos2:((nPos > nPos2 )?nPos2:nPos);

			if( nPos3 != -1 )
				nPos = ( nPos == -1 )? nPos3:((nPos > nPos3 )?nPos3:nPos);
				
			if( nPos != -1 )
				strCookie = strCookie.Left( nPos );

			// 删除最后的;
			if( strCookie.Right(1) == ";" )
				strCookie = strCookie.Left( strCookie.GetLength() -1 );
		}
			
	}
	return HTTP_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：CTime GetTime( LPCTSTR lpszTime ) 
//	用  途：将GMT字符串转化成时间
//	对全局变量的影响：无
//	参  数：
//		lpszTime : 时间字符串
//	返回值：CTime
////////////////////////////////////////////////////////////////////////////////
CTime CHttpDownload::GetTime(LPCTSTR lpszTime)
{
	INT nDay,nMonth,nYear,nHour,nMinute,nSecond;

	CString strTime = lpszTime;
	INT nPos = strTime.Find(',');
	if (nPos != -1)
	{
		strTime = strTime.Mid(nPos+1);
		strTime.TrimLeft();

		CString strDay,strMonth,strYear,strHour,strMinute,strSecond;
		CString strAllMonth = "jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec";
		strDay = strTime.Left(2);
		nDay = atoi(strDay);
		strMonth = strTime.Mid(3,3);
		strMonth.MakeLower();
		nPos = strAllMonth.Find(strMonth);
		if (nPos != -1)
		{
			strMonth.Format("%d",((nPos/4)+1));
			nMonth = atoi(strMonth);
		}
		else
			nMonth = 1;
		strTime = strTime.Mid(6);
		strTime.TrimLeft();
		nPos = strTime.FindOneOf(" \t");
		if (nPos != -1)
		{
			strYear = strTime.Left(nPos);
			nYear = atoi(strYear);
		}
		else
			nYear = 2000;
		strTime = strTime.Mid(nPos+1);
		strHour = strTime.Left(2);
		nHour = atoi(strHour);
		strMinute = strTime.Mid(3,2);
		nMinute = atoi(strMinute);
		strSecond = strTime.Mid(6,2);
		nSecond = atoi(strSecond);
	}
	
	CTime time(nYear,nMonth,nDay,nHour,nMinute,nSecond);
	return time;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID GetFileName()
//	用  途：从下载URL中获取文件名
//	对全局变量的影响：无
//	参  数：无
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::GetFileName()
{
	// 获取的文件名
	INT nSlash = m_strObject.ReverseFind(_T('/'));
	if (nSlash == -1)
		nSlash = m_strObject.ReverseFind(_T('\\'));

	if (nSlash != -1 && m_strObject.GetLength() > 1)
		m_strFileName = m_strObject.Right(m_strObject.GetLength() - nSlash - 1);
	else
		m_strFileName = m_strObject;

	m_strFileName.TrimLeft();
	m_strFileName.TrimRight();
	// 文件名为空或无效
	if( m_strFileName.IsEmpty() || !IsValidFileName(m_strFileName) )
		m_strFileName = DEFAULT_SAVE_FILE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID StopDownload()
//	用  途：终止下载 
//	对全局变量的影响：无
//	参  数：无
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::StopDownload()
{
	m_bStopDownload = TRUE;

	if ( m_hStopEvent != NULL )
		SetEvent(m_hStopEvent);
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT Base64Decode(
//				IN LPCTSTR lpszDecoding,
//				OUT CString &strDecoded ) 
//	用  途：BASE64解码
//	对全局变量的影响：无
//	参  数：
//		lpszDecoding : 待解码字符串
//		strDecoded   : 解码结果
//	返回值：INT，解码后的长度
////////////////////////////////////////////////////////////////////////////////
INT CHttpDownload::Base64Decode(IN LPCTSTR lpszDecoding, OUT CString &strDecoded)
{
	INT nIndex =0;
	INT nDigit;
    INT nDecode[ 256 ];
	INT nSize;
	INT nNumBits = 6;
	INT i = 0;

	if( lpszDecoding == NULL )
		return 0;
	
	if( ( nSize = lstrlen(lpszDecoding) ) == 0 )
		return 0;

	// Build Decode Table
	for( i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for( i=0; i < 64; i++ )
	{
		nDecode[ m_strBase64TAB[ i ] ] = i;
		nDecode[ '=' ] = -1; 
    }

	// Clear the output buffer
	strDecoded = _T("");
	INT lBitsStorage  =0;
	INT nBitsRemaining = 0;
	INT nScratch = 0;
	UCHAR c;
	
	// Decode the Input
	for( nIndex = 0, i = 0; nIndex < nSize; nIndex++ )
	{
		c = lpszDecoding[ nIndex ];

		// 忽略所有不合法的字符
		if( c> 0x7F)
			continue;

		nDigit = nDecode[c];
		if( nDigit >= 0 ) 
		{
			lBitsStorage = (lBitsStorage << nNumBits) | (nDigit & 0x3F);
			nBitsRemaining += nNumBits;
			while( nBitsRemaining > 7 ) 
			{
				nScratch = lBitsStorage >> (nBitsRemaining - 8);
				strDecoded += (CHAR)(nScratch & 0xFF);
				i++;
				nBitsRemaining -= 8;
			}
		}
    }	

	return strDecoded.GetLength();
}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT Base64Encode(
//				IN LPCTSTR lpszEncoding,
//				OUT CString &strEncoded ) 
//	用  途：BASE64编码
//	对全局变量的影响：无
//	参  数：
//		lpszEncoding : 待编码的字符串
//		strEncoded   : 编码结果
//	返回值：INT，编码后的长度
////////////////////////////////////////////////////////////////////////////////
INT CHttpDownload::Base64Encode(IN LPCTSTR lpszEncoding, OUT CString &strEncoded)
{
	INT nDigit;
	INT nNumBits = 6;
	INT nIndex = 0;
	INT nInputSize;

	strEncoded = _T( "" );
	if( lpszEncoding == NULL )
		return 0;

	if( ( nInputSize = lstrlen(lpszEncoding) ) == 0 )
		return 0;

	INT nBitsRemaining = 0;
	INT lBitsStorage	=0;
	INT lScratch		=0;
	INT nBits;
	UCHAR c;

	while( nNumBits > 0 )
	{
		while( ( nBitsRemaining < nNumBits ) &&  ( nIndex < nInputSize ) ) 
		{
			c = lpszEncoding[ nIndex++ ];
			lBitsStorage <<= 8;
			lBitsStorage |= (c & 0xff);
			nBitsRemaining += 8;
		}
		if( nBitsRemaining < nNumBits ) 
		{
			lScratch = lBitsStorage << ( nNumBits - nBitsRemaining );
			nBits    = nBitsRemaining;
			nBitsRemaining = 0;
		}	 
		else 
		{
			lScratch = lBitsStorage >> ( nBitsRemaining - nNumBits );
			nBits	 = nNumBits;
			nBitsRemaining -= nNumBits;
		}
		nDigit = (INT)(lScratch & m_nBase64Mask[nNumBits]);
		nNumBits = nBits;
		if( nNumBits <=0 )
			break;
		
		strEncoded += m_strBase64TAB[ nDigit ];
	}
	// Pad with '=' as per RFC 1521
	while( strEncoded.GetLength() % 4 != 0 )
		strEncoded += '=';

	return strEncoded.GetLength();

}


//////////////////////////////////////////////////////////////////////////////////
//	函数名：INT MakeConnection(
//				CBufSocket* pBufSocket,
//				CString strServer,
//				INT nPort ) 
//	用  途：建立连接
//	对全局变量的影响：无
//	参  数：
//		pBufSocket : CBufSocket类指针
//		strServer  : 待连接服务器
//		nPort      : 端口
//	返回值：INT
////////////////////////////////////////////////////////////////////////////////
INT CHttpDownload::MakeConnection(CBufSocket* pBufSocket,CString strServer,INT nPort)
{
	CSocksPacket	cSocks( pBufSocket );
	DWORD			dwIP;
	BYTE			byAuth,byAtyp;
	SOCKSREPPACKET	pack;
	CString			strAuth,strAddr;

	strAuth = _T("");
	strAddr = _T("");

	switch( m_nProxyType )
	{
	case HTTP_PROXY_NONE:
		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(strServer,nPort,m_dwConnTimeout,TRUE),HTTP_REQUEST_ERROR);
		break;
	case HTTP_PROXY_HTTPGET:
		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),HTTP_REQUEST_ERROR);
		break;
	case HTTP_PROXY_SOCKS4A:
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		if( dwIP == INADDR_NONE )
		{
			Check( m_bStopDownload,HTTP_REQUEST_STOP);
			Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),HTTP_REQUEST_ERROR);
			Check( m_bStopDownload,HTTP_REQUEST_STOP);
			Check( !cSocks.SendSocks4aReq(CMD_CONNECT,nPort,strServer,m_strProxyUsername,m_dwSendTimeout),HTTP_REQUEST_ERROR);

			Check( m_bStopDownload,HTTP_REQUEST_STOP);
			Check( !cSocks.RecvPacket(PACKET_SOCKS4AREP,m_dwRecvTimeout),HTTP_REQUEST_ERROR);
			Check( !cSocks.IsSocksOK(),HTTP_REQUEST_FAIL);	// 请求有错误，重试可能是没有用的
			break;// NOTICE:如果本地能够解析域名，可以使用SOCKS4 Proxy
		}
	case HTTP_PROXY_SOCKS4:
		// 必须要得到Proxy Server的IP地址(不能为域名)
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		Check( dwIP == INADDR_NONE,HTTP_REQUEST_ERROR);

		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),HTTP_REQUEST_ERROR);
		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !cSocks.SendSocks4Req(CMD_CONNECT,nPort,dwIP,m_strProxyUsername,m_dwSendTimeout),HTTP_REQUEST_ERROR);

		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !cSocks.RecvPacket(PACKET_SOCKS4REP,m_dwRecvTimeout),HTTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(),HTTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的
		break;

	case HTTP_PROXY_SOCKS5:

		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),HTTP_REQUEST_ERROR);

		if( m_bProxyAuthorization )
		{
			CHAR ch	= (CHAR)AUTH_NONE;
			strAuth += ch;
			ch 		= (CHAR)AUTH_PASSWD;
			strAuth += ch;
		}
		else
		{
			CHAR ch	= (CHAR)AUTH_NONE;
			strAuth += ch;
		}
		byAuth =(BYTE)strAuth.GetLength();
			
		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !cSocks.SendSocks5AuthReq(byAuth,(LPCTSTR)strAuth,m_dwSendTimeout),HTTP_REQUEST_ERROR);
		
		Check( m_bStopDownload,HTTP_REQUEST_STOP);

		ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
		Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS5AUTHREP,m_dwRecvTimeout),HTTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS5AUTHREP),HTTP_REQUEST_FAIL);	// 请求有错误，重试可能是没有用的

		switch( pack.socks5AuthRep.byAuth )
		{
		case AUTH_NONE:
			break;
		case AUTH_PASSWD:
			
			Check( !m_bProxyAuthorization,HTTP_REQUEST_FAIL);
			Check( m_bStopDownload,HTTP_REQUEST_STOP);
			Check( !cSocks.SendSocks5AuthPasswdReq(m_strProxyUsername,m_strProxyPassword,m_dwSendTimeout),HTTP_REQUEST_ERROR);
			Check( m_bStopDownload,HTTP_REQUEST_STOP);
			
			Check( !cSocks.RecvPacket(PACKET_SOCKS5AUTHPASSWDREP,m_dwRecvTimeout),HTTP_REQUEST_ERROR);
			Check( !cSocks.IsSocksOK(),HTTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的
			break;
		case AUTH_GSSAPI:
		case AUTH_CHAP:
		case AUTH_UNKNOWN:
		default:
			return HTTP_REQUEST_FAIL;
			break;
		}
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		if( dwIP != INADDR_NONE )
		{
			byAtyp = ATYP_IPV4ADDR;
			//不用转换字节序
			strAddr += (CHAR)( (dwIP    ) &0x000000ff) ; 
			strAddr += (CHAR)( (dwIP>>8 ) &0x000000ff); 
			strAddr += (CHAR)( (dwIP>>16) &0x000000ff); 
			strAddr += (CHAR)( (dwIP>>24) &0x000000ff); 

		}
		else
		{
			byAtyp = ATYP_HOSTNAME;
			CHAR ch = (CHAR)strServer.GetLength();
			strAddr += ch;
			strAddr += strServer;
		}

		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !cSocks.SendSocks5Req(CMD_CONNECT,byAtyp,strAddr,nPort,m_dwSendTimeout),HTTP_REQUEST_ERROR);
		Check( m_bStopDownload,HTTP_REQUEST_STOP);
		Check( !cSocks.RecvPacket(PACKET_SOCKS5REP,m_dwRecvTimeout),HTTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(),HTTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的
		break;
	case HTTP_PROXY_HTTPCONNECT:
	default:
		return HTTP_REQUEST_FAIL;
		break;
	}
	return HTTP_REQUEST_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetIEProxy(
//				CString &strProxyServer,
//				INT &nProxyPort,
//				INT &nProxyType ) 
//	用  途：获取IE的Proxy设置
//	对全局变量的影响：无
//	参  数：
//		strProxyServer : 代理服务器地址
//		nProxyPort     : 代理服务器的端口
//		nProxyType     : 代理服务器的类型
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::GetIEProxy(CString &strProxyServer, INT &nProxyPort, INT &nProxyType)
{
	// 读取注册表
	HKEY hRegKey;
	DWORD dwRet = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
		0L,
		KEY_QUERY_VALUE,&hRegKey);
	if (dwRet != ERROR_SUCCESS)
		return FALSE;
	
	TCHAR szAddr[256]	= { 0 };
	DWORD dwLen			= 256;
	dwRet = RegQueryValueEx(hRegKey,"ProxyServer",NULL,NULL,(LPBYTE)szAddr,&dwLen);
	if (dwRet != ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		return FALSE;
	}
    RegCloseKey(hRegKey);
    
    // 注：可能存在所有代理同一的情况，此时无 http=...ftp=...信息
    //    comment by Linsuyi, 2002/01/22  23:30
    // 
    // 分析Proxy的设置
    //http=193.168.10.1:80;socks=192.168.10.235:1080
    //193.168.10.1:1090
    //ftp=193.168.10.1:1090;gopher=193.168.10.1:1090;https=193.168.10.1:1090;socks=193.168.10.2:1080
    INT nPos = -1;
    CString strProxy = szAddr;
    
    if (strProxy.IsEmpty())
        return FALSE;
    
    strProxy.MakeLower();
    nProxyType = HTTP_PROXY_HTTPGET;
    
    nPos = strProxy.Find(';');
    if (nPos != -1)
    {
        nPos = strProxy.Find("http=");        // HTTP GET Proxy
        if (nPos != -1)
            strProxy = strProxy.Mid(nPos+strlen("http="));
        else if ((nPos = strProxy.Find("socks=")) != -1) // SOCKS Proxy ( SOCKS5 )
        {
            strProxy = strProxy.Mid( nPos+strlen("socks="));
            nProxyType = HTTP_PROXY_SOCKS5;
        }
        nPos = strProxy.Find(";");
        if (nPos != -1)
    	    strProxy = strProxy.Left(nPos);
    }
    
    nPos = strProxy.Find(":");
    strProxyServer = strProxy.Left(nPos);
    nProxyPort = _ttoi(strProxy.Mid(nPos+1));
    if (nProxyPort <= 0)
        return FALSE;
    
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：VOID VOID SetCookie(
//				LPCTSTR lpszCookie,
//				BOOL bUseIECookie /*= FALSE*/ ) 
//	用  途：设置Cookie
//	对全局变量的影响：无
//	参  数：
//		lpszCookie   : Cookie值 
//		bUseIECookie : 是否使用IE的Cookie值
//	返回值：VOID
////////////////////////////////////////////////////////////////////////////////
VOID CHttpDownload::SetCookie(LPCTSTR lpszCookie,BOOL bUseIECookie /*= FALSE*/ )
{
	if( bUseIECookie )
	{
		m_bUseIECookie = TRUE;
/*
		CString strCookie;
		if( GetIECookie(strCookie,m_strDownloadUrl) )
			m_strCookie = strCookie;
		else
			m_strCookie = _T("");
*/

	}
	else
	{
		m_bUseIECookie = FALSE;

		if( lpszCookie != NULL)
			m_strCookie = lpszCookie;
		else
			m_strCookie = _T("");
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetIECookie(
//				CString& strCookie,
//				LPCTSTR lpszURL )
//	用  途：获取IE的Cookie设置
//	对全局变量的影响：无
//	参  数：
//		strCookie  : 用于保存Cookie
//		lpszURL    : 服务器URL
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::GetIECookie(CString& strCookie,LPCTSTR lpszURL )
{
/*
	Cookies Folder:
		HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\ 
		Cookies = ***

	The simpliest way to Get Cookie value is call :
		CInternetSession::GetCookie(***,***,strCookie);
	But this way need the Wininet.dll which i don't want to use,How can i do?
	OR: You can call :
		CInternetSession::GetCookie(***,***,strCookie);
	before you use HttpDownload,and then call SetCookie(***);
*/
	strCookie = _T("");
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL IsValidFileName( LPCTSTR lpszFileName ) 
//	用  途：判断文件名是否合法
//	对全局变量的影响：无
//	参  数：
//		lpszFileName : 文件名
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CHttpDownload::IsValidFileName(LPCTSTR lpszFileName)
{
/*
	Short and INT file names must not contain the following characters:
	\   ?   |   >  <   :  /  *  "
	In addition, SHORT file names must not contain the following characters:
	+  ,  ;  =  [  ] 
	Short file names may not include a space, although a INT file name may. 
	No space is allowed preceding the vertical bar (|) separator for the SHORT 
	file name/INT file name syntax. If a space exists after the separator, then 
	the INT file name must have a space at the beginning of the file name. No 
	full-path syntax is allowed.
*/
	CString strFileName(lpszFileName);
	if( strFileName.FindOneOf("\\?|></\"*:") != -1 )
		return FALSE;

	return TRUE;
}

//seawind
BOOL CHttpDownload::IsUserStop()
{
	ASSERT(m_hStopEvent != NULL);

	if (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
		return TRUE;
	else
		return FALSE;
}

BOOL CHttpDownload::InitSocket( INT nMajorVer /*= 2*/ ,INT nMinorVer /*= 2*/ )
{
	return m_cBufSocket.InitLibrary(nMajorVer, nMinorVer);
}

VOID CHttpDownload::UnInitSocket()
{
	return m_cBufSocket.CleanupLibrary();
}