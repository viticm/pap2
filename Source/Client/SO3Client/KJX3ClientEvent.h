#ifndef KJX3CLIENTEVENT_H
#define KJX3CLIENTEVENT_H

struct LoginResult;

int FireJX3SdoaEvent(int nErrorCode, LoginResult const* pLoginResult, int nUserData, int nReserved);
int FireJX3SdoaInitFinishedEvent();

#endif // KJX3CLIENTEVENT_H
