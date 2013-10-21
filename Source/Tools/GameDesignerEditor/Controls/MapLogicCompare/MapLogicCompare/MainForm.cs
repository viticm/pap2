using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Threading;
using System.Runtime.InteropServices;

using DevComponents.DotNetBar;
using DevComponents.AdvTree;

namespace MapLogicalCompare
{
    public partial class MainForm : Office2007Form
    {
        private LogicalData leftLogicalData;
        private LogicalData rightLogicalData;
        private enum EditState { None, LeftReady, RightReady, AllReady }; // 编辑状态
        private EditState editState = EditState.None; // 当前的编辑状态
        private char[] lineSplitArray = new char[] { '\n' };
        private char[] rowSplitArray = new char[] { '\r' };

        private const int SB_HORZ = 0x0;
        private const int SB_VERT = 0x1; 
        private const int WM_HSCROLL = 0x114;
        private const int WM_VSCROLL = 0x115;
        private const int SB_THUMBPOSITION = 4;

        // 调用Windows的API
        [DllImport("user32.dll")]
        private static extern int SetScrollPos(IntPtr hwnd, int nBar, int nPos, bool bRedraw);

        [DllImport("user32.dll")]
        private static extern int GetScrollPos(IntPtr hwnd, int nBar);

        [DllImport("user32.dll")]
        private static extern bool PostMessage(IntPtr hWnd, int nBar, int wParam, int lParam);

        [DllImport("user32", CharSet = CharSet.Auto)]
        private static extern bool GetScrollRange(IntPtr hWnd, int nBar, out int lpMinPos, out int lpMaxPos); 

        public MainForm()
        {
            InitializeComponent();

            // 注册事件
            richTextBox1.AllowDrop = true;
            richTextBox1.DragEnter += new DragEventHandler(richTextBox_DragEnter);
            richTextBox1.DragDrop += new DragEventHandler(richTextBox1_DragDrop);

            richTextBox2.AllowDrop = true;
            richTextBox2.DragEnter += new DragEventHandler(richTextBox_DragEnter);
            richTextBox2.DragDrop += new DragEventHandler(richTextBox2_DragDrop);            
        }

        class LogicalData
        {
            private bool isReady = false;
            private string fileName;
            private string content;
            private IniReader iniReader;
            private Hashtable infoTable = new Hashtable();

            private int nNumNPC = 0;
            private int nNumDoodad = 0;
            private int nNumNpcRefreshSet = 0;
            private int nNumNpcRefreshSetMaxID = 0;
            private int nNumDoodadRefreshSet = 0;
            private int nNumDoodadRefreshSetMaxID = 0;
            private int nNumAIGroup = 0;
            private int nNumAIGroupMaxID = 0;
            private int nNumWapPointSet = 0;
            private int nNumWapPointSetMaxID = 0;
            private int logicalSceneID = 0;
            private float logicalSceneXStart = 0;
            private float logicalSceneZStart = 0;
            private int nNumLogicalPoly = 0;
            private int nNumTrafficPointSet = 0;
            private int nNumTrafficPoint = 0;
            private int nNumTrafficPointSetMaxID = 0;
            private int nNumAISetCount = 0;
            private int nNumNpcReviveGroup = 0;
            private int nNumDoodadReviveGroup = 0;
            private int nNumNpcRandomGroup = 0;

            private List<string> aiSetList = new List<string>();
            private List<string> wayPointSetList = new List<string>();
            private List<string> npcList = new List<string>();
            private List<string> doodadList = new List<string>();
            private List<string> aiGroupList = new List<string>();
            private List<string> logicalPolyList = new List<string>();
            private List<string> trafficPointList = new List<string>();
            private List<string> trafficPointSetList = new List<string>();
            private List<string> npcReviveGroupList = new List<string>();
            private List<string> doodadReviveGroupList = new List<string>();
            private List<string> npcRandomGroupList = new List<string>();

            public LogicalData(IniReader iniReader, string fileName)
            {
                this.iniReader = iniReader;
                this.fileName = fileName;
                Init();
            }
            
            public string FileName
            {
                get
                {
                    return fileName;
                }
            }

            public string Content
            {
                get
                {
                    return content;
                }
            }

            public bool IsReady
            {
                get
                {
                    return isReady;
                }
            }

            public List<string> AISetList
            {
                get
                {
                    return aiSetList;
                }
            }

            public List<string> WayPointSetList
            {
                get
                {
                    return wayPointSetList;
                }
            }

            public List<string> NpcList
            {
                get
                {
                    return npcList;
                }
            }

            public List<string> DoodadList
            {
                get
                {
                    return doodadList;
                }
            }

            public List<string> AIGroupList
            {
                get
                {
                    return aiGroupList;
                }
            }

            public List<string> LogicalPolyList
            {
                get
                {
                    return logicalPolyList;
                }
            }

            public List<string> TrafficPointList
            {
                get
                {
                    return trafficPointList;
                }
            }

            public List<string> TrafficPointSetList
            {
                get
                {
                    return trafficPointSetList;
                }
            }

            public List<string> NpcReviveGroupList
            {
                get
                {
                    return npcReviveGroupList;
                }
            }

            public List<string> DoodadReviveGroupList
            {
                get
                {
                    return doodadReviveGroupList;
                }
            }

            public List<string> NpcRandomGroupList
            {
                get
                {
                    return npcRandomGroupList;
                }
            }

            public Hashtable InfoTable
            {
                get
                {
                    return infoTable;
                }
            }

            public int NumNPC
            {
                get
                {
                    return nNumNPC;
                }
            }

            public int NumDoodad
            {
                get
                {
                    return nNumDoodad;
                }
            }

            public int NumNpcRefreshSet
            {
                get
                {
                    return nNumNpcRefreshSet;
                }
            }

            public int NumNpcRefreshSetMaxID
            {
                get
                {
                    return nNumNpcRefreshSetMaxID;
                }
            }

            public int NumDoodadRefreshSet
            {
                get
                {
                    return nNumDoodadRefreshSet;
                }
            }

            public int NumDoodadRefreshSetMaxID
            {
                get
                {
                    return nNumDoodadRefreshSetMaxID;
                }
            }

            public int NumAIGroup
            {
                get
                {
                    return nNumAIGroup;
                }
            }

            public int NumAIGroupMaxID
            {
                get
                {
                    return nNumAIGroupMaxID;
                }
            }

            public int NumWapPointSet
            {
                get
                {
                    return nNumWapPointSet;
                }
            }

            public int NumWapPointSetMaxID
            {
                get
                {
                    return nNumWapPointSetMaxID;
                }
            }

            public int LogicalSceneID
            {
                get
                {
                    return logicalSceneID;
                }
            }

            public float LogicalSceneXStart
            {
                get
                {
                    return logicalSceneXStart;
                }
            }

            public float LogicalSceneZStart
            {
                get
                {
                    return logicalSceneZStart;
                }
            }

            public int NumLogicalPoly
            {
                get
                {
                    return nNumLogicalPoly;
                }
            }

            public int NumTrafficPointSet
            {
                get
                {
                    return nNumTrafficPointSet;
                }
            }

            public int NumTrafficPoint
            {
                get
                {
                    return nNumTrafficPoint;
                }
            }

            public int NumTrafficPointSetMaxID
            {
                get
                {
                    return nNumTrafficPointSetMaxID;
                }
            }

            public int NumAISetCount
            {
                get
                {
                    return nNumAISetCount;
                }
            }

            public int NumNpcReviveGroup
            {
                get
                {
                    return nNumNpcReviveGroup;
                }
            }

            public int NumDoodadReviveGroup 
            {
                get
                {
                    return nNumDoodadReviveGroup;
                }
            }

            public int NumNpcRandomGroup
            {
                get
                {
                    return nNumNpcRandomGroup;
                }
            }

            /// <summary>
            /// 初始化数据
            /// </summary>
            private void Init()
            {
                Thread loadingThread = new Thread(new ThreadStart(InitInThread));
                loadingThread.Start();                
            }

            /// <summary>
            /// 在新线程内初始化数据
            /// </summary>
            private void InitInThread()
            {
                int currentValue = 1;
                ProgressForm pForm = new ProgressForm(12);
                pForm.Show();
                pForm.Refresh();

                foreach (string s in iniReader.SectionNames)
                {
                    infoTable[s] = iniReader[s];
                }

                string keyValue = null;

                // 初始化Main
                content = infoTable["MAIN"] as string;

                if (content != null)
                {
                    keyValue = iniReader["MAIN", "NumNPC"];
                    if (keyValue != "")
                    {
                        nNumNPC = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumDoodad"];
                    if (keyValue != "")
                    {
                        nNumDoodad = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumNpcRefreshSet"];
                    if (keyValue != "")
                    {
                        nNumNpcRefreshSet = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumNpcRefreshSetMaxID"];
                    if (keyValue != "")
                    {
                        nNumNpcRefreshSetMaxID = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumDoodadRefreshSet"];
                    if (keyValue != "")
                    {
                        nNumDoodadRefreshSet = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumDoodadRefreshSetMaxID"];
                    if (keyValue != "")
                    {
                        nNumDoodadRefreshSetMaxID = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumAIGroup"];
                    if (keyValue != "")
                    {
                        nNumAIGroup = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumAIGroupMaxID"];
                    if (keyValue != "")
                    {
                        nNumAIGroupMaxID = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumWapPointSet"];
                    if (keyValue != "")
                    {
                        nNumWapPointSet = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumWapPointSetMaxID"];
                    if (keyValue != "")
                    {
                        nNumWapPointSetMaxID = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "LogicalSceneID"];
                    if (keyValue != "")
                    {
                        logicalSceneID = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "LogicalSceneXStart"];
                    if (keyValue != "")
                    {
                        logicalSceneXStart = float.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "LogicalSceneZStart"];
                    if (keyValue != "")
                    {
                        logicalSceneZStart = float.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumLogicalPoly"];
                    if (keyValue != "")
                    {
                        nNumLogicalPoly = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumTrafficPointSet"];
                    if (keyValue != "")
                    {
                        nNumTrafficPointSet = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumTrafficPoint"];
                    if (keyValue != "")
                    {
                        nNumTrafficPoint = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "AISetCount"];
                    if (keyValue != "")
                    {
                        nNumAISetCount = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumNpcReviveGroup"];
                    if (keyValue != "")
                    {
                        nNumNpcReviveGroup = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumDoodadReviveGroup"];
                    if (keyValue != "")
                    {
                        nNumDoodadReviveGroup = int.Parse(keyValue);
                    }

                    keyValue = iniReader["MAIN", "NumNpcRandomGroup"];
                    if (keyValue != "")
                    {
                        nNumNpcRandomGroup = int.Parse(keyValue);
                    }
                }

                pForm.Progress(currentValue, "读取Main数据...");
                pForm.Refresh();
                currentValue++;                
                Thread.Sleep(100);

                // 初始化AISet
                for (int i = 0; i < nNumAISetCount; i++)
                {
                    string sectionName = string.Format("AISET{0}", i.ToString());
                    aiSetList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取AISet数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化WayPointSet
                for (int i = 0; i < nNumWapPointSet; i++)
                {
                    string sectionName = string.Format("WayPointSet{0}", i.ToString());
                    wayPointSetList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取WayPointSet数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化NPC
                for (int i = 0; i < nNumNPC; i++)
                {
                    string sectionName = string.Format("NPC{0}", i.ToString());
                    npcList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取NPC数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化Doodad
                for (int i = 0; i < nNumDoodad; i++)
                {
                    string sectionName = string.Format("Doodad{0}", i.ToString());
                    doodadList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取Doodad数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化AIGroup
                for (int i = 0; i < nNumAIGroup; i++)
                {
                    string sectionName = string.Format("AIGroup{0}", i.ToString());
                    aiGroupList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取AIGroup数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化LogicalPoly
                for (int i = 0; i < nNumLogicalPoly; i++)
                {
                    string sectionName = string.Format("LogicalPoly{0}", i.ToString());
                    logicalPolyList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取LogicalPoly数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化TrafficPoint
                for (int i = 0; i < nNumTrafficPoint; i++)
                {
                    string sectionName = string.Format("TrafficPoint{0}", i.ToString());
                    trafficPointList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取TrafficPoint数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化TrafficPointset
                for (int i = 0; i < nNumTrafficPointSet; i++)
                {
                    string sectionName = string.Format("TrafficPointSet{0}", i.ToString());
                    trafficPointSetList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取TrafficPointSet数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化NpcReviveGroups
                for (int i = 0; i < nNumNpcReviveGroup; i++)
                {
                    string sectionName = string.Format("NpcReviveSection{0}", i.ToString());
                    npcReviveGroupList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取NpcReviveGroups数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化DoodadReviveGroups
                for (int i = 0; i < nNumDoodadReviveGroup; i++)
                {
                    string sectionName = string.Format("DoodadReviveSection{0}", i.ToString());
                    doodadReviveGroupList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取DoodadReviveGroups数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                // 初始化NpcRandomGroups
                for (int i = 0; i < nNumNpcRandomGroup; i++)
                {
                    string sectionName = string.Format("NpcRandomSection{0}", i.ToString());
                    npcRandomGroupList.Add(iniReader[sectionName]);
                }

                pForm.Progress(currentValue, "读取NpcRandomGroups数据...");
                pForm.Refresh();
                currentValue++;
                Thread.Sleep(100);

                content = iniReader.FileContent;
                isReady = true;
                pForm.Close();
            }
        }

        /// <summary>
        /// 比较稳健内容
        /// </summary>
        private void CompareFile()
        {
            // 比较全文本
            // ShowText(leftLogicalData.Content, rightLogicalData.Content);

            // 比较Main            
            CompareMain();            

            // 比较AISet
            CompareAISet();

            // 比较WayPointSet
            CompareWayPointSet();

            // 比较NPC
            CompareNpc();

            // 比较Doodad
            CompareDoodad();

            // 比较AIGroup
            CompareAIGroup();

            // 比较LogicalPoly
            CompareLogicalPoly();

            // 比较TrafficPoint
            CompareTrafficPoint();

            // 比较TrafficPointSet
            CompareTrafficPointSet();

            // 比较Npc重生组
            CompareNpcReviveGroups();

            // 比较Doodad重生组
            CompareDoodadReviveGroups();

            // 比较Npc随机组
            CompareNpcRandomGroups();
        }

        /// <summary>
        /// 比较Npc随机组
        /// </summary>
        private void CompareNpcRandomGroups()
        {
            CommonCompare(11, leftLogicalData.NpcRandomGroupList, rightLogicalData.NpcRandomGroupList, "NpcRandomSection");
        }

        /// <summary>
        /// 比较Doodad重生组
        /// </summary>
        private void CompareDoodadReviveGroups()
        {
            CommonCompare(10, leftLogicalData.DoodadReviveGroupList, rightLogicalData.DoodadReviveGroupList, "DoodadReviveSection");
        }

        /// <summary>
        /// 比较Npc重生组
        /// </summary>
        private void CompareNpcReviveGroups()
        {
            CommonCompare(9, leftLogicalData.NpcReviveGroupList, rightLogicalData.NpcReviveGroupList, "NpcReviveSection");
        }

        /// <summary>
        /// 比较TrafficPointSet
        /// </summary>
        private void CompareTrafficPointSet()
        {
            CommonCompare(8, leftLogicalData.TrafficPointSetList, rightLogicalData.TrafficPointSetList, "TrafficPointSet");
        }

        /// <summary>
        /// 比较TrafficPoint
        /// </summary>
        private void CompareTrafficPoint()
        {
            CommonCompare(7, leftLogicalData.TrafficPointList, rightLogicalData.TrafficPointList, "TrafficPoint");
        }

        /// <summary>
        /// 比较LogicalPoly
        /// </summary>
        private void CompareLogicalPoly()
        {
            CommonCompare(6, leftLogicalData.LogicalPolyList, rightLogicalData.LogicalPolyList, "LogicalPoly");
        }

        /// <summary>
        /// 比较AIGroup
        /// </summary>
        private void CompareAIGroup()
        {
            CommonCompare(5, leftLogicalData.AIGroupList, rightLogicalData.AIGroupList, "AIGroup");
        }

        /// <summary>
        /// 比较Doodad
        /// </summary>
        private void CompareDoodad()
        {
            CommonCompare(4, leftLogicalData.DoodadList, rightLogicalData.DoodadList, "Doodad");
        }

        /// <summary>
        /// 比较NPC
        /// </summary>
        private void CompareNpc()
        {
            CommonCompare(3, leftLogicalData.NpcList, rightLogicalData.NpcList, "NPC");
        }

        /// <summary>
        /// 比较WayPointSet
        /// </summary>
        private void CompareWayPointSet()
        {
            CommonCompare(2, leftLogicalData.WayPointSetList, rightLogicalData.WayPointSetList, "WayPointSet");
        }

        /// <summary>
        /// 通用比较
        /// </summary>
        /// <param name="index">树结点序号</param>
        /// <param name="dataList1">数据链表1</param>
        /// <param name="dataList2">数据链表2</param>
        /// <param name="sectionName">分区名字</param>
        private void CommonCompare(int index, List<string> dataList1, List<string> dataList2, string sectionName)
        {
            itemTree.Nodes[index].Nodes.Clear();

            List<string> list1 = new List<string>();
            list1.AddRange(dataList1);

            List<string> list2 = new List<string>();
            list2.AddRange(dataList2);

            // 先替换掉两边都相同的内容            
            for (int i = 0; i < list1.Count; i++)
            {
                bool find = false;

                for (int k = 0; k < list2.Count; k++)
                {
                    if (list2[k] == list1[i])
                    {
                        list2[k] = "囧";
                        find = true;
                        break;
                    }
                }

                if (find)
                {
                    list1[i] = "囧";
                }
            }

            // 开始比较不同的内容
            int j = 0;
            for (int i = 0; i < list1.Count; i++)
            {
                string data1 = list1[i];

                if (data1 != "囧")
                {
                    if (j < list2.Count)
                    {
                        string data2 = list2[j];

                        while (data2 == "囧")
                        {
                            j++;

                            if (j < list2.Count)
                            {
                                data2 = list2[j];
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (data2 != "囧")
                        {
                            Node newNode = new Node();
                            newNode.Text = string.Format("{0}{1}", sectionName, i.ToString());
                            newNode.Tag = new string[] { string.Format("{0}{1}\r\n{2}", sectionName, i.ToString(), data1), string.Format("{0}{1}\r\n{2}", sectionName, j.ToString(), data2) };
                            itemTree.Nodes[index].Nodes.Add(newNode);
                        }

                        j++;
                    }
                    else
                    {
                        Node newNode = new Node();
                        newNode.Text = string.Format("{0}{1}", sectionName, i.ToString());
                        newNode.Tag = new string[] { string.Format("{0}{1}\r\n{2}", sectionName, i.ToString(), data1), "" };
                        itemTree.Nodes[index].Nodes.Add(newNode);
                    }
                }
            }

            while (j < list2.Count)
            {
                string data2 = list2[j];

                if (data2 != "囧")
                {
                    Node newNode = new Node();
                    newNode.Text = string.Format("{0}空", sectionName);
                    newNode.Tag = new string[] { "", string.Format("{0}{1}\r\n{2}", sectionName, j.ToString(), data2) };
                    itemTree.Nodes[index].Nodes.Add(newNode);
                }

                j++;
            }

            if (itemTree.Nodes[index].Nodes.Count > 0)
            {
                itemTree.Nodes[index].Text = string.Format("{0} ({1})", sectionName, itemTree.Nodes[index].Nodes.Count.ToString());
            }
            else
            {
                itemTree.Nodes[index].Text = sectionName;
            }
        }

        /// <summary>
        /// 比较Main
        /// </summary>
        private void CompareMain()
        {
            itemTree.Nodes[0].Nodes.Clear();

            if (leftLogicalData.InfoTable["MAIN"] as string == rightLogicalData.InfoTable["MAIN"] as string)
            {
                itemTree.Nodes[0].Text = "MAIN";
            }
            else
            {
                Node newNode = new Node();
                newNode.Text = "MAIN";
                newNode.Tag = new string[] { string.Format("MAIN\r\n{0}", leftLogicalData.InfoTable["MAIN"] as string),
                                            string.Format("MAIN\r\n{0}", rightLogicalData.InfoTable["MAIN"] as string)};
                itemTree.Nodes[0].Nodes.Add(newNode);
                itemTree.Nodes[0].Text = string.Format("MAIN ({0})", itemTree.Nodes[0].Nodes.Count.ToString());
            }
        }

        /// <summary>
        /// 比较AISet
        /// </summary>
        private void CompareAISet()
        {
            CommonCompare(1, leftLogicalData.AISetList, rightLogicalData.AISetList, "AISet");  
        }

        /// <summary>
        /// 比较稳健
        /// </summary>
        /// <param name="leftFileName">源文件路径</param>
        /// <param name="rightFileName">目标文件路径</param>
        public void CompareFile(string leftFileName, string rightFileName)
        {            
            OpenLeftFile(leftFileName);
            OpenRightFile(rightFileName);              
        }

        /// <summary>
        /// 调整滚动条
        /// </summary>
        /// <param name="srcTextBox">源文本框</param>
        /// <param name="descTextBox">目标文本框</param>
        private void AdjustScroll(RichTextBox srcTextBox, RichTextBox descTextBox)
        {            
            int min = 0;
            int max = 0;
            int position = GetScrollPos(srcTextBox.Handle, SB_VERT);
            GetScrollRange(descTextBox.Handle, SB_VERT, out min, out max);
            int endPosition = max - descTextBox.ClientRectangle.Height;

            if (position < endPosition)
            {
                SetScrollPos(descTextBox.Handle, SB_VERT, position, true);
                PostMessage(descTextBox.Handle, WM_VSCROLL, SB_THUMBPOSITION + 0x10000 * position, 0);
            }                       
        }

        /// <summary>
        /// 显示比较字符串
        /// </summary>
        /// <param name="leftString">左边显示的文本内容</param>
        /// <param name="rightString">右边显示的文本内容</param>
        private void ShowText(string leftString, string rightString)
        {            
            richTextBox1.Text = leftString;
            richTextBox2.Text = rightString;

            // 先清理一遍颜色
            richTextBox1.SelectionStart = 0;
            richTextBox1.SelectionLength = leftString.Length;
            richTextBox1.SelectionColor = Color.Black;

            richTextBox2.SelectionStart = 0;
            richTextBox2.SelectionLength = rightString.Length;
            richTextBox2.SelectionColor = Color.Black;

            string[] lines1 = leftString.Split(lineSplitArray, StringSplitOptions.RemoveEmptyEntries);
            string[] lines2 = rightString.Split(lineSplitArray, StringSplitOptions.RemoveEmptyEntries);

            int index1 = 0;
            int index2 = 0;

            if (lines1.Length > 0)
            {
                richTextBox1.Select(0, lines1[0].Length);
                richTextBox1.SelectionColor = Color.Blue;
                index1 += lines1[0].Length;
            }
            
            if (lines2.Length > 0)
            {
                richTextBox2.Select(0, lines2[0].Length);
                richTextBox2.SelectionColor = Color.Blue;
                index2 += lines2[0].Length;
            }
            
            int j = 1;
            string line1;
            string line2;

            for (int i = 1; i < lines1.Length; i++)
            {
                line1 = lines1[i];                

                if (j < lines2.Length)
                {
                    line2 = lines2[j];

                    if (line1 != line2)
                    {
                        richTextBox1.Select(index1, line1.Length);
                        richTextBox1.SelectionColor = Color.Red;
                        richTextBox2.Select(index2, line2.Length);
                        richTextBox2.SelectionColor = Color.Red;
                    }

                    index2 += line2.Length;
                    j++;
                }
                else
                {
                    richTextBox1.Select(index1, line1.Length);
                    richTextBox1.SelectionColor = Color.Green;
                }

                index1 += line1.Length;
            }

            while (j < lines2.Length)
            {
                line2 = lines2[j];
                richTextBox2.Select(index2, line2.Length);
                richTextBox2.SelectionColor = Color.Green;

                index2 += line2.Length;
                j++;
            }          
        }

        /// <summary>
        /// 加载文件
        /// </summary>
        /// <param name="title">标题文字</param>
        /// <returns>文件路径</returns>
        private string LoadFile(string title)
        {
            string fileName = null;

            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = title;
            openFileDialog.Filter = "Logical文件|*.Logical";
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
            }            

            return fileName;
        }

        /// <summary>
        /// 初始化逻辑数据
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="currentEditState">当前编辑状态</param>
        private void InitData(string fileName, EditState currentEditState)
        {            
            IniReader iniReader = new IniReader(fileName);
            
            switch (currentEditState)
            {
                case EditState.LeftReady:
                    {
                        leftLogicalData = new LogicalData(iniReader, fileName) ;
                        break;
                    }
                case EditState.RightReady:
                    {
                        rightLogicalData = new LogicalData(iniReader, fileName);                        
                        break;
                    }
            }
        }

        /// <summary>
        /// 打开源对比文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        private void OpenLeftFile(string fileName)
        {
            InitData(fileName, EditState.LeftReady);

            // 等待数据初始化完毕
            while (!leftLogicalData.IsReady)
            {
                ;
            }

            richTextBox1.Text = leftLogicalData.Content;
            richTextBox1.SelectionStart = 0;
            richTextBox1.SelectionLength = leftLogicalData.Content.Length;
            richTextBox1.SelectionColor = Color.Black;

            switch (editState)
            {
                case EditState.None:
                    {
                        editState = EditState.LeftReady;
                        this.Text = string.Format("MapLogical文件比较 {0} <->", leftLogicalData.FileName);
                        break;
                    }
                case EditState.LeftReady:
                    {
                        break;
                    }
                case EditState.RightReady:
                    {
                        editState = EditState.AllReady;
                        this.Text = string.Format("MapLogical文件比较 {0} <-> {1}", leftLogicalData.FileName, rightLogicalData.FileName);
                        CompareFile();
                        break;
                    }
                case EditState.AllReady:
                    {
                        CompareFile();
                        break;
                    }
            }
        }

        /// <summary>
        /// 打开目标对比文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        private void OpenRightFile(string fileName)
        {
            InitData(fileName, EditState.RightReady);

            // 等待数据初始化完毕
            while (!rightLogicalData.IsReady)
            {
                ;
            }

            richTextBox2.Text = rightLogicalData.Content;
            richTextBox2.SelectionStart = 0;
            richTextBox2.SelectionLength = rightLogicalData.Content.Length;
            richTextBox2.SelectionColor = Color.Black;

            switch (editState)
            {
                case EditState.None:
                    {
                        editState = EditState.RightReady;
                        this.Text = string.Format("MapLogical文件比较 <-> {0}", rightLogicalData.FileName);
                        break;
                    }
                case EditState.LeftReady:
                    {
                        editState = EditState.AllReady;
                        this.Text = string.Format("MapLogical文件比较 {0} <-> {1}", leftLogicalData.FileName, rightLogicalData.FileName);
                        CompareFile();
                        break;
                    }
                case EditState.RightReady:
                    {
                        break;
                    }
                case EditState.AllReady:
                    {
                        CompareFile();
                        break;
                    }
            }
        }

        /// <summary>
        /// 文本比较分割容器大小改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void compareContainer_Resize(object sender, EventArgs e)
        {
            Size containerSize = compareContainer.Size;
            int splitDistance = (int)((containerSize.Width - 4) / 2);
            compareContainer.SplitterDistance = splitDistance;
        }

        /// <summary>
        /// 加载文件1
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bLoadFile1_Click(object sender, EventArgs e)
        {
            string fileName = LoadFile("请选择要打开的文件");

            if (fileName != null)
            {
                OpenLeftFile(fileName);
            }
        }

        /// <summary>
        /// 加载文件2
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bLoadFile2_Click(object sender, EventArgs e)
        {
            string fileName = LoadFile("请选择要打开的文件");

            if (fileName != null)
            {
                OpenRightFile(fileName);
            }
        }

        /// <summary>
        /// 选择分类树节点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void itemTree_AfterNodeSelect(object sender, DevComponents.AdvTree.AdvTreeNodeEventArgs e)
        {
            if (e.Node.Level == 1)
            {
                string[] compareData = e.Node.Tag as string[];
                ShowText(compareData[0], compareData[1]);
            }
            else
            {
                richTextBox1.Text = "";
                richTextBox2.Text = "";
            }
        }

        /// <summary>
        /// 文件拖动事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void richTextBox_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Link;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        /// <summary>
        /// 文件放入事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void richTextBox1_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();

            if (fileName.ToLower().EndsWith(".logical"))
            {
                OpenLeftFile(fileName);                
            }
            else
            {
                MessageBox.Show("只能打开类型为MapLogical的文件！", "文件比较", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 文件放入事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void richTextBox2_DragDrop(object sender, DragEventArgs e)
        {
            string fileName = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();

            if (fileName.ToLower().EndsWith(".logical"))
            {
                OpenRightFile(fileName);
            }
            else
            {
                MessageBox.Show("只能打开类型为MapLogical的文件！", "文件比较", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 垂直滚动
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void richTextBox1_VScroll(object sender, EventArgs e)
        {
            if (richTextBox1.Focused) // 避免出现消息死循环
            {
                AdjustScroll(richTextBox1, richTextBox2);
            }
            
        }

        /// <summary>
        /// 垂直滚动
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void richTextBox2_VScroll(object sender, EventArgs e)
        {
            if (richTextBox2.Focused) // 避免出现消息死循环
            {
                AdjustScroll(richTextBox2, richTextBox1);
            }            
        }

        /// <summary>
        /// 查找
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bFind_Click(object sender, EventArgs e)
        {
            FindForm fForm = new FindForm();
            fForm.AISetCount = leftLogicalData.NumAISetCount;
            fForm.WayPointSetCount = leftLogicalData.NumWapPointSet;
            fForm.NpcCount = leftLogicalData.NumNPC;
            fForm.DoodadCount = leftLogicalData.NumDoodad;
            fForm.AIGroupCount = leftLogicalData.NumAIGroup;
            fForm.LogicalPolyCount = leftLogicalData.NumLogicalPoly;
            fForm.TrafficPointCount = leftLogicalData.NumTrafficPoint;
            fForm.TrafficPointSetCount = leftLogicalData.NumTrafficPointSet;
            fForm.NpcReviveGroupsCount = leftLogicalData.NumNpcReviveGroup;
            fForm.DoodadReviveGroupsCount = leftLogicalData.NumDoodadReviveGroup;
            fForm.NpcRandomGroupsCount = leftLogicalData.NumNpcRandomGroup;
            
            if (fForm.ShowDialog() == DialogResult.OK)
            {
                Node sectionNode = null;
                string keyName = string.Format("{0}{1}", fForm.SectionName, fForm.KeyIndex);

                switch (fForm.SectionName)
                {
                    case "AISet":
                        {
                            sectionNode = itemTree.Nodes[1];
                            break;
                        }
                    case "WayPointSet":
                        {
                            sectionNode = itemTree.Nodes[2];
                            break;
                        }
                    case "NPC":
                        {
                            sectionNode = itemTree.Nodes[3];
                            break;
                        }
                    case "Doodad":
                        {
                            sectionNode = itemTree.Nodes[4];
                            break;
                        }
                    case "AIGroup":
                        {
                            sectionNode = itemTree.Nodes[5];
                            break;
                        }
                    case "LogicalPoly":
                        {
                            sectionNode = itemTree.Nodes[6];
                            break;
                        }
                    case "TrafficPoint":
                        {
                            sectionNode = itemTree.Nodes[7];
                            break;
                        }
                    case "TrafficPointSet":
                        {
                            sectionNode = itemTree.Nodes[8];
                            break;
                        }
                    case "NpcReviveSection":
                        {
                            sectionNode = itemTree.Nodes[9];
                            break;
                        }
                    case "DoodadReviveSection":
                        {
                            sectionNode = itemTree.Nodes[10];
                            break;
                        }
                    case "NpcRandomSection":
                        {
                            sectionNode = itemTree.Nodes[11];
                            break;
                        }
                }

                if (sectionNode != null)
                {                    
                    foreach (Node node in sectionNode.Nodes)
                    {
                        if (node.Text == keyName)
                        {
                            itemTree.SelectedNode = node;
                            break;
                        }
                    }
                }
            }
        }
    }
}