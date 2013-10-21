using System;
using System.Collections.Generic;
using System.Text;
using LuaInterface;

namespace GameDesingerTools
{
    public class LuaEx : LuaInterface.Lua
    {
        public LuaEx()
            : base()
        {
            Init();
        }
        void Init()
        {
            try
            {
                this["MainForm"] = Program.MainForm;// (MainForm)ParentForm;

                //建立环境帮助函数
                //this.DoString("function trace() \r\n end\r\n");
                //string sCode = "____main_env_varTable = {}\r\nlocal function _MakeEnv(envname)\r\n	____main_env_varTable[envname] = {}\r\n	_G[\"__fname__\"..envname] = ____main_env_varTable[envname]\r\n____main_env_varTable[envname].envname = envname\r\n	setmetatable(____main_env_varTable[envname], {__index = _G})\r\nend\r\nfunction _ChgEnv(envname)\r\n	if (envname == nil) then\r\n        setfenv(2, _G)\r\n	elseif (____main_env_varTable[envname] == nil) then\r\n  		_MakeEnv(envname)\r\n  		setfenv(2, ____main_env_varTable[envname])\r\n else\r\n    	setfenv(2, ____main_env_varTable[envname])\r\n	end\r\nend\r\n";
                string sCode = @"
                    local ____main_env_varTable = {}   -- package
                    local function _MakeEnv(envname, parentenv)
                        --setfenv(1, _G)
	                    ____main_env_varTable[envname] = {}
	                    _G['__fname__'..envname] = ____main_env_varTable[envname]
	                    ____main_env_varTable[envname].envname = envname
                    	
	                    if (parentenv and ____main_env_varTable[parentenv] == nil) then
		                    _MakeEnv(parentenv, nil)
	                    end

	                    if (parentenv and ____main_env_varTable[parentenv]) then
		                    ____main_env_varTable[envname].parent = ____main_env_varTable[parentenv]
	                    else
    	                    ____main_env_varTable[envname].parent = _G
	                    end

	                    if (parentenv == nil) then
		                    setmetatable(____main_env_varTable[envname], {__index = _G})
	                    else
	                        setmetatable(____main_env_varTable[envname], {__index = ____main_env_varTable[parentenv]})  --设置父环境
	                    end
                    end

                    function _ChgEnv(envname, parentenv)
	                    if (envname == nil) then
                            setfenv(2, _G)
	                    elseif (____main_env_varTable[envname] == nil) then
  		                    _MakeEnv(envname, parentenv)
	                    end
  	                    setfenv(2, ____main_env_varTable[envname])
  	                    if (parentenv == nil or ____main_env_varTable[parentenv] == nil) then
  	                        _parentenv = _G
	                    else
		                    _parentenv = ____main_env_varTable[parentenv] --设置父环境
	                    end
                    end                    
                    
                    luanet.load_assembly('System.Drawing')
                    Rectangle=luanet.import_type('System.Drawing.Rectangle')                    
                ";
                this.DoString(sCode);
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
            }
        }
    };
}
