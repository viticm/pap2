#ifndef K_MISSILE_REPRESENT_MISSILE_PROCESSOR_H
#define K_MISSILE_REPRESENT_MISSILE_PROCESSOR_H

class KRepresentMissileProcessor
{
protected:
	virtual ~KRepresentMissileProcessor();

public:
	ULONG Release();

    int Init();
    int UnInit();

	int Update(double fTime, double fTimeLast);
	int IsFinished() const;
	int Start(double fTime);
	int Stop(double fTime);

protected:
	virtual int OnInit() = 0;
    virtual int OnUnInit() = 0;
	virtual int OnUpdate(double fTime, double fTimeLast) = 0;
	virtual int OnIsFinished() const = 0;
	virtual int OnStart(double fTime) = 0;
	virtual int OnStop(double fTime) = 0;
};

#endif // K_MISSILE_REPRESENT_MISSILE_PROCESSOR_H