#include "header.h"
#include "KG3DHelper.h"

KG3DHelper& GetHelper()
{
	static KG3DHelper g_Helper;
	return g_Helper;
}

KG3DHelper::KG3DHelper(void)
{
}

KG3DHelper::~KG3DHelper(void)
{
}


TriObject* KG3DHelper::GetTriObjectFromNode(INode *node, TimeValue t, BOOL &bNeedDel)
{
	bNeedDel = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{ 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri) 
		{
			bNeedDel = TRUE;
		}
		return tri;
	}
	else
	{
		return NULL;
	}
}

Modifier* KG3DHelper::FindSkinModifier(INode* pNode)
{
	Object* ObjectPtr = pNode->GetObjectRef();
	if (NULL == ObjectPtr)
	{
		return NULL;
	}

	if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
			Class_ID clsid = ModifierPtr->ClassID();

			if (ModifierPtr->ClassID() == SKIN_CLASSID)
			{
				return ModifierPtr;
			}
			ModStackIndex++;
		}
	}
	return NULL;
}

Modifier* KG3DHelper::FindPhysiqueModifier(INode* pNode)
{
	Object* ObjectPtr = pNode->GetObjectRef();
	if ( NULL == ObjectPtr)
	{
		return NULL;
	}

	if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
			Class_ID clsid = ModifierPtr->ClassID();
			if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				return ModifierPtr;
			}
			ModStackIndex++;
		}
	}
	return NULL;
}

void KG3DHelper::ChangeMatrixYZ(Matrix3* mat)
{
	Point3 RowY = mat->GetRow(1);
	Point3 RowZ = mat->GetRow(2);
	mat->SetRow(1, RowZ);
	mat->SetRow(2, RowY);
	Point4 ColY = mat->GetColumn(1);
	Point4 ColZ = mat->GetColumn(2);
	mat->SetColumn(1, ColZ);
	mat->SetColumn(2, ColY);
}