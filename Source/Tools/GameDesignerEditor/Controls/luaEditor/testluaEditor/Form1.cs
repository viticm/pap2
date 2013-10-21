using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.Collections;

namespace testluaEditor
{
    public partial class Form1 : Form
    {
        LuaEx m_lua = new LuaEx();
        LuaEx m_lua2 = new LuaEx();
        ArrayList m_list = new ArrayList();
        

        public Form1()
        {
            InitializeComponent();
        }

#if false

        //反查事件
        private void luaEditorControl1_queryEvents(object sender, EventArgs e)
        {
            regEvent(e as QueryEventsArgs,
                "baselib",
                "基本库",
                new leParm("pages", typeof(leParm[]),"baselib"),
                new leParm("application", typeof(Application),"baselib"),
                new leParm("form1", typeof(testluaEditor.Form1),"baselib")
                );
        }

        private void regEvent(QueryEventsArgs e, string funcName, string strInfo, params leParm[] leps)
        {
            List<leParm> tlist = new List<leParm>();
            foreach(leParm lp in leps)
            {
                tlist.Add(lp);
            }
            e.leec.m_list.Add(new leEvent(funcName, tlist, strInfo));
        }


        private void luaEditorControl1_Load(object sender, EventArgs e)
        {

        }

        //反查对象
        private void luaEditorControl1_queryObjects(object sender, EventArgs e)
        {
            /*
            e.leTable.Add("len", new luaEditor.leTableItem("len", "function", "string", "string's length"));
            e.leTable.Add("format", new luaEditor.leTableItem("format", "function", "stringFormat,...", "format~~~"));
            e.leTable.Add("left", new luaEditor.leTableItem("left", "table", "string, length", "string's left"));
            e.leTable.Add("sub", new luaEditor.leTableItem("sub", "function", "string, nstart, nlength", "like indexof..."));
            * */
        }

        //反查参数
        private void luaEditorControl1_queryParms(object sender, EventArgs e)
        {
            //根据e.keyword返回参数
            //e.parms_list.Add("void func(hahaha,hehehe,gagaga)");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_uc.Text =

@"function baselib(pages, application, form1)
	
end

";
            //this.Hide();
            //MessageBox.Show("wow");
            //this.Show();
        }
#endif
        private void Form1_Load(object sender, EventArgs e)
        {

            m_lua2.DoString(@"
function fun_in_mlua2(name, sex, age, no) 
    print('xx');
end
function test2() 
    print('xx');
end
");

            m_lua["CodeForm"] = this;
            m_lua["p_lua"] = m_lua2;
            m_lua.RegisterFunction("lt2ArrayList",this, typeof(Form1).GetMethod("lt2ArrayList"));
            m_lua.RegisterFunction("lt2ArrayList_parms", this, typeof(Form1).GetMethod("lt2ArrayList_parms"));
            m_list.Add("a string");

            string strInit = @"
Type = luanet.System.Type
GetType = luanet.System.Type.GetType
Forms = luanet.System.Windows.Forms
tracebox = Forms.MessageBox.Show

luanet.load_assembly('luaEditor.dll')
luaEditor = luanet.luaEditor
leParm = luaEditor.leParm
leEvent = luaEditor.leEvent
leParmsCache = luaEditor.leParmsCache
leTableItem = luaEditor.leTableItem
regEvent = leParmsCache.regEvent

--反查事件
local function handle_queryEvents(sender, e)
    regEvent(e,
           'ongettreevalue',
           '用于自定义树结点的显示内容 调用时机：每次树结点显示出来时，会调用此函数，当此函数返回非空值时，会把这个值作为树结点的显示',
           leParm('prop', GetType('System.String'), 'ongettreevalue'),
           leParm('e', GetType('System.Int32'), 'ongettreevalue')
           )
    
end

--反查对象
local function handle_queryObjects(sender, e)
    local _lt = lt2ArrayList(p_lua, e.keyword)
    local ind
    local v,k
    for ind = 0, _lt.Count - 1 do
        k = _lt[ind][0]
        v = _lt[ind][1]
        if v == 'table' or v == 'function' then
		e.leTable:Add(k, leTableItem(k, v, v, v))   
        end         
    end
end

--反查参数
local function handle_queryParms(sender, e)
	local ret = lt2ArrayList_parms(p_lua, e.keyword)
    if ret ~= nil and ret ~= '' then
        e.parms_list:Add(ret);   
    end     
end

--注册全局变量
local function regGlobalObjects(obj)
    obj:regObject('Conn', GetType('System.String'), nil, '');
    obj:regObject('RootDir', GetType('System.String'), nil, '');
    obj:regObject('StartUpDir', GetType('System.String'), nil, '');
    obj:regObject('Pages', GetType('testluaEditor.Form1.m_list'), nil, '');
    obj:regObject('tclass', GetType('testluaEditor.clsC'), nil, '');
end

ctl_ce = luanet.luaEditor.luaEditorControl()
CodeForm.Controls:Add(ctl_ce)
ctl_ce.queryEvents:Add(handle_queryEvents)
ctl_ce.queryObjects:Add(handle_queryObjects)
ctl_ce.queryParms:Add(handle_queryParms)
regGlobalObjects(ctl_ce)
";
            m_lua.DoString(strInit);
        }

        public ArrayList lt2ArrayList(LuaEx lua, string keyword)
        {
            LuaInterface.LuaTable lt = lua.GetTable(keyword);
            ArrayList ret = new ArrayList();
            if (lt == null) return ret;
            foreach (object k in lt.Keys)
            {
                object[] astr = { k.ToString(), lt[k].ToString() };
                ret.Add(astr);
            }
            return ret;
            //ret.Count
        }

        public string lt2ArrayList_parms(LuaEx lua, string keyword)
        {   
            string ret = keyword;
            try
            {
                LuaInterface.LuaTable lt = (LuaInterface.LuaTable)(lua.DoString("return debug.getinfo(" + keyword + ")")[0]);
                if (lt["what"].ToString() == "Lua")
                {
                    ret += "(";
                    ret += lt["strparms"].ToString();
                    if (lt["has3dot"].ToString() == "1")
                        ret += ",...";
                    ret += ")";
                }
            }
            catch
            {
                ret = "";
            }
            return (ret);
        }
    }

    public class clsA
    {

    };
    public class clsB
    {
        public clsA a1;
        public clsA a2;
        public clsA a3;
    };
    public class clsC
    {
        public clsB ba;
        public clsB bb;
        public clsB bc;
        public clsA aa;
    };
}