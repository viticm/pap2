using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;
using System.Data;
using System.Collections;
using System.Reflection;

namespace StandardNpcCreate
{
    public class Program
    {
        private SqlConnection conn; // sql连接
        private string npcModel; // npc模型
        private string leftHandMesh; // 左手骨骼
        private string rightHandMesh; // 右手骨骼
        private string faceMesh; // 骨骼
        private string hatMesh; // 骨骼
        private string lhMesh; // 骨骼
        private string lpMesh; // 骨骼
        private string lcMesh; // 骨骼
        private string rhMesh; // 骨骼
        private string rpMesh; // 骨骼
        private string rcMesh; // 骨骼
        private string longMesh; // 骨骼
        private string spineMesh; // 骨骼
        private string spine2Mesh; // 骨骼
        private string leftHandMaterial; // 左手武器材质
        private string rightHandMaterial; // 右手武器材质
        private string faceMaterial; // 材质
        private string hatMaterial; // 材质
        private string lhMaterial; // 材质
        private string lpMaterial; // 材质
        private string lcMaterial; // 材质
        private string rhMaterial; // 材质
        private string rpMaterial; // 材质
        private string rcMaterial; // 材质
        private string longMaterial; // 材质
        private string spineMaterial; // 材质
        private string spine2Material; // 材质
        private int npcTemplateID = 0; // npc模板ID
        private int npcRepresentID = 0; // npc表现ID
        private string mapName; // npc所属场景名
        private string longRangeWeaponType = "暗器"; // 远程武器类型
        private string modelForce; // 模型所属势力
        
        private DataTable boneToActionTable; // 数据表
        private DataTable itemForInsertTable; // 数据表
        private DataTable modelTable; // 数据表     
        private DataTable npcGlobeTable; // npc全局表
        private bool updateExistAnimation; // 是否更新已有动作
        private string relevatePath; // 相对路径
        private Random random = new Random(); // 随机数对象
        private StringBuilder logText = new StringBuilder(); // 日志文本
        private string rootDir; // 根目录
        private float modelScale = 1; // 模型缩放比例
        private List<int> updateSuccessIDList = new List<int>(); // 更新成功ID链表

        private string npcDisplayName; // npc描述名称
        private Type boolType;
        Helper helper = Helper.GetHelper();

        enum UserSection {None, AutoCreate, NotCreate}; // 用户选择枚举
        UserSection userSection = UserSection.None; // 当前用户选择 

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public Program(SqlConnection conn)
        {
            this.conn = conn;
        }        

        /// <summary>
        /// 模型所属势力
        /// </summary>
        public string ModelForce
        {
            set
            {
                modelForce = value;
            }

            get
            {
                return modelForce;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string Spine2Material
        {
            set
            {
                spine2Material = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string SpineMaterial
        {
            set
            {
                spineMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string LongMaterial
        {
            set
            {
                longMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string RCMaterial
        {
            set
            {
                rcMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string RPMaterial
        {
            set
            {
                rpMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string RHMaterial
        {
            set
            {
                rhMaterial = value;
                rightHandMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string LCMaterial
        {
            set
            {
                lcMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string LPMaterial
        {
            set
            {
                lpMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string LHMaterial
        {
            set
            {
                lhMaterial = value;
                leftHandMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string HatMaterial
        {
            set
            {
                hatMaterial = value;
            }
        }

        /// <summary>
        /// 材质
        /// </summary>
        public string FaceMaterial
        {
            set
            {
                faceMaterial = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string Spine2Mesh
        {
            set
            {
                spine2Mesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string SpineMesh
        {
            set
            {
                spineMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string LongMesh
        {
            set
            {
                longMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string RCMesh
        {
            set
            {
                rcMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string RPMesh
        {
            set
            {
                rpMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string RHMesh
        {
            set
            {
                rhMesh = value;
                rightHandMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string LCMesh
        {
            set
            {
                lcMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string LPMesh
        {
            set
            {
                lpMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string LHMesh
        {
            set
            {
                lhMesh = value;
                leftHandMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string HatMesh
        {
            set
            {
                hatMesh = value;
            }
        }

        /// <summary>
        /// 骨骼
        /// </summary>
        public string FaceMesh
        {
            set
            {
                faceMesh = value;
            }
        }

        /// <summary>
        /// 远程武器类型
        /// </summary>
        public string LongRangeWeaponType
        {
            set
            {
                longRangeWeaponType = value;
            }
        }

        /// <summary>
        /// npc所属场景名
        /// </summary>
        public string MapName
        {
            set
            {
                mapName = value;
            }
        }

        /// <summary>
        /// npc模板ID
        /// </summary>
        public int NpcTemplateID
        {
            set
            {
                npcTemplateID = value;
            }
        }

        /// <summary>
        /// npc表现ID
        /// </summary>
        public int NpcRepresentID
        {
            set
            {
                npcRepresentID = value;
            }
        }

        /// <summary>
        /// npc模型
        /// </summary>
        public string NpcModel
        {
            set
            {
                npcModel = value;
            }
        }

        /// <summary>
        /// 根目录
        /// </summary>
        public string RootDir
        {
            set
            {
                rootDir = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {            
            // 初始化数据
            bool tempBool = true;
            boolType = tempBool.GetType();

            // 读取文件内容
            string filePath = Path.Combine(Application.StartupPath, @"Design\BoneToAction.tab");
            string content = helper.GetFileContent(filePath);   
         
            if (content != null)
            {
                boneToActionTable = helper.CreateDataTable(content);
            }

            filePath = Path.Combine(Application.StartupPath, @"Design\ItemForInsert.tab");
            content = helper.GetFileContent(filePath);

            if (content != null)
            {
                itemForInsertTable = helper.CreateDataTable(content);
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
        /// 批量复制NPC
        /// </summary>
        /// <param name="baseTree">当前分类树</param>
        public void MultiCopyNPC(TreeView baseTree)
        {            
            TreeNode treeNode = baseTree.SelectedNode;

            if (treeNode != null && treeNode.Level == 1)
            {
                treeNode = treeNode.Parent;
            }

            if (treeNode != null)
            {
                string sourceMapName = treeNode.Text;
                string destinationMapName = string.Format("{0}副本", treeNode.Text);
                List<string> idList = new List<string>();
                List<string> displayTextList = new List<string>();
                string sqlString = string.Format("SELECT * FROM NpcTemplate WHERE MapName = '{0}'", sourceMapName);
                DataTable npcTable = helper.GetDataTable(conn, sqlString);
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

                    sqlString = "SELECT MAX(ID) FROM NpcTemplate";
                    cmd.CommandText = sqlString;
                    int maxID = (int)cmd.ExecuteScalar() + 1;

                    foreach (DataRow row in npcTable.Rows)
                    {
                        string npcName = row["Name"].ToString();

                        // 先插入数据行
                        sqlString = string.Format("INSERT INTO NpcTemplate (ID) VALUES ('{0}')", maxID.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();

                        foreach (DataColumn column in npcTable.Columns)
                        {
                            string data = row[column.ColumnName].ToString();

                            switch (column.ColumnName)
                            {
                                case "ID":
                                    {
                                        break;
                                    }
                                case "MapName":
                                    {
                                        sqlString = string.Format("UPDATE NpcTemplate SET MapName = '{0}' WHERE ID = '{1}'",
                                                                  destinationMapName, maxID.ToString());
                                        cmd.CommandText = sqlString;
                                        cmd.ExecuteNonQuery();

                                        break;
                                    }
                                default:
                                    {
                                        if (data != "")
                                        {
                                            if (column.DataType == boolType) // 布尔类型需要特殊处理下
                                            {
                                                switch (data)
                                                {
                                                    case "True":
                                                        {
                                                            data = "1";
                                                            break;
                                                        }
                                                    case "False":
                                                        {
                                                            data = "0";
                                                            break;
                                                        }
                                                }
                                            }

                                            sqlString = string.Format("UPDATE NpcTemplate SET [{0}] = '{1}' WHERE ID = '{2}'", 
                                                                      column.ColumnName, data, maxID.ToString());
                                            cmd.CommandText = sqlString;
                                            cmd.ExecuteNonQuery();
                                        }

                                        break;
                                    }
                            }                            
                        }

                        idList.Add(maxID.ToString());
                        displayTextList.Add(string.Format("{0}({1})", npcName, maxID.ToString()));
                        maxID++;
                    }

                    transaction.Commit();

                    // 创建对应的树结点
                    TreeNode parentNode = new TreeNode(destinationMapName);
                    parentNode.Tag = destinationMapName;
                    baseTree.Nodes.Add(parentNode);

                    for (int i = 0; i < idList.Count; i++)
                    {
                        TreeNode newNode = new TreeNode(displayTextList[i]);
                        newNode.Tag = new object[] { int.Parse(idList[i]) };
                        parentNode.Nodes.Add(newNode);
                    }

                    baseTree.SelectedNode = parentNode;
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("在复制NPC数据的时候产生sql异常：" + ex.Message, "批量复制NPC", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);

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
            }                       
        }

        /// <summary>
        /// 重新刷新一次数据表
        /// </summary>
        public void ReloadDataTable()
        {
            string sqlString = "SELECT ID, Name, ForceID, RepresentID1, SkillID1, SkillID2, SkillID3, SkillID4, MapName FROM NpcTemplate";
            npcGlobeTable = helper.GetDataTable(conn, sqlString);
        }

        /// <summary>
        /// 编辑npc表现处理
        /// </summary>
        /// <returns>是否处理成功</returns>
        public bool EditWithoutMessage()
        {
            bool editSuccess = false;

            InitModelScale();
            editSuccess = AutoCreateActions(false, updateExistAnimation, true);

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

            switch (userSection)
            {
                case UserSection.None: // 未保存选择
                    {
                        NotifyForm nForm = new NotifyForm();
                        DialogResult dialogResult = nForm.ShowDialog();

                        switch (dialogResult)
                        {
                            case DialogResult.OK:
                                {
                                    InitModelScale();
                                    editSuccess = AutoCreateActions(true, nForm.UpdateExistAnimation, true);                                    

                                    if (nForm.RecordSelection)
                                    {
                                        userSection = UserSection.AutoCreate;
                                        updateExistAnimation = nForm.UpdateExistAnimation;
                                    }

                                    break;
                                }
                            case DialogResult.Cancel:
                                {
                                    if (nForm.RecordSelection)
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
                        InitModelScale();
                        editSuccess = AutoCreateActions(true, updateExistAnimation, true);                        

                        break;
                    }
                case UserSection.NotCreate:
                    {
                        break;
                    }
            }           

            return editSuccess;
        }

        /// <summary>
        /// 更新动作信息补丁
        /// </summary>
        /// <param name="cmd">sql命令</param>
        private void UpdateAnimationPlusInfo(SqlCommand cmd, string animationID)
        {
            // AttackMove，IsAnimationShockWave，IsSFXShockWave，三个字段如果为空，则填为0
            string sqlString = string.Format("UPDATE animation_npc SET AttackMove = '0' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                             npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            sqlString = string.Format("UPDATE animation_npc SET IsAnimationShockWave = '0' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            sqlString = string.Format("UPDATE animation_npc SET IsSFXShockWave = '0' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            // SFXScale，SFXSpeed，SFXType，AnimationSpeed，四个字段如果是空和0，则全部改成1
            sqlString = string.Format("UPDATE animation_npc SET SFXScale = '1' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            sqlString = string.Format("UPDATE animation_npc SET SFXSpeed = '1' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            sqlString = string.Format("UPDATE animation_npc SET SFXType = '1' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            sqlString = string.Format("UPDATE animation_npc SET AnimationSpeed = '1' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();

            // IsLoadDefaultSocket， 始终填为1
            sqlString = string.Format("UPDATE animation_npc SET IsLoadDefaultSocket = '1' WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                      npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();
        }

        /// <summary>
        /// 初始化模型缩放比例
        /// </summary>        
        private void InitModelScale()
        {
            string sqlString = string.Format("SELECT ModelScale FROM npc WHERE RepresentID = '{0}'",
                                             npcRepresentID.ToString());
            DataTable dataTable = helper.GetDataTable(conn, sqlString);

            if (dataTable.Rows.Count > 0)
            {
                DataRow dataRow = dataTable.Rows[0];
                string modelScaleString = dataRow["ModelScale"].ToString();

                if (modelScaleString != "" && modelScaleString != "0")
                {
                    modelScale = float.Parse(modelScaleString);
                }
                else
                {
                    modelScale = 1;
                }
            }
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
        /// 更新多个NPC动作
        /// </summary>
        /// <param name="mapName">地图名</param>        
        /// <param name="templateID">模板ID</param>
        private void MultiUpdate(string mapName, int templateID)
        {
            List<string> selectedNpcList = new List<string>();            
            logText = new StringBuilder();
            this.mapName = mapName;
            updateSuccessIDList.Clear();

            string sqlString = "SELECT * FROM npc";
            DataTable representTable = helper.GetDataTable(conn, sqlString);

            DataTable npcTable = null;
            if (templateID == 0) // 更新多个npc
            {
                sqlString = string.Format("SELECT ID, Name, RepresentID1 FROM NpcTemplate WHERE MapName = '{0}'", mapName);
                npcTable = helper.GetDataTable(conn, sqlString);
            }
            else // 更新单个npc
            {
                sqlString = string.Format("SELECT ID, Name, RepresentID1 FROM NpcTemplate WHERE ID = '{0}'", templateID.ToString());
                npcTable = helper.GetDataTable(conn, sqlString);
            }            

            sqlString = string.Format("SELECT * FROM npc_animation_record WHERE MapName = '{0}'", mapName);
            DataTable npcRecordTable = helper.GetDataTable(conn, sqlString);

            Hashtable representID1Table = new Hashtable();

            foreach(DataRow dataRow in npcTable.Rows)
            {
                string representID1 = dataRow["RepresentID1"].ToString();
                string npcName = dataRow["Name"].ToString();
                string npcID = dataRow["ID"].ToString();
                representID1Table[npcID] = representID1;

                if (representID1 != "" && representID1 != "0")
                {
                    DataRow[] rows = npcRecordTable.Select(string.Format("ID = '{0}'", npcID));

                    if (rows.Length > 0)
                    {
                        DataRow row = rows[0];

                        if (row["IsSpecific"].ToString() == "True") // 是特殊NPC
                        {
                            selectedNpcList.Add(string.Format("{0} ({1})    [特殊]", npcName, npcID));
                        }
                        else
                        {
                            if (CheckAnimationIntegrity(representID1)) // 动作完整
                            {
                                selectedNpcList.Add(string.Format("{0} ({1})", npcName, npcID));
                            }
                            else
                            {
                                selectedNpcList.Add(string.Format("{0} ({1})    [动作不完整]", npcName, npcID));
                            } 
                        }
                    }
                    else
                    {
                        if (CheckAnimationIntegrity(representID1)) // 动作完整
                        {
                            selectedNpcList.Add(string.Format("{0} ({1})", npcName, npcID));
                        }
                        else
                        {
                            selectedNpcList.Add(string.Format("{0} ({1})    [动作不完整]", npcName, npcID));
                        }                        
                    }                   
                }                
            }

            MultiUpdateForm mForm = new MultiUpdateForm(selectedNpcList, conn, mapName);

            if (mForm.ShowDialog() == DialogResult.OK)
            {
                DataRow[] rows;
                DataRow row;   
                bool updateExistAnimation = mForm.UpdateExistAnimation;
                bool updateCommonStandbyAnimation = mForm.UpdateCommonStandbyAnimation;
                
                // 更新动作
                List<string> successNpcList = new List<string>();
                List<string> failNpcList = new List<string>();
                this.mapName = mapName;                

                foreach (string s in selectedNpcList)
                {
                    npcDisplayName = s;
                    int npcID = GetNpcID(npcDisplayName);
                    string representID1 = representID1Table[npcID.ToString()] as string;

                    if (!string.IsNullOrEmpty(representID1) && representID1 != "0")
                    {
                        rows = representTable.Select(string.Format("RepresentID = '{0}'", representID1));                    

                        if (rows.Length > 0)
                        {
                            row = rows[0];
                            this.npcModel = row["MainModelFile"].ToString();
                            this.leftHandMesh = row["S_LH_MeshFile"].ToString();
                            this.rightHandMesh = row["S_RH_MeshFile"].ToString();
                            this.faceMesh = row["S_Face_MeshFile"].ToString();
                            this.hatMesh = row["S_Hat_MeshFile"].ToString();
                            this.lhMesh = row["S_LH_MeshFile"].ToString();
                            this.lpMesh = row["S_LP_MeshFile"].ToString();
                            this.lcMesh = row["S_LC_MeshFile"].ToString();
                            this.rhMesh = row["S_RH_MeshFile"].ToString();
                            this.rpMesh = row["S_RP_MeshFile"].ToString();
                            this.rcMesh = row["S_RC_MeshFile"].ToString();
                            this.longMesh = row["S_Long_MeshFile"].ToString();
                            this.spineMesh = row["S_Spine_MeshFile"].ToString();
                            this.spine2Mesh = row["S_Spine2_MeshFile"].ToString();
                            this.leftHandMaterial = row["S_LH_MaterialFile"].ToString();
                            this.rightHandMaterial = row["S_RH_MaterialFile"].ToString();
                            this.faceMaterial = row["S_Face_MaterialFile"].ToString();
                            this.hatMaterial = row["S_Hat_MaterialFile"].ToString();
                            this.lhMaterial = row["S_LH_MaterialFile"].ToString();
                            this.lpMaterial = row["S_LP_MaterialFile"].ToString();
                            this.lcMaterial = row["S_LC_MaterialFile"].ToString();
                            this.rhMaterial = row["S_RH_MaterialFile"].ToString();
                            this.rpMaterial = row["S_RP_MaterialFile"].ToString();
                            this.rcMaterial = row["S_RC_MaterialFile"].ToString();
                            this.longMaterial = row["S_Long_MaterialFile"].ToString();
                            this.spineMaterial = row["S_Spine_MaterialFile"].ToString();
                            this.spine2Material = row["S_Spine2_MaterialFile"].ToString();
                            this.longRangeWeaponType = row["LongRangeWeaponType"].ToString();
                            this.npcRepresentID = int.Parse(representID1);                        
                            this.npcTemplateID = npcID;

                            // 替换数据
                            if (this.leftHandMesh == "")
                            {
                                this.leftHandMesh = row["LeftHandMesh"].ToString();

                                if (this.leftHandMaterial == "")
                                {
                                    this.leftHandMaterial = row["LeftHandMaterial"].ToString();
                                }
                            }

                            if (this.rightHandMesh == "")
                            {
                                this.rightHandMesh = row["RightHandMesh"].ToString();

                                if (this.rightHandMaterial == "")
                                {
                                    this.rightHandMaterial = row["RightHandMaterial"].ToString();
                                }
                            }

                            string modelScaleString = row["ModelScale"].ToString();
                            if (modelScaleString != "" && modelScaleString != "0")
                            {
                                modelScale = float.Parse(modelScaleString);
                            }
                            else
                            {
                                modelScale = 1;
                            }

                            bool success = AutoCreateActions(false, updateExistAnimation, updateCommonStandbyAnimation);

                            // 记录成功失败日志                                                                              
                            if (success)
                            {
                                successNpcList.Add(npcDisplayName);
                            }
                            else
                            {
                                failNpcList.Add(npcDisplayName);
                            }                                                
                        }
                    }                    
                }

                UpdateResultForm rForm = new UpdateResultForm(successNpcList, failNpcList);
                rForm.LogText = logText.ToString();
                rForm.Init();
                rForm.ShowDialog();
            }
        }

        /// <summary>
        /// 检查动作的完整性
        /// </summary>
        /// <param name="representID">表现ID</param>
        /// <returns>动作是否完整</returns>
        private bool CheckAnimationIntegrity(string representID)
        {
            DataRow[] rows;            
            string sqlString = string.Format("SELECT AnimationID FROM animation_npc WHERE RepresentID = '{0}'", representID);
            DataTable animationTable = helper.GetDataTable(conn, sqlString);

            List<string> conditionList = new List<string>(); // 检查条件链表
            conditionList.Add("AnimationID = 30 OR AnimationID = 31 OR AnimationID = 32 OR AnimationID = 33 OR AnimationID = 34 OR AnimationID = 35"); // 检查普通待机动作
            conditionList.Add("AnimationID = 9"); // 检查行走动作
            conditionList.Add("AnimationID = 16"); // 检查跑步动作
            conditionList.Add("AnimationID = 47"); // 检查战斗待机动作
            conditionList.Add("AnimationID = 73"); // 检查战斗被击动作
            conditionList.Add("AnimationID = 54 OR AnimationID = 55 OR AnimationID = 56 OR AnimationID = 57"); // 检查战斗攻击动作
            conditionList.Add("AnimationID = 64"); // 检查技能BUFF动作
            conditionList.Add("AnimationID = 65"); // 检查技能AOE动作
            conditionList.Add("AnimationID = 66"); // 检查战斗重击动作
            conditionList.Add("AnimationID = 67"); // 检查技能远程动作
            conditionList.Add("AnimationID = 1"); // 检查死亡动作
            conditionList.Add("AnimationID = 2"); // 检查被击倒动作
            conditionList.Add("AnimationID = 5"); // 检查起身动作
            conditionList.Add("AnimationID = 287"); // 检查滑步动作
            conditionList.Add("AnimationID = 250"); // 检查闪避动作
            conditionList.Add("AnimationID = 69"); // 检查聚力动作
            conditionList.Add("AnimationID = 68"); // 检查聚力释放动作
            conditionList.Add("AnimationID = 3"); // 检查击退动作
            conditionList.Add("AnimationID = 291"); // 检查倒地中动作
            conditionList.Add("AnimationID = 292"); // 检查倒地被击动作
            conditionList.Add("AnimationID = 5"); // 检查起身动作
            conditionList.Add("AnimationID = 80"); // 检查眩晕动作
            conditionList.Add("AnimationID = 84"); // 检查格挡动作
            conditionList.Add("AnimationID = 288"); // 检查滑步停止动作

            foreach (string condition in conditionList)
            {
                rows = animationTable.Select(condition);

                if (rows.Length == 0)
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 检查动作是否有效
        /// </summary>
        /// <param name="mapName">地图名</param>
        /// <param name="templateID">模板名</param>
        public void CheckAnimationValid(string mapName, int templateID)
        {
            string sqlString = "SELECT * FROM npc";
            DataTable representTable = helper.GetDataTable(conn, sqlString);

            sqlString = "SELECT * FROM animation_npc";
            DataTable animationTable = helper.GetDataTable(conn, sqlString);

            DataTable npcTable = null;
            
            if (templateID == 0) // 更检查多个npc
            {
                sqlString = string.Format("SELECT ID, Name, RepresentID1 FROM NpcTemplate WHERE MapName = '{0}'", mapName);
                npcTable = helper.GetDataTable(conn, sqlString);
            }
            else // 检查单个npc
            {
                sqlString = string.Format("SELECT ID, Name, RepresentID1 FROM NpcTemplate WHERE ID = '{0}'", templateID.ToString());
                npcTable = helper.GetDataTable(conn, sqlString);
            }                                    

            AnimationCheckForm aForm = new AnimationCheckForm(rootDir, boneToActionTable, itemForInsertTable, npcTable, representTable, animationTable);
            aForm.ShowDialog();
        }

        /// <summary>
        /// 获取打击效果ID
        /// </summary>
        /// <param name="itemNumber">ItemNumber</param>
        /// <returns>打击效果ID</returns>
        private string GetHitEffectResultID(string itemNumber)
        {
            string hitEffectResultID = "5000";

            DataRow[] rows = itemForInsertTable.Select(string.Format("ItemNumber = '{0}'", itemNumber));
            if (rows.Length > 0)
            {
                DataRow row = rows[0];
                string tempValue = row["EffectType"] as string;

                if (!string.IsNullOrEmpty(tempValue))
                {
                    switch(tempValue)
                    {
                        case "1": // 轻型利器（笔，匕首，手刀）
                            {
                                hitEffectResultID = "5010";
                                break;
                            }
                        case "2": // 利器（刀，剑，吴钩，斧头）
                            {
                                hitEffectResultID = "5015";
                                break;
                            }
                        case "3": // 重型利器（大刀，重剑，枪，大斧）
                            {
                                hitEffectResultID = "5020";
                                break;
                            }
                        case "4": // 钝器（棍，木剑，钳，小锤）
                            {
                                hitEffectResultID = "5025";
                                break;
                            }
                        case "5": // 重型钝器（柱子，大棍，铁锚，大锤）
                            {
                                hitEffectResultID = "5030";
                                break;
                            }
                        case "6": // 爪（拳套）
                            {
                                hitEffectResultID = "5005";
                                break;
                            }
                    }
                }
            }

            return hitEffectResultID;
        }

        /// <summary>
        /// 获取ItemForInsert.tab的Key值
        /// </summary>
        /// <param name="meshFile">mesh路径</param>
        /// <returns>ItemForInsert.tab的Key值</returns>
        private string GetItemNumber(string meshFile)
        {
            string itemNumber = null;

            string[] data = meshFile.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
            if (data.Length > 0)
            {
                string meshName = data[data.Length - 1];
                itemNumber = meshName.Replace(".Mesh", "").Replace(".mesh", "");                
            }

            return itemNumber;
        }

        /// <summary>
        /// 获取模型文件的类型
        /// </summary>
        /// <param name="modelPath">模型文件的路径</param>
        /// <returns>模型文件的类型</returns>
        private string GetModelType(string modelPath)
        {
            string modelType = null;

            string[] data = modelPath.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
            if (data.Length > 4)
            {
                string modelClass = data[2];
 
                switch(modelClass)
                {
                    case "player":
                        {
                            modelType = data[3];

                            break;
                        }
                    case "NPC_source":
                        {
                            modelType = data[3];

                            break;
                        }
                }                
            }

            return modelType;
        }    

        /// <summary>
        /// 根据优先级填充动作
        /// </summary>
        /// <param name="dataRow">数据行</param>
        /// <returns>数据表</returns>
        private Hashtable FillAnimationWithPriorty(DataRow dataRow)
        {
            Hashtable animationTable = new Hashtable();
            string[] tempArray;

            string walkAnimation = dataRow["Walk"] as string; // 行走
            string runAnimation = dataRow["Run"] as string; // 跑步
            string battleStandbyAnimation = dataRow["BattleStandby"] as string; // 战斗待机
            string commonStandbyAnimation = dataRow["CommonStandbySeries1"] as string; // 普通待机
            string battleUnderAttackAnimation = dataRow["BattleUnderAttack"] as string; // 战斗被击
            string attackSeriesAnimation = dataRow["AttackSeries"] as string; // 战斗攻击
            string attackBuffAnimation = dataRow["AttackBuff"] as string; // 技能buff           
            string attackAoeAnimation = dataRow["AttackAoe"] as string; // 技能aoe
            string attackBangAnimation = dataRow["AttackBang"] as string; // 战斗重击
            string attackLongRangeAnimation = dataRow["AttackLongRange"] as string; // 远程攻击
            string deathAnimation = dataRow["Death"] as string; // 死亡
            string knockDownStandUpAnimation = dataRow["KnockDown_StandUp"] as string; // 击倒起身
            string onGroundAnimation = dataRow["OnGround"] as string; // 倒地中
            string onGroundUnderAttackAnimation = dataRow["OnGroundUnderAttack"] as string; // 倒地被击
            string beatBackAnimation = dataRow["BeatBack"] as string; // 被击退
            string glissadeAnimation = dataRow["Glissade"] as string; // 滑步
            string voltAnimation = dataRow["Volt"] as string; // 闪避
            string intonateReleaseAnimation = dataRow["Intonate_Release"] as string; // 聚力释放
            string vertigoAnimation = dataRow["Vertigo"] as string; // 眩晕
            string wardOffAnimation = dataRow["WardOff"] as string; // 格挡
            string stopGlissadeAnimation = dataRow["StopGlissade"] as string; // 滑步停止
            string animationGroup = dataRow["Knockdown_OnGround_OnGroundUnderAttack_Standup_death_Time"] as string; // 动作组          

            // 随机取待机动作序列
            int commonStandbyAnimationIndex = 0;
            List<string> validCommonStandbyAnimationList = new List<string>();
            string commonStandbyAnimation1 = dataRow["CommonStandbySeries1"] as string;
            string commonStandbyAnimation2 = dataRow["CommonStandbySeries2"] as string;
            string commonStandbyAnimation3 = dataRow["CommonStandbySeries3"] as string;
            string commonStandbyAnimation4 = dataRow["CommonStandbySeries4"] as string;
            string commonStandbyAnimation5 = dataRow["CommonStandbySeries5"] as string;
            string commonStandbyAnimation6 = dataRow["CommonStandbySeries6"] as string;

            if (!string.IsNullOrEmpty(commonStandbyAnimation1))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation1);                
            }

            if (!string.IsNullOrEmpty(commonStandbyAnimation2))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation2);                
            }

            if (!string.IsNullOrEmpty(commonStandbyAnimation3))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation3);                
            }

            if (!string.IsNullOrEmpty(commonStandbyAnimation4))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation4);                
            }

            if (!string.IsNullOrEmpty(commonStandbyAnimation5))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation5);                
            }

            if (!string.IsNullOrEmpty(commonStandbyAnimation6))
            {
                validCommonStandbyAnimationList.Add(commonStandbyAnimation6);                
            }

            if (validCommonStandbyAnimationList.Count > 1)
            {
                commonStandbyAnimationIndex = random.Next(validCommonStandbyAnimationList.Count);
                commonStandbyAnimation = validCommonStandbyAnimationList[commonStandbyAnimationIndex];
            }                        

            if (string.IsNullOrEmpty(commonStandbyAnimation)) // 检查普通待机动作是否为空
            {
                // 无替换动作
            }

            if (string.IsNullOrEmpty(battleStandbyAnimation)) // 检查战斗待机动作是否为空
            {
                // 替换为普通待机动作
                battleStandbyAnimation = commonStandbyAnimation;
            }

            if (string.IsNullOrEmpty(battleUnderAttackAnimation)) // 检查战斗被击动作是否为空
            {
                // 替换为战斗待机动作
                battleUnderAttackAnimation = battleStandbyAnimation;
            }

            if (string.IsNullOrEmpty(attackSeriesAnimation)) // 检查战斗攻击动作是否为空
            {
                // 替换为战斗待机动作
                attackSeriesAnimation = battleStandbyAnimation;
            }                        

            if (string.IsNullOrEmpty(runAnimation)) // 检查跑步动作是否为空
            {
                // 替换为普通待机动作
                runAnimation = commonStandbyAnimation;
            }

            if (string.IsNullOrEmpty(walkAnimation)) // 检查行走动作是否为空
            {
                // 替换为跑步动作
                walkAnimation = runAnimation;
            }

            if (string.IsNullOrEmpty(voltAnimation)) // 检查闪避动作是否为空
            {
                // 替换为被击动作
                voltAnimation = battleUnderAttackAnimation;
            }

            if (string.IsNullOrEmpty(vertigoAnimation)) // 检查眩晕动作是否为空
            {
                // 替换为被击动作
                vertigoAnimation = battleUnderAttackAnimation;
            }

            if (string.IsNullOrEmpty(wardOffAnimation)) // 检查格挡动作是否为空
            {
                // 替换为被击动作
                wardOffAnimation = battleUnderAttackAnimation;
            }

            if (string.IsNullOrEmpty(beatBackAnimation)) // 检查被击退动作是否为空
            {
                // 替换为战斗待机动作
                beatBackAnimation = battleStandbyAnimation;
            }

            if (string.IsNullOrEmpty(attackBuffAnimation)) // 检查技能buff动作是否为空
            {
                // 替换为战斗攻击动作                
                attackBuffAnimation = attackSeriesAnimation;                
            }

            if (string.IsNullOrEmpty(attackAoeAnimation)) // 检查技能aoe动作是否为空
            {
                // 替换为战斗攻击动作                
                attackAoeAnimation = attackSeriesAnimation;                
            }

            if (string.IsNullOrEmpty(attackBangAnimation)) // 检查战斗重击动作是否为空
            {
                // 替换为战斗攻击动作
                attackBangAnimation = attackSeriesAnimation;                
            }

            if (string.IsNullOrEmpty(attackLongRangeAnimation)) // 检查远程攻击动作是否为空
            {
                // 替换为战斗攻击动作
                attackLongRangeAnimation = attackSeriesAnimation;                
            }

            if (string.IsNullOrEmpty(deathAnimation)) // 检查死亡动作是否为空
            {
                // 替换为普通待机动作
                deathAnimation = commonStandbyAnimation;
            }

            if (string.IsNullOrEmpty(onGroundAnimation)) // 检查倒地中动作是否为空
            {
                // 替换为死亡动作
                onGroundAnimation = deathAnimation;
            }

            if (string.IsNullOrEmpty(onGroundUnderAttackAnimation)) // 检查倒地被击动作是否为空
            {
                // 替换为倒地中动作
                onGroundUnderAttackAnimation = onGroundAnimation;
            }
           
            if (string.IsNullOrEmpty(glissadeAnimation)) // 检查滑步动作是否为空
            {
                // 替换为被击动作
                glissadeAnimation = battleUnderAttackAnimation;
            }           

            if (string.IsNullOrEmpty(knockDownStandUpAnimation)) // 检查击倒起身动作是否为空
            {
                if (!string.IsNullOrEmpty(battleUnderAttackAnimation)) // 替换为被击动作
                {
                    tempArray = battleUnderAttackAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries); // 只取1个动作                   
                    knockDownStandUpAnimation = string.Format("{0},{1}", tempArray[0], tempArray[0]);
                }
            }

            if (string.IsNullOrEmpty(intonateReleaseAnimation)) // 检查聚力释放动作是否为空
            {
                if (!string.IsNullOrEmpty(attackSeriesAnimation) && !string.IsNullOrEmpty(battleStandbyAnimation)) // 替换为战斗攻击动作和战斗待机动作
                {
                    tempArray = battleStandbyAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    string battleStandbyAnimationString = tempArray[0]; // 只取1个动作
                    tempArray = attackSeriesAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    string attackSeriesAnimationString = tempArray[0]; // 只取1个动作

                    intonateReleaseAnimation = string.Format("{0},{1}", battleStandbyAnimationString, attackSeriesAnimationString);
                }                
            }

            if (string.IsNullOrEmpty(stopGlissadeAnimation)) // 检查滑步停止动作是否为空
            {
                // 替换为战斗待机动作
                stopGlissadeAnimation = battleStandbyAnimation;
            }

            if (!string.IsNullOrEmpty(animationGroup)) // 检查动作组是否为空
            {
                // 优先将动作组内动作替换掉对应的单个动作
                tempArray = animationGroup.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                if (tempArray.Length > 0)
                {
                    tempArray = tempArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    if (tempArray.Length == 6)
                    {
                        knockDownStandUpAnimation = string.Format("{0},{1}", tempArray[0], tempArray[3]);
                        onGroundAnimation = tempArray[1];
                        onGroundUnderAttackAnimation = tempArray[2];
                        deathAnimation = tempArray[4];
                    }
                    else
                    {
                        logText.Append(string.Format("数据格式错误：{0}\r\n", animationGroup));
                    }
                }
            }

            // 填入数据
            animationTable["Walk"] = walkAnimation;
            animationTable["Run"] = runAnimation;
            animationTable["BattleStandby"] = battleStandbyAnimation;
            animationTable["CommonStandby"] = commonStandbyAnimation;
            animationTable["BattleUnderAttack"] = battleUnderAttackAnimation;
            animationTable["AttackSeries"] = attackSeriesAnimation;
            animationTable["AttackBuff"] = attackBuffAnimation;
            animationTable["AttackAoe"] = attackAoeAnimation;
            animationTable["AttackBang"] = attackBangAnimation;
            animationTable["AttackLongRange"] = attackLongRangeAnimation;
            animationTable["Death"] = deathAnimation;
            animationTable["KnockDownStandUp"] = knockDownStandUpAnimation;
            animationTable["OnGround"] = onGroundAnimation;
            animationTable["OnGroundUnderAttack"] = onGroundUnderAttackAnimation;
            animationTable["BeatBack"] = beatBackAnimation;
            animationTable["Glissade"] = glissadeAnimation;
            animationTable["Volt"] = voltAnimation;
            animationTable["IntonateRelease"] = intonateReleaseAnimation;
            animationTable["Vertigo"] = vertigoAnimation;
            animationTable["WardOff"] = wardOffAnimation;
            animationTable["StopGlissade"] = stopGlissadeAnimation;
            animationTable["BoardHeight"] = dataRow["BoneToHeight"] as string;
            animationTable["SelectionScale"] = dataRow["BoneToSelectionScale"] as string;
            animationTable["BuffInsertPosition"] = dataRow["BuffInsertPosition"] as string;
            animationTable["AOEInsertPosition"] = dataRow["AOEInsertPosition"] as string;
            animationTable["BangInsertPosition"] = dataRow["BangInsertPosition"] as string;            
            animationTable["LongRangeInsertPosition"] = dataRow["LongRangeInsertPosition"] as string;
            // 先暂时屏蔽掉 animationTable["SocketScale"] = dataRow["SocketScale"] as string;

            return animationTable;
        }

        /// <summary>
        /// 获取武器类型
        /// </summary>
        /// <param name="meshFile">插槽文件</param>
        /// <returns>武器类型</returns>
        private string GetWeaponType(string meshFile)
        {
            string weaponType = null;
            string itemType = "";

            string[] data = meshFile.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
            if (data.Length > 0)
            {
                string meshName = data[data.Length - 1];
                string itemNumber = meshName.Replace(".Mesh", "").Replace(".mesh", "");
                DataRow[] rows = itemForInsertTable.Select(string.Format("ItemNumber = '{0}'", itemNumber));

                if (rows.Length > 0)
                {
                    itemType = rows[0]["ItemType"].ToString();
                }
            }

            switch (itemType)
            {
                case "1":
                    {
                        weaponType = "单兵";
                        break;
                    }
                case "2":
                    {
                        weaponType = "长兵";
                        break;
                    }
                case "3":
                    {
                        weaponType = "特殊";
                        break;
                    }
                case "4":
                    {
                        weaponType = "拳套";
                        break;
                    }
                case "5":
                    {
                        weaponType = "笔";
                        break;
                    }
                case "6":
                    {
                        weaponType = "拐杖";
                        break;
                    }
                case "7":
                    {
                        weaponType = "蛋刀";
                        break;
                    }
                default:
                    {
                        weaponType = itemType;
                        break;
                    }
            }

            return weaponType;
        }

        /// <summary>
        /// 选择DetailType
        /// </summary>
        /// <param name="rows">数据行数组</param>
        /// <param name="showMessage">是否显示消息</param>
        /// <returns>选择的编号</returns>
        private int SelectDetailType(DataRow[] rows, bool showMessage)
        {
            int index = 0;
            NpcTypeSelectForm nForm = null;

            if (rows.Length > 1) // 有多个匹配的数据行需要用户选择
            {
                List<string> detailTypeList = new List<string>();

                foreach (DataRow dataRow in rows)
                {
                    // string forceString = dataRow["BoneForce"].ToString(); 临时屏蔽掉
                    string forceString = "";

                    if (forceString == modelForce || forceString == "" || modelForce == "")
                    {
                        if (forceString == "")
                        {
                            forceString = "无势力";
                        }

                        detailTypeList.Add(string.Format("{0} ({1})", dataRow["DetailType"].ToString(), forceString));
                    }                                       
                }

                if (showMessage)
                {
                    nForm = new NpcTypeSelectForm(detailTypeList, "NPC类型选择");
                }
                else
                {
                    nForm = new NpcTypeSelectForm(detailTypeList, string.Format("NPC类型选择 {0}", npcDisplayName));
                }

                if (nForm.ShowDialog() == DialogResult.OK)
                {
                    index = nForm.SelectionIndex;
                }                
            }

            return index;
        }

        /// <summary>
        /// 更新npc动作
        /// </summary>
        /// <param name="modelType">模型类型</param>
        /// <param name="showMessage">是否显示消息</param>
        /// <param name="updateExistAnimation">是否更新已有动作</param>
        /// <param name="updateCommonStandbyAnimation">是否更新普通待机动作</param>
        /// <return>是否更新成功</return>
        private bool UpdateAnimation(string modelType, bool showMessage, bool updateExistAnimation, bool updateCommonStandbyAnimation)
        {
            bool success = false;
            Hashtable infoTable = null;
            DataRow[] rows;
            DataRow row;
            string leftHandPriorty = null; // 左手插槽优先级
            string rightHandPriorty = null; // 右手插槽优先级
            string leftHandWeaponType = GetWeaponType(leftHandMesh); // 左手武器资源
            string rightHandWeaponType = GetWeaponType(rightHandMesh); // 右手武器资源
            string leftHandItemNumber = GetItemNumber(leftHandMesh); // 左手插槽key值
            string rightHandItemNumber = GetItemNumber(rightHandMesh); // 右手插槽key值
            string leftHandHitEffectResultID = GetHitEffectResultID(leftHandItemNumber); // 左手打击效果ID
            string rightHandHitEffectResultID = GetHitEffectResultID(rightHandItemNumber);  // 右手打击效果ID
            string hitEffectResultID = null;

            // 初始化左手插槽优先级
            if (leftHandItemNumber != null)
            {
                rows = itemForInsertTable.Select(string.Format("ItemNumber = '{0}'", leftHandItemNumber));

                if (rows.Length > 0)
                {
                    row = rows[0];
                    leftHandPriorty = row["S_LH_MeshFile"] as string;
                }
            }

            // 初始化右手插槽优先级
            if (rightHandItemNumber != null)
            {
                rows = itemForInsertTable.Select(string.Format("ItemNumber = '{0}'", rightHandItemNumber));

                if (rows.Length > 0)
                {
                    row = rows[0];
                    rightHandPriorty = row["S_RH_MeshFile"] as string;
                }
            }
            
            if (!string.IsNullOrEmpty(rightHandMesh)) // 检查右手有无填写模型资源
            {
                if (!string.IsNullOrEmpty(rightHandPriorty) && rightHandPriorty != "0") // 检查模型资源是否正确包含右手插槽
                {
                    if (!string.IsNullOrEmpty(leftHandMesh)) // 检查左手有无填写模型资源 
                    {
                        if (!string.IsNullOrEmpty(leftHandPriorty) && leftHandPriorty != "0") // 检查模型资源是否正确包含左手插槽
                        {
                            if (leftHandWeaponType == rightHandWeaponType) // 比较两手的模型分类是否相同
                            {
                                if (leftHandWeaponType == "拳套") // 都属于拳套类型
                                {
                                    // 归类为空手类型填写动作表
                                    rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                                    modelType, "空手"));
                                    if (rows.Length > 0)
                                    {
                                        row = rows[SelectDetailType(rows, showMessage)];

                                        if (modelType.StartsWith("A")) // 动物模型需要填成撕咬
                                        {
                                            hitEffectResultID = "5070"; // 空手填撕咬类型的打击效果
                                        }
                                        else
                                        {
                                            hitEffectResultID = "5000"; // 空手填拳脚类型的打击效果
                                        }                                            
                                        infoTable = FillAnimationWithPriorty(row);
                                    }
                                }
                                else if (leftHandWeaponType == "蛋刀") // 都属于蛋刀类型
                                {
                                    // 归类于蛋刀类型填写动作表
                                    rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                                    modelType, "蛋刀"));
                                    if (rows.Length > 0)
                                    {
                                        row = rows[SelectDetailType(rows, showMessage)];

                                        hitEffectResultID = rightHandHitEffectResultID; // 填右手的打击效果
                                        infoTable = FillAnimationWithPriorty(row);
                                    }
                                }
                                else
                                {
                                    // 归类为双兵类型填写动作表
                                    rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                                    modelType, "双兵"));
                                    if (rows.Length > 0)
                                    {
                                        row = rows[SelectDetailType(rows, showMessage)];

                                        hitEffectResultID = rightHandHitEffectResultID; // 填右手的打击效果
                                        infoTable = FillAnimationWithPriorty(row);
                                    }
                                }
                            }
                            else
                            {
                                // 按右手模型所属分类填写动作表
                                rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                                modelType, rightHandWeaponType));

                                if (rows.Length > 0)
                                {
                                    row = rows[SelectDetailType(rows, showMessage)];

                                    hitEffectResultID = rightHandHitEffectResultID; // 填右手的打击效果
                                    infoTable = FillAnimationWithPriorty(row);
                                }
                            }
                        }
                        else
                        {
                            if (showMessage)
                            {
                                MessageBox.Show("模型资源不包含左手插槽！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            }
                        }
                    }
                    else
                    {
                        // 查找右手模型资源所属武器分类
                        rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                        modelType, rightHandWeaponType));

                        if (rows.Length > 0)
                        {
                            row = rows[SelectDetailType(rows, showMessage)];

                            hitEffectResultID = rightHandHitEffectResultID; // 填右手的打击效果
                            infoTable = FillAnimationWithPriorty(row);
                        }
                    }
                }
                else
                {
                    if (showMessage)
                    {
                        MessageBox.Show("模型资源不包含右手插槽！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
            }
            else // 按空手类型填写动作表
            {
                rows = boneToActionTable.Select(string.Format("BoneNumber = '{0}' AND WeaponType = '{1}'",
                                                modelType, "空手"));
                if (rows.Length > 0)
                {
                    row = rows[SelectDetailType(rows, showMessage)];

                    if (modelType.StartsWith("A")) // 动物模型需要填成撕咬
                    {
                        hitEffectResultID = "5070"; // 空手填撕咬类型的打击效果
                    }
                    else
                    {
                        hitEffectResultID = "5000"; // 空手填拳脚类型的打击效果
                    }     
                    infoTable = FillAnimationWithPriorty(row);
                }
            }            

            if (infoTable != null)
            {
                success = UpdateAnimation(infoTable, hitEffectResultID, showMessage, updateExistAnimation, updateCommonStandbyAnimation);
            }
            else
            {
                if (showMessage)
                {
                    MessageBox.Show("没有找到匹配的动作资源！", "更新动作",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);                    
                }

                logText.Append("Error：没有找到匹配的动作资源！\r\n");
            }

            return success;
        }

        /// <summary>
        /// 检查动作是否已经存在
        /// </summary>
        /// <param name="animationTable">动作表</param>
        /// <param name="animationID">动作ID</param>
        /// <returns>动作是否已经存在</returns>
        private bool AnimationExist(DataTable animationTable, string animationID)
        {
            DataRow[] rows = animationTable.Select(string.Format("AnimationID = '{0}'", animationID));
            bool result = rows.Length > 0;

            return result;
        }

        /// <summary>
        /// 获取动作文件路径
        /// </summary>
        /// <param name="animationTable">动作表</param>
        /// <param name="animationID">动作ID</param>
        /// <returns>动作文件路径</returns>
        private string GetAnimationFile(DataTable animationTable, string animationID)
        {
            string animationFile = null;

            DataRow[] rows = animationTable.Select(string.Format("AnimationID = '{0}'", animationID));
            if (rows.Length > 0)
            {
                animationFile = rows[0]["AnimationFile"].ToString();
            }

            return animationFile;
        }

        /// <summary>
        /// 检查待机动作是否已经存在
        /// </summary>
        /// <param name="animationTable">动作表</param>
        /// <returns>待机动作是否已经存在</returns>
        private bool CommonStandbyAnimationExist(DataTable animationTable)
        {
            DataRow[] rows = animationTable.Select("AnimationID = 30 OR AnimationID = 31 OR AnimationID = 32 OR AnimationID = 33 OR AnimationID = 34 OR AnimationID = 35");
            bool result = rows.Length > 0;

            return result;
        }

        /// <summary>
        /// 清掉动作
        /// </summary>
        /// <param name="cmd">sql命令对象</param>
        /// <param name="animationID">动作ID</param>
        private void ClearAnimation(SqlCommand cmd, string animationID)
        {
            string sqlString = string.Format("DELETE FROM animation_npc WHERE RepresentID = '{0}' AND AnimationID = '{1}'",
                                             npcRepresentID.ToString(), animationID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();
        }

        /// <summary>
        /// 更新动作
        /// </summary>
        /// <param name="cmd">SqlCommand</param>
        /// <param name="animationTable">动作数据表</param>
        /// <param name="animationFile">动作文件名</param>
        /// <param name="animationID">动作ID</param>
        /// <param name="formatString">查询格式字符串</param>
        /// <param name="updateExistAnimation">是否更新已有动作</param>
        private void UpdateAnimation(SqlCommand cmd, DataTable animationTable, string animationFile, string animationID, string formatString, bool updateExistAnimation)
        {
            if (!string.IsNullOrEmpty(animationFile))
            {
                if (updateExistAnimation) // 先清动作
                {
                    ClearAnimation(cmd, animationID);
                }

                if (updateExistAnimation || !AnimationExist(animationTable, animationID)) // 强制更新动作或者动作不存在
                {
                    // 从资源中随机选出一个填动作表                        
                    string animationString = GetAnimationString(animationFile);
                    string[] tempArray = animationFile.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                    if (tempArray.Length > 1)
                    {
                        int randomIndex = random.Next(tempArray.Length);
                        animationString = GetAnimationString(tempArray[randomIndex]);
                    }

                    string sqlString = string.Format(formatString, animationString, npcRepresentID.ToString(), mapName);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    // 动作表补丁
                    UpdateAnimationPlusInfo(cmd, animationID);
                }

                // 复制插槽数据，强制更新                               
                UpdateSlotInfo(cmd, animationID);
            }
            else
            {
                if (AnimationExist(animationTable, animationID))
                {
                    // 复制插槽数据                             
                    UpdateSlotInfo(cmd, animationID);
                }
            }
        }

        /// <summary>
        /// 更新npc动作
        /// </summary>
        /// <param name="infoTable">动作数据</param>
        /// <param name="hitEffectResultID">打击效果ID</param>
        /// <param name="showMessage">是否显示提示信息</param>
        /// <param name="updateExistAnimation">是否更新已有动作</param>
        /// <param name="updateCommonStandbyAnimation">是否更新普通待机动作</param>
        /// <return>是否更新成功</return>
        private bool UpdateAnimation(Hashtable infoTable, string hitEffectResultID, bool showMessage, bool updateExistAnimation, bool updateCommonStandbyAnimation)
        {
            bool success = true;
            string walkAnimation = infoTable["Walk"] as string; // 行走
            string runAnimation = infoTable["Run"] as string; // 跑步
            string battleStandbyAnimation = infoTable["BattleStandby"] as string; // 战斗待机
            string commonStandbyAnimation = infoTable["CommonStandby"] as string; // 普通待机
            string battleUnderAttackAnimation = infoTable["BattleUnderAttack"] as string; // 战斗被击            
            string attackSeriesAnimation = infoTable["AttackSeries"] as string; // 战斗攻击
            string attackBuffAnimation = infoTable["AttackBuff"] as string; // 技能buff           
            string attackAoeAnimation = infoTable["AttackAoe"] as string; // 技能aoe
            string attackBangAnimation = infoTable["AttackBang"] as string; // 战斗重击
            string attackLongRangeAnimation = infoTable["AttackLongRange"] as string; // 远程攻击
            string deathAnimation = infoTable["Death"] as string; // 死亡
            string knockDownStandUpAnimation = infoTable["KnockDownStandUp"] as string; // 起身
            string onGroundAnimation = infoTable["OnGround"] as string; // 警戒
            string onGroundUnderAttackAnimation = infoTable["OnGroundUnderAttack"] as string; // 警戒被击
            string beatBackAnimation = infoTable["BeatBack"] as string; // 被击退
            string glissadeAnimation = infoTable["Glissade"] as string; // 滑步
            string voltAnimation = infoTable["Volt"] as string; // 闪避
            string intonateReleaseAnimation = infoTable["IntonateRelease"] as string; // 聚力释放
            string vertigoAnimation = infoTable["Vertigo"] as string; // 眩晕
            string wardOffAnimation = infoTable["WardOff"] as string; // 格挡
            string stopGlissadeAnimation = infoTable["StopGlissade"] as string; // 滑步停止
            string boardHeight = infoTable["BoardHeight"] as string; // 公告板高度
            string selectionScale = infoTable["SelectionScale"] as string; // 选择框缩放比例
            string buffInsertPosition = infoTable["BuffInsertPosition"] as string; // 技能buff插槽位置
            string aoeInsertPosition = infoTable["AOEInsertPosition"] as string; // 技能aoe插槽位置
            string bangInsertPosition = infoTable["BangInsertPosition"] as string; // 战斗重击插槽位置
            string longRangeInsertPosition = infoTable["LongRangeInsertPosition"] as string; // 远程攻击插槽位置
            // 先暂时屏蔽掉 string socketScale = infoTable["SocketScale"] as string; // 插槽缩放比

            // 执行数据库读写操作
            string sqlString = string.Format("SELECT AnimationID, AnimationFile FROM animation_npc WHERE RepresentID = '{0}'",
                                             npcRepresentID);
            DataTable animationTable = helper.GetDataTable(conn, sqlString);

            string[] tempArray;
            string animationString;
            int randomIndex;
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

                // 自动补全技能表
                sqlString = string.Format("SELECT RepresentID FROM dic_npc_skill_caster_npc WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1",
                                          npcRepresentID.ToString());
                cmd.CommandText = sqlString;
                if (cmd.ExecuteScalar() == null) // 需要自动补全
                {
                    sqlString = string.Format("INSERT INTO dic_npc_skill_caster_npc (RepresentID, SkillID, SkillLevel) VALUES ('{0}', 28, 1)",
                                              npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                    logText.Append("没有找到技能动作，自动补全。\r\n");
                }

                // 逐条添加新动作
                // 添加行走动作
                string formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (9, 2, '{0}', '1', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, walkAnimation, "9", formatString, updateExistAnimation);

                // 添加跑步动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (16, 3, '{0}', '1', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, runAnimation, "16", formatString, updateExistAnimation);                

                // 添加普通待机动作
                if (!string.IsNullOrEmpty(commonStandbyAnimation))
                {
                    if (updateExistAnimation && updateCommonStandbyAnimation) // 先清动作
                    {
                        ClearAnimation(cmd, "30");
                        ClearAnimation(cmd, "31");
                        ClearAnimation(cmd, "32");
                        ClearAnimation(cmd, "33");
                        ClearAnimation(cmd, "34");
                        ClearAnimation(cmd, "35");
                    }

                    // 选择的武器也要填入锁对应的插槽里
                    tempArray = commonStandbyAnimation.Split(new char[] { ';' });
                    string[] commonStandbySeriousArray = new string[] { "30", "31", "32", "33", "34", "35" };
                    string[] fieldArray = new string[] { "IdleAniRatio0", "IdleAniRatio1", "IdleAniRatio2", "IdleAniRatio3", "IdleAniRatio4", "IdleAniRatio5" };
                    string[] aniRatioArray = new string[] { "10000", "500", "200", "200", "200", "200" };

                    if ((updateExistAnimation && updateCommonStandbyAnimation) || !CommonStandbyAnimationExist(animationTable)) // 已经存在一个就不需要更新
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            if (i < tempArray.Length)
                            {                                         
                                // 更新动作
                                animationString = GetAnimationString(tempArray[i]);

                                if (tempArray.Length > 1) // 普通待机动作数量 > 1时，循环类型为0
                                {
                                    sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES ('{0}', 1, '{1}', '0', '{2}', '{3}')",
                                                          commonStandbySeriousArray[i], animationString, npcRepresentID.ToString(), mapName);
                                }
                                else // 普通待机动作数量 == 1时，循环类型是1
                                {
                                    sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES ('{0}', 1, '{1}', '1', '{2}', '{3}')",
                                                          commonStandbySeriousArray[i], animationString, npcRepresentID.ToString(), mapName);
                                }
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();                                                                                                  

                                // 更新播放几率
                                sqlString = string.Format("UPDATE npc SET [{0}] = '{1}' WHERE RepresentID = '{2}'",
                                                          fieldArray[i], aniRatioArray[i], npcRepresentID.ToString());
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                // 动作表补丁
                                UpdateAnimationPlusInfo(cmd, commonStandbySeriousArray[i]);
                                
                                // 复制插槽数据，强制更新
                                UpdateSlotInfo(cmd, commonStandbySeriousArray[i]);
                            }
                            else // 强制刷播放几率
                            {
                                sqlString = string.Format("UPDATE npc SET [{0}] = '0' WHERE RepresentID = '{1}'",
                                                          fieldArray[i], npcRepresentID.ToString());
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                        } 
                    }
                    else
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            if (AnimationExist(animationTable, commonStandbySeriousArray[i]))
                            {
                                // 复制插槽数据
                                UpdateSlotInfo(cmd, commonStandbySeriousArray[i]);
                            }
                            else // 强制刷播放几率
                            {
                                sqlString = string.Format("UPDATE npc SET [{0}] = '0' WHERE RepresentID = '{1}'",
                                                          fieldArray[i], npcRepresentID.ToString());
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();
                            }
                        }
                    }                                                           
                }

                // 添加战斗待机动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (47, 4, '{0}', '1', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, battleStandbyAnimation, "47", formatString, updateExistAnimation);

                // 添加被击动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (73, 6, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, battleUnderAttackAnimation, "73", formatString, updateExistAnimation);
                

                // 添加战斗攻击动作
                if (!string.IsNullOrEmpty(attackSeriesAnimation))
                {
                    if (updateExistAnimation) // 先清动作
                    {
                        ClearAnimation(cmd, "54");
                        ClearAnimation(cmd, "55");
                        ClearAnimation(cmd, "56");
                        ClearAnimation(cmd, "57");
                    }

                    string[] attackSeriesIDArray = new string[] { "54", "55", "56", "57" };
                    tempArray = attackSeriesAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    int maxAttackTime = 0;
                    int currentAttackTime = 0;

                    // 更新动作表
                    if (tempArray.Length >= 4) // 抽出4项，填入CastSkillAnimationID0 -- CastSkillAnimationID3字段中
                    {
                        List<string> attackAnimationList = new List<string>();
                        List<string> attackRandomList = new List<string>();

                        attackRandomList.AddRange(tempArray);
                        
                        // 第一个攻击动作必填
                        attackAnimationList.Add(tempArray[0]);
                        attackRandomList.Remove(tempArray[0]);                        

                        // 再随机抽取三个动作出来
                        for (int i = 0; i < 3; i++)
                        {
                            randomIndex = random.Next(attackRandomList.Count);
                            attackAnimationList.Add(attackRandomList[randomIndex]);
                            attackRandomList.RemoveAt(randomIndex);
                        }

                        for (int i = 0; i < 4; i++)
                        {
                            if (updateExistAnimation || !AnimationExist(animationTable, attackSeriesIDArray[i])) // 强制更新动作或者动作不存在
                            {                                
                                animationString = GetAnimationString(attackAnimationList[i]);
                                sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES ('{0}', 5, '{1}', '0', '{2}', '{3}')",
                                                          attackSeriesIDArray[i], animationString, npcRepresentID.ToString(), mapName);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                currentAttackTime = GetAnimationTime(animationString);
                                if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                {
                                    maxAttackTime = currentAttackTime;
                                }

                                // 动作表补丁
                                UpdateAnimationPlusInfo(cmd, attackSeriesIDArray[i]);
                            }
                            else
                            {
                                animationString = GetAnimationFile(animationTable, attackSeriesIDArray[i]);

                                if (!string.IsNullOrEmpty(animationString))
                                {
                                    currentAttackTime = GetAnimationTime(animationString);
                                    if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                    {
                                        maxAttackTime = currentAttackTime;
                                    }
                                }
                            }
                        }                                               
                    }
                    else // 允许重复，保证4个字段都有动作
                    {                        
                        for (int i = 0; i < 4; i++)
                        {
                            if (i < tempArray.Length)
                            {
                                if (updateExistAnimation || !AnimationExist(animationTable, attackSeriesIDArray[i])) // 强制更新动作或者动作不存在
                                {                                    
                                    animationString = GetAnimationString(tempArray[i]);
                                    sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES ('{0}', 5, '{1}', '0', '{2}', '{3}')",
                                                              attackSeriesIDArray[i], animationString, npcRepresentID.ToString(), mapName);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();

                                    currentAttackTime = GetAnimationTime(animationString);
                                    if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                    {
                                        maxAttackTime = currentAttackTime;
                                    }

                                    // 动作表补丁
                                    UpdateAnimationPlusInfo(cmd, attackSeriesIDArray[i]);
                                }
                                else
                                {
                                    animationString = GetAnimationFile(animationTable, attackSeriesIDArray[i]);

                                    if (!string.IsNullOrEmpty(animationString))
                                    {
                                        currentAttackTime = GetAnimationTime(animationString);
                                        if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                        {
                                            maxAttackTime = currentAttackTime;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (updateExistAnimation || !AnimationExist(animationTable, attackSeriesIDArray[i])) // 强制更新动作或者动作不存在
                                {                                    
                                    animationString = GetAnimationString(tempArray[tempArray.Length - 1]);
                                    sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES ('{0}', 5, '{1}', '0', '{2}', '{3}')",
                                                              attackSeriesIDArray[i], animationString, npcRepresentID.ToString(), mapName);
                                    cmd.CommandText = sqlString;
                                    cmd.ExecuteNonQuery();

                                    currentAttackTime = GetAnimationTime(animationString);
                                    if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                    {
                                        maxAttackTime = currentAttackTime;
                                    }

                                    // 动作表补丁
                                    UpdateAnimationPlusInfo(cmd, attackSeriesIDArray[i]);
                                }
                                else
                                {
                                    animationString = GetAnimationFile(animationTable, attackSeriesIDArray[i]);

                                    if (!string.IsNullOrEmpty(animationString))
                                    {
                                        currentAttackTime = GetAnimationTime(animationString);
                                        if (currentAttackTime > maxAttackTime) // 取得最长攻击时间
                                        {
                                            maxAttackTime = currentAttackTime;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // 更新技能表
                    sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID0 = 54 WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1", 
                                              npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID1 = 55 WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1", 
                                              npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID2 = 56 WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1", 
                                              npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET CastSkillAnimationID3 = 57 WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1", 
                                              npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    // 更新AI动作
                    UpdateAISkillAniFrame(cmd, maxAttackTime);

                    // 复制插槽数据                               
                    UpdateSlotInfo(cmd, "54");                                       
                    UpdateSlotInfo(cmd, "55");                                       
                    UpdateSlotInfo(cmd, "56");                                             
                    UpdateSlotInfo(cmd, "57");
                }

                // 添加技能buff动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (64, 5, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, attackBuffAnimation, "64", formatString, updateExistAnimation);

                // 添加技能aoe动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (65, 5, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, attackAoeAnimation, "65", formatString, updateExistAnimation);

                // 添加战斗重击动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (66, 5, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, attackBangAnimation, "66", formatString, updateExistAnimation);

                // 添加远程攻击动作                
                if (!string.IsNullOrEmpty(attackLongRangeAnimation))
                {
                    if (updateExistAnimation) // 先清动作
                    {
                        ClearAnimation(cmd, "67");
                    }

                    if (updateExistAnimation || !AnimationExist(animationTable, "67")) // 强制更新动作或者动作不存在
                    {
                        // 根据远程武器类型，进行选择填表，根据资源文件名字的最后两个汉字， 决定是那种远程动作，再和类型对应，选择一个填入动作表
                        tempArray = attackLongRangeAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                        // 选出适合的远程动作
                        if (string.IsNullOrEmpty(longRangeWeaponType)) // 默认填为暗器类型
                        {
                            longRangeWeaponType = "暗器";
                        }

                        int tempIndex = 0;
                        for (int i = 0; i < tempArray.Length; i++)
                        {
                            if (tempArray[i].EndsWith(longRangeWeaponType))
                            {
                                tempIndex = i;
                                break;
                            }
                        }
                        
                        animationString = GetAnimationString(tempArray[tempIndex]);
                        sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (67, 5, '{0}', '0', '{1}', '{2}')",
                                                  animationString, npcRepresentID.ToString(), mapName);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();

                        // 动作表补丁
                        UpdateAnimationPlusInfo(cmd, "67");
                    }  
                  
                    // 复制插槽数据，强制更新                               
                    UpdateSlotInfo(cmd, "67");
                }
                else
                {
                    if (AnimationExist(animationTable, "67"))
                    {
                        // 复制插槽数据                             
                        UpdateSlotInfo(cmd, "67");
                    }
                }

                // 添加死亡动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (1, 8, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, deathAnimation, "1", formatString, updateExistAnimation);

                // 添加被击倒和起身动作
                if (!string.IsNullOrEmpty(knockDownStandUpAnimation))
                {
                    // 从资源中随机选出一个填动作表
                    tempArray = knockDownStandUpAnimation.Split(new char[]{ ';' }, StringSplitOptions.RemoveEmptyEntries);
                    // 分解出被击倒动作和起身动作

                    if (tempArray.Length > 1)
                    {
                        randomIndex = random.Next(tempArray.Length);
                        tempArray = tempArray[randomIndex].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    }
                    else
                    {
                        tempArray = tempArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    }
                    
                    if (tempArray.Length == 2)
                    {
                        if (updateExistAnimation) // 先清动作
                        {
                            ClearAnimation(cmd, "2");
                            ClearAnimation(cmd, "5");
                        }

                        // 添加被击倒动作
                        if (updateExistAnimation || !AnimationExist(animationTable, "2")) // 强制更新动作或者动作不存在
                        {                        
                            animationString = GetAnimationString(tempArray[0]);
                            sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (2, 11, '{0}', '0', '{1}', '{2}')",
                                                      animationString, npcRepresentID.ToString(), mapName);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            // 动作表补丁
                            UpdateAnimationPlusInfo(cmd, "2");
                        }
                    
                        // 复制插槽数据，强制更新                               
                        UpdateSlotInfo(cmd, "2");

                        // 添加起身动作
                        if (updateExistAnimation || !AnimationExist(animationTable, "5")) // 强制更新动作或者动作不存在
                        {                        
                            animationString = GetAnimationString(tempArray[1]);
                            sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (5, 12, '{0}', '0', '{1}', '{2}')",
                                                      animationString, npcRepresentID.ToString(), mapName);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            // 动作表补丁
                            UpdateAnimationPlusInfo(cmd, "5");
                        } 

                        // 复制插槽数据，强制更新                              
                        UpdateSlotInfo(cmd, "5");
                    }
                    else
                    {
                        if (AnimationExist(animationTable, "2"))
                        {
                            // 复制插槽数据                              
                            UpdateSlotInfo(cmd, "2");
                        }

                        if (AnimationExist(animationTable, "5"))
                        {
                            // 复制插槽数据                            
                            UpdateSlotInfo(cmd, "5");
                        }

                        logText.Append(string.Format("数据格式错误：{0}\r\n", knockDownStandUpAnimation));
                    }
                }
                else
                {
                    if (AnimationExist(animationTable, "2"))
                    {
                        // 复制插槽数据                              
                        UpdateSlotInfo(cmd, "2");
                    }

                    if (AnimationExist(animationTable, "5"))
                    {
                        // 复制插槽数据                            
                        UpdateSlotInfo(cmd, "5");
                    }
                }

                // 添加倒地中动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (291, 4, '{0}', '1', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, onGroundAnimation, "291", formatString, updateExistAnimation);

                // 添加倒地被击动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (292, 6, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, onGroundUnderAttackAnimation, "292", formatString, updateExistAnimation);

                // 添加被击退动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (3, 11, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, beatBackAnimation, "3", formatString, updateExistAnimation);

                // 添加滑步动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (287, 11, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, glissadeAnimation, "287", formatString, updateExistAnimation);

                // 添加闪避动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (250, 13, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, voltAnimation, "250", formatString, updateExistAnimation);

                // 添加聚力释放动作
                if (!string.IsNullOrEmpty(intonateReleaseAnimation))
                {
                    // 从资源中随机选出一个填动作表
                    tempArray = intonateReleaseAnimation.Split(new char[]{ ';' }, StringSplitOptions.RemoveEmptyEntries);
                    // 分解出聚力动作和释放动作
                    if (tempArray.Length > 1)
                    {
                        randomIndex = random.Next(tempArray.Length);
                        tempArray = tempArray[randomIndex].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    }
                    else
                    {
                        tempArray = tempArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    }                    

                    if (tempArray.Length == 2)
                    {
                        if (updateExistAnimation) // 先清动作
                        {
                            ClearAnimation(cmd, "68");
                            ClearAnimation(cmd, "69");
                        }

                        // 添加聚力动作
                        if (updateExistAnimation || !AnimationExist(animationTable, "69")) // 强制更新动作或者动作不存在
                        {                        
                            animationString = GetAnimationString(tempArray[0]);
                            sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (69, 25, '{0}', '1', '{1}', '{2}')",
                                                      animationString, npcRepresentID.ToString(), mapName);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            // 动作表补丁
                            UpdateAnimationPlusInfo(cmd, "69");
                        }
                    
                        // 复制插槽数据，强制更新                             
                        UpdateSlotInfo(cmd, "69");

                        // 添加释放动作
                        if (updateExistAnimation || !AnimationExist(animationTable, "68")) // 强制更新动作或者动作不存在
                        {
                            animationString = GetAnimationString(tempArray[1]);
                            sqlString = string.Format("INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (68, 5, '{0}', '0', '{1}', '{2}')",
                                                      animationString, npcRepresentID.ToString(), mapName);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            // 动作表补丁
                            UpdateAnimationPlusInfo(cmd, "68");
                        }              
      
                        // 复制插槽数据，强制更新                               
                        UpdateSlotInfo(cmd, "68");

                        // 更新技能表
                        sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET PrepareCastSkillAnimationID = 69 WHERE RepresentID = '{0}' AND SkillID = 28 AND SkillLevel = 1", 
                                                  npcRepresentID.ToString());
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }
                    else
                    {
                        if (AnimationExist(animationTable, "69"))
                        {
                            // 复制插槽数据                            
                            UpdateSlotInfo(cmd, "69");
                        }

                        if (AnimationExist(animationTable, "68"))
                        {
                            // 复制插槽数据                               
                            UpdateSlotInfo(cmd, "68");
                        }

                        logText.Append(string.Format("数据格式错误：{0}\r\n", intonateReleaseAnimation));
                    }
                }
                else
                {
                    if (AnimationExist(animationTable, "69"))
                    {
                        // 复制插槽数据                            
                        UpdateSlotInfo(cmd, "69");
                    }

                    if (AnimationExist(animationTable, "68"))
                    {
                        // 复制插槽数据                               
                        UpdateSlotInfo(cmd, "68");
                    }
                }

                // 添加眩晕动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (80, 15, '{0}', '1', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, vertigoAnimation, "80", formatString, updateExistAnimation);

                // 添加格挡动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (84, 14, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, wardOffAnimation, "84", formatString, updateExistAnimation);

                // 添加滑步停止动作
                formatString = "INSERT INTO animation_npc (AnimationID, KindID, AnimationFile, AnimationType, RepresentID, Map) VALUES (288, 11, '{0}', '0', '{1}', '{2}')";
                UpdateAnimation(cmd, animationTable, stopGlissadeAnimation, "288", formatString, updateExistAnimation);               

                // 更新打击效果
                if (!string.IsNullOrEmpty(hitEffectResultID))
                {
                    sqlString = string.Format("UPDATE dic_npc_skill_caster_npc SET HitEffectResultID = '{0}' WHERE RepresentID = '{1}' AND SkillID = 28 AND SkillLevel = 1",
                                              hitEffectResultID, npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                // 更新公告板高度
                if (!string.IsNullOrEmpty(boardHeight))
                {
                    float boardHeightF = float.Parse(boardHeight);
                    boardHeightF = boardHeightF * modelScale;
                    sqlString = string.Format("UPDATE npc SET Height = '{0}' WHERE RepresentID = '{1}'", boardHeightF.ToString(), npcRepresentID.ToString());
                }
                else
                {
                    sqlString = string.Format("UPDATE npc SET Height = '180' WHERE RepresentID = '{0}'", npcRepresentID.ToString());
                }

                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();                

                // 更新选择框缩放比例
                if (!string.IsNullOrEmpty(selectionScale))
                {
                    float selectionScaleF = float.Parse(selectionScale);
                    selectionScaleF = selectionScaleF * modelScale;
                    sqlString = string.Format("UPDATE npc SET SelectionScale = '{0}' WHERE RepresentID = '{1}'", selectionScaleF.ToString(), npcRepresentID.ToString());
                }
                else
                {
                    sqlString = string.Format("UPDATE npc SET SelectionScale = '1' WHERE RepresentID = '{0}'", npcRepresentID.ToString());
                }

                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();               

                // 更新插槽缩放比例
                /* 先暂时屏蔽掉
                if (!string.IsNullOrEmpty(socketScale))
                {                    
                    sqlString = string.Format("UPDATE npc SET SocketScale = '{0}' WHERE RepresentID = '{1}'", socketScale, npcRepresentID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery(); 
                }
                */

                // 清除原有插槽信息     
                ClearSlotInfo(cmd);     
                
                // 更新地图分类
                sqlString = string.Format("UPDATE npc SET map = '{0}' WHERE RepresentID = '{1}'", mapName, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                sqlString = string.Format("UPDATE animation_npc SET Map = '{0}' WHERE RepresentID = '{1}'", mapName, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                transaction.Commit();

                if (showMessage)
                {
                    MessageBox.Show("NPC动作更新成功！", "自动更新动作", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }

                logText.Append("NPC动作更新成功！\r\n");
            }
            catch (SqlException ex)
            {
                if (showMessage)
                {
                    MessageBox.Show("数据库读写失败：" + ex.Message, "数据库读写",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);                    
                }                

                if (transaction != null)
                {
                    transaction.Rollback();
                }

                logText.Append(string.Format("Error：更新数据表失败，出错信息为{0}\r\n", ex.Message));
                success = false;
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
        /// 清空插槽信息
        /// </summary>
        /// <param name="cmd">sql命令</param>
        private void ClearSlotInfo(SqlCommand cmd)
        {
            string sqlString = null;
     
            if (!string.IsNullOrEmpty(leftHandMesh))
            {
                sqlString = string.Format("UPDATE npc SET LeftHandMesh = '{0}' WHERE RepresentID = '{1}'",
                                          leftHandMesh, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }

            if (!string.IsNullOrEmpty(leftHandMaterial))
            {
                sqlString = string.Format("UPDATE npc SET LeftHandMaterial = '{0}' WHERE RepresentID = '{1}'",
                                          leftHandMaterial, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }

            if (!string.IsNullOrEmpty(rightHandMesh))
            {
                sqlString = string.Format("UPDATE npc SET RightHandMesh = '{0}' WHERE RepresentID = '{1}'",
                                          rightHandMesh, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }

            if (!string.IsNullOrEmpty(rightHandMaterial))
            {
                sqlString = string.Format("UPDATE npc SET RightHandMaterial = '{0}' WHERE RepresentID = '{1}'",
                                          rightHandMaterial, npcRepresentID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }

            sqlString = string.Format("UPDATE npc SET S_LH_MeshFile = NULL, S_LH_MaterialFile = NULL, S_RH_MeshFile = NULL, S_RH_MaterialFile = NULL WHERE RepresentID = '{0}'",
                                             npcRepresentID);
            cmd.CommandText = sqlString;
            cmd.ExecuteNonQuery();
        }

        /// <summary>
        /// 更新插槽信息
        /// </summary>
        /// <param name="cmd">SqlCommand</param>
        /// <param name="animationID">动作ID</param>
        /// <param name="meshField">Mesh字段名</param>
        /// <param name="meshValue">Mesh值</param>
        /// <param name="materialField">Material字段名</param>
        /// <param name="materialValue">Material值</param>
        private void UpdateSlotInfo(SqlCommand cmd, string animationID, string meshField, string meshValue, string materialField, string materialValue)
        {
            if (!string.IsNullOrEmpty(meshValue))
            {
                string sqlString = string.Format("UPDATE animation_npc SET [{0}] = '{1}' WHERE RepresentID = '{2}' AND AnimationID = '{3}'",
                                                 meshField, meshValue, npcRepresentID.ToString(), animationID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                if (!string.IsNullOrEmpty(materialValue))
                {
                    sqlString = string.Format("UPDATE animation_npc SET [{0}] = '{1}' WHERE RepresentID = '{2}' AND AnimationID = '{3}'",
                                              materialField, materialValue, npcRepresentID.ToString(), animationID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
            }            
        }

        /// <summary>
        /// 更新插槽信息
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="animationID">动作ID</param>
        private void UpdateSlotInfo(SqlCommand cmd, string animationID)
        {
            UpdateSlotInfo(cmd, animationID, ""); // 左右手插糟及材质
            UpdateSlotInfo(cmd, animationID, "S_Face_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_Hat_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_LP_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_LC_MeshFile");                       
            UpdateSlotInfo(cmd, animationID, "S_RP_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_RP_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_Long_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_Spine_MeshFile");
            UpdateSlotInfo(cmd, animationID, "S_Spine2_MeshFile");
        }

        /// <summary>
        /// 更新插槽信息
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="animationID">动作ID</param>
        /// <param name="slotString">插槽描述字符串</param>
        private void UpdateSlotInfo(SqlCommand cmd, string animationID, string slotString)
        {
            string sqlString = null;

            switch(slotString)
            {
                case "0": // 原来武器不放在任何插槽上
                    {
                        break;
                    }
                case "1": // 插槽不变
                    {
                        UpdateSlotInfo(cmd, animationID, "S_LH_MeshFile", leftHandMesh, "S_LH_MaterialFile", leftHandMaterial);
                        UpdateSlotInfo(cmd, animationID, "S_RH_MeshFile", rightHandMesh, "S_RH_MaterialFile", rightHandMaterial);

                        break;
                    }
                case "": // 插槽不变
                    {
                        UpdateSlotInfo(cmd, animationID, "S_LH_MeshFile", leftHandMesh, "S_LH_MaterialFile", leftHandMaterial);
                        UpdateSlotInfo(cmd, animationID, "S_RH_MeshFile", rightHandMesh, "S_RH_MaterialFile", rightHandMaterial);

                        break;
                    }
                case "S_Face_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_Face_MeshFile", faceMesh, "S_Face_MaterialFile", faceMaterial);

                        break;
                    }
                case "S_Hat_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_Hat_MeshFile", hatMesh, "S_Hat_MaterialFile", hatMaterial);

                        break;
                    }
                case "S_LH_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_LH_MeshFile", lhMesh, "S_LH_MaterialFile", lhMaterial);

                        break;
                    }
                case "S_LP_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_LP_MeshFile", lpMesh, "S_LP_MaterialFile", lpMaterial);

                        break;
                    }
                case "S_LC_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_LC_MeshFile", lcMesh, "S_LC_MaterialFile", lcMaterial);

                        break;
                    }
                case "S_RH_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_RH_MeshFile", rhMesh, "S_RH_MaterialFile", rhMaterial);                        

                        break;
                    }
                case "S_RP_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_RP_MeshFile", rpMesh, "S_RP_MaterialFile", rpMaterial);

                        break;
                    }
                case "S_RC_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_RP_MeshFile", rcMesh, "S_RP_MaterialFile", rcMaterial);

                        break;
                    }
                case "S_Long_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_Long_MeshFile", longMesh, "S_Long_MaterialFile", longMaterial);

                        break;
                    }
                case "S_Spine_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_Spine_MeshFile", spineMesh, "S_Spine_MaterialFile", spineMaterial);

                        break;
                    }
                case "S_Spine2_MeshFile":
                    {
                        UpdateSlotInfo(cmd, animationID, "S_Spine2_MeshFile", spine2Mesh, "S_Spine2_MaterialFile", spine2Material);

                        break;
                    }
                default:
                    {
                        logText.Append(string.Format("Error：动作{0}的武器摆放插槽描述无效 {1}\r\n", animationID, slotString));
                        break;
                    }
            }
        }

        /// <summary>
        /// 选择武器
        /// </summary>
        /// <param name="isLeftHand">是否是左手插槽</param>
        /// <param name="modelFileString">MainModelFile</param>
        /// <returns>选择的武器路径</returns>
        private string SelectWeapon(bool isLeftHand, string modelFileString)
        {
            string weaponFilePath = null;
            WeaponSelectForm.WeaponType weaponType = WeaponSelectForm.WeaponType.RightHand;

            if (isLeftHand)
            {
                weaponType = WeaponSelectForm.WeaponType.LeftHand;
            }

            string editorID = null;

            if (!string.IsNullOrEmpty(modelFileString))
            {
                string[] tempArray = modelFileString.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
                editorID = tempArray[3];
            }

            WeaponSelectForm wForm = new WeaponSelectForm(conn, boneToActionTable, itemForInsertTable, modelTable, weaponType, editorID, modelFileString);

            if (wForm.ShowDialog() == DialogResult.OK)
            {
                weaponFilePath = wForm.WeaponFilePath;
            }

            return weaponFilePath;
        }

        /// <summary>
        /// 更新npc的AI技能时间
        /// </summary>
        /// <param name="cmd">sql命令</param>
        /// <param name="maxAttackTime">最大技能时间</param>
        private void UpdateAISkillAniFrame(SqlCommand cmd, int maxAttackTime)
        {
            string sqlString = null;
            DataRow[] rows = npcGlobeTable.Select(string.Format("ID = '{0}'", npcTemplateID));            

            if (rows.Length > 0)
            {
                DataRow row = rows[0];
                string skillID1 = row["SkillID1"].ToString();

                if (skillID1 == "" || skillID1 == "0") // 没填攻击动作，补充填上一个
                {
                    sqlString = string.Format("UPDATE NpcTemplate SET SkillID1 = 28, SkillLevel1 = 1, SkillType1 = 1, SkillInterval1 = 1024, SkillRate1 = 1024, SkillAniFrame1 = '{0}', SkillRestFrame1 = 10 WHERE ID = '{1}'",
                                              maxAttackTime.ToString(), npcTemplateID.ToString());
                    skillID1 = "28";
                }                
                else 
                {
                    sqlString = string.Format("UPDATE NpcTemplate SET SkillAniFrame1 = '{0}', SkillRestFrame1 = 10 WHERE ID = '{1}'",
                                              maxAttackTime.ToString(), npcTemplateID.ToString());
                }

                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                if (skillID1 == "28") // 技能间隔字段SkillInterval1填入值为SkillAniFrame1 + SkillRestFrame1
                {
                    int skillInterval = maxAttackTime + 10;
                    sqlString = string.Format("UPDATE NpcTemplate SET SkillInterval1 = '{0}' WHERE ID = '{1}'",
                                              skillInterval.ToString(), npcTemplateID.ToString());
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
            }
        }

        /// <summary>
        /// 武器缺失mesh后缀的补丁
        /// </summary>
        /// <param name="mapName">场景名</param>
        public void WeaponMeshFilePatch(string mapName)
        {
            DataRow[] rows = npcGlobeTable.Select(string.Format("MapName = '{0}'", mapName));

            string sqlString = "SELECT RepresentID, LeftHandMesh, RightHandMesh FROM npc";
            DataTable representTable = helper.GetDataTable(conn, sqlString);
            List<string> npcDisplayNameList = new List<string>();

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();

                foreach (DataRow row in rows)
                {
                    string representID = row["RepresentID1"].ToString();
                    string npcID = row["ID"].ToString();
                    string npcName = row["Name"].ToString();
                    string npcDisplayName = string.Format("{0} ({1})", npcName, npcID);

                    if (representID != "" && representID != "0")
                    {
                        DataRow[] dataRows = representTable.Select(string.Format("RepresentID = '{0}'", representID));

                        if (dataRows.Length > 0)
                        {
                            DataRow dataRow = dataRows[0];
                            string leftHandMesh = dataRow["LeftHandMesh"].ToString();
                            string rightHandMesh = dataRow["RightHandMesh"].ToString();
                            string newFileName = null;
                            bool updated = false;

                            if (leftHandMesh != "" && !leftHandMesh.EndsWith("Mesh"))
                            {
                                newFileName = string.Format("{0}.Mesh", leftHandMesh);
                                sqlString = string.Format("UPDATE npc SET LeftHandMesh = '{0}' WHERE RepresentID = '{1}'",
                                                          newFileName, representID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                sqlString = string.Format("UPDATE animation_npc SET S_LH_MeshFile = '{0}' WHERE RepresentID = '{1}'",
                                                          newFileName, representID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                updated = true;
                            }

                            if (rightHandMesh != "" && !rightHandMesh.EndsWith("Mesh"))
                            {
                                newFileName = string.Format("{0}.Mesh", rightHandMesh);
                                sqlString = string.Format("UPDATE npc SET RightHandMesh = '{0}' WHERE RepresentID = '{1}'",
                                                          newFileName, representID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                sqlString = string.Format("UPDATE animation_npc SET S_RH_MeshFile = '{0}' WHERE RepresentID = '{1}'",
                                                          newFileName, representID);
                                cmd.CommandText = sqlString;
                                cmd.ExecuteNonQuery();

                                updated = true;
                            }

                            if (updated && !npcDisplayNameList.Contains(npcDisplayName))
                            {
                                npcDisplayNameList.Add(npcDisplayName);
                            }
                        }
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在打武器缺失.Mesh后缀补丁时产生sql异常：" + ex.Message, "打补丁", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            StringBuilder updateInfo = new StringBuilder();
            updateInfo.AppendLine(string.Format("共更新{0}个npc：", npcDisplayNameList.Count.ToString()));
            updateInfo.AppendLine();
            foreach (string s in npcDisplayNameList)
            {
                updateInfo.AppendLine(s);
            }

            MessageBox.Show(updateInfo.ToString(), "更新结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
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
        /// 自动生成动作字符串
        /// </summary>
        /// <param name="animationFileName">动作文件名</param>
        /// <returns>动作字符串</returns>
        private string GetAnimationString(string animationFileName)
        {
            string animationString = string.Format("{0}{1}", relevatePath, animationFileName);

            if (animationFileName.EndsWith(".ani") || animationFileName.EndsWith(".tani")) // 已经有后缀名
            {
                // 不需要处理
            }
            else
            {
                string filePath = Path.Combine(rootDir, animationString);
                filePath = filePath + ".tani";

                if (File.Exists(filePath))
                {
                    animationString = animationString + ".tani";
                }
                else
                {
                    animationString = animationString + ".ani";
                }
            }            

            return animationString;
        }        

        /// <summary>
        /// 自动创建npc动作
        /// </summary>
        /// <param name="showMessage">是否显示提示信息</param>
        /// <param name="updateExistAnimation">是否更新已有动作</param>
        /// <param name="updateCommonStandbyAnimation">是否更新普通待机动作</param>
        /// <return>是否创建成功</return>
        private bool AutoCreateActions(bool showMessage, bool updateExistAnimation, bool updateCommonStandbyAnimation)
        {
            bool success = true;
            logText.Append(string.Format("开始初始化，npc表现ID为{0}。\r\n", npcRepresentID.ToString()));

            if (npcRepresentID == 0) // 检查表现ID是否有效
            {
                if (showMessage)
                {
                    MessageBox.Show("请先为当前NPC分配新RepresentID！", "更新动作",
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }

                logText.Append("Error：表现ID为空！\r\n");
                success = false;
            }
            else
            {
                string modelType = GetModelType(npcModel);

                if (modelType != null) // 输入有效
                {
                    string firstLetter = modelType.Substring(0, 1);
                    switch(firstLetter)
                    {
                        case "F": // player动作
                            {
                                relevatePath = string.Format(@"data\source\player\{0}\动作\", modelType);

                                break;
                            }
                        case "M": // player动作
                            {
                                relevatePath = string.Format(@"data\source\player\{0}\动作\", modelType);

                                break;
                            }
                        default: // npc动作
                            {
                                relevatePath = string.Format(@"data\source\NPC_source\{0}\动作\", modelType);

                                break;
                            }
                    }

                    success = UpdateAnimation(modelType, showMessage, updateExistAnimation, updateCommonStandbyAnimation);
                }
                else
                {
                    if (showMessage)
                    {
                        MessageBox.Show("模型文件路径不能为空！", "更新动作",
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }

                    logText.Append("Error：模型路径为空！\r\n");
                    success = false;
                }
            }

            return success;
        }

        /// <summary>
        /// 导出数据表
        /// </summary>
        /// <param name="rootPath">客户端目录</param>
        /// <param name="fields">数据列</param>
        /// <param name="values">数据行</param>
        public void ExportNpcFile(string rootPath, ArrayList fields, string[] values)
        {            
            StringBuilder content = new StringBuilder();
            StringBuilder headLine = new StringBuilder();
            List<int> excludeFiledIndexList = new List<int>();

            for (int i = 0; i < fields.Count; i++)
            {
                string fieldString = fields[i].ToString().TrimEnd(new char[] { '\r', '\n' });

                switch(fieldString)
                {
                    case "LongRangeWeaponType":
                        {                            
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "LeftHandMesh":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "LeftHandMaterial":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "RightHandMesh":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "RightHandMaterial":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "EditorID":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "Force":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    case "ForceDetail":
                        {
                            excludeFiledIndexList.Add(i);
                            break;
                        }
                    default:
                        {
                            headLine.Append(string.Format("{0}\t", fieldString));
                            break;
                        }
                }     
            }

            content.AppendLine(string.Format("{0}", headLine.ToString().Substring(0, headLine.Length - 1)));

            for (int i = 0; i < values.Length; i++)
            {
                string[] data = values[i].Split(new char[]{'\t'});
                StringBuilder line = new StringBuilder();

                for (int j = 0; j < data.Length; j++)
                {
                    if (!excludeFiledIndexList.Contains(j))
                    {
                        line.Append(string.Format("{0}\t", data[j]));
                    }                    
                }

                content.Append(string.Format("{0}\r\n", line.ToString().Substring(0, line.Length - 1)));
            }
                        
            string fileName = Path.Combine(rootPath, @"represent\npc\npc.txt");
            helper.SaveDataToFile(fileName, content.ToString().TrimEnd(new char[] { '\r', '\n' }));
        }
    }
}
