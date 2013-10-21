-- NPC脚本, 对话事件函数
function OnDialogue(npc, player)	    
	player.OpenWindow(TARGET.NPC, npc.dwID,	    
	    npc.GetQuestString()
        )
end