using System;
using System.Collections.Generic;
using System.Text;
using DataElementLibrary;
using System.Diagnostics;
using System.Collections;
using System.Net;
using System.IO;
using System.Windows.Forms;

using DataInteractor;

namespace AIDiagramPlugin
{
    [Serializable]
    public class DataEditor : DataElement 
    {
        [NonSerialized]
        private static StateNodeForm stateForm = new StateNodeForm(); // 状态结点编辑窗体

        // [NonSerialized]
        // private static SingleActionForm singleActionForm = new SingleActionForm(); // 单动作结点编辑窗体

        /// <summary>
        /// 编辑数据
        /// </summary>
        /// <param name="table">数据哈希表</param>
        /// <returns>是否编辑成功</returns>
        public override bool EditData(System.Collections.Hashtable table)
        {           
            switch (this.DataType)
            {
                case "AIStateNode": // 状态结点
                    {                        
                        AI_State state = table["data"] as AI_State;

                        if (state == null) 
                        {
                            state = new AI_State();
                        }

                        // 优化窗体显示
                        stateForm.GraphElementID = id;                        
                        stateForm.Reload(state);
                        stateForm.ShowDialog();

                        if (stateForm.EditSuccess) // 编辑成功
                        {
                            this.Data = state;

                            // 清空连接线
                            List<AI_Event> elist = state.EventList;
                            Hashtable outlineList = (table["next_data"] as Hashtable);
                            foreach (DataElement de in outlineList.Values)
                            {
                                if (!elist.Contains(de.Data as AI_Event))
                                {
                                    de.Data = null;
                                    de.Text = "";
                                    de.TooltipText = "";
                                }
                            }
                            this.Text = state.ToString();
                            this.TooltipText = state.ToFullText();
                        }
                        else
                        {
                            return false;
                        }

                        break;
                    }                    
                case "AIActionNode": // 动作结点
                    {
                        AI_SingleAction action = table["data"] as AI_SingleAction;

                        if (action == null)
                        {   
                            action = new AI_SingleAction();
                        }

                        // 优化窗体显示
                        SingleActionForm singleActionForm = new SingleActionForm();
                        singleActionForm.GraphElementID = id;
                        singleActionForm.InActionGroup = false;
                        singleActionForm.Reload(action);
                        singleActionForm.ShowDialog();

                        if (singleActionForm.EditSuccess) // 编辑成功
                        {
                            this.Data = action;

                            // 清空连接线
                            List<string> eventList = action.Action.RetList;
                            Hashtable outLineList = (table["next_data"] as Hashtable);

                            foreach (DataElement dataElement in outLineList.Values)
                            {
                                if (!eventList.Contains(dataElement.Data as string))
                                {
                                    dataElement.Data = null;
                                    dataElement.Text = "";
                                    dataElement.TooltipText = "";
                                }
                            }

                            this.Text = action.ToString();
                            this.TooltipText = action.ToFullText();
                        }
                        else
                        {
                            return false;
                        }
                        
                        break;
                    }                    
                case "AIActionsNode": // 动作组结点
                    {
                        AI_MultiAction multiAction = table["data"] as AI_MultiAction;

                        if (multiAction == null) 
                        {
                            multiAction = new AI_MultiAction();
                        }

                        MultiActionForm multiActionForm = new MultiActionForm(multiAction);
                        multiActionForm.GraphElementID = id;

                        if (multiActionForm.ShowDialog() == DialogResult.OK)
                        {
                            this.Data = multiAction;

                            //清空连接线
                            Hashtable outlineList = (table["next_data"] as Hashtable);
                            if (multiAction.ActionList.Count == 0)
                            {
                                foreach (DataElement dataElement in outlineList.Values)
                                {
                                    dataElement.Data = null;
                                    dataElement.Text = "";
                                    dataElement.TooltipText = "";
                                }
                            }
                            else
                            {
                                List<string> elist = multiAction.ActionList[multiAction.ActionList.Count - 1].Action.RetList;
                                foreach (DataElement dataElement in outlineList.Values)
                                {
                                    if (!elist.Contains(dataElement.Data as string))
                                    {
                                        dataElement.Data = null;
                                        dataElement.Text = "";
                                        dataElement.TooltipText = "";
                                    }
                                }
                            }
                            this.Text = multiAction.ToString();
                            this.TooltipText = multiAction.ToFullText();
                        }
                        else
                        {
                            return false;
                        }

                        break;
                    }                    
                case "AILine": // 连接线
                    {
                        Hashtable ht_prev_data = table["prev_data"] as Hashtable;
                        Hashtable ht_neightbor_data = table["neighbor_data"] as Hashtable;

                        if (ht_prev_data.Count == 0) 
                        {
                            return false;
                        }

                        object prev_data = null;
                        DataElement previousDataElement = null;

                        foreach (object o in ht_prev_data.Values)
                        {
                            DataElement dataElement = o as DataElement;

                            if (dataElement.Data != null)
                            {
                                previousDataElement = dataElement;
                                prev_data = dataElement.Data;
                                break;
                            }
                        }

                        if (previousDataElement == null || prev_data == null)
                        {
                            return false;
                        }

                        switch (previousDataElement.DataType)
                        {
                            case "AIStateNode": // 状态结点连出的线
                                {
                                    AI_State state = prev_data as AI_State;
                                    List<AI_Event> selectionList = new List<AI_Event>(state.EventList);

                                    foreach (DataElement de in ht_neightbor_data.Values) // 剔除已经选择过了的事件
                                    {
                                        if ((de.Data as AI_Event) != null)
                                        {
                                            selectionList.Remove(de.Data as AI_Event);
                                        }
                                    }

                                    this.data = LineForm<AI_Event>.ShowSelect(table["data"] as AI_Event, selectionList);

                                    if (this.data == null)
                                    {
                                        this.Text = "";
                                    }
                                    else
                                    {
                                        this.Text = this.Data.ToString();
                                    }

                                    break;
                                }
                            case "AIActionNode":
                                {
                                    AI_Action action = (prev_data as AI_SingleAction).Action;

                                    if (action == null)
                                    {
                                        return false;
                                    }

                                    List<string> selectionList = new List<string>(action.RetList);
                                    EditLine(selectionList, ht_neightbor_data);

                                    break;
                                }
                            case "AIActionsNode":
                                {
                                    List<AI_SingleAction> actionList = (prev_data as AI_MultiAction).ActionList;

                                    if (actionList.Count == 0)
                                    {
                                        return false;
                                    }

                                    AI_Action action = actionList[actionList.Count - 1].Action;

                                    if (action == null)
                                    {
                                        return false;
                                    }

                                    List<string> selectionList = new List<string>(action.RetList);
                                    EditLine(selectionList, ht_neightbor_data);
                                     
                                    break;
                                }
                            case "InnerChart":
                                {
                                    Hashtable infoTable = prev_data as Hashtable;
                                    Hashtable outSlotInfo = infoTable["outSlotInfo"] as Hashtable;                                    

                                    List<string> selectionList = new List<string>();

                                    foreach (string id in outSlotInfo.Keys)
                                    {
                                        string interfaceName = outSlotInfo[id] as string;

                                        if (interfaceName == null)
                                        {
                                            interfaceName = "未命名";
                                        }

                                        selectionList.Add(string.Format("{0}:{1}", id, interfaceName));
                                    }

                                    EditLine(selectionList, ht_neightbor_data);

                                    break;
                                }
                            case "InterfaceNode":
                                {
                                    this.data = "1:下一步";
                                    this.text = "";

                                    break;
                                }
                        }

                        break;                    
                    }
            }            

            return true;
        }

        /// <summary>
        /// 编辑连接线
        /// </summary>
        /// <param name="selectionList">选项链表</param>
        /// <param name="neightborInfo">同级连接线信息</param>
        private void EditLine(List<string> selectionList, Hashtable neightborInfo)
        {
            bool singleSelection = true;

            if (selectionList.Count > 1)
            {
                singleSelection = false;
            }

            foreach (DataElement dataElement in neightborInfo.Values) // 剔除已经选择过了的分支
            {
                string selection = dataElement.Data as string;

                if (selection != null)
                {
                    selectionList.Remove(selection);
                }
            }

            this.data = LineForm<string>.ShowSelect(this.data as string, selectionList);

            if (this.data == null || singleSelection)
            {
                this.text = "";
            }
            else
            {                
                string displayText = this.data as string;                
                int index = displayText.IndexOf(":");

                if (index != -1)
                {
                    this.text = displayText.Substring(index + 1);
                }
            }
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        /// <param name="table">公共数据容器</param>
        /// <returns>是否刷新成功</returns>
        public override bool ReloadData(System.Collections.Hashtable table)
        {
            if (this.data != null)
            {
                if (this.data is IReloadable)
                {
                    IReloadable obj = this.Data as IReloadable;
                    string forceReload = table["forceReload"] as string;

                    if (forceReload == null || forceReload == "0") // 检查是否需要强制刷新
                    {
                        obj.Reload();
                        this.Text = obj.ToString();

                        if (obj.RequestEdit()) // 检查是否需要重新编辑
                        {
                            string requestReloadID = table["requestReloadID"] as string;

                            if (string.IsNullOrEmpty(requestReloadID))
                            {
                                requestReloadID = this.id.ToString();
                            }
                            else
                            {
                                requestReloadID += string.Format(",{0}", this.id.ToString());                            
                            }

                            table["requestReloadID"] = requestReloadID;
                        }
                    }
                    else
                    {
                        obj.ForceReload();
                        this.Text = obj.ToString();
                    }  
                }
                else if (dataType == "AILine") // 连接线的显示需要单独刷一下
                {
                    string displayText = this.data as string;
                    int index = displayText.IndexOf(":");

                    if (index != -1)
                    {
                        this.text = displayText.Substring(index + 1);
                    }
                }           
            }

            // 刷新Tip文本
            if (this.data is IFullTextConvertable)
            {
                this.TooltipText = (this.data as IFullTextConvertable).ToFullText();
            }

            return true;
        }

        /// <summary>
        /// 获取数据元信息
        /// </summary>
        /// <returns>数据元信息哈希表</returns>
        public override System.Collections.Hashtable GetOutputArgs()
        {
            Hashtable infoTable = new Hashtable();
            infoTable["DataType"] = this.dataType;

            switch (this.dataType)
            {
                case "AIStateNode": // 状态结点
                    {                        
                        AI_State state = this.data as AI_State;

                        if (state != null)
                        {
                            infoTable["Name"] = state.Name;
                            infoTable["EnglishName"] = state.Ename;                            
                        }                        

                        break;
                    }
                case "AIActionNode": // 动作结点
                    {
                        AI_SingleAction action = this.data as AI_SingleAction;

                        if (action != null && action.Action != null)
                        {
                            infoTable["Name"] = action.Action.Name;                            
                            infoTable["EnglishName"] = action.Action.Ename;
                            
                            if (int.Parse(action.Action.ActionID) > 2000)
                            {
                                infoTable["Script"] = action.Action.Info;
                            }
                        }                        

                        break;
                    }
                case "AIActionsNode": // 动作组结点
                    {
                        AI_MultiAction multiAction = this.data as AI_MultiAction;

                        if (multiAction != null)
                        {
                            List<string> scriptList = new List<string>();
                            
                            foreach (AI_SingleAction singleAction in multiAction.ActionList)
                            {
                                if (singleAction.Action != null && int.Parse(singleAction.Action.ActionID) > 2000)
                                {
                                    if (!string.IsNullOrEmpty(singleAction.Action.Info))
                                    {
                                        scriptList.Add(singleAction.Action.Info);
                                    }
                                }
                            }

                            infoTable["ScriptList"] = scriptList;
                        }                        

                        break;
                    }
                case "AILine": // 连接线
                    {
                        // 无需求，暂时忽略                        

                        break;
                    }
            }

            return infoTable;
        }
    }

    /// <summary>
    /// AI事件比较类
    /// </summary>
    public class AIEventComparer: IComparer<AI_Event>
    {        
        public int Compare(AI_Event x, AI_Event y)
        {
            int result = 0;

            if (x != null && y != null)
            {
                if (x.eventid == "0" && y.eventid == "0")
                {
                    result = x.Name.CompareTo(y.Name);
                }
                else if (x.eventid == "0")
                {
                    result = -1;
                }
                else if (y.eventid == "0")
                {
                    result = 1;
                }
                else
                {
                    result = x.Name.CompareTo(y.Name);
                }
            }
            else if (x == null)
            {
                result = -1;
            }
            else
            {
                result = 1;
            }

            return result;
        }
    }

    /// <summary>
    /// 状态节点
    /// </summary>
    [Serializable]
    public class AI_State : IReloadable, ICloneable, IFullTextConvertable
    {
        public string Name;	// 中文名
        public string Ename; // 英文名
        public List<AI_Event> EventList = new List<AI_Event>();	// 事件id列表

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return Name;
        }

        /// <summary>
        /// 获取全文本字符串
        /// </summary>
        /// <returns>全文文本字符串</returns>
        public string ToFullText()
        {
            StringBuilder tooltipText = new StringBuilder();

            tooltipText.AppendLine("<bold>注册事件");            

            foreach (AI_Event aiEvent in EventList)
            {
                tooltipText.AppendLine(aiEvent.Name);
            }            

            return tooltipText.ToString();
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload()
        {
            foreach (AI_Event aiEvent in EventList)
            {
                aiEvent.Reload();
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
        /// 是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            return false;
        }

        /// <summary>
        /// 拷贝副本
        /// </summary>
        /// <returns>副本</returns>
        public object Clone()
        {
            AI_State state = new AI_State();
            state.Name = this.Name;
            state.Ename = this.Ename;

            foreach (AI_Event aiEvent in EventList)
            {
                state.EventList.Add(aiEvent.Clone() as AI_Event);
            }

            return state;
        }
    }
    
    /// <summary>
    /// 单动作结点
    /// </summary>
    [Serializable]
    public class AI_SingleAction : IReloadable, ICloneable, IFullTextConvertable
    {
        public AI_Action Action;

        /// <summary>
        /// 构造函数
        /// </summary>
        public AI_SingleAction()
        {
            Action = new AI_Action();
        }

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {            
            if (Action == null || Action.Name == null || Action.Ename == null)
            {
                return "";
            }
            else
            {
                return Action.Name;
            }            
        }

        /// <summary>
        /// 获取Tip字符串
        /// </summary>
        /// <returns>Tip字符串</returns>
        public string ToFullText()
        {
            if (Action == null || Action.Name == null || Action.Ename == null)
            {
                return "";
            }

            StringBuilder description = new StringBuilder();
            description.AppendLine("<bold>参数");            

            for (int i = 0; i < Action.Args.Count; i++)
            {
                AI_Parm parm = Action.Args[i];

                description.AppendLine(string.Format("{0}：{1}", parm.Name, parm.Value.ToString()));                
            }

            description.AppendLine("<split>");
            
            string[] returnInfo = Action.rettype.Split(new char[] { ',' });            
            description.AppendLine("<bold>返回值");

            for (int i = 0; i < returnInfo.Length; i++)
            {
                description.AppendLine(string.Format("{0}：{1}", i + 1, returnInfo[i]));
            }

            return description.ToString();
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload()
        {
            Action.Reload();
        }

        /// <summary>
        /// 强制刷新数据
        /// </summary>
        public void ForceReload()
        {
            Action.ForceReload();
        }

        /// <summary>
        /// 检查是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            return Action.RequestEdit();
        }

        /// <summary>
        /// 复制拷贝
        /// </summary>
        /// <returns>拷贝对象</returns>
        public object Clone()
        {
            AI_SingleAction sig = new AI_SingleAction();
            sig.Action = this.Action.Clone() as AI_Action;

            return sig;
        }
    }

    /// <summary>
    /// 多动作结点
    /// </summary>
    [Serializable]
    public class AI_MultiAction : IReloadable, ICloneable, IFullTextConvertable
    {
        public List<AI_SingleAction> ActionList; // 动作序列

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void Reload()
        {
            foreach (AI_SingleAction saction in ActionList)
            {
                saction.Reload();
            }
        }

        /// <summary>
        /// 强制刷新数据
        /// </summary>
        public void ForceReload()
        {
            foreach (AI_SingleAction saction in ActionList)
            {
                saction.ForceReload();
            }
        }

        /// <summary>
        /// 检查是否需要重新编辑
        /// </summary>
        /// <returns>是否需要重新编辑</returns>
        public bool RequestEdit()
        {
            bool result = false;

            foreach (AI_SingleAction saction in ActionList)
            {
                result = saction.RequestEdit();
                if (result)
                {
                    break;
                }
            }

            return result;
        }

        /// <summary>
        /// 复制拷贝
        /// </summary>
        /// <returns>拷贝对象</returns>
        public object Clone()
        {
            AI_MultiAction maction = new AI_MultiAction();
            maction.ActionList = new List<AI_SingleAction>();

            foreach (AI_SingleAction action in this.ActionList)
            {
                maction.ActionList.Add(action.Clone() as AI_SingleAction);
            }

            return maction;
        }        

        /// <summary>
        /// 获取字符串输出
        /// </summary>
        /// <returns>字符串输出</returns>
        public override string ToString()
        {
            if (ActionList == null) 
            {
                return "";
            }

            string strRet = "";

            foreach (AI_SingleAction sigAction in ActionList)
            {
                strRet += string.Format("◆ {0}\r\n", sigAction.ToString());
            }

            return strRet;
        }

        /// <summary>
        ///  获取Tip字符串
        /// </summary>
        /// <returns>Tip字符串</returns>
        public string ToFullText()
        {            
            StringBuilder toolTipText = new StringBuilder();

            if (ActionList != null)
            {
                for (int i = 0; i < ActionList.Count; i++)
                {
                    AI_SingleAction singleAction = ActionList[i];

                    toolTipText.AppendLine(string.Format("<bold>动作{0}\r\n{1}\r\n<split>\r\n{2}\r\n<split>\r\n<split>",
                                                         i + 1, singleAction.Action.Name, singleAction.ToFullText()));
                }

                if (toolTipText.Length > 0)
                {
                    toolTipText.Remove(toolTipText.Length - 20, 20);
                }
            }

            return toolTipText.ToString();
        }
        
    }

    /// <summary>
    /// Tip字符串接口
    /// </summary>
    public interface IFullTextConvertable
    {
        string ToFullText();
    }
}
