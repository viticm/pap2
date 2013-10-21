using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;

using Fireball.Syntax;
using Fireball.Windows.Forms.CodeEditor;

namespace GameLuaEditor
{
    public partial class EditForm : Form
    {
        private string m_id = ""; // 数据库的唯一标识
        private string m_code = ""; // 缓存中的代码
        private string m_filename = ""; // 文件全名（含相对路经）
        private bool isLocked = false; // 记录当前编辑窗口是否被操作者本人锁定
        private Word lastHoveredWord = null; // 上一次查询的单词
        private IntelligentSenseManager intelligentSenseManager = IntelligentSenseManager.GetIntelligentSenseManager(); // 智能感知数据管理器

        // 处理断点相关的回调
        public delegate void BreakPointAdded(EditForm editForm, Row row);
        public delegate void BreakPointRemoved(EditForm editForm, Row row);
        private BreakPointAdded handleBreakPointAdded;
        private BreakPointRemoved handleBreakPointRemoved;

        // 处理取词的回调
        public delegate void WordMouseHover(EditForm editForm, ref WordMouseEventArgs e);
        private WordMouseHover handleWordMouseHover;

        // 脚本的类型
        private readonly string scriptType; // 脚本类型

        /// <summary>
        /// 取词的回调
        /// </summary>
        public WordMouseHover HandleWordMouseHover
        {
            set
            {
                handleWordMouseHover = value;
            }

            get
            {
                return handleWordMouseHover;
            }
        }

        /// <summary>
        /// 添加断点的回调
        /// </summary>
        public BreakPointAdded HandleBreakPointAdded
        {
            set
            {
                handleBreakPointAdded = value;
            }

            get
            {
                return handleBreakPointAdded;
            }
        }

        /// <summary>
        /// 删除断点的回调
        /// </summary>
        public BreakPointRemoved HandleBreakPointRemoved
        {
            set
            {
                handleBreakPointRemoved = value;
            }

            get
            {
                return handleBreakPointRemoved;
            }
        }

        /// <summary>
        /// 断点行号链表
        /// </summary>
        public List<int> BreakPointIndexList
        {
            get
            {
                return luaEditBox.BreakPointIndexList;
            }
        }

        /// <summary>
        /// 脚本内容是否改变
        /// </summary>
        public bool IsChanged
        {
            get 
            { 
                return this.ScriptCode != this.luaEditBox.Text; 
            }
        }

        /// <summary>
        /// 脚本文件路径
        /// </summary>
        public string FileName
        {
            get 
            { 
                return m_filename; 
            }

            set 
            { 
                this.m_filename = value;
                string strText = m_filename;

                if (strText.IndexOf('\\') != -1)
                {
                    strText = strText.Substring(strText.LastIndexOf('\\') + 1);
                }

                this.Text = strText;
            }
        }

        /// <summary>
        /// 当前选中的行号，从0开始
        /// </summary>
        public int SelectedLineIndex
        {
            get
            {
                return luaEditBox.SelectedLineNumber;
            }
        }

        /// <summary>
        /// 当前脚本文件的ID
        /// </summary>
        public string ID
        {
            get
            {
                return m_id;
            }

            set 
            { 
                m_id = value; 
            }
        }

        /// <summary>
        /// 赋值会重置both编辑区域and缓存， 获取会只取缓存
        /// </summary>
        public string ScriptCode
        {
            get 
            { 
                return m_code; 
            }

            set 
            { 
                m_code = value; 
                this.luaEditBox.Text = value; 
            }
        }

        /// <summary>
        /// 获取编辑区域内容
        /// </summary>
        public string NewScriptCode
        {
            get 
            { 
                return this.luaEditBox.Text; 
            }
        }

        /// <summary>
        /// 右键菜单
        /// </summary>
        public ContextMenuStrip EditorContextMenu
        {
            get 
            { 
                return luaEditBox.ContextMenuStrip; 
            }

            set 
            { 
                luaEditBox.ContextMenuStrip = value; 
            }
        }

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="scriptType">脚本类型</param>
        public EditForm(string scriptType)
        {
            this.scriptType = scriptType;
            InitializeComponent();
            Init();   
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            luaEditBox.ForWho = "GameLuaEditor";
            LuaRuntime.LuaInitEx();

            // 初始化虚拟机
            string strLuaFile = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\IntelliSense.lua");            

            if (File.Exists(strLuaFile))
            {
                StreamReader sr = new StreamReader(strLuaFile, Encoding.GetEncoding("gb2312"));
                string strOut = "";
                bool bret = LuaRuntime.LuaInit(sr.ReadToEnd(), ref strOut);
                sr.Close();
            }                       
        }

        /// <summary>
        /// 去掉断点
        /// </summary>
        /// <param name="lineIndex">断点所在的行号</param>
        public void RemoveBreakPoint(int lineIndex)
        {
            luaEditBox.SetBreakPoint(lineIndex, false);
        }

        /// <summary>
        /// 脚本内容改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEdit1_OnChange(object sender, EventArgs e)
        {
            string scriptName = m_filename;

            if (scriptName.IndexOf('\\') != -1)
            {
                scriptName = scriptName.Substring(scriptName.LastIndexOf('\\') + 1);
            }

            if (scriptType == "localFile") // 本地文件跳过加锁机制
            {
                this.Text = scriptName + "*";
                return;
            }

            if (isLocked) // 已经锁了，没人会再改掉服务器端了，忽略检查吧
            {
                return;
            }

            if (IsChanged) // 真的改了
            {                
                DataBaseManager databaseManager = DataBaseManager.GetDataBaseManager();
                string strServerContent = databaseManager.GetScriptData(this.m_id);

                if (strServerContent == null)
                {
                    MessageBox.Show("校验服务器端文件失败，可能服务器端文件已删除。", "锁定脚本", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                    this.luaEditBox.Text = "";
                    this.luaEditBox.Text = this.ScriptCode;

                    return;
                }

                if (strServerContent != this.ScriptCode)
                {
                    MessageBox.Show("服务器端的文件已被修改过，要重新加载最新内容！", "锁定脚本", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    this.ScriptCode = strServerContent;
                }

                // 加锁前先解锁 如果是自己加锁后程序崩溃了，再次启动编辑可以解自己以前加的锁。前提是自己不能unlock别人的锁（待考证）
                databaseManager.UnLockScript(this.ID);

                // 正式加锁
                bool bret = databaseManager.LockScript(this.m_id);

                if (bret)
                {
                    // 锁定成功，那么置修改标志于文件名后                    
                    this.Text = scriptName + "*";
                    this.isLocked = true;
                }
                else
                {
                    //锁定失败，说明被别人锁了(由于LockScript中包含报错界面，故我就不重复报错了再)
                    this.luaEditBox.Text = this.ScriptCode;
                }
            }
        }

        public void Reload()
        {
            if(IsChanged)
            {
                throw (new Exception("文件改变后reload无效")); 
            }
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            string strServerContent = dbm.GetScriptData(this.m_id);
            if(strServerContent != null)
            {
                this.ScriptCode = strServerContent;
            }
        }

        /// <summary>
        /// 编辑面板关闭
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void frmEdit_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (IsChanged)
            {
                if (MessageBox.Show(string.Format(" 内容已改变是否保存？", m_filename), "保存脚本", 
                                    MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    // 保存前查错
                    MainForm mainForm = this.MdiParent as MainForm;
                    string strOut = mainForm.ScriptCheck(luaEditBox.Text);

                    if (strOut != null)
                    {
                        strOut = strOut.TrimEnd('\n');
                    }                        

                    if (!string.IsNullOrEmpty(strOut))
                    {
                        MessageBox.Show(string.Format("{0} 脚本中包含错误：\r\n{1}", m_filename, strOut), "脚本检查",
                                        MessageBoxButtons.OK, MessageBoxIcon.Question);
                        TreePhOpera.SetNodeColorByID(mainForm.treePh, this.ID, Color.Red);
                    }
                    else
                    {
                        TreePhOpera.SetNodeColorByID(mainForm.treePh, this.ID, Color.Black);
                    }

                    // 保存
                    DoSave();
                }
                else
                {
                    // 不保存
                    DataBaseManager databaseManager = DataBaseManager.GetDataBaseManager();
                    databaseManager.UnLockScript(this.ID);

                    this.isLocked = false;
                }
            }
        }

        /// <summary>
        /// 保存脚本
        /// </summary>
        /// <returns>是否保存成功</returns>
        public bool DoSave()
        {
            bool saveSuccess = false;

            if (IsChanged) // 需要保存
            {
                string scriptName = m_filename;

                if (scriptName.IndexOf('\\') != -1)
                {
                    scriptName = scriptName.Substring(scriptName.LastIndexOf('\\') + 1);
                }

                switch (scriptType)
                {
                    case "databaseFile": // 数据库脚本文件
                        {
                            DataBaseManager databaseManager = DataBaseManager.GetDataBaseManager();
                            string strView = "";
                            bool bret = databaseManager.SaveScriptData(this.FileName, this.NewScriptCode, out strView);

                            if (bret)
                            {
                                if (strView != this.NewScriptCode)
                                {
                                    this.ScriptCode = databaseManager.GetBeforeOpen(strView, FileName);
                                }

                                this.ScriptCode = this.NewScriptCode;
                                

                                this.Text = scriptName;
                                databaseManager.UnLockScript(this.ID);
                                bret = true;
                                this.isLocked = false;
                            }

                            saveSuccess = bret;

                            break;
                        }
                    case "localFile": // 本地脚本文件
                        {
                            Helper.WriteStringToFile(NewScriptCode, m_filename);
                            this.ScriptCode = this.NewScriptCode;
                            this.Text = scriptName;
                            saveSuccess = true;

                            break;
                        }
                    default:
                        {
                            break;
                        }
                }                
            }

            return saveSuccess;
        }

        /// <summary>
        /// 智能感知
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEdit1_queryObjects(object sender, luaEditor.QueryEventsArgs e)
        {
            string strOut = "";
            IntPtr ls_out = IntPtr.Zero;

            // 参数规定：
            // szCode:   所有的代码
            // nPos:     光标位置（汉字按2个字符算）
            // nIsDelete 0为正常输入，1为删除，
            // 11-22为F1-F12,
            // 1111代表Ctrl+Shift+F1,
            // 1011代表Ctrl+F1
            // 111代表Shift+F1

            int nIsDelete = e.nPos > 0 ? 0 : 1;

            if (e.keyword != "")
            {
                nIsDelete = 10 + Int32.Parse(e.keyword);
            }            
                        
            bool bret = LuaRuntime.LuaRun("OnCodeSense", e.szCode, Math.Abs(e.nPos), nIsDelete, Helper.GameLuaEditorSetting, ref ls_out);
            strOut = Marshal.PtrToStringAnsi(ls_out);            

            if (strOut == null)
            {
                return;
            }

            string[] as_data = strOut.Split(new char[] { '|' });

            if(as_data.Length < 3)
            {
                MessageBox.Show("返回值不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 读取overlen
            e.nOverLen = Int32.Parse((as_data[0]));

            // 初始化游标
            int nPosition = 3;

            // 读取list
            int nListCount = Int32.Parse((as_data[1]));

            if (as_data.Length < 3 + nListCount)
            {
                MessageBox.Show("List返回值个数不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }      

            for(int i = 0; i < nListCount; i++ )
            {
                string s_name = as_data[nPosition++] as string;
                string s_word = as_data[nPosition++] as string;
                string s_type = as_data[nPosition++] as string;
                string s_info = as_data[nPosition++] as string;
                e.leTable.Add(s_name, new luaEditor.LuaEditorTableItem(s_name, s_type, s_word, s_info));
            }

            // 读取info
            int nInfoCount = Int32.Parse((as_data[2]));

            if (as_data.Length < 3 + nListCount + nInfoCount)
            {
                MessageBox.Show("Info返回值个数不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            for (int i = 0; i < nInfoCount; i++)
            {
                string s_text = as_data[nPosition++] as string;
                e.parms_list.Add(s_text.Replace("\n", "<BR>"));
            }
        }

        /// <summary>
        /// 添加断点事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditBox_OnBreakPointAdded(object sender, Fireball.Syntax.RowEventArgs e)
        {
            if (handleBreakPointAdded != null)
            {
                handleBreakPointAdded(this, e.Row);
            }
        }

        /// <summary>
        /// 删除断点事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditBox_OnBreakPointRemoved(object sender, Fireball.Syntax.RowEventArgs e)
        {
            if (handleBreakPointRemoved != null)
            {
                handleBreakPointRemoved(this, e.Row);
            }
        }

        /// <summary>
        /// 鼠标取词事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditBox_OnWordMouseHover(object sender, ref Fireball.Windows.Forms.CodeEditor.WordMouseEventArgs e)
        {
            Word hoveredWord = e.Word;

            if (e.Word.Index < e.Word.Row.Count - 1) // 行末尾的单词定位不准，暂时取消掉取词
            {
                if (hoveredWord != lastHoveredWord) // 同一个单词只显示一次Tip就够了
                {                   
                    if (handleWordMouseHover != null)
                    {
                        handleWordMouseHover(this, ref e);
                    }

                    lastHoveredWord = hoveredWord;                   
                }
            }
        }
    }
    
    /// <summary>
    /// Lua虚拟机
    /// </summary>
    public static class LuaRuntime
    {
        public static void LuaInitEx()
        {
            
        }

        [DllImport("Plugins\\Luacheck\\LuaRuntime.dll", EntryPoint = "LuaInit", CharSet = CharSet.Ansi)]
        public static extern bool LuaInit(string strCode, ref string szOutput);

        [DllImport("Plugins\\Luacheck\\LuaRuntime.dll", EntryPoint = "LuaRun", CharSet = CharSet.Ansi)]
        public static extern bool LuaRun(string szFuncName, string szCode, int nPos, int nIsDelete, string szSetting, ref IntPtr szOutput);
    };
}