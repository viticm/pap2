 -- 运行相对路径是基于 GameLuaEditor.exe 所在的目录
dofile("io.lua")	-- IO 包含了 STRING 头

-- -------------------------------------------------
-- 下面是一些工具函数
-- -------------------------------------------------

-- 导出NPCTEMPLATE表的时候, 处理同时导出的AITYPE表
-- NPCTEMPLATE表做了特殊处理, 会将数据库的内容分成两部分分别导出到两个表中: NpcTemplate.tab AIType.tab

function ExportAIData(szTargetPath, mapInfo)
	-- 获取工作路径(客户端位置)
	local szFileFolder, szFileName = szTargetPath:SplitFileName()
	local szWorkFolder = szFileFolder:SplitFileName()
	
	-- 合并所有的AI参数表到一份文件中, 并且自动判断需要使用哪种AI模式
	local szAITypeTabFileName, szAITypeLuaFileName = CombineAITab(szWorkFolder, mapInfo)	
end

function GetMapNameList(szWorkFolder, mapInfo)
	local tMapNameList = {}

	for i = 1, mapInfo.Count do
		tMapNameList[i] = mapInfo[i - 1]
	end

	return tMapNameList
end

function CombineAITab(szWorkFolder, mapInfo)
	-- 取得所有相关表的 table	
	local tAITypeTab = io.LoadTabFile(szWorkFolder .. "\\settings\\AIType.tab", "AIType")
	local tAITypeCustomTab = io.LoadTabFile(szWorkFolder .. "\\settings\\AITypeCustom.tab", "AIType")		-- 自定义AI
	local tAIPramTabs = {}
	local tSceneName = {}
	for nKey, szValue in pairs(GetMapNameList(szWorkFolder, mapInfo)) do
		tAIPramTabs[nKey] = io.LoadTabFile(szWorkFolder .. "\\settings\\map\\" .. szValue .. "\\NpcAIParameter\\NpcAIParameterList.tab", "ID")
		tSceneName[nKey] = szValue
		tSceneName[szValue] = szValue
	end
	
	-- 下面将所有相关表的内容转到一张总表里
	local tMainTabTitle = getmetatable(tAITypeTab).Title		-- 取得主表头, 以AI总表为准
	local szOutputTabName = szWorkFolder .. "\\settings\\AIType.tab"
	local fileTab = io.open(szOutputTabName, "w+")
	local szOutputLuaName = szWorkFolder .. "\\scripts\\ai\\AIParam.lua"
	local fileLua = io.open(szOutputLuaName, "w+")
	fileTab:write(table.concat(tMainTabTitle, "\t") .. "\n")
	fileLua:write("g_AIParam = {\n\n")

	-- 这里的表头肯定是个数组, 所以下面按照表头的数组顺序依次组织每一行, 这样和原表的顺序无关
	-- 处理AIType表
	
	if tAITypeTab then
		for i, v in ipairs(getmetatable(tAITypeTab).Array) do
			local szLine = {}
			v = IdentifyAIMode(v)
			for _, szField in ipairs(tMainTabTitle) do
				local szFieldValue = v[szField]
				if not szFieldValue then
					szFieldValue = ""
				end
				table.insert(szLine, szFieldValue)
			end
			
			if v.AIType and v.AIType ~= "0" then
				fileLua:write(Trans2LuaCode(szLine, tAITypeTab[0], tMainTabTitle))
				fileTab:write(table.concat(szLine, "\t") .. "\n")
			end
		end
	end
	
	-- 处理AITypeCustom表
	if tAITypeCustomTab then
		for i, v in ipairs(getmetatable(tAITypeCustomTab).Array) do
			local szLine = {}
			v = IdentifyAIMode(v)
			for _, szField in ipairs(tMainTabTitle) do
				local szFieldValue = v[szField]
				if not szFieldValue then
					szFieldValue = ""
				end
				table.insert(szLine, szFieldValue)
			end
			
			if v.AIType and v.AIType ~= "0" then
				fileLua:write(Trans2LuaCode(szLine, tAITypeTab[0], tMainTabTitle))
				fileTab:write(table.concat(szLine, "\t") .. "\n")
			end
		end
	end
	
	-- 处理各场景的NpcAIParameterList表
	for _, tAISceneNpcTab in pairs(tAIPramTabs) do
		if tAISceneNpcTab then
			for i, v in ipairs(getmetatable(tAISceneNpcTab).Array) do
				local szLine = {}
				v = IdentifyAIMode(v)
		
				for _, szField in ipairs(tMainTabTitle) do
					local szFieldValue = v[szField]
					
					if szField:lower() == "aitype" then
						szFieldValue = v.ID
					end
					if not szFieldValue then
						szFieldValue = ""
					end

					table.insert(szLine, szFieldValue)
				end

				if v.ID and v.ID ~= "0" then
					fileLua:write(Trans2LuaCode(szLine, tAITypeTab[0], tMainTabTitle))
					fileTab:write(table.concat(szLine, "\t") .. "\n")
				end
			end
		end
	end
	
	fileLua:write("}")
	fileTab:close()	-- 写文件结束
	fileLua:close()	-- 写文件结束
	return szOutputTabName, szOutputLuaName
end

function IdentifyAIMode(tAIPrams)
	local tAIMode = {
		["scripts/ai/StandardAI.lua"] = true,
		["scripts/ai/idlepassive.lua"] = true,
		["scripts/ai/wanderpassive.lua"] = true,
		["scripts/ai/patrolpassive.lua"] = true,
		["scripts/ai/idleactive.lua"] = true,
		["scripts/ai/wanderactive.lua"] = true,
		["scripts/ai/patrolactive.lua"] = true,
	}
	
	-- 由于现在统一成一个LUA了, 所以这段代码保留来仅仅做转换
	tAIPrams.ScriptFile = tAIPrams.ScriptFile or ""
	
	if tAIPrams.ScriptFile == "" or tAIMode[tAIPrams.ScriptFile:lower():gsub("\\", "/")] then
		tAIPrams.ScriptFile = "scripts/ai/StandardAI.lua"
	end
	
	if not tAIPrams.NpcSceneType or tAIPrams.NpcSceneType == "" then
		tAIPrams.NpcSceneType = "0"
	end
	if tAIPrams.NpcSceneType == "1" and tAIPrams.ScriptFile == "scripts/ai/StandardAI.lua" then
		tAIPrams.ScriptFile = "scripts/ai/DungeonStandardAI.lua"
	elseif tAIPrams.NpcSceneType == "0" and tAIPrams.ScriptFile == "scripts/ai/DungeonStandardAI.lua" then
		tAIPrams.ScriptFile = "scripts/ai/StandardAI.lua"
	end

	return tAIPrams
end

function Trans2LuaCode(tAIPrams, tDefaultAIPrams, tTitle)
	local tIgnoreList = {
		Name 			= "Name",				-- 列出了不需要输出的字段名称
		ID				= "ID",
		MainState		= "MainState",
		PursuitRange	= "PursuitRange",
		ScriptName		= "ScriptName",
		AIType			= "AIType",
		CustomArguments	= "CustomArguments",	-- AI流程图用到的可变参数的列表, 这里先忽略, 后面做单独处理
		ScriptFile		= "ScriptFile",			-- 这个不做常规导出, 而是由专门模块计算出来
	}
	local szCodeLine = ""
	local FormatValue = function(value)
		if tonumber(value) then
			value = tonumber(value)
		elseif type(value) == "string" then
			value = "\"" .. value .. "\""
		elseif type(value) == "nil" then
			value = ""
		end
		return value
	end
	
	local nCustomArgsIndex = nil
	local nAINameIndex = nil
	local tTitleMap = {}
	for nIndex, szKey in ipairs(tTitle) do
		tTitleMap[szKey] = true
		if not tIgnoreList[szKey] then			-- 保证忽略列表外
			if tAIPrams[nIndex] == "" then
				if tDefaultAIPrams[szKey] and tDefaultAIPrams[szKey] ~= "" then
					szCodeLine = szCodeLine .. szKey .. " = " .. FormatValue(tDefaultAIPrams[szKey]) .. ", "
				end
			else
				szCodeLine = szCodeLine .. szKey .. " = " .. FormatValue(tAIPrams[nIndex]) .. ", "
			end
		end
		if szKey == "CustomArguments" then
			nCustomArgsIndex = nIndex
		elseif szKey == "ScriptFile" then
			nAINameIndex = nIndex
		end
	end	

	szCodeLine = "\t\[" .. tAIPrams[1] .. "\]\t= {" .. szCodeLine .. "},\n"
	return szCodeLine
end

