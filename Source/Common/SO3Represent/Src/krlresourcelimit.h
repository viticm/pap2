#ifndef KRLRESOURCELIMIT_H
#define KRLRESOURCELIMIT_H

struct KRLRemoteCharacterOptimizationEvaluator;

struct KRLResourceLimit
{
    BOOL bEnable;

    size_t nCharacterVisibleCount;
    size_t nCharacterMaxVisibleCount;

    KRLRemoteCharacterOptimizationEvaluator* pRLRemoteCharacterOptimizationEvaluators;
    size_t nRLRemoteCharacterOptimizationEvaluatorCount;
    size_t nRLRemoteCharacterOptimizationEvaluatorMaxCount;
};

int InitCharacterResourceLimit(KRLResourceLimit* pResourceLimit);
int UnInitCharacterResourceLimit(KRLResourceLimit* pResourceLimit);

int UpdateCharacterResourceLimit(KRLResourceLimit* pResourceLimit);

#endif // KRLRESOURCELIMIT_H
