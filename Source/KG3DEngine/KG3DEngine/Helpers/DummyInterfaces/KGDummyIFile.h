////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGDummyIFile.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-2 14:23:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGDUMMYIFILE_H_
#define _INCLUDE_KGDUMMYIFILE_H_

////////////////////////////////////////////////////////////////////////////////
class KGDummyIFile : public IFile
{
public:
	// 读取文件数据
	virtual unsigned long	Read(void* Buffer, unsigned long ReadBytes) {return ReadBytes;}
	// 写入文件数据
	virtual unsigned long	Write(const void* Buffer, unsigned long WriteBytes) {return WriteBytes;}
	// 获得文件内容Buffer,由iFile接口负责释放这个缓冲区，外部应用不可自行释放它。
	virtual void*			GetBuffer() {return 0;}
	// 移动文件指针位置，Origin	-> Initial position: SEEK_CUR SEEK_END SEEK_SET
	virtual long	Seek(long Offset, int Origin) {return 0;}
	// 取得文件指针位置,失败返回-1。
	virtual long	Tell() {return 0;}
	// 取得文件长度,失败返回0。
	virtual unsigned long	Size() {return 0;}
	// 判断打开的是否是包中的文件,返回布尔值
	virtual int		IsFileInPak() {return 0;}

	//判断文件是否分块压缩的
	virtual int		IsPackedByFragment() {return 0;}
	//获取文件分块的数目
	virtual int		GetFragmentCount() {return 0;}
	//获取分块的大小
	virtual unsigned int	GetFragmentSize(int nFragmentIndex) {return 0;}
	//读取一个文件分块，传入的pBuffer为目标缓冲区，如果传入的缓冲区指针为空，则内部会分配新的缓冲区，并传出指针，外部需要负责销毁。
	//缓冲区的大小需要能容纳下分块的内容，可以通过GetFragmentCount知道每个分块的数据大小，缓冲区必须至少要有这么大。
	//返回值表示实际读取出来的这个分块的数据大小,如果操作失败(包括分块不存在,子文件不是按分块方式存储等等)返回0。
	virtual unsigned long	ReadFragment(int nFragmentIndex, void*& pBuffer) {return 0;}

	// 关闭打开的文件
	virtual void	Close() {}
	// 接口对象销毁
	virtual void	Release() {}

	virtual ~KGDummyIFile() {};
};

#endif //_INCLUDE_KGDUMMYIFILE_H_
