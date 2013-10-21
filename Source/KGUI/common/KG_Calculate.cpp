////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Calculate.cpp
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 10/29/2009
//  Comment     : 
//  
//  Copyright (C) 2009 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KG_Calculate.h"
#include <math.h>

enum KG_RPN_NODE_TYPE
{
    RPN_NODE_TYPE_ERROR = 0,

    RPN_NODE_TYPE_BOOLEAN,
    RPN_NODE_TYPE_FLOAT,
    RPN_NODE_TYPE_STRING,
    RPN_NODE_TYPE_VARIABLE,
    RPN_NODE_TYPE_JOIN,

    RPN_NODE_TYPE_OPERATOR,
};

struct KG_RPN_STRING
{
    const char *pcszData;
    int nLength;
};

struct KG_RPN_FLOAT
{
    float fData;
    int nPrecision;
};


struct KG_RPN_NODE;
typedef std::list<KG_RPN_NODE> KG_RPNNodeList;

struct KG_RPN_NODE
{
    KG_RPN_NODE_TYPE eNodeType;
    union
    {
        bool bData;
        KG_RPN_FLOAT Float;
        KG_RPN_STRING Str;
        int nVariableIndex;
        KG_RPNNodeList *pJoinList;
        char chOperator;
    };
};

static int GetOperatorPriority(char chOperator)
{
    int nPriorty = -1;

    switch (chOperator)
    {
    case '(':
        nPriorty = 10;
        break;
    case '~':
        nPriorty = 20;
        break;
    case '>':
    case '<':
    case '=':
        nPriorty = 25;
        break;
    case '#':
        nPriorty = 30;
        break;
    case '+':
    case '-':
        nPriorty = 40;
        break;
    case '*':
    case '/':
        nPriorty = 50;
        break;
    case '^':
        nPriorty = 60;
        break;
    case ')':
        nPriorty = 70;
        break;
    default:
        break;
    }
    return nPriorty;
}

static int IsOperator(char chOperator)
{
    return GetOperatorPriority(chOperator) > 0;
}

static int Calaculate(KG_RPN_NODE *pLNode, KG_RPN_NODE *pRNode, char chOperator)
{
    int nResult = false;
    int nRetCode = false;

    KG_RPNNodeList *pJoinList = NULL;
    KGLOG_PROCESS_ERROR(pLNode);
    KGLOG_PROCESS_ERROR(pRNode);

    switch (chOperator)
    {
    case '~':
        if (pLNode->eNodeType == RPN_NODE_TYPE_JOIN)
        {
            KGLOG_PROCESS_ERROR(pLNode->pJoinList);
            pLNode->pJoinList->push_back(*pRNode);
        }
        else
        {
            if (pRNode->eNodeType == RPN_NODE_TYPE_JOIN)
            {
                pJoinList = pRNode->pJoinList;
                pRNode->pJoinList = NULL;
            }
            else
            {
                pJoinList = new KG_RPNNodeList;
                KGLOG_PROCESS_ERROR(pJoinList);
                pJoinList->push_back(*pRNode);
            }
            pJoinList->push_front(*pLNode);
            pLNode->eNodeType = RPN_NODE_TYPE_JOIN;
            pLNode->pJoinList = pJoinList;
        }
        break;
    case '#':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.nPrecision = (int)pRNode->Float.fData;
        break;
    case '^':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.fData = pow(pLNode->Float.fData, pRNode->Float.fData);
        break;
    case '+':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.fData += pRNode->Float.fData;
        break;
    case '-':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.fData -= pRNode->Float.fData;
        break;
    case '*':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.fData *= pRNode->Float.fData;
        break;
    case '/':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->Float.fData /= pRNode->Float.fData;
        break;
    case '>':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->eNodeType = RPN_NODE_TYPE_BOOLEAN;
        pLNode->bData = (pLNode->Float.fData > pRNode->Float.fData);
        break;
    case '<':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->eNodeType = RPN_NODE_TYPE_BOOLEAN;
        pLNode->bData = (pLNode->Float.fData < pRNode->Float.fData);
        break;
    case '=':
        KGLOG_PROCESS_ERROR(pLNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        KGLOG_PROCESS_ERROR(pRNode->eNodeType == RPN_NODE_TYPE_FLOAT);
        pLNode->eNodeType = RPN_NODE_TYPE_BOOLEAN;
        if (pLNode->Float.nPrecision != pRNode->Float.nPrecision)
        {
            pLNode->bData = false;
        }
        else
        {
            int nFactor = (int)pow(10.0f, pLNode->Float.nPrecision);
            pLNode->bData = ((int)(pLNode->Float.fData * nFactor) == (int)(pRNode->Float.fData * nFactor));
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_DELETE(pJoinList);
    }
    return nResult;
}

static int BuildNode(const char cszData[], int nDataLength, KG_RPN_NODE *pNode)
{
    int nResult = false;
    int nRetCode = false;

    int i = 0;
    int nDotIndex = -1;
    int nFactor = 10;
    int nFloatBeginFlag = false;
    int nFloatEndFlag = false;
    
    KGLOG_PROCESS_ERROR(cszData);
    KGLOG_PROCESS_ERROR(nDataLength > 0);
    KGLOG_PROCESS_ERROR(pNode);

    if (
        cszData[0] == 'D' && 
        nDataLength >= 2 && 
        nDataLength <=3 && 
        cszData[1] >= '0' && 
        cszData[1] <= '9'
    ) // D3 or D42
    {
        pNode->eNodeType = RPN_NODE_TYPE_VARIABLE;
        pNode->nVariableIndex = cszData[1] - '0';
        KG_PROCESS_SUCCESS(nDataLength == 2);

        if (cszData[2] >= '0' && cszData[2] <= '9')
        {
            pNode->nVariableIndex = pNode->nVariableIndex * 10 + (cszData[2] - '0');
            KG_PROCESS_SUCCESS(true);
        }
    }

    pNode->eNodeType = RPN_NODE_TYPE_FLOAT;
    pNode->Float.fData = 0;
    pNode->Float.nPrecision = 0;
    nFloatBeginFlag = false;
    nFloatEndFlag = false;
    for (i = 0; i < nDataLength; i++)
    {
        if (!nFloatBeginFlag && cszData[i] == ' ')
        {
            continue;
        }

        if (nFloatBeginFlag && cszData[i] == ' ')
        {
            nFloatEndFlag = true;
            continue;
        }

        if (nFloatEndFlag && cszData[i] == ' ')
        {
            continue;
        }
        
        if (nFloatBeginFlag && cszData[i] == '.' && nDotIndex == -1)
        {
            nDotIndex = i;
            continue;
        }

        if (cszData[i] >= '0' && cszData[i] <= '9')
        {
            nFloatBeginFlag = true;
            if (nDotIndex == -1)
            {
                pNode->Float.fData = pNode->Float.fData * 10.0f + cszData[i] - '0';
            }
            else
            {
                pNode->Float.fData += (float)(cszData[i] - '0') / nFactor;
                nFactor *= 10;
            }
            continue;
        }
        
        pNode->eNodeType = RPN_NODE_TYPE_STRING;
        pNode->Str.pcszData = cszData;
        pNode->Str.nLength = nDataLength;
        break;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

typedef std::vector<KG_RPN_NODE> KG_RPNNodeVector;
typedef std::vector<char> KG_RPNOperatorVector;

static int ConverntToRPN(const char cszExp[], int nExpLength, KG_RPNNodeVector *pNodeVector)
{
    int nResult  = false;
    int nRetCode = false;

    int i = 0;
    int j = 0;
    int nDataBeginIndex = 0;
    int nNewSubExpFlag = false;

    KG_RPNOperatorVector OperatorVector;
    KG_RPN_NODE ExpData;

    KGLOG_PROCESS_ERROR(cszExp);
    KGLOG_PROCESS_ERROR(nExpLength > 0);
    KGLOG_PROCESS_ERROR(pNodeVector);

    nDataBeginIndex = -1;
    nNewSubExpFlag = true;
    for (i = 0; i < nExpLength; i++)
    {
        char chNotation = cszExp[i];
        int nIsOperator = IsOperator(chNotation);
        if (nIsOperator && i > 0 && cszExp[i - 1] == '\\')
        {
            nIsOperator = false;
        }

        if (nDataBeginIndex == -1 && !nIsOperator)
        {
            nDataBeginIndex = i;
        }

        if (nDataBeginIndex != -1 && (nIsOperator || i == nExpLength - 1))
        {
            int nDataEndIndex = i;
            if (nIsOperator)
            {
                nDataEndIndex--;
            }
            KGLOG_PROCESS_ERROR(nDataBeginIndex <= nDataEndIndex);

            nRetCode = BuildNode(cszExp + nDataBeginIndex, nDataEndIndex - nDataBeginIndex + 1, &ExpData);
            KGLOG_PROCESS_ERROR(nRetCode);

            pNodeVector->push_back(ExpData);
            nDataBeginIndex = -1;
            nNewSubExpFlag = false;
        }

        if (!nIsOperator)
            continue;

        if (chNotation == '(')
        {
            OperatorVector.push_back(chNotation);
            nNewSubExpFlag = true;
            continue;
        }

        if (chNotation == ')')
        {
            for (j = (int)OperatorVector.size() - 1; j >= 0; j--)
            {
                if (OperatorVector[j] == '(')
                {
                    OperatorVector.pop_back();
                    break;
                }

                ExpData.eNodeType = RPN_NODE_TYPE_OPERATOR;
                ExpData.chOperator = OperatorVector[j];
                pNodeVector->push_back(ExpData);
                OperatorVector.pop_back();
            }
            continue;
        }

        if (nNewSubExpFlag)
        {
            ExpData.eNodeType = RPN_NODE_TYPE_FLOAT;
            ExpData.Float.fData = 0.0f;
            ExpData.Float.nPrecision = 0;
            pNodeVector->push_back(ExpData);
            nNewSubExpFlag = false;
        }

        for (j = (int)OperatorVector.size() - 1; j >= 0; j--)
        {
            int nCurPriority = GetOperatorPriority(chNotation);
            int nTopPriority = GetOperatorPriority(OperatorVector[j]);
            int nDataEndIndex = 0;

            if (nTopPriority < nCurPriority)
                break;

            ExpData.eNodeType = RPN_NODE_TYPE_OPERATOR;
            ExpData.chOperator = OperatorVector[j];
            pNodeVector->push_back(ExpData);
            OperatorVector.pop_back();
        }

        OperatorVector.push_back(cszExp[i]);
    }

    for (j = (int)OperatorVector.size() - 1; j >= 0; j--)
    {
        ExpData.eNodeType = RPN_NODE_TYPE_OPERATOR;
        ExpData.chOperator = OperatorVector[j];
        pNodeVector->push_back(ExpData);
    }
    OperatorVector.clear();

// Exit1:
    nResult = true;
Exit0:
    return nResult;
}


static int CalculateRPN(KG_RPNNodeVector *pRPNVector)
{
    int nResult  = false;
    int nRetCode = false;

    KG_RPNNodeVector::iterator itLNode;
    KG_RPNNodeVector::iterator itRNode;
    KG_RPNNodeVector::iterator itOperatorNode;
    KG_RPNNodeVector::iterator itEnd;

    int nVectorLength = 0;

    KGLOG_PROCESS_ERROR(pRPNVector);

    nVectorLength = (int)pRPNVector->size();
    KGLOG_PROCESS_ERROR(nVectorLength > 0);

    itLNode = pRPNVector->begin();
    while (true)
    {
        itEnd = pRPNVector->end();
        KGLOG_PROCESS_ERROR(itLNode != itEnd);
        KGLOG_PROCESS_ERROR(itLNode->eNodeType != RPN_NODE_TYPE_OPERATOR);

        itRNode = itLNode + 1;
        if (itRNode == itEnd)
            break;

        KGLOG_PROCESS_ERROR(itRNode->eNodeType != RPN_NODE_TYPE_OPERATOR);

        itOperatorNode = itRNode + 1;
        KGLOG_PROCESS_ERROR(itOperatorNode != itEnd);
        if (itOperatorNode->eNodeType != RPN_NODE_TYPE_OPERATOR)
        {
            ++itLNode;
            continue;
        }

        nRetCode = Calaculate(&*itLNode, &*itRNode, itOperatorNode->chOperator);
        KGLOG_PROCESS_ERROR(nRetCode);

        pRPNVector->erase(itOperatorNode);
        pRPNVector->erase(itRNode);
        itLNode = pRPNVector->begin();
    }

// Exit1:
    nResult = true;
Exit0:
    return nResult;    
}

static int ReleaseRPNNode(KG_RPN_NODE *pDataParam)
{
    int nResult = false;
    int nRetCode = false;

    KG_RPNNodeList::iterator itJoinList;

    if (pDataParam)
    {
        if (pDataParam->eNodeType == RPN_NODE_TYPE_JOIN && pDataParam->pJoinList)
        {
            for (
                itJoinList = pDataParam->pJoinList->begin(); 
                itJoinList != pDataParam->pJoinList->end(); 
                ++itJoinList
            )
            {
                nRetCode = ReleaseRPNNode(&*itJoinList);
                // KGLOG_PROCESS_ERROR(nRetCode);
            }
            SAFE_DELETE(pDataParam->pJoinList);
        }
        
        pDataParam->eNodeType = RPN_NODE_TYPE_ERROR;
    }
    return true;
}

static int DumpRPNNode(KG_RPN_NODE *pDataParam, char szResult[], int nSize, int *pnRetLength)
{
    int nResult  = false;
    int nRetCode = false;

    int nLength = 0;
    KG_RPNNodeList::iterator itJoinList;
    char szFloatFormat[16];
    int i = 0;
    int nEscaping = false;

    KGLOG_PROCESS_ERROR(pDataParam);
    KGLOG_PROCESS_ERROR(szResult);
    KGLOG_PROCESS_ERROR(nSize > 0);

    switch (pDataParam->eNodeType)
    {
    case RPN_NODE_TYPE_FLOAT:
        nRetCode = snprintf(szFloatFormat, sizeof(szFloatFormat), "%%.%df", pDataParam->Float.nPrecision);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        nLength = snprintf(szResult, nSize, szFloatFormat, pDataParam->Float.fData);
        KGLOG_PROCESS_ERROR(nLength > 0);
    	break;
    case RPN_NODE_TYPE_STRING:
        KGLOG_PROCESS_ERROR(pDataParam->Str.pcszData);
        KGLOG_PROCESS_ERROR(pDataParam->Str.nLength < nSize);
        nLength = 0;
        for (i = 0; i < pDataParam->Str.nLength; i++)
        {
            if (!nEscaping && pDataParam->Str.pcszData[i] == '\\')
            {
                nEscaping = true;
                continue;
            }

            szResult[nLength] = pDataParam->Str.pcszData[i];
            nLength++;
            nEscaping = false;
        }
        break;
    case RPN_NODE_TYPE_BOOLEAN:
        nLength = snprintf(szResult, nSize, "%s", pDataParam->bData ? "true" : "false");
        KGLOG_PROCESS_ERROR(nLength > 0);
        break;
    case RPN_NODE_TYPE_JOIN:
        KGLOG_PROCESS_ERROR(pDataParam->pJoinList);
        for (
            itJoinList = pDataParam->pJoinList->begin(); 
            itJoinList != pDataParam->pJoinList->end(); 
            ++itJoinList
        )
        {
            int nSubLength = 0;
            nRetCode = DumpRPNNode(&*itJoinList, szResult + nLength, nSize - nLength, &nSubLength);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(nLength + nSubLength < nSize);
            nLength += nSubLength;
        }
        break;
    }
    
// Exit1:
    if (pnRetLength)
    {
        *pnRetLength = nLength;
    }
    
    nResult = true;
Exit0:
    return nResult;
}

static int CalculateBinaryExpression(
    const char cszExp[], int nExpLength, 
    const KG_EXPRESSION_PARAM *aParamList, int nParamCount, 
    KG_RPN_NODE *pRetResult
)
{
    int nResult  = false;
    int nRetCode = false;

    KG_RPNNodeVector RPNVector;
    KG_RPNNodeVector::iterator itRPN;

    KGLOG_PROCESS_ERROR(cszExp);
    KGLOG_PROCESS_ERROR(nExpLength > 0);
    // KGLOG_PROCESS_ERROR(pParamList);
    // KGLOG_PROCESS_ERROR(nParamCount > 0);
    KGLOG_PROCESS_ERROR(pRetResult);

    pRetResult->eNodeType = RPN_NODE_TYPE_ERROR;

    nRetCode = ConverntToRPN(cszExp, nExpLength, &RPNVector);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (itRPN = RPNVector.begin(); itRPN != RPNVector.end(); ++itRPN)
    {
        const KG_EXPRESSION_PARAM *pcParam = NULL;

        if (itRPN->eNodeType != RPN_NODE_TYPE_VARIABLE)
            continue;

        KGLOG_PROCESS_ERROR(aParamList);
        KGLOG_PROCESS_ERROR(itRPN->nVariableIndex < nParamCount);
        pcParam = &(aParamList[itRPN->nVariableIndex]);

        switch (pcParam->eParamType)
        {
        case EXPRESSION_PARAM_TYPE_FLOAT:
            itRPN->eNodeType = RPN_NODE_TYPE_FLOAT;
            itRPN->Float.fData = pcParam->fParam;
            itRPN->Float.nPrecision = 0;
            break;
        case EXPRESSION_PARAM_TYPE_STRING:
            itRPN->eNodeType = RPN_NODE_TYPE_STRING;
            itRPN->Str.pcszData = pcParam->pcszParam;
            itRPN->Str.nLength = (int)strlen(pcParam->pcszParam);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }

    nRetCode = CalculateRPN(&RPNVector);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = (int)RPNVector.size();
    KGLOG_PROCESS_ERROR(nRetCode == 1);

// Exit1:
    *pRetResult = RPNVector.back();

    nResult = true;
Exit0:
    if (!nResult)
    {
        for (itRPN = RPNVector.begin(); itRPN != RPNVector.end(); ++itRPN)
        {
            ReleaseRPNNode(&*itRPN);
        }        
    }
    RPNVector.clear();
    return nResult;
}

int CalculateExpression(
    const char cszExp[], int nExpLength, 
    const KG_EXPRESSION_PARAM *pParamList, int nParamCount, 
    char szResult[], int nResultSize,
    int *pnRetResultLength
)
{
    int nResult  = false;
    int nRetCode = false;

    int i = 0;
    int nSplitMarkIndex = 0;
    KG_RPN_NODE ConditionNode = {RPN_NODE_TYPE_ERROR};
    KG_RPN_NODE ResultNode = {RPN_NODE_TYPE_ERROR};

    KGLOG_PROCESS_ERROR(cszExp);
    KGLOG_PROCESS_ERROR(nExpLength > 0);
    // KGLOG_PROCESS_ERROR(pParamList);
    // KGLOG_PROCESS_ERROR(nParamCount > 0);
    KGLOG_PROCESS_ERROR(szResult);
    KGLOG_PROCESS_ERROR(nResultSize > 0);
    // KGLOG_PROCESS_ERROR(pnRetExpLength);

    nSplitMarkIndex = -1;
    for (i = 0; i < nExpLength; i++)
    {
        switch (cszExp[i])
        {
        case '\\':
            i++;
            break;
        case '?':
            KGLOG_PROCESS_ERROR(nSplitMarkIndex == -1);
            
            nRetCode = CalculateBinaryExpression(
                cszExp, i,
                pParamList, nParamCount,
                &ConditionNode
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(ConditionNode.eNodeType == RPN_NODE_TYPE_BOOLEAN);
            nSplitMarkIndex = i;
            break;
        case ':':
            KGLOG_PROCESS_ERROR(nSplitMarkIndex != -1);
            if (ConditionNode.bData)
            {
                nRetCode = CalculateBinaryExpression(
                    cszExp + nSplitMarkIndex + 1, i - nSplitMarkIndex - 1,
                    pParamList, nParamCount,
                    &ResultNode
                );
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                nRetCode = CalculateBinaryExpression(
                    cszExp + i + 1, nExpLength - i - 1,
                    pParamList, nParamCount,
                    &ResultNode
                );
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            KG_PROCESS_SUCCESS(true);
        default:
            break;
        }
    }

    nRetCode = CalculateBinaryExpression(
        cszExp, nExpLength,
        pParamList, nParamCount,
        &ResultNode
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    KGLOG_PROCESS_ERROR(ResultNode.eNodeType != RPN_NODE_TYPE_ERROR);
    KGLOG_PROCESS_ERROR(ResultNode.eNodeType != RPN_NODE_TYPE_VARIABLE);
    KGLOG_PROCESS_ERROR(ResultNode.eNodeType != RPN_NODE_TYPE_OPERATOR);

    nRetCode = DumpRPNNode(&ResultNode, szResult, nResultSize, pnRetResultLength);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    ReleaseRPNNode(&ConditionNode);
    ReleaseRPNNode(&ResultNode);
    return nResult;
}
