using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Drawing;

using FlowChartEditor.BaseGraphElement;
using FlowChartEditor.ButtonGraphElement;
using FlowChartEditor.Command.FlowChartCommand;
using GraphElementLibrary;

namespace FlowChartEditor
{
    public class GraphManager
    {
        // 控件
        private Canvas canvas; // 绘图面板
        private Background background; // 画图背景

        // 管理器
        private DocumentManager documentManager; // 主控制器
        private Helper helper; // 辅助类
        private FlowChartManager flowChartManager; // 流程图管理器
        private RegionManager regionManager = new RegionManager(); // 区域管理器
        private GraphSetting graphSetting = GraphSetting.GetGraphSetting(); // 绘图参数配置对象
        private EventManager eventManager; // 事件管理器

        // 数据
        private GraphElement selectedGraphElement; // 当前选中的图元
        private Point lastSelectedPoint; // 上一个选择的点           
        private Point multiSelectStartPoint; // 多选图元的起始点
        private GraphElement lastActivatedGraphElement; // 上一个选中的图元
        private GraphElement lastSelectedGraphElement; // 上一个激活的图元
        private SlotContainer lastResizingGraphElement; // 上一个缩放的图元
        private GraphElement lastConnectGraphElement; // 上一个连接的图元

        private bool controlMode = false; // 是否是Control键特殊状态
        private bool shiftMode = false; // 是否是Shift键特殊状态
        private bool altMode = false; // 是否是Alt键特殊状态         
        private bool autoConnect = false; // 是否自动连接图元

        private List<SlotContainer> slotContainerList = new List<SlotContainer>(); // 插槽容器链表
        private List<ConnectorContainer> connectorContainerList = new List<ConnectorContainer>(); // 连接线控制点容器链表
        private List<GraphElement> selectedGraphElementList = new List<GraphElement>(); // 选中的图元链表
        private List<GraphElement> moveGraphElementList = new List<GraphElement>(); // 移动的图元链表
                
        private Size moveOffset = Size.Empty; // 移动图元的偏移
        private Rectangle multiMoveRegion; // 移动图元的区域
        private int scrollOffset = 25; // 绘图板滚动的偏移
        
        private long lastMoveTime; // 上一次移动图元的时间
        private int moveLagTime = 200; // 图元移动延迟时间
        private bool moveLagged = false; // 是否已经延迟移动 
        private Direction lastLagDirection = Direction.None; // 上一次移动延迟的方向
        private int lastLagPosition = -1; // 上一次移动延迟的位置
        private int moveAdjustWidth = 5; // 图元移动调整的间距    
        private int moveLagOffset = 3; // 图元延迟移动的偏移
        private bool lockMoveDrag = false; // 是否锁定鼠标拖动图元  
      
        private int idIndex = 0; // 自动分配id的序号

        // 状态
        private enum MoveType { SingleElement, MultiElement, AbbreviateElement }; // 移动的类型       
        private enum UserOperation { None, Create, SingleSelect, MultiSelect, RegionSelect, ScrollCanvas, MoveRodman }; // 用户操作枚举
        private UserOperation userOperation = UserOperation.None; // 当前用户操作
        
        // 命令支持
        private List<FlowChartBaseCommand> commandList = new List<FlowChartBaseCommand>(); // 命令链表
        private int commandIndex = -1; // 当前命令所在的序号
        private FlowChartMoveCommand moveCommand; // 移动命令

        /// <summary>
        /// 单例模式的构造函数
        /// </summary>
        /// <param name="mainForm">主窗体</param>
        /// <param name="canvas">绘图面板</param>
        /// <param name="propertyGrid">属性面板</param>
        public GraphManager(FlowChartManager flowChartManager, Canvas canvas)
        {
            this.flowChartManager = flowChartManager;
            this.canvas = canvas;
            helper = Helper.GetHelper();
            documentManager = DocumentManager.GetDocumentManager();

            // 初始化背景绘图参数
            InitBackground(); 
      
            // 初始化区域管理器
            regionManager.Init(slotContainerList, connectorContainerList, background.BaseSize, background.GridSize.Width);
        }               

        /// <summary>
        /// 事件管理器
        /// </summary>
        public EventManager CurrentEventManager
        {
            set
            {
                eventManager = value;
            }

            get
            {
                return eventManager;
            }
        }

        /// <summary>
        /// 当前的命令索引
        /// </summary>
        public int CommandIndex
        {
            get
            {
                return commandIndex;
            }
        }

        /// <summary>
        /// 命令总数
        /// </summary>
        public int CommandCount
        {
            get
            {
                return commandList.Count;
            }
        }

        /// <summary>
        /// 命令的描述链表
        /// </summary>
        public List<string> CommandDescriptionList
        {
            get
            {
                List<string> descriptionList = new List<string>();

                foreach (FlowChartBaseCommand command in commandList)
                {
                    descriptionList.Add(string.Format("撤销 ({0})", command.Description));
                }

                return descriptionList;
            }
        }

        /// <summary>
        /// 自动分配给图元的起始ID
        /// </summary>
        public int IDIndex
        {            
            get
            {
                return idIndex;
            }
        }

        /// <summary>
        /// 缩略图绘图板
        /// </summary>
        public AbbreviateCanvas CurrentAbbreviateCanvas
        {
            set
            {
                canvas.CurrentAbbreviateCanvas = value;
                value.CurrentCanvas = canvas;
            }                        
        }

        /// <summary>
        /// 当前绘图板
        /// </summary>
        public Canvas CurrentCanvas
        {
            get
            {
                return canvas;
            }
        }

        /// <summary>
        ///  当前绘图板数据类
        /// </summary>
        public Background CurrentBackground
        {
            get
            {
                return background;
            }
        }

        /// <summary>
        /// 流程图管理器对象
        /// </summary>
        public FlowChartManager CurrentFlowChartManager
        {
            get
            {
                return flowChartManager;
            }
        }        

        /// <summary>
        /// 当前选中的图元
        /// </summary>
        public GraphElement SelectedGraphElement
        {
            get
            {
                return selectedGraphElement;
            }
        }

        /// <summary>
        /// 选中的图元链表
        /// </summary>
        public List<GraphElement> SelectedGraphElementList
        {
            get
            {
                return selectedGraphElementList;
            }
        }

        /// <summary>
        /// 连接线控制点链表
        /// </summary>
        public List<ConnectorContainer> ConnectorContainerList
        {
            get
            {
                return connectorContainerList;
            }
        }

        /// <summary>
        /// 插槽容器链表
        /// </summary>
        public List<SlotContainer> SlotContainerList
        {
            get
            {
                return slotContainerList;
            }
        }

        /// <summary>
        /// 是否是Control键特殊状态
        /// </summary>
        public bool ControlMode
        {
            set
            {
                controlMode = value;

                if (!value)
                {
                    canvas.AutoScroll = true;
                }
            }

            get
            {
                return controlMode;
            }
        }

        /// <summary>
        /// 是否是Shift键特殊状态
        /// </summary>
        public bool ShiftMode
        {
            set
            {
                shiftMode = value;                
            }

            get
            {
                return shiftMode;
            }
        }

        /// <summary>
        /// 是否是Alt键特殊状态
        /// </summary>
        public bool AltMode
        {
            set
            {
                altMode = value;
            }

            get
            {
                return altMode;
            }
        }

        /// <summary>
        /// 初始化绘图板背景
        /// </summary>
        private void InitBackground()
        {
            background = new Background();
            canvas.CanvasBackgound = background;
            canvas.AutoScroll = true;
            canvas.AutoScrollMinSize = background.CanvasSize;

            background.Refresh();
        }

        /// <summary>
        /// 获取流程图的绘图参数
        /// </summary>
        /// <returns>流程图的绘图参数哈希表</returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();
            args["slotContainerList"] = slotContainerList;
            args["connectorContainerList"] = connectorContainerList;
            args["background"] = background;            
            args["idIndex"] = idIndex;

            return args;
        }

        /// <summary>
        /// 设置流程图的绘图参数
        /// </summary>
        /// <param name="args">流程图的绘图哈希表</param>
        public void SetArguments(Hashtable args)
        {
            slotContainerList = args["slotContainerList"] as List<SlotContainer>;
            connectorContainerList = args["connectorContainerList"] as List<ConnectorContainer>;
            background = args["background"] as Background;
            idIndex = (int)args["idIndex"];

            selectedGraphElementList = new List<GraphElement>();

            // 刷新画板参数
            background.Refresh();

            // 重设绘图参数
            canvas.CanvasBackgound = background;  
            canvas.AutoScrollMinSize = background.CanvasSize;
            canvas.BackColor = graphSetting.BackgroundColor;
            background.BaseSize = background.CanvasSize;
            graphSetting.CanvasScale = 1.0f;

            // 刷新图元数据
            foreach (SlotContainer slotContainer in slotContainerList)
            {
                slotContainer.Refresh();               
            }

            foreach (ConnectorContainer connectorContainer in connectorContainerList)
            {
                connectorContainer.Refresh();
            }            

            // 初始化区域管理器
            RefreshRegion();

            // 刷新绘图板
            ReconstructCanvasGraphElementList();
        }  

        /// <summary>
        /// 刷新绘图区域
        /// </summary>
        public void RefreshRegion()
        {
            regionManager.Init(slotContainerList, connectorContainerList, background.BaseSize, background.GridSize.Width);
        }

        /// <summary>
        /// 重新分配图元的ID
        /// </summary>
        public void ResetGraphElementID()
        {
            idIndex = 0;

            foreach (SlotContainer node in slotContainerList)
            {
                node.ID = AllocateGraphElementID();
            }

            foreach (ConnectorContainer line in connectorContainerList)
            {
                line.ID = AllocateGraphElementID();
            }
        }

        /// <summary>
        /// 分配图元ID
        /// </summary>
        /// <returns>图元的ID</returns>
        public int AllocateGraphElementID()
        {
            idIndex++;

            return idIndex;
        }

        /// <summary>
        /// 在编辑前使所有图元处于有效状态
        /// </summary>
        public void ValidateBeforeSave()
        {
            // 检查连接线，去掉移动和隐藏状态
            foreach (ConnectorContainer line in connectorContainerList)
            {
                line.Activated = false;
                line.Selected = false;
                line.Moving = false;
                line.Visible = true;                
            }
            
            // 检查插槽容器，解除绑定所有不在连接线链表里的连接线
            foreach (SlotContainer slotContainer in slotContainerList)
            {
                slotContainer.Activated = false;
                slotContainer.Selected = false;
                slotContainer.Moving = false;

                foreach (SlotGraphElement slot in slotContainer.GetSlotList())
                {
                    if (slot.Binded)
                    {
                        if (!connectorContainerList.Contains(slot.BindingConnector.Line))
                        {
                            slot.UnBind();
                        }
                    }
                }
            }

            // 恢复选择状态
            if (selectedGraphElement != null)
            {
                selectedGraphElement.Selected = true;
            }
            else 
            {
                foreach (GraphElement graphElemnt in selectedGraphElementList)
                {
                    graphElemnt.Selected = true;
                }
            }
        }

        /// <summary>
        /// 调整连接线
        /// </summary>
        /// <param name="slotContainer">插槽容器</param>
        private void AdjustLine(SlotContainer slotContainer)
        {
            List<ConnectorContainer> lineList = regionManager.GetConnectorContainerList(slotContainer.InvalidRectangle);

            foreach (ConnectorContainer line in lineList)
            {
                if (line.LineIntersect(slotContainer))
                {
                    AdjustLine(line, slotContainer);
                }
            }
        }   
        
        /// <summary>
        /// 调整连接线
        /// </summary>
        /// <param name="line">当前连接线</param>
        /// <param name="slotContainer">相关的插槽容器</param>
        private void AdjustLine(ConnectorContainer line , SlotContainer slotContainer)
        {
            Rectangle adjustRectangle = line.GetAdjustRectangle(background.CanvasSize, background.GridSize.Width);
            List<SlotContainer> totalSlotContainerList = regionManager.GetSlotContainerList(adjustRectangle);
            List<ConnectorContainer> totalLineList = regionManager.GetConnectorContainerList(adjustRectangle);

            line.AdjustLine(slotContainer, totalSlotContainerList, totalLineList, background.GridSize.Width, background.BaseSize, regionManager.CanvasGridBoard);
            line.AdjustRectangle();  
        }

        /// <summary>
        /// 重设绘图板的大小
        /// </summary>
        /// <param name="plusSize">新增的大小</param>
        private void ResetCanvasSize(Size plusSize)
        {
            if (plusSize.Width != 0 || plusSize.Height != 0) // 先检查是否有必要更新
            {
                background.CanvasSize += plusSize;
                background.BaseSize += plusSize;
                canvas.AutoScrollMinSize = background.CanvasSize;

                // 更新网格数据
                regionManager.InitCanvasGridBoard(slotContainerList, background.BaseSize, background.GridSize.Width);
            }            
        }

        /// <summary>
        /// 最大化绘图板
        /// </summary>
        public void MaximumCanvasSize()
        {
            background.CanvasSize = canvas.ClientSize;
            background.BaseSize = canvas.ClientSize;
            canvas.AutoScrollMinSize = canvas.ClientSize;

            // 更新网格数据
            regionManager.InitCanvasGridBoard(slotContainerList, background.BaseSize, background.GridSize.Width);
        }

        /// <summary>
        /// 调整绘图板的大小
        /// </summary>
        public void AdjustCanvasSize()
        {
            int right = -1;
            int bottom = -1;

            foreach(GraphElement graphElement in canvas.GraphElementList)
            {
                if(graphElement.InvalidRectangle.Right > right)
                {
                    right = graphElement.InvalidRectangle.Right;
                }

                if(graphElement.InvalidRectangle.Bottom > bottom)
                {
                    bottom = graphElement.InvalidRectangle.Bottom;
                }
            }

            if (right < background.MinSize.Width)
            {
                right = background.MinSize.Width;
            }

            if (bottom < background.MinSize.Height)
            {
                bottom = background.MinSize.Height;
            }

            if ((right != background.CanvasSize.Width && right != -1) || 
                (bottom != background.CanvasSize.Height && bottom != -1))
            {
                Size newSize = new Size(right, bottom);

                background.CanvasSize = newSize;
                background.BaseSize = newSize;
                canvas.AutoScrollMinSize = newSize;

                // 更新网格数据
                regionManager.InitCanvasGridBoard(slotContainerList, background.BaseSize, background.GridSize.Width);
            }
        }

        /// <summary>
        /// 初始化第一条命令
        /// </summary>
        /// <param name="command">当前命令</param>
        private void InitFirstCommand(FlowChartBaseCommand command)
        {
            if (commandIndex == -1) // 检查是否是第一条命令
            {
                command.FirstCommand = true;
            }
        }

        /// <summary>
        /// 创建图元
        /// </summary>
        public void CreateGraphElement()
        {            
            Point p = canvas.AbbreviatGraphElement.Location;
            FlowChartCreateCommand cmd = new FlowChartCreateCommand(this, "新建图元");
            InitFirstCommand(cmd);            

            if (cmd.Execute(new object[] { p, canvas.AbbreviatGraphElement.TypeString })) // 命令执行成功
            {
                AdjustCommandList(cmd);                                

                // 自动调整连接线
                AdjustLine(selectedGraphElement as SlotContainer);

                GraphElement graphElement = selectedGraphElement; 
               
                if (autoConnect)
                {
                    ConnectGraphElement(lastConnectGraphElement as SlotContainer, selectedGraphElement as SlotContainer);                   
                }

                if (lastSelectedGraphElement != null) // 将上一个被选中的图元置为非选中状态
                {
                    if (!selectedGraphElementList.Contains(lastSelectedGraphElement)) // 在多选状态需要检查上一个被选中的图元是否在多选容器中
                    {
                        lastSelectedGraphElement.Selected = false;
                    }
                }

                SelectGraphElement(graphElement, p); // 选中新建的图元

                // 编辑新建的图元
                FlowChartEditCommand editCommand = new FlowChartEditCommand(this, "编辑图元");                
                InitFirstCommand(editCommand);

                if (editCommand.Execute(graphElement)) // 命令执行成功
                {
                    AdjustCommandList(editCommand);
                } 

                regionManager.AddToRegion(graphElement); // 调整图元所在区域
            }                                       
        }

        /// <summary>
        /// 自动连接图元
        /// </summary>
        /// <param name="previousContainer">连出的插槽容器</param>
        /// <param name="currentContainer">连入的插槽容器</param>
        private void ConnectGraphElement(SlotContainer previousContainer, SlotContainer currentContainer)
        {
            SlotGraphElement outSlot = previousContainer.GetOutSlot();
            SlotGraphElement inSlot = currentContainer.GetInSlot();            

            if(outSlot != null && inSlot != null) // 可能有些结点不能分配出新插槽
            {
                FlowChartCreateAndConnectLineCommand cmd = new FlowChartCreateAndConnectLineCommand(this, "创建并连接图元");
                InitFirstCommand(cmd);

                if (cmd.Execute(new object[] { outSlot, inSlot })) // 命令执行成功
                {
                    AdjustCommandList(cmd);

                    ConnectorContainer line = selectedGraphElement as ConnectorContainer;
                    regionManager.AddToRegion(line); // 调整图元所在区域                    
                    
                    // 自动调整连接线                    
                    AdjustLine(line, currentContainer);

                    if (previousContainer is ConditionGraphElement) // 编辑条件结点连出的连接线
                    {
                        FlowChartEditCommand editCommand = new FlowChartEditCommand(this, "编辑图元");

                        if (editCommand.Execute(line))
                        {
                            AdjustCommandList(editCommand);
                        }
                    }
                }
            }                       

            autoConnect = false;
        }

        /// <summary>
        /// 调整显示缩略的图元的位置
        /// </summary>
        /// <param name="p">鼠标的位置</param>
        private void AdjustAbbreviatGraphElement(Point p)
        {
            canvas.AbbreviatGraphElement.Location = p - moveOffset;
            RecordLastSelectedPoint(p);
        }

        /// <summary>
        /// 重新构建绘图板的图元链表
        /// </summary>
        public void ReconstructCanvasGraphElementList()
        {
            canvas.PaintCanvas = false;
            canvas.GraphElementList.Clear();

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                canvas.GraphElementList.Add(slotContainer);
            }

            foreach (ConnectorContainer connectorContainer in connectorContainerList)
            {
                canvas.GraphElementList.Add(connectorContainer);
            }

            canvas.PaintCanvas = true;

            // 刷新绘图板
            RefreshCanvas();
        }

        /// <summary>
        /// 释放流程图所有的绘图资源
        /// </summary>
        public void Release()
        {
            foreach(SlotContainer slotContainer in slotContainerList)
            {
                slotContainer.Release();
            }

            foreach(ConnectorContainer connectorContainer in connectorContainerList)
            {
                connectorContainer.Release();
            }
        }

        /// <summary>
        /// 剪切图元
        /// </summary>
        public void CutGraphElement()
        {
            // 将当前图元从所在区域清除
            if (selectedGraphElementList.Count > 0) // 剪切多个图元
            {
                foreach (GraphElement graphElement in selectedGraphElementList)
                {                    
                    regionManager.DeleteFromRegion(graphElement);
                }                
            }
            else if (selectedGraphElement != null) // 剪切单个图元
            {
                regionManager.DeleteFromRegion(selectedGraphElement);
            }

            FlowChartCutCommand cmd = new FlowChartCutCommand(this, "剪切图元");
            InitFirstCommand(cmd);

            if (cmd.Execute(null)) // 命令执行成功
            {
                AdjustCommandList(cmd);
                
                if (selectedGraphElementList.Count > 0) // 剪切多个图元
                {
                    selectedGraphElementList.Clear();
                    canvas.CurrentMultiSelectMark.Visible = false;
                }
                else if (selectedGraphElement != null) // 剪切单个图元
                {
                    selectedGraphElement = null;
                }

                ReconstructCanvasGraphElementList();
            }
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        public void CopyGraphElement()
        {
            FlowChartCopyCommand cmd = new FlowChartCopyCommand(this, "复制图元");
            InitFirstCommand(cmd);

            if (cmd.Execute(null)) // 命令执行成功
            {
                AdjustCommandList(cmd);                
            }
        }

        /// <summary>
        /// 自动调整拖动后的绘图板大小
        /// </summary>
        private void AdjustOutOfBorder()
        {
            int right = background.BaseSize.Width;
            int bottom = background.BaseSize.Height;
            Rectangle multiSelectRegion = canvas.CurrentMultiSelectMark.RegionRectangle;

            switch (userOperation)
            {
                case UserOperation.SingleSelect: // 拖动单个图元
                    {
                        if (selectedGraphElement.InvalidRectangle.Right > right)
                        {
                            right = selectedGraphElement.InvalidRectangle.Right;
                        }

                        if (selectedGraphElement.InvalidRectangle.Bottom > bottom)
                        {
                            bottom = selectedGraphElement.InvalidRectangle.Bottom;
                        }

                        break;
                    }
                case UserOperation.MultiSelect: // 拖动图元组
                    {
                        if (multiSelectRegion.Right > right)
                        {
                            right = multiSelectRegion.Right;
                        }

                        if (multiSelectRegion.Bottom > bottom)
                        {
                            bottom = multiSelectRegion.Bottom;
                        }

                        break;
                    }
                case UserOperation.MoveRodman: // 拖动游标
                    {
                        if (multiMoveRegion.Right > right)
                        {
                            right = multiMoveRegion.Right;
                        }

                        if (multiMoveRegion.Bottom > bottom)
                        {
                            bottom = multiMoveRegion.Bottom;
                        }

                        break;
                    }
                default:
                    {                        
                        if (selectedGraphElementList.Count > 0)
                        {
                            if (multiSelectRegion.Right > right)
                            {
                                right = multiSelectRegion.Right;
                            }

                            if (multiSelectRegion.Bottom > bottom)
                            {
                                bottom = multiSelectRegion.Bottom;
                            }
                        }
                        else if (selectedGraphElement != null)
                        {
                            if (selectedGraphElement.InvalidRectangle.Right > right)
                            {
                                right = selectedGraphElement.InvalidRectangle.Right;
                            }

                            if (selectedGraphElement.InvalidRectangle.Bottom > bottom)
                            {
                                bottom = selectedGraphElement.InvalidRectangle.Bottom;
                            }
                        }

                        break;
                    }
            }

            if (right != background.BaseSize.Width || bottom != background.BaseSize.Height)
            {
                background.CanvasSize = new Size((int)Math.Ceiling(right * graphSetting.CanvasScale), 
                                                 (int)Math.Ceiling(bottom * graphSetting.CanvasScale));
                background.BaseSize = new Size(right, bottom);
                canvas.AutoScrollMinSize = background.CanvasSize;
                RefreshCanvas();

                // 更新网格数据
                regionManager.InitCanvasGridBoard(slotContainerList, background.BaseSize, background.GridSize.Width);                
            }
        }

        /// <summary>
        /// 缩放绘图板
        /// </summary>
        /// <param name="scale">缩放参数</param>
        public void SetCanvasScale(float scale)
        {
            graphSetting.CanvasScale = scale;
            background.CanvasSize = new Size((int)Math.Ceiling(background.BaseSize.Width * scale),
                                             (int)Math.Ceiling(background.BaseSize.Height * scale));
            canvas.AutoScrollMinSize = background.CanvasSize;

            RefreshCanvas();            
        }

        /// <summary>
        /// 缩放绘图板
        /// </summary>
        /// <param name="scale">缩放参数</param>
        public void AdjustCanvasScale(float scale)
        {
            graphSetting.CanvasScale *= scale;            
            background.CanvasSize = new Size((int)Math.Ceiling(background.CanvasSize.Width * scale),
                                             (int)Math.Ceiling(background.CanvasSize.Height * scale));
            canvas.AutoScrollMinSize = background.CanvasSize;

            RefreshCanvas();
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        public void PasteGraphElement()
        {
            Point pastePoint = canvas.PointToClient(Cursor.Position) - (Size)canvas.AutoScrollPosition; // 粘贴位置
            FlowChartPasteCommand cmd = new FlowChartPasteCommand(this, "粘贴图元");
            InitFirstCommand(cmd);

            if (cmd.Execute(pastePoint)) // 命令执行成功
            {
                AdjustCommandList(cmd);

                bool multiPaste = false;

                if(flowChartManager.PasteTable.Keys.Count > 1) // 复制多个图元
                {
                    multiPaste = true;

                    // 清空多选区域
                    ClearSelectedGraphElementList();
                }

                // 将粘贴的图元加入到区域中
                foreach(object o in flowChartManager.PasteTable.Keys)
                {
                    GraphElement graphElement = o as GraphElement;

                    if (graphElement is ConnectorContainer) // 连接线
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;
                        line.AdjustRectangle();
                    }

                    regionManager.AddToRegion(graphElement);

                    if (multiPaste)
                    {                        
                        selectedGraphElementList.Add(graphElement);
                    }
                    else
                    {
                        selectedGraphElement = graphElement;
                    }

                    graphElement.Selected = true;
                }

                // 选中粘贴的多个图元
                if (multiPaste)
                {                    
                    CreateMultiSelectRegion();
                }

                // 自动调整绘图板大小
                AdjustOutOfBorder();
               
                ReconstructCanvasGraphElementList();

                // 执行逻辑操作                
                List<GraphElement> graphElementList = new List<GraphElement>();

                if (multiPaste)
                {
                    graphElementList.AddRange(selectedGraphElementList);
                }
                else
                {
                    graphElementList.Add(selectedGraphElement);
                }

                LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
                logicManager.DoLogicOperation(flowChartManager, LogicType.Paste, graphElementList);                
            }
        }

        /// <summary>
        /// 弹出绘图板的右键菜单
        /// </summary>
        /// <param name="p">弹出位置</param>
        public void ShowCanvasContextMenu(Point point)
        {
            Point p = point + new Size(canvas.RectangleToScreen(canvas.ClientRectangle).Location); // 坐标转换            

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.PopUpContextMenu, p);                      
        }

        /// <summary>
        /// 编辑图元
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        public void EditDataElement(GraphElement graphElement)
        {
            FlowChartEditCommand cmd = new FlowChartEditCommand(this, "编辑数据元");
            InitFirstCommand(cmd);

            if (cmd.Execute(graphElement)) // 命令执行成功
            {
                AdjustCommandList(cmd);
                regionManager.ChangeRegion(selectedGraphElement);
            }
        }

        /// <summary>
        /// 编辑图元
        /// </summary>
        public void EditDataElement()
        {
            if (selectedGraphElement != null) // 有选中的图元
            {
                if (selectedGraphElement.Enable) // 选中的图元可以编辑
                {
                    if (selectedGraphElement is RemarkGraphElement) // 编辑注释
                    {
                        FlowChartEditRemarkCommand cmd = new FlowChartEditRemarkCommand(this, "编辑元注释");
                        InitFirstCommand(cmd);

                        if (cmd.Execute((selectedGraphElement as RemarkGraphElement).Owner)) // 命令执行成功
                        {
                            AdjustCommandList(cmd);
                        }
                    }
                    else // 编辑图元
                    {
                        FlowChartEditCommand cmd = new FlowChartEditCommand(this, "编辑图元");
                        InitFirstCommand(cmd);

                        if (cmd.Execute(selectedGraphElement)) // 命令执行成功
                        {
                            AdjustCommandList(cmd);
                            regionManager.ChangeRegion(selectedGraphElement);                         
                        }                        
                    }                    
                }
                else
                {
                    MessageBox.Show("当前图元数据无效，请检查是否连接正确！", "图元编辑", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }                
            }
        }

        /// <summary>
        /// 创建缩略的图元
        /// </summary>
        /// <param name="graphType">图元类型</param>
        /// <param name="centerLocation">图元中心位置</param>
        /// <param name="elementSize">图元大小</param>
        /// <param name="moveSize">图元的移动大小</param>
        /// <returns>图元对象</returns>
        private GraphElement CreateAbbreviateGraphElement(GraphType graphType, Point centerLocation)
        {
            GraphElement abbreviateGraphElement = null;

            switch (graphType)
            {
                case GraphType.ConditionNode: // 条件结点
                    {
                        abbreviateGraphElement = new ConditionGraphElement(centerLocation - graphSetting.ConditionNodeMoveOffset, graphSetting.ConditionNodeElementSize);
                        moveOffset = graphSetting.ConditionNodeMoveOffset;

                        break;
                    }
                case GraphType.ActionNode: // 动作结点
                    {
                        abbreviateGraphElement = new ActionGraphElement(centerLocation - graphSetting.ActionNodeMoveOffset, graphSetting.ActionNodeElementSize);
                        moveOffset = graphSetting.ActionNodeMoveOffset;

                        break;
                    }
                case GraphType.EventNode: // 事件结点
                    {
                        abbreviateGraphElement = new EventGraphElement(centerLocation - graphSetting.EventNodeMoveOffset, graphSetting.EventNodeElementSize);
                        moveOffset = graphSetting.EventNodeMoveOffset;

                        break;
                    }
                case GraphType.AIStateNode: // AI状态结点
                    {
                        abbreviateGraphElement = new AIStateGraphElement(centerLocation - graphSetting.AIStateNodeMoveOffset, graphSetting.AIStateNodeElementSize);
                        moveOffset = graphSetting.AIStateNodeMoveOffset;

                        break;
                    }
                case GraphType.AIActionNode: // AI动作结点
                    {
                        abbreviateGraphElement = new AIActionGraphElement(centerLocation - graphSetting.AIActionNodeMoveOffset, graphSetting.AIActionsNodeElementSize);
                        moveOffset = graphSetting.AIActionNodeMoveOffset;

                        break;
                    }
                case GraphType.AIActionsNode: // AI动作组结点
                    {
                        abbreviateGraphElement = new AIActionsGraphElement(centerLocation - graphSetting.AIActionsNodeMoveOffset, graphSetting.AIActionsNodeElementSize);
                        moveOffset = graphSetting.AIActionsNodeMoveOffset;

                        break;
                    }
                case GraphType.InnerChart: // 子绘图结点
                    {
                        abbreviateGraphElement = new InnerChart(centerLocation - graphSetting.InnerChartMoveOffset, graphSetting.InnerChartElementSize);
                        moveOffset = graphSetting.InnerChartMoveOffset;

                        break;
                    }
                case GraphType.InterfaceNode: // 接口结点
                    {
                        abbreviateGraphElement = new InterfaceGraphElement(centerLocation - graphSetting.InterfaceNodeMoveOffset, graphSetting.InterfaceNodeElementSize);
                        moveOffset = graphSetting.InterfaceNodeMoveOffset;

                        break;
                    }
            }

            if (abbreviateGraphElement != null)
            {
                abbreviateGraphElement.Refresh();
            }

            return abbreviateGraphElement;
        }

        /// <summary>
        /// 创建显示缩略的图元
        /// </summary>
        /// <param name="graphType">图元的类型</param>
        /// <param name="point">图元的位置</param>
        /// <param name="autoConnect">是否自动连接</param>
        /// <param name="jumpConnect">是否跳转连接</param>
        public void CreateAbbreviateGraphElement(GraphType graphType, Point p, bool autoConnect)
        {
            Point point = Point.Empty;

            if (!p.IsEmpty)
            {
                point = p - new Size(canvas.AutoScrollPosition);
            }

            canvas.AbbreviatGraphElement = CreateAbbreviateGraphElement(graphType, point);
            userOperation = UserOperation.Create;
            this.autoConnect = autoConnect;

            if (autoConnect) // 记录当前选中的插槽容器
            {
                lastConnectGraphElement = selectedGraphElement as SlotContainer;

                // 创建缩略图元的连接线
                int tailX = (int)(lastConnectGraphElement.Location.X + lastConnectGraphElement.ElementSize.Width / 2);
                int tailY = (int)(lastConnectGraphElement.Location.Y + lastConnectGraphElement.ElementSize.Height);
                int headX = (int)(canvas.AbbreviatGraphElement.Location.X + canvas.AbbreviatGraphElement.ElementSize.Width / 2);
                int headY = (int)(canvas.AbbreviatGraphElement.Location.Y);

                canvas.AbbreviateLine = new ConnectorContainer(new Point(tailX, tailY), new Point(headX, headY));
                canvas.AbbreviateLine.Init();
            }
            else // 清空连接线
            {
                canvas.AbbreviateLine = null;
            }

            if (!p.IsEmpty) // 显示提示信息
            {
                InitTooltipText(canvas.AbbreviatGraphElement, "<underline>鼠标拖动移动图元\r\n<underline>鼠标点击创建图元\r\n<underline>按Esc键取消创建图元", point);
            }

            RefreshCanvas();
        }

        /// <summary>
        /// 标记插槽容器相关的连接线
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        /// <param name="mark">是否标记</param>
        private void MarkRelavateLines(SlotContainer slotContainer, bool mark)
        {            
            foreach (ConnectorContainer line in slotContainer.GetConnectedLine())
            {
                line.Marked = mark;
            }
        }

        /// <summary>
        /// 重设编辑状态
        /// </summary>
        public void ResetUserOperation()
        {
            userOperation = UserOperation.None;
        }

        /// <summary>
        /// 删除显示缩略的图元
        /// </summary>
        public void DeleteAbbreviateGraphElement()
        {
            userOperation = UserOperation.None;
            canvas.AbbreviatGraphElement = null;
            canvas.AbbreviateLine = null;
            RefreshCanvas();
        }               

        /// <summary>
        /// 初始化文本提示
        /// </summary>
        /// <param name="graphElement">需要文本提示的图元</param>
        /// <param name="location">文本提示的位置</param>
        private void InitTooltipText(GraphElement graphElement, Point location)
        {
            string tooltipText = "";

            if (!string.IsNullOrEmpty(graphElement.TooltipText) &&
                graphElement.Text != graphElement.TooltipText) // 显示图元的提示文本
            {
                tooltipText = graphElement.TooltipText;
            }

            if (selectedGraphElementList.Contains(graphElement)) // 多选图元状态
            {
                tooltipText = "<underline>鼠标拖动移动框选的图元组";
            }
            else
            {
                if (graphElement is ConnectorGraphElement) // 激活的图元是连接线控制点
                {
                    ConnectorGraphElement connector = graphElement as ConnectorGraphElement;

                    if (connector.IsHeadPoint || connector.IsTailPoint) // 连接线控制点是头结点或尾结点
                    {
                        tooltipText = "<underline>鼠标拖动连接图元结点或调整连接线形状";
                    }
                    else
                    {
                        tooltipText = "<underline>鼠标拖动调整连接线形状";
                    }
                }
                else if (graphElement is ConnectorContainer) // 激活的图元是连接线
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;

                    if (tooltipText == "")
                    {
                        tooltipText = line.Text;
                    }
                    
                    if (tooltipText != "")
                    {
                        if (line.InSlotContainer != null && line.OutSlotContainer != null) // 显示连接两端的内容
                        {
                            tooltipText = string.Format("<bold>分支内容\r\n{0}\r\n<split>\r\n<bold>连接信息\r\n{1} --> {2}\r\n<split>\r\n<underline>按<>键切换到连接线两端结点\r\n<underline>鼠标双击或按Enter编辑图元",
                                                        tooltipText, line.InSlotContainer.Text, line.OutSlotContainer.Text);
                        }
                        else
                        {
                            tooltipText = string.Format("<bold>分支内容\r\n{0}\r\n<split>\r\n<underline>鼠标双击或按Enter键编辑图元",
                                                        tooltipText);
                        }
                    }
                    else
                    {
                        if (line.InSlotContainer != null && line.OutSlotContainer != null) // 显示连接两端的内容
                        {
                            tooltipText = string.Format("<bold>连接信息\r\n{0} --> {1}\r\n<split>\r\n<underline>按<>键切换到连接线两端结点\r\n<underline>鼠标双击或按Enter编辑图元",
                                                        line.InSlotContainer.Text, line.OutSlotContainer.Text);
                        }
                        else
                        {
                            tooltipText = "<underline>鼠标双击或按Enter键编辑图元";
                        }
                    }
                    
                }
                else if (graphElement is ResizeControler) // 激活的图元是缩放控制点
                {
                    tooltipText = "<underline>鼠标拖动调整图元形状";
                }
                else if (graphElement is AddButton) // 激活的图元是添加按钮
                {
                    tooltipText = "<underline>鼠标点击增加图元插槽";
                }
                else if (graphElement is ConnectButton) // 激活的图元是连接按钮
                {
                    tooltipText = "<underline>鼠标拖动创建连接线";
                }
                else if (graphElement is SlotGraphElement) // 激活的图元是插槽
                {
                    if (string.IsNullOrEmpty(graphElement.TooltipText))
                    {
                        tooltipText = "<underline>鼠标拖入或拖出连接线控制点调整图元连接";
                    }
                    else
                    {
                        tooltipText = string.Format("<bold>插槽内容\r\n{0}\r\n<split>\r\n<underline>鼠标拖入或拖出连接线控制点调整图元连接",
                                                    graphElement.TooltipText);
                    }
                }
                else if (graphElement is RemarkGraphElement) // 激活的图元是注释
                {
                    tooltipText = "<underline>鼠标双击或按Enter键编辑图元注释";
                }
                else if (graphElement is SlotContainer) // 激活的图元是插槽容器
                {
                    if (graphElement.Enable) // 激活的图元可用
                    {
                        if (tooltipText != "")
                        {
                            tooltipText = string.Format("{0}\r\n<split>\r\n<underline>鼠标双击或按Enter键编辑图元", tooltipText);
                        }
                        else
                        {
                            tooltipText = "<underline>鼠标双击或按Enter键编辑图元";
                        }
                    }
                    else // 激活的图元不可用
                    {
                        if (tooltipText != "")
                        {
                            tooltipText = string.Format("{0}\r\n<split>\r\n<underline>当前图元不可编辑，请确认连接是否正确", tooltipText);
                        }
                        else
                        {
                            tooltipText = "<underline>当前图元不可编辑，请确认连接是否正确";
                        }
                    }
                }
                else if (graphElement is Rodman)
                {
                    tooltipText = "<underline>鼠标拖动批量移动游标右边或下边的图元";
                }
            }

            InitTooltipText(graphElement, tooltipText, location);
        }

        /// <summary>
        /// 初始化文本提示
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <param name="text">文本内容</param>
        /// <param name="mouseLocation">鼠标位置</param>
        private void InitTooltipText(GraphElement graphElement, string text, Point mouseLocation)
        {
            canvas.TooltipGraphElement.Adjust(text, graphElement, canvas.VisibleRectangle, mouseLocation);
            canvas.TooltipGraphElement.Visible = true;
        }        

        /// <summary>
        /// 滚动并调整绘图板大小
        /// </summary>
        /// <param name="moveType">移动的类型</param>
        /// <param name="direction">移动的位置</param>
        private void AutoScrollAndResizeCanvas(MoveType moveType, Direction direction)
        {        
            int leftX = -1;
            int rightX = -1;
            int topY = -1;
            int bottomY = -1;

            switch (moveType)
            {
                case MoveType.SingleElement:
                    {
                        leftX = selectedGraphElement.InvalidRectangle.Left;
                        rightX = selectedGraphElement.InvalidRectangle.Right;
                        topY = selectedGraphElement.InvalidRectangle.Top;
                        bottomY = selectedGraphElement.InvalidRectangle.Bottom;

                        break;
                    }
                case MoveType.MultiElement:
                    {
                        Rectangle multiSelectRegion = canvas.CurrentMultiSelectMark.RegionRectangle;
                        leftX = multiSelectRegion.Left;
                        rightX = multiSelectRegion.Right;
                        topY = multiSelectRegion.Top;
                        bottomY = multiSelectRegion.Bottom;

                        break;
                    }
            }

            AutoScrollAndResizeCanvas(leftX, rightX, topY, bottomY, direction);
        }

        /// <summary>
        /// 滚动并调整绘图板大小
        /// </summary>
        /// <param name="leftX">左边框值</param>
        /// <param name="rightX">右边框值</param>
        /// <param name="topY">上边框值</param>
        /// <param name="bottomY">下边框值</param>
        /// <param name="moveDirection">移动方向</param>
        private void AutoScrollAndResizeCanvas(int leftX, int rightX, int topY, int bottomY, Direction moveDirection)
        {
            Size moveSize = new Size(0, 0);
            Size canvasPlusSize = new Size(0, 0);
            bool requireScroll = false;
            bool requireResize = false;

            switch (moveDirection)
            {
                case Direction.Right:
                    {
                        if ((rightX + scrollOffset) * graphSetting.CanvasScale > canvas.ClientRectangle.Right - canvas.AutoScrollPosition.X) // 在绘图板的右边缘
                        {
                            moveSize.Width = scrollOffset;
                            requireScroll = true;

                            if ((rightX - canvas.AutoScrollPosition.X + scrollOffset) * graphSetting.CanvasScale > background.CanvasSize.Width) // 需要扩大绘图版的宽度
                            {
                                canvasPlusSize.Width = scrollOffset;
                                requireResize = true;
                            }
                        }

                        break;
                    }
                case Direction.Left:
                    {
                        if ((leftX - scrollOffset) * graphSetting.CanvasScale < -canvas.AutoScrollPosition.X) // 在绘图板的左边缘
                        {
                            if (canvas.AutoScrollPosition.X < 0) // 滚动条还未走到尽头
                            {
                                moveSize.Width = -scrollOffset;
                                requireScroll = true;
                            }
                        }

                        break;
                    }
                case Direction.Up:
                    {
                        if ((topY - scrollOffset) * graphSetting.CanvasScale < -canvas.AutoScrollPosition.Y) // 在绘图板的上边缘
                        {
                            if (canvas.AutoScrollPosition.Y < 0) // 滚动条还未走到尽头
                            {
                                moveSize.Height = -scrollOffset;
                                requireScroll = true;
                            }
                        }

                        break;
                    }
                case Direction.Down:
                    {
                        if ((bottomY + scrollOffset) * graphSetting.CanvasScale > canvas.ClientRectangle.Bottom - canvas.AutoScrollPosition.Y) // 在绘图版的下边缘
                        {
                            moveSize.Height = scrollOffset;
                            requireScroll = true;

                            if ((bottomY - canvas.AutoScrollPosition.Y + scrollOffset) * graphSetting.CanvasScale > background.CanvasSize.Height) // 需要扩大绘图版的高度
                            {
                                canvasPlusSize.Height = scrollOffset;
                                requireResize = true;
                            }
                        }

                        break;
                    }
            }

            if (requireScroll)
            {
                ScrollCanvas(moveSize, true);
            }

            if (requireResize)
            {
                ResetCanvasSize(canvasPlusSize);
            }
        }

        /// <summary>
        /// 滚动并调整绘图板大小
        /// </summary>
        /// <param name="p">当前鼠标所在点</param>
        private void AutoScrollAndResizeCanvas(Point p)
        {            
            Direction moveDirection = Direction.None;

            // 避免绘图板画面抖动，只滚动四个方向
            if (Math.Abs(p.X - lastSelectedPoint.X) > Math.Abs(p.Y - lastSelectedPoint.Y))
            {
                if (p.X > lastSelectedPoint.X)
                {
                    moveDirection = Direction.Right;
                }
                else if (p.X < lastSelectedPoint.X)
                {
                    moveDirection = Direction.Left;
                }
            }
            else if (Math.Abs(p.Y - lastSelectedPoint.Y) > Math.Abs(p.X - lastSelectedPoint.X))
            {
                if (p.Y > lastSelectedPoint.Y)
                {
                    moveDirection = Direction.Down;
                }
                else if (p.Y < lastSelectedPoint.Y)
                {
                    moveDirection = Direction.Up;
                }
            }

            int leftX = -1;
            int rightX = -1;
            int topY = -1;
            int bottomY = -1;            

            switch (userOperation)
            {
                case UserOperation.RegionSelect: // 框选图元
                    {
                        leftX = p.X;
                        rightX = p.X;
                        topY = p.Y;
                        bottomY = p.Y;                        

                        break;
                    }
                case UserOperation.ScrollCanvas: // 滚动绘图板
                    {                        
                        leftX = p.X - lastSelectedPoint.X - canvas.AutoScrollPosition.X;
                        rightX = p.X - lastSelectedPoint.X - canvas.AutoScrollPosition.X + canvas.ClientRectangle.Right;
                        topY = p.Y - lastSelectedPoint.Y - canvas.AutoScrollPosition.Y;
                        bottomY = p.Y - lastSelectedPoint.Y - canvas.AutoScrollPosition.Y + canvas.ClientRectangle.Bottom;

                        break;
                    }
                case UserOperation.SingleSelect: // 移动单个图元
                    {
                        leftX = selectedGraphElement.InvalidRectangle.Left;
                        rightX = selectedGraphElement.InvalidRectangle.Right;
                        topY = selectedGraphElement.InvalidRectangle.Top;
                        bottomY = selectedGraphElement.InvalidRectangle.Bottom;

                        break;
                    }
                case UserOperation.MultiSelect: // 移动多个图元
                    {
                        Rectangle multiSelectRegion = canvas.CurrentMultiSelectMark.RegionRectangle;
                        leftX = multiSelectRegion.Left;
                        rightX = multiSelectRegion.Right;
                        topY = multiSelectRegion.Top;
                        bottomY = multiSelectRegion.Bottom;

                        break;
                    }
                case UserOperation.Create: // 移动缩略图元
                    {
                        GraphElement abbreviateGraphElement = canvas.AbbreviatGraphElement;
                        leftX = abbreviateGraphElement.InvalidRectangle.Left;
                        rightX = abbreviateGraphElement.InvalidRectangle.Right;
                        topY = abbreviateGraphElement.InvalidRectangle.Top;
                        bottomY = abbreviateGraphElement.InvalidRectangle.Bottom;

                        break;
                    }
                case UserOperation.MoveRodman: // 移动游标
                    {
                        break;
                    }
            }

            AutoScrollAndResizeCanvas(leftX, rightX, topY, bottomY, moveDirection);
            RecordLastSelectedPoint(p);
        }

        /// <summary>
        /// 移动选中的图元
        /// </summary>
        /// <param name="moveSize">移动的位移</param>
        public void MoveSelectedGraphElement(Size moveSize)
        {
            if (selectedGraphElementList.Count > 0) // 多选图元
            {
                foreach (GraphElement graphElement in selectedGraphElementList)
                {
                    if (graphElement is ConnectorContainer) // 当前图元是连接线控制点容器
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;

                        foreach (ConnectorGraphElement connector in line.GetConnectorList()) // 移动连接线控制点
                        {
                            if (!connector.Binded) // 连接线控制点不是连接线的头结点和尾结点
                            {
                                connector.Move(moveSize);
                            }
                            else
                            {
                                SlotGraphElement slot = connector.GetBindingSlot();

                                if (slot != null)
                                {
                                    SlotContainer slotContainer = slot.SlotContainer;

                                    if (!selectedGraphElementList.Contains(slotContainer))
                                    {
                                        FlowChartDisconnectCommand cmd = new FlowChartDisconnectCommand(this, "解除连接图元");
                                        InitFirstCommand(cmd);

                                        if (cmd.Execute(slot)) // 命令执行成功
                                        {
                                            AdjustCommandList(cmd);
                                        }

                                        slot.Move(moveSize);
                                    }
                                }
                            }
                        }
                    }
                    else // 非连接线控制点容器的图元直接移动即可
                    {
                        graphElement.Move(moveSize);
                    }

                    // 刷新绘图区域数据
                    regionManager.ChangeRegion(graphElement);
                }

                canvas.CurrentMultiSelectMark.Move(moveSize);

                Direction direction = Direction.None;
                if (moveSize.Width > 0)
                {
                    direction = Direction.Right;
                }
                else if (moveSize.Width < 0)
                {
                    direction = Direction.Left;
                }
                else if (moveSize.Height > 0)
                {
                    direction = Direction.Down;
                }
                else if (moveSize.Height < 0)
                {
                    direction = Direction.Up;
                }

                AutoScrollAndResizeCanvas(MoveType.MultiElement, direction); // 自动滚动并放缩
                RefreshCanvas();
            }
            else if (selectedGraphElement != null) // 单选图元
            {
                selectedGraphElement.Move(moveSize);
                Direction direction = Direction.None;

                if (moveSize.Width > 0)
                {
                    direction = Direction.Right;
                }
                else if (moveSize.Width < 0)
                {
                    direction = Direction.Left;
                }
                else if(moveSize.Height > 0)
                {
                    direction = Direction.Down;
                }
                else if(moveSize.Height < 0)
                {
                    direction = Direction.Up;
                }

                // 显示网格调整线
                if (selectedGraphElement is SlotContainer)
                {
                    CreateAdjustLine(selectedGraphElement as SlotContainer, direction, false);
                }        
        
                AutoScrollAndResizeCanvas(MoveType.SingleElement, direction); // 自动滚动并放缩

                // 刷新绘图区域数据
                regionManager.ChangeRegion(selectedGraphElement);

                RefreshCanvas();
            }
            else // 滚动滚动条
            {
                ScrollCanvas(moveSize, true);
            }
        }

        /// <summary>
        /// 将鼠标所在点坐标转换为绘图坐标
        /// </summary>
        /// <param name="mouseLocation">鼠标所在点坐标</param>
        /// <returns>绘图坐标</returns>
        private Point AdjustMouseLocation(Point mouseLocation)
        {
            // 坐标转换，处理滚动条滚动的情况  
            Point newLocation = mouseLocation - new Size(canvas.AutoScrollPosition);          
            
            // 缩放绘图板调整
            newLocation.X = (int)(newLocation.X / graphSetting.CanvasScale);
            newLocation.Y = (int)(newLocation.Y / graphSetting.CanvasScale);

            return newLocation;
        }

        /// <summary>
        /// 移动图元
        /// </summary>
        /// <param name="p">鼠标当前所在点</param>
        public void MoveGraphElement(Point point)
        {
            Point p = AdjustMouseLocation(point);            

            if (lastActivatedGraphElement != null) // 将上一个被激活的图元钝化
            {
                lastActivatedGraphElement.Activated = false;

                if (lastActivatedGraphElement is SlotContainer) // 取消标记插槽容器周围的连接线
                {
                    MarkRelavateLines(lastActivatedGraphElement as SlotContainer, false);
                }
            }            

            switch (userOperation)
            {
                case UserOperation.None:
                    {                                                                    
                        ActivateGraphElement(p); // 激活图元                  

                        break;
                    }
                case UserOperation.ScrollCanvas: // 滚动绘图板
                    {                                                        
                        AutoScrollAndResizeCanvas(p);

                        break;
                    }
                case UserOperation.RegionSelect:
                    {
                        Rectangle multiSelectRectangle = GetMultiSelectRectangle(p, multiSelectStartPoint);
                        canvas.CurrentMultiSelectMark.Adjust(multiSelectRectangle.Location, multiSelectRectangle.Size);
                        canvas.CurrentMultiSelectMark.ShowBackground = true;
                        canvas.CurrentMultiSelectMark.Visible = true;
                        AutoScrollAndResizeCanvas(p);

                        break;
                    }
                case UserOperation.Create:
                    {
                        GraphElement abbreviateGraphElement = canvas.AbbreviatGraphElement;

                        if (abbreviateGraphElement != null) // 显示缩略的图元
                        {                            
                            canvas.TooltipGraphElement.Visible = false;
                            Size moveSize = new Size(p.X - lastSelectedPoint.X, p.Y - lastSelectedPoint.Y);
                            abbreviateGraphElement.Move(moveSize);

                            if (!abbreviateGraphElement.IsInRegion(p)) // 检查是否需要调整缩略图元
                            {
                                AdjustAbbreviatGraphElement(p);
                            }

                            if (autoConnect) // 需要修改缩略图元连接线的位置
                            {
                                int headX = (int)(canvas.AbbreviatGraphElement.Location.X + canvas.AbbreviatGraphElement.ElementSize.Width / 2);
                                int headY = (int)(canvas.AbbreviatGraphElement.Location.Y);
                                canvas.AbbreviateLine.HeadConnector.Location = new Point(headX, headY);
                            }

                            AutoScrollAndResizeCanvas(p); // 检查滚动并调整绘图板大小                            
                        }

                        break;
                    }
                case UserOperation.SingleSelect:
                    {
                        if (p != lastSelectedPoint)
                        {
                            List<GraphElement> graphElementList = new List<GraphElement>();
                            graphElementList.Add(selectedGraphElement);

                            DragGraphElement(graphElementList, p, MoveType.SingleElement);
                            AutoScrollAndResizeCanvas(p); // 检查滚动并调整绘图板大小
                            canvas.TooltipGraphElement.Visible = false;
                        }

                        break;
                    }
                case UserOperation.MultiSelect:
                    {
                        if (p != lastSelectedPoint)
                        {
                            DragGraphElement(selectedGraphElementList, p, MoveType.MultiElement);
                            AutoScrollAndResizeCanvas(p); // 检查滚动并调整绘图板大小
                            canvas.TooltipGraphElement.Visible = false;
                        }

                        break;
                    }
                case UserOperation.MoveRodman:
                    {
                        if (p != lastSelectedPoint)
                        {
                            canvas.CurrentRodman.Init(p, canvas.VisibleRectangle);
                            MoveMultiGraphElement(moveGraphElementList, p);
                            AutoScrollAndResizeCanvas(p); // 检查滚动并调整绘图板大小
                            canvas.TooltipGraphElement.Visible = false;
                        }
                        

                        break;
                    }
            }            
            
            RefreshCanvas();
        }

        /// <summary>
        /// 移动多个图元
        /// </summary>
        /// <param name="graphElementList">图元链表</param>
        /// <param name="p">鼠标当前所在点</param>
        private void MoveMultiGraphElement(List<GraphElement> graphElementList, Point p)
        {
            Size moveSize = new Size(0, 0);

            switch (canvas.CurrentRodman.CurrentRodmanType)
            {
                case RodmanType.None:
                    {
                        break;
                    }
                case RodmanType.Horizonal:
                    {
                        moveSize.Width = p.X - lastSelectedPoint.X;
                        break;
                    }
                case RodmanType.Vertical:
                    {
                        moveSize.Height = p.Y - lastSelectedPoint.Y;
                        break;
                    }
            }
            
            foreach (GraphElement graphElement in graphElementList)
            {
                if (graphElement is ConnectorContainer) // 当前图元是连接线控制点容器
                {
                    ConnectorContainer line = graphElement as ConnectorContainer;
                    line.Visible = false;

                    foreach (ConnectorGraphElement connector in line.GetConnectorList()) // 移动连接线控制点
                    {
                        if (!connector.Binded) // 连接线控制点不是连接线的头结点和尾结点
                        {
                            connector.Move(moveSize);
                        }
                    }
                }
                else if (graphElement is SlotContainer) // 当前图元是插槽容器
                {
                    SlotContainer slotContainer = graphElement as SlotContainer;

                    slotContainer.RefreshRelevateLine(false);
                    graphElement.Move(moveSize);
                }
                else // 非连接线控制点容器的图元直接移动即可
                {
                    graphElement.Move(moveSize);
                }

                graphElement.Moving = true;
            }

            multiMoveRegion.Location += moveSize;       
        }

        /// <summary>
        /// 延迟移动图元
        /// </summary>
        /// <param name="direction">延迟移动方向</param>
        /// <param name="lagTime">延迟时间</param>     
        /// <returns>是否移动成功</returns>
        private bool LagMove(Direction direction, int lagPosition)
        {
            if (direction == lastLagDirection && lagPosition == lastLagPosition)
            {
                moveLagged = false;
            }
            else
            {                
                this.lastLagDirection = direction;
                this.lastLagPosition = lagPosition;
                lastMoveTime = DateTime.Now.Ticks;
                moveLagged = true;    
            }

            return moveLagged;
        }

        /// <summary>
        /// 检查移动延迟
        /// </summary>
        /// <param name="direction">移动的方向</param>
        /// <returns>是否允许移动</returns>
        private bool CheckMoveLag(Direction direction)
        {
            bool enableMove = true;
            lockMoveDrag = false;

            if (DateTime.Now.Ticks - lastMoveTime < moveLagTime * 10000)
            {
                if (moveLagged && direction == lastLagDirection)
                {
                    enableMove = false;
                    lockMoveDrag = true;
                }                
            }
            else // 超时重置
            {
                lastLagDirection = Direction.None;
                lastLagPosition = -1;
            }

            return enableMove;
        }

        /// <summary>
        /// 记录上一次选中的点
        /// </summary>
        /// <param name="currentLocation">当前点坐标</param>
        private void RecordLastSelectedPoint(Point currentLocation)
        {
            if (!lockMoveDrag)
            {
                lastSelectedPoint = currentLocation;
            }
        }        

        /// <summary>
        /// 拖动图元
        /// </summary>
        /// <param name="p">鼠标当前所在点</param>
        /// <param name="moveType">移动类型</param>
        private void DragGraphElement(List<GraphElement> graphElementList, Point p, MoveType moveType)
        {            
            Size moveSize = new Size(p.X - lastSelectedPoint.X, p.Y - lastSelectedPoint.Y);

            // 计算移动方向
            Direction direction = Direction.None;

            if (moveSize.Width > 0)
            {
                direction = Direction.Right;
            }
            else if (moveSize.Width < 0)
            {
                direction = Direction.Left;
            }
            else if (moveSize.Height > 0)
            {
                direction = Direction.Down;
            }
            else if (moveSize.Height < 0)
            {
                direction = Direction.Up;
            }

            if (!CheckMoveLag(direction))
            {
                // 重置鼠标位置
                Cursor.Position = canvas.PointToScreen(lastSelectedPoint) + new Size(canvas.AutoScrollPosition);
                return;
            }

            switch (moveType)
            {
                case MoveType.SingleElement:
                    {
                        GraphElement graphElement = graphElementList[0];

                        graphElement.Move(moveSize); // 移动选中的图元

                        if (graphElement is ConnectorGraphElement) // 当前拖动的图元是连接线控制点
                        {
                            ConnectorGraphElement connector = graphElement as ConnectorGraphElement;
                            bool activated = false; // 是否有插槽被激活

                            if (connector.IsHeadPoint || connector.IsTailPoint) // 当前拖动的连接线控制点是头结点或者尾结点
                            {
                                List<SlotContainer> regionSlotContainerList = regionManager.GetSlotContainerList(connector.Location);

                                foreach (SlotContainer slotContainer in regionSlotContainerList)
                                {
                                    if (activated) // 已经有插槽或插槽容器激活，则直接跳出
                                    {
                                        break;
                                    }

                                    SlotGraphElement slot = slotContainer.GetSlotInRegion(p);

                                    if (slot != null) // 激活当前插槽
                                    {
                                        slot.Activated = true;
                                        activated = true;
                                        lastActivatedGraphElement = slot;
                                        break;
                                    }

                                    if (slotContainer.IsInRegion(p)) // 激活当前插槽容器
                                    {
                                        slotContainer.Activated = true;
                                        activated = true;
                                        lastActivatedGraphElement = slotContainer;
                                        break;
                                    }
                                }
                            }

                            connector.Line.Moving = true;

                            // 自动调整连接线                                
                            connector.Line.AdjustLine(connector);
                        }
                        else if (graphElement is SlotContainer) // 当前拖动的图元是插槽容器
                        {
                            SlotContainer slotContainer = graphElement as SlotContainer;
                            
                            slotContainer.RefreshRelevateLine(false);
                            CreateAdjustLine(slotContainer, direction, true);
                        }
                        else if (graphElement is ResizeControler) // 当前拖动的是缩放控制点
                        {
                            SlotContainer slotContainer = (graphElement as ResizeControler).Owner;
                            CreateAdjustLine(slotContainer, direction, false);
                        }

                        graphElement.Moving = true;

                        break;
                    }
                case MoveType.MultiElement:
                    {                        
                        foreach (GraphElement graphElement in graphElementList)
                        {
                            if (graphElement is ConnectorContainer) // 当前图元是连接线控制点容器
                            {
                                ConnectorContainer line = graphElement as ConnectorContainer;
                                line.Visible = false;

                                foreach (ConnectorGraphElement connector in line.GetConnectorList()) // 移动连接线控制点
                                {
                                    if (!connector.Binded) // 连接线控制点不是连接线的头结点和尾结点
                                    {
                                        connector.Move(moveSize);
                                    }
                                    else
                                    {
                                        SlotGraphElement slot = connector.GetBindingSlot();

                                        if (slot != null)
                                        {
                                            SlotContainer slotContainer = slot.SlotContainer;

                                            if (!graphElementList.Contains(slotContainer))
                                            {
                                                FlowChartDisconnectCommand cmd = new FlowChartDisconnectCommand(this, "解除连接图元");
                                                InitFirstCommand(cmd);

                                                if (cmd.Execute(slot)) // 命令执行成功
                                                {
                                                    AdjustCommandList(cmd);
                                                }

                                                slot.Move(moveSize);
                                            }
                                        }                                        
                                    }
                                }
                            }
                            else if (graphElement is SlotContainer) // 当前图元是插槽容器
                            {
                                SlotContainer slotContainer = graphElement as SlotContainer;
                                
                                slotContainer.RefreshRelevateLine(false);
                                graphElement.Move(moveSize);
                            }
                            else // 非连接线控制点容器的图元直接移动即可
                            {
                                graphElement.Move(moveSize);
                            }

                            graphElement.Moving = true;
                        }

                        canvas.CurrentMultiSelectMark.Move(moveSize);

                        break;
                    }
            }
        }       

        /// <summary>
        /// 创建调整网格线
        /// </summary>
        /// <param name="slotContaienr">当前插槽容器</param>
        private void CreateAdjustLine(SlotContainer slotContainer, Direction direction, bool autoAdjust)
        {
            canvas.CurrentGuideLine.ClearGuideLineList();

            Rectangle rectangle = Rectangle.Empty;
            rectangle.Location = canvas.ClientRectangle.Location - (Size)canvas.AutoScrollPosition;
            rectangle.Size = canvas.ClientSize;
            List<SlotContainer> list = regionManager.GetSlotContainerList(rectangle);            
            int leftX = -1;
            int rightX = -1;
            int topY = -1;
            int bottomY = -1;

            canvas.PaintCanvas = false;

            foreach(SlotContainer s in list)
            {
                if (s != slotContainer)
                {
                    // 插槽调整线
                    if (s.TextRectangle.Left < slotContainer.TextRectangle.Right &&
                        s.TextRectangle.Right > slotContainer.TextRectangle.Left)
                    {
                        if (s.TextRectangle.Bottom < slotContainer.TextRectangle.Top)
                        {
                            foreach (SlotGraphElement slot1 in s.GetOutSlotList())
                            {
                                foreach (SlotGraphElement slot2 in slotContainer.GetInSlotList())
                                {
                                    if (slot1.Location.X == slot2.Location.X)
                                    {
                                        canvas.CurrentGuideLine.AddGuideLine(new Point[] { slot1.Location, slot2.Location });
                                    }
                                }
                            }
                        }

                        if (s.TextRectangle.Top > slotContainer.TextRectangle.Bottom)
                        {
                            foreach (SlotGraphElement slot1 in s.GetInSlotList())
                            {
                                foreach (SlotGraphElement slot2 in slotContainer.GetOutSlotList())
                                {
                                    if (slot1.Location.X == slot2.Location.X)
                                    {
                                        canvas.CurrentGuideLine.AddGuideLine(new Point[] { slot1.Location, slot2.Location });
                                    }
                                }
                            }
                        }
                    }                   

                    // 显示边框调整线
                    switch (direction)
                    {
                        case Direction.Left:
                            {                                
                                if (slotContainer.TextRectangle.Left - s.TextRectangle.Left >= 0 &&
                                    slotContainer.TextRectangle.Left - s.TextRectangle.Left < moveAdjustWidth)
                                {
                                    canvas.CurrentGuideLine.AddGuideLine(new Point[]{ new Point(s.TextRectangle.Left, canvas.VisibleRectangle.Top), 
                                                                                      new Point(s.TextRectangle.Left, canvas.VisibleRectangle.Bottom)});   
                                    
                                    if (s.TextRectangle.Left < leftX || leftX == -1)
                                    {
                                        leftX = s.TextRectangle.Left;
                                    }
                                }

                                break;
                            }
                        case Direction.Right:
                            {
                                if (s.TextRectangle.Right - slotContainer.TextRectangle.Right >= 0 &&
                                    s.TextRectangle.Right - slotContainer.TextRectangle.Right < moveAdjustWidth)
                                {
                                    canvas.CurrentGuideLine.AddGuideLine(new Point[]{ new Point(s.TextRectangle.Right, canvas.VisibleRectangle.Top), 
                                                                                      new Point(s.TextRectangle.Right, canvas.VisibleRectangle.Bottom)});

                                    if (s.TextRectangle.Right > rightX || rightX == -1)
                                    {
                                        rightX = s.TextRectangle.Right;
                                    }
                                }

                                break;
                            }
                        case Direction.Up:
                            {
                                if (slotContainer.TextRectangle.Top - s.TextRectangle.Top >= 0 &&
                                    slotContainer.TextRectangle.Top - s.TextRectangle.Top < moveAdjustWidth)
                                {
                                    canvas.CurrentGuideLine.AddGuideLine(new Point[]{ new Point(canvas.VisibleRectangle.Left, s.TextRectangle.Top), 
                                                                                      new Point(canvas.VisibleRectangle.Right, s.TextRectangle.Top)});

                                    if (s.TextRectangle.Top < topY || topY == -1)
                                    {
                                        topY = s.TextRectangle.Top;
                                    }
                                }

                                break;
                            }
                        case Direction.Down:
                            {
                                if (s.TextRectangle.Bottom - slotContainer.TextRectangle.Bottom >= 0 &&
                                    s.TextRectangle.Bottom - slotContainer.TextRectangle.Bottom < moveAdjustWidth)
                                {
                                    canvas.CurrentGuideLine.AddGuideLine(new Point[]{ new Point(canvas.VisibleRectangle.Left, s.TextRectangle.Bottom), 
                                                                                      new Point(canvas.VisibleRectangle.Right, s.TextRectangle.Bottom)});

                                    if (s.TextRectangle.Bottom > bottomY || bottomY == -1)
                                    {
                                        bottomY = s.TextRectangle.Bottom;
                                    }
                                }

                                break;
                            }
                    }                                      
                }                
            }               
            
            if (autoAdjust) // 自动调整位置
            {
                bool requireAdjust = false;
                Size moveSize = Size.Empty;
                int lagPosition = -1;

                switch (direction)
                {
                    case Direction.Left:
                        {
                            if (leftX != -1)
                            {
                                moveSize.Width = leftX - slotContainer.TextRectangle.Left;
                                lagPosition = leftX;

                                if (moveSize.Width < moveLagOffset)
                                {
                                    requireAdjust = true;
                                }                                
                            }

                            break;
                        }
                    case Direction.Right:
                        {
                            if (rightX != -1)
                            {
                                moveSize.Width = rightX - slotContainer.TextRectangle.Right;
                                lagPosition = rightX;

                                if (moveSize.Width < moveLagOffset)
                                {
                                    requireAdjust = true;
                                }   
                            }

                            break;
                        }
                    case Direction.Up:
                        {
                            if (topY != -1)
                            {
                                moveSize.Height = topY - slotContainer.TextRectangle.Top;
                                lagPosition = topY;

                                if (moveSize.Height < moveLagOffset)
                                {
                                    requireAdjust = true;
                                }
                            }

                            break;
                        }
                    case Direction.Down:
                        {
                            if (bottomY != -1)
                            {
                                moveSize.Height = bottomY - slotContainer.TextRectangle.Bottom;
                                lagPosition = bottomY;

                                if (moveSize.Height < moveLagOffset)
                                {
                                    requireAdjust = true;
                                }   
                            }

                            break;
                        }
                    default:
                        {
                            break;
                        }
                }

                if (requireAdjust)
                {
                    slotContainer.Move(moveSize);

                    if (LagMove(direction, lagPosition)) // 移动延迟 
                    {
                        Cursor.Position += moveSize;     
                    }
                }
            }            

            canvas.PaintCanvas = true;
        }      

        /// <summary>
        /// 改变鼠标指针的形状
        /// </summary>
        /// <param name="direction">当前的方向</param>
        private void ChangeMouseCursur(Direction direction)
        {
            switch (direction) // 改变鼠标指针
            {
                case Direction.Up:
                    {
                        canvas.Cursor = Cursors.SizeNS;
                        break;
                    }
                case Direction.Up_Left:
                    {
                        canvas.Cursor = Cursors.SizeNWSE;
                        break;
                    }
                case Direction.Left:
                    {
                        canvas.Cursor = Cursors.SizeWE;
                        break;
                    }
                case Direction.Down_Left:
                    {
                        canvas.Cursor = Cursors.SizeNESW;
                        break;
                    }
                case Direction.Down:
                    {
                        canvas.Cursor = Cursors.SizeNS;
                        break;
                    }
                case Direction.Down_Right:
                    {
                        canvas.Cursor = Cursors.SizeNWSE;
                        break;
                    }
                case Direction.Right:
                    {
                        canvas.Cursor = Cursors.SizeWE;
                        break;
                    }
                case Direction.Up_Right:
                    {
                        canvas.Cursor = Cursors.SizeNESW;
                        break;
                    }
            }
        }

        /// <summary>
        /// 选中当前激活连接线的入口插槽容器
        /// </summary>
        public void SelectLineInSlotContainer()
        {
            ConnectorContainer line = lastSelectedGraphElement as ConnectorContainer;

            if (line != null) // 先检查被选中的图元
            {
                if (line.InSlotContainer != null)
                {
                    SelectGraphElement(line.InSlotContainer, true);
                }
            }
            else 
            {
                line = lastActivatedGraphElement as ConnectorContainer;

                if (line != null) // 再检查被激活的图元
                {
                    if (line.InSlotContainer != null)
                    {
                        SelectGraphElement(line.InSlotContainer, true);
                    }
                }
            }
        }

        /// <summary>
        /// 选中当前激活连接线的出口插槽容器
        /// </summary>
        public void SelectLineOutSlotContainer()
        {
            ConnectorContainer line = lastSelectedGraphElement as ConnectorContainer;

            if (line != null) // 先检查被选中的图元
            {
                if (line.OutSlotContainer != null)
                {
                    SelectGraphElement(line.OutSlotContainer, true);
                }
            }
            else 
            {
                line = lastActivatedGraphElement as ConnectorContainer;

                if (line != null) // 再检查被激活的图元
                {
                    if (line.OutSlotContainer != null)
                    {
                        SelectGraphElement(line.OutSlotContainer, true);
                    }
                }
            }
        }

        /// <summary>
        /// 激活图元
        /// </summary>
        /// <param name="p">鼠标当前所在点</param>
        private void ActivateGraphElement(Point p)
        {
            bool activated = false; // 是否有图元被激活
            canvas.TooltipGraphElement.Visible = false; // 消除文本提示                  
            
            // 先做一次短路检查
            if (lastActivatedGraphElement != null)
            {
                if (lastActivatedGraphElement is ConnectorContainer)
                {
                    ConnectorContainer line = lastActivatedGraphElement as ConnectorContainer;
                    ConnectorGraphElement connector = line.GetConnectorInRegion(p);

                    if (connector != null)
                    {
                        lastActivatedGraphElement = connector;
                        activated = true;
                    }
                    else
                    {
                        if (line.IsInRegion(p))
                        {
                            activated = true;
                        }                        
                    }
                }
                else if (lastActivatedGraphElement is SlotContainer)
                {
                    SlotContainer node = lastActivatedGraphElement as SlotContainer;
                    SlotGraphElement slot = node.GetSlotInRegion(p);

                    if (slot != null)
                    {
                        lastActivatedGraphElement = slot;
                        activated = true;
                    }
                    else
                    {
                        if (node.IsInRegion(p))
                        {
                            activated = true;
                        }
                    }
                }
                else if (lastActivatedGraphElement is Rodman)
                {
                    ; // 游标需要特殊处理
                }
                else
                {
                    if (lastActivatedGraphElement.IsInRegion(p))
                    {
                        activated = true;
                    }
                }                
            }

            if (!activated)
            {
                // 检查是否激活游标
                canvas.CurrentRodman.Init(p, canvas.VisibleRectangle);

                if (canvas.CurrentRodman.IsInRegion(p))
                {
                    lastActivatedGraphElement = canvas.CurrentRodman;
                    activated = true;
                }
            }            

            // 检查是否有连接线控制点被激活
            foreach (ConnectorContainer connectorContainer in regionManager.GetConnectorContainerList(p))
            {
                if (activated) // 如果已经有图元被激活，则直接跳出循环
                {
                    break;
                }

                ConnectorGraphElement connector = connectorContainer.GetConnectorInRegion(p);

                if (connector != null)
                {
                    lastActivatedGraphElement = connector;
                    activated = true;
                    break;
                }
                
                if (connectorContainer.IsInRegion(p)) // 检查连接线是否可以激活
                {
                    lastActivatedGraphElement = connectorContainer;
                    activated = true;
                    break;
                }                        
            }

            bool resizeControlerActivated = false; // 是否有缩放控制点被激活

            foreach (SlotContainer slotContainer in regionManager.GetSlotContainerList(p)) // 检查是否有插槽容器被激活
            {
                if (activated) // 如果已经有图元被激活，则直接跳出循环
                {
                    break;
                }

                ResizeControler resizeControler = slotContainer.GetResizeControlerInRegion(p);
                
                if (resizeControler != null)
                {
                    resizeControlerActivated = true;
                    activated = true;
                    lastActivatedGraphElement = resizeControler;
                    ChangeMouseCursur(resizeControler.CurrentDirection); // 改变鼠标指针形状
                    break;
                }                

                // 遍历检查容器的按钮是否被激活
                BaseButton baseButton = slotContainer.GetButtonInRegion(p);

                if (baseButton != null)
                {
                    activated = true;
                    lastActivatedGraphElement = baseButton;
                    break;
                }                

                // 检查是否有注释图元被选中
                if (slotContainer.RemarkNode != null)
                {
                    if (slotContainer.RemarkNode.IsInRegion(p))
                    {
                        lastActivatedGraphElement = slotContainer.RemarkNode;
                        activated = true;
                        break;
                    }
                }

                if (slotContainer.IsInRegion(p)) // 检查当前点是否在图元区域内
                {
                    SlotGraphElement slot = slotContainer.GetSlotInRegion(p);

                    if (slot != null)
                    {
                        activated = true;
                        lastActivatedGraphElement = slot;
                        break;
                    }
                    
                    activated = true;
                    lastActivatedGraphElement = slotContainer;
                    break;                    
                }
            }

            if (activated) // 有图元被激活
            {
                lastActivatedGraphElement.Activated = true; // 激活图元                

                if (lastActivatedGraphElement is SlotContainer) // 取消标记插槽容器周围的连接线
                {
                    MarkRelavateLines(lastActivatedGraphElement as SlotContainer, true);
                }

                // 初始化文本提示
                InitTooltipText(lastActivatedGraphElement, p);
            }

            if (!resizeControlerActivated) // 恢复绘图板的默认指针样式
            {
                canvas.Cursor = Cursors.Default;
            }
        }

        /// <summary>
        /// 生成多选图元的标识矩形
        /// </summary>
        /// <param name="p1">矩形的对角点</param>
        /// <param name="p2">矩形的对角点</param>
        /// <returns>标识矩形</returns>
        private Rectangle GetMultiSelectRectangle(Point p1, Point p2)
        {
            int x1 = p1.X;
            int y1 = p1.Y;
            int x2 = p2.X;
            int y2 = p2.Y;
            int sizeWidth;
            int sizeHeight;
            int pointX;
            int pointY;

            if(x1 > x2)
            {
                pointX = x2;
                sizeWidth = x1 - x2;
            }
            else
            {
                pointX = x1;
                sizeWidth = x2 - x1;
            }
            
            if(y1 > y2)
            {
                pointY = y2;
                sizeHeight = y1 - y2;
            }
            else
            {
                pointY = y1;
                sizeHeight = y2 - y1;
            }

            Point multiSelectPoint = new Point(pointX, pointY);
            Size multiSelectSize = new Size(sizeWidth, sizeHeight);
            Rectangle multiSelectRectangle = new Rectangle(multiSelectPoint, multiSelectSize);

            return multiSelectRectangle;
        }

        /// <summary>
        /// 选中并记录图元
        /// </summary>
        /// <param name="graphElement">图元</param>
        /// <param name="p">鼠标当前所在点</param>
        private void SelectGraphElement(GraphElement graphElement, Point p)
        {
            graphElement.Selected = true;
            selectedGraphElement = graphElement;
            lastSelectedGraphElement = graphElement;
            moveOffset.Width = p.X - graphElement.Location.X;
            moveOffset.Height = p.Y - graphElement.Location.Y;            
            documentManager.ShowObjectProperty(graphElement);

            // 创建Move命令，保存当前状态
            moveCommand = new FlowChartMoveCommand(this, "移动图元");

            if (commandIndex == -1) // 检查是否是第一条命令
            {
                moveCommand.FirstCommand = true;
                moveCommand.SaveCurrentState(p);
            }            
        }

        /// <summary>
        /// 选中图元
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        /// <param name="showMultiSelectRegion">是否显示框选区域</param>
        public void SelectGraphElement(GraphElement graphElement, bool showMultiSelectRegion)
        {
            if (lastSelectedGraphElement != null) // 将上一个被选中的图元置为非选中状态
            {
                if (!selectedGraphElementList.Contains(lastSelectedGraphElement)) // 在多选状态需要检查上一个被选中的图元是否在多选容器中
                {
                    lastSelectedGraphElement.Selected = false;
                }
            }

            if (lastResizingGraphElement != null) // 将上一个缩放的插槽容器置为非缩放状态
            {
                lastResizingGraphElement.Resizing = false;
            }

            // 清空选中的图元链表
            ClearSelectedGraphElementList();

            // 很奇怪，Control键有很大几率会被认为是按下的，在这里重置一下
            controlMode = false;

            graphElement.Selected = true;
            selectedGraphElement = graphElement;
            lastSelectedGraphElement = graphElement;            
            documentManager.ShowObjectProperty(graphElement);

            // 创建框选区域显示突出显示
            if (showMultiSelectRegion)
            {
                canvas.CurrentMultiSelectMark.Adjust(graphElement.Location + new Size(-5, -5), graphElement.ElementSize + new Size(10, 10));
                canvas.CurrentMultiSelectMark.ShowBackground = false;
                canvas.CurrentMultiSelectMark.Visible = true;

                // 屏幕自动滚动居中
                AutoScrollToPoint(graphElement.Location - new Size((canvas.ClientSize.Width - graphElement.ElementSize.Width) / 2,
                                  (canvas.ClientSize.Height - graphElement.ElementSize.Height) / 2));
            }                        

            // 刷新绘图            
            RefreshCanvas(graphElement.InvalidRectangle);
        }

        /// <summary>
        /// 创建多选区域
        /// </summary>
        private void CreateMultiSelectRegion()
        {
            int left = -1;
            int right = -1;
            int top = -1;
            int buttom = -1;

            foreach (GraphElement graphElement in selectedGraphElementList)
            {
                if(graphElement.Location.X < left || left == -1)
                {
                    left = graphElement.Location.X;
                }

                if(graphElement.Location.X + graphElement.ElementSize.Width > right || right == -1)
                {
                    right = graphElement.Location.X + graphElement.ElementSize.Width;
                }

                if(graphElement.Location.Y < top || top == -1)
                {
                    top = graphElement.Location.Y;
                }

                if(graphElement.Location.Y + graphElement.ElementSize.Height > buttom || buttom == -1)
                {
                    buttom = graphElement.Location.Y + graphElement.ElementSize.Height;
                }
            }

            canvas.CurrentMultiSelectMark.Adjust(new Point(left - 5, top - 5), new Size(right - left + 10, buttom - top + 10));
            canvas.CurrentMultiSelectMark.ShowBackground = false;
            canvas.CurrentMultiSelectMark.Visible = true;
        }       

        /// <summary>
        /// 创建图元移动区域
        /// </summary>
        private void CreateMultiMoveRegion()
        {
            int left = -1;
            int right = -1;
            int top = -1;
            int buttom = -1;

            foreach (GraphElement graphElement in moveGraphElementList)
            {
                if (graphElement.Location.X < left || left == -1)
                {
                    left = graphElement.Location.X;
                }

                if (graphElement.Location.X + graphElement.ElementSize.Width > right || right == -1)
                {
                    right = graphElement.Location.X + graphElement.ElementSize.Width;
                }

                if (graphElement.Location.Y < top || top == -1)
                {
                    top = graphElement.Location.Y;
                }

                if (graphElement.Location.Y + graphElement.ElementSize.Height > buttom || buttom == -1)
                {
                    buttom = graphElement.Location.Y + graphElement.ElementSize.Height;
                }
            }

            multiMoveRegion = new Rectangle(new Point(left - 5, top - 5), new Size(right - left + 10, buttom - top + 10));
        }

        /// <summary>
        /// 选中所有的图元
        /// </summary>
        public void SelectAllGraphElement()
        {
            selectedGraphElementList.Clear();

            foreach(GraphElement graphElement in slotContainerList)
            {
                graphElement.Selected = true;
                selectedGraphElementList.Add(graphElement);
            }

            foreach(GraphElement graphElement in connectorContainerList)
            {
                graphElement.Selected = true;
                selectedGraphElementList.Add(graphElement);
            }

            CreateMultiSelectRegion();
            RefreshCanvas();
        }                          
        
        /// <summary>
        /// 选中链表中的所有图元
        /// </summary>
        /// <param name="list">保存图元的链表</param>
        public void SelectGraphElements(List<GraphElement> list)
        {
            ClearSelectedGraphElementList();

            foreach (GraphElement node in list)
            {
                node.Selected = true;
                selectedGraphElementList.Add(node);
            }

            CreateMultiSelectRegion();

            Rectangle multiSelectRegion = canvas.CurrentMultiSelectMark.RegionRectangle;
            AutoScrollToPoint(multiSelectRegion.Location - new Size((canvas.ClientSize.Width - multiSelectRegion.Width) / 2,
                              (canvas.ClientSize.Height - multiSelectRegion.Height) / 2));
            RefreshCanvas();
        }

        /// <summary>
        /// 自动将滚动条滚动到某个点上
        /// </summary>
        /// <param name="p">当前点</param>
        private void AutoScrollToPoint(Point p)
        {
            int horizontalValue = p.X + canvas.AutoScrollPosition.X;
            int verticalValue = p.Y + canvas.AutoScrollPosition.Y;
            canvas.Visible = false; // 强制刷新滚动条

            if (canvas.HorizontalScroll.Value + horizontalValue >= canvas.HorizontalScroll.Minimum &&
                            canvas.HorizontalScroll.Value + horizontalValue <= canvas.HorizontalScroll.Maximum)
            {
                canvas.HorizontalScroll.Value += horizontalValue;
            }
            else
            {
                if (horizontalValue > 0) // 向右滚动
                {
                    canvas.HorizontalScroll.Value = canvas.HorizontalScroll.Maximum;
                }
                else // 向左滚动
                {
                    canvas.HorizontalScroll.Value = canvas.HorizontalScroll.Minimum;
                }
            }

            if (canvas.VerticalScroll.Value + verticalValue >= canvas.VerticalScroll.Minimum &&
                            canvas.VerticalScroll.Value + verticalValue <= canvas.VerticalScroll.Maximum)
            {
                canvas.VerticalScroll.Value += verticalValue;
            }
            else
            {
                if (verticalValue > 0) // 向下滚动
                {
                    canvas.VerticalScroll.Value = canvas.VerticalScroll.Maximum;
                }
                else // 向上滚动
                {
                    canvas.VerticalScroll.Value = canvas.VerticalScroll.Minimum;
                }
            }

            canvas.Visible = true;
            canvas.Focus();
        }

        /// <summary>
        /// 开始滚动绘图板
        /// </summary>
        /// <param name="point">当前鼠标位置</param>
        public void BeginScrollCavas(Point point)
        {
            RecordLastSelectedPoint(AdjustMouseLocation(point));
            canvas.Cursor = Cursors.SizeAll;
            userOperation = UserOperation.ScrollCanvas;
        }

        /// <summary>
        /// 选中区域内的所有图元
        /// </summary>
        /// <param name="rectangle">区域</param>
        private void SelectGraphElementInRegion(Rectangle rectangle)
        {
            foreach (GraphElement graphElement in canvas.GraphElementList)
            {
                if (graphElement.IsInRegion(rectangle)) // 该图元在多选区域内
                {
                    graphElement.Selected = true;
                    selectedGraphElementList.Add(graphElement);
                }
            }

            if (selectedGraphElementList.Count > 0) // 创建多选图元框选的区域
            {
                CreateMultiSelectRegion();
            }
            else
            {
                canvas.CurrentMultiSelectMark.Visible = false; // 隐藏多选区域
            }
        }

        /// <summary>
        /// 确认移动操作
        /// </summary>
        /// <param name="p">鼠标当前所在点</param>
        private void ConfirmLocation(Point point)
        {
            Point p = AdjustMouseLocation(point);
            Rectangle multiSelectRegion = canvas.CurrentMultiSelectMark.RegionRectangle;

            switch (userOperation) // 检查用户当前操作状态
            {                        
                case UserOperation.RegionSelect:
                    {
                        SelectGraphElementInRegion(multiSelectRegion);

                        break;
                    }
                case UserOperation.SingleSelect:
                    {
                        bool bind = false; // 是否已经绑定图元
                        bool unbind = false; // 是否已经解除绑定图元
                        selectedGraphElement.Moving = false;

                        // 自动调整绘图板大小
                        AdjustOutOfBorder();

                        if (selectedGraphElement is ConnectorGraphElement) // 拖动的是连接线控制点
                        {
                            ConnectorGraphElement connector = selectedGraphElement as ConnectorGraphElement;
                            SlotContainer slotContainer;
                            SlotGraphElement slot = null;

                            // 先解除绑定连接线控制点
                            if (connector.Binded)
                            {
                                slot = connector.GetBindingSlot();                                

                                if (slot != null)
                                {
                                    FlowChartDisconnectCommand cmd = new FlowChartDisconnectCommand(this, "解除连接图元");
                                    InitFirstCommand(cmd);

                                    if (cmd.Execute(slot)) // 命令执行成功
                                    {
                                        AdjustCommandList(cmd);
                                        unbind = true;
                                    }
                                }                                
                            }

                            if (lastActivatedGraphElement is SlotGraphElement) // 检查是否激活了插槽
                            {
                                slot = lastActivatedGraphElement as SlotGraphElement;

                                if (slot.IsInRegion(p)) // 绑定连接线控制点
                                {
                                    FlowChartConnectCommand cmd = new FlowChartConnectCommand(this, "连接图元");
                                    InitFirstCommand(cmd);

                                    if (cmd.Execute(new object[] { slot, connector })) // 命令执行成功
                                    {
                                        AdjustCommandList(cmd);
                                        bind = true;
                                    }
                                }
                            }
                            else if (lastActivatedGraphElement is SlotContainer) // 检查是否激活了插槽容器
                            {
                                slotContainer = lastActivatedGraphElement as SlotContainer;

                                if (slotContainer.IsInRegion(p)) // 绑定连接线控制点
                                {
                                    if (connector.IsHeadPoint) // 头连接点
                                    {
                                        slot = slotContainer.GetInSlot();
                                    }
                                    else if (connector.IsTailPoint) // 尾连接点
                                    {
                                        slot = slotContainer.GetOutSlot();
                                    }

                                    if (slot != null)
                                    {
                                        FlowChartConnectCommand cmd = new FlowChartConnectCommand(this, "连接图元");
                                        InitFirstCommand(cmd);

                                        if (cmd.Execute(new object[] { slot, connector })) // 命令执行成功
                                        {
                                            AdjustCommandList(cmd);
                                            bind = true;
                                        }
                                        else
                                        {
                                            slotContainer.RemoveSlot(slot);                                            
                                        }
                                    }
                                }
                            }

                            connector.Line.Moving = false;
                            connector.Line.AdjustRectangle();
                            regionManager.ChangeRegion((selectedGraphElement as ConnectorGraphElement).Line);
                        }
                        else if (selectedGraphElement is SlotContainer) // 拖动的是插槽容器
                        {
                            SlotContainer slotContainer = selectedGraphElement as SlotContainer;

                            foreach (ConnectorContainer line in slotContainer.GetConnectedLine())
                            {
                                AdjustLine(line, slotContainer);
                            }

                            slotContainer.RefreshRelevateLine(true);
                            AdjustLine(slotContainer);
                        }
                        else if (selectedGraphElement is ResizeControler) // 拖动的是缩放控制点
                        {
                            regionManager.ChangeRegion((selectedGraphElement as ResizeControler).Owner);
                        }

                        regionManager.ChangeRegion(selectedGraphElement);

                        if (!bind && !unbind) // 没有绑定和接触绑定图元，则是在移动图元
                        {
                            if (moveCommand.Execute(new object[] { selectedGraphElement, p })) // 命令执行成功
                            {
                                AdjustCommandList(moveCommand);
                            }
                        }
                        else // 已经绑定或者解除绑定图元
                        {
                            ConnectorContainer line = (selectedGraphElement as ConnectorGraphElement).Line;
                            
                            // 自动调整连接线
                            ConnectorGraphElement connector = selectedGraphElement as ConnectorGraphElement;
                            SlotContainer currentSlotContainer = null;
                            if (connector.IsHeadPoint)
                            {
                                currentSlotContainer = connector.Line.OutSlotContainer;
                            }
                            else
                            {
                                currentSlotContainer = connector.Line.InSlotContainer;
                            }
                            
                            AdjustLine(line, currentSlotContainer);
                        } 

                        break;
                    }
                case UserOperation.MultiSelect:
                    {                        
                        // 自动调整绘图板大小
                        AdjustOutOfBorder();

                        foreach (GraphElement graphElement in selectedGraphElementList)
                        {
                            if (graphElement is ConnectorContainer) // 连接线
                            {
                                graphElement.Visible = true;                               
                            }
                            else if (graphElement is SlotContainer) // 插槽容器
                            {
                                SlotContainer slotContainer = graphElement as SlotContainer;                                
                                slotContainer.RefreshRelevateLine(true);
                            }

                            regionManager.ChangeRegion(graphElement);
                            graphElement.Moving = false;
                        }
                       
                        // 自动调整连接线
                        List<SlotContainer> affectedSlotContainerList = regionManager.GetSlotContainerList(multiSelectRegion);
                        List<ConnectorContainer> affectedLineList = regionManager.GetConnectorContainerList(multiSelectRegion);

                        foreach (ConnectorContainer line in affectedLineList)
                        {
                            foreach (SlotContainer slotContainer in affectedSlotContainerList)
                            {
                                if (line.LineIntersect(slotContainer))
                                {                                    
                                    AdjustLine(line, null);
                                    break;
                                }
                            }
                        }                        

                        if (moveCommand.Execute(new object[] { selectedGraphElement, p })) // 命令执行成功
                        {
                            AdjustCommandList(moveCommand);
                        }

                        break;
                    }
                case UserOperation.MoveRodman:
                    {
                        // 自动调整绘图板大小
                        AdjustOutOfBorder();

                        foreach (GraphElement graphElement in moveGraphElementList)
                        {
                            if (graphElement is ConnectorContainer) // 连接线
                            {
                                ConnectorContainer line = graphElement as ConnectorContainer;
                                line.Visible = true;
                                line.AdjustRectangle();
                            }
                            else if (graphElement is SlotContainer) // 插槽容器
                            {
                                SlotContainer slotContainer = graphElement as SlotContainer;                                
                                slotContainer.RefreshRelevateLine(true);
                            }

                            regionManager.ChangeRegion(graphElement);
                            graphElement.Moving = false;
                        }

                        // 自动调整连接线
                        List<SlotContainer> affectedSlotContainerList = regionManager.GetSlotContainerList(multiMoveRegion);
                        List<ConnectorContainer> affectedLineList = regionManager.GetConnectorContainerList(multiMoveRegion);

                        foreach (ConnectorContainer line in affectedLineList)
                        {
                            foreach (SlotContainer slotContainer in affectedSlotContainerList)
                            {
                                if (line.LineIntersect(slotContainer))
                                {
                                    AdjustLine(line, null);
                                    break;
                                }
                            }
                        } 

                        if (moveCommand.Execute(new object[] { selectedGraphElement, p })) // 命令执行成功
                        {
                            AdjustCommandList(moveCommand);
                        }

                        break;
                    }
            }

            lockMoveDrag = false; // 拖动图元解锁
            userOperation = UserOperation.None;         
            canvas.Cursor = Cursors.Default; // 恢复绘图板的鼠标形状
            canvas.CurrentGuideLine.ClearGuideLineList();

            RefreshCanvas();
        }

        /// <summary>
        /// 刷新绘图板
        /// </summary>
        public void RefreshCanvas()
        {
            canvas.Invalidate(canvas.ClientRectangle);            
        }

        /// <summary>
        /// 刷新特定区域内的绘图板
        /// </summary>
        /// <param name="rectangle">刷新特定区域内的绘图板</param>
        private void RefreshCanvas(Rectangle rectangle)
        {
            canvas.Invalidate(rectangle);
        }

        /// <summary>
        /// 初始化移动图元的链表
        /// </summary>
        private void InitMoveGraphElementList()
        {
            moveGraphElementList.Clear();

            switch (canvas.CurrentRodman.CurrentRodmanType)
            {
                case RodmanType.None:
                    {
                        break;
                    }
                case RodmanType.Horizonal:
                    {
                        foreach (SlotContainer slotContainer in slotContainerList)
                        {
                            if (slotContainer.Location.X > canvas.CurrentRodman.Location.X)
                            {
                                moveGraphElementList.Add(slotContainer);
                            }
                        }

                        foreach (ConnectorContainer line in connectorContainerList)
                        {
                            if (line.Location.X > canvas.CurrentRodman.Location.X)
                            {
                                moveGraphElementList.Add(line);
                            }
                        }

                        break;
                    }
                case RodmanType.Vertical:
                    {
                        foreach (SlotContainer slotContainer in slotContainerList)
                        {
                            if (slotContainer.Location.Y > canvas.CurrentRodman.Location.Y)
                            {
                                moveGraphElementList.Add(slotContainer);
                            }
                        }

                        foreach (ConnectorContainer line in connectorContainerList)
                        {
                            if (line.Location.Y > canvas.CurrentRodman.Location.Y)
                            {
                                moveGraphElementList.Add(line);
                            }
                        }

                        break;
                    }
            }            
        }

        /// <summary>
        /// 清空选中的图元链表
        /// </summary>
        private void ClearSelectedGraphElementList()
        {
            foreach (GraphElement graphElement in selectedGraphElementList)
            {
                graphElement.Selected = false;
            }

            selectedGraphElementList.Clear();
            canvas.CurrentMultiSelectMark.Visible = false;
        }

        /// <summary>
        /// 绘图板鼠标按下
        /// </summary>
        /// <param name="mouseLocation">当前鼠标位置</param>
        public void CanvasMouseDown(Point mouseLocation)
        {
            switch (userOperation)
            {
                case UserOperation.Create:
                    {
                        CreateGraphElement();
                        DeleteAbbreviateGraphElement();
                        ReconstructCanvasGraphElementList();
                        break;
                    }
                default:
                    {
                        CheckGraphElementSelected(mouseLocation);
                        break;
                    }
            }            
        }

        /// <summary>
        /// 绘图板鼠标弹起
        /// </summary>
        /// <param name="mouseLocation">当前鼠标位置</param>
        public void CanvasMouseUp(Point mouseLocation)
        {
            ConfirmLocation(mouseLocation);
        }

        /// <summary>
        /// 检查是否图元被选中
        /// </summary>
        /// <param name="p">鼠标当前所在点</param>
        public void CheckGraphElementSelected(Point point)
        {
            Point p = AdjustMouseLocation(point);
            RecordLastSelectedPoint(p);          
  
            selectedGraphElement = null;
            ConnectorContainer newLine = null; // 自动新建的连接线
            bool selected = false; // 是否已经有图元被选中
            GraphElement graphElement = null; // 当前图元

            if (lastSelectedGraphElement != null) // 将上一个被选中的图元置为非选中状态
            {
                if (!selectedGraphElementList.Contains(lastSelectedGraphElement)) // 在多选状态需要检查上一个被选中的图元是否在多选容器中
                {
                    lastSelectedGraphElement.Selected = false;
                }
            }

            if (lastResizingGraphElement != null) // 将上一个缩放的插槽容器置为非缩放状态
            {
                lastResizingGraphElement.Resizing = false;
            }

            // 检查游标是否被选中
            if (canvas.CurrentRodman.IsInRegion(p))
            {
                InitMoveGraphElementList();
                graphElement = canvas.CurrentRodman;
                selected = true;
            }

            // 索引检查是否有连接线控制点容器被选中
            foreach (ConnectorContainer connectorContainer in regionManager.GetConnectorContainerList(p))
            {
                if (selected) // 已经有图元被选中时自动跳出循环
                {
                    break;
                }

                // 先遍历检查是否有连接线控制点被选中
                ConnectorGraphElement connector = connectorContainer.GetConnectorInRegion(p);

                if (connector != null)
                {
                    graphElement = connector;
                    selected = true;
                    break;
                }

                if (connectorContainer.IsInRegion(p)) // 连接线被选中
                {
                    graphElement = connectorContainer;
                    selected = true;
                    break;
                }
            }

            foreach (SlotContainer slotContainer in regionManager.GetSlotContainerList(p)) // 索引检查是否有插槽容器被选中
            {
                if (selected) // 如果有图元被选中的话则自动跳出循环
                {
                    break;
                }

                // 检查是否有缩放控制点被选中
                ResizeControler resizeControler = slotContainer.GetResizeControlerInRegion(p);
                
                if (resizeControler != null)
                {
                    graphElement = resizeControler;
                    selected = true;
                    slotContainer.Resizing = true;
                    lastResizingGraphElement = slotContainer;
                    ChangeMouseCursur(resizeControler.CurrentDirection); // 改变鼠标指针形状
                    break;
                }
                
                // 检查是否有按钮被选中
                BaseButton baseButton = slotContainer.GetButtonInRegion(p);

                if (baseButton != null)
                {
                    if (baseButton is ConnectButton) // 是连接按钮
                    {
                        SlotGraphElement slot = (baseButton as ConnectButton).BindingSlot;

                        if (!slot.Binded && slot.IsOutSlot) // 插槽没有绑定连接线控制点，则新建连接线并绑定到当前插槽上
                        {
                            FlowChartCreateLineCommand cmd = new FlowChartCreateLineCommand(this, "创建图元");
                            InitFirstCommand(cmd);

                            if (cmd.Execute(new object[] { slot, p }))
                            {
                                AdjustCommandList(cmd);
                                newLine = selectedGraphElement as ConnectorContainer;
                                ConnectorGraphElement connector = newLine.GetConnectorList()[1];
                                graphElement = connector;
                                selected = true;
                            }

                            break;
                        }
                    }
                    else if (baseButton is AddButton) // 是添加按钮
                    {
                        SlotGraphElement slot = slotContainer.AddOutSlot();
                        graphElement = slotContainer;
                        selected = true;
                        break;
                    }
                }

                // 检查是否有注释图元被选中
                if (slotContainer.RemarkNode != null)
                {
                    if (slotContainer.RemarkNode.IsInRegion(p))
                    {
                        graphElement = slotContainer.RemarkNode;
                        selected = true;
                        break;
                    }
                }
                
                if (slotContainer.IsInRegion(p))
                {
                    SlotGraphElement slot = slotContainer.GetSlotInRegion(p);

                    if (slot != null)
                    {
                        graphElement = slot;
                        selected = true;
                        break;
                    }

                    slotContainer.Resizing = true;
                    lastResizingGraphElement = slotContainer;
                    graphElement = slotContainer;
                    selected = true;
                    break;
                }
            }

            if (selected) // 已经有图元被选中
            {
                SelectGraphElement(graphElement, p);
                userOperation = UserOperation.SingleSelect;

                if (selectedGraphElementList.Contains(selectedGraphElement)) // 进行多图元操作
                {
                    if (controlMode) // 在control模式下需要反选图元
                    {
                        selectedGraphElementList.Remove(selectedGraphElement);
                        selectedGraphElement.Selected = false;
                        selectedGraphElement = null;
                        lastSelectedGraphElement = null;
                        userOperation = UserOperation.None;

                        if (selectedGraphElementList.Count == 0)
                        {
                            canvas.CurrentMultiSelectMark.Visible = false;
                        }
                        else
                        {
                            CreateMultiSelectRegion();
                        }
                    }
                    else
                    {
                        userOperation = UserOperation.MultiSelect;
                    }
                }
                else
                {
                    if (controlMode) // 在control模式下需要加入图元
                    {
                        selectedGraphElementList.Add(selectedGraphElement);
                        CreateMultiSelectRegion();

                        userOperation = UserOperation.MultiSelect;
                    }
                    else
                    {
                        if (graphElement is Rodman)
                        {
                            userOperation = UserOperation.MoveRodman;
                            CreateMultiMoveRegion();
                        }

                        // 清空多选图元链表
                        ClearSelectedGraphElementList();
                    }
                }
            }
            else // 没有图元被选中
            {
                userOperation = UserOperation.RegionSelect;

                // 显示背景属性                
                documentManager.ShowObjectProperty(background);
                multiSelectStartPoint = p;

                // 清空多选图元链表
                ClearSelectedGraphElementList();
            }            

            RefreshCanvas();
        }

        /// <summary>
        /// 删除选中的图元
        /// </summary>
        public void DeleteSelectedGraphElement()
        {
            if (selectedGraphElementList.Count > 0) // 删除所有选中的图元
            {
                // 调整图元区域
                foreach (GraphElement graphElement in selectedGraphElementList)
                {                    
                    regionManager.DeleteFromRegion(graphElement);
                }

                FlowChartMultiDeleteCommand cmd = new FlowChartMultiDeleteCommand(this, "删除图元");
                InitFirstCommand(cmd);

                if (cmd.Execute(selectedGraphElementList)) // 命令执行成功
                {
                    AdjustCommandList(cmd);
                    canvas.CurrentMultiSelectMark.Visible = false;
                    selectedGraphElement = null;
                    lastSelectedGraphElement = null;
                    lastActivatedGraphElement = null;
                    ReconstructCanvasGraphElementList();
                }                
            }

            if (selectedGraphElement != null) // 删除单个选中的图元
            {
                List<ConnectorContainer> list = new List<ConnectorContainer>();
                DataManager dataManager = flowChartManager.CurrentDataManager;
                
                if (selectedGraphElement is SlotContainer) // 要删除插槽容器
                {
                    SlotContainer slotContainer = selectedGraphElement as SlotContainer;

                    foreach (GraphElement graphElement in dataManager.GetPreviousGraphElements(slotContainer))
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;

                        if (dataManager.GetData(graphElement) == null && !list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }

                    foreach (GraphElement graphElement in dataManager.GetNextGraphElements(slotContainer, false))
                    {
                        ConnectorContainer line = graphElement as ConnectorContainer;

                        if (dataManager.GetData(graphElement) == null && !list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }
                }       
         
                regionManager.DeleteFromRegion(selectedGraphElement);

                FlowChartDeleteCommand cmd = new FlowChartDeleteCommand(this, "删除图元");
                InitFirstCommand(cmd);

                if (cmd.Execute(selectedGraphElement)) // 命令执行成功
                {
                    AdjustCommandList(cmd);
                    
                    if (list.Count > 0) // 删除无数据的连接线
                    {
                        foreach (ConnectorContainer line in list)
                        {
                            SlotContainer inSlotContainer = line.InSlotContainer;
                            List<GraphElement> graphElementList = new List<GraphElement>();

                            // 执行逻辑操作
                            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
                            logicManager.DoLogicOperation(flowChartManager, LogicType.BeforeDelete, line);

                            if (line.InSlotContainer != null)
                            {
                                line.UnbindInSlotContainer();
                            }

                            if (line.OutSlotContainer != null)
                            {
                                graphElementList.Add(line.OutSlotContainer);
                                line.UnbindOutSlotContainer();
                            }

                            // 执行逻辑操作
                            logicManager.DoLogicOperation(flowChartManager, LogicType.AfterDelete, graphElementList);

                            connectorContainerList.Remove(line); // 从连接线控制点索引容器中删除
                        }
                    }
                    
                    selectedGraphElement = null;
                    lastSelectedGraphElement = null;
                    lastActivatedGraphElement = null;
                    ReconstructCanvasGraphElementList();                    
                }                                               
            }

            canvas.PaintCanvas = true; // 避免绘制时使用已经释放的资源
            RefreshCanvas();
        }

        /// <summary>
        /// 滚动绘图面板
        /// </summary>
        /// <param name="moveSize">滚动数值</param>
        /// <param name="autoRefresh">是否自动刷新</param>
        public void ScrollCanvas(Size moveSize, bool autoRefresh)
        {
            if (autoRefresh)
            {
                canvas.Visible = false; // 避免绘图板闪烁的问题 
            }

            if (moveSize.Width != 0)
            {
                moveSize.Width = (int)((double)moveSize.Width * canvas.ClientSize.Width * graphSetting.CanvasScale / background.CanvasSize.Width); // 滚动条的单位与绘图板单位需要转换

                if (canvas.HorizontalScroll.Value + moveSize.Width >= canvas.HorizontalScroll.Minimum &&
                            canvas.HorizontalScroll.Value + moveSize.Width <= canvas.HorizontalScroll.Maximum)
                {
                    canvas.HorizontalScroll.Value += moveSize.Width;
                }
                else
                {
                    if (moveSize.Width > 0) // 向右滚动
                    {
                        canvas.HorizontalScroll.Value = canvas.HorizontalScroll.Maximum;
                    }
                    else // 向左滚动
                    {
                        canvas.HorizontalScroll.Value = canvas.HorizontalScroll.Minimum;
                    }
                }
            }

            if (moveSize.Height != 0)
            {
                moveSize.Height = (int)((double)moveSize.Height * canvas.ClientSize.Height * graphSetting.CanvasScale / background.CanvasSize.Height); // 滚动条的单位与绘图板单位需要转换

                if (canvas.VerticalScroll.Value + moveSize.Height >= canvas.VerticalScroll.Minimum &&
                            canvas.VerticalScroll.Value + moveSize.Height <= canvas.VerticalScroll.Maximum)
                {
                    canvas.VerticalScroll.Value += moveSize.Height;
                }
                else
                {
                    if (moveSize.Height > 0) // 向下滚动
                    {
                        canvas.VerticalScroll.Value = canvas.VerticalScroll.Maximum;
                    }
                    else // 向上滚动
                    {
                        canvas.VerticalScroll.Value = canvas.VerticalScroll.Minimum;                        
                    }
                }
            }

            if (autoRefresh)
            {
                canvas.Visible = true; // 避免绘图板闪烁的问题            
                canvas.Refresh();
                canvas.Focus();
            }
        }

        /// <summary>
        /// 清理命令链表
        /// </summary>
        public void ClearCommandList()
        {
            // 清理命令链表
            commandList.Clear();
            commandIndex = -1;            
        }

        /// <summary>
        /// 调整命令链表
        /// </summary>
        /// <param name="cmd">当前命令</param>
        private void AdjustCommandList(FlowChartBaseCommand cmd)
        {
            if (flowChartManager.SupportCommand)
            {
                List<FlowChartBaseCommand> deleteCommandList = new List<FlowChartBaseCommand>(); // 要删除的命令的链表

                for (int i = commandIndex + 1; i < commandList.Count; i++)
                {
                    deleteCommandList.Add(commandList[i]);
                }

                // 删除被覆盖的命令
                foreach (FlowChartBaseCommand command in deleteCommandList)
                {
                    commandList.Remove(command);
                }

                documentManager.AdjustCommandList(commandIndex, string.Format("撤销 ({0} {1})", DateTime.Now, cmd.Description), 
                                                  commandList.Count);

                // 添加新命令
                commandList.Add(cmd);
                commandIndex++;              
            }            
        }

        /// <summary>
        /// 撤销操作
        /// </summary>
        /// <param name="index">命令编号</param>
        public void Undo(int index)
        {
            commandIndex = index;
            Undo();
        }

        /// <summary>
        /// 撤销操作
        /// </summary>
        public void Undo()
        {
            FlowChartBaseCommand cmd = commandList[commandIndex];

            if (cmd.FirstCommand) // 只有第一条命令保存执行前和执行后的数据
            {
                cmd.Unexecute();
            }
            else // 其余命令只保存执行后的数据
            {
                cmd = commandList[commandIndex - 1];
                cmd.Execute();
            }
            
            commandIndex--;
            documentManager.SetRedoState(true);

            if (commandIndex < 0)
            {
                documentManager.SetUndoState(false);                
                flowChartManager.ContentChanged = false;
            }
            else
            {                
                documentManager.SetUndoState(true);
            }
        }

        /// <summary>
        /// 重做操作
        /// </summary>
        public void Redo()
        {
            FlowChartBaseCommand cmd = commandList[commandIndex + 1];
            cmd.Execute();
            commandIndex++;            
            documentManager.SetUndoState(true);

            if (commandIndex > commandList.Count - 2)
            {                
                documentManager.SetRedoState(false);
            }
            else
            {
                documentManager.SetRedoState(true);                
            }
        }
    }
}
