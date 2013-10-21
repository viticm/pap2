using System;
using System.Collections.Generic;
using System.Text;
using DataElementLibrary;
using BaseExp;
using System.Diagnostics;
using System.Collections;

namespace ConditionNode
{
    [Serializable]
    public class ConditionEditor:DataElement
    {
        private void BindDelegate()
        {
            //用于lua的Trace
            if (BaseExp.LuaManager.TraceText == null)
                BaseExp.LuaManager.TraceText = new BaseExp.LuaManager.TraceTextHandle(this.printInfo);
        }

        public override bool EditData(System.Collections.Hashtable table)
        {
            BindDelegate();
            Trace.Assert(table != null);
            Trace.Assert(table.ContainsKey("data"));
            Trace.Assert(table.ContainsKey("event_data"));
            Trace.Assert(table.ContainsKey("prev_data"));
            Trace.Assert(table.ContainsKey("next_data"));
            Trace.Assert(table.ContainsKey("flowchart_name"));
            Trace.Assert(table.ContainsKey("map_name"));

            LuaManager.GetLuaManager().InitOther("flowchart_name", table["flowchart_name"].ToString());
            LuaManager.GetLuaManager().InitOther("map_name", table["map_name"].ToString());
            LuaManager.GetLuaManager().InitOther("client_dir", table["client_dir"].ToString());

            //初始化历史表
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;

            Exp exp;
            List<Exp> elist;
            if(table["data"] == null)
            {
                exp = ExpManager.CreateUnknownExp();
                elist = new List<Exp>();
            }
            else
            {
                ConditionData cd = table["data"] as ConditionData;
                object[] olist = cd.datalist;
                exp = olist[0] as Exp;
                elist = olist[1] as List<Exp>;
            }

            ConditionForm expform = new ConditionForm(exp, table["event_data"] as GameEvent, elist);
            //conForm.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            //ExpForm expform = new ExpForm(exp, table["event_data"] as GameEvent, true, null);
            expform.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            if (expform.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {

                //清空连接线
                Hashtable outlineList = (table["next_data"] as Hashtable);
                foreach (DataElement de in outlineList.Values)
                {
                    if (!elist.Contains(de.Data as Exp))
                    {
                        de.Data = null;
                        de.Text = "";
                        de.TooltipText = "";
                    }
                }

                ConditionData cd = new ConditionData();
                cd.datalist = new object[]{ expform.ResultExp, expform.LineExpList};
                this.data = cd;
                this.text = expform.ResultExp.ToString();
                if (this.text.Length > 15) this.text = this.text.Substring(0, 13) + "...";
                if (expform.ResultExp is ActionExp)
                    this.TooltipText = (expform.ResultExp as ActionExp).strFullText;
                ScanConst(table);
                return true;
            }
            return false;
        }

        private void ScanConst(Hashtable table)
        {
            ConditionData cd = this.data as ConditionData;
            if (cd != null)
            {
                object[] olist = cd.datalist;
                if (olist != null)
                {
                    Exp exp = olist[0] as Exp;
                    if (exp != null)
                        ExpManager.ScanConstantIntoHistory(exp);
                }
            }

        }

        public override bool ReloadData(Hashtable table)
        {
            ConditionData cd = this.data as ConditionData;
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;
            if (cd!= null)
            {
                object[] olist = cd.datalist;
                if (olist != null)
                {
                    Exp exp = olist[0] as Exp;
                    List<Exp> elist = olist[1] as List<Exp>;
                    ExpManager.ReloadExp(exp);
                    foreach (Exp ex in elist)
                    {
                        ExpManager.ReloadExp(ex);
                    }
                }
            }
            ScanConst(table);

            return true;
        }

        public override System.Collections.Hashtable GetOutputArgs()
        {
            return null;
        }
    }

    [Serializable]
    public class ConditionData : ICloneable
    {
        public object[] datalist;
        public object Clone()
        {
            ConditionData cd = new ConditionData();
            if(datalist != null)
            {
                Exp exp = datalist[0] as Exp;
                List<Exp> elist = datalist[1] as List<Exp>;
                List<Exp> elist_new = new List<Exp>();
                foreach (Exp ex in elist)
                {
                    elist_new.Add(ex.Clone() as Exp);
                }
                cd.datalist = new object[] { exp.Clone(), elist_new };
            }
            return cd;
        }
    }
}
