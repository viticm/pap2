using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using BaseExp;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using DataElementLibrary;
using ConditionNode;
using ActionNode;

namespace FlowChartLogicManager
{
    public static class LogicCheck
    {
        // 检查事件互斥
        public static bool CheckEventExclusion(List<DataElement> eventList)
        {
            List<GameEvent> elist = new List<GameEvent>();
            foreach (DataElement de in eventList)
            {
                if (de != null && de.Data is GameEvent)
                {
                    elist.Add(de.Data as GameEvent);
                }
                else
                {
                    MessageBox.Show("无效连接！", "错误",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
            }
            if (elist.Count == 0) return true;
            if (elist.Count == 1) return true;
            GameEvent getmp = elist[0];
            for (int i = 1; i < elist.Count; i++)
            {
                if (getmp.CompareTo(elist[i]) != 0)
                {
                    MessageBox.Show("不同事件结点不能连接在同一个动作结点上！", "错误",
                       MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
            }
            return true;
        }

        //根据结点数据获取 常数列表   nodeList: DataElement数组
        public static ArrayList GetConstCollectionFromNode(ArrayList nodeList)
        {
            ArrayList al_ret = new ArrayList();
            if (nodeList == null || nodeList.Count == 0) return al_ret;
            foreach (DataElement node in nodeList)
            {
                if (node.Data is ConditionData)
                {
                    object[] datalist = (node.Data as ConditionData).datalist;
                    Exp exp = datalist[0] as Exp;
                    List<Exp> elist = datalist[1] as List<Exp>;
                    al_ret.AddRange(GetConstCollectionFromExp(exp));
                    foreach (Exp expi in elist)
                    {
                        ArrayList al = GetConstCollectionFromExp(expi);
                        foreach (Exp expadd in al)
                        {
                            if (!al_ret.Contains(expadd)) al_ret.Add(expadd);
                        }
                    }
                }
                else if (node.Data is ActionData)
                {
                    ActionExp[] ActionExpArray = (node.Data as ActionData).ActionExpArray;
                    foreach (ActionExp expi in ActionExpArray)
                    {
                        ArrayList al = GetConstCollectionFromExp(expi);
                        foreach (Exp expadd in al)
                        {
                            if (!al_ret.Contains(expadd)) al_ret.Add(expadd);
                        }
                    }
                }
            }
            return al_ret;
        }

        public static ArrayList GetConstCollectionFromExpArray(ArrayList explist)
        {
            ArrayList al_ret = new ArrayList();
            if (explist == null) return al_ret;
            foreach (Exp exp in explist)
            {
                ArrayList al = GetConstCollectionFromExp(exp);
                foreach (Exp expadd in al)
                {
                    if (!al_ret.Contains(expadd)) al_ret.Add(expadd);
                }
            }
            return al_ret;
        }


        //根据表达式获取 常数列表
        public static ArrayList GetConstCollectionFromExp(Exp exp)
        {
            ArrayList al_ret = new ArrayList();
            if (exp is ActionExp)
            {
                ActionExp aexp = exp as ActionExp;
                if (aexp != null)
                {
                    foreach (Exp e in aexp.API.ArgValues)
                    {
                        ArrayList al = GetConstCollectionFromExp(e);
                        foreach (Exp expadd in al)
                        {
                            if (!al_ret.Contains(expadd)) al_ret.Add(expadd);
                        }
                    }
                }
            }
            else if (exp is ConstExp)
            {
                ConstExp cexp = exp as ConstExp;
                if (!al_ret.Contains(cexp)) al_ret.Add(cexp);
            }
            else if (exp is ObjectExp)
            {
            }
            else if (exp is VarExp)
            {
            }
            else
            {
            }

            return al_ret;
        }
    }
}
