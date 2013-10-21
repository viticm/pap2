
#ifndef DESIGN_EXTRA_STRUCT_DEF
#define DESIGN_EXTRA_STRUCT_DEF

typedef struct _AtlVector3 {
	FLOAT x;
	FLOAT y;
	FLOAT z;
} AtlVector3;


#define DE_MAX_FIELDS 32
typedef struct _AtlObjInfo
{
	BSTR strType;					// "NPC", "Doodad" 等
	LONG iIndex;					// 序号
	LONG iLogicObjPtr;				// 逻辑对象结构体指针，例如NpcInfo*
	LONG iRepresentObjPtr;			// 3D表现obj的指针，在LogicObj里总为pPointer
	LONG bChanged;					// editor用，表示是否需要更新到逻辑数据
	LONG iFieldsCount;				// 值个数
	BSTR strFields[DE_MAX_FIELDS];	// 列
	BSTR strValues[DE_MAX_FIELDS];	// 值
} AtlObjInfo;

#endif
