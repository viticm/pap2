using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using MapLogicalFileReader;
using System.Data;
using System.Data.SqlClient;


namespace QuestDirector2
{
    class OneQuest
    {
        private string m_strRootDir;       
        private string m_strCurrentMapName;
        private int m_nCurrentStartMapId;
        private int m_nCurrentEndMapId;
        private static OneQuest g_OneQuestInstance;
        private Hashtable m_htMapId;
        private Hashtable m_htMapName;

        private Hashtable m_htMapNameNpcTable;
        private Hashtable m_htMapNameDoodadTable;
        private Hashtable m_htMapNameHashTableGreeyIds;//key:mapname value:(hashtabel key:templateid,value:greeyids)
        private Hashtable m_htRegionInfo;//key : mapname,value: RegionInfo


        const string nTempleteIDColumnName = "nTempleteID";
        const string nXColumnName = "nX";
        const string nYColumnName = "nY";

        private int nStartX;
        private int nStartY;


        

        readonly int MaxTableCapcity = 59;
        readonly int MaxTemplateCapcity = 199;

        public static OneQuest GetOneQuestInstance()
        {
            if (g_OneQuestInstance == null)
            {
                g_OneQuestInstance = new OneQuest();
            }
            return g_OneQuestInstance;
        }
        public string CurrentMapName
        {
            set
            {
                m_strCurrentMapName = value;
            }
        }
        public int CurrentStartMapId
        {
            set
            {
                m_nCurrentStartMapId = value;
            }
        }
        public int CurrentEndMapId
        {
            set
            {
                m_nCurrentEndMapId = value;
            }
        }
        public OneQuest()
        {
            m_htMapNameNpcTable = new Hashtable(MaxTableCapcity);
            m_htMapNameDoodadTable = new Hashtable(MaxTableCapcity);
            m_htMapNameHashTableGreeyIds = new Hashtable(MaxTableCapcity);
            m_htRegionInfo = new Hashtable(MaxTableCapcity);
            m_htMapId = new Hashtable(MaxTableCapcity);
            m_htMapName = new Hashtable(MaxTableCapcity);

        }
        ~OneQuest()
        {
            m_htMapNameNpcTable.Clear();
            m_htMapNameDoodadTable.Clear();            
            m_htRegionInfo.Clear();
            m_htMapId.Clear();
            m_htMapName.Clear();
            foreach (DictionaryEntry de in m_htMapNameHashTableGreeyIds)
            {
                (de.Value as Hashtable).Clear();
            }
            m_htMapNameHashTableGreeyIds.Clear();
        }
      
        public bool Init(string strRootdir, Hashtable htMapId)
        {
            m_strRootDir = strRootdir;

            string strMapName;
            int nMapId;
            foreach (DictionaryEntry de in htMapId)
            {
                strMapName = de.Value.ToString();
                nMapId = (int)de.Key;
                m_htMapId[strMapName] = nMapId;
                m_htMapName[nMapId] = strMapName;
            }
            string strLogicalFilePath;
            const string NPC = "NPC";
            const string Doodad = "Doodad";
            string[] ReadingColumns = { nXColumnName, nYColumnName, nTempleteIDColumnName };
            foreach (string mapname in m_htMapName.Values)
            {
                strLogicalFilePath = string.Format("{0}/data/source/maps/{1}/{1}.Map.Logical", m_strRootDir, mapname);

                MapLogicalTableReader npcReader = new MapLogicalTableReader(strLogicalFilePath, NPC, ReadingColumns);
                MapLogicalTableReader DoodadReader = new MapLogicalTableReader(strLogicalFilePath, Doodad, ReadingColumns);
                if (npcReader.Init())
                {
                    m_htMapNameNpcTable[mapname] = npcReader;
                }                
                if (DoodadReader.Init())
                {
                    m_htMapNameDoodadTable[mapname] = DoodadReader;
                } 

                //regionInfo
                //RegionInfo region = new RegionInfo();
                //if (region.Init(m_strRootDir, mapname))
                //{
                //    m_htRegionInfo[mapname] = region;
                //}
                //
                //Hashtable htTemplateIdGreeyIds = new Hashtable(MaxTemplateCapcity);
                //m_htMapNameHashTableGreeyIds[mapname] = htTemplateIdGreeyIds;

            }
            return true;
        }
        public bool FillDate(Template[] temp)
        {
            nStartX = 0;
            nStartY = 0;
            for (int i = 0; i < temp.Length; ++i)
            {
                FillOneTemplate(ref temp[i]);
            }
            return true;
        }
       
        private void FillOneTemplate(ref Template OneTemplate)
        {
            if (OneTemplate.type == TemplateType.NONE ||
                OneTemplate.nQuestColumnValue == 0 ||
                OneTemplate.emQuestColumnType == QuestColumnType.UNKNOWN
                )
                return;

            DataRow[] templateRows = null;
            string CurrentTemplateMapName = m_strCurrentMapName;
            if (!UpdateCurrentMapName(OneTemplate.nQuestColumnValue, OneTemplate.type, OneTemplate.emQuestColumnType, ref templateRows, ref CurrentTemplateMapName))
            {
                //所有地图都不存在此tempateId(OneTemplate.nQuestColumnValue)
                return;
            }
            switch (OneTemplate.emQuestColumnType)
            {
                case QuestColumnType.START:
                    FillOneStartTemplate(ref OneTemplate, ref templateRows, ref CurrentTemplateMapName);
                    break;
                case QuestColumnType.END:
                    FillOneEndTemplate(ref OneTemplate, ref templateRows, ref CurrentTemplateMapName);
                    break;
                case QuestColumnType.KILL_DROP:
                   // FillOneKill_DropTemplate(ref OneTemplate, ref templateRows, ref CurrentTemplateMapName);
                    int mapid = Convert.ToInt32(m_htMapId[CurrentTemplateMapName]);
                    string greeyIds = "''";
                    OneTemplate.strGuideColumnValues = string.Format("{0}-{1}", greeyIds, mapid);
                    break;
                default:
                    return;
            }
                  
        }

        private void FillOneStartTemplate(ref Template OneTemplate, ref DataRow[] templateRows, ref string CurrentTemplateMapName)
        {          
            int mapid = Convert.ToInt32(m_htMapId[CurrentTemplateMapName]);
            nStartX = Convert.ToInt32(templateRows[0][nXColumnName]);
            nStartY = Convert.ToInt32(templateRows[0][nYColumnName]);
            OneTemplate.strGuideColumnValues = string.Format("{0}-{1}-{2}", nStartX, nStartY, mapid);
 
        }
        private void FillOneEndTemplate(ref Template OneTemplate, ref DataRow[] templateRows, ref string CurrentTemplateMapName)
        {
            int mapid = Convert.ToInt32(m_htMapId[CurrentTemplateMapName]);
            int nX = 0;
            int nY = 0;
            GetNearestEnd(templateRows, ref nX, ref nY);
            OneTemplate.strGuideColumnValues = string.Format("{0}-{1}-{2}", nX, nY, mapid);
        }
        private void FillOneKill_DropTemplate(ref Template OneTemplate, ref DataRow[] templateRows, ref string CurrentTemplateMapName)
        {
            int mapid = Convert.ToInt32(m_htMapId[CurrentTemplateMapName]);            
            RegionInfo region = m_htRegionInfo[CurrentTemplateMapName] as RegionInfo;
            Hashtable htGreeyIds = m_htMapNameHashTableGreeyIds[CurrentTemplateMapName] as Hashtable;
            if (region == null || htGreeyIds == null)
            {
                return;            
            } 

            int nTemplateId = OneTemplate.nQuestColumnValue;
            string greeyIds = string.Empty;
            if (htGreeyIds.ContainsKey(nTemplateId))
            {
                greeyIds = htGreeyIds[nTemplateId].ToString();
            }
            else
            {
                int nX = 0;
                int nY = 0;
                int nGreeyId = 0;
             
                ArrayList arrGreeyId = new ArrayList();
                for (int i = 0; i < templateRows.Length; ++i)
                {
                    nX = Convert.ToInt32(templateRows[i][nXColumnName]);
                    nY = Convert.ToInt32(templateRows[i][nYColumnName]);
                    if (region.GetGreyDegree(nX, nY, ref nGreeyId))
                    {
                        if (arrGreeyId.Contains(nGreeyId))
                            continue;
                        arrGreeyId.Add(nGreeyId);
                    }
                }
                for (int j = 0; j < arrGreeyId.Count; ++j)
                {
                    greeyIds += string.Format("{0},", arrGreeyId[j]);
                }
                if (greeyIds.Length > 0)
                {
                    greeyIds = string.Format("'{0}'", greeyIds.Substring(0, greeyIds.Length - 1));
                }                 
                htGreeyIds[nTemplateId] = greeyIds;
            }

            OneTemplate.strGuideColumnValues = string.Format("{0}-{1}", greeyIds, mapid);


        }
       
        private void GetNearestEnd(DataRow[] rows, ref int nX, ref int nY)
        {
            if (rows.Length == 1 || 
                (nStartX == 0 && nStartY == 0)
                )
            {
                nX = Convert.ToInt32(rows[0][nXColumnName]);
                nY = Convert.ToInt32(rows[0][nYColumnName]);
                return;
            }

            double minDistance = double.MaxValue;
            double distance;
            int tempX;
            int tempY;
            for (int i = 0; i < rows.Length; ++i)
            {
                tempX = Convert.ToInt32(rows[i][nXColumnName]);
                tempY = Convert.ToInt32(rows[i][nYColumnName]);
                distance = (tempX - nStartX) * (tempX - nStartX) + (tempY - nStartX) * (tempY - nStartX);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nX = tempX;
                    nY = tempY;
                }
            }
        }


        public bool IsMapLegal(string strMapName)
        {
            string str = strMapName.Trim();
            if (str == "" ||
                str == "默认分类" ||
                str == "测试")
            {
                return false;
            }
            return true;
        }

        /// <summary>
        /// 根据nTemplateId 和type更新当前地图名称
        /// 如果所有的地图都找不到返回false
        /// </summary>
        /// <param name="nTemplateId"></param>
        /// <param name="rows">返回该nTemplateId对应的行</param>
        /// <param name="type"></param>
        /// <returns></returns>
        private bool UpdateCurrentMapName(int nTemplateId, TemplateType ttype, QuestColumnType colType, ref DataRow[] rows, ref string newMapName)
        {
           
            Hashtable htMapNameReader = null;
            switch (ttype)
            {
                case TemplateType.NPC_TEMPLATE:
                    htMapNameReader = m_htMapNameNpcTable;
                    break;
                case TemplateType.DOODAD_TEMPLATE:
                    htMapNameReader = m_htMapNameDoodadTable;
                    break;
                default:
                    return false;
            }            
            if (htMapNameReader.ContainsKey(m_strCurrentMapName))                
            {
                MapLogicalTableReader reader = htMapNameReader[m_strCurrentMapName] as MapLogicalTableReader;
                rows = reader.Select(nTempleteIDColumnName, nTemplateId.ToString());
                if (rows.Length > 0)
                {
                    return true;
                }                
            }
            if (colType == QuestColumnType.START && m_htMapName.ContainsKey(m_nCurrentStartMapId))
            {
                string mapname = m_htMapName[m_nCurrentStartMapId] as string;
                MapLogicalTableReader reader = htMapNameReader[mapname] as MapLogicalTableReader;

                if(reader != null)
                {
                    rows = reader.Select(nTempleteIDColumnName, nTemplateId.ToString());
                    if (rows.Length > 0)
                    {
                        newMapName = mapname;
                        return true;
                    } 
                }
               
            }
            else if (colType == QuestColumnType.END && m_htMapName.ContainsKey(m_nCurrentEndMapId))
            {
                string mapname = m_htMapName[m_nCurrentEndMapId] as string;
                MapLogicalTableReader reader = htMapNameReader[mapname] as MapLogicalTableReader;

                if (reader != null)
                {
                    rows = reader.Select(nTempleteIDColumnName, nTemplateId.ToString());
                    if (rows.Length > 0)
                    {
                        newMapName = mapname;
                        return true;
                    }
                }

            }
            foreach (DictionaryEntry de in htMapNameReader)
            {
                MapLogicalTableReader reader = de.Value as MapLogicalTableReader;
                string mapname = de.Key as string;

                rows = reader.Select(nTempleteIDColumnName, nTemplateId.ToString());
                if (rows.Length > 0)
                {
                    newMapName = mapname;
                    return true;
                } 
            }
            return false;
            
        }

    }
}
