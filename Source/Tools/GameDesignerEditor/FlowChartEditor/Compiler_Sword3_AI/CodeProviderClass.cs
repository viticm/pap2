using System;
using System.Collections.Generic;
using System.Text;
using AIDiagramPlugin;
using DataInteractor;

namespace Compiler_Sword3_AI
{
    public static class CodeProviderClass
    {                        
        /// <summary>
        /// 获取Action的信息
        /// </summary>
        /// <param name="data">Action的数据</param>
        /// <returns>Action的信息</returns>
        public static string[] GetActionInformation(object data)
        {
            if (data == null || !(data is AI_SingleAction))
            {
                return null;
            }

            AI_SingleAction saction = data as AI_SingleAction;
            AI_Action action = saction.Action;

            //获取参数
            string strRet = "";
            foreach (AI_Parm parm in action.Args)
            {
                strRet += parm.Value.GetCode() + ", ";
            }
            strRet = strRet.TrimEnd((new char[] { ',', ' ' }));

            // 返回值包含3个字符串：
            // 0: Action名
            // 1: 参数字符串，已经用,分隔开了
            // 2: ActionID
            // 3: ActionInfo 
            return new string[] { action.Ename.Trim(), strRet, action.ActionID, action.Info };
        }

        /// <summary>
        /// 分解动作组
        /// </summary>
        /// <param name="data">动作组的数据</param>
        /// <returns>动作数据链表</returns>
        public static List<object> SplitMultiActionIntoSingle(object data)
        {
            if (data == null || !(data is AI_MultiAction))
            {
                return null;
            }

            AI_MultiAction maction = data as AI_MultiAction;
            List<object> list = new List<object>();
            foreach (AI_SingleAction saction in maction.ActionList)
            {
                list.Add(saction);
            }

            return list;
        }

        /// <summary>
        /// 获取状态结点名称
        /// </summary>
        /// <param name="data">状态结点数据</param>
        /// <returns>状态结点名称</returns>
        public static string GetStateString(object data)
        {
            string stateString = null;

            if (data != null)
            {
                AI_State state = data as AI_State;

                if (state != null)
                {
                    stateString = state.Ename.Trim();
                }                
            }
            
            return stateString;
        }

        /// <summary>
        /// 获取事件名称
        /// </summary>
        /// <param name="data">事件连接线数据</param>
        /// <returns>事件名称</returns>
        public static string GetEventString(object data)
        {
            string eventString = null;

            if (data != null)
            {
                AI_Event evt = data as AI_Event;

                if (evt != null)
                {
                    if (int.Parse(evt.eventid) > 1000) // 自定义事件
                    {
                        eventString = evt.eventid;
                    }
                    else // 系统事件
                    {                        
                        eventString = "AI_EVENT." + evt.Ename.Trim();
                    }
                }                
            }            

            return eventString;
        }

        /// <summary>
        /// 获取动作结点返回值个数
        /// </summary>
        /// <param name="data">动作结点数据</param>
        /// <returns>返回值个数</returns>
        public static int GetActionRetCount(object data)
        {
            int count = 0;

            if (data != null)
            {
                AI_SingleAction saction = data as AI_SingleAction;
                
                if (saction != null)
                {
                    AI_Action action = saction.Action;
                    count = action.RetList.Count;
                }                
            }
           
            return count;
        }
        
        /// <summary>
        /// 获取状态结点的事件个数
        /// </summary>
        /// <param name="data">状态结点数据</param>
        /// <returns>事件个数</returns>
        public static int GetStateEvtCount(object data)
        {
            int count = 0;

            if (data != null)
            {
                AI_State state = data as AI_State;

                if (state != null)
                {
                    count = state.EventList.Count;
                }
            }            

            return count;
        }
    }
}
