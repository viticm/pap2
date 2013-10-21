using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

using Fireball.CodeEditor.SyntaxFiles;
using Fireball.Windows.Forms;
using Fireball.Syntax;
using Fireball.Windows.Forms.CodeEditor;

namespace luaEditor
{
    public partial class luaEditorControl : UserControl
    {
        public string ForWho = "";
        private string m_curword = ""; // keyword前的词语
        private string m_list_context = "";

        private Color normalWordColor = Color.Gray; // 普通单词颜色
        private Color commentWordColor = Color.Green; // 注释单词颜色
        private Color fieldWordColor = Color.Purple; // 字段单词颜色
        private Color stringWordColor = Color.Magenta; // 字符串单词颜色
        private Color functionWordColor = Color.Brown; // 方法单词颜色
        private Color gameObjectWordColor = Color.DeepSkyBlue; // 游戏对象单词颜色
        private TextStyle fieldWordStyle = new TextStyle(); // 字段单词配置
        private TextStyle functionWordStyle = new TextStyle(); // 方法单词配置
        private TextStyle gameObjectWordStyle = new TextStyle(); // 游戏对象单词配置
        private string gameObjectDescription = "npc|doodad|player|scene|item|skill"; // 游戏对象描述  
        private int markWordDelay = 1000; // 标记单词延迟
        private Timer markWordTimer = new Timer(); // 标记单词用的计时器
        private bool markingWord = false; // 是否正在标记单词
        private char[] invalidChartArray = new char[] { ' ', '\t' }; // 无效字符数组
        private string[] switchRowKeyWordArray = new string[] { "function ", "local function ", "if ", "for " }; // 换行关键字数组
        private string[] queryObjectStringArray = new string[] { "+" }; // 智能感知字符串分隔符数组

        // 断点相关事件
        public event BreakPointAdded OnBreakPointAdded;
        public event BreakPointRemoved OnBreakPointRemoved;

        // 取词事件
        public event WordMouseHover OnWordMouseHover;

        // 查询Events
        public event QueryEventsHandle queryEvents;

        // 查询objects
        public event QueryObjectHandle queryObjects;

        // 查询parms
        public event QueryParmsHandle queryParms;
        public event OnChangeHandle OnChange;

        public string strNs = "";

        // 画面状态
        private InputState inputState = InputState.NORMAL;
        private bool m_changefunc = false;

        // 自定义消息
        private const int WM_CW_SCANCODE_FALSE = 0x500 + 3000;

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, string lParam);

        /// <summary>
        /// 构造函数
        /// </summary>
        public luaEditorControl()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 方法单词颜色
        /// </summary>
        public Color FunctionWordColor
        {
            set
            {
                functionWordColor = value;
            }

            get
            {
                return functionWordColor;
            }
        }

        /// <summary>
        /// 游戏对象单词颜色
        /// </summary>
        public Color GameObjectWordColor
        {
            set
            {
                gameObjectWordColor = value;
            }

            get
            {
                return gameObjectWordColor;
            }
        }

        /// <summary>
        /// 游戏对象描述
        /// </summary>
        public string GameObjectDescription
        {
            set
            {
                gameObjectDescription = value;
            }

            get
            {
                return gameObjectDescription;
            }
        }

        /// <summary>
        /// 选中的文本
        /// </summary>
        public string SelectedText
        {
            get 
            { 
                return codeEditorControl.Selection.Text; 
            }
        }

        /// <summary>
        /// 选中的单词
        /// </summary>
        public string SelectedWord
        {
            get 
            {
                if (codeEditorControl.Caret.CurrentWord != null)
                {
                    return codeEditorControl.Caret.CurrentWord.Text;
                }
                else
                {
                    return "";
                }
            }
        }

        /// <summary>
        /// 选中的行号
        /// </summary>
        public int SelectedLineNumber
        {
            get 
            { 
                return codeEditorControl.Caret.CurrentRow.Index; 
            }
        }

        /// <summary>
        /// 选中的行文本
        /// </summary>
        public string SelectedLineText
        {
            get 
            { 
                return codeEditorControl.Caret.CurrentRow.Text; 
            }
        }

        /// <summary>
        /// 脚本的字体
        /// </summary>
        public Font ScriptFont
        {
            set
            {
                codeEditorControl.FontName = value.Name;
                codeEditorControl.FontSize = value.Size;
            }

            get
            {
                return codeEditorControl.Font;
            }
        }

        /// <summary>
        /// 脚本的字体颜色
        /// </summary>
        public Color ScriptForeColor
        {
            set
            {
                codeEditorControl.ForeColor = value;
            }

            get
            {
                return codeEditorControl.ForeColor;
            }
        }

        /// <summary>
        /// 取到断点
        /// </summary>
        public List<int> BreakPointIndexList
        {
            get
            {
                List<int> list = new List<int>();                               

                for (int i = 0; i < codeEditorControl.Document.VisibleRows.Count; i++)
                {
                    if (codeEditorControl.Document.VisibleRows[i].Breakpoint)
                    {
                        list.Add(i + 1); // lua行号从1开始
                    }
                }

                return list;
            }
        }      
        
        /// <summary>
        /// 调试行号
        /// </summary>
        public int DebugRowIndex
        {
            set
            {
                codeEditorControl.ActiveViewControl.DebugRowIndex = value;
            }

            get
            {
                return codeEditorControl.ActiveViewControl.DebugRowIndex;
            }
        }

        /// <summary>
        /// 是否只读
        /// </summary>
        public bool ReadOnly
        {
            set
            {
                codeEditorControl.ReadOnly = value;
            }

            get
            {
                return codeEditorControl.ReadOnly;
            }
        }

        /// <summary>
        /// 右键菜单
        /// </summary>
        public ContextMenuStrip ContentMenu
        {
            get 
            { 
                return codeEditorControl.ContextMenuStrip; 
            }

            set 
            { 
                codeEditorControl.ContextMenuStrip = value; 
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化单词配置
            fieldWordStyle.BackColor = Color.Transparent;
            fieldWordStyle.ForeColor = fieldWordColor;
            fieldWordStyle.Bold = false;
            fieldWordStyle.Italic = false;
            fieldWordStyle.Underline = false;

            functionWordStyle.BackColor = Color.Transparent;
            functionWordStyle.ForeColor = functionWordColor;
            functionWordStyle.Bold = false;
            functionWordStyle.Italic = false;
            functionWordStyle.Underline = false;

            gameObjectWordStyle.BackColor = Color.Transparent;
            gameObjectWordStyle.ForeColor = gameObjectWordColor;
            gameObjectWordStyle.Bold = false;
            gameObjectWordStyle.Italic = false;
            gameObjectWordStyle.Underline = false;

            // 初始化标记单词计时器
            markWordTimer.Interval = markWordDelay;
            markWordTimer.Tick += MarkWord;

            // 转发事件回调
            codeEditorControl.Document.BreakPointAdded += new RowEventHandler(luaEditorBreakPointAdded);
            codeEditorControl.Document.BreakPointRemoved += new RowEventHandler(luaEditorBreakPointRemoved);
            codeEditorControl.WordMouseHover += new WordMouseHandler(luaEditorWordMouseHover);
        }

        /// <summary>
        /// 处理添加断点事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditorBreakPointAdded(object sender, RowEventArgs e)
        {
            if (OnBreakPointAdded != null)
            {
                OnBreakPointAdded(sender, e);
            }            
        }

        /// <summary>
        /// 选中文本
        /// </summary>
        /// <param name="RowIndex">行号</param>
        /// <param name="Column">列号</param>
        /// <param name="Length">文本长度</param>
		public void SelectText(int RowIndex, int Column, int Length)
        {
            codeEditorControl.SelectText(RowIndex, Column, Length);
        }

        public void SetupFindText()
        {
            codeEditorControl.SetupFindText();
        }

        public void FindNextSelectText()
        {
            codeEditorControl.FindNextSelectText();
        }

        public void FindPreviousSelectText()
        {
            codeEditorControl.FindPreviousSelectText();
        }

        /// <summary>
        /// 处理删除断点事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditorBreakPointRemoved(object sender, RowEventArgs e)
        {
            if (OnBreakPointRemoved != null)
            {
                OnBreakPointRemoved(sender, e);
            }
        }

        /// <summary>
        /// 处理取词事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void luaEditorWordMouseHover(object sender, ref WordMouseEventArgs e)
        {
            if (OnWordMouseHover != null)
            {
                OnWordMouseHover(sender, ref e);
            }
        }

        /// <summary>
        /// 清理debug标记
        /// </summary>
        public void ClearDebugMark()
        {
            codeEditorControl.ActiveViewControl.ClearDebugMark();
        }

        /// <summary>
        /// 开启/关闭断点
        /// </summary>
        /// <param name="lineIndex">断点所在行号</param>
        /// <param name="enable">是否开启</param>
        public void SetBreakPoint(int lineIndex, bool enable)
        {
            codeEditorControl.Document.VisibleRows[lineIndex].Breakpoint = enable;
        }

        /// <summary>
        /// 设置提示文本
        /// </summary>
        /// <param name="tipText">文本内容</param>
        /// <param name="location">位置</param>
        public void SetTipText(string tipText, TextPoint location)
        {
            codeEditorControl.InfoTipPosition = location;
            codeEditorControl.InfoTipCount = 1;
            codeEditorControl.InfoTipSelectedIndex = 0;
            codeEditorControl.InfoTipText = tipText;
            codeEditorControl.InfoTipVisible = true;
        }

        /// <summary>
        /// 关闭提示文本
        /// </summary>
        public void CloseTipText()
        {
            codeEditorControl.InfoTipVisible = false;
        }

        /// <summary>
        /// 切换到执行行
        /// </summary>
        /// <param name="nLine">行号</param>
        public void MoveToLine(int nLine)
        {
            this.codeEditorControl.GotoLine(nLine - 1);
        }

        /// <summary>
        /// 编辑光标切换到位置
        /// </summary>
        /// <param name="nX">x坐标</param>
        /// <param name="nY">y坐标</param>
        public void MoveToPos(int nX, int nY)
        {
            this.codeEditorControl.Caret.SetPos(new TextPoint(nX, nY));
            this.codeEditorControl.Caret.CurrentRow.EnsureVisible();
        }


        /// <summary>
        /// 文本内容
        /// </summary>
        public override string Text
        {
            get
            {
                return codeEditorControl.Document.Text;
            }

            set
            {
                cmbFunctionList.Items.Clear();
                codeEditorControl.Document.Text = value;
                QueryEventsArgs qe_arg = new QueryEventsArgs();
                qe_arg.leec = new leEventConnection();

                if (queryEvents != null)
                {
                    queryEvents(this, qe_arg); // 初始化，先回调获取Event列表
                }

                foreach (LuaEditorEvent le in qe_arg.leec.m_list)
                {
                    foreach (LuaEditorParameter lp in le.parms)
                    {
                        ParameterCache.leParm_Cache.Add(lp);
                    }
                }
                
                CodeScan();

                // 开始标记单词
                BeginMarkWord();
            }
        }

        /// <summary>
        /// 开始标记单词
        /// </summary>
        private void BeginMarkWord()
        {
            if (!markingWord)
            {
                markWordTimer.Start();
                markingWord = true;
            }            
        }

        /// <summary>
        /// 标记单词
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void MarkWord(object sender, EventArgs e)
        {
            Timer timer = sender as Timer;

            MarkWord();
            timer.Stop();
            markingWord = false;
        }

        /// <summary>
        /// 标记单词
        /// </summary>
        private void MarkWord()
        {
            codeEditorControl.Document.ParseSome(codeEditorControl.Document.Count);

            foreach (Row row in codeEditorControl.Document.VisibleRows)
            {
                MarkWord(row);
            }
        }

        /// <summary>
        /// 检查是否是普通单词
        /// </summary>
        /// <param name="word">单词</param>
        /// <returns>是否是普通单词</returns>
        private bool IsNormalWord(Word word)
        {
            bool isNormal = false;

            if (word.Style != null)
            {
                if (word.Style.ForeColor == normalWordColor)
                {
                    isNormal = true;
                }
            }

            return isNormal;
        }

        /// <summary>
        /// 标记单词
        /// </summary>
        /// <param name="row">数据行</param>
        private void MarkWord(Row row)
        {            
            string content = row.Text;
            content = content.Trim(new char[] { '\t', ' ' });

            // 标记字段
            string regexPattern = @"(\.(?<MatchWord>\w+))";
            List<string> fieldWordList = GetWordListInPattern(content, regexPattern);

            // 标记Function
            regexPattern = @"((\.|\s)*(?<MatchWord>\w+)\()";            
            List<string> functionWordList  = GetWordListInPattern(content, regexPattern);

            // 标记游戏对象
            regexPattern = string.Format(@"((?<MatchWord>{0}))", gameObjectDescription);
            List<string> gameObjectWordList = GetWordListInPattern(content, regexPattern);

            foreach (Word word in row.FormattedWords)
            {
                if (gameObjectWordList.Contains(word.Text) && IsNormalWord(word))
                {
                    word.Style = gameObjectWordStyle;
                }
                else if (functionWordList.Contains(word.Text) && IsNormalWord(word))
                {
                    word.Style = functionWordStyle;
                }
                else if (fieldWordList.Contains(word.Text) && IsNormalWord(word))
                {
                    word.Style = fieldWordStyle;
                }
            }          
        }        

        /// <summary>
        /// 获取匹配正则表达式的单词
        /// </summary>
        /// <param name="content">文本内容</param>
        /// <param name="pattern">正则表达式</param>
        /// <returns>单词</returns>
        private List<string> GetWordListInPattern(string content, string pattern)
        {
            List<string> wordList = new List<string>();            
            Regex r = new Regex(pattern, RegexOptions.Singleline);

            if (r.IsMatch(content))
            {
                foreach (Match match in r.Matches(content))
                {
                    wordList.Add(match.Result("${MatchWord}"));
                }              
            }

            return wordList;
        }

        public void regObject(string name, Type type, System.Reflection.MethodInfo mi, string strNs)
        {
            LuaEditorParameter lp = new LuaEditorParameter(name, type, mi, strNs);
            ParameterCache.leParm_Cache.Add(lp);
        }

        public void clearObject()
        {
            ParameterCache.leParm_Cache.Clear();
            ParameterCache.asmCache.Clear();
        }

        /// <summary>
        /// 扫描一行语句，模拟赋值，绑定未知对象到已知对象的类型
        /// </summary>
        /// <param name="sLine"></param>
        /// <param name="strNs"></param>
        private void CodeLineScan(string sline, string strNs)
        {
            //忽略注释
            if (sline.IndexOf("--") != -1)
            {
                string[] asp = { "--" };
                sline = sline.Split(asp, StringSplitOptions.None)[0];
            }
            sline = sline.Trim();
            if (sline == "") return;

            //单独处理luanet.load_assembly()
            if(GetObjectFullName(sline) == "luanet.load_assembly()")
            {
                char[] asp = { '\"', '\''};
                if (sline.Trim().Split(asp).Length < 2) return;
                string dllname = sline.Trim().Split(asp)[1];
                ParameterCache.loadDll(dllname);
            }

            //寻找赋值语句和函数执行语句（暂时的规则是，不多于一个=号，括号匹配，单双引号匹配）
            if (sline.Length > 2 &&
                (!(sline.StartsWith("="))) &&
                sline.Length - sline.Replace("=", "").Length == 1 &&
                sline.Replace("(", "").Length == sline.Replace(")", "").Length &&
                sline.Replace("{", "").Length == sline.Replace("}", "").Length &&
                sline.Replace("[", "").Length == sline.Replace("]", "").Length &&
                (sline.Replace("'", "").Length - sline.Length) % 2 == 0 &&
                (sline.Replace("\"", "").Length - sline.Length) % 2 == 0
                )
            {
                string sleft = "", sright = "";
                if (sline.IndexOf('=') == -1)
                {
                    sright = sline.Trim();
                }
                else
                {
                    sleft = sline.Substring(0, sline.IndexOf('=') - 1).Trim();
                    sright = sline.Substring(sline.IndexOf('=') + 1).Trim();
                }

                //对右侧表达式进行分析 如 abc.e(ert.s(),2).value
                for (int i = 0; i < sright.Length; i++)
                {
                    string m = sright.Substring(i, 1);
                    if (m == "." || m == ":")
                    {
                        //取对象名，送全局缓存，让它去反射去吧
                        string s = GetObjectFullName(sright.Substring(0, i));
                        ParameterCache.findMethodInfoByFullName(s, strNs);
                    }
                }

                if (sleft != "")
                {
                    //单独处理 [local] var = luanet.import_type('VisualEditor.Doit')情况
                    if(GetObjectFullName(sright) == "luanet.import_type()")
                    {
                        string leftns = strNs;
                        if (sleft.IndexOf("local ") == -1)  leftns = "";
                        sleft = sleft.Replace("local ", "");
                        char[] asp = { '\"', '\'' };
                        string classname = sright.Trim().Split(asp)[1];
                        if (classname != "" && classname != ")")
                        {
                            //之所以现在不查出来type直接赋了，是考虑到，万一用户写错了dll名字，还可以修正
                            //修正完以后，让他点“.”的时候，才load那个dll。一次load不到，就每“.”都load。
                            ParameterCache.leParm_Cache.Add(new LuaEditorParameter(sleft.Trim() + "()", classname, leftns));  //加这个是为了让得到返回值的类型
                            ParameterCache.leParm_Cache.Add(new LuaEditorParameter(sleft.Trim(), classname, leftns));         //而这个是为了让得到构造函数的参数
                        }
                    }
                    //其他正常情况
                    else
                    {
                        //看来我得给赋值一下了 :)
                        if (sleft.IndexOf("local ") != -1)
                        {
                            //私有变量
                            sleft = sleft.Replace("local ", "");
                            ParameterCache.bindTwoObjects(GetObjectFullName(sleft), strNs, GetObjectFullName(sright), strNs);
                        }
                        else
                        {
                            //全局变量
                            ParameterCache.bindTwoObjects(GetObjectFullName(sleft), "", GetObjectFullName(sright), strNs);
                        }
                    }
                }
            }

        }


        /// <summary>
        /// 代码扫描，代码扫描，猜出各种对象的类型，各种函数的返回值等等。默认为深扫描。
        /// </summary>
        private void CodeScan()
        {
            CodeScan(true);
        }

        /// <summary>
        /// 代码扫描，代码扫描，猜出各种对象的类型，各种函数的返回值等等。
        /// </summary>
        /// <param name="scanSyn">是否深度扫描,深扫描包括语句解释，浅扫描仅扫描到函数定义</param>
        public void CodeScan(bool scanSyn)
        {
            int curLine = -1;
            string strNs = "", lastNs = "";

            //记下旧值
            string oldfunc = cmbFunctionList.Text;
            cmbFunctionList.Items.Clear();

            foreach(string sline1 in codeEditorControl.Document.Lines)
            {
                curLine++;
                string sline = sline1.Replace("\r", "");
                Segment seg = codeEditorControl.Document.GetSegmentFromPos(new TextPoint(0, curLine));
                strNs = GetNameSpace(seg);
                lastNs = strNs;

                if (scanSyn)
                {
                    CodeLineScan(sline, strNs);
                }

                //加入到functionlist
                if (sline.IndexOf("function ") != -1)
                {
                    AddToFunctionList(sline, curLine);
                }
            }

            // 扫描完了，如果有，则换回旧值
            foreach (FunctionList fl in cmbFunctionList.Items)
            {
                if (fl.ToString() == oldfunc)
                {
                    cmbFunctionList.SelectedItem = fl;
                    break;
                }
            }
        }
        
        /// <summary>
        /// 消息处理
        /// </summary>
        /// <param name="m">消息内容</param>
        protected override void WndProc(ref Message message)
        {
            switch(message.Msg)
            {
                case WM_CW_SCANCODE_FALSE:
                    {
                        CodeScan(false);

                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            base.WndProc(ref message);
        }

        /// <summary>
        /// 加入到函数列表，重复的更新，没有的加
        /// </summary>
        /// <param name="strLine"></param>
        /// <param name="nLineNumber"></param>
        private void AddToFunctionList(string strLine, int nLineNumber)
        {
            ++nLineNumber;
            string new_func_name = strLine.Replace("local function ", "").Trim();
            new_func_name = new_func_name.Replace("function ", "").Trim();
            if (new_func_name.IndexOf('(') == -1) return;
            new_func_name = new_func_name.Substring(0, new_func_name.IndexOf('(')).Trim();

            for( int i = 0; i <cmbFunctionList.Items.Count; i++ )
            {
                FunctionList fl = (FunctionList)cmbFunctionList.Items[i];

                if (fl.FunctionName == new_func_name)
                {
                    if (fl.LineNumber != nLineNumber || fl.FullText != strLine)
                    {
                        cmbFunctionList.Items[i] = new FunctionList(new_func_name, strLine, nLineNumber);
                    }

                    return;
                }
            }

            cmbFunctionList.Items.Add(new FunctionList(new_func_name, strLine, nLineNumber));
        }

        /// <summary>
        /// 取完整对象名，把 obj.func(obj1:method(xx.a(), x).value 变成 obj1:method().value
        /// </summary>
        /// <param name="sexp">待分析语句</param>
        /// <returns></returns>
        private string GetObjectFullName(string sexp)
        {
            string keyword = "";
            try
            {
                string keyW = sexp;

                //去方法的括号 把 obj.func(obj1:method(xx.a(), x).value 变成 obj1:method().value
                int nleft = 0, nRight = 0;
                string keyW2 = "";
                for (int i = keyW.Length; i > 0; i--)
                {
                    string m = keyW.Substring(i - 1, 1);
                    if (m == "(") nleft++;
                    if (m == ")") nRight++;
                    if (nleft > nRight) break;
                    if ((m == ")") && nRight - nleft < 2) keyW2 = m + keyW2;
                    else if (nleft == nRight)
                    {
                        keyW2 = m + keyW2;
                    }
                }
                keyW = keyW2;

                keyW2 = "";
                nleft = 0;
                nRight = 0;
                for (int i = keyW.Length; i > 0; i--)
                {
                    string m = keyW.Substring(i - 1, 1);
                    if (m == "[") nleft++;
                    if (m == "]") nRight++;
                    if (nleft > nRight) break;
                    if ((m == "]") && nRight - nleft < 2) keyW2 = m + keyW2;
                    else if (nleft == nRight)
                    {
                        keyW2 = m + keyW2;
                    }
                }
                keyW = keyW2;


                //去前面的部分，只留完整对象名
                char[] ac_sp = { ' ', '=', '\t', '!', '@', '#', '$', '%', '^', '&', '*', '+', '-', '/', '\\', ',' };
                string[] ss = keyW.Split(ac_sp);
                keyword = ss[ss.Length - 1];

                string[] ac_sp1 = { ".." };
                string[] ss1 = keyword.Split(ac_sp1, StringSplitOptions.None);
                keyword = ss1[ss1.Length - 1];                
            }
            catch
            {
                keyword = "";
            }
            return (keyword);
        }

        /// <summary>
        /// 返回完整函数名，以及光标所在参数的位置
        /// 把 obj.func(obj1:method(xx.a(b()), 变成 obj1:method##2
        /// </summary>
        /// <param name="sexp">本行中光标以左的所有字符</param>
        /// <returns></returns>
        private string GetFunctionFullNameAndTurns(string sexp)
        {
            string keyword = "";
            try
            {
                string keyW = sexp;

                //去函数的其他参数 把 obj.func(obj1:method(xx.a(b()), 变成 obj1:method##2
                int nleft = 0, nRight = 0, nRet = 1;
                string keyW2 = "";
                for (int i = keyW.Length; i > 0; i--)
                {
                    string m = keyW.Substring(i - 1, 1);
                    if (nleft > nRight) 
                        keyW2 = m + keyW2;
                    if (m == "(") 
                        nleft++;
                    if (m == ")") 
                        nRight++;
                    if (nleft == nRight && m == ",") 
                        nRet++;
                }
                keyW = GetObjectFullName(keyW2);
                keyword = keyW + "##" + nRet.ToString();
            }
            catch
            {
                keyword = "";
            }
            return (keyword);
        }


        /// <summary>
        /// 根据段落，获取ns名，也就是函数名
        /// </summary>
        /// <param name="seg"></param>
        /// <returns>函数名</returns>
        private string GetNameSpace(Segment seg)
        {
            string strNs = "";
            
            if (seg != null)
            {
                if (seg.Parent != null)
                {
                    while (seg.Parent.Scope != null)
                    {
                        seg = seg.Parent;
                    }
                }

                strNs = seg.StartRow.Text;
                
                if (seg.EndRow != null)
                {
                    int index = strNs.LastIndexOf('(');

                    if (index == -1)
                    {
                        return "";
                    }

                    strNs = strNs.Substring(0, strNs.LastIndexOf('('));

                    index = strNs.LastIndexOf(' ');

                    if (index == -1)
                    {
                        return "";
                    }

                    strNs = strNs.Substring(index).Trim();
                }
                else
                {
                    strNs = "";
                }                
            }

            return strNs;
        }        

        /// <summary>
        /// 按键事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CodeEditor_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (this.OnChange != null)
            {
                this.OnChange(this, new EventArgs());
            }

            if (this.codeEditorControl.Caret.CurrentWord != null)
            {
                string s = this.codeEditorControl.Caret.CurrentWord.Text;
                m_curword = s == null ? "" : s;
            }
            else
            {
                m_curword = "";
            }

            if (e.KeyChar > 128)
            {
                return;
            }

            if (this.ForWho == "GameLuaEditor")
            {
                ShowAutoList4GameLuaEditor(e, this.codeEditorControl);
            }
            else
            {
                ShowAutoList(e, this.codeEditorControl);
            }            
        }

        /// <summary>
        /// 键盘按下事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CodeEditor_KeyDown(object sender, KeyEventArgs e)
        {
            // 触发按键事件
            this.OnKeyDown(e);

            if (this.ForWho == "GameLuaEditor")
            {
                if ((e.KeyCode== Keys.Left || e.KeyCode == Keys.Right) && e.Alt == false)
                {
                    //如果是移动光标，那就关闭列表
                    codeEditorControl.AutoListVisible = false;
                    codeEditorControl.InfoTipVisible = false;
                }

                Keys[] fkey = new Keys[] { Keys.F1, Keys.F2, Keys.F3, Keys.F4, Keys.F5, Keys.F6, Keys.F7, Keys.F8, Keys.F9, Keys.F10, Keys.F11, Keys.F12};

                for (int i = 0; i < fkey.Length; i++)
                {
                    if (e.KeyCode == fkey[i])
                    {
                        // 强制规定，如果ascii码>130，那么00131为F1，00132为F2。。。。。
                        // XY131 X代表Ctrl，Y代表Shift
                        int nadd = 0;
                        if (e.Control) nadd += 1000;
                        if (e.Shift) nadd += 100;
                        ShowAutoList4GameLuaEditor(new KeyPressEventArgs((char)(nadd + 131 + i)), this.codeEditorControl);
                        break;
                    }
                }
            }

            // 开始标记单词
            BeginMarkWord();
        }

        /// <summary>
        /// 显示提示选项
        /// </summary>
        /// <param name="eKey">按键信息</param>
        /// <param name="codeEditor">当前编辑控件</param>
        private void ShowAutoList4GameLuaEditor(KeyPressEventArgs eKey, CodeEditorControl codeEditor)
        {
            // 强制规定，如果ascii码>130，那么131为F1，132为F2。。。。。
            // 不管三七二十一，先关闭列表再说
            codeEditor.AutoListVisible = false;
            codeEditor.InfoTipVisible = false;

            //计算当前输入的东东以及位置
            int nOffset = 0;
            QueryEventsArgs queryEventsArgs = new QueryEventsArgs();

            if (eKey.KeyChar != '\b' && eKey.KeyChar <= 128)
            {
                queryEventsArgs.szCode = string.Format("{0}{1}{2}", 
                                                       codeEditor.Document.Text.Substring(0, codeEditor.Selection.LogicalSelStart),
                                                       eKey.KeyChar,
                                                       codeEditor.Document.Text.Substring(codeEditor.Selection.LogicalSelStart));
                queryEventsArgs.nPos = codeEditor.Selection.LogicalSelStart + 1;

                nOffset++;
            }
            else
            {
                queryEventsArgs.szCode = codeEditor.Document.Text;
                queryEventsArgs.nPos = codeEditor.Selection.LogicalSelStart;
            }

            // 如果什么也没输入，只是按下了热键
            if (eKey.KeyChar > 128)
            {
                int ikey = (int)eKey.KeyChar - 130;
                queryEventsArgs.keyword = ikey.ToString();
            }

            // 由于Lua很变态，认为汉字的长度为2，所以我得手工看看有多少中文，然后加在长度上
            int nAdd = 0;

            for (int i = 0; i < queryEventsArgs.nPos; i++ )
            {
                if (queryEventsArgs.szCode[i] > 128)
                {
                    nAdd++;
                }
            }
            queryEventsArgs.nPos += nAdd;

            if (eKey.KeyChar == '\b')
            {
                queryEventsArgs.nPos *= -1;
            }

            // 获取新列表
            if (queryObjects != null && !codeEditorControl.ReadOnly) // 调试模式下不用启动自动完成功能
            {
                queryObjects(this, queryEventsArgs);
                bool querySuccess = false;

                if (queryEventsArgs.parms_list.Count > 0)
                {
                    codeEditor.InfoTipPosition = new TextPoint(codeEditor.Caret.Position.X + 1, codeEditor.Caret.Position.Y);
                    codeEditor.InfoTipCount = queryEventsArgs.parms_list.Count;

                    for (int i = 0; i < queryEventsArgs.parms_list.Count; i++)
                    {
                        codeEditor.InfoTipSelectedIndex = i;
                        string argIn = queryEventsArgs.parms_list[i];
                        codeEditor.InfoTipText = argIn;
                    }

                    codeEditor.InfoTipVisible = true;
                    querySuccess = true;
                }

                if (queryEventsArgs.leTable.Count > 0)
                {
                    codeEditor.AutoListBeginLoad();
                    codeEditor.AutoListClear();

                    foreach (LuaEditorTableItem tableItem in queryEventsArgs.leTable.Values)
                    {
                        codeEditor.AutoListAdd(tableItem.Name, tableItem.ItemValue.ToString(), tableItem.Info, int.Parse(tableItem.ItemType));                        
                    }

                    codeEditor.AutoListEndLoad();
                    codeEditor.AutoListPosition = new TextPoint(codeEditor.Caret.Position.X + nOffset, codeEditor.Caret.Position.Y);
                    codeEditor.AutoListPosition.X -= queryEventsArgs.nOverLen;

                    if (codeEditor.AutoListPosition.X < 0)
                    {
                        codeEditor.AutoListPosition.X = 0;
                    }

                    codeEditor.AutoListVisible = true;
                    querySuccess = true;
                }

                switch (eKey.KeyChar)
                {
                    case '\r':
                        {
                            IndentText(codeEditor);                           

                            break;
                        }
                        
                    default:
                        {
                            break;
                        }                    
                }

                if (!querySuccess)
                {
                    // ShowAutoList(eKey, codeEditor);
                }
            }            
        }

        /// <summary>
        /// 缩进文本
        /// </summary>
        /// <param name="codeEditor">当前编辑控件</param>
        private void IndentText(CodeEditorControl codeEditor)
        {
            Row currentRow = codeEditor.Document[codeEditor.Caret.Position.Y - 1];

            if (currentRow != null)
            {
                Row nextRow = codeEditor.Document[codeEditor.Caret.Position.Y + 1];

                if (nextRow == null || nextRow.Text.TrimStart(invalidChartArray) == "")
                {
                    string rowText = currentRow.Text.TrimStart(invalidChartArray);

                    foreach (string keyWord in switchRowKeyWordArray)
                    {
                        if (rowText.StartsWith(keyWord))
                        {
                            SendMessage(this.Handle, WM_CW_SCANCODE_FALSE, IntPtr.Zero, "");
                            break;
                        }
                    }
                }
            }  
        }

        /// <summary>
        /// 显示提示选项
        /// </summary>
        /// <param name="eKey">按键信息</param>
        /// <param name="codeEditor">当前编辑控件</param>
        private void ShowAutoList(KeyPressEventArgs eKey, CodeEditorControl codeEditor)
        {
            if (eKey.KeyChar > 128)
            {
                return;
            }

            //获取完整对象名                     
            string keyWord = codeEditor.Caret.CurrentRow.Text.Substring(0, codeEditor.Caret.Position.X);
            string objectFullName = GetObjectFullName(keyWord);

            string functionName = "";
            int cursorPosition = 0;

            if (!((eKey.KeyChar >= 'a' && eKey.KeyChar <= 'z') || (eKey.KeyChar >= 'A' && eKey.KeyChar <= 'Z') || (eKey.KeyChar >= '0' && eKey.KeyChar <= '9') || eKey.KeyChar == '_'))
            {
                // 过滤之前的参数，获取函数名，以及光标参数所在的位置
                if (eKey.KeyChar == '\b')
                {
                    if (keyWord.Length > 0)
                    {
                        keyWord = keyWord.Substring(0, keyWord.Length - 1);
                    }
                }
                else
                {
                    keyWord += eKey.KeyChar.ToString(); // 把','也追加后面去吧
                }

                functionName = GetFunctionFullNameAndTurns(keyWord);
                int index = functionName.IndexOf("##");
                cursorPosition = int.Parse(functionName.Substring(index + 2));
                functionName = functionName.Substring(0, index);                
                
                // 获取命名空间（说白了就是函数名）
                Segment segment = codeEditor.Caret.CurrentSegment();
                
                // 看看如果是注释内，或者是引号内，就拉倒
                segment = codeEditor.Caret.CurrentSegment();

                if (segment != null)
                {
                    if (segment.BlockType.Style.Name == "Lua Comment")
                    {
                        return;
                    }

                    if (segment.BlockType.Style.Name == "Lua String")
                    {
                        return;
                    }
                    
                    string currentText = codeEditor.Caret.CurrentRow.Text;
                    currentText = currentText.Substring(0, codeEditor.Caret.Position.X);
                    currentText = currentText.Trim(invalidChartArray);

                    if (currentText.StartsWith("--"))
                    {
                        return;
                    }

                    if ((currentText.Replace("'", "").Length - currentText.Length) % 2 == 1)
                    {
                        return;
                    }

                    if ((currentText.Replace("\"", "").Length - currentText.Length) % 2 == 1)
                    {
                        return;
                    }                    
                }
            }     
               
            switch (eKey.KeyChar)
            {
                case ':':
                    {
                        goto case '.';
                    }
                case '.':
                    {
                        switch (inputState)
                        {
                            case InputState.FUNCTIONLIST_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;

                                    break;
                                }
                            case InputState.OBJECTLIST_OPEN:
                                {
                                    goto case InputState.FUNCTIONLIST_OPEN;
                                }
                            case InputState.INTELLISENCE_OPEN:
                                {
                                    goto case InputState.FUNCTIONLIST_OPEN;
                                }
                            default:
                                {
                                    break;
                                }
                        }

                        if (objectFullName != "")
                        {
                            // 触发事件
                            QueryEventsArgs queryEventsArg = new QueryEventsArgs();
                            queryEventsArg.keyword = objectFullName;
                            List<string> objectList = StandardLib.GetObjectList(objectFullName);

                            if (objectList.Count == 0)
                            {
                                objectList = ParameterCache.findMethodInfoByFullName(objectFullName, strNs);
                            }
        
                            foreach (string objectInfo in objectList)
                            {
                                string[] infoArray = objectInfo.Split(queryObjectStringArray, StringSplitOptions.RemoveEmptyEntries);
                                string type = infoArray[1];
                                string value = infoArray[0];

                                if (!queryEventsArg.leTable.ContainsKey(objectInfo))
                                {
                                    queryEventsArg.leTable.Add(objectInfo, new LuaEditorTableItem(value, type, value, ""));
                                }
                            }

                            objectList.Clear();

                            if (queryEventsArg.leTable.Count > 0)
                            {
                                codeEditor.AutoListBeginLoad();
                                codeEditor.AutoListClear();

                                m_list_context = "";

                                foreach (LuaEditorTableItem tableItem in queryEventsArg.leTable.Values)
                                {
                                    switch (tableItem.ItemType)
                                    {
                                        case "method":
                                            {
                                                if (eKey.KeyChar == ':')
                                                {
                                                    codeEditor.AutoListAdd(tableItem.Name, tableItem.ItemValue.ToString(), tableItem.Info, 1);
                                                }

                                                break;
                                            }                                            
                                        case "function":
                                            {
                                                if (eKey.KeyChar == '.')
                                                {
                                                    codeEditor.AutoListAdd(tableItem.Name, tableItem.ItemValue.ToString(), tableItem.Info, 6);
                                                }

                                                break;
                                            }                                            
                                        case "table":
                                            {
                                                if (eKey.KeyChar == '.')
                                                {
                                                    codeEditor.AutoListAdd(tableItem.Name, tableItem.ItemValue.ToString(), tableItem.Info, 3);
                                                }

                                                break;
                                            }
                                            
                                        case "var":
                                            {
                                                if (eKey.KeyChar == '.')
                                                {
                                                    codeEditor.AutoListAdd(tableItem.Name, tableItem.ItemValue.ToString(), tableItem.Info, 4);
                                                }

                                                break;
                                            }          
                                        default:
                                            {
                                                break;
                                            }
                                    }

                                    m_list_context += "|" + tableItem.Name;
                                }

                                codeEditor.AutoListEndLoad();
                                codeEditor.AutoListPosition = new TextPoint(codeEditor.Caret.Position.X + 1, codeEditor.Caret.Position.Y);
                                codeEditor.AutoListVisible = true;
                                inputState = InputState.OBJECTLIST_OPEN;
                            }
                        }

                        break;
                    }                                      
                case ' ':
                    {
                        switch (inputState)
                        {
                            case InputState.FUNCTIONLIST_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;

                                    break;
                                }
                            case InputState.OBJECTLIST_OPEN:
                                {
                                    goto case InputState.OBJECTLIST_OPEN;
                                }
                            case InputState.INTELLISENCE_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;

                                    goto case InputState.NORMAL;
                                }
                            case InputState.NORMAL:
                                {
                                    break;
                                }
                            default:
                                {
                                    break;
                                }
                        }
                       
                        break;
                    }
                case ')':
                    {
                        codeEditor.InfoTipVisible = false;
                        codeEditor.AutoListVisible = false;
                        inputState = InputState.NORMAL;

                        break;
                    }
                    
                case ',':
                    {
                        switch (inputState)
                        {
                            case InputState.INTELLISENCE_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    codeEditor.InfoTipVisible = false;
                                    inputState = InputState.NORMAL;

                                    break;
                                }
                            case InputState.FUNCTIONLIST_OPEN:
                                {
                                    goto case InputState.INTELLISENCE_OPEN;
                                }
                            default:
                                {
                                    break;
                                }
                        }

                        if (functionName != "")
                        {
                            PopInfo(functionName, cursorPosition, strNs, codeEditor);
                        }

                        break;
                    }                    
                case '(' :
                    {
                        switch (inputState)
                        {
                            case InputState.FUNCTIONLIST_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;

                                    break;
                                }
                            case InputState.OBJECTLIST_OPEN:
                                {
                                    goto case InputState.OBJECTLIST_OPEN;

                                    break;
                                }
                            default:
                                {
                                    break;
                                }
                        }                        

                        if (objectFullName != "")
                        {
                            PopInfo(objectFullName, 1, strNs, codeEditor);
                        }

                        break;
                    }
                    
                case '\r':
                    {
                        switch (inputState)
                        {
                            case InputState.FUNCTIONLIST_OPEN:
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;

                                    break;
                                }
                            case InputState.OBJECTLIST_OPEN:
                                {
                                    goto case InputState.FUNCTIONLIST_OPEN;
                                }
                            case InputState.INTELLISENCE_OPEN:
                                {
                                    goto case InputState.FUNCTIONLIST_OPEN;
                                }
                            default:
                                {
                                    // 尝试模拟执行当前行语句
                                    Row currentRow = codeEditor.Document[codeEditor.Caret.Position.Y - 1];
                                    CodeLineScan(currentRow.Text, strNs);

                                    // 尝试自动缩进
                                    IndentText(codeEditor);

                                    break;
                                }
                        }

                        break;
                    }
                case '\b':
                    {
                        if (inputState == InputState.INTELLISENCE_OPEN)
                        {
                            if (objectFullName == "")
                            {
                                codeEditor.AutoListVisible = false;
                                inputState = InputState.NORMAL;                                
                            }
                        }

                        break;
                    }
                case '=':
                    {
                        if (inputState != InputState.NORMAL)
                        {
                            codeEditor.AutoListVisible = false;
                            inputState = InputState.NORMAL;
                        }

                        break;
                    }
                case '+':
                    {
                        goto case '=';
                    }
                case '-':
                    {
                        goto case '=';
                    }
                case '*':
                    {
                        goto case '=';
                    }
                case '/':
                    {
                        goto case '=';
                    }
                case '?':
                    {
                        goto case '=';
                    }
                default:
                    {
                        if (inputState == InputState.NORMAL)
                        {
                            if ((eKey.KeyChar >= 'a' && eKey.KeyChar <= 'z') || (eKey.KeyChar >= 'A' && eKey.KeyChar <= 'Z') || (eKey.KeyChar >= '0' && eKey.KeyChar <= '9') || eKey.KeyChar == '_')
                            {
                                // 智能感知>
                                List<string> wordList;
                                wordList = ParameterCache.GetIntelliSenceWordList(objectFullName + eKey.KeyChar.ToString(), strNs);

                                if (wordList.Count > 0)
                                {
                                    codeEditor.AutoListBeginLoad();
                                    codeEditor.AutoListClear();

                                    m_list_context = "";

                                    foreach (string s in wordList)
                                    {
                                        codeEditor.AutoListAdd(s, s, s, 1);
                                        m_list_context += "|" + s;
                                    }

                                    codeEditor.AutoListEndLoad();
                                    
                                    codeEditor.AutoListPosition = new TextPoint(codeEditor.Caret.Position.X/* + 1*/, codeEditor.Caret.Position.Y);
                                    codeEditor.AutoListVisible = true;
                                    inputState = InputState.INTELLISENCE_OPEN;
                                }

                                if (m_list_context.ToLower().IndexOf(string.Format("|{0}{1}", objectFullName, eKey.KeyChar).ToLower()) == -1) // 输入前几个字母不匹配，则干掉列表
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;
                                }

                                if (string.Format("{0}|", m_list_context).IndexOf(string.Format("|{0}{1}|", objectFullName, eKey.KeyChar)) != -1)  // 完全匹配也干掉列表
                                {
                                    codeEditor.AutoListVisible = false;
                                    inputState = InputState.NORMAL;
                                }
                            }
                        }

                        break;
                    }
            }
        }

        /// <summary>
        /// 根据keyword弹出参数提示
        /// </summary>
        /// <param name="keyword">函数全名</param>
        /// <param name="index">活动参数位置</param>
        /// <param name="codeEditor">luaEditorControl引用</param>
        private void PopInfo(string keyword, int index, string strNs, CodeEditorControl codeEditor)
        {
            string content = "";
            QueryEventsArgs queryEventArgs = new QueryEventsArgs();

            // 先看本文档里有没有
            foreach (FunctionList functionList in cmbFunctionList.Items)
            {
                if (keyword == functionList.FunctionName)
                {
                    content = functionList.FullText;
                    break;
                }
            }

            // 没有啊，那么去查标准库
            if (content == "")
            {
                queryEventArgs.keyword = keyword;
                content = StandardLib.GetFunctionInfo(keyword);
            }

            // 标准库里查不到，那就去反射缓存里查查
            if (content == "")
            {
                content = ParameterCache.FindMethodParameterByFullName(keyword, strNs);
            }

            //缓存里也没有，那就只能回调了
            if (content == "" && queryParms != null)
            {
                queryParms(this, queryEventArgs);         /*回调，获取Parms列表*/
            }


            if (content != "")
            {
                string[] spec = { "<%newline%>" };
                string[] aspp = content.Split(spec, StringSplitOptions.RemoveEmptyEntries);
                foreach (string sitem in aspp)
                {
                    queryEventArgs.parms_list.Add(sitem.Replace("\r\n", "<BR>"));
                }
            }

            //把重载了的参数放进去
            if (queryEventArgs.parms_list.Count > 0)
            {
                codeEditor.InfoTipPosition = new TextPoint(codeEditor.Caret.Position.X + 1, codeEditor.Caret.Position.Y);
                codeEditor.InfoTipCount = queryEventArgs.parms_list.Count;

                for (int i = 0; i < queryEventArgs.parms_list.Count; i++)
                {
                    codeEditor.InfoTipSelectedIndex = i;
                    string argIn = queryEventArgs.parms_list[i];
                    if(argIn.IndexOf("<BR>") == -1)
                    {
                        argIn += "<BR>";
                    }

                    try 
                    {
                        string sc1 = argIn.Substring(0, argIn.IndexOf("<BR>"));
                        if(sc1.IndexOf('(') != -1)
                        {
                            string s1 = sc1.Substring(0, sc1.LastIndexOf('('));
                            string s2 = sc1.Substring(sc1.LastIndexOf('(') + 1);
                            s2 = s2.Substring(0, s2.Length - 1);
                            string s3 = argIn.Substring(argIn.IndexOf("<BR>"));

                            string[] asp = { "," };
                            string[] argIns = s2.Split(asp, StringSplitOptions.RemoveEmptyEntries);
                            if (argIns.Length > index - 1)
                            {
                                string t = argIns[index - 1].Trim();
                                if (t.Substring(t.Length - 1) == "[")
                                    argIns[index - 1] = "<b>" + t.Substring(0, t.Length - 1) + "</b>[";
                                else if (t.Substring(t.Length - 1) == "]")
                                    argIns[index - 1] = "<b>" + t.Substring(0, t.Length - 1) + "</b>]";
                                else
                                    argIns[index - 1] = "<b>" + t + "</b>";
                            }
                            s2 = string.Join(",", argIns);
                            codeEditor.InfoTipText = s1 + "(" + s2 + ")" + s3;         
                        }
                    }
                    catch
                    {
                        codeEditor.InfoTipText = argIn;
                        throw;
                    }

                }
                codeEditor.InfoTipVisible = true;
                inputState = InputState.PARMSLIST_OPEN;
            }
        }

        private void cmbFunctionList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_changefunc) return;
            FunctionList fl = (FunctionList)cmbFunctionList.SelectedItem;
            if (fl == null) return;
            codeEditorControl.Caret.SetPos(new TextPoint(codeEditorControl.Caret.Position.X, fl.LineNumber));
            codeEditorControl.Focus();
            //ce1.Document.ResetVisibleRows();
            //ce1.Caret.CropPosition();
            codeEditorControl.Caret.MoveUp(false);
            codeEditorControl.Caret.MoveDown(false);
            m_changefunc = false;
        }

        private void cmbFunctionList_MouseEnter(object sender, EventArgs e)
        {
            CodeScan(false);
            m_changefunc = true;
        }

        private void cmbFunctionList_MouseLeave(object sender, EventArgs e)
        {
            m_changefunc = false;
        }

        private void luaEditorControl_Load(object sender, EventArgs e)
        {
            CodeEditorSyntaxLoader.SetSyntax(this.codeEditorControl, SyntaxLanguage.Lua);
            codeEditorControl.Caret.Change += new EventHandler(this.OnCarChange);
        }

        //用于切换行的时候，关闭提示列表
        private int lastline = 0; 

        private void OnCarChange(object sender, EventArgs e)
        {
            if (lastline != codeEditorControl.Caret.Position.Y)
            {
                if(inputState != InputState.NORMAL)
                {
                    codeEditorControl.AutoListVisible = false;
                    codeEditorControl.InfoTipVisible = false;
                    inputState = InputState.NORMAL;
                }
            }
            lastline = codeEditorControl.Caret.Position.Y;


            //刷新上头的functionlist
            Fireball.Syntax.Segment seg = codeEditorControl.Caret.CurrentSegment();
            string strNs = GetNameSpace(seg);
            this.strNs = strNs;
            foreach (FunctionList fl in cmbFunctionList.Items)
            {
                if (fl.FunctionName == strNs)
                {
                    cmbFunctionList.SelectedItem = fl;
                    break;
                }
            }
        }        
    }

    public enum InputState
    {
        NORMAL, // 正常
        FUNCTIONLIST_OPEN, // 事件列表显示
        OBJECTLIST_OPEN, // 对象列表显示
        PARMSLIST_OPEN, // 参数列表显示
        INTELLISENCE_OPEN // 智能感知列表显示
    }

    /************************************************************************/
    /* 事件相关                                                             */
    /************************************************************************/
    public delegate void QueryEventsHandle(object sender, QueryEventsArgs e);   /*事件*/
    public delegate void QueryObjectHandle(object sender, QueryEventsArgs e);   /*对象*/
    public delegate void QueryParmsHandle(object sender, QueryEventsArgs e);    /*参数*/
    public delegate void OnChangeHandle(object sender, EventArgs e);
    public delegate void BreakPointAdded(object sender, RowEventArgs e);
    public delegate void BreakPointRemoved(object sender, RowEventArgs e);
    public delegate void WordMouseHover(object sender, ref WordMouseEventArgs e);

    public class QueryEventsArgs : EventArgs
    {
        // 存放事件列表
        public leEventConnection leec;

        // 存放成员列表，Value存放leTableItem
        public Hashtable leTable = new Hashtable();
        
        // 存放参数列表，每个Item存放一个字符串如 "prop,e"
        public List<string> parms_list = new List<string>(); 

        // 存对象完整路径例如 _G.string.len
        public string keyword = "";

        //后期用
        public string szCode = "";
        public int nPos = 0;
        public int nOverLen = 0;
    };

    /************************************************************************/
}
