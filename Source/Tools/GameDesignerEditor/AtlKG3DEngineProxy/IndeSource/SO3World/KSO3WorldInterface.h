#ifndef _KSO3_WORLD_INTERFACE_H_
#define	_KSO3_WORLD_INTERFACE_H_

class KNpc;
class KPlayer;

class KSO3WorldInterface
{
public:
	KSO3WorldInterface(void);
	~KSO3WorldInterface(void);

	void (*OnNpcHurt)(KNpc* pNpc);
	void (*OnPlayerHurt)(KPlayer* pNpc);

};

extern KSO3WorldInterface g_SO3WorldInterface;
#endif	//_KSO3_WORLD_INTERFACE_H_
