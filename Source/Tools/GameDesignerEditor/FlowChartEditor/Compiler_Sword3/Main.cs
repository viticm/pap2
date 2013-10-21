using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;

using LuaInterface;

using FlowChartDataLibrary;
using BaseExp;
using CodeProvider;
using DBManager;
using CodeCompiler;

namespace Compiler_Sword3
{
    public class Main : Compiler
    {
        private Encoding m_gb2312Encoding = Encoding.GetEncoding("gb2312");
        CodeWriterMemory m_cwm = new CodeWriterMemory();
        private LuaEx m_lua = new LuaEx();
        private string m_outdir = "";
        private string m_rootdir = "";
        private string m_mapname = "";
        private IniStructure m_inis = null;
        private List<string> exportTableList = new List<string>(); // 导出数据表的链表

        /// <summary>
        /// 构造函数
        /// </summary>
        public Main()
        {
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            m_lua.Init(); //重载载入编译模板
        }

        /// <summary>
        /// 注册lua函数
        /// </summary>
        private void RegistLuaFunction(FlowChartMetaDataManager manager)
        {                        
            m_lua.RegisterFunction("GetEventCount", manager, typeof(FlowChartMetaDataManager).GetMethod("GetEventCount"));
            m_lua.RegisterFunction("GetEventFunctionName", manager, typeof(FlowChartMetaDataManager).GetMethod("GetEventFunctionName"));
            m_lua.RegisterFunction("GetNodeNames", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeNames"));
            m_lua.RegisterFunction("GetNodeCount", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeCount"));
            m_lua.RegisterFunction("GetNodeName", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeName"));
            m_lua.RegisterFunction("GetNodeType", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeType"));
            m_lua.RegisterFunction("GetNodeCode", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeCode"));
            m_lua.RegisterFunction("GetNextConnetionCount", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNextConnetionCount"));
            m_lua.RegisterFunction("IsNextNodeFinish", manager, typeof(FlowChartMetaDataManager).GetMethod("IsNextNodeFinish"));
            m_lua.RegisterFunction("GetNextNodeName", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNextNodeName"));
            m_lua.RegisterFunction("GetNextThreadCount", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNextThreadCount"));
            m_lua.RegisterFunction("GetNodeSwitchCount", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeSwitchCount"));
            m_lua.RegisterFunction("GetConditionResult", manager, typeof(FlowChartMetaDataManager).GetMethod("GetConditionResult"));
            m_lua.RegisterFunction("GetNodeText", manager, typeof(FlowChartMetaDataManager).GetMethod("GetNodeText"));
            m_lua.RegisterFunction("GetEventText", manager, typeof(FlowChartMetaDataManager).GetMethod("GetEventText"));
            m_lua.RegisterFunction("GetEventData", manager, typeof(FlowChartMetaDataManager).GetMethod("GetEventData"));
            // m_lua.RegisterFunction("log", this, typeof(Main).GetMethod("Log"));
            m_lua.RegisterFunction("ExpToString", this, typeof(CodeProviderClass).GetMethod("ExpToString"));
            m_lua.RegisterFunction("SetScript", this, typeof(Main).GetMethod("SetScript"));
            m_lua["out"] = m_cwm;
        }

        /// <summary>
        /// 生成流程图脚本
        /// </summary>
        /// <param name="FlowID">流程图ID</param>
        /// <param name="data">流程图数据</param>
        /// <param name="manager">流程图管理器</param>
        /// <returns>是否生成成功</returns>
        public override bool GenerateCode(int FlowID, List<FlowChartMetaData> data, FlowChartMetaDataManager manager)
        {
            // 初始化数据
            string fullPath = Path.Combine(rootDirectory, "scripts\\Map");
            fullPath = Path.Combine(fullPath, scriptPath);

            string strOutDir = fullPath;
            this.m_outdir = fullPath;
            this.m_rootdir = rootDirectory;
            this.m_mapname = manager.MapName;

#if DEBUG
            m_lua.Init(); //重载载入编译模板
#endif

            RegistLuaFunction(manager); // 注册lua函数
            
            LuaFunction fun = m_lua.GetFunction("Compiler.main");
            string strCurrTemp = Path.Combine(Application.StartupPath, "eventimp.template");
            object[] rets = fun.Call(strCurrTemp);

            if (rets != null) 
            {
                string s = rets[0].ToString();

                DirectoryInfo dinfo = Directory.GetParent(strOutDir);
                dinfo.Create();

                #region 测试代码
                File.WriteAllText(strOutDir + ".temp.obj", s, m_gb2312Encoding);
                #endregion
                           
                m_cwm.Init();

                try
                {
                    m_lua.DoString(s);
                    ExportLib(); // 导出库文件

                    return true;                    
                }
                catch(Exception e)
                {
                    throw e;
                }
                finally
                {
                    if (saveFile != null)
                    {                        
                        // 为多语言版本转换中文字符串
                        string scriptText = m_cwm.ToString();                                                
                        saveFile(string.Format("{0}.lua", strOutDir), scriptText);
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 挂脚本
        /// </summary>
        /// <param name="constExp">表达式对象</param>
        /// <returns>是否挂接成功</returns>
        public bool SetScript(ConstExp constExp)
        {
            string FileName = "";
            StringBuilder Content = new StringBuilder();
            string Sql = "";
            Content.AppendLine("--" + constExp.ToString());
            Content.AppendLine("Include('scripts/flowlib/api.lua')");
            Content.AppendLine("Include('scripts/flowlib/event_dispatch.lua')");
            Content.AppendLine("using('EventDispatch')");

            switch (constExp.ReturnType.DBID)
            {
                case 13:    //道具模板
                    #region 道具生成

                    //生成文件
                    FileName = Path.Combine(this.m_rootdir, @"scripts\flowlib\catcher\item\" + constExp.DBValue.Replace(",", "_") + ".lua");
                    Content.Append(string.Format(@"
function OnUse(player, item)
	local sta, err = pcall(function() EventDispatch.AOnUse('{0}', player, item) end)
	if err then
		print('err: ' .. err)
	end
	return false, false
end
", constExp.DBValue));
                    //写数据库
                    if (!constExp.DBValue.Contains(","))
                        return false;
                    string r1 = constExp.DBValue.Split(new char[] { ',' })[0];
                    string r2 = constExp.DBValue.Split(new char[] { ',' })[1];
                    string sql = "";
                    switch(r1)
                    {
                        case "5":
                            {
                                sql = "Other";
                                break;
                            }
                        case "6":
                            {
                                sql = "item_Custom_Weapon";
                                break;
                            }
                        case "7":
                            {
                                sql = "item_Custom_Armor";                                
                                break;
                            }
                        case "8":
                            {
                                sql = "Custom_Trinket";                                
                                break;
                            }
                    }

                    if (!exportTableList.Contains(sql))
                    {
                        exportTableList.Add(sql);
                    }                    

                    sql = string.Format("update [{0}] set scriptname='scripts\\flowlib\\catcher\\item\\{1}.lua' where id='{2}'", sql, constExp.DBValue.Replace(",", "_"), r2);
                    Sql = sql;
                    #endregion
                    break;
                case 20:    //NPC模板
                    #region NPC模板
                    //生成文件
                    FileName = Path.Combine(this.m_rootdir, @"scripts\flowlib\catcher\npc\" + constExp.DBValue + ".lua");
                    Content.Append(string.Format(@"
function OnDialogue(npc, player)
    local gotevent = false
    local sta, err = pcall(function() gotevent = EventDispatch.AOnDialogue('{0}', npc, player) end)
    if err then
	    print('err: ' .. err)
    end
    if not gotevent then
	  	player.OpenWindow(TARGET.NPC, npc.dwID,
                  npc.GetAutoDialogString(player.dwID)
                  )
    end
end;

function OnDeath(npc, killer)
    local sta, err = pcall(function() EventDispatch.AOnDeath('{0}', npc) end)     --任何情况下的死亡
    if err then
        print('err: ' .. err)
    end
    if not killer then
        sta, err = pcall(function() EventDispatch.AOnNaturalDeath('{0}', npc) end)   --自然死亡
        if err then
            print('err: ' .. err)
        end
    elseif IsPlayer(killer) then
        sta, err = pcall(function() EventDispatch.AOnDeathByPlayer('{0}', npc, killer) end)  --被player杀死的
        if err then
            print('err: ' .. err)
        end   
    else
        sta, err = pcall(function() EventDispatch.AOnDeathByNpc('{0}', npc, killer) end)     --被NPC杀死的
        if err then
            print('err: ' .. err)
        end        
    end    
end;
", constExp.DBValue));

                    //写数据库
                    Sql = string.Format("update NpcTemplate set scriptname='scripts\\flowlib\\catcher\\npc\\{0}.lua' where id='{0}'", constExp.DBValue);
                    
                    //检查实体覆盖模板的问题
                    string strResult = "";

                    if (this.m_inis == null)
                    {
                        string mapname = this.m_mapname;
                        string iniFileName = Path.Combine(this.m_rootdir,
                            @"data\source\maps\" + mapname + "\\" + mapname + ".Map.Logical");
                        if (!File.Exists(iniFileName)) break;
                        this.m_inis = new IniStructure();
                        this.m_inis = IniStructure.ReadIni(iniFileName);
                    }
                    
                    if (m_inis == null) break;
                    int nNpcNumber = Convert.ToInt32(m_inis.GetValue("MAIN", "NumNPC").ToString());

                    for (int i = 0; i < nNpcNumber; i++)
                    {
                        string szName = m_inis.GetValue("NPC" + i.ToString(), "szName");
                        string dwScriptID = m_inis.GetValue("NPC" + i.ToString(), "dwScriptID");
                        string nTempleteID = m_inis.GetValue("NPC" + i.ToString(), "nTempleteID");
                        string nX = m_inis.GetValue("NPC" + i.ToString(), "nX");
                        string nY = m_inis.GetValue("NPC" + i.ToString(), "nY");
                        string nZ = m_inis.GetValue("NPC" + i.ToString(), "nZ");
                        if (nTempleteID == constExp.DBValue && dwScriptID != "00000000")
                        {
                            strResult += string.Format("{4} 的 {0}({1},{2},{3}) 在场景编辑器已经挂接了脚本，自动挂接是通过写模板实现的，所以流程图对于这个实体将无效。", szName, nX, nY, nZ, this.m_mapname) + "\r\n";
                        }
                    }
                    if(strResult != "")
                    {
                        MessageBox.Show(strResult, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }

                    if (!exportTableList.Contains("NpcTemplate"))
                    {
                        exportTableList.Add("NpcTemplate");
                    }     

                    #endregion
                    break;
                case 21:    //任务模板
                    #region 任务模板
                    //生成文件
                    FileName = Path.Combine(this.m_rootdir, @"scripts\flowlib\catcher\quest\" + constExp.DBValue + ".lua");
                    Content.Append(string.Format(@"
function OnAcceptQuest(player)
	local sta, err = pcall(function() EventDispatch.AOnAcceptQuest('{0}', player, '{0}') end)
	if err then
		print('err: ' .. err)
	end
end;

function OnFinishQuest(player)
	local sta, err = pcall(function() EventDispatch.AOnFinishQuest('{0}', player, '{0}') end)
	if err then
		print('err: ' .. err)
	end
end;

function OnCancelQuest(player)
	local sta, err = pcall(function() EventDispatch.AOnCancelQuest('{0}', player, '{0}') end)
	if err then
		print('err: ' .. err)
	end
end;
", constExp.DBValue));
                    //写数据库
                    Sql = string.Format("update tbl_quests set scriptname='scripts\\flowlib\\catcher\\quest\\{0}.lua' where questid='{0}'", constExp.DBValue);

                    if (!exportTableList.Contains("tbl_quests"))
                    {
                        exportTableList.Add("tbl_quests");
                    }     

                    #endregion
                    break;
                case 35:    //Doodad模板
                    #region Doodad模板
                    //生成文件
                    FileName = Path.Combine(this.m_rootdir, @"scripts\flowlib\catcher\doodad\" + constExp.DBValue + ".lua");
                    Content.Append(string.Format(@"
function OnOpen(doodad, player)
    local gotevent = false
    local sta, err = pcall(function() gotevent = EventDispatch.AOnOpen('{0}', doodad, player) end)
    if err then
	    print('err: ' .. err)
    end
    if not gotevent then
	  	--做默认的事情
    end
	return false
end;

function OnBreak(doodad, player)
    local gotevent = false
    local sta, err = pcall(function() gotevent = EventDispatch.AOnBreak('{0}', doodad, player) end)
    if err then
	    print('err: ' .. err)
    end
end;

function OnPick(doodad, player)
    local gotevent = false
    local sta, err = pcall(function() gotevent = EventDispatch.AOnPick('{0}', doodad, player) end)
    if err then
	    print('err: ' .. err)
    end
end;

", constExp.DBValue));
                    //写数据库
                    Sql = string.Format("update tbl_doodad set script='scripts\\flowlib\\catcher\\doodad\\{0}.lua' where id='{0}'", constExp.DBValue);
                    
                    //实体覆盖模板
                    strResult = "";
                    if (this.m_inis == null)
                    {
                        string mapname = this.m_mapname;
                        string iniFileName = Path.Combine(this.m_rootdir,
                            @"data\source\maps\" + mapname + "\\" + mapname + ".Map.Logical");
                        if (!File.Exists(iniFileName)) break;
                        this.m_inis = new IniStructure();
                        this.m_inis = IniStructure.ReadIni(iniFileName);
                    }
                    if (m_inis == null) break;
                    nNpcNumber = Convert.ToInt32(m_inis.GetValue("MAIN", "NumDoodad").ToString());

                    for (int i = 0; i < nNpcNumber; i++)
                    {
                        string szName = m_inis.GetValue("Doodad" + i.ToString(), "szName");
                        string dwScriptID = m_inis.GetValue("Doodad" + i.ToString(), "dwScriptID");
                        string nTempleteID = m_inis.GetValue("Doodad" + i.ToString(), "nTempleteID");
                        string nX = m_inis.GetValue("Doodad" + i.ToString(), "nX");
                        string nY = m_inis.GetValue("Doodad" + i.ToString(), "nY");
                        string nZ = m_inis.GetValue("Doodad" + i.ToString(), "nZ");
                        if (nTempleteID == constExp.DBValue && dwScriptID != "00000000")
                        {
                            strResult += string.Format("{4} 的 {0}({1},{2},{3}) 在场景编辑器已经挂接了脚本，自动挂接是通过写模板实现的，所以流程图对于这个实体将无效。", szName, nX, nY, nZ, this.m_mapname) + "\r\n";
                        }
                    }
                    if (strResult != "")
                    {
                        MessageBox.Show(strResult, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }

                    if (!exportTableList.Contains("tbl_doodad"))
                    {
                        exportTableList.Add("tbl_doodad");
                    }

                    #endregion
                    break;
                case 36:    //Trap模板
                    #region Trap模板
                    string[] list = constExp.DBValue.Split(new char[] { ',' });
                    //生成文件
                    FileName = Path.Combine(this.m_rootdir, @"scripts\flowlib\catcher\trap\" + list[0] + ".map." + list[1] + ".lua");
                    Content.Append(string.Format(@"
function OnEnterTrap(player, cell)
	local sta, err = pcall(function() EventDispatch.AOnEnterTrap('{0}', player, cell, '{0}') end)
	if err then
		print('err: ' .. err)
	end
end;

function OnLeaveTrap(player, oldCell)
	local sta, err = pcall(function() EventDispatch.AOnLeaveTrap('{0}', player, oldCell, '{0}') end)
	if err then
		print('err: ' .. err)
	end
end;

", constExp.DBValue));
                    #endregion
                    break;
                case 39:    //地图模板
                    SetSceneScript();
                    return true;
                default:
                    throw new Exception("目前不支持[" + constExp.ToString() + "]类型的捕获器挂接");
            }
            if (saveFile != null)
            {
                saveFile(FileName, Content.ToString());            //导出lua文件
            }

            /*
            if (executeSQLCommand != null && Sql != "")
            {
                executeSQLCommand(Sql);
            }
            */ 

            return true;
        }

        public void SetSceneScript()
        {
            string FileName = "";
            StringBuilder Content = new StringBuilder();
            Content.Append("--" + this.m_mapname + "\r\n");
            Content.Append("Include('scripts/flowlib/api.lua')\r\n");
            Content.Append("Include('scripts/flowlib/event_dispatch.lua')\r\n");
            Content.Append("using('EventDispatch')\r\n");
            //生成文件
            FileName = Path.Combine(this.m_rootdir, @"scripts\Map\" + this.m_mapname + "\\" + this.m_mapname + ".lua");
            DataTable dt = DBManager.DataBaseManager.GetDataBaseManager().GetDataTable("select id from maplist where name='" + this.m_mapname + "'", DataBaseManager.GetDataBaseManager().Connection_Jx3web);
            if (dt.Rows.Count == 0)
            {
                return;
            }
            string strMapDbValue = this.m_mapname + "," + dt.Rows[0]["id"].ToString();
            Content.Append(string.Format(@"

----------------------------------------------
-- 此脚本文件的名字必须和场景名称一样(此脚本文件当前目录名字)才会正常工作。
----------------------------------------------
Include('Scripts/Include/Player.lh');

-- 场景事件, 当地图加载初始化的时候触发一次
function OnInitScene(scene)
	local sta, err = pcall(function() EventDispatch.AOnInitScene('{0}', scene) end)
	if err then
		print('err: ' .. err)
	end
end

-- 场景事件, 当有玩家进入这张场景时触发
function OnEnterScene(player, scene)
	local sta, err = pcall(function() EventDispatch.AOnEnterScene('{0}', player, scene) end)
	if err then
		print('err: ' .. err)
	end
end;

-- 场景事件, 当有玩家离开这张场景时触发
function OnLeaveScene(player, scene)
	local sta, err = pcall(function() EventDispatch.AOnLeaveScene('{0}', player, scene) end)
	if err then
		print('err: ' .. err)
	end
end;

-- 场景事件, 当有玩家在这张场景复活时触发
function OnPlayerRevive(player, nMode)
	-- 下面是复活点的列表, 可以根据需要添加
	local tRevivePos = {{ 
		{{MapID = 11, X = 61657, Y = 16738, Z = 0, Face = 70, Camp = 'Neutral', Commit = '天策复活点一。'}},
		{{MapID = 11, X = 27641, Y = 23918, Z = 0, Face = 70, Camp = 'Neutral', Commit = '天策复活点二。'}},
		{{MapID = 11, X = 25886, Y = 44363, Z = 0, Face = 70, Camp = 'Neutral', Commit = '天策复活点三。'}},
		{{MapID = 11, X = 43793, Y = 47875, Z = 0, Face = 70, Camp = 'Neutral', Commit = '天策复活点四。'}},
	}}

	-- 这个是调用的 Include/player.lh 中的一个函数, 用来控制玩家复活的位置等, 默认是复活到最近的复活点
	-- 实际上下面可以自己写符合自己场景设计的复活规则, 比如不是复活在最近的复活点, 或者副本里面不能原地复活等.
	RevivePlayerCommon(player, nMode, tRevivePos)

	local sta, err = pcall(function() EventDispatch.AOnPlayerRevive('{0}', player, scene) end)
	if err then
		print('err: ' .. err)
	end    
end

", strMapDbValue));
            saveFile(FileName, Content.ToString());            //导出场景lua文件
        }

        //导出库文件
        private void ExportLib()
        {
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
//---------------------------------------------------------------------------------
            string strLib = @"
if (_G.flowlib == nil) then
	_G.flowlib = {}
end

_G.using = function(varname)
	local thisenv = getfenv(2)
	thisenv[varname] = _G.flowlib[varname]	
end

_G.declare = function(varname, varvalue)
    if not _G.flowlib[varname] then
	    _G.flowlib[varname] = varvalue
	    local thisenv = getfenv(2)
	    thisenv[varname] = _G.flowlib[varname]
    end
end

_G.regcallback = _G.declare

_G.catcharg = function (...)
	return arg
end

_G.resume = function (...)
	local t = catcharg(coroutine.resume(...))
	local errmsg = t[1]
	if (errmsg == false) then
		print('协程内错误: ' .. t[2])
		print(debug.traceback())
	end

    return unpack(t)
end

if not comgr then
	declare('comgr', {})
	comgr.colist = {}
	-- setmetatable(comgr.colist, {__mode = 'v'})

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
	-- setmetatable(comgr.colist_w, {__mode = 'v'})

	comgr.pushw = function(player, co)
	   comgr.colist_w[player.dwID] = co
	end

	comgr.findw = function(player)
	   return comgr.colist_w[player.dwID]
	end


	comgr.colist_o = {}
	-- setmetatable(comgr.colist_o, {__mode = 'v'})

	comgr.pusho = function(player, co)
	   comgr.colist_o[player.dwID] = co
	end

	comgr.findo = function(player)
	   return comgr.colist_o[player.dwID]
	end

    comgr.colist_npc = {}
	-- setmetatable(comgr.colist_npc, {__mode = 'v'})

	comgr.pushNpc = function(npc, co)
	   comgr.colist_npc[npc.dwID] = co
	end

	comgr.findNpc = function(npc)
	   return comgr.colist_npc[npc.dwID]
	end
end

_G.event_dispatch = function(func, ...)
	local co = coroutine.create(func)
	local thisenvid = comgr.push(co)
	resume(co, {envid=thisenvid, co=co}, ...)
	return
end

_G.create_subflow = function(func, context, ...)
	local co = coroutine.create(func)
	local thisenvid = comgr.push(co)
	resume(co, {envid=thisenvid, co=co, args=context.args}, ...)
	return
end


_G.finishnode = function(parentcontext)  --自动生成的结束节点 
	if parentcontext then
		resume(parentcontext.co) --分支节点使用，用于退出本子流程，回到上一层流程
        -- 释放协程资源
		if parentcontext.co then
			local tempKey
			local findKey = false
			
			for k, v in pairs(comgr.colist) do
			   if v == parentcontext.co then
			       tempKey = k
			       findKey = true;
			       break
			   end
			end
			
			if findKey then
				comgr.colist[tempKey] = nil
			end
			
			findKey = false
			for k, v in pairs(comgr.colist_w) do
			   if v == parentcontext.co then
			       tempKey = k
			       findKey = true;
			       break
			   end
			end
			
			if findKey then
				comgr.colist_w[tempKey] = nil
			end
			
			findKey = false
			for k, v in pairs(comgr.colist_o) do
			   if v == parentcontext.co then
			       tempKey = k
			       findKey = true;
			       break
			   end
			end
			
			if findKey then
				comgr.colist_o[tempKey] = nil
			end
			
			findKey = false
			for k, v in pairs(comgr.colist_npc) do
			   if v == parentcontext.co then
			       tempKey = k
			       findKey = true;
			       break
			   end
			end
			
			if findKey then
				comgr.colist_npc[tempKey] = nil
			end
		end
	end
end

_G.null = {}
setmetatable(_G.null, {__tostring = function() return 'null' end})

_G.trace = print

--保存参数 对象转为ID
_G.SaveArgsToID = function(oldargs)
	local argslist = {}
	local type2fun = {
		['KNpc'] = GetNpc,
		['KPla'] = GetPlayer,
		['KIte'] = GetItem,
		['KSce'] = GetScene,
		['KDoo'] = GetDoodad,
		['KSki'] = GetSkill,
	}
	for k = 1, table.getn(oldargs) do
		v = oldargs[k]

		if type(v) == 'userdata' then
			local szType = string.sub(tostring(v), 0, 4)	
			for k1,v1 in pairs(type2fun) do
			   if szType == k1 then
			       argslist[k] = {v1, v.dwID}
			   end
			end
			if argslist[k] == nil then error('unknown object:' .. tostring(v)) end
		else
		   argslist[k] = { (function(id) return id end), v}
		end
	end
	return argslist
end

--恢复参数 ID 转回对象
_G.RestoreArgsFromID =function(argslist)
	local oldargs = {}
	for k,v in pairs(argslist) do
		local fun, id = unpack(v)
		oldargs[k] = fun(id)
	end
	oldargs.n = table.getn(argslist)
	return oldargs
end


";
//-------------------------------------------------------------------------------------
            string strSystem = @"
Include('scripts/flowlib/lib.lua')
using('comgr')

function playerlogout(player)
	if comgr.colist_w[player.dwID] then
		table.remove(comgr.colist_w, player.dwID)
	end
end
";
            saveFile(Path.Combine(this.m_rootdir, @"scripts\flowlib\lib.lua"), strLib);
            saveFile(Path.Combine(this.m_rootdir, @"scripts\flowlib\system.lua"), strSystem);
            saveFile(Path.Combine(this.m_rootdir, @"scripts\flowlib\api.lua"), dbm.GetAPIFileContext());
            saveFile(Path.Combine(this.m_rootdir, @"scripts\flowlib\event_dispatch.lua"), dbm.GetEventDispatchFileContext());
            saveFile(Path.Combine(this.m_rootdir, @"scripts\flowlib\event_filter.lua"), dbm.GetEventFilterFileContext());
        }

        /// <summary>
        /// 导出数据表
        /// </summary>
        /// <returns>是否导出成功</returns>
        public bool GenerateFile()
        {            
            TabExport te = new TabExport(exportTableList, this.m_rootdir);
            Hashtable ht = te.WriteFileList;

            if (ht == null)
            {
                return false;
            }

            foreach (string filename in ht.Keys)
            {
                saveFile(filename, ht[filename].ToString()); // 导出tab文件
            }

            return true;
        }
    }
}
