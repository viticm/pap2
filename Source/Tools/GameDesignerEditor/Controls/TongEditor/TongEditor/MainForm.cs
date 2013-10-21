using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;

namespace ToneEditor
{
    public partial class MainForm : Office2007Form
    {
        private int fileVersion; // 文件版本号
        private DataTable tongOperationTable; // 帮会操作数据表
        private DataTable tongOperationGroupTable; // 帮会操作描述数据表
        private List<TongTemplate> tongTemplateList = new List<TongTemplate>(); // 帮会模板链表

        private bool beginEdit = false; // 是否已经开始编辑
        private bool groupContentChanged = false; // 帮会信息是否改变
        private int lastTongIndex = -1; // 上一次选择的TongIndex;

        private enum DisplayMode { HideCheckedItem, HideUncheckedItem, ShowAllItem };
        private DisplayMode displayMode = DisplayMode.ShowAllItem;

        /// <summary>
        /// 构造函数
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 帮会组
        /// </summary>
        class TongGroup
        {
            private string groupName = ""; // 分组名称
            private bool enable = false; // 是否开启

            private bool[] baseOperationMask = new bool[8]; // 基本操作权限
            private bool[,] advanceOperationMask = new bool[16, 8]; // 高级操作权限,即与被操作对象所属Group相关的操作权限
            private bool[,] baseGrantMask = new bool[16, 8]; // 基本授权
            private bool[, ,] advanceGrantMask = new bool[16, 16, 8]; // // 高级授权,即与被操作对象所属Group相关的操作授权

            /// <summary>
            /// 分组名称
            /// </summary>
            [Browsable(true)]
            [Category("帮会成员属性")]
            [DisplayName("成员名称")]
            [Description("帮会成员的名称")]
            public string GroupName
            {
                set
                {
                    groupName = value;
                }

                get
                {
                    return groupName;
                }
            }

            /// <summary>
            /// 是否开启
            /// </summary>
            [Browsable(true)]
            [Category("帮会成员属性")]
            [DisplayName("是否开启")]
            [Description("是否开启当前成员")]
            public bool Enable
            {
                set
                {
                    enable = value;
                }

                get
                {
                    return enable;
                }
            }

            /// <summary>
            /// 基本操作权限
            /// </summary>
            [Browsable(false)]
            public bool[] BaseOperationMask
            {
                get
                {
                    return baseOperationMask;
                }
            }

            /// <summary>
            /// 高级操作权限
            /// </summary>
            [Browsable(false)]
            public bool[,] AdvanceOperationMask
            {
                get
                {
                    return advanceOperationMask;
                }
            }

            /// <summary>
            /// 基本授权
            /// </summary>
            [Browsable(false)]
            public bool[,] BaseGrantmask
            {
                get
                {
                    return baseGrantMask;
                }
            }

            /// <summary>
            /// 高级授权
            /// </summary>
            [Browsable(false)]
            public bool[ , , ] AdvanceGrantMask
            {
                get
                {
                    return advanceGrantMask;
                }
            }

            /// <summary>
            /// 作为会长初始化
            /// </summary>
            public void InitAsLeader()
            {
                enable = true;

                for (int i = 0; i < 8; i++)
                {
                    baseOperationMask[i] = true;
                }

                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        advanceOperationMask[i, j] = true;
                        baseGrantMask[i, j] = true;
                    }
                    
                    for (int j = 0; j < 16; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            advanceGrantMask[i, j, k] = true;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 帮会模板
        /// </summary>
        class TongTemplate
        {
            private int masterGroup = 0; // 领队编号
            private int defaultGroup = 0; // 默认成员编号
            private TongGroup[] tongGroupArray = new TongGroup[16]; // 成员数据

            /// <summary>
            /// 构造函数
            /// </summary>
            /// <param name="masterGroup">领队编号</param>
            /// <param name="defaultGroup">默认成员编号</param>
            /// <param name="tongGroupArray">成员数据</param>
            public TongTemplate(int masterGroup, int defaultGroup, TongGroup[] tongGroupArray)
            {
                this.masterGroup = masterGroup;
                this.defaultGroup = defaultGroup;
                this.tongGroupArray = tongGroupArray;
            }

            /// <summary>
            /// 领队编号
            /// </summary>
            [Browsable(true)]
            [Category("帮会属性")]
            [DisplayName("领队编号")]
            [Description("领队编号")]
            public int MasterGroup
            {
                set
                {
                    if (value >= 0 && value < 16)
                    {
                        masterGroup = value;
                    }
                    else
                    {
                        MessageBox.Show("编号值只能在0到15之间！", "有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }

                get
                {
                    return masterGroup;
                }
            }

            // 默认成员编号
            [Browsable(true)]
            [Category("帮会属性")]
            [DisplayName("默认编号")]
            [Description("默认编号")]
            public int DefaultGroup
            {
                set
                {
                    if (value >= 0 && value < 16)
                    {
                        defaultGroup = value;
                    }
                    else
                    {
                        MessageBox.Show("编号值只能在0到15之间！", "有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }

                get
                {
                    return defaultGroup;
                }
            }

            /// <summary>
            /// 帮会成员数组
            /// </summary>
            [Browsable(false)]
            public TongGroup[] TongGroupArray
            {
                get
                {
                    return tongGroupArray;
                }
            }
        }

        /// <summary>
        /// 刷新帮会成员树
        /// </summary>
        private void RefreshTongTree()
        {
            tongTree.Nodes.Clear();

            // 初始化帮会树
            for (int i = 0; i < tongTemplateList.Count; i++)
            {
                TongGroup[] tArray = tongTemplateList[i].TongGroupArray;
                Node tongNode = new Node();
                int tongIndex = i + 1;
                tongNode.Text = string.Format("帮会{0}", tongIndex);
                tongNode.Tag = i.ToString();
                tongTree.Nodes.Add(tongNode);

                // 初始化成员树
                for (int j = 0; j < 16; j++)
                {
                    Node node = new Node();
                    node.Text = GetNodeDisplayText(i, j);
                    node.Tag = j.ToString();
                    tongNode.Nodes.Add(node);
                }

                tongNode.Expand();
            }                

            // 自动选中第一个结点，刷新数据
            tongTree.SelectedNode = tongTree.Nodes[0].Nodes[0];
        }

        /// <summary>
        /// 获取帮会组链表二进制数据
        /// </summary>
        /// <returns>帮会组链表二进制数据</returns>
        private byte[] GetTongGroupArrayListByteData()
        {
            List<byte> byteList = new List<byte>();
            byte[] buffer;

            // 记录版本号
            int fileVersion = 3;
            buffer = BitConverter.GetBytes(fileVersion);
            byteList.AddRange(buffer);

            // 记录帮会数量
            int groupCount = tongTemplateList.Count;
            buffer = BitConverter.GetBytes(groupCount);
            byteList.AddRange(buffer);

            // 记录帮会数据
            foreach (TongTemplate tongTemplate in tongTemplateList)
            {
                
                // 转换领队索引
                buffer = BitConverter.GetBytes(tongTemplate.MasterGroup);
                byteList.AddRange(buffer);

                // 转换默认成员索引
                buffer = BitConverter.GetBytes(tongTemplate.DefaultGroup);
                byteList.AddRange(buffer);

                TongGroup[] tArray = tongTemplate.TongGroupArray;
                List<byte> dataList = GetTongGroupByteList(tArray);
                byteList.AddRange(dataList);
            }

            return byteList.ToArray();
        }

        /// <summary>
        /// 初始化帮会数组链表
        /// </summary>
        /// <param name="byteArray">字节数组</param>
        private void InitTongGroupArrayListByByte(byte[] byteArray, int tongDataLength)
        {            
            tongTemplateList.Clear();

            for (int i = 0; i < byteArray.Length; i += tongDataLength)
            {
                List<byte> byteList = new List<byte>();

                for (int j = 0; j < tongDataLength; j++)
                {
                    byteList.Add(byteArray[i + j]);
                }
                
                TongTemplate tongTemplate = GetTongTemplate(byteList);
                tongTemplateList.Add(tongTemplate);
            }
        }

        /// <summary>
        /// 获取帮会模板数据
        /// </summary>
        /// <param name="byteList">数据字节链表</param>
        /// <returns>帮会模板数据</returns>
        private TongTemplate GetTongTemplate(List<byte> byteList)
        {
            int masterGroup = 0;
            int defaultGroup = 0;            
            TongGroup[] tArray = new TongGroup[16];

            byte[] buffer;
            int currentIndex = 0;

            // 读取领队索引
            buffer = new byte[4];
            for (int i = 0; i < 4; i++)
            {
                buffer[i] = byteList[currentIndex + i];
            }
            masterGroup = BitConverter.ToInt32(buffer, 0);
            currentIndex += 4;

            // 读取默认成员索引            
            buffer = new byte[4];
            for (int i = 0; i < 4; i++)
            {
                buffer[i] = byteList[currentIndex + i];
            }
            defaultGroup = BitConverter.ToInt32(buffer, 0);
            currentIndex += 4;

            // 读取分组
            for (int i = 0; i < 16; i++)
            {
                TongGroup tongGroup = new TongGroup();

                // 读取分组名
                List<byte> tempByteList = new List<byte>();
                for (int j = 0; j < 32; j++)
                {
                    byte b = byteList[currentIndex + j];
                    char c = Convert.ToChar(b);

                    if (c == '\0')
                    {
                        break;
                    }
                    else
                    {
                        tempByteList.Add(b);
                    }
                }

                string groupName = Encoding.GetEncoding("gb2312").GetString(tempByteList.ToArray());
                tongGroup.GroupName = groupName;
                currentIndex += 32;

                // 读取是否开启
                bool enable = Convert.ToBoolean(byteList[currentIndex]);
                tongGroup.Enable = enable;
                currentIndex += 1;

                // 读取工资额度
                if (fileVersion < 3)
                {
                    buffer = new byte[4];
                    for (int j = 0; j < 4; j++)
                    {
                        buffer[j] = byteList[currentIndex + j];
                    }
                    int wage = BitConverter.ToInt32(buffer, 0);
                    // tongGroup.Wage = wage;
                    currentIndex += 4;
                }
                
                // 读取基本操作权限
                byte bMask = byteList[currentIndex];
                currentIndex += 1;
                bool[] boolArray = ConvertByteToBooleanArray(bMask);
                
                for (int j = 0; j < 8; j++)
                {
                    tongGroup.BaseOperationMask[j] = boolArray[j];
                }                

                // 读取高级操作权限
                for (int j = 0; j < 16; j++)
                {
                    bMask = byteList[currentIndex];
                    currentIndex += 1;
                    boolArray = ConvertByteToBooleanArray(bMask);

                    for (int k = 0; k < 8; k++)
                    {
                        tongGroup.AdvanceOperationMask[j, k] = boolArray[k];
                    }                                       
                }

                // 读取基本授权
                for (int j = 0; j < 16; j++)
                {
                    bMask = byteList[currentIndex];
                    currentIndex += 1;
                    boolArray = ConvertByteToBooleanArray(bMask);

                    for (int k = 0; k < 8; k++)
                    {
                        tongGroup.BaseGrantmask[j, k] = boolArray[k];
                    }
                }

                // 读取高级授权
                for (int j = 0; j < 16; j++)
                {
                    for (int k = 0; k < 16; k++)
                    {
                        bMask = byteList[currentIndex];
                        currentIndex += 1;
                        boolArray = ConvertByteToBooleanArray(bMask);

                        for (int l = 0; l < 8; l++)
                        {
                            tongGroup.AdvanceGrantMask[j, k, l] = boolArray[l];
                        }
                    }                                           
                }

                tArray[i] = tongGroup;
            }

            TongTemplate tongTemplate = new TongTemplate(masterGroup, defaultGroup, tArray);
            return tongTemplate;
        }

        /// <summary>
        /// 获取帮会数组数据字节链表
        /// </summary>
        /// <param name="tongGroupArray">帮会数组</param>
        /// <return>帮会数组数据字节链表</return>
        private List<byte> GetTongGroupByteList(TongGroup[] tongGroupArray)
        {
            List<byte> byteList = new List<byte>();
            byte[] buffer;            

            // 转换分组
            foreach (TongGroup tongGroup in tongGroupArray)
            {
                // 分组名需要补充满32字节
                string groupName = tongGroup.GroupName;
                buffer = Encoding.GetEncoding("gb2312").GetBytes(groupName);                
                
                byte[] groupNameBuffer = new byte[32];
                for (int i = 0; i < buffer.Length; i++)
                {
                    groupNameBuffer[i] = buffer[i];
                }
                byteList.AddRange(groupNameBuffer);

                // 转换是否开启
                buffer = BitConverter.GetBytes(tongGroup.Enable);
                byteList.AddRange(buffer);

                // 转换基本操作权限
                string tempString = ConvertBooleanArrayToString(tongGroup.BaseOperationMask);
                byte tempByte = byte.Parse(tempString);
                byteList.Add(tempByte);

                // 转换高级操作权限
                bool[] tempArray = new bool[8];
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        tempArray[j] = tongGroup.AdvanceOperationMask[i, j];
                    }

                    tempString = ConvertBooleanArrayToString(tempArray);
                    tempByte = byte.Parse(tempString);
                    byteList.Add(tempByte);
                }

                // 转换基本授权
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        tempArray[j] = tongGroup.BaseGrantmask[i, j];
                    }

                    tempString = ConvertBooleanArrayToString(tempArray);
                    tempByte = byte.Parse(tempString);
                    byteList.Add(tempByte);
                }

                // 转换高级授权
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 16; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            tempArray[k] = tongGroup.AdvanceGrantMask[i, j, k];
                        }

                        tempString = ConvertBooleanArrayToString(tempArray);
                        tempByte = byte.Parse(tempString);
                        byteList.Add(tempByte);
                    }
                }
            }

            return byteList;
        }        

        /// <summary>
        /// 把字节转换成布尔数组
        /// </summary>
        /// <param name="b">字节</param>
        /// <returns>布尔数组</returns>
        private bool[] ConvertByteToBooleanArray(byte b)
        {
            bool[] boolArray = new bool[8];
            int[] tempArray = new int[] { 1, 2, 4, 8, 16, 32, 64, 128 };

            for (int i = 0; i < 8; i++)
            {
                boolArray[i] = ((b & tempArray[i]) != 0);
            }

            return boolArray;
        }

        /// <summary>
        /// 把布尔值数组转换成数字字符串
        /// </summary>
        /// <param name="boolArray">布尔值数组</param>
        /// <returns>数字字符串</returns>
        private string ConvertBooleanArrayToString(bool[] boolArray)
        {
            int number = 0;

            if (boolArray[0])
            {
                number += 1;
            }

            if (boolArray[1])
            {
                number += 2;
            }

            if (boolArray[2])
            {
                number += 4;
            }

            if (boolArray[3])
            {
                number += 8;
            }

            if (boolArray[4])
            {
                number += 16;
            }

            if (boolArray[5])
            {
                number += 32;
            }

            if (boolArray[6])
            {
                number += 64;
            }

            if (boolArray[7])
            {
                number += 128;
            }

            return number.ToString();
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        private DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }
            }

            return table;
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {           
            // 加载数据表
            string filePath = Path.Combine(Application.StartupPath, @"TongOperation.tab");
            string content = null;

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();

                tongOperationTable = CreateDataTable(content);
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "加载数据文件",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            filePath = Path.Combine(Application.StartupPath, @"TongOperationGroup.tab");

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();

                tongOperationGroupTable = CreateDataTable(content);
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "加载数据文件",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            // 初始化数据列
            DataGridViewTextBoxColumn operationColumn = new DataGridViewTextBoxColumn();
            operationColumn.Name = "Operation";
            operationColumn.HeaderText = "Operation";

            DataGridViewTextBoxColumn indexColumn = new DataGridViewTextBoxColumn();
            indexColumn.Name = "Index";
            indexColumn.HeaderText = "Index";

            DataGridViewTextBoxColumn descriptionColumn = new DataGridViewTextBoxColumn();
            descriptionColumn.Name = "Description";
            descriptionColumn.HeaderText = "Description";

            DataGridViewComboBoxColumn typeColumn = new DataGridViewComboBoxColumn();
            typeColumn.Name = "Type";
            typeColumn.HeaderText = "Type";
            typeColumn.Items.Add("Common");
            typeColumn.Items.Add("Advance");

            tableOperationView.Columns.Add(operationColumn);
            tableOperationView.Columns.Add(indexColumn);
            tableOperationView.Columns.Add(descriptionColumn);
            tableOperationView.Columns.Add(typeColumn);

            // 初始化帮会数组
            TongGroup[] tongGroupArray = CreateTongGroup();     
            tongTemplateList.Add(new TongTemplate(0, 0, tongGroupArray));

            // 刷新数据
            RefreshData();

            // 刷新分类树
            RefreshTongTree();

            // 居然是窗体最大化引起的树列单元格对齐的问题...
            this.WindowState = FormWindowState.Maximized;
        }

        /// <summary>
        /// 创建帮会数组
        /// </summary>
        /// <returns>帮会数组</returns>
        private TongGroup[] CreateTongGroup()
        {
            TongGroup[] newTongGroupArray = new TongGroup[16];

            TongGroup tongLeader = new TongGroup();
            tongLeader.InitAsLeader();
            newTongGroupArray[0] = tongLeader;

            for (int i = 1; i < newTongGroupArray.Length; i++)
            {
                newTongGroupArray[i] = new TongGroup();
            }

            return newTongGroupArray;
        }

        /// <summary>
        /// 获取树结点描述字符串
        /// </summary>
        /// <param name="tongIndex">帮会编号</param>
        /// <param name="groupID">分组ID</param>
        /// <returns>树结点描述字符串</returns>
        private string GetNodeDisplayText(int tongIndex, int groupID)
        {
            TongGroup[] tGroup = tongTemplateList[tongIndex].TongGroupArray;
            string displayText = groupID.ToString();

            string groupName = tGroup[groupID].GroupName;
            if (groupName != "")
            {
                displayText = string.Format("{0} {1}", groupID, groupName);
            }

            if (tGroup[groupID].Enable)
            {
                displayText += " √";
            }

            return displayText;
        }

        /// <summary>
        /// 加载帮会数据
        /// </summary>
        /// <param name="tongIndex">帮会组编号</param>
        /// <param name="groupID">帮会ID</param>
        private void LoadTongData(int tongIndex, int groupID)
        {
            TongGroup tongGroup = tongTemplateList[tongIndex].TongGroupArray[groupID];

            // 加载基本操作权限
            foreach (Node node in commonOperationTree.Nodes)
            {
                int index = int.Parse(node.Cells[0].Tag.ToString());
                node.Checked = tongGroup.BaseOperationMask[index];
            }

            // 加载高级操作权限
            foreach (Node node in advanceOperationTree.Nodes)
            {
                int index = int.Parse(node.Cells[0].Tag.ToString());
                bool isNodeChecked = false;
                Node lastNode = node.LastNode;

                foreach (Node childNode in lastNode.Nodes)
                {
                    int memberID = int.Parse(childNode.Tag.ToString());
                    childNode.Checked = tongGroup.AdvanceOperationMask[memberID, index];

                    if (childNode.Checked)
                    {
                        isNodeChecked = true;
                    }
                }

                node.Checked = isNodeChecked;                
            }

            // 加载基本授权
            foreach (Node memberNode in commonAuthorityTree.Nodes)
            {
                int memberID = int.Parse(memberNode.Tag.ToString());
                bool isNodeChecked = false;

                foreach (Node childNode in memberNode.Nodes)
                {
                    int index = int.Parse(childNode.Cells[1].Tag.ToString());
                    childNode.Checked = tongGroup.BaseGrantmask[memberID, index];

                    if (childNode.Checked)
                    {
                        isNodeChecked = true;
                    }
                }

                memberNode.Checked = isNodeChecked;
            }

            // 加载高级授权
            foreach (Node memberNode in advanceAuthorityTree.Nodes)
            {
                int memberID = int.Parse(memberNode.Tag.ToString());
                bool childChecked = false;

                foreach (Node childNode in memberNode.Nodes)
                {
                    int index = int.Parse(childNode.Cells[1].Tag.ToString());
                    bool objectChecked = false;
                    Node lastNode = childNode.LastNode;

                    foreach (Node objectNode in lastNode.Nodes)
                    {
                        int objectID = int.Parse(objectNode.Tag.ToString());
                        objectNode.Checked = tongGroup.AdvanceGrantMask[memberID, objectID, index];

                        if (objectNode.Checked)
                        {
                            objectChecked = true;
                        }
                    }

                    childNode.Checked = objectChecked;

                    if (childNode.Checked)
                    {
                        childChecked = true;
                    }
                }

                memberNode.Checked = childChecked;
            }
        }

        /// <summary>
        /// 更新帮会数据
        /// </summary>
        /// <param name="tongIndex">帮会组ID</param>
        /// <param name="groupID">帮会ID</param>
        private void UpdateTongData(int tongIndex,int groupID)
        {
            TongGroup tongGroup = tongTemplateList[tongIndex].TongGroupArray[groupID];

            // 更新基本操作权限
            foreach (Node node in commonOperationTree.Nodes)
            {
                int index = int.Parse(node.Cells[0].Tag.ToString());
                tongGroup.BaseOperationMask[index] = node.Checked;
            }

            // 更新高级操作权限
            foreach (Node node in advanceOperationTree.Nodes)
            {
                int index = int.Parse(node.Cells[0].Tag.ToString());
                Node lastNode = node.LastNode;
                
                if (node.Checked)
                {
                    foreach (Node childNode in lastNode.Nodes)
                    {
                        int memberID = int.Parse(childNode.Tag.ToString());
                        tongGroup.AdvanceOperationMask[memberID, index] = childNode.Checked;
                    }
                }
                else
                {
                    for (int i = 0; i < 16; i++)
                    {
                        tongGroup.AdvanceOperationMask[i, index] = false;
                    }
                }                
            }

            // 更新普通授权
            foreach (Node memberNode in commonAuthorityTree.Nodes)
            {
                int memberID = int.Parse(memberNode.Tag.ToString());

                if (memberNode.Checked)
                {
                    foreach (Node childNode in memberNode.Nodes)
                    {
                        int index = int.Parse(childNode.Cells[1].Tag.ToString());
                        tongGroup.BaseGrantmask[memberID, index] = childNode.Checked;                        
                    }
                }
                else
                {
                    for (int i = 0; i < 8; i++)
                    {
                        tongGroup.BaseGrantmask[memberID, i] = false;
                    }
                }
            }

            // 更新高级授权
            foreach (Node memberNode in advanceAuthorityTree.Nodes)
            {
                int memberID = int.Parse(memberNode.Tag.ToString());

                if (memberNode.Checked)
                {
                    foreach (Node childNode in memberNode.Nodes)
                    {
                        int index = int.Parse(childNode.Cells[1].Tag.ToString());

                        if (childNode.Checked)
                        {
                            Node lastNode = childNode.LastNode;

                            foreach (Node objectNode in lastNode.Nodes)
                            {
                                int objectID = int.Parse(objectNode.Tag.ToString());
                                tongGroup.AdvanceGrantMask[memberID, objectID, index] = objectNode.Checked;
                            }
                        }
                        else
                        {
                            for (int i = 0; i < 16; i++)
                            {
                                tongGroup.AdvanceGrantMask[memberID, i, index] = false;
                            }
                        }                        
                    }
                }
                else
                {
                    for (int i = 0; i < 16; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            tongGroup.AdvanceGrantMask[memberID, i, j] = false;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 更新数据表
        /// </summary>
        private void UpdateTongTable()
        {
            DataTable tempTable = new DataTable();
            DataColumn operationColumn = new DataColumn("Operation");
            DataColumn indexColumn = new DataColumn("Index");
            DataColumn descColumn = new DataColumn("Desc");
            DataColumn typeColumn = new DataColumn("Type");

            tempTable.Columns.Add(operationColumn);
            tempTable.Columns.Add(indexColumn);
            tempTable.Columns.Add(descColumn);
            tempTable.Columns.Add(typeColumn);           

            foreach (DataGridViewRow dataGridViewRow in tableOperationView.Rows)
            {               
                string operationString = dataGridViewRow.Cells["Operation"].EditedFormattedValue as string;
                string indexString = dataGridViewRow.Cells["Index"].EditedFormattedValue as string;
                string descString = dataGridViewRow.Cells["Description"].EditedFormattedValue as string;
                string typeString = dataGridViewRow.Cells["Type"].EditedFormattedValue as string;

                if (!string.IsNullOrEmpty(operationString) && !string.IsNullOrEmpty(indexString) &&
                    !string.IsNullOrEmpty(descString) && !string.IsNullOrEmpty(typeString))
                {
                    DataRow newRow = tempTable.NewRow();
                    newRow["Operation"] = operationString;
                    newRow["Index"] = indexString;
                    newRow["Desc"] = descString;
                    newRow["Type"] = typeString;

                    tempTable.Rows.Add(newRow);
                }                
            }

            tongOperationTable.Rows.Clear();
            DataRow[] dataRows = tempTable.Select("1 = 1", "Type DESC, Index ASC");
            foreach (DataRow dataRow in dataRows)
            {
                DataRow newRow = tongOperationTable.NewRow();
                newRow["Operation"] = dataRow["Operation"];
                newRow["Index"] = dataRow["Index"];
                newRow["Desc"] = dataRow["Desc"];
                newRow["TYPE"] = dataRow["Type"];

                tongOperationTable.Rows.Add(newRow);
            }
        }

        /// <summary>
        /// 把数据表转换为字符串
        /// </summary>
        /// <param name="dataTable">数据表</param>
        /// <returns>字符串</returns>
        private string ConvertDataTableToString(DataTable dataTable)
        {
            StringBuilder content = new StringBuilder();

            StringBuilder titleLine = new StringBuilder();
            foreach(DataColumn dataColumn in dataTable.Columns)
            {
                titleLine.Append(string.Format("{0}\t", dataColumn.ColumnName));
            }
            titleLine.Remove(titleLine.Length - 1, 1);
            content.AppendLine(titleLine.ToString());
                        
            foreach(DataRow dataRow in dataTable.Rows)
            {
                StringBuilder line = new StringBuilder();

                for (int i = 0; i < dataTable.Columns.Count; i++)
                {
                    line.Append(string.Format("{0}\t", dataRow[i].ToString()));
                }
                
                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }            

            return content.ToString().TrimEnd(new char[] { '\r', '\n' });
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">文本内容</param>
        private void SaveDataToFile(string fileName, string content)
        {
            try
            {
                string filePath = Path.GetDirectoryName(fileName);
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if (!Directory.Exists(filePath)) // 目录不存在
                    {
                        Directory.CreateDirectory(filePath); // 先创建目录，再创建文件
                    }

                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                ;
            }
        }

        /// <summary>
        /// 刷新树结点文本
        /// </summary>
        /// <param name="tongIndex">帮会编号</param>
        private void RefreshNodeText(int tongIndex)
        {
            DataRow[] rows;

            // 刷新高级操作面板
            for (int i = 0; i < 8; i++)
            {
                rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Advance'", i.ToString()));

                if (rows.Length > 0)
                {
                    Node parentNode = advanceOperationTree.Nodes[i];
                    Node lastNode = parentNode.LastNode;

                    for (int j = 0; j < 16; j++)
                    {
                        Node memberNode = lastNode.Nodes[j];
                        memberNode.Cells[3].Text = GetNodeDisplayText(tongIndex, j);
                    }
                }
                else
                {
                    break;
                }
            }

            // 刷新基础授权面板
            for (int i = 0; i < 16; i++)
            {
                Node memberNode = commonAuthorityTree.Nodes[i];
                memberNode.Cells[0].Text = GetNodeDisplayText(tongIndex, i);
            }

            // 刷新高级授权面板
            for (int i = 0; i < 16; i++)
            {
                Node memberNode = advanceAuthorityTree.Nodes[i];
                memberNode.Cells[0].Text = GetNodeDisplayText(tongIndex, i);

                for (int j = 0; j < 8; j++)
                {
                    rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Advance'", j.ToString()));

                    if (rows.Length > 0)
                    {
                        Node parentNode = memberNode.Nodes[j];
                        Node lastNode = parentNode.LastNode;

                        for (int k = 0; k < 16; k++)
                        {
                            Node objectNode = lastNode.Nodes[k];
                            objectNode.Cells[0].Text = GetNodeDisplayText(tongIndex, k);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        
        /// <summary>
        /// 获取帮会操作编号的描述
        /// </summary>
        /// <param name="index">帮会操作编号</param>
        /// <param name="operationType">帮会操作类型</param>
        /// <returns>帮会操作编号的描述</returns>
        private string GetTongOperationIndexDesplayText(string index, string operationType)
        {
            string displayText = index;

            DataRow[] rows = tongOperationGroupTable.Select(string.Format("Index = '{0}' AND TYPE = '{1}'", index, operationType));

            if (rows.Length > 0)
            {
                string descrption = rows[0]["Desc"].ToString();

                if (descrption != "")
                {
                    displayText = string.Format("{0} {1}", index, descrption);
                }
            }

            return displayText;
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        private void RefreshData()
        {
            DataRow[] rows;                       

            // 初始化基础操作面板
            commonOperationTree.Nodes.Clear();

            for (int i = 0; i < 8; i++)
            {
                rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Common'", i.ToString()));                

                if (rows.Length > 0)
                {
                    Node parentNode = new Node();                    
                    parentNode.Cells.Add(new Cell());
                    parentNode.Cells.Add(new Cell());
                    parentNode.Cells[0].Text = GetTongOperationIndexDesplayText(i.ToString(), "Common");
                    parentNode.Cells[0].Tag = i.ToString();
                    parentNode.CheckBoxVisible = true;

                    foreach (DataRow row in rows)
                    {
                        Node childNode = new Node();
                        childNode.Cells.Add(new Cell());
                        childNode.Cells.Add(new Cell());
                        childNode.Cells[0].Text = i.ToString();
                        childNode.Cells[1].Text = row["Operation"] as string;
                        childNode.Cells[2].Text = row["Desc"] as string;

                        parentNode.Nodes.Add(childNode);
                    }

                    parentNode.Expand();                    
                    commonOperationTree.Nodes.Add(parentNode);
                }
                else
                {
                    break;
                }
            }            

            // 初始化高级操作面板
            advanceOperationTree.Nodes.Clear();

            for (int i = 0; i < 8; i++)
            {
                rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Advance'", i.ToString()));             

                if (rows.Length > 0)
                {
                    Node parentNode = new Node();
                    parentNode.Cells.Add(new Cell());
                    parentNode.Cells.Add(new Cell());
                    parentNode.Cells.Add(new Cell());
                    parentNode.Cells[0].Text = GetTongOperationIndexDesplayText(i.ToString(), "Advance");
                    parentNode.Cells[0].Tag = i.ToString();
                    parentNode.CheckBoxVisible = true;

                    foreach(DataRow row in rows)
                    {
                        Node childNode = new Node();
                        childNode.Cells.Add(new Cell());
                        childNode.Cells.Add(new Cell());
                        childNode.Cells.Add(new Cell());
                        childNode.Cells[0].Text = i.ToString();
                        childNode.Cells[1].Text = row["Operation"] as string;
                        childNode.Cells[2].Text = row["Desc"] as string;

                        parentNode.Nodes.Add(childNode);
                    }

                    Node lastNode = parentNode.LastNode;

                    for (int j = 0; j < 16; j++)
                    {
                        Node memberNode = new Node();
                        memberNode.Cells.Add(new Cell());
                        memberNode.Cells.Add(new Cell());
                        memberNode.Cells.Add(new Cell());
                        memberNode.Cells[3].Text = j.ToString();
                        memberNode.Tag = j.ToString();
                        memberNode.CheckBoxVisible = true;
                        lastNode.Nodes.Add(memberNode);                        
                    }

                    lastNode.Expand();
                    parentNode.Expand();
                    advanceOperationTree.Nodes.Add(parentNode);                    
                }
                else
                {
                    break;
                }
            }            

            // 初始化基础授权面板
            commonAuthorityTree.Nodes.Clear();

            for (int i = 0; i < 16; i++)
            {
                Node memberNode = new Node();                                
                memberNode.Cells.Add(new Cell());
                memberNode.Cells.Add(new Cell());
                memberNode.Cells.Add(new Cell());
                memberNode.Cells[0].Text = i.ToString();
                memberNode.Tag = i.ToString();
                memberNode.CheckBoxVisible = true;

                for (int j = 0; j < 8; j++)
                {
                    rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Common'", j.ToString()));

                    if (rows.Length > 0)
                    {
                        Node parentNode = new Node();
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells[1].Text = GetTongOperationIndexDesplayText(j.ToString(), "Common");
                        parentNode.Cells[1].Tag = j.ToString();
                        parentNode.CheckBoxVisible = true;

                        foreach (DataRow row in rows)
                        {
                            Node childNode = new Node();
                            childNode.Cells.Add(new Cell());
                            childNode.Cells.Add(new Cell());
                            childNode.Cells.Add(new Cell());
                            childNode.Cells[1].Text = j.ToString();
                            childNode.Cells[2].Text = row["Operation"] as string;
                            childNode.Cells[3].Text = row["Desc"] as string;

                            parentNode.Nodes.Add(childNode);
                        }

                        // parentNode.Expand();
                        memberNode.Nodes.Add(parentNode);
                    }
                    else
                    {
                        break;
                    }
                }

                // memberNode.Expand();
                commonAuthorityTree.Nodes.Add(memberNode);
            }

            // 初始化高级授权面板
            advanceAuthorityTree.Nodes.Clear();

            for (int i = 0; i < 16; i++)
            {
                Node memberNode = new Node();
                memberNode.Cells.Add(new Cell());
                memberNode.Cells.Add(new Cell());
                memberNode.Cells.Add(new Cell());
                memberNode.Cells[0].Text = i.ToString();
                memberNode.Tag = i.ToString();
                memberNode.CheckBoxVisible = true;

                for (int j = 0; j < 8; j++)
                {
                    rows = tongOperationTable.Select(string.Format("Index = '{0}' AND TYPE = 'Advance'", j.ToString()));

                    if (rows.Length > 0)
                    {
                        Node parentNode = new Node();
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells.Add(new Cell());
                        parentNode.Cells[1].Text = GetTongOperationIndexDesplayText(j.ToString(), "Advance");
                        parentNode.Cells[1].Tag = j.ToString();
                        parentNode.CheckBoxVisible = true;

                        foreach (DataRow row in rows)
                        {
                            Node childNode = new Node();
                            childNode.Cells.Add(new Cell());
                            childNode.Cells.Add(new Cell());
                            childNode.Cells.Add(new Cell());
                            childNode.Cells[1].Text = j.ToString();
                            childNode.Cells[2].Text = row["Operation"] as string;
                            childNode.Cells[3].Text = row["Desc"] as string;  
                          
                            parentNode.Nodes.Add(childNode);
                        }

                        Node lastNode = parentNode.LastNode;

                        for (int k = 0; k < 16; k++)
                        {
                            Node objectNode = new Node();
                            objectNode.Cells.Add(new Cell());
                            objectNode.Cells.Add(new Cell());
                            objectNode.Cells.Add(new Cell());
                            objectNode.Cells[0].Text = k.ToString();
                            objectNode.Tag = k.ToString();
                            objectNode.CheckBoxVisible = true;

                            lastNode.Nodes.Add(objectNode);
                        }

                        // lastNode.Expand();                        
                        // parentNode.Expand();
                        memberNode.Nodes.Add(parentNode);
                    }
                    else
                    {
                        break;
                    }                    
                }

                memberNode.Expand();
                advanceAuthorityTree.Nodes.Add(memberNode);
            }

            // 初始化操作配置表
            tableOperationView.Rows.Clear();

            for (int i = 0; i < tongOperationTable.Rows.Count; i++)
            {
                DataRow dataRow = tongOperationTable.Rows[i];
                tableOperationView.Rows.Add(1);
                DataGridViewRow newRow = tableOperationView.Rows[i];

                newRow.Cells["Operation"].Value = dataRow["Operation"].ToString();
                newRow.Cells["Index"].Value = dataRow["Index"].ToString();
                newRow.Cells["Description"].Value = dataRow["Desc"].ToString();
                newRow.Cells["Type"].Value = dataRow["TYPE"].ToString();
            }
        }

        /// <summary>
        /// 勾选高级操作选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advanceOperationTree_AfterCheck(object sender, AdvTreeCellEventArgs e)
        {
            if (beginEdit)
            {
                Node currentNode = advanceOperationTree.SelectedNode;

                if (currentNode != null)
                {
                    if (currentNode.Level == 0)
                    {
                        Node lastNode = currentNode.LastNode;
                        foreach (Node node in lastNode.Nodes)
                        {
                            node.Checked = currentNode.Checked;
                        }
                    }

                    if (currentNode.CheckBoxVisible && currentNode.Checked) // 自动勾选父结点
                    {
                        beginEdit = false;
                        Node parentNode = currentNode.Parent;

                        while (parentNode != null)
                        {
                            if (parentNode.CheckBoxVisible)
                            {
                                parentNode.Checked = true;
                            }

                            parentNode = parentNode.Parent;
                        }

                        beginEdit = true;
                    } 
                }

                groupContentChanged = true;
            }            
        }

        /// <summary>
        /// 勾选高级授权选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advanceAuthorityTree_AfterCheck(object sender, AdvTreeCellEventArgs e)
        {
            if (beginEdit)
            {
                Node currentNode = advanceAuthorityTree.SelectedNode;

                if (currentNode != null)
                {
                    switch (currentNode.Level)
                    {
                        case 0:
                            {
                                foreach (Node node in currentNode.Nodes)
                                {
                                    node.Checked = currentNode.Checked;

                                    Node lastNode = node.LastNode;

                                    foreach (Node childNode in lastNode.Nodes)
                                    {
                                        childNode.Checked = currentNode.Checked;
                                    }
                                }

                                break;
                            }
                        case 1:
                            {
                                Node lastNode = currentNode.LastNode;

                                foreach (Node childNode in lastNode.Nodes)
                                {
                                    childNode.Checked = currentNode.Checked;
                                }

                                break;
                            }
                    }

                    if (currentNode.CheckBoxVisible && currentNode.Checked) // 自动勾选父结点
                    {
                        beginEdit = false;
                        Node parentNode = currentNode.Parent;

                        while (parentNode != null)
                        {
                            if (parentNode.CheckBoxVisible)
                            {
                                parentNode.Checked = true;
                            }

                            parentNode = parentNode.Parent;
                        }

                        beginEdit = true;
                    }                    
                }

                groupContentChanged = true;
            }
            
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRefresh_Click(object sender, EventArgs e)
        {
            // 刷新数据表内容
            UpdateTongTable();

            // 刷新窗体显示数据
            RefreshData();

            // 刷新树结点的显示
            lastTongIndex = -1;
            tongTree.SelectedNode = null;
            tongTree.SelectedNode = tongTree.Nodes[0].Nodes[0];
        }

        /// <summary>
        /// 关闭窗体
        /// </summary>
        /// <param name="sender">事件</param>
        /// <param name="e"></param>
        private void bExit_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定关闭当前窗体？", "关闭窗体", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                this.Close();
            }            
        }

        /// <summary>
        /// 勾选基础授权选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void commonAuthorityTree_AfterCheck(object sender, AdvTreeCellEventArgs e)
        {
            if (beginEdit)
            {
                Node currentNode = commonAuthorityTree.SelectedNode;

                if (currentNode != null)
                {
                    if (currentNode.Level == 0)
                    {
                        foreach (Node node in currentNode.Nodes)
                        {
                            node.Checked = currentNode.Checked;
                        }
                    }                    

                    if (currentNode.CheckBoxVisible && currentNode.Checked) // 自动勾选父结点
                    {
                        beginEdit = false;
                        Node parentNode = currentNode.Parent;

                        while (parentNode != null)
                        {
                            if (parentNode.CheckBoxVisible)
                            {
                                parentNode.Checked = true;
                            }

                            parentNode = parentNode.Parent;
                        }

                        beginEdit = true;
                    } 
                }

                groupContentChanged = true;
            }                                    
        }

        /// <summary>
        /// 勾选基础操作选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void commonOperationTree_AfterCheck(object sender, AdvTreeCellEventArgs e)
        {
            if (beginEdit)
            {
                groupContentChanged = true;
            }            
        }

        /// <summary>
        /// 切换树结点后
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            if (e.Node != null)
            {
                if (e.Node.Level == 1)
                {
                    int groupID = int.Parse(e.Node.Tag.ToString());
                    int tongIndex = int.Parse(e.Node.Parent.Tag.ToString());

                    if (lastTongIndex != tongIndex)
                    {
                        RefreshNodeText(tongIndex);
                        lastTongIndex = tongIndex;
                    }

                    beginEdit = false;
                    LoadTongData(tongIndex, groupID);
                    groupContentChanged = false;
                    beginEdit = true;

                    // 刷新是否隐藏项
                    switch (displayMode)
                    {
                        case DisplayMode.ShowAllItem:
                            {
                                break;
                            }
                        case DisplayMode.HideCheckedItem:
                            {
                                HideItem(true);
                                break;
                            }
                        case DisplayMode.HideUncheckedItem:
                            {
                                HideItem(false);
                                break;
                            }
                    }

                    TongGroup tongGroup = tongTemplateList[tongIndex].TongGroupArray[groupID];
                    tongPropertyGrid.SelectedObject = tongGroup;
                }
                else if (e.Node.Level == 0)
                {
                    int tongIndex = int.Parse(e.Node.Tag.ToString());
                    TongTemplate tongTemplate = tongTemplateList[tongIndex];
                    tongPropertyGrid.SelectedObject = tongTemplate;
                }
            }            
        }

        /// <summary>
        /// 树结点被切换之后
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_AfterNodeDeselect(object sender, AdvTreeNodeEventArgs e)
        {
            if (e.Node != null && e.Node.Level == 1)
            {
                if (groupContentChanged)
                {
                    // 去掉保存信息
                    // if (MessageBox.Show("帮会信息改变，是否保存数据？", "编辑帮会信息", MessageBoxButtons.OKCancel,
                    //                    MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        int groupID = int.Parse(e.Node.Tag.ToString());
                        int tongIndex = int.Parse(e.Node.Parent.Tag.ToString());
                        UpdateTongData(tongIndex, groupID);
                    }
                } 
            }             
        }

        /// <summary>
        /// 添加帮会
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddTong_Click(object sender, EventArgs e)
        {
            // 先保存当前分组的数据
            Node currentNode = tongTree.SelectedNode;
            if (currentNode != null && currentNode.Level == 1 && groupContentChanged)
            {
                // 去掉保存提示
                // if (MessageBox.Show("帮会信息改变，是否保存数据？", "编辑帮会信息", MessageBoxButtons.OKCancel,
                //                     MessageBoxIcon.Question) == DialogResult.OK)
                {
                    int groupID = int.Parse(currentNode.Tag.ToString());
                    int tongIndex = int.Parse(currentNode.Parent.Tag.ToString());
                    UpdateTongData(tongIndex, groupID);
                }
            }

            TongGroup[] newTongGroupArray = CreateTongGroup();         
            tongTemplateList.Add(new TongTemplate(0, 0, newTongGroupArray));

            // 刷新分类树
            groupContentChanged = false;
            RefreshTongTree();
        }

        /// <summary>
        /// 删除帮会
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteTong_Click(object sender, EventArgs e)
        {
            Node currentNode = tongTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 0)
            {
                if (tongTemplateList.Count > 1)
                {
                    if (MessageBox.Show("确定删除当前帮会？", "删除帮会", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        int tongIndex = int.Parse(currentNode.Tag.ToString());   
                        tongTemplateList.RemoveAt(tongIndex);
                        groupContentChanged = false;
                        RefreshTongTree();
                    }                
                }
                else
                {
                    MessageBox.Show("需要至少存在一个帮会！", "删除帮会", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先选择要删除的帮会!！", "删除帮会", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSave_Click(object sender, EventArgs e)
        {
            SaveConfigForm sForm = new SaveConfigForm();

            if (sForm.ShowDialog() == DialogResult.OK)
            {
                string fileFolder;
                string fileName;

                // 保存TongTemplate.dat文件
                if (sForm.SaveTongTemplateFile)
                {
                    fileFolder = sForm.TongTemplateFolder;

                    // 先保存数据
                    Node currentNode = tongTree.SelectedNode;
                    if (currentNode != null && currentNode.Level == 1 && groupContentChanged)
                    {
                        int groupID = int.Parse(currentNode.Tag.ToString());
                        int tongIndex = int.Parse(currentNode.Parent.Tag.ToString());
                        UpdateTongData(tongIndex, groupID);
                    }            

                    if (!Directory.Exists(fileFolder)) // 目录不存在
                    {
                        Directory.CreateDirectory(fileFolder); // 先创建目录，再创建文件
                    }

                    fileName = Path.Combine(fileFolder, "TongTemplate.dat");                    
                    FileStream fileStream = new FileStream(fileName, FileMode.OpenOrCreate);

                    byte[] buffer = GetTongGroupArrayListByteData();
                    fileStream.Write(buffer, 0, buffer.Length);
                    fileStream.Close();

                    MessageBox.Show(string.Format("文件{0}保存成功！", fileName), "保存文件", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);    
                }

                // 保存TongOperation.tab文件
                if (sForm.SaveTongOperationFile)
                {
                    fileFolder = sForm.TongOperationFolder;

                    // 先保存数据
                    UpdateTongTable();

                    string content = ConvertDataTableToString(tongOperationTable);
                    fileName = Path.Combine(fileFolder, "TongOperation.tab");
                    SaveDataToFile(fileName, content);

                    MessageBox.Show(string.Format("文件{0}保存成功！", fileName), "保存文件", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information); 
                }
            }                    
        }

        /// <summary>
        /// 加载数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bLoad_Click(object sender, EventArgs e)
        {
            bool loadSuccess = true;

            // 读取数据表
            string fileName = Path.Combine(Application.StartupPath, @"TongOperation.tab");
            string content = null;

            if (File.Exists(fileName))
            {
                StreamReader reader = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();

                tongOperationTable = CreateDataTable(content);
            }
            else
            {
                loadSuccess = false;
                MessageBox.Show(string.Format("文件{0}不存在！", fileName), "加载文件", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }

            fileName = Path.Combine(Application.StartupPath, @"TongOperationGroup.tab");

            if (File.Exists(fileName))
            {
                StreamReader reader = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();

                tongOperationGroupTable = CreateDataTable(content);
            }
            else
            {
                loadSuccess = false;
                MessageBox.Show(string.Format("文件{0}不存在！", fileName), "加载文件", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }

            // 读取帮会数据
            fileName = Path.Combine(Application.StartupPath, "TongTemplate.dat");            

            if (File.Exists(fileName))
            {
                FileStream fileStream = new FileStream(fileName, FileMode.Open);

                // 读取version
                byte[] buffer = new byte[4];

                for (int i = 0; i < 4; i++)
                {
                    buffer[i] = (byte)fileStream.ReadByte();
                }

                fileVersion = BitConverter.ToInt32(buffer, 0);

                // 读取帮会数量
                buffer = new byte[4];

                for (int i = 0; i < 4; i++)
                {
                    buffer[i] = (byte)fileStream.ReadByte();
                }

                int tongCount = BitConverter.ToInt32(buffer, 0);

                // 读取帮会信息
                int tongDataLength = 5220;
                switch (fileVersion)
                {
                    case 1:
                        {
                            break;
                        }
                    case 2:
                        {
                            tongDataLength = 5224;
                            break;
                        }
                    case 3:
                        {
                            tongDataLength = 5160;
                            break;
                        }
                }

                buffer = new byte[tongCount * tongDataLength];
                fileStream.Read(buffer, 0, buffer.Length);
                fileStream.Close();
                InitTongGroupArrayListByByte(buffer, tongDataLength);

                // 初始化帮会树
                lastTongIndex = -1; // 刷一次树结点文本
                groupContentChanged = false;
                RefreshTongTree();
            }
            else
            {
                loadSuccess = false;
                MessageBox.Show(string.Format("文件{0}不存在！", fileName), "加载文件", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);                
            }
            
            if (loadSuccess)
            {
                MessageBox.Show("文件加载成功！", "加载文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 帮会成员属性值改变
        /// </summary>
        /// <param name="s">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tongPropertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            Node currentNode = tongTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                int groupID = int.Parse(currentNode.Tag.ToString());
                int tongIndex = int.Parse(currentNode.Parent.Tag.ToString());                
                currentNode.Text = GetNodeDisplayText(tongIndex, groupID);

                RefreshNodeText(tongIndex);
            }
        }

        /// <summary>
        /// 帮会树鼠标按下
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tongTree_NodeMouseDown(object sender, TreeNodeMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && e.Node != null && e.Node.Level == 0)
            {
                Point location = new Point(e.X, e.Y) + new Size(tongTree.RectangleToScreen(tongTree.ClientRectangle).Location)
                                 - new Size(tongTree.AutoScrollOffset); // 坐标转换，处理滚动条滚动的情况
                bTong.Popup(location);
            }            
        }

        /// <summary>
        /// 隐藏项
        /// </summary>
        /// <param name="isChecked">是否勾选</param>
        private void HideItem(bool isChecked)
        {
            Node lastCheckedNode = null;
            bool allNodeInvisible = true;

            // 刷新基础操作面板            
            foreach (Node parentNode in commonOperationTree.Nodes)
            {
                if (parentNode.Checked == isChecked)
                {
                    lastCheckedNode = parentNode;
                    parentNode.Visible = false;
                }
                else
                {
                    parentNode.Visible = true;
                    allNodeInvisible = false;
                }
            }

            // 至少要有一个结点可见
            if (allNodeInvisible)
            {
                lastCheckedNode.Visible = true;
            }

            // 刷新高级操作面板
            foreach (Node parentNode in advanceOperationTree.Nodes)
            {
                allNodeInvisible = true;

                foreach (Node childNode in parentNode.LastNode.Nodes)
                {
                    if (childNode.Checked == isChecked)
                    {
                        lastCheckedNode = childNode;
                        childNode.Visible = false;
                    }
                    else
                    {
                        childNode.Visible = true;
                        allNodeInvisible = false;
                    }
                }

                // 至少要有一个结点可见
                if (allNodeInvisible)
                {
                    lastCheckedNode.Visible = true;
                }
            }

            // 刷新基础授权面板
            foreach (Node parentNode in commonAuthorityTree.Nodes)
            {
                foreach (Node childNode in parentNode.Nodes)
                {
                    allNodeInvisible = true;

                    foreach (Node memberNode in childNode.Nodes)
                    {
                        if (memberNode.Checked == isChecked)
                        {
                            lastCheckedNode = memberNode;
                            memberNode.Visible = false;
                        }
                        else
                        {
                            memberNode.Visible = true;
                            allNodeInvisible = false;
                        }
                    }

                    // 至少要有一个结点可见
                    if (allNodeInvisible)
                    {
                        lastCheckedNode.Visible = true;
                    }
                }
            }

            // 刷新高级授权面板
            foreach (Node memberNode in advanceAuthorityTree.Nodes)
            {
                foreach (Node parentNode in memberNode.Nodes)
                {
                    allNodeInvisible = true;

                    foreach (Node objectNode in parentNode.LastNode.Nodes)
                    {
                        if (objectNode.Checked == isChecked)
                        {
                            lastCheckedNode = objectNode;
                            objectNode.Visible = false;
                        }
                        else
                        {
                            objectNode.Visible = true;
                            allNodeInvisible = false;
                        }
                    }

                    // 至少要有一个结点可见
                    if (allNodeInvisible)
                    {
                        lastCheckedNode.Visible = true;
                    }
                }
            } 
        }

        /// <summary>
        /// 隐藏已勾选的项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bHideCheckedItem_Click(object sender, EventArgs e)
        {
            displayMode = DisplayMode.HideCheckedItem;
            HideItem(true);
            bHideCheckedItem.Text = "隐藏已勾选的项 √";
            bHideUncheckedItem.Text = "隐藏未勾选的项";
            bShowAllItem.Text = "显示所有项";
        }

        /// <summary>
        /// 显示所有项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bShowAllItem_Click(object sender, EventArgs e)
        {
            displayMode = DisplayMode.ShowAllItem;

            // 刷新基础操作面板
            foreach (Node parentNode in commonOperationTree.Nodes)
            {
                parentNode.Visible = true;
            }

            // 刷新高级操作面板
            foreach (Node parentNode in advanceOperationTree.Nodes)
            {
                foreach (Node childNode in parentNode.LastNode.Nodes)
                {
                    childNode.Visible = true;
                }
            }

            // 刷新基础授权面板
            foreach (Node parentNode in commonAuthorityTree.Nodes)
            {
                foreach (Node childNode in parentNode.Nodes)
                {
                    foreach (Node memberNode in childNode.Nodes)
                    {
                        memberNode.Visible = true;
                    }
                }
            }

            // 刷新高级授权面板
            foreach (Node memberNode in advanceAuthorityTree.Nodes)
            {
                foreach (Node parentNode in memberNode.Nodes)
                {
                    foreach (Node objectNode in parentNode.LastNode.Nodes)
                    {
                        objectNode.Visible = true;
                    }
                }
            }

            bHideCheckedItem.Text = "隐藏已勾选的项";
            bHideUncheckedItem.Text = "隐藏未勾选的项";
            bShowAllItem.Text = "显示所有项 √";
        }

        /// <summary>
        /// 隐藏未勾选的项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bHideUncheckedItem_Click(object sender, EventArgs e)
        {
            displayMode = DisplayMode.HideUncheckedItem;
            HideItem(false);
            bHideCheckedItem.Text = "隐藏已勾选的项";
            bHideUncheckedItem.Text = "隐藏未勾选的项 √";
            bShowAllItem.Text = "显示所有项";
        }
    }
}