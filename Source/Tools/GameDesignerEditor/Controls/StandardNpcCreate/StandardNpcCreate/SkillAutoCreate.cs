using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Windows.Forms;
using System.Reflection;
using System.Collections;

namespace StandardNpcCreate
{
    class SkillAutoCreate
    {
        private SqlConnection conn; // sql连接
        private string npcForce; // npc的势力
        private string forceDetail; // 势力细节
        private string currentSkillID2; // 技能ID
        private DataTable npcGlobeTable; // npc全局表
        private DataTable npcRepresentTable; // npc表现表
        private DataTable npcForceSkillTable; // npc势力技能总表
        private DataTable npcAnimationTable; // npc动作表
        private DataTable modelTable; // 模型表
        private StringBuilder logText = new StringBuilder(); // 日志文本
        private string rootDir; // 根目录
        private string npcRepresentID; // npc表现ID
        private string npcTemplateID; // npc模板ID        
        private int skillLevel; // 技能等级
        private Random random = new Random(); // 随机数种子
        private int randomIndex; // 随机编号
        private Helper helper = Helper.GetHelper();

        private string updateModelFile; // 需要外部更新的字段
        private string updateSourceID; // 需要外部更新的字段
        private string updateEditorID; // 需要外部更新的字段
        private string updateNpcForce; // 需要外部更新的字段
        private string updateForceDetail; // 需要外部更新的字段
        private string updateColorChannel; // 需要外部更新的字段
        private string faceMeshFile; // 需要外部更新的字段
        private string faceMaterialFile; // 需要外部更新的字段

        private List<string[]> updateRepresentInfoList = new List<string[]>(); // 需要外部更新的数据链表

        enum UserSection { None, AutoCreate, NotCreate }; // 用户选择枚举
        UserSection userSection = UserSection.None; // 当前用户选择 

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="rootDir">根目录</param>
        public SkillAutoCreate(SqlConnection conn, string rootDir)
        {
            this.conn = conn;
            this.rootDir = rootDir;
        }        

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateColorChannel
        {
            get
            {
                return updateColorChannel;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string FaceMaterialFile
        {
            get
            {
                return faceMaterialFile;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string FaceMeshFile
        {
            get
            {
                return faceMeshFile;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateModelFile
        {
            get
            {
                return updateModelFile;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateEditorID
        {
            get
            {
                return updateEditorID;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateSourceID
        {
            get
            {
                return updateSourceID;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateNpcForce
        {
            get
            {
                return updateNpcForce;
            }
        }

        /// <summary>
        /// 需要外部更新的字段
        /// </summary>
        public string UpdateForceDetail
        {
            get
            {
                return updateForceDetail;
            }
        }

        /// <summary>
        /// npc表现ID
        /// </summary>
        public string NpcRepresentID
        {
            set
            {
                npcRepresentID = value;
            }
        }

        /// <summary>
        /// npc模板ID
        /// </summary>
        public string NpcTemplateID
        {
            set
            {
                npcTemplateID = value;
            }
        }

        /// <summary>
        /// 势力细节
        /// </summary>
        public string ForceDetail
        {
            set
            {
                forceDetail = value;
            }
        }

        /// <summary>
        /// npc的势力
        /// </summary>
        public string NpcForce
        {
            set
            {
                npcForce = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {                       
            string filePath = Path.Combine(Application.StartupPath, @"Design\NPC势力技能总表.xls");
            string content = helper.GetFileContent(filePath);

            if (content != null)
            {
                npcForceSkillTable = helper.CreateDataTableFromExcelFile(filePath, "势力");
            }

            filePath = Path.Combine(Application.StartupPath, @"Design\Mdl.tab");
            content = helper.GetFileContent(filePath);

            if (content != null)
            {
                modelTable = helper.CreateDataTable(content);
            }            

            // 初始化数据表
            ReloadDataTable();
        }

        /// <summary>
        /// 重新加载数据表
        /// </summary>
        public void ReloadDataTable()
        {
            string sqlString = "SELECT ID, Name, RepresentID1, Level, SkillID1, SkillLevel1, SkillID2, SkillLevel2, SkillID3, SkillLevel3, SkillID4, SkillLevel4, MapName FROM NpcTemplate";
            npcGlobeTable = helper.GetDataTable(conn, sqlString);

            sqlString = "SELECT RepresentID, AnimationID, AnimationFile FROM animation_npc";
            npcAnimationTable = helper.GetDataTable(conn, sqlString);

            sqlString = "SELECT * FROM npc";
            npcRepresentTable = helper.GetDataTable(conn, sqlString);
        }

        /// <summary>
        /// 清空需要更新的表现信息
        /// </summary>
        public void ClearRepresentInfo()
        {
            updateRepresentInfoList.Clear();
        }

        /// <summary>
        /// 增加需要更新的表现信息
        /// </summary>
        /// <param name="fieldName"></param>
        /// <param name="fieldValue"></param>
        public void AddRepresentInfo(string fieldName, string fieldValue)
        {
            updateRepresentInfoList.Add(new string[] { fieldName, fieldValue });
        }

        /// <summary>
        /// 强制保存NPC表里表现ID的信息
        /// </summary>
        /// <param name="representID">表现ID</param>
        /// <param name="templateID">模板ID</param>
        public void UpdateNpcTemplateRepresentInfo(string representID, string templateID)
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = null;
                SqlCommand cmd = conn.CreateCommand();
                sqlString = string.Format("UPDATE NpcTemplate SET RepresentID1 = '{0}' WHERE ID = '{1}'", representID,
                                          templateID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("更新NPC表的RepresentID时产生sql异常：" + ex.Message, "更新NPC表现",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }   
        }

        /// <summary>
        /// 刷新表现信息
        /// </summary>
        /// <param name="representID">表现ID</param>
        /// <param name="mapName">地图名</param>
        /// <param name="npcName">NPC名</param>
        public void RefreshRepresentInfo(string representID)
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = null;
                SqlCommand cmd = conn.CreateCommand();

                foreach(string[] stringArray in updateRepresentInfoList)
                {
                    sqlString = string.Format("UPDATE npc SET [{0}] = '{1}' WHERE RepresentID = {2}", stringArray[0],
                                              stringArray[1], representID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("更新NPC表现时产生sql异常：" + ex.Message, "更新NPC表现",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }             
        }

        /// <summary>
        /// 获取npc的信息表
        /// </summary>
        /// <param name="templateID">模板ID</param>
        /// <returns>npc的信息表</returns>
        public Hashtable GetNpcInfoTable(int templateID)
        {            
            Hashtable infoTable = new Hashtable();

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Name FROM NpcTemplate WHERE ID = {0}", templateID.ToString());
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                object selectResult = cmd.ExecuteScalar();
                string npcName = selectResult.ToString();

                infoTable["npcName"] = npcName;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("获取NPC信息时产生sql异常：" + ex.Message, "获取NPC信息",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }                                                                 

            return infoTable;
        }

        /// <summary>
        /// 获取动作ID
        /// </summary>
        /// <param name="skillName">动作名称</param>
        /// <returns>动作ID</returns>
        private string GetAnimationID(string animationName)
        {
            string animationID = "0";

            switch (animationName.ToUpper()) // 大小写转换一次
            {
                case "BUFF":
                    {
                        animationID = "64";
                        break;
                    }
                case "范围":
                    {
                        animationID = "65";
                        break;
                    }
                case "重击":
                    {
                        animationID = "66";
                        break;
                    }
                case "远程":
                    {
                        animationID = "67";
                        break;
                    }
                case "释放":
                    {
                        animationID = "68";
                        break;
                    }
                case "聚力":
                    {
                        animationID = "69";
                        break;
                    }
            }

            return animationID;
        }

        /// <summary>
        /// 去掉字符串中的括号
        /// </summary>
        /// <param name="inString">源字符串</param>
        /// <returns>去掉括号的字符串</returns>
        private string RemoveBracket(string inString)
        {
            string outString = inString;
            int firstIndex = inString.IndexOf("（");

            if (firstIndex >= 0)
            {
                outString = inString.Substring(0, firstIndex);
            }

            return outString;
        }

        /// <summary>
        /// 自动创建npc技能
        /// </summary>
        /// <returns>是否创建成功</returns>
        private bool AutoCreateSkills(bool showMessage)
        {
            bool success = true;

            if (!string.IsNullOrEmpty(npcForce) && !string.IsNullOrEmpty(forceDetail))
            {
                // 去掉括号里的内容
                npcForce = RemoveBracket(npcForce);
                forceDetail = RemoveBracket(forceDetail);

                DataRow[] rows = npcForceSkillTable.Select(string.Format("[势力] = '{0}' AND [名字] = '{1}'", npcForce, forceDetail));
                if (rows.Length > 0)
                {
                    DataRow row = rows[0];
                    DataRow[] npcRows = npcGlobeTable.Select(string.Format("ID = '{0}'", npcTemplateID));

                    if (npcRows.Length > 0)
                    {
                        DataRow npcRow = npcRows[0];
                        string npcLevelString = npcRow["Level"].ToString();
                        int npcLevel = 1;
                        if (npcLevelString != "")
                        {
                            npcLevel = int.Parse(npcLevelString);
                        }                        
                        skillLevel = GetSkillLevel(npcLevel);
                        success = UpdateSkill(row);
                    }                    
                }
                else
                {
                    if (!string.IsNullOrEmpty(currentSkillID2) && currentSkillID2 != "0")
                    {
                        rows = npcForceSkillTable.Select(string.Format("[skillid2] = '{0}'", currentSkillID2));

                        if (rows.Length > 0)
                        {
                            logText.AppendLine("势力和名字匹配不上，尝试用第二技能匹配...");

                            DataRow row = rows[0];
                            DataRow[] npcRows = npcGlobeTable.Select(string.Format("ID = '{0}'", npcTemplateID));

                            if (npcRows.Length > 0)
                            {
                                DataRow npcRow = npcRows[0];
                                string npcLevelString = npcRow["Level"].ToString();
                                int npcLevel = 1;
                                if (npcLevelString != "")
                                {
                                    npcLevel = int.Parse(npcLevelString);
                                }
                                skillLevel = GetSkillLevel(npcLevel);
                                success = UpdateSkill(row);
                            } 
                        }
                        else
                        {
                            success = false;
                            logText.AppendLine("没有找到匹配的数据行！");
                        }
                    }
                    else
                    {
                        success = false;
                        logText.AppendLine("没有找到匹配的数据行！");
                    }                    
                }
            }
            else
            {
                success = false;

                if (showMessage)
                {
                    MessageBox.Show("NPC的Force和ForceDetail信息不完整！", "更新技能",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    logText.AppendLine("Error:NPC的Force和ForceDetail信息不完整！");
                }
            }

            if (success)
            {                
                if (showMessage)
                {
                    MessageBox.Show("技能更新成功！", "更新技能",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    logText.AppendLine("技能更新成功！");
                }
            }
            else
            {
                if (showMessage)
                {
                    MessageBox.Show(string.Format("技能更新失败，原因如下：\r\n\r\n{0}", logText.ToString()), "更新技能",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    logText.AppendLine("技能更新失败！");
                }
                
            }

            return success;
        }

        /// <summary>
        /// 批量更新技能
        /// </summary>
        /// <param name="templateID">模板ID</param>
        /// <param name="mapName">地图名</param>        
        public void MultiUpdateSkills(int templateID, string mapName)
        {            
            List<string> npcInfoList = new List<string>();
            List<string> updateSuccessNpcList = new List<string>();
            List<string> updateFailNpcList = new List<string>();
            logText = new StringBuilder();
            DataRow[] rows;
            DataRow row;
            string npcID;
            string npcName;
            string skillID2;

            string sqlString = string.Format("SELECT * FROM npc_skill_record");
            DataTable recordTable = helper.GetDataTable(conn, sqlString);

            sqlString = string.Format("SELECT RepresentID, SkillID FROM dic_npc_skill_caster_npc");
            DataTable skillTable = helper.GetDataTable(conn, sqlString);

            if (templateID == 0) // 更新多个npc
            {
                rows = npcGlobeTable.Select(string.Format("MapName = '{0}'", mapName));
            }
            else // 更新单个npc
            {
                rows = npcGlobeTable.Select(string.Format("ID = {0}", templateID.ToString()));
            }

            foreach(DataRow dataRow in rows)
            {
                npcID = dataRow["ID"].ToString();
                npcName = dataRow["Name"].ToString();
                skillID2 = dataRow["SkillID2"].ToString();

                DataRow[] dataRows = recordTable.Select(string.Format("ID = '{0}'", npcID));
                if (dataRows.Length > 0) // 有更新记录
                {
                    if (dataRows[0]["IsSpecific"].ToString() == "True") // 特殊npc优先
                    {
                        npcInfoList.Add(string.Format("{0} ({1})    [特殊]", npcName, npcID));
                    }
                    else
                    {
                        if (skillID2 == "" || skillID2 == "0") // 未填第二技能
                        {
                            npcInfoList.Add(string.Format("{0} ({1})    [未填第二技能]", npcName, npcID));
                        }
                        else
                        {
                            npcInfoList.Add(string.Format("{0} ({1})", npcName, npcID));                            
                        }
                    }
                }
                else
                {
                    if (skillID2 == "" || skillID2 == "0") // 未填第二技能
                    {                       
                        npcInfoList.Add(string.Format("{0} ({1})    [未填第二技能]", npcName, npcID));
                    }
                    else
                    {
                        npcInfoList.Add(string.Format("{0} ({1})", npcName, npcID));
                    }
                }
            }

            MultiUpdateSkillForm mForm = new MultiUpdateSkillForm(conn, npcInfoList);
            if (mForm.ShowDialog() == DialogResult.OK)
            {
                List<string> selectedNpcInfoList = mForm.SelectedNpcInfoList;
                
                // 更新技能
                foreach(string s in selectedNpcInfoList)
                {
                    bool updateSuccess = false;
                    npcID = GetNpcID(s).ToString();
                    npcName = GetNpcName(s);
                    logText.AppendLine(string.Format("开始为ID为{0}的NPC更新技能...", npcID));

                    rows = npcGlobeTable.Select(string.Format("ID = {0}", npcID));
                    if (rows.Length > 0)
                    {
                        row = rows[0];
                        npcTemplateID = npcID;
                        npcRepresentID = row["RepresentID1"].ToString();                        

                        if (npcRepresentID != "" && npcRepresentID != "0")
                        {
                            // 获取第二技能ID
                            DataRow[] dataRows = skillTable.Select(string.Format("RepresentID = '{0}' AND SkillID <> 28", npcRepresentID));
                            if (dataRows.Length > 0) 
                            {
                                currentSkillID2 = dataRows[0]["SkillID"].ToString();
                            }
                            else
                            {
                                currentSkillID2 = null;
                            }

                            string mainModelFile = GetMainModelFile(npcRepresentID);

                            if (!string.IsNullOrEmpty(mainModelFile))
                            {
                                string[] skillInfo = GetSkillInfo(mainModelFile);

                                if (skillInfo != null)
                                {
                                    npcForce = skillInfo[0];
                                    forceDetail = skillInfo[1];
                                }
                                else // 填空，按第二技能ID来找
                                {
                                    npcForce = "空";
                                    forceDetail = "空";
                                }

                                if (npcRepresentID != "" && npcRepresentID != "0")
                                {
                                    updateSuccess = AutoCreateSkills(false);
                                }
                                else
                                {
                                    logText.AppendLine("Error：RepresentID1为空！");
                                }                                
                            }
                            else
                            {
                                logText.AppendLine("Error：MainModelFile字段为空！");
                            } 
                        }
                        else
                        {
                            logText.AppendLine("Error：无效的表现ID！");
                        }
                    }
                    
                    if (updateSuccess)
                    {
                        updateSuccessNpcList.Add(string.Format("{0} ({1})", npcName, npcID));
                    }
                    else
                    {
                        updateFailNpcList.Add(string.Format("{0} ({1})", npcName, npcID));
                    }
                }

                // 更新记录
                try
                {
                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    SqlCommand cmd = conn.CreateCommand();

                    foreach(string s in updateSuccessNpcList)
                    {
                        npcID = GetNpcID(s).ToString();

                        sqlString = string.Format("SELECT ID FROM npc_skill_record WHERE ID = {0}", npcID);
                        cmd.CommandText = sqlString;

                        if (cmd.ExecuteScalar() == null) // 插入
                        {
                            sqlString = string.Format("INSERT INTO npc_skill_record (ID, UpdateSuccess) VALUES ({0}, 1)", npcID);
                        }
                        else // 更新
                        {
                            sqlString = string.Format("UPDATE npc_skill_record SET UpdateSuccess = 1 WHERE ID = {0}", npcID);
                        }
                        
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("更新NPC技能记录时产生sql异常：" + ex.Message, "更新技能记录",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }

                // 显示更新结果
                UpdateResultForm uForm = new UpdateResultForm(updateSuccessNpcList, updateFailNpcList);
                uForm.LogText = logText.ToString();
                uForm.Init();
                uForm.ShowDialog();
            }
        }
       
        /// <summary>
        /// 获取技能信息
        /// </summary>
        /// <param name="mainModelFile">模型文件路径</param>
        /// <returns>技能信息</returns>
        private string[] GetSkillInfo(string mainModelFile)
        {
            string[] skillInfo = null;

            DataRow[] rows = modelTable.Select(string.Format("SourceID = '{0}'", mainModelFile));
            if (rows.Length > 0)
            {
                DataRow row = rows[0];
                string force = row["Force"].ToString();
                string forceDetail = row["ForceDetail"].ToString();                

                if (force != "" && forceDetail != "")
                {
                    // 拆分forceDetail
                    string[] tempArray = forceDetail.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    forceDetail = tempArray[0];

                    skillInfo = new string[] { force, forceDetail };
                }
            }

            return skillInfo;
        }

        /// <summary>
        /// 从NPC描述字符串里取到NPC的ID
        /// </summary>
        /// <param name="npcString">NPC描述字符串</param>
        /// <return>NPC的ID</return>
        private int GetNpcID(string npcString)
        {
            int npcID = -1;

            int firstIndex = npcString.LastIndexOf("(");
            int lastIndex = npcString.LastIndexOf(")");

            if (firstIndex >= 0 && lastIndex >= 0)
            {
                npcID = int.Parse(npcString.Substring(firstIndex + 1, lastIndex - firstIndex - 1));
            }

            return npcID;
        }

        /// <summary>
        /// 从NPC描述字符串里取到NPC的名称
        /// </summary>
        /// <param name="npcString">NPC描述字符串</param>
        /// <returns>NPC的名称</returns>
        private string GetNpcName(string npcString)
        {
            string npcName = null;

            int index = npcString.LastIndexOf("(");

            if (index > 0)
            {
                npcName = npcString.Substring(0, index - 1);
            }

            return npcName;
        }

        /// <summary>
        /// 检查npc的技能是否已经被更新过
        /// </summary>
        /// <param name="npcID">npc的ID</param>
        /// <param name="recordTable"></param>
        /// <returns></returns>
        private bool CheckNpcUpdated(string npcID, DataTable recordTable)
        {
            bool updated = false;

            DataRow[] rows = recordTable.Select(string.Format("ID = '{0}'", npcID.ToString()));
            if (rows.Length > 0)
            {
                if (rows[0]["UpdateSuccess"].ToString() == "True")
                {
                    updated = true;
                }
            }

            return updated;
        }    
        
        /// <summary>
        /// 清掉npc的技能
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="skillID">技能ID</param>
        /// <param name="skillLevel">技能等级</param>
        private void ClearNpcSkill(SqlCommand cmd, string skillID, string skillLevel)
        {
            string sqlString = string.Format("DELETE FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                             npcRepresentID, skillID, skillLevel);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();
        }

        /// <summary>
        /// 更新npc技能
        /// </summary>
        /// <param name="dataRow">数据行</param>
        /// <returns>是否更新成功</returns>
        private bool UpdateSkill(DataRow dataRow)
        {
            bool success = false;

            SqlTransaction transaction = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                // 先清掉当前填的所有技能，不清28号技能
                // string sqlString = string.Format("DELETE FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID <> 28", npcRepresentID.ToString());
                // cmd.CommandText = sqlString;
                // cmd.ExecuteNonQuery();

                // 清掉NPC的所有技能ID
                // sqlString = string.Format("UPDATE NpcTemplate SET SkillID2 = 0, SkillID3 = 0, SkillID4 = 0 WHERE ID = '{0}'", npcTemplateID);
                // cmd.CommandText = sqlString;
                // cmd.ExecuteNonQuery();                

                // 更新技能2
                string animationName = dataRow["动作分类2"].ToString();
                string skillID = dataRow["skillid2"].ToString();
                string skillType = dataRow["skilltype2"].ToString();
                string skillInterval = dataRow["skillinterval2"].ToString();
                string skillRate = dataRow["skillrate2"].ToString();
                string hitEffectID = dataRow["HitEffectResultID2"].ToString();
                string animationFile = null;                
                int animationTime = 0;
                string animationID = GetAnimationID(animationName);                
                DataRow[] rows = null;
                DataRow row = null;
                string sqlString = null;

                rows = npcGlobeTable.Select(string.Format("ID = '{0}'", npcTemplateID));
                row = rows[0];
                string npcSkillID2 = row["SkillID2"].ToString();
                string npcSkillLevel2 = row["SkillLevel2"].ToString();
                string npcSkillID3 = row["SkillID3"].ToString();
                string npcSkillLevel3 = row["SkillLevel3"].ToString();
                string npcSkillID4 = row["SkillID4"].ToString();
                string npcSkillLevel4 = row["SkillLevel4"].ToString();

                if (animationID != "0")
                {
                    // 自动清掉npc的当前对应技能
                    if (npcSkillID2 != "" && npcSkillID2 != "0")
                    {
                        ClearNpcSkill(cmd, npcSkillID2, npcSkillLevel2.ToString());
                    }                    

                    // 自动补全技能表
                    sqlString = string.Format("SELECT RepresentID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                              npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    cmd.CommandText = sqlString;

                    if (cmd.ExecuteScalar() == null) // 需要自动补全
                    {
                        sqlString = string.Format("INSERT INTO dic_npc_skill_caster_npc (RepresentID, SkillID, SkillLevel) VALUES ('{0}', '{1}', '{2}')",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        logText.AppendLine(string.Format("添加ID为{0}的技能动作", skillID));
                    }

                    if (animationName == "聚力") // 当这列填为聚力时。将69填入PrepareCastSkillAnimationID字段，将68填入是CastSkillAnimationID0字段。
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = '68', PrepareCastSkillAnimationID = '69' WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = {0} WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  animationID, npcRepresentID.ToString(), skillID, skillLevel.ToString());                        
                    }

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (hitEffectID != "") // 填上打击特效
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET HitEffectResultID = '{0}' WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  hitEffectID, npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillID2 = '{0}' WHERE ID = '{1}'", skillID, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillType2 = '{0}' WHERE ID = '{1}'", skillType, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillInterval2 = '{0}' WHERE ID = '{1}'", skillInterval, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRate2 = '{0}' WHERE ID = '{1}'", skillRate, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillLevel2 = '{0}' WHERE ID = '{1}'", skillLevel.ToString(), npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRestFrame2 = 8 WHERE ID = '{0}'", npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (animationName == "聚力")
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = 69", npcRepresentID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame2 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                    else
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = '{1}'", npcRepresentID, animationID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame2 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                }
                else
                {
                    logText.AppendLine("动作分类2为空...");
                }

                // 更新技能3
                animationName = dataRow["动作分类3"].ToString();
                skillID = dataRow["skillid3"].ToString();
                skillType = dataRow["skilltype3"].ToString();
                skillInterval = dataRow["skillinterval3"].ToString();
                skillRate = dataRow["skillrate3"].ToString();
                hitEffectID = dataRow["HitEffectResultID3"].ToString();
                animationID = GetAnimationID(animationName);

                if (animationID != "0")
                {
                    // 自动清掉npc的当前对应技能
                    if (npcSkillID3 != "" && npcSkillID3 != "0")
                    {
                        ClearNpcSkill(cmd, npcSkillID3, npcSkillLevel3.ToString());
                    } 

                    // 自动补全技能表
                    sqlString = string.Format("SELECT RepresentID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                              npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    cmd.CommandText = sqlString;

                    if (cmd.ExecuteScalar() == null) // 需要自动补全
                    {
                        sqlString = string.Format("INSERT INTO dic_npc_skill_caster_npc (RepresentID, SkillID, SkillLevel) VALUES ('{0}', '{1}', '{2}')",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        logText.AppendLine(string.Format("添加ID为{0}的技能动作", skillID));
                    }

                    if (animationName == "聚力") // 当这列填为聚力时。将69填入PrepareCastSkillAnimationID字段，将68填入是CastSkillAnimationID0字段。
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = '68', PrepareCastSkillAnimationID = '69' WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = {0} WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  animationID, npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    }                    

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (hitEffectID != "") // 填上打击特效
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET HitEffectResultID = '{0}' WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  hitEffectID, npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillID3 = '{0}' WHERE ID = '{1}'", skillID, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillType3 = '{0}' WHERE ID = '{1}'", skillType, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillInterval3 = '{0}' WHERE ID = '{1}'", skillInterval, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRate3 = '{0}' WHERE ID = '{1}'", skillRate, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillLevel3 = '{0}' WHERE ID = '{1}'", skillLevel.ToString(), npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRestFrame3 = 8 WHERE ID = '{0}'", npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (animationName == "聚力")
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = 69", npcRepresentID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame3 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                    else
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = '{1}'", npcRepresentID, animationID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame3 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                }
                else
                {
                    logText.AppendLine("动作分类3为空...");
                }

                // 更新技能4
                animationName = dataRow["动作分类4"].ToString();
                skillID = dataRow["skillid4"].ToString();
                skillType = dataRow["skilltype4"].ToString();
                skillInterval = dataRow["skillinterval4"].ToString();
                skillRate = dataRow["skillrate4"].ToString();
                hitEffectID = dataRow["HitEffectResultID4"].ToString();
                animationID = GetAnimationID(animationName);

                if (animationID != "0")
                {
                    // 自动清掉npc的当前对应技能
                    if (npcSkillID4 != "" && npcSkillID4 != "0")
                    {
                        ClearNpcSkill(cmd, npcSkillID4, npcSkillLevel4.ToString());
                    } 

                    // 自动补全技能表
                    sqlString = string.Format("SELECT RepresentID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                              npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    cmd.CommandText = sqlString;

                    if (cmd.ExecuteScalar() == null) // 需要自动补全
                    {
                        sqlString = string.Format("INSERT INTO dic_npc_skill_caster_npc (RepresentID, SkillID, SkillLevel) VALUES ('{0}', '{1}', '{2}')",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        logText.AppendLine(string.Format("添加ID为{0}的技能动作", skillID));
                    }

                    if (animationName == "聚力") // 当这列填为聚力时。将69填入PrepareCastSkillAnimationID字段，将68填入是CastSkillAnimationID0字段。
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = '68', PrepareCastSkillAnimationID = '69' WHERE RepresentID = '{0}' AND SkillID = '{1}' AND SkillLevel = '{2}'",
                                                  npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = {0} WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  animationID, npcRepresentID.ToString(), skillID, skillLevel.ToString());
                    }

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (hitEffectID != "") // 填上打击特效
                    {
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET HitEffectResultID = '{0}' WHERE RepresentID = '{1}' AND SkillID = '{2}' AND SkillLevel = '{3}'",
                                                  hitEffectID, npcRepresentID.ToString(), skillID, skillLevel.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillID4 = '{0}' WHERE ID = '{1}'", skillID, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillType4 = '{0}' WHERE ID = '{1}'", skillType, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillInterval4 = '{0}' WHERE ID = '{1}'", skillInterval, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRate4 = '{0}' WHERE ID = '{1}'", skillRate, npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillLevel4 = '{0}' WHERE ID = '{1}'", skillLevel.ToString(), npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE NpcTemplate SET SkillRestFrame4 = 8 WHERE ID = '{0}'", npcTemplateID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    if (animationName == "聚力")
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = 69", npcRepresentID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame4 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                    else
                    {
                        rows = npcAnimationTable.Select(string.Format("RepresentID = '{0}' AND AnimationID = '{1}'", npcRepresentID, animationID));
                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            animationFile = row["AnimationFile"].ToString();
                            if (!string.IsNullOrEmpty(animationFile))
                            {
                                animationTime = GetAnimationTime(animationFile);

                                if (animationTime > 0)
                                {
                                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame4 = '{0}' WHERE ID = '{1}'", animationTime.ToString(), npcTemplateID);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    logText.AppendLine(string.Format("Error：无法获取ID为{0}的动作的播放时间！", animationID));
                                }
                            }
                            else
                            {
                                logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作的文件！", animationID));
                            }
                        }
                        else
                        {
                            logText.AppendLine(string.Format("Error：没有找到ID为{0}的动作！", animationID));
                        }
                    }
                }
                else
                {
                    logText.AppendLine("动作分类4为空...");
                }

                transaction.Commit();
                success = true;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在更新NPC技能时产生sql异常：" + ex.Message, "更新技能",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                if (transaction != null)
                {
                    transaction.Rollback();
                }
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return success;
        }

        /// <summary>
        /// 获得动作的播放时间
        /// </summary>
        /// <returns>动作的播放时间</returns>
        private int GetAnimationTime(string animationString)
        {
            int animationTime = 0;

            AnimationHelper animationHelper = AnimationHelper.GetAnimationHelper();
            animationHelper.RootPath = rootDir;
            animationTime = animationHelper.GetAniFrameCount(animationString);

            return animationTime;
        }

        /// <summary>
        /// 获取MainModelFile字段值
        /// </summary>
        /// <param name="representID">表现ID</param>
        /// <returns>MainModelFile字段值</returns>
        private string GetMainModelFile(string representID)
        {
            string mainModelFile = "";

            DataRow[] rows = npcRepresentTable.Select(string.Format("RepresentID = {0}", representID));
            if (rows.Length > 0)
            {
                mainModelFile = rows[0]["MainModelFile"].ToString();
            }

            return mainModelFile;
        }

        /// <summary>
        /// 获取EditorID字段值
        /// </summary>
        /// <param name="representID">表现ID</param>
        /// <returns>EditorID字段值</returns>
        private string GetEditorID(string representID)
        {
            string editorID = "";

            DataRow[] rows = npcRepresentTable.Select(string.Format("RepresentID = {0}", representID));
            if (rows.Length > 0)
            {
                editorID = rows[0]["EditorID"].ToString();
            }

            return editorID;
        }

        /// <summary>
        /// 获取技能等级
        /// </summary>
        /// <param name="npcLevel">npc等级</param>
        /// <returns>技能等级</returns>
        private int GetSkillLevel(int npcLevel)
        {
            int skillLevel = (int)((npcLevel + 5) / 5);

            return skillLevel;
        }

        /// <summary>
        /// 用SourceID拼出脸部Mesh的相对路径
        /// </summary>
        /// <param name="sourceID">SourceID</param>
        /// <param name="fieldValue">字段值</param>
        /// <returns>脸部Mesh的相对路径</returns>
        private string GetFaceMeshFile(string sourceID, string fieldValue)
        {
            StringBuilder meshFile = new StringBuilder();
            string[] tempArray = sourceID.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);

            if (sourceID.StartsWith(@"data\source\NPC_source")) // npc的模型
            {
                for (int i = 0; i < tempArray.Length - 1; i++)
                {
                    meshFile.Append(string.Format("{0}\\", tempArray[i]));
                }
                meshFile.Append(string.Format("{0}.mesh", fieldValue));
            }
            else // player的模型
            {
                for (int i = 0; i < tempArray.Length - 2; i++)
                {
                    meshFile.Append(string.Format("{0}\\", tempArray[i]));
                }
                meshFile.Append(string.Format("{0}.mesh", fieldValue));
            }

            return meshFile.ToString();
        }

        /// <summary>
        /// 用SourceID拼出脸部Material的相对路径
        /// </summary>
        /// <param name="sourceID">SourceID</param>
        /// <param name="fieldValue">字段值</param>
        /// <returns>脸部Material的相对路径</returns>
        private string GetFaceMaterialFile(string sourceID, string fieldValue)
        {
            StringBuilder materialFile = new StringBuilder();
            string[] tempArray = sourceID.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);

            if (sourceID.StartsWith(@"data\source\NPC_source")) // npc的模型
            {
                for (int i = 0; i < tempArray.Length - 1; i++)
                {
                    materialFile.Append(string.Format("{0}\\", tempArray[i]));
                }
                materialFile.Append(string.Format("{0}.mtl", fieldValue));
            }
            else // player的模型
            {
                for (int i = 0; i < tempArray.Length - 2; i++)
                {
                    materialFile.Append(string.Format("{0}\\", tempArray[i]));
                }
                materialFile.Append(string.Format("{0}.mtl", fieldValue));
            }            

            return materialFile.ToString();
        }

        /// <summary>
        /// 更新插槽信息
        /// </summary>
        /// <param name="dataRow">数据行</param>
        /// <returns>是否更新成功</returns>
        private bool UpdateSlot(DataRow dataRow)
        {
            bool editSuccess = false;
            SqlTransaction transaction = null;
            faceMeshFile = null;
            faceMaterialFile = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;
                string sqlString = null;

                string sourceID = dataRow["SourceID"].ToString();
                string face = dataRow["Face"].ToString();

                if (helper.CheckStringValid(face) && helper.CheckStringValid(sourceID))
                {
                    string[] tempArray = face.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    if (tempArray.Length > 1)
                    {                        
                        randomIndex = random.Next(tempArray.Length);
                        faceMeshFile = GetFaceMeshFile(sourceID, tempArray[randomIndex]);                       
                    }
                    else
                    {
                        faceMeshFile = GetFaceMeshFile(sourceID, tempArray[0]);
                    }

                    sqlString = string.Format("UPDATE npc SET S_Face_MeshFile = '{0}' WHERE RepresentID = '{1}'", faceMeshFile, npcRepresentID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
                else
                {
                    sqlString = string.Format("UPDATE npc SET S_Face_MeshFile = NULL WHERE RepresentID = '{0}'", npcRepresentID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                string faceMaterial = dataRow["FaceTexture"].ToString();

                if (helper.CheckStringValid(faceMaterial))
                {
                    faceMaterialFile = GetFaceMaterialFile(sourceID, faceMaterial);
                    sqlString = string.Format("UPDATE npc SET S_Face_MaterialFile = '{0}' WHERE RepresentID = '{1}'", faceMaterialFile, npcRepresentID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
                else
                {
                    sqlString = string.Format("UPDATE npc SET S_Face_MaterialFile = NULL WHERE RepresentID = '{0}'", npcRepresentID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                string specialTrinket1 = dataRow["SpecialTrinket1"].ToString();
                string insertPosition1 = dataRow["InsertPosition1"].ToString();

                if (helper.CheckStringValid(insertPosition1))
                {
                    if (helper.CheckStringValid(specialTrinket1))
                    {
                        sqlString = string.Format("UPDATE npc SET {0}_MeshFile = '{1}' WHERE RepresentID = '{2}'", insertPosition1, specialTrinket1, npcRepresentID);
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE npc SET {0}_MeshFile = NULL WHERE RepresentID = '{1}'", insertPosition1, npcRepresentID);                        
                    }

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }                

                string specialTrinket2 = dataRow["SpecialTrinket2"].ToString();
                string insertPosition2 = dataRow["InsertPosition2"].ToString();

                if (helper.CheckStringValid(insertPosition2))
                {
                    if (helper.CheckStringValid(specialTrinket2))
                    {
                        sqlString = string.Format("UPDATE npc SET {0}_MeshFile = '{1}' WHERE RepresentID = '{2}'", insertPosition2, specialTrinket2, npcRepresentID);
                    }
                    else
                    {
                        sqlString = string.Format("UPDATE npc SET {0}_MeshFile = NULL WHERE RepresentID = '{1}'", insertPosition2, npcRepresentID);
                    }

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }                

                // 更新模型缩放值
                string modelScale = dataRow["ModelScale"].ToString();

                if (helper.CheckStringValid(modelScale))
                {
                    sqlString = string.Format("UPDATE npc SET ModelScale = '{0}' WHERE RepresentID = '{1}'", modelScale, npcRepresentID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                transaction.Commit();
                editSuccess = true;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("更新插槽数据时产生sql异常：" + ex.Message, "更新插槽数据",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                if (transaction != null)
                {
                    transaction.Rollback();
                }
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }            

            return editSuccess;
        }

        /// <summary>
        /// 编辑npc编号
        /// </summary>
        /// <returns>是否处理成功</returns>
        public bool EditSeriesID()
        {
            bool editSuccess = false;
            SourceIDSelectForm sForm = new SourceIDSelectForm(conn, modelTable, npcForceSkillTable);

            if (sForm.ShowDialog() == DialogResult.OK)
            {
                updateEditorID = sForm.EditorID;
                updateNpcForce = sForm.NpcForce;
                updateForceDetail = sForm.ForceDetail;
                updateModelFile = sForm.SourceID;
                updateColorChannel = sForm.ColorChannel;
                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 编辑npc编号
        /// </summary>
        /// <param name="seriesID">npc序列ID</param>
        /// <returns>是否处理成功</returns>
        public bool EditSeriesIDWithoutMessage(string seriesID)
        {
            bool editSuccess = false;

            if (!string.IsNullOrEmpty(seriesID))
            {
                DataRow[] rows = modelTable.Select(string.Format("EditorID = '{0}'", seriesID));
                if (rows.Length > 0)
                {
                    DataRow row = rows[0];
                    editSuccess = UpdateSlot(row);
                }
            }

            return editSuccess;
        }

        /// <summary>
        /// 编辑npc编号
        /// </summary>
        /// <param name="seriesID">npc序列ID</param>
        /// <returns>是否处理成功</returns>
        public bool EditSeriesID(string seriesID)
        {
            bool editSuccess = false;
            DialogResult result = MessageBox.Show("是否更新NPC脸部插槽及挂件信息？", "更新插槽", MessageBoxButtons.OKCancel, MessageBoxIcon.Information);

            if (result == DialogResult.OK)
            {                
                logText = new StringBuilder();

                if (!string.IsNullOrEmpty(seriesID))
                {
                    DataRow[] rows = modelTable.Select(string.Format("EditorID = '{0}'", seriesID));
                    if (rows.Length > 0)
                    {
                        DataRow row = rows[0];
                        editSuccess = UpdateSlot(row);
                    }
                    else
                    {
                        logText.AppendLine("Error：没有找到匹配的数据！");
                    }
                }
                else
                {
                    logText.AppendLine("Error：NPC编号不能为空！");
                }

                if (editSuccess)
                {
                    MessageBox.Show("插槽更新成功！", "更新插槽", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show(string.Format("插槽更新失败，原因如下：\r\n\r\n{0}", logText.ToString()), "更新插槽",
                                                  MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                editSuccess = false;
            }            

            return editSuccess;
        }

        /// <summary>
        /// 编辑npc表现处理
        /// </summary>
        /// <returns>是否处理成功</returns>
        public bool EditWithoutMessage()
        {
            bool editSuccess = false;
            logText = new StringBuilder();

            string sqlString = string.Format("SELECT SkillID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}'", npcRepresentID);
            DataTable skillTable = helper.GetDataTable(conn, sqlString);

            if (skillTable.Rows.Count > 0)
            {
                currentSkillID2 = skillTable.Rows[0]["SkillID"].ToString();
            }
            else
            {
                currentSkillID2 = null;
            }

            editSuccess = AutoCreateSkills(false);

            return editSuccess;
        }

        /// <summary>
        /// 编辑npc表现处理
        /// </summary>
        /// <returns>是否处理成功</returns>
        public bool Edit()
        {
            bool editSuccess = false;
            logText = new StringBuilder();

            string sqlString = string.Format("SELECT SkillID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}'", npcRepresentID);
            DataTable skillTable = helper.GetDataTable(conn, sqlString);

            if (skillTable.Rows.Count > 0)
            {
                currentSkillID2 = skillTable.Rows[0]["SkillID"].ToString();
            }
            else
            {
                currentSkillID2 = null;
            }

            switch (userSection)
            {
                case UserSection.None: // 未保存选择
                    {
                        SkillNotifyForm sForm = new SkillNotifyForm();
                        DialogResult dialogResult = sForm.ShowDialog();

                        switch (dialogResult)
                        {
                            case DialogResult.OK:
                                {
                                    editSuccess = AutoCreateSkills(true);

                                    if (sForm.RecordSelection)
                                    {
                                        userSection = UserSection.AutoCreate;       
                                    }

                                    break;
                                }
                            case DialogResult.Cancel:
                                {
                                    if (sForm.RecordSelection)
                                    {
                                        userSection = UserSection.NotCreate;
                                    }

                                    break;
                                }
                        }

                        break;
                    }
                case UserSection.AutoCreate:
                    {
                        editSuccess = AutoCreateSkills(true);

                        break;
                    }
                case UserSection.NotCreate:
                    {
                        break;
                    }
            }

            return editSuccess;
        }
    }
}
