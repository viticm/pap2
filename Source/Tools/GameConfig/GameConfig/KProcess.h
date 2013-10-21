#ifndef _KPROCESS_H_
#define _KPROCESS_H_

#pragma comment(lib, "Psapi.lib")

class KProcess
{
public:
    KProcess(void);
    ~KProcess(void);

    int FindProcess(const TCHAR cszExeFilePath[], int* nNum);
};

#endif //_KPROCESS_H_