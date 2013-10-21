using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using DataBaseIO;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Text.RegularExpressions;

using LuaInterface;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using Fireball.Syntax;
using Fireball.Windows.Forms.CodeEditor;

namespace GameLuaEditor
{
    public partial class MainForm : Form
    {
        private ExportForm exportForm = null;
        public static MainForm CurrentMainForm = null;

        private Lua m_lua = new Lua();
        public TreeView tree = null;
        private SqlConnection Conn;
        private bool startDebug = false; // 是否已经开始debug   
        private bool lockVarValue = false; // 是否允许修改变量值
        private bool enableFetchWord = true; // 是否允许取词
        private bool mouseFetchWord = true; // 是否开启鼠标取词
        private bool enableServerDebugOutput = false; // 是否允许服务端输出调试信息
        private string currentDebugFile = null; // 当前调试的文件
        private List<ButtonItem> debugButtonList = new List<ButtonItem>(); // debug相关的按钮
        private NetworkManager networkManager = null; // 网络管理器实例
        private System.Windows.Forms.Timer debugWaitTimer = new System.Windows.Forms.Timer(); // 调试等待用的计时器        

        private enum DebugState { None, WaitBreakPointActivated, WaitStartDebugActivated, WaitStepDebugActivated }; // 调试状态
        private DebugState debugState = DebugState.None; // 当前的调试状态 

        private enum DebugMode { None, Client, Server }; // 调试的模式
        private DebugMode debugMode = DebugMode.None;

        private StringReplaceForm stringReplaceForm; // 字符串替换窗体
        private List<string> localFileList; // 本地文件链表

        private TreeNode localFileRootNode; // 本地脚本根节点

        /// <summary>
        /// 构造函数
        /// </summary>
        public MainForm()
        {
            InitializeComponent();            
            Init();                        
        }        

        /// <summary>
        /// 获取数据行
        /// </summary>
        /// <param name="tablename">数据表名</param>
        /// <param name="keys">数据表主键</param>
        /// <param name="ltReturn">结果数据表</param>
        /// <returns>是否获取成功</returns>
        public bool GetDataTableRow(string tablename, LuaTable keys, ref LuaTable ltReturn)
        {
            string sql = string.Format("SELECT * FROM {0}", tablename);
            DataTable tbl = Helper.GetDataTable(sql, Conn);

            if (tbl.PrimaryKey.Length != keys.Keys.Count)
            {
                MessageBox.Show(string.Format("{0} —— 函数GetDataTableRow断言：{1}\r\n", DateTime.Now, "输入参数keys长度错误。"));
            }

            object[] KEYS = new object[tbl.PrimaryKey.Length];

            for (int i = 0; i < KEYS.Length; i++)
            {
                KEYS[i] = ((LuaTable)keys)[i];
            }

            DataRow row = tbl.Rows.Find(KEYS);

            if (row == null)
            {
                return false;
            }

            foreach (DataColumn col in tbl.Columns)
            {
                ltReturn[col.ColumnName] = (row[col.ColumnName] == DBNull.Value ? null : row[col.ColumnName]);
            }

            return true;
        }

        /// <summary>
        /// 设置配置项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnSetup_Click(object sender, EventArgs e)
        {
            SettingForm settingForm = new SettingForm();
            settingForm.CurrentUpdateScriptFont = new SettingForm.UpdateScriptFont(UpdateScriptFont);
            settingForm.CurrentUpdateScriptForeColor = new SettingForm.UpdateScriptForeColor(UpdateScriptForeColor);
            settingForm.ShowDialog();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            tree = treePh;
            this.m_lua["this"] = this;
            tabStrip1.MdiForm = this;            

            // 读取策划工具的配置信息
            Helper.ClientPath = IniIO.IniReadValue("General", "RootDir", "./GameDesingerTools.ini");
            string filename = Application.StartupPath + "/GameDesingerTools_Public.ini";
            string content = FileFolderHelper.FileToString(filename);
            SymmetricMethod sm = new SymmetricMethod();
            content = sm.Decrypto(content);
            IniStructure m_inis = new IniStructure();
            m_inis = IniStructure.ReadIniWithContent(content);
            Helper.ConnectionString = m_inis.GetValue("General", "ConnString");

            string configFileName = Path.Combine(Application.StartupPath, "LuaEditor.ini");

            // 读取本机配置信息
            if (File.Exists(configFileName))
            {
                string serverPath = IniIO.IniReadValue("General", "ServerFolder", configFileName);

                if (!string.IsNullOrEmpty(serverPath))
                {
                    Helper.ServerPath = serverPath;
                }

                /* 调试相关的参数暂时不允许自定义
                string receiveMessageDelay = IniIO.IniReadValue("General", "ReceiveMessageDelay", configFileName);
                
                if (!string.IsNullOrEmpty(receiveMessageDelay))
                {
                    Helper.ReceiveMessageDelay = int.Parse(receiveMessageDelay);
                }

                string receiveMessageRetryTime = IniIO.IniReadValue("General", "ReceiveMessageRetryTime", configFileName);

                if (!string.IsNullOrEmpty(receiveMessageRetryTime))
                {
                    Helper.ReceiveMessageRetryTime = int.Parse(receiveMessageRetryTime);
                }

                string breakPointWaitCircle = IniIO.IniReadValue("General", "BreakPointWaitCircle", configFileName);

                if (!string.IsNullOrEmpty(breakPointWaitCircle))
                {
                    Helper.BreakPointWaitCircle = int.Parse(breakPointWaitCircle);
                }
                */ 

                string serverIP = IniIO.IniReadValue("General", "ServerIP", configFileName);

                if (!string.IsNullOrEmpty(serverIP))
                {
                    Helper.ServerIP = serverIP;
                }

                string fontName = IniIO.IniReadValue("General", "ScriptFontName", configFileName);
                string fontSize = IniIO.IniReadValue("General", "ScriptFontSize", configFileName);

                if (!string.IsNullOrEmpty(fontName) && !string.IsNullOrEmpty(fontSize))
                {
                    Helper.ScriptFont = new Font(fontName, float.Parse(fontSize), FontStyle.Regular);
                }

                string fontColor = IniIO.IniReadValue("General", "ScriptForeColor", configFileName);

                if (!string.IsNullOrEmpty(fontColor))
                {
                    Helper.ScriptForeColor = Color.FromName(fontColor);
                }                
            }

            // 读取_setting.cfg
            string strCfgFile = System.IO.Directory.GetCurrentDirectory().TrimEnd(new char[] { '\\' });
            strCfgFile += @"\Plugins\LuaCheck\_setting.cfg";
            Helper.GameLuaEditorSetting = Helper.FileToString(strCfgFile);

            // 如果setting读不到，就取默认setting
            if (Helper.GameLuaEditorSetting == "")
            {
                string strFolder = System.IO.Directory.GetCurrentDirectory().TrimEnd(new char[] { '\\' });
                string strCfg = Helper.FileToString(strFolder + @"\Plugins\LuaCheck\setting.cfg");
                // strCfg = "哈哈,int,123,|是否允许开启,list,false;true,|";
                string[] as_item = strCfg.Split(new string[] { ",", "|" }, StringSplitOptions.None);
                string strUserCfg = "";
                for (int i = 0; i < as_item.Length - 1; i += 4)
                {
                    strUserCfg += as_item[i + 2].Split(new char[] { ';' })[0] + ",";
                }
                Helper.GameLuaEditorSetting = strUserCfg.TrimEnd(new char[]{','});
            }

            // 注册debug相关的按钮
            debugButtonList.Add(bStepInto);
            debugButtonList.Add(bContinueDebug);
            debugButtonList.Add(bRefreshVar);
            debugButtonList.Add(bStepOver);

            // 清理锁
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            dbm.Init(Helper.ConnectionString);
            dbm.ClearRecordLocks();
            Conn = dbm.conn;

            bar4.Items[1].Text = dbm.GetDataBaseName();
            bar4.Items[2].Text = dbm.GetHostName();

            // 生成数据库脚本树
            LoadDataBaseScriptTree();

            // 生成本地脚本树
            LoadLocalScriptTree();

            this.m_lua.RegisterFunction("GetDataTableRow", this, typeof(MainForm).GetMethod("GetDataTableRow"));
            CurrentMainForm = this;

            // 初始化调试计时器
            debugWaitTimer.Tick += new EventHandler(OnTimerExpired);
        }

        /// <summary>
        /// 打印调用栈
        /// </summary>
        private void RefreshCallStack()
        {
            callStackView.Rows.Clear();

            string message = "print_call_stack";
            string receivedMessage = SendAndWaitMessage(message, "<ldb>print_call_stack:", true);
            
            if (receivedMessage == null)
            {
                callStackView.Rows.Add(1);
                DataGridViewRow newRow = callStackView.Rows[0];

                newRow.Cells["CallStackLevel"].Value = "1";
                newRow.Cells["CallStackFileName"].Value = "接收消息超时";
                newRow.Cells["CallStackLineIndex"].Value = "接收消息超时";
                newRow.Cells["CallStackFunctionName"].Value = "接收消息超时";
            }
            else
            {
                receivedMessage = receivedMessage.Replace("<ldb>print_call_stack: ", "");

                string[] lines = receivedMessage.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                for (int i = 0; i < lines.Length; i++)
                {
                    string line = lines[i];
                    int callLevel = i + 1;
                    string[] info = line.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                    callStackView.Rows.Add(1);
                    DataGridViewRow newRow = callStackView.Rows[callStackView.Rows.Count - 1];

                    newRow.Cells["CallStackLevel"].Value = callLevel.ToString();
                    newRow.Cells["CallStackFileName"].Value = info[0];
                    newRow.Cells["CallStackLineIndex"].Value = info[1];
                    newRow.Cells["CallStackFunctionName"].Value = info[2];
                }
            }            
        }

        /// <summary>
        /// 检查调试所需的文件是否已经存在
        /// </summary>
        /// <returns>调试所需文件是否已经存在</returns>
        private bool CheckDebugFileReady()
        {
            if (Helper.ClientPath == "")
            {
                MessageBox.Show("请先设置客户端所在目录！", "开始调试", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (Helper.ServerPath == "")
            {
                MessageBox.Show("请先设置服务端所在目录！", "开始调试", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            List<string> srcFileList = new List<string>();

            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Engine_Lua5.dll"));
            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Engine_Lua5D.dll"));
            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\Lua5D.dll"));
            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\udpsocket.dll"));
            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\ldb.lua"));
            srcFileList.Add(Path.Combine(Application.StartupPath, @"Plugins\LuaDebug\udpsocket.dll"));

            List<string> destFileList = new List<string>();

            destFileList.Add(Path.Combine(Helper.ServerPath, "Engine_Lua5.dll"));
            destFileList.Add(Path.Combine(Helper.ServerPath, "Engine_Lua5D.dll"));
            destFileList.Add(Path.Combine(Helper.ServerPath, "Lua5D.dll"));
            destFileList.Add(Path.Combine(Helper.ServerPath, "udpsocket.dll"));
            destFileList.Add(Path.Combine(Helper.ClientPath, @"scripts\ldb.lua"));
            destFileList.Add(Path.Combine(Helper.ClientPath, "udpsocket.dll"));

            for (int i = 0; i < srcFileList.Count; i++)
            {
                string srcFileName = srcFileList[i];
                string destFileName = destFileList[i];

                if (!File.Exists(destFileName))
                {
                    MessageBox.Show(string.Format("文件{0}不存在！", destFileName), "开始调试", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return false;
                }

                DateTime srcFileLastWriteTime = File.GetLastWriteTime(srcFileName);
                DateTime destFileLastWriteTime = File.GetLastWriteTime(destFileName);

                if (srcFileLastWriteTime > destFileLastWriteTime)
                {
                    MessageBox.Show(string.Format("文件{0}的版本太旧！", destFileName), "开始调试", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 计时器事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void OnTimerExpired(object sender,  EventArgs e)
        {
            switch (debugState)
            {
                case DebugState.None:
                    {
                        debugWaitTimer.Stop();
                        break;
                    }
                case DebugState.WaitBreakPointActivated:
                    {
                        WaitBreakPointActivated(sender, e);
                        break;
                    }
                case DebugState.WaitStartDebugActivated:
                    {
                        WaitStartDebugActivated(sender, e);
                        break;
                    }
                case DebugState.WaitStepDebugActivated:
                    {
                        WaitStepDebugActivated(sender, e);
                        break;
                    }
            }
        }

        /// <summary>
        /// 添加本地文件
        /// </summary>
        /// <param name="folderName">文件夹名称</param>
        private void AddLocalFile(string folderName)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = folderName;
            openFileDialog.Title = "请选择要加载的脚本文件";
            openFileDialog.Filter = "脚本文件|*.lua";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string fileName = openFileDialog.FileName;

                AddLocalFileNode(fileName);
                OpenFileInEditor(fileName, false);                
            }
        }

        /// <summary>
        /// 等待单步调试消息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void WaitStepDebugActivated(object sender, EventArgs e)
        {
            // 开启调试状态
            string receivedMessage = null;          

            if (networkManager.HasReceivedMessage)
            {
                // 检查是否是单步消息
                receivedMessage = networkManager.GetMessageForHead("<ldb>step_at:");

                if (receivedMessage == null)
                {
                    // 检查是否是断点消息
                    receivedMessage = networkManager.GetMessageForHead("<ldb>break_at:");
                }
            }            

            if (receivedMessage != null)
            {
                // 先暂停当前计时器
                StopDebugWaitTimer();

                PrintOutputText(receivedMessage);
                object[] stepInfo = GetJumpInfoFromMessage(receivedMessage);

                if (stepInfo != null)
                {
                    string fileName = stepInfo[0] as string;
                    int lineIndex = (int)stepInfo[1];
                    EditForm editForm = tabStrip1.SelectedTab.AttachedControl as EditForm;

                    if (ProcessFileName(editForm.FileName) == fileName)
                    {
                        editForm.luaEditBox.DebugRowIndex = lineIndex - 1;
                        editForm.luaEditBox.MoveToLine(lineIndex);
                    }
                    else
                    {
                        editForm.luaEditBox.ClearDebugMark();

                        // 在编辑窗体中打开跳转到的脚本文件
                        string filePath = Path.Combine(Helper.ClientPath, fileName);
                        EditForm stepForm = OpenFileInEditor(filePath, true);

                        // 高亮当前行号
                        if (stepForm != null)
                        {
                            stepForm.luaEditBox.DebugRowIndex = lineIndex - 1;
                            stepForm.luaEditBox.MoveToLine(lineIndex);
                            stepForm.luaEditBox.Refresh();
                        }
                    }

                    currentDebugFile = fileName;
                    editForm.luaEditBox.Refresh();
                }

                // 刷新一次变量值
                RefreshVarValue();

                // 刷新调用栈的信息
                RefreshCallStack();

                // 清掉接收到的所有消息
                networkManager.ClearReceivedMessageList();

                // 开启取词功能
                enableFetchWord = true;

                // 清掉超时计数
                debugWaitTimer.Tag = 0;
            }
            else
            {                
                int waitCount = 0;

                if (debugWaitTimer.Tag != null)
                {
                    waitCount = (int)debugWaitTimer.Tag;
                }

                waitCount++;

                if (waitCount > 10)
                {
                    waitCount = 0;
                    StopDebugWaitTimer();                    

                    MessageBox.Show("等待单步消息超时，可能当前调试行已离开当前作用域！", "单步调试", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);         
         
                    ContinueDebug();
                }
                
                debugWaitTimer.Tag = waitCount;
            }
        }

        /// <summary>
        /// 等待开始调试确认的调试消息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void WaitStartDebugActivated(object sender, EventArgs e)
        {
            // 开启调试状态
            string message = "start_debug";
            string receivedMessage = SendAndWaitMessage(message, "<ldb>start_debug:", true);

            if (receivedMessage == "<ldb>start_debug: success!") // 开始调试成功
            {
                // 停掉当前计时器
                StopDebugWaitTimer();

                PrintOutputText("开始调试成功...");

                // 清掉所有消息
                networkManager.ClearReceivedMessageList();

                // 注册断点
                RegistBreakPoint();

                // 切换状态
                startDebug = true;
                bStartDebug.Enabled = false;
                bStartDebugClient.Enabled = false;
                bStopDebug.Enabled = true;

                foreach (ButtonItem buttonItem in debugButtonList)
                {
                    buttonItem.Enabled = true;
                }

                // 锁定所有已打开的窗体
                foreach (EditForm editForm in this.MdiChildren)
                {
                    editForm.luaEditBox.ReadOnly = true;
                    editForm.Text = string.Format("{0} [已锁定]", editForm.Text);
                }                

                // 等待服务端触发断点
                PrintOutputText("开始等待触发断点...");
                StartDebugWaitTimer(Helper.BreakPointWaitCircle, DebugState.WaitBreakPointActivated);
            }
            else if (receivedMessage == "<ldb>start_debug: failed!") // 开始调试失败
            {
                PrintOutputText("开始调试失败...");
                MessageBox.Show("注册调试信息失败，已经有别的主机连接并进行脚本调试！", "开始调试",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 等待断点触发的调试消息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void WaitBreakPointActivated(object sender, EventArgs e)
        {
            string receivedMessage = networkManager.GetMessageForHead("<ldb>break_at:");

            if (receivedMessage != null)
            {
                // 先暂停当前计时器
                StopDebugWaitTimer();

                PrintOutputText(receivedMessage);

                object[] stepInfo = GetJumpInfoFromMessage(receivedMessage);

                if (stepInfo != null)
                {
                    string fileName = stepInfo[0] as string;
                    int lineIndex = (int)stepInfo[1];

                    // 自动切到当前脚本 
                    foreach (EditForm editForm in this.MdiChildren)
                    {
                        if (ProcessFileName(editForm.FileName) == fileName)
                        {
                            SelectEditForm(editForm);

                            // 高亮当前行号
                            editForm.luaEditBox.DebugRowIndex = lineIndex - 1;
                            editForm.luaEditBox.MoveToLine(lineIndex);
                            editForm.luaEditBox.Refresh();
                            break;
                        }
                    }

                    currentDebugFile = fileName;

                    // 刷新一次变量值
                    RefreshVarValue();

                    // 刷新调用栈信息
                    RefreshCallStack();

                    // 清掉所有接收的消息
                    networkManager.ClearReceivedMessageList();
                }
            }
        }

        /// <summary>
        /// 编辑控件按键事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void EditForm_KeyDown(object sender, KeyEventArgs e)
        {            
            switch (e.KeyCode)
            {
                case Keys.F10: // 单步跳过
                    {
                        if (startDebug)
                        {
                            StepDebug("单步跳过", "step_over");
                        }                       
                        
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }

        /// <summary>
        /// 加载本地脚本树
        /// </summary>
        private void LoadLocalScriptTree()
        {
            // 读取配置文件
            List<string> fileNameList = LoadLocalFileList();
            localFileList = fileNameList;

            // 加载数据库脚本
            TreePhOpera tpo = new TreePhOpera();
            Hashtable infoTable = new Hashtable();

            DataTable treeTable = new DataTable();
            DataColumn idColumn = new DataColumn("id");
            DataColumn pathColumn = new DataColumn("path");

            treeTable.Columns.Add(idColumn);
            treeTable.Columns.Add(pathColumn);

            foreach (string s in fileNameList)
            {
                DataRow newRow = treeTable.NewRow();
                newRow["id"] = "0";
                newRow["path"] = s;
                treeTable.Rows.Add(newRow);
            }

            infoTable["dtable"] = treeTable;

            ArrayList icoList = new ArrayList();
            icoList.Add("folder");
            icoList.Add("file");

            infoTable["icolist"] = icoList;
            infoTable["pop_folder"] = popTreePh_LocalFolder;
            infoTable["pop_file"] = popTreePh_LocalFile;
            infoTable["scriptType"] = "localFile";

            foreach (TreeNode tn in tpo.LoadTreeNodeCollection(infoTable))
            {
                localFileRootNode.Nodes.Add(tn);
                tn.Expand();
            }         
        }

        /// <summary>
        /// 加载数据库脚本树
        /// </summary>
        private void LoadDataBaseScriptTree()
        {
            treePh.Nodes.Clear();

            // 加载数据库脚本
            TreePhOpera tpo = new TreePhOpera();
            Hashtable htable = new Hashtable();
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            
            treePh.Tag = dbm.GetScriptInformation();
            htable["dtable"] = dbm.GetScriptInformation();                      
            htable["pop_folder"] = popTreePh_Folder;
            htable["pop_file"] = popTreePh_File;
            htable["scriptType"] = "databaseFile";

            foreach (TreeNode tn in tpo.LoadTreeNodeCollection(htable))
            {
                treePh.Nodes.Add(tn);
                tn.Expand();
            }                 
     
            // 增加本地文件结点
            Hashtable infoTable = new Hashtable();
            infoTable["type"] = "folder";
            localFileRootNode = TreePhOpera.CreateNode("本地文件", "folder", popTreePh_LocalFileRoot, infoTable);

            treePh.Nodes.Add(localFileRootNode);  
        }

        /// <summary>
        /// 选中编辑控件
        /// </summary>
        /// <param name="editForm">编辑控件</param>
        private void SelectEditForm(EditForm editForm)
        {           
            foreach (DevComponents.DotNetBar.TabItem tabItem in this.tabStrip1.Tabs)
            {
                if (tabItem.AttachedControl == editForm)
                {
                    tabStrip1.SelectedTab = tabItem;
                    break;
                }
            }

            editForm.Activate();
            editForm.Focus();
        }

        /// <summary>
        /// 在编辑窗体中打开脚本文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="requireProcessFileName">是否需要处理文件名</param>
        /// <returns>是否打开成功</returns>
        private EditForm OpenFileInEditor(string fileName, bool requireProcessFileName)
        {
            EditForm editForm = null;    
            string filePath = fileName.Replace(Helper.ClientPath, "").TrimStart(new char[] { '\\', '/' });

            foreach (EditForm eForm in this.MdiChildren)
            {
                string editorFileName = eForm.FileName;

                if (requireProcessFileName)
                {
                    editorFileName = ProcessFileName(editorFileName);
                }

                if (editorFileName == filePath)
                {
                    SelectEditForm(eForm);
                    return eForm;                    
                }
            }

            string content = Helper.FileToString(fileName);

            if (!string.IsNullOrEmpty(content))
            {
                editForm = new EditForm("localFile");
                editForm.ID = "0";

                // InitEditForm(editForm, content, filePath);                
                InitEditForm(editForm, content, fileName);
                editForm.Show();
                SelectEditForm(editForm);
            }

            return editForm;
        }

        /// <summary>
        /// 打开对应文件名的脚本
        /// </summary>
        /// <param name="filename">文件名</param>
        /// <returns>对应脚本是否存在</returns>
        public bool OpenFileByName(string filename)
        {
            if (SelectNodeByFileName(filename))
            {
                treePh_NodeMouseDoubleClick(this, new TreeNodeMouseClickEventArgs(treePh.SelectedNode, MouseButtons.Left, 2, 0, 0));
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 打开对应文件名的脚本
        /// </summary>
        /// <param name="filename">文件名</param>
        /// <returns>是否打开成功</returns>
        public bool SelectNodeByFileName(string filename)
        {
            return _SelectNodeByFileName(treePh.Nodes, filename);
        }

        /// <summary>
        /// 打开对应文件名的脚本
        /// </summary>
        /// <param name="tnc">树结点集合</param>
        /// <param name="filename">文件名</param>
        /// <returns>是否打开成功</returns>
        private bool _SelectNodeByFileName(TreeNodeCollection tnc, string filename)
        {
            bool bret = false;

            foreach (TreeNode tn in tnc)
            {
                if(tn.Nodes.Count > 0)
                {
                    bret = _SelectNodeByFileName(tn.Nodes, filename);

                    if (bret)
                    {
                        return true;
                    }
                }
                else
                {
                    if (tn.FullPath == filename)
                    {
                        treePh.SelectedNode = tn;
                        return true;
                    }
                }
            }

            return bret;
        }

        // ahpho
        public void OpenTreeNodeScript(TreeNode node)
        {
            TreeNodeMouseClickEventArgs e = new TreeNodeMouseClickEventArgs(node, MouseButtons.Left, 2, 0, 0);
            treePh_NodeMouseDoubleClick(null, e);
        }

        public string GetTreeNodeScriptContent(TreeNode node)
        {
            if (node.Tag == null)
                return string.Empty;

            Hashtable ht = node.Tag as Hashtable;
            if (ht["type"].ToString() == "folder")
                return string.Empty;

            string id = ht["id"] as string;
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            string strCode = dbm.GetScriptData(id);

            if (strCode != null)
            {
                return strCode;
            }
            return string.Empty;
        }

        /// <summary>
        /// 检查本地文件树有没有改变过
        /// </summary>
        private bool CheckLocalFileTreeChanged()
        {
            bool changed = false;
            List<string> fileNameList = new List<string>();

            InitLocalFileNameList(localFileRootNode, fileNameList);

            if (fileNameList.Count == localFileList.Count)
            {
                foreach (string s in localFileList)
                {
                    if (!fileNameList.Contains(s))
                    {
                        changed = true;
                        break;
                    }
                }
            }
            else
            {
                changed = true;
            }

            return changed;
        }

        /// <summary>
        /// 保存本地的文件链表
        /// </summary>
        private void SaveLocalFileList()
        {
            List<string> fileNameList = new List<string>();

            InitLocalFileNameList(localFileRootNode, fileNameList);

            if (fileNameList.Count > 0)
            {
                string fileName = Path.Combine(Application.StartupPath, "LuaEditor.ini");

                IniIO.IniWriteValue("General", "LocalFileCount", fileNameList.Count.ToString(), fileName);

                for (int i = 0; i < fileNameList.Count; i++)
                {
                    IniIO.IniWriteValue("LocalFile", string.Format("File{0}", i), fileNameList[i], fileName);
                }                
            }
        }

        /// <summary>
        /// 加载本地的文件链表
        /// </summary>
        /// <returns>文件链表</returns>
        private List<string> LoadLocalFileList()
        {
            List<string> fileNameList = new List<string>();
            string fileName = Path.Combine(Application.StartupPath, "LuaEditor.ini");

            string fileCountString = IniIO.IniReadValue("General", "LocalFileCount", fileName);

            if (!string.IsNullOrEmpty(fileCountString))
            {
                int fileCount = int.Parse(fileCountString);

                for (int i = 0; i < fileCount; i++)
                {
                    string localFileName = IniIO.IniReadValue("LocalFile", string.Format("File{0}", i), fileName);
                    fileNameList.Add(localFileName);
                }
            }

            return fileNameList;
        }

        /// <summary>
        /// 初始化本地文件名链表
        /// </summary>
        /// <param name="treeNode"></param>
        /// <param name="fileNameList"></param>
        private void InitLocalFileNameList(TreeNode treeNode, List<string> fileNameList)
        {
            Hashtable infoTable = treeNode.Tag as Hashtable;

            if (infoTable["type"] as string == "file")
            {
                fileNameList.Add(treeNode.FullPath.Replace("本地文件\\", ""));
            }
            else
            {
                foreach (TreeNode node in treeNode.Nodes)
                {
                    InitLocalFileNameList(node, fileNameList);
                }
            }            
        }

        /// <summary>
        /// 转换文件名
        /// </summary>
        /// <param name="fileName">旧文件名</param>
        /// <returns>新文件名</returns>
        private string ProcessFileName(string fileName)
        {
            string newFileName = fileName.ToLower().Replace("\\", "/");

            return newFileName;
        }

        /// <summary>
        /// 初始化编辑窗体
        /// </summary>
        /// <param name="editForm">编辑窗体</param>
        /// <param name="scriptCode">脚本内容</param>
        /// <param name="scriptPath">脚本路径</param>
        private void InitEditForm(EditForm editForm, string scriptCode, string scriptPath)
        {
            editForm.ScriptCode = scriptCode;
            editForm.WindowState = FormWindowState.Maximized;
            editForm.MdiParent = this;
            editForm.FileName = scriptPath;
            editForm.EditorContextMenu = popRight;
            editForm.luaEditBox.ScriptFont = Helper.ScriptFont;
            editForm.luaEditBox.ScriptForeColor = Helper.ScriptForeColor;
            editForm.HandleBreakPointAdded += new EditForm.BreakPointAdded(EditFormBreakPointAdded);
            editForm.HandleBreakPointRemoved += new EditForm.BreakPointRemoved(EditFormBreakPointRemoved);
            editForm.HandleWordMouseHover += new EditForm.WordMouseHover(EditFormWordMouseHover);
            editForm.luaEditBox.KeyDown += new KeyEventHandler(EditForm_KeyDown);
            editForm.luaEditBox.ReadOnly = startDebug; // 调试模式下的打开窗体只能只读的

            if (startDebug)
            {
                editForm.Text = string.Format("{0} [已锁定]", editForm.Text);
            }
        }

        /// <summary>
        /// 添加本地文件脚本结点
        /// </summary>
        /// <param name="fileName">文件名</param>
        private void AddLocalFileNode(string fileName)
        {
            fileName = fileName.Replace(Helper.ClientPath, "").TrimStart(new char[] { '\\' });
            string[] dataArray = fileName.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);

            TreeNode rootNode = localFileRootNode;
            TreeNode currentNode = null;

            for (int i = 0; i < dataArray.Length; i++)
            {
                string s = dataArray[i];
                currentNode = null;
                
                foreach (TreeNode childNode in rootNode.Nodes)
                {
                    if (childNode.Text == s)
                    {
                        currentNode = childNode;
                        rootNode = childNode;
                        break;
                    }
                }

                if (currentNode == null)
                {
                    Hashtable infoTable = new Hashtable();
                    ContextMenuStrip contextMenuStrip = null;
                    string nodeType = null;

                    if (i == dataArray.Length - 1) // 文件结点
                    {
                        nodeType = "file";
                        infoTable["type"] = "file";
                        infoTable["scriptType"] = "localFile";
                        infoTable["id"] = "0";
                        infoTable["filePath"] = fileName;
                    }
                    else // 文件夹结点
                    {
                        nodeType = "folder";
                        infoTable["type"] = nodeType;
                        contextMenuStrip = popTreePh_LocalFolder;
                    }                                        
                    
                    currentNode = TreePhOpera.CreateNode(s, nodeType, contextMenuStrip, infoTable);
                    rootNode.Nodes.Add(currentNode);
                    rootNode = currentNode;
                }
            }

            treePh.SelectedNode = currentNode;
        }

        /// <summary>
        /// 双击树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void treePh_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode.Tag == null)
            {
                return;
            }

            Hashtable infoTable = currentNode.Tag as Hashtable;

            if (infoTable["type"] as string == "folder")
            {
                return;
            }

            string scriptType = infoTable["scriptType"] as string;

            if (scriptType == "databaseFile") // 打开数据库里的脚本文件
            {
                string id = infoTable["id"] as string;
                EditForm editForm = null;

                foreach (EditForm eForm in this.MdiChildren)
                {
                    if (eForm.ID == id)
                    {
                        editForm = eForm;
                    }
                }

                if (editForm == null)
                {
                    editForm = new EditForm("databaseFile");
                    editForm.ID = id;
                    DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
                    string strCode = dbm.GetScriptData(id);

                    if (strCode == null)
                    {
                        MessageBox.Show("加载脚本失败！", "加载脚本", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }

                    InitEditForm(editForm, strCode, currentNode.FullPath);
                    editForm.Show();
                }

                SelectEditForm(editForm);
            }
            else if (scriptType == "localFile") // 打开本地的脚本文件
            {
                string filePath = infoTable["filePath"] as string;
                filePath = Path.Combine(Helper.ClientPath, filePath);
                OpenFileInEditor(filePath, false);
            }
        }

        /// <summary>
        /// 脚本分类树鼠标单击事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void treePh_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            // 刷新右键菜单状态
            if (e.Node != null)
            {
                // 先选中一次，避免右键菜单操作对象不正确的问题
                treePh.SelectedNode = e.Node;

                string scriptPath = e.Node.FullPath;                
                string[] data = scriptPath.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);

                cmdTreePhFolder_newFolder.Enabled = true;
                cmdTreePhFolder_newFile.Enabled = true;
                cmdTreePhFolder_rename.Enabled = true;
                cmdTreePhFolder_del.Enabled = true;

                if (data.Length > 1)
                {                    
                    if (data.Length == 2 && scriptPath == @"scripts\Map") // scripts\Maps目录下只能是场景文件夹
                    {
                        cmdTreePhFolder_newFile.Enabled = false;
                    }                    
                }
                else // 根目录不允许重命名，删除
                {
                    cmdTreePhFolder_rename.Enabled = false;
                    cmdTreePhFolder_del.Enabled = false;
                }                               
            }
        }
      
        /// <summary>
        /// 重命名文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFile_reanme_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                currentNode.BeginEdit();
            }
        }

        /// <summary>
        /// 删除脚本文件夹
        /// </summary>
        /// <param name="folderNode">脚本文件夹树结点</param>
        /// <return>是否删除成功</return>
        private bool DeleteScriptFolder(TreeNode folderNode)
        {
            List<TreeNode> childNodeList = new List<TreeNode>();

            foreach (TreeNode childNode in folderNode.Nodes)
            {
                childNodeList.Add(childNode);
            }

            foreach (TreeNode childNode in childNodeList)
            {
                Hashtable infoTable = childNode.Tag as Hashtable;

                if (infoTable["type"] as string == "folder")
                {
                    DeleteScriptFolder(childNode);
                }
                else if (infoTable["type"] as string == "file")
                {
                    DeleteScriptFile(childNode);
                }
            }

            folderNode.Remove();

            return true;
        }

        /// <summary>
        /// 删除脚本文件
        /// </summary>
        /// <param name="scriptNode">脚本文件树结点</param>
        /// <returns>是否删除成功</returns>
        private bool DeleteScriptFile(TreeNode fileNode)
        {
            bool deleteSuccess = false;
            
            Hashtable infoTable = fileNode.Tag as Hashtable;
            string id = infoTable["id"].ToString();
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();

            if (!dataBaseManager.IsScriptLocked(id))
            {
                if (dataBaseManager.DeleteScriptData(id)) // 现在数据库里删除
                {
                    fileNode.Remove();

                    // 修改成功，更新树缓存
                    treePh.Tag = dataBaseManager.GetScriptInformation();
                }
            }
            else
            {
                MessageBox.Show("该脚本已被锁定，无法删除！", "删除脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return deleteSuccess;
        }

        /// <summary>
        /// 删除文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFile_del_Click(object sender, EventArgs e)
        {            
            if (MessageBox.Show("确定要删除当前脚本吗？", "删除脚本", 
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)                
            {
                TreeNode currentNode = treePh.SelectedNode;

                if (currentNode != null)
                {
                    DeleteScriptFile(currentNode);
                }            
            }            
        }
        
        /// <summary>
        /// 导出文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFile_export_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;
                string id = infoTable["id"].ToString();
                List<string> scriptIDList = new List<string>();
                scriptIDList.Add(id);                

                exportForm = new ExportForm(scriptIDList, Helper.ClientPath);
                exportForm.ShowDialog();
            }            
        }

        #region 目录右键菜单

        /// <summary>
        /// 检查树结点是否已经存在
        /// </summary>
        /// <param name="parentNode">父结点</param>
        /// <param name="nodeName">结点名称</param>
        /// <returns></returns>
        private bool CheckNodeExist(TreeNode parentNode, string nodeName)
        {
            bool exist = false;

            foreach (TreeNode node in parentNode.Nodes)
            {
                if (node.Text == nodeName)
                {
                    exist = true;
                    break;
                }
            }

            return exist;
        }

        /// <summary>
        /// 新建目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_newFolder_Click(object sender, EventArgs e)
        {
            if (treePh.SelectedNode != null)
            {
                string strName = Helper.InputBox("新目录", "输入目录名");

                if (!string.IsNullOrEmpty(strName))
                {                    
                    if (CheckNodeExist(treePh.SelectedNode, strName))
                    {
                        MessageBox.Show("当前树结点已经存在！", "新建目录", 
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }

                    TreeNode newNode = treePh.SelectedNode.Nodes.Add(strName, strName);
                    Hashtable ht = new Hashtable();
                    ht["type"] = "folder";
                    newNode.Tag = ht;
                    newNode.ImageKey = newNode.SelectedImageKey = "folder";
                    newNode.ContextMenuStrip = this.popTreePh_Folder;
                    treePh.SelectedNode = newNode;
                }
            }                        
        }

        /// <summary>
        /// 新建文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_newFile_Click(object sender, EventArgs e)
        {
            if (treePh.SelectedNode != null)
            {
                // 开始和lua交互
                string strFullPath = treePh.SelectedNode.FullPath;
                Lua plua = new Lua();
                object[] oRet = null;
                
                plua.DoString(DataBaseManager.GetDataBaseManager().GetScriptData(this.getFileIDsFromPath("Template")[0].ToString()));
                LuaFunction fun = plua.GetFunction("LoadParms");

                if (fun != null)
                {
                    object[] args = { strFullPath };
                    oRet = fun.Call(args);

                    if (oRet == null)
                    {
                        return;
                    }
                }                            
                
                string strControls = oRet[0] as string;

                // 孙韬返回的内容
                object[] strParms;

                // 文件名
                string strName = "";
                string[] asp1 = { "|" };
                ModelForm mf = new ModelForm(strControls.Split(asp1, StringSplitOptions.RemoveEmptyEntries));

                if (mf.ShowDialog() == DialogResult.OK)
                {
                    List<string> list = mf.InputList;

                    if (list.Count >= 2)
                    {
                        strParms = list.ToArray();
                        strName = strParms[1] as string;

                        string strCode = "";
                        fun = plua.GetFunction("LoadScript");

                        if (fun != null)
                        {
                            oRet = fun.Call(strParms);
                            strCode = oRet[0] as string;
                        }
                        else
                        {
                            return;
                        }                

                        // 交互完毕
                        if (string.IsNullOrEmpty(strName))
                        {
                            return;
                        }

                        if (CheckNodeExist(treePh.SelectedNode, strName))
                        {
                            MessageBox.Show("当前树结点已经存在！", "新建文件",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                            return;
                        }

                        Hashtable infoTable = new Hashtable();
                        infoTable["type"] = "file";

                        // 存数据库
                        DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
                        string strPath = treePh.SelectedNode.FullPath;
                        string[] asp = { "\\" };
                        string[] as_Path = strPath.Split(asp, StringSplitOptions.RemoveEmptyEntries);

                        if (as_Path.Length < 2 && as_Path[0] == "scripts")
                        {
                            MessageBox.Show("暂时不支持在其他地图以外的目录建文件", "新建文件", 
                                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }

                        string id = dbm.CreateScriptData(string.Format("{0}\\{1}", strPath, strName));

                        if (id == null)
                        {
                            MessageBox.Show("新建文件失败！", "新建文件", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }
                        else
                        {
                            // 新建成功，更新树缓存
                            treePh.Tag = dbm.GetScriptInformation();
                        }

                        infoTable["id"] = id;
                        infoTable["scriptType"] = "databaseFile";
                        TreeNode newNode = treePh.SelectedNode.Nodes.Add(strName, strName);
                        newNode.ImageKey = "file";
                        newNode.SelectedImageKey = "file";
                        newNode.Tag = infoTable;
                        newNode.ContextMenuStrip = this.popTreePh_File;
                        treePh.SelectedNode.Expand();
                        treePh.SelectedNode = newNode;
                        
                        // 模拟点击节点
                        this.treePh_NodeMouseDoubleClick(this, new TreeNodeMouseClickEventArgs(newNode, MouseButtons.Left, 2, 1, 1));

                        EditForm fe = this.ActiveMdiChild as EditForm;
                        
                        // 模板内容写入
                        if (fe != null)
                        {
                            fe.luaEditBox.Text = strCode;
                        } 
                    } 
                }                
            }        
        }

        /// <summary>
        /// 重命名脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_rename_Click(object sender, EventArgs e)
        {
            if (treePh.SelectedNode != null)
            {
                treePh.SelectedNode.BeginEdit();
            }
        }

        /// <summary>
        /// 删除目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_del_Click(object sender, EventArgs e)
        {
            TreeNode folderNode = treePh.SelectedNode;

            if (folderNode != null)
            {
                if (MessageBox.Show("确定要删除当前目录吗？", "删除目录",
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    DeleteScriptFolder(folderNode);
                }
            }
        }

        /// <summary>
        /// 导出目录下的所有脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_export_Click(object sender, EventArgs e)
        {
            TreeNode tn_folder = treePh.SelectedNode;            

            if (tn_folder != null)
            {
                List<string> scriptIDList = new List<string>();
                fillArrayListInIDFromNode(scriptIDList, tn_folder);
                exportForm = new ExportForm(scriptIDList, Helper.ClientPath);
                exportForm.ShowDialog();
            }            
        }

        //给定目录节点/文件节点，递归搜索文件的id 填充到al_list
        private void fillArrayListInIDFromNode(List<string> idList, TreeNode tnode)
        {
            Hashtable ht = tnode.Tag as Hashtable;
            if(ht["type"].ToString() == "file")
            {
                idList.Add(ht["id"].ToString());
                return;
            }
            foreach(TreeNode tn in tnode.Nodes)
            {
                    Hashtable _ht = tn.Tag as Hashtable;
                if(_ht["type"].ToString() == "file")
                {
                    idList.Add(_ht["id"].ToString());
                }
                else
                {
                    fillArrayListInIDFromNode(idList, tn);
                }
            }
        }

        //给定目录名，返回文件ID列表
        public List<string> getFileIDsFromPath(string strPath)
        {
            List<string> idList = new List<string>();
            string [] asp = {"\\"};
            string [] as_path = strPath.Split(asp,  StringSplitOptions.RemoveEmptyEntries);
            foreach( TreeNode tn in treePh.Nodes.Find( as_path[as_path.Length - 1], true))
            {
                if(tn.FullPath == strPath)
                {
                    fillArrayListInIDFromNode(idList, tn);
                    break;
                }
            }
            return idList;
        }

        /// <summary>
        /// 查看目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cmdTreePhFolder_openlocal_Click(object sender, EventArgs e)
        {            
            string strPath = System.IO.Path.Combine(Helper.ClientPath, treePh.SelectedNode.FullPath);

            if (System.IO.Directory.Exists(strPath))
            {
                Helper.Execute("explorer.exe", string.Format("\"{0}\"", strPath), "true");
            }
            else
            {
                MessageBox.Show("找不到路径，请先导出！");
            }
        }

        #endregion

        /// <summary>
        /// 取消挂接客户端调试脚本
        /// </summary>
        private void DetachClientDebugFile()
        {
            string filePath = Path.Combine(Helper.ClientPath, @"ui\loginscript.ini");

            if (File.Exists(filePath))
            {
                string content = Helper.FileToString(filePath);

                if (content.Contains(@"\scripts\ldb.lua")) // 需要挂接
                {
                    StringBuilder newContent = new StringBuilder();
                    string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);                    

                    foreach (string s in lines)
                    {
                        if (!s.EndsWith(@"\scripts\ldb.lua"))
                        {
                            newContent.AppendLine(s);
                        }
                    }

                    Helper.WriteStringToFile(newContent.ToString(), filePath);
                }
            }
        }

        /// <summary>
        /// 挂接客户端调试脚本
        /// </summary>
        /// <return>是否挂接成功</return>
        private bool AttachClientDebugFile()
        {
            bool attachSuccess = false;
            string filePath = Path.Combine(Helper.ClientPath, @"ui\loginscript.ini");

            if (File.Exists(filePath))
            {
                string content = Helper.FileToString(filePath);

                if (!content.Contains(@"\scripts\ldb.lua")) // 需要挂接
                {
                    StringBuilder newContent = new StringBuilder();
                    string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
                    int fileCount = lines.Length - 2; // 计算加载脚本的数量

                    foreach (string s in lines)
                    {
                        newContent.AppendLine(s);
                    }

                    newContent.AppendLine(string.Format(@"lua_{0}=\scripts\ldb.lua", fileCount));
                    Helper.WriteStringToFile(newContent.ToString(), filePath);
                }

                attachSuccess = true;
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "挂接调试脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return attachSuccess;
        }

        /// <summary>
        /// 取消挂接服务端调试脚本
        /// </summary>
        private void DetachServerDebugFile()
        {
            string filePath = Path.Combine(Helper.ClientPath, @"scripts\server_main.lua");

            if (File.Exists(filePath))
            {
                string content = Helper.FileToString(filePath);

                if (!string.IsNullOrEmpty(content))
                {
                    if (content.Contains("Include('scripts/ldb.lua')"))
                    {
                        StringBuilder newContent = new StringBuilder();
                        string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                        foreach (string s in lines)
                        {
                            string line = s.Trim(new char[] { ' ', '\t' });

                            if (line != "Include('scripts/ldb.lua')")
                            {
                                newContent.AppendLine(s);
                            }
                        }

                        // 需要写文件
                        if (content != newContent.ToString())
                        {
                            Helper.WriteStringToFile(newContent.ToString(), filePath);
                        }
                    }                    
                }
            }
        }

        /// <summary>
        /// 挂接服务端调试脚本
        /// </summary>
        /// <return>是否挂接成功</return>
        private bool AttachServerDebugFile()
        {
            bool attachSuccess = false;
            string filePath = Path.Combine(Helper.ClientPath, @"scripts\server_main.lua");

            if (File.Exists(filePath))
            {
                string content = Helper.FileToString(filePath);

                if (!string.IsNullOrEmpty(content))
                {
                    StringBuilder newContent = new StringBuilder();
                    string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                    foreach (string s in lines)
                    {
                        string line = s.Trim(new char[] { ' ', '\t' });

                        switch (line)
                        {
                            case "function OnGameServerConnected(nConnIndex)":
                                {
                                    newContent.AppendLine(s);
                                    newContent.AppendLine("\tInclude('scripts/ldb.lua')");
                                    break;
                                }
                            case "Include('scripts/ldb.lua')":
                                {
                                    break;
                                }
                            default:
                                {
                                    newContent.AppendLine(s);
                                    break;
                                }
                        }
                    }

                    // 需要写文件
                    if (content != newContent.ToString())
                    {
                        Helper.WriteStringToFile(newContent.ToString(), filePath);
                    }

                    attachSuccess = true;  
                }
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "挂接调试脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return attachSuccess;
        }

        /// <summary>
        /// 给定目录名，返回{ [{文件ID}: {文件名、文件内容}] 。。。}的Hashtable
        /// </summary>
        /// <param name="strPath">目录名</param>
        /// <returns>信息数据表</returns>
        public Hashtable getFileListFromPath(string strPath)
        {
            List<string> fileList = getFileIDsFromPath(strPath);
            ProgressForm pForm = null;

            if (fileList.Count > 0)
            {
                pForm = new ProgressForm(0, fileList.Count);
                pForm.Show(); 
            }                       
            
            Hashtable ht_ret = new Hashtable();
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            DataTable dt = dbm.GetScriptInformation();
            int index = 1;

            foreach(string id in fileList)
            {                
                Hashtable ht_tmp = new Hashtable();
                DataRow[] dr = dt.Select("id = " + id);
                if(dr.Length == 0 ) return null;
                ht_tmp["filename"] = dr[0]["path"];
                ht_tmp["filecontent"] = dbm.GetScriptData(id);
                ht_ret.Add(id, ht_tmp);

                if (pForm != null)
                {
                    pForm.ShowProgress(index, string.Format("遍历{0}...", ht_tmp["filename"] as string));
                }
                
                index++;
            }

            return ht_ret;
        }

        public Hashtable getFileNameListFromPath(string strPath)
        {
            List<string> idList = getFileIDsFromPath(strPath);
            Hashtable ht_ret = new Hashtable();
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            DataTable dt = dbm.GetScriptInformation();
            foreach(string id in idList)
            {
                DataRow[] dr = dt.Select("id=" + id);
                if(dr.Length == 0 ) return null;
                ht_ret.Add(id, dr[0]["path"]);
            }
            return ht_ret;
        }        

        /// <summary>
        /// 检查服务器的分类树
        /// </summary>
        /// <returns>分类树是否改变</returns>
        private bool CheckTreePhServerChanged()
        {
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            DataTable dt = treePh.Tag as DataTable;

            if(DataTable2String(dbm.GetScriptInformation()) != DataTable2String(dt))
            {
                MessageBox.Show("目录结构已经在服务器端被更改，程序将会重新载入目录树", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                LoadDataBaseScriptTree();
                return true;
            }

            return false;
        }

        private string DataTable2String(DataTable dt)
        {
            string strRet = "";
            foreach(DataRow dr in dt.Rows)
            {
                strRet += dr["id"] + "," + dr["path"] + ",";
            }
            return strRet;
        }

        //文件、目录重命名操作
        private void treePh_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Label == null || e.Label == "" || e.Label == e.Node.Text)
            {
                e.CancelEdit = true;
                return;
            }

            //检查缓存内容是否和服务器匹配
            if(CheckTreePhServerChanged())
            {
                e.CancelEdit = true;
                return;
            }

            //强制不允许改名的目录
            // if(e.Node.FullPath == @"scripts\Map" || e.Node.FullPath == @"scripts")
            // {
            //     MessageBox.Show("由于更改scripts目录或Map目录会造成所有子目录文件的地图信息丢失，故强制规定这两个目录不能改名。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //     e.CancelEdit = true; 
            //     return;
            // }
            if(e.Node.FullPath.StartsWith(@"scripts\Map\"))
            {
                char [] asp = {'\\'};
                if(e.Node.FullPath.Split(asp).Length == 3)
                {
                    MessageBox.Show("地图名称暂时不允许修改", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    e.CancelEdit = true;
                    return;
                }
            }

            Hashtable ht = e.Node.Tag as Hashtable;
            string strType = ht["type"] as string;
            if (strType == "file")
            {
                DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
                bool bret = false;
                string id = ht["id"].ToString();
                if (dbm.IsScriptLocked(id))
                {
                    MessageBox.Show("文件正在被人编辑，不能改名", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    e.CancelEdit = true;
                    return;
                }
                string strParent = e.Node.FullPath.Substring(0, e.Node.FullPath.LastIndexOf("\\"));
                string strNewFilename = strParent + "\\" + e.Label;

                //检查重名
                DataTable dt = treePh.Tag as DataTable;
                if (dt.Select("path='" + strNewFilename + "'").Length > 0)
                {
                    MessageBox.Show("重命名失败，有重复的文件名称", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    e.CancelEdit = true;
                    return;
                }

                bret = dbm.UpdateScriptPath(e.Node.FullPath, strNewFilename);
                if (!bret) 
                    e.CancelEdit = true;
                else
                {
                    //修改成功，更新树缓存
                    treePh.Tag = dbm.GetScriptInformation();
                }
            }
            else if(strType == "folder")
            {
                string strParent = "";
                string strNewFolderRoot = "";
                if (e.Node.FullPath.IndexOf("\\") != -1)
                {
                    strParent = e.Node.FullPath.Substring(0, e.Node.FullPath.LastIndexOf("\\"));
                    strNewFolderRoot = strParent + "\\" + e.Label;
                }
                else
                {
                    strNewFolderRoot = e.Label;
                }
                if (e.Node.FullPath.ToLower() != strNewFolderRoot.ToLower())
                {
                    MessageBox.Show("目录名暂时只允许改大小写。");
                    e.CancelEdit = true;
                    return;
                }

                DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
                bool bret = true;
                List<string> idList = getFileIDsFromPath(e.Node.FullPath);

                foreach(string tid in idList)
                {
                    if (dbm.IsScriptLocked(tid))
                    {
                        MessageBox.Show("目录中有文件正在被别人编辑，故不能改名。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        e.CancelEdit = true;
                        return;
                    }
                }

                //检查重名
                DataTable dt = treePh.Tag as DataTable;
                if (e.Node.FullPath.ToLower() != strNewFolderRoot.ToLower())
                {
                    if (dt.Select("path like '" + strNewFolderRoot + "\\%'").Length > 0)
                    {
                        MessageBox.Show("重命名失败，有重复的目录名称");
                        e.CancelEdit = true;
                        return;
                    }
                }

                bret = dbm.UpdateScriptPath(e.Node.FullPath, strNewFolderRoot);
                if (!bret)
                {
                    MessageBox.Show("操作数据库失败，无法重命名");
                    e.CancelEdit = true;
                }
                else
                {
                    //修改成功，更新树缓存
                    treePh.Tag = dbm.GetScriptInformation();
                }
            }
        }

        /// <summary>
        /// 保存当前脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnSave_Click(object sender, EventArgs e)
        {
            EditForm editForm = this.ActiveMdiChild as EditForm;

            string strOut = ScriptCheck();

            if (strOut != null)
            {                
                strOut = strOut.TrimEnd('\n');
            }

            if (!string.IsNullOrEmpty(strOut)) // 脚本检查有问题
            {
                if (MessageBox.Show(string.Format("{0} 脚本中包含错误：\r\n{1}\r\n\r\n确认保存吗？", editForm.FileName, strOut), "脚本检查", 
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    editForm.DoSave();
                    TreePhOpera.SetNodeColorByID(treePh, editForm.ID, Color.Red);
                }
            }
            else
            {
                TreePhOpera.SetNodeColorByID(treePh, editForm.ID, Color.Black);
                bool bret = editForm.DoSave();

                if (bret)
                {
                    // 保存后自动导出
                    if (editForm.ID != "0")
                    {
                        List<string> scriptIDList = new List<string>();
                        scriptIDList.Add(editForm.ID);
                        exportForm = new ExportForm(scriptIDList, Helper.ClientPath);
                        exportForm.ShowDialog();
                    }                    
                }
            }            
        }

        /// <summary>
        /// 保存所有脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnSaveAll_Click(object sender, EventArgs e)
        {
            foreach (EditForm editForm in this.MdiChildren)
            {
                string strOut = ScriptCheck(editForm);

                if (strOut != null)
                {
                    strOut = strOut.TrimEnd('\n');
                }

                if (!string.IsNullOrEmpty(strOut))
                {
                    if (MessageBox.Show(string.Format("{0} 脚本中包含错误：\r\n{1}\r\n\r\n确认保存吗？", editForm.FileName, strOut), "脚本检查",
                                        MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        editForm.DoSave();
                        TreePhOpera.SetNodeColorByID(treePh, editForm.ID, Color.Red);
                    }

                    continue;
                }
                else
                {
                    TreePhOpera.SetNodeColorByID(treePh, editForm.ID, Color.Black);
                    editForm.DoSave();
                }                
            }
        }

        /// <summary>
        /// 查找替换字符串
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnStringReplace_Click(object sender, EventArgs e)
        {
            //读当前地图和选中内容
            string mapName = "";
            string findText = "";
            EditForm editForm = this.ActiveMdiChild as EditForm;

            if (editForm != null) 
            {
                if (editForm.luaEditBox.SelectedText != "")
                {
                    findText = editForm.luaEditBox.SelectedText;
                }
                else
                {
                    findText = editForm.luaEditBox.SelectedWord;
                }

                if (editForm.FileName.StartsWith(@"scripts\Map\"))
                {                    
                    string[] dataArray = editForm.FileName.Split(new char[] { '\\' }, StringSplitOptions.None);
                    mapName = dataArray[2];
                }
            }
            
            if (stringReplaceForm == null)
            {
                stringReplaceForm = new StringReplaceForm(this);
            }
            
            stringReplaceForm.Init(findText, mapName);            
        }

        /// <summary>
        /// 恢复已删除的脚本并添加树结点
        /// </summary>
        /// <param name="path">事件发送者</param>
        /// <param name="content">事件</param>
        private void InsertNewNode(string path, string content)
        {
            string[] paths = path.Split('\\');
            TreeNodeCollection currentNodes = treePh.Nodes;
            TreeNode currentNode = null;
            bool findNode = false;

            foreach (string s in paths)
            {
                findNode = false;
                if (s != "")
                {
                    foreach (TreeNode node1 in currentNodes)
                    {
                        if (node1.Text == s)
                        {
                            if(s.EndsWith(".lua")) // 该树结点已经存在
                            {
                                DialogResult result = MessageBox.Show("已经存在相同路径和文件名的记录，是否覆盖内容?", "历史记录恢复",
                                    MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                                if(result == DialogResult.Yes)
                                {
                                    string strView = "";
                                    bool coverSuccess = DataBaseManager.GetDataBaseManager().SaveScriptData(path, content, out strView);
                                    if(coverSuccess)
                                    {
                                        treePh.SelectedNode = node1;
                                        MessageBox.Show("已删除的脚本恢复成功!", "历史脚本恢复", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                    }
                                    else
                                    {
                                        MessageBox.Show("已删除的脚本恢复失败!", "历史脚本恢复", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                    }
                                }
                                findNode = true;
                                break;
                            }
                            else
                            {
                                findNode = true;
                                currentNodes = node1.Nodes;
                                break;
                            }
                            
                        }
                    }

                    if (!findNode)
                    {
                        currentNode = currentNodes.Add(s);
                        currentNodes = currentNode.Nodes;
                        if (s.EndsWith(".lua")) // 文件
                        {
                            currentNode.ImageIndex = 1;
                            currentNode.SelectedImageIndex = 1;

                            string id = DataBaseManager.GetDataBaseManager().CreateScriptData(path);
                            if(id != null)
                            {
                                string strView = "";
                                bool result = DataBaseManager.GetDataBaseManager().SaveScriptData(path, content, out strView);

                                if(result)
                                {
                                    Hashtable ht = new Hashtable();
                                    ht["type"] = "file";
                                    ht["id"] = id;
                                    currentNode.Tag = ht;
                                    MessageBox.Show("已删除的脚本恢复成功!", "历史脚本恢复", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                    treePh.SelectedNode = currentNode;
                                }
                                else
                                {
                                    MessageBox.Show("已删除的脚本恢复失败!", "历史脚本恢复", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                }
                            }
                        }
                        else // 文件夹
                        {
                            currentNode.ImageIndex = 0;
                            currentNode.SelectedImageIndex = 0;
                            Hashtable ht = new Hashtable();
                            ht["type"] = "folder";
                            currentNode.Tag = ht;
                        }
                    }
                }
            }

            
        }

        /// <summary>
        /// 用户选择查看历史记录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void btnHistory_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null && currentNode.Tag != null)
            {
                Hashtable infoTable = currentNode.Tag as Hashtable;

                if (infoTable["type"] as string == "file" && infoTable["scriptType"] as string == "databaseFile")
                {
                    string id = infoTable["id"] as string;          
                    HistoryForm hForm = new HistoryForm(DataBaseManager.GetDataBaseManager().Connection, id);
                    hForm.TreeImageList = imageList1;
                    hForm.Text = string.Format("查看历史记录  {0}", currentNode.FullPath);

                    if (hForm.ShowDialog() == DialogResult.Yes)
                    {
                        if (hForm.RecoverRecord) // 恢复历史记录
                        {
                            InsertNewNode(hForm.Path, hForm.ScriptData);
                        }
                        else // 读取历史记录
                        {
                            EditForm feshow = null;

                            foreach (EditForm fe in this.MdiChildren)
                            {
                                if (fe.ID == id)
                                {
                                    feshow = fe;
                                    break;
                                }
                            }

                            if (feshow == null)
                            {
                                feshow = new EditForm("databaseFile");
                            }

                            feshow.ID = id;
                            feshow.ScriptCode = hForm.ScriptData;
                            feshow.WindowState = FormWindowState.Maximized;
                            feshow.MdiParent = this;
                            feshow.FileName = currentNode.FullPath;
                            feshow.Show();
                        }                  
                    }
                }
                else
                {
                    MessageBox.Show("请选择要查看历史记录的脚本文件!", "查看历史记录", MessageBoxButtons.OK, MessageBoxIcon.Information);
                } 
            }
            else
            {
                MessageBox.Show("请选择要查看历史记录的脚本文件!", "查看历史记录", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void popRight_Opening(object sender, CancelEventArgs e)
        {
            
        }

        private void treePh_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Return)
            {
                if (treePh.SelectedNode != null)
                    treePh_NodeMouseDoubleClick(this, new TreeNodeMouseClickEventArgs(treePh.SelectedNode, MouseButtons.Left, 2, 0, 0));
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        //脚本检查
        private void btnCheck_Click(object sender, EventArgs e)
        {
            string strOut = "";
            try
            {
                strOut = ScriptCheck();
                if (strOut == null) return;
                strOut = strOut.TrimEnd('\n');
                if (strOut == "")
                {
                    MessageBox.Show("没有发现错误！", "检查结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show(strOut, "检查结果", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            //尝试跳转到出错行
            string[] strLines = strOut.Split(new string[] { ":" }, StringSplitOptions.RemoveEmptyEntries);
            for (int i = 0; i < strLines.Length - 1; i++)
            {
                if (strLines[i].Trim() == "LuaExec.lua" && strLines[i + 1].Trim() != "")
                {
                    string strLineNumber = strLines[i + 1].Trim();
                    try
                    {
                        int nLineNumber = Int32.Parse(strLineNumber);
                        EditForm fe = this.ActiveMdiChild as EditForm;
                        fe.luaEditBox.MoveToLine(nLineNumber);
                        return;
                    }
                    catch (Exception ex)
                    {
                        ex.GetType();
                    }
                }
            }
        }

        //检查焦点窗口的代码错误
        public string ScriptCheck()
        {
            return ScriptCheck(this.ActiveMdiChild as EditForm);
        }

        //检查指定代码的错误
        public string ScriptCheck(string strCode)
        {
            string sCKDir = Path.Combine(Application.StartupPath,  @"Plugins\LuaCheck\");

            string sourceCode = strCode; // = DataBaseManager.GetDataBaseManager().GetContentAfterConvert(strCode);
            Helper.WriteStringToFile(sourceCode, Path.Combine(sCKDir, "tmp.lua"));
        
            if (File.Exists(Path.Combine(sCKDir, "tmp.lua")) == false)
            {
                //MessageBox.Show("生成临时脚本失败！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                throw (new Exception("生成临时脚本失败！"));
            }

            //生成配置文件
            string strConfig = @"-- Settings
FILE_CLIENTPATH = '{0}'
FILE_LUAOUTPUT = false";
            string strClientPath = Helper.ClientPath.Replace("\\", "/");
            if (!strClientPath.EndsWith("/"))
            {
                strClientPath += "/";
            }
            strConfig = string.Format(strConfig, strClientPath);
            Helper.WriteStringToFile(strConfig, Path.Combine(sCKDir, "config.lh"));

            //读取用户配置以确定具体检查工具
            string strUserCfg = Helper.FileToString(Application.StartupPath + @"\Plugins\LuaCheck\_setting.cfg");
            string[] astrUserCfg = strUserCfg.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);
            string strExeName = "";
            if (astrUserCfg.Length == 6 && astrUserCfg[5] == "逻辑检查")
            {
                strExeName = "ExecLua.exe";
            }
            else
            {
                strExeName = "luac5.1.exe";
            }

            //执行检查工具
            if (File.Exists(Path.Combine(sCKDir, strExeName)))
            {
                Process p = new Process();
                p.StartInfo.FileName = Path.Combine(sCKDir, strExeName);
                p.StartInfo.Arguments = "\"" + Path.Combine(sCKDir, @"tmp.lua") + "\"";
                p.StartInfo.WorkingDirectory = sCKDir;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardError = true;
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.CreateNoWindow = true;
                p.Start();

                StreamReader stream = p.StandardOutput;                
                StreamReader streamErr = p.StandardError;

                string strOut = "";
                while (!p.HasExited)
                {
                    strOut += stream.ReadLine() + "\n";
                    strOut += streamErr.ReadLine() + "\n";
                }
                strOut += stream.ReadToEnd();
                strOut += streamErr.ReadToEnd();
                p.WaitForExit();
                File.Delete(Path.Combine(sCKDir, @"tmp.lua"));
                return strOut;
            }
            else
            {
                //MessageBox.Show("检查工具不存在！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                throw (new Exception("检查工具不存在！"));
            }
        }

        //检查指定窗口的代码错误
        public string ScriptCheck(EditForm fe)
        {
            string strCode = "";
            if (fe != null)
            {
                if (fe.luaEditBox.Text != null)
                {
                    strCode = fe.luaEditBox.Text;
                }
            }
            if (strCode == "")
            {
                return null;
            }
            return ScriptCheck(strCode);
        }        

        private void bunToolCheck_Click(object sender, EventArgs e)
        {
            btnCheck_Click(this, new EventArgs());
        }

        private void cmdTreePhFolder_check_Click(object sender, EventArgs e)
        {
            TreeNode tn_folder = treePh.SelectedNode;

            if (tn_folder == null) return;
            List<string> idList = new List<string>();
            fillArrayListInIDFromNode(idList, tn_folder);
            int nErrorCount = 0;
            foreach(string strID in idList)
            {
                string strContent = DataBaseManager.GetDataBaseManager().GetScriptData(strID);
                string strOut = "";
                try
                {
                    strOut = ScriptCheck(strContent);
                    strOut = strOut.Trim().TrimEnd('\n');
                }
                catch(Exception ex)
                {
                    MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                if (strOut != "")             //发现错误
                {
                    TreePhOpera.SetNodeColorByID(treePh, strID, Color.Red);
                    nErrorCount++;
                }
                else
                {
                    TreePhOpera.SetNodeColorByID(treePh, strID, Color.Black);
                }
            }
            MessageBox.Show("检查完毕，" + ((nErrorCount > 0) ? ("发现" + nErrorCount + "个错误文件") : ("没有发现错误")),
                    "检查完毕",
                    MessageBoxButtons.OK,
                    ((nErrorCount > 0) ? MessageBoxIcon.Error : MessageBoxIcon.Information));
        }

        /// <summary>
        /// 清理ls表
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bClearLS_Click(object sender, EventArgs e)
        {
            return;
            ClearLSForm cForm = new ClearLSForm();
            cForm.ShowDialog();
        }

        private void btnToolFormat_Click(object sender, EventArgs e)
        {
            string strFileName = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\filter.lua");
            if (File.Exists(strFileName))
            {
                try
                {
                    this.m_lua.DoFile(strFileName);
                    LuaInterface.LuaFunction fun = this.m_lua.GetFunction("main");
                    if (fun != null)
                    {
                        fun.Call(sender, e);
                    }
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.ToString(), "错误",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void btnFormat_Click(object sender, EventArgs e)
        {
            btnToolFormat_Click(btnToolFormat, e);
        }

        private void buttonItem4_Click(object sender, EventArgs e)
        {
            NewsForm nf = new NewsForm();
            nf.ShowDialog();
        }

        private void buttonItem5_Click(object sender, EventArgs e)
        {
            HistoryTotalForm htf = new HistoryTotalForm(DataBaseManager.GetDataBaseManager().Connection);
            htf.Show();
        }

        /// <summary>
        /// 删除断点
        /// </summary>
        /// <param name="fileName">断点所在文件</param>
        /// <param name="lineIndex">断点所在行号</param>
        private void RemoveBreakPoint(string fileName, int lineIndex)
        {
            foreach (EditForm editForm in this.MdiChildren)
            {
                if (editForm.FileName == fileName)
                {
                    editForm.RemoveBreakPoint(lineIndex);
                    break;
                }
            }
        }

        /// <summary>
        /// 输出文本
        /// </summary>
        /// <param name="text">文本内容</param>
        private void PrintOutputText(string text)
        {
            outputBox.AppendText(string.Format("{0}\r\n", text));
        }

        /// <summary>
        /// 注册断点
        /// </summary>
        private void RegistBreakPoint()
        {
            PrintOutputText("开始注册断点...");

            foreach (DataGridViewRow currentRow in breakPointView.Rows)
            {
                bool enableBreakPoint = (bool)currentRow.Cells["EnableBreakPoint"].Value;
                string fileName = currentRow.Cells["BreakPointFileName"].Value as string;
                string lineIndex = currentRow.Cells["BreakPointLineIndex"].Value as string;
                string condition = currentRow.Cells["BreakPointCondition"].Value as string;

                if (enableBreakPoint)
                {
                    string message = null;
                    string returnMessage = null;

                    if (!string.IsNullOrEmpty(condition)) // 带有条件信息的断点
                    {
                        message = string.Format("add_break_point {0} {1} {2}", fileName, lineIndex.ToString(), condition);
                        returnMessage = string.Format("<ldb>add_break_point: {0},{1},{2}", fileName, lineIndex.ToString(), condition);
                        PrintOutputText(string.Format("注册断点 —— 文件名：{0}，行号：{1}，条件：{2}", 
                                                      fileName, lineIndex.ToString(), condition));

                    }
                    else
                    {
                        message = string.Format("add_break_point {0} {1}", fileName, lineIndex.ToString());
                        returnMessage = string.Format("<ldb>add_break_point: {0},{1}", fileName, lineIndex.ToString());
                        PrintOutputText(string.Format("注册断点 —— 文件名：{0}，行号：{1}", fileName, lineIndex.ToString()));
                    }

                    string receivedMessage = SendAndWaitMessage(message, returnMessage, false);

                    if (receivedMessage == null)
                    {
                        receivedMessage = "接收消息超时...";
                        currentRow.Cells["BreakPointState"].Value = "接收消息超时";
                        currentRow.Cells["BreakPointState"].Style.ForeColor = Color.Red;
                    }
                    else
                    {
                        currentRow.Cells["BreakPointState"].Value = "注册成功";
                        currentRow.Cells["BreakPointState"].Style.ForeColor = Color.Black;
                    }

                    PrintOutputText(receivedMessage);
                }
            }

            PrintOutputText("注册断点完成...");
        }

        /// <summary>
        /// 发送并开始接收消息
        /// </summary>
        /// <param name="message">发送的消息</param>
        /// <param name="returnMessage">期望返回的消息</param>
        /// <param name="forHead">是否是期望指消息头</param>
        /// <returns>接收到的消息</returns>
        private string SendAndWaitMessage(string message, string returnMessage, bool forHead)
        {
            string receivedMessage = null;
            networkManager.SendUdpMessage(message);

            //重试几次，如果超时认为接收失败
            for (int i = 0; i < Helper.ReceiveMessageRetryTime; i++)
            {
                if (forHead)
                {
                    receivedMessage = networkManager.GetMessageForHead(returnMessage);
                }
                else
                {
                    receivedMessage = networkManager.GetMessage(returnMessage);
                }

                if (receivedMessage != null)
                {
                    break;
                }

                Thread.Sleep(Helper.ReceiveMessageDelay);
                // Application.DoEvents();
            }            

            return receivedMessage;
        }

        /// <summary>
        /// 从调试信息中获取跳转信息
        /// </summary>
        /// <param name="message">文本消息</param>        
        /// <returns>单步信息</returns>
        private object[] GetJumpInfoFromMessage(string message)
        {
            object[] info = new object[3];
            int index = message.IndexOf(":");
            string tempString = message.Substring(index + 2);            
            string[] data = tempString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            info[0] = data[0];
            info[1] = int.Parse(data[1]);

            if (data.Length == 3)
            {
                info[2] = data[2];
            }
                      
            return info;
        }      
        
        /// <summary>
        /// 创建变量结点
        /// </summary>
        /// <returns>变量结点</returns>
        private Node CreateVarNode()
        {
            Node newNode = new Node();
            newNode.Cells.Add(new Cell());
            newNode.Cells.Add(new Cell());
            newNode.Cells.Add(new Cell());

            newNode.Cells[0].Tag = "varName";
            newNode.Cells[1].Tag = "varValue";
            newNode.Cells[2].Tag = "varFileName";
            newNode.Cells[3].Tag = "varState";            

            return newNode;
        }

        /// <summary>
        /// 刷新变量值
        /// </summary>
        private void RefreshVarValue()
        {
            PrintOutputText("开始刷新变量值...");

            lockVarValue = true; // 这个时候不允许修改变量值
            EditForm editForm = this.ActiveMdiChild as EditForm;

            foreach (Node node in watchTree.Nodes)
            {                
                string varName = node.Cells[0].Text;
                string fileName = node.Cells[2].Text;

                if (!string.IsNullOrEmpty(varName) && fileName == editForm.FileName)
                {
                    PrintOutputText(string.Format("获取变量值 —— 变量名：{0}，文件名：{1}", varName, fileName));
                    string message = null;

                    if (varName.Contains("(")) // 检查是否包含方法调用
                    {
                        message = string.Format("print_fun {0}", varName);
                    }
                    else
                    {
                        message = string.Format("print_var {0}", varName);
                    }

                    string receivedMessage = SendAndWaitMessage(message, string.Format("<ldb>print_var: {0}", varName), true);

                    if (receivedMessage == null)
                    {                        
                        node.Cells[3].Text = "接收消息超时";
                    }
                    else if (receivedMessage == string.Format("<ldb>print_var: {0} failed!", varName))
                    {
                        node.Cells[1].Text = "无效变量";
                        node.Cells[3].Text = "获取成功";                        
                    }
                    else
                    {
                        string varNewValue = receivedMessage.Replace(string.Format("<ldb>print_var: {0} -> ", varName), "");

                        if (varNewValue.Contains("\n")) // 多行分解出来
                        {
                            node.Nodes.Clear();
                            string[] valueArray = varNewValue.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
                            node.Cells[1].Text = valueArray[0];

                            for (int i = 1; i < valueArray.Length; i++)
                            {
                                Node newNode = CreateVarNode();
                                string valueString = valueArray[i];
                                int index = valueString.IndexOf("=");

                                if (index >= 0)
                                {
                                    newNode.Cells[0].Text = valueString.Substring(0, index - 1);
                                    newNode.Cells[1].Text = valueString.Substring(index + 2);
                                }
                                else
                                {
                                    newNode.Cells[1].Text = valueString;
                                }

                                node.Nodes.Add(newNode);
                            }
                        }
                        else
                        {
                            node.Cells[1].Text = varNewValue;
                        }
                        
                        node.Cells[3].Text = "获取成功";                        
                    }
                }    
            }

            // 允许修改变量值
            lockVarValue = false; 
            PrintOutputText("刷新变量值完成...");
        }

        /// <summary>
        /// 编辑窗体取词
        /// </summary>
        /// <param name="editForm">编辑窗体</param>
        /// <param name="e">取词参数</param>
        private void EditFormWordMouseHover(EditForm editForm, ref WordMouseEventArgs e)
        {
            string fileName = editForm.FileName;
            string varName = e.Word.Text;
            editForm.luaEditBox.CloseTipText();

            if (startDebug && enableFetchWord && mouseFetchWord && !string.IsNullOrEmpty(varName))
            {
                // 只接收合法命名的单词
                Regex regex = new Regex(@"[A-Za-z0-9_]+");
               
                if (regex.IsMatch(varName))
                {
                    if (ProcessFileName(editForm.FileName) == currentDebugFile)
                    {
                        string message = string.Format("print_var {0}", varName);
                        string receivedMessage = SendAndWaitMessage(message, string.Format("<ldb>print_var: {0}", varName), true);
                        string tipText = null;

                        if (receivedMessage != null)
                        {
                            if (receivedMessage == string.Format("<ldb>print_var: {0} failed!", varName))
                            {
                                tipText = "无效变量";
                            }
                            else
                            {
                                receivedMessage = receivedMessage.Replace(string.Format("<ldb>print_var: {0} -> ", varName), "");
                                receivedMessage = receivedMessage.Replace("<", "＜").Replace(">", "＞"); // 遇到<和>符号的时候会出bug，先暂时替换掉
                                receivedMessage = receivedMessage.Replace("\n", "<BR>"); // 替换换行符
                                tipText = string.Format("<b>{0}</b><BR>{1}", varName, receivedMessage);
                            }                           
                        }
                        else
                        {
                            tipText = "接收消息超时";
                        }

                        TextPoint location = new TextPoint(e.Word.Column + e.Word.Text.Length, e.Word.Row.Index);
                        editForm.luaEditBox.SetTipText(tipText, location);
                    }
                }
            }
        }

        /// <summary>
        /// 编辑窗体添加断点
        /// </summary>
        /// <param name="editForm">编辑窗体</param>
        /// <param name="row">断点所在行</param>
        private void EditFormBreakPointAdded(EditForm editForm, Fireball.Syntax.Row row)
        {
            bool exist = false;
            int currentLineIndex = -1;
            string fileName = null;

            foreach (DataGridViewRow breakPointRow in breakPointView.Rows)
            {
                fileName = breakPointRow.Cells["BreakPointFileName"].Value as string;
                string lineIndex = breakPointRow.Cells["BreakPointLineIndex"].Value as string;
                currentLineIndex = row.Index + 1;

                if (fileName == editForm.FileName && lineIndex == currentLineIndex.ToString())
                {
                    exist = true;
                    break;
                }
            }

            if (!exist)
            {
                fileName = editForm.FileName;
                currentLineIndex = row.Index + 1;                
                breakPointView.Rows.Add(1);
                DataGridViewRow newRow = breakPointView.Rows[breakPointView.Rows.Count - 1];
                newRow.Cells["EnableBreakPoint"].Value = true;
                newRow.Cells["BreakPointFileName"].Value = fileName;
                newRow.Cells["BreakPointLineIndex"].Value = currentLineIndex.ToString();

                // 动态注册断点
                if (startDebug)
                {
                    string message = string.Format("add_break_point {0} {1}", fileName, currentLineIndex.ToString());
                    PrintOutputText(string.Format("注册断点 —— 文件名：{0}，行号：{1}", fileName, currentLineIndex.ToString()));

                    string receivedMessage = SendAndWaitMessage(message, string.Format("<ldb>add_break_point: {0},{1}", fileName, currentLineIndex), false);

                    if (receivedMessage != null)
                    {
                        newRow.Cells["BreakPointState"].Value = "注册成功";
                        newRow.Cells["BreakPointState"].Style.ForeColor = Color.Black;
                    }
                    else
                    {
                        receivedMessage = "接收消息超时...";
                        newRow.Cells["BreakPointState"].Value = "接收消息超时";
                        newRow.Cells["BreakPointState"].Style.ForeColor = Color.Red;
                    }

                    PrintOutputText(receivedMessage);
                }
            }
        }

        /// <summary>
        /// 编辑窗体删除断点
        /// </summary>
        /// <param name="editForm">编辑窗体</param>
        /// <param name="row">断点所在行</param>
        private void EditFormBreakPointRemoved(EditForm editForm, Fireball.Syntax.Row row)
        {
            int removeRowIndex = -1;
            string fileName = null;
            int currentLineIndex = row.Index + 1;

            foreach (DataGridViewRow breakPointRow in breakPointView.Rows)
            {
                fileName = breakPointRow.Cells["BreakPointFileName"].Value as string;
                string lineIndex = breakPointRow.Cells["BreakPointLineIndex"].Value as string;

                if (fileName == editForm.FileName && lineIndex == currentLineIndex.ToString())
                {
                    removeRowIndex = breakPointRow.Index;
                    break;
                }
            }

            if (removeRowIndex != -1)
            {
                breakPointView.Rows.RemoveAt(removeRowIndex);
                
                // 动态取消注册断点
                if (startDebug)
                {
                    string message = string.Format("delete_break_point {0} {1}", fileName, currentLineIndex.ToString());
                    PrintOutputText(string.Format("取消注册断点 —— 文件名：{0}，行号：{1}", fileName, currentLineIndex.ToString()));
                    string receivedMessage = SendAndWaitMessage(message, string.Format("<ldb>delete_break_point: {0},{1}", 
                                                                                       fileName, currentLineIndex.ToString()), false);

                    if (receivedMessage != null)
                    {
                        receivedMessage = "取消注册断点成功...";
                    }
                    else
                    {
                        receivedMessage = "接收消息超时...";
                    }

                    PrintOutputText(receivedMessage);
                }
            }            
        }

        /// <summary>
        /// 单步调试
        /// </summary>
        /// <param name="stepType">单步类型</param>
        /// <param name="stepMessage">单步消息</param>
        private void StepDebug(string stepType, string stepMessage)
        {
            PrintOutputText(string.Format("开始{0}...", stepType));
            enableFetchWord = false; // 关闭取词功能，防止接收到取词的消息

            // 开启调试状态
            string message = stepMessage;
            networkManager.SendUdpMessage(message);
            
            PrintOutputText(string.Format("开始等待{0}消息...", stepType));          
            StartDebugWaitTimer(Helper.BreakPointWaitCircle, DebugState.WaitStepDebugActivated);
        }

        /// <summary>
        /// 运行到下一个断点
        /// </summary>
        private void ContinueDebug()
        {
            PrintOutputText("开始运行到下一个断点...");
            
            // 清掉当前接收到的所有消息
            networkManager.ClearReceivedMessageList();

            // 开启调试状态
            string message = "continue_debug";
            networkManager.SendUdpMessage(message);

            // 清除调用栈信息
            callStackView.Rows.Clear();

            // 消除当前的调试行标记
            EditForm editForm = tabStrip1.SelectedTab.AttachedControl as EditForm;
            editForm.luaEditBox.ClearDebugMark();
            editForm.luaEditBox.Refresh();

            // 去掉当前调试文件的信息
            currentDebugFile = null;

            // 等待服务端触发断点
            PrintOutputText("开始等待触发断点...");
            StartDebugWaitTimer(Helper.BreakPointWaitCircle, DebugState.WaitBreakPointActivated);
        }

        /// <summary>
        /// 更新脚本的字体
        /// </summary>
        /// <param name="font">字体</param>
        private void UpdateScriptFont(Font font)
        {
            foreach (EditForm editForm in this.MdiChildren)
            {
                editForm.luaEditBox.ScriptFont = Helper.ScriptFont;
            }
        }

        /// <summary>
        /// 更新脚本字体的颜色
        /// </summary>
        /// <param name="color">颜色</param>
        private void UpdateScriptForeColor(Color color)
        {
            foreach (EditForm editForm in this.MdiChildren)
            {
                editForm.luaEditBox.ScriptForeColor = Helper.ScriptForeColor;
            }
        }        

        /// <summary>
        /// 取消挂接调试文件
        /// </summary>
        private void DetachDebugFile()
        {
            string filePath = Path.Combine(Helper.ClientPath, @"scripts\server_main.lua");
            string content = Helper.FileToString(filePath);

            if (!string.IsNullOrEmpty(content))
            {
                StringBuilder newContent = new StringBuilder();
                string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
                bool findLine = false;
                bool ignoreLine = false;
                bool requireWriteFile = false;

                foreach (string s in lines)
                {
                    string line = s.Trim(new char[] { ' ', '\t' });

                    if (findLine)
                    {
                        if (line == "Include('scripts/ldb.lua')")
                        {
                            ignoreLine = true;
                            requireWriteFile = true;
                        }
                    }
                    else
                    {
                        if (line == "function OnGameServerConnected(nConnIndex)")
                        {
                            findLine = true;
                        }
                    }

                    if (!ignoreLine)
                    {
                        newContent.AppendLine(s);
                    }
                    else
                    {
                        ignoreLine = false;
                    }
                }

                if (requireWriteFile)
                {
                    Helper.WriteStringToFile(newContent.ToString(), filePath);
                }
            }
        }

        /// <summary>
        /// 开启调试等待计时器
        /// </summary>
        /// <param name="interval">计时器间隔</param>
        /// <param name="debugState">调试状态</param>
        private void StartDebugWaitTimer(int interval, DebugState debugState)
        {
            this.debugState = debugState;
            debugWaitTimer.Interval = interval;
            debugWaitTimer.Start();
        }

        /// <summary>
        /// 关闭调试等待计时器
        /// </summary>
        private void StopDebugWaitTimer()
        {
            if (debugWaitTimer != null)
            {
                debugState = DebugState.None;
                debugWaitTimer.Stop();
            }
        }

        /// <summary>
        /// 开始调试脚本
        /// </summary>
        private void StartDebug(DebugMode currentDebugMode)
        {
            // 检查调试文件是否都已经存在
            if (!CheckDebugFileReady())
            {
                MessageBox.Show("请先确定调试文件都已经正确挂接！", "开始调试", MessageBoxButtons.OK, MessageBoxIcon.Information);
                SettingForm sForm = new SettingForm();
                sForm.CurrentUpdateScriptFont = new SettingForm.UpdateScriptFont(UpdateScriptFont);
                sForm.CurrentUpdateScriptForeColor = new SettingForm.UpdateScriptForeColor(UpdateScriptForeColor);
                sForm.ShowDialog();
                return;
            }

            // 检查调试模式是否变更过
            if (debugMode != DebugMode.None && debugMode != currentDebugMode)
            {
                switch (currentDebugMode)
                {
                    case DebugMode.Client:
                        {
                            if (Helper.CheckProcessExist("JX3Client", false))
                            {
                                MessageBox.Show("从服务端脚本切换到客户端脚本调试时，需要修改挂接文件，请先关闭JX3Client.exe！", "开始调试",
                                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                                return;
                            }

                            break;
                        }
                    case DebugMode.Server:
                        {
                            if  (Helper.CheckProcessExist("SO3GameServer", false))
                            {
                                MessageBox.Show("从客户端脚本切换到服务端脚本调试时，需要修改挂接文件，请先关闭JX3Client.exe！", "开始调试",
                                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                                return;
                            }

                            break;
                        }
                }
            }

            // 挂接调试文件
            switch (currentDebugMode)
            {
                case DebugMode.Client:
                    {
                        DetachServerDebugFile();

                        if (!AttachClientDebugFile())
                        {
                            return;
                        }

                        break;
                    }
                case DebugMode.Server:
                    {
                        DetachClientDebugFile();

                        if (!AttachServerDebugFile())
                        {
                            return;
                        }

                        break;
                    }
            }

            debugMode = currentDebugMode;

            PrintOutputText("开始调试脚本...");
            networkManager = NetworkManager.GetNetworkManager();
            networkManager.BeginReceiveUdpMessage();
            PrintOutputText("开始等待响应调试请求...");
            StartDebugWaitTimer(Helper.BreakPointWaitCircle, DebugState.WaitStartDebugActivated);
        }

        /// <summary>
        /// 开始调试服务端脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bStartDebug_Click(object sender, EventArgs e)
        {            
            StartDebug(DebugMode.Server);            
        }

        /// <summary>
        /// 停止调试脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bStopDebug_Click(object sender, EventArgs e)
        {
            // 关闭调试状态
            PrintOutputText("停止调试脚本...");
            string message = "stop_debug";
            string receivedMessage = SendAndWaitMessage(message, "<ldb>stop_debug: success!", false);

            while (receivedMessage == null)
            {
                if (MessageBox.Show("接收确认消息超时，是否重新发送调试消息？", "停止调试",
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    receivedMessage = SendAndWaitMessage(message, "<ldb>stop_debug: success!", false);
                }
                else
                {
                    break;
                }
            }

            if (receivedMessage != null)
            {
                PrintOutputText("停止调试脚本成功...");

                // 清空消息队列
                networkManager.ClearReceivedMessageList();
            }

            //  切换状态
            startDebug = false;
            bStartDebug.Enabled = true;
            bStartDebugClient.Enabled = true;
            bStopDebug.Enabled = false;
            currentDebugFile = null;

            foreach (ButtonItem buttonItem in debugButtonList)
            {
                buttonItem.Enabled = false;
            }

            // 清除变量信息
            watchTree.Nodes.Clear();

            // 清除调用栈信息
            callStackView.Rows.Clear();

            // 消除调试标记
            foreach (EditForm editForm in this.MdiChildren)
            {
                editForm.Text = editForm.Text.Replace("[已锁定]", "").Trim();
                editForm.luaEditBox.ReadOnly = false;
                editForm.luaEditBox.ClearDebugMark();
                editForm.Refresh();
            }

            // 关掉调试计时器
            StopDebugWaitTimer();            
        }

        /// <summary>
        /// 删除断点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteBreakPoint_Click(object sender, EventArgs e)
        {
            if (breakPointView.SelectedCells != null && breakPointView.SelectedCells.Count> 0)
            {
                DataGridViewCell currentCell = breakPointView.SelectedCells[0];

                if (currentCell.RowIndex >= 0) // 检查下是不是HeaderRow
                {
                    DataGridViewRow currentRow = currentCell.OwningRow;
                    string fileName = currentRow.Cells["BreakPointFileName"].Value as string;
                    int lineIndex = int.Parse(currentRow.Cells["BreakPointLineIndex"].Value as string);
                    RemoveBreakPoint(fileName, lineIndex - 1);
                }
                else
                {
                    MessageBox.Show("请先选择要删除的断点！", "删除断点", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先选择要删除的断点！", "删除断点", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 删除所有断点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteAllBreakPoint_Click(object sender, EventArgs e)
        {
            // 因为在去掉断点的时候，breakPointView的数据行也是动态变化的，所以不能直接在循环中访问
            List<string> fileNameList = new List<string>();
            List<int> lineIndexList = new List<int>();

            foreach (DataGridViewRow currentRow in breakPointView.Rows)
            {
                string fileName = currentRow.Cells["BreakPointFileName"].Value as string;
                int lineIndex = int.Parse(currentRow.Cells["BreakPointLineIndex"].Value as string);                
                fileNameList.Add(fileName);
                lineIndexList.Add(lineIndex - 1);
            }

            for (int i = 0; i < fileNameList.Count; i++)
            {
                RemoveBreakPoint(fileNameList[i], lineIndexList[i]);
            }
        }

        /// <summary>
        /// 刷新变量值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRefreshVar_Click(object sender, EventArgs e)
        {
            RefreshVarValue();
        }

        /// <summary>
        /// 删除变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteVar_Click(object sender, EventArgs e)
        {
            Node currentNode = watchTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 0)
            {
                currentNode.Remove();
            }
        }

        /// <summary>
        /// 删除所有变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteAllVar_Click(object sender, EventArgs e)
        {
            watchTree.Nodes.Clear();            
        }

        /// <summary>
        /// 单步运行
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bStepDebug_Click(object sender, EventArgs e)
        {
            StepDebug("单步进入", "step_into");            
        }

        /// <summary>
        /// 运行到下一个断点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bContinueDebug_Click(object sender, EventArgs e)
        {
            ContinueDebug();
        }

        /// <summary>
        /// 添加变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddVar_Click(object sender, EventArgs e)
        {
            if (tabStrip1.SelectedTab != null)
            {
                EditForm editForm = tabStrip1.SelectedTab.AttachedControl as EditForm;
                string fileName = editForm.FileName;

                Node newNode = CreateVarNode();
                newNode.Cells[2].Text = fileName;
                watchTree.Nodes.Add(newNode);
            }
            else
            {
                MessageBox.Show("请先选择要添加变量的脚本！", "添加变量", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 清空输出
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteOutput_Click(object sender, EventArgs e)
        {
            outputBox.Clear();
        }

        /// <summary>
        /// 设置/取消断点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSwitchBreakPoint_Click(object sender, EventArgs e)
        {
            EditForm editForm = this.ActiveMdiChild as EditForm;
 
            if (editForm != null)
            {
                int currentLineIndex = editForm.luaEditBox.SelectedLineNumber;

                if (currentLineIndex >= 0)
                {
                    List<int> breakPointIndexList = editForm.luaEditBox.BreakPointIndexList;
                    
                    if (breakPointIndexList.Contains(currentLineIndex + 1))
                    {
                        editForm.luaEditBox.SetBreakPoint(currentLineIndex, false);
                    }
                    else
                    {
                        editForm.luaEditBox.SetBreakPoint(currentLineIndex, true);
                    }                    
                }
            }
        }

        /// <summary>
        /// 单步跳过
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bStepOver_Click(object sender, EventArgs e)
        {
            StepDebug("单步跳过", "step_over");
        }

        /// <summary>
        /// 主窗体的按键事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            EditForm_KeyDown(sender, e);
        }

        /// <summary>
        /// 鼠标双击调用栈信息
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void callStackView_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.RowIndex >= 0)
            {
                DataGridViewRow currentRow = callStackView.Rows[e.RowIndex];

                string fileName = currentRow.Cells["CallStackFileName"].Value.ToString();
                string lineIndex = currentRow.Cells["CallStackLineIndex"].Value.ToString();

                EditForm editForm = OpenFileInEditor(fileName, true);

                if (editForm != null)
                {
                    editForm.luaEditBox.MoveToLine(int.Parse(lineIndex) - 1);
                }
            }
        }

        /// <summary>
        /// 开启/关闭服务端调试输出
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bEnableServerOutput_Click(object sender, EventArgs e)
        {
            enableServerDebugOutput = !enableServerDebugOutput;
            string message = null;

            if (enableServerDebugOutput)
            {
                message = "enable_debug_output";
                bEnableServerDebugOutput.Text = "关闭服务端调试输出";
                PrintOutputText("开启服务端调试输出...");
            }
            else
            {
                message = "disable_debug_output";
                bEnableServerDebugOutput.Text = "开启服务端调试输出";
                PrintOutputText("关闭服务端调试输出...");
            }

            networkManager.SendUdpMessage(message);
        }

        /// <summary>
        /// 显示/隐藏通用工具栏
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewGeneralToolBar_Click(object sender, EventArgs e)
        {
            bViewGeneralToolBar.Checked = !bViewGeneralToolBar.Checked;
            barGeneral.Visible = bViewGeneralToolBar.Checked;
        }

        /// <summary>
        /// 显示/隐藏调试工具栏
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewDebugToolBar_Click(object sender, EventArgs e)
        {
            bViewDebugToolBar.Checked = !bViewDebugToolBar.Checked;
            barDebug.Visible = bViewDebugToolBar.Checked;
        }

        /// <summary>
        /// 开启/关闭鼠标取词
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMouseGetWord_Click(object sender, EventArgs e)
        {
            mouseFetchWord = !mouseFetchWord;

            if (mouseFetchWord)
            {
                bMouseGetWord.Text = "关闭鼠标取词";
            }
            else
            {
                bMouseGetWord.Text = "开启鼠标取词";
                
                // 关闭鼠标取词的Tip
                EditForm editForm = this.ActiveMdiChild as EditForm;

                if (editForm != null)
                {
                    editForm.luaEditBox.CloseTipText();
                }
            }
        }

        /// <summary>
        /// 窗体关闭
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // 取消脚本调试
            if (networkManager != null)
            {
                networkManager.Uninit();
            }
            
            // 保存本地脚本
            if (CheckLocalFileTreeChanged())
            {                
                if (MessageBox.Show("是否保存对本地文件的配置信息？", "保存配置", 
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    SaveLocalFileList();
                }
            }
        }

        /// <summary>
        /// 加载本地文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bLoadLocalFile_Click(object sender, EventArgs e)
        {            
            AddLocalFile(Helper.ClientPath);
        }

        /// <summary>
        /// 移除本地目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemoveLocalFolder_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                if (MessageBox.Show("确定要在编辑器中移除当前目录吗？", "移除目录",
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                {
                    currentNode.Remove();
                }
            }
        }

        /// <summary>
        /// 添加文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddLocalFile_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                string folderName = currentNode.FullPath.Replace("本地文件", "").TrimStart(new char[] { '\\' });
                folderName = Path.Combine(Helper.ClientPath, folderName);

                AddLocalFile(folderName);
            }            
        }

        /// <summary>
        /// 移除本地文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemoveLocalFile_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                if (MessageBox.Show("确定要在编辑器中移除当前文件吗？", "移除文件",
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                {
                    currentNode.Remove();
                }
            }
        }

        /// <summary>
        /// 调试客户端脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bStartDebugClient_Click(object sender, EventArgs e)
        {
            StartDebug(DebugMode.Client);
        }

        /// <summary>
        /// 添加目录下的所有文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddAllLocalFile_Click(object sender, EventArgs e)
        {
            TreeNode currentNode = treePh.SelectedNode;

            if (currentNode != null)
            {
                string folderName = currentNode.FullPath.Replace("本地文件", "").TrimStart(new char[] { '\\' });
                folderName = Path.Combine(Helper.ClientPath, folderName);

                DirectoryInfo di = new DirectoryInfo(folderName);

                if (di.Exists)
                {
                    foreach (FileInfo fi in di.GetFiles())
                    {
                        if (fi.Extension == ".lua")
                        {
                            AddLocalFileNode(fi.FullName);
                        }
                    }
                }                
            } 
        }

        /// <summary>
        /// 变量观察参数改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void watchTree_AfterCellEdit(object sender, DevComponents.AdvTree.CellEditEventArgs e)
        {
            Node currentNode = e.Cell.Parent;
            EditForm editForm = this.ActiveMdiChild as EditForm;
            string varName = currentNode.Cells[0].Text;
            string varValue = currentNode.Cells[1].Text;
            
            switch (e.Cell.Tag.ToString())
            {
                case "varName":
                    {
                        varName = e.NewText;
                        break;
                    }
                case "varValue":
                    {
                        varValue = e.NewText;
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            if (!string.IsNullOrEmpty(varName) && editForm != null && startDebug)
            {
                switch (e.Cell.Tag.ToString())
                {
                    case "varName": // 改变量名
                        {
                            RefreshVarValue();

                            break;
                        }
                    case "varValue": // 改变量值
                        {
                            if (!lockVarValue) // 避免与刷新变量值产生冲突
                            {
                                if (varName.Contains("."))
                                {
                                    MessageBox.Show("不能直接修改对象的属性！", "修改变量", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                }
                                else
                                {
                                    PrintOutputText(string.Format("开始修改变量值 —— {0} = {1}", varName, varValue));
                                    string message = string.Format("set_var {0} {1}", varName, varValue);
                                    string receivedMessage = SendAndWaitMessage(message, string.Format("<ldb>set_var: {0}", varName), true);

                                    if (receivedMessage != null)
                                    {
                                        if (receivedMessage == string.Format("<ldb>set_var: {0} success!", varName))
                                        {
                                            currentNode.Cells[3].Text = "修改成功";                                            
                                            PrintOutputText("变量值修改成功...");
                                        }
                                        else if (receivedMessage == string.Format("<ldb>set_var: {0} failed!", varName))
                                        {
                                            currentNode.Cells[3].Text = "修改失败";                                            
                                            PrintOutputText("变量值修改失败...");
                                        }
                                    }
                                    else
                                    {
                                        currentNode.Cells[3].Text = "接收消息超时";                                        
                                        PrintOutputText("接收消息超时...");
                                    }
                                }
                            }

                            break;
                        }
                    default:
                        {
                            MessageBox.Show("只允许修改变量名和变量值！", "修改变量",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                            e.Cancel = true;
                            
                            break;
                        }
                }
            }          
        }

        /// <summary>
        /// 文本放入观察面板
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void watchTree_DragDrop(object sender, DragEventArgs e)
        {
            string varName = e.Data.GetData(Type.GetType("System.String")) as string;

            EditForm editForm = this.ActiveMdiChild as EditForm;
            string fileName = editForm.FileName;

            Node newNode = CreateVarNode();

            newNode.Cells[0].Text = varName;
            newNode.Cells[2].Text = fileName;

            watchTree.Nodes.Add(newNode);

            // 刷新一次变量看看
            if (startDebug)
            {
                RefreshVarValue();
            }
        }

        /// <summary>
        /// 文本拖入观察面板
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void watchTree_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(Type.GetType("System.String")))
            {
                e.Effect = DragDropEffects.Move;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        /// <summary>
        /// 加载本地目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bLoadLocalFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.Description = "请选择要加载脚本所在的目录";
            folderBrowserDialog.SelectedPath = Helper.ClientPath;

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                string folderName = folderBrowserDialog.SelectedPath;

                DirectoryInfo di = new DirectoryInfo(folderName);

                if (di.Exists)
                {
                    foreach (FileInfo fi in di.GetFiles())
                    {
                        if (fi.Extension == ".lua")
                        {
                            AddLocalFileNode(fi.FullName);
                        }
                    }
                }
            } 
        }

        /// <summary>
        /// 取消挂接脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDetachDebugFile_Click(object sender, EventArgs e)
        {
            DetachServerDebugFile();
            DetachClientDebugFile();

            MessageBox.Show("取消挂接调试脚本完成！", "取消挂接脚本",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }

}