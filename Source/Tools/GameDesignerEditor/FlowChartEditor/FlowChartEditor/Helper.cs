using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Reflection;
using System.Collections;
using DevComponents.DotNetBar.Controls;

namespace FlowChartEditor
{
    public class Helper
    {
        private static Helper helper; // 唯一的私有实例

        private Image actionNodeNormalImage; // 动作结点普通状态下的图片
        private Image actionNodeSelectedImage; // 动作结点被选中状态下的图片
        private Image actionNodeActivatedImage; // 动作结点被激活状态下的图片
        private Image actionNodeShadowImage; // 动作结点的阴影图片
        private Image actionNodeDisableNormalImage; // 动作结点无效普通状态下的图片
        private Image actionNodeDisableSelectedImage; // 动作结点无效被选中状态下的图片
        private Image actionNodeDisableActivatedImage; // 动作结点无效被激活状态下的图片
        private Image actionNodeDisableShadowImage; // 动作结点无效状态下的阴影图片
        private Image actionNodeAbbreviateImage; // 动作结点的缩略图片
        private Image actionNodeDisableAbbreviateImage; // 动作结点无效状态下的缩略图片

        private Image conditionNodeNormalImage; // 条件结点普通状态下的图片
        private Image conditionNodeSelectedImage; // 条件结点被选中状态下的图片
        private Image conditionNodeActivatedImage; // 条件结点被激活状态下的图片
        private Image conditionNodeShadowImage; // 条件结点的阴影图片
        private Image conditionNodeDisableNormalImage; // 条件结点无效普通状态下的图片
        private Image conditionNodeDisableSelectedImage; // 条件结点无效被选中状态下的图片
        private Image conditionNodeDisableActivatedImage; // 条件结点无效被激活状态下的图片
        private Image conditionNodeDisableShadowImage; // 条件结点无效状态下的阴影图片
        private Image conditionNodeAbbreviateImage; // 条件结点的缩略图片
        private Image conditionNodeDisableAbbreviateImage; // 条件结点无效状态的缩略图片

        private Image eventNodeNormalImage; // 事件结点普通状态下的图片
        private Image eventNodeSelectedImage; // 事件结点被选中状态下的图片
        private Image eventNodeActivatedImage; // 事件结点被激活状态下的图片
        private Image eventNodeShadowImage; // 事件结点的阴影图片
        private Image eventNodeDisableNormalImage; // 事件结点无效普通状态下的图片
        private Image eventNodeDisableSelectedImage; // 事件结点无效被选中状态下的图片
        private Image eventNodeDisableActivatedImage; // 事件结点无效被激活状态下的图片
        private Image eventNodeDisableShadowImage; // 事件结点无效状态下的阴影图片
        private Image eventNodeAbbreviateImage; // 事件结点的缩略图片
        private Image eventNodeDisableAbbreviateImage; // 事件结点缩略状态的图片

        private Image aiActionNodeNormalImage; // ai动作结点普通状态下的图片
        private Image aiActionNodeSelectedImage; // ai动作结点被选中状态下的图片
        private Image aiActionNodeActivatedImage; // ai动作结点被激活状态下的图片
        private Image aiActionNodeShadowImage; // ai动作结点阴影图片
        private Image aiActionNodeAbbreviateImage; // ai动作结点缩略图片

        private Image aiActionsNodeNormalImage; // ai动作组结点普通状态下的图片
        private Image aiActionsNodeSelectedImage; // ai动作组结点被选中状态下的图片
        private Image aiActionsNodeActivatedImage; // ai动作组结点被激活状态下的图片
        private Image aiActionsNodeShadowImage; // ai动作组结点阴影图片
        private Image aiActionsNodeAbbreviateImage; // ai动作组结点缩略图片

        private Image aiStateNodeNormalImage; // ai状态结点普通状态下的图片
        private Image aiStateNodeSelectedImage; // ai状态结点被选中状态下的图片
        private Image aiStateNodeActivatedImage; // ai状态结点被激活状态下的图片
        private Image aiStateNodeShadowImage; // ai状态结点阴影图片
        private Image aiStateNodeAbbreviateImage; // ai状态结点缩略图片

        private Image connectButtonNormalImage; // 连接按钮普通状态下的图片
        private Image connectButtonActivatedImage; // 连接按钮被激活状态下的图片        

        private Image addButtonNormalImage; // 添加按钮普通状态下的图片
        private Image addButtonActivatedImage; // 添加按钮被激活状态下的图片   

        private Image scrollUpArraw; // 向上滚动的箭头
        private Image scrollLeftArraw; // 向左滚动的箭头
        private Image scrollDownArraw; // 向下滚动的箭头
        private Image scrollRightArraw; // 向右滚动的箭头

        private Font textFont; // 图元文本的字体
        private Font titleFont;  // 图元标题的字体 
        private Font tooltipFont;  // 图元提示文本的字体

        private Graphics graphicsObject; // 绘图对象
        private TextBoxX logBox; // 日志输出文本框

        private Point abbreviatePanelLocation = Point.Empty; // 绘图板缩略图控件的位置

        private string outputDir = ""; // 流程图的输出路径
        private bool showRemark = true; // 是否显示图元注释
        private bool autoAdjustLine = true; // 是否自动调整连接线
        private bool autoAdjustGrid = false; // 是否自动对齐到网格 
        private List<int> latestDiagramList = new List<int>(); // 最近打开的绘图
        private int latestDiagramIndex = 0; // 最近打开的绘图的索引序号

        private LogicBaseManager flowChartLogicManager; // 流程图逻辑管理器实例
        private LogicBaseManager aiChartLogicManager; // AI图逻辑管理器实例

        /// <summary>
        /// 构造函数
        /// </summary>
        private Helper()
        {

        }

        /// <summary>
        /// 获取辅助类的实例
        /// </summary>
        public static Helper GetHelper()
        {
            if(helper == null)
            {
                helper = new Helper();
            }

            return helper;
        }

        /// <summary>
        /// 设置流程图参数
        /// </summary>
        /// <param name="args">参数哈希表</param>
        public void SetArguments(Hashtable args)
        {           
            outputDir = args["OutPutDir"] as string;
            showRemark = (bool)args["ShowRemark"];
            autoAdjustLine = (bool)args["AutoAdjustLine"];
            autoAdjustGrid = (bool)args["AutoAdjustGrid"];
            latestDiagramList = args["LatestDiagramList"] as List<int>;

            if (latestDiagramList == null)
            {
                latestDiagramList = new List<int>();
            }

            if (args["LatestDiagramIndex"] == null)
            {
                latestDiagramIndex = 0;
            }
            else
            {
                latestDiagramIndex = int.Parse(args["LatestDiagramIndex"].ToString());
            }
        }

        /// <summary>
        /// 获取流程图参数
        /// </summary>
        /// <returns>参数哈希表</returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();
            args["OutPutDir"] = outputDir;
            args["ShowRemark"] = showRemark;
            args["AutoAdjustLine"] = autoAdjustLine;
            args["AutoAdjustGrid"] = autoAdjustGrid;
            args["LatestDiagramList"] = latestDiagramList;
            args["LatestDiagramIndex"] = latestDiagramIndex.ToString();

            return args;
        }

        /// <summary>
        /// 最近打开的绘图链表s
        /// </summary>
        public List<int> LatestDiagramList
        {
            get
            {
                return latestDiagramList;
            }
        }

        /// <summary>
        /// 流程图逻辑管理器实例
        /// </summary>
        public LogicBaseManager FlowChartLogicManager
        {
            get
            {
                if(flowChartLogicManager == null)
                {
                    try
                    {
                        Assembly assembly = Assembly.LoadFrom(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\FlowChartLogicManager.dll"));
                        flowChartLogicManager = assembly.CreateInstance("FlowChartLogicManager.LogicManager") as LogicBaseManager;
                    }
                    catch (Exception ex)
                    {
                        logBox.AppendText(string.Format("\r\n加载流程图逻辑dll时产生异常：{0}\r\n", ex.Message));
                        flowChartLogicManager = new LogicBaseManager();
                    }
                }

                return flowChartLogicManager;
            }
        }

        /// <summary>
        /// AI逻辑管理器实例
        /// </summary>
        public LogicBaseManager AIChartLogicManager
        {
            get
            {
                if (aiChartLogicManager == null)
                {
                    try
                    {
                        Assembly assembly = Assembly.LoadFrom(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\AIChartLogicManager.dll"));
                        aiChartLogicManager = assembly.CreateInstance("AIChartLogicManager.LogicManager") as LogicBaseManager;
                    }
                    catch (Exception ex)
                    {
                        logBox.AppendText(string.Format("\r\n加载AI图逻辑dll时产生异常：{0}\r\n", ex.Message));
                        aiChartLogicManager = new LogicBaseManager();
                    }
                }

                return aiChartLogicManager;
            }
        }

        /// <summary>
        /// 日志输出文本框
        /// </summary>
        public TextBoxX LogBox
        {
            set
            {
                logBox = value;
            }
        }

        /// <summary>
        /// 流程图的输入路径属性
        /// </summary>
        public string OutputDir
        {
            set
            {
                outputDir = value;
            }

            get
            {
                return outputDir;
            }
        }

        /// <summary>
        /// 绘图对象
        /// </summary>
        public Graphics GraphicsObject
        {
            get
            {
                if(graphicsObject == null)
                {
                    Bitmap bmp = new Bitmap(1, 1);
                    graphicsObject = Graphics.FromImage(bmp);
                }

                return graphicsObject;
            }
        }

        /// <summary>
        /// 是否自动对齐到网格属性
        /// </summary>
        public bool AutoAdjustGrid
        {
            set
            {
                autoAdjustGrid = value;
            }

            get
            {
                return autoAdjustGrid;
            }
        }

        /// <summary>
        /// 是否自动调整连接线
        /// </summary>
        public bool AutoAdjustLine
        {
            set
            {
                autoAdjustLine = value;
            }

            get
            {
                return autoAdjustLine;
            }
        }

        /// <summary>
        /// 是否显示图元的注释属性
        /// </summary>
        public bool ShowRemark
        {
            set
            {
                showRemark = value;                
            }

            get
            {
                return showRemark;
            }
        }

        /// <summary>
        /// 向上滚动的箭头图片属性
        /// </summary>
        public Image ScrollUpArraw
        {
            get
            {
                if (scrollUpArraw == null)
                {
                    scrollUpArraw = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\scrollArraw-up.wmf"));
                }

                return scrollUpArraw;
            }
        }

        /// <summary>
        /// 向下滚动的箭头图片属性
        /// </summary>
        public Image ScrollDownArraw
        {
            get
            {
                if(scrollDownArraw == null)
                {
                    scrollDownArraw = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\scrollArraw-down.wmf"));
                }

                return scrollDownArraw;
            }
        }

        /// <summary>
        /// 向左滚动的箭头图片属性
        /// </summary>
        public Image ScrollLeftArraw
        {
            get
            {
                if (scrollLeftArraw == null)
                {
                    scrollLeftArraw = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\scrollArraw-left.wmf"));
                }

                return scrollLeftArraw;
            }
        }

        /// <summary>
        /// 向右滚动的箭头图片属性
        /// </summary>
        public Image ScrollRightArraw
        {
            get
            {
                if (scrollRightArraw == null)
                {
                    scrollRightArraw = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\scrollArraw-right.wmf"));
                }

                return scrollRightArraw;
            }
        }

        /// <summary>
        /// 绘图板缩略图控件的位置属性
        /// </summary>
        public Point AbbreviatePanelLocation
        {
            set
            {
                abbreviatePanelLocation = value;
            }

            get
            {
                return abbreviatePanelLocation;
            }
        }

        /// <summary>
        /// 图元提示文本的字体属性
        /// </summary>
        public Font TooltipFont
        {
            get
            {
                if(tooltipFont == null)
                {
                    tooltipFont = new Font("Sim Sun", 8, FontStyle.Regular);
                }

                return tooltipFont;
            }
        }

        /// <summary>
        /// 图元标题的字体属性
        /// </summary>
        public Font TitleFont
        {
            get
            {
                if(titleFont == null)
                {
                    titleFont = new Font("Sim Sun", 8, FontStyle.Bold);
                }

                return titleFont;
            }
        }

        /// <summary>
        /// 图元文本的字体属性
        /// </summary>
        public Font TextFont
        {
            get
            {
                if(textFont == null)
                {
                    textFont = new Font("Sim Sun", 8, FontStyle.Regular);
                }

                return textFont;
            }
        }

        /// <summary>
        /// 动作结点普通状态的图片属性
        /// </summary>
        public Image ActionNodeNormalImage
        {
            get
            {
                if(actionNodeNormalImage == null)
                {
                    actionNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-normal.wmf"));
                }

                return actionNodeNormalImage;
            }
        }

        /// <summary>
        /// 动作结点被选中状态的图片属性
        /// </summary>
        public Image ActionNodeSelectedImage
        {
            get
            {
                if(actionNodeSelectedImage == null)
                {
                    actionNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-selected.wmf"));
                }

                return actionNodeSelectedImage;
            }
        }

        /// <summary>
        /// 动作结点被激活状态的图片属性
        /// </summary>
        public Image ActionNodeActivatedImage
        {
            get
            {
                if(actionNodeActivatedImage == null)
                {
                    actionNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-activated.wmf"));
                }

                return actionNodeActivatedImage;
            }
        }

        /// <summary>
        /// 动作结点阴影的图片属性
        /// </summary>
        public Image ActionNodeShadowImage
        {
            get
            {
                if(actionNodeShadowImage == null)
                {
                    actionNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-shadow.wmf"));
                }

                return actionNodeShadowImage;
            }
        }

        /// <summary>
        /// 动作结点无效普通状态的图片属性
        /// </summary>
        public Image ActionNodeDisableNormalImage
        {
            get
            {
                if(actionNodeDisableNormalImage == null)
                {
                    actionNodeDisableNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-disable-normal.wmf"));
                }

                return actionNodeDisableNormalImage;
            }
        }

        /// <summary>
        /// 动作结点无效被选中状态的图片属性
        /// </summary>
        public Image ActionNodeDisableSelectedImage
        {
            get
            {
                if (actionNodeDisableSelectedImage == null)
                {
                    actionNodeDisableSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-disable-selected.wmf"));
                }

                return actionNodeDisableSelectedImage;
            }
        }

        /// <summary>
        /// 动作结点无效被激活状态的图片属性
        /// </summary>
        public Image ActionNodeDisableActivatedImage
        {
            get
            {
                if (actionNodeDisableActivatedImage == null)
                {
                    actionNodeDisableActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-disable-activated.wmf"));
                }

                return actionNodeDisableActivatedImage;
            }
        }

        /// <summary>
        /// 动作结点无效状态的阴影图片属性
        /// </summary>
        public Image ActionNodeDisableShadowImage
        {
            get
            {
                if (actionNodeDisableShadowImage == null)
                {
                    actionNodeDisableShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-disable-shadow.wmf"));
                }

                return actionNodeDisableShadowImage;
            }
        }

        /// <summary>
        /// 动作结点的缩略图片属性
        /// </summary>
        public Image ActionNodeAbbreviateImage
        {
            get
            {
                if (actionNodeAbbreviateImage == null)
                {
                    actionNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-normal.wmf"));
                }

                return actionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 动作结点无效状态的缩略图片属性
        /// </summary>
        public Image ActionNodeDisableAbbreviateImage
        {
            get
            {
                if (actionNodeDisableAbbreviateImage == null)
                {
                    actionNodeDisableAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-disable-normal.wmf"));
                }

                return actionNodeDisableAbbreviateImage;
            }
        }

        /// <summary>
        /// 条件结点普通状态的图片属性
        /// </summary>
        public Image ConditionNodeNormalImage
        {
            get
            {
                if(conditionNodeNormalImage == null)
                {
                    conditionNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-normal.wmf"));
                }

                return conditionNodeNormalImage;
            }
        }

        /// <summary>
        /// 条件结点被选中状态的图片属性
        /// </summary>
        public Image ConditionNodeSelectedImage
        {
            get
            {
                if(conditionNodeSelectedImage == null)
                {
                    conditionNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-selected.wmf"));
                }

                return conditionNodeSelectedImage;
            }
        }

        /// <summary>
        /// 条件结点被激活状态的图片属性
        /// </summary>
        public Image ConditionNodeActivatedImage
        {
            get
            {
                if(conditionNodeActivatedImage == null)
                {
                    conditionNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-activated.wmf"));
                }

                return conditionNodeActivatedImage;
            }
        }

        /// <summary>
        /// 条件结点阴影的图片属性
        /// </summary>
        public Image ConditionNodeShadowImage
        {
            get
            {
                if(conditionNodeShadowImage == null)
                {
                    conditionNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-shadow.wmf"));
                }

                return conditionNodeShadowImage;
            }
        }

        /// <summary>
        /// 条件结点无效普通状态的图片属性
        /// </summary>
        public Image ConditionNodeDisableNormalImage
        {
            get
            {
                if (conditionNodeDisableNormalImage == null)
                {
                    conditionNodeDisableNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-disable-normal.wmf"));
                }

                return conditionNodeDisableNormalImage;
            }
        }

        /// <summary>
        /// 条件结点无效被选中状态的图片属性
        /// </summary>
        public Image ConditionNodeDisableSelectedImage
        {
            get
            {
                if (conditionNodeDisableSelectedImage == null)
                {
                    conditionNodeDisableSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-disable-selected.wmf"));
                }

                return conditionNodeDisableSelectedImage;
            }
        }

        /// <summary>
        /// 条件结点无效被激活状态的图片属性
        /// </summary>
        public Image ConditionNodeDisableActivatedImage
        {
            get
            {
                if (conditionNodeDisableActivatedImage == null)
                {
                    conditionNodeDisableActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-disable-activated.wmf"));
                }

                return conditionNodeDisableActivatedImage;
            }
        }

        /// <summary>
        /// 条件结点无效状态的阴影图片属性
        /// </summary>
        public Image ConditionNodeDisableShadowImage
        {
            get
            {
                if (conditionNodeDisableShadowImage == null)
                {
                    conditionNodeDisableShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-disable-shadow.wmf"));
                }

                return conditionNodeDisableShadowImage;
            }
        }

        /// <summary>
        /// 条件结点的缩略图片属性
        /// </summary>
        public Image ConditionNodeAbbreviateImage
        {
            get
            {
                if (conditionNodeAbbreviateImage == null)
                {
                    conditionNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-normal.wmf"));
                }

                return conditionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 条件结点无效状态的缩略图片属性
        /// </summary>
        public Image ConditionNodeDisableAbbreviateImage
        {
            get
            {
                if (conditionNodeDisableAbbreviateImage == null)
                {
                    conditionNodeDisableAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\conditionNode-disable-normal.wmf"));
                }

                return conditionNodeDisableAbbreviateImage;
            }
        }

        /// <summary>
        /// 事件结点普通状态的图片属性
        /// </summary>
        public Image EventNodeNormalImage
        {
            get
            {
                if(eventNodeNormalImage == null)
                {
                    eventNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-normal.wmf"));
                }

                return eventNodeNormalImage;
            }
        }

        /// <summary>
        /// 事件结点被选中状态的图片属性
        /// </summary>
        public Image EventNodeSelectedImage
        {
            get
            {
                if(eventNodeSelectedImage == null)
                {
                    eventNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-selected.wmf"));
                }

                return eventNodeSelectedImage;
            }
        }

        /// <summary>
        /// 事件结点被激活状态的图片属性
        /// </summary>
        public Image EventNodeActivatedImage
        {
            get
            {
                if(eventNodeActivatedImage == null)
                {
                    eventNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-activated.wmf"));
                }

                return eventNodeActivatedImage;
            }
        }

        /// <summary>
        /// 事件结点阴影的图片属性
        /// </summary>
        public Image EventNodeShadowImage
        {
            get
            {
                if(eventNodeShadowImage == null)
                {
                    eventNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-shadow.wmf"));
                }

                return eventNodeShadowImage;
            }
        }

        /// <summary>
        /// 事件结点无效普通状态的图片属性
        /// </summary>
        public Image EventNodeDisableNormalImage
        {
            get
            {
                if (eventNodeDisableNormalImage == null)
                {
                    eventNodeDisableNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-disable-normal.wmf"));
                }

                return eventNodeDisableNormalImage;
            }
        }

        /// <summary>
        /// 事件结点无效被选中状态的图片属性
        /// </summary>
        public Image EventNodeDisableSelectedImage
        {
            get
            {
                if (eventNodeDisableSelectedImage == null)
                {
                    eventNodeDisableSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-disable-selected.wmf"));
                }

                return eventNodeDisableSelectedImage;
            }
        }

        /// <summary>
        /// 事件结点无效被激活状态的图片属性
        /// </summary>
        public Image EventNodeDisableActivatedImage
        {
            get
            {
                if (eventNodeDisableActivatedImage == null)
                {
                    eventNodeDisableActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-disable-activated.wmf"));
                }

                return eventNodeDisableActivatedImage;
            }
        }

        /// <summary>
        /// 事件结点无效状态的阴影图片属性
        /// </summary>
        public Image EventNodeDisableShadowImage
        {
            get
            {
                if (eventNodeDisableShadowImage == null)
                {
                    eventNodeDisableShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-disable-shadow.wmf"));
                }

                return eventNodeDisableShadowImage;
            }
        }

        /// <summary>
        /// 事件结点的缩略图片属性
        /// </summary>
        public Image EventNodeAbbreviateImage
        {
            get
            {
                if (eventNodeAbbreviateImage == null)
                {
                    eventNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-normal.wmf"));
                }

                return eventNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 事件结点无效状态的缩略图片属性
        /// </summary>
        public Image EventNodeDisableAbbreviateImage
        {
            get
            {
                if (eventNodeDisableAbbreviateImage == null)
                {
                    eventNodeDisableAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\eventNode-disable-normal.wmf"));
                }

                return eventNodeDisableAbbreviateImage;
            }
        }

        /// <summary>
        /// 添加按钮普通状态的图片属性
        /// </summary>
        public Image AddButtonNormalImage
        {
            get
            {
                if(addButtonNormalImage == null)
                {
                    addButtonNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\addButton-normal.wmf"));
                }

                return addButtonNormalImage;
            }
        }

        /// <summary>
        /// 添加按钮被激活状态的图片属性
        /// </summary>
        public Image AddButtonActivatedImage
        {
            get
            {
                if(addButtonActivatedImage == null)
                {
                    addButtonActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\addButton-activated.wmf"));
                }

                return addButtonActivatedImage;
            }
        }

        /// <summary>
        /// 连接按钮普通状态的图片属性
        /// </summary>
        public Image ConnectButtonNormalImage
        {
            get
            {
                if(connectButtonNormalImage == null)
                {
                    connectButtonNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\connectButton-normal.wmf"));
                }

                return connectButtonNormalImage;
            }
        }

        /// <summary>
        /// 连接按钮被激活状态的图片属性
        /// </summary>
        public Image ConnectButtonActivatedImage
        {
            get
            {
                if(connectButtonActivatedImage == null)
                {
                    connectButtonActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\connectButton-activated.wmf"));
                }

                return connectButtonActivatedImage;
            }
        }

        /// <summary>
        /// ai动作结点普通状态的图片
        /// </summary>
        public Image AIActionNodeNormalImage
        {
            get
            {
                if(aiActionNodeNormalImage == null)
                {
                    aiActionNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionNode-normal.wmf"));
                }

                return aiActionNodeNormalImage;
            }
        }

        /// <summary>
        /// ai动作结点被选中状态的图片
        /// </summary>
        public Image AIActionNodeSelectedImage
        {
            get
            {
                if(aiActionNodeSelectedImage == null)
                {
                    aiActionNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionNode-selected.wmf"));
                }

                return aiActionNodeSelectedImage;
            }
        }

        /// <summary>
        /// ai动作结点被激活状态的图片
        /// </summary>
        public Image AIActionNodeActivatedImage
        {
            get
            {
                if(aiActionNodeActivatedImage == null)
                {
                    aiActionNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionNode-activated.wmf"));
                }

                return aiActionNodeActivatedImage;
            }
        }

        /// <summary>
        /// ai动作结点阴影状态的图片
        /// </summary>
        public Image AIActionNodeShadowImage
        {
            get
            {
                if(aiActionNodeShadowImage == null)
                {
                    aiActionNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionNode-shadow.wmf"));
                }

                return aiActionNodeShadowImage;
            }
        }

        /// <summary>
        /// ai动作结点缩略的图片
        /// </summary>
        public Image AIActionNodeAbbreviateImage
        {
            get
            {
                if (aiActionNodeAbbreviateImage == null)
                {
                    aiActionNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionNode-normal.wmf"));
                }

                return aiActionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// ai动作组结点普通状态下的图片
        /// </summary>
        public Image AIActionsNodeNormalImage
        {
            get
            {
                if(aiActionsNodeNormalImage == null)
                {
                    aiActionsNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionsNode-normal.wmf"));
                }

                return aiActionsNodeNormalImage;
            }
        }

        /// <summary>
        /// ai动作组结点被选中状态下的图片
        /// </summary>
        public Image AIActionsNodeSelectedImage
        {
            get
            {
                if(aiActionsNodeSelectedImage == null)
                {
                    aiActionsNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionsNode-selected.wmf"));
                }

                return aiActionsNodeSelectedImage;
            }
        }

        /// <summary>
        /// ai动作组结点被激活状态下的图片
        /// </summary>
        public Image AIActionsNodeActivatedImage
        {
            get
            {
                if(aiActionsNodeActivatedImage == null)
                {
                    aiActionsNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionsNode-activated.wmf"));
                }

                return aiActionsNodeActivatedImage;
            }
        }

        /// <summary>
        /// ai状态结点阴影图片
        /// </summary>
        public Image AIActionsNodeShadowImage
        {
            get
            {
                if(aiActionsNodeShadowImage == null)
                {
                    aiActionsNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\actionNode-shadow.wmf"));
                }

                return aiActionsNodeShadowImage;
            }
        }

        /// <summary>
        /// ai状态结点缩略图片
        /// </summary>
        public Image AIActionsNodeAbbreviateImage
        {
            get
            {
                if (aiActionsNodeAbbreviateImage == null)
                {
                    aiActionsNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiActionsNode-normal.wmf"));
                }

                return aiActionsNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// ai状态结点普通状态的图片
        /// </summary>
        public Image AIStateNodeNormalImage
        {
            get
            {
                if(aiStateNodeNormalImage == null)
                {
                    aiStateNodeNormalImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiStateNode-normal.wmf"));
                }

                return aiStateNodeNormalImage;
            }
        }

        /// <summary>
        /// ai状态结点被选中状态的图片
        /// </summary>
        public Image AIStateNodeSelectedImage
        {
            get
            {
                if(aiStateNodeSelectedImage == null)
                {
                    aiStateNodeSelectedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiStateNode-selected.wmf"));
                }

                return aiStateNodeSelectedImage;
            }
        }

        /// <summary>
        /// ai状态结点被激活状态的图片
        /// </summary>
        public Image AIStateNodeActivatedImage
        {
            get
            {
                if(aiStateNodeActivatedImage == null)
                {
                    aiStateNodeActivatedImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiStateNode-activated.wmf"));
                }

                return aiStateNodeActivatedImage;
            }
        }

        /// <summary>
        /// ai状态结点阴影图片
        /// </summary>
        public Image AIStateNodeShadowImage
        {
            get
            {
                if(aiStateNodeShadowImage == null)
                {
                    aiStateNodeShadowImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiStateNode-shadow.wmf"));
                }

                return aiStateNodeShadowImage;
            }
        }

        /// <summary>
        /// ai状态结点缩略图片
        /// </summary>
        public Image AIStateNodeAbbreviateImage
        {
            get
            {
                if (aiStateNodeAbbreviateImage == null)
                {
                    aiStateNodeAbbreviateImage = LoadImageFromFile(Path.Combine(Application.StartupPath, "Plugins\\FlowChart\\Images\\aiStateNode-normal.wmf"));
                }

                return aiStateNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 读取图片文件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>图片</returns>
        private Image LoadImageFromFile(string filePath)
        {
            Image image = null;

            try
            {
                image = Image.FromFile(filePath);
            }
            catch (IOException ex)
            {
                MessageBox.Show(string.Format("读取文件{0}时产生异常:" + ex.ToString(), filePath), "图片文件读取",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return image;
        }

        /// <summary>
        /// 获取当前用户名
        /// </summary>
        /// <returns>当前用户名</returns>
        public string GetHostName()
        {
            string hostName = Dns.GetHostName();

            return hostName;
        }

        /// <summary>
        /// 获取当前用户地址
        /// </summary>
        /// <returns>当前用户地址</returns>
        public string GetHostAddress()
        {            
            string ip = Dns.GetHostEntry(Dns.GetHostName()).AddressList[0].ToString();;

            return ip;
        }

        /// <summary>
        /// 将文本保存到文件
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">文本内容</param>
        public bool SaveTextToFile(string fileName, string content)
        {
            bool success = true;

            try
            {
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    string path = Path.GetDirectoryName(fileName); // 获取目录
                    if (!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));                        
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
                success = false;
            }

            return success;
        }

        /// <summary>
        /// 输出调试文本
        /// </summary>
        /// <param name="text">调试文本</param>
        public void PrintText(string text)
        {
            logBox.AppendText(string.Format("调试信息： {0}\r\n", text));
        }

        /// <summary>
        /// 记录最近打开的绘图ID
        /// </summary>
        /// <param name="id">最近打开的绘图ID</param>
        public void RecordLatestDiagram(int id)
        {
            if (latestDiagramList.Count < 5)
            {
                if (!latestDiagramList.Contains(id))
                {
                    latestDiagramList.Add(id);
                }
            }
            else
            {
                if (!latestDiagramList.Contains(id))
                {
                    latestDiagramList[latestDiagramIndex] = id;
                    latestDiagramIndex++;

                    if (latestDiagramIndex > 4)
                    {
                        latestDiagramIndex = 0;
                    }
                }                                
            }
        }

        /// <summary>
        /// 获取逻辑管理器实例
        /// </summary>
        /// <param name="chartMode">当前编辑模式</param>
        /// <returns>逻辑管理器实例</returns>
        public LogicBaseManager GetLogicManager(ChartMode chartMode)
        {
            LogicBaseManager logicManager = null;

            switch(chartMode)
            {
                case ChartMode.FlowChart: // 编辑流程图模式
                    {
                        logicManager = FlowChartLogicManager;
                        break;
                    }

                case ChartMode.AIChart: // 编辑AI图模式
                    {
                        logicManager = AIChartLogicManager;
                        break;
                    }
            }

            return logicManager;
        }
    }
}
