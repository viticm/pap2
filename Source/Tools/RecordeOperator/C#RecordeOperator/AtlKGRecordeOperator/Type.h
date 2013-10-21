
#ifndef _TYPE_H
#define _TYPE_H

struct IParameters
{
    static const int sMaxNum = 10;
    int nNum;
    int nFrom[sMaxNum];
    size_t size_tDataSize;
    char* pData; 
    IParameters()
    {
        nNum = 0;
        size_tDataSize = 0;
        pData = NULL; 
    };
};

struct IEventRecord
{
    DWORD dwID;
    FunctionType EventFunction;
    DWORD dwTime;
    DWORD dwFrame;
    Parameters AParameters;
    EventRecord* pNextEvent;
    IEventRecord()
    {
        dwID = 0;
        EventFunction = FT_NONE;
        dwTime = 0;
        dwFrame = 0;
        pNextEvent = NULL;
    };
};

enum PLAY_STATUS
{
    PLAY_PLAY,
    PLAY_EDIT
};


#endif // _TYPE_H