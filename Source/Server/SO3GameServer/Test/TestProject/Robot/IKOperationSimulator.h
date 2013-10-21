#ifndef _IK_OPERATION_SIMULATOR_H_
#define _IK_OPERATION_SIMULATOR_H_

enum OPERATION_SIMULATOR_TYPE
{
    ostGeneral,
    ostTalk
};

class KMonster;
class IKOperationSimulator
{
public:
    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int Activate() = 0;

    virtual ~IKOperationSimulator(){};
};

IKOperationSimulator *CreateSimulator(OPERATION_SIMULATOR_TYPE eType, KMonster *pOwner);
int DestroySimulator(IKOperationSimulator *pSimulator);

#endif