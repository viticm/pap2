using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Data;
using System.Collections;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

using FlowChartEditor;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;
using AIDiagramPlugin;
using FlowChartDataLibrary;
using Compiler_Sword3_AI;

namespace AIChartLogicManager
{
    public class LogicManager:LogicBaseManager
    {
        private int maxID = -1; // 自动分配的的ID

        /// <summary>
        /// 构造函数
        /// </summary>
        public LogicManager()
        {
            
        }

        /// <summary>
        /// 初始化绘图参数
        /// </summary>
        protected override void InitGraphSetting()
        {
            GraphSetting graphSetting = GraphSetting.GetGraphSetting();
            graphSetting.GraphElementTextColor = Color.Yellow;
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        protected override void InitMenuStrip()
        {
            // 初始化菜单参数
            contextMenyBar.Items.Add(editMenu);
            contextMenyBar.Items.Add(canvasMenu);
            contextMenyBar.Style = eDotNetBarStyle.Office2003;

            // 初始化编辑菜单
            ButtonItem bSetInitialState = new ButtonItem("bSetInitialState", "设置为初态");   
            bSetInitialState.Click += new EventHandler(SetInitialState);
            editMenu.SubItems.Add(bSetInitialState);

            ButtonItem bEditRemark = new ButtonItem("bEditRemark", "编辑注释");
            bEditRemark.Click += new EventHandler(EditRemark);
            editMenu.SubItems.Add(bEditRemark);

            ButtonItem bCutGraphElement = new ButtonItem("bCutGraphElement", "剪切 (Ctrl + P)");
            bCutGraphElement.Click += new EventHandler(CutGraphElement);
            editMenu.SubItems.Add(bCutGraphElement);

            ButtonItem bCopyGraphElement = new ButtonItem("bCopyGraphElement", "复制 (Ctrl + C)");
            bCopyGraphElement.Click += new EventHandler(CopyGraphElement);
            editMenu.SubItems.Add(bCopyGraphElement);

            ButtonItem bCAndCStateNode = new ButtonItem("bCAndCStateNode", "创建并连接 状态结点 (1)");
            bCAndCStateNode.BeginGroup = true;
            bCAndCStateNode.Click += new EventHandler(CAndCStateNode);
            editMenu.SubItems.Add(bCAndCStateNode);

            ButtonItem bCAndCActionNode = new ButtonItem("bCAndCActionNode", "创建并连接 动作结点 (2)");
            bCAndCActionNode.Click += new EventHandler(CAndCActionNode);
            editMenu.SubItems.Add(bCAndCActionNode);

            ButtonItem bCAndCActionsNode = new ButtonItem("bCAndCActionsNode", "创建并连接 动作组结点 (3)");
            bCAndCActionsNode.Click += new EventHandler(CAndCActionsNode);
            editMenu.SubItems.Add(bCAndCActionsNode);

            ButtonItem bCAndCInnerChartNode = new ButtonItem("bCAndCInnerChartNode", "创建并连接 子绘图结点 (4)");
            bCAndCInnerChartNode.Click += new EventHandler(CAndCInnerChartNode);
            editMenu.SubItems.Add(bCAndCInnerChartNode);

            ButtonItem bCAndCInterfaceNode = new ButtonItem("bCAndCInterfaceNode", "创建并连接 接口结点 5)");
            bCAndCInterfaceNode.Click += new EventHandler(CAndCInterfaceNode);
            editMenu.SubItems.Add(bCAndCInterfaceNode);

            // 初始化绘图板菜单
            ButtonItem bRefreshCanvas = new ButtonItem("bRefreshCancas", "刷新(F5)");
            bRefreshCanvas.Click += new EventHandler(RefreshCanvas);
            canvasMenu.SubItems.Add(bRefreshCanvas);

            ButtonItem bResetGraphElementID = new ButtonItem("bResetGraphElementID", "重新分配图元ID");
            bResetGraphElementID.Click += new EventHandler(ResetGraphElementID);
            canvasMenu.SubItems.Add(bResetGraphElementID);

            ButtonItem bPasteGraphElement = new ButtonItem("bPasteGraphElement", "粘贴 (Ctrl + V)");
            bPasteGraphElement.Click += new EventHandler(PasteGraphElement);
            canvasMenu.SubItems.Add(bPasteGraphElement);

            ButtonItem bCreateStateNode = new ButtonItem("bCreateStateNode", "创建状态结点 (1)");
            bCreateStateNode.BeginGroup = true;
            bCreateStateNode.Click += new EventHandler(CreateStateNode);
            canvasMenu.SubItems.Add(bCreateStateNode);

            ButtonItem bCreateActionNode = new ButtonItem("bCreateActionNode", "创建动作结点 (2)");
            bCreateActionNode.Click += new EventHandler(CreateActionNode);
            canvasMenu.SubItems.Add(bCreateActionNode);

            ButtonItem bCreateActionsNode = new ButtonItem("bCreateActionsNode", "创建动作组结点 (3)");
            bCreateActionsNode.Click += new EventHandler(CreateActionsNode);
            canvasMenu.SubItems.Add(bCreateActionsNode);

            ButtonItem bCreateInnerChartNode = new ButtonItem("bCreateInnerChartNode", "创建子绘图结点 (4)");
            bCreateInnerChartNode.Click += new EventHandler(CreateInnerChartNode);
            canvasMenu.SubItems.Add(bCreateInnerChartNode);

            ButtonItem bCreateInterfaceNode = new ButtonItem("bCreateInterfaceNode", "创建接口结点 (5)");
            bCreateInterfaceNode.Click += new EventHandler(CreateInterfaceNode);
            canvasMenu.SubItems.Add(bCreateInterfaceNode);
        }
        
        /// <summary>
        /// 创建接口结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CreateInterfaceNode(object sender, EventArgs e)
        {            
            CreateNode(GraphType.InterfaceNode, Point.Empty, false);
        }

        /// <summary>
        /// 创建子绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CreateInnerChartNode(object sender, EventArgs e)
        {            
            CreateNode(GraphType.InnerChart, Point.Empty, false);
        }

        /// <summary>
        /// 创建动作组结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CreateActionsNode(object sender, EventArgs e)
        {            
            CreateNode(GraphType.AIActionsNode, Point.Empty, false);
        }

        /// <summary>
        /// 创建动作结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CreateActionNode(object sender, EventArgs e)
        {            
            CreateNode(GraphType.AIActionNode, Point.Empty, false);
        }

        /// <summary>
        /// 创建状态结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CreateStateNode(object sender, EventArgs e)
        {
            CreateNode(GraphType.AIStateNode, Point.Empty, false);
        }

        /// <summary>
        /// 创建并连接接口结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CAndCInterfaceNode(object sender, EventArgs e)
        {
            CreateNode(GraphType.InterfaceNode, Point.Empty, true);
        }

        /// <summary>
        ///  创建并连接子绘图结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CAndCInnerChartNode(object sender, EventArgs e)
        {
            CreateNode(GraphType.InnerChart, Point.Empty, true);
        }

        /// <summary>
        /// 创建并连接动作组结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CAndCActionsNode(object sender, EventArgs e)
        {            
            CreateNode(GraphType.AIActionsNode, Point.Empty, true);
        }

        /// <summary>
        /// 创建并连接动作结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CAndCActionNode(object sender, EventArgs e)
        {
            CreateNode(GraphType.AIActionNode, Point.Empty, true);
        }

        /// <summary>
        /// 创建并连接状态结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CAndCStateNode(object sender, EventArgs e)
        {
            CreateNode(GraphType.AIStateNode, Point.Empty, true);
        }

        /// <summary>
        /// 设置为初态
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void SetInitialState(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            AIStateGraphElement aiStateGraphElement = graphManager.SelectedGraphElement as AIStateGraphElement;

            foreach (SlotContainer slotContainer in graphManager.SlotContainerList)
            {
                if (slotContainer is AIStateGraphElement)
                {
                    AIStateGraphElement stateNode = slotContainer as AIStateGraphElement;
                    stateNode.IsInitialState = false;                    
                }
            }

            aiStateGraphElement.IsInitialState = true;
            graphManager.RefreshCanvas();
        }

        /// <summary>
        /// 重新分配图元的ID
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected override void ResetGraphElementID(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;

            base.ResetGraphElementID(sender, e);                        

            foreach (SlotContainer node in graphManager.SlotContainerList)
            {
                RenameGraphElement(dataManager, node);
            }            
        }

        /// <summary>
        /// 删除图元之后的操作
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicAfterDelete(FlowChartManager flowChartManager, object logicData)
        {
            DataManager dataManager = flowChartManager.CurrentDataManager;
            List<GraphElement> graphElementList = logicData as List<GraphElement>;

            foreach (GraphElement graphElement in graphElementList) // 清空连接线上的数据
            {
                if (graphElement is ConnectorContainer)
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;
                    DataElement dataElement = dataManager.GetDataElement(line);

                    if (dataElement != null)
                    {
                        dataElement.Data = null;
                    }

                    dataManager.SetData(line, null);

                    // 连接线恢复可见状态
                    line.Moving = false;
                    line.Visible = true;
                    graphElement.ShowText = false;
                }
            }

            return true;
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicPaste(FlowChartManager flowChartManager, object logicData)
        {
            DataManager dataManager = flowChartManager.CurrentDataManager;

            List<GraphElement> graphElementList = logicData as List<GraphElement>;
            dataManager.InitCompileTable();

            foreach (GraphElement graphElement in graphElementList)
            {
                RenameGraphElement(dataManager, graphElement);
            }

            return true;
        }

        /// <summary>
        /// 创建绘图
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCreateChart(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            TreeView tree = logicData as TreeView;

            TreeNode currentNode = tree.SelectedNode;
            string path = documentManager.GetNodePath(currentNode);

            CreateChartForm cForm = new CreateChartForm(path);

            if (cForm.ShowDialog() == DialogResult.OK)
            {
                string chartID = cForm.ChartID;
                string chartName = cForm.ChartName;

                TreeNode newNode = currentNode.Nodes.Add(chartName);
                newNode.Name = chartName;
                newNode.ImageIndex = 1;
                newNode.SelectedImageIndex = 1;
                newNode.Tag = chartID;
                tree.SelectedNode = newNode;
                documentManager.CurrentTreeMode = EditMode.Normal;
            }

            return true;
        }

        /// <summary>
        /// 绘图加载之后的操作
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicAfterLoad(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 弹出右键菜单
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicPopUpMenuStrip(FlowChartManager flowChartManager, object logicData)
        {
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            Point popUpLocation = (Point)logicData;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();

            if (graphManager.SelectedGraphElement is SlotContainer) // 有AI插槽容器被选中
            {
                if (graphManager.SelectedGraphElement is AIStateGraphElement)
                {
                    editMenu.SubItems["bCAndCStateNode"].Enabled = false;
                    editMenu.SubItems["bSetInitialState"].Enabled = true;
                }
                else
                {
                    editMenu.SubItems["bCAndCStateNode"].Enabled = true;
                    editMenu.SubItems["bSetInitialState"].Enabled = false;
                }

                if (flowChartManager.MapName == "子绘图")
                {
                    // 子绘图内不能再创建子绘图结点
                    editMenu.SubItems["bCAndCInnerChartNode"].Enabled = false;

                    // 只有子绘图能创建接口结点
                    editMenu.SubItems["bCAndCInterfaceNode"].Enabled = true;
                }
                else
                {                    
                    editMenu.SubItems["bCAndCInnerChartNode"].Enabled = true;                    
                    editMenu.SubItems["bCAndCInterfaceNode"].Enabled = false;
                }

                editMenu.Popup(popUpLocation);
            }
            else if (graphManager.SelectedGraphElement == null) // 没有选中任何图元
            {
                if (documentManager.CopyTable != null)
                {
                    canvasMenu.SubItems["bPasteGraphElement"].Enabled = true;
                }
                else
                {
                    canvasMenu.SubItems["bPasteGraphElement"].Enabled = false;
                }

                if (flowChartManager.MapName == "子绘图") 
                {
                    // 子绘图不允许重设图元ID
                    canvasMenu.SubItems["bResetGraphElementID"].Enabled = false;
                    
                    // 子绘图内不能再创建子绘图结点
                    canvasMenu.SubItems["bCreateInnerChartNode"].Enabled = false;

                    // 只有子绘图能创建接口结点
                    canvasMenu.SubItems["bCreateInterfaceNode"].Enabled = true;
                }
                else
                {
                    canvasMenu.SubItems["bResetGraphElementID"].Enabled = true;
                    canvasMenu.SubItems["bCreateInnerChartNode"].Enabled = true;
                    canvasMenu.SubItems["bCreateInterfaceNode"].Enabled = false;
                }

                canvasMenu.Popup(popUpLocation);
            }

            return true;
        }

        /// <summary>
        /// 在指定点创建结点
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCreateNodeAtPoint(FlowChartManager flowChartManager, object logicData)
        {
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            object[] args = logicData as object[];
            int graphType = (int)args[0];
            Point location = (Point)args[1];

            switch (graphType)
            {
                case 1: // 创建AI状态结点
                    {
                        if ((graphManager.SelectedGraphElement is AISlotContainer) &&
                            !(graphManager.SelectedGraphElement is AIStateGraphElement))
                        {
                            CreateNode(GraphType.AIStateNode, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.AIStateNode, location, false);
                        }

                        break;
                    }
                case 2: // 创建AI动作结点
                    {
                        if (graphManager.SelectedGraphElement is AISlotContainer)
                        {
                            CreateNode(GraphType.AIActionNode, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.AIActionNode, location, false);
                        }

                        break;
                    }
                case 3: // 创建AI动作组结点
                    {
                        if (graphManager.SelectedGraphElement is AISlotContainer)
                        {
                            CreateNode(GraphType.AIActionsNode, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.AIActionsNode, location, false);
                        }

                        break;
                    }
                case 4: // 创建子绘图结点
                    {
                        if (graphManager.SelectedGraphElement is AISlotContainer)
                        {
                            CreateNode(GraphType.InnerChart, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.InnerChart, location, false);
                        }

                        break;
                    }
                case 5:// 创建 接口结点
                    {
                        if (graphManager.SelectedGraphElement is AISlotContainer)
                        {
                            CreateNode(GraphType.InterfaceNode, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.InterfaceNode, location, false);
                        }

                        break;
                    }
            }

            return true;
        }

        /// <summary>
        /// 打开脚本
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicViewCode(FlowChartManager flowChartManager, object logicData)
        {
            Helper helper = Helper.GetHelper();
            string relevatePath = logicData as string;

            string[] data = relevatePath.Split(new char[] { '\\' });
            string shortPath = data[1];

            for (int i = 2; i < data.Length; i++)
            {
                shortPath += string.Format("\\{0}", data[i]);
            }

            string path = string.Format(@"{0}\scripts\Map\{1}\ai\{2}.lua", helper.OutputDir, data[0], shortPath);            

            if (File.Exists(path)) // 文件存在
            {
                try
                {
                    System.Diagnostics.Process.Start(path);
                }
                catch (IOException ex)
                {
                    MessageBox.Show("在查看脚本文件时产生IO异常：" + ex.Message, "打开脚本",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", path), "打开脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }  

            return true;
        }

        /// <summary>
        /// 打开文件夹
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicViewFolder(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            Helper helper = Helper.GetHelper();
            TreeNode currentNode = logicData as TreeNode;

            string currentPath = documentManager.GetNodePath(currentNode);
            string[] data = currentPath.Split(new char[] { '\\' });
            string path = "";
            char[] pathSymbleArray = new char[] { '\\' };

            if (data.Length > 1) // 查看场景根目录
            {
                string shortPath = data[1];

                for (int i = 2; i < data.Length; i++)
                {
                    shortPath += string.Format("\\{0}", data[i]);
                }

                path = string.Format(@"{0}\scripts\Map\{1}\ai\{2}", helper.OutputDir.TrimEnd(pathSymbleArray), data[0], shortPath);
            }
            else
            {
                path = string.Format(@"{0}\scripts\Map\{1}\ai", helper.OutputDir.TrimEnd(pathSymbleArray), data[0]);
            }
            
            if (Directory.Exists(path)) // 当前目录存在
            {
                System.Diagnostics.Process.Start("explorer.exe", path);
            }
            else // 当前目录不存在 
            {
                MessageBox.Show("该目录不存在！", "打开文件夹", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return true;
        }

        /// <summary>
        /// 保存绘图
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicSave(FlowChartManager flowChartManager, object logicData)
        {
            if (flowChartManager.CurrentPath.StartsWith("子绘图"))
            {
                string description = flowChartManager.CustomData["description"] as string;

                if (string.IsNullOrEmpty(description))
                {
                    description = flowChartManager.Name;
                }

                InputForm inputForm = new InputForm("请输入子绘图的描述", description);

                if (inputForm.ShowDialog() == DialogResult.OK)
                {
                    description = inputForm.InputText;
                    flowChartManager.CustomData["description"] = description;
                }
            }

            return true;
        }

        /// <summary>
        /// 生成脚本
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCompileCode(FlowChartManager flowChartManager, object logicData)
        {
            bool executeSuccess = GenerateCode();

            // 检查脚本内容
            string[] dataArray = flowChartManager.CurrentPath.Split(new char[] { '\\' });
            string shortPath = dataArray[1];

            for (int i = 2; i < dataArray.Length; i++)
            { 
                shortPath = shortPath + string.Format("\\{0}", dataArray[i]);
            }

            string fullPath = string.Format(@"{0}\scripts\Map\{1}\ai\{2}", Helper.GetHelper().OutputDir.TrimEnd(new char[] { '\\' }), dataArray[0], shortPath);
            string fileName = string.Format("{0}.lua", fullPath);
            string content = Helper.GetHelper().ReadFile(fileName);

            if (!AIDiagramHelper.CheckScript(null, content))
            {
                MessageBox.Show("没有通过脚本检查，请根据提示信息检查AI脚本是否正确！", "脚本检查",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return executeSuccess;
        }

        /// <summary>
        /// 检查逻辑
        /// </summary>
        /// <param name="flowChartManager">绘图对象</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicCheckLogic(FlowChartManager flowChartManager, object logicData)
        {
            bool executeResult = CheckAIChartAvail(flowChartManager);

            return executeResult;
        }

        /// <summary>
        /// 管理变量
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected override bool LogicManageArguments(FlowChartManager flowChartManager, object logicData)
        {
            bool executeResult = true;

            if (flowChartManager == null || flowChartManager.CurrentChartMode != displayName)
            {
                MessageBox.Show("当前没有AI图被激活！", "变量管理", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);

                executeResult = false;
            }
            else
            {
                TemplateParmForm tForm = new TemplateParmForm();
                DialogResult editResult = tForm.ShowDialog();

                if (editResult == DialogResult.OK)
                {
                    flowChartManager.ContentChanged = true;
                }
            }

            return executeResult;
        }

        /// <summary>
        /// 创建连接线
        /// </summary>
        /// <param name="headLocation">头结点位置</param>
        /// <param name="tailLocation">尾结点位置</param>
        /// <param name="id">连接线ID</param>
        /// <returns>连接线</returns>
        protected override ConnectorContainer CreateLine(Point headLocation, Point tailLocation, int id)
        {
            AILineGraphElement line = new AILineGraphElement(headLocation, tailLocation);
            line.ID = id;
            line.Name = "连接线";
            line.Init();

            return line;
        }

        /// <summary>
        /// 重命名图元
        /// </summary>
        /// <param name="graphElement">数据管理器</param>
        /// <param name="graphElement">图元</param>
        private void RenameGraphElement(DataManager dataManager, GraphElement graphElement)
        {
            object data = dataManager.GetCompileData(graphElement);

            if (graphElement is AIStateGraphElement) // 自动为状态结点重命名
            {
                AI_State aiState = data as AI_State;

                if (aiState != null)
                {
                    aiState.Ename = string.Format("State{0}", graphElement.ID.ToString());
                }
            }
            else if (graphElement is AIActionGraphElement) // 自动为自定义脚本的动作结点重命名
            {
                AI_SingleAction singleAction = data as AI_SingleAction;

                if (singleAction != null)
                {
                    string newName = string.Format("CustomAction{0}", graphElement.ID.ToString());
                    int actionID = 2000 + graphElement.ID;
                    RenameAIAction(singleAction.Action, newName, actionID);
                }
            }
            else if (graphElement is AIActionsGraphElement) // 自动为动作组中自定义脚本的动作结点重命名
            {                
                AI_MultiAction multiAction = data as AI_MultiAction;

                if (multiAction != null)
                {
                    int actionIndex = 0;
                    List<AI_SingleAction> aiActionList = multiAction.ActionList;

                    foreach (AI_SingleAction singleAction in aiActionList)
                    {
                        string newName = string.Format("CustomAction{0}_{1}", graphElement.ID.ToString(),
                                                        actionIndex.ToString());
                        int actionID = (2000 + graphElement.ID) * 10 + actionIndex;
                        RenameAIAction(singleAction.Action, newName, actionID);
                        actionIndex++;
                    }
                }
            } 
        }

        /// <summary>
        /// 重命名AI动作
        /// </summary>
        /// <param name="singleAction">AI动作</param>
        /// <param name="newName">新的动作名</param>
        /// <param name="actionID">新的动作ID</param>
        private void RenameAIAction(AI_Action aiAction, string newName, int actionID)
        {
            if (int.Parse(aiAction.ActionID) > 2000) // 脚本自定义动作，需要重命名
            {
                string script = aiAction.Info;
                string actionName = aiAction.Ename;
                aiAction.Info = script.Replace(aiAction.Ename, newName);
                aiAction.Ename = newName;
                aiAction.ActionID = actionID.ToString();
            }
        }

        /// <summary>
        /// 获取动作组的元数据链表
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="aiActionsGraphElement">动作组图元</param>
        /// <returns>动作组的元数据链表</returns>
        private List<FlowChartMetaData> GetActionsMetaDataList(DataManager dataManager, AIActionsGraphElement aiActionsGraphElement)
        {
            object data = dataManager.GetCompileData(aiActionsGraphElement);

            List<object> actions = CodeProviderClass.SplitMultiActionIntoSingle(data);
            List<FlowChartMetaData> actionsMetaDataList = new List<FlowChartMetaData>();

            // 把显示文本拆分
            List<string> displayTextList = new List<string>();
            string[] tempArray = aiActionsGraphElement.Text.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string s in tempArray)
            {
                displayTextList.Add(s.TrimEnd(new char[] { '\r' }).TrimStart(new char[] { '◆', ' ' }));
            }

            for (int i = 0; i < actions.Count; i++)
            {
                FlowChartMetaData actionMetaData = new FlowChartMetaData(maxID, actions[i], "AIActionNode");
                actionMetaData.DisplayText = displayTextList[i];
                actionsMetaDataList.Add(actionMetaData);
                maxID++;
            }

            for (int i = 0; i < actions.Count - 1; i++)
            {
                FlowChartMetaData actionMetaData = actionsMetaDataList[i];
                FlowChartMetaData nextMetaData = actionsMetaDataList[i + 1];

                FlowChartMetaData lineMetaData;
                int returnValueCount = CodeProviderClass.GetActionRetCount(actions[i]);

                for (int j = 1; j <= returnValueCount; j++)
                {
                    lineMetaData = new FlowChartMetaData(maxID, j.ToString(), "AILine");
                    lineMetaData.DisplayText = j.ToString();
                    actionMetaData.AddNextMetaData(lineMetaData);
                    lineMetaData.AddPreviousMetaData(actionMetaData);
                    lineMetaData.AddNextMetaData(nextMetaData);
                    nextMetaData.AddPreviousMetaData(lineMetaData);

                    maxID++;
                }
            }

            return actionsMetaDataList;
        }

        /// <summary>
        /// 获取流程图元数据
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="slotContainerList">流程图插槽容器的链表</param>
        /// <returns>流程图元数据的链表</returns>
        protected override List<FlowChartMetaData> GetMetaData(DataManager dataManager, List<SlotContainer> slotContainerList)
        {
            // 初始化ID
            maxID = dataManager.CurrentFlowChartManager.CurrentGraphManager.IDIndex + 1;

            List<FlowChartMetaData> metaDataList = new List<FlowChartMetaData>();
            List<GraphElement> graphElementList = new List<GraphElement>();
            Hashtable graphElementTable = new Hashtable();
            dataManager.InitCompileTable();

            Hashtable actionsTable = new Hashtable(); // 动作组数据哈希表

            // 预处理动作组结点
            foreach (SlotContainer slotContainer in slotContainerList)
            {
                if (slotContainer is AIActionsGraphElement)
                {
                    AIActionsGraphElement actionsGraphElement = slotContainer as AIActionsGraphElement;
                    List<FlowChartMetaData> actionsMetaDataList = GetActionsMetaDataList(dataManager, actionsGraphElement);
                    actionsTable[actionsGraphElement] = actionsMetaDataList;
                }
            }

            // 预处理子绘图结点
            AIStateGraphElement stateNode = null;            

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                // 所有子绘图的图元都重新分配ID
                if (slotContainer is InnerChart)
                {
                    InnerChart innerChart = slotContainer as InnerChart;
                    InnerChartEditor innerChartEditor = dataManager.GetDataElement(innerChart) as InnerChartEditor;
                    List<GraphElement> list = innerChartEditor.GraphElementList;

                    foreach (GraphElement graphElement in list)
                    {
                        graphElement.ID = maxID;
                        graphElement.Tag = innerChart.ID;
                        maxID++;

                        dataManager.AddCompileData(graphElement, innerChartEditor.GetDataElement(graphElement));

                        // 重命名名称                        
                        RenameGraphElement(dataManager, graphElement);

                        if (graphElement is AIStateGraphElement)
                        {
                            AIStateGraphElement aiStateGraphElement = graphElement as AIStateGraphElement;

                            if (aiStateGraphElement.IsInitialState)
                            {
                                stateNode = aiStateGraphElement;
                            }
                        }
                        else if (graphElement is AIActionsGraphElement)
                        {
                            AIActionsGraphElement actionsGraphElement = graphElement as AIActionsGraphElement;
                            List<FlowChartMetaData> actionsMetaDataList = GetActionsMetaDataList(dataManager, actionsGraphElement);
                            actionsTable[actionsGraphElement] = actionsMetaDataList;
                        }
                    }
                }
            }

            // 处理绘图结点
            foreach (SlotContainer slotContainer in slotContainerList)
            {
                if (slotContainer is AIStateGraphElement)
                {
                    AIStateGraphElement stateGraphElement = slotContainer as AIStateGraphElement;

                    if (stateGraphElement.IsInitialState)
                    {
                        stateNode = stateGraphElement;
                        break;
                    }                    
                }
            }

            if (stateNode != null)
            {
                object data = dataManager.GetCompileData(stateNode);
                DataElement dataElement = dataManager.GetCompileDataElement(stateNode);
                FlowChartMetaData metaData = new FlowChartMetaData(stateNode.ID, data, "AIStateNode");
                string stateNodeName = CodeProviderClass.GetStateString(data);

                metaData.Code = stateNodeName;
                metaData.DisplayText = stateNode.Text;
                FindNextMetaData(dataManager, stateNode, metaData, graphElementList, graphElementTable, actionsTable);
                metaDataList.Add(metaData);
            }

            return metaDataList;
        }

        /// <summary>
        /// 获取设置状态的绘图元数据
        /// </summary>
        /// <param name="previousMetaData">前一个元数据</param>
        /// <param name="stateMetaData">状态元数据</param>
        private void InitSetStateMetaData(FlowChartMetaData previousMetaData, FlowChartMetaData stateMetaData)
        {
            FlowChartMetaData setStateMetaData = new FlowChartMetaData(maxID, null, "AISetStateNode");
            setStateMetaData.DisplayText = "设置状态";
            setStateMetaData.AddPreviousMetaData(previousMetaData);
            previousMetaData.AddNextMetaData(setStateMetaData);
            maxID++;

            FlowChartMetaData lineMetaData = new FlowChartMetaData(maxID, "1", "AILine");
            lineMetaData.DisplayText = "1";
            setStateMetaData.AddNextMetaData(lineMetaData);
            lineMetaData.AddPreviousMetaData(setStateMetaData);
            maxID++;

            lineMetaData.AddNextMetaData(stateMetaData);
            stateMetaData.AddPreviousMetaData(lineMetaData);            
        }

        /// <summary>
        /// 获取下一个绘图元数据
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="metaData">绘图元数据</param>
        /// <param name="graphElementList">遍历过的图元链表</param>
        /// <param name="graphElementTable">流程图索引哈希表</param>
        /// <param name="actionsTable">动作组数据哈希表</param>
        private void FindNextMetaData(DataManager dataManager, GraphElement graphElement, FlowChartMetaData metaData, List<GraphElement> graphElementList, Hashtable graphElementTable, Hashtable actionsTable)
        {
            graphElementList.Add(graphElement);
            graphElementTable[graphElement] = metaData;
            List<GraphElement> list = dataManager.GetNextGraphElements(graphElement, true);

            foreach (GraphElement g in list)
            {
                object data = dataManager.GetCompileData(g);
                DataElement dataElement =  dataManager.GetCompileDataElement(g);

                if (!graphElementList.Contains(g))
                {                    
                    FlowChartMetaData newMetaData = new FlowChartMetaData(g.ID, data, dataElement.DataType);
                    newMetaData.DisplayText = g.Text;                    
                    
                    if (g is AIStateGraphElement) // 状态
                    {
                        string stateNodeName = CodeProviderClass.GetStateString(data);
                        newMetaData.Code = stateNodeName;

                        InitSetStateMetaData(metaData, newMetaData);
                        FindNextMetaData(dataManager, g, newMetaData, graphElementList, graphElementTable, actionsTable);
                    }
                    else if (g is AILineGraphElement) // 连接线
                    {
                        newMetaData.Code = CodeProviderClass.GetEventString(data);

                        newMetaData.AddPreviousMetaData(metaData);
                        metaData.AddNextMetaData(newMetaData);
                        FindNextMetaData(dataManager, g, newMetaData, graphElementList, graphElementTable, actionsTable);                        
                    }
                    else if (g is AIActionGraphElement) // 动作
                    {
                        newMetaData.AddPreviousMetaData(metaData);
                        metaData.AddNextMetaData(newMetaData);
                        FindNextMetaData(dataManager, g, newMetaData, graphElementList, graphElementTable, actionsTable);
                    }
                    else if (g is AIActionsGraphElement) // 动作组
                    {                    
                        List<FlowChartMetaData> metaDataList = actionsTable[g] as List<FlowChartMetaData>;

                        metaDataList[0].AddPreviousMetaData(metaData);
                        metaData.AddNextMetaData(metaDataList[0]);

                        FindNextMetaData(dataManager, g, metaDataList[metaDataList.Count - 1], graphElementList, graphElementTable, actionsTable);
                    }
                }
                else
                {
                    if (g is AIActionsGraphElement) // 动作组要特殊处理
                    {
                        List<FlowChartMetaData> metaDataList = actionsTable[g] as List<FlowChartMetaData>;
                        metaDataList[0].AddPreviousMetaData(metaData);
                        metaData.AddNextMetaData(metaDataList[0]);
                    }
                    else if (g is AIStateGraphElement) // 状态结点要特殊处理
                    {
                        FlowChartMetaData newMetaData = graphElementTable[g] as FlowChartMetaData;

                        InitSetStateMetaData(metaData, newMetaData);
                    }
                    else
                    {
                        FlowChartMetaData newMetaData = graphElementTable[g] as FlowChartMetaData;
                        newMetaData.AddPreviousMetaData(metaData);
                        metaData.AddNextMetaData(newMetaData);
                    }                    
                }
            }                      
        }

        /// <summary>
        /// 检查AI图的有效性
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <returns>AI图是否有效</returns>
        private bool CheckAIChartAvail(FlowChartManager flowChartManager)
        {
            bool avail = true;

            // 检查连接线的有效性
            avail = CheckLineAvail(flowChartManager);

            // 检查图元的有效性
            if (avail)
            {                
                avail = CheckAIGraphElementAvail(flowChartManager);
            }

            return avail;
        }

        /// <summary>
        /// 检查图元是否有效
        /// </summary>
        /// <param name="dataManager">绘图管理器</param>
        /// <returns>图元是否有效</returns>
        private bool CheckAIGraphElementAvail(FlowChartManager flowChartManager)
        {
            GraphElement initialStateNode = null;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            DataManager dataManager = flowChartManager.CurrentDataManager;

            foreach (GraphElement graphElement in flowChartManager.CurrentGraphManager.CurrentCanvas.GraphElementList)
            {
                if (dataManager.GetData(graphElement) == null)
                {
                    documentManager.PrintText(string.Format("ID为{0}的图元没有完成编辑！", graphElement.ID));

                    MessageBox.Show("绘图中有没有完成编辑的图元，请确认所有的图元都已正确编辑过！", "AI图有效性检查", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                    // 显示细节
                    List<GraphElement> list = new List<GraphElement>();
                    list.Add(graphElement);
                    dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);

                    return false;
                }

                if (graphElement is AIStateGraphElement) // AI状态结点
                {
                    AIStateGraphElement stateNode = graphElement as AIStateGraphElement;

                    if(stateNode.IsInitialState)
                    {
                        initialStateNode = graphElement;
                    }

                    int stateEventCount = CodeProviderClass.GetStateEvtCount(dataManager.GetData(stateNode));

                    if(stateEventCount > dataManager.GetNextGraphElements(stateNode, false).Count)
                    {
                        documentManager.PrintText(string.Format("ID为{0}的AI状态结点没有处理所有的事件！", graphElement.ID));
                    }

                    List<SlotContainer> list = dataManager.GetNextSlotContainers(stateNode);

                    foreach (SlotContainer slotContainer in list)
                    {
                        if (slotContainer is AIStateGraphElement)
                        {
                            documentManager.PrintText(string.Format("ID为{0}的AI状态结点直接连接了状态结点！", graphElement.ID));                            

                            MessageBox.Show("绘图中有状态结点直接连接了状态结点，状态结点只能连接动作结点！", "AI图有效性检查", 
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);

                            // 显示细节
                            List<GraphElement> detailList = new List<GraphElement>();
                            detailList.Add(graphElement);
                            dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(detailList);

                            return false;
                        }
                    }
                }
                else if (graphElement is AIActionGraphElement) // AI动作结点
                {
                    int actionBranchCount = CodeProviderClass.GetActionRetCount(dataManager.GetData(graphElement));

                    if (actionBranchCount > dataManager.GetNextGraphElements(graphElement, false).Count)
                    {
                        documentManager.PrintText(string.Format("ID为{0}的AI动作结点没有处理所有的返回值！", graphElement.ID));                        
                        
                        MessageBox.Show("绘图中有动作结点没有处理所有的返回值，请确认已连接实现所有的分支！", "AI图有效性检查", 
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);

                        // 显示细节
                        List<GraphElement> list = new List<GraphElement>();
                        list.Add(graphElement);
                        dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);

                        return false;
                    }                    
                }
                else if (graphElement is InnerChart) // 子绘图结点
                {
                    if (initialStateNode == null)
                    {
                        InnerChart innerChart = graphElement as InnerChart;
                        InnerChartEditor innerChartEditor = dataManager.GetDataElement(innerChart) as InnerChartEditor;

                        List<GraphElement> graphElementList = innerChartEditor.GraphElementList;

                        foreach (GraphElement g in graphElementList)
                        {
                            if (g is AIStateGraphElement)
                            {
                                AIStateGraphElement stateNode = g as AIStateGraphElement;

                                if (stateNode.IsInitialState)
                                {
                                    initialStateNode = innerChart;
                                    break;
                                }
                            }
                        }
                    }                    
                }
            }

            if (flowChartManager.CurrentPath.StartsWith("子绘图")) // 子绘图不需要和初态的检查
            {                
                bool hasInterfaceGraphElement = false; // 检查是否有接口结点
                bool interfaceNameConflict = false; // 接口结点名称是否冲突
                List<string> interfaceNameList = new List<string>();

                foreach (SlotContainer slotContainer in flowChartManager.CurrentGraphManager.SlotContainerList)
                {
                    if (slotContainer is InterfaceGraphElement)
                    {
                        hasInterfaceGraphElement = true;

                        if (!interfaceNameList.Contains(slotContainer.Text))
                        {
                            interfaceNameList.Add(slotContainer.Text);
                        }
                        else
                        {
                            interfaceNameConflict = true;
                        }

                        break;
                    }
                }

                if (!hasInterfaceGraphElement)
                {
                    documentManager.PrintText("没有接口结点！");

                    MessageBox.Show("子绘图需要至少有一个接口结点！", "AI图有效性检查",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                    return false;
                }

                if (interfaceNameConflict)
                {
                    documentManager.PrintText("接口结点名称冲突！");

                    MessageBox.Show("接口结点不能有相同的名称！", "AI图有效性检查",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                    return false;
                }
            }
            else
            {
                if (initialStateNode == null) // 没有找到初态结点
                {
                    documentManager.PrintText("没有设置初态！");

                    MessageBox.Show("绘图中没有设置初态，请确保AI图有初态作为流程入口！", "AI图有效性检查", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                    return false;
                }

                // 检查是否所有的图元都与初态结点正确连接            
                foreach (GraphElement graphElement in flowChartManager.CurrentGraphManager.SlotContainerList)
                {
                    if (!dataManager.IsConnect(initialStateNode, graphElement))
                    {
                        MessageBox.Show("绘图中有初态无法到达的图元，请确认所有的图元都能由初态正确连接上！", "AI图有效性检查", 
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);

                        // 显示细节
                        List<GraphElement> list = new List<GraphElement>();
                        list.Add(graphElement);
                        dataManager.CurrentFlowChartManager.CurrentGraphManager.SelectGraphElements(list);

                        return false;
                    }
                }  
            }                      

            return true;
        }

        /// <summary>
        /// 检查连接线是否有效
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="showDetail">是否显示细节</param>
        /// <returns>连接线是否有效</returns>
        private bool CheckLineAvail(FlowChartManager flowChartManager)
        {
            bool avail = true;
            ConnectorContainer invalidLine = null;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();

            foreach (ConnectorContainer line in flowChartManager.CurrentGraphManager.ConnectorContainerList)
            {
                if (line.InSlotContainer == null || line.OutSlotContainer == null) // 连接线两端必须连接上结点
                {
                    avail = false;
                    invalidLine = line;
                    documentManager.PrintText(string.Format("ID为{0}的连接线没有正确连接！", line.ID));

                    break;
                }
            }

            if (!avail) // 绘图无效 
            {
                MessageBox.Show("绘图中有不完整的连接，请确认所有的连接是否正确！", "绘图有效性检查", 
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 显示细节
                List<GraphElement> list = new List<GraphElement>();
                list.Add(invalidLine);
                flowChartManager.CurrentGraphManager.SelectGraphElements(list);                
            }

            return avail;
        }
    }
}
