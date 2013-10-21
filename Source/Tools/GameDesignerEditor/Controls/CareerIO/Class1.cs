using System;
using System.Data;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using DataTableRemotingClient;
using RHelper = DataTableRemotingClient.TableHelper;

namespace CareerIO
{
    public class Class1
    {
        public Class1()
        {

        }

        public void ExportFile(string strClient, SqlConnection Conn)
        {
            // CareerEvent.txt
            string sql1 = "select [Level],[Name],[Title],[Introduction],[MapID1],[MapID2],[MapID3],[MapID4],[MapID5],[Tab0],[Tab1],[Tab2],[Tab3],[Tab4],[Tab5],[Tab6],[Tab7] from tbl_Guide_CareerEvent where [level]>0 or ([level]=0 and [tabid]=0) order by [level]";
            DataTable t1 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_CareerEvent", sql1, Conn);
            string content1 = TableToString(t1);
            string out1 = string.Format("{0}/ui/scheme/case/cyclopaedia/CareerEvent.txt", strClient);
            FileFolderHelper.StringToFile(content1, out1);

            // CareerTab.txt
            string sql2 = "select [TabID],[Name],[Title],[Image1],[Note1],[Image2],[Note2],[Image3],[Note3],[Description] from tbl_Guide_CareerEvent where [TabID]>=0 order by [tabid]";
            DataTable t2 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_CareerEvent", sql2, Conn);
            string content2 = TableToString(t2);
            string out2 = string.Format("{0}/ui/scheme/case/cyclopaedia/CareerTab.txt", strClient);
            FileFolderHelper.StringToFile(content2, out2);
        }

        public void ExportJx3Library(string strClient, SqlConnection Conn)
        {
            // JX3Library.txt
            string sql1 = "select [ClassID],[SubClassID],[ID],[ClassName],[SubClassName],[Title],[Content],[Link] from tbl_Guide_JX3Library order by [ClassID],[SubClassID],[ID]";
            DataTable t1 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_JX3Library", sql1, Conn);
            foreach (DataRow row in t1.Rows)
            {
                string content = row["Content"].ToString();
                int nIndexP = 0;
                int nIndexE = 0;

                nIndexP = content.IndexOf("\"");
                if (nIndexP == -1)
                    continue;
                nIndexE = content.IndexOf("\"", nIndexP + 1);

                string newContext = content.Substring(nIndexP + 1, nIndexE - nIndexP - 1).TrimEnd(new char[] { ' ', '\n' });
                newContext = newContext.Replace(" ", "  ");

                row["Content"] = content.Substring(0, nIndexP + 1) + newContext + content.Substring(nIndexE, content.Length - nIndexE);
            }
            string content1 = TableToString(t1);
            int FirstReturn = content1.IndexOf("\r\n");
            string stringBefore = content1.Substring(0, FirstReturn + 2);
            string stringAfter = content1.Substring(FirstReturn + 2);
            string stringMiddle = "0\t0\t0\t\t\t\t\t\r\n";
            content1 = stringBefore + stringMiddle + stringAfter;
            string out1 = string.Format("{0}/ui/scheme/case/cyclopaedia/JX3Library.txt", strClient);
            FileFolderHelper.StringToFile(content1, out1);
        }

        public void ExportActivityDailyQuest(string strClient, SqlConnection Conn)
        {
            //ActivityInfo.txt
            string sql1 = "select [ClassID],[ActivityID],[ClassName],[Title],[Content],[Link] from tbl_Guide_ActivityInfo where [ClassID] > -1 and [ActivityID] > -1 order by [ClassID],[ActivityID]";
            DataTable dt1 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_ActivityInfo", sql1, Conn);
            string content1 = TableToString(dt1);
            int FirstReturn1 = content1.IndexOf("\r\n");
            string stringBefore1 = content1.Substring(0, FirstReturn1 + 2);
            string stringAfter1 = content1.Substring(FirstReturn1 + 2);
            string stringMiddle1 = "0\t0\t\t\t\t\r\n";
            content1 = stringBefore1 + stringMiddle1 + stringAfter1;
            string out1 = string.Format("{0}/ui/scheme/case/cyclopaedia/ActivityInfo.txt", strClient);
            FileFolderHelper.StringToFile(content1, out1);

            //DailyQuestInfo.txt
            string sql2 = "select [TypeID],[QuestID],[TypeName],[Content],[Link] from tbl_Guide_DailyQuestInfo where [TypeID] > -1 and [QuestID] > -1 order by [TypeID],[QuestID]";
            DataTable dt2 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_DailyQuestInfo", sql2, Conn);
            string content2 = TableToString(dt2);
            int FirstReturn2 = content2.IndexOf("\r\n");
            string stringBefore2 = content2.Substring(0, FirstReturn2 + 2);
            string stringAfter2 = content2.Substring(FirstReturn2 + 2);
            string stringMiddle2 = "0\t0\t\t\t\r\n";
            content2 = stringBefore2 + stringMiddle2 + stringAfter2;
            string out2 = string.Format("{0}/ui/scheme/case/cyclopaedia/DailyQuestInfo.txt", strClient);
            FileFolderHelper.StringToFile(content2, out2);
        }

        public void ExportDungeon(string strClient, SqlConnection Conn)
        {
            //DungeonClass.txt
            string sql1 = "select [ClassID],[ClassName],[Content],[Link] from tbl_Guide_Dungeon where [ClassID] > -1 and [MapID] = 0 order by [ClassID]";
            DataTable dt1 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_ActivityInfo", sql1, Conn);
            string content1 = TableToString(dt1);
            string out1 = string.Format("{0}/ui/scheme/case/cyclopaedia/DungeonClass.txt", strClient);
            FileFolderHelper.StringToFile(content1, out1);

            //DungeonInfo.txt
            string sql2 = "select [MapID],[ClassID],[MinLevel],[MaxLevel],[FitLevel],[EnterWay],[BossInfo],[Introduction] from tbl_Guide_Dungeon where [MapID] > 0 order by [MapID],[ClassID]";
            DataTable dt2 = RHelper.GetDataTableWithSqlProxy("tbl_Guide_Dungeon", sql2, Conn);
            string content2 = TableToString(dt2);
            int FirstReturn2 = content2.IndexOf("\r\n");
            string stringBefore2 = content2.Substring(0, FirstReturn2 + 2);
            string stringAfter2 = content2.Substring(FirstReturn2 + 2);
            string stringMiddle2 = "0\t0\t0\t0\t0\t\t\t\r\n";
            content2 = stringBefore2 + stringMiddle2 + stringAfter2;
            string out2 = string.Format("{0}/ui/scheme/case/cyclopaedia/ActivityInfo.txt", strClient);
            FileFolderHelper.StringToFile(content2, out2);
        }

        private string TableToString(DataTable t)
        {
            StringBuilder sb = new StringBuilder();
            // 表头
            foreach (DataColumn col in t.Columns)
            {
                sb.Append(col.ColumnName);
                sb.Append('\t');
            }
            sb = sb.Remove(sb.Length - 1, 1);
            sb.Append("\r\n");
            // 各行内容
            foreach (DataRow row in t.Rows)
            {
                foreach (DataColumn col in t.Columns)
                {
                    string s = (row[col.ColumnName] == null || row[col.ColumnName] == DBNull.Value) ? string.Empty : row[col.ColumnName].ToString().Trim();
                    s = s.Replace("\r\n", "\\n");
                    sb.Append(s);
                    sb.Append('\t');
                }
                sb = sb.Remove(sb.Length - 1, 1);
                sb.Append("\r\n");
            }
            return sb.ToString();
        }
    }
}
