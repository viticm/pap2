using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
//using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using System.Data;

namespace QuestDirector2
{
    
    enum TemplateType
    {
        NPC_TEMPLATE,
        DOODAD_TEMPLATE,
        NONE
    };
    enum QuestColumnType
    {
        START,
        END,
        KILL_DROP,
        STATE,
        UNKNOWN
    }
    
    struct Template
    {
        public string strQuestColumnName;//对应与tbl_quests表里的某个templeteid列名（例如：DropDoodadTemplateID1）
        public int nQuestColumnValue;//该列的值也就是templeteid
        public TemplateType type;
        public string strGuideColumnNames;//
        public string strGuideColumnValues;

        public QuestColumnType emQuestColumnType;

        public void Init(string columnName, TemplateType ttype, string guideColumns, QuestColumnType columnType)
        {
            strQuestColumnName = columnName;
            nQuestColumnValue = 0;
            type = ttype;
            strGuideColumnNames = guideColumns;
            strGuideColumnValues = string.Empty;

            emQuestColumnType = columnType;
        }
        

    };
    public class KQuestDirect
    {
        private SqlConnection m_sqlConn;
        private string m_strRootDir;
        private Hashtable m_htMapIdName;

        const string QuestGuideTableName = "QuestGuide2";
        const string QuestsTableName = "tbl_quests";

        private OneQuest m_oneQuest = OneQuest.GetOneQuestInstance();

        

        public KQuestDirect(SqlConnection conn, string rootdir)
        {
            m_sqlConn = conn;
            m_strRootDir = rootdir;
        }
        public bool Init()
        {            
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
      
        public bool Run()
        {
            const int TEMPLATECOUNT = 16;
            Template[] arrTemplate = new Template[TEMPLATECOUNT];
            string strMapName = string.Empty;
            int nQuestId = 0;
            int nStartMapId = 0;
            int nEndMapId = 0;

            InitArrTemplate(arrTemplate);

            if (m_sqlConn.State == System.Data.ConnectionState.Closed)
            {
                m_sqlConn.Open();
            }
            string strSql_tbl_quests = ConstructSqlSelect(arrTemplate);
            SqlDataAdapter da = new SqlDataAdapter(strSql_tbl_quests, m_sqlConn);
            DataSet ds = new DataSet();
            da.Fill(ds, QuestsTableName);

            foreach (DataRow dr in ds.Tables[QuestsTableName].Rows)
            {
                Prepare(arrTemplate, ref nQuestId, ref strMapName, ref nStartMapId, ref nEndMapId, dr);                
                m_oneQuest.CurrentMapName = strMapName;
                m_oneQuest.CurrentStartMapId = nStartMapId;
                m_oneQuest.CurrentEndMapId = nEndMapId;
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
            arrTemplate[0].Init("StartDoodadTemplateID", TemplateType.DOODAD_TEMPLATE, "StartDoodadTemplateID_nX, StartDoodadTemplateID_nY, StartDoodadTemplateID_MapID", QuestColumnType.START);
            arrTemplate[1].Init("StartNpcTemplateID", TemplateType.NPC_TEMPLATE, "StartNpcTemplateID_nX, StartNpcTemplateID_nY, StartNpcTemplateID_MapId", QuestColumnType.START);

            arrTemplate[2].Init("EndDoodadTemplateID", TemplateType.DOODAD_TEMPLATE, "EndDoodadTemplateID_nX, EndDoodadTemplateID_nY, EndDoodadTemplateID_MapId", QuestColumnType.END);
            arrTemplate[3].Init("EndNpcTemplateID", TemplateType.NPC_TEMPLATE, "EndNpcTemplateID_nX, EndNpcTemplateID_nY, EndNpcTemplateID_MapId", QuestColumnType.END);

            arrTemplate[4].Init("DropDoodadTemplateID1", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID1_Areas, DropDoodadTemplateID1_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[5].Init("DropDoodadTemplateID2", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID2_Areas, DropDoodadTemplateID2_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[6].Init("DropDoodadTemplateID3", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID3_Areas, DropDoodadTemplateID3_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[7].Init("DropDoodadTemplateID4", TemplateType.DOODAD_TEMPLATE, "DropDoodadTemplateID4_Areas, DropDoodadTemplateID4_MapId", QuestColumnType.KILL_DROP);

            arrTemplate[8].Init("DropNpcTemplateID1", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID1_Areas, DropNpcTemplateID1_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[9].Init("DropNpcTemplateID2", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID2_Areas, DropNpcTemplateID2_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[10].Init("DropNpcTemplateID3", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID3_Areas, DropNpcTemplateID3_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[11].Init("DropNpcTemplateID4", TemplateType.NPC_TEMPLATE, "DropNpcTemplateID4_Areas, DropNpcTemplateID4_MapId", QuestColumnType.KILL_DROP);

            arrTemplate[12].Init("KillNpcTemplateID1", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID1_Areas, KillNpcTemplateID1_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[13].Init("KillNpcTemplateID2", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID2_Areas, KillNpcTemplateID2_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[14].Init("KillNpcTemplateID3", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID3_Areas, KillNpcTemplateID3_MapId", QuestColumnType.KILL_DROP);
            arrTemplate[15].Init("KillNpcTemplateID4", TemplateType.NPC_TEMPLATE, "KillNpcTemplateID4_Areas, KillNpcTemplateID4_MapId", QuestColumnType.KILL_DROP);
            
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

            strRet = string.Format("select QuestID, _Cat, StartMapId, EndMapId, {0} from {1}", strColumns, QuestsTableName);

            return strRet;

        }
        private void Prepare(Template[] arrTemplate, ref int questId, ref string mapName, ref int startMapId, ref int endMapId, DataRow dr)
        {
            for (int i = 0; i < arrTemplate.Length; ++i)
            {
                arrTemplate[i].strGuideColumnValues = string.Empty;
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
            startMapId = dr.IsNull("StartMapId") ? 0 : Convert.ToInt32(dr["StartMapId"]);
            endMapId = dr.IsNull("EndMapId") ? 0 : Convert.ToInt32(dr["EndMapId"]);
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
                if (temp.strGuideColumnValues != string.Empty)
                {
                    strColumns += string.Format("{0},", temp.strGuideColumnNames);
                    strValues += string.Format("{0},", temp.strGuideColumnValues);
                }
                
            }  
            strRet = string.Format("insert into {3} ({0} QuestId) values({1} {2})", strColumns, strValues, nQuestId, QuestGuideTableName).Replace('-', ',');
            

            return strRet;

        }
        private string ConstructSqlUpdate(Template[] arrTemplate, int nQuestId)
        {
            string strRet = string.Empty;
            string strSet = string.Empty;           

            foreach (Template temp in arrTemplate)
            {
                string[] cellNames = temp.strGuideColumnNames.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                string[] cellValues = temp.strGuideColumnValues.Split(new char[] { '-' }, StringSplitOptions.RemoveEmptyEntries);
                if (cellValues.Length < cellNames.Length)
                {
                    //error:
                    continue;
                }
                for (int i = 0; i < cellNames.Length; ++i)
                {
                    strSet += string.Format(
                        "{0} = {1},",
                        cellNames[i], cellValues[i]                        
                        );
                }
            }
            if (strSet == string.Empty)
            {
                strSet = string.Format("QuestID = {0},", nQuestId);
            }

            strRet = string.Format("update {2} set {0} where QuestID = {1}", strSet.Substring(0, strSet.Length - 1), nQuestId, QuestGuideTableName);

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
