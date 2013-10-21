#ifndef _KPROCESS_H_
#define _KPROCESS_H_

class KProcess
{
public:
    KProcess(void);
    ~KProcess(void);

    //************************************
    // Method:    FindProcess
    // FullName:  KProcess::FindProcess
    // Access:    public static 
    // Returns:   int 成功 true，失败 false
    // Qualifier:
    // Parameter: const TCHAR cszExeFilePath[] 进程exe路径
    // Parameter: int * pnNumRunning 正在运行进程数量，如果没在运行，值为0
    //************************************
    static int FindProcess(const TCHAR cszExeFilePath[], int* pnNumRunning);
};

#endif //_KPROCESS_H_