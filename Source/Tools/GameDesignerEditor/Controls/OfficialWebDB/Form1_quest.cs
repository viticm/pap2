using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace OfficialWebDB
{
    public partial class Form1
    {
        private bool Fill_Quests()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_quests", Conn);

            // 填入
            DataRow defaultrow = TblQuests.Rows[0];
            foreach (DataRow row in TblQuests.Rows)
            {
                // 任务等级
                int questlevel = Convert.ToInt32(row["QuestLevel"] == DBNull.Value ? defaultrow["QuestLevel"] : row["QuestLevel"]);
                if (questlevel > 70)
                    continue;

                // 任务id
                int questid = Convert.ToInt32(row["QuestID"]);
                // 任务场景
                string questmap = row["_Cat"].ToString().Trim();
                // 任务名称
                string questname = row["QuestName"].ToString().Trim();
                // 任务需求等级
                int requirelevel = Convert.ToInt32(row["RequiredLevel"] == DBNull.Value ? defaultrow["RequiredLevel"] : row["RequiredLevel"]);
                // 是否可重复接
                string repeat = Convert.ToInt32(row["CanRepeat"] == DBNull.Value ? defaultrow["CanRepeat"] : row["CanRepeat"]) == 0 ? "否" : "是";
                // 任务目标
                string objective = row["Objective"].ToString().Trim();
                objective = objective.Replace("<G>", string.Empty);

                // 接任务
                string startquest = string.Empty;
                if (!IsLogicallyNull(row["StartNpcTemplateID"]))
                {
                    int startnpctid = Convert.ToInt32(row["StartNpcTemplateID"]);
                    startquest = GetNpcNameByID(startnpctid);
                }
                else if (!IsLogicallyNull(row["StartDoodadTemplateID"]))
                {
                    int startdoodadtid = Convert.ToInt32(row["StartDoodadTemplateID"]);
                    startquest = GetDoodadNameByID(startdoodadtid);
                }
                else if (!IsLogicallyNull(row["StartItemType"]) && !IsLogicallyNull(row["StartItemIndex"]))
                {
                    int startitemtype = Convert.ToInt32(row["StartItemType"]);
                    int startitemid = Convert.ToInt32(row["StartItemIndex"]);
                    int count = 0;
                    startquest = GetItemNameByTypeIndex(startitemtype, startitemid, ref count);
                }
                if (startquest.Contains("<非法"))
                    Helper.AddLog(string.Format("任务{0}的StartXXXTemplateID有错误。", questid));

                // 交任务
                string endquest = string.Empty;
                if (!IsLogicallyNull(row["EndNpcTemplateID"]))
                {
                    int Endnpctid = Convert.ToInt32(row["EndNpcTemplateID"]);
                    endquest = GetNpcNameByID(Endnpctid);
                }
                else if (!IsLogicallyNull(row["EndDoodadTemplateID"]))
                {
                    int Enddoodadtid = Convert.ToInt32(row["EndDoodadTemplateID"]);
                    endquest = GetDoodadNameByID(Enddoodadtid);
                }
                else if (!IsLogicallyNull(row["EndItemType"]))
                {
                    int Enditemtype = Convert.ToInt32(row["EndItemType"]);
                    int Enditemid = Convert.ToInt32(row["EndItemIndex"]);
                    int count = 0;
                    endquest = GetItemNameByTypeIndex(Enditemtype, Enditemid, ref count);
                }
                if (endquest.Contains("<非法"))
                    Helper.AddLog(string.Format("任务{0}的EndXXXTemplateID有错误。", questid));

                // 任务类型
                string questtype = GetQuestType(row);
                // 任务需求
                string questrequirement = string.Empty;
                if (questtype == "对话任务")
                {
                    questrequirement = "对话";
                }
                else if (questtype == "杀怪任务")
                {
                    questrequirement = "杀怪：";
                    for (int i = 1; i <= 4; i++)
                    {
                        if (!IsLogicallyNull(row["KillNpcTemplateID" + i.ToString()]) && !IsLogicallyNull(row["KillNpcAmount" + i.ToString()]))
                        {
                            int killnpctid = Convert.ToInt32(row["KillNpcTemplateID" + i.ToString()]);
                            int killnpcamount = Convert.ToInt32(row["KillNpcAmount" + i.ToString()]);
                            questrequirement += GetNpcNameByID(killnpctid) + "*" + killnpcamount.ToString() + " ";
                        }
                    }
                }
                else if (questtype == "采矿任务" || questtype == "送信任务" || questtype == "收集任务")
                {
                    questrequirement = "物品：";
                    for (int i = 1; i <= 4; i++)
                    {
                        if (!IsLogicallyNull(row["EndRequireItemIndex" + i.ToString()]) && !IsLogicallyNull(row["EndRequireItemType" + i.ToString()]) && !IsLogicallyNull(row["EndRequireItemAmount" + i.ToString()]))
                        {
                            int itemtype = Convert.ToInt32(row["EndRequireItemType" + i.ToString()]);
                            int itemindex = Convert.ToInt32(row["EndRequireItemIndex" + i.ToString()]);
                            int itemamount = Convert.ToInt32(row["EndRequireItemAmount" + i.ToString()]);
                            questrequirement += GetItemNameByTypeIndex(itemtype, itemindex, ref itemamount) + "*" + itemamount.ToString() + " ";
                        }
                    }
                }
                if (questrequirement.Contains("<非法"))
                    Helper.AddLog(string.Format("任务{0}的任务需求字段们有错误。", questid));

                // 任务奖励
                string questreward = string.Empty;
                if (!IsLogicallyNull(row["PresentExp"]))
                {
                    questreward += "阅历：" + row["PresentExp"].ToString() + "   ";
                }
                if (!IsLogicallyNull(row["PresentMoney"]))
                {
                    questreward += "金钱：" + MoneyToString(Convert.ToInt32(row["PresentMoney"])) + "   ";
                }
                if (!IsLogicallyNull(row["RewardSkillID"]))
                {
                    int skillid = Convert.ToInt32(row["RewardSkillID"]);
                    questreward += "招式：" + GetSkillNameByID(skillid) + "   ";
                }
                if (!IsLogicallyNull(row["AffectForceIndex1"]) && !IsLogicallyNull(row["AffectValue1"]))
                {
                    int forceindex = Convert.ToInt32(row["AffectForceIndex1"]);
                    int avalue = Convert.ToInt32(row["AffectValue1"]);
                    questreward += "声望：" + GetForceNameByID(forceindex) + "+" + avalue.ToString() + "   ";
                }

                string group1 = string.Empty;
                for (int i = 1; i <= 4; i++)
                {
                    if (!IsLogicallyNull(row["PresentItemType" + i.ToString()]) && !IsLogicallyNull(row["PresentItemIndex" + i.ToString()]) && !IsLogicallyNull(row["PresentItemAmount" + i.ToString()]))
                    {
                        int itemtype = Convert.ToInt32(row["PresentItemType" + i.ToString()]);
                        int itemindex = Convert.ToInt32(row["PresentItemIndex" + i.ToString()]);
                        int itemamount = Convert.ToInt32(row["PresentItemAmount" + i.ToString()]);
                        group1 += GetItemNameByTypeIndex(itemtype, itemindex, ref itemamount) + "*" + itemamount.ToString() + " ";
                    }
                }
                if (group1.Length > 0)
                {
                    if (!IsLogicallyNull(row["PresentAll1"]))
                    {
                        if (row["PresentAll1"].ToString().Trim() == "1")
                        {
                            group1 = "全部 " + group1;
                        }
                    }
                }
                string group2 = string.Empty;
                for (int i = 5; i <= 8; i++)
                {
                    if (!IsLogicallyNull(row["PresentItemType" + i.ToString()]) && !IsLogicallyNull(row["PresentItemIndex" + i.ToString()]) && !IsLogicallyNull(row["PresentItemAmount" + i.ToString()]))
                    {
                        int itemtype = Convert.ToInt32(row["PresentItemType" + i.ToString()]);
                        int itemindex = Convert.ToInt32(row["PresentItemIndex" + i.ToString()]);
                        int itemamount = Convert.ToInt32(row["PresentItemAmount" + i.ToString()]);
                        group2 += GetItemNameByTypeIndex(itemtype, itemindex, ref itemamount) + "*" + itemamount.ToString() + " ";
                    }
                }
                if (group2.Length > 0)
                {
                    if (!IsLogicallyNull(row["PresentAll2"]))
                    {
                        if (row["PresentAll2"].ToString().Trim() == "1")
                        {
                            group2 = "全部 " + group2;
                        }
                    }
                }
                string strItem = (group1.Length > 0 || group2.Length > 0) ? "物品：" + group1 + group2 : string.Empty;
                questreward = questreward + strItem;
                if (questreward.Contains("<非法"))
                    Helper.AddLog(string.Format("任务{0}的任务奖励字段们有错误。", questid));

                // 前置任务
                string foreQuest = string.Empty;
                for (int i = 1; i <= 4; i++)
                {
                    if (!IsLogicallyNull(row["PrequestID" + i.ToString()]))
                    {
                        int qid = Convert.ToInt32(row["PrequestID" + i.ToString()]);
                        foreQuest += GetQuestNameByID(qid) + "  ";
                    }
                }
                if (foreQuest.Length > 0)
                {
                    string sfor = "前置任务：";
                    if (row["PrequestLogic"].ToString() == "0")
                        sfor += "[可选] ";
                    foreQuest = sfor + foreQuest;
                }
                if (foreQuest.Contains("<非法"))
                    Helper.AddLog(string.Format("任务{0}的前置任务有错误。", questid));

                // 副本相关
                string strCopy = string.Empty;
                string diff = row["Difficulty"].ToString().Trim();
                if (diff != string.Empty)
                {
                    strCopy += "(" + row["Class"].ToString().Trim() + ")" + diff;
                }
  

                // 插入一条quest的数据
                string sql = string.Format("insert into web_quests values({0}, '{1}', '{2}', {3}, {4}, '{5}', '{6}', '{7}', '{8}', '{9}', '{10}', '{11}', '{12}', '{13}')",
                    questid, questmap, questname, questlevel, requirelevel,
                    repeat, objective, startquest, endquest,
                    questtype, questrequirement, questreward, foreQuest, strCopy);
                Helper.ExecSqlCmd(sql, Conn);
            }

            return true;
        }


        private string GetQuestType(DataRow row)
        {
            if (IsLogicallyNull(row["StartNpcTemplateID"]) && IsLogicallyNull(row["StartDoodadTemplateID"]) && IsLogicallyNull(row["StartItemType"]) && IsLogicallyNull(row["StartItemIndex"]))
            {
                return "不可接任务";
            }

            for (int i = 1; i < 8; i++)
            {
                if (!IsLogicallyNull(row["QuestValue" + i]) && !IsLogicallyNull(row["QuestValueStr" + i]))
                {
                    return "脚本任务";
                }
            }

            for (int i = 1; i <= 4; i++) // 检查是否是道具任务
            {
                if (!IsLogicallyNull(row["EndRequireItemType" + i]) && !IsLogicallyNull(row["EndRequireItemIndex" + i]) && !IsLogicallyNull(row["EndRequireItemAmount" + i]))
                {
                    if (!IsLogicallyNull(row["DropDoodadTemplateID" + i]))
                    {
                        return "采矿任务";
                    }
                    else
                    {
                        if (IsLogicallyNull(row["DropNpcTemplateID" + i]) && IsItemDeliver(row, i))
                        {
                            return "送信任务";
                        }
                        else
                        {
                            return "收集任务";
                        }
                    }
                }
            }

            for (int i = 1; i <= 4; i++)
            {
                if (!IsLogicallyNull(row["KillNpcTemplateID" + i]) && !IsLogicallyNull(row["KillNpcAmount" + i]))
                {
                    return "杀怪任务";
                }
            }

            for (int i = 1; i <= 4; i++)
            {
                if (IsItemDeliver(row, i))
                {
                    return "送信任务";
                }
            }

            for (int i = 1; i <= 4; i++)
            {
                if (IsLogicallyNull("QuestValue" + i) && IsLogicallyNull("QuestValueStr" + i) &&
                    IsLogicallyNull("EndRequireItemType" + i) && IsLogicallyNull("EndRequireItemIndex" + i) && IsLogicallyNull("EndRequireItemAmount" + i) &&
                    IsLogicallyNull("KillNpcTemplateID" + i) && IsLogicallyNull("KillNpcAmount" + i)
                )
                {
                    return "对话任务";
                }
            }

            return "未知任务类型";
        }

        private static bool IsItemDeliver(DataRow row, int i)
        {
            if (!IsLogicallyNull(row["OfferItemType" + i]) &&
                !IsLogicallyNull(row["OfferItemIndex" + i]) &&
                !IsLogicallyNull(row["OfferItemAmount" + i]) &&
                !IsLogicallyNull(row["EndRequireItemType" + i]) &&
                !IsLogicallyNull(row["EndRequireItemIndex" + i]) &&
                !IsLogicallyNull(row["EndRequireItemAmount" + i]) &&
                IsLogicallyEqual(row, "OfferItemType" + i, "EndRequireItemType" + i) &&
                IsLogicallyEqual(row, "OfferItemIndex" + i, "EndRequireItemIndex" + i) &&
                IsLogicallyEqual(row, "OfferItemAmount" + i, "EndRequireItemAmount" + i)
               )
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
