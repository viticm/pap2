using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.Collections;
using System.Drawing;

using DevComponents.DotNetBar;

using CodeCompiler;
using DBManager;
using FlowChartDataLibrary;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor
{
    public class LogicBaseManager
    {
        protected const string pluginPath = @"Plugins\FlowChart"; // 插件路径
        protected Compiler compiler;
        protected Dictionary<string, DataElementInfo> dataElementInfoDirectory = new Dictionary<string, DataElementInfo>();
        protected string displayName = "绘图"; // 显示名称

        protected ContextMenuBar contextMenyBar = new ContextMenuBar(); // 右键菜单
        protected ButtonItem editMenu = new ButtonItem("editMenu", "editMenu"); // 编辑菜单
        protected ButtonItem canvasMenu = new ButtonItem("canvasMenu", "canvasMenu"); // 绘图板菜单

        /// <summary>
        /// 构造函数
        /// </summary>
        public LogicBaseManager()
        {
            // 初始化绘图参数
            InitGraphSetting();

            // 初始化右键菜单
            InitMenuStrip();
        }

        /// <summary>
        /// 显示名称
        /// </summary>
        public string DisplayName
        {
            set
            {
                displayName = value;
            }

            get
            {
                return displayName;
            }
        }

        /// <summary>
        /// 初始化绘图参数
        /// </summary>
        protected virtual void InitGraphSetting()
        {
            GraphSetting graphSetting = GraphSetting.GetGraphSetting();
            graphSetting.GraphElementTextColor = Color.Black;
        }

        /// <summary>
        /// 初始化右键菜单
        /// </summary>
        protected virtual void InitMenuStrip()
        {
            // 初始化菜单参数                        
            contextMenyBar.Items.Add(editMenu);
            contextMenyBar.Items.Add(canvasMenu);
            contextMenyBar.Style = eDotNetBarStyle.Office2003;

            // 初始化编辑菜单
            ButtonItem bEditRemark = new ButtonItem("bEditRemark", "编辑注释");
            bEditRemark.Click += new EventHandler(EditRemark);
            editMenu.SubItems.Add(bEditRemark);

            ButtonItem bCutGraphElement = new ButtonItem("bCutGraphElement", "剪切 (Ctrl + P)");
            bCutGraphElement.Click += new EventHandler(CutGraphElement);
            editMenu.SubItems.Add(bCutGraphElement);

            ButtonItem bCopyGraphElement = new ButtonItem("bCopyGraphElement", "复制 (Ctrl + C)");
            bCopyGraphElement.Click += new EventHandler(CopyGraphElement);
            editMenu.SubItems.Add(bCopyGraphElement);

            ButtonItem bCAndCConditionNode = new ButtonItem("bCAndCConditionNode", "创建并连接 条件结点 (2)");
            bCAndCConditionNode.Click += new EventHandler(CAndCConditionNode);
            editMenu.SubItems.Add(bCAndCConditionNode);

            ButtonItem bCAndCActionNode = new ButtonItem("bCAndCActionNode", "创建并连接 动作结点 (3)");
            bCAndCActionNode.Click += new EventHandler(CAndCActionNode);
            editMenu.SubItems.Add(bCAndCActionNode);

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

            ButtonItem bCreateEventNode = new ButtonItem("bCreateEventNode", "创建事件结点 (1)");
            bCreateEventNode.BeginGroup = true;
            bCreateEventNode.Click += new EventHandler(CreateEventNode);
            canvasMenu.SubItems.Add(bCreateEventNode);
        }

        /// <summary>
        /// 创建事件结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void CreateEventNode(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.EventNode, Point.Empty, false);
        }

        /// <summary>
        /// 刷新绘图板
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void RefreshCanvas(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            // 刷新缩略图元
            graphManager.CurrentCanvas.AbbreviatGraphElement = null;
            graphManager.CurrentCanvas.AbbreviateLine = null;
            
            // 恢复默认按键状态
            graphManager.ControlMode = false;
            graphManager.ShiftMode = false;
            graphManager.AltMode = false;

            // 刷新绘图区域
            graphManager.RefreshRegion();
            graphManager.RefreshCanvas();
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void PasteGraphElement(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            documentManager.PasteGraphElement();
        }

        /// <summary>
        /// 创建并连接条件结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void CAndCConditionNode(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ConditionNode, Point.Empty, true);
        }

        /// <summary>
        /// 创建并连接动作结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void CAndCActionNode(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(GraphType.ActionNode, Point.Empty, true);
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void CopyGraphElement(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            documentManager.CopyGraphElement();
        }

        /// <summary>
        /// 剪切图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void CutGraphElement(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            documentManager.CutGraphElement();
        }

        /// <summary>
        /// 编辑注释
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void EditRemark(object sender, EventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            documentManager.EditRemark();
        }

        /// <summary>
        /// 重新分配图元的ID
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        protected virtual void ResetGraphElementID(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定重新分配图元ID吗？", "重新分配图元ID", 
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                DocumentManager documentManager = DocumentManager.GetDocumentManager();
                FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
                GraphManager graphManager = flowChartManager.CurrentGraphManager;
                DataManager dataManager = flowChartManager.CurrentDataManager;

                graphManager.ResetGraphElementID();
                dataManager.ResetDataElementID();
                graphManager.RefreshCanvas();
            }            
        }

        /// <summary>
        /// 刷新子绘图连接线的数据
        /// </summary>
        /// <param name="innerChart">子绘图</param>
        protected virtual void RefreshInnerChartLine(InnerChart innerChart)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;
        }
        
        /// <summary>
        /// 创建图元结点
        /// </summary>
        /// <param name="graphType">图元类型</param>
        /// <param name="location">图元位置</param>
        /// <param name="autoConnect">是否自动连接</param>
        protected void CreateNode(GraphType graphType, Point location, bool autoConnect)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            flowChartManager.CurrentGraphManager.CreateAbbreviateGraphElement(graphType, location, autoConnect);
        }

        /// <summary>
        /// 初始化脚本生成插件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="className">类名</param>
        public void InitCompiler(string filePath, string className)
        {
            try
            {
                string fullPath = Path.Combine(Application.StartupPath, pluginPath);
                fullPath = Path.Combine(fullPath, filePath);
                Assembly assembly = Assembly.LoadFrom(fullPath);
                compiler = assembly.CreateInstance(className) as Compiler;
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("加载脚本生成文件 {0} 时产生异常：\r\n{1}", filePath, ex.Message), "初始化脚本生成文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// 注册数据元信息
        /// </summary>
        public void RegistDataElementInfo(string graphType, string filePath, string className)
        {
            string fullPath = Path.Combine(Application.StartupPath, pluginPath);
            fullPath = Path.Combine(fullPath, filePath);
            DataElementInfo dataElementInfo = new DataElementInfo(graphType, fullPath, className);
            dataElementInfoDirectory[graphType] = dataElementInfo;
        }

        /// <summary>
        /// 连接图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicConnect(FlowChartManager flowChartManager, object logicData)
        {
            object[] data = logicData as object[];
            SlotGraphElement slot = data[0] as SlotGraphElement;
            ConnectorGraphElement connector = data[1] as ConnectorGraphElement;
            ConnectorContainer line = connector.Line;

            if ((line.InSlotContainer != null && line.OutSlotContainer != null) || connector.Selected) // 连接线控制点处于被选中状态
            {
                flowChartManager.CurrentGraphManager.EditDataElement(line);
            }

            return true;
        }

        /// <summary>
        /// 解除连接图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicDisconnect(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 删除图元之前的处理
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicBeforeDelete(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 删除图元之后的处理
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicAfterDelete(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 编辑图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicEdit(FlowChartManager flowChartManager, object logicData)
        {
            bool executeResult = true;

            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            GraphElement graphElement = logicData as GraphElement;
            DataElement dataElement = dataManager.GetDataElement(graphElement);

            object data = dataManager.GetData(graphElement);
            Hashtable information = new Hashtable();

            information["data"] = data;
            information["prev_data"] = dataManager.GetPreviousData(graphElement);
            information["next_data"] = dataManager.GetNextData(graphElement);
            information["neighbor_data"] = dataManager.GetNeighborData(graphElement);
            information["globe_args"] = dataManager.GlobeArgs;
            information["flowchart_name"] = flowChartManager.Name;
            information["map_name"] = flowChartManager.MapName;
            information["client_dir"] = Helper.GetHelper().OutputDir;

            dataElement.PrintInformation = new DataElement.PrintInfo(documentManager.PrintText);

            try
            {
                executeResult = dataElement.EditData(information);
            }
            catch (Exception ex)
            {
                executeResult = false;

                MessageBox.Show("当前图元由于以下原因无法编辑：\n\n" + ex.Message, "图元编辑",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            if (executeResult) // 保存图元数据
            {
                Hashtable previousDataTable = information["prev_data"] as Hashtable;
                Hashtable nextDataTable = information["next_data"] as Hashtable;
                GraphElement currentGraphElement;
                DataElement editDataElement;

                // 检查是否需要更新图元和数据元的数据
                foreach (string id in previousDataTable.Keys)
                {
                    editDataElement = previousDataTable[id] as DataElement;

                    if (editDataElement != null && editDataElement.Data == null)
                    {
                        currentGraphElement = dataManager.FindGraphElementByID(int.Parse(id));
                        currentGraphElement.Text = editDataElement.Text;
                        currentGraphElement.TooltipText = editDataElement.TooltipText;
                        currentGraphElement.ShowText = false;
                    }
                }

                foreach (string id in nextDataTable.Keys)
                {
                    editDataElement = nextDataTable[id] as DataElement;

                    if (editDataElement != null && editDataElement.Data == null)
                    {
                        currentGraphElement = dataManager.FindGraphElementByID(int.Parse(id));
                        currentGraphElement.Text = editDataElement.Text;
                        currentGraphElement.TooltipText = editDataElement.TooltipText;
                        currentGraphElement.ShowText = false;
                    }
                }

                graphElement.TooltipText = dataElement.TooltipText;
                graphElement.Text = dataElement.Text;

                if (string.IsNullOrEmpty(dataElement.Text))
                {
                    graphElement.ShowText = false;
                }
                else
                {
                    graphElement.ShowText = true;
                }

                // 调整文本
                if (graphElement is SlotContainer)
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;
                    slotContainer.AdjustText();

                    // 根据文本内容调整插槽容器的大小
                    slotContainer.AdjustElementSize();
                }
                else if (graphElement is ConnectorContainer)
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;
                    line.AdjustText();
                }
            }

            return executeResult;
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicPaste(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 创建连接线
        /// </summary>
        /// <param name="headLocation">头结点位置</param>
        /// <param name="tailLocation">尾结点位置</param>
        /// <param name="id">连接线ID</param>
        /// <returns>连接线</returns>
        protected virtual ConnectorContainer CreateLine(Point headLocation, Point tailLocation, int id)
        {            
            LineGraphElement line = new LineGraphElement(headLocation, tailLocation);
            line.ID = id;
            line.Name = "连接线";
            line.Init();            

            return line;
        }

        /// <summary>
        /// 创建连接线
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicCreateLine(FlowChartManager flowChartManager, object logicData)
        {            
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            object[] args = logicData as object[];
            int id = graphManager.AllocateGraphElementID();
            SlotGraphElement slot = args[0] as SlotGraphElement;
            Point p = (Point)args[1];            

            ConnectorContainer line = CreateLine(slot.Location, p, id);
            slot.Bind(line.TailConnector);            
            flowChartManager.CurrentGraphManager.SelectGraphElement(line, false);

            return true;
        }

        /// <summary>
        /// 创建并连接连接线
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicCreateAndConnectLine(FlowChartManager flowChartManager, object logicData)
        {
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            object[] args = logicData as object[];
            int id = graphManager.AllocateGraphElementID();
            SlotGraphElement outSlot = args[0] as SlotGraphElement;
            SlotGraphElement inSlot = args[1] as SlotGraphElement;

            ConnectorContainer line = CreateLine(outSlot.Location, inSlot.Location, id);                        
            flowChartManager.CurrentGraphManager.SelectGraphElement(line, false);

            return true;
        }

        /// <summary>
        /// 创建绘图
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicCreateChart(FlowChartManager flowChartManager, object logicData)
        {
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            TreeView tree = logicData as TreeView;
            TreeNode currentNode = tree.SelectedNode;

            InputForm iForm = new InputForm("请输入新建绘图的名称", "新建绘图");

            if (iForm.ShowDialog() == DialogResult.OK)
            {
                TreeNode newNode = currentNode.Nodes.Add(iForm.InputText);
                newNode.ImageIndex = 1;
                newNode.SelectedImageIndex = 1;
                tree.SelectedNode = newNode;
                documentManager.CurrentTreeMode = EditMode.Normal;

                // 对数据库进行操作
                string path = documentManager.GetNodePath(newNode);
                string id = dataBaseManager.CreateNewDiagram(path, false, documentManager.CurrentChartMode);
                newNode.Tag = id;
            }

            return true;
        }

        /// <summary>
        /// 绘图加载之后的处理
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicAfterLoad(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 初始化数据元
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicInitializeDataElement(FlowChartManager flowChartManager, object logicData)
        {
            bool executeSuccess = true;
            GraphElement graphElement = logicData as GraphElement;
            DataElement dataElement = null;

            try
            {
                switch (graphElement.TypeString)
                {
                    case "InnerChart": // 基础图元
                        {
                            dataElement = new InnerChartEditor();

                            break;
                        }
                    case "InterfaceNode": // 基础图元
                        {
                            dataElement = new InterfaceNodeEditor();

                            break;
                        }
                    default: // 自定义图元
                        {
                            DataElementInfo dataElementInfo = dataElementInfoDirectory[graphElement.TypeString];

                            if (dataElementInfo != null)
                            {
                                dataElement = dataElementInfo.GetDataElement();                                                                    
                            }

                            break;
                        }
                }

                if (dataElement != null)
                {
                    dataElement.ID = graphElement.ID;
                    dataElement.DataType = graphElement.TypeString;
                }
            }
            catch (Exception ex)
            {
                executeSuccess = false;
                MessageBox.Show("在初始化数据元时产生异常: " + ex.ToString(), "数据元初始化", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            flowChartManager.CurrentDataManager.InitializedDataElement = dataElement;

            return executeSuccess;
        }

        /// <summary>
        /// 弹出右键菜单
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicPopUpMenuStrip(FlowChartManager flowChartManager, object logicData)
        {
            Point popUpLocation = (Point)logicData;
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            GraphManager graphManager = flowChartManager.CurrentGraphManager;

            if (graphManager.SelectedGraphElement is SlotContainer) // 有AI插槽容器被选中
            {                
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

                if (flowChartManager.MapName == "子绘图") // 子绘图不允许重设图元ID
                {
                    canvasMenu.SubItems["bResetGraphElementID"].Enabled = false;
                }
                else
                {
                    canvasMenu.SubItems["bResetGraphElementID"].Enabled = true;
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
        /// <return>是否操作成功</return>
        protected virtual bool LogicCreateNodeAtPoint(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            object[] args = logicData as object[];
            int graphType = (int)args[0];
            Point location = (Point)args[1];

            switch (graphType)
            {
                case 1: // 创建事件结点
                    {
                        CreateNode(GraphType.EventNode, location, false);
                        break;
                    }
                case 2: // 创建条件结点
                    {
                        if (flowChartManager.CurrentGraphManager.SelectedGraphElement is SlotContainer) // 必须在有可连接的结点的基础上创建
                        {
                            CreateNode(GraphType.ConditionNode, location, true);
                        }
                        else
                        {
                            CreateNode(GraphType.ConditionNode, location, false);
                        }

                        break;
                    }
                case 3: // 创建动作结点
                    {
                        if (flowChartManager.CurrentGraphManager.SelectedGraphElement is SlotContainer) // 必须在有可能连接的结点的基础上创建
                        {
                            CreateNode(GraphType.ActionNode, location, true);
                        }
                        else 
                        {
                            CreateNode(GraphType.ActionNode, location, false);
                        }

                        break;
                    }
                default:
                    {
                        break;
                    }                    
            }

            return true;
        }

        /// <summary>
        /// 打开脚本
        /// </summary>
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicViewCode(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            Helper helper = Helper.GetHelper();
            string relevatePath = logicData as string;
            string path = Path.Combine(helper.OutputDir, string.Format("{0}.lua", relevatePath));
            
            if (File.Exists(path)) // 文件存在
            {
                try
                {
                    System.Diagnostics.Process.Start(path);
                }
                catch (IOException ex)
                {
                    MessageBox.Show("在查看脚本时产生IO异常：" + ex.Message, "打开脚本",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show(string.Format("脚本文件{0}不存在！", path), "打开脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return true;
        }

        /// <summary>
        /// 打开文件夹
        /// </summary>
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicViewFolder(FlowChartManager flowChartManager, object logicData)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            Helper helper = Helper.GetHelper();
            TreeNode currentNode = logicData as TreeNode;

            string path = Path.Combine(helper.OutputDir, documentManager.GetNodePath(currentNode));
                        
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
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicSave(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 生成脚本
        /// </summary>
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicCompileCode(FlowChartManager flowChartManager, object logicData)
        {
            bool executeSuccess = GenerateCode();

            return executeSuccess;
        }

        /// <summary>
        /// 检查逻辑
        /// </summary>
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicCheckLogic(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 管理变量
        /// </summary>
        /// <param name="flowChartManager">逻辑管理器</param>
        /// <param name="logicData">逻辑数据</param>
        /// <returns>是否操作成功</returns>
        protected virtual bool LogicManageArguments(FlowChartManager flowChartManager, object logicData)
        {
            return true;
        }

        /// <summary>
        /// 执行逻辑功能
        /// </summary>
        /// <param name="flowChartManager">绘图管理器</param>
        /// <param name="logicType">逻辑类型</param>
        /// <param name="logicData">逻辑数据</param>
        /// <return>执行结果</return>
        public virtual bool DoLogicOperation(FlowChartManager flowChartManager, LogicType logicType, object logicData)
        {
            bool executeSuccess = true;

            switch (logicType)
            {
                case LogicType.Connect: // 连接图元
                    {
                        executeSuccess = LogicConnect(flowChartManager, logicData);
                        break;
                    }
                case LogicType.Disconnect: // 解除连接图元
                    {
                        executeSuccess = LogicDisconnect(flowChartManager, logicData);
                        break;
                    }
                case LogicType.BeforeDelete: // 删除图元之前
                    {
                        executeSuccess = LogicBeforeDelete(flowChartManager, logicData);
                        break;
                    }
                case LogicType.AfterDelete: // 删除图元之后
                    {
                        executeSuccess = LogicAfterDelete(flowChartManager, logicData);
                        break;
                    }
                case LogicType.Edit: // 编辑图元
                    {
                        executeSuccess = LogicEdit(flowChartManager, logicData);
                        break;
                    }
                case LogicType.Paste: // 粘贴图元
                    {
                        executeSuccess = LogicPaste(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CreateLine: // 创建连接线
                    {
                        executeSuccess = LogicCreateLine(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CreateAndConnectLine: // 创建并连接连接线
                    {
                        executeSuccess = LogicCreateAndConnectLine(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CreateChart: // 创建绘图
                    {
                        executeSuccess = LogicCreateChart(flowChartManager, logicData);
                        break;
                    }
                case LogicType.AfterLoad: // 加载绘图之后的处理
                    {
                        executeSuccess = LogicAfterLoad(flowChartManager, logicData);
                        break;
                    }
                case LogicType.InitializeDataElement: // 初始化数据元
                    {
                        executeSuccess = LogicInitializeDataElement(flowChartManager, logicData);
                        break;
                    }
                case LogicType.PopUpContextMenu: // 弹出右键菜单
                    {
                        executeSuccess = LogicPopUpMenuStrip(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CreateAtPoint: // 在指定点创建图元
                    {
                        executeSuccess = LogicCreateNodeAtPoint(flowChartManager, logicData);
                        break;
                    }
                case LogicType.ViewCode: // 打开脚本
                    {
                        executeSuccess = LogicViewCode(flowChartManager, logicData);
                        break;
                    }
                case LogicType.ViewFolder: // 打开文件夹
                    {
                        executeSuccess = LogicViewFolder(flowChartManager, logicData);
                        break;
                    }
                case LogicType.Save: // 保存绘图
                    {
                        executeSuccess = LogicSave(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CompileCode: // 生成脚本
                    {
                        executeSuccess = LogicCompileCode(flowChartManager, logicData);
                        break;
                    }
                case LogicType.CheckLogic: // 检查逻辑
                    {
                        executeSuccess = LogicCheckLogic(flowChartManager, logicData);
                        break;
                    }
                case LogicType.ManageArguments: // 管理变量
                    {
                        executeSuccess = LogicManageArguments(flowChartManager, logicData);
                        break;
                    }             
                default:
                    {
                        executeSuccess = false;
                        break;
                    }
            }

            return executeSuccess;
        }

        /// <summary>
        /// 获取图元数据链表
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="slotContainerList">插槽容器链表</param>
        /// <returns>图元数据链表</returns>
        protected virtual List<FlowChartMetaData> GetMetaData(DataManager dataManager, List<SlotContainer> slotContainerList)
        {
            List<FlowChartMetaData> metaDataList = new List<FlowChartMetaData>();
            List<GraphElement> graphElementList = new List<GraphElement>();
            Hashtable graphElementTable = new Hashtable();

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                if (slotContainer.IsEntrance)
                {
                    object data = dataManager.GetData(slotContainer);
                    DataElement dataElement = dataManager.GetDataElement(slotContainer);
                    FlowChartMetaData metaData = new FlowChartMetaData(slotContainer.ID, data, slotContainer.TypeString);
                    metaData.DisplayText = slotContainer.TooltipText;
                    FindNextMetaData(dataManager, slotContainer, metaData, graphElementList, graphElementTable);
                    metaDataList.Add(metaData);
                }
            }

            return metaDataList;
        }

        /// <summary>
        /// 获取下一个图元数据
        /// </summary>
        /// <param name="dataManager">数据管理器</param>
        /// <param name="graphElement">当前图元</param>
        /// <param name="metaData">图元数据</param>
        /// <param name="graphElementList">遍历过的图元链表</param>
        /// <param name="graphElementTable">绘图索引哈希表</param>
        private void FindNextMetaData(DataManager dataManager, GraphElement graphElement, FlowChartMetaData metaData, List<GraphElement> graphElementList, Hashtable graphElementTable)
        {
            graphElementList.Add(graphElement);
            graphElementTable[graphElement] = metaData;
            List<GraphElement> list = dataManager.GetNextGraphElements(graphElement, false);

            foreach (GraphElement g in list)
            {
                object data = dataManager.GetData(g);

                if (!graphElementList.Contains(g))
                {
                    DataElement dataElement = dataManager.GetDataElement(g);
                    FlowChartMetaData newMetaData = new FlowChartMetaData(g.ID, data, dataElement.DataType);
                    newMetaData.DisplayText = g.TooltipText;
                    newMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(newMetaData);
                    FindNextMetaData(dataManager, g, newMetaData, graphElementList, graphElementTable);
                }
                else
                {
                    FlowChartMetaData newMetaData = graphElementTable[g] as FlowChartMetaData;
                    newMetaData.AddPreviousMetaData(metaData);
                    metaData.AddNextMetaData(newMetaData);
                }
            }
        }

        /// <summary>
        /// 生成脚本
        /// </summary>
        /// <returns>是否生成成功</returns>
        protected virtual bool GenerateCode()
        {
            bool success = true;
            Helper helper = Helper.GetHelper();
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            Hashtable fileTable = documentManager.FileTable;
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;

            if (flowChartManager == null)
            {
                return false;
            }

            string id = flowChartManager.ID;
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;

            List<SlotContainer> slotContainerList = graphManager.SlotContainerList;
            List<FlowChartMetaData> metaDataList = GetMetaData(dataManager, slotContainerList);

            if (compiler  == null)
            {
                return false;
            }

            compiler.PrintDebugInformation = documentManager.PrintText;
            compiler.SaveTheFile = documentManager.SaveTextToFile;
            FlowChartMetaDataManager flowChartMetaDataManager = new FlowChartMetaDataManager(int.Parse(id), metaDataList);
            fileTable.Clear();
            documentManager.PrintText("------ 开始初始化脚本 ------");

            // 设置生成脚本参数
            compiler.RootDirectory = helper.OutputDir;
            compiler.ScriptPath = flowChartManager.CurrentPath;
            flowChartMetaDataManager.MapName = flowChartManager.MapName;
            flowChartMetaDataManager.Version = flowChartMetaDataManager.Version;

            try
            {
                success = compiler.GenerateCode(int.Parse(id), metaDataList, flowChartMetaDataManager);
            }
            catch (Exception ex) // 抛出异常则输出调试信息
            {
                documentManager.PrintText(string.Format("异常信息：{0}", ex.Message));                
                success = false;
            }

            if (!success)
            {
                documentManager.PrintText("初始化脚本失败！");
                return false;
            }            

            documentManager.PrintText("初始化脚本成功！");

            // 写文件
            documentManager.PrintText("------ 开始生成脚本 ------");
                    
            foreach (string fileName in fileTable.Keys)
            {
                success = helper.SaveFile(fileName, fileTable[fileName] as string);

                if (success) // 写文件成功
                {
                    documentManager.PrintText(string.Format("生成脚本文件 {0} 成功！", fileName));
                }
                else
                {
                    documentManager.PrintText(string.Format("生成脚本文件 {0} 失败！", fileName));
                    break;
                }
            }

            if (!success)
            {
                return false;
            }

            documentManager.PrintText(string.Format("====== 所有文件都成功生成！共生成{0}个文件 ======", fileTable.Keys.Count));                              
                                       
            return success;
        }

        /// <summary>
        /// 数据元信息
        /// </summary>
        protected class DataElementInfo
        {
            private static Dictionary<string, Assembly> assemblyDictionary = new Dictionary<string, Assembly>();
            private readonly string graphType;
            private readonly string filePath;
            private readonly string className;

            public DataElementInfo(string graphType, string filePath, string className)
            {
                this.graphType = graphType;
                this.filePath = filePath;
                this.className = className;
            }

            public string GraphType
            {
                get
                {
                    return graphType;
                }
            }

            public string FilePath
            {
                get
                {
                    return filePath;
                }
            }

            public string ClassName
            {
                get
                {
                    return className;
                }
            }

            /// <summary>
            /// 获取数据元
            /// </summary>
            /// <returns>数据元实例</returns>
            public DataElement GetDataElement()
            {
                DataElement dataElement = null;

                if (assemblyDictionary.ContainsKey(graphType))
                {
                    Assembly assembly = assemblyDictionary[graphType];
                    
                    if (assembly != null)
                    {
                        dataElement = assembly.CreateInstance(className) as DataElement;
                    }
                }
                else
                {
                    if (File.Exists(filePath))
                    {
                        Assembly assembly = Assembly.LoadFrom(filePath);

                        if (assembly != null)
                        {
                            dataElement = assembly.CreateInstance(className) as DataElement;
                            assemblyDictionary[graphType] = assembly;
                        }                        
                    }
                }

                return dataElement;
            }
        }
    }
}
