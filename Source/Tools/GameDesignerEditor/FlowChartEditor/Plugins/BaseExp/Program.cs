using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data;
using LuaInterface;
using System.Collections;
using DataInteractor;

namespace BaseExp
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            //定义事件
            GameEvent evt = new GameEvent(2);

            //创建一个空的表达式
            Exp exp = ExpManager.CreateNewExp(TypeDefine.NilType);

            //手工创建一个完整的表达式
            ActionExp expa = new ActionExp();
            expa.ReturnType = exp.ReturnType;
            expa.strText = "设置 使用者 等于 true";
            expa.API = new GameAPI(1);
            expa.API.ArgValues[0] = evt.GetArgExp(1);
            expa.API.ArgValues[1] = new ConstExp("true",TypeDefine.BoolType);

            Application.Run(new ExpForm(expa, evt, true, expa.ReturnType, null));
        }
    }

    public interface IReloadable
    {
        void Reload();
    }

    public static class ExpManager
    {
        //获取事件列表
        public static GameEvent[] GetGameEventList()
        {
            int[] event_id_list = DBManager.DataBaseManager.GetDataBaseManager().GetEvents();
            if (event_id_list == null) return null;
            GameEvent[] ret = new GameEvent[event_id_list.Length];
            for (int i = 0; i < ret.Length; i++)
            {
                ret[i] = new GameEvent(event_id_list[i]);
            }
            return ret;
        }


        //根据返回值获取API列表
        public static GameAPIDefine[] GetAPIDefineByReturnType(TypeDefine type)
        {
            int[] api_id_list = DBManager.DataBaseManager.GetDataBaseManager().GetAPIsByRetType(type.DBID);
            if (api_id_list == null) return null;
            GameAPIDefine[] ret = new GameAPIDefine[api_id_list.Length];
            for(int i = 0; i < ret.Length; i++)
            {
                ret[i] = new GameAPIDefine(api_id_list[i]);
            }
            return ret;
        }

        //根据返回值获取API列表，不是某返回值的API
        public static GameAPIDefine[] GetAPIDefineListExceptReturnType(TypeDefine type)
        {
            int[] api_id_list = DBManager.DataBaseManager.GetDataBaseManager().GetAPIs_Except_RetType(type.DBID);
            if (api_id_list == null) return null;
            GameAPIDefine[] ret = new GameAPIDefine[api_id_list.Length];
            for(int i = 0; i < ret.Length; i++)
            {
                ret[i] = new GameAPIDefine(api_id_list[i]);
            }
            return ret;
        }

        //获取所有API列表(获取所有API，无论有没有返回值，此函数自从动作加了过滤，还没给谁调过)
        public static GameAPIDefine[] GetAPIDefine_All()
        {
            int[] api_id_list = DBManager.DataBaseManager.GetDataBaseManager().GetAllAPIs();
            if (api_id_list == null) return null;
            GameAPIDefine[] ret = new GameAPIDefine[api_id_list.Length];
            for(int i = 0; i < ret.Length; i++)
            {
                ret[i] = new GameAPIDefine(api_id_list[i]);
            }
            return ret;
        }


        //根据返回值取常数列表
        public static ConstExp[] GetConstExpByReturnType(TypeDefine type)
        {
            string[] value_list = DBManager.DataBaseManager.GetDataBaseManager().GetConstDBValuesByRetType(type.DBID);
            if (value_list == null) return null;
            ConstExp[] ret = new ConstExp[value_list.Length];
            for (int i = 0; i < ret.Length; i++)
            {
                ret[i] = new ConstExp(value_list[i], type);
            }
            return ret;
        }

        //创建一个空动作表达式
        public static ActionExp CreateNewActionExp(GameAPIDefine apidefine)
        {
            ActionExp expAction = new ActionExp();
            expAction.API = new GameAPI(apidefine.DBID);
            expAction.ReturnType = expAction.API.ReturnType;
            return expAction;
        }

        //创建一个空表达式
        public static Exp CreateNewExp(TypeDefine returnType)
        {
            Exp e = new Exp();
            e.ReturnType = returnType;
            return e;
        }

        //创建一个未知返回值的表达式
        public static Exp CreateUnknownExp()
        {
            Exp e = new Exp();
            e.ReturnType = null;
            return e;
        }

        //扫描引用列表，根据常数类型返回常数列表
        public static ConstExp[] ScanConstantFromHistory(TypeDefine type)
        {
            Hashtable ht_history = CacheManager.GetCacheManager().Global_Args_Table;
            if (ht_history == null)
                return new ConstExp[] { };
            if(ht_history.ContainsKey(type.DBID))
            {
                Hashtable ht = ht_history[type.DBID] as Hashtable;
                ConstExp[] exps = new ConstExp[ht.Count];
                ht.Values.CopyTo(exps, 0);
                return exps;
            }
            else
            {
                return new ConstExp[] { };
            }
        }

        //扫描表达式的常量，保存到本图引用列表
        public static void ScanConstantIntoHistory(Exp exp)
        {
            //保存结构：
            // ht_history = { type_id : ht }
            // ht = { dbvalue : const_exp }
            Hashtable ht_history = CacheManager.GetCacheManager().Global_Args_Table;
            if (exp is ConstExp)
            {
                ConstExp cexp = exp as ConstExp;
                if(!ht_history.ContainsKey(cexp.ReturnType.DBID))
                {
                    Hashtable ht = new Hashtable();
                    ht.Add(cexp.DBValue, cexp);
                    ht_history.Add(cexp.ReturnType.DBID, ht);
                }
                else
                {
                    Hashtable ht = ht_history[cexp.ReturnType.DBID] as Hashtable;
                    if(!ht.ContainsKey(cexp.DBValue))
                    {
                        ht.Add(cexp.DBValue, cexp);
                    }
                }
            }
            if (exp is ObjectExp)
            {
                //do nothing
            }
            if (exp is ActionExp)
            {
                ActionExp aexp = exp as ActionExp;
                foreach (Exp ex in aexp.API.ArgValues)
                {
                    ScanConstantIntoHistory(ex);
                }
            }
        }
        

        //从数据库中重读Exp的未序列化信息
        public static void ReloadExp(Exp exp)
        {
            if(exp is ConstExp)
            {
                ConstExp cexp = exp as ConstExp;
                cexp.Reload();
                cexp.ReturnType.Reload();
            }
            if(exp is ObjectExp)
            {
                ObjectExp oexp = exp as ObjectExp;
                oexp.ReturnType.Reload();
                oexp.Parent.Reload();
            }
            if(exp is ActionExp)
            {
                ActionExp aexp = exp as ActionExp;
                aexp.API.Reload();
                aexp.ReturnType.Reload();
                foreach(Exp ex in aexp.API.ArgValues)
                {
                    ReloadExp(ex);
                }
            }
        }
    };

    //大类
    [Serializable]  /*只有ID序列化*/
    public class TypeDefine : IReloadable
    {
        public int DBID = 0;               //数据库ID
        [NonSerialized]
        private string m_strText = "";      //屏显
        [NonSerialized]
        public bool isDuplicate = false; //是否存值
        [NonSerialized]
        public bool isEnum = false;      //是否为枚举类型
        [NonSerialized]
        public bool genAsString = false; // 是否作为字符串值类型
        [NonSerialized]
        public string QueryScript = "";   //用于查屏显的脚本

        public bool ContainsValueEdit
        {
            get
            {
                if (this.DBID == 0) this.Reload();
                return (QueryScript.Contains("\nfunction OnValueEdit(")
                    || QueryScript.StartsWith("function OnValueEdit("));
            }
        }
          

        public static TypeDefine BoolType
        {
            get { return new TypeDefine(FixedValueProvider.TYPE_BOOL_ID); }
        }

        public static TypeDefine NilType
        {
            get { return new TypeDefine(FixedValueProvider.TYPE_NIL_ID); }
        }

        public TypeDefine(int nDBID)
        {
            this.DBID = nDBID;
            this.Reload();
        }
        public override string ToString()
        {
            return m_strText;
        }
        public void Reload()
        {
            if (this.DBID == 0) return;
            TypeDefine cache = CacheManager.GetCacheManager().GetCache("TypeDefine", this.DBID.ToString()) as TypeDefine;
            if(cache == null)
            {
                object[] objs = DBManager.DataBaseManager.GetDataBaseManager().GetTypeDefine(this.DBID);
                if (objs == null) return;
                this.m_strText = objs[0].ToString();
                this.isDuplicate = (bool)objs[1];
                this.isEnum = (bool)objs[2];
                this.QueryScript = objs[3].ToString();
                this.genAsString = !(objs[4] is DBNull) && (bool)objs[4];
                if (this.QueryScript == null) this.QueryScript = "";
                CacheManager.GetCacheManager().SaveCache("TypeDefine", this.DBID.ToString(), this);
            }
            else
            {
                this.m_strText = cache.m_strText;
                this.isDuplicate = cache.isDuplicate;
                this.isEnum = cache.isEnum;
                this.QueryScript = cache.QueryScript;
                this.genAsString = cache.genAsString;
            }
        }
        
    };

    public class ArgsDefine
    {
        public ArgsDefine(string name, TypeDefine type, string DefaultValue)
        {
            this.ArgName = name; 
            this.ArgType = type;
            this.DefaultValue = DefaultValue;
        }
        public string ArgName = "";
        public TypeDefine ArgType = null;
        public string DefaultValue = "";
        public override string ToString()
        {
            return this.ArgName;
        }
    };


    [Serializable]
    public class EventDefine : IReloadable
    {
        public int DBID = 0;
        [NonSerialized]
        private string m_strText = "";         //笼统描述
        [NonSerialized]
        public string combText = "";           //屏显
        [NonSerialized]
        public int mainClassIndex;             //绑定用主分类，在参数中的位置
        [NonSerialized]
        public ArgsDefine[] ArgList = null;    //参数列表


        public override string ToString()
        {
            return m_strText;
        }

        public EventDefine(){Reload();}

        public EventDefine(int nDBID)
        {
            this.DBID = nDBID;
            this.Reload();
        }

        public virtual void Reload()
        {
            if (this.DBID == 0) return;
            //读取事件信息
            object[] objs = DBManager.DataBaseManager.GetDataBaseManager().GetEventDefine(this.DBID);
            if (objs == null) return;
            this.m_strText = objs[0].ToString();
            this.combText = objs[1].ToString();
            this.mainClassIndex = Int32.Parse(objs[2].ToString());         
            //读取事件参数定义
            DataTable dt = DBManager.DataBaseManager.GetDataBaseManager().GetEventDefineArg(this.DBID);
            if (dt == null)
            {
                this.ArgList = new ArgsDefine[0];  return;
            }
            this.ArgList = new ArgsDefine[dt.Rows.Count];
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                this.ArgList[i] = new ArgsDefine(dt.Rows[i]["name"].ToString(), 
                       new TypeDefine(Convert.ToInt32(dt.Rows[i]["type"].ToString())),
                       dt.Rows[i]["default"].ToString());
            }
        }        
    };


    //事件
    [Serializable]
    public class GameEvent : EventDefine, ICloneable, IComparable
    {
        //扩展XML信息
        public string tag = "";
        //事件API的参数值存这里
        private EventAPI m_eventAPI = null;

        public GameEvent():base()
        {
            this.m_eventAPI = GetEventAPI();
        }

        public GameEvent(int DBID):base(DBID)
        {
            this.m_eventAPI = GetEventAPI();
        }
       
        //根据事件，获取参数表达式
        public ObjectExp GetArgExp(int index_BeginWith1)
        {
            int index = index_BeginWith1 - 1;
            if (index > ArgList.Length - 1) throw new Exception("要获取的的参数表达式，超出了事件的定义范围");
            ObjectExp oexp = new ObjectExp(this, index,ArgList[index].ArgType);
            return oexp;
        }

        public override void Reload()
        {
            base.Reload();
            if (m_eventAPI == null)
                m_eventAPI = GetEventAPI();
            else
            {
                m_eventAPI.Parent = this;
                m_eventAPI.combText = this.combText;
                m_eventAPI.ArgList = this.ArgList;
                if(m_eventAPI.ArgValues.Count != m_eventAPI.ArgList.Length)
                {
                    //重读后发现事件参数个数变化
                    if(m_eventAPI.ArgValues.Count < m_eventAPI.ArgList.Length)
                    {
                        for (int i = 0; i < m_eventAPI.ArgList.Length - m_eventAPI.ArgValues.Count; i++)
                            m_eventAPI.ArgValues.Add(null);
                        throw new Exception("事件" + this.ToString() + "的参数个数变多了。");
                    }
                    else
                    {
                        throw new Exception("事件" + this.ToString() + "的参数个数变少。原流程图末尾的参数将被删除。");
                    }
                }
                foreach(Exp exp in m_eventAPI.ArgValues)
                {
                    if (exp != null) ExpManager.ReloadExp(exp);
                }
            }
        }
        public EventAPI GetEventAPI()
        {
            if (this.m_eventAPI != null) return this.m_eventAPI;
            EventAPI api = new EventAPI();
            api.DBID = this.DBID;
            api.Parent = this;
            api.combText = this.combText;
            api.ArgList = this.ArgList;
            api.ArgValues = new List<Exp>();
            for (int i = 0; i < api.ArgList.Length; i++)
            {
                api.ArgValues.Add(null);
            }
            return api;
        }

        public object Clone()
        {
            GameEvent gevent = new GameEvent(this.DBID);
            gevent.m_eventAPI = this.m_eventAPI.Clone() as EventAPI;
            return gevent;
        }

        public int CompareTo(object obj)
        {
            if (obj is GameEvent)
            {
                GameEvent ge = obj as GameEvent;
                if(this.m_eventAPI == null || ge.m_eventAPI == null)
                {
                    throw new Exception("事件比较前请先Reload初始化");
                }
                return this.m_eventAPI.DBID.CompareTo(ge.m_eventAPI.DBID);
            }
            else
            {
                throw new Exception("事件不能和其他类型比较");
            }
        }
    };

    //表达式
    [Serializable]
    public class Exp: ICloneable
    {
        //中文文本
        public string strText = null;
        public TypeDefine ReturnType = null;
        public virtual object Clone() 
        {
            Exp e = new Exp();
            e.ReturnType = this.ReturnType;
            return e;
        }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            if (!(obj is Exp)) return false;
            Exp exp = obj as Exp;
            if (this.strText != exp.strText) return false;
            return this.ReturnType.DBID == exp.ReturnType.DBID;
        }

        public override string ToString()
        {
            return "空表达式";
        }
    };

    //常数表达式
    [Serializable]
    public class ConstExp : Exp, IReloadable
    {
        //常数的值。（常数的类型在其类）
        public string DBValue = "";

        private ConstExp() {    /*私有构造函数，克隆用*/        }

        public ConstExp(string DBValue, TypeDefine typeDefine)
        {
            this.ReturnType = typeDefine;
            this.DBValue = DBValue;
            this.Reload();
        }
        public void Reload()
        {
            if (this.strText != null) return;
            if (this.ReturnType.isDuplicate)
            {
                this.strText = DBValue;
                return;
            }
            ConstExp cache = CacheManager.GetCacheManager().GetCache("ConstExp." + this.ReturnType.DBID.ToString(), this.DBValue) as ConstExp;
            if (cache == null)
            {
                if (this.ReturnType.QueryScript == "")
                    this.strText = DBManager.DataBaseManager.GetDataBaseManager().GetConstText(this.ReturnType.DBID, this.DBValue);
                else
                    this.strText = LuaManager.GetLuaManager().GetConstName(this.ReturnType.QueryScript, this.ReturnType.DBID.ToString(), this.DBValue);
                CacheManager.GetCacheManager().SaveCache("ConstExp." + this.ReturnType.DBID.ToString(), this.DBValue, this);
            }
            else
            {
                this.strText = cache.strText;
            }
        }
        public override string ToString()
        {
            return this.strText;
        }

        public override object Clone()
        {
            ConstExp exp = new ConstExp();
            exp.DBValue = this.DBValue;
            exp.ReturnType = this.ReturnType;
            exp.strText = this.strText;
            return exp;
        }

        public override bool Equals(object obj)
        {
            if (!base.Equals(obj)) return false;
            if (obj == null) return false;
            if (!(obj is ConstExp)) return false;
            ConstExp exp = obj as ConstExp;
            return this.DBValue == exp.DBValue;
        }
    };

    //对象表达式
    [Serializable]
    public class ObjectExp : Exp,IComparable
    {
        public GameEvent Parent = null;
        public int AsEventArgPos = 0;
        public ObjectExp(GameEvent parent, int pos, TypeDefine retuenType)
        {
            this.Parent = parent;
            this.AsEventArgPos = pos;
            this.ReturnType = retuenType;
            this.strText = this.Parent.ArgList[AsEventArgPos].ArgName;
        }
        public override string ToString()
        {
            return strText;
        }
        public override object Clone()
        {
            return new ObjectExp(this.Parent, this.AsEventArgPos, this.ReturnType);
        }
        public int CompareTo(object obj)
        {
            return this.ToString().CompareTo(obj.ToString());
        }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            if (!(obj is ObjectExp)) return false;
            ObjectExp exp = obj as ObjectExp;
            if (this.AsEventArgPos != exp.AsEventArgPos) return false;
            return this.Parent.DBID.Equals(exp.Parent.DBID);
        }
    };

    //动作表达式
    [Serializable]
    public class ActionExp:Exp
    {
        public string strFullText = "";
        //API
        public GameAPI API = null;
        public override string ToString()
        {
            return strText;
        }
        public override object Clone()
        {
            ActionExp exp = new ActionExp();
            exp.API = (GameAPI)this.API.Clone();
            exp.ReturnType = this.ReturnType;
            exp.strText = this.strText;
            exp.strFullText = this.strFullText;
            return exp;
        }

        public override bool Equals(object obj)
        {
            if (!base.Equals(obj)) return false;
            if (obj == null) return false;
            if (!(obj is ActionExp)) return false;
            ActionExp exp = obj as ActionExp;
            if (this.strFullText != exp.strFullText) return false;
            if (this.API == null && exp.API == null)
                return true;
            else if (this.API == null || exp.API == null) 
                return false;
            return this.API.Equals(exp.API);
        }
    };

    //变量表达式
    [Serializable]
    public class VarExp: Exp, ICloneable
    {
        public override string ToString()
        {
            return "判断结果";
        }
        public override object Clone()
        {
            VarExp e = new VarExp();
            e.ReturnType = this.ReturnType;
            e.strText = this.strText;
            return e;
        }
        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            if (!(obj is VarExp)) return false;
            VarExp exp = obj as VarExp;
            if (this.strText != exp.strText) return false;
            //if (this.ReturnType == null || exp.ReturnType == null) return true;
            return this.ReturnType.DBID == exp.ReturnType.DBID;
        }
    }


    //API定义
    [Serializable] /*只有ID序列化*/
    public class GameAPIDefine : IReloadable,IComparable
    {
        public int DBID = 0;
        [NonSerialized]
        public TypeDefine ReturnType = null;
        [NonSerialized]
        public string strText = "";  //屏显
        [NonSerialized]
        public string strName = "";   //生成用，函数英文名   --原本已废弃的，但没舍得删，说不定以后重构成其它的(果然后来又启用了)
        [NonSerialized]
        public string combText = ""; //描述文本  
        [NonSerialized]
        public ArgsDefine[] ArgList = null;    //参数列表
        [NonSerialized]
        public bool isAsyn = false;
        [NonSerialized]
        public string strTip = "";      //API帮助提示 

        protected GameAPIDefine() {  }

        public GameAPIDefine(int id)
        {
            this.DBID = id;
            this.Reload();
        }

        public override string ToString()
        {
            return this.strText;
        }

        public virtual void Reload()
        {
            if (this.DBID == 0) return;
            //读取API信息
            object[] objs = DBManager.DataBaseManager.GetDataBaseManager().GetAPIDefine(this.DBID);
            if (objs == null) return;
            this.strText = objs[0].ToString();
            this.combText = objs[1].ToString();
            this.ReturnType = new TypeDefine(Convert.ToInt32(objs[2].ToString()));
            this.strName = objs[3].ToString();
            this.isAsyn = (bool)objs[4];
            this.strTip = objs[5].ToString();

            //读取API参数定义
            DataTable dt = DBManager.DataBaseManager.GetDataBaseManager().GetAPIDefineArg(this.DBID);
            if (dt == null) 
            {
                this.ArgList = new ArgsDefine[0]; return; 
            }
            this.ArgList = new ArgsDefine[dt.Rows.Count];
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                this.ArgList[i] = new ArgsDefine(dt.Rows[i]["name"].ToString(),
                       new TypeDefine(Convert.ToInt32(dt.Rows[i]["type"].ToString())),
                       dt.Rows[i]["default"].ToString());
            }
        }

        public int CompareTo(object obj)
        {
            return this.ToString().CompareTo(obj.ToString());
        }
    };


    //动作实体
    [Serializable]
    public class GameAPI : GameAPIDefine,ICloneable
    {
        //API的参数的值
        public List<Exp> ArgValues = new List<Exp>();
        protected GameAPI(){}
        public GameAPI(int DBID) :base(DBID)
        {
            for(int i = 0 ;i < this.ArgList.Length; i++)
            {
                ArgValues.Add(null);
            }
        }
        public object Clone()
        {
            GameAPI api = new GameAPI();
            api.DBID = this.DBID;
            api.ReturnType = this.ReturnType;
            api.strText = this.strText;
            api.isAsyn = this.isAsyn;
            api.strTip = this.strTip;
            api.strName = this.strName;
            api.combText = this.combText;
            api.ArgList = this.ArgList;
            for (int i = 0; i < this.ArgList.Length; i++)
            {
                if (i < this.ArgValues.Count && this.ArgValues[i] != null)
                    api.ArgValues.Add(this.ArgValues[i].Clone() as Exp);
                else
                    api.ArgValues.Add(null as Exp);
            }
            return api;
        }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            if (!(obj is GameAPI)) return false;
            GameAPI api = obj as GameAPI;
            if (this.ArgValues.Count != api.ArgValues.Count) return false;
            for (int i = 0; i < this.ArgValues.Count; i++)
            {
                if (this.ArgValues[i] == null && api.ArgValues[i] == null) continue;
                if (this.ArgValues[i] == null || api.ArgValues[i] == null) return false;
                if (!this.ArgValues[i].Equals(api.ArgValues[i])) return false;
            }
            return this.DBID.Equals(api.DBID);
        }
    };

    //缓存类
    public class CacheManager
    {
        private static CacheManager manager = null;  // LuaManager的实例
        private Hashtable m_Data = new Hashtable();  //用于缓存
        public Hashtable Global_Args_Table = null;

        //私有构造函数，不允许在外部初始化
        private CacheManager() { }

        //获取LuaManager的实例
        public static CacheManager GetCacheManager()
        {
            if (manager == null) // 对象未初始化
            {
                manager = new CacheManager();
            }
            return manager;
        }

        //清除所有缓存
        public void  ClearAllCache()
        {
            this.m_Data = new Hashtable();
        }

        //保存进缓存
        public void SaveCache(string strClassName, string strKey, object data)
        {
            string strCombo = strClassName + "::" + strKey;
            if (!m_Data.ContainsKey(strCombo))
                m_Data.Add(strCombo, data);
            else
                m_Data[strCombo] = data;
        }

        //从缓存读出
        public object GetCache(string strClassName, string strKey)
        {
            string strCombo = strClassName + "::" + strKey;
            if (!m_Data.ContainsKey(strCombo))
                return null;
            else
                return m_Data[strCombo];
        }

        
    };

    //Lua相关的类
    public class LuaManager
    {
        private static LuaManager manager = null;  // LuaManager的实例
        private Lua m_lua = null;
        //private Hashtable m_other = new Hashtable();

        public delegate void TraceTextHandle(string strText);
        public static TraceTextHandle TraceText = null;

        //私有构造函数，不允许在外部初始化
        private LuaManager() { }

        //获取LuaManager的实例
        public static LuaManager GetLuaManager()
        {
            if (manager == null) // 对象未初始化
            {
                manager = new LuaManager();
                manager.m_lua = new Lua();
                manager.m_lua["DBM"] = DBManager.DataBaseManager.GetDataBaseManager();
                manager.m_lua["DataInter"] = Interactor.GetInteractor();
                manager.m_lua.RegisterFunction("trace", manager, typeof(LuaManager).GetMethod("TraceForLua"));
            }
            return manager;
        }

        //给lua虚拟机注册其他信息
        public void InitOther(string Name , string value)
        {
            this.m_lua[Name] = value;
        }

        public void TraceForLua(string s)
        {
            if (TraceText != null)
            {
                TraceText(s);
            }
        }

        public string GetConstName(string strLua, string typedefine, string dbvalue)
        {
            this.m_lua.DoString("GetConstName = nil; OnValueEdit = nil;");
            this.m_lua.DoString(strLua);    // function GetConstName(typed, dbvalue) ... end
            LuaFunction fun1 = this.m_lua.GetFunction("GetConstName");
            if (fun1 == null)
                return dbvalue;
            object[] objRet = fun1.Call(new object[] { typedefine, dbvalue });
            if(objRet.Length == 1 && objRet[0]!=null)
            {   
                return objRet[0].ToString();
            }
            else
            {
                throw new Exception("lua 函数返回值异常");
            }
        }

        public object[] OnValueEdit(string strLua, string typedefine, string dbvalue)
        {
            this.m_lua.DoString("GetConstName = nil; OnValueEdit = nil;");
            this.m_lua.DoString(strLua);    // function GetConstName(typed, dbvalue) ... end
            LuaFunction fun1 = this.m_lua.GetFunction("OnValueEdit");
            if (fun1 == null)
                return new object[0];
            object[] objRet = fun1.Call(new object[] { typedefine, dbvalue });
            return objRet;
        }

    };

    [Serializable]
    public class EventAPI : GameAPI, ICloneable, IReloadable
    {
        public GameEvent Parent = null;
        public EventAPI(){}

        public void Reload()
        {

        }

        public new object Clone()
        {
            EventAPI api = new EventAPI();
            api.DBID = this.DBID;
            api.ReturnType = this.ReturnType;
            api.strText = this.strText;
            api.combText = this.combText;
            api.ArgList = this.ArgList;
            api.Parent = this.Parent;
            if (this.ArgList != null)
            {
                for (int i = 0; i < this.ArgList.Length; i++)
                {
                    if (i >= this.ArgValues.Count || this.ArgValues[i] == null)
                        api.ArgValues.Add(null);
                    else
                        api.ArgValues.Add(this.ArgValues[i].Clone() as Exp);
                }
            }
            return api;
        }

    }


    //保存写死的常数们
    public static class FixedValueProvider
    {
        public const int TYPE_NIL_ID        = 1;            //写死的 动作     的ID
        public const int TYPE_BOOL_ID       = 2;            //写死的 布尔大类 的ID
        public const int TYPE_STRING_ID     = 4;            //写死的 字符串   的ID
        public const int TYPE_PATROL_ID     = 28;           //写死的 路径     的ID


        public const int COMPARE_API_ID     = 58;           //写死的 比较API  在数据库中ID
        public const int OPENWINDOW_ID      = 49;           //写死的 窗口API  在数据库中ID
    }
}