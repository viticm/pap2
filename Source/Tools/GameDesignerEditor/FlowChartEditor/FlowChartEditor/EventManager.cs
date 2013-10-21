using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

using DevComponents.DotNetBar;

using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    public class EventManager
    {
        GraphManager graphManager; // 绘图管理器
        
        // 事件相关
        public delegate void OnConnectDelegate(object sender, ConnectEventArgs e);
        public delegate void OnDisconnectDelegate(object sender, ConnectEventArgs e);


        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="graphManager">绘图管理器对象</param>
        public EventManager(GraphManager graphManager)
        {
            this.graphManager = graphManager;
        }

        /// <summary>
        /// 注册绘图板的事件
        /// </summary>
        /// <param name="canvas">绘图板对象</param>
        public void RegistCanvasEvent(Canvas canvas)
        {
            canvas.MouseDoubleClick += canvas_MouseDoubleClick;
            canvas.MouseDown += canvas_MouseDown;
            canvas.MouseEnter += canvas_MouseEnter;
            canvas.MouseLeave += canvas_MouseLeave;
            canvas.MouseMove += canvas_MouseMove;
            canvas.MouseUp += canvas_MouseUp;
            canvas.MouseWheel += canvas_MouseWheel;
            canvas.KeyDown += canvas_KeyDown;
            canvas.KeyUp += canvas_KeyUp;
        }

        /// <summary>
        /// 注册命令按钮的事件
        /// </summary>
        /// <param name="buttonItem">按钮对象</param>
        public void RegistCommandButtonEvent(ButtonItem buttonItem)
        {
            buttonItem.Click += commandButton_Click;            
        }

        /// <summary>
        /// 命令按钮事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void commandButton_Click(object sender, EventArgs e)
        {
            ButtonItem buttonItem = sender as ButtonItem;

            if (buttonItem != null)
            {
                int commandIndex = int.Parse(buttonItem.Tag.ToString());
                graphManager.Undo(commandIndex);
            }
        }

        /// <summary>
        /// 键盘按下事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_KeyDown(object sender, KeyEventArgs e)
        {
            DocumentManager documentManager = DocumentManager.GetDocumentManager();
            FlowChartManager flowChartManager = documentManager.CurrentFlowChartManager;
            graphManager.ControlMode = e.Control;
            graphManager.ShiftMode = e.Shift;
            graphManager.AltMode = e.Alt;

            switch (e.KeyCode)
            {
                case Keys.Enter: // 响应enter按键
                    {
                        graphManager.EditDataElement();

                        break;
                    }
                case Keys.Escape: // 响应esc按键
                    {
                        graphManager.DeleteAbbreviateGraphElement();

                        break;
                    }
                case Keys.Space: // 响应空格键
                    {
                        graphManager.CurrentBackground.DebugMode = !graphManager.CurrentBackground.DebugMode;

                        break;
                    }
                case Keys.Left: // 响应左方向键
                    {
                        graphManager.MoveSelectedGraphElement(new Size(-1, 0));

                        break;
                    }
                case Keys.Up: // 响应上方向键
                    {
                        graphManager.MoveSelectedGraphElement(new Size(0, -1));

                        break;
                    }
                case Keys.Right: // 响应右方向键
                    {
                        graphManager.MoveSelectedGraphElement(new Size(1, 0));

                        break;
                    }
                case Keys.Down: // 响应下方向键
                    {
                        graphManager.MoveSelectedGraphElement(new Size(0, 1));

                        break;
                    }
                case Keys.Delete:// 响应delete按键
                    {
                        graphManager.DeleteSelectedGraphElement();

                        break;
                    }
                case Keys.D1: // 响应1按键
                    {
                        // 执行逻辑操作                
                        LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                        object[] logicData = new object[2];
                        logicData[0] = 1;
                        logicData[1] = (sender as Canvas).PointToClient(Cursor.Position);
                        logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAtPoint, logicData);

                        break;
                    }
                case Keys.NumPad1: // 响应1按键
                    {
                        goto case Keys.D1;
                    }
                case Keys.D2: // 响应2按键
                    {
                        // 执行逻辑操作 
                        LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                        object[] logicData = new object[2];
                        logicData[0] = 2;
                        logicData[1] = (sender as Canvas).PointToClient(Cursor.Position);
                        logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAtPoint, logicData);

                        break;
                    }
                case Keys.NumPad2: // 响应2按键
                    {
                        goto case Keys.D2;
                    }
                case Keys.D3: // 响应3按键
                    {
                        // 执行逻辑操作 
                        LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                        object[] logicData = new object[2];
                        logicData[0] = 3;
                        logicData[1] = (sender as Canvas).PointToClient(Cursor.Position);
                        logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAtPoint, logicData);

                        break;
                    }
                case Keys.NumPad3: // 响应3按键
                    {
                        goto case Keys.D3;
                    }
                case Keys.D4: // 响应4按键
                    {
                        // 执行逻辑操作
                        LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                        object[] logicData = new object[2];
                        logicData[0] = 4;
                        logicData[1] = (sender as Canvas).PointToClient(Cursor.Position);
                        logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAtPoint, logicData);

                        break;
                    }
                case Keys.NumPad4: // 响应4按键
                    {
                        goto case Keys.D4;
                    }
                case Keys.D5: // 响应5按键
                    {
                        // 执行逻辑操作
                        LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(flowChartManager.CurrentChartMode);
                        object[] logicData = new object[2];
                        logicData[0] = 5;
                        logicData[1] = (sender as Canvas).PointToClient(Cursor.Position);
                        logicManager.DoLogicOperation(flowChartManager, LogicType.CreateAtPoint, logicData);

                        break;
                    }
                case Keys.NumPad5: // 响应5按键
                    {
                        goto case Keys.D5;
                    }
                case Keys.Oemplus: // 响应+按键
                    {
                        graphManager.CurrentCanvas.AutoScroll = false;
                        graphManager.AdjustCanvasScale(1.1f);
                        documentManager.RefreshCanvasScaleDisplay();                        

                        break;
                    }
                case Keys.OemMinus: // 响应-按键
                    {                        
                        graphManager.CurrentCanvas.AutoScroll = false;
                        graphManager.AdjustCanvasScale(0.9f);
                        documentManager.RefreshCanvasScaleDisplay();                        

                        break;
                    }
                case Keys.Oemcomma: // 逗号
                    {
                        graphManager.SelectLineInSlotContainer();

                        break;
                    }
                case Keys.OemPeriod: // 句号
                    {
                        graphManager.SelectLineOutSlotContainer();

                        break;
                    }
                case Keys.F5: // 刷新绘图板
                    {
                        graphManager.RefreshRegion();

                        break;
                    }
            }            
        }

        /// <summary>
        /// 键盘弹起事件相应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_KeyUp(object sender, KeyEventArgs e)
        {
            graphManager.ControlMode = e.Control;
            graphManager.ShiftMode = e.Shift;
            graphManager.AltMode = e.Alt;
        }

        /// <summary>
        /// 鼠标按键按下事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseDown(object sender, MouseEventArgs e)
        {
            (sender as Canvas).Focus(); // 获取焦点

            switch (e.Button)
            {
                case MouseButtons.Left: // 鼠标左键点击
                    {
                        graphManager.CanvasMouseDown(e.Location);

                        break;
                    }
                case MouseButtons.Right: // 鼠标右键点击
                    {
                        // 先选中当前图元，再弹出右键菜单
                        graphManager.CheckGraphElementSelected(e.Location);
                        graphManager.ShowCanvasContextMenu(e.Location);

                        break;
                    }
                case MouseButtons.Middle: // 鼠标中键点击
                    {
                        graphManager.BeginScrollCavas(e.Location);

                        break;
                    }
            }
        }

        /// <summary>
        /// 绘图板鼠标滚动事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseWheel(object sender, MouseEventArgs e)
        {
            if (graphManager.ControlMode) // 绘图板缩放
            {
                return; // Control键的响应有问题，暂时屏蔽掉
                graphManager.CurrentCanvas.AutoScroll = false;

                if (e.Delta > 0)
                {
                    graphManager.AdjustCanvasScale(1.1f);
                }
                else
                {
                    graphManager.AdjustCanvasScale(0.9f);
                }

                DocumentManager.GetDocumentManager().RefreshCanvasScaleDisplay();                
            }
            else
            {
                if (graphManager.ShiftMode) // 按shift键时横向滚动
                {
                    graphManager.CurrentCanvas.Visible = false;
                    Size moveSize = new Size(-e.Delta, e.Delta); // 垂直方向反向滚动一次
                    moveSize.Width = (int)((double)moveSize.Width * graphManager.CurrentBackground.CanvasSize.Width / graphManager.CurrentCanvas.ClientSize.Width * GraphSetting.GetGraphSetting().CanvasScale); // 滚动条的单位与绘图板单位需要转换
                    moveSize.Height = (int)((double)moveSize.Height * graphManager.CurrentBackground.CanvasSize.Height / graphManager.CurrentCanvas.ClientSize.Height * GraphSetting.GetGraphSetting().CanvasScale); // 滚动条的单位与绘图板单位需要转换
                    graphManager.ScrollCanvas(moveSize, false);
                    graphManager.CurrentCanvas.Visible = true;
                    graphManager.CurrentCanvas.Focus();
                }

                graphManager.CurrentCanvas.Refresh();
            }
        }

        /// <summary>
        /// 绘图板鼠标按键弹起事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseUp(object sender, MouseEventArgs e)
        {            
            graphManager.CanvasMouseUp(e.Location);
        }

        /// <summary>
        /// 绘图板鼠标移动事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right) // 禁止右键移动
            {
                graphManager.MoveGraphElement(e.Location);
            }
        }

        /// <summary>
        /// 绘图板鼠标进入事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseEnter(object sender, EventArgs e)
        {
            
        }

        /// <summary>
        /// 绘图板鼠标离开事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseLeave(object sender, EventArgs e)
        {
            graphManager.RefreshCanvas();
        }

        /// <summary>
        /// 绘图板鼠标双击事件响应
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void canvas_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            graphManager.EditDataElement();
        }        
    }

    public class ConnectEventArgs : EventArgs
    {
        private readonly SlotGraphElement slot;
        private readonly ConnectorGraphElement connector;

        public ConnectEventArgs(SlotGraphElement slot, ConnectorGraphElement connector)
        {
            this.slot = slot;
            this.connector = connector;
        }

        public SlotGraphElement Slot
        {
            get
            {
                return slot;
            }
        }

        public ConnectorGraphElement Connector
        {
            get
            {
                return connector;
            }
        }
    }
}
