//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSGPublic.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2004-4-19 20:44:49
//  Comment     :   Kingsoft Game Public header file
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGPUBLIC_H
#define _KGPUBLIC_H    1

#include <assert.h>

#ifndef ASSERT
#define ASSERT assert
#endif

#define KG_FUNCTION __FUNCTION__

#define KG_USE_ARGUMENT(arg) (arg)

#define KG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)


#define KG_PROCESS_SUCCESS(Condition) \
    do  \
    {   \
        if (Condition)      \
            goto Exit1;     \
    } while (false)

#define KG_PROCESS_ERROR_OR_SUCCESS(Condition) \
    do  \
    {   \
        if (Condition)      \
            goto Exit1;     \
        goto Exit0;         \
    } while (false)

#define KG_PROCESS_ERROR_RET_CODE(Condition, Code) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            nResult = Code;     \
            goto Exit0;         \
        }                       \
    } while (false)



#define KG_COM_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (FAILED(Condition))  \
            goto Exit0;         \
    } while (false)


#define KG_COM_PROCESS_SUCCESS(Condition)   \
    do  \
    {   \
        if (SUCCEEDED(Condition))   \
            goto Exit1;             \
    } while (false)


// KG_COM_PROCESS_ERROR_RETURN_ERROR
#define KG_COM_PROC_ERR_RET_ERR(Condition)  \
    do  \
    {   \
        if (FAILED(Condition))      \
        {                           \
            hrResult = Condition;   \
            goto Exit0;             \
        }   \
    } while (false)


#define KG_COM_PROC_ERROR_RET_CODE(Condition, Code)     \
    do  \
    {   \
        if (FAILED(Condition))  \
        {                       \
            hrResult = Code;    \
            goto Exit0;         \
        }                       \
    } while (false)


#define KG_COM_RELEASE(pInterface) \
    do  \
    {   \
        if (pInterface)                 \
        {                               \
            (pInterface)->Release();    \
            (pInterface) = NULL;        \
        }                               \
    } while (false)


#define KG_DELETE_ARRAY(pArray)     \
    do  \
    {   \
        if (pArray)                 \
        {                           \
            delete [](pArray);      \
            (pArray) = NULL;        \
        }                           \
    } while (false)


#define KG_DELETE(p)    \
    do  \
    {   \
        if (p)              \
        {                   \
            delete (p);     \
            (p) = NULL;     \
        }                   \
    } while (false)
    

#ifdef _DEBUG

#define KG_ASSERT_EXIT(Condition) \
    do  \
    {   \
        assert(Condition);  \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)

#else

#define KG_ASSERT_EXIT(Condition)  \
    do  \
    {   \
        if (!(Condition))       \
            goto Exit0;         \
    } while (false)


#endif

#ifndef KG_HANDLE_DEFINED
#define KG_HANDLE_DEFINED

typedef void *KGHANDLE;    

#endif // KG_HANDLE_DEFINED

#endif // _KGPUBLIC_H
