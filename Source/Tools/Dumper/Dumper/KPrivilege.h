
#ifndef _KPRIVILEGE_H_
#define _KPRIVILEGE_H_

class KPrivilege
{
public:
    KPrivilege();
    ~KPrivilege();

    int EnablePrivilege(TCHAR szPrivilege[]);
    int RestorePrivilege();

private:
    HANDLE m_hToken;
    TOKEN_PRIVILEGES m_PreviousState;
};

#endif //_KPRIVILEGE_H_