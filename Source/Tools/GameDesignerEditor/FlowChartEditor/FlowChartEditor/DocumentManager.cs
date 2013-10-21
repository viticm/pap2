using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Data;
using System.Collections;
using System.IO;
using System.ComponentModel;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

using FlowChartEditor.BaseGraphElement;
using FlowChartEditor.Command.TreeCommand;
using DBManager;
using GraphElementLibrary;
using DataElementLibrary;
using FlowChartDataLibrary;
using DataInteractor;

namespace FlowChartEditor
{
    public class DocumentManager
    {
        // 管理器
        private static DocumentManager documentManager; // 唯一的分类管理对象实例
        private DataBaseManager dataBaseManager; // 数据库管理器实例            
        private Helper helper = Helper.GetHelper(); // 系统辅助对象   
        private LogRecorder logRecorder = LogRecorder.GetLogRecorder(); // 日志管理器对象  
        private Interactor interactor = Interactor.GetInteractor(); // 交互管理器实例
        private FlowChartManager flowChartManager; // 绘图管理器 
        
        // 数据
        private TreeNode cutNode; // 当前剪切的树结点
        private TreeNode copyNode; // 当前复制的树结点
        private string cutTreeNodePath; // 剪切的树结点的路径        
        private TreeNode selectedNode; // 当前选中的树结点        
        private Hashtable flowChartManagerTable = new Hashtable(); // 图元管理器哈希表                       
        private List<string> mapList; // 地图链表    
        private Hashtable fileTable = new Hashtable(); // 保存导出文件数据的哈希表
        private List<string> sqlCommandList = new List<string>(); // 保存sql命令的链表
        private Hashtable copyTable; // 复制数据的哈希表
        private Hashtable diagramInfoTable = new Hashtable(); // 绘图信息哈希表  
        private int backupIDOffset = 10000; // 绘图备份ID偏移
        
        // 窗体
        private SearchForm searchForm = new SearchForm(); // 查找窗体
        private InnerChartReferenceForm innerChartReferenceForm = new InnerChartReferenceForm(); // 子绘图引用关系窗体

        // 状态
        private EditMode treeMode; // 当前树结点的处理模式
        private string chartMode; // 当前分类的编辑模式

        // 控件
        private AbbreviateCanvas abbreviateCanvas; // 缩略图绘图板
        private DevComponents.DotNetBar.TabControl tabControl; // 绘图面板容器
        private DevComponents.DotNetBar.TabControl treeTabControl; // 分类树面板容器    
        private PropertyGrid propertyGrid; // 属性控件
        private TextBoxX logBox; // 日志控件
        private ButtonItem redoButton; // 重做按钮
        private ButtonItem undoButton; // 撤销按钮
        private ButtonItem commandSwitchButton; // 命令状态切换按钮
        private ButtonItem treeMenu; // 右键菜单
        private ExpandablePanel treePanel; // 分类树面板
        private ImageList treeImageList; // 分类树的图片链表     
        private ComboBoxItem scaleComboBox; // 缩放面板下拉框
        private List<ButtonItem> diagramRecordButtonList = new List<ButtonItem>(); // 记录打开绘图按钮的链表

        /// <summary>
        /// 私有构造函数，构建单例模式
        /// </summary>
        private DocumentManager()
        {

        }

        /// <summary>
        /// 缩放面板下拉框
        /// </summary>
        public ComboBoxItem ScaleComboBox
        {
            set
            {
                scaleComboBox = value;
            }
        }

        /// <summary>
        /// 复制数据的哈希表
        /// </summary>
        public Hashtable CopyTable
        {
            set
            {
                copyTable = value;
            }

            get
            {
                return copyTable;
            }
        }

        /// <summary>
        /// 当前分类树
        /// </summary>
        public TreeView CurrentTree
        {
            get
            {
                return treeTabControl.SelectedTab.Tag as TreeView;
            }
        }

        /// <summary>
        /// 保存导出文件数据的哈希表
        /// </summary>
        public Hashtable FileTable
        {
            get
            {
                return fileTable;
            }
        }

        /// <summary>
        /// 当前绘图管理器
        /// </summary>
        public FlowChartManager CurrentFlowChartManager
        {
            get
            {
                return flowChartManager;
            }
        }

        /// <summary>
        /// 剪切的树结点的路径
        /// </summary>
        public string CutTreeNodePath
        {
            set
            {
                cutTreeNodePath = value;
            }
        }

        /// <summary>
        /// 分类树面板容器
        /// </summary>
        public DevComponents.DotNetBar.TabControl TreeTabControl
        {
            set
            {
                treeTabControl = value;
            }
        }

        /// <summary>
        /// 分类树的图片链表
        /// </summary>
        public ImageList TreeImageList
        {
            set
            {
                treeImageList = value;
            }
        }
        
        /// <summary>
        /// 当前分类的编辑模式
        /// </summary>
        public string CurrentChartMode 
        {
            get
            {
                return chartMode;
            }
        }

        /// <summary>
        /// 分类树面板
        /// </summary>
        public ExpandablePanel TreePanel
        {
            set
            {
                treePanel = value;
            }
        }

        /// <summary>
        /// 分类树菜单
        /// </summary>
        public ButtonItem TreeMenu
        {
            set
            {
                treeMenu = value;
            }
        }

        /// <summary>
        /// 日志控件
        /// </summary>
        public TextBoxX LogBox
        {
            set
            {
                logBox = value;
            }
        }

        /// <summary>
        /// 属性控件
        /// </summary>
        public PropertyGrid CurrentPropertyGrid
        {
            set
            {
                propertyGrid = value;
            }
        }

        /// <summary>
        /// 缩略图绘图板
        /// </summary>
        public AbbreviateCanvas CurrentAbbreviateCanvas
        {
            set
            {
                abbreviateCanvas = value;
            }
        }

        /// <summary>
        /// 绘图面板容器
        /// </summary>
        public DevComponents.DotNetBar.TabControl CurrentTabControl
        {
            set
            {
                tabControl = value;
            }
        }

        /// <summary>
        /// 当前复制的树结点
        /// </summary>
        public TreeNode CopyTreeNode
        {
            set
            {
                copyNode = value;
            }

            get
            {
                return copyNode;
            }
        }

        /// <summary>
        /// 当前剪切的树结点
        /// </summary>
        public TreeNode CutTreeNode
        {
            set
            {
                cutNode = value;
            }

            get
            {
                return cutNode;
            }
        }

        /// <summary>
        /// 当前分类树的状态
        /// </summary>
        public EditMode CurrentTreeMode
        {
            set
            {
                treeMode = value;
            }

            get
            {
                return treeMode;
            }
        }

        /// <summary>
        /// 重做按钮
        /// </summary>
        public ButtonItem RedoButton
        {
            set
            {
                redoButton = value;
            }
        }

        /// <summary>
        /// 撤销按钮
        /// </summary>
        public ButtonItem UndoButton
        {
            set
            {
                undoButton = value;
            }
        }

        /// <summary>
        /// 命令切换按钮
        /// </summary>
        public ButtonItem CommandSwitchButton
        {
            set
            {
                commandSwitchButton = value;
            }
        }

        /// <summary>
        /// 添加绘图记录按钮
        /// </summary>
        /// <param name="buttonItem">绘图记录按钮</param>
        public void AddDiagramRecordButton(ButtonItem buttonItem)
        {
            diagramRecordButtonList.Add(buttonItem);
            buttonItem.Tag = diagramRecordButtonList.Count.ToString();
        }
    
        /// <summary>
        /// 获取分类管理器的实例
        /// </summary>
        /// <returns>分类管理器的实例</returns>
        public static DocumentManager GetDocumentManager()
        {
            if(documentManager == null)
            {
                documentManager = new DocumentManager();
            }

            return documentManager;
        }

        /// <summary>
        /// 开始查找
        /// </summary>
        public void BeginSearch()
        {
            if (flowChartManager != null)
            {
                searchForm.CurrentFlowChartManager = flowChartManager;
                searchForm.Show();
                searchForm.Focus();
            }
            else
            {
                MessageBox.Show("请先打开要查找的绘图！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 切换命令状态
        /// </summary>
        public void SwitchCommandState()
        {
            if (flowChartManager != null)
            {
                flowChartManager.SupportCommand = !flowChartManager.SupportCommand;

                if (flowChartManager.SupportCommand)
                {
                    commandSwitchButton.Text = "关闭命令支持";
                    RefreshCommandButton();
                }
                else
                {
                    commandSwitchButton.Text = "开启命令支持";
                    flowChartManager.CurrentGraphManager.ClearCommandList();
                }
            }
            else
            {
                MessageBox.Show("请先打开要切换命令支持状态的绘图！", "切换命令支持", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }          

        /// <summary>
        /// 查看子绘图引用关系
        /// </summary>
        public void ViewInnerChartReference()
        {
            innerChartReferenceForm.RefreshData();
            innerChartReferenceForm.Show();
            innerChartReferenceForm.Focus();
        }

        /// <summary>
        /// 变量管理
        /// </summary>
        public void EditVar()
        {
            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.ManageArguments, null);                                
        }

        /// <summary>
        /// 选中所有的图元
        /// </summary>
        public void SelectAllGraphElement()
        {
            if (flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.SelectAllGraphElement();
            }
        }

        /// <summary>
        /// 鼠标在分类树按下事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tree_MouseDown(object sender, MouseEventArgs e)
        {
            switch (e.Button)
            {
                case MouseButtons.Left: // 鼠标左键点击
                    {
                        TreeView tree = sender as TreeView;                                          

                        // 先检查当前是否有树结点被选中
                        foreach (TreeNode node in tree.Nodes)
                        {
                            CheckNodeSelected(node, e.Location);
                        }

                        if (selectedNode != null)
                        {
                            tree.SelectedNode = selectedNode;
                            ShowDiagramProperty(selectedNode);
                        }
                        
                        break;
                    }
                case MouseButtons.Right: // 鼠标右键点击
                    {
                        PopTreeNodeMenu(e.Location);

                        break;
                    }
            }
        }

        /// <summary>
        /// 显示绘图的属性
        /// </summary>        
        /// <param name="node">当前树结点</param>
        private void ShowDiagramProperty(TreeNode node)
        {
            string id = node.Tag.ToString();
            DiagramInfo info = diagramInfoTable[id] as DiagramInfo;
            propertyGrid.SelectedObject = info;
        }

        /// <summary>
        /// 双击分类树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tree_NodeMouseDoubleClick(object sender, MouseEventArgs e)
        {
            AddNewPanel();            
        }

        /// <summary>
        /// 刷新未初始化数据元的图元的数据
        /// </summary>
        /// <param name="dataManager">数据管理器实例</param>
        /// <param name="graphElement">图元对象</param>
        /// <param name="data">数据</param>
        private void ReloadData(DataManager dataManager, GraphElement graphElement, object data)
        {
            DataElement dataElement = dataManager.InitDataElement(graphElement);
            Hashtable table = new Hashtable();
            table["globe_args"] = dataManager.GlobeArgs;

            if (dataElement != null)
            {
                dataElement.Data = data;
                dataElement.ReloadData(table);
            }
        }
        
        /// <summary>
        /// 获取绘图的描述字符串
        /// </summary>
        /// <param name="id">绘图ID</param>
        /// <param name="diagramTable">绘图信息数据表</param>
        /// <returns>绘图的描述字符串</returns>
        private string GetDiagramDescription(string id, DataTable diagramTable)
        {
            string description = null;

            DataRow[] rows = diagramTable.Select(string.Format("ID = '{0}'", id));
            if (rows.Length > 0)
            {
                string diagramPath = rows[0]["FullPath"].ToString();
                string diagramClass = rows[0]["Class"].ToString();

                description = string.Format("{0} {1}", diagramClass, diagramPath);
            }

            return description;
        }

        /// <summary>
        /// 保存绘图数据到本地文件
        /// </summary>
        public void SaveDiagramDataToFile()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (tree != null)
            {
                TreeNode currentNode = tree.SelectedNode;

                if (currentNode != null)
                {
                    SaveFileDialog saveFileDialog = new SaveFileDialog();
                    saveFileDialog.Title = "保存绘图数据";
                    saveFileDialog.Filter = "绘图数据|*.Diagram";
                    saveFileDialog.InitialDirectory = Application.StartupPath;
                    saveFileDialog.FileName = string.Format("{0}.Diagram", currentNode.Text);
                    
                    if (saveFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        string fileName = saveFileDialog.FileName;
                        string id = currentNode.Tag.ToString();
                        object data = dataBaseManager.GetDiagramData(id);
                        bool saveSuccess = false;

                        if (data != null)
                        {
                            saveSuccess = dataBaseManager.SaveDiagramDataToFile(fileName, data);
                        }                        

                        if (saveSuccess)
                        {
                            MessageBox.Show("绘图数据保存成功！", "保存绘图数据",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                    }                                                           
                }
                else
                {
                    MessageBox.Show("请先选择要保存到文件的绘图！", "保存绘图数据 ",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        /// <summary>
        /// 从本地文件加载绘图数据
        /// </summary>
        public void LoadDiagramDataFromFile()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (tree != null)
            {
                TreeNode currentNode = tree.SelectedNode;

                if (currentNode != null)
                {
                    OpenFileDialog openFileDialog = new OpenFileDialog();
                    openFileDialog.Title = "加载绘图数据";
                    openFileDialog.Filter = "绘图数据|*.Diagram";
                    openFileDialog.InitialDirectory = Application.StartupPath;
                    openFileDialog.FileName = string.Format("{0}.Diagram", currentNode.Text);

                    if (openFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        string fileName = openFileDialog.FileName;
                        string id = currentNode.Tag.ToString();
                        object data = dataBaseManager.LoadDiagramDataFromFile(fileName);

                        if (data != null)
                        {                            
                            string panelName = currentNode.Text;
                            string path = GetNodePath(currentNode);

                            OpenDiagram(panelName, id, path, false, data); 
                        }
                    }
                }
                else
                {
                    MessageBox.Show("请先选择要从文件加载文件的绘图！", "加载绘图数据 ",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        /// <summary>
        /// 初始化分类管理器
        /// </summary>
        public void Init()
        {
            // 检查文件更新
            if (helper.CheckRequireUpdate())
            {
                MessageBox.Show("编辑器文件有更新，请运行UpdateProgram.exe获取最新文件！", "检查更新", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            // 初始化数据库管理器
            dataBaseManager = DataBaseManager.GetDataBaseManager();

            // 清空当前用户的所有锁
            dataBaseManager.ClearRecordLocks();

            // 读取地图链表
            mapList = dataBaseManager.GetMapList();

            // 读取用户配置信息
            string ip = helper.GetHostAddress();
            object userInfo = dataBaseManager.GetUserConfigInfo(ip);
            Hashtable info = null;
            bool infoExist = false;

            if (userInfo is Hashtable)
            {
                info = userInfo as Hashtable;                                 
                infoExist = true;                
            }
            else if (userInfo is object[]) // 兼容旧数据
            {
                object[] oldInfo = userInfo as object[];
                info = new Hashtable();
                info["OutPutDir"] = oldInfo[0];
                info["ShowRemark"] = oldInfo[1];
                info["AutoAdjustLine"] = oldInfo[2];
                info["AutoAdjustGrid"] = oldInfo[3];
                infoExist = true;
            }            

            if (infoExist)
            {
                helper.SetArguments(info);

                // 更新最近打开绘图链表
                DataTable diagramTable = dataBaseManager.GetDiagramInformation(null);
                List<int> latestDiagramList = info["LatestDiagramList"] as List<int>;                

                if (latestDiagramList != null)
                {
                    for (int i = 0; i < diagramRecordButtonList.Count; i++)
                    {
                        if (i < latestDiagramList.Count)
                        {
                            string diagramID = helper.LatestDiagramList[i].ToString();
                            string diagramInformation = GetDiagramDescription(diagramID, diagramTable);

                            if (diagramInformation != null)
                            {
                                diagramRecordButtonList[i].Text = diagramInformation;
                                diagramRecordButtonList[i].Tag = diagramID;
                            }
                            else
                            {
                                diagramRecordButtonList[i].Visible = false;
                            }
                        }
                        else
                        {
                            diagramRecordButtonList[i].Visible = false;
                        }
                    }                        
                }
                else
                {
                    foreach (ButtonItem buttonItem in diagramRecordButtonList)
                    {
                        buttonItem.Visible = false;
                    }
                }
            }
            else
            {
                SettingForm sForm = new SettingForm();
                DialogResult result = sForm.ShowDialog();

                if(result != DialogResult.OK)
                {
                    MessageBox.Show("客户端目录没有配置，可能有部分功能无法使用，可以在主窗体菜单中打开配置面板进行配置！",
                                    "参数配置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }

            helper.LogBox = logBox;

            // 初始化逻辑管理器
            helper.InitLogicManager();

            // 初始化默认绘图分类
            chartMode = helper.DiagramNameList[0];

            // 刷新交互管理器数值
            interactor.RootDir = helper.OutputDir;
            
            // 构建分类面板
            ConstructClassPanel();

            // 写日志
            logRecorder.SaveLog("初始化程序", "加载用户配置，绘图分类信息和自定义模板", true);
        }

        /// <summary>
        /// 构建分类面板
        /// </summary>
        public void ConstructClassPanel()
        {
            List<string> list = helper.DiagramNameList;

            foreach (string s in list)
            {                
                TabItem tabItem = new TabItem();
                TabControlPanel tabControlPanel = new TabControlPanel();

                tabControlPanel.SuspendLayout();
                treeTabControl.Controls.Add(tabControlPanel);

                tabControlPanel.Dock = DockStyle.Fill;
                tabControlPanel.Name = "classPanel" + s;
                tabControlPanel.TabItem = tabItem;                
                tabControlPanel.ResumeLayout(false);

                TreeView tree = new TreeView();

                tree.Tag = s;
                tabItem.ImageIndex = 0;

                tree.ImageList = treeImageList;
                ConstructTree(tree, s);
                tree.MouseDown += new MouseEventHandler(tree_MouseDown);
                tree.MouseDoubleClick += new MouseEventHandler(tree_NodeMouseDoubleClick);
                tabControlPanel.Controls.Add(tree);
                tree.Dock = DockStyle.Fill;

                tabItem.AttachedControl = tabControlPanel;
                tabItem.Name = "classTab" + s;
                tabItem.Text = s;
                tabItem.Tag = tree;
                treeTabControl.Tabs.Add(tabItem);                
            }
        }

        /// <summary>
        /// 最大化绘图板
        /// </summary>
        public void MaximizeCanvas()
        {
            if (flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.MaximumCanvasSize();
            }
        }

        /// <summary>
        /// 改变绘图板的大小
        /// </summary>
        /// <param name="scaleArgument">缩放参数</param>
        public void ChangeCanvasScale(float scaleArgument)
        {
            if(flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.SetCanvasScale(scaleArgument);
            }
        }

        /// <summary>
        /// 显示对象的属性
        /// </summary>
        /// <param name="o">当前对象</param>
        public void ShowObjectProperty(object o)
        {
            propertyGrid.SelectedObject = o;
        }

        /// <summary>
        /// 记录日志
        /// </summary>
        public void RecordLog()
        {
            LogManager.LogManager.GetLogManager().WriteLog();
        }

        /// <summary>
        /// 保存用户的配置信息
        /// </summary>
        public void SaveUserConfigInfo()
        {
            bool success = true;
            string ip = helper.GetHostAddress();
            string hostName = helper.GetHostName();
            Hashtable info = helper.GetArguments();
            bool exist = dataBaseManager.CheckUserInfoExist(ip);            

            if(exist) // 用户配置信息已经存在
            {
                success = dataBaseManager.SetUserConfigInfo(ip, info);
            }
            else // 需要添加用户配置信息
            {
                success = dataBaseManager.AddUserConfigInfo(ip, hostName, info);
            }

            // 写日志
            logRecorder.SaveLog("保存用户配置信息", "保存当前用户的配置信息", true);

            // 写日志
            logRecorder.SaveLog("退出程序", "退出程序", true);
        }

        /// <summary>
        /// 检查绘图是否有效
        /// </summary>
        public void CheckFlowChartAvail()
        {
            if(flowChartManager != null)
            {                
                // 执行逻辑功能
                LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
                GraphManager graphManager = flowChartManager.CurrentGraphManager;
                object[] logicData = new object[]{flowChartManager.CurrentDataManager, flowChartManager.CurrentPath,
                    graphManager.SlotContainerList, graphManager.ConnectorContainerList, logBox, true};
                bool avail = logicManager.DoLogicOperation(flowChartManager, LogicType.CheckLogic, logicData);

                if(avail)
                {
                    MessageBox.Show("当前绘图逻辑信息有效！", "绘图有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("当前没有已经打开的绘图！", "绘图有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 切换绘图面板
        /// </summary>
        public void SwitchPanel()
        {
            TabItem currentTab = tabControl.SelectedTab;

            if (currentTab == null) // 当前没有激活的绘图编辑窗口
            {
                flowChartManager = null;
                abbreviateCanvas.PaintCanvas = false;
                abbreviateCanvas.CurrentCanvas = null;
                abbreviateCanvas.Invalidate();
                undoButton.SubItems.Clear();
                undoButton.Enabled = false;
                redoButton.Enabled = false;
                commandSwitchButton.Enabled = false;
            }
            else
            {
                if (flowChartManagerTable[currentTab.Tag] != null)
                {
                    // 切换当前激活的绘图
                    FlowChartManager currentFlowChartManager = flowChartManagerTable[currentTab.Tag] as FlowChartManager;
                    flowChartManager = currentFlowChartManager;
                    flowChartManager.CurrentGraphManager.CurrentCanvas.CurrentAbbreviateCanvas = abbreviateCanvas;
                    abbreviateCanvas.CurrentCanvas = flowChartManager.CurrentGraphManager.CurrentCanvas;
                    RefreshCommandButton(); // 刷新命令按钮
                    flowChartManager.CurrentGraphManager.CurrentCanvas.Focus();

                    interactor.CurrentFlowChartInteractor = flowChartManager.CurrentFlowChartInteractor;
                    scaleComboBox.WatermarkText = string.Format("{0}%", (int)(GraphSetting.GetGraphSetting().CanvasScale * 100));
                }
                else
                {
                    commandSwitchButton.Enabled = true;                
                    commandSwitchButton.Text = "开启命令支持";                
                }                
            }
        }

        /// <summary>
        /// 清空命令按钮
        /// </summary>
        public void ClearCommandButton()
        {
            // 清理命令按钮
            undoButton.SubItems.Clear();
            SetUndoState(false);
            SetRedoState(false);            
        }

        /// <summary>
        /// 刷新命令按钮
        /// </summary>
        private void RefreshCommandButton()
        {
            if (flowChartManager.SupportCommand)
            {
                GraphManager graphManager = flowChartManager.CurrentGraphManager;
                List<string> commandDescriptionList = graphManager.CommandDescriptionList;
                int commandIndex = graphManager.CommandIndex;
                int commandCount = graphManager.CommandCount;

                // 删除撤销按钮
                undoButton.SubItems.Clear();                

                // 增加撤销按钮
                for (int i = commandIndex; i >= 0; i--)
                {                    
                    ButtonItem newButton = new ButtonItem("undoButton" + commandIndex, commandDescriptionList[i]);
                    newButton.Tag = commandIndex.ToString();
                    graphManager.CurrentEventManager.RegistCommandButtonEvent(newButton);                    
                    undoButton.SubItems.Add(newButton);
                }

                // 设置按钮是否有效
                if ((commandIndex == commandCount - 1) && (commandIndex != -1))
                {
                    SetUndoState(true);                    
                }

                if (commandIndex < commandCount - 1)
                {
                    SetRedoState(true);                    
                }

                commandSwitchButton.Enabled = true;
                commandSwitchButton.Text = "关闭命令支持";
            }
            else
            {
                if (undoButton.SubItems.Count > 0)
                {
                    undoButton.SubItems.Clear();
                }
                
                SetUndoState(false);
                SetRedoState(false);
                commandSwitchButton.Enabled = true;
                commandSwitchButton.Text = "打开命令支持";
            }
        }

        /// <summary>
        /// 获取树结点的路径
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <returns>树结点的路径</returns>
        public string GetNodePath(TreeNode node)
        {
            string path = node.Text;
            TreeNode parentNode = node.Parent;

            while (parentNode != null && parentNode.Level > 0)
            {
                path = parentNode.Text + "\\" + path;
                parentNode = parentNode.Parent;
            }

            return path;
        }

        /// <summary>
        /// 打开指定路径的绘图
        /// </summary>
        /// <param name="path">绘图路径</param>
        /// <param name="chartMode">绘图类型</param>
        public void OpenDiagramInPath(string path, string chartMode)
        {            
            TreeNode currentNode = null;            

            foreach (TabItem tabItem in treeTabControl.Tabs)
            {
                if (tabItem.Text == chartMode)
                {
                    TreeView tree = tabItem.Tag as TreeView;
                    List<TreeNode> nodeList = new List<TreeNode>();

                    foreach (TreeNode node in tree.Nodes)
                    {
                        GetAllChildNodes(node, nodeList);
                    }

                    foreach (TreeNode node in nodeList)
                    {
                        if (GetNodePath(node) == path)
                        {
                            currentNode = node;
                            break;
                        }
                    }

                    if (currentNode != null)
                    {
                        break;
                    }
                }                
            }

            if (currentNode != null)
            {
                string panelName = currentNode.Text;
                string nodePath = GetNodePath(currentNode);
                string id = currentNode.Tag.ToString();

                OpenDiagram(panelName, id, nodePath, false, null);
            }
        }

        /// <summary>
        /// 打开绘图
        /// </summary>
        /// <param name="id">绘图ID</param>        
        public void OpenDiagram(string id)
        {
            TreeNode currentNode = null;

            // 需要重设当前绘图的编辑状态
            if (flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.ResetUserOperation();
            }

            foreach (TabItem tabItem in treeTabControl.Tabs)
            {                
                TreeView tree = tabItem.Tag as TreeView;
                List<TreeNode> nodeList = new List<TreeNode>();

                foreach(TreeNode node in tree.Nodes)
                {
                    GetAllChildNodes(node, nodeList);
                }

                foreach(TreeNode node in nodeList)
                {
                    if (node.Tag.ToString() == id)
                    {
                        currentNode = node;
                        treeTabControl.SelectedTab = tabItem;
                        break;
                    }
                }

                if (currentNode != null)
                {
                    break;
                }
            }

            if (currentNode != null)
            {
                string panelName = currentNode.Text;
                string path = GetNodePath(currentNode);

                OpenDiagram(panelName, id, path, false, null);
            }
        }

        /// <summary>
        /// 查看帮助文档
        /// </summary>
        public void ViewHelpDoc()
        {
            string filePath = Path.Combine(Application.StartupPath, @"Plugins\FlowChart\流程图(AI图)脚本工具使用文档.doc");
            FileInfo fi = new FileInfo(filePath);
            
            if (fi.Exists)
            {
                try
                {
                    System.Diagnostics.Process.Start(filePath);
                }
                catch (IOException ex)
                {
                    MessageBox.Show("在查看帮助文档时产生IO异常：" + ex.ToString());
                }
            }
            else
            {
                MessageBox.Show("帮助文档不存在，请尝试自动更新文件！", "查看帮助文档", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
        
        /// <summary>
        /// 打开绘图
        /// </summary>
        /// <param name="panelName">面板名称</param>
        /// <param name="id">绘图ID</param>
        /// <param name="path">绘图路径</param>        
        /// <param name="backupDiagram">是否是绘图备份</param>
        /// <param name="diagramData">绘图数据</param>
        private void OpenDiagram(string panelName, string id, string path, bool backupDiagram, object diagramData)
        {
            // 检查对应id的面板没有被打开
            foreach (TabItem t in tabControl.Tabs)
            {
                if (t.Tag.ToString() == id) // 激活绘图编辑窗口
                {
                    tabControl.SelectedTab = t;
                    return;
                }
            }

            // 初始化绘图控件
            TabControlPanel tabControlPanel = new TabControlPanel();
            TabItem tabItem = CreateTabItemInstance(id, panelName, tabControlPanel, path);

            // 初始化绘图面板并绑定事件和响应方法
            Canvas canvas = CreateCanvasInstance();
            tabControlPanel.Controls.Add(canvas);
            canvas.Dock = DockStyle.Fill;

            // 记录入最近打开的绘图
            if (!backupDiagram)
            {
                helper.RecordLatestDiagram(int.Parse(id));
            }            

            // 初始化绘图管理器
            FlowChartManager newFlowChartManager = CreateFlowChartManagerInstance(canvas, tabItem, id, panelName, path);
            flowChartManagerTable[tabItem.Tag] = newFlowChartManager;
            this.tabControl.SelectedTab = tabItem;
            flowChartManager = newFlowChartManager;

            // 刷新当前激活的绘图
            interactor.CurrentFlowChartInteractor = flowChartManager.CurrentFlowChartInteractor;

            // 绑定事件
            EventManager eventManager = new EventManager(flowChartManager.CurrentGraphManager);
            eventManager.RegistCanvasEvent(canvas);
            flowChartManager.CurrentGraphManager.CurrentEventManager = eventManager;

            // 加载绘图的数据        
            if (!backupDiagram)
            {
                LoadFlowChart(id, diagramData);
            }
            else
            {
                LoadBackupFlowChart(id);
            }            

            // 获取焦点 
            canvas.Focus();

            // 检查绘图数据的有效性
            flowChartManager.CheckDataValid();
        }

        /// <summary>
        /// 在新的面板中打开绘图某个历史记录的内容
        /// </summary>
        /// <param name="backupID">绘图的记录ID</param>
        /// <param name="diagramVersion">绘图的版本号</param>
        public void OpenDiaramAtNewPanel(string backupID, string diagramVersion)
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null && currentNode.ImageIndex != 0) // 检查是否是有效的绘图结点
            {
                string panelName = string.Format("{0} [{1}]", currentNode.Text, diagramVersion);
                int newID = int.Parse(backupID) + backupIDOffset; // 避免出现重复ID的绘图
                string id = newID.ToString();
                string path = GetNodePath(currentNode);

                OpenDiagram(panelName, id, path, true, null);
            }
        }

        /// <summary>
        /// 添加新的绘图面板
        /// </summary>        
        public void AddNewPanel()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null && currentNode.ImageIndex != 0) // 检查是否是有效的绘图结点
            {
                string panelName = currentNode.Text;
                string id = currentNode.Tag.ToString();
                string path = GetNodePath(currentNode);

                OpenDiagram(panelName, id, path, false, null);
            }            
        }

        /// <summary>
        /// 创建分页控件对象
        /// </summary>
        /// <param name="id">绘图ID</param>
        /// <param name="panelName">绘图名称</param>
        /// <param name="tabControlPanel">分页面板控件对象</param>
        /// <param name="path">绘图路径</param>
        /// <returns>分页控件对象</returns>        
        private TabItem CreateTabItemInstance(string id, string panelName, TabControlPanel tabControlPanel, string path)
        {
            TabItem tabItem = new TabItem();

            tabControlPanel.SuspendLayout();
            tabControl.Controls.Add(tabControlPanel);

            tabControlPanel.Dock = DockStyle.Fill;
            tabControlPanel.Name = "tabControlPanel" + id.ToString();
            tabControlPanel.TabItem = tabItem;
            tabControlPanel.ResumeLayout(false);

            tabItem.AttachedControl = tabControlPanel;
            tabItem.Name = "tabItem" + id.ToString();
            tabItem.Text = panelName;
            tabItem.Tag = id;
            tabItem.Tooltip = path;
            tabItem.ImageIndex = 0;

            this.tabControl.Tabs.Add(tabItem);

            return tabItem;
        }

        /// <summary>
        /// 创建绘图管理器对象
        /// </summary>
        /// <param name="canvas">绘图板对象</param>
        /// <param name="tabItem">分页控件对象</param>
        /// <param name="id">绘图ID</param>
        /// <param name="panelName">绘图名称</param>
        /// <param name="path">绘图路径</param>
        /// <returns>绘图管理器对象</returns>
        private FlowChartManager CreateFlowChartManagerInstance(Canvas canvas, TabItem tabItem, string id, string panelName, string path)
        {
            FlowChartManager newFlowChartManager = new FlowChartManager(chartMode, tabItem, id, panelName, path);
            newFlowChartManager.Init(canvas);

            return newFlowChartManager;
        }

        /// <summary>
        /// 创建绘图板对象
        /// </summary>
        /// <return>绘图板对象</return>
        private Canvas CreateCanvasInstance()
        {
            Canvas canvas = new Canvas();            
            canvas.BackColor = GraphSetting.GetGraphSetting().BackgroundColor;
            canvas.CurrentAbbreviateCanvas = abbreviateCanvas;
            abbreviateCanvas.CurrentCanvas = canvas;

            return canvas;
        }

        /// <summary>
        /// 检查是否有当前树结点及其子结点锁对应的绘图被打开
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <returns>是否有绘图被打开</returns>
        public bool CheckFlowChartOpened(TreeNode node)
        {
            bool opened = false;
            List<TreeNode> nodeList = new List<TreeNode>();

            GetAllChildNodes(node, nodeList);

            foreach(TreeNode treeNode in nodeList)
            {
                foreach (TabItem tabItem in tabControl.Tabs)
                {
                    if (tabItem.Tag.ToString() == treeNode.Tag.ToString())
                    {
                        opened = true;
                        break;
                    }
                }

                if (opened)
                {
                    break;
                }
            }

            return opened;
        }

        /// <summary>
        /// 查看回收站
        /// </summary>
        public void ViewRecycle()
        {
            
        }

        /// <summary>
        /// 检查当前树有没有发生变化
        /// </summary>
        /// <returns>当前树有没有发生变化</returns>
        public bool CheckTreeChanged()
        {
            bool changed = false;

            DataTable newTable = dataBaseManager.GetDiagramInformation(chartMode);
            DataTable treeTable = SaveTree();

            for (int i = 0; i < newTable.Rows.Count; i++)
            {
                DataRow newRow = newTable.Rows[i];                
                string id = newRow["ID"].ToString();                
                DataRow[] rows = treeTable.Select(string.Format("ID = '{0}'", id));

                if (rows.Length > 0) // 找到匹配的数据行
                {
                    foreach (DataRow oldRow in rows)
                    {
                        changed = false;

                        if (newRow["FullPath"].ToString() != oldRow["FullPath"].ToString())
                        {
                            changed = true;
                        }

                        if ((bool)newRow["IsFolder"] != (bool)oldRow["IsFolder"])
                        {
                            changed = true;
                        }

                        if (!changed) // 内容没有改变
                        {
                            break;
                        }                    
                    }
                }
                else
                {
                    changed = true;
                }
                
                if (changed) // 内容改变了
                {
                    break;
                }                               
            }                

            return changed;
        }

        /// <summary>
        /// 切换分类
        /// </summary>
        public void SwitchClass()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;            
            chartMode = tree.Tag as string;
            treeMode = EditMode.Normal;
            copyTable = null;
        }

        /// <summary>
        /// 构造分类树
        /// </summary>
        /// <param name="tree">分类树</param>
        /// <param name="chartClass">分类</param>
        public void ConstructTree(TreeView tree, string chartClass)
        {
            DataTable treeTable = dataBaseManager.GetDiagramInformation(chartClass);
            TreeNodeCollection currentNodes;
            TreeNode newNode;

            tree.Nodes.Clear(); // 清空树结点
            treeMode = EditMode.Normal; // 重设树的状态

            TreeNode classNode = tree.Nodes.Add("地图分类");
            classNode.ImageIndex = 0;
            classNode.SelectedImageIndex = 0;
            classNode.Tag = chartClass;
            diagramInfoTable[chartClass] = new DiagramInfo(chartClass, DateTime.Now, "0", 0);

            foreach (DataRow row in treeTable.Rows)
            {
                string fullPath = row["FullPath"] as string;
                string id = row["ID"].ToString();
                bool isFolder = (bool)row["IsFolder"];
                DateTime editTime = (DateTime)row["EditTime"];
                string hostName = row["Host"].ToString();
                string currentVersion = row["Version"].ToString();

                if (currentVersion == "")
                {
                    currentVersion = "0";
                }

                string[] data = fullPath.Split(new char[] { '\\' });
                currentNodes = classNode.Nodes;

                for (int i = 0; i < data.Length; i++)
                {
                    string classification = data[i];

                    if ((i == data.Length - 1) && !isFolder) // 绘图结点
                    {
                        newNode = currentNodes.Add(classification);
                        newNode.Name = classification;
                        newNode.Tag = id;
                        newNode.ImageIndex = 1;
                        newNode.SelectedImageIndex = 1;
                        diagramInfoTable[id] = new DiagramInfo(hostName, editTime, currentVersion, 0);
                    }
                    else // 绘图分类结点
                    {
                        bool find = false;

                        foreach (TreeNode node in currentNodes)
                        {
                            if (node.Text == classification)
                            {
                                find = true;
                                currentNodes = node.Nodes;
                                break;
                            }
                        }

                        if (!find) // 当前分类不存在
                        {
                            StringBuilder path = new StringBuilder();

                            for (int j = 0; j <= i; j++)
                            {
                                path.Append(string.Format("{0}\\", data[j]));                                
                            }

                            path.Remove(path.Length - 1, 1);                            
                            DataRow[] rows = treeTable.Select(string.Format("FullPath = '{0}'", path.ToString()));

                            if (rows.Length > 0)
                            {
                                newNode = currentNodes.Add(classification);
                                newNode.Name = classification;
                                newNode.Tag = rows[0]["ID"];
                                newNode.ImageIndex = 0;
                                newNode.SelectedImageIndex = 0;
                                diagramInfoTable[newNode.Tag.ToString()] = new DiagramInfo(hostName, editTime, currentVersion, 0);

                                currentNodes = newNode.Nodes;
                                diagramInfoTable[id] = new DiagramInfo(hostName, editTime, currentVersion, 0);
                            }
                            else
                            {
                                MessageBox.Show(string.Format("绘图分类信息不完整，没有找到路径为{0}的绘图！", path),
                                                "分类树构建", MessageBoxButtons.OK, MessageBoxIcon.Information);
                            }
                        }
                    }
                }
                               
            }

            // 刷新树节点子结点个数信息
            RefreshNodeCount(tree);            
        }

        /// <summary>
        /// 刷新树结点子结点个数信息
        /// </summary>
        /// <param name="tree">分类树</param>
        private void RefreshNodeCount(TreeView tree)
        {
            foreach(TreeNode node in tree.Nodes)
            {
                int childCount = 0;
                RefreshNodeCount(node, ref childCount);
            }
        }

        /// <summary>
        /// 刷新树结点子结点个数信息
        /// </summary>
        /// <param name="node">树结点</param>
        /// <param name="childCount">子结点个数</param>
        private void RefreshNodeCount(TreeNode node, ref int childCount)
        {
            if (node.ImageIndex == 0) // 分类结点才需要刷新子结点个数
            {
                int nodeCount = 0;

                foreach (TreeNode childNode in node.Nodes)
                {
                    nodeCount++;
                    RefreshNodeCount(childNode, ref nodeCount);                    
                }                

                DiagramInfo diagramInfo = diagramInfoTable[node.Tag.ToString()] as DiagramInfo;                
                diagramInfo.ReadOnly = false;
                diagramInfo.ChildCount = nodeCount;
                diagramInfo.ReadOnly = true;                
                
                childCount += nodeCount;
            }            
        }

        /// <summary>
        /// 保存分类树
        /// </summary>
        /// <returns>记录分类树信息的数据表</returns>
        private DataTable SaveTree()
        {
            DataTable table = new DataTable("");
            Type stringType = Type.GetType("System.String");
            Type boolType = Type.GetType("System.Boolean");

            table.Columns.Add("ID", stringType);
            table.Columns.Add("FullPath", stringType);
            table.Columns.Add("IsFolder", boolType);
            List<TreeNode> flowChartNodeList = new List<TreeNode>();

            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            // 遍历找出所有的绘图结点
            foreach(TreeNode node in tree.Nodes)
            {
                SeekChildNode(node, flowChartNodeList);
            }

            // 构造数据表
            string id;
            string fullPath;  
            DataRow newRow;

            foreach(TreeNode node in flowChartNodeList)
            {
                id = node.Tag.ToString();
                fullPath = GetNodePath(node);                
                newRow = table.NewRow();
                newRow["FullPath"] = fullPath;
                newRow["ID"] = id;
                newRow["IsFolder"] = (node.ImageIndex == 0);
                table.Rows.Add(newRow);
            }

            // 加入剪切的树结点代码的数据行
            if(treeMode == EditMode.Cut)
            {
                id = cutNode.Tag.ToString();
                newRow = table.NewRow();
                newRow["FullPath"] = cutTreeNodePath;
                newRow["ID"] = id;
                newRow["IsFolder"] = (cutNode.ImageIndex == 0);
                table.Rows.Add(newRow);
            }

            return table;
        }

        /// <summary>
        /// 检索子结点
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <param name="list">保存结点的链表</param>
        private void SeekChildNode(TreeNode node, List<TreeNode> list)
        {
            list.Add(node);

            foreach(TreeNode childNode in node.Nodes)
            {
                SeekChildNode(childNode, list);
            }
        }        

        /// <summary>
        /// 数据迁移
        /// </summary>
        public void TransferData()
        {
            TransferDataForm transferDataForm = new TransferDataForm();
            transferDataForm.ShowDialog();
        }

        /// <summary>
        /// 恢复绘图
        /// </summary>
        public void RestoreDiagram()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            TreeNode node = tree.SelectedNode;

            string originID = node.Tag.ToString();
            string sqlString = string.Format("SELECT ID, EditTime, Host, Version FROM DiagramBackup WHERE OriginID = '{0}'", originID);
            DataTable restoreTable = dataBaseManager.GetDataTable(sqlString);

            if (restoreTable.Rows.Count > 0) // 找到备份信息
            {
                RestoreForm rForm = new RestoreForm(originID, restoreTable);
                DialogResult result = rForm.ShowDialog();

                if (result == DialogResult.OK)
                {
                    // 写日志                    
                    string nodePath = GetNodePath(node);
                    logRecorder.SaveLog("恢复绘图", string.Format("{0}分类树恢复绘图[{1}]", chartMode, nodePath), true);
                }
            }
            else
            {
                MessageBox.Show("当前绘图没有找到对应的备份信息！", "恢复绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 增加新的分类
        /// </summary>
        public void AddClass()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeChanged()) // 当前树结构发生过变化
            {
                MessageBox.Show("当前目录结构已经被修改过，需要重新加载更新！", "修改目录",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 重新加载分类树
                ConstructTree(tree, chartMode);
            }
            else
            {                
                TreeAddClassCommand cmd = new TreeAddClassCommand(tree, "新建分类");

                if(cmd.Execute(mapList)) // 调整命令链表
                {
                    // 写日志
                    TreeNode node = tree.SelectedNode;
                    string nodePath = GetNodePath(node);
                    logRecorder.SaveLog("新建分类", string.Format("{0}分类树新建分类[{1}]", chartMode, nodePath), true);
                }
            }                                             
        }

        /// <summary>
        /// 查看绘图属性
        /// </summary>
        public void ViewChartProperty()
        {
            if (flowChartManager != null)
            {
                PropertyForm pForm = new PropertyForm(flowChartManager.CurrentGraphManager.CurrentCanvas.GraphElementList);
                pForm.ShowDialog();
            }
            else
            {
                MessageBox.Show("请先打开要查看属性的绘图！", "查看绘图属性", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 设置程序参数
        /// </summary>
        public void SetArgument()
        {
            SettingForm sForm = new SettingForm();
            sForm.ShowDialog();
        }

        /// <summary>
        /// 输出日志文本
        /// </summary>
        /// <param name="text">文本内容</param>
        public void PrintText(string text)
        {
            helper.PrintText(text);            
        }

        /// <summary>
        /// 将文本保存到文件中
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">文本内容</param>
        public void SaveTextToFile(string fileName, string content)
        {
            fileTable[fileName] = content;
        }

        /// <summary>
        /// 剪切图元
        /// </summary>
        public void CutGraphElement()
        {
            if (flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.CutGraphElement();
            }
            else
            {
                MessageBox.Show("当前没有正在编辑的绘图！", "剪切图元", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
            
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        public void CopyGraphElement()
        {
            if(flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.CopyGraphElement();
            }
            else
            {
                MessageBox.Show("当前没有正在编辑的绘图！", "复制图元", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        public void PasteGraphElement()
        {
            if(flowChartManager != null)
            {
                flowChartManager.CurrentGraphManager.PasteGraphElement();
            }
            else
            {
                MessageBox.Show("当前没有正在编辑的绘图！", "粘贴图元", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 查看日志
        /// </summary>
        public void ViewLog()
        {
            logRecorder.ViewLog();
        }

        /// <summary>
        /// 刷新图元的缩放比例显示
        /// </summary>
        public void RefreshCanvasScaleDisplay()
        {
            scaleComboBox.SelectedIndex = -1;
            scaleComboBox.WatermarkText = string.Format("{0}%", (int)(GraphSetting.GetGraphSetting().CanvasScale * 100));
        }

        /// <summary>
        /// 生成当前绘图的代码
        /// </summary>
        public void GenerateCode()
        {
            string outputDir = helper.OutputDir; // 输出文件目录

            if (outputDir != "") // 输入文件目录不为空
            {
                if(flowChartManager != null) // 当前绘图管理器不为空
                {
                    GraphManager graphManager = flowChartManager.CurrentGraphManager;
                    DataManager dataManager = flowChartManager.CurrentDataManager;
                    logBox.Text = "";
                    logBox.Refresh(); // 强制刷新一次

                    PrintText(string.Format("------ 开始加载绘图 {0} ------", flowChartManager.CurrentPath));
                    PrintText("初始化绘图成功！");
                    PrintText("------ 开始检查逻辑有效性 ------");

                    // 执行逻辑操作
                    LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
                    bool avail = logicManager.DoLogicOperation(flowChartManager, LogicType.CheckLogic, null);
                    bool success = false;

                    if (avail) // 检测绘图有效性
                    {
                        PrintText("逻辑有效性检查成功！");

                        // 先保存当前绘图
                        if (flowChartManager.ContentChanged)
                        {
                            SaveFlowChart(flowChartManager);
                        }

                        // 执行逻辑操作
                        success = logicManager.DoLogicOperation(flowChartManager, LogicType.CompileCode, null);                      
                    }
                    else
                    {
                        PrintText("逻辑有效性检查失败！");
                    }

                    logRecorder.SaveLog("生成代码", string.Format("{0}分类树生成绘图[{1}]的代码", chartMode, flowChartManager.CurrentPath), success);
                }
                else
                {
                    MessageBox.Show("当前没有正在打开的绘图！", "生成代码", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("输出文件目录不能为空！", "生成代码", MessageBoxButtons.OK, MessageBoxIcon.Information);
                SettingForm sForm = new SettingForm();
                sForm.ShowDialog();
            } 
        }

        /// <summary>
        /// 编辑注释
        /// </summary>
        public void EditRemark()
        {
            flowChartManager.CurrentGraphManager.EditDataElement();
        }

        /// <summary>
        /// 保存绘图
        /// </summary>
        /// <param name="flowChartManager">要保存的绘图</param>
        private void SaveFlowChart(FlowChartManager flowChartManager)
        {
            GraphManager graphManager = flowChartManager.CurrentGraphManager;
            DataManager dataManager = flowChartManager.CurrentDataManager;

            // 自动调整绘图版的大小
            graphManager.AdjustCanvasSize(); 

            // 保证绘图有效性
            graphManager.ValidateBeforeSave();
            dataManager.ValidateBeforeSave();

            // 保存绘图
            dataBaseManager.SaveDiagramData(flowChartManager.ID, flowChartManager.GetArguments());
            dataBaseManager.Diagram_Unlock(int.Parse(flowChartManager.ID)); // 解锁
            flowChartManager.EditTime = dataBaseManager.GetDiagramEditTime(flowChartManager.ID);
            flowChartManager.Version = dataBaseManager.GetDiagramVersion(flowChartManager.ID);
            flowChartManager.ContentChanged = false;            

            // 写日志
            logRecorder.SaveLog("保存绘图", string.Format("{0}分类树保存绘图[{1}]", chartMode, flowChartManager.CurrentPath), true);
        }

        /// <summary>
        /// 加载绘图
        /// </summary>
        /// <param name="id">绘图ID</param>
        /// <param name="data">绘图</param>
        private void LoadFlowChart(string id, object data)
        {
            Hashtable args = null; 
            
            if (data == null)
            {
                args = dataBaseManager.GetDiagramData(id) as Hashtable;
            }
            else 
            {
                args = data as Hashtable;
            }

            if (args != null) // 绘图数据不为空
            {
                flowChartManager.SetArguments(args as Hashtable);
            }

            DateTime editTime = dataBaseManager.GetDiagramEditTime(id);
            int version = dataBaseManager.GetDiagramVersion(id);
            flowChartManager.EditTime = editTime;
            flowChartManager.Version = version;

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.AfterLoad, null);

            // 写日志
            logRecorder.SaveLog("加载绘图", string.Format("{0}分类树加载绘图[{1}]", chartMode, flowChartManager.CurrentPath), true);
        }

        /// <summary>
        /// 加载备份的绘图
        /// </summary>
        /// <param name="backupID">备份ID</param>
        private void LoadBackupFlowChart(string backupID)
        {
            int originID = int.Parse(backupID) - backupIDOffset;

            Hashtable args = dataBaseManager.GetBackupDiagramData(originID.ToString()) as Hashtable;

            if (args != null) // 绘图数据不为空
            {
                flowChartManager.SetArguments(args as Hashtable);
            }

            DateTime editTime = dataBaseManager.GetBackupDiagramEditTime(originID.ToString());
            int version = dataBaseManager.GetBackupDiagramVersion(originID.ToString());
            flowChartManager.EditTime = editTime;
            flowChartManager.Version = version;
            flowChartManager.EnableSave = false; // 备份的绘图不允许保存

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
            logicManager.DoLogicOperation(flowChartManager, LogicType.AfterLoad, null);

            // 写日志
            logRecorder.SaveLog("加载绘图", string.Format("{0}分类树加载绘图[{1}]", chartMode, flowChartManager.CurrentPath), true);
        }

        /// <summary>
        /// 检查并保存绘图
        /// </summary>
        public void CheckAndSaveFlowChart()
        {
            TabItem currentTab = tabControl.SelectedTab;
            bool cancelSave = false;

            if (currentTab != null)
            {
                FlowChartManager currentFlowChartManager = flowChartManagerTable[currentTab.Tag] as FlowChartManager;

                if (currentFlowChartManager != null && currentFlowChartManager.ContentChanged) // 当前绘图被修改过并且未保存
                {
                    DialogResult result = MessageBox.Show(string.Format("绘图{0}尚未保存，是否现在保存？", currentFlowChartManager.Name), "数据保存",
                        MessageBoxButtons.YesNoCancel, MessageBoxIcon.Information);

                    switch (result)
                    {
                        case DialogResult.Yes:
                            {
                                SaveFlowChart(currentFlowChartManager);                                
                                break;
                            }
                        case DialogResult.No:
                            {
                                // 解锁
                                dataBaseManager.Diagram_Unlock(int.Parse(currentFlowChartManager.ID));
                                break;
                            }
                        case DialogResult.Cancel:
                            {
                                cancelSave = true;
                                break;
                            }
                    }
                }

                if (!cancelSave) // 移除标签页
                {
                    currentFlowChartManager.CurrentGraphManager.Release(); // 释放绘图的绘图资源                    
                    flowChartManagerTable.Remove(currentTab.Tag);
                    tabControl.Tabs.Remove(currentTab);                    
                }
            }
        }

        /// <summary>
        /// 保存当前绘图
        /// </summary>
        public void SaveCurrentFlowChart()
        {
            if (flowChartManager != null && flowChartManager.ContentChanged) // 当前绘图已经被修改过
            {                
                // 执行逻辑操作
                LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
                logicManager.DoLogicOperation(flowChartManager, LogicType.Save, null);

                SaveFlowChart(flowChartManager);                                
            } 
        }

        /// <summary>
        /// 检查并保存绘图
        /// </summary>
        public bool CheckAndSaveFlowCharts()
        {
            bool cancelSave = false;

            foreach (object o in flowChartManagerTable.Values)
            {
                if (cancelSave) // 取消保存则直接退出
                {
                    break;
                }

                FlowChartManager currentFlowChartManager = o as FlowChartManager;

                if (currentFlowChartManager != null && currentFlowChartManager.ContentChanged) // 当前绘图被修改过并且未保存
                {
                    DialogResult result = MessageBox.Show(string.Format("绘图{0}尚未保存，是否现在保存？", currentFlowChartManager.Name), "数据保存",
                        MessageBoxButtons.YesNoCancel, MessageBoxIcon.Information);

                    switch (result)
                    {
                        case DialogResult.Yes:
                            {
                                SaveFlowChart(currentFlowChartManager);
                                break;
                            }
                        case DialogResult.No:
                            {
                                // 解锁
                                dataBaseManager.Diagram_Unlock(int.Parse(currentFlowChartManager.ID));
                                break;
                            }
                        case DialogResult.Cancel:
                            {
                                cancelSave = true;
                                break;
                            }
                    }
                }
            }

            return cancelSave;
        }

        /// <summary>
        /// 增加新的绘图
        /// </summary>
        public void AddFlowChart()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeChanged()) // 当前树结构发生过变化
            {
                MessageBox.Show("当前目录结构已经被修改过，需要重新加载更新！", "修改目录",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 重新加载分类树               
                ConstructTree(tree, chartMode);
            }
            else
            {                
                TreeAddFlowChartCommand cmd = new TreeAddFlowChartCommand(tree, "新建绘图");
                
                if(cmd.Execute(tree)) // 调整命令链表
                {
                    AddNewPanel(); // 打开编辑该绘图

                    // 写日志
                    TreeNode node = tree.SelectedNode;
                    string nodePath = GetNodePath(node);
                    logRecorder.SaveLog("新建绘图", string.Format("{0}分类树新建绘图[{1}]", chartMode, nodePath), true);
                }
            }            
        }

        /// <summary>
        /// 剪切树结点
        /// </summary>
        public void CutNode()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeCanOperate(tree))
            {                
                TreeNode node = tree.SelectedNode;
                string nodePath = GetNodePath(node);
                
                TreeCutNodeCommand cmd = new TreeCutNodeCommand(tree, "剪切树结点");
                
                if (cmd.Execute(tree)) // 调整命令链表
                {
                    // 写日志
                    logRecorder.SaveLog("剪切分类/绘图", string.Format("{0}分类树剪切分类/绘图[{1}]", chartMode, nodePath), true);
                }
            }                        
        }

        /// <summary>
        /// 复制树结点
        /// </summary>
        public void CopyNode()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            TreeCopyCommand cmd = new TreeCopyCommand(tree, "复制树结点");
            
            if (cmd.Execute(tree)) // 调整命令链表
            {
                // 写日志
                TreeNode node = tree.SelectedNode;
                string nodePath = GetNodePath(node);
                logRecorder.SaveLog("复制分类/绘图", string.Format("{0}分类树复制分类/绘图[{1}]", chartMode, nodePath), true);
            }
        }

        /// <summary>
        /// 粘贴树结点
        /// </summary>
        public void PasteNode()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeChanged()) // 当前树结构发生过变化
            {
                MessageBox.Show("当前目录结构已经被修改过，需要重新加载更新！", "修改目录",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 重新加载分类树
                ConstructTree(tree, chartMode);
            }
            else
            {                
                TreePasteNodeCommand cmd = new TreePasteNodeCommand(tree, "粘贴树结点");
                
                if (cmd.Execute(tree)) // 调整命令链表
                {
                    // 写日志
                    TreeNode node = tree.SelectedNode;
                    string nodePath = GetNodePath(node);
                    logRecorder.SaveLog("粘贴分类/绘图", string.Format("{0}分类树粘贴分类/绘图[{1}]", chartMode, nodePath), true);
                }
            }            
        }

        /// <summary>
        /// 删除树结点
        /// </summary>
        public void DeleteNode()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeCanOperate(tree))
            {                
                TreeNode node = tree.SelectedNode;
                string nodePath = GetNodePath(node);
                TreeDeleteNodeCommand cmd = new TreeDeleteNodeCommand(tree, "删除树结点");
                
                if (cmd.Execute(tree)) // 命令执行成功
                {
                    List<TreeNode> list = new List<TreeNode>();
                    GetAllChildNodes(node, list);

                    foreach (TreeNode treeNode in list)
                    {
                        TabItem currentTab = null;

                        // 检查对应id的面板没有被打开
                        foreach (TabItem t in tabControl.Tabs)
                        {
                            if (t.Tag.ToString() == treeNode.Tag.ToString()) // 找到删除树结点对应的绘图面板
                            {
                                currentTab = t;
                                break;
                            }
                        }

                        if (currentTab != null) // 找到树结点对应的绘图面板
                        {
                            if (tabControl.SelectedTab == currentTab) // 置空当前绘图控制器
                            {
                                flowChartManager = null;
                            }

                            flowChartManagerTable.Remove(currentTab.Tag);
                            tabControl.Tabs.Remove(currentTab);
                        }
                    }

                    // 写日志
                    logRecorder.SaveLog("删除分类/绘图", string.Format("{0}分类树删除分类/绘图[{1}]", chartMode, nodePath), true);
                }
            }
        }

        /// <summary>
        /// 获取当前树结点的所有子结点
        /// </summary>
        /// <param name="node">获取当前树结点的所有子结点</param>
        /// <param name="list">保存结点的链表</param>
        private void GetAllChildNodes(TreeNode node, List<TreeNode> list)
        {
            list.Add(node);

            foreach(TreeNode childNode in node.Nodes)
            {
                GetAllChildNodes(childNode, list);
            }
        }

        /// <summary>
        /// 检查是否可以对分类树进行操作
        /// </summary>
        /// <param name="tree">分类树</param>
        /// <returns>是否可以进行操作</returns>
        private bool CheckTreeCanOperate(TreeView tree)
        {
            bool canOperate = true;

            if (CheckTreeChanged()) // 当前树结构发生过变化
            {
                MessageBox.Show("当前目录结构已经被修改过，需要重新加载更新！", "修改目录",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 重新加载分类树
                ConstructTree(tree, chartMode);

                canOperate = false;
            }
            else if (CheckFlowChartOpened(tree.SelectedNode)) // 操作影响当前打开的绘图
            {
                MessageBox.Show("有已经打开的绘图会受当前操作的影响，请先将其关闭！", "数据一致性检查",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);

                canOperate = false;
            }

            return canOperate;
        }

        /// <summary>
        /// 重命名树结点
        /// </summary>
        public void RenameNode()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeCanOperate(tree))
            {                
                TreeRenameCommand cmd = new TreeRenameCommand(tree, "重命名树结点");

                if (cmd.Execute(tree))
                {
                    // 写日志
                    TreeNode node = tree.SelectedNode;
                    string nodePath = GetNodePath(node);
                    logRecorder.SaveLog("重命名分类/绘图", string.Format("{0}分类树重命名分类/绘图[{1}]", chartMode, nodePath), true);
                }
            }                                
        }

        /// <summary>
        /// 检查树结点是否被选中
        /// </summary>
        /// <param name="currentNode">当前树结点</param>
        /// <param name="selectedNode">选中的树结点</param>
        /// <param name="p">当前点位置</param>
        private void CheckNodeSelected(TreeNode currentNode, Point p)
        {
            if (currentNode.Bounds.Contains(p))
            {
                selectedNode = currentNode;
            }
            else
            {
                foreach(TreeNode node in currentNode.Nodes)
                {
                    CheckNodeSelected(node, p);
                }
            }
        }

        /// <summary>
        /// 打开目录
        /// </summary>
        public void OpenFolder()
        {
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;

            if (CheckTreeChanged()) // 当前树结构发生过变化
            {
                MessageBox.Show("当前目录结构已经被修改过，需要重新加载更新！", "修改目录",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                // 重新加载分类树
                ConstructTree(tree, chartMode);
            }
            else
            {
                TreeNode currentNode = tree.SelectedNode;
                if(currentNode != null)
                {
                    // 执行逻辑操作
                    LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
                    logicManager.DoLogicOperation(null, LogicType.ViewFolder, currentNode);                    
                }
            }            
        }

        /// <summary>
        /// 查看绘图的代码
        /// </summary>
        public void ViewCode()
        {
            // 执行逻辑操作
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            string path = GetNodePath(tree.SelectedNode);
            LogicBaseManager logicManager = helper.GetLogicManager(chartMode);
            logicManager.DoLogicOperation(null, LogicType.ViewCode, path);
        }

        /// <summary>
        /// 弹出分类树右键菜单
        /// </summary>
        /// <param name="point">鼠标所在点</param>
        public void PopTreeNodeMenu(Point point)
        {
            selectedNode = null;

            // 获取当前分类树
            TreeView tree = treeTabControl.SelectedTab.Tag as TreeView;
            Point p = point + new Size(tree.RectangleToScreen(tree.ClientRectangle).Location) 
                    - new Size(tree.AutoScrollOffset); // 坐标转换，处理滚动条滚动的情况                   
           
            // 先检查当前是否有树结点被选中
            foreach(TreeNode node in tree.Nodes)
            {
                CheckNodeSelected(node, point);
            }

            if (selectedNode != null) // 有选中的树结点
            {
                treeMenu.SubItems["bNewNode"].SubItems["bNewClass"].Enabled = false;
                treeMenu.SubItems["bNewNode"].SubItems["bNewFlowChart"].Enabled = false;
                treeMenu.SubItems["bCutNode"].Enabled = false;
                treeMenu.SubItems["bCopyNode"].Enabled = false;
                treeMenu.SubItems["bPasteNode"].Enabled = false;
                treeMenu.SubItems["bDeleteNode"].Enabled = false;
                treeMenu.SubItems["bRenameNode"].Enabled = false;
                treeMenu.SubItems["bOpenFolder"].Enabled = false;
                treeMenu.SubItems["bViewCode"].Enabled = false;
                treeMenu.SubItems["bRestoreNode"].Enabled = false;
                treeMenu.SubItems["bSaveToFile"].Enabled = false;
                treeMenu.SubItems["bLoadFromFile"].Enabled = false;

                tree.SelectedNode = selectedNode;

                if (selectedNode.ImageIndex == 0) // 当前选中树结点是分类结点
                {
                    treeMenu.SubItems["bNewNode"].SubItems["bNewClass"].Enabled = true;                                                                               

                    if(selectedNode.Level > 0) // 对地图分类结点没有操作
                    {
                        treeMenu.SubItems["bNewNode"].SubItems["bNewFlowChart"].Enabled = true;
                        treeMenu.SubItems["bDeleteNode"].Enabled = true;
                        treeMenu.SubItems["bOpenFolder"].Enabled = true;
                    }

                    if (selectedNode.Level > 0) // 不允许重命名地图分类结点
                    {
                        treeMenu.SubItems["bRenameNode"].Enabled = true;
                    }                    
                    
                    if (treeMode == EditMode.Copy || treeMode == EditMode.Cut) // 只有剪切和复制模式才能粘贴
                    {
                        treeMenu.SubItems["bPasteNode"].Enabled = true;
                    }
                }
                else // 当前选中树结点是绘图结点
                {
                    treeMenu.SubItems["bCutNode"].Enabled = true;
                    treeMenu.SubItems["bCopyNode"].Enabled = true;
                    treeMenu.SubItems["bPasteNode"].Enabled = true;
                    treeMenu.SubItems["bDeleteNode"].Enabled = true;
                    treeMenu.SubItems["bRenameNode"].Enabled = true;
                    treeMenu.SubItems["bViewCode"].Enabled = true;
                    treeMenu.SubItems["bRestoreNode"].Enabled = true;
                    treeMenu.SubItems["bSaveToFile"].Enabled = true;
                    treeMenu.SubItems["bLoadFromFile"].Enabled = true;

                    if (treeMode == EditMode.Copy || treeMode == EditMode.Cut) // 只有剪切和复制模式才能粘贴
                    {
                        treeMenu.SubItems["bPasteNode"].Enabled = true;
                    }
                }
                
                // 弹出树结点操作菜单
                treeMenu.Popup(p);
            }                        
        }

        /// <summary>
        /// 调整命令链表
        /// </summary>
        /// <param name="commandIndex">当前命令编号</param>
        /// <param name="description">命令的描述</param>
        /// <param name="commandCount">命令总数</param>
        public void AdjustCommandList(int commandIndex, string description, int commandCount)
        {            
            List<ButtonItem> deleteButtonList = new List<ButtonItem>(); // 要删除的按钮的链表

            for (int i = commandIndex + 1; i < commandCount; i++)
            {                
                int buttonIndex = commandCount - 1 - i;

                if (buttonIndex >= 0 && buttonIndex < undoButton.SubItems.Count)
                {
                    deleteButtonList.Add((ButtonItem)undoButton.SubItems[buttonIndex]);
                }                
            }            

            // 删除命令按钮
            foreach (ButtonItem buttonItem in deleteButtonList)
            {
                undoButton.SubItems.Remove(buttonItem);
            }

            commandIndex++;
            commandCount++;

            // 添加新按钮            
            ButtonItem newButton = new ButtonItem("undoButton" + commandIndex, description);
            newButton.Tag = commandIndex.ToString();
            flowChartManager.CurrentGraphManager.CurrentEventManager.RegistCommandButtonEvent(newButton);
            undoButton.SubItems.Add(newButton, 0);

            if (commandIndex == commandCount - 1)
            {
                SetUndoState(true);
            }

            SetRedoState(false);  
        }

        /// <summary>
        /// 设置重做按钮的状态
        /// </summary>
        /// <param name="enable">是否有效</param>
        public void SetRedoState(bool enable)
        {
            redoButton.Enabled = enable;
        }

        /// <summary>
        /// 设置撤销按钮的状态
        /// </summary>
        /// <param name="enable">是否有效</param>
        public void SetUndoState(bool enable)
        {
            undoButton.Enabled = enable;
        }

        /// <summary>
        /// 撤销操作
        /// </summary>
        public void Undo()
        {
            if (flowChartManager != null) // 执行绘图的撤销操作
            {
                flowChartManager.CurrentGraphManager.Undo();
            }
        }

        /// <summary>
        /// 重做操作
        /// </summary>
        public void Redo()
        {
            if (flowChartManager != null) // 执行绘图的重做操作
            {
                flowChartManager.CurrentGraphManager.Redo();
            }
        }

        /// <summary>
        /// 绘图信息类
        /// </summary>
        class DiagramInfo
        {
            private string hostName; // 编辑者
            private DateTime editTime; // 编辑时间  
            private string currentVersion; // 版本号
            private int childCount = 0; // 子绘图的个数
            private bool readOnly = true; // 成员数据是否是只读
            
            /// <summary>
            /// 构造函数
            /// </summary>
            /// <param name="hostName">编辑者</param>
            /// <param name="editTime">编辑时间</param>
            /// <param name="currentVersion">版本号</param>
            /// <param name="childCount">子绘图的个数</param>
            public DiagramInfo(string hostName, DateTime editTime, string currentVersion, int childCount)
            {
                this.hostName = hostName;
                this.editTime = editTime;
                this.currentVersion = currentVersion;
                this.childCount = childCount;
            }

            /// <summary>
            /// 成员数据是否是只读
            /// </summary>
            [Browsable(false)]
            public bool ReadOnly
            {
                set
                {
                    readOnly = value;
                }

                get
                {
                    return readOnly;
                }
            }

            /// <summary>
            /// 编辑者
            /// </summary>
            [Browsable(true), DisplayName("编辑者"), Description("绘图上次编辑的用户名"), Category("绘图属性")]
            public string HostName
            {
                set
                {
                    if (readOnly)
                    {
                        MessageBox.Show("绘图信息不能被外部编辑！", "编辑绘图信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        hostName = value;
                    }
                }

                get
                {
                    return hostName;
                }
            }

            /// <summary>
            /// 版本号
            /// </summary>
            [Browsable(true), DisplayName("版本号"), Description("绘图当前的版本号"), Category("绘图属性")]
            public string CurrentVersion
            {
                set
                {
                    if (readOnly)
                    {
                        MessageBox.Show("绘图信息不能被外部编辑！", "编辑绘图信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        currentVersion = value;
                    }                    
                }

                get
                {
                    return currentVersion;
                }
            }

            /// <summary>
            /// 编辑时间
            /// </summary>
            [Browsable(true), DisplayName("编辑时间"), Description("绘图上次编辑的时间"), Category("绘图属性")]
            public DateTime EditTime
            {
                set
                {
                    if (readOnly)
                    {
                        MessageBox.Show("绘图信息不能被外部编辑！", "编辑绘图信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        editTime = value;
                    }
                }

                get
                {
                    return editTime;
                }
            }

            /// <summary>
            /// 子绘图个数
            /// </summary>
            [Browsable(true), DisplayName("子绘图个数"), Description("子绘图的个数"), Category("绘图属性")]
            public int ChildCount
            {
                set
                {
                    if (readOnly)
                    {
                        MessageBox.Show("绘图信息不能被外部编辑！", "编辑绘图信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        childCount = value;
                    }                    
                }

                get
                {
                    return childCount;
                }
            }
        }
    }
}
