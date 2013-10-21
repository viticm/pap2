using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;
using DataElementLibrary;
using GraphElementLibrary;
using DBManager;
using DataInteractor;

namespace FlowChartEditor
{
    public class FlowChartManager
    {
        private TabItem tabItem; // 流程图对应的标签页
        private string id; // 流程图的id
        private string name; // 流程图的名称
        private string mapName; // 流程图所在地图的名称
        private string path; // 流程图的路径
        private int version; // 流程图的编辑版本 
        private DateTime editTime; // 流程图的编辑时间
        private GraphManager graphManager; // 图元管理器
        private DataManager dataManager; // 数据管理器
        private DataBaseManager dataBaseManager; // 数据库管理器
        private bool locked = false; // 是否已经锁定当前流程图
        private bool contentChanged = false; // 流程图内容是否改变属性
        private Hashtable pasteTable = new Hashtable(); // 粘贴复制图元的哈希表
        private FlowChartInteractor flowChartInteractor; // 流程图交互管理器实例
        private bool supportCommand = false; // 是否提供命令支持
        private bool enableSave = true; // 是否允许保存
        private Hashtable customData = new Hashtable(); // 自定义数据
        private readonly string chartMode; // 绘图类型

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="chartMode">绘图的编辑模式</param>
        public FlowChartManager(string chartMode)
        {
            this.chartMode = chartMode;
        }

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="chartMode">绘图的编辑模式</param>
        /// <param name="tabItem">标签页</param>
        /// <param name="id">绘图的id</param>
        /// <param name="name">绘图的名称</param>
        /// <param name="path">绘图的路径</param>
        public FlowChartManager(string chartMode, TabItem tabItem, string id, string name, string path)
        {
            this.chartMode = chartMode;
            this.tabItem = tabItem;
            this.id = id;
            this.name = name;
            this.path = path;            
        }
        
        /// <summary>
        /// 构造函数
        /// </summary>
        public FlowChartManager()
        {

        }

        /// <summary>
        /// 绘图的自定义数据
        /// </summary>
        public Hashtable CustomData
        {
            get
            {
                return customData;
            }
        }

        /// <summary>
        /// 是否允许保存
        /// </summary>
        public bool EnableSave
        {
            set
            {
                enableSave = value;
            }

            get
            {
                return enableSave;
            }
        }

        /// <summary>
        /// 命令支持
        /// </summary>
        public bool SupportCommand
        {
            set
            {
                supportCommand = value;
            }

            get
            {
                return supportCommand;
            }
        }

        /// <summary>
        /// 流程图交互管理器实例
        /// </summary>
        public FlowChartInteractor CurrentFlowChartInteractor
        {
            get
            {
                return flowChartInteractor;
            }
        }

        /// <summary>
        /// 绘图的编辑模式
        /// </summary>
        public string CurrentChartMode
        {
            get
            {
                return chartMode;
            }
        }

        /// <summary>
        /// 粘贴复制图元的哈希表
        /// </summary>
        public Hashtable PasteTable
        {
            set
            {
                pasteTable = value;
            }

            get
            {
                return pasteTable;
            }
        }

        /// <summary>
        /// 流程图的内容是否改变属性
        /// </summary>
        public bool ContentChanged
        {
            set
            {
                if (enableSave) // 不存的数据就不必存了
                {
                    MarkTabContent(value);
                }                
            }

            get
            {
                return contentChanged;
            }
        }

        /// <summary>
        /// 流程图的编辑版本
        /// </summary>
        public int Version
        {
            set
            {
                version = value;
            }

            get
            {
                return version;
            }
        }

        /// <summary>
        /// 流程图的编辑时间
        /// </summary>
        public DateTime EditTime
        {
            set
            {
                editTime = value;
            }

            get
            {
                return editTime;
            }
        }

        /// <summary>
        /// 流程图的路径属性
        /// </summary>
        public string CurrentPath
        {
            set
            {
                path = value;
            }

            get
            {
                return path;
            }
        }

        /// <summary>
        /// 流程图的ID
        /// </summary>
        public string ID
        {
            get
            {
                return id;
            }
        }

        /// <summary>
        /// 流程图的名称
        /// </summary>
        public string Name
        {
            get
            {
                return name;
            }
        }

        /// <summary>
        /// 流程图所在地图的名称
        /// </summary>
        public string MapName
        {
            get
            {
                return mapName;
            }
        }

        /// <summary>
        /// 图元管理器属性
        /// </summary>
        public GraphManager CurrentGraphManager
        {
            get
            {
                return graphManager;
            }
        }

        /// <summary>
        /// 数据管理器属性
        /// </summary>
        public DataManager CurrentDataManager
        {
            get
            {
                return dataManager;
            }
        }        

        /// <summary>
        /// 检查数据是否有效
        /// </summary>
        public void CheckDataValid()
        {
            string requestReloadID = dataManager.RequestReloadID;

            if (requestReloadID != "")
            {
                DialogResult result = MessageBox.Show("发现绘图部分数据在程序外发生改变，需要重新编辑更新！", "数据一致性检查", 
                                                      MessageBoxButtons.OKCancel, MessageBoxIcon.Information);
                if (result == DialogResult.OK)
                {
                    string[] idArray = requestReloadID.Split(new char[] { ',' });
                    Hashtable argTable = new Hashtable();
                    argTable["globe_args"] = dataManager.GlobeArgs;
                    argTable["requestReloadID"] = "";
                    argTable["forceReload"] = "1";

                    foreach(string id in idArray)
                    {
                        GraphElement graphElement = dataManager.FindGraphElementByID(int.Parse(id));

                        if (graphElement != null)
                        {
                            DataElement dataElement = dataManager.FindDataElementByID(int.Parse(id));
                            dataElement.ReloadData(argTable);
                            graphManager.SelectGraphElement(graphElement, true);
                            graphManager.EditDataElement(graphElement);
                        }
                    }                    
                }                
            }
        }

        /// <summary>
        /// 设置流程图参数
        /// </summary>
        /// <param name="args">参数数组</param>
        public void SetArguments(Hashtable args)
        {
            try
            {                
                flowChartInteractor.SetArguments(args["flowChartInteractData"] as Hashtable);

                Hashtable graphData = args["graphData"] as Hashtable;
                Hashtable logicData = args["logicData"] as Hashtable;                

                graphManager.SetArguments(graphData);                
                dataManager.SetArguments(logicData);
                customData = args["customData"] as Hashtable;

                if (customData == null)
                {
                    customData = new Hashtable();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("初始化绘图失败：" + ex.Message, "加载绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 获取绘图参数
        /// </summary>
        /// <returns>参数数组</returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();            
            args["graphData"] = graphManager.GetArguments();
            args["logicData"] = dataManager.GetArguments();
            args["flowChartInteractData"] = flowChartInteractor.GetArguments();
            args["customData"] = customData;           

            return args;
        }

        /// <summary>
        /// 标记标签页的内容
        /// </summary>
        /// <param name="mark">是否标记</param>
        public void MarkTabContent(bool mark)
        {
            if (mark)
            {                
                if (!locked) // 还没锁定当前流程图
                {
                    DocumentManager documentManager = DocumentManager.GetDocumentManager();
                    DateTime latestEditTime = dataBaseManager.GetDiagramEditTime(id);
                    string editor = dataBaseManager.GetDiagramEditor(id);
                    int latestVersion = dataBaseManager.GetDiagramVersion(id);

                    if (version != latestVersion) // 检查是否是最新内容
                    {
                        MessageBox.Show(string.Format("当前绘图已经被{0}在{1}修改过，需要重新加载以获取最新内容！", editor, latestEditTime.ToString()),
                                        "绘图锁定", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        ReLoadFlowChart();
                        graphManager.ClearCommandList();
                        documentManager.ClearCommandButton();
                        contentChanged = false;
                    }
                    else // 尝试加锁
                    {
                        try
                        {                        
                            if(dataBaseManager.Diagram_Lock(int.Parse(id))) // 加锁成功
                            {
                                locked = true;
                                tabItem.Text = name + " *";
                                contentChanged = true;
                            }
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(string.Format("当前绘图已经被{0}锁定编辑，请等待修改完成后再尝试打开！", ex.Message), "绘图锁定",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                            ReLoadFlowChart();
                            graphManager.ClearCommandList();
                            documentManager.ClearCommandButton();
                            contentChanged = false;
                        } 
                    }                                                           
                }
                else // 已经锁定
                {
                    tabItem.Text = name + " *";
                    contentChanged = true;                 
                }
            }
            else
            {
                tabItem.Text = name;                
                locked = false;
                contentChanged = false;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        /// <param name="canvas">绘图板实例</param>
        /// <param name="propertyGrid">属性面板实例</param>
        public void Init(Canvas canvas)
        {
            graphManager = new GraphManager(this, canvas);
            dataManager = new DataManager(this);
            dataBaseManager = DataBaseManager.GetDataBaseManager();
            int i = path.IndexOf('\\');
            mapName = path.Substring(0, i);
            flowChartInteractor = new FlowChartInteractor();
        }                

        /// <summary>
        /// 编辑数据元
        /// </summary>
        /// <param name="graphElement">索引的数据元</param>
        /// <return>是否编辑成功</return>
        public bool EditDataElement(GraphElement graphElement)
        {            
            bool result = dataManager.EditData(graphElement);
            return result;
        }

        /// <summary>
        /// 重新加载流程图
        /// </summary>
        private void ReLoadFlowChart()
        {
            Hashtable args = dataBaseManager.GetDiagramData(id) as Hashtable;
            if (args != null) // 流程图数据不为空
            {
                graphManager.SetArguments(args["graphData"] as Hashtable);
                dataManager.SetArguments(args["logicData"] as Hashtable);
                flowChartInteractor.SetArguments(args["flowChartInteractData"] as Hashtable);
            }

            editTime = dataBaseManager.GetDiagramEditTime(id);
            version = dataBaseManager.GetDiagramVersion(id);
        }
    }
}
