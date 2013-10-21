
update tbl_quests set objective=replace(objective,'<G>','') 
update tbl_quests set objective=replace(objective,'\n','\n<G>') 
update tbl_quests set objective=replace(objective,'\n<G><H28>','\n<H28><G>') 
update tbl_quests set objective='<G>' + objective +'!@#$%' 
update tbl_quests set objective=replace(objective,'<G>!@#$%','') 
update tbl_quests set objective=replace(objective,'!@#$%','')

update tbl_quests set [description]=replace([description],'<G>','') 
update tbl_quests set [description]=replace([description],'\n','\n<G>') 
update tbl_quests set [description]=replace([description],'\n<G><H28>','\n<H28><G>') 
update tbl_quests set [description]='<G>' + [description] +'!@#$%' 
update tbl_quests set [description]=replace([description],'<G>!@#$%','') 
update tbl_quests set [description]=replace([description],'!@#$%','')

update tbl_quests set unfinisheddialogue=replace(unfinisheddialogue,'<G>','') 
update tbl_quests set unfinisheddialogue=replace(unfinisheddialogue,'\n','\n<G>') 
update tbl_quests set unfinisheddialogue=replace(unfinisheddialogue,'\n<G><H28>','\n<H28><G>') 
update tbl_quests set unfinisheddialogue='<G>' + unfinisheddialogue +'!@#$%' 
update tbl_quests set unfinisheddialogue=replace(unfinisheddialogue,'<G>!@#$%','') 
update tbl_quests set unfinisheddialogue=replace(unfinisheddialogue,'!@#$%','')

update tbl_quests set finisheddialogue=replace(finisheddialogue,'<G>','') 
update tbl_quests set finisheddialogue=replace(finisheddialogue,'\n','\n<G>') 
update tbl_quests set finisheddialogue=replace(finisheddialogue,'\n<G><H28>','\n<H28><G>') 
update tbl_quests set finisheddialogue='<G>' + finisheddialogue +'!@#$%' 
update tbl_quests set finisheddialogue=replace(finisheddialogue,'<G>!@#$%','') 
update tbl_quests set finisheddialogue=replace(finisheddialogue,'!@#$%','')

update tbl_quests set dunningdialogue=replace(dunningdialogue,'<G>','') 
update tbl_quests set dunningdialogue=replace(dunningdialogue,'\n','\n<G>') 
update tbl_quests set dunningdialogue=replace(dunningdialogue,'\n<G><H28>','\n<H28><G>') 
update tbl_quests set dunningdialogue='<G>' + dunningdialogue +'!@#$%' 
update tbl_quests set dunningdialogue=replace(dunningdialogue,'<G>!@#$%','') 
update tbl_quests set dunningdialogue=replace(dunningdialogue,'!@#$%','')