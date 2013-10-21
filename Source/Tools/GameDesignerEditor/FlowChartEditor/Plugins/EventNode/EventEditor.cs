using System;
using System.Collections.Generic;
using System.Text;
using DataElementLibrary;
using BaseExp;
using System.Diagnostics;
using System.Collections;
using DataInteractor;

namespace EventNode
{
    [Serializable]
    public class EventEditor:DataElement
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
           
            EventListForm eventForm = new EventListForm();
            eventForm.selectedEvent = table["data"] as GameEvent;
            eventForm.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            if (eventForm.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (eventForm.selectedEvent == null) return false;
                this.data = eventForm.selectedEvent;
                this.text = eventForm.ToString();
                ScanConst(table);
                return true;
            }
            else
            {
                return false;
            }
        }

        private void ScanConst(Hashtable table)
        {
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;
            GameEvent gameevent = this.data as GameEvent;
            foreach (Exp exp in gameevent.GetEventAPI().ArgValues)
            {
                if (exp != null)
                    ExpManager.ScanConstantIntoHistory(exp);
            }
        }

        public override bool ReloadData(Hashtable table)
        {
            GameEvent gameevent = this.data as GameEvent;
            CacheManager.GetCacheManager().Global_Args_Table = table["globe_args"] as Hashtable;
            if(gameevent != null)
            {
                gameevent.Reload();
            }

            ScanConst(table);
            return true;
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
}
