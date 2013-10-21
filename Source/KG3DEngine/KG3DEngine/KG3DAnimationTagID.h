#pragma once

class KG3DAnimationTagID
{
public:
    KG3DAnimationTagID();
    ~KG3DAnimationTagID();
    virtual HRESULT GetID(UINT uFrameIndex, UINT uKeyIndex, UINT& uID);
    virtual HRESULT GetFrame(UINT uID, UINT& uFrameIndex, UINT& uKeyIndex);
    UINT InsertKey(UINT uFrameIndex, UINT uKeyIndex);
    void RemoveKey(UINT uID);
private:
    DWORD GetNextID() { return m_dwCurrentID++; }
    struct ID2KeyFrame
    {
        UINT uFrame;
        UINT uIndex;
    };

    struct Frame2ID
    {
        UINT uIndex;
        UINT uID;
    };
    DWORD m_dwCurrentID;
    std::map<UINT, ID2KeyFrame> m_ID2Key;
    std::map<UINT, std::vector<Frame2ID>> m_Key2ID;
};
