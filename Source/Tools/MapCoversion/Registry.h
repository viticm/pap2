// Registry.h

class CRegistry
{

public:
	CRegistry();
	CRegistry( HKEY, const char * );
	~CRegistry();

	BOOL Open( HKEY, const char * );
	BOOL Close( void );
	BOOL IsOpen( void );

	BOOL ReadDWORD( const char *, DWORD *, DWORD *pdwLastError = NULL );
	BOOL ReadString( const char *, LPVOID, int, DWORD *pdwLastError = NULL );
	char **ReadMultipleStrings( const char *, DWORD *pdwLastError = NULL );
	static void DeleteList( char ** );

	BOOL WriteDWORD( const char *, DWORD, DWORD *pdwLastError = NULL );
	BOOL WriteString( const char *, LPVOID, DWORD *pdwLastError = NULL );
	BOOL Write( const char *, LPVOID, DWORD, int );

	static BOOL CreateKey( HKEY, const char *, const char * );
	static BOOL DeleteKey( HKEY, const char * );

protected:
	HKEY m_hKey;
	DWORD m_dwLastError;
	int m_nSize;

	BOOL Read( const char *, LPVOID, int );

};
