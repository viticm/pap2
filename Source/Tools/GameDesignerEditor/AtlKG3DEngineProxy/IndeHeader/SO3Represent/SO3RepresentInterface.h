#ifndef SO3_REPRESENT_INTERFACE_H
#define SO3_REPRESENT_INTERFACE_H


#include "SO3GameWorldRepresentHandler.h"
#include "SO3RepresentHandler.h"


class IKSO3RepresentInterface
{
public:
	virtual BOOL Init() = 0;
	virtual void Release() = 0;

	virtual IKSO3GameWorldRepresentHandler* GetGameWorldRepresentHandler() = 0;
	virtual IKSO3RepresentHandler* GetRepresentHandelr() = 0;
};

#define	FN_CREATE_SO3REPRESENT	"CreateSO3Represent"
typedef IKSO3RepresentInterface*	(*fnCreateSO3Represent)();

#ifdef SO3REPRESENT_EXPORTS
extern "C" __declspec(dllexport) IKSO3RepresentInterface* CreateSO3Represent();
#else
extern "C" __declspec(dllimport) IKSO3RepresentInterface* CreateSO3Represent();
#endif

#endif // SO3_REPRESENT_INTERFACE_H

