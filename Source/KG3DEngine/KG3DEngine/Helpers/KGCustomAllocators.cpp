////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCustomAllocators.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-17 9:34:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KGCustomAllocators.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(_DEBUG) | defined(DEBUG)
static int localSTCount = -1;//有一个是GetDefault的
static KG_CUSTOM_HELPERS::FAST_ALLOC::TesterOf_index_fast_vec st_TesterOf_index_fast_vec;
#endif

namespace KG_CUSTOM_HELPERS
{

	namespace FAST_ALLOC
	{
#if defined(_DEBUG) | defined(DEBUG)
		TesterOf_index_fast_vec::TesterOf_index_fast_vec()
		{
			struct TypeTemp
			{
				void* pVoid;//纯粹为占位

				TypeTemp():pVoid(NULL){++localSTCount;}
				~TypeTemp(){--localSTCount;}
				TypeTemp(const TypeTemp& Other){++localSTCount;}

			};
			typedef index_fast_vec<TypeTemp> TypeFastVec;
			typedef TypeFastVec::handle_type  TypeHandle;

			//UnitTest
			do
			{
				TypeFastVec	FastVec;
				
				{
					TypeTemp& DefaultElem = FastVec.get(UINT_MAX);//保证没有。初始化那个GetDefault的
					_ASSERTE(FastVec.is_error_elem(DefaultElem));

					TypeTemp Default;
					for (size_t i = 0; i < 10; i++)
					{
						TypeHandle handle = FastVec.push(Default);
						_ASSERTE(handle == i);
					}

					_ASSERTE(FastVec.size() == 10);


					{
						TypeHandle handle = FastVec.get_first_handle();
						_ASSERTE(handle == 0);
						_ASSERTE(! FastVec.is_error_elem(FastVec.get(handle)));
						for (size_t i = 1; i < 10; i++)
						{
							handle = FastVec.get_next_handle(i-1);
							_ASSERTE(handle == i);
							_ASSERTE(! FastVec.is_error_elem(FastVec.get(handle)));
						}
					}

					for (size_t i = 0; i < 10; i++)
					{
						FastVec.erase(i);
					}
					_ASSERTE(FastVec.size() == 0);
					_ASSERTE(! FastVec.is_valid_handle(0));

					FastVec.clear();
					_ASSERTE(FastVec.empty());
					
					{
						TypeTemp& Temp = FastVec.get(11);
						_ASSERTE(FastVec.is_error_elem(Temp));
					}
					
				}
				_ASSERTE(localSTCount == 0);
			}while(false);
			//WhiteBox Test
			do
			{
				TypeFastVec	FastVec;
				_ASSERTE(FastVec.empty());

				//Case1
				{
					TypeTemp Default;
					TypeHandle handle = FastVec.push(Default);
					_ASSERTE(FastVec.is_valid_handle(handle));
					handle = FastVec.push(Default);
					_ASSERTE(FastVec.is_valid_handle(handle));
					handle = FastVec.push(Default);
					_ASSERTE(FastVec.is_valid_handle(handle));
					handle = FastVec.push(Default);
					_ASSERTE(FastVec.is_valid_handle(handle));
					_ASSERTE(FastVec.size() == 4);

				}
				_ASSERTE(localSTCount == 4);

				//Case2
				{
					_ASSERTE(FastVec.is_valid_handle(1));
					_ASSERTE(FastVec.is_valid_handle(2));

					bool bRet = FastVec.erase(1);
					_ASSERTE(bRet);
					bRet = FastVec.erase(2);
					_ASSERTE(bRet);
					_ASSERTE(FastVec.size() == 2);
					_ASSERTE(localSTCount == 2);

					_ASSERTE(! FastVec.is_valid_handle(1));
					_ASSERTE(! FastVec.is_valid_handle(2));
				}

				//Case3
				{
					TypeHandle handle = FastVec.get_first_handle();
					_ASSERTE(handle == 0);
					handle = FastVec.get_next_handle(handle);
					_ASSERTE(handle == 3);
				}

				//Case4
				{
					TypeTemp& Type0 = FastVec.get(0);
					TypeTemp& Type1 = FastVec.get(3);
					_ASSERTE((char*)&Type1 == (char*)&Type0 + TypeFastVec::gap_between_two_elem*3);
				}

				//Case5
				{
					TypeTemp Default;
					TypeHandle handle = FastVec.push(Default);
					_ASSERTE(handle == 2);
					handle = FastVec.push(Default);
					_ASSERTE(handle == 1);
					handle = FastVec.push(Default);
					_ASSERTE(handle == 4);
				}
				_ASSERTE(localSTCount == 5);

				//Case6
				{
					FastVec.clear();
					_ASSERTE(FastVec.empty());
					_ASSERTE(localSTCount == 0);
				}
				{
					FastVec.push(TypeTemp());
				}
			}while(false);
			
			//Check Destructor
			_ASSERTE(localSTCount == 0);
			//_KG3D_DEBUG_REPORT("KGCustomAllocators-index_fast_vec RunTime Test Complete! Congratulations!\n");
		}
#endif

#if 0
		void FixedAllocator::Chunk::Init(std::size_t blockSize, unsigned char blocks)
		{
			assert(blockSize > 0);
			assert(blocks > 0);
			// Overflow check
			assert((blockSize * blocks) / blockSize == blocks);

			m_pData = new unsigned char[blockSize * blocks];
			Reset(blockSize, blocks);
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Chunk::Reset
		// 清除Chunk，在每个block的开头写入0-255
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::Chunk::Reset(std::size_t blockSize, unsigned char blocks)
		{
			assert(blockSize > 0);
			assert(blocks > 0);
			// Overflow check
			assert((blockSize * blocks) / blockSize == blocks);

			m_FirstAvailableBlock = 0;
			m_BlocksAvailable = blocks;

			unsigned char i = 0;
			unsigned char* p = m_pData;
			for (; i != blocks; p += blockSize)
			{
				*p = ++i;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Chunk::Release
		// 释放分配的空间
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::Chunk::Release()
		{
			delete[] m_pData;
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Chunk::Allocate
		// 分配一个Block，实际上没有分配空间，而是从已分配空间里面拿一个出来
		////////////////////////////////////////////////////////////////////////////////

		void* FixedAllocator::Chunk::Allocate(std::size_t blockSize)
		{
			if (!m_BlocksAvailable) return 0;

			assert((m_FirstAvailableBlock * blockSize) / blockSize == 
				m_FirstAvailableBlock);

			unsigned char* pResult =
				m_pData + (m_FirstAvailableBlock * blockSize);
			m_FirstAvailableBlock = *pResult;
			--m_BlocksAvailable;

			return pResult;
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Chunk::Deallocate
		// 释放一个Block，实际上没有释放空间，只是从已有空间中写入待分配Index
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::Chunk::Deallocate(void* p, std::size_t blockSize)
		{
			assert(p >= m_pData);

			unsigned char* toRelease = static_cast<unsigned char*>(p);
			// Alignment check
			assert((toRelease - m_pData) % blockSize == 0);

			*toRelease = m_FirstAvailableBlock;
			m_FirstAvailableBlock = static_cast<unsigned char>(
				(toRelease - m_pData) / blockSize);
			// Truncation check
			assert(m_FirstAvailableBlock == (toRelease - m_pData) / blockSize);

			++m_BlocksAvailable;
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::FixedAllocator
		// 创建，大小是固定的
		////////////////////////////////////////////////////////////////////////////////

		FixedAllocator::FixedAllocator(std::size_t blockSize)
			: blockSize_(blockSize)
			, allocChunk_(0)
			, deallocChunk_(0)
		{
			assert(blockSize_ > 0);

			prev_ = next_ = this;

			std::size_t numBlocks = default_chunk_size / blockSize;
			if (numBlocks > UCHAR_MAX) numBlocks = UCHAR_MAX;
			else if (numBlocks == 0) numBlocks = 8 * blockSize;

			numBlocks_ = static_cast<unsigned char>(numBlocks);
			assert(numBlocks_ == numBlocks);
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::FixedAllocator(const FixedAllocator&)
		// Creates a FixedAllocator object of a fixed block size
		////////////////////////////////////////////////////////////////////////////////

		FixedAllocator::FixedAllocator(const FixedAllocator& rhs)
			: blockSize_(rhs.blockSize_)
			, numBlocks_(rhs.numBlocks_)
			, chunks_(rhs.chunks_)
		{
			prev_ = &rhs;
			next_ = rhs.next_;
			rhs.next_->prev_ = this;
			rhs.next_ = this;

			allocChunk_ = rhs.allocChunk_
				? &chunks_.front() + (rhs.allocChunk_ - &rhs.chunks_.front())
				: 0;

			deallocChunk_ = rhs.deallocChunk_
				? &chunks_.front() + (rhs.deallocChunk_ - &rhs.chunks_.front())
				: 0;
		}

		FixedAllocator& FixedAllocator::operator=(const FixedAllocator& rhs)
		{
			FixedAllocator copy(rhs);
			copy.Swap(*this);
			return *this;
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::~FixedAllocator
		////////////////////////////////////////////////////////////////////////////////

		FixedAllocator::~FixedAllocator()
		{
			if (prev_ != this)
			{
				prev_->next_ = next_;
				next_->prev_ = prev_;
				return;
			}

			assert(prev_ == next_);
			TypeChunks::iterator i = chunks_.begin();
			for (; i != chunks_.end(); ++i)
			{
				assert(i->m_BlocksAvailable == numBlocks_);
				i->Release();
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Swap
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::Swap(FixedAllocator& rhs)
		{
			using namespace std;

			swap(blockSize_, rhs.blockSize_);
			swap(numBlocks_, rhs.numBlocks_);
			chunks_.swap(rhs.chunks_);
			swap(allocChunk_, rhs.allocChunk_);
			swap(deallocChunk_, rhs.deallocChunk_);
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Allocate
		// Allocates a block of fixed size
		////////////////////////////////////////////////////////////////////////////////

		void* FixedAllocator::Allocate()
		{
			if (allocChunk_ == 0 || allocChunk_->m_BlocksAvailable == 0)
			{
				TypeChunks::iterator i = chunks_.begin();
				for (;; ++i)
				{
					if (i == chunks_.end())
					{
						// Initialize
						chunks_.reserve(chunks_.size() + 1);
						Chunk newChunk;
						newChunk.Init(blockSize_, numBlocks_);
						chunks_.push_back(newChunk);
						allocChunk_ = &chunks_.back();
						deallocChunk_ = &chunks_.front();
						break;
					}
					if (i->m_BlocksAvailable > 0)
					{
						allocChunk_ = &*i;
						break;
					}
				}
			}
			assert(allocChunk_ != 0);
			assert(allocChunk_->m_BlocksAvailable > 0);

			return allocChunk_->Allocate(blockSize_);
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::Deallocate
		// Deallocates a block previously allocated with Allocate
		// (undefined behavior if called with the wrong pointer)
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::Deallocate(void* p)
		{
			assert(!chunks_.empty());
			assert(&chunks_.front() <= deallocChunk_);
			assert(&chunks_.back() >= deallocChunk_);

			deallocChunk_  = VicinityFind(p);
			assert(deallocChunk_);

			DoDeallocate(p);
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::VicinityFind (internal)
		// Finds the chunk corresponding to a pointer, using an efficient search
		////////////////////////////////////////////////////////////////////////////////

		FixedAllocator::Chunk* FixedAllocator::VicinityFind(void* p)
		{
			assert(!chunks_.empty());
			assert(deallocChunk_);

			const std::size_t chunkLength = numBlocks_ * blockSize_;

			Chunk* lo = deallocChunk_;
			Chunk* hi = deallocChunk_ + 1;
			Chunk* loBound = &chunks_.front();
			Chunk* hiBound = &chunks_.back() + 1;

			for (;;)
			{
				if (lo)
				{
					if (p >= lo->m_pData && p < lo->m_pData + chunkLength)
					{
						return lo;
					}
					if (lo == loBound) lo = 0;
					else --lo;
				}

				if (hi)
				{
					if (p >= hi->m_pData && p < hi->m_pData + chunkLength)
					{
						return hi;
					}
					if (++hi == hiBound) hi = 0;
				}
			}
			assert(false);
			return 0;
		}

		////////////////////////////////////////////////////////////////////////////////
		// FixedAllocator::DoDeallocate (internal)
		// Performs deallocation. Assumes deallocChunk_ points to the correct chunk
		////////////////////////////////////////////////////////////////////////////////

		void FixedAllocator::DoDeallocate(void* p)
		{
			assert(deallocChunk_->m_pData <= p);
			assert(deallocChunk_->m_pData + numBlocks_ * blockSize_ > p);

			// call into the chunk, will adjust the inner list but won't release memory
			deallocChunk_->Deallocate(p, blockSize_);

			if (deallocChunk_->m_BlocksAvailable == numBlocks_)
			{
				// deallocChunk_ is completely free, should we release it? 

				Chunk& lastChunk = chunks_.back();

				if (&lastChunk == deallocChunk_)
				{
					// check if we have two last chunks empty

					if (chunks_.size() > 1 && 
						deallocChunk_[-1].m_BlocksAvailable == numBlocks_)
					{
						// Two free chunks, discard the last one
						lastChunk.Release();
						chunks_.pop_back();
						allocChunk_ = deallocChunk_ = &chunks_.front();
					}
					return;
				}

				if (lastChunk.m_BlocksAvailable == numBlocks_)
				{
					// Two free blocks, discard one
					lastChunk.Release();
					chunks_.pop_back();
					allocChunk_ = deallocChunk_;
				}
				else
				{
					// move the empty chunk to the end
					std::swap(*deallocChunk_, lastChunk);
					allocChunk_ = &chunks_.back();
				}
			}
		}
#endif

		

		
	};
};

////////////////////////////////////////////////////////////////////////////////

