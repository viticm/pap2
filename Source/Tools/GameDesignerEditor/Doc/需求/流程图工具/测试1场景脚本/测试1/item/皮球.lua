-- ITEM脚本, 在玩家使用道具的时候触发
Include("scripts/map/测试1/event.lua")
function OnUse(player, item, dwSourceBox, dwSourceSlot, dwTargetBox, dwTargetSlot)
	local sta, err = pcall(function() AOnUse(player, item) end)
	if err then
		print("err: " .. err)
		print(debug.traceback())
	end
	return false, false
end