#ifndef KG3DENGINEOPTION_H
#define KG3DENGINEOPTION_H

#include "../../../Include/KG3DEngine/KG3DInterface.h"


HRESULT ValidateTextureSamplerFilter(KG3DEngineOption& Option);
HRESULT ValidateTextureMaxAnisotory(KG3DEngineOption& Option);

HRESULT ValidateTextureOption(KG3DEngineOption& Option);
HRESULT ValidateCameraOption(KG3DEngineOption& Option);

HRESULT ValidateBloomOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateHDROption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateShockWaveOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateHSIOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateMotionBlurOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateDetailOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateGodRayOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateSSAOOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateGlobalFogOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidateCurveCMYKOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);
HRESULT ValidatePostEffectOption(KG3DEngineOption& Option);
HRESULT ValidateDOFOption(D3DCAPS9 D3DCaps, KG3DEngineOption& Option);

HRESULT ValidateModelShadowTypeOption(KG3DEngineOption& Option);

HRESULT ValidateGrassAnimationOption(KG3DEngineOption& Option);
HRESULT ValidateGrassAlphaBlendOption(KG3DEngineOption& Option);

HRESULT ValidateWaterReflectionOption(KG3DEngineOption& Option);
HRESULT ValidateWaterReflectEnvironmentOption(KG3DEngineOption& Option);
HRESULT ValidateWaterReflectGroundOption(KG3DEngineOption& Option);
HRESULT ValidateWaterReflectNPCOption(KG3DEngineOption& Option);
HRESULT ValidateWaterReflectEntityOption(KG3DEngineOption& Option);
HRESULT ValidateWaterRefractionOption(KG3DEngineOption& Option);
HRESULT ValidateWaterOption(KG3DEngineOption& Option);

HRESULT ValidateWaterDetailOption(KG3DEngineOption& Option);
HRESULT ValidateVegetationDensityOption(KG3DEngineOption& Option);
HRESULT ValidateCpuSkinOption(KG3DEngineOption& Option);
HRESULT ValidateNumCpuThreadOption(KG3DEngineOption& Option);
HRESULT ValidateTerrainDetailOption(KG3DEngineOption& Option);

HRESULT ValidateEngineOption(KG3DEngineOption& Option);
HRESULT ValidateAfterInitDeviceEngineOption(KG3DEngineOption& Option);

#endif // KG3DENGINEOPTION_H
