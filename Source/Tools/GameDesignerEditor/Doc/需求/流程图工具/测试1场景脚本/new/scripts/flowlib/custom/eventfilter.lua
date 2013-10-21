Include("scripts/flowlib/system/lib.lua")
using('comgr')
declare('Event', {})

Event[8] ={}
Event[8].FunList = {}

Event[8].Condition = function(player, questid, ...)
	return questid==arg[3]
end
Event[8].Args = function(player, questid, ...)
	return player, questid
end


Event[2] = {}
Event[2].FunList = {}

Event[2].Condition = function(player, item, ...)
	return((item.dwTabType .. "," .. item.dwIndex) == arg[3] and player.GetQuestPhase(arg[4]) == 1)
end
Event[2].Args = function(player, item, ...)
	return player, item, arg[3] , arg[4]
end	

Event[7] ={}
Event[7].FunList = {}
--以上内容请不要删除

--可用参数：
-- arg[1]: 玩家
-- arg[2]: NPC
-- arg[3]: 指定NPC
Event[7].Condition = function(npc, player, ...)
	--TODO:
	return npc.dwTemplateID == arg[3]
end

Event[7].Args = function(npc, player, ...)
	--TODO:
	return player, npc, arg[3] --共3个返回值 
end
