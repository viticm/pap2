dofile("string.lua")

-- -------------------------
-- LUANET
-- -------------------------
System = luanet.System
Forms = System.Windows.Forms

if luanet then
	if ___GIsServer then
		-- 服务器自动导出，全部采用静默方式
		io.MessageBox = luanet.System.Console.WriteLine
		io.MessageBoxResult = function() end
	else
		-- 消息框, 会暂停脚本执行, 直到点击按钮
		io.MessageBox = luanet.System.Windows.Forms.MessageBox.Show
		
		-- 带返回的消息框, 会暂停脚本执行, 直到点击按钮
		io.MessageBoxResult = function(szContent, szTitle)
			szTitle = szTitle or "提示"
			local lnMessageBox = luanet.System.Windows.Forms.MessageBox
			local lnMessageBoxButtons = luanet.System.Windows.Forms.MessageBoxButtons
			local lnDialogResult = luanet.System.Windows.Forms.DialogResult
			local lnMessageBoxIcon = luanet.System.Windows.Forms.MessageBoxIcon
			if lnMessageBox.Show(szContent, szTitle, lnMessageBoxButtons.YesNo, lnMessageBoxIcon.Question) == lnDialogResult.Yes then
				return true
			else
				return false
			end
		end
		
		-- 创建一个导出LOG的窗体
		PAGES_NAME = {"全局调试信息", "空行规范", "相关表检查", "操作符空格规范", "返回值规范", "变量规范", "魔法数字", "*版本说明*"}
		io.CreateOutputForm = function(OutputFunction, CommentFunction, IndentFunction)
			local lnOutputForm = Forms.Form();lnOutputForm:SuspendLayout();
			local lnTabControl = Forms.TabControl();lnTabControl:SuspendLayout();
			local lntTabPages = {}
			local lntRichTextBoxs = {}
			for i = 1, #PAGES_NAME do
				lntTabPages[i] = Forms.TabPage();lntTabPages[i]:SuspendLayout();
				lntRichTextBoxs[i] = Forms.RichTextBox()
			end
			local lnButtonWrite = Forms.Button()
			local lnButtonCancel = Forms.Button()
			local lnButtonComment = Forms.Button()
			local lnButtonIndent = Forms.Button()
			local tProtectedObj = {}
			
			-- TabControl 控件
			lnTabControl.Location = System.Drawing.Point(13, 13)
			lnTabControl.Name = "TabControl"
			lnTabControl.SelectedIndex = 0
			lnTabControl.Size = System.Drawing.Size(769, 514)
			lnTabControl.TabIndex = 0
			for i = 1, #PAGES_NAME do
				lntTabPages[i].Name = "TabPage" .. i
				lntTabPages[i].Padding = Forms.Padding(3)
				lntTabPages[i].Size = System.Drawing.Size(761, 489)
				lntTabPages[i].TabIndex = 0
				lntTabPages[i].Text = PAGES_NAME[i]
				lntTabPages[i].UseVisualStyleBackColor = true
			
				-- PAGE内的TEXTBOX
				lntRichTextBoxs[i].Dock = Forms.DockStyle.Fill
				lntRichTextBoxs[i].Location = System.Drawing.Point(3, 3)
				lntRichTextBoxs[i].Name = "RichTextBox" .. i
				lntRichTextBoxs[i].Size = System.Drawing.Size(755, 483)
				lntRichTextBoxs[i].TabIndex = 0
				lntRichTextBoxs[i].Text = ""
				lntRichTextBoxs[i].MouseDoubleClick:Add(function(sender, data)
					local nSelectionStart = sender.SelectionStart
					local nCount = 0
					for i = 1, #sender.Text do
						if sender.Text:sub(i, i):byte() > 127 then
							nCount = nCount + 1
						else
							nCount = nCount + 2
						end
						if nCount % 2 == 0 and math.floor(nCount / 2) == nSelectionStart then
							nSelectionStart = i + 1
							break
						end
					end
					local szTemp = sender.Text:sub(1, nSelectionStart)
					local nLineStart = 1
					local nLineEnd = 1
					for i = #szTemp - 1, 1, -1 do
						if szTemp:sub(i, i) == "\n" then
							nLineStart = i + 1
							szTemp = szTemp:sub(nLineStart, nSelectionStart)
							break
						end
					end
					for i = nLineStart, #sender.Text, 1 do
						if sender.Text:sub(i, i) == "\n" then
							nLineEnd = i
							break
						end
					end
					if nLineEnd == 1 then
						nLineEnd = #sender.Text
					end
					szTemp = sender.Text:sub(nLineStart, nLineEnd)
					
					szLineCount = szTemp:match("Line (%d+) :")
					if szLineCount then
						local codepage = this.ActiveMdiChild.luaEdit1
						codepage.ce1:GotoLine(tonumber(szLineCount) - 1)
						codepage:Focus()
					end
				end)
				
				lntTabPages[i].Controls:Add(lntRichTextBoxs[i])						-- 挂接控件
				lnTabControl.Controls:Add(lntTabPages[i])							-- 挂接控件
		    end
			
			-- OutputForm 窗体
			lnOutputForm.StartPosition = Forms.FormStartPosition.CenterScreen		-- 起始位置, 居中
			lnOutputForm.AutoScaleMode = Forms.AutoScaleMode.None					-- 自动缩放关闭
			lnOutputForm.ClientSize = System.Drawing.Size(794, 568)					-- 可用区域(800, 600)
			lnOutputForm.FormBorderStyle = Forms.FormBorderStyle.FixedDialog		-- 固定大小
			lnOutputForm.Opacity = 0.9
			lnOutputForm.MaximizeBox = false
            lnOutputForm.MinimizeBox = false
            lnOutputForm.CancelButton = lnButtonCancel
            lnOutputForm.KeyPreview = true
			lnOutputForm.Name = "OutputForm"
			lnOutputForm.Text = "脚本检查LOG输出窗口"
			lnOutputForm:ResumeLayout(false)
			lnOutputForm.Controls:Add(lnTabControl)									-- 挂接控件
			lnOutputForm.Controls:Add(lnButtonWrite)								-- 挂接控件
			lnOutputForm.Controls:Add(lnButtonCancel)								-- 挂接控件
			lnOutputForm.Controls:Add(lnButtonComment)								-- 挂接控件
			lnOutputForm.Controls:Add(lnButtonIndent)								-- 挂接控件
			lnOutputForm.KeyUp:Add(function(sender, data)							-- ESC 关闭窗口
				if data.KeyCode == Forms.Keys.Escape then
					sender:Dispose()
				end
			end)
			
			-- ButtonCancel 控件
			lnButtonCancel.Location = System.Drawing.Point(700, 533)
			lnButtonCancel.Name = "ButtonCancel"
			lnButtonCancel.Size = System.Drawing.Size(75, 23)
			lnButtonCancel.TabIndex = 1
			lnButtonCancel.Text = "退出 [ESC]"
			lnButtonCancel.UseVisualStyleBackColor = true
			lnButtonCancel.DialogResult = Forms.DialogResult.Cancel
			lnButtonCancel.MouseClick:Add(function(sender, data) sender.Parent:Dispose() end)
			
			-- ButtonWrite 控件
			lnButtonWrite.Location = System.Drawing.Point(619, 533)
			lnButtonWrite.Name = "ButtonWrite"
			lnButtonWrite.Size = System.Drawing.Size(75, 23)
			lnButtonWrite.TabIndex = 2
			lnButtonWrite.Text = "*规范文档*"
			lnButtonWrite.UseVisualStyleBackColor = true
			if OutputFunction then
				lnButtonWrite.MouseClick:Add(OutputFunction)
			end
			
			-- ButtonComment 控件
			lnButtonComment.Location = System.Drawing.Point(17, 533)
			lnButtonComment.Name = "ButtonComment"
			lnButtonComment.Size = System.Drawing.Size(75, 23)
			lnButtonComment.TabIndex = 3
			lnButtonComment.Text = "整理头注释"
			lnButtonComment.UseVisualStyleBackColor = true
			if CommentFunction then
				lnButtonComment.MouseClick:Add(CommentFunction)
			end
			
			-- ButtonIndent 控件
			lnButtonIndent.Location = System.Drawing.Point(98, 533)
			lnButtonIndent.Name = "ButtonIndent"
			lnButtonIndent.Size = System.Drawing.Size(75, 23)
			lnButtonIndent.TabIndex = 4
			lnButtonIndent.Text = "自动缩进"
			lnButtonIndent.UseVisualStyleBackColor = true
			if IndentFunction then
				lnButtonIndent.MouseClick:Add(IndentFunction)
			end
			
			-- 处理代理 TABLE
			tProtectedObj.__form = lnOutputForm
			-- 显示窗口
			tProtectedObj.ShowDialog = function(self)
				--self.__form:ShowDialog()
				self.__form:Show()
			end
			tProtectedObj.SetLogsInfo = function(self, tLogs)
				local nTabControlIndex = self.__form.Controls:IndexOfKey("TabControl")
				local lnTabControl = self.__form.Controls[nTabControlIndex]
				local lntTabPages = {}
				local lntRichTextBoxs = {}

				-- 定义 RTF 颜色
				local szRtfText = [[{\rtf1\ansi\ansicpg936\deff0\deflang1033\deflangfe2052{\fonttbl{\f0\fmodern\fprq6\fcharset134\'cb\'ce\'cc\'e5;}{\f1\fnil\fcharset134\'cb\'ce\'cc\'e5;}}]]
				local tColor = {
					black	= 1, {name = "black",	r =   0, g =   0, b =   0},
					red		= 2, {name = "red",		r = 196, g =  48, b =  48},
					green	= 3, {name = "green",	r =  48, g = 196, b =   0},
					blue	= 4, {name = "blue",	r =   0, g =   0, b = 255},
					gray	= 5, {name = "gray",	r = 196, g = 196, b = 196},
					yellow	= 6, {name = "yellow",	r = 196, g = 128, b =   0},
				}
				local szColorDefine = ""
				for i, v in pairs(tColor) do
					if type(v) == "table" then
						szColorDefine = szColorDefine .. "\\red" .. v.r .. "\\green" .. v.g .. "\\blue" .. v.b .. ";"
					end
				end
				szColorDefine = [[{\colortbl;]] .. szColorDefine .. [[}\viewkind4\uc1\pard\lang2052\f0\fs20 ]]
				
				local RtfFormat = function(szText)
					szText = szText:gsub("\n", "\\par\r\n")
					local TagConvert = function(szMatched)
						local szTextOrg = szMatched
						for k, v in pairs(tColor) do
							if type(v) ~= "table" then
								szMatched = szMatched:gsub("{" .. k .. "|", "\\cf" .. v)
								szMatched = szMatched:gsub("}", "\\cf0")
							end
						end
						return szMatched
					end
					szText = szText:gsub("%b{}", TagConvert)
					
					return szText
				end

				for i = 1, #tLogs do
					if type(tLogs[i]) == "string" then
						lntTabPages[i] = lnTabControl.Controls[lnTabControl.Controls:IndexOfKey("TabPage" .. i)]
						lntRichTextBoxs[i] = lntTabPages[i].Controls[lntTabPages[i].Controls:IndexOfKey("RichTextBox" .. i)]
						lntRichTextBoxs[i].Rtf = szRtfText .. szColorDefine .. RtfFormat(tLogs[i]) .. [[\cf0\f1\fs18\par}]]
					end
				end
			end
			
			return tProtectedObj
		end
	end
end

-- -------------------------
-- CUSTOM
-- -------------------------

-- 读取一个tab文件, 并将这个tab文件的内容保存到一个二维TABLE中
-- > szFileName`	: 文件名, 文件强制以只读("r")模式打开, 不允许在此处写操作
-- > szKeyName		: 主键名, 主键必须是一张表中, 值绝对不重复的字段
io.LoadTabFile = function(szFileName, szKeyName)
	szKeyName = szKeyName or "$NIL$"
	local tLinesData = {}							-- 根据顺序编号的数据
	local file = io.open(szFileName, "r")			-- 用默认的只读方式打开文件
	if not file then								-- 文件不存在则直接返回空
		--io.MessageBox("没有找到文件: " .. szFileName)
		return
	end
	local tMetaData = {								-- 保存在 Metatable 中的数据
		Title = "",									-- 保存表头
		Array = {},									-- 保存为数组
	}

	-- 尝试读取表头字符串
	local szTableHeadLine = file:read("*line")			-- 读取第一行表头
	tMetaData.Title = szTableHeadLine:Split()			-- 将表头按照默认切分符("\t")切分
	 
	-- 下面尝试遍历整张表
	repeat
		local szLine = file:read("*line")				-- 顺序读取每一行, read line 会自动维护计数器, 每次调用都自动读取下一行
		if szLine then
			local tDatas = szLine:Split()				-- 将每一行数据拆分
			local tRowDatas = {}
			local szKeyValue = nil
			for i = 1, #tMetaData.Title do				-- 循环每行的字段次数
				tRowDatas[tMetaData.Title[i]] = tostring(tDatas[i])		-- 统一保存为字符串类型
				if tMetaData.Title[i]:lower() == szKeyName:lower() then	-- 如果找到了 KEY, 则记录下 KEY 的值
					szKeyValue = tDatas[i]
				end
			end
			
			table.insert(tMetaData.Array, tRowDatas)	-- 按照顺序保存在数组里面
			if szKeyValue then
				local nID = tonumber(szKeyValue)
				if nID then
					tLinesData[nID] = tRowDatas
				else
					tLinesData[szKeyValue] = tRowDatas
				end
			end
		end
	until not szLine
	
	file:close()													-- 关闭文件
	setmetatable(tLinesData, tMetaData)								-- 装载 MetaData , 这部分数据不直接作为 table 的数据
	return tLinesData
end

io.Trace = function(...)
	if ... then
		file = io.open("lualog.txt", "a+")
		file:write(... .. "\n")
		file:close()
	end
end

io.TraceInit = function()
	file = io.open("lualog.txt", "w")
	file:write(os.date() .. "\n\n")
	file:close()
end