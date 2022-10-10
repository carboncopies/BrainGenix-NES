import random

FileEnding = '''
DirectionalLights:
  {}
PointLights:
  {}
SpotLights:
  0:
    AssetName: New Spot Light
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 22.3399544
    RotY: 338.475494
    RotZ: 359.943207
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
  1:
    AssetName: New Spot Light - Copy
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 21.8568878
    RotY: 83.4749985
    RotZ: 359.534851
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
  2:
    AssetName: New Spot Light - Copy - Copy
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 202.320557
    RotY: 1.52463949
    RotZ: 180.053711
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
  3:
    AssetName: New Spot Light - Copy - Copy - Copy
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 202.182648
    RotY: 291.524689
    RotZ: 180.146957
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
  4:
    AssetName: New Spot Light - Copy - Copy - Copy - Copy
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 265
    RotY: 0
    RotZ: 0
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
  5:
    AssetName: New Spot Light - Copy - Copy - Copy - Copy - Copy
    AssetType: SpotLight
    ColorRed: 1
    ColorGreen: 1
    ColorBlue: 1
    PosX: 1.00931978
    PosY: 0.877606809
    PosZ: -0.315448433
    RotX: 109.999832
    RotY: 0
    RotZ: 0
    Intensity: 209.5
    MaxDistance: 38
    CutOff: 120
    RollOff: 0
    CastShadows: true
    AttachedScripts:
      {}
SceneCameras:
  {}'''

FileBegin = '''SceneName: Untitled Scene
SceneFormatVersion: 6
ActiveCameraIndex: -1
Models:'''

NumberAssets = 5000
RandMin = -100
RandMax = 100
RandDiv = 10
Output = FileBegin

for x in range(NumberAssets):
    Output += f'''
  {x}:
    AssetName: storage_RC.fbx
    AssetType: Model
    AssetID: 1339095228
    AssetPositionX: {random.randint(RandMin,RandMax)/RandDiv}
    AssetPositionY: {random.randint(RandMin,RandMax)/RandDiv}
    AssetPositionZ: {random.randint(RandMin,RandMax)/RandDiv}
    AssetRotationX: {random.randint(0,360)}
    AssetRotationY: {random.randint(0,360)}
    AssetRotationZ: {random.randint(0,360)}
    AssetScaleX: 0.25
    AssetScaleY: 0.25
    AssetScaleZ: 0.25
    ModelMinLOD: -1
    ModelMaxLOD: 10
    CastDynamicShadows: true
    CastStaticShadows: true
    ReceiveShadows: true
    TreatMissingTexturesAsTransparent: false
    ShaderOverrideIndex: -1
    AttachedScripts:
      0: 0'''

Output += FileEnding

with open("out.ERS", "w") as F:
    F.write(Output)
