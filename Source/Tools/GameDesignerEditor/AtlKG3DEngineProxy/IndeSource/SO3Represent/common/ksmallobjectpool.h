#ifndef COMMON_K_SMALL_OBJECT_POOL_H
#define COMMON_K_SMALL_OBJECT_POOL_H

class KSmallObjectPool
{
public:
	KSmallObjectPool();
	KSmallObjectPool(LPCSTR szName, size_t m_nObjectSize, size_t nChunkObjectCount = 32);
	~KSmallObjectPool();

	int Init(size_t nObjectSize, size_t nChunkObjectCount = 32);

	void* Alloc();
	void Free(void* pValue);

	void Purge();

private:
	LPCSTR m_szName;

    struct KChunk* m_pChunkList;
    struct KObject* m_pFreeObjectList;

    size_t m_nChunkObjectCount;
	size_t m_nObjectSize;
};

#endif // COMMON_K_SMALL_OBJECT_POOL_H