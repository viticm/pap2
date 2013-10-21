#ifndef KRLOPTIMIZATION_H
#define KRLOPTIMIZATION_H

struct KRLOptimization
{
    DWORD dwFPS;
    DWORD dwFrameCount;
    double fFPSUpdateTime;

    DWORD dwCurrentOptimizeFPS;
    double fOptimizeEvaluationExpiredTime;
    DWORD dwOptimizeLevel;
};

int InitOptimization(KRLOptimization* pRLOptimization);

int UpdateFPS(KRLOptimization* pRLOptimization);

int UpdateOptimization(KRLOptimization* pRLOptimization);

int Optimize(DWORD dwLevel);

#endif // KRLOPTIMIZATION_H
