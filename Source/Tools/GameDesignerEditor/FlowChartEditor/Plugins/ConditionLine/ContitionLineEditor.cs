using System;
using System.Collections.Generic;
using System.Text;
using DataElementLibrary;
using BaseExp;
using System.Diagnostics;
using System.Collections;
using ConditionNode;

namespace ConditionLine
{
    [Serializable]
    public class ContitionLineEditor:DataElement
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
            Trace.Assert(table.ContainsKey("prev_data"));
            Trace.Assert(table.ContainsKey("flowchart_name"));
            Trace.Assert(table.ContainsKey("map_name"));

            LuaManager.GetLuaManager().InitOther("flowchart_name", table["flowchart_name"].ToString());
            LuaManager.GetLuaManager().InitOther("map_name", table["map_name"].ToString());
            LuaManager.GetLuaManager().InitOther("client_dir", table["client_dir"].ToString());

            //初始化历史表
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;

            Hashtable ht_prev_data = table["prev_data"] as Hashtable;
            if (ht_prev_data.Count == 0) return false;
            List<Exp> prev_List = null;
            foreach (object o in ht_prev_data.Values)
            {
                if ((o as DataElement).Data != null)
                {
                    prev_List = (((o as DataElement).Data as ConditionData).datalist as object[])[1] as List<Exp>;
                }
            }
            if (prev_List == null) return false;

            Exp exp = table["data"] as Exp;

            SelectionForm expform = new SelectionForm(exp, prev_List);
            expform.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            if (expform.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.data = expform.ResultExp;
                this.text = expform.ResultExp.ToString();
                this.TooltipText = this.Text;
                if (this.text.Length > 15) this.text = this.text.Substring(0, 13) + "...";
                ScanConst(table);
                return true;
            }
            return false;;
        }


        private void ScanConst(Hashtable table)
        {
            Exp exp = this.data as Exp;
            if (exp != null)
                ExpManager.ScanConstantIntoHistory(exp);
        }

        public override bool ReloadData(Hashtable table)
        {
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;
            Exp exp = this.data as Exp;
            if (exp != null)
            {
                ExpManager.ReloadExp(exp);
            }

            ScanConst(table);
            return true;
        }

        public override System.Collections.Hashtable GetOutputArgs()
        {
            return null;
        }
    }
}
