using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using System.Data;

namespace QuestDirector
{
    
    enum TemplateType
    {
        NPC_TEMPLATE,
        DOODAD_TEMPLATE,
        NONE
    }
    struct Point3d
    {
        public int nX;
        public int nY;
        public int nZ;
    };
    struct Template
    {
        public string strQuestColumnName;//对应与tbl_quests表里的某个templeteid列名（例如：DropDoodadTemplateID1）
        public int nQuestColumnValue;//该列的值也就是templeteid
        public TemplateType type;
        public string strNewColumnXName;
        public string strNewColumnYName;
        public string strNewColumnZName;
        public string strNewColumnMapIdName;
        public int nMapId;
        public Point3d pointFinal;
        public bool bIsRefenceMe;
        public bool bNeedRefence;

        public void Init(string columnName, TemplateType ttype, string xname, string yname, string zname, string mapidname, bool refenceMe, bool needRefence)
        {
            strQuestColumnName = columnName;
            type = ttype;
            strNewColumnXName = xname;
            strNewColumnYName = yname;
            strNewColumnZName = zname;
            strNewColumnMapIdName = mapidname;
            pointFinal.nX = 0;
            pointFinal.nY = 0;
            pointFinal.nZ = 0;
            nQuestColumnValue = 0;
            bIsRefenceMe = refenceMe;
            bNeedRefence = needRefence;
        }
        

    };
    public class KQuestDirect
    {
        private SqlConnection m_sqlConn;
        private string m_strRootDir;
        private Hashtable m_htMapIdName;

        const string QuestGuideTableName = "QuestGuide";
        const string QuestsTableName = "tbl_quests";

        private OneQuest m_oneQuest = OneQuest.GetOneQuestInstance();

        public KQuestDirect(SqlConnection conn, string rootdir)
        {
            m_sqlConn = conn;
            m_strRootDir = rootdir;
        }
        public bool Init()
        {
            //OldDataClear();
            InitMapIdName();
            m_oneQuest.Init(m_strRootDir, m_htMapIdName);
            return true;
        }
        private void InitMapIdName()
        {
            m_htMapIdName = new Hashtable();
            if (m_sqlConn.State == ConnectionState.Closed)
            {
                m_sqlConn.Open();
            }
            string strSql = "select ID, Name from maplist where [name] <> '测试1'";
            SqlCommand cmd = new SqlCommand(strSql, m_sqlConn);
            SqlDataReader dr = cmd.ExecuteReader();

            int nMapId = 0;
            string strMapName = string.Empty;
            while (dr.Read())
            {
                nMapId = (int)dr[0];
                strMapName = dr[1].ToString();
                m_htMapIdName[nMapId] = strMapName;
            }
            dr.Close();
            m_sqlConn.Close();            
        }
        private void OldDataClear()
        {
            if (m_sqlConn.State == ConnectionState.Closed)
            {
                m_sqlConn.Open();
            }
            string strSql = "delete from questguide";
            SqlCommand cmd = new SqlCommand(strSql, m_sqlConn);
            cmd.ExecuteNonQuery();
            m_sqlConn.Close();
        }
        public bool Run()
        {
            const int TEMPLATECOUNT = 16;
            Template[] arrTemplate = new Template[TEMPLATECOUNT];
            string strMapName = string.Empty;
            int nQuestId = 0;

            InitArrTemplate(arrTemplate);

            if (m_sqlConn.State == System.Data.ConnectionState.Closed)
            {
                m_sqlConn.Open();
            }
            string strSql_tbl_quests = ConstructSqlSelect(arrTemplate);
            SqlDataAdapter da = new SqlDataAdapter(strSql_tbl_quests, m_sqlConn);
            DataSet ds = new DataSet();
            da.Fill(ds, "tbl_quests");
            
            foreach(DataRow dr in ds.Tables["tbl_quests"].Rows)
            {
                Prepare(arrTemplate, ref nQuestId, ref strMapName, dr);                
                m_oneQuest.CurrentMapName = strMapName;
                LoadData(arrTemplate);
                UpdateOneQuestDirect(arrTemplate, nQuestId);
            }
            DeleteNotExists();
            if (m_sqlConn.State == System.Data.ConnectionState.Open)
            {
                m_sqlConn.Close();
            }
            return true;
        }

        private void InitArrTemplate(Template[] arrTemplate)        {
            const bool needRefence = true;
            const bool refenceMe = true;
            arrTemplate[0].Init("StartDoodadTemplateID", TemplateType.DOODAD_TEMPLATE, "StartDoodadTemplateID_nX", "StartDoodadTemplateID_nY", "StartDoodadTemplateID_nZ", "StartDoodadTemplateID_MapID", refenceMe, !needRefence);
            arrTemplate[1].Init("StartNpcTemplateID", TemplateType.NPC_TEMPLATE, "StartNpcTemplateID_nX", "StartNpcTemplateID_nY", "StartNpcTemplateID_nZ", "StartNpcTemplateID_MapId", refenceMe, !needRefence);

            arrTemplate[2].Init("EndDoodadTemplateID", TemplateType.DOODAD_TEMPLATE, "EndDoodadTemplateID_nX", "EndDoodadTemplateID_nY", "EndDoodadTemplateID_nZ", "EndDoodadTemplateID_MapId", !refenceMe, needRefence);
            arrTemplate[3].Init("EndNpcTemplateID", TemplateType.NPC_TEMPLATE, "EndNpcTemplateID_nX", "EndNpcTemplateID_nY", "EndNpcTemplateID_nZ", "EndNpcTemplateID_MapId", !refenceMe, needRefence);

            arrTemplate[4].Init("DropDoodadTemplateID1", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID1_nX", "DropDoodadTemplateID1_nY", "DropDoodadTemplateID1_nZ", "DropDoodadTemplateID1_MapId", !refenceMe, !needRefence);
            arrTemplate[5].Init("DropDoodadTemplateID2", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID2_nX", "DropDoodadTemplateID2_nY", "DropDoodadTemplateID2_nZ", "DropDoodadTemplateID2_MapId", !refenceMe, !needRefence);
            arrTemplate[6].Init("DropDoodadTemplateID3", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID3_nX", "DropDoodadTemplateID3_nY", "DropDoodadTemplateID3_nZ", "DropDoodadTemplateID3_MapId", !refenceMe, !needRefence);
            arrTemplate[7].Init("DropDoodadTemplateID4", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID4_nX", "DropDoodadTemplateID4_nY", "DropDoodadTemplateID4_nZ", "DropDoodadTemplateID4_MapId", !refenceMe, !needRefence);

            arrTemplate[8].Init("DropNpcTemplateID1", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID1_nX", "DropNpcTemplateID1_nY", "DropNpcTemplateID1_nZ", "DropNpcTemplateID1_MapId", !refenceMe, !needRefence);
            arrTemplate[9].Init("DropNpcTemplateID2", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID2_nX", "DropNpcTemplateID2_nY", "DropNpcTemplateID2_nZ", "DropNpcTemplateID2_MapId", !refenceMe, !needRefence);
            arrTemplate[10].Init("DropNpcTemplateID3", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID3_nX", "DropNpcTemplateID3_nY", "DropNpcTemplateID3_nZ", "DropNpcTemplateID3_MapId", !refenceMe, !needRefence);
            arrTemplate[11].Init("DropNpcTemplateID4", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID4_nX", "DropNpcTemplateID4_nY", "DropNpcTemplateID4_nZ", "DropNpcTemplateID4_MapId", !refenceMe, !needRefence);

            arrTemplate[12].Init("KillNpcTemplateID1", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID1_nX", "KillNpcTemplateID1_nY", "KillNpcTemplateID1_nZ", "KillNpcTemplateID1_MapId", !refenceMe, !needRefence);
            arrTemplate[13].Init("KillNpcTemplateID2", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID2_nX", "KillNpcTemplateID2_nY", "KillNpcTemplateID2_nZ", "KillNpcTemplateID2_MapId", !refenceMe, !needRefence);
            arrTemplate[14].Init("KillNpcTemplateID3", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID3_nX", "KillNpcTemplateID3_nY", "KillNpcTemplateID3_nZ", "KillNpcTemplateID3_MapId", !refenceMe, !needRefence);
            arrTemplate[15].Init("KillNpcTemplateID4", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID4_nX", "KillNpcTemplateID4_nY", "KillNpcTemplateID4_nZ", "KillNpcTemplateID4_MapId", !refenceMe, !needRefence);

            
        }

        private string ConstructSqlSelect(Template[] arrTemplate)
        {
            string strRet = string.Empty;
            string strColumns = string.Empty;

            for (int i = 0; i < arrTemplate.Length - 1; ++i)
            {
                strColumns += arrTemplate[i].strQuestColumnName + ",";
            }
            strColumns += arrTemplate[arrTemplate.Length - 1].strQuestColumnName;

            strRet = string.Format("select QuestID, _Cat, {0} from tbl_quests", strColumns);

            return strRet;

        }
        private void Prepare(Template[] arrTemplate, ref int questId, ref string mapName, DataRow dr)
        {
            for (int i = 0; i < arrTemplate.Length; ++i)
            {
                arrTemplate[i].nMapId = 0;
                arrTemplate[i].pointFinal.nX = 0;
                arrTemplate[i].pointFinal.nY = 0;

                if (dr.IsNull(arrTemplate[i].strQuestColumnName))
                {
                    arrTemplate[i].nQuestColumnValue = 0;
                }
                else
                {
                    arrTemplate[i].nQuestColumnValue = Convert.ToInt32(dr[arrTemplate[i].strQuestColumnName]);
                    
                }
                
            }
            questId = dr.IsNull("QuestId") ? 0 : Convert.ToInt32(dr["QuestId"]);
            mapName = dr["_Cat"].ToString();
        }
        private void LoadData(Template[] arrTemplate)
        {
            m_oneQuest.FillDate(arrTemplate); 
        }
        private string ConstructSqlInsert(Template[] arrTemplate, int nQuestId)
        {
            string strRet = string.Empty;
            string strColumns = string.Empty;
            string strValues = string.Empty;

            foreach (Template temp in arrTemplate)
            {
                strColumns += string.Format("{0},{1},{2},{3},", temp.strNewColumnXName, temp.strNewColumnYName, temp.strNewColumnZName, temp.strNewColumnMapIdName);
                strValues += string.Format("{0},{1},{2},{3},", temp.pointFinal.nX, temp.pointFinal.nY, temp.pointFinal.nZ, temp.nMapId);                
            }
            //设置******_Type的值（默认值）(strColumns与strValues要对应）
            strColumns += string.Format(
                "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19},{20},{21},{22},{23}",
                "StartDoodadTemplateID_Type", "StartNpcTemplateID_Type",
                "EndDoodadTemplateID_Type",   "EndNpcTemplateID_Type",
                "DropDoodadTemplateID1_Type", "DropDoodadTemplateID2_Type", "DropDoodadTemplateID3_Type", "DropDoodadTemplateID4_Type",
                "DropNpcTemplateID1_Type",    "DropNpcTemplateID2_Type",    "DropNpcTemplateID3_Type",    "DropNpcTemplateID4_Type",
                "KillNpcTemplateID1_Type",    "KillNpcTemplateID2_Type",    "KillNpcTemplateID3_Type",    "KillNpcTemplateID4_Type",
                "QuestValue1_Type",
                "QuestValue2_Type",
                "QuestValue3_Type",
                "QuestValue4_Type",
                "QuestValue5_Type",
                "QuestValue6_Type",
                "QuestValue7_Type",
                "QuestValue8_Type"
                );
             strValues += string.Format(
                "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19},{20},{21},{22},{23}",
                0, 0,
                1, 1,
                3, 3, 3, 3,
                3, 3, 3, 3,
                3, 3, 3, 3,
                3, 3, 3, 3, 3, 3, 3, 3    
                );

            strRet = string.Format("insert into QuestGuide ({0}, QuestId) values({1}, {2})", strColumns, strValues, nQuestId);

            return strRet;

        }
        private string ConstructSqlUpdate(Template[] arrTemplate, int nQuestId)
        {
            string strRet = string.Empty;
            string strSet = string.Empty;

            foreach (Template temp in arrTemplate)
            {
                strSet += string.Format(
                    "{0} = {1},{2} = {3},{4} = {5},{6} = {7},",
                    temp.strNewColumnXName, temp.pointFinal.nX,
                    temp.strNewColumnYName, temp.pointFinal.nY, 
                    temp.strNewColumnZName, temp.pointFinal.nZ,
                    temp.strNewColumnMapIdName, temp.nMapId
                    );                
            }

            strRet = string.Format("update QuestGuide set {0} where QuestID = {1}", strSet.Substring(0, strSet.Length - 1), nQuestId);

            return strRet;
        }

        private void UpdateOneQuestDirect(Template[] arrTemplate, int nQuestId)
        {
            string strSql = ConstructSqlUpdate(arrTemplate, nQuestId);

            if (Helper.SqlNoneQureyExcute(m_sqlConn, strSql) == 0)
            {
                strSql = ConstructSqlInsert(arrTemplate, nQuestId);
                Helper.SqlNoneQureyExcute(m_sqlConn, strSql);
            }
        }

        private void DeleteNotExists()
        {
            string sql = string.Format("delete from {0} where QuestID not in (select QuestID from {1})", QuestGuideTableName, QuestsTableName);
            Helper.SqlNoneQureyExcute(m_sqlConn, sql);
        }
    }
    
}
