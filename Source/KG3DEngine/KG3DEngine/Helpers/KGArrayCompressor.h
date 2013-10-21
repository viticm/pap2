////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGArrayCompressor.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:35:26
//  Comment     : 手工写的数组压缩。方法是把数组中连续的相同元素压缩成{元素，个数}的方式
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGARRAYCOMPRESSOR_H_
#define _INCLUDE_KGARRAYCOMPRESSOR_H_
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	class ArrayCompressor
	{
	public:
		enum
		{
			MAX_WIDTH = 4096,	//256*16	//最大的接受的宽
			MAX_HEIGHT = MAX_WIDTH,			//最大的高
			MAX_ELEMENT = SHRT_MAX*4,		//最多不超过每行10个Element
			MAX_DIFF_DATA = 256,			//最大只能有256个不同的元素
		};
		struct Element 
		{
			UCHAR m_ucData;
			UCHAR m_ucCount;
		};
	public:
		ArrayCompressor();
		~ArrayCompressor();

	private:
		Element* m_cDataArray;
		INT* m_pRowIndexArray;
		DWORD*	m_cDiffDataArray;
		INT m_nDiffDataCount;
		INT m_nElementCount;///并不一定就是m_cDataArray分配的空间的大小
		INT m_nWidth;
		INT m_nHeigth;
		UCHAR m_ucErrData;
		UCHAR m_ucDefaultData;///用于替换ErrData
		INT m_nDiffDataEnumer;
		std::tstring	m_strError;
	private:
		BOOL GenerateDiffDataArray();	//整理生成不同元素的序列，只有编辑器会需要这个
		void ClearDiffArray();
		BOOL RemoveErrData();			
		BOOL ChangeData(UCHAR uDataToChange, UCHAR uNewData);	//把某个元素替换成另外一个元素
		//<用于清除孤立像素的代码>
	public:
		enum{IsIsoFalse = -1, IsNotIso = 0, IsIso = 1,};
		enum{DirUpper = 0, DirLower = 1, DirLeft = 2, DirRight = 3,};///数组相关，不要改
		enum{UseUpper = 0, UseLower = 1, UseBothUpperLower = 2,};
	private:
		Element* GetRowElementWhenEliminating(INT nRow, INT nColumn);
		INT IsIsoVerticalHelper(TNumToType<UseUpper>,			INT nRow, INT nColumn, UCHAR uCurrData, UCHAR& uUpperData, UCHAR& uLowerData);
		INT IsIsoVerticalHelper(TNumToType<UseLower>,			INT nRow, INT nColumn, UCHAR uCurrData, UCHAR& uUpperData, UCHAR& uLowerData);
		INT IsIsoVerticalHelper(TNumToType<UseBothUpperLower>,	INT nRow, INT nColumn, UCHAR uCurrData, UCHAR& uUpperData, UCHAR& uLowerData);
		template<size_t uUsage>
		inline INT IsIsoVertical(INT nRow, INT nColumn, UCHAR uCurrData, UCHAR& uUpperData, UCHAR& uLowerData)
		{
			return IsIsoVerticalHelper(TNumToType<uUsage>(), nRow, nColumn, uCurrData, uUpperData, uLowerData);
		}
		BOOL EliminateIsolatedData(INT nIsoCount);///邻近多少个像素是孤立的，删除掉，用于删除点采样造成的边沿碎片
		BOOL EliminateIsolatedValue(INT nTotalValue);///仅保留多少Value；
		//</用于清除孤立像素的代码>

		inline INT GetRowBeginIndex( INT nRow );//第N行从第几元素开始
		inline INT GetRowCount(INT nRow);		//第N行有几个元素
	public:
		UINT GetMemorySize(){return sizeof(ArrayCompressor) + m_cDataArray ? sizeof(Element)*m_nElementCount : 0 + m_pRowIndexArray? sizeof(INT) * (m_nHeigth+1) : 0;}//得到内存占用，用于估计空间
		BOOL Compress(const UCHAR* cInputRawArray, INT nHeight, INT nWidth, INT nTolerance = 0, INT nIsoCount = 0, INT nTotalValue = 0);	//压缩的主函数
		BOOL GetDiffDataArray(DWORD* pData, INT nBufferCount);	//得到不同元素的序列
		BOOL IsDataArrayGenerated();	//压缩数组是否生成
		BOOL IsDiffDataArrayGenerated();	//不同元素序列是否生成
		INT	GetDiffDataArrayCount();
		void Clear();

		BOOL SetErrorDate(UCHAR cErr);
		UCHAR GetErrorData(){return m_ucErrData;}
		UCHAR GetDefaultData() const { return m_ucDefaultData; }
		BOOL SetDefaultData(UCHAR val);

		UCHAR GetData(INT nRow, INT nColumn);	//查找元素，错误的时候返回DefaultData
		BOOL GetData(INT nRow, INT nColumn, INT& nRet);	//查找元素，错误返回FALSE
		INT GetHeight(){return m_nHeigth;}
		INT GetWidth(){return m_nWidth;}

		std::tstring&	GetErrorString(){return m_strError;}
		
		BOOL PushDataInBuffer(UCHAR* pBuffer, INT nBufferSize);	//把数据还原成二维数组
#if defined(DEBUG) | defined(_DEBUG)
		VOID DebugPrintByRow();
		VOID DebugPrintRawCompressedData();
		VOID DebugPrintRawArray();
#endif
	};
}
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGARRAYCOMPRESSOR_H_
