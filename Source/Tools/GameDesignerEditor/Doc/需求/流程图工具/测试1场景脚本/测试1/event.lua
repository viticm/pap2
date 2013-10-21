Include("scripts/map/≤‚ ‘1/quest.lua")
Include("scripts/map/≤‚ ‘1/EventAPI.lua")

function AOnAcceptQuest(player, questid)
	if Event.OnAcceptQuest.event1.Condition(player,questid, nil, nil, 231) then
		Event.Public.event_dispatch(OnAcceptQuest_q_id_chiyao, Event.OnAcceptQuest.event1.Args(player, questid, nil, nil, 231))
	end
end

function AOnUse(player, item)
	if Event.OnUse.event1.Condition(player,item,nil,nil,"5,1844",231) then
		Event.Public.event_dispatch(onuse_hongyao, Event.OnUse.event1.Args(player,item,nil,nil,"5,1844",231))
	end
	if Event.OnUse.event1.Condition(player,item,nil,nil,"5,1846",231) then
		Event.Public.event_dispatch(onuse_main_jieyao, Event.OnUse.event1.Args(player,item,nil,nil,"5,1846",231))
	end
end