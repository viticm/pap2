using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Collections;

namespace QuestGuideExport
{
    public class KQuestGuide2Export
    {
        private Hashtable htConvert;
        private static string[] arrstrFileColumn ;

        const int MaxColumnCapcity = 131;

        public KQuestGuide2Export()
        {
            Init();
        }
        
        private void Init()
        {
            htConvert = new Hashtable(101);
            htConvert["QuestId"] = "QuestID";
            htConvert["StartDoodadTemplateID_MapID"] = "AcceptMap";
            htConvert["StartDoodadTemplateID_nX"] = "AcceptX";
            htConvert["StartDoodadTemplateID_nY"] = "AcceptY";
            
            htConvert["StartNpcTemplateID_MapId"] = "AcceptMap";
            htConvert["StartNpcTemplateID_nX"] = "AcceptX";
            htConvert["StartNpcTemplateID_nY"] = "AcceptY";
            

            htConvert["EndDoodadTemplateID_MapID"] = "FinishMap";
            htConvert["EndDoodadTemplateID_nX"] = "FinishX";
            htConvert["EndDoodadTemplateID_nY"] = "FinishY";
            
            htConvert["EndNpcTemplateID_MapId"] = "FinishMap";
            htConvert["EndNpcTemplateID_nX"] = "FinishX";
            htConvert["EndNpcTemplateID_nY"] = "FinishY";
            

            htConvert["KillNpcTemplateID1_MapId"] = "KillNpc1Map";
            htConvert["KillNpcTemplateID1_Areas"] = "KillNpc1Area";
            
            htConvert["KillNpcTemplateID2_MapId"] = "KillNpc2Map";
            htConvert["KillNpcTemplateID2_Areas"] = "KillNpc2Area";
            htConvert["KillNpcTemplateID3_MapId"] = "KillNpc3Map";
            htConvert["KillNpcTemplateID3_Areas"] = "KillNpc3Area";
            htConvert["KillNpcTemplateID4_MapId"] = "KillNpc4Map";
            htConvert["KillNpcTemplateID4_Areas"] = "KillNpc4Area";
            
            htConvert["DropDoodadTemplateID1_MapId"] = "NeedItem1Map";
            htConvert["DropDoodadTemplateID1_Areas"] = "NeedItem1Area";
            htConvert["DropNpcTemplateID1_MapId"] = "NeedItem1Map";
            htConvert["DropNpcTemplateID1_Areas"] = "NeedItem1Area";
            
            htConvert["DropDoodadTemplateID2_MapId"] = "NeedItem2Map";
            htConvert["DropDoodadTemplateID2_Areas"] = "NeedItem2Area";
            htConvert["DropNpcTemplateID2_MapId"] = "NeedItem2Map";
            htConvert["DropNpcTemplateID2_Areas"] = "NeedItem2Area";
            
            htConvert["DropDoodadTemplateID3_MapId"] = "NeedItem3Map";
            htConvert["DropDoodadTemplateID3_Areas"] = "NeedItem3Area";
            htConvert["DropNpcTemplateID3_MapId"] = "NeedItem3Map";
            htConvert["DropNpcTemplateID3_Areas"] = "NeedItem3Area";
            
            htConvert["DropDoodadTemplateID4_MapId"] = "NeedItem4Map";
            htConvert["DropDoodadTemplateID4_Areas"] = "NeedItem4Area";
            htConvert["DropNpcTemplateID4_MapId"] = "NeedItem4Map";
            htConvert["DropNpcTemplateID4_Areas"] = "NeedItem4Area";
            
            htConvert["QuestValue1_MapId"] = "QuestState1Map";
            htConvert["QuestValue1_Areas"] = "QuestState1Area";
            htConvert["QuestValue2_MapId"] = "QuestState2Map";
            htConvert["QuestValue2_Areas"] = "QuestState2Area";
            htConvert["QuestValue3_MapId"] = "QuestState3Map";
            htConvert["QuestValue3_Areas"] = "QuestState3Area";
            htConvert["QuestValue4_MapId"] = "QuestState4Map";
            htConvert["QuestValue4_Areas"] = "QuestState4Area";
            htConvert["QuestValue5_MapId"] = "QuestState5Map";
            htConvert["QuestValue5_Areas"] = "QuestState5Area";
            htConvert["QuestValue6_MapId"] = "QuestState6Map";
            htConvert["QuestValue6_Areas"] = "QuestState6Area";
            htConvert["QuestValue7_MapId"] = "QuestState7Map";
            htConvert["QuestValue7_Areas"] = "QuestState7Area";
            htConvert["QuestValue8_MapId"] = "QuestState8Map";
            htConvert["QuestValue8_Areas"] = "QuestState8Area";
            
            //
            arrstrFileColumn = new string[]{
                "QuestID",
                "AcceptMap","AcceptX","AcceptY",
                "FinishMap","FinishX","FinishY",
                "QuestState1Map","QuestState1Area",
                "QuestState2Map","QuestState2Area",
                "QuestState3Map","QuestState3Area",
                "QuestState4Map","QuestState4Area",
                "QuestState5Map","QuestState5Area",
                "QuestState6Map","QuestState6Area",
                "QuestState7Map","QuestState7Area",
                "QuestState8Map","QuestState8Area",
                "KillNpc1Map","KillNpc1Area",
                "KillNpc2Map","KillNpc2Area",
                "KillNpc3Map","KillNpc3Area",
                "KillNpc4Map","KillNpc4Area",
                "NeedItem1Map","NeedItem1Area",
                "NeedItem2Map","NeedItem2Area",
                "NeedItem3Map","NeedItem3Area",
                "NeedItem4Map","NeedItem4Area" 
            };

        }
        private string DB2String(SqlConnection conn)
        {
            StringBuilder sbRet = new StringBuilder();           
            string strErrorLog = string.Empty;

            for (int i = 0; i < arrstrFileColumn.Length; ++i)
            {
                sbRet.Append(arrstrFileColumn[i]);
                sbRet.Append('\t');
            }
            sbRet.Replace("\t", "\r\n", sbRet.Length - 1, 1);
                if (conn.State == System.Data.ConnectionState.Closed)
                {
                    conn.Open();
                }
            try
            {
                string strSql = "select * from questguide2 order by QuestId";
                SqlDataAdapter da = new SqlDataAdapter(strSql, conn);
                DataSet ds = new DataSet();
                da.Fill(ds);
                DataTable dt = ds.Tables[0];
                Hashtable htOneRow = new Hashtable(MaxColumnCapcity);
                
                string strDBColumnName;
                string strFileColumnName;

                foreach (DataRow dr in dt.Rows)
                {
                    htOneRow.Clear();
                    foreach (DictionaryEntry de in htConvert)
                    {
                        strDBColumnName = de.Key.ToString();
                        strFileColumnName = de.Value.ToString();
                        if (dr.IsNull(strDBColumnName) || dr[strDBColumnName].ToString() == "0")
                        {
                            continue;
                        }
                        if (!htOneRow.ContainsKey(strFileColumnName))
                        {
                            htOneRow[strFileColumnName] = dr[strDBColumnName].ToString();
                        }                        
                        else
                        {
                            //error log(npc 和 doodad同时不为0)
                            System.Diagnostics.Debug.WriteLine("QuestGuideExport Has Error");
                        }
                    }
                    for (int i = 0; i < arrstrFileColumn.Length; ++i)
                    {
                        if (htOneRow.ContainsKey(arrstrFileColumn[i]))
                        {
                            sbRet.Append(htOneRow[arrstrFileColumn[i]].ToString());
                        }
                        else if (!arrstrFileColumn[i].Contains("Area"))
                        {
                            sbRet.Append('0');
                        }                        
                        sbRet.Append('\t');
                    }
                    sbRet.Replace("\t", "\r\n", sbRet.Length - 1, 1);
                }
            }
            catch(SqlException ex)
            {
                string strErr = ex.ToString();
            }
            finally
            {                
                conn.Close();
            }
            return sbRet.ToString();

        }
        private void SetHashTableZero(Hashtable ht)
        {
            if (ht == null)
            {
                return;
            }
            foreach (DictionaryEntry deht in ht)
            {
                ht[deht.Key.ToString()] = "0";
            }
        }    
        private bool String2File(string strContent, string Filename)
        {
            try
            {
                FileStream IniStream = new FileStream(Filename, FileMode.Create);
                if (!IniStream.CanWrite)
                {
                    IniStream.Close();
                    return false;
                }
                StreamWriter writer = new StreamWriter(IniStream);
                writer.Write(strContent);
                writer.Flush();
                writer.Close();
                IniStream.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        public bool Export(SqlConnection conn, string filePath)
        {
            bool bRet = false;
            string strContent = string.Empty;            
            strContent = DB2String(conn);
            bRet = String2File(strContent, filePath);

            return bRet;
        }
    }
}
