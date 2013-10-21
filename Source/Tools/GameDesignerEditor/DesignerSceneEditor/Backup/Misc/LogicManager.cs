using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using LuaInterface;

namespace DesignerSceneEditor
{
	class LogicManager
	{
        private static LogicManager logicManager; // 逻辑管理器对象
        private string newValue; // 新值
        private string rootPath; // 客户端根目录
        Lua lua; // lua虚拟机

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private LogicManager()
        {            
        }

        /// <summary>
        /// 新值
        /// </summary>
        public string NewValue
        {
            get
            {
                return newValue;
            }
        }

        /// <summary>
        /// 客户端根目录
        /// </summary>
        public string RootPath
        {
            set
            {
                rootPath = value;
                if(lua != null)
                {
                    lua["rootPath"] = value;
                }                
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            string luaFile = Path.Combine(Application.StartupPath, "ConfigEdit.lua");
            FileInfo fi = new FileInfo(luaFile);
            if(fi.Exists)
            {
                lua = new Lua();
                lua.DoFile(luaFile);
            }
        }

        /// <summary>
        /// 获取逻辑管理器实例
        /// </summary>
        /// <returns>逻辑管理器实例</returns>
        public static LogicManager GetLogicManager()
        {
            if(logicManager == null)
            {
                logicManager = new LogicManager();
                logicManager.Init();
            }

            return logicManager;
        }

        /// <summary>
        /// 自定义编辑
        /// </summary>
        /// <param name="modelName">模块名</param>
        /// <param name="propertyName">属性名</param>
        /// <param name="oldValue">旧值</param>
        /// <returns>是否编辑成功</returns>
        public bool ConfigEdit(string modelName, string propertyName, string oldValue)
        {
            bool success = false;

            if(lua != null)
            {
                LuaFunction function = lua.GetFunction("ConfigEdit");
                object[] results = function.Call(modelName, propertyName, oldValue);
                if(results != null && results.Length > 1)
                {
                    success = (bool)results[0];
                    if (success)
                    {
                        newValue = results[1].ToString();
                    }                    
                }
            }
            
            return success;
        }

        /// <summary>
        /// 自定义加载
        /// </summary>
        /// <param name="modelName">模块名</param>
        /// <returns>编辑字段链表</returns>
        public List<string> ConfigLoad(string modelName)
        {
            List<string> fieldList = new List<string>();

            if(lua != null)
            {
                LuaFunction function = lua.GetFunction("ConfigLoad");
                object[] results = function.Call(modelName);
                if(results != null && results.Length > 0 && results[0] != null)
                {
                    string[] fields = (results[0] as string).Split(new char[] { ',' });
                    fieldList.AddRange(fields);
                }
            }

            return fieldList;
        }
	}
}
