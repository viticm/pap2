using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.Data.SqlClient;

using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using DevComponents.DotNetBar.Controls;

using DataInteractor;
using DBManager;

namespace AIDiagramPlugin
{
    public partial class SingleActionForm : Office2007Form
    {
        private bool editSuccess = false; // 是否编辑成功
        private bool beginEdit = false; // 是否已经开始编辑

        private LabelX[] labelArray; 
        private ComboBoxEx[] comboBoxArray;
        private TextBox[] returnValueBoxArray;
        private CheckBoxX[] checkBoxArray;
        private AI_SingleAction singleAction;  // 当前动作

        private int graphElementID; // 当前编辑图元的ID
        private bool inActionGroup; // 是否在动作组中
        private int groupIndex; // 动作组编号
        private Hashtable actionTable = new Hashtable(); // 动作数据表

        private char[] splitLineArray = new char[] { '\n' };
        private char[] splitReturnValueArray = new char[] { ',' };                

        /// <summary>
        /// 构造函数
        /// </summary>
        public SingleActionForm()
        {
            InitializeComponent();            
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化控件           
            codeEditBox.ForWho = "GameLuaEditor";
            labelArray = new LabelX[] { labelX2, labelX3, labelX4, labelX5, labelX6 };
            comboBoxArray = new ComboBoxEx[] { comboBoxEx1, comboBoxEx2, comboBoxEx3, comboBoxEx4, comboBoxEx5 };
            checkBoxArray = new CheckBoxX[] { checkBoxX1, checkBoxX2, checkBoxX3, checkBoxX4, checkBoxX5 };
            returnValueBoxArray = new TextBox[] { returnValue1Box, returnValue2Box, returnValue3Box };   

            foreach (AI_Action action in ActionTable.ActionItems)
            {
                Node classNode = null;

                foreach (Node node in actionTree.Nodes)
                {
                    if (node.Text == action.ActionType)
                    {
                        classNode = node;
                        break;
                    }
                }

                if (classNode == null)
                {
                    classNode = new Node();
                    classNode.Text = action.ActionType;
                    actionTree.Nodes.Add(classNode);
                }

                Node newNode = new Node();
                newNode.Text = action.Name;
                newNode.Tag = action.DBID;
                classNode.Nodes.Add(newNode);

                AI_Action newAction = action.Clone() as AI_Action;
                actionTable[action.DBID] = newAction;
                actionComboBox.Items_Add(newAction);
            }

            // 展开所有的分类结点
            foreach (Node classNode in actionTree.Nodes)
            {
                classNode.Expand();
            }

            beginEdit = true;
        }

        /// <summary>
        /// 当前动作是否在动作组中
        /// </summary>
        public bool InActionGroup
        {
            set
            {
                inActionGroup = value;
            }
        }

        /// <summary>
        /// 动作组编号
        /// </summary>
        public int GroupIndex
        {
            set
            {
                groupIndex = value;
            }
        }

        /// <summary>
        /// 当前编辑图元的ID
        /// </summary>
        public int GraphElementID
        {
            set
            {
                graphElementID = value;
            }
        }

        /// <summary>
        /// 是否编辑成功
        /// </summary>
        public bool EditSuccess
        {
            get
            {
                return editSuccess;
            }
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        /// <param name="sigaction">AId动作</param>
        public void Reload(AI_SingleAction sigaction)
        {
            actionTree.SelectedNode = null;
            actionComboBox.SelectedItem = null;
            editSuccess = false;

            // 保存传进来的参数
            this.singleAction = sigaction;
            AI_Action action = sigaction.Action;                      

            // 本文件内的自定义脚本动作
            string actionNameID = graphElementID.ToString();

            if (inActionGroup)
            {
                actionNameID = string.Format("{0}_{1}", graphElementID.ToString(), groupIndex.ToString());
            }

            if (action.ActionID != null)
            {
                int actionID = int.Parse(action.ActionID);
                string[] returnValueArray;

                if (actionID > 2000) // 本绘图内自定义动作
                {
                    actionNameBox.Text = action.Name;
                    codeEditBox.Text = action.Info;

                    returnValueArray = action.rettype.Split(splitReturnValueArray, StringSplitOptions.RemoveEmptyEntries);

                    for (int i = 0; i < 3; i++)
                    {
                        if (i < returnValueArray.Length)
                        {
                            returnValueBoxArray[i].Text = returnValueArray[i];
                        }
                        else
                        {
                            returnValueBoxArray[i].Text = "";
                        }
                    }                        
                    
                    tabControl1.SelectedTabIndex = 1;
                }
                else // 系统库自定义动作
                {
                    tabControl1.SelectedTabIndex = 0;

                    // 选中当前动作
                    foreach (Node classNode in actionTree.Nodes)
                    {
                        if (classNode.Text == action.ActionType)
                        {
                            foreach (Node childNode in classNode.Nodes)
                            {
                                if (childNode.Tag.ToString() == action.DBID)
                                {
                                    actionTree.SelectedNode = childNode;
                                    break;
                                }
                            }

                            break;
                        }
                    }                  

                    // 自定义动作加上初始值
                    actionNameBox.Text = string.Format("自定义动作{0}", actionNameID);
                    returnValue1Box.Text = "成功";
                    returnValue2Box.Text = "";
                    returnValue3Box.Text = "";

                    StringBuilder code = new StringBuilder();
                    code.Append(string.Format("function CustomAction{0}(npc, actionData)\r\n", actionNameID));
                    code.Append("    -- 输入自定义动作实现脚本\r\n\r\n");
                    code.Append("    return 1\r\n");
                    code.Append("end");
                    codeEditBox.Text = code.ToString();
                }
            }
            else
            {
                tabControl1.SelectedTabIndex = 0;               

                // 自定义动作加上初始值
                actionNameBox.Text = string.Format("自定义动作{0}", actionNameID);
                returnValue1Box.Text = "成功";
                returnValue2Box.Text = "";
                returnValue3Box.Text = "";

                StringBuilder code = new StringBuilder();
                code.Append(string.Format("function CustomAction{0}(npc, actionData)\r\n", actionNameID));
                code.Append("    -- 输入自定义动作实现脚本\r\n\r\n");
                code.Append("    return 1\r\n");
                code.Append("end");
                codeEditBox.Text = code.ToString();
            }
        }

        /// <summary>
        /// 检查脚本是否有效
        /// </summary>
        /// <param name="content">脚本内容</param>
        /// <returns>是否有效</returns>
        private bool CheckScriptValid(string content)
        {
            bool valid = false;
            bool endSearch = false;
            bool findEnd = false;
            string[] lines = content.Split(splitLineArray, StringSplitOptions.RemoveEmptyEntries);

            for (int i = lines.Length - 1; i >= 0; i--)
            {
                string line = lines[i].TrimStart(new char[] { ' ', '\t' } ).TrimEnd(new char[] { '\r', ';', ' ', '\t' });

                switch (line)
                {
                    case "":
                        {
                            break;
                        }
                    case "end":
                        {
                            findEnd = true;
                            break;
                        }
                    case "return 1":
                        {
                            if (findEnd)
                            {
                                valid = true;
                            }
                            
                            break;
                        }
                    case "return 2":
                        {
                            if (findEnd)
                            {
                                valid = true;
                            }
                            
                            break;
                        }
                    case "return 3":
                        {
                            if (findEnd)
                            {
                                valid = true;
                            }
                            
                            break;
                        }
                    default:
                        {
                            endSearch = true;
                            break;
                        }
                }

                if (endSearch)
                {
                    break;
                }
            }

            return valid;
        }

        /// <summary>
        /// 检查参数有效性
        /// </summary>
        /// <param name="parameterType">参数类型</param>
        /// <param name="parameterValue">参数值</param>
        /// <param name="varType">是否是变脸类型</param>
        /// <returns>参数是否有效</returns>
        private bool CheckParameterValid(int parameterType, string parameterValue, bool varType)
        {
            bool valid = true;            

            switch (parameterType)
            {
                case -1: // 数字
                    {
                        if (string.IsNullOrEmpty(parameterValue))
                        {
                            valid = false;
                        }
                        else
                        {
                            if (!varType && !(parameterValue.StartsWith("<") && parameterValue.EndsWith(">")))
                            {
                                int testValue = 0;

                                if (!int.TryParse(parameterValue, out testValue))
                                {
                                    valid = false;
                                }
                            }
                        }

                        break;
                    }
                case -2: // 字符串
                    {
                        // 字符串允许空值
                        break;
                    }
                default: // 枚举值
                    {
                        if (string.IsNullOrEmpty(parameterValue))
                        {
                            valid = false;
                        }

                        break;
                    }
            }

            return valid;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            AI_Action newAction = null;

            if (tabControl1.SelectedTabIndex == 0) // 选择系统Action
            {
                Node currentNode = actionTree.SelectedNode;

                if (currentNode != null && currentNode.Level == 1)
                {
                    AI_Action selectedAction = actionTable[currentNode.Tag.ToString()] as AI_Action;                

                    if (selectedAction != null)
                    {                   
                        // 第一步，检查输入数据正确性
                        for (int i = 0; i < selectedAction.Args.Count; i++)
                        {
                            if (!CheckParameterValid(selectedAction.Args[i].m_type, comboBoxArray[i].Text, checkBoxArray[i].Checked))
                            {
                                MessageBox.Show(string.Format("{0}参数值无效！", labelArray[i].Text), "参数检查",
                                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                                comboBoxArray[i].Focus();

                                return;
                            }
                        }

                        // 第二步，把输入数据存到Action里
                        newAction = selectedAction.Clone() as AI_Action;

                        for (int i = 0; i < newAction.Args.Count; i++)
                        {
                            AI_Parm parm = newAction.Args[i];
                            object objSelect = comboBoxArray[i].SelectedItem;

                            if (comboBoxArray[i].Text.StartsWith("<") && comboBoxArray[i].Text.EndsWith(">")) // 模板参数
                            {
                                if (checkBoxArray[i].Checked)
                                {
                                    MessageBox.Show("模板参数不能为变量类型！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    comboBoxArray[i].Focus();

                                    return;
                                }

                                parm.m_IsTemplateParm = true;
                                AI_TemplateParmItem tpitem = objSelect as AI_TemplateParmItem;

                                if (objSelect == null)
                                {
                                    MessageBox.Show("参数错误", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    comboBoxArray[i].Focus();

                                    return;
                                }

                                int index = AI_TemplateParmTable.GetAIParamTemplateTable().GetAIParamIndex(tpitem.Name);
                                parm.SetTemplateValue(index.ToString());
                            }
                            else // 非模板参数
                            {
                                parm.m_IsTemplateParm = false;

                                if (objSelect == null) // 是填的，不是选的，那就是字符串或者数字了
                                {
                                    parm.m_Value = comboBoxArray[i].Text;
                                }
                                else if (objSelect is EnumItem) // 是枚举
                                {
                                    EnumItem ei = objSelect as EnumItem;
                                    parm.m_Value = ei.m_asParentsPos.ToString();
                                }
                            }

                            // 记录下是否是变量类型
                            parm.IsVar = checkBoxArray[i].Checked;
                        }                                                
                    }
                }
                else
                {
                    MessageBox.Show("请先选择动作！", "参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }                
            }
            else // 选择自定义Action
            {
                string functionName = actionNameBox.Text;
                string functionText = codeEditBox.Text;
                string returnValue = "";

                if (functionName == "")
                {
                    MessageBox.Show("动作名不能为空！", "参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                if (functionText == "")
                {
                    MessageBox.Show("自定义脚本不能为空！", "参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                for (int i = 0; i < 3; i++)
                {
                    if (returnValueBoxArray[i].Text.Trim() != "")
                    {
                        returnValue += string.Format("{0},", returnValueBoxArray[i].Text.Trim());
                    }
                    else
                    {
                        break;
                    }
                }
                returnValue = returnValue.TrimEnd(new char[] { ',' });

                if (returnValue == "")
                {
                    MessageBox.Show("至少需要有一个返回值！", "参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }               
                
                int actionID = graphElementID + 2000;

                if (inActionGroup) // 动作组内的自定义动作需要重新分配ID
                {
                    actionID = (graphElementID + 2000) * 10 + groupIndex;
                }

                string englishName = GetFunctionName(functionText);

                if (!CheckScriptValid(functionText)) // 检查脚本有效性
                {
                    MessageBox.Show("脚本必须在最后明确返回执行结果（如：return 1）！", "脚本检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                if (!AIDiagramHelper.CheckScript(codeEditBox, functionText)) // 没有通过脚本检查
                {
                    if (MessageBox.Show("没有通过脚本检查，是否重新编辑脚本？", "脚本检查", 
                                        MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        return;
                    }
                }

                newAction = new AI_Action();
                newAction.ActionID = actionID.ToString();
                newAction.Name = functionName;
                newAction.Info = functionText;
                newAction.Ename = englishName;
                newAction.Args = new List<AI_Parm>();
                newAction.rettype = returnValue;
            }

            singleAction.Action = newAction;
            editSuccess = true;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 获取方法名
        /// </summary>
        /// <param name="content">全部字符串</param>
        /// <returns>方法名</returns>
        private string GetFunctionName(string content)
        {
            string functionName = "";

            string[] lines = content.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i].Trim(new char[] { '\r', ' ' });
                if(line.StartsWith("function"))
                {
                    line = line.Replace("function", "");
                    int index = line.IndexOf("(");
                    if(index > 0)
                    {
                        functionName = line.Substring(0, index).Trim();
                    }

                    break;
                }
            }

            return functionName;
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 显示AI动作的数据
        /// </summary>
        /// <param name="action">AI动作</param>
        private void ShowActionData(AI_Action action)
        {            
            this.textBoxX1.Text = action.Info;

            for (int i = 0 ; i < 5; i++)
            {
                // 多余的参数，禁止掉
                if (i >= action.Args.Count)
                {
                    labelArray[i].Text = "-";
                    comboBoxArray[i].Text = "";
                    comboBoxArray[i].Enabled = false;
                    checkBoxArray[i].Checked = false;
                    checkBoxArray[i].Enabled = false;
                }
                else
                {
                    AI_Parm parm = action.Args[i];

                    // 步骤1：初始化枚举（常数）列表 如果是枚举，则添加枚举项进来。
                    comboBoxArray[i].Enabled = true;
                    comboBoxArray[i].Items.Clear();

                    if (parm.m_type > 0)
                    {
                        comboBoxArray[i].Items.AddRange(EnumTable.GetEnumTypeByID(parm.m_type.ToString()).Items.ToArray());
                    }

                    // 改标签
                    string strLab = null ;

                    switch (parm.m_type)
                    {
                        case -1:
                            {
                                strLab = "数字";

                                // 目前只支持数字的变量类型
                                checkBoxArray[i].Enabled = true;

                                break;
                            }
                        case -2:
                            {
                                strLab = "字符串";

                                // 暂时不支持字符串的变量类型
                                checkBoxArray[i].Enabled = false;

                                break;
                            }
                        default: 
                            {
                                strLab = "枚举";

                                // 暂时不支持枚举的变量类型
                                checkBoxArray[i].Enabled = false;

                                break;
                            }
                    }

                    if (parm.Name.Length > 10)
                    {
                        labelArray[i].Text = string.Format("{0}... ({1})", parm.Name.Substring(0, 8), strLab);
                    }
                    else
                    {
                        labelArray[i].Text = string.Format("{0} ({1})", parm.Name, strLab);
                    }

                    // 步骤2：初始化 模板参数，如果定义了这种类型的模板变量，就加进来。    
                    comboBoxArray[i].Items.AddRange(AI_TemplateParmTable.GetAIParamTemplateTable().AIParamItemList.ToArray());

                    // 步骤3：设值
                    if (parm.Value != null)
                    {
                        comboBoxArray[i].Text = parm.Value.ToString();
                    }
                    else
                    {
                        comboBoxArray[i].Text = "";
                    }

                    // 步骤4：读取参数类型
                    checkBoxArray[i].Checked = parm.IsVar;
                }                
            } 
        }

        /// <summary>
        /// 选中了某个Action
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void RefreshActionInfo(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = actionTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                AI_Action act = actionTable[currentNode.Tag.ToString()] as AI_Action;

                if (act != null)
                {
                    if (act.Equals(singleAction.Action))
                    {
                        ShowActionData(singleAction.Action);
                    }
                    else
                    {
                        ShowActionData(act);                        
                    }                
                }
            }
            else
            {
                for (int i = 0; i < 5; i++)
                {
                    labelArray[i].Text = "-";
                    comboBoxArray[i].Text = "";
                    comboBoxArray[i].Enabled = false;     
                }
            }                               
        }

        /// <summary>
        /// 查询对象
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void codeEditBox_queryObjects(object sender, luaEditor.QueryEventsArgs e)
        {
            AIDiagramHelper.QueryObjects(sender, e);            
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void SingleActionForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // e.Cancel = true;
            // this.Visible = false;
        }

        /// <summary>
        /// 选择选择框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void actionComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            AI_Action currentAction = actionComboBox.Items_SelectedItem as AI_Action;

            if (beginEdit && currentAction != null)
            {
                string actionID = currentAction.DBID;
                Node selectedNode = null;

                foreach (Node classNode in actionTree.Nodes)
                {
                    foreach (Node actionNode in classNode.Nodes)
                    {
                        if (actionNode.Tag.ToString() == actionID)
                        {
                            selectedNode = actionNode;
                            break;
                        }
                    }

                    if (selectedNode != null)
                    {
                        break;
                    }
                }

                actionTree.SelectedNode = selectedNode;
            }                       
        }
    }
}