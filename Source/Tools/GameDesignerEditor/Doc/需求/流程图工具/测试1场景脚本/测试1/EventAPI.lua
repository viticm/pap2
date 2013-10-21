Include("scripts/map/≤‚ ‘1/lib.lua")
if not comgr then
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

declare('Event', {})
Event.Public={}
	Event.Public.event_dispatch = function(func, ...)
		print('func:'..tostring(func))
		local co = coroutine.create(func)
		local thisenvid = comgr.push(co)
		resume(co, thisenvid, ...)
		return
	end

Event.OnAcceptQuest={}
	Event.OnAcceptQuest.event1={}
	Event.OnAcceptQuest.event1.Condition = function(player, questid, u1, u2, u3)
		return questid==u3
	end
	Event.OnAcceptQuest.event1.Args = function(player, questid, u1, u2, u3)
		return player, questid
	end

Event.OnUse={}
	Event.OnUse.event1={}
	Event.OnUse.event1.Condition = function(player, item, u1, u2, u3, u4)
		return((item.dwTabType .. "," .. item.dwIndex) == u3 and player.GetQuestPhase(player.GetQuestIndex(u4)) == 0)
	end
	Event.OnUse.event1.Args = function(player, item, u1, u2, u3, u4)
		return player, item, u3 , u4
	end