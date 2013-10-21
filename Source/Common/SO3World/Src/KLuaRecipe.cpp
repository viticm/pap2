#include "stdafx.h"
#include "KRecipe.h"

DEFINE_LUA_CLASS_BEGIN(KCraftCollection)
	REGISTER_LUA_ENUM(KRecipeBase, CraftType)
    REGISTER_LUA_INTEGER(KCraftCollection, Thew)
	REGISTER_LUA_DWORD(KRecipeBase, ProfessionID)
	REGISTER_LUA_DWORD(KRecipeBase, RequireProfessionLevel)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemType)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemIndex)
	REGISTER_LUA_INTEGER(KRecipeBase, EquipmentType)
	REGISTER_LUA_DWORD(KRecipeBase, Exp)
	REGISTER_LUA_INTEGER(KRecipeBase, PrepareFrame)

	REGISTER_LUA_DWORD(KCraftCollection, DoodadTemplateID)
DEFINE_LUA_CLASS_END(KCraftCollection)

DEFINE_LUA_CLASS_BEGIN(KCraftRecipe)
	REGISTER_LUA_ENUM(KRecipeBase, CraftType)
	REGISTER_LUA_DWORD(KRecipeBase, ProfessionID)
	REGISTER_LUA_DWORD(KRecipeBase, RequireProfessionLevel)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemType)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemIndex)
	REGISTER_LUA_INTEGER(KRecipeBase, EquipmentType)
	REGISTER_LUA_DWORD(KRecipeBase, Exp)
    REGISTER_LUA_INTEGER(KRecipeBase, PrepareFrame)

    REGISTER_LUA_INTEGER(KCraftRecipe, Stamina)
	REGISTER_LUA_STRING(KCraftRecipe, Belong)
	REGISTER_LUA_BOOL(KCraftRecipe, ResultOnLevel)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireBranchID)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireDoodadID)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType1)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex1)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount1)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType2)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex2)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount2)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType3)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex3)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount3)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType4)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex4)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount4)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType5)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex5)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount5)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType6)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex6)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount6)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType7)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex7)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount7)

	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemType8)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemIndex8)
	REGISTER_LUA_DWORD(KCraftRecipe, RequireItemCount8)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType1)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex1)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount1)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount1)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType2)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex2)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount2)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount2)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType3)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex3)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount3)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount3)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType4)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex4)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount4)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount4)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType5)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex5)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount5)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount5)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType6)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex6)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount6)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount6)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType7)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex7)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount7)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount7)

	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemType8)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemIndex8)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMinCount8)
	REGISTER_LUA_DWORD(KCraftRecipe, CreateItemMaxCount8)

    REGISTER_LUA_DWORD(KCraftRecipe, CoolDownID)
DEFINE_LUA_CLASS_END(KCraftRecipe)

DEFINE_LUA_CLASS_BEGIN(KCraftEnchant)
	REGISTER_LUA_ENUM(KRecipeBase, CraftType)
	REGISTER_LUA_INTEGER(KCraftEnchant, Stamina)
	REGISTER_LUA_DWORD(KRecipeBase, ProfessionID)
	REGISTER_LUA_DWORD(KRecipeBase, RequireProfessionLevel)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemType)
	REGISTER_LUA_DWORD(KRecipeBase, ToolItemIndex)
	REGISTER_LUA_INTEGER(KRecipeBase, EquipmentType)
	REGISTER_LUA_DWORD(KRecipeBase, Exp)
	REGISTER_LUA_INTEGER(KRecipeBase, PrepareFrame)

	REGISTER_LUA_STRING(KCraftEnchant, Name)
	REGISTER_LUA_STRING(KCraftEnchant, Belong)
	REGISTER_LUA_DWORD(KCraftEnchant, EnchantID)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireBranchID)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireDoodadID)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType1)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex1)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount1)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType2)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex2)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount2)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType3)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex3)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount3)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType4)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex4)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount4)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType5)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex5)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount5)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType6)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex6)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount6)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType7)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex7)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount7)

	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemType8)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemIndex8)
	REGISTER_LUA_DWORD(KCraftEnchant, RequireItemCount8)
DEFINE_LUA_CLASS_END(KCraftEnchant)

DEFINE_LUA_CLASS_BEGIN(KCraftRead)
    REGISTER_LUA_ENUM(KRecipeBase, CraftType)
    REGISTER_LUA_DWORD(KRecipeBase, ProfessionID)
    REGISTER_LUA_DWORD(KRecipeBase, RequireProfessionLevel)
    REGISTER_LUA_DWORD(KRecipeBase, ToolItemType)
    REGISTER_LUA_DWORD(KRecipeBase, ToolItemIndex)
    REGISTER_LUA_INTEGER(KRecipeBase, EquipmentType)
    REGISTER_LUA_DWORD(KRecipeBase, Exp)
    REGISTER_LUA_INTEGER(KRecipeBase, PrepareFrame)
    
    REGISTER_LUA_DWORD(KCraftRead, ID)
    REGISTER_LUA_DWORD(KCraftRead, SubID)
    
    REGISTER_LUA_INTEGER(KCraftRead, Stamina)
    REGISTER_LUA_INTEGER(KCraftRead, PlayerExp)

    REGISTER_LUA_DWORD(KCraftRead, ExtendProfessionID1)
    REGISTER_LUA_DWORD(KCraftRead, ExtendExp1)
    REGISTER_LUA_DWORD(KCraftRead, ExtendProfessionID2)
    REGISTER_LUA_DWORD(KCraftRead, ExtendExp2)
    REGISTER_LUA_DWORD(KCraftRead, CreateItemTab)
    REGISTER_LUA_DWORD(KCraftRead, CreateItemIndex)
    REGISTER_LUA_INTEGER(KCraftRead, StackNum)
    REGISTER_LUA_DWORD(KCraftRead, BuffID)
    REGISTER_LUA_DWORD(KCraftRead, BuffLevel)
    REGISTER_LUA_INTEGER(KCraftRead, Train)

    REGISTER_LUA_STRING(KCraftRead, Name)
DEFINE_LUA_CLASS_END(KCraftRead)


DEFINE_LUA_CLASS_BEGIN(KCraftCopy)
    REGISTER_LUA_ENUM(KRecipeBase, CraftType)
    REGISTER_LUA_DWORD(KRecipeBase, ProfessionID)
    REGISTER_LUA_DWORD(KRecipeBase, RequireProfessionLevel)
    REGISTER_LUA_DWORD(KRecipeBase, ToolItemType)
    REGISTER_LUA_DWORD(KRecipeBase, ToolItemIndex)
    REGISTER_LUA_INTEGER(KRecipeBase, EquipmentType)
    REGISTER_LUA_DWORD(KRecipeBase, Exp)
    REGISTER_LUA_INTEGER(KRecipeBase, PrepareFrame)

    REGISTER_LUA_INTEGER(KCraftCopy, Thew)
    REGISTER_LUA_DWORD(KCraftCopy, CoolDownID)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemType1)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemIndex1)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemCount1)

    REGISTER_LUA_DWORD(KCraftCopy, RequireItemType2)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemIndex2)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemCount2)

    REGISTER_LUA_DWORD(KCraftCopy, RequireItemType3)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemIndex3)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemCount3)

    REGISTER_LUA_DWORD(KCraftCopy, RequireItemType4)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemIndex4)
    REGISTER_LUA_DWORD(KCraftCopy, RequireItemCount4)

    REGISTER_LUA_DWORD(KCraftCopy, CreateItemType)
    REGISTER_LUA_DWORD(KCraftCopy, CreateItemIndex)
    REGISTER_LUA_DWORD(KCraftCopy, ProfessionIDExt)
	REGISTER_LUA_DWORD(KCraftCopy, RequireProfessionLevelExt)
    REGISTER_LUA_INTEGER(KCraftCopy, RequirePlayerLevel)
DEFINE_LUA_CLASS_END(KCraftCopy)
