#pragma once

class KG3DHelper
{
public:
	KG3DHelper(void);
	~KG3DHelper(void);
	static TriObject* GetTriObjectFromNode(INode* pNode, TimeValue t, BOOL &bNeedDel);
	static Modifier* FindSkinModifier(INode* pNode);
	static Modifier* FindPhysiqueModifier(INode* pNode);
	static void ChangeMatrixYZ(Matrix3* mat);
private:
	void Swap(float& f1, float& f2);
};

KG3DHelper& GetHelper();

