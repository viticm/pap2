
if (_G.flowlib == nil) then
	_G.flowlib = {}
end

_G.using = function(varname)
	local thisenv = getfenv(2)
	thisenv[varname] = _G.flowlib[varname]	
end

_G.declare = function(varname, varvalue)
	_G.flowlib[varname] = varvalue
	local thisenv = getfenv(2)
	thisenv[varname] = _G.flowlib[varname]
end

_G.regcallback = _G.declare

_G.catcharg = function (...)
	return arg
end

_G.resume = function (...)
	local t = catcharg(coroutine.resume(...))
	local errmsg = t[1]
	if (errmsg == false) then
		print("协程内错误: " .. t[2])
		print(debug.traceback())
	end

    return unpack(t)

end


if not comgr then
	--print('create comgr')
	declare('comgr', {})
	comgr.colist = {}
	setmetatable(comgr.colist, {__mode = "v"})

	comgr.seed = 0

	comgr.push = function (co)
		comgr.seed = comgr.seed + 1
		comgr.colist[comgr.seed] = co	
		return comgr.seed
	end

	comgr.find = function (id)
		return comgr.colist[id]
	end


	comgr.colist_w = {}
	setmetatable(comgr.colist_w, {__mode = "v"})

	comgr.pushw = function(player, co)
	   comgr.colist_w[player.dwID] = co
	end

	comgr.findw = function(player)
	   return comgr.colist_w[player.dwID]
	end


	comgr.colist_o = {}
	setmetatable(comgr.colist_o, {__mode = "v"})

	comgr.pusho = function(player, co)
	   comgr.colist_o[player.dwID] = co
	end

	comgr.findo = function(player)
	   return comgr.colist_o[player.dwID]
	end
end

_G.event_dispatch = function(func, ...)
	local co = coroutine.create(func)
	local thisenvid = comgr.push(co)
	resume(co, {envid=thisenvid, co=co}, ...)
	return
end



_G.create_subflow = event_dispatch


_G.finishnode = function(parentcontext)  --自动生成的结束节点 
	if parentcontext then
		resume(parentcontext.co) --分支节点使用，用于退出本子流程，回到上一层流程
	end
end

_G.null = {}
setmetatable(_G.null, {__tostring = function() return 'null' end})
