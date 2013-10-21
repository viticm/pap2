# Microsoft Developer Studio Project File - Name="SceneEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SceneEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SceneEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SceneEditor.mak" CFG="SceneEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SceneEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SceneEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ENGINE/Tools/SceneEditor", MDJAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SceneEditor - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx9.lib strmbasd.lib d3d9.lib dinput8.lib winmm.lib dxerr8.lib uuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib d3dx8dt.lib d3dxof.lib d3d8.lib dxguid.lib strmiids.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SceneEditor - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9.lib strmbasd.lib d3d9.lib dinput8.lib winmm.lib dxerr8.lib uuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib d3dx8dt.lib d3dxof.lib d3d8.lib dxguid.lib strmiids.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SceneEditor - Win32 Release"
# Name "SceneEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_frmframe.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\CntrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_GroundTool.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_MainTool.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphiceEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphicTool.cpp
# End Source File
# Begin Source File

SOURCE=.\KColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_MeshBasePropage.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_PorItem_STLString.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Ani.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Color.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Enum.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Float.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Int.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlList.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlOption.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlState.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlTex.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_SceneObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_SFXNew.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_String.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Terrain_Ground_Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Vec3.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItemBase.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_PropertyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_PropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\KDlg_SpeedTree.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageColor.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageMapTest.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageView.cpp
# End Source File
# Begin Source File

SOURCE=.\KEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\KFileFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelEditorMaterialAlphaOperatorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelSceneSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelSFXBillBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelSpeedTree.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelSubd.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelTerrainLOD.cpp
# End Source File
# Begin Source File

SOURCE=.\KModelWater.cpp
# End Source File
# Begin Source File

SOURCE=.\KMovieTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\KObjectEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\KPhysicsSimulator.cpp
# End Source File
# Begin Source File

SOURCE=.\KRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\KRepGirlNo1.cpp
# End Source File
# Begin Source File

SOURCE=.\KRepNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\KRigidBody.cpp
# End Source File
# Begin Source File

SOURCE=.\KRigidBodyTable.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneEditBaseSelectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorAnimationPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorMeshBindDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorMeshSocketSetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorModelSaveDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPanelView.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXBillBoardDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXBladeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXNewDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXSceneSFXDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXSpeedTreeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSword2CheckTool.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPageModel.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPanelView.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPropertyPageBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogDistrict.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogGroundTextureMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogNewMapWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogObjectMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogOranment.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogTerrian.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorObscaleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorPanelDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorRadarView.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorUnitPreviewView.cpp
# End Source File
# Begin Source File

SOURCE=.\KSceneSwordTestWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\MathTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshBillBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshBlade.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshButton.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshColorPick.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshGlare.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshItem.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshLineGrass.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMainTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMeshAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMeshMaterialTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMeshSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMeshTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMoveGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshObjectTableTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshRotateGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshScreenSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshSkyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshSlideBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshTerran.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshWinBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Operation.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceMgrEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\RInterface.cpp

!IF  "$(CFG)" == "SceneEditor - Win32 Release"

!ELSEIF  "$(CFG)" == "SceneEditor - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneEditBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneEditor.rc
# End Source File
# Begin Source File

SOURCE=.\SceneEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneMeshEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneSkinEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneSwordTest.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneTable.cpp
# End Source File
# Begin Source File

SOURCE=.\SFXBase.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SwordTestDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SwordTestView.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureTable.cpp
# End Source File
# Begin Source File

SOURCE=.\TimePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\XMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\XMeshTable.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_frmframe.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CntrItem.h
# End Source File
# Begin Source File

SOURCE=.\DataObject.h
# End Source File
# Begin Source File

SOURCE=.\DLG_GroundTool.h
# End Source File
# Begin Source File

SOURCE=.\DLG_MainTool.h
# End Source File
# Begin Source File

SOURCE=.\DLLBase.h
# End Source File
# Begin Source File

SOURCE=.\GraphiceEngine.h
# End Source File
# Begin Source File

SOURCE=.\GraphicTool.h
# End Source File
# Begin Source File

SOURCE=.\IAudio.h
# End Source File
# Begin Source File

SOURCE=.\IAudioManager.h
# End Source File
# Begin Source File

SOURCE=.\IMusicApplication.h
# End Source File
# Begin Source File

SOURCE=.\KColorButton.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_Mesh.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_MeshBasePropage.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_PorItem_STLString.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Ani.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Color.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Enum.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Float.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Int.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlEffect.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlList.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlOption.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlState.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_MtlTex.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Particle.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_SceneObjects.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_SFXNew.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_String.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Terrain_Ground_Texture.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItem_Vec3.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_ProItemBase.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_PropertyDialog.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_PropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\KDlg_SpeedTree.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingDialog.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageColor.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageGeneral.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageMapTest.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageMusic.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageVideo.h
# End Source File
# Begin Source File

SOURCE=.\KEditorSettingPageView.h
# End Source File
# Begin Source File

SOURCE=.\KEntity.h
# End Source File
# Begin Source File

SOURCE=.\KFileFinder.h
# End Source File
# Begin Source File

SOURCE=.\KModelEditorMaterialAlphaOperatorDialog.h
# End Source File
# Begin Source File

SOURCE=.\KModelSceneSFX.h
# End Source File
# Begin Source File

SOURCE=.\KModelSFXBillBoard.h
# End Source File
# Begin Source File

SOURCE=.\KModelSpeedTree.h
# End Source File
# Begin Source File

SOURCE=.\KModelSubd.h
# End Source File
# Begin Source File

SOURCE=.\KModelTerrainLOD.h
# End Source File
# Begin Source File

SOURCE=.\KModelWater.h
# End Source File
# Begin Source File

SOURCE=.\KMovieTexture.h
# End Source File
# Begin Source File

SOURCE=.\KObjectEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\KPhysicsSimulator.h
# End Source File
# Begin Source File

SOURCE=.\KRegion.h
# End Source File
# Begin Source File

SOURCE=.\KRepGirlNo1.h
# End Source File
# Begin Source File

SOURCE=.\KRepNPC.h
# End Source File
# Begin Source File

SOURCE=.\KRigidBody.h
# End Source File
# Begin Source File

SOURCE=.\KRigidBodyTable.h
# End Source File
# Begin Source File

SOURCE=.\KSceneEditBaseSelectDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorAnimationPanel.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorMeshBindDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorMeshSocketSetDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorModelSaveDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPanelView.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageAnimation.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageMaterial.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertyPageMesh.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXBillBoardDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXBladeDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXNewDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXSceneSFXDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSFXSpeedTreeDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneModelEditorSword2CheckTool.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditor.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPageModel.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPanelView.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPropertyPageBlock.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorView.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectEditorWizard.h
# End Source File
# Begin Source File

SOURCE=.\KSceneObjectFrame.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogCamera.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogDistrict.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogGroundTextureMgr.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogNewMapWizard.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogObjectMgr.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogOranment.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogTerrian.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorDialogUnit.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorObscaleDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorPanelDialog.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorRadarView.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSceneEditorUnitPreviewView.h
# End Source File
# Begin Source File

SOURCE=.\KSceneSwordTestWizard.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Manager.h
# End Source File
# Begin Source File

SOURCE=.\MathTool.h
# End Source File
# Begin Source File

SOURCE=.\MeshBillBoard.h
# End Source File
# Begin Source File

SOURCE=.\MeshBlade.h
# End Source File
# Begin Source File

SOURCE=.\MeshButton.h
# End Source File
# Begin Source File

SOURCE=.\MeshColorPick.h
# End Source File
# Begin Source File

SOURCE=.\MeshComboBox.h
# End Source File
# Begin Source File

SOURCE=.\MeshEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MeshEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\MeshEditorView.h
# End Source File
# Begin Source File

SOURCE=.\MeshFlare.h
# End Source File
# Begin Source File

SOURCE=.\MeshGlare.h
# End Source File
# Begin Source File

SOURCE=.\MeshGroup.h
# End Source File
# Begin Source File

SOURCE=.\MeshItem.h
# End Source File
# Begin Source File

SOURCE=.\MeshLineGrass.h
# End Source File
# Begin Source File

SOURCE=.\MeshListBox.h
# End Source File
# Begin Source File

SOURCE=.\MeshListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MeshMainTool.h
# End Source File
# Begin Source File

SOURCE=.\MeshMeshAnimation.h
# End Source File
# Begin Source File

SOURCE=.\MeshMeshMaterialTool.h
# End Source File
# Begin Source File

SOURCE=.\MeshMeshSelect.h
# End Source File
# Begin Source File

SOURCE=.\MeshMeshTool.h
# End Source File
# Begin Source File

SOURCE=.\MeshMoveGrid.h
# End Source File
# Begin Source File

SOURCE=.\MeshObjectTableTool.h
# End Source File
# Begin Source File

SOURCE=.\MeshParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\MeshPropertySet.h
# End Source File
# Begin Source File

SOURCE=.\MeshRotateGrid.h
# End Source File
# Begin Source File

SOURCE=.\MeshScreenSFX.h
# End Source File
# Begin Source File

SOURCE=.\MeshSkyBox.h
# End Source File
# Begin Source File

SOURCE=.\MeshSlideBar.h
# End Source File
# Begin Source File

SOURCE=.\MeshStrip.h
# End Source File
# Begin Source File

SOURCE=.\MeshTerran.h
# End Source File
# Begin Source File

SOURCE=.\MeshTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MeshWinBase.h
# End Source File
# Begin Source File

SOURCE=.\MusicApplication.h
# End Source File
# Begin Source File

SOURCE=.\ObjectTable.h
# End Source File
# Begin Source File

SOURCE=.\Operation.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\ResourceMgrEffect.h
# End Source File
# Begin Source File

SOURCE=.\RInterface.h
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# Begin Source File

SOURCE=.\SceneEditBase.h
# End Source File
# Begin Source File

SOURCE=.\SceneEditor.h
# End Source File
# Begin Source File

SOURCE=.\SceneEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\SceneEditorView.h
# End Source File
# Begin Source File

SOURCE=.\SceneMeshEditor.h
# End Source File
# Begin Source File

SOURCE=.\SceneSkinEdit.h
# End Source File
# Begin Source File

SOURCE=.\SceneSwordTest.h
# End Source File
# Begin Source File

SOURCE=.\SceneTable.h
# End Source File
# Begin Source File

SOURCE=.\SFXBase.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\SwordTestDoc.h
# End Source File
# Begin Source File

SOURCE=.\SwordTestView.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TextureTable.h
# End Source File
# Begin Source File

SOURCE=.\TimePoint.h
# End Source File
# Begin Source File

SOURCE=.\Types.h
# End Source File
# Begin Source File

SOURCE=.\XMesh.h
# End Source File
# Begin Source File

SOURCE=.\XMeshTable.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmapDrop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmaproll.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\maintool.bmp
# End Source File
# Begin Source File

SOURCE=.\mdi.ico
# End Source File
# Begin Source File

SOURCE=.\res\SceneEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\SceneEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SceneEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# End Group
# Begin Group "FileOperation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Public\KIniFile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section SceneEditor : {96BBD80A-EF9F-4911-81AB-CDB9922B2FCF}
# 	2:5:Class:C_frmFrame
# 	2:10:HeaderFile:_frmframe.h
# 	2:8:ImplFile:_frmframe.cpp
# End Section
# Section SceneEditor : {5BBFCB3B-74CC-49F5-B0AB-3CC9107BAFCB}
# 	2:21:DefaultSinkHeaderFile:_frmframe.h
# 	2:16:DefaultSinkClass:C_frmFrame
# End Section
