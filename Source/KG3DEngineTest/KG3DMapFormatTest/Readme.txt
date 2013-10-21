========================================================================================
地图场景新格式自动化测试工具
========================================================================================

【应用程序名称】KG3DMapFormatTest.exe

【功能】

	1. 验证新旧引擎加载旧格式地图的数据一致。
	2. 验证新旧格式地图的新建和保存后的数据正确
	3. 验证已存在的新旧格式地图加载后再重新保存，确认保存后的数据和原数据一致
	4. 验证旧格式地图保存为新格式地图后与原来旧格式的数据一致


【运行环境】

	KG3DEngine_AutoTest.dll
	KGOldEngineController.dll
	KG3DShaderLibrary.dll
	KG3DSound.dll
	Engine_Lua5.dll 
	Lua5.dll
	CaseSetting.ini
	config.ini

	NewFormatMap：新格式地图场景文件
	OldEngineDll：旧格式引擎相关DLL
	
	
【使用说明】

	方法一：KG3DMapFormatTest.exe [客户端根目录]
	              例如：KG3DMapFormatTest.exe D:\sword3-products\client
	
	方法二：直接将应用程序及相关依赖拷贝到客户端下双击运行

【输出结果目录】

	应用程序运行目录\logs\KG3DMapFormatTest\运行日期\

【相关文档】

	http://jx3web/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-3DEngine-MapNewFormatCheckTool

========================================================================================
CaseSetting.ini配置说明
========================================================================================

[MAPLIST]
	OldMap_01		#指定测试的旧地图，如果该项不为空，则不会读取MapList.tab，最多到OldMap_99
	OldMap_02
	NewMap_01		#指定测试的新地图，如果该项不为空，则不会读取MapList.tab，最多到NewMap_99

[SETTING]
	TestingAllMap		#1：对MapList.tab里的所有场景进行所有用例的测试 0：每个用例随机取MapList.tab里的某个场景测试
	MaxModelToCreate		#创建场景测试用例时添加到场景里的最大模型数
	MinModelToCreate		#创建场景测试用例时添加到场景里的最小模型数
	MaxOldMapID		#MapList.tab中旧场景名称最大ID号
	ResourcePath		#添加模型的资源路径

========================================================================================
旧格式引擎控制动态库
========================================================================================

【动态库名称】KGOldEngineController.dll

【功能】

	加载旧的引擎Dll，并提供相应的控制接口，获取相应的数据用于测试。

【加载环境】

	OldEngineDll：旧格式引擎相关DLL

========================================================================================