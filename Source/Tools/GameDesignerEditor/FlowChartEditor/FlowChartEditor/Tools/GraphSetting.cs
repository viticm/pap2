using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlowChartEditor
{
    public class GraphSetting
    {
        private static GraphSetting graphSetting; // 绘图配置对象        

        private Color backgroundColor = Color.White; // 背景颜色
        private Brush backgroundBrush; // 背景画刷

        private Font graphElementTextFont; // 图元的文本字体
        private Font graphElementTextBoldFont; // 图元的文本粗体字体
        private Font graphElementTextItalicFont; // 图元的文本斜体字体
        private Font graphElementTextUnderLineFont; // 图元的文本下划线字体
        private Color graphElementTextColor = Color.Yellow; // 图元的文本颜色
        private Brush graphElementTextBrush; // 图元的文本笔刷
        
        private Color graphElementTextSelectedColor = Color.Red; // 图元的文本被选中状态的颜色
        private Brush graphElementTextSelectedBrush; // 图元的文本被选中状态的笔刷

        private int lineWidth = 2; // 连接线的宽度
        private Color lineTextColor = Color.Black; // 连接线的文本颜色
        private Color lineMarkedColor = Color.Goldenrod; // 连接线被标记状态的颜色

        private Brush lineTextBrush; // 连接线的文本画刷
        private Pen lineNormalPen; // 连接线的画笔
        private Pen lineActivatedPen; // 连接线被激活状态的画笔
        private Pen lineSelectedPen; // 连接线被选中状态的画笔        
        private Pen lineMarkdPen; // 连接线被被标记状态的画笔
 
        private Pen lineNormalArrawPen; // 带箭头连接线的画笔
        private Pen lineActivatedArrawPen; // 带箭头连接线被激活状态的画笔
        private Pen lineSelectedArrawPen; // 带箭头连接线被选中状态的画笔
        private Pen lineMarkedArrawPen; // 带箭头连接线被标记状态的画笔

        private int graphElementBorderWidth = 1; // 图元边框宽度

        private Color graphElementNormalColor = Color.Black; // 图元普通状态颜色
        private Color graphElementActivatedColor = Color.DarkOrange; // 图元被激活状态颜色
        private Color graphElementSelectedColor = Color.Red; // 图元被选中状态颜色
        private Color graphElementBindedColor = Color.Green; // 图元被绑定状态颜色
        private Color graphElementBorderColor = Color.RoyalBlue; // 图元的边框颜色

        private Brush graphElementNormalBrush; // 图元普通状态的画刷
        private Brush graphElementActivatedBrush; // 图元被激活状态的画刷
        private Brush graphElementSelectedBrush; // 图元被选中状态的画刷
        private Brush graphElementBindedBrush; // 图元被绑定状态的画刷

        private Pen graphElementNormalPen; // 图元普通状态的画笔
        private Pen graphElementActivatePen; // 图元被激活状态的画笔
        private Pen graphElementSelectedPen; // 图元被选中状态的画笔        
        private Pen graphElementBorderPen; // 图元的边框画笔

        private Color remarkBackgroundColor = Color.LightSteelBlue; // 注释的背景色
        private Brush remarkBackgroundBrush; // 注释的背景画刷

        private Color rodmanBackgroundColor = Color.Gray; // 游标的背景颜色
        private Color rodmanActivatedColor = Color.DarkOrange; // 游标被激活状态的颜色
        private Color rodmanSelectedColor = Color.Red; // 游标被选中状态的颜色
        private int rodmanWidth = 25; // 游标的宽度

        private Brush rodmanBackgroundBrush; // 游标的背景画刷
        private Brush rodmanActivatedBrush; // 游标被激活状态的画刷
        private Brush rodmanSelectedBrush; // 游标被选中状态的画刷        

        private Color tooltipBackgroundColor = Color.LightYellow; // 提示框背景颜色
        private Brush tooltipBackgroundBrush; // 提示框背景画刷

        private int multiSelectMarkWith = 2; // 多选标记的边框宽度
        private Color multiSelectMarkBackgroundColor = Color.FromArgb(127, Color.LightSteelBlue); // 多选标记的背景颜色
        private Color multiSelectMarkColor = Color.RoyalBlue; // 多选标记的颜色

        private Brush multiSelectMarkBackgroundBrush; // 多选标记的背景画刷
        private Pen multiSelectMarkPen; // 多选标记的画笔

        private int adjustLineWidth = 2; // 调整线的宽度
        private Color adjustLineColor = Color.Purple; // 调整线的颜色
        private Pen adjustLinePen; // 调整线的画笔

        private Color gridLineColor = Color.Silver; // 边框线的颜色
        private Pen gridLinePen; // 边框线的画笔

        private Image actionNodeNormalImage; // 动作结点普通状态下的图片
        private Image actionNodeSelectedImage; // 动作结点被选中状态下的图片
        private Image actionNodeActivatedImage; // 动作结点被激活状态下的图片
        private Image actionNodeShadowImage; // 动作结点的阴影图片
        private Image actionNodeAbbreviateImage; // 动作结点的缩略图片

        private Image conditionNodeNormalImage; // 条件结点普通状态下的图片
        private Image conditionNodeSelectedImage; // 条件结点被选中状态下的图片
        private Image conditionNodeActivatedImage; // 条件结点被激活状态下的图片
        private Image conditionNodeShadowImage; // 条件结点的阴影图片
        private Image conditionNodeAbbreviateImage; // 条件结点的缩略图片

        private Image eventNodeNormalImage; // 事件结点普通状态下的图片
        private Image eventNodeSelectedImage; // 事件结点被选中状态下的图片
        private Image eventNodeActivatedImage; // 事件结点被激活状态下的图片
        private Image eventNodeShadowImage; // 事件结点的阴影图片
        private Image eventNodeAbbreviateImage; // 事件结点的缩略图片

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

        private Image interfaceNodeNormalImage; // 接口结点普通状态下的图片
        private Image interfaceNodeSelectedImage; // 接口结点被选中状态下的图片
        private Image interfaceNodeActivatedImage; // 接口结点被激活状态下的图片
        private Image interfaceNodeShadowImage; // 接口结点阴影图片
        private Image interfaceNodeAbbreviateImage; // 接口结点缩略图片

        private Image connectButtonNormalImage; // 连接按钮普通状态下的图片
        private Image connectButtonActivatedImage; // 连接按钮被激活状态下的图片        

        private Image addButtonNormalImage; // 添加按钮普通状态下的图片
        private Image addButtonActivatedImage; // 添加按钮被激活状态下的图片   

        private Dictionary<string, Image> imageDictionary = new Dictionary<string, Image>(); // 图片字典

        private float canvasScale = 1.0f; // 绘图板的缩放参数

        private Size eventNodeElementSize = new Size(90, 90); // 事件结点的图元大小
        private Size eventNodeMoveOffset = new Size(45, 45); // 事件结点的移动偏移
        private Size conditionNodeElementSize = new Size(150, 90); // 条件结点的图元大小
        private Size conditionNodeMoveOffset = new Size(75, 45); // 条件结点的移动偏移
        private Size actionNodeElementSize = new Size(150, 90); // 动作结点的图元大小
        private Size actionNodeMoveOffset = new Size(75, 45); // 动作结点的移动偏移
        private Size aiStateNodeElementSize = new Size(100, 25); // AI状态结点的图元大小
        private Size aiStateNodeMovOffset = new Size(50, 12); // AI状态结点的移动偏移
        private Size aiActionNodeElementSize = new Size(100, 25); // AI动作结点的图元大小
        private Size aiActionNodeMoveOffset = new Size(50, 12); // AI动作结点的移动偏移
        private Size aiActionsNodeElementSize = new Size(100, 25); // AI动作组结点的图元大小
        private Size aiActionsNodeMoveOffset = new Size(50, 12); // AI动作组结点的移动偏移
        private Size innerChartElementSize = new Size(100, 25); // 子绘图结点的图元大小
        private Size innerChartMoveOffset = new Size(50, 12); // 子绘图结点的移动偏移
        private Size interfaceNodeElementSize = new Size(100, 25); // 接口结点的图元大小
        private Size interfaceNodeMoveOffset = new Size(50, 12); // 接口结点的移动偏移

        private Helper helper = Helper.GetHelper(); // 辅助类

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private GraphSetting()
        {

        }

        /// <summary>
        /// 获取绘图配置对象
        /// </summary>
        /// <returns>绘图配置对象</returns>
        public static GraphSetting GetGraphSetting()
        {
            if (graphSetting == null)
            {
                graphSetting = new GraphSetting();
            }

            return graphSetting;
        }        

        /// <summary>
        /// 事件结点的图元大小
        /// </summary>
        public Size EventNodeElementSize
        {
            set
            {
                eventNodeElementSize = value;
            }

            get
            {
                return eventNodeElementSize;
            }
        }

        /// <summary>
        /// 事件结点的移动偏移
        /// </summary>
        public Size EventNodeMoveOffset
        {
            set
            {
                eventNodeMoveOffset = value;
            }

            get
            {
                return eventNodeMoveOffset;
            }
        }

        /// <summary>
        /// 条件结点的图元大小
        /// </summary>
        public Size ConditionNodeElementSize
        {
            set
            {
                conditionNodeElementSize = value;
            }

            get
            {
                return conditionNodeElementSize;
            }
        }

        /// <summary>
        /// 条件结点的移动偏移
        /// </summary>
        public Size ConditionNodeMoveOffset
        {
            set
            {
                conditionNodeMoveOffset = value;
            }

            get
            {
                return conditionNodeMoveOffset;
            }
        }

        /// <summary>
        /// 动作结点图元大小
        /// </summary>
        public Size ActionNodeElementSize
        {
            set
            {
                actionNodeElementSize = value;
            }

            get
            {
                return actionNodeElementSize;
            }
        }

        /// <summary>
        /// 条件结点移动偏移
        /// </summary>
        public Size ActionNodeMoveOffset
        {
            set
            {
                actionNodeMoveOffset = value;
            }

            get
            {
                return actionNodeMoveOffset;
            }
        }

        /// <summary>
        /// AI状态结点的图元大小
        /// </summary>
        public Size AIStateNodeElementSize
        {
            set
            {
                aiStateNodeElementSize = value;
            }

            get
            {
                return aiStateNodeElementSize;
            }
        }

        /// <summary>
        /// AI状态结点的移动偏移
        /// </summary>
        public Size AIStateNodeMoveOffset
        {
            set
            {
                aiStateNodeMovOffset = value;
            }

            get
            {
                return aiStateNodeMovOffset;
            }
        }

        /// <summary>
        /// AI动作结点的图元大小
        /// </summary>
        public Size AIActionNodeElementSize
        {
            set
            {
                aiActionNodeElementSize = value;
            }

            get
            {
                return aiActionNodeElementSize;
            }
        }

        /// <summary>
        /// AI动作结点的移动偏移
        /// </summary>
        public Size AIActionNodeMoveOffset
        {
            set
            {
                aiActionNodeMoveOffset = value;
            }

            get
            {
                return aiActionNodeMoveOffset;
            }
        }

        /// <summary>
        /// AI动作组结点的图元大小
        /// </summary>
        public Size AIActionsNodeElementSize
        {
            set
            {
                aiActionsNodeElementSize = value;
            }

            get
            {
                return aiActionsNodeElementSize;
            }
        }

        /// <summary>
        /// AI动作组结点的移动偏移
        /// </summary>
        public Size AIActionsNodeMoveOffset
        {
            set
            {
                aiActionsNodeMoveOffset = value;
            }

            get
            {
                return aiActionsNodeMoveOffset;
            }
        }

        /// <summary>
        /// 子绘图结点的图元大小
        /// </summary>
        public Size InnerChartElementSize
        {
            set
            {
                innerChartElementSize = value;
            }

            get
            {
                return innerChartElementSize;
            }
        }

        /// <summary>
        /// 子绘图结点的移动偏移
        /// </summary>
        public Size InnerChartMoveOffset
        {
            set
            {
                innerChartMoveOffset = value;
            }

            get
            {
                return innerChartMoveOffset;
            }
        }

        /// <summary>
        /// 接口结点的图元大小
        /// </summary>
        public Size InterfaceNodeElementSize
        {
            set
            {
                interfaceNodeElementSize = value;
            }

            get
            {
                return interfaceNodeElementSize;
            }
        }

        /// <summary>
        /// 接口结点的移动偏移
        /// </summary>
        public Size InterfaceNodeMoveOffset
        {
            set
            {
                interfaceNodeMoveOffset = value;
            }

            get
            {
                return interfaceNodeMoveOffset;
            }
        }

        /// <summary>
        /// 图元的文本被选中状态的颜色
        /// </summary>
        public Color GraphElementTextSelectedColor
        {
            set
            {
                graphElementTextSelectedColor = value;
            }

            get
            {
                return graphElementTextSelectedColor;
            }
        }

        /// <summary>
        /// 图元的文本被选中状态的笔刷
        /// </summary>
        public Brush GraphElementTextSelectedBrush
        {
            get
            {
                if (graphElementTextSelectedBrush == null)
                {
                    graphElementTextSelectedBrush = new SolidBrush(graphElementTextSelectedColor);
                }

                return graphElementTextSelectedBrush;
            }
        }

        /// <summary>
        /// 背景颜色
        /// </summary>
        public Color BackgroundColor
        {
            set
            {
                backgroundColor = value;
            }

            get
            {
                return backgroundColor;
            }
        }

        /// <summary>
        /// 边框线的画笔
        /// </summary>
        public Pen GridLinePen
        {
            get
            {
                if (gridLinePen == null)
                {
                    gridLinePen = new Pen(gridLineColor);
                }

                return gridLinePen;
            }
        }

        /// <summary>
        /// 提示框背景笔刷
        /// </summary>
        public Brush TooltipBackgroundBrush
        {
            get
            {
                if (tooltipBackgroundBrush == null)
                {
                    tooltipBackgroundBrush = new SolidBrush(tooltipBackgroundColor);
                }

                return tooltipBackgroundBrush;
            }
        }

        /// <summary>
        /// 绘图板的缩放参数
        /// </summary>
        public float CanvasScale
        {
            set
            {
                canvasScale = value;
            }

            get
            {
                return canvasScale;
            }
        }

        /// <summary>
        /// 多选标记的背景画刷
        /// </summary>
        public Brush MultiSelectBackgroudBrush
        {
            get
            {
                if (multiSelectMarkBackgroundBrush == null)
                {
                    multiSelectMarkBackgroundBrush = new SolidBrush(multiSelectMarkBackgroundColor);
                }

                return multiSelectMarkBackgroundBrush;
            }
        }

        /// <summary>
        /// 多选标记的画笔
        /// </summary>
        public Pen MultiSelectMarkPen
        {
            get
            {
                if (multiSelectMarkPen == null)
                {
                    multiSelectMarkPen = new Pen(multiSelectMarkColor, multiSelectMarkWith);
                }

                return multiSelectMarkPen;
            }
        }

        /// <summary>
        /// 调整线的画笔
        /// </summary>
        public Pen AdjustLinePen
        {
            get
            {
                if (adjustLinePen == null)
                {
                    adjustLinePen = new Pen(adjustLineColor, adjustLineWidth);
                    adjustLinePen.DashStyle = DashStyle.Dash;
                }

                return adjustLinePen;
            }
        }

        /// <summary>
        /// 游标背景画刷
        /// </summary>
        public Brush RodmanBackgroundBrush
        {
            get
            {
                if (rodmanBackgroundBrush == null)
                {
                    rodmanBackgroundBrush = new SolidBrush(rodmanBackgroundColor);
                }

                return rodmanBackgroundBrush;
            }
        }

        /// <summary>
        /// 游标被激活状态的画刷
        /// </summary>
        public Brush RodmanActivatedBrush
        {
            get
            {
                if (rodmanActivatedBrush == null)
                {
                    rodmanActivatedBrush = new SolidBrush(rodmanActivatedColor);
                }

                return rodmanActivatedBrush;
            }
        }

        /// <summary>
        /// 游标被选中状态的画刷
        /// </summary>
        public Brush RodmanSelectedBrush
        {
            get
            {
                if (rodmanSelectedBrush == null)
                {
                    rodmanSelectedBrush = new SolidBrush(rodmanSelectedColor);
                }

                return rodmanSelectedBrush;
            }
        }        

        /// <summary>
        /// 动作结点普通状态的图片属性
        /// </summary>
        public Image ActionNodeNormalImage
        {
            get
            {
                if (actionNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("actionNode_normal");
                    actionNodeNormalImage = ResizeImage(image, actionNodeElementSize);
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
                if (actionNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("actionNode_selected");
                    actionNodeSelectedImage = ResizeImage(image, actionNodeElementSize);
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
                if (actionNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("actionNode_activated");
                    actionNodeActivatedImage = ResizeImage(image, actionNodeElementSize);
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
                if (actionNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("actionNode_shadow");
                    actionNodeShadowImage = ResizeImage(image, actionNodeElementSize);
                }

                return actionNodeShadowImage;
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
                    Image image = helper.LoadImageFromResource("actionNode_normal");
                    actionNodeAbbreviateImage = ResizeImage(image, actionNodeElementSize);
                }

                return actionNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 条件结点普通状态的图片属性
        /// </summary>
        public Image ConditionNodeNormalImage
        {
            get
            {
                if (conditionNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("conditionNode_normal");
                    conditionNodeNormalImage = ResizeImage(image, conditionNodeElementSize);
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
                if (conditionNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("conditionNode_selected");
                    conditionNodeSelectedImage = ResizeImage(image, conditionNodeElementSize);
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
                if (conditionNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("conditionNode_activated");
                    conditionNodeActivatedImage = ResizeImage(image, conditionNodeElementSize);
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
                if (conditionNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("conditionNode_shadow");
                    conditionNodeShadowImage = ResizeImage(image, conditionNodeElementSize);
                }

                return conditionNodeShadowImage;
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
                    Image image = helper.LoadImageFromResource("conditionNode_normal");
                    conditionNodeAbbreviateImage = ResizeImage(image, conditionNodeElementSize);
                }

                return conditionNodeAbbreviateImage;
            }
        }       

        /// <summary>
        /// 事件结点普通状态的图片属性
        /// </summary>
        public Image EventNodeNormalImage
        {
            get
            {
                if (eventNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("eventNode_normal");
                    eventNodeNormalImage = ResizeImage(image, eventNodeElementSize);
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
                if (eventNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("eventNode_selected");
                    eventNodeSelectedImage = ResizeImage(image, eventNodeElementSize);
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
                if (eventNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("eventNode_activated");
                    eventNodeActivatedImage = ResizeImage(image, eventNodeElementSize);
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
                if (eventNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("eventNode_shadow");
                    eventNodeShadowImage = ResizeImage(image, eventNodeElementSize);
                }

                return eventNodeShadowImage;
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
                    Image image = helper.LoadImageFromResource("eventNode_normal");
                    eventNodeAbbreviateImage = ResizeImage(image, eventNodeElementSize);
                }

                return eventNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 添加按钮普通状态的图片属性
        /// </summary>
        public Image AddButtonNormalImage
        {
            get
            {
                if (addButtonNormalImage == null)
                {
                    addButtonNormalImage = helper.LoadImageFromResource("addButton_normal");
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
                if (addButtonActivatedImage == null)
                {
                    addButtonActivatedImage = helper.LoadImageFromResource("addButton_activated");
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
                if (connectButtonNormalImage == null)
                {
                    connectButtonNormalImage = helper.LoadImageFromResource("connectButton_normal");
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
                if (connectButtonActivatedImage == null)
                {
                    connectButtonActivatedImage = helper.LoadImageFromResource("connectButton_activated");
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
                if (aiActionNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionNode_normal");
                    aiActionNodeNormalImage = ResizeImage(image, aiActionNodeElementSize);
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
                if (aiActionNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionNode_selected");
                    aiActionNodeSelectedImage = ResizeImage(image, aiActionNodeElementSize);
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
                if (aiActionNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionNode_activated");
                    aiActionNodeActivatedImage = ResizeImage(image, aiActionNodeElementSize);
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
                if (aiActionNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionNode_shadow");
                    aiActionNodeShadowImage = ResizeImage(image, aiActionNodeElementSize);
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
                    Image image = helper.LoadImageFromResource("aiActionNode_normal");
                    aiActionNodeAbbreviateImage = ResizeImage(image, aiActionNodeElementSize);
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
                if (aiActionsNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionsNode_normal");
                    aiActionsNodeNormalImage = ResizeImage(image, aiActionsNodeElementSize);
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
                if (aiActionsNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionsNode_selected");
                    aiActionsNodeSelectedImage = ResizeImage(image, aiActionsNodeElementSize);
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
                if (aiActionsNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionsNode_activated");
                    aiActionsNodeActivatedImage = ResizeImage(image, aiActionsNodeElementSize);
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
                if (aiActionsNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("actionNode_shadow");
                    aiActionsNodeShadowImage = ResizeImage(image, aiActionsNodeElementSize);
                }

                return aiActionsNodeShadowImage;
            }
        }

        /// <summary>
        /// ai动作组结点缩略图片
        /// </summary>
        public Image AIActionsNodeAbbreviateImage
        {
            get
            {
                if (aiActionsNodeAbbreviateImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiActionsNode_normal");
                    aiActionsNodeAbbreviateImage = ResizeImage(image, aiActionsNodeElementSize);
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
                if (aiStateNodeNormalImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiStateNode_normal");
                    aiStateNodeNormalImage = ResizeImage(image, aiStateNodeElementSize);
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
                if (aiStateNodeSelectedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiStateNode_selected");
                    aiStateNodeSelectedImage = ResizeImage(image, aiStateNodeElementSize);
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
                if (aiStateNodeActivatedImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiStateNode_activated");
                    aiStateNodeActivatedImage = ResizeImage(image, aiStateNodeElementSize);
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
                if (aiStateNodeShadowImage == null)
                {
                    Image image = helper.LoadImageFromResource("aiStateNode_shadow");
                    aiStateNodeShadowImage = ResizeImage(image, aiStateNodeElementSize);
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
                    Image image = helper.LoadImageFromResource("aiStateNode_normal");
                    aiStateNodeAbbreviateImage = ResizeImage(image, aiStateNodeElementSize);
                }

                return aiStateNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 接口结点普通状态图片
        /// </summary>
        public Image InterfaceNodeNormalImage
        {
            get
            {
                if (interfaceNodeNormalImage == null)
                {
                    interfaceNodeNormalImage = helper.LoadImageFromResource("interfaceNode_normal");
                }

                return interfaceNodeNormalImage;
            }
        }

        /// <summary>
        /// 接口结点被激活状态图片
        /// </summary>
        public Image InterfaceNodeActivatedImage
        {
            get
            {
                if (interfaceNodeActivatedImage == null)
                {
                    interfaceNodeActivatedImage = helper.LoadImageFromResource("interfaceNode_activated");
                }

                return interfaceNodeActivatedImage;
            }
        }

        /// <summary>
        /// 接口结点被选中状态图片
        /// </summary>
        public Image InterfaceNodeSelectedImage
        {
            get
            {
                if (interfaceNodeSelectedImage == null)
                {
                    interfaceNodeSelectedImage = helper.LoadImageFromResource("interfaceNode_selected");
                }

                return interfaceNodeSelectedImage;
            }
        }

        /// <summary>
        /// 接口结点阴影图片
        /// </summary>
        public Image InterfaceNodeShadowImage
        {
            get
            {
                if (interfaceNodeShadowImage == null)
                {
                    interfaceNodeShadowImage = helper.LoadImageFromResource("interfaceNode_shadow");
                }

                return interfaceNodeShadowImage;
            }
        }

        /// <summary>
        /// 接口结点缩略图片
        /// </summary>
        public Image InterfaceNodeAbbreviateImage
        {
            get
            {
                if (interfaceNodeAbbreviateImage == null)
                {                    
                    interfaceNodeAbbreviateImage = helper.LoadImageFromResource("interfaceNode_normal");
                }

                return interfaceNodeAbbreviateImage;
            }
        }

        /// <summary>
        /// 注释的背景画刷
        /// </summary>
        public Brush RemarkBackgroundBrush
        {
            get
            {
                if (remarkBackgroundBrush == null)
                {
                    remarkBackgroundBrush = new SolidBrush(remarkBackgroundColor);
                }

                return remarkBackgroundBrush;
            }
        }

        /// <summary>
        /// 图元的画笔
        /// </summary>
        public Pen GraphElementNormalPen
        {
            get
            {
                if (graphElementNormalPen == null)
                {
                    graphElementNormalPen = new Pen(graphElementNormalColor, graphElementBorderWidth);
                }

                return graphElementNormalPen;
            }
        }

        /// <summary>
        /// 图元被激活状态的画笔
        /// </summary>
        public Pen GraphElementActivatedPen
        {
            get
            {
                if (graphElementActivatePen == null)
                {
                    graphElementActivatePen = new Pen(graphElementActivatedColor, graphElementBorderWidth);
                }

                return graphElementActivatePen;
            }
        }

        /// <summary>
        /// 图元被选中状态的画笔
        /// </summary>
        public Pen GraphElementSelectedPen
        {
            get
            {
                if (graphElementSelectedPen == null)
                {
                    graphElementSelectedPen = new Pen(graphElementSelectedColor, graphElementBorderWidth);
                }

                return graphElementSelectedPen;
            }
        }

        /// <summary>
        /// 图元的边框画笔
        /// </summary>
        public Pen GraphElementBorderPen
        {
            get
            {
                if (graphElementBorderPen == null)
                {
                    graphElementBorderPen = new Pen(graphElementBorderColor, graphElementBorderWidth);
                }

                return graphElementBorderPen;
            }
        }

        /// <summary>
        /// 图元被绑定状态的画刷
        /// </summary>
        public Brush GraphElementBindedBrush
        {
            get
            {
                if (graphElementBindedBrush == null)
                {
                    graphElementBindedBrush = new SolidBrush(graphElementBindedColor);
                }

                return graphElementBindedBrush;
            }
        }

        /// <summary>
        /// 背景画刷
        /// </summary>
        public Brush BackgroundBrush
        {
            get
            {
                if (backgroundBrush == null)
                {
                    backgroundBrush = new SolidBrush(backgroundColor);
                }

                return backgroundBrush;
            }
        }

        /// <summary>
        /// 带箭头的连接线的画笔
        /// </summary>
        public Pen LineNormalArrawPen
        {
            get
            {
                if (lineNormalArrawPen == null)
                {
                    lineNormalArrawPen = new Pen(graphElementNormalColor, lineWidth);
                    lineNormalArrawPen.CustomEndCap = new AdjustableArrowCap(lineWidth * 2, lineWidth * 3, true);
                }

                return lineNormalArrawPen;
            }
        }

        /// <summary>
        /// 带箭头的连接线被激活状态的画笔
        /// </summary>
        public Pen LineActivatedArrawPen
        {
            get
            {
                if (lineActivatedArrawPen == null)
                {
                    lineActivatedArrawPen = new Pen(graphElementActivatedColor, lineWidth);
                    lineActivatedArrawPen.CustomEndCap = new AdjustableArrowCap(lineWidth * 2, lineWidth * 3, true);
                }

                return lineActivatedArrawPen;
            }
        }

        /// <summary>
        /// 带箭头连接线被选中状态的画笔
        /// </summary>
        public Pen LineSelectedArrawPen
        {
            get
            {
                if (lineSelectedArrawPen == null)
                {
                    lineSelectedArrawPen = new Pen(graphElementSelectedColor, lineWidth);
                    lineSelectedArrawPen.CustomEndCap = new AdjustableArrowCap(lineWidth * 2, lineWidth * 3, true);
                }

                return lineSelectedArrawPen;
            }
        }

        /// <summary>
        /// 带箭头连接线被标记状态的画笔
        /// </summary>
        public Pen LineMarkedArrawPen
        {
            get
            {
                if (lineMarkedArrawPen == null)
                {
                    lineMarkedArrawPen = new Pen(lineMarkedColor, lineWidth);
                    lineMarkedArrawPen.CustomEndCap = new AdjustableArrowCap(lineWidth * 2, lineWidth * 3, true);
                }

                return lineMarkedArrawPen;
            }
        }

        /// <summary>
        /// 连接线被标记状态的画笔
        /// </summary>
        public Pen LineMarkedPen
        {
            get
            {
                if (lineMarkdPen == null)
                {
                    lineMarkdPen = new Pen(lineMarkedColor, lineWidth);
                }

                return lineMarkdPen;
            }
        }

        /// <summary>
        /// 连接线的画笔
        /// </summary>
        public Pen LineNormalPen
        {
            get
            {
                if (lineNormalPen == null)
                {
                    lineNormalPen = new Pen(graphElementNormalColor, lineWidth);
                }

                return lineNormalPen;
            }
        }

        /// <summary>
        /// 连接线被激活状态的画笔
        /// </summary>
        public Pen LineActivatedPen
        {
            get
            {
                if (lineActivatedPen == null)
                {
                    lineActivatedPen = new Pen(graphElementActivatedColor, lineWidth);
                }

                return lineActivatedPen;
            }
        }

        /// <summary>
        /// 连接线被选中状态的画笔
        /// </summary>
        public Pen LineSelectedPen
        {
            get
            {
                if (lineSelectedPen == null)
                {
                    lineSelectedPen = new Pen(graphElementSelectedColor, lineWidth);
                }

                return lineSelectedPen;
            }
        }

        /// <summary>
        /// 连接线的文本笔刷
        /// </summary>
        public Brush LineTextBrush
        {
            get
            {
                if (lineTextBrush == null)
                {
                    lineTextBrush = new SolidBrush(lineTextColor);
                }

                return lineTextBrush;
            }
        }

        /// <summary>
        /// 游标的宽度
        /// </summary>
        public int RodmanWidth
        {
            set
            {
                rodmanWidth = value;
            }

            get
            {
                return rodmanWidth;
            }
        }

        /// <summary>
        /// 游标被选中的颜色
        /// </summary>
        public Color RodmanselectedColor
        {
            set
            {
                rodmanSelectedColor = value;
            }

            get
            {
                return rodmanSelectedColor;
            }
        }

        /// <summary>
        /// 游标被激活的颜色
        /// </summary>
        public Color RodmanActivatedColor
        {
            set
            {
                rodmanActivatedColor = value;
            }

            get
            {
                return rodmanActivatedColor;
            }
        }

        /// <summary>
        /// 游标的颜色
        /// </summary>
        public Color RodmanBackgroundColor
        {
            set
            {
                rodmanBackgroundColor = value;
            }

            get
            {
                return rodmanBackgroundColor;
            }
        }

        /// <summary>
        /// 图元的文本颜色
        /// </summary>
        public Color GraphElementTextColor
        {
            set
            {
                graphElementTextColor = value;
            }

            get
            {
                return graphElementTextColor;
            }
        }

        /// <summary>
        /// 图元的文本笔刷
        /// </summary>
        public Brush GraphElementTextBrush
        {
            get
            {
                if (graphElementTextBrush == null)
                {
                    graphElementTextBrush = new SolidBrush(graphElementTextColor);
                }

                return graphElementTextBrush;
            }
        }

        /// <summary>
        /// 图元的文本字体
        /// </summary>
        public Font GraphElementTextFont
        {
            get
            {
                if (graphElementTextFont == null)
                {
                    graphElementTextFont = new Font("Sim Sun", 8, FontStyle.Regular);
                }

                return graphElementTextFont;
            }
        }

        /// <summary>
        /// 图元的文本粗体字体
        /// </summary>
        public Font GraphElementTextBoldFont
        {
            get
            {
                if (graphElementTextBoldFont == null)
                {
                    graphElementTextBoldFont = new Font("Sim Sun", 8, FontStyle.Bold);
                }

                return graphElementTextBoldFont;
            }            
        }

        /// <summary>
        /// 图元的文本斜体字体
        /// </summary>
        public Font GraphElementTextItalicFont
        {
            get
            {
                if (graphElementTextItalicFont == null)
                {
                    graphElementTextItalicFont = new Font("Sim Sun", 8, FontStyle.Italic);
                }

                return graphElementTextItalicFont;
            }
        }

        /// <summary>
        /// 图元的文本下划线字体
        /// </summary>
        public Font GraphElementTextUnderLineFont
        {
            get
            {
                if (graphElementTextUnderLineFont == null)
                {
                    graphElementTextUnderLineFont = new Font("Sim Sun", 8, FontStyle.Underline);                    
                }

                return graphElementTextUnderLineFont;
            }
        }

        /// <summary>
        /// 图元普通状态的画刷
        /// </summary>
        public Brush GraphElementNormalBrush
        {
            get
            {
                if (graphElementNormalBrush == null)
                {
                    graphElementNormalBrush = new SolidBrush(graphElementNormalColor);
                }

                return graphElementNormalBrush;
            }
        }

        /// <summary>
        /// 图元被选中状态的画刷
        /// </summary>
        public Brush GraphElementSelectedBrush
        {
            get
            {
                if (graphElementSelectedBrush == null)
                {
                    graphElementSelectedBrush = new SolidBrush(graphElementSelectedColor);
                }

                return graphElementSelectedBrush;
            }
        }

        /// <summary>
        /// 图元被激活状态的画刷
        /// </summary>
        public Brush GraphElementActivatedBrush
        {
            get
            {
                if (graphElementActivatedBrush == null)
                {
                    graphElementActivatedBrush = new SolidBrush(graphElementActivatedColor);
                }

                return graphElementActivatedBrush;
            }
        }

        /// <summary>
        /// 缩放图片
        /// </summary>
        /// <param name="image">原始图片</param>
        /// <param name="size">图片大小</param>
        /// <returns>新图片</returns>
        private Image ResizeImage(Image image, Size size)
        {
            Bitmap bmp = new Bitmap(size.Width, size.Height);
            Graphics g = Graphics.FromImage(bmp);
            Rectangle rectangle = new Rectangle(Point.Empty, size);
            g.DrawImage(image, rectangle);
            g.Dispose();

            return bmp;
        }
    }
}
