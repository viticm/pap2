using System;
using System.Collections.Generic;
using System.Text;
using DataElementLibrary;
using BaseExp;
using System.Diagnostics;
using System.Collections;

namespace ActionNode
{
    [Serializable]
    public class ActionEditor:DataElement
    {
        public override bool EditData(System.Collections.Hashtable table)
        {
            BindDelegate();

            Trace.Assert(table != null);
            Trace.Assert(table.ContainsKey("data"));
            Trace.Assert(table.ContainsKey("event_data"));
            Trace.Assert(table.ContainsKey("prev_data"));
            Trace.Assert(table.ContainsKey("flowchart_name"));
            Trace.Assert(table.ContainsKey("map_name"));

            LuaManager.GetLuaManager().InitOther("flowchart_name", table["flowchart_name"].ToString());
            LuaManager.GetLuaManager().InitOther("map_name", table["map_name"].ToString());
            LuaManager.GetLuaManager().InitOther("client_dir", table["client_dir"].ToString());

            //初始化历史表
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;

            System.Windows.Forms.DialogResult r;
            ActionListForm actionlistform;
            try
            {
                ActionExp[] exps = null;
                if (table["data"] != null)
                {
                    exps = (table["data"] as ActionData).ActionExpArray;                    
                }
                actionlistform = new ActionListForm(exps, table["event_data"] as GameEvent);
                actionlistform.StartPosition =  System.Windows.Forms.FormStartPosition.CenterParent;
                r = actionlistform.ShowDialog();
            }
            catch(LuaInterface.LuaException ex)
            {
                this.printInfo(ex.ToString());
                return false;
            }

            if( r == System.Windows.Forms.DialogResult.OK)
            {
                this.data = new ActionData(actionlistform.actionExpList);
                this.text = actionlistform.actionExpListText;
                this.TooltipText = actionlistform.actionExpListText_Full;
                ScanConst(table);
                return true;
            }
            return false;
        }

        private void ScanConst(Hashtable table)
        {
            //扫描常量至缓存
            if (this.data == null) return;
            ActionExp[] exps = (this.data as ActionData).ActionExpArray;
            foreach (ActionExp exp in exps)
            {
                if (exp != null)
                    ExpManager.ScanConstantIntoHistory(exp);
            }
        }

        public override bool ReloadData(Hashtable table)
        {
#if true    //正常代码
            if (this.data == null) return true;
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;
            ActionExp[] exps = (this.data as ActionData).ActionExpArray;
            if (exps != null)
            {
                foreach (ActionExp exp in exps)
                {
                    ExpManager.ReloadExp(exp);
                }
            }
            ScanConst(table);
            return true;
#else       //兼容代码
            if (this.data == null) return true;
            ActionExp[] exps = this.data as ActionExp[];
            if (exps != null)
            {
                foreach (ActionExp exp in exps)
                {
                    ExpManager.ReloadExp(exp);
                }
            }
            ScanConst(table);
            return true;
#endif
        }

        public override System.Collections.Hashtable GetOutputArgs()
        {
            return null;
        }

        private void BindDelegate()
        {
            //用于lua的Trace
            if (BaseExp.LuaManager.TraceText == null)
                BaseExp.LuaManager.TraceText = new BaseExp.LuaManager.TraceTextHandle(this.printInfo);
        }
    }

    [Serializable]
    public class ActionData : ICloneable
    {
        public ActionExp[] ActionExpArray = null;

        public ActionData(){}

        public ActionData(ActionExp[] aexpList)
        {
            this.ActionExpArray = aexpList;
        }

        public object Clone()
        {
            ActionData ad = new ActionData();
            if (this.ActionExpArray != null)
            {
                ad.ActionExpArray = new ActionExp[this.ActionExpArray.Length];
                for (int i = 0; i < this.ActionExpArray.Length; i++)
                {
                    ad.ActionExpArray[i] = this.ActionExpArray[i].Clone() as ActionExp;
                }
            }
            return ad;
        }
    }
}
