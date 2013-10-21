-- -------------------------
-- LUANET
-- -------------------------

-- 正则表达式

-- -------------------------
-- CUSTOM
-- -------------------------
-- 将一个字符串按照给定的分隔符切分成一个多个部件存入一个数组
-- > szString	: 需要被切割的字符串
-- > szSpliter	: 切割符, 可以是多个字符组成的字符串, 如 "@_@"
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

-- 取得一个文件全名的路径和文件名
string.SplitFileName = function(szFileName)
	if type(szFileName) ~= "string" then
		return
	end
	
	if not szFileName:find("\\") then
		return "", szFileName
	end
	return szFileName:match("(.*)\\(.*)$")
end

