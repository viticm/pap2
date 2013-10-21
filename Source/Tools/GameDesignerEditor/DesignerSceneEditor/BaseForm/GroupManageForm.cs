using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using DevComponents.AdvTree;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	public partial class GroupManageForm: Form
	{
        private BaseDoc baseDoc; // BaseDoc
        private List<_AtlObjInfo> npcReviveGroupInfoList = new List<_AtlObjInfo>(); // npc重生组信息链表
        private List<_AtlObjInfo> doodadReviveGroupInfoList = new List<_AtlObjInfo>(); // doodad重生组信息链表
        private List<_AtlObjInfo> aiGroupInfoList = new List<_AtlObjInfo>(); // 仇恨组信息链表
        private List<_AtlObjInfo> randomGroupInfoList = new List<_AtlObjInfo>(); // 随机组信息链表
        private List<string[]> aiParameterGroupInfoList = new List<string[]>(); // AI参数组信息链表

        private List<Hashtable> npcInfoList = new List<Hashtable>(); // npc数据链表
        private List<Hashtable> doodadInfoList = new List<Hashtable>(); // doodad数据链表

        // 供GetObjDisplayInfo用的临时变量。
        int logicObj = 0;
        int representObj = 0;
        int hasScript = 0;
        string name = string.Empty;
        string nickName = string.Empty;
        int templateID = -1;


        /// <summary>
        /// 构造函数
        /// </summary>
		public GroupManageForm()
		{
			InitializeComponent();
		}

        /// <summary>
        /// BaseDoc
        /// </summary>
        public BaseDoc CurrentBaseDoc
        {
            set
            {
                baseDoc = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        public void Init()
        {
            InitReviveGroup();
            FillReviveGroup();
            
            InitHatredGroup();
            FillHatredGroup();
            
            InitRandomGroup();
            FillRandomGroup();

            InitAIParameterGroup();
            FillAIParameterGroup();

            InitMemberInfoList("NPC", npcInfoList);
            InitMemberInfoList("Doodad", doodadInfoList);

            // 初始化数据列
            DataGridViewTextBoxColumn groupInfoNameColumn = new DataGridViewTextBoxColumn();
            groupInfoNameColumn.HeaderText = "分组参数";
            groupInfoNameColumn.Name = "GroupInfoName";
            groupInfoNameColumn.ReadOnly = true;

            DataGridViewTextBoxColumn groupInfoValueColumn = new DataGridViewTextBoxColumn();
            groupInfoValueColumn.HeaderText = "参数值";
            groupInfoValueColumn.Name = "GroupInfoValue";

            groupView.Columns.Add(groupInfoNameColumn);
            groupView.Columns.Add(groupInfoValueColumn);

            DataGridViewTextBoxColumn npcNameColumn = new DataGridViewTextBoxColumn();
            npcNameColumn.HeaderText = "名称";
            npcNameColumn.Name = "NpcName";

            DataGridViewTextBoxColumn npcTemplateIDColumn = new DataGridViewTextBoxColumn();
            npcTemplateIDColumn.HeaderText = "模板ID";
            npcTemplateIDColumn.Name = "NpcTemplateID";

            DataGridViewTextBoxColumn npcIndexColumn = new DataGridViewTextBoxColumn();
            npcIndexColumn.HeaderText = "编号";
            npcIndexColumn.Name = "NpcIndex";

            npcView.Columns.Add(npcNameColumn);
            npcView.Columns.Add(npcTemplateIDColumn);
            npcView.Columns.Add(npcIndexColumn);
        }

        /// <summary>
        /// 初始化成员信息链表
        /// </summary>
        /// <param name="objectType">对象类型</param>
        /// <param name="memberInfoList">成员信息链表</param>
        private void InitMemberInfoList(string objectType, List<Hashtable> memberInfoList)
        {
            int logicObj = 0;
            int representObj = 0;
            int hasScript = 0;
            string name = "";
            string nickName = "";
            int memberCount = 0;

            memberInfoList.Clear();
            baseDoc.DocLogical.GetObjCount(objectType, 0, ref memberCount);

            for (int i = 0; i < memberCount; i++)
            {
                _AtlObjInfo objectInfo = new _AtlObjInfo();
                baseDoc.DocLogical.GetObjDisplayInfo(objectType, i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);

                if (logicObj != 0) // 貌似无效的成员也会读到
                {
                    baseDoc.DocLogical.GetSetObjInfo(objectType, ref objectInfo, logicObj, 1);
                    Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                    memberInfoList.Add(infoTable);
                }
            }
        }

        /// <summary>
        /// 初始化分组链表
        /// </summary>
        /// <param name="groupName">分组名称</param>
        /// <param name="groupInfoList">分组链表</param>
        private void InitGroupInfoList(string groupName, List<_AtlObjInfo> groupInfoList)
        {
            int groupCount = 0;            
            _AtlObjInfo objectInfo = new _AtlObjInfo();

            groupInfoList.Clear();
            baseDoc.DocLogical.GetObjCount(groupName, 0, ref groupCount);            

            for (int i = 0; i < groupCount; i++)
            {
                baseDoc.DocLogical.GetObjDisplayInfo(groupName, i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                baseDoc.DocLogical.GetSetObjInfo(groupName, ref objectInfo, logicObj, 1);
                groupInfoList.Add(objectInfo);
            }
        }

        /// <summary>
        /// 初始化仇恨组
        /// </summary>
        private void InitHatredGroup()
        {
            InitGroupInfoList("AIGroup", aiGroupInfoList);
        }

        /// <summary>
        /// 初始化随机组
        /// </summary>
        private void InitRandomGroup()
        {
            InitGroupInfoList("NpcRandomGroup", randomGroupInfoList);
        }

        /// <summary>
        /// 初始化AI参数组
        /// </summary>
        private void InitAIParameterGroup()
        {
            string info = "";

            aiParameterGroupInfoList.Clear();
            baseDoc.DocLogical.GetAIParameterGroupInfo(ref info);

            string[] infoArray = info.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);

            foreach (string s in infoArray)
            {
                aiParameterGroupInfoList.Add(s.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries));
            }
        }

        /// <summary>
        /// 初始化重生组
        /// </summary>
        private void InitReviveGroup()
        {
            InitGroupInfoList("NpcReviveGroup", npcReviveGroupInfoList);
            InitGroupInfoList("DoodadReviveGroup", doodadReviveGroupInfoList);
        }

        /// <summary>
        /// 填充分组分类树
        /// </summary>
        /// <param name="idField">分组ID字段名</param>
        /// <param name="nameField">分组名称字段名</param>
        /// <param name="groupInfoList">分组信息链表</param>
        /// <param name="groupTree">分组树</param>
        private void FillGroupTree(string idField, string nameField, List<_AtlObjInfo> groupInfoList, AdvTree groupTree)
        {
            groupTree.Nodes.Clear();

            foreach (_AtlObjInfo objectInfo in groupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable[idField] as string;
                string groupName = infoTable[nameField] as string;
                Node newNode = new Node();
                newNode.Text = string.Format("{0} ({1})", groupName, groupID);
                newNode.Tag = infoTable;

                groupTree.Nodes.Add(newNode);
            }
        }

        /// <summary>
        /// 填充仇恨组
        /// </summary>
        private void FillHatredGroup()
        {
            FillGroupTree("dwSetID", "szName", aiGroupInfoList, aiTree);
        }

        /// <summary>
        /// 填充随机组
        /// </summary>
        private void FillRandomGroup()
        {
            FillGroupTree("dwGroupID", "szName", randomGroupInfoList, randomTree);
        }

        /// <summary>
        /// 填充AI参数组
        /// </summary>
        private void FillAIParameterGroup()
        {
            aiParameterTree.Nodes.Clear();

            for (int i = 0; i < aiParameterGroupInfoList.Count; i++)
            {
                string[] infoArray = aiParameterGroupInfoList[i];

                Node newNode = new Node();

                string[] nodeInfo = infoArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                newNode.Text = string.Format("{0} ({1})", nodeInfo[0], nodeInfo[1]);

                List<string[]> npcInfo = new List<string[]>();

                for (int j = 1; j < infoArray.Length; j++)
                {
                    npcInfo.Add(infoArray[j].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries));
                }

                newNode.Tag = npcInfo;
                aiParameterTree.Nodes.Add(newNode);
            }
        }

        /// <summary>
        /// 填充重生组
        /// </summary>
        private void FillReviveGroup()
        {
            reviveTree.Nodes.Clear();

            Node npcReviveNode = new Node();
            npcReviveNode.Text = "NPC";
            reviveTree.Nodes.Add(npcReviveNode);

            foreach(_AtlObjInfo objectInfo in npcReviveGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;
                Node newNode = new Node();
                newNode.Text = string.Format("{0} ({1})", groupName, groupID);
                newNode.Tag = infoTable;

                npcReviveNode.Nodes.Add(newNode);
            }

            Node doodadReviveNode = new Node();
            doodadReviveNode.Text = "Doodad";
            reviveTree.Nodes.Add(doodadReviveNode);

            foreach (_AtlObjInfo objectInfo in doodadReviveGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;
                Node newNode = new Node();
                newNode.Text = string.Format("{0} ({1})", groupName, groupID);
                newNode.Tag = infoTable;

                doodadReviveNode.Nodes.Add(newNode);
            }
        }

        /// <summary>
        /// 更新仇恨组
        /// </summary>
        /// <return>是否更新成功</return>
        private bool UpdateHatredGroup()
        {
            bool updateSuccess = false;
            Node currentNode = aiTree.SelectedNode;

            if (currentNode != null)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;
                int groupID = int.Parse(infoTable["dwSetID"] as string);
                string groupName = groupView.Rows[0].Cells["GroupInfoValue"].Value as string;

                if (!string.IsNullOrEmpty(groupName))
                {
                    baseDoc.DocLogical.DecoratedSetAIGroup(groupID, groupName);

                    infoTable["szName"] = groupName;
                    currentNode.Text = string.Format("{0} ({1})", groupName, groupID.ToString());
                    updateSuccess = true;
                }
                else
                {
                    MessageBox.Show("分组数据参数不能为空！", "更新分组数据", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }
            }

            return updateSuccess;
        }

        /// <summary>
        /// 更新随机组
        /// </summary>
        /// <return>是否更新成功</return>
        private bool UpdateRandomGroup()
        {
            bool updateSuccess = false;
            Node currentNode = randomTree.SelectedNode;

            if (currentNode != null)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;
                int groupID = int.Parse(infoTable["dwGroupID"] as string);
                string groupName = groupView.Rows[0].Cells["GroupInfoValue"].Value as string;
                string groupInfo = groupView.Rows[1].Cells["GroupInfoValue"].Value as string;

                if (!string.IsNullOrEmpty(groupName) && !string.IsNullOrEmpty(groupInfo))
                {
                    baseDoc.DocLogical.DecoratedSetRandomGroup(groupID, groupName, groupInfo);

                    infoTable["szName"] = groupName;
                    currentNode.Text = string.Format("{0} ({1})", groupName, groupID.ToString());
                    updateSuccess = true;
                }
                else
                {
                    MessageBox.Show("分组数据参数不能为空！", "更新分组数据", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }
            }

            return updateSuccess;
        }

        /// <summary>
        /// 更新重生组
        /// </summary>
        private bool UpdateReviveGroup()
        {
            bool updateSuccess = false;
            Node currentNode = reviveTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;
                int groupID = int.Parse(infoTable["dwGroupID"] as string);
                string groupName = groupView.Rows[0].Cells["GroupInfoValue"].Value as string;
                string maxString = groupView.Rows[1].Cells["GroupInfoValue"].Value as string;
                string minString = groupView.Rows[2].Cells["GroupInfoValue"].Value as string;

                if (!string.IsNullOrEmpty(groupName) && !string.IsNullOrEmpty(maxString) && !string.IsNullOrEmpty(minString))
                {
                    int maxCount = int.Parse(maxString);
                    int minCount = int.Parse(minString);

                    switch (currentNode.Parent.Text)
                    {
                        case "NPC":
                            {
                                baseDoc.DocLogical.DecoratedSetNpcReviveGroup(groupID, groupName, minCount, maxCount);
                                break;
                            }
                        case "Doodad":
                            {
                                baseDoc.DocLogical.DecoratedSetDoodadReviveGroup(groupID, groupName, minCount, maxCount);
                                break;
                            }
                    }
                    
                    infoTable["szName"] = groupName;
                    infoTable["nMinNpcCount"] = minString;
                    infoTable["nMaxNpcCount"] = maxString;
                    currentNode.Text = string.Format("{0} ({1})", groupName, groupID.ToString());
                    updateSuccess = true;                    
                }
                else
                {
                    MessageBox.Show("分组数据参数不能为空！", "更新分组数据", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }
            }

            return updateSuccess;
        }

        /// <summary>
        /// 删除重生组
        /// </summary>        
        private void DeleteReviveGroup()
        {
            Node currentNode = reviveTree.SelectedNode;

            if (currentNode != null)
            {
                if (currentNode.Level == 1)
                {
                    if (MessageBox.Show("确定要删除选中的分组吗？", "删除分组", MessageBoxButtons.OKCancel,
                                        MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        Hashtable infoTable = currentNode.Tag as Hashtable;
                        int groupID = int.Parse(infoTable["dwGroupID"] as string);

                        switch (currentNode.Parent.Text)
                        {
                            case "NPC":
                                {
                                    baseDoc.DocLogical.DecoratedDeleteNpcReviveGroup(groupID);
                                    break;
                                }
                            case "Doodad":
                                {
                                    baseDoc.DocLogical.DecoratedDeleteDoodadReviveGroup(groupID);
                                    break;
                                }
                        }
                    }

                    Node parentNode = currentNode.Parent;
                    parentNode.Nodes.Remove(currentNode);                    
                }
                else
                {
                    MessageBox.Show("不能删除分类结点！", "删除分组", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
        }

        /// <summary>
        /// 删除仇恨组
        /// </summary>
        private void DeleteHatredGroup()
        {
            Node currentNode = aiTree.SelectedNode;

            if (currentNode != null)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;
                string groupID = infoTable["dwSetID"] as string;

                // 先更新所有NPC的分组信息
                string groupField = "TeamID";
                int npcCount = 0;
                baseDoc.DocLogical.GetObjCount("NPC", 0, ref npcCount);

                for (int i = 0; i < npcCount; i++)
                {
                    _AtlObjInfo objectInfo = new _AtlObjInfo();
                    baseDoc.DocLogical.GetObjDisplayInfo("NPC", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    baseDoc.DocLogical.GetSetObjInfo("NPC", ref objectInfo, logicObj, 1);

                    for (int j = 0; j < objectInfo.strFields.Length; j++)
                    {
                        string fieldName = objectInfo.strFields[j];

                        if (fieldName == groupField)
                        {
                            if (objectInfo.strValues[j] == groupID)
                            {
                                objectInfo.strValues[j] = "0";
                                objectInfo.bChanged = 1;
                                baseDoc.DocLogical.GetSetObjInfo("NPC", ref objectInfo, logicObj, 0);
                            }

                            break;
                        }
                    }
                }

                // 再删除该分组
                baseDoc.DocLogical.DecoratedDeleteAIGroup(int.Parse(groupID));
                aiTree.Nodes.Remove(currentNode);  
            }
        }

        /// <summary>
        /// 删除随机组
        /// </summary>
        private void DeleteRandomGroup()
        {
            Node currentNode = randomTree.SelectedNode;

            if (currentNode != null)
            {
                if (currentNode.Level == 1)
                {
                    if (MessageBox.Show("确定要删除选中的分组吗？", "删除分组", MessageBoxButtons.OKCancel,
                                        MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        Hashtable infoTable = currentNode.Tag as Hashtable;
                        int groupID = int.Parse(infoTable["dwGroupID"] as string);

                        baseDoc.DocLogical.DecoratedDeleteRandomGroup(groupID);
                    }

                    Node parentNode = currentNode.Parent;
                    parentNode.Nodes.Remove(currentNode);
                }
                else
                {
                    MessageBox.Show("不能删除分类结点！", "删除分组", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        /// <summary>
        /// 获取NPC的信息数据表
        /// </summary>
        /// <param name="npcIndex">npc编号</param>
        /// <returns>信息数据表</returns>
        private Hashtable GetNpcInfoTable(string npcIndex)
        {
            Hashtable npcInfo = null;

            foreach (Hashtable infoTable in npcInfoList)
            {
                if (infoTable["nIndex"] as string == npcIndex)
                {
                    npcInfo = infoTable;
                    break;
                }
            }

            return npcInfo;
        }

        /// <summary>
        /// 显示成员信息
        /// </summary>
        /// <param name="memberInfoList">成员信息链表</param>
        /// <param name="fieldName">字段名</param>
        /// <param name="fieldValue">字段值</param>
        private void ShowMemberInfo(List<Hashtable> memberInfoList, string fieldName, string fieldValue)
        {
            for (int i = 0; i < memberInfoList.Count; i++)
            {
                Hashtable infoTable = memberInfoList[i];

                string memberName = infoTable["szName"] as string;
                string memberTemplateID = infoTable["nTempleteID"] as string;
                string memberIndex = infoTable["nIndex"] as string;
                string currentFieldValue = infoTable[fieldName] as string;

                if (currentFieldValue == fieldValue)
                {
                    npcView.Rows.Add(1);
                    DataGridViewRow newRow = npcView.Rows[npcView.Rows.Count - 1];
                    newRow.Cells["NpcName"].Value = memberName;
                    newRow.Cells["NpcTemplateID"].Value = memberTemplateID;
                    newRow.Cells["NpcIndex"].Value = memberIndex;
                }
            }
        }

        /// <summary>
        /// 获取AI参数集合的ID
        /// </summary>
        /// <param name="nodeText">文本</param>
        /// <returns>ID</returns>
        private int GetAIParameterSetID(string nodeText)
        {            
            int index = nodeText.LastIndexOf("(");
            string idString = nodeText.Substring(index + 1, nodeText.Length - index - 2);
            int aiSetID = int.Parse(idString);

            return aiSetID;
        }

        /// <summary>
        /// 显示分组的成员信息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void ShowGroupMember(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode;
            Hashtable infoTable;
            string groupID;
            string groupName;
            DataGridViewRow newRow;

            groupView.Rows.Clear();
            npcView.Rows.Clear();

            switch(tabControl1.SelectedTab.Text)
            {
                case "重生组":
                    {
                        currentNode = reviveTree.SelectedNode;

                        if (currentNode != null && currentNode.Level == 1)
                        {
                            infoTable = currentNode.Tag as Hashtable;
                            groupID = infoTable["dwGroupID"] as string;
                            groupName = infoTable["szName"] as string;
                            string minCount = infoTable["nMinNpcCount"] as string;
                            string maxCount = infoTable["nMaxNpcCount"] as string;
                            string randomSeed = infoTable["nRandom"] as string;

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[0];
                            newRow.Cells["GroupInfoName"].Value = "名称";
                            newRow.Cells["GroupInfoValue"].Value = groupName;

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[1];
                            newRow.Cells["GroupInfoName"].Value = "MaxCount";
                            newRow.Cells["GroupInfoValue"].Value = maxCount;

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[2];
                            newRow.Cells["GroupInfoName"].Value = "MinCount";
                            newRow.Cells["GroupInfoValue"].Value = minCount;

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[3];
                            newRow.Cells["GroupInfoName"].Value = "Random";
                            newRow.Cells["GroupInfoValue"].Value = randomSeed;
                            newRow.ReadOnly = true; // Random不能被编辑

                            switch (currentNode.Parent.Text)
                            {
                                case "NPC":
                                    {
                                        ShowMemberInfo(npcInfoList, "ReliveID", groupID);
                                        break;
                                    }
                                case "Doodad":
                                    {
                                        ShowMemberInfo(doodadInfoList, "ReliveID", groupID);
                                        break;
                                    }
                                default:
                                    {
                                        break;
                                    }
                            }                            
                        }

                        break;
                    }
                case "仇恨组":
                    {
                        currentNode = aiTree.SelectedNode;

                        if (currentNode != null)
                        {
                            infoTable = currentNode.Tag as Hashtable;
                            groupID = infoTable["dwSetID"] as string;
                            groupName = infoTable["szName"] as string;

                            string infoString = "";
                            baseDoc.DocLogical.GetAIGroupInfo(int.Parse(groupID), ref infoString);

                            string[] dataArray = infoString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                            int currentIndex = 0;

                            foreach (string s in dataArray)
                            {
                                Hashtable npcInfoTable = GetNpcInfoTable(s);

                                if (npcInfoTable != null)
                                {
                                    string npcName = npcInfoTable["szName"] as string;
                                    string npcTemplateID = npcInfoTable["nTempleteID"] as string;
                                    string npcIndex = npcInfoTable["nIndex"] as string;
                                    
                                    npcView.Rows.Add(1);
                                    newRow = npcView.Rows[currentIndex];
                                    newRow.Cells["NpcName"].Value = npcName;
                                    newRow.Cells["NpcTemplateID"].Value = npcTemplateID;
                                    newRow.Cells["NpcIndex"].Value = npcIndex;
                                    currentIndex++;                                    
                                }
                            }

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[0];
                            newRow.Cells["GroupInfoName"].Value = "名称";
                            newRow.Cells["GroupInfoValue"].Value = groupName;
                        }

                        break;
                    }
                case "随机组":
                    {
                        currentNode = randomTree.SelectedNode;

                        if (currentNode != null)
                        {
                            infoTable = currentNode.Tag as Hashtable;
                            groupID = infoTable["dwGroupID"] as string;
                            groupName = infoTable["szName"] as string;

                            ShowMemberInfo(npcInfoList, "RandomID", groupID);

                            string infoString = "";
                            baseDoc.DocLogical.GetRandomGroupInfo(int.Parse(groupID), ref infoString);

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[0];
                            newRow.Cells["GroupInfoName"].Value = "名称";
                            newRow.Cells["GroupInfoValue"].Value = groupName;

                            groupView.Rows.Add(1);
                            newRow = groupView.Rows[1];
                            newRow.Cells["GroupInfoName"].Value = "模板ID";
                            newRow.Cells["GroupInfoValue"].Value = infoString;
                        }                        

                        break;
                    }
                case "AI参数组":
                    {
                        currentNode = aiParameterTree.SelectedNode;

                        if (currentNode != null)
                        {
                            List<string[]> infoList = currentNode.Tag as List<string[]>;
                            int currentIndex = 0;

                            foreach (string[] infoArray in infoList)
                            {
                                string npcName = infoArray[0];
                                string npcTemplateID = infoArray[2];
                                string npcIndex = infoArray[1];

                                npcView.Rows.Add(1);
                                newRow = npcView.Rows[currentIndex];
                                newRow.Cells["NpcName"].Value = npcName;
                                newRow.Cells["NpcTemplateID"].Value = npcTemplateID;
                                newRow.Cells["NpcIndex"].Value = npcIndex;
                                currentIndex++;
                            }                            

                            int aiSetID = GetAIParameterSetID(currentNode.Text);

                            string infoString = "";
                            baseDoc.DocLogical.GetAIParameterSetInfo(aiSetID, ref infoString);
                            string[] tempArray = infoString.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                            foreach (string s in tempArray)
                            {
                                string[] dataArray = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);                                    

                                groupView.Rows.Add(1);
                                newRow = groupView.Rows[groupView.Rows.Count - 1];
                                newRow.Cells["GroupInfoName"].Value = dataArray[0];
                                newRow.Cells["GroupInfoValue"].Value = dataArray[1];
                                newRow.ReadOnly = true;
                            }                            
                        }

                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }

        /// <summary>
        ///  切换分组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tabControl1_SelectedTabChanged(object sender, DevComponents.DotNetBar.TabStripTabChangedEventArgs e)
        {
            groupView.Rows.Clear();
            npcView.Rows.Clear();
        }

        /// <summary>
        /// 删除分组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteGroup_Click(object sender, EventArgs e)
        {
            switch (tabControl1.SelectedTab.Text)
            {
                case "重生组":
                    {
                        DeleteReviveGroup();
                        break;
                    }
                case "仇恨组":
                    {
                        DeleteHatredGroup();
                        break;
                    }
                case "随机组":
                    {
                        DeleteRandomGroup();
                        break;
                    }
                case "AI参数组":
                    {                        
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }

        /// <summary>
        /// 更新分组数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bUpdateGroup_Click(object sender, EventArgs e)
        {
            bool updateSuccess = false;

            switch (tabControl1.SelectedTab.Text)
            {
                case "重生组": // 重生组
                    {                        
                        updateSuccess = UpdateReviveGroup();
                        break;
                    }
                case "仇恨组": // 仇恨组
                    {
                        updateSuccess = UpdateHatredGroup();
                        break;
                    }
                case "随机组": // 随机组
                    {
                        updateSuccess = UpdateRandomGroup();
                        break;
                    }
                case "AI参数组":
                    {
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            if (updateSuccess)
            {
                MessageBox.Show("分组数据更新成功！", "更新分组数据", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }
	}
}