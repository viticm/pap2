using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;
using System.Data.SqlClient;

namespace CustomLoadTree
{
    class Logic
    {
        public enum QuestType
        {
            QUEST_NULL = 0,
            QUEST_SCRIPT,
            QUEST_ITEM_MINE,
            QUEST_ITEM_COLLECT,
            QUEST_KILL,
            QUEST_ITEM_DELIVER,
            QUEST_DIALOG,
            QUEST_TIMER,
            QUEST_UNKNOWN
        };

        public static Hashtable ht_qt;
        public static DataTable tbl;

        public static DataTable QuestTable
        {
            get
            {
                return tbl;
            }
            set
            {
                tbl = value;
            }
        }

        public static string[] tableColumns =
                            {
                                "QuestId",
                                "_Cat",
                                "QuestName",
                                "QuestLevel",
                                "PrequestLogic",
                                "PrequestID1",
                                "PrequestID2",
                                "PrequestID3",
                                "PrequestID4",
                                "_AreaQuest",
                                "SubsequenceID",

                                "StartNpcTemplateID",
                                "StartDoodadTemplateID",
                                "StartItemType",
                                "StartItemIndex",
                                
                                "QuestValue1",
                                "QuestValueStr1",
                                "QuestValue2",
                                "QuestValueStr2",
                                "QuestValue3",
                                "QuestValueStr3",
                                "QuestValue4",
                                "QuestValueStr4",
                                "QuestValue5",
                                "QuestValueStr5",
                                "QuestValue6",
                                "QuestValueStr6",
                                "QuestValue7",
                                "QuestValueStr7",
                                "QuestValue8",
                                "QuestValueStr8",

                                "EndRequireItemType1",
                                "EndRequireItemIndex1",
                                "EndRequireItemAmount1",
                                "DropDoodadTemplateID1",
                                "DropNpcTemplateID1",

                                "EndRequireItemType2", 
                                "EndRequireItemIndex2",
                                "EndRequireItemAmount2",
                                "DropDoodadTemplateID2",
                                "DropNpcTemplateID2",

                                "EndRequireItemType3",
                                "EndRequireItemIndex3",
                                "EndRequireItemAmount3",
                                "DropDoodadTemplateID3",
                                "DropNpcTemplateID3",

                                "EndRequireItemType4",
                                "EndRequireItemIndex4",
                                "EndRequireItemAmount4",
                                "DropDoodadTemplateID4",
                                "DropNpcTemplateID4",

                                "KillNpcTemplateID1",
                                "KillNpcAmount1",
                                "KillNpcTemplateID2",
                                "KillNpcAmount2",
                                "KillNpcTemplateID3",
                                "KillNpcAmount3",
                                "KillNpcTemplateID4",
                                "KillNpcAmount4",

                                "OfferItemType1",
                                "OfferItemIndex1",
                                "OfferItemAmount1",
                                "EndRequireItemType1",
                                "EndRequireItemIndex1",
                                "EndRequireItemAmount1",

                                "OfferItemType2",
                                "OfferItemIndex2",
                                "OfferItemAmount2",
                                "EndRequireItemType2",
                                "EndRequireItemIndex2",
                                "EndRequireItemAmount2",
                                "OfferItemType3",
                                "OfferItemIndex3",
                                "OfferItemAmount3",
                                "EndRequireItemType3",
                                "EndRequireItemIndex3",
                                "EndRequireItemAmount3",
                                "OfferItemType4",
                                "OfferItemIndex4",
                                "OfferItemAmount4",
                                "EndRequireItemType4",
                                "EndRequireItemIndex4",
                                "EndRequireItemAmount4"
                            };

     
        public static bool ReInit(string sql, SqlConnection conn)
        {
            bool bReturn = true;

            if (ht_qt != null)
            {
                ht_qt.Clear();
            }
            if (tbl != null)
            {
                tbl.Clear();
            }

            ht_qt = new Hashtable();
            try
            {

                tbl = Helper.GetDataTable(sql, conn);
            }
            catch
            {
                bReturn = false;
            }

            return bReturn;
        }
        private static DataRow row;

        public static int GetQuestImageIndex(int quest_id)
        {
            if (ht_qt[quest_id] == null)
            {
                QuestType qt = GetQuestType(quest_id);
                ht_qt[quest_id] = Convert.ToInt32(qt);
            }
            return Convert.ToInt32(ht_qt[quest_id]);
        }

        private static bool IsItemDeliver(int i)
        {
            if (!IsNull("OfferItemType" + i) &&
                !IsNull("OfferItemIndex" + i) &&
                !IsNull("OfferItemAmount" + i) &&
                !IsNull("EndRequireItemType" + i) &&
                !IsNull("EndRequireItemIndex" + i) &&
                !IsNull("EndRequireItemAmount" + i) &&
                Equal("OfferItemType" + i, "EndRequireItemType" + i) &&
                Equal("OfferItemIndex" + i, "EndRequireItemIndex" + i) &&
                Equal("OfferItemAmount" + i, "EndRequireItemAmount" + i)
               )
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public static QuestType GetQuestType(int quest_id)
        {
            object[] keys = new object[1];
            keys[0] = quest_id;
            row = tbl.Rows.Find(keys);


            if (IsNull("StartNpcTemplateID") && IsNull("StartDoodadTemplateID") && IsNull("StartItemType") && IsNull("StartItemIndex")) // 检查是否是不可接任务
            {
                return QuestType.QUEST_NULL;
            }

            for (int i = 1; i < 8; i++) // 检查是否是脚本任务
            {
                if (!IsNull("QuestValue" + i) && !IsNull("QuestValueStr" + i))
                {
                    return QuestType.QUEST_SCRIPT;
                }
            }

            for (int i = 1; i <= 4; i++) // 检查是否是道具任务
            {
                if (!IsNull("EndRequireItemType" + i) && !IsNull("EndRequireItemIndex" + i) && !IsNull("EndRequireItemAmount" + i))
                {
                    if (!IsNull("DropDoodadTemplateID" + i))
                    {
                        return QuestType.QUEST_ITEM_MINE;
                    }
                    else
                    {
                        if (IsNull("DropNpcTemplateID" + i) && IsItemDeliver(i))
                        {
                            return QuestType.QUEST_ITEM_DELIVER;
                        }
                        else
                        {
                            return QuestType.QUEST_ITEM_COLLECT;
                        }
                    }
                }
            }

            for (int i = 1; i <= 4; i++) // 检查是否是杀怪任务
            {
                if (!IsNull("KillNpcTemplateID" + i) && !IsNull("KillNpcAmount" + i))
                {
                    return QuestType.QUEST_KILL;
                }
            }

            for (int i = 1; i <= 4; i++) // 检查是否是送信任务
            {
                if (IsItemDeliver(i))
                {
                    return QuestType.QUEST_ITEM_DELIVER;
                }
            }

            for (int i = 1; i <= 4; i++) // 检查是否是对话任务
            {
                if (IsNull("QuestValue" + i) && IsNull("QuestValueStr" + i) &&
                    IsNull("EndRequireItemType" + i) && IsNull("EndRequireItemIndex" + i) && IsNull("EndRequireItemAmount" + i) &&
                    IsNull("KillNpcTemplateID" + i) && IsNull("KillNpcAmount" + i)
                )
                {
                    return QuestType.QUEST_DIALOG;
                }
            }

            return QuestType.QUEST_UNKNOWN;
        }

        private static bool IsNull(string field)
        {
            if (row == null || row[field] == DBNull.Value || row[field].ToString().Trim() == string.Empty
                || row[field].ToString().Trim() == "0") // 可以认为是空的数据
                return true;
            else
                return false;
        }

        private static bool Equal(string field1, string field2)
        {
            if (row[field1].ToString().Trim() == row[field2].ToString().Trim())
                return true;
            else
                return false;
        }
    }
}
