# -*- coding: utf-8 -*-

CVS_EXE = r"C:\Program Files\TortoiseCVS\cvs.exe"
CVS_ROOT = ':pserver:liqing@192.168.22.46:2401/SourceCode'

GameWorldBase = ur'E:\jx3src-test\Sword3\Source\Common\SO3World\src'
GameWorldFiles = [
    u'KBaseFuncList.cpp',
    u'KShopFuncList.cpp',
    u'KLuaCell.cpp',
    u'KLuaCharacter.cpp',
    u'KLuaConstList.cpp',
    u'KLuaDoodad.cpp',
    u'KLuaDoodadTemplate.cpp',
    u'KLuaItem.cpp',
    u'KLuaNpc.cpp',
    u'KLuaPlayer.cpp',
    u'KLuaQuestInfo.cpp',
    u'KLuaScene.cpp',
    u'KLuaShop.cpp',
    u'KLuaSkill.cpp',
    u'KLuaProfession.cpp',
    u'KLuaRecipe.cpp'
    ]

CHECK_FILES = [(GameWorldBase, GameWorldFiles)]

__all__ = ["CHECK_FILES", 'CVS_EXE', 'CVS_ROOT']
