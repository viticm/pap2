using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.IO;
using System.Collections;

namespace luaEditor
{
    /// <summary>
    /// 事件的参数项，存放参数名和参数类型
    /// ps:也用来保存对象 :D  由于次类名已经在我脑子里根深蒂固了，所以实在舍不得重构类名。
    /// ps:后来，这个东西的用途更多了，根本不是什么参数了。直接就是广义上的各种对象。包括逻辑对象，假对象。
    /// </summary>
    public class LuaEditorParameter
    {
        public LuaEditorParameter(string name, Type type, MethodInfo mi, string strNS)
        {
            this.parm_name = name;
            this.parm_type = type;
            this.mi = mi;
            this.parm_ns = strNS;
        }

        public LuaEditorParameter(string name, string parm_classname, string strNS)
        {
            this.parm_name = name;
            this.parm_type = null;
            this.mi = null;
            this.parm_classname = parm_classname;
            this.parm_ns = strNS;
        }

        public LuaEditorParameter(string name, Type type, string strNS)
        {
            this.parm_name = name;
            this.parm_type = type;
            this.mi = null;
            this.parm_ns = strNS;
        }

        public override string ToString()
        {
            return (string.Format("{0}({1})", parm_name,parm_ns));
        }

        public string parm_name;        //名字
        public Type parm_type;          //类型
        public MethodInfo mi;           //方法信息
        public string parm_ns;          //名称空间
        public string parm_classname;   //专为luanet.import_type使用，存放“VisualEditor.Doit”之类的
    };

    /// <summary>
    /// 事件项，存放事件名、以及参数列表，还有一些说明信息
    /// </summary>
    public class LuaEditorEvent
    {
        public string eventName;
        public List<LuaEditorParameter> parms;
        public string info;

        public LuaEditorEvent(string eventName, List<LuaEditorParameter> parms, string info)
        {
            this.eventName = eventName;
            this.parms = parms;
            this.info = info;
        }

        public LuaEditorEvent(string eventName, List<LuaEditorParameter> parms)
        {
            this.eventName = eventName;
            this.parms = parms;
        }

        public override string ToString()
        {
            string sret = eventName;
            string sparm = "";
            foreach(LuaEditorParameter lp in parms)
            {
                sparm += lp.parm_name + ",";
            }
            if(sparm != "" )
            {
                sparm = "(" + sparm.Substring(0, sparm.Length - 1) + ")";
            }
            sret += sparm;
            return (sret);
        }
    }

    public class leEventConnection
    {
        public List<LuaEditorEvent> m_list = new List<LuaEditorEvent>();
    };

    public static class ParameterCache
    {
        //所有参数的缓存，嗷嗷重要
        public static List<LuaEditorParameter> leParm_Cache = new List<LuaEditorParameter>();

        //所有load进来的asm的缓存集，key: string 类名  value: asm引用
        public static Hashtable asmCache = new Hashtable();

        //构造函数，暂时还没想到要做什么
        static ParameterCache()
        {

        }

        /// <summary>
        /// 搬过来的函数
        /// </summary>
        /// <param name="e"></param>
        /// <param name="funcName"></param>
        /// <param name="strInfo"></param>
        /// <param name="leps"></param>
        /// 
        public static void regEvent(QueryEventsArgs e, string funcName, string strInfo)
        {
            List<LuaEditorParameter> tlist = new List<LuaEditorParameter>();
            e.leec.m_list.Add(new LuaEditorEvent(funcName, tlist, strInfo));
        }
        public static void regEvent(QueryEventsArgs e, string funcName, string strInfo, LuaEditorParameter lep1)
        {
            List<LuaEditorParameter> tlist = new List<LuaEditorParameter>();
            tlist.Add(lep1);
            e.leec.m_list.Add(new LuaEditorEvent(funcName, tlist, strInfo));
        }
        public static void regEvent(QueryEventsArgs e, string funcName, string strInfo, LuaEditorParameter lep1, LuaEditorParameter lep2)
        {
            List<LuaEditorParameter> tlist = new List<LuaEditorParameter>();
            tlist.Add(lep1);
            tlist.Add(lep2);
            e.leec.m_list.Add(new LuaEditorEvent(funcName, tlist, strInfo));
        }
        public static void regEvent(QueryEventsArgs e, string funcName, string strInfo, LuaEditorParameter lep1, LuaEditorParameter lep2, LuaEditorParameter lep3)
        {
            List<LuaEditorParameter> tlist = new List<LuaEditorParameter>();
            tlist.Add(lep1);
            tlist.Add(lep2);
            tlist.Add(lep3);
            e.leec.m_list.Add(new LuaEditorEvent(funcName, tlist, strInfo));
        }
        public static void regEvent(QueryEventsArgs e, string funcName, string strInfo, LuaEditorParameter lep1, LuaEditorParameter lep2, LuaEditorParameter lep3, LuaEditorParameter lep4)
        {
            List<LuaEditorParameter> tlist = new List<LuaEditorParameter>();
            tlist.Add(lep1);
            tlist.Add(lep2);
            tlist.Add(lep3);
            tlist.Add(lep4);
            e.leec.m_list.Add(new LuaEditorEvent(funcName, tlist, strInfo));
        }

        /// <summary>
        /// 获取所有方法，获取的同时缓存获取到的方法。
        /// </summary>
        /// <param name="strFullName">完整名字，例如prop.ID</param>
        /// <returns>方法的字符串列表，如methodname+method   或 funcname+var</returns>
        public static List<string> findMethodInfoByFullName(string strFullName, string strNs)
        {
            List<string> ret = new List<string>();

            //判断中括号情况
            //if(strFullName.Length>2 && strFullName.Substring(strFullName.Length-2) == "[]")
            //{
            //    strFullName = strFullName.Substring(0, strFullName.Length);
            //    haskou = true;
            //}

            for (int i = 0; i < leParm_Cache.Count; i++)
            {
                LuaEditorParameter lp = leParm_Cache[i];
                if (lp.parm_name == strFullName && (lp.parm_ns == strNs || lp.parm_ns == ""))
                {
                    if (lp.parm_type == null)
                    //两种可能：1。函数名。 函数名点了，当然什么也没有。 
                    //          2。luanet.import_type()。 这玩意儿返回值的类型未知
                    {
                        if (lp.parm_classname == "" || lp.parm_classname == null)        //第一种可能应验了
                            return (ret);
                        if (!asmCache.ContainsKey(lp.parm_classname))                   //可是，找不到类名
                            return (ret);
                        Assembly asm = (Assembly)asmCache[lp.parm_classname];
                        lp.parm_type = asm.GetType(lp.parm_classname);
                    }

                    //读取方法列表
                    MethodInfo[] ami = lp.parm_type.GetMethods();
                    foreach (MethodInfo mi in ami)
                    {
                        if (mi.Name.Length > 4)
                        {
                            string head = mi.Name.Substring(0, 4);
                            if (head == "get_" || head == "set_")
                                continue;
                        }

                        //中括号的情况
                        MethodInfo tmi = mi.ReturnType.GetMethod("get_Item", new Type[1] { typeof(int) });
                        if (tmi == null) 
                            tmi = mi.ReturnType.GetMethod("Get", new Type[1] { typeof(int) });
                        if (tmi != null)
                        {
                            if (!hasItem(strFullName + ":" + mi.Name + "()[]", lp.parm_ns))
                                leParm_Cache.Add(new LuaEditorParameter(strFullName + ":" + mi.Name + "()[]", tmi.ReturnType, lp.parm_ns));
                        }

                        ret.Add(mi.Name + "+method");
                        if (!hasItem(strFullName + ":" + mi.Name + "()", lp.parm_ns, mi.ToString()))
                            leParm_Cache.Add(new LuaEditorParameter(strFullName + ":" + mi.Name + "()", mi.ReturnType, mi, lp.parm_ns));
                        if (!hasItem(strFullName + ":" + mi.Name, lp.parm_ns, mi.ToString()))
                            leParm_Cache.Add(new LuaEditorParameter(strFullName + ":" + mi.Name, null, mi, lp.parm_ns));
                    }

                    //读取属性列表
                    PropertyInfo[] api = lp.parm_type.GetProperties();
                    foreach (PropertyInfo pi in api)
                    {

                        //中括号的情况
                        MethodInfo tmi = pi.PropertyType.GetMethod("get_Item", new Type[1] { typeof(int) });
                        if (tmi == null) 
                            tmi = pi.PropertyType.GetMethod("Get", new Type[1] { typeof(int) });
                        if (tmi != null)
                        {
                            if (!hasItem(strFullName + ":" + pi.Name + "[]", lp.parm_ns))
                                leParm_Cache.Add(new LuaEditorParameter(strFullName + "." + pi.Name + "[]", tmi.ReturnType, lp.parm_ns));
                        }

                        ret.Add(pi.Name + "+table");
                        if (!hasItem(strFullName + ":" + pi.Name, lp.parm_ns))
                            leParm_Cache.Add(new LuaEditorParameter(strFullName + "." + pi.Name, pi.PropertyType, lp.parm_ns));
                    }

                    //读取类型列表
                    FieldInfo[] afi = lp.parm_type.GetFields();
                    foreach (FieldInfo fi in afi)
                    {
                        //中括号的情况
                        MethodInfo tmi = fi.FieldType.GetMethod("get_Item", new Type[1] { typeof(int) });
                        if (tmi == null) 
                            tmi = fi.FieldType.GetMethod("Get", new Type[1] { typeof(int) });
                        if (tmi != null)
                        {
                            if (!hasItem(strFullName + ":" + fi.Name + "[]", lp.parm_ns))
                                leParm_Cache.Add(new LuaEditorParameter(strFullName + "." + fi.Name + "[]", tmi.ReturnType, lp.parm_ns));
                        }

                        ret.Add(fi.Name + "+var");
                        if (!hasItem(strFullName + ":" + fi.Name, lp.parm_ns))
                            leParm_Cache.Add(new LuaEditorParameter(strFullName + "." + fi.Name, fi.FieldType, lp.parm_ns));
                    }
                    break; 
                }
            }
            return (ret);
        }

        /// <summary>
        /// 绑定两个对象
        /// 例如： c.value = b.width.tostring()
        /// b.width.tostring()是已知类型，也就是objold
        /// c.value 是未知类型，绑完就成了已知的了 :)
        /// </summary>
        /// <param name="objnew"></param>
        /// <param name="nsnew"></param>
        /// <param name="objold"></param>
        /// <param name="nsold"></param>
        public static void bindTwoObjects(string objnew,string nsnew, string objold, string nsold)
        {
            //旧的没有，就算了
            if(!hasItem(objold, nsold, true))
            {
                return;
            }

            //新的有了，也算了（其实是应该覆盖的，暂时先懒一下）
            if (hasItem(objnew, nsnew, true))
            {
                return;
            }

            //这段也有bug：  a = func,   a. 的时候，出不来func的重载，因为我找到一个就break了。不过还是那句话，先懒一下 
            for (int i = 0; i < leParm_Cache.Count; i++)
            {
                LuaEditorParameter lp = leParm_Cache[i];
                if (lp.parm_name == objold && (lp.parm_ns == nsold || lp.parm_ns == ""))
                {
                    LuaEditorParameter tlp = new LuaEditorParameter(objnew, lp.parm_type, lp.mi, nsnew);
                    tlp.parm_classname = lp.parm_classname;
                    leParm_Cache.Add(tlp);
                    break;
                }
            }
        }


        /// <summary>
        /// 通过反射拿函数的参数，带重载的，用%newline%作为分割标记
        /// </summary>
        /// <param name="strFullName">如txt1.Value.Split</param>
        /// <returns></returns>
        public static string FindMethodParameterByFullName(string strFullName, string strNs)
        {
            string ret = "";
            for (int i = 0; i < leParm_Cache.Count; i++)
            {
                LuaEditorParameter lp = leParm_Cache[i];
                if (lp.parm_name == strFullName && (lp.parm_ns == strNs || lp.parm_ns == "") &&  lp.parm_classname != "")
                {
                    //处理cls()之类的东西的构造函数的参数
                    //预留。。。。。
                }

                if (lp.parm_name == strFullName && lp.mi != null && (lp.parm_ns == strNs || lp.parm_ns == "") && lp.parm_type == null)
                {
                    //读取参数列表
                    ParameterInfo[] api = lp.mi.GetParameters();
                    string p = "";
                    foreach (ParameterInfo pi in api)
                    {
                        p +=  pi.ToString() + ",";
                    }
                    if (p != "") p = p.Substring(0, p.Length - 1);
                    string com = lp.mi.ReturnType.FullName + " " + lp.parm_name + "("  + p + ")";
                    ret += com + "\r\n<%newline%>";
                }
            }
            return (ret);
        }

        /// <summary>
        /// 获取智能感知列表
        /// </summary>
        /// <param name="key">前几个字母</param>
        /// <returns></returns>
        public static List<String> GetIntelliSenceWordList(String key, string strNs)
        {
            List<String> ret = new List<string>();
            foreach (LuaEditorParameter lp in leParm_Cache)
            {
                if (lp.parm_name == "") 
                    continue;
                if(lp.parm_ns == strNs || lp.parm_ns == "")
                {
                    char[] asp = { ':', '(', '.' };
                    string strf = lp.parm_name.Split(asp)[0];
                    string bstrf = "|" + strf.ToLower();
                    if (bstrf.IndexOf("|" + key.ToLower()) != -1)
                    {
                        if (!ret.Contains(strf))
                            ret.Add(strf);
                    }
                }
            }
            List<string> stdlst = StandardLib.getIntelliSenceWordList();
            foreach (string lp in stdlst)
            {
                if (lp == "") continue;
                string bstrf = "|" + lp.ToLower();
                if (bstrf.IndexOf("|" + key.ToLower()) != -1)
                {
                    if (!ret.Contains(lp))
                        ret.Add(lp);
                }
            }
            return (ret);
        }

        /// <summary>
        /// 判断是否缓存中包含某项
        /// </summary>
        /// <param name="leParm_name">项名</param>
        /// <param name="strns">空间</param>
        /// <param name="igNs">传这个参数，表示忽略空命名空间</param>
        /// <returns></returns>
        public static bool hasItem(string leParm_name, string strns, bool igNs)
        {
            foreach (LuaEditorParameter lp in leParm_Cache)
            {
                if (lp.parm_name == leParm_name && (lp.parm_ns == strns || lp.parm_ns == ""))
                    return (true);
            }
            return (false);
        }

        /// <summary>
        /// 判断是否缓存中包含某项
        /// </summary>
        /// <param name="leParm_name">项名</param>
        /// <param name="strns">空间</param>
        /// <returns></returns>
        public static bool hasItem(string leParm_name, string strns)
        {
            foreach (LuaEditorParameter lp in leParm_Cache)
            {
                if (lp.parm_name == leParm_name && lp.parm_ns == strns)
                    return (true);
            }
            return (false);
        }

        /// <summary>
        /// 判断是否缓存中包含某项
        /// </summary>
        /// <param name="leParm_name">项名</param>
        /// <param name="strns">空间</param>
        /// <param name="strmi">类型字串</param>
        /// <returns></returns>
        public static bool hasItem(string leParm_name, string strns, string strmi)
        {
            foreach(LuaEditorParameter lp in leParm_Cache )
            {
                if (lp.parm_name == leParm_name && lp.mi.ToString() == strmi && lp.parm_ns == strns)
                    return (true);
            }
            return (false);
        }

        /// <summary>
        /// 专用于luanet.load_assembly()，加载后保存至asmCache中
        /// </summary>
        /// <param name="dllname">dll名字</param>
        public static void loadDll(string dllname)
        {
            foreach( string fullname in Directory.GetFiles(Directory.GetCurrentDirectory(),dllname) )
            {
                Assembly asm =  Assembly.LoadFrom(fullname);
                foreach(Type t in asm.GetTypes())
                {
                    if(!asmCache.ContainsKey(t.FullName))
                        asmCache.Add(t.FullName, asm);
                }
                break;
            }
        }
    };
}