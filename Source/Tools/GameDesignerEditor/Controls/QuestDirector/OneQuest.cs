using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;


namespace QuestDirector
{
    class OneQuest
    {
        private Hashtable m_htMapInfo4Quest;
        private string m_strRootDir;       
        private string m_strCurrentMapName;
        private static OneQuest g_OneQuestInstance;
        private Hashtable m_htMapId;
        private Hashtable m_htMapName;

        readonly int MaxTableCapcity = 29;

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
        public OneQuest()
        {
            m_htMapInfo4Quest = new Hashtable(MaxTableCapcity);
            m_htMapId = new Hashtable(MaxTableCapcity);
            m_htMapName = new Hashtable(MaxTableCapcity);

        }
        ~OneQuest()
        {
            m_htMapInfo4Quest.Clear();
            m_htMapId.Clear();
            m_htMapName.Clear();
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
            foreach (string mapname in m_htMapName.Values)
            {
                strLogicalFilePath = string.Format("{0}/data/source/maps/{1}/{1}.Map.Logical", m_strRootDir, mapname);
                MapInfo4Quest mapInfo = new MapInfo4Quest(strLogicalFilePath);
                if (mapInfo.Init())
                {
                    m_htMapInfo4Quest[mapname] = mapInfo;
                }                                
            }
            return true;
        }
        public bool FillDate(Template[] temp)
        {
            Point3d pointRefence = new Point3d();
            bool canRefence = GetRefencePoint(temp, ref pointRefence);

            for (int i = 0; i < temp.Length; ++i)
            {
                if (temp[i].bIsRefenceMe)
                {
                    continue;
                }
                else if (temp[i].bNeedRefence && canRefence)
                {
                    FillOneTemplate(ref temp[i], true, pointRefence);
                }
                else
                {
                    FillOneTemplate(ref temp[i], false, pointRefence);
                }                
            }
                return true;
        }
        private bool GetRefencePoint(Template[] temps, ref Point3d pointRet)
        {
            bool bRet = false;

            for (int i = 0; i < temps.Length; ++i)
            {
                if (temps[i].nQuestColumnValue != 0 && temps[i].bIsRefenceMe)
                {
                    FillOneTemplate(ref temps[i], false, new Point3d());
                    pointRet = temps[i].pointFinal;
                    bRet = true;
                    break;
                }
            }            
            return bRet;

        }
        /// <summary>
        /// 获取一个id在logical文件里的最终坐标
        /// </summary>
        /// <param name="mapName">所属地图名称</param>
        /// <param name="templateId">模块id</param>
        /// <param name="templatetype">模块类型</param>
        /// <returns></returns>
        private Point3d GetFinalPoint(int templateId, TemplateType templatetype, bool needRefence, Point3d pointRefence)
        {
            Point3d pointRet;
            MapInfo4Quest mapInfo;

            pointRet.nX = -1;
            pointRet.nY = -1;
            pointRet.nZ = -1;
            if (m_htMapInfo4Quest.ContainsKey(m_strCurrentMapName))
            {
                mapInfo = (MapInfo4Quest)m_htMapInfo4Quest[m_strCurrentMapName];
            }
            else
            {
                string strLogicalFilePath = string.Format("{0}/data/source/maps/{1}/{1}.Map.Logical", m_strRootDir, m_strCurrentMapName);
                mapInfo = new MapInfo4Quest(strLogicalFilePath);
                if (mapInfo.Init())
                {
                    m_htMapInfo4Quest[m_strCurrentMapName] = mapInfo;
                }
            }
            if (mapInfo.IsAvailabel)
            {
                pointRet = mapInfo.GetFinalPoint(templateId, templatetype, needRefence, pointRefence);
            }
            return pointRet;
        }
        private void FillOneTemplate(ref Template OneTemplate, bool needRefence, Point3d pointRefence)
        {
            int nTemplateId = 0;
            TemplateType type = TemplateType.NONE;
            
            nTemplateId = OneTemplate.nQuestColumnValue;
            type = OneTemplate.type;
            if (nTemplateId == 0 || type == TemplateType.NONE)
            {
                return;
            }
            if( !UpdateCurrentMapName(nTemplateId, type))
            {
                return;
            }            
            OneTemplate.nMapId = (int)m_htMapId[m_strCurrentMapName];
            Point3d pointFinal;
            pointFinal = GetFinalPoint(nTemplateId, type, needRefence, pointRefence);
            OneTemplate.pointFinal = pointFinal;
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
        /// <param name="type"></param>
        /// <returns></returns>
        private bool UpdateCurrentMapName(int nTemplateId, TemplateType type)
        {            
            if (m_htMapInfo4Quest.ContainsKey(m_strCurrentMapName) &&
                ((MapInfo4Quest)m_htMapInfo4Quest[m_strCurrentMapName]).IsTemplateIdExists(nTemplateId, type))
            {
                return true;
            }
            foreach (DictionaryEntry de in m_htMapInfo4Quest)
            {
                if (((MapInfo4Quest)de.Value).IsTemplateIdExists(nTemplateId, type))
                {
                    m_strCurrentMapName = de.Key.ToString();
                    return true;
                }
            }
            return false;
            
        }

    }
}
