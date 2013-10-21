#ifndef _KPLAYER_PK_H_
#define	_KPLAYER_PK_H_

#include "Global.h"
#include "SO3Result.h"

class KPlayer;

#define PK_SCRIPT SCRIPT_DIR"/player/PVPScript.lua"

enum PK_STATE
{
	pksInvalid,
	pksIdle,

	pksWaitForDuelRespond,	// 等待切磋回复
	pksConfirmDuel,		    // 有人请求切磋，在考虑是否同意
	pksPrepareDuel,	        // 切磋同意了，等待开始倒计时
	pksDueling, 			// 切磋中
	pksDuelOutOfRange,	    // 切磋中，但是超出范围了

	pksStartSlay,			// 开启屠杀倒计时
    pksSlaying,             // 屠杀中 
	pksExitSlay,			// 申请关闭屠杀，倒计时中

	pksTotal
};

// PK 结束的原因
enum PK_END_CODE
{
    pkecInvalid,

    pkecBeat,               // 被击败
    pkecOffline,            // 掉线
    pkecUnexpectedDie,      // 意外死亡
    pkecSwitchMap,          // 切地图
    pkecOutofRangeTimeUp,   // 超出范围超时

    pkecTotal
};

enum PK_OPERATE
{
	pkoInvalid,

	pkoApplyDuel,
	pkoRefuseDuel,
	pkoAcceptDuel,
	pkoLossDuel,
	pkoApplySlay,
	pkoCloseSlay,

	pkoTotal
};

class KPKController
{
public:
	KPKController();
	virtual ~KPKController();

	BOOL			Init(KPlayer* pPlayer);
	void			UnInit();
    
    DWORD   		GetTargetID(){ return m_dwTargetID; }
    KPlayer*   		GetTargetPlayer();
    DWORD           GetLoserID(){ return m_dwDuelLoser; }
    void            SetLoserID(DWORD dwID) { m_dwDuelLoser = dwID; }

    PK_RESULT_CODE	CanApplyDuel(KPlayer* pTarget);
    PK_RESULT_CODE	CanApplySlay();

	void			SetState(
        PK_STATE    eState, 
        int         nEndFrame   = 0, 
        DWORD       dwTarget    = ERROR_ID, 
        BOOL        bRefuseDuel = false
    );
	PK_STATE		GetState()	{ return m_eState; }
    
    BOOL            IsDuel();
    BOOL            IsOnSlay();
    PK_STATE        GetPKState();

#ifdef _SERVER
    int             GetCloseSlayLeftTime();
    void			Activate();
	void			SetDuelPostion(DWORD dwMapID, int nX, int nY, int nZ);

    PK_RESULT_CODE	ApplyDuel(DWORD dwTargetID);	// 申请切磋
    PK_RESULT_CODE	RefuseDuel();                   // 拒绝切磋
    PK_RESULT_CODE	AcceptDuel();                   // 接受切磋
    PK_RESULT_CODE  LossDuel();        	            // 切磋失败
	PK_RESULT_CODE	ApplySlay();                    // 申请屠杀
	PK_RESULT_CODE	CloseSlay();                    // 关闭屠杀
#endif

private:

#ifdef _SERVER
	BOOL			CheckDuelDistance(int nDistance);
	void			CancelDuel(BOOL bRefuseDuelFlag = false);

	BOOL			CreateDuelFlag(int nX, int nY, int nZ);
	void			DeleteDuelFlag();

    void			OnWaitConfirmDuel();
	void			OnPrepareDuel();
	void			OnDueling();
	void			OnDuelOutOfRange();
    void            OnStartSlay();
	void			OnSlaying();
	void			OnExitSlay();

public:
    BOOL            CallDuelAcceptScript();
    BOOL            CallDuelBeginScript();
    BOOL            CallDuelEndScript(int nCode);
#endif

private:
    KPlayer*		m_pPlayer;
    PK_STATE		m_eState;
    DWORD			m_dwTargetID;
    DWORD           m_dwDuelLoser;

#ifdef _SERVER
    int				m_nDuelCenterX;
    int				m_nDuelCenterY;
    int				m_nDuelCenterZ;
    DWORD			m_dwMapID;
    int				m_nTimer;
    DWORD			m_dwFlagDoodadID;
#endif
};
#endif
