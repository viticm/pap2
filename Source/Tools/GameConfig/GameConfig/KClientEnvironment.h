#ifndef _KCLIENTENVIRONMENT_H_
#define _KCLIENTENVIRONMENT_H_

#include "GameConfig.h"
#include <Wbemidl.h>

#define CPU_REG_KEY		HKEY_LOCAL_MACHINE
#define CPU_REG_SUBKEY	L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define CPU_NAME		L"ProcessorNameString"
#define CPU_IDENTIFIER	L"Identifier"


class KClientEnvironment : public CDialog
{
	DECLARE_DYNAMIC(KClientEnvironment)

public:
	KClientEnvironment(CWnd* pParent = NULL);   // standard constructor
	virtual ~KClientEnvironment();

// Dialog Data
	enum {IDD = IDD_INFORMATON_DLG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int Init();
private:
	int _SetCPUName();
	int _SetMemory();
	int _SetDisplayCard();
	int _SetOSVersion();
	int _SetBrowserName();
	int _SetIP();
	int _SetDxVersion();
	int _SetVideoRAM();

	int _GetDxdiagVersion(TCHAR szFileVersion[]);
	int _GetDxVersionInRegister(TCHAR szDxVersion[]);
	int _FindDxVersion(const TCHAR szDxVersion[], int& nID);

	HRESULT _GetEnumerator(IEnumWbemClassObject **ppEnumerator, char szEnumName[128]);
	IWbemLocator* m_pWbemLocator;
	IWbemServices* m_pWbemServices;
private:
	int m_nInited;

public:
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
};

#endif 