using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	public partial class GroupForm: Form
	{        
        private enum GroupType { HatredGroup, RandomGroup, ReviveGroup }; // 分组类型        
        private string objectType = "NPC"; // 对象类型
        private BaseDoc baseDoc; // BaseDoc
        private List<_AtlObjInfo> objectInfoList = new List<_AtlObjInfo>(); // 对象信息链表
        private List<_AtlObjInfo> reviveGroupInfoList = new List<_AtlObjInfo>(); // 重生组信息链表
        private List<_AtlObjInfo> aiGroupInfoList = new List<_AtlObjInfo>(); // 仇恨组信息链表
        private List<_AtlObjInfo> randomGroupInfoList = new List<_AtlObjInfo>(); // 随机组信息链表        
        private List<_AtlObjInfo> allGroupInfoList = new List<_AtlObjInfo>(); // 所有组信息链表 

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
		public GroupForm()
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

            get
            {
                return baseDoc;
            }
        }

        /// <summary>
        /// 对象信息链表
        /// </summary>
        public List<_AtlObjInfo> ObjectInfoList
        {
            set
            {
                objectInfoList = value;
            }
        }

        /// <summary>
        /// 对象类型
        /// </summary>
        public string ObjectType
        {
            set
            {
                objectType = value;
            }
        }    
        
        /// <summary>
        /// 初始化
        /// </summary>
        public void Init()
        {                       
            switch(objectType)
            {
                case "NPC":
                    {                        
                        break;
                    }
                case "Doodad":
                    {
                        tabControlPanel1.Enabled = false;
                        tabControlPanel2.Enabled = false;
                        break;
                    }
            }

            InitHatredGroup();
            InitRandomGroup();
            InitReviveGroup();

            FillHatredGroup();
            FillAllGroup();
        }                

        /// <summary>
        /// 清除当前选中NPC的分组信息
        /// </summary>
        /// <param name="fieldName">分组字段</param>
        private void ClearGroupInfo(string fieldName)
        {
            int logicObj = 0;
            int representObj = 0;
            int hasScript = 0;
            string name = "";
            string nickName = "";

            for (int i = 0; i < objectInfoList.Count; i++)
            {
                _AtlObjInfo objectInfo = objectInfoList[i];
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                int nIndex = int.Parse(infoTable["nIndex"] as string);

                for (int j = 0; j < objectInfo.strFields.Length; j++)
                {
                    if (objectInfo.strFields[j] == fieldName)
                    {
                        objectInfo.strValues[j] = "0";
                        objectInfo.bChanged = 1;
                        baseDoc.DocLogical.GetObjDisplayInfo(objectType, nIndex, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                        baseDoc.DocLogical.GetSetObjInfo(objectType, ref objectInfo, logicObj, 0);

                        break;
                    }
                }

                switch (fieldName)
                {
                    case "TeamID": // 仇恨组
                        {
                            baseDoc.DocLogical.DecoratedRemoveNPCFromAIGroup(nIndex);
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
        /// <summary>
        /// 设置当前选中的NPC的分组信息
        /// </summary>        
        /// <param name="fieldName">分组字段</param>
        /// <param name="groupID">分组ID</param>
        private void SetGroupInfo(string fieldName, string groupID)
        {
            int logicObj = 0;
            int representObj = 0;
            int hasScript = 0;
            string name = "";
            string nickName = "";

            for (int i = 0; i < objectInfoList.Count; i++)
            {
                _AtlObjInfo objectInfo = objectInfoList[i];
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                int nIndex = int.Parse(infoTable["nIndex"] as string);

                for (int j = 0; j < objectInfo.strFields.Length; j++)
                {
                    if (objectInfo.strFields[j] == fieldName)
                    {
                        bool reqireUpdate = false;

                        if (objectInfo.strValues[j] == "0" || objectInfo.strValues[j] == "")
                        {
                            reqireUpdate = true;
                        }
                        else
                        {
                            if (objectInfo.strValues[j] != groupID && checkBoxX1.Checked)
                            {
                                reqireUpdate = true;
                            }
                        }

                        if (reqireUpdate)
                        {
                            objectInfo.strValues[j] = groupID;
                            objectInfo.bChanged = 1;
                            baseDoc.DocLogical.GetObjDisplayInfo(objectType, nIndex, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                            baseDoc.DocLogical.GetSetObjInfo(objectType, ref objectInfo, logicObj, 0);
                        }

                        break;
                    }
                }
            }
        }

        /// <summary>
        /// 设置分组
        /// </summary>
        /// <param name="groupID">分组ID</param>
        /// <param name="groupType">分组类型</param>
        private void SetGroup(int groupID, GroupType groupType)
        {            
            switch (groupType)
            {
                case GroupType.HatredGroup: // 仇恨组
                    {
                        SetGroupInfo("TeamID", groupID.ToString());

                        for (int i = 0; i < objectInfoList.Count; i++)
                        {
                            _AtlObjInfo objectInfo = objectInfoList[i];
                            Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                            int nIndex = int.Parse(infoTable["nIndex"] as string);

                            baseDoc.DocLogical.AddNpcToAIGroup(nIndex, groupID);                            
                        }

                        break;
                    }
                case GroupType.RandomGroup: // 随机组
                    {
                        SetGroupInfo("RandomID", groupID.ToString());                       
                        break;
                    }
                case GroupType.ReviveGroup: // 重生组
                    {
                        SetGroupInfo("ReliveID", groupID.ToString());
                        break;
                    }
            }
        }

        /// <summary>
        /// 初始化仇恨组
        /// </summary>
        private void InitHatredGroup()
        {
            int aiGroupCount = 0;
            
            aiGroupInfoList.Clear();
            baseDoc.DocLogical.GetObjCount("AIGroup", 0, ref aiGroupCount);

            if (aiGroupCount > 0)
            {
                _AtlObjInfo objectInfo = new _AtlObjInfo();

                for (int i = 0; i < aiGroupCount; i++)
                {
                    baseDoc.DocLogical.GetObjDisplayInfo("AIGroup", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    baseDoc.DocLogical.GetSetObjInfo("AIGroup", ref objectInfo, logicObj, 1);
                    aiGroupInfoList.Add(objectInfo);
                }
            }                       
        }

        /// <summary>
        /// 初始化随机组
        /// </summary>
        private void InitRandomGroup()
        {
            int randomGroupCount = 0;

            randomGroupInfoList.Clear();
            baseDoc.DocLogical.GetObjCount("NpcRandomGroup", 0, ref randomGroupCount);

            if (randomGroupCount > 0)
            {
                _AtlObjInfo objectInfo = new _AtlObjInfo();

                for (int i = 0; i < randomGroupCount; i++)
                {
                    baseDoc.DocLogical.GetObjDisplayInfo("NpcRandomGroup", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    baseDoc.DocLogical.GetSetObjInfo("NpcRandomGroup", ref objectInfo, logicObj, 1);
                    randomGroupInfoList.Add(objectInfo);
                }
            }
        }
        
        /// <summary>
        /// 初始化重生组
        /// </summary>
        private void InitReviveGroup()
        {
            int reviveGroupCount = 0;
            string groupName = "NpcReviveGroup";

            switch(objectType)
            {
                case "NPC":
                    {                        
                        break;
                    }
                case "Doodad":
                    {
                        groupName = "DoodadReviveGroup";
                        break;
                    }
            }

            reviveGroupInfoList.Clear();
            baseDoc.DocLogical.GetObjCount(groupName, 0, ref reviveGroupCount);
            if (reviveGroupCount > 0)
            {
                _AtlObjInfo objectInfo = new _AtlObjInfo();

                for (int i = 0; i < reviveGroupCount; i++)
                {
                    baseDoc.DocLogical.GetObjDisplayInfo(groupName, i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    baseDoc.DocLogical.GetSetObjInfo(groupName, ref objectInfo, logicObj, 1);                    
                    reviveGroupInfoList.Add(objectInfo);                                        
                }
            }
        }
        
        /// <summary>
        /// 填充仇恨组
        /// </summary>
        private void FillHatredGroup()
        {
            comboBoxEx1.Items.Clear();

            foreach (_AtlObjInfo objectInfo in aiGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwSetID"] as string;
                string groupName = infoTable["szName"] as string;
                comboBoxEx1.Items.Add(string.Format("{0} ({1})", groupName, groupID));
            }
        }

        /// <summary>
        /// 填充随机组
        /// </summary>
        private void FillRandomGroup()
        {
            comboBoxEx1.Items.Clear();

            foreach (_AtlObjInfo objectInfo in randomGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;
                comboBoxEx1.Items.Add(string.Format("{0} ({1})", groupName, groupID));
            }
        }
        
        /// <summary>
        /// 填充重生组
        /// </summary>
        private void FillReviveGroup()
        {            
            comboBoxEx1.Items.Clear();

            foreach(_AtlObjInfo objectInfo in reviveGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;
                comboBoxEx1.Items.Add(string.Format("{0} ({1})", groupName, groupID));
            }
        }

        /// <summary>
        /// 填充所有分组
        /// </summary>
        private void FillAllGroup()
        {
            allGroupInfoList.Clear();
            comboBoxEx2.Items.Clear();            

            // 填充仇恨组
            foreach (_AtlObjInfo objectInfo in aiGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwSetID"] as string;
                string groupName = infoTable["szName"] as string;

                allGroupInfoList.Add(objectInfo);
                comboBoxEx2.Items.Add(string.Format("仇恨组 —— {0} ({1})", groupName, groupID));
            }

            // 填充随机组
            foreach(_AtlObjInfo objectInfo in randomGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;

                allGroupInfoList.Add(objectInfo);
                comboBoxEx2.Items.Add(string.Format("随机组 —— {0} ({1})", groupName, groupID));
            }

            // 填充重生组
            foreach(_AtlObjInfo objectInfo in reviveGroupInfoList)
            {
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                string groupID = infoTable["dwGroupID"] as string;
                string groupName = infoTable["szName"] as string;

                allGroupInfoList.Add(objectInfo);
                comboBoxEx2.Items.Add(string.Format("重生组 —— {0} ({1})", groupName, groupID));
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(comboBoxEx1.SelectedIndex == -1 && comboBoxEx2.SelectedIndex == -1) // 没有选择分组
            {
                MessageBox.Show("请先选择要设置的分组！", "设置分组", MessageBoxButtons.OK, MessageBoxIcon.Information);                
            }
            else
            {
                GroupType groupType = GroupType.ReviveGroup;
                _AtlObjInfo objectInfo;
                int groupID = 0;

                if(comboBoxEx1.SelectedIndex != -1)
                {                   
                    switch(tabControl1.SelectedTab.Text)
                    {
                        case "仇恨组":
                            {            
                                groupType = GroupType.HatredGroup;
                                objectInfo = aiGroupInfoList[comboBoxEx1.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                                groupID = int.Parse(infoTable["dwSetID"] as string);

                                break;
                            }
                        case "随机组":
                            {
                                groupType = GroupType.RandomGroup;
                                objectInfo = randomGroupInfoList[comboBoxEx1.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                                groupID = int.Parse(infoTable["dwGroupID"] as string);

                                break;
                            }
                        case "重生组":
                            {
                                groupType = GroupType.ReviveGroup;
                                objectInfo = reviveGroupInfoList[comboBoxEx1.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);                                
                                groupID = int.Parse(infoTable["dwGroupID"] as string);

                                break;
                            }
                    }
                }
                else
                {
                    string[] data = comboBoxEx2.Text.Split(new char[] { ' ' });

                    switch(data[0])
                    {
                        case "仇恨组":
                            {
                                groupType = GroupType.HatredGroup;
                                objectInfo = allGroupInfoList[comboBoxEx2.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                                groupID = int.Parse(infoTable["dwSetID"] as string);

                                break;
                            }
                        case "随机组":
                            {
                                groupType = GroupType.RandomGroup;
                                objectInfo = allGroupInfoList[comboBoxEx2.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                                groupID = int.Parse(infoTable["dwGroupID"] as string);

                                break;
                            }
                        case "重生组":
                            {
                                groupType = GroupType.ReviveGroup;
                                objectInfo = allGroupInfoList[comboBoxEx2.SelectedIndex];
                                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                                groupID = int.Parse(infoTable["dwGroupID"] as string);

                                break;
                            }
                    }
                }

                SetGroup(groupID, groupType);
                DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 创建仇恨组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            string groupName = textBoxX1.Text;

            if (groupName != "")
            {
                StringBuilder npcInfo = new StringBuilder();
                _AtlObjInfo objectInfo;
                Hashtable infoTable;

                for (int i = 0; i < objectInfoList.Count; i++)
                {
                    objectInfo = objectInfoList[i];
                    infoTable = Helper.GetInfoTable(objectInfo);
                    int nIndex = int.Parse(infoTable["nIndex"] as string);

                    npcInfo.Append(string.Format("{0},", nIndex.ToString()));
                }

                npcInfo.Remove(npcInfo.Length - 1, 1);

                baseDoc.DocLogical.DecoratedAddOneAIGroup(groupName, npcInfo.ToString());
                InitHatredGroup();
                FillHatredGroup();
                FillAllGroup();

                // 自动为当前选中的对象设置分组
                objectInfo = aiGroupInfoList[aiGroupInfoList.Count - 1];
                infoTable = Helper.GetInfoTable(objectInfo);
                int groupID = int.Parse(infoTable["dwSetID"] as string);
                SetGroup(groupID, GroupType.HatredGroup);

                MessageBox.Show("仇恨组创建成功！", "创建仇恨组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("请先输入分组名称！", "创建仇恨组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 清除仇恨组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {           
            if (MessageBox.Show("确定清除选中对象的仇恨组信息？", "清除仇恨组",
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                ClearGroupInfo("TeamID");
            }            
        }

        /// <summary>
        /// 创建随机组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX5_Click(object sender, EventArgs e)
        {
            string groupName = textBoxX2.Text;

            if (groupName != "")
            {
                List<string> npcInfoList = new List<string>();

                foreach (DataRow dataRow in BaseForm.npcTable.Rows)
                {
                    npcInfoList.Add(string.Format("{0} ({1})", dataRow["Name"].ToString(), dataRow["ID"].ToString()));
                }

                RandomGroupConfigForm rForm = new RandomGroupConfigForm(npcInfoList);

                if (rForm.ShowDialog() == DialogResult.OK)
                {
                    StringBuilder infoString = new StringBuilder();
                    
                    foreach (string s in rForm.NpcTemplateIDList)
                    {
                        infoString.Append(string.Format("{0},", s));
                    }

                    infoString.Remove(infoString.Length - 1, 1);

                    baseDoc.DocLogical.DecoratedNewOneRandomGroup(groupName, infoString.ToString());
                    InitRandomGroup();
                    FillRandomGroup();
                    FillAllGroup();

                    // 自动为当前选中的对象设置分组
                    _AtlObjInfo objectInfo = randomGroupInfoList[randomGroupInfoList.Count - 1];
                    Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                    int groupID = int.Parse(infoTable["dwGroupID"] as string);
                    SetGroup(groupID, GroupType.RandomGroup);

                    MessageBox.Show("随机组创建成功！", "创建随机组", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
            else
            {
                MessageBox.Show("请先输入分组名称！", "创建随机组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 清除随机组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX8_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定清除选中对象的随机组信息？", "清除随机组",
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                ClearGroupInfo("RandomID");
            }
        }

        /// <summary>
        /// 创建重生组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX9_Click(object sender, EventArgs e)
        {
            int minValue = integerInput1.Value;
            int maxValue = integerInput2.Value;
            string groupName = textBoxX3.Text;

            if(groupName != "")
            {
                int groupID = 0;

                switch(objectType)
                {
                    case "NPC":
                        {
                            baseDoc.DocLogical.DecoratedNewOneNpcReviveGroup(ref groupID, groupName, minValue, maxValue);

                            break;
                        }
                    case "Doodad":
                        {
                            baseDoc.DocLogical.DecoratedNewOneDoodadReviveGroup(ref groupID, groupName, minValue, maxValue);

                            break;
                        }
                    default:
                        {
                            MessageBox.Show("没有指定分组类型！", "创建重生组", MessageBoxButtons.OK, MessageBoxIcon.Information);
                            break;
                        }
                }

                InitReviveGroup();
                FillReviveGroup();
                FillAllGroup();

                // 自动为当前选中的对象设置分组
                _AtlObjInfo groupInfo = reviveGroupInfoList[reviveGroupInfoList.Count - 1];
                Hashtable groupInfoTable = Helper.GetInfoTable(groupInfo);
                int newGroupID = int.Parse(groupInfoTable["dwGroupID"] as string);
                SetGroup(newGroupID, GroupType.ReviveGroup);

                MessageBox.Show("重生组创建成功！", "创建重生组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("请先输入分组名称！", "创建重生组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 清除重生组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX10_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定清除选中对象的重生组信息？", "清除重生组",
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {                
                ClearGroupInfo("ReliveID");
            }            
        }

        /// <summary>
        /// 切换标签页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tabControl1_SelectedTabChanged(object sender, DevComponents.DotNetBar.TabStripTabChangedEventArgs e)
        {
            if (tabControl1.SelectedTab != null)
            {
                switch(tabControl1.SelectedTab.Text)
                {
                    case "仇恨组":
                        {
                            FillHatredGroup();
                            break;
                        }
                    case "随机组":
                        {
                            FillRandomGroup();
                            break;
                        }
                    case "重生组":
                        {
                            FillReviveGroup();
                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 管理分组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX11_Click(object sender, EventArgs e)
        {
            GroupManageForm gForm = new GroupManageForm();
            gForm.CurrentBaseDoc = baseDoc;
            gForm.Init();
            gForm.ShowDialog();
        }
	}
}