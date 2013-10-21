---------------------------------------------------------------------->
-- 脚本名称:	CheckLog.lua
-- 更新时间:	09/05/15 09:48:30
-- 更新用户:	蔡宇韬
-- 脚本说明:	根据同步日志生成分析表，并与预期结果比较
--		有问题请RTX caiyutao或者 MailTo: caiyutao@kingsoft.com
----------------------------------------------------------------------<

LogFileName = "D:\\代码\\我的\\同步日志分析\\log.txt";
ExpectFileName = "D:\\代码\\我的\\同步日志分析\\1.lua";
ResultFileName = "D:\\代码\\我的\\同步日志分析\\ResultFile.tab"

local tLogResult = {}
local tExpertResult = {}
local tRegionMove = {}
local VelocityDiff = 50 * 2


-- 基本功能函数，拆分字符串
string.Split = function(szString, szSpliter)
	local tSplited = {}
	local nCount = 0								-- 记录找到的切分符数量
	local nStartLoc = 0								-- 记录单个被切分字符串的开始段
	local nEndLoc = 0								-- 记录单个被切分字符串的结束段
	local bFoundSpliter = false						-- 记录是否找到切分符
	
	if not szSpliter then							-- 如果没有传入指定的切分符, 则默认为制表符tab
		szSpliter = "\t"
	end
	local szFormated = szString .. szSpliter		-- 格式化原字符串
	
	repeat
		nCount = nCount + 1
		bFoundSpliter, nEndLoc = szFormated:find(szSpliter, nStartLoc)
		if bFoundSpliter then
			tSplited[nCount] = szFormated:sub(nStartLoc, nEndLoc - #szSpliter)
			if not tSplited[nCount] then
				tSplited[nCount] = ""
			end
			nStartLoc = nEndLoc + 1
		end
	until not bFoundSpliter
	return tSplited
end

--基本函数 获取第参数的函数
GetParam = function(szLine, nParamNum, szUseSplit)
	local szParams = szLine;
	szParams = string.gsub(szParams, "%(", "");
	szParams = string.gsub(szParams, "%)", "");
	szParams = string.gsub(szParams, " ", "");
	local tParams = string.Split(szParams, szUseSplit);
	return tParams[nParamNum]
end


-- 基本参数，读取文件信息的函数
LoadData = function(FileName)
	local nAddCount = 1
	local SourceFile = io.open(FileName, "r")
	local t = SourceFile:read("*all")
	local szFileData = t:Split("\n")
	SourceFile:close()
	return szFileData
end

Compare = function(ResultFileName)
	local szResult = "用例编号\t预期玩家1坐标\t预期玩家2坐标\t预期同步标识\t实际玩家1坐标\t实际玩家2坐标\t实际同步标识\t用例是否通过\t\t玩家1移动\t玩家2移动\n"
	local szIsPassed = "unknown"
	for nAddCount in ipairs(tLogResult) do
		--这里进行是否用例通过的判断
		if tonumber(tLogResult[nAddCount].CaseSyn) ~= tonumber(tExpertResult[nAddCount].CaseSyn) then
			szIsPassed = "Failed"
		elseif math.abs(tLogResult[nAddCount].nP1X - tExpertResult[nAddCount].nP1X) > VelocityDiff then
			szIsPassed = "Failed"
		elseif math.abs(tLogResult[nAddCount].nP1Y - tExpertResult[nAddCount].nP1Y) > VelocityDiff then
			szIsPassed = "Failed"
		elseif math.abs(tLogResult[nAddCount].nP2X - tExpertResult[nAddCount].nP2X) > VelocityDiff then
			szIsPassed = "Failed"	
		elseif math.abs(tLogResult[nAddCount].nP2Y - tExpertResult[nAddCount].nP2Y) > VelocityDiff then
			szIsPassed = "Failed"
		else
			szIsPassed = "Passed"
		end
		
		if szIsPassed == "Passed" then
				szResult = szResult .. 
									tLogResult[nAddCount].CaseNum ..
									"\t" ..
									tExpertResult[nAddCount].nP1X ..
									", " ..
									tExpertResult[nAddCount].nP1Y ..
									"\t" ..
									tExpertResult[nAddCount].nP2X ..
									", " ..
									tExpertResult[nAddCount].nP2Y ..
									"\t" ..
									tExpertResult[nAddCount].CaseSyn ..
									"\t" ..
									tLogResult[nAddCount].nP1X .. 
									", "  .. 
									tLogResult[nAddCount].nP1Y ..
									"\t" ..
									tLogResult[nAddCount].nP2X ..
									", " ..
									tLogResult[nAddCount].nP2Y ..
									"\t" ..
									tLogResult[nAddCount].CaseSyn ..
									"\t" ..
									szIsPassed ..
									"\n"
		else
			szResult = szResult .. 
									tLogResult[nAddCount].CaseNum ..
									"\t" ..
									tExpertResult[nAddCount].nP1X ..
									", " ..
									tExpertResult[nAddCount].nP1Y ..
									"\t" ..
									tExpertResult[nAddCount].nP2X ..
									", " ..
									tExpertResult[nAddCount].nP2Y ..
									"\t" ..
									tExpertResult[nAddCount].CaseSyn ..
									"\t" ..
									tLogResult[nAddCount].nP1X .. 
									", "  .. 
									tLogResult[nAddCount].nP1Y ..
									"\t" ..
									tLogResult[nAddCount].nP2X ..
									", " ..
									tLogResult[nAddCount].nP2Y ..
									"\t" ..
									tLogResult[nAddCount].CaseSyn ..
									"\t" ..
									szIsPassed ..
									"\t\t" .. "玩家一：(" .. tRegionMove[nAddCount].n1XFrom .. ", " .. tRegionMove[nAddCount].n1YFrom .. ") -> (" .. tRegionMove[nAddCount].n1XTo .. ", " .. tRegionMove[nAddCount].n1YTo .. ")" .. "\t" ..
									"玩家二：(" .. tRegionMove[nAddCount].n2XFrom .. ", " .. tRegionMove[nAddCount].n2YFrom .. ") -> (" .. tRegionMove[nAddCount].n2XTo .. ", " .. tRegionMove[nAddCount].n2YTo .. ")" .. "\t" ..
									"\n"
		end
								
	
	
	end
	print(szResult)
	local SourceFile = io.open(ResultFileName, "w")
	SourceFile:write(szResult)
	SourceFile:close()
	

end

--分析函数
DoAnalyse = function(LogFileName, ExpectFileName, ResultFileName)

	
	local tFileData = LoadData(LogFileName);
	local tExpectFileData = LoadData(ExpectFileName);
	
	local nTestCaseNum;
	local nPosition1X;
	local nPosition1Y;
	local nPosition2X;
	local nPosition2Y;
	local nCaseSyn = 2;
	local nAddSynTime = 0;
	local nDelSynTime = 0;
	
	local m_nCaseNum = 0;
	local m_n1X = 0;
	local m_n1Y = 0;
	local m_n2X = 0;
	local m_n2Y = 0;
	local m_nCaseSyn = 0;
	local m_nAddCaseSyn = 0;
	local bBeginFlag = false
	local nBeginFlagExpect = false
	
	local n1XSrcRegion = 1
	local n1YSrcRegion = 1
	local n1XDstRegion = 1
	local n1YDstRegion = 1
	
	local n2XSrcRegion = 1
	local n2YSrcRegion = 1
	local n2XDstRegion = 1
	local n2YDstRegion = 1

	--开始分析
	for nAddCount in pairs(tFileData) do
		--读取Case号
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]TestCase %d+ begin..") ~= nil then
				local szTemp = string.match(tFileData[nAddCount], ": %[SYNCTEST%]TestCase %d+ begin..");
				nTestCaseNum = tonumber(string.match(szTemp, "%d+"));
				nCaseSyn = 2;
				nAddSynTime = 0;
				nDelSynTime = 0;
				bBeginFlag = true
			end	
		
			
		--读取player1信息
			if string.match(tFileData[nAddCount], "%[SYNCTEST%]Player1:") ~= nil and bBeginFlag == true then
				local szTemp = string.match(tFileData[nAddCount],"%b()");
				nPosition1X = tonumber(GetParam(szTemp, 1, ","));
				nPosition1Y = tonumber(GetParam(szTemp, 2, ","));
			end
		
		
		--读取player2信息
		
			if string.match(tFileData[nAddCount], "%[SYNCTEST%]Player2:") ~= nil and bBeginFlag == true then
				local szTemp = string.match(tFileData[nAddCount],"%b()");
				nPosition2X = tonumber(GetParam(szTemp, 1, ","));
				nPosition2Y = tonumber(GetParam(szTemp, 2, ","));
			end
		
		--读取同步信息
		
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]Sync player%(ID: 1%) to player%(ID: 2%)") ~= nil  and bBeginFlag == true then
				if nCaseSyn == 1 then
					if nAddSynTime < 2 then
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 2 then
					nCaseSyn = 1
					nAddSynTime = nAddSynTime + 1
				elseif nCaseSyn == 3 then
					if nDelSynTime == 2 then
						nCaseSyn = 4
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 4 then
					if nAddSynTime < 2 then
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 5 then
					nCaseSyn = 0
				end
			end
			
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]Sync player%(ID: 1%) to player%(ID: 2%)") ~= nil  and bBeginFlag == true then
				if nCaseSyn == 1 then
					if nAddSynTime < 2 then
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 2 then
					nCaseSyn = 1
					nAddSynTime = nAddSynTime + 1
				elseif nCaseSyn == 3 then
					if nDelSynTime == 2 then
						nCaseSyn = 4
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 4 then
					if nAddSynTime < 2 then
						nAddSynTime = nAddSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 5 then
					nCaseSyn = 0
				end
			end
			
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]Del player%(ID: 1%)") ~= nil and bBeginFlag == true then
				if nCaseSyn == 3 then
					if nDelSynTime < 2 then
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 2 then
					nCaseSyn = 3
					nDelSynTime = nDelSynTime + 1
				elseif nCaseSyn == 1 then
					if nAddSynTime == 2 then
						nCaseSyn = 5
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 5 then
					if nDelSynTime < 2 then
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 4 then
					nCaseSyn = 0
				end
			end
			
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]Del player%(ID: 2%)") ~= nil and bBeginFlag == true then
				if nCaseSyn == 3 then
					if nDelSynTime < 2 then
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 2 then
					nCaseSyn = 3
					nDelSynTime = nDelSynTime + 1
				elseif nCaseSyn == 1 then
					if nAddSynTime == 2 then
						nCaseSyn = 5
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 5 then
					if nDelSynTime < 2 then
						nDelSynTime = nDelSynTime + 1
					else
						nCaseSyn = 0
					end
				elseif nCaseSyn == 4 then
					nCaseSyn = 0
				end
			end
		--读取结束消息
			
			if string.match(tFileData[nAddCount], ": %[SYNCTEST%]TestCase %d+ end") ~= nil and bBeginFlag == true then
				if nDelSynTime % 2 ~= 0 or nAddSynTime % 2 ~= 0 then
					nCaseSyn = 0
				end
				local szTemp = string.match(tFileData[nAddCount], ": %[SYNCTEST%]TestCase %d+ end")
				local nTime = tonumber(string.match(szTemp, "%d+"))
				if nTime == nTestCaseNum then
					tLogResult[#tLogResult + 1] = {CaseNum = nTestCaseNum, nP1X = nPosition1X, nP1Y = nPosition1Y, nP2X = nPosition2X, nP2Y = nPosition2Y, CaseSyn = nCaseSyn}			
				end
				
				--阶段清理
					nTestCaseNum = 0;
					nPosition1X = 0;
					nPosition1Y = 0;
					nPosition2X = 0;
					nPosition2Y = 0;
					
					nCaseSyn = 2;
					nAddSynTime = 0;
					nDelSynTime = 0;
					bBeginFlag = false
			end
		
		
	end



	--比较函数
	for nAddCount in pairs(tExpectFileData) do
		local nTempa
		local nTempb
		local szTempParam
	
		if string.match(tExpectFileData[nAddCount], "tTestCase%[%d+%] = function%(player1, player2") ~= nil then
			local szTemp = string.match(tExpectFileData[nAddCount], "tTestCase%[%d+%]")
			m_nCaseNum = tonumber(string.match(szTemp, "%d+"))
			nBeginFlagExpect = true
		end
		if nBeginFlagExpect == true and string.match(tExpectFileData[nAddCount], "player1.SetPosition") ~= nil then
			local szTemp = string.match(tExpectFileData[nAddCount], "%b()")
			szTempParam = GetParam(szTemp, 1, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			n1XSrcRegion = math.floor(nTempa) + 1
			szTempParam = GetParam(szTemp, 2, ",")
			nTempb = tonumber(GetParam(szTempParam, 1, "*"))
			n1YSrcRegion = math.floor(nTempb) + 1
		end
		
		if nBeginFlagExpect == true and string.match(tExpectFileData[nAddCount], "player2%.SetPosition") ~= nil then
			local szTemp = string.match(tExpectFileData[nAddCount], "%b()")
			szTempParam = GetParam(szTemp, 1, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			n2XSrcRegion = math.floor(nTempa) + 1
			szTempParam = GetParam(szTemp, 2, ",")
			nTempb = tonumber(GetParam(szTempParam, 1, "*"))
			n2YSrcRegion = math.floor(nTempb) + 1
		end
			
		
		if string.match(tExpectFileData[nAddCount], "RegionTest%(player1, player2") ~= nil and 
			string.match(tExpectFileData[nAddCount], "function RegionTest") == nil then
			local szTemp = string.match(tExpectFileData[nAddCount], "%b()")
			szTempParam = GetParam(szTemp, 3, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			nTempb = tonumber(GetParam(szTempParam, 2, "*"))
			m_n1X = nTempa * nTempb
			n1XDstRegion = math.floor(nTempa) + 1
			
			szTempParam = GetParam(szTemp, 4, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			nTempb = tonumber(GetParam(szTempParam, 2, "*"))
			m_n1Y = nTempa * nTempb
			n1YDstRegion = math.floor(nTempa) + 1
			
			szTempParam = GetParam(szTemp, 6, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			nTempb = tonumber(GetParam(szTempParam, 2, "*"))
			m_n2X = nTempa * nTempb
			n2XDstRegion = math.floor(nTempa) + 1
			
			szTempParam = GetParam(szTemp, 7, ",")
			nTempa = tonumber(GetParam(szTempParam, 1, "*"))
			nTempb = tonumber(GetParam(szTempParam, 2, "*"))
			m_n2Y = nTempa * nTempb
			n2YDstRegion = math.floor(nTempa) + 1
		end
		
		if string.match(tExpectFileData[nAddCount], "-- AddSyncFlag:%d+") ~= nil then
			m_nAddCaseSyn = string.match(tExpectFileData[nAddCount], "%d+")
		end
		
		if string.match(tExpectFileData[nAddCount], "-- SyncFlag:%d+") ~= nil then
			m_nCaseSyn = string.match(tExpectFileData[nAddCount], "%d+")
			
			local nFirstSyn = 2
			local nSecondSyn = 2
			
			local nSyn1 = 0;
			local nSyn2 = 0;
			local nSyn3 = 0;
			
			local nTempInt1 = 0
			local nTempInt2 = 0
			nTempInt1 = math.abs(n2XSrcRegion - n1XSrcRegion)
			nTempInt2 = math.abs(n2YSrcRegion - n1YSrcRegion)
			
			if nTempInt1 < 2 and nTempInt2 < 2 then
				nSyn1 = 1
			else
				nSyn1 = 0
			end
			
			nTempInt1 = math.abs(n1XDstRegion - n2XSrcRegion)
			nTempInt2 = math.abs(n1YDstRegion - n2YSrcRegion)
			
			if nTempInt1 < 2 and nTempInt2 < 2 then
				nSyn2 = 1
			else
				nSyn2 = 0
			end
			
			nTempInt1 = math.abs(n1XDstRegion - n2XDstRegion)
			nTempInt2 = math.abs(n1YDstRegion - n2YDstRegion)
			
			if nTempInt1 < 2 and nTempInt2 < 2 then
				nSyn3 = 1
			else
				nSyn3 = 0
			end			
 
			if nSyn1 == 1 then
				if nSyn2 == 0 then
					nFirstSyn = 3
				else
					nFirstSyn = 2
				end
			else
				if nSyn2 == 0 then
					nFirstSyn = 2
				else
					nFirstSyn = 1
				end
			end
		
			if nSyn2 == 1 then
				if nSyn3 == 0 then
					nSecondSyn = 3
				else
					nSecondSyn = 2
				end
			else
				if nSyn3 == 0 then
					nSecondSyn = 2
				else
					nSecondSyn = 1
				end
			end				
				
			if nFirstSyn == 1 then
				if nSecondSyn == 2 then
					m_nCaseSyn = 1
				elseif nSecondSyn == 3 then
					m_nCaseSyn = 5
				end
			elseif nFirstSyn == 2 then
				if nSecondSyn == 1 then
					m_nCaseSyn = 1
				elseif nSecondSyn == 2 then
					m_nCaseSyn = 2
				elseif nSecondSyn == 3 then
					m_nCaseSyn = 3
				end
			elseif nFirstSyn == 3 then
				if nSecondSyn == 2 then
					m_nCaseSyn = 3
				elseif nSecondSyn == 1 then
					m_nCaseSyn = 4
				end
			end
			
			if m_nAddCaseSyn ~= 0 then
				m_nCaseSyn = m_nAddCaseSyn
			end
			
		--这里做存储的处理
		
			tExpertResult[#tExpertResult + 1] = {CaseNum = m_nCaseNum, nP1X = m_n1X, nP1Y = m_n1Y, nP2X = m_n2X, nP2Y = m_n2Y, CaseSyn = m_nCaseSyn}
			tRegionMove[#tRegionMove + 1] = {CaseNum = m_nCaseNum, n1XFrom = n1XSrcRegion, n1YFrom = n1YSrcRegion, n2XFrom = n2XSrcRegion, n2YFrom = n2YSrcRegion,
																 n1XTo = n1XDstRegion, n1YTo = n1YDstRegion, n2XTo = n2XDstRegion, n2YTo = n2YDstRegion}
			
			
			n1XSrcRegion = 1
			n1YSrcRegion = 1
			n1XDstRegion = 1
			n1YDstRegion = 1
		
			n2XSrcRegion = 1
			n2YSrcRegion = 1
			n2XDstRegion = 1
			n2YDstRegion = 1
			m_nCaseNum = 0;
			m_n1X = 0;
			m_n1Y = 0;
			m_n2X = 0;
			m_n2Y = 0;
			m_nCaseSyn = 0;
			m_nAddCaseSyn = 0;
			nBeginFlag = false
		end
	end
	
	Compare(ResultFileName)
end
DoAnalyse(LogFileName,ExpectFileName,ResultFileName)