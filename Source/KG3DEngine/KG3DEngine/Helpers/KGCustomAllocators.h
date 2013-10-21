////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCustomAllocators.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-17 9:34:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGCUSTOMALLOCATORS_H_
#define _INCLUDE_KGCUSTOMALLOCATORS_H_

////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	namespace FAST_ALLOC
	{
		template<typename _Type>
		class index_fast_vec
		{
			struct _TypeBuffer 
			{					
				unsigned char m_Buffer[sizeof(_Type)];
				int m_NextAvailablePlace;
				_TypeBuffer():m_NextAvailablePlace(INT_MAX){}
				bool IsInited()const{return m_NextAvailablePlace < 0;}
				void SetInited(){m_NextAvailablePlace = -1;}
			};
			enum{alloc_step = 4, max_size = INT_MAX -1, invalid_handle = INT_MAX};
			typedef std::vector<_TypeBuffer> TypeCan;
			TypeCan m_vec;
			size_t m_FirstAvailablePos;
			size_t m_Size;
		public:
			enum{gap_between_two_elem = sizeof(_TypeBuffer),};
			typedef size_t handle_type;
		private://暂时封闭拷贝语义
			index_fast_vec(const index_fast_vec& Other);
			index_fast_vec& operator=(const index_fast_vec& Other);
		public:
			index_fast_vec();
			~index_fast_vec(){clear();}

			_Type& DefaultElem()const{static _Type Default; return Default;}

			bool is_valid_handle(handle_type handle)const;
			bool is_error_elem(const _Type& Other)const{return &Other == &DefaultElem();}

			handle_type push(const _Type& Other);
			
			handle_type	get_first_handle()const;
			handle_type get_next_handle(handle_type prevhandle)const;
			_Type&		get(handle_type handle);
			bool	erase(handle_type handle);
			void	clear();
			size_t	size()const {return m_Size;}
			bool	empty()const {return size() == 0;}
			void	reserve(size_t SizeToReserve){m_vec.reserve(SizeToReserve);}
			size_t	capacity()const{return m_vec.capacity();}
		};
#if defined(_DEBUG) | defined(DEBUG)
		struct TesterOf_index_fast_vec
		{
			TesterOf_index_fast_vec();
		};
#endif

		template<typename _Type>
		index_fast_vec<_Type>::index_fast_vec() :m_FirstAvailablePos(0),m_Size(0)
		{
#if defined(_DEBUG) | defined(DEBUG)
			DefaultElem();//初始化了Default的static
#endif
			_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Type)>= sizeof(void*), SizeTwoSmall);
		}

		template<typename _Type>
		bool index_fast_vec<_Type>::is_valid_handle( handle_type handle ) const
		{
			if(handle >= m_vec.size())
				return false;
			if(! m_vec[handle].IsInited())
				return false;
			return true;
		}
		
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		bool index_fast_vec<_Type>::erase( handle_type handle )
		{
			if(! is_valid_handle(handle))
				return false;
			_TypeBuffer& Buffer = m_vec[handle];
			_ASSERTE(Buffer.IsInited());
			_Type* p = reinterpret_cast<_Type*>(Buffer.m_Buffer);
			p->~_Type();
			(void)p;
			_ASSERTE(m_FirstAvailablePos <= INT_MAX);
			Buffer.m_NextAvailablePlace = static_cast<int>(m_FirstAvailablePos);
			m_FirstAvailablePos = handle;
			--m_Size;
			_ASSERTE(m_Size <= m_vec.size());
			_ASSERTE(! Buffer.IsInited());
			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		_Type& index_fast_vec<_Type>::get(handle_type handle)
		{
			if(is_valid_handle(handle))
			{
				_TypeBuffer& Buffer = m_vec[handle];
				if (Buffer.IsInited())
				{
					return *reinterpret_cast<_Type*>(Buffer.m_Buffer);
				}
				
			}			
			return DefaultElem();
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		typename index_fast_vec<_Type>::handle_type index_fast_vec<_Type>::get_first_handle()const
		{
			for (size_t i = 0; i < m_vec.size(); i++)
			{
				if(m_vec[i].IsInited())
					return i;
			}
			return invalid_handle;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		typename index_fast_vec<_Type>::handle_type index_fast_vec<_Type>::get_next_handle( handle_type prevhandle )const
		{
			if(! is_valid_handle(prevhandle))
				return invalid_handle;
			for (size_t i = prevhandle+1; i < m_vec.size(); i++)
			{
				if(m_vec[i].IsInited())
					return i;
			}
			return invalid_handle;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		void index_fast_vec<_Type>::clear()
		{
			for (size_t i = 0; i < m_vec.size(); i++)
			{
				_TypeBuffer& Buffer = m_vec[i];
				if(Buffer.IsInited())
				{
					_Type* p = reinterpret_cast<_Type*>(Buffer.m_Buffer);
					p->~_Type();
					p = NULL;
				}
			}
			m_vec.clear();
			m_Size = 0;
			m_FirstAvailablePos = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		typename index_fast_vec<_Type>::handle_type index_fast_vec<_Type>::push( const _Type& Other )
		{
			if (m_FirstAvailablePos == m_vec.size())
			{
				size_t oldSize = m_vec.size();

				_TypeBuffer Temp;
				KG_PROCESS_ERROR(oldSize + 1 <= max_size);
				Temp.m_NextAvailablePlace = static_cast<int>(oldSize+1);
				m_vec.push_back(Temp);
				//m_FirstAvailablePos = oldSize;//这句不用
			}

			size_t uNowPos = m_FirstAvailablePos;
			_TypeBuffer& Buffer = m_vec[uNowPos];
			size_t uNextPos = static_cast<size_t>(Buffer.m_NextAvailablePlace);

			
			_ASSERTE(! Buffer.IsInited());
			
			_Type* p = new(Buffer.m_Buffer) _Type(Other);
			KG_PROCESS_ERROR(p);
			
			_ASSERTE(! Buffer.IsInited());
			Buffer.SetInited();
			m_FirstAvailablePos = uNextPos;
			++m_Size;
			_ASSERTE(Buffer.IsInited());
#if defined(_DEBUG) | defined(DEBUG)
			if (m_FirstAvailablePos != m_vec.size())
			{
				_ASSERTE(m_FirstAvailablePos < m_vec.size());
				_ASSERTE(! m_vec[m_FirstAvailablePos].IsInited());
			}
#endif
			return uNowPos;
Exit0:
			_KG3D_DEBUG_REPORT("Size Not Enough");
			return 0;
		}
#if 0
		enum{default_chunk_size = 4096, max_small_object_size = 64,};

		class FixedAllocator
		{
		private:	//层0大区块分块，255最大
			class Chunk
			{
				friend FixedAllocator;
			private:
				unsigned char* m_pData;
				unsigned char
					m_FirstAvailableBlock,
					m_BlocksAvailable;
			public:
				Chunk():m_pData(NULL), m_FirstAvailableBlock(0), m_BlocksAvailable(0){}
				Chunk(const Chunk& Other);
				Chunk& operator=(const Chunk& Other);
				void Init(std::size_t blockSize, unsigned char blocks);
				void* Allocate(std::size_t blockSize);
				void Deallocate(void* p, std::size_t blockSize);
				void Reset(std::size_t blockSize, unsigned char blocks);
				void Release();
			};

			//内部函数        
			void DoDeallocate(void* p);
			Chunk* VicinityFind(void* p);	//邻近查找

			//数据
			std::size_t blockSize_;		//Block大小
			unsigned char numBlocks_;	//内含多少Block
			typedef std::vector<Chunk> TypeChunks;	//Chunks序列
			TypeChunks chunks_;	//chunks
			Chunk* allocChunk_;	//保存最近分配的那个Chunk
			Chunk* deallocChunk_;
			//为保证正确的拷贝语义
			mutable const FixedAllocator* prev_;
			mutable const FixedAllocator* next_;

		public:
			//创建一个 FixedAllocator able to manage blocks of 'blockSize' size
			explicit FixedAllocator(std::size_t blockSize = 0);
			FixedAllocator(const FixedAllocator&);
			FixedAllocator& operator=(const FixedAllocator&);
			~FixedAllocator();

			void Swap(FixedAllocator& rhs);

			//分配一个内存Block
			void* Allocate();
			//释放一个内存Block，如果不在空间中，行为不确定
			void Deallocate(void* p);
			//返回BlockSize
			std::size_t BlockSize() const
			{ return blockSize_; }
			//用于std::sort
			bool operator<(std::size_t rhs) const
			{ return BlockSize() < rhs; }
		};
#endif
	};
};
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGCUSTOMALLOCATORS_H_
