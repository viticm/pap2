////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KObjectIndex.h
//  Version     : 1.0
//  Creator     : zhaochunfeng
//  Comment     : ID生成,以及ID-->Object的映射
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KOBJECTINDEX_H_
#define _KOBJECTINDEX_H_

template <class T>
class KObjectIndex
{
public:
    KObjectIndex()
    {
        m_dwLastID = 1;
    }

    void SetPrefix(DWORD dwPrefix)
    { 
        m_dwLastID = (DWORD)(dwPrefix << (sizeof(m_dwLastID) * CHAR_BIT - ID_PREFIX_BIT_NUM)) + 1;
    }

    BOOL Register(T* pObject, DWORD dwID)
    {
        std::pair<typename KOBJECT_INDEX::iterator, bool> InsRet;

        assert(pObject);
        assert(pObject->m_dwID == ERROR_ID);
        assert(m_dwLastID != ERROR_ID);

        if (dwID == ERROR_ID)
        {
            dwID = m_dwLastID;
        }

        InsRet = m_ObjectIndex.insert(make_pair(dwID, pObject));
        if (InsRet.second)
        {
            pObject->m_dwID = dwID;
            m_dwLastID++;
        }

        return InsRet.second;
    }

    void Unregister(T* pObject)
    {
        m_ObjectIndex.erase(pObject->m_dwID);

        pObject->m_dwID = ERROR_ID;
    }

    T* GetObj(DWORD dwID)
    {
        typename KOBJECT_INDEX::iterator it = m_ObjectIndex.find(dwID);
        if (it != m_ObjectIndex.end())
        {
            return it->second;
        }
        return NULL;
    }

    size_t GetObjCount()
    {
        return m_ObjectIndex.size();
    }

	template <class TFunc>
	void Traverse(TFunc& Func)
    {
        BOOL bContinue = false;
        typename KOBJECT_INDEX::iterator it = m_ObjectIndex.begin();
        while (it != m_ObjectIndex.end())
        {
            typename KOBJECT_INDEX::iterator Current = it++;

            bContinue = Func(Current->first, Current->second);
            if (!bContinue)
                break;
        }
    }

private:
    typedef std::map<DWORD, T*> KOBJECT_INDEX;
    KOBJECT_INDEX   m_ObjectIndex;
    DWORD           m_dwLastID;
};

#endif
