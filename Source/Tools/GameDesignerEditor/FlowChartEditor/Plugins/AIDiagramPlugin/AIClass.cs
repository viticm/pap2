using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;
using DataInteractor;

namespace AIDiagramPlugin
{
    /// <summary>
    /// AI事件
    /// </summary>
    [Serializable]
    public class AI_Event : IReloadable, ICloneable
	{
        internal string DBID = ""; // 数据库 [AI_Event_Define].id
        public string eventid = ""; // 事件的ID
        public string Name = ""; // 事件的中文名
        public string Ename = ""; // 事件的英文名
        public string Info = ""; // 事件的描述

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload()
        {
            if (!string.IsNullOrEmpty(this.Name))
            {
                AI_Event aiEvent = EventTable.FindItemByDBID(this.DBID);

                if (aiEvent != null)
                {
                    this.Ename = aiEvent.Ename;
                    this.Name = aiEvent.Name;
                    this.Info = aiEvent.Info;
                    this.eventid = aiEvent.eventid;
                }
            }
        }

        /// <summary>
        /// 强制刷新数据
        /// </summary>
        public void ForceReload()
        {
            Reload();
        }

        /// <summary>
        /// 检查是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            return false;
        }

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            if (int.Parse(this.eventid) <= 1000)
            {
                return Name;
            }
            else
            {
                return string.Format("{0} [{1}]", Name, this.eventid);                
            }
        }

        /// <summary>
        /// 比较对象是否相等
        /// </summary>
        /// <param name="obj">比较对象</param>
        /// <returns>是否相等</returns>
        public override bool Equals(object obj)
        {
            AI_Event aiEvent = obj as AI_Event;

            if (aiEvent != null)
            {
                return (this.DBID == aiEvent.DBID);
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 获取哈希码
        /// </summary>
        /// <returns>哈希码</returns>
        public override int GetHashCode()
        {
            return int.Parse(this.DBID);
        }

        /// <summary>
        /// 重载复制
        /// </summary>
        /// <returns>复制的对象</returns>
        public object Clone()
        {
            AI_Event newEvent = new AI_Event();
            newEvent.DBID = this.DBID;
            newEvent.Name = this.Name;
            newEvent.Ename = this.Ename;
            newEvent.Info = this.Info;
            newEvent.eventid = this.eventid;

            return newEvent;
        }
	}

    /// <summary>
    /// AIAction
    /// </summary>
    [Serializable]
	public class AI_Action : ICloneable, IReloadable
	{
		internal string DBID; // 数据库ID
        public string ActionID; // 动作的ID
        public string Name; // 动作的中文名
        public string Ename; // 动作的英文名
        public string Info; // 动作的描述信息
        public string ActionType; // 动作的类型
		internal string rettype; // 返回值代表的含义，用字符串保存，逗号隔开
        public List<AI_Parm> Args = new List<AI_Parm>(); //保存动作输入的参数的值

        // 复制AI动作
        public object Clone()
        {
            AI_Action action = new AI_Action();
            action.DBID = this.DBID;
            action.ActionID = this.ActionID;
            action.Name = this.Name;
            action.Ename = this.Ename;
            action.Info = this.Info;
            action.rettype = this.rettype;
            action.ActionType = this.ActionType;

            foreach (AI_Parm parm in this.Args)
            {
                action.Args.Add(parm.Clone() as AI_Parm);
            }

            return action;
        }

        /// <summary>
        /// 比较对象
        /// </summary>
        /// <param name="obj">对象</param>
        /// <returns>是否相等</returns>
        public override bool Equals(object obj)
        {
            bool result = false;

            AI_Action action = obj as AI_Action;

            if (action != null && this.DBID == action.DBID)
            {
                result = true;
            }

            return result;
        }

        /// <summary>
        /// 生成哈希码
        /// </summary>
        /// <returns>哈希码</returns>
        public override int GetHashCode()
        {
            int hashCode = int.Parse(this.DBID);

            return hashCode;
        }

        /// <summary>
        /// 动作的返回值链表
        /// </summary>
        public List<string> RetList
        {
            get
            {
                string rettype_add = rettype + ",,";
                string[] strRetList = rettype_add.Split(new string[] { ","}, StringSplitOptions.None);
                List<string> list = new List<string>();

                for(int i = 1; i <= 3; i++) // 三个参数
                {
                    if (strRetList[i-1] != "")
                    {
                        list.Add(i.ToString() + ":" + strRetList[i - 1]);
                    }
                }

                return list;
            }
        }

        /// <summary>
        /// 重载字符串输出方法
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            return string.Format("{0} —— {1}", ActionType, Name);
        }

        /// <summary>
        /// 刷新动作数据
        /// </summary>
        public void Reload()
        {
            AI_Action action = ActionTable.FindItemByDBID(this.DBID);

            if (action != null)
            {
                this.ActionID = action.ActionID;
                this.Name = action.Name;
                this.Ename = action.Ename;
                this.Info = action.Info;
                this.ActionType = action.ActionType;
                this.rettype = action.rettype;
            }
        }

        /// <summary>
        /// 强制刷新数据
        /// </summary>
        public void ForceReload()
        {
            AI_Action action = ActionTable.FindItemByDBID(this.DBID);

            if (action != null)
            {
                this.ActionID = action.ActionID;
                this.Name = action.Name;
                this.Ename = action.Ename;
                this.Info = action.Info;
                this.ActionType = action.ActionType;
                this.Args = action.Args;                
                this.rettype = action.rettype;
            }
        }

        /// <summary>
        /// 检查是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            bool result = false;
            AI_Action action = ActionTable.FindItemByDBID(this.DBID);

            if (action != null)
            {                           
                if (this.Args.Count == action.Args.Count)
                {
                    for (int i = 0; i < this.Args.Count; i++)
                    {
                        AI_Parm parameter1 = this.Args[i];
                        AI_Parm parameter2 = action.Args[i];

                        if (parameter1.m_type != parameter2.m_type)
                        {
                            result = true;
                            break;
                        }
                    }
                }
                else
                {
                    result = true;
                }                                                                           
            }
            
            return result;
        }
	}
    
    /// <summary>
    /// AI参数
    /// </summary>
    [Serializable]
    public class AI_Parm : ICloneable
    {
        public string Name; // 参数名
        internal string m_Value; // 参数真正存的值   模板参数的话，这里存模板参数的keyID；数字字符串的话，存值；枚举的话，存是某个枚举的第几个值，0开始。
        internal int m_type; // 记录参数允许存什么。强制规定-2 为 字符串,-1为数字，正数为枚举。
        internal bool m_IsTemplateParm = false; // 是不是存了模板参数
        private bool isVar = false; // 是否是变量类型
        
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="Name">参数名</param>
        /// <param name="mValue">参数值</param>
        /// <param name="mType">参数类型</param>
        /// <param name="istp">是否是模板参数</param>
        public AI_Parm(string Name, string mValue, int mType, bool istp)
        {
            this.Name = Name;
            this.m_Value = mValue;
            this.m_type = mType;
            this.m_IsTemplateParm = istp;
        }

        /// <summary>
        /// 设置参数的值为模板参数
        /// </summary>
        /// <param name="keyID">模板参数ID</param>
        public void SetTemplateValue(string keyID)
        {
            this.m_Value = keyID;
            this.m_IsTemplateParm = true;
        }

        /// <summary>
        /// 是否是变量类型
        /// </summary>
        public bool IsVar
        {
            set
            {
                isVar = value;
            }

            get
            {
                return isVar;
            }
        }

        /// <summary>
        /// 参数值
        /// </summary>
        public IAI_Arg Value
        {
            get
            {
                IAI_Arg arg = null;

                if (m_IsTemplateParm) // 存了模板参数
                {
                    // 在本流程图作用域内取到参数表。
                    string[] infoArray = AI_TemplateParmTable.GetAIParamTemplateTable().AIParamInfoList[int.Parse(m_Value) - 1]; // 旧图的编号是从1开始的
                    arg = new AI_TemplateParmItem(infoArray[0], infoArray[1]);                    
                }
                else // 存的其他东西
                {
                    if (m_type == -1 || m_type == -2)
                    {
                        arg = new AI_ArgValue();
                        (arg as AI_ArgValue).Value = m_Value == null ? "" : m_Value;
                        (arg as AI_ArgValue).type = m_type;
                    }
                    else
                    {
                        EnumItem ei = null;

                        foreach (EnumType et in EnumTable.EnumTypes)
                        {
                            if (et.DBID == this.m_type.ToString())
                            {
                                ei = et.Items[Int32.Parse(m_Value.ToString())];
                            }
                        }

                        arg = ei;
                    }                    
                }

                return arg;
            }
        }

        /// <summary>
        /// 复制参数
        /// </summary>
        /// <returns>参数副本</returns>
        public object Clone()
        {
            AI_Parm newParm = new AI_Parm(this.Name, this.m_Value, this.m_type, this.m_IsTemplateParm);
            newParm.IsVar = isVar;

            return newParm;
        }
    }    

    /// <summary>
    /// 数值/字符串
    /// </summary>
    [Serializable]
	public class AI_ArgValue : IAI_Arg
	{
		internal string Value; // 具体数字、字符串（字符串不+引号）。
        internal int type;

        public string GetCode()
        {
            if (type == -1)
            {
                return Value == null ? "" : Value;
            }
            else
            {
                // 添加到ls表，并返回ID
                string argValue = DBManager.DataBaseManager.GetDataBaseManager().InsertIntoLS(Value);

                return argValue;
            }
        }

        public override string ToString()
        {
            return Value;
        }        
	}

    /// <summary>
    /// 模板参数
    /// </summary>
    [Serializable]
	public class AI_TemplateParmItem : IAI_Arg
	{
        private string name; // 英文名称
        private string chineseName; // 中文名称
        
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="name">英文名称</param>
        /// <param name="chineseName">中文名称</param>
        public AI_TemplateParmItem(string name, string chineseName)
        {
            this.name = name;
            this.chineseName = chineseName;
        }

        /// <summary>
        /// 名称
        /// </summary>
        public string Name
        {
            get
            {
                return name;
            }
        }

        /// <summary>
        /// 中文名称
        /// </summary>
        public string ChineseName
        {
            get
            {
                return chineseName;
            }
        }

        /// <summary>
        /// 获取描述字符串
        /// </summary>
        /// <returns>描述字符串</returns>
        public string GetCode()
        {
            return string.Format("param['{0}']", name);
        }

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            return  string.Format("<{0}>", chineseName);
        }
	}

    /// <summary>
    /// 枚举项，同时也可以是动作的参数
    /// </summary>
    [Serializable]
    public class EnumItem : IAI_Arg, IReloadable
    {
        internal string m_parentDBID; // 枚举类型的ID
        internal int m_asParentsPos; // 自己是枚举类型的第几个参数，从0开始

        [NonSerialized]
        public string Name;

        [NonSerialized]
        public string Ename;

        [NonSerialized]
        public string Parent_Ename;

        internal string Value;  // 存值，但是好像必要不大
        
        /// <summary>
        /// 获取枚举类型
        /// </summary>
        public EnumType Parent
        {
            get 
            { 
                return null; 
            }
        }

        /// <summary>
        /// 获取参数值
        /// </summary>
        /// <returns>参数值</returns>
        public string GetCode()
        {
            return Value;
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload()
        {
            EnumType etype = EnumTable.GetEnumTypeByID(m_parentDBID);
            EnumItem item = etype.Items[m_asParentsPos];
            this.Name = item.Name;
            this.Ename = item.Ename;
            this.Parent_Ename = item.Parent_Ename;
        }

        /// <summary>
        /// 强制刷新数据
        /// </summary>
        public void ForceReload()
        {
            Reload();
        }

        /// <summary>
        /// 检查是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            return false;
        }

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            return Name;
        }
    }

    /// <summary>
    /// 模板参数表
    /// </summary>
    [Serializable]
    public class AI_TemplateParmTable
    {
        private List<string[]> aiParamInfoList = new List<string[]>(); // 模板参数信息链表
        private static AI_TemplateParmTable aiTemplateParamTable;
        private List<AI_TemplateParmItem> aiParamItemList; // AI参数链表

        /// <summary>
        /// 构造函数
        /// </summary>
        public AI_TemplateParmTable()
        {
            // 初始化模板参数信息链表
            aiParamInfoList.Add(new string[] { "IsPositive", "是否主动怪" });
            aiParamInfoList.Add(new string[] { "AlertRange", "警戒范围" });
            aiParamInfoList.Add(new string[] { "AlertOverTime", "警戒状态超时" });
            aiParamInfoList.Add(new string[] { "AttackRange", "战斗范围" });
            aiParamInfoList.Add(new string[] { "EscapeTime", "逃跑时间" });
            aiParamInfoList.Add(new string[] { "EscapeRange", "逃跑范围" });
            aiParamInfoList.Add(new string[] { "EscapeInterval", "逃跑调整间隔" });
            aiParamInfoList.Add(new string[] { "WanderRange", "游荡范围" });
            aiParamInfoList.Add(new string[] { "WanderDistance", "游荡距离" });
            aiParamInfoList.Add(new string[] { "WanderInterval", "游荡周期" });
            aiParamInfoList.Add(new string[] { "PatrolPathID", "巡逻路经ID" });
            aiParamInfoList.Add(new string[] { "EscapeLife", "逃跑血量" });
            aiParamInfoList.Add(new string[] { "PatrolOrderIndex", "巡逻阵型位置" });
            aiParamInfoList.Add(new string[] { "ReviveTeammate", "复活时复活队友" });
            aiParamInfoList.Add(new string[] { "IsAutoTurn", "对话时朝向玩家" });
            aiParamInfoList.Add(new string[] { "IsSnarlInFight", "进入战斗是否吼叫" });
            aiParamInfoList.Add(new string[] { "NpcSceneType", "NPC场景类型" });
        }

        /// <summary>
        /// 获取AI模板参数信息表
        /// </summary>
        /// <returns>AI模板参数信息表</returns>
        public static AI_TemplateParmTable GetAIParamTemplateTable()
        {
            if (aiTemplateParamTable == null)
            {
                aiTemplateParamTable = new AI_TemplateParmTable();
            }

            return aiTemplateParamTable;
        }
        
        /// <summary>
        /// AI参数链表
        /// </summary>
        public List<AI_TemplateParmItem> AIParamItemList
        {
            get
            {
                if (aiParamItemList == null)
                {
                    aiParamItemList = new List<AI_TemplateParmItem>();

                    foreach (string[] infoArray in aiParamInfoList)
                    {
                        AI_TemplateParmItem aiParamItem = new AI_TemplateParmItem(infoArray[0], infoArray[1]);
                        aiParamItemList.Add(aiParamItem);
                    }
                }

                return aiParamItemList;
            }            
        }

        /// <summary>
        /// 模板参数信息链表
        /// </summary>
        public List<string[]> AIParamInfoList
        {
            get
            {
                return aiParamInfoList;
            }
        }

        /// <summary>
        /// 获取AI模板参数的编号
        /// </summary>
        /// <param name="paramName">参数名称</param>
        /// <returns>编号</returns>
        public int GetAIParamIndex(string paramName)
        {
            int index = -1;

            for (int i = 0; i < aiParamInfoList.Count; i++)
            {
                if (aiParamInfoList[i][0] == paramName)
                {
                    index = i + 1; // 兼容旧图，编号从1开始
                    break;
                }
            }

            return index;
        }
    }

    //  以下不参与序列化

    /// <summary>
    /// Action的参数值
    /// </summary>
	public interface IAI_Arg
	{
        string GetCode();
	}

    /// <summary>
    /// 可刷新接口
    /// </summary>
    public interface IReloadable
    {
        void Reload();
        void ForceReload();
        bool RequestEdit();
    }

    /// <summary>
    /// 枚举类型，包含此类型所有的枚举项
    /// </summary>
    public class EnumType
    {
        internal string DBID;
        public string Name;
        public string Ename;
        public string Info;
        internal string[] m_values;
        private List<EnumItem> m_valueList = null;

        public List<EnumItem> Items
        {
            get
            {
                //自动初始化m_valueList再返回
                if (m_valueList == null)
                {
                    m_valueList = new List<EnumItem>();
                    int pos = 0;
                    foreach (string line in this.m_values)
                    {
                        string[] strList = line.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);
                        EnumItem enu = new EnumItem();
                        enu.m_parentDBID = DBID;
                        enu.m_asParentsPos = pos;
                        enu.Name = strList[0].ToString();
                        enu.Ename = strList[1].ToString();
                        enu.Value = strList[2].ToString();
                        enu.Parent_Ename = this.Ename;
                        m_valueList.Add(enu);
                        pos++;
                    }
                }
                return m_valueList;
            }
        }

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            return Name;
        }
    }

    /// <summary>
    /// 枚举列表，包含所有的枚举类型
    /// </summary>
    class EnumTable 
    {
        private static bool inited = false;
        private static string[] parameterSpliter = new string[] { "\r\n" };
        private static List<EnumType> enumTypeList = new List<EnumType>();
        private static Dictionary<string, EnumType> enumTypeDictionary = new Dictionary<string, EnumType>();

        public static List<EnumType> EnumTypes
        {
            get
            {
                Init();
                return enumTypeList;
            }
        }

        private static void Init()
        {
            if (!inited)
            {
                DataTable infoTable = DBManager.DataBaseManager.GetDataBaseManager().GetTable_AI_Enum_Define();

                foreach (DataRow dataRow in infoTable.Rows)
                {
                    EnumType enumType = new EnumType();
                    enumType.DBID = dataRow["id"].ToString();
                    enumType.Name = dataRow["name"].ToString();
                    enumType.Ename = dataRow["ename"].ToString();
                    enumType.Info = dataRow["info"].ToString();
                    enumType.m_values = dataRow["values"].ToString().Split(parameterSpliter, StringSplitOptions.RemoveEmptyEntries);

                    enumTypeList.Add(enumType);
                    enumTypeDictionary[enumType.DBID] = enumType;
                }

                inited = true;
            }
        }

        /// <summary>
        /// 根据数据库ID返回枚举类型
        /// </summary>
        /// <param name="DBID">数据库ID</param>
        /// <returns>枚举类型</returns>
        public static EnumType GetEnumTypeByID(string databaseID)
        {            
            EnumType enumType = null;

            if (!string.IsNullOrEmpty(databaseID))
            {
                Init();

                if (!enumTypeDictionary.TryGetValue(databaseID, out enumType))
                {
                    enumType = null;
                }                
            }

            return enumType;
        }
    }

    //
    /// <summary>
    /// 事件列表
    /// </summary>
    public class EventTable
    {
        private static bool inited = false;
        private static List<AI_Event> eventList = new List<AI_Event>();
        private static Dictionary<string, AI_Event> eventDictionary = new Dictionary<string, AI_Event>();

        private static void Init()
        {
            if (!inited)
            {
                DataTable infoTable = DBManager.DataBaseManager.GetDataBaseManager().GetTable_AI_Event_Define();

                foreach (DataRow dataRow in infoTable.Rows)
                {
                    AI_Event aiEvent = new AI_Event();
                    aiEvent.DBID = dataRow["id"].ToString();
                    aiEvent.Name = dataRow["name"].ToString();
                    aiEvent.Ename = dataRow["ename"].ToString();
                    aiEvent.Info = dataRow["info"].ToString();
                    aiEvent.eventid = dataRow["eventid"].ToString();

                    if (string.IsNullOrEmpty(aiEvent.eventid))
                    {
                        aiEvent.eventid = "0";
                    }

                    eventList.Add(aiEvent);
                    eventDictionary[aiEvent.DBID] = aiEvent;
                }

                eventList.Sort(new AIEventComparer());

                inited = true;
            }
        }

        public static List<AI_Event> EventItems
        {
            get
            {
                Init();                    
                return eventList;
            }
        }

        /// <summary>
        /// 索引事件数据
        /// </summary>
        /// <param name="databaseID">KeyID</param>
        /// <returns>事件数据</returns>
        public static AI_Event FindItemByDBID(string databaseID)
        {
            AI_Event aiEvent = null;

            if (!string.IsNullOrEmpty(databaseID))
            {
                Init();

                if (!eventDictionary.TryGetValue(databaseID, out aiEvent))
                {
                    aiEvent = null;
                }                
            }

            return aiEvent;
        }
    }

    /// <summary>
    /// 动作列表
    /// </summary>
    class ActionTable
    {        
        private static bool inited = false;
        private static string[] parameterSpliter = new string[] { "\r\n" };
        private static char[] parameterInfoSpliter = new char[] { ',' };
        private static List<AI_Action> actionList = new List<AI_Action>();
        private static Dictionary<string, AI_Action> actionDictionary = new Dictionary<string, AI_Action>();       

        /// <summary>
        /// AI动作集合
        /// </summary>
        public static List<AI_Action> ActionItems
        {
            get
            {
                Init();
                return actionList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private static void Init()
        {
            if (!inited)
            {
                DataTable actionInfo = DBManager.DataBaseManager.GetDataBaseManager().GetTable_AI_Action_Define();

                foreach (DataRow dataRow in actionInfo.Rows)
                {
                    AI_Action aiAction = new AI_Action();
                    aiAction.DBID = dataRow["id"].ToString();
                    aiAction.ActionID = dataRow["actionid"].ToString();
                    aiAction.Name = dataRow["name"].ToString();
                    aiAction.Ename = dataRow["ename"].ToString();
                    aiAction.Info = dataRow["info"].ToString();
                    aiAction.ActionType = dataRow["class"].ToString();
                    aiAction.rettype = dataRow["rettype"].ToString();

                    string parms = dataRow["parms"].ToString();

                    foreach (string line in parms.Split(parameterSpliter, StringSplitOptions.RemoveEmptyEntries))
                    {
                        // line: 姓名,-2,"张三"
                        string sline = line + ",";
                        string[] strlist = sline.Split(parameterInfoSpliter);
                        aiAction.Args.Add(new AI_Parm(strlist[0], strlist[2] == "" ? null : strlist[2], int.Parse(strlist[1]), false));
                    }

                    actionList.Add(aiAction);
                    actionDictionary[aiAction.DBID] = aiAction;
                }

                inited = true;
            }
        }

        /// <summary>
        /// 索引动作数据
        /// </summary>
        /// <param name="DBID">KeyID</param>
        /// <returns>动作数据</returns>
        public static AI_Action FindItemByDBID(string databaseID)
        {
            AI_Action aiAction = null;
            
            if (!string.IsNullOrEmpty(databaseID))
            {
                Init();

                if (!actionDictionary.TryGetValue(databaseID, out aiAction))
                {
                    aiAction = null;
                }                
            }

            return aiAction;
        }
    }

}
