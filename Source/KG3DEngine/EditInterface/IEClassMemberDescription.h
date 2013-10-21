////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEClassMemberDescription.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-30 14:20:59
/*  Comment     : 规范化的结构体成员描述。用IEClassMemberDescription声明一个静态
数组，然后用这个数组初始化一个IEClassMemberDescriptionPack，然后传到外面，如：
IEClassMemberDescriptionPack	GetMemberDescrption()const
{
	typedef KG3DBillboardCloudCreaterThresholdStruct MyType;
	static IEClassMemberDescription Desc[] = 
	{
		{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxNormalBias), NULL, _T("最大Normal偏移/弧度")},
		{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxNormalDistanceBiasPercent), NULL, _T("Normal方向最大距离偏移百分比")},
		{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxDistanceToPoint), NULL, _T("点到点最大距离偏移百分比")},
		{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMinAreaInfluenceAtNormalCorrection), NULL, _T("最小面积权值")},
		{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMinAreaAccepted), NULL, _T("绝对最小面积")},
	};
	static IEClassMemberDescriptionPack Pack(Desc, _countof(Desc), this);
	return Pack;
}
这样外面就可以用这些数据初始化界面了。而结构体改动的时候，只要改动这些描述，UI的代码不用做任何改动，
能够自动生成界面
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IECLASSMEMBERDESCRIPTION_H_
#define _INCLUDE_IECLASSMEMBERDESCRIPTION_H_

////////////////////////////////////////////////////////////////////////////////
struct IEClassMemberDescription 
{
	DWORD	m_Type;
	size_t	m_Size;
	size_t	m_Offset;
	LPCTSTR m_MemberName;	//成员的原名，可以是NULL
	LPCTSTR m_MemberTitle;	//成员的名字的意思，用在界面，应该简洁，最好用中文，可以为NULL
};
//初始化这个必须这样 static IEClassMemberDescriptionPack stPack(stDesc, _countof(stDesc), this);
//初始化函数会帮你做必要的检查
struct IEClassMemberDescriptionPack 
{
	enum{max_num = 1024,};
	const IEClassMemberDescription* m_pDesc;
	size_t	m_NumberOfMembers;

	IEClassMemberDescriptionPack():m_pDesc(NULL), m_NumberOfMembers(0){}

	template<class _Type>
	IEClassMemberDescriptionPack(const IEClassMemberDescription* pDesc, size_t num, const _Type* pType)//num不能超过max_num
		:m_pDesc(pDesc)
		,m_NumberOfMembers(num)
	{
		_ASSERTE(NULL != pDesc && num <= max_num);
		//帮你检查合法性
		if(num > 1)
		{
			for(size_t i = 1; i < num; ++i)
			{
				_ASSERTE(pDesc[i-1].m_Offset < pDesc[i].m_Offset);
			}
		}
#if defined(DEBUG) | defined(_DEBUG)
		size_t Bound = pDesc[num-1].m_Offset + pDesc[num-1].m_Size;
		_ASSERTE(Bound <= sizeof(_Type));
#endif		
	}

	friend bool operator==(const IEClassMemberDescriptionPack& A1
		, const IEClassMemberDescriptionPack& A2)
	{
		return 0 == memcmp(&A1, &A2, sizeof(IEClassMemberDescriptionPack));
	}
	friend bool operator!=(const IEClassMemberDescriptionPack& A1
		, const IEClassMemberDescriptionPack& A2)
	{
		return ! (A1 == A2);
	}
};

#endif //_INCLUDE_IECLASSMEMBERDESCRIPTION_H_
