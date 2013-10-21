
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
屏幕特效自动化测试工具
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

【应用程序名称】KGPostRenderTest.exe

【功能简介】

	工具从用例配置文件（CaseConfig.ini）中读取测试用例数据，针对每个测试用例加载指定场景，设置屏幕特效进行场景渲染，收集特效设置信息、指定帧的数据和截图，保存到测试结果输出目录下。

【程序运行依赖项】
	
	KG3DEngine_AutoTest.dll
	KG3DShaderLibrary.dll
	KG3DSound.dll
	Engine_Lua5.dll
	Lua5.dll

【工具输出】

	Log文件：应用程序所在目录\Logs\运行日期\运行时间.log
	测试结果输出目录：应用程序所在目录\Logs\运行日期\运行时间\

【用例配置说明】

	文件名：CaseConfig.ini

	[DefaultSetting]			#设置默认的摄像机位置和朝向
	TestMap=				#默认测试场景(相对路径，如data\source\maps\TestMap\TestMap.map)
	TotalFrame=1000			#默认测试运行总帧数
	CameraPosX=11.11			#默认摄像机位置
	CameraPosY=22.22
	CameraPosZ=33.33
	LookAtPosX=44.44			#默认朝向位置
	LookAtPosY=55.55
	LookAtPosZ=66.66

	[Case_1]				#单个用例的数据（Case_n）
	TestMap=				#测试场景（无此项时使用默认位置）
	TotalFrame=1000			#测试运行总帧数
	EffectType_1=HDR			#测试的特效类型（可处理多种特效混合，未完善）
	TestFrame_1=200			#第1测试帧数
	CameraPosX_1=10.10		#第1测试帧的摄像机位置（无此项时使用默认位置）
	CameraPosY_1=10.20
	CameraPosZ_1=10.30
	LookAtPosX_1=10.40		#第1测试帧的朝向位置（无此项时使用默认位置）
	LookAtPosY_1=10.50
	LookAtPosZ_1=10.60
	TestFrame_2=400			#第2测试帧数
	CameraPosX_2=20.10		#第2测试帧的摄像机位置（无此项时使用上一测试帧的设置）
	CameraPosY_2=20.20
	CameraPosZ_2=20.30
	LookAtPosX_2=20.40		#第2测试帧的朝向位置（无此项时使用上一测试帧的设置）
	LookAtPosY_2=20.50
	LookAtPosZ_2=20.60

【相关文档】

	http://jx3web/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-3DEngine-PostRenderSemiautomaticTesting

＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
地图场景查看工具
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

【应用程序名称】KGSceneViewer.exe

【功能简介】

	通过菜单的打开功能，加载指定场景，控制摄像机在场景中游走，调整视角查看场景。开启屏幕特效的相关功能。复制摄像机信息到剪切板，用于设置测试屏幕特效的场景位置。

	控制方式：
	ASDW：		摄像机在场景中游走
	鼠标右键：	调整视角
	C：		复制当前摄像机信息到剪切板
	P：		截图
	1：		启用/禁用HDR
	2：		启用/禁用Shadow
	3：		启用/禁用GodRay
	4：		启用/禁用Aoe
	5：		启用/禁用ShockWare
	6：		启用/禁用DOF
	

【程序运行依赖项】

	KG3DEngine_AutoTest.dll
	KG3DShaderLibrary.dll
	KG3DSound.dll
	Engine_Lua5.dll
	Lua5.dll

【相关文档】
	
	http://jx3web/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-3DEngine-PostRenderSemiautomaticTesting

＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
