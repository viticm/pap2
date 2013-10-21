Include("scripts/flowlib/system/lib.lua")
using('comgr')
declare('API', {})
API.scene = {}
API.Quest = {}
API.player={}
API.npc={}

API.Quest.GetQuestValue = function(player, quest_id, value_index)
	return(player.GetQuestValue(player.GetQuestIndex(quest_id), value_index - 1))
end

API.Quest.SetQuestValue = function(player, quest_id, value_index, new_value)
	player.SetQuestValue(player.GetQuestIndex(quest_id), value_index - 1, new_value)
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

API.player.GetBoolProp = function(player, id)
	if id == 1 then
		return player.nCurrentLife > 0
	end
end

API.player.IsALive = function(player)
	return player.nCurrentLife > 0
end


API.player.OpenWindow = function(context, npc, player, str1, str2, str3, bShowAutoString)
	local str = string.format('%s\n<$C %s>\n<$C %s>\n', str1, str2, str3)
	comgr.pushw(player, context.co)
	local recall = {}
	for w in string.gfind(str, '<$(.-)>') do
		table.insert(recall, "OnSelect")
	end
	if (bShowAutoString) then
		player.OpenWindow(TARGET.NPC, npc.dwID, str.."\n<H28>"..npc.GetAutoDialogString(player.dwID).."\n<H28>",unpack(recall))
	else
  	player.OpenWindow(TARGET.NPC, npc.dwID, str, unpack(recall))
	end
	local nIndex = coroutine.yield()
	if nIndex == 0 then
		return str2
	elseif nIndex == 1 then
		return str3
	end
	return nil
end


--OpenWindow的回调函数
function OnSelect(player, nTargetType, nTargetID, nIndex)
	local co = comgr.findw(player)
	resume(co, nIndex)
end

-------------------------------------------------------------------------------------------
API.npc.SayToEveryBody = function(npc, str)
	npc.Say(str)
end


-------------------------------------------------------------------------------------------
--进度条
API.player.DoCustomOTAction = function(context, player, ...)
	comgr.pusho(player, context.co)
	local a1, a2 = unpack(arg)
	--local THIS_FILE = "scripts/map/测试1/api.lua"
	--系统接口，有Bug，传进去的文件名是不起作用的。会回调API所在的文件的文件名，而不是指定的文件名。 
	player.DoCustomOTAction(0, 1, a2 * 16, TARGET.NO_TARGET, 0, "scripts/flowlib/custom/api.lua", a1)
	local ret = coroutine.yield()
	return ret
end

--todo:此事件服务端待实现
function OnCustomEventBreak(Player, Target)
	local co = comgr.findo(Player)
	resume(co, "打断")
end
--进度条结束

--scene.Setimer
API.scene.Sleep = function(context, scene, time)
	print("into sleep")
	scene.SetTimer(time * 16, "scripts/flowlib/custom/api.lua", context.envid, 1)	
	coroutine.yield()
end

--player.Setimer
API.player.Sleep = function(context, player, time)
	--print("into player sleep")
	--print(debug.traceback())
	player.SetTimer(time * 16, "scripts/flowlib/custom/api.lua", context.envid, 2)	
	--print("before player sleep yield")
	--print(context.envid)	
	coroutine.yield()
	--print("after player sleep yield")
	--print(context.envid)
end

--回调函数，直接回调
function OnTimer(sender, p1, p2)
	local co = comgr.find(p1)
	resume(co) 
end

--回调函数，被别人includel了然后间接回调
function OnCustomEvent(Player, Target)
	local co = comgr.findo(Player)
	resume(co, 1)
end

--settimer结束
