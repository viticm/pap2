Include("scripts/flowlib/custom/api.lua")
using('API')
using('comgr')
using('Event')


--=================================================================
--接任务 
function OnAcceptQuest_q_id_chiyao()
	--API.player.AddItem(player, 5, 1844, 1) -- 红药丸
	--API.player.AddItem(player, 5, 1846, 1) -- 白药丸
end;

--=================================================================
--红药 
function onuse_hongyao(context, player, item, item_key, questid)	
	local co = context.co
	local node1, node2, node3, node4, node5, node6
	node1 = function(context, parentcontext)
		print('吃红药')		
		print('开始走吃药进度条')
		print(THIS_FILE)
		local ret = API.player.DoCustomOTAction(context, player, 0, 1, 3 * 16, TARGET.NO_TARGET, item.dwID, THIS_FILE, "正在吃药")    
		print('走完吃药进度条')
		if ret == '完成' then
			print('to node2' .. tostring(node2))
			return node2(context, parentcontext)
		else
			return finishnode(parentcontext)
		end 
	end

	node2 = function(context, parentcontext)
		print('设置任务变量1为真')
		API.Quest.SetQuestValue(player, questid, 0, 1) -- 吃过红药
		--循环中不能有子流程
		--分支计时
		--加经验
		create_subflow(node3, context)

		--sub2		
		create_subflow(node5, context)
	
		coroutine.yield() --等待子流程结束1
		coroutine.yield() --等待子流程结束2

	end

	node3 = function(context, parentcontext)
		API.player.Sleep({envid=envid}, player, 1*16)
		print('加 1 经验')
		API.player.AddExp(player, 1)
		return node4(context, parentcontext) --goto nextnode
	end

	node4 = function(context, parentcontext)
		if (API.Quest.GetQuestValue(player, questid, 1) == 1) then  --接了没完成 
			return finishnode(parentcontext)
		else
			return node3(context, parentcontext)
		end
	end

	node5 = function(context, parentcontext)
		API.player.Sleep({envid=envid}, player, 10*16)
		if API.Quest.GetQuestValue(player, questid, 1) == 1 then 
			print('已经正常完成,不用死了!')
			return finishnode(parentcontext)
		else
			return node6(context, parentcontext)
		end
	end

	node6 = function(context, parentcontext)
		API.Quest.SetQuestValue(player, questid, 1, 1)
		print('没有在规定时间内吃解药,玩家死了!')
		--API.player.FinishQuest(player, questid)    		
		API.player.Die(player)		
		return finishnode(parentcontext)
	end

	node1(context, nil)
end

function onuse_main_jieyao(context, player, item, item_key, questid) --吃解药 
	local co = context.co
	local node1, node2, node4, node5, node6, node7
	node1 = function(envid)
		print('吃解药') 	
		return node2(envid)
	end

	node2 = function(envid)
		if API.Quest.GetQuestValue(player, questid, 0) == 1 then
			return node4(envid)
		else
			return node5(envid)
		end		
	end

	node4 = function(envid)
		print('开始走吃解药进度条')
		local ret = API.player.DoCustomOTAction({envid=envid, co=co}, player, 0, 1, 3 * 16, TARGET.NO_TARGET, item.dwID, THIS_FILE, "正在吃解药...")    
		print('走完吃解药进度条')
		if ret == '完成' then
			return node7(envid)
		end
	end

	node5 = function(envid)
		API.player.SendMessage(player, "你还没有吃过红药") 
	end

	node6 = function(envid)
		API.Quest.SetQuestValue(player, questid, 1, 1) -- 吃过解药
		--API.player.FinishQuest(player, questid) 
	end

	node7 = function(envid)
		if (API.player.IsALive(player)) then
			return node6(envid)
		end
	end

	node1(envid)
end

--注册事件
if Event.OnUse.event1.FunList["5,1844"] == nil then
	Event.OnUse.event1.FunList["5,1844"] = {}
end
Event.OnUse.event1.FunList["5,1844"][onuse_hongyao]		= {nil, nil, "5,1844",231}

if Event.OnUse.event1.FunList["5,1846"] == nil then 
	Event.OnUse.event1.FunList["5,1846"] = {} 
end
Event.OnUse.event1.FunList["5,1846"][onuse_main_jieyao]		= {nil, nil, "5,1846",231}
--todo: questid 替换 