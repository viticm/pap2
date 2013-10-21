Include("scripts/map/测试1/EventAPI.lua")
Include("scripts/map/测试1/lib.lua")
using('comgr')
declare('API', {})
API.scene = {}
API.Quest = {}
API.player={}

API.Quest.GetQuestValue = function(player, quest_id, value_index)
	return(player.GetQuestValue(player.GetQuestIndex(quest_id), value_index))
end

API.Quest.SetQuestValue = function(player, quest_id, value_index, new_value)
	player.SetQuestValue(player.GetQuestIndex(quest_id), value_index, new_value)
end	

------------------------------------------------
API.player.AddItem = function(player, p1, p2, p3)
	player.AddItem(p1, p2, p3)
end

API.player.DelItem = function(player, p1, p2, p3)
	player.DelItem(p1, p2, p3)
end

API.player.GetQuestPhase = function(player, p1)
	print(tostring(player), tostring(p1))
	local key = player.GetQuestPhase(p1)
	return key
end

API.player.SendMessage = function(player, message)
	player.SendSystemMessage(message)
end

API.player.SetCustomValue = function(player, name, value)

end

API.player.FinishQuest = function(player, id)
	player.ForceFinishQuest(id)
end

API.player.Die = function(player)
	player.Die()
end

API.player.AddExp = function(player, nExp)
	player.AddExp(nExp)
end

API.player.IsALive = function(player)
	return player.nCurrentLife > 0
end

--进度条
API.player.DoCustomOTAction = function(context, player, ...)
	comgr.pusho(player, context.co)
	
	local a1, a2, a3, a4, a5, a6, a7 = unpack(arg)
	--local THIS_FILE = "scripts/map/测试1/api.lua"
	player.DoCustomOTAction(a1, a2, a3, a4, a5, "scripts/map/测试1/api.lua", a7)
	local ret = coroutine.yield()
	return ret
end

function OnCustomEvent(Player, Target)
	local co = comgr.findo(Player)
	resume(co, "完成")
end

regcallback("OnCustomEvent", OnCustomEvent)  --注册回调函数

--todo:此事件服务端待实现
function OnCustomEventBreak(Player, Target)
	local co = comgr.findo(Player)
	resume(co, "打断")
end
--进度条结束

--scene.Setimer
API.scene.Sleep = function(context, scene, time)
	scene.SetTimer(time, "scripts/map/测试1/api.lua", context.envid, 1)	
	coroutine.yield()
end

--player.Setimer
API.player.Sleep = function(context, player, time)
	player.SetTimer(time, "scripts/map/测试1/api.lua", context.envid, 2)	
	coroutine.yield()
end

function OnTimer_Dispatch(sender, p1, p2)
	local co = comgr.find(p1)
	resume(co) 
end

regcallback("OnTimer", OnTimer_Dispatch)  --注册回调函数 

--settimer结束
