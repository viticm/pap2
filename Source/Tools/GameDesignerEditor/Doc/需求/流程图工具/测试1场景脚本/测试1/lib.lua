
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
		print("Ð­³ÌÄÚ´íÎó: " .. t[2])
	end

    return unpack(t)

end


