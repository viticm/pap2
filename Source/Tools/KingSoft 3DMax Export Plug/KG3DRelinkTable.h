#pragma once


class KG3DBiped;

class KG3DRelinkTable
{
private:
	enum enuUnfoundType
	{
		RELINK_UNFOUND_NONE = 0,
		RELINK_UNFOUND_CHILD = 1,
		RELINK_UNFOUND_PARENT = (1<<1),
		RELINK_FORCE_DWORD = 0xffffffff,	
	};

	enum enuRelinkType
	{
		RELINK_TYPE_CHILD = 0,
		RELINK_TYPE_PARENT = 1,
		RELINK_TYPE_FORCE_DWORD = 0xfffffff,
	};

	struct ReadInData
	{
		std::string strChild;
		std::string strParent;
	};

	struct UnfoundInfo
	{
		int nIndex;
		enuUnfoundType Type;
	};


	struct NodeInfo
	{
		INode* Parent;
		enuRelinkType Type;
		std::set<INode*> Children;
		~NodeInfo();
	};

	typedef std::map<INode*, NodeInfo> RelinkTable;

public:
	KG3DRelinkTable(void);
	~KG3DRelinkTable(void);
	HRESULT LoadFromFile(LPCSTR strFileName);

	HRESULT LoadFromFile(FILE* fpFile);
	INode* GetParentNode(INode* Node);
	
	int GetNumChildren(INode* pNode);

	INode* GetChildNode(INode* pNode, int i);
	
	//Desc: Set a link relative ship between two INode
	void SetLink(INode* pChild, INode* pParent);
	
	//Desc: Clean up
	void CleanUp();

	//Desc: Generate an INode relink table by a kg3dbiped object and information
	//      in ReadIn.
	//return: Num of link failed to set.
	int AjustWithSkeleton(KG3DBiped* pSk);

	int FillReadIn(TCHAR* strInput, ReadInData& Data);
	
	void EraseLink(INode* pNode);

	
private:
	
	
	//Desc: insert a node into relink table
	//Note: It will not fix the link relationship
	void InsertNode(INode* pNode);

	void CopyNodeInfo(NodeInfo& Dest, const NodeInfo& Src);

	RelinkTable m_RelinkTable;

	std::vector<ReadInData> m_ReadIn;
	std::vector<UnfoundInfo> m_UnfoundLink;

	

};
