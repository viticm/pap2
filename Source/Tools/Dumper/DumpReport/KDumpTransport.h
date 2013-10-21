#ifndef _KDUMP_TRANSPORT_H_
#define _KDUMP_TRANSPORT_H_

class KDumpTransport
{
public:
	KDumpTransport(void);
    ~KDumpTransport(void);

    int ReportCrash(
		const TCHAR szServer[], 
        int nPort, 
		const TCHAR szDumpKey[], 
		const TCHAR szExceptionModuleName[],
		const TCHAR szProcessName[], 
		const TCHAR szFileVersion[],
        const TCHAR szFileVersionEx[],
        const TCHAR szAccount[],
        const TCHAR szRoleName[],
        const TCHAR szRegionName[],
        const TCHAR szServerName[],
        const TCHAR szVideoCard[], 
        const TCHAR szDriver[], 
        const TCHAR szDriverVersion[], 
        const TCHAR szCPUBrand[], 
        DWORD dwTotalPhysMem,
        const TCHAR szGUID[]
	);
	
    int ReportPlayerLoginBase(
        const TCHAR szServer[], 
        int nPort,         
        const TCHAR szFileVersion[],
        const TCHAR szFileVersionEx[],
        const TCHAR szVideoCard[], 
        const TCHAR szDriver[], 
        const TCHAR szDriverVersion[], 
        const TCHAR szCPUBrand[], 
        DWORD dwTotalPhysMem 
    );

	int TransportDumpFile(
		const TCHAR szServer[],
		int nPort,
		const TCHAR szDumpKey[], 
		const TCHAR szExceptionModuleName[],
		const TCHAR szProcessName[],
		const TCHAR szFileVersion[],
		const TCHAR szFilePathName[],
        const TCHAR szFileLevel[],
        const TCHAR szGUID[]
	);
};

#endif
