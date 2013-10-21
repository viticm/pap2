//注意 增加全局参数的步骤：
//1. 增加KGlobalSystemValueManager成员变量KSYSTEM_VALUE m_Name
//2. 使用宏DECLARE_SYSTEM_VALUE(Name)申明导出, 使用宏REGISTER_LUA_INTEGER注册
//3. 使用宏SAVE_GLOBAL_SYSTEM_VALUE(Name)在Activate()中保存
//4. 在KDBTools::LoadGlobalSystemValue()中用GetGlobalSystemValue()读取
//5. 增加KGlobalSystemValueCache对应成员变量m_nName
//6. 修改同步协议R2S_SYNC_GLOBAL_SYSTEM_VALUE 和发送和接收协议包的代码

#ifndef _KGLOBAL_SYSTEM_VALUE_MANAGER_H_
#define _KGLOBAL_SYSTEM_VALUE_MANAGER_H_

#define DECLARE_SYSTEM_VALUE(name)                  \
    int get##name(void) { return m_##name.nValue; }	\
    void set##name(int nValue)          \
    {                                   \
        if (nValue != m_##name.nValue)  \
        {                               \
            m_bNeedSyncToServer = true; \
			m_##name.bUpdate = true;    \
			m_##name.nValue = nValue;   \
		}                               \
	}

class KGlobalSystemValueManager
{
public:
    KGlobalSystemValueManager();
    ~KGlobalSystemValueManager();

    BOOL Load();

    void Activate();

    void SaveAll();

private:
    struct KSYSTEM_VALUE
    {
        KSYSTEM_VALUE() : bUpdate(false), nValue(0) {}

        BOOL    bUpdate;
        int     nValue;
    };

    time_t          m_NextSaveTime;

public:
    BOOL            m_bNeedSyncToServer;

    KSYSTEM_VALUE   m_GameWorldStartTime;
    KSYSTEM_VALUE   m_MaxPlayerLevel;
    KSYSTEM_VALUE   m_GameWorldChargeTime;

	DECLARE_LUA_CLASS(KGlobalSystemValueManager);
	DECLARE_SYSTEM_VALUE(GameWorldStartTime);
	DECLARE_SYSTEM_VALUE(MaxPlayerLevel);
	DECLARE_SYSTEM_VALUE(GameWorldChargeTime);
};

#endif
