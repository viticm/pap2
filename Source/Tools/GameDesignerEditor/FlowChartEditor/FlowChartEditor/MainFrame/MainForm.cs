using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;
using System.Collections;
using System.Drawing.Imaging;
using System.Diagnostics;
using System.Reflection;

using DevComponents.DotNetBar;

using GraphElementLibrary;
using DBManager;

namespace FlowChartEditor
{
    public partial class MainForm : Office2007Form
    {                
        private DataBaseManager dataBaseManager; // 数据库管理器
        private DocumentManager documentManager; // 分类管理器               

        /// <summary>
        /// 构造函数
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化数据库管理器
            dataBaseManager = DataBaseManager.GetDataBaseManager();                       

            // 初始化分类管理器
            documentManager = DocumentManager.GetDocumentManager();
            documentManager.TreeMenu = this.bTreeMenu;
            documentManager.UndoButton = this.bUndoCommand;
            documentManager.RedoButton = this.bRedoCommand;
            documentManager.CommandSwitchButton = this.bCommandSwitch;
            documentManager.CurrentTabControl = this.canvasGroupContainer;
            documentManager.TreeTabControl = this.treeContainer;
            documentManager.CurrentAbbreviateCanvas = this.abbreviateCanvas;
            documentManager.CurrentPropertyGrid = this.propertyGrid1;
            documentManager.LogBox = this.LogEditBox;
            documentManager.TreePanel = this.expandablePanel1;
            documentManager.TreeImageList = this.treeNodeImageList;
            documentManager.ScaleComboBox = this.canvasScaleBox;
            documentManager.AddDiagramRecordButton(this.bLatestDiagram1);
            documentManager.AddDiagramRecordButton(this.bLatestDiagram2);
            documentManager.AddDiagramRecordButton(this.bLatestDiagram3);
            documentManager.AddDiagramRecordButton(this.bLatestDiagram4);
            documentManager.AddDiagramRecordButton(this.bLatestDiagram5);
            documentManager.Init();
                       
            // 显示数据库名称
            LDBName.Text = dataBaseManager.Connection.Database;

            #if (!DEBUG) // 自动隐藏日志条
                barLog.AutoHide = true; 
            #endif
        }

        /// <summary>
        /// 加载主窗体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void MainForm_Load(object sender, EventArgs e)
        {
            try
            {                
                tabStrip1.MdiForm = this;
                
                // 读取布局设置
                FileInfo fi = new FileInfo(Path.Combine(Application.StartupPath, "_flowchart_layout.xml"));
                if (fi.Exists)
                {
                    dotNetBarManager1.LoadLayout(Path.Combine(Application.StartupPath, "_flowchart_layout.xml")); 
                }

                // 初始化状态条内容
                LHostName.Text = Helper.GetHelper().GetHostName();
            }
            catch (IOException ex)
            {
                LogEditBox.AppendText(string.Format("初始化程序时产生异常：{0}\r\n", ex.Message));
                this.Close();
            }
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            dotNetBarManager1.SaveLayout(Path.Combine(Application.StartupPath, "_flowchart_layout.xml")); // 保存布局设置
            documentManager.SaveUserConfigInfo();

            if(documentManager.CheckAndSaveFlowCharts()) // 用户取消保存
            {
                e.Cancel = true;
            }
        }

        /// <summary>
        /// 主窗口关闭事件处理
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            documentManager.RecordLog();
        }                

        /// <summary>
        /// tab页切换事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void tabControl1_SelectedTabChanged(object sender, TabStripTabChangedEventArgs e)
        {
            documentManager.SwitchPanel();
        }     

        /// <summary>
        /// 鼠标离开预览控件事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void abbreviateCanvas_MouseLeave(object sender, EventArgs e)
        {
            abbreviateCanvas.EndMove();
        }

        /// <summary>
        /// 鼠标在预览控件上移动事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void abbreviateCanvas_MouseMove(object sender, MouseEventArgs e)
        {
            abbreviateCanvas.MoveRegion(e.Location);
        }

        /// <summary>
        /// 鼠标在预览控件上按下事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void abbreviateCanvas_MouseDown(object sender, MouseEventArgs e)
        {
            abbreviateCanvas.SelectRegion(e.Location);
        }

        /// <summary>
        /// 鼠标在预览控件上放开事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void abbreviateCanvas_MouseUp(object sender, MouseEventArgs e)
        {
            abbreviateCanvas.EndMove(e.Location);
        }        

        /// <summary>
        /// 新建分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bNewClass_Click(object sender, EventArgs e)
        {
            documentManager.AddClass();
        }

        /// <summary>
        /// 新建绘图
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bNewFlowChart_Click(object sender, EventArgs e)
        {
            documentManager.AddFlowChart();
        }

        /// <summary>
        /// 剪切树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bCutNode_Click(object sender, EventArgs e)
        {
            documentManager.CutNode();
        }

        /// <summary>
        /// 复制树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bCopyNode_Click(object sender, EventArgs e)
        {
            documentManager.CopyNode();
        }

        /// <summary>
        /// 粘贴树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bPasteNode_Click(object sender, EventArgs e)
        {
            documentManager.PasteNode();
        }

        /// <summary>
        /// 删除树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bDeleteNode_Click(object sender, EventArgs e)
        {
            documentManager.DeleteNode();
        }

        /// <summary>
        /// 重命名树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bRenameNode_Click(object sender, EventArgs e)
        {
            documentManager.RenameNode();
        }

        /// <summary>
        /// 关闭绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void tabControl1_TabItemClose(object sender, TabStripActionEventArgs e)
        {
            documentManager.CheckAndSaveFlowChart();
        }      

        /// <summary>
        /// 保存数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bSaveFlowChart_Click(object sender, EventArgs e)
        {
            // 保存当前绘图
            documentManager.SaveCurrentFlowChart();           
        }

        /// <summary>
        /// 撤销操作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bUndoCommand_Click(object sender, EventArgs e)
        {
            documentManager.Undo();
        }

        /// <summary>
        /// 重做操作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bRedoCommand_Click(object sender, EventArgs e)
        {            
            documentManager.Redo();
        }
        
        /// <summary>
        /// 全选当前图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSelectAddGraphElement_Click(object sender, EventArgs e)
        {
            documentManager.SelectAllGraphElement();
        }

        /// <summary>
        /// 属性面板的参数发生改变
        /// </summary>
        /// <param name="s">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            ;
        }

        /// <summary>
        /// 检测绘图的有效性
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCheckCircle_Click(object sender, EventArgs e)
        {
            documentManager.CheckFlowChartAvail();
        }

        /// <summary>
        /// 设置程序参数
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSetArgument_Click(object sender, EventArgs e)
        {
            documentManager.SetArgument();
        }

        /// <summary>
        /// 生成当前流程图的代码
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bBuildFlowChart_Click(object sender, EventArgs e)
        {
            documentManager.GenerateCode();
        }

        /// <summary>
        /// 打开目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOpenFolder_Click(object sender, EventArgs e)
        {
            documentManager.OpenFolder();
        }

        /// <summary>
        /// 剪切图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCutFlowChart_Click(object sender, EventArgs e)
        {
            documentManager.CutGraphElement();
        }

        /// <summary>
        /// 查看代码
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewCode_Click(object sender, EventArgs e)
        {
            documentManager.ViewCode();
        }

        /// <summary>
        /// 切换分类树
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tabControl3_SelectedTabChanged(object sender, TabStripTabChangedEventArgs e)
        {
            documentManager.SwitchClass();
        }

        /// <summary>
        /// 变量管理
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bEditVar_Click(object sender, EventArgs e)
        {
            documentManager.EditVar();
        }

        /// <summary>
        /// 复制图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCopyGraphElements_Click(object sender, EventArgs e)
        {
            documentManager.CopyGraphElement();
        }

        /// <summary>
        /// 剪切图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCutGraphElements_Click(object sender, EventArgs e)
        {
            documentManager.CutGraphElement();
        }

        /// <summary>
        /// 粘贴图元
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bPasteGraphElements_Click(object sender, EventArgs e)
        {
            documentManager.PasteGraphElement();
        }

        /// <summary>
        /// 改变绘图板的大小
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxItem1_SelectedIndexChanged(object sender, EventArgs e)
        {
            /* 因为支持的并不是很完善，暂时屏蔽掉缩放功能
            float scaleArgument = 1.0f;            
            bool setMaxSize = false;
            
            switch (canvasScaleBox.SelectedIndex)
            {
                case 0: // 25%
                    {
                        scaleArgument = 0.25f;
                        break;
                    }
                case 1: // 50%
                    {
                        scaleArgument = 0.5f;
                        break;
                    }
                case 2: // 75%
                    {
                        scaleArgument = 0.75f;
                        break;
                    }
                case 3: // 100%
                    {
                        scaleArgument = 1.0f;
                        break;
                    }
                case 4: // 最大化
                    {

                        setMaxSize = true;
                        break;
                    }
            }
            
            if (setMaxSize)
            {
                groupPanel1.Visible = false;
                barLog.Visible = false;
                documentManager.ChangeCanvasScale(1.0f);
                // documentManager.MaximizeCanvas();
            }
            else
            {
                groupPanel1.Visible = true;
                barLog.Visible = true;
                documentManager.ChangeCanvasScale(scaleArgument);
            }
            */

            switch (canvasScaleBox.SelectedIndex)
            {
                case 0: // 正常大小
                    {
                        groupPanel1.Visible = true;
                        barLog.Visible = true;
                        
                        break;
                    }
                case 1: // 最大化
                    {
                        groupPanel1.Visible = false;
                        barLog.Visible = false;

                        break;
                    }
            }                        
        }

        /// <summary>
        /// 查看日志
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewLog_Click(object sender, EventArgs e)
        {
            documentManager.ViewLog();
        }

        /// <summary>
        /// 恢复绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRestoreNode_Click(object sender, EventArgs e)
        {
            documentManager.RestoreDiagram();
        }

        /// <summary>
        /// 查看程序信息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAbout_Click(object sender, EventArgs e)
        {
            MessageBox.Show(string.Format("绘图脚本生成工具 {0} 版\r\n\r\n剑网三工具组制作", Application.ProductVersion), "程序信息", 
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /// <summary>
        /// 退出程序
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void ExitApplication_Click(object sender, EventArgs e)
        {
            this.Close();
        }  

        /// <summary>
        /// 打开最近打开的绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void OpenLatestDiagram(object sender, EventArgs e)
        {
            ButtonItem buttonItem = sender as ButtonItem;
            documentManager.OpenDiagram(buttonItem.Tag.ToString());
        }

        /// <summary>
        /// 查看帮助文档
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bHelpDoc_Click(object sender, EventArgs e)
        {
            documentManager.ViewHelpDoc();
        }

        /// <summary>
        /// 编辑属性
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bEditProperty_Click(object sender, EventArgs e)
        {
            documentManager.ViewChartProperty();
        }

        /// <summary>
        /// 查找编辑
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdFindRecord_Click(object sender, EventArgs e)
        {
            documentManager.BeginSearch();
        }

        /// <summary>
        /// 切换命令支持
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCommandSwitch_Click(object sender, EventArgs e)
        {
            documentManager.SwitchCommandState();
        }

        /// <summary>
        /// 绘图数据保存到本地文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSaveToFile_Click(object sender, EventArgs e)
        {
            documentManager.SaveDiagramDataToFile();
        }

        /// <summary>
        /// 从本地文件加载绘图数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e"></param>
        private void bLoadFromFile_Click(object sender, EventArgs e)
        {
            documentManager.LoadDiagramDataFromFile();
        }

        /// <summary>
        /// 恢复绘图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRestoreDiagram_Click(object sender, EventArgs e)
        {
            documentManager.ViewRecycle();
        }

        /// <summary>
        /// 查看子绘图引用关系
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewInnerChartReference_Click(object sender, EventArgs e)
        {
            documentManager.ViewInnerChartReference();
        }

        /// <summary>
        /// 数据迁移
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bTransferData_Click(object sender, EventArgs e)
        {
            documentManager.TransferData();
        }
    }

    /// <summary>
    /// 文档处理接口
    /// </summary>
    public interface IKDocument
    {
        void ExecCommand(string strCmd, object data);
    }
}