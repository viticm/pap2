Include("scripts/map/测试1/api.lua")
using('API')
using('comgr')
using('Event')

create_sub = Event.Public.event_dispatch

--=================================================================
--接任务 
function OnAcceptQuest_q_id_chiyao()
	--API.player.AddItem(player, 5, 1844, 1) -- 红药丸
	--API.player.AddItem(player, 5, 1846, 1) -- 白药丸
end;

--=================================================================
--红药 
function onuse_hongyao(envid, player, item, item_key, questid)	
	local co = comgr.find(envid)
	local i
	g_finish = false
	print('吃红药')
	--print('API:'..tostring(API))
	--print('API.player:'..tostring(API.player))
	--print('API.player.GetQuestPhase='..tostring(API.player.GetQuestPhase))
 	if API.player.GetQuestPhase(player, 231) == 1 then 				
		print('开始走吃药进度条')
		local ret = API.player.DoCustomOTAction({envid=envid, co=co}, player, 0, 1, 3 * 16, TARGET.NO_TARGET, item.dwID, THIS_FILE, "正在吃药")    
		print('走完吃药进度条')
    		if ret == '完成' then
			print('设置任务变量1为真')
			API.Quest.SetQuestValue(player, questid, 0, 1) -- 吃过红药
			--循环中不能有子流程
			--分支计时
			--加经验
			create_sub(sub1, co, player, item, questid)			
			--sub2
			create_sub(sub2, co, player, item, questid)			
			--
		
			coroutine.yield() --等待子流程结束1
			coroutine.yield() --等待子流程结束2
		end 
	end
end

function onuse_main_jieyao(envid, player, item, item_key, questid) --吃解药 
	local co = comgr.find(envid)
	local i
	g_finish = false
	print('吃解药') 	
 	
	if API.player.GetQuestPhase(player, questid) == 1 then 	
		if API.Quest.GetQuestValue(player, questid, 0) == 1 then
			print('开始走吃解药进度条')
			local ret = API.player.DoCustomOTAction({envid=envid, co=co}, player, 0, 1, 3 * 16, TARGET.NO_TARGET, item.dwID, THIS_FILE, "正在吃解药...")    
			print('走完吃解药进度条')
			if ret == '完成' then
				if (API.player.IsALive(player)) then
					API.Quest.SetQuestValue(player, questid, 1, 1) -- 吃过解药
					--API.player.FinishQuest(player, questid) 
				end
			end
		else
			API.player.SendMessage(player, "你还没有吃过红药") 
		end		
	else
		API.player.SendMessage(player, "你没有接过任务") 
	end
end

function onusejieyao(envid)
	print('吃解药')
	g_finish = true
end

--加经验分支 
function sub1(envid, coparent, player, item, questid)
	local node1, node2
	local exit1 = 0;
		
	--有多入口节点	
	local exp = 0
	node1 = function()
		API.player.Sleep({envid=envid}, player, 1*16)
		print('加 1 经验, 经验值='..exp)
		API.player.AddExp(player, 1)
		exp = exp + 1
		return node2() --goto nextnode
	end
	
	node2 = function()	
	    if (API.Quest.GetQuestValue(player, questid, 1) == 1) then  --接了没完成 
			return
		else
			return node1()
		end
	end

	finishnode = function()  --自动生成的结束节点 
		resume(coparent)
	end

	node1()
	
	finishnode()	
end

function sub2(envid, coparent, player, item, questid)
	API.player.Sleep({envid=envid}, player, 10*16)
    if API.Quest.GetQuestValue(player, questid, 1) == 1 then 
		print('已经正常完成,不用死了!')
 	else
		API.Quest.SetQuestValue(player, questid, 1, 1)
		print('没有在规定时间内吃解药,玩家死了!')
  		--API.player.FinishQuest(player, questid)    		
		API.player.Die(player)					
	end
	resume(coparent)
end

--todo: questid 替换 