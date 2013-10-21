////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Calculate.h
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 10/29/2009
//  Comment     : 使用逆波兰算法实现的表达式计算器
//  
//  Copyright (C) 2009 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef KG_CALCULATE_H
#define KG_CALCULATE_H

enum KG_EXPRESSION_PARAM_TYPE
{
    EXPRESSION_PARAM_TYPE_ERROR,

    EXPRESSION_PARAM_TYPE_FLOAT,
    EXPRESSION_PARAM_TYPE_STRING,
};

struct KG_EXPRESSION_PARAM
{
    KG_EXPRESSION_PARAM_TYPE eParamType;
    union
    {
        float fParam;
        const char *pcszParam;
    };
};

int CalculateExpression(
    const char cszExp[], int nExpLength, 
    const KG_EXPRESSION_PARAM *pParamList, int nParamCount, 
    char szResult[], int nResultSize,
    int *pnRetResultLength
);

#endif // KG_CALCULATE_H
