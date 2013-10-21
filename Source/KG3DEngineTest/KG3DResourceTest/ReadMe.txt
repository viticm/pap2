========================================================================================
资源加载检查和加载日志分析工具
========================================================================================

【应用程序名称】KG3DResourceLoadTest.exe

【功能】

	1.加载材质资源文件（.MTL）并分析日志中记录加载不正确的材质，输出分析结果
	2.加载.Mesh文件并记录和输出加载结果

【运行环境】

	Engine_Lua5.dll 
	Lua5.dll

【使用说明】

	KG3DResourceLoadTest.exe [客户端根目录] [资源文件存放目录]
	例如：KG3DResourceLoadTest.exe D:\sword3-products\client E:\ResourceFile
	备注：如果只传[客户端根目录]参数，则默认检查分析[客户端根目录]下的所有材质和Mesh文件

【输出结果目录】

	应用程序运行目录\logs\KG3DResourceLoadTest\运行日期\

【相关文档】

	http://jx3web/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-3DEngine-MaterialLoadAnalyseTool

========================================================================================
地图场景废弃资源检查工具
========================================================================================

【应用程序名称】KG3DDisuseResourceTest.exe

【功能】

	从所有地图文件（.map）中分析出所有引用到的指定资源文件，并与"data/source/map_source"和"data\source\other\特效\场景"目录中的资源做对比，输出map_source中没有被引用到的资源

【运行环境】

	Engine_Lua5.dll 
	Lua5.dll

【使用说明】

	KG3DResourceLoadTest.exe [客户端根目录] 
	例如：KG3DResourceLoadTest.exe D:\sword3-products\client

【输出结果目录】

	应用程序运行目录\logs\KG3DDisuseResourceTest\运行日期\

【相关文档】

	http://jx3web/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-3DEngine-MapDisuseResourceCheckTool

========================================================================================