using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using System.Text.RegularExpressions;

namespace CustomLoadTree
{
    public partial class ComputeForm : Form
    {
        private TreeNode node; // 开始计算的父结点
        private Hashtable table; // 记录结点信息数据的hash表
        private SqlConnection conn; // sql连接
        private List<KeyValuePair<string, string>> kindList = new List<KeyValuePair<string, string>>(); // 任务类型键值对链表
        private List<KeyValuePair<string, Logic.QuestType>> questTypeList = new List<KeyValuePair<string, Logic.QuestType>>(); // 任务类型键值对表(转换用)
        private List<KeyValuePair<string, string>> mapList = new List<KeyValuePair<string, string>>(); // 物品地图键值对链表
        private List<float[]> virtualFlowSectionList = new List<float[]>(); // 实际流量区间链表
        private List<float[]> additionalFlowSectionList = new List<float[]>(); // 加算流量区间链表
        private List<List<string>> coTaskList = new List<List<string>>(); // 同期任务链表
        private List<string> resourceList = new List<string>(); // 资源链表
        private List<string> publicResourceList = new List<string>(); // 公共资源链表
        private List<int> fillableColumnList = new List<int>(); // 可自动填充列序号链表
        private List<int> defaultColumnList = new List<int>(); // 可恢复默认值的列序号链表
        private List<int> simpleModeList = new List<int>(); // 精简模式可以隐藏的列序号链表
        private List<int> savableColumnList = new List<int>(); // 可存的列序号链表
        private int npcKind = 0; // npc的种类
        private int npcReviveTime = 0; // npc的刷新时间
        private int doodadReviveDelay = 0; // doodad的刷新时间
        private int index = 0; // 行序号
        private Progress pForm = new Progress(); //进度条窗口
        private int nodeTotal = 0; // 树结点总数
        private int nodeCount = 0; // 当前树结点数
        private DataGridViewCell computeCell; // 正在处理的单元格
        private string plusTable = "sys_QuestPlus"; // 附加表格
        private bool selectAll = true; // 是否全部选中
        private bool startEdit = false; // 是否已开始编辑
        private bool simpleMode = false; // 是否是精简模式
        private Color readOnlyColor = Color.LavenderBlush; // 只读单元格的颜色
        private Color savableColor = Color.LightYellow; // 可存单元格的颜色
        private SetArgument sa = new SetArgument(); // 设置参数数值窗口
        private TaskData taskData = new TaskData(); // 任务数值类
        private Hashtable finalMoneyTable = new Hashtable(); // 金钱终值表
        private Hashtable finalExpTable = new Hashtable(); // 经验终值表
        private Hashtable npcIDTable = new Hashtable(); // NPC的ID表
        private Hashtable doodadIDTable = new Hashtable(); // Doodad的ID表
        private Hashtable publicResourceTable = new Hashtable(); // 公有资源的hash表
        private Hashtable coTaskTable = new Hashtable(); // 同期任务的hash表
        private Hashtable idTable = new Hashtable(); // 同一id的hash表
        private Hashtable coTaskTime = new Hashtable(); // 同期任务时间的hash表
        private DataTable dataTable; // 数据表
        private DataRow npcDefaultRow; // npc表默认行
        private DataRow doodadDefaultRow; // doodad表默认行
        private int totalNormalExp = 0; // 标准经验总和
        private int totalActualExp = 0; // 实际经验总和
        private Font boldFont; // 粗体
        private Font regularFont; // 常体
        private bool fontChangable = false; // 是否改变字体

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="node">开始计算的父结点</param>
        /// <param name="list">记录结点信息数据的链表</param>
        public ComputeForm(SqlConnection conn, TreeNode node, Hashtable table)
        {
            this.conn = conn;
            this.node = node;
            this.table = table;
            InitializeComponent();
            InitTable();
            InitDataTable();
            FillKindList();
            FillMapList();
            FillFillableColumnList();
            FillDefaultColumnList();
            FillSimpleModeList();
            FillSavableColumnList();
            boldFont = new Font(dataGridView1.DefaultCellStyle.Font, FontStyle.Bold);
            regularFont = new Font(dataGridView1.DefaultCellStyle.Font, FontStyle.Regular);
            CountNodeTotal(node); // 计算结点总数
            nodeCount = 0;
            pForm.SetMaxValue(nodeTotal);
            pForm.Visible = true;
            InitData(node);           
            buttonItem1_Click(null, null);
            startEdit = true;
        }

        /// <summary>
        /// 初始化表格
        /// </summary>
        private void InitTable()
        {
            // 定义列
            DataGridViewCheckBoxColumn checkColumn = new DataGridViewCheckBoxColumn();
            checkColumn.HeaderText = "自动计算";
            checkColumn.Name = "自动计算";

            DataGridViewTextBoxColumn questNameColumn = new DataGridViewTextBoxColumn();
            questNameColumn.HeaderText = "任务名称";
            questNameColumn.Name = "任务名称";

            DataGridViewTextBoxColumn questTypeColumn = new DataGridViewTextBoxColumn();
            questTypeColumn.HeaderText = "任务类型";
            questTypeColumn.Name = "任务类型";

            DataGridViewTextBoxColumn virtualFlowColumn = new DataGridViewTextBoxColumn();
            virtualFlowColumn.HeaderText = "实际流量";
            virtualFlowColumn.Name = "实际流量";

            DataGridViewTextBoxColumn additionalFlowColumn = new DataGridViewTextBoxColumn();
            additionalFlowColumn.HeaderText = "加算流量";
            additionalFlowColumn.Name = "加算流量";

            DataGridViewTextBoxColumn singleExpColumn = new DataGridViewTextBoxColumn();
            singleExpColumn.HeaderText = "单位时间经验(分)";
            singleExpColumn.Name = "单位时间经验(分)";

            DataGridViewTextBoxColumn singleMoneyColumn = new DataGridViewTextBoxColumn();
            singleMoneyColumn.HeaderText = "单位时间金钱(分)";
            singleMoneyColumn.Name = "单位时间金钱(分)";

            DataGridViewTextBoxColumn levelColumn = new DataGridViewTextBoxColumn();
            levelColumn.HeaderText = "等级";
            levelColumn.Name = "等级";

            DataGridViewTextBoxColumn requireResourceColumn = new DataGridViewTextBoxColumn();
            requireResourceColumn.HeaderText = "需求资源";
            requireResourceColumn.Name = "需求资源";

            DataGridViewTextBoxColumn requireNumberColumn = new DataGridViewTextBoxColumn();
            requireNumberColumn.HeaderText = "需求数量";
            requireNumberColumn.Name = "需求数量";

            DataGridViewTextBoxColumn getRateColumn = new DataGridViewTextBoxColumn();
            getRateColumn.HeaderText = "获得几率";
            getRateColumn.Name = "获得几率";

            DataGridViewTextBoxColumn resourceNumberColumn = new DataGridViewTextBoxColumn();
            resourceNumberColumn.HeaderText = "资源数量";
            resourceNumberColumn.Name = "资源数量";

            DataGridViewTextBoxColumn refreshTimeColumn = new DataGridViewTextBoxColumn();
            refreshTimeColumn.HeaderText = "刷新时间(秒)";
            refreshTimeColumn.Name = "刷新时间(秒)";

            DataGridViewTextBoxColumn operateTimeColumn = new DataGridViewTextBoxColumn();
            operateTimeColumn.HeaderText = "操作时间(秒)";
            operateTimeColumn.Name = "操作时间(秒)";

            DataGridViewTextBoxColumn runTimeColumn = new DataGridViewTextBoxColumn();
            runTimeColumn.HeaderText = "跑路时间(秒)";
            runTimeColumn.Name = "跑路时间(秒)";

            DataGridViewTextBoxColumn publicResourcePlusColumn = new DataGridViewTextBoxColumn();
            publicResourcePlusColumn.HeaderText = "公有资源加算";
            publicResourcePlusColumn.Name = "公有资源加算";

            DataGridViewTextBoxColumn coTaskPlusColumn = new DataGridViewTextBoxColumn();
            coTaskPlusColumn.HeaderText = "同期任务加算";
            coTaskPlusColumn.Name = "同期任务加算";

            DataGridViewTextBoxColumn flowPlusColumn = new DataGridViewTextBoxColumn();
            flowPlusColumn.HeaderText = "流量加算";
            flowPlusColumn.Name = "流量加算";

            DataGridViewTextBoxColumn expPlusColumn = new DataGridViewTextBoxColumn();
            expPlusColumn.HeaderText = "经验加算";
            expPlusColumn.Name = "经验加算";

            DataGridViewTextBoxColumn singleTaskTimeColumn = new DataGridViewTextBoxColumn();
            singleTaskTimeColumn.HeaderText = "单个任务时间(秒)";
            singleTaskTimeColumn.Name = "单个任务时间(秒)";

            DataGridViewTextBoxColumn otherCoTaskTimeColumn = new DataGridViewTextBoxColumn();
            otherCoTaskTimeColumn.HeaderText = "同期其他任务时间(秒)";
            otherCoTaskTimeColumn.Name = "同期其他任务时间(秒)";

            DataGridViewTextBoxColumn allCoTaskTimeColumn = new DataGridViewTextBoxColumn();
            allCoTaskTimeColumn.HeaderText = "同期所有任务时间(秒)";
            allCoTaskTimeColumn.Name = "同期所有任务时间(秒)";

            DataGridViewTextBoxColumn pressRateColumn = new DataGridViewTextBoxColumn();
            pressRateColumn.HeaderText = "压力比例";
            pressRateColumn.Name = "压力比例";

            DataGridViewTextBoxColumn decPressRateColumn = new DataGridViewTextBoxColumn();
            decPressRateColumn.HeaderText = "降压比例";
            decPressRateColumn.Name = "降压比例";

            DataGridViewTextBoxColumn plusColumn = new DataGridViewTextBoxColumn();
            plusColumn.HeaderText = "加算比例";
            plusColumn.Name = "加算比例";

            DataGridViewTextBoxColumn normalTimeColumn = new DataGridViewTextBoxColumn();
            normalTimeColumn.HeaderText = "标准时间(秒)";
            normalTimeColumn.Name = "标准时间(秒)";

            DataGridViewTextBoxColumn normalExpColumn = new DataGridViewTextBoxColumn();
            normalExpColumn.HeaderText = "标准经验";
            normalExpColumn.Name = "标准经验";

            DataGridViewTextBoxColumn actualExpColumn = new DataGridViewTextBoxColumn();
            actualExpColumn.HeaderText = "实际经验";
            actualExpColumn.Name = "实际经验";

            DataGridViewTextBoxColumn reallocExpColumn = new DataGridViewTextBoxColumn();
            reallocExpColumn.HeaderText = "重新分配经验";
            reallocExpColumn.Name = "重新分配经验";

            DataGridViewTextBoxColumn finalExpColumn = new DataGridViewTextBoxColumn();
            finalExpColumn.HeaderText = "经验终值";
            finalExpColumn.Name = "经验终值";

            DataGridViewTextBoxColumn reallocMoneyColumn = new DataGridViewTextBoxColumn();
            reallocMoneyColumn.HeaderText = "重新分配金钱";
            reallocMoneyColumn.Name = "重新分配金钱";

            DataGridViewTextBoxColumn finalMoneyColumn = new DataGridViewTextBoxColumn();
            finalMoneyColumn.HeaderText = "金钱终值";
            finalMoneyColumn.Name = "金钱终值";

            // 添加列
            dataGridView1.Columns.Add(checkColumn);
            dataGridView1.Columns.Add(questNameColumn);
            dataGridView1.Columns.Add(questTypeColumn);
            dataGridView1.Columns.Add(virtualFlowColumn);
            dataGridView1.Columns.Add(additionalFlowColumn);
            dataGridView1.Columns.Add(singleExpColumn);
            dataGridView1.Columns.Add(singleMoneyColumn);
            dataGridView1.Columns.Add(levelColumn);
            dataGridView1.Columns.Add(requireResourceColumn);
            dataGridView1.Columns.Add(requireNumberColumn);
            dataGridView1.Columns.Add(getRateColumn);
            dataGridView1.Columns.Add(resourceNumberColumn);
            dataGridView1.Columns.Add(refreshTimeColumn);
            dataGridView1.Columns.Add(operateTimeColumn);
            dataGridView1.Columns.Add(runTimeColumn);
            dataGridView1.Columns.Add(publicResourcePlusColumn);
            dataGridView1.Columns.Add(coTaskPlusColumn);
            dataGridView1.Columns.Add(flowPlusColumn);
            dataGridView1.Columns.Add(expPlusColumn);
            dataGridView1.Columns.Add(singleTaskTimeColumn);
            dataGridView1.Columns.Add(otherCoTaskTimeColumn);
            dataGridView1.Columns.Add(allCoTaskTimeColumn);
            dataGridView1.Columns.Add(pressRateColumn);
            dataGridView1.Columns.Add(decPressRateColumn);
            dataGridView1.Columns.Add(plusColumn);
            dataGridView1.Columns.Add(normalTimeColumn);
            dataGridView1.Columns.Add(normalExpColumn);
            dataGridView1.Columns.Add(actualExpColumn);
            dataGridView1.Columns.Add(reallocExpColumn);
            dataGridView1.Columns.Add(finalExpColumn);
            dataGridView1.Columns.Add(reallocMoneyColumn);
            dataGridView1.Columns.Add(finalMoneyColumn);

            // 定义列的只读属性
            dataGridView1.Columns["任务名称"].ReadOnly = true; // 任务名称只读
            dataGridView1.Columns["任务名称"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["任务类型"].ReadOnly = true; // 任务类型只读
            dataGridView1.Columns["任务类型"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["实际流量"].ReadOnly = true; // 实际流量只读
            dataGridView1.Columns["实际流量"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["加算流量"].ReadOnly = true; // 加算流量只读
            dataGridView1.Columns["加算流量"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["单位时间经验(分)"].ReadOnly = true; // 单位时间经验只读
            dataGridView1.Columns["单位时间经验(分)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["单位时间金钱(分)"].ReadOnly = true; // 单位时间金钱只读
            dataGridView1.Columns["单位时间金钱(分)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["需求资源"].ReadOnly = true; // 需求资源只读
            dataGridView1.Columns["需求资源"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["公有资源加算"].ReadOnly = true; // 公有资源加算只读
            dataGridView1.Columns["公有资源加算"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["同期任务加算"].ReadOnly = true; // 同期任务加算只读
            dataGridView1.Columns["同期任务加算"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["单个任务时间(秒)"].ReadOnly = true; // 单个任务时间只读
            dataGridView1.Columns["单个任务时间(秒)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["同期其他任务时间(秒)"].ReadOnly = true; // 同期其他任务时间只读
            dataGridView1.Columns["同期其他任务时间(秒)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["同期所有任务时间(秒)"].ReadOnly = true; // 同期所有任务时间只读
            dataGridView1.Columns["同期所有任务时间(秒)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["压力比例"].ReadOnly = true; // 压力比例只读
            dataGridView1.Columns["压力比例"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["降压比例"].ReadOnly = true; // 降压比例只读
            dataGridView1.Columns["降压比例"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["加算比例"].ReadOnly = true; // 加算比例只读  
            dataGridView1.Columns["加算比例"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["标准时间(秒)"].ReadOnly = true; // 标准时间只读
            dataGridView1.Columns["标准时间(秒)"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["标准经验"].ReadOnly = true; // 标准经验只读
            dataGridView1.Columns["标准经验"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["实际经验"].ReadOnly = true; // 实际经验只读
            dataGridView1.Columns["实际经验"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["重新分配经验"].ReadOnly = true; // 重新分配经验只读
            dataGridView1.Columns["重新分配经验"].DefaultCellStyle.BackColor = readOnlyColor;
            dataGridView1.Columns["重新分配金钱"].ReadOnly = true; // 重新分配金钱只读
            dataGridView1.Columns["重新分配金钱"].DefaultCellStyle.BackColor = readOnlyColor;

            // 定义列的颜色
            dataGridView1.Columns["等级"].DefaultCellStyle.BackColor = savableColor; // 等级可存
            dataGridView1.Columns["需求数量"].DefaultCellStyle.BackColor = savableColor; // 需求数量可存
            dataGridView1.Columns["获得几率"].DefaultCellStyle.BackColor = savableColor; // 获得几率可存
            dataGridView1.Columns["资源数量"].DefaultCellStyle.BackColor = savableColor; // 资源数量可存
            dataGridView1.Columns["刷新时间(秒)"].DefaultCellStyle.BackColor = savableColor; // 刷新时间可存
            dataGridView1.Columns["跑路时间(秒)"].DefaultCellStyle.BackColor = savableColor; // 跑路时间可存
            dataGridView1.Columns["流量加算"].DefaultCellStyle.BackColor = savableColor; // 流量加算可存
            dataGridView1.Columns["经验加算"].DefaultCellStyle.BackColor = savableColor; // 经验加算可存
            dataGridView1.Columns["经验终值"].DefaultCellStyle.BackColor = savableColor; // 经验终值可存
            dataGridView1.Columns["金钱终值"].DefaultCellStyle.BackColor = savableColor; // 金钱终值可存
        }

        /// <summary>
        /// 初始化数据表
        /// </summary>
        private void InitDataTable()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                string sql = "SELECT * FROM tbl_quests";
                dataTable = Helper.GetDataTable(sql, conn);
                npcDefaultRow = GetNpcDefaultRow(); // 获取npc表默认行
                doodadDefaultRow = GetDoodadDefaultRow(); // 获取doodad表默认行
            }
            catch(SqlException ex)
            {
                MessageBox.Show("初始化数据表时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 填充任务类型键值对链表
        /// </summary>
        private void FillKindList()
        {
            kindList.Add(new KeyValuePair<string, string>("QUEST_NULL", "前置任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_SCRIPT", "脚本任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_ITEM_MINE", "采集任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_ITEM_COLLECT", "收集任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_KILL", "杀怪任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_ITEM_DELIVER", "送信任务"));
            kindList.Add(new KeyValuePair<string, string>("QUEST_DIALOG", "对话任务"));
        }

        /// <summary>
        /// 填充物品地图键值对链表
        /// </summary>
        private void FillMapList()
        {
            mapList.Add(new KeyValuePair<string, string>("5", "Other"));
            mapList.Add(new KeyValuePair<string, string>("6", "item_Custom_Weapon"));
            mapList.Add(new KeyValuePair<string, string>("7", "item_Custom_Armor"));
            mapList.Add(new KeyValuePair<string, string>("8", "Custom_Trinket"));
        }

        /// <summary>
        /// 填充可自动填充列序号链表
        /// </summary>
        private void FillFillableColumnList()
        {
            fillableColumnList.Add(11); // 资源数量
            fillableColumnList.Add(14); // 跑路时间
            fillableColumnList.Add(17); // 流量加算
            fillableColumnList.Add(18); // 经验加算
        }

        /// <summary>
        /// 填充可以恢复默认值的列序号链表
        /// </summary>
        private void FillDefaultColumnList()
        {
            defaultColumnList.Add(12); // 刷新时间
            defaultColumnList.Add(13); // 操作时间
            defaultColumnList.Add(17); // 流量加算
            defaultColumnList.Add(18); // 经验加算
        }

        /// <summary>
        /// 填充可以在精简模式中隐藏的列序号链表
        /// </summary>
        private void FillSimpleModeList()
        {
            simpleModeList.Add(1);
            simpleModeList.Add(3);
            simpleModeList.Add(5);
            simpleModeList.Add(6);
            simpleModeList.Add(15);
            simpleModeList.Add(16);
            simpleModeList.Add(19);
            simpleModeList.Add(20);
            simpleModeList.Add(21);
            simpleModeList.Add(22);
            simpleModeList.Add(23);
            simpleModeList.Add(24);
            simpleModeList.Add(25);
            simpleModeList.Add(26);
            simpleModeList.Add(27);
            simpleModeList.Add(28);
            simpleModeList.Add(30);
        }

        /// <summary>
        /// 填充可以保存列的链表
        /// </summary>
        private void FillSavableColumnList()
        {
            savableColumnList.Add(7); // 任务等级
            savableColumnList.Add(9); // 需求数量
            savableColumnList.Add(10); // 获得几率
            savableColumnList.Add(12); // 刷新时间
            savableColumnList.Add(29); // 经验终值
            savableColumnList.Add(31); // 金钱终值
        }

        /// <summary>
        /// 计算树结点总数
        /// </summary>
        private void CountNodeTotal(TreeNode node)
        {
            nodeTotal++;
            foreach(TreeNode node1 in node.Nodes)
            {
                CountNodeTotal(node1);
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        /// <param name="node">计算的树结点</param>
        private void InitData(TreeNode node)
        {
            // 刷新进度条
            nodeCount++;
            pForm.SetValue(nodeCount);
            if(conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }
            AddData(node);
            if(node.Nodes.Count > 1) // 子结点存在同期任务
            {
                List<string> taskids = new List<string>();
                foreach(TreeNode node1 in node.Nodes)
                {
                    taskids.Add(((object[]) node1.Tag)[0].ToString());
                    coTaskTable[((object[]) node1.Tag)[0].ToString()] = taskids;
                }
                coTaskList.Add(taskids);
            }

            foreach (TreeNode node1 in node.Nodes)
            {                
                InitData(node1);
            }
            if (conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 获取npc表的默认行
        /// </summary>
        /// <returns>npc表的默认行</returns>
        private DataRow GetNpcDefaultRow()
        {
            DataRow row = null;

            string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "NpcTemplate");
            DataTable table = Helper.GetDataTable(sqlString, conn);
            if(table.Rows.Count > 0) // 找到默认行
            {
                row = table.Rows[0];
            }

            return row;
        }

        /// <summary>
        /// 获取doodad表的默认行
        /// </summary>
        /// <returns>doodad表的默认行</returns>
        private DataRow GetDoodadDefaultRow()
        {
            DataRow row = null;

            string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "tbl_doodad");
            DataTable table = Helper.GetDataTable(sqlString, conn);
            if (table.Rows.Count > 0) // 找到默认行
            {
                row = table.Rows[0];
            }

            return row;
        }

        /// <summary>
        /// 添加数据入列表中
        /// </summary>
        /// <param name="node"></param>
        private void AddData(TreeNode node)
        {
            try
            {
                int fatherid = 0;
                if(node.Parent != null)
                {
                    fatherid = int.Parse(((object[])node.Parent.Tag)[0].ToString());
                }
                int id = int.Parse(((object[])node.Tag)[0].ToString());
                if(id > 0 && (fatherid == -3 || fatherid == -4)) // 是镜像结点
                {
                    return;
                }

                DataRow dr = dataTable.Rows.Find(id);
                if(dr == null)
                {
                    return;
                }

                if(id > 0) // 普通结点
                {
                    List<string> list= table[node] as List<string>;
                    dataGridView1.Rows.Add(1); // 添加新行
                    virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                    additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                    // 添加任务ID及名称
                    dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                    dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                    // 添加任务类型
                    string kind = list[3];

                    string info = GetQuestPlusInfo(list[0]); // 查找添加附加信息
                    List<List<string>> dataList = null; // 附加信息组
                    bool plus = false;
                    if(info != null && info != "")
                    {
                        dataList = GetPlusData(info);
                        plus = true;
                    }

                    foreach(KeyValuePair<string, string> k in kindList)
                    {
                        if(kind == k.Key)
                        {
                            kind = k.Value;
                            break;
                        }
                    }
                    dataGridView1.Rows[index].Cells["任务类型"].Style.BackColor = readOnlyColor;
                    dataGridView1.Rows[index].Cells["任务类型"].Value = kind;  
                    dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                    // 添任务等级
                    dataGridView1.Rows[index].Cells["等级"].Style.BackColor = savableColor;
                    dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                    
                    switch(kind)
                    {
                        case "前置任务": // 前置任务
                            {
                                // 添加获得几率
                                dataGridView1.Rows[index].Cells["获得几率"].Value = "——";
                                dataGridView1.Rows[index].Cells["获得几率"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["获得几率"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加刷新时间
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加操作时间
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加需求资源
                                dataGridView1.Rows[index].Cells["需求资源"].Value = "——";
                                // 添加需求数量
                                dataGridView1.Rows[index].Cells["需求数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["需求数量"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["需求数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加单位时间经验
                                dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = "——";
                                dataGridView1.Rows[index].Cells["单位时间经验(分)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加单位时间金钱
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = "——";
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加资源数量
                                dataGridView1.Rows[index].Cells["资源数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["资源数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加跑路时间
                                dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["跑路时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加流量加算
                                dataGridView1.Rows[index].Cells["流量加算"].Value = "——";
                                dataGridView1.Rows[index].Cells["流量加算"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加经验加算
                                dataGridView1.Rows[index].Cells["经验加算"].Value = "——";
                                dataGridView1.Rows[index].Cells["经验加算"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加标准时间
                                dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["标准时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色

                                // 记录该ID的所有行号
                                List<int> indexList = new List<int>();
                                indexList.Add(index);
                                idTable[id.ToString()] = indexList;

                                break;
                            }
                        case "脚本任务": // 脚本任务
                            {
                                int requireResourceCount = 0; // 需求资源计数
                                for (int i = 1; i <= 8; i++) // 遍历需求资源
                                {
                                    if (dr["QuestValueStr" + i].ToString().Trim() != "" && dr["QuestValueStr" + i].ToString().Trim() != "0") 
                                    {
                                        if (requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                        }
                                        
                                        // 添加获得几率
                                        dataGridView1.Rows[index].Cells["获得几率"].Style.BackColor = Color.White;
                                        dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "100%";
                                        // 添加刷新时间
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = "1";
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Style.BackColor = Color.White;
                                        // 添加操作时间
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "2";
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "2";
                                        // 添加需求资源
                                        string resource = dr["QuestValueStr" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = resource;
                                        if(resourceList.Contains(resource)) // 是否是未记录的公共资源
                                        {
                                            if(!publicResourceList.Contains(resource))
                                            {
                                                publicResourceList.Add(resource);
                                            }
                                            List<int> rowIndexList = publicResourceTable[resource] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(resource);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[resource] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["QuestValue" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "QuestValue" + i;
                                        // 添加单位时间经验
                                        int level = int.Parse(list[2]);
                                        int exp = 0;
                                        if(level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if(level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if(level < taskData.NormalTimeArray3.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray3[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";
                                        
                                        if(plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if(idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }
                                        
                                        requireResourceCount++;
                                    }
                                }
                                break;
                            }
                        case "采集任务": // 采集任务
                            {
                                int requireResourceCount = 0; // 需求资源计数
                                for (int i = 1; i <= 4; i++)
                                {
                                    if (dr["DropDoodadTemplateID" + i].ToString().Trim() != "" &&  
                                        dr["DropDoodadTemplateID" + i].ToString().Trim() != "0") // 采集任务判断
                                    {
                                        if(requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                        }

                                        // 添加获得几率
                                        string tmpStr = dr["DropItemRate" + i].ToString();
                                        if (tmpStr == null || tmpStr == "")
                                        {
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        }
                                        else
                                        {
                                            float number = float.Parse(tmpStr);
                                            float rate = number * 100 / 1048576;
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = string.Format("{0:f1}%", rate);
                                        }
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "DropItemRate" + i;
                                        // 添加需求资源
                                        string itemid = dr["EndRequireItemIndex" + i].ToString();
                                        string mapid = dr["EndRequireItemType" + i].ToString();
                                        if(itemid == "" || itemid == "0" || mapid == "" || mapid == "0") // item相关内容无效
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务物品设置不匹配!", list[0]));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index); 
                                            index--;
                                            continue;
                                        }
                                        string itemName = GetItemName(itemid, mapid);
                                        if(itemName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的物品不存在!", list[0], itemid));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string doodadName = GetDoodadName(dr["DropDoodadTemplateID" + i].ToString());
                                        if (doodadName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的小物件不存在!", list[0], dr["DropDoodadTemplateID" + i].ToString()));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string resource = string.Format("{0} ({1})", itemName, doodadName);
                                        dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Green;
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = doodadName;
                                        doodadIDTable[index.ToString()] = dr["DropDoodadTemplateID" + i].ToString(); // 记录doodad的id
                                        if (resourceList.Contains(doodadName)) // 是否是未记录的公共资源
                                        {
                                            if (!publicResourceList.Contains(doodadName))
                                            {
                                                publicResourceList.Add(doodadName);
                                            }
                                            List<int> rowIndexList = publicResourceTable[doodadName] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(doodadName);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[doodadName] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["EndRequireItemAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "EndRequireItemAmount" + i;
                                        // 添加刷新时间
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = doodadReviveDelay.ToString();
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                        // 添加操作时间
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "5";
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "5";
                                        // 添加单位时间经验
                                        int level = int.Parse(list[2]);
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 变更单元格颜色
                                        int requireItemAmount = 0;
                                        if (dr["EndRequireItemAmount" + i] != null && dr["EndRequireItemAmount" + i].ToString() != "")
                                        {
                                            requireItemAmount = int.Parse(dr["EndRequireItemAmount" + i].ToString());
                                        }
                                        if (requireItemAmount > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray3.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray3[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }
                                    else if (dr["DropNpcTemplateID" + i].ToString().Trim() != "" && 
                                        dr["DropNpcTemplateID" + i].ToString().Trim() != "0") // 收集任务判断
                                    {
                                        if (requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                        }

                                        // 修改任务类型
                                        dataGridView1.Rows[index].Cells["任务类型"].Value = "收集任务";
                                        dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], "收集任务");
                                        // 添加获得几率
                                        string tmpStr = dr["DropItemRate" + i].ToString();
                                        if (tmpStr == null || tmpStr == "")
                                        {
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        }
                                        else
                                        {
                                            float number = float.Parse(tmpStr);
                                            float rate = number * 100 / 1048576;
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = string.Format("{0:f1}%", rate);;
                                        }
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "DropItemRate" + i;
                                        // 添加需求资源
                                        string itemid = dr["EndRequireItemIndex" + i].ToString();
                                        string mapid = dr["EndRequireItemType" + i].ToString();
                                        if (itemid == "" || itemid == "0" || mapid == "" || mapid == "0") // item相关内容无效
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务物品设置不匹配!", list[0]));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string itemName = GetItemName(itemid, mapid);
                                        if (itemName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的物品不存在!", list[0], itemid));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string npcName = GetNpcName(dr["DropNpcTemplateID" + i].ToString());
                                        if (npcName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的NPC不存在!", list[0], dr["DropNpcTemplateID" + i].ToString()));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string resource = string.Format("{0} ({1})", itemName, npcName);
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = npcName;
                                        npcIDTable[index.ToString()] = dr["DropNpcTemplateID" + i].ToString(); // 记录NPC的ID
                                        if (resourceList.Contains(npcName)) // 是否是未记录的公共资源
                                        {
                                            if (!publicResourceList.Contains(npcName))
                                            {
                                                publicResourceList.Add(npcName);
                                            }
                                            List<int> rowIndexList = publicResourceTable[npcName] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(npcName);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[npcName] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["EndRequireItemAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "EndRequireItemAmount" + i;
                                        // 添加操作时间
                                        int level = int.Parse(list[2]);
                                        int operateTime = 0;
                                        if(level < taskData.TimeArray.Length)
                                        {
                                            operateTime = taskData.TimeArray[level];
                                        }
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = operateTime.ToString();
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = operateTime.ToString();
                                        // 添加单位时间经验                            
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray4.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray4[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        // 变更单元格颜色
                                        switch (npcKind)
                                        {
                                            case 0: // 普通怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Black;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                                    break;
                                                }
                                            case 1: // 主动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Red;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "120";
                                                    break;
                                                }
                                            case 2: // 被动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Orange;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "90";
                                                    break;
                                                }
                                        }
                                        int requireItemAmount = 0;
                                        if (dr["EndRequireItemAmount" + i] != null && dr["EndRequireItemAmount" + i].ToString() != "")
                                        {
                                            requireItemAmount = int.Parse(dr["EndRequireItemAmount" + i].ToString());
                                        }
                                        if (requireItemAmount > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }
                                        npcKind = 0;

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }
                                }
                                break;
                            }
                        case "收集任务": // 收集任务
                            {
                                int requireResourceCount = 0; // 需求资源计数
                                for (int i = 1; i <= 4; i++)
                                {
                                    if (dr["DropNpcTemplateID" + i].ToString().Trim() != "" 
                                        && dr["DropNpcTemplateID" + i].ToString().Trim() != "0") // 收集任务判断
                                    {
                                        if (requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                                                                 
                                        }
                                        
                                        // 添加获得几率
                                        string tmpStr = dr["DropItemRate" + i].ToString();
                                        if(tmpStr == null || tmpStr == "")
                                        {
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        }
                                        else
                                        {
                                            float number = float.Parse(tmpStr);
                                            float rate = number * 100 / 1048576;
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = string.Format("{0:f1}%", rate);;
                                        }
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "DropItemRate" + i;
                                        // 添加需求资源
                                        string itemid = dr["EndRequireItemIndex" + i].ToString();
                                        string mapid = dr["EndRequireItemType" + i].ToString();
                                        if (itemid == "" || itemid == "0" || mapid == "" || mapid == "0") // item相关内容无效
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务物品设置不匹配!", list[0]));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index); 
                                            index--;
                                            continue;
                                        }
                                        string itemName = GetItemName(itemid, mapid);
                                        if (itemName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的物品不存在!", list[0], itemid));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string npcName = GetNpcName(dr["DropNpcTemplateID" + i].ToString());
                                        if (npcName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的NPC不存在!", list[0], dr["DropNpcTemplateID" + i].ToString()));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string resource = string.Format("{0} ({1})", itemName, npcName);
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = npcName;
                                        npcIDTable[index.ToString()] = dr["DropNpcTemplateID" + i].ToString(); // 记录NPC的ID
                                        if (resourceList.Contains(npcName)) // 是否是未记录的公共资源
                                        {
                                            if (!publicResourceList.Contains(npcName))
                                            {
                                                publicResourceList.Add(npcName);
                                            }
                                            List<int> rowIndexList = publicResourceTable[npcName] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(npcName);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[npcName] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["EndRequireItemAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "EndRequireItemAmount" + i;
                                        // 添加操作时间
                                        int level = int.Parse(list[2]);
                                        int operateTime = 0;
                                        if(level < taskData.TimeArray.Length)
                                        {
                                            operateTime = taskData.TimeArray[level];
                                        }
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = operateTime.ToString();
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = operateTime.ToString();
                                        // 添加单位时间经验                                       
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] *1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray4.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray4[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        // 变更单元格颜色
                                        switch (npcKind)
                                        {
                                            case 0: // 普通怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Black;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                                    break;
                                                }
                                            case 1: // 主动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Red;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "120";
                                                    break;
                                                }
                                            case 2: // 被动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Orange;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "90";
                                                    break;
                                                }
                                        }
                                        int requireItemAmount = 0;
                                        if (dr["EndRequireItemAmount" + i] != null && dr["EndRequireItemAmount" + i].ToString() != "")
                                        {
                                            requireItemAmount = int.Parse(dr["EndRequireItemAmount" + i].ToString());
                                        }
                                        if (requireItemAmount > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }
                                        npcKind = 0;

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }
                                    else if (dr["DropDoodadTemplateID" + i].ToString().Trim() != "" &&
                                        dr["DropDoodadTemplateID" + i].ToString().Trim() != "0") // 采集任务判断
                                    {
                                        if (requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                        }

                                        // 修改任务类型
                                        dataGridView1.Rows[index].Cells["任务类型"].Value = "采集任务";
                                        dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], "采集任务");
                                        // 添加获得几率
                                        string tmpStr = dr["DropItemRate" + i].ToString();
                                        if (tmpStr == null || tmpStr == "")
                                        {
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        }
                                        else
                                        {
                                            float number = float.Parse(tmpStr);
                                            float rate = number * 100 / 1048576;
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = string.Format("{0:f1}%", rate);;
                                        }
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "DropItemRate" + i;
                                        // 添加需求资源
                                        string itemid = dr["EndRequireItemIndex" + i].ToString();
                                        string mapid = dr["EndRequireItemType" + i].ToString();
                                        if (itemid == "" || itemid == "0" || mapid == "" || mapid == "0") // item相关内容无效
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务物品设置不匹配!", list[0]));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string itemName = GetItemName(itemid, mapid);
                                        if (itemName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的物品不存在!", list[0], itemid));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string doodadName = GetDoodadName(dr["DropDoodadTemplateID" + i].ToString());
                                        if (doodadName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的小物件不存在!", list[0], dr["DropDoodadTemplateID" + i].ToString()));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string resource = string.Format("{0} ({1})", itemName, doodadName);
                                        dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Green;
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = doodadName;
                                        doodadIDTable[index.ToString()] = dr["DropDoodadTemplateID" + i].ToString(); // 记录doodad的id
                                        if (resourceList.Contains(doodadName)) // 是否是未记录的公共资源
                                        {
                                            if (!publicResourceList.Contains(doodadName))
                                            {
                                                publicResourceList.Add(doodadName);
                                            }
                                            List<int> rowIndexList = publicResourceTable[doodadName] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(doodadName);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[doodadName] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["EndRequireItemAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "EndRequireItemAmount" + i;
                                        // 添加刷新时间
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = doodadReviveDelay.ToString();
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                        // 添加操作时间
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "5";
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "5";
                                        // 添加单位时间经验
                                        int level = int.Parse(list[2]);
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.15);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray3.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray3[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        // 变更单元格颜色
                                        int requireItemAmount = 0;
                                        if (dr["EndRequireItemAmount" + i] != null && dr["EndRequireItemAmount" + i].ToString() != "")
                                        {
                                            requireItemAmount = int.Parse(dr["EndRequireItemAmount" + i].ToString());
                                        }
                                        if (requireItemAmount > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }
                                    else if (dr["EndRequireItemIndex" + i].ToString().Trim() != "" && 
                                        dr["EndRequireItemIndex" + i].ToString().Trim() != "0") // 收集任务判断
                                    {
                                        if (requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];                                        
                                        }
                                        
                                        // 添加获得几率
                                        string tmpStr = dr["DropItemRate" + i].ToString();
                                        if (tmpStr == null || tmpStr == "")
                                        {
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        }
                                        else
                                        {
                                            float number = float.Parse(tmpStr);
                                            float rate = number * 100 / 1048576;
                                            dataGridView1.Rows[index].Cells["获得几率"].Value = string.Format("{0:f1}%", rate);;
                                        }
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "DropItemRate" + i;
                                        // 添加需求资源
                                        string itemid = dr["EndRequireItemIndex" + i].ToString();
                                        string mapid = dr["EndRequireItemType" + i].ToString();
                                        if (itemid == "" || itemid == "0" || mapid == "" || mapid == "0") // item相关内容无效
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务物品设置不匹配!", list[0]));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index); 
                                            index--;
                                            continue;
                                        }
                                        string itemName = GetItemName(itemid, mapid);
                                        if (itemName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的物品不存在!", list[0], itemid));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        string resource = string.Format("{0} (other)", itemName);
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = resource;
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["EndRequireItemAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "EndRequireItemAmount" + i;
                                        // 添加操作时间
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "2";
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "2";
                                        // 添加单位时间经验
                                        int level = int.Parse(list[2]);
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray4.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray4[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加刷新时间
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = "1";
                                        dataGridView1.Rows[index].Cells["刷新时间(秒)"].Style.BackColor = Color.White;
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        // 变更单元格颜色
                                        int requireItemAmount = 0;
                                        if (dr["EndRequireItemAmount" + i] != null && dr["EndRequireItemAmount" + i].ToString() != "")
                                        {
                                            requireItemAmount = int.Parse(dr["EndRequireItemAmount" + i].ToString());
                                        }
                                        if (requireItemAmount > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }                                   
                                }

                                break;
                            }
                        case "杀怪任务": // 杀怪任务
                            {
                                int requireResourceCount = 0; // 需求资源计数
                                for (int i = 1; i <= 4; i++)
                                {
                                    if(dr["KillNpcTemplateID" + i].ToString() != "" && dr["KillNpcTemplateID" + i].ToString() != "0")
                                    {
                                        if(requireResourceCount > 0)
                                        {
                                            dataGridView1.Rows.Add(1); // 添加新行
                                            virtualFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            additionalFlowSectionList.Add(new float[] { 0.00f, 0.00f });
                                            index++;
                                            // 添加任务ID及名称
                                            dataGridView1.Rows[index].Cells["任务名称"].Value = string.Format("({0}) {1}", list[0], list[1]);
                                            dataGridView1.Rows[index].Cells["任务名称"].Tag = list[0];
                                            // 添加任务类型
                                            dataGridView1.Rows[index].Cells["任务类型"].Value = kind;
                                            dataGridView1.Rows[index].HeaderCell.Value = string.Format("({0}) {1}, {2}", list[0], list[1], kind);
                                            // 添任务等级
                                            dataGridView1.Rows[index].Cells["等级"].Value = list[2];
                                        }
                                       
                                        // 添加获得几率
                                        dataGridView1.Rows[index].Cells["获得几率"].Style.BackColor = readOnlyColor;
                                        dataGridView1.Rows[index].Cells["获得几率"].Value = "100%";
                                        dataGridView1.Rows[index].Cells["获得几率"].Tag = "100%";
                                        dataGridView1.Rows[index].Cells["获得几率"].ReadOnly = true; // 设置为只读
                                        // 添加需求资源
                                        string npcName = GetNpcName(dr["KillNpcTemplateID" + i].ToString());
                                        if (npcName == null)
                                        {
                                            MessageBox.Show(string.Format("第{0}号任务的ID为{1}的NPC不存在!", list[0], dr["KillNpcTemplateID" + i].ToString()));
                                            dataGridView1.Rows.RemoveAt(index); // 删减行
                                            virtualFlowSectionList.RemoveAt(index);
                                            additionalFlowSectionList.RemoveAt(index);
                                            index--;
                                            continue;
                                        }
                                        dataGridView1.Rows[index].Cells["需求资源"].Value = npcName;
                                        dataGridView1.Rows[index].Cells["需求资源"].Tag = npcName;
                                        npcIDTable[index.ToString()] = dr["KillNpcTemplateID" + i].ToString(); // 记录NPC的ID
                                        if (resourceList.Contains(npcName)) // 是否是未记录的公共资源
                                        {
                                            if (!publicResourceList.Contains(npcName))
                                            {
                                                publicResourceList.Add(npcName);
                                            }
                                            List<int> rowIndexList = publicResourceTable[npcName] as List<int>;
                                            rowIndexList.Add(index);
                                        }
                                        else
                                        {
                                            resourceList.Add(npcName);
                                            List<int> rowIndexList = new List<int>();
                                            rowIndexList.Add(index);
                                            publicResourceTable[npcName] = rowIndexList;
                                        }
                                        // 添加需求数量
                                        dataGridView1.Rows[index].Cells["需求数量"].Value = dr["KillNpcAmount" + i].ToString();
                                        dataGridView1.Rows[index].Cells["需求数量"].Tag = "KillNpcAmount" + i;
                                        // 添加操作时间
                                        int level = int.Parse(list[2]);
                                        int operateTime = 0;
                                        if(level < taskData.TimeArray.Length)
                                        {
                                            operateTime = taskData.TimeArray[level];
                                        }
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = operateTime.ToString();
                                        dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = operateTime.ToString();
                                        // 添加单位时间经验
                                        int exp = 0;
                                        if (level < taskData.ExpArray.Length)
                                        {
                                            exp = (int)(taskData.ExpArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                        // 添加单位时间金钱
                                        int money = 0;
                                        if (level < taskData.MoneyArray.Length)
                                        {
                                            money = (int)(taskData.MoneyArray[level] * 1.2);
                                        }
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                        dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                        // 添加标准时间
                                        int normalTime = 0;
                                        if (level < taskData.NormalTimeArray4.Length)
                                        {
                                            normalTime = taskData.NormalTimeArray4[level];
                                        }
                                        dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                        // 添加流量加算
                                        dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                        // 添加经验加算
                                        dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                        dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";

                                        // 变更单元格颜色
                                        switch (npcKind)
                                        {
                                            case 0: // 普通怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Black;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "1";
                                                    break;
                                                }
                                            case 1: // 主动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Red;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "120";
                                                    break;
                                                }
                                            case 2: // 被动怪
                                                {
                                                    dataGridView1.Rows[index].Cells["需求资源"].Style.ForeColor = Color.Orange;
                                                    // 添加刷新时间
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = npcReviveTime.ToString();
                                                    dataGridView1.Rows[index].Cells["刷新时间(秒)"].Tag = "90";
                                                    break;
                                                }
                                        }
                                        if (int.Parse(dr["KillNpcAmount" + i].ToString()) > 10)
                                        {
                                            dataGridView1.Rows[index].Cells["需求数量"].Style.ForeColor = Color.Red;
                                        }
                                        npcKind = 0;

                                        if (plus) // 添加附加信息
                                        {
                                            virtualFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][0]);
                                            virtualFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][1]);
                                            additionalFlowSectionList[index][0] = float.Parse(dataList[requireResourceCount][2]);
                                            additionalFlowSectionList[index][1] = float.Parse(dataList[requireResourceCount][3]);
                                            // 添加实际流量区间
                                            dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                                virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                            // 添加加算流量区间
                                            dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                                additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                            // 添加资源数量
                                            dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[requireResourceCount][4];
                                            // 添加跑路时间
                                            dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[requireResourceCount][5];
                                            // 添加流量加算
                                            dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[requireResourceCount][6];
                                            // 添加经验加算
                                            dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[requireResourceCount][7];
                                        }

                                        // 记录该ID的所有行号
                                        if (idTable[id.ToString()] == null)
                                        {
                                            List<int> indexList = new List<int>();
                                            indexList.Add(index);
                                            idTable[id.ToString()] = indexList;
                                        }
                                        else
                                        {
                                            List<int> indexList = idTable[id.ToString()] as List<int>;
                                            indexList.Add(index);
                                        }

                                        requireResourceCount++;
                                    }
                                }
                                break;
                            }
                        case "送信任务": // 送信任务
                            {
                                // 添加获得几率
                                dataGridView1.Rows[index].Cells["获得几率"].Value = "——";
                                dataGridView1.Rows[index].Cells["获得几率"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["获得几率"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加刷新时间
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加操作时间
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "2";
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "2";
                                // 添加需求资源
                                dataGridView1.Rows[index].Cells["需求资源"].Value = "——";
                                // 添加需求数量
                                dataGridView1.Rows[index].Cells["需求数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["需求数量"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["需求数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加单位时间经验
                                int level = int.Parse(list[2]);
                                int exp = 0;
                                if (level < taskData.ExpArray.Length)
                                {
                                    exp = (int)(taskData.ExpArray[level] * 1.05);
                                }
                                dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                // 添加单位时间金钱
                                int money = 0;
                                if (level < taskData.MoneyArray.Length)
                                {
                                    money = (int)(taskData.MoneyArray[level] * 1.05);
                                }
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                // 添加标准时间
                                int normalTime = 0;
                                if (level < taskData.NormalTimeArray2.Length)
                                {
                                    normalTime = taskData.NormalTimeArray2[level];
                                }
                                dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                // 添加流量加算
                                dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                // 添加经验加算
                                dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";
                                // 添加资源数量
                                dataGridView1.Rows[index].Cells["资源数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["资源数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色

                                if (plus) // 添加附加信息
                                {
                                    virtualFlowSectionList[index][0] = float.Parse(dataList[0][0]);
                                    virtualFlowSectionList[index][1] = float.Parse(dataList[0][1]);
                                    additionalFlowSectionList[index][0] = float.Parse(dataList[0][2]);
                                    additionalFlowSectionList[index][1] = float.Parse(dataList[0][3]);
                                    // 添加实际流量区间
                                    dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                        virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                    // 添加加算流量区间
                                    dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                        additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                    // 添加资源数量
                                    dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[0][4];
                                    // 添加跑路时间
                                    dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[0][5];
                                    // 添加流量加算
                                    dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[0][6];
                                    // 添加经验加算
                                    dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[0][7];
                                }

                                // 记录该ID的所有行号
                                List<int> indexList = new List<int>();
                                indexList.Add(index);
                                idTable[id.ToString()] = indexList;

                                break;
                            }
                        case "对话任务": // 对话任务
                            {
                                // 添加获得几率
                                dataGridView1.Rows[index].Cells["获得几率"].Value = "——";
                                dataGridView1.Rows[index].Cells["获得几率"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["获得几率"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加刷新时间
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Value = "——";
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["刷新时间(秒)"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加操作时间
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Value = "2";
                                dataGridView1.Rows[index].Cells["操作时间(秒)"].Tag = "2";
                                // 添加需求资源
                                dataGridView1.Rows[index].Cells["需求资源"].Value = "——";
                                // 添加需求数量
                                dataGridView1.Rows[index].Cells["需求数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["需求数量"].ReadOnly = true; // 设置为只读
                                dataGridView1.Rows[index].Cells["需求数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色
                                // 添加单位时间经验
                                int level = int.Parse(list[2]);
                                int exp = 0;
                                if (level < taskData.ExpArray.Length)
                                {
                                    exp = taskData.ExpArray[level];
                                }
                                dataGridView1.Rows[index].Cells["单位时间经验(分)"].Value = exp.ToString();
                                // 添加单位时间金钱
                                int money = 0;
                                if (level < taskData.MoneyArray.Length)
                                {
                                    money = taskData.MoneyArray[level];
                                }
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Value = money.ToString();
                                dataGridView1.Rows[index].Cells["单位时间金钱(分)"].Tag = money.ToString();
                                // 添加标准时间
                                int normalTime = 0;
                                if (level < taskData.NormalTimeArray1.Length)
                                {
                                    normalTime = taskData.NormalTimeArray1[level];
                                }
                                dataGridView1.Rows[index].Cells["标准时间(秒)"].Value = normalTime.ToString();
                                // 添加流量加算
                                dataGridView1.Rows[index].Cells["流量加算"].Value = "1";
                                dataGridView1.Rows[index].Cells["流量加算"].Tag = "1";
                                // 添加经验加算
                                dataGridView1.Rows[index].Cells["经验加算"].Value = "1";
                                dataGridView1.Rows[index].Cells["经验加算"].Tag = "1";
                                // 添加资源数量
                                dataGridView1.Rows[index].Cells["资源数量"].Value = "——";
                                dataGridView1.Rows[index].Cells["资源数量"].Style.BackColor = readOnlyColor; // 设置单元格颜色

                                if (plus) // 添加附加信息
                                {
                                    virtualFlowSectionList[index][0] = float.Parse(dataList[0][0]);
                                    virtualFlowSectionList[index][1] = float.Parse(dataList[0][1]);
                                    additionalFlowSectionList[index][0] = float.Parse(dataList[0][2]);
                                    additionalFlowSectionList[index][1] = float.Parse(dataList[0][3]);
                                    // 添加实际流量区间
                                    dataGridView1.Rows[index].Cells["实际流量"].Value = string.Format("[{0}, {1}]",
                                        virtualFlowSectionList[index][1].ToString(), virtualFlowSectionList[index][0].ToString());
                                    // 添加加算流量区间
                                    dataGridView1.Rows[index].Cells["加算流量"].Value = string.Format("[{0}, {1}]",
                                        additionalFlowSectionList[index][1].ToString(), additionalFlowSectionList[index][0].ToString());
                                    // 添加资源数量
                                    dataGridView1.Rows[index].Cells["资源数量"].Value = dataList[0][4];
                                    // 添加跑路时间
                                    dataGridView1.Rows[index].Cells["跑路时间(秒)"].Value = dataList[0][5];
                                    // 添加流量加算
                                    dataGridView1.Rows[index].Cells["流量加算"].Value = dataList[0][6];
                                    // 添加经验加算
                                    dataGridView1.Rows[index].Cells["经验加算"].Value = dataList[0][7];
                                }

                                // 记录该ID的所有行号
                                List<int> indexList = new List<int>();
                                indexList.Add(index);
                                idTable[id.ToString()] = indexList;

                                break;
                            }
                    }
                    index++;
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在流量计算第{0}号任务添加数据时产生异常: " + ex.ToString(), ((object[])node.Tag)[0].ToString()));
            }
        }

        /// <summary>
        /// 获取NPC的名称
        /// </summary>
        /// <param name="id">NPC的ID</param>
        /// <returns>NPC的名称</returns>
        private string GetNpcName(string id)
        {
            string sql = string.Format("SELECT Name, AIParamTemplateID, ReviveTime FROM NpcTemplate WHERE ID = {0}", id);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string name = "未找到该NPC";
            int aiid = 0;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if (!reader.HasRows) // 查询结果为空
                {
                    return null;
                }
                else
                {
                    if(reader[0] != null)
                    {
                        name = reader[0].ToString().Trim();
                    }
                    if(reader[1] != null)
                    {   
                        if((reader[1] is DBNull) && (npcDefaultRow != null)) // 如果是空值的话替换为默认行的内容
                        {
                            aiid = int.Parse(npcDefaultRow["AIParamTemplateID"].ToString());
                        }
                        else
                        {
                            aiid = int.Parse(reader[1].ToString().Trim());
                        }
                    }
                    if(reader[2] != null)
                    {
                        if ((reader[2] is DBNull) && (npcDefaultRow != null)) // 如果是空值的话替换为默认行的内容
                        {
                            npcReviveTime = int.Parse(npcDefaultRow["ReviveTime"].ToString());
                        }
                        else
                        {
                            npcReviveTime = int.Parse(reader[2].ToString().Trim());
                        }
                    }
                    
                    if(aiid >= 11 && aiid <= 16) // 主动怪
                    {
                        npcKind = 1;
                    }
                    else if(aiid >= 21 && aiid <= 26) // 被动怪
                    {
                        npcKind = 2;
                    }
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("获取NPC的名称时产生异常: " + ex.ToString());
            }
            finally
            {
                if(reader != null) // 关闭reader
                {
                    reader.Close();
                }
            }
            return name;
        }

        /// <summary>
        /// 获取Doodad的名称
        /// </summary>
        /// <param name="id">Doodad的ID</param>
        /// <returns>Doodad的名称</returns>
        private string GetDoodadName(string id)
        {
            string sql = string.Format("SELECT Name, ReviveDelay FROM tbl_doodad WHERE ID = {0}", id);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string name = "未找到该小物件";

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if (!reader.HasRows) // 查询结果为空
                {
                    reader.Close();
                    return null;
                }
                else
                {
                    name = reader[0].ToString().Trim();

                    if(reader[1] != null) // 没有定义刷新时间
                    {
                        if ((reader[1] is DBNull) && (npcDefaultRow != null)) // 如果是空值的话替换为默认行的内容
                        {
                            doodadReviveDelay = int.Parse(doodadDefaultRow["ReviveDelay"].ToString());
                        }
                        else
                        {
                            doodadReviveDelay = int.Parse(reader[1].ToString().Trim());
                        }
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("获取小物件的名称时产生异常: " + ex.ToString());
            }
            finally
            {
                reader.Close();
            }
            return name;
        }

        /// <summary>
        /// 获取Item的名称
        /// </summary>
        /// <param name="id">Item的ID</param>
        /// <param name="mapid">Map的ID</param>
        /// <returns>Item的名称</returns>
        private string GetItemName(string id, string mapid)
        {
            string map = "0";
            foreach(KeyValuePair<string, string> k in mapList) // 将map的ID转换为Name
            {
                if(mapid == k.Key)
                {
                    map = k.Value;
                }
            }

            string sql = string.Format("SELECT Name FROM {0} WHERE ID = {1}", map, id);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string name = "未找到该物品";
            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if (!reader.HasRows) // 查询结果为空
                {
                    reader.Close();
                    return null;
                }
                else
                {
                    name = reader[0].ToString().Trim();
                }           
            }
            catch (SqlException ex)
            {
                MessageBox.Show("获取物品的名称时产生异常: " + ex.ToString());
            }
            finally
            {
                reader.Close();
            }
            return name;
        }

        /// <summary>
        /// 获取任务附加信息
        /// </summary>
        /// <param name="id">任务ID号</param>
        /// <returns>任务附加信息字符串</returns>
        private string GetQuestPlusInfo(string id)
        {
            string sql = string.Format("SELECT PlusInfo FROM {0} WHERE QuestID = {1}", plusTable, id);
            string info = null;
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                if(!reader.HasRows) // 该行不存在
                {
                    reader.Close();
                    cmd = conn.CreateCommand();
                    cmd.CommandText = string.Format("INSERT INTO {0} ({1}) VALUES ({2})", plusTable, "QuestID", id);
                    cmd.ExecuteNonQuery();
                    return null;
                }
                else
                {
                    info = reader[0].ToString().Trim();
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("获取任务附加信息时产生Sql异常: " + ex.ToString());
            }
            finally
            {
                if(!reader.IsClosed) // reader是否已关闭
                {
                    reader.Close();
                }
            }
            return info;
        }

        /// <summary>
        /// 设置任务附加信息
        /// </summary>
        /// <param name="id">任务ID号</param>
        /// <param name="info">任务附加信息</param>
        private void SetQuestPlusInfo(string id, string info)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", plusTable, "PlusInfo", info, id);
                cmd.ExecuteNonQuery();
            }
            catch(SqlException ex)
            {
                MessageBox.Show("设置任务附加信息时产生Sql异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 将附加信息转换为附加信息组
        /// </summary>
        /// <param name="info">附加信息</param>
        /// <returns>附加信息组</returns>
        private List<List<string>> GetPlusData(string info)
        {
            List<List<string>> result = new List<List<string>>();
            string[] data1 = info.Split(';');
            
            foreach(string s1 in data1)
            {
                List<string> list1 = new List<string>();
                string[] data2 = s1.Split(',');
                foreach(string s2 in data2)
                {
                    list1.Add(s2);
                }
                result.Add(list1);
            }
            return result;
        }

        /// <summary>
        ///  更新任务的等级信息
        /// </summary>
        /// <param name="id">任务的ID</param>
        /// <param name="level">任务的等级</param>
        private void UpdateMapLevel(string id, string level)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "tbl_quests", "QuestLevel", level, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的等级信息时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新任务的需求数量
        /// </summary>
        /// <param name="id">任务的ID</param>
        /// <param name="attribute">物品属性</param>
        /// <param name="number">需求数量</param>
        private void UpdateRequestNumber(string id, string attribute,string number)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "tbl_quests", attribute, number, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的需求数量时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新任务的获得几率
        /// </summary>
        /// <param name="id">任务的ID</param>
        /// <param name="attribute">几率属性</param>
        /// <param name="rate">获得几率</param>
        private void UpdateGetRate(string id, string attribute, string rate)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "tbl_quests", attribute, rate, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的获得几率时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新任务的奖励经验
        /// </summary>
        /// <param name="id">任务的ID</param>
        /// <param name="value">经验数值</param>
        private void UpdatePresentExp(string id, string value)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "tbl_quests", "PresentExp", value, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的奖励经验时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新任务的奖励金钱
        /// </summary>
        /// <param name="id">任务的ID</param>
        /// <param name="value">金钱数值</param>
        private void UpdatePresentMoney(string id, string value)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE QuestID = {3}", "tbl_quests", "PresentMoney", value, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的奖励金钱时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新NPC刷新时间
        /// </summary>
        /// <param name="id">NPC的ID</param>
        /// <param name="value">刷新时间</param>
        private void UpdateNPCRefreshTime(string id, string value)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE ID = {3}", "NpcTemplate", "ReviveTime", value, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的NPC刷新时间时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 更新Doodad刷新时间
        /// </summary>
        /// <param name="id">Doodad的ID</param>
        /// <param name="value">刷新时间</param>
        private void UpdateDoodadRefreshTime(string id, string value)
        {
            try
            {
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE ID = {3}", "tbl_doodad", "ReviveDelay", value, id);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("更新第{0}号任务的Doodad刷新时间时产生Sql异常: " + ex.ToString(), id));
            }
        }

        /// <summary>
        /// 计算实际流量
        /// </summary>
        /// <param name="i">数据所在行</param>
        private void ComputeVirtualFlow(int i)
        {
            float virtualFlow = 0.00f; // 实际流量
            string rC = dataGridView1.Rows[i].Cells["资源数量"].EditedFormattedValue.ToString().Trim(); // 资源数量
            string rT = dataGridView1.Rows[i].Cells["刷新时间(秒)"].EditedFormattedValue.ToString().Trim(); // 刷新时间
            string oT = dataGridView1.Rows[i].Cells["操作时间(秒)"].EditedFormattedValue.ToString().Trim(); // 操作时间
            string gR = dataGridView1.Rows[i].Cells["获得几率"].EditedFormattedValue.ToString().TrimEnd('%'); // 获得几率
            string rN = dataGridView1.Rows[i].Cells["需求数量"].EditedFormattedValue.ToString(); // 需求数量
            if (rC == "" || rC == "——") // 未设置需求数量
            {
                MessageBox.Show(string.Format("未设置第{0}行资源数量!", i.ToString()));
                return;
            }
            if (rT == "" || rT == "——") // 未设置刷新时间
            {
                MessageBox.Show(string.Format("未设置第{0}行刷新时间!", i.ToString()));
                return;
            }
            if (oT == "" || oT == "——") // 未设置操作时间
            {
                MessageBox.Show(string.Format("未设置第{0}行操作时间!", i.ToString()));
                return;
            }
            if (gR == "" || gR == "——") // 未设置获得几率
            {
                MessageBox.Show(string.Format("未设置第{0}行获得几率!", i.ToString()));
                return;
            }
            if(rN == "" || rN == "——") // 未设置需求数量
            {
                MessageBox.Show(string.Format("未设置第{0}行需求数量!", i.ToString()));
                return;
            }
            try
            {
                int resourceCount = int.Parse(rC); // 资源数量
                int refreshTime = int.Parse(rT); // 刷新时间
                int operateTime = int.Parse(oT); // 操作时间
                float getRate = float.Parse(gR); // 获得几率
                int requireNumber = int.Parse(rN); // 需求数量
                //实际流量 = (((资源数量/(刷新时间+操作时间))*60)*获得几率)/需求数量
                virtualFlow = ((((float)resourceCount / (refreshTime + operateTime)) * 60) * getRate) / requireNumber / 100; // 计算实际流量
                float downNumber = virtualFlowSectionList[i][1]; // 数值下限
                float upNumber = virtualFlowSectionList[i][0]; // 数值上限
                if(downNumber == 0.00 && upNumber == 0.00) // 未设置上下限
                {
                    MessageBox.Show(string.Format("未设置第{0}行实际流量数据的上下限!", i.ToString()));
                    return;
                }
                if(virtualFlow < downNumber)
                {
                    dataGridView1.Rows[i].Cells["实际流量"].Style.ForeColor = Color.Red;
                }
                else if(virtualFlow > upNumber)
                {
                    dataGridView1.Rows[i].Cells["实际流量"].Style.ForeColor = Color.Blue;
                    if(virtualFlow > upNumber * 2)
                    {
                        dataGridView1.Rows[i].Cells["实际流量"].Value = "∞";
                        return;
                    }
                }
                else
                {
                    dataGridView1.Rows[i].Cells["实际流量"].Style.ForeColor = Color.Green;
                }
                dataGridView1.Rows[i].Cells["实际流量"].Value = string.Format("{0:f1}", virtualFlow);
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行实际流量时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算单个任务时间
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeSingleTaskTime(int i)
        {
            int singleTaskTime = 0; // 单个任务时间
            string rT = dataGridView1.Rows[i].Cells["跑路时间(秒)"].EditedFormattedValue.ToString().Trim(); // 跑路时间
            string oT = dataGridView1.Rows[i].Cells["操作时间(秒)"].EditedFormattedValue.ToString().Trim(); // 操作时间
            string rN = dataGridView1.Rows[i].Cells["资源数量"].EditedFormattedValue.ToString().Trim(); // 资源数量
            string dR = dataGridView1.Rows[i].Cells["获得几率"].EditedFormattedValue.ToString().TrimEnd('%'); // 掉落几率

            if (rT == "" || rT == "——") // 未设置跑路时间
            {
                MessageBox.Show(string.Format("未设置第{0}行跑路时间!", i.ToString()));
                return;
            }
            if (oT == "" || oT == "——") // 未设置操作时间
            {
                MessageBox.Show(string.Format("未设置第{0}行操作时间!", i.ToString()));
                return;
            }
            if (rN == "") // 未设置资源数量
            {
                MessageBox.Show(string.Format("未设置第{0}行资源数量!", i.ToString()));
                return;
            }
            if (dR == "") // 未设置掉落几率
            {
                MessageBox.Show(string.Format("未设置第{0}行掉落几率!", i.ToString()));
                return;
            }

            string kind = dataGridView1.Rows[i].Cells["任务类型"].Value.ToString();
            if(kind == "对话任务" || kind == "送信任务") // 对话任务或送信任务
            {
                rN = "0";
                dR = "100";
            }

            try
            {
                int runTime = int.Parse(rT); // 跑路时间
                int operateTime = int.Parse(oT); // 操作时间
                int resourceNumber = int.Parse(rN); // 资源数量
                float dropRate = float.Parse(dR); // 掉落几率
                // 单个任务时间 = 跑路时间 + 操作时间 * 资源数量 / 掉落几率
                singleTaskTime = (int)(runTime + operateTime * resourceNumber * 100 / dropRate);
                dataGridView1.Rows[i].Cells["单个任务时间(秒)"].Value = singleTaskTime.ToString();
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行单个任务时间时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算所有同期任务时间
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeCoTaskTime(int i)
        {
            string id = dataGridView1.Rows[i].Cells["任务名称"].Tag.ToString().Trim();
            int sum = 0;

            if(coTaskTable[id] != null) // 存在同期任务
            {
                List<string> idList = coTaskTable[id] as List<string>;
                foreach(string s in idList)
                {
                    List<int> indexList = idTable[s] as List<int>;
                    if(indexList != null)
                    {
                        foreach(int k in indexList)
                        {
                            DataGridViewRow r = dataGridView1.Rows[k];
                            string str = r.Cells["单个任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 获取单个任务时间
                            if (str == "" || str == "——") // 忽略空值
                            {
                                continue;
                            }
                            int value = int.Parse(str);
                            sum += value;
                        }
                    }
                }
                // 同期所有任务时间 = ∑同期单个任务时间
                dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].Value = sum.ToString();
            }
            else // 不存在同期任务
            {
                dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].Value = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].EditedFormattedValue.ToString();
            }
        }

        /// <summary>
        /// 计算其他同期任务的时间
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeOtherCoTaskTime(int i)
        {
            string total = dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期所有任务时间
            string single = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 单个任务时间

            if (total == "" || total == "——") // 未设置同期所有任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期所有任务时间!", i.ToString()));
                return;
            }
            if (single == "" || single == "——") // 未设置单个任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行单个任务时间!", i.ToString()));
                return;
            }
          
            try
            {
                int totalNumber = int.Parse(total);
                int singleNumber = int.Parse(single);
                // 同期其他任务时间 = 同期所有任务时间 - 单个任务时间
                dataGridView1.Rows[i].Cells["同期其他任务时间(秒)"].Value = (totalNumber - singleNumber).ToString();
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行其他同期任务时间时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算同期任务加算
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeCoTaskPlus(int i)
        {
            float coTaskPlus = 0.00f;
            // 加算比例 = (1/(同期其他任务时间/同期所有任务时间)) * 100%
            string str1 = dataGridView1.Rows[i].Cells["同期其他任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期其他任务时间
            string str2 = dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期所有任务时间

            if (str1 == "" || str1 == "——") // 未设置同期其他任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期其他任务时间!", i.ToString()));
                return;
            }
            if (str2 == "" || str2 == "——") // 未设置同期所有任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期所有任务时间!", i.ToString()));
                return;
            }      
            
            try
            {
                int otherCoTaskTime = int.Parse(str1);
                int coTaskTime = int.Parse(str2);
                // 加算比例 = (1/(同期其他任务时间/同期所有任务时间)) * 100%
                coTaskPlus = (float) coTaskTime / otherCoTaskTime;
                if(coTaskPlus > 1)
                {
                    dataGridView1.Rows[i].Cells["同期任务加算"].Style.ForeColor = Color.Green;
                }
                if(coTaskPlus > 1.5)
                {
                    coTaskPlus = 1.50f;
                }
                dataGridView1.Rows[i].Cells["同期任务加算"].Value = string.Format("{0:f1}", coTaskPlus);
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行同期任务加算比例时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算公有资源加算
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputePublicResourcePlus(int i)
        {
            float publicResourcePlus = 0.00f;
            if(dataGridView1.Rows[i].Cells["需求资源"].Tag == null) // 没有资源
            {
                dataGridView1.Rows[i].Cells["公有资源加算"].Value = "1";
                return;
            }
            string requireResource = dataGridView1.Rows[i].Cells["需求资源"].Tag.ToString();
            
            if(publicResourceList.Contains(requireResource)) // 是公共需求资源
            {
                float maxValue = 0.00f;
                float totalValue = 0.00f;
                try
                {
                    List<int> resourceList = publicResourceTable[requireResource] as List<int>;
                    foreach (int j in resourceList)
                    {
                        DataGridViewRow r = dataGridView1.Rows[j];
                        string requireNumber = r.Cells["需求数量"].EditedFormattedValue.ToString();
                        string requireRate = r.Cells["获得几率"].EditedFormattedValue.ToString().TrimEnd('%');
                        int number = int.Parse(requireNumber);
                        float rate = float.Parse(requireRate);
                        //加算比例 = MAX(需求数量/获得几率) / ∑(需求数量/获得几率)
                        float value = (float)number / rate;
                        totalValue += value;
                        if (value > maxValue)
                        {
                            maxValue = value;
                        }
                    }

                    /*foreach (DataGridViewRow r in dataGridView1.Rows)
                    {
                        if(r.Cells["需求资源"].Tag == null) // 没有资源
                        {                         
                            continue;
                        }
                        string resource = r.Cells["需求资源"].Tag.ToString();
                        if (resource == requireResource)
                        {
                            string requireNumber = r.Cells["需求数量"].EditedFormattedValue.ToString();
                            string requireRate = r.Cells["获得几率"].EditedFormattedValue.ToString().TrimEnd('%');
                            int number = int.Parse(requireNumber);
                            float rate = float.Parse(requireRate);
                            //加算比例 = MAX(需求数量/获得几率) / ∑(需求数量/获得几率)
                            float value = (float)number / rate;
                            totalValue += value;
                            if (value > maxValue)
                            {
                                maxValue = value;
                            }
                        }
                    }*/
                    publicResourcePlus = maxValue / totalValue;
                    if(publicResourcePlus < 1)
                    {
                        dataGridView1.Rows[i].Cells["公有资源加算"].Style.ForeColor = Color.Red;
                    }
                }
                catch(Exception ex)
                {
                    MessageBox.Show(string.Format("在计算第{0}行公有资源加算比例时产生异常: " + ex.ToString(), i.ToString()));
                }
            }
            else
            {
                publicResourcePlus = 1.00f;    
            }
            dataGridView1.Rows[i].Cells["公有资源加算"].Value = publicResourcePlus.ToString();
        }

        /// <summary>
        /// 计算加算流量
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeAdditionFlow(int i)
        {
            float additionalFlow = 0.00f;
            string virtualFlow = dataGridView1.Rows[i].Cells["实际流量"].EditedFormattedValue.ToString().Trim(); // 实际流量
            string publicResourcePlus = dataGridView1.Rows[i].Cells["公有资源加算"].EditedFormattedValue.ToString().Trim(); // 公有资源加算
            string coTaskPlus = dataGridView1.Rows[i].Cells["同期任务加算"].EditedFormattedValue.ToString().Trim(); // 同期任务加算
            string flowPlus = dataGridView1.Rows[i].Cells["流量加算"].EditedFormattedValue.ToString().Trim(); // 流量加算

            if (virtualFlow == "" || virtualFlow == "——") // 未设置实际流量
            {
                MessageBox.Show(string.Format("未设置第{0}行实际流量!", i.ToString()));
                return;
            }
            if (virtualFlow == "∞") // 实际流量为∞
            {
                dataGridView1.Rows[i].Cells["加算流量"].Style.ForeColor = Color.Blue;
                dataGridView1.Rows[i].Cells["加算流量"].Value = "∞";
                return;
            }
            if (publicResourcePlus == "" || publicResourcePlus == "——") // 未设置公有资源加算
            {
                MessageBox.Show(string.Format("未设置第{0}行公有资源加算!", i.ToString()));
                return;
            }
            if (coTaskPlus == "" || coTaskPlus == "——") // 未设置同期任务加算
            {
                MessageBox.Show(string.Format("未设置第{0}行同期任务加算!", i.ToString()));
                return;
            }
            if (flowPlus == "" || flowPlus == "——") // 未设置流量加算
            {
                MessageBox.Show(string.Format("未设置第{0}行流量加算!", i.ToString()));
                return;
            } 
            
            try
            {
                float vFlow = float.Parse(virtualFlow);
                float pR = float.Parse(publicResourcePlus);
                float cP = float.Parse(coTaskPlus);
                float fP = float.Parse(flowPlus);
                // 加算流量 = 实际流量*公有资源加算*同期任务加算*流量加算
                additionalFlow = vFlow * pR * cP * fP;
                float downNumber = additionalFlowSectionList[i][1]; // 数值下限
                float upNumber = additionalFlowSectionList[i][0]; // 数值上限
                if (downNumber == 0.00 && upNumber == 0.00) // 未设置上下限
                {
                    MessageBox.Show(string.Format("未设置第{0}行加算流量数据的上下限!", i.ToString()));
                    return;
                }
                if (additionalFlow < downNumber)
                {
                    dataGridView1.Rows[i].Cells["加算流量"].Style.ForeColor = Color.Red;
                }
                else if (additionalFlow > upNumber)
                {
                    dataGridView1.Rows[i].Cells["加算流量"].Style.ForeColor = Color.Blue;
                    if(additionalFlow > upNumber * 2)
                    {
                        dataGridView1.Rows[i].Cells["加算流量"].Value = "∞";
                        return;
                    }
                }
                else
                {
                    dataGridView1.Rows[i].Cells["加算流量"].Style.ForeColor = Color.Green;
                }
                dataGridView1.Rows[i].Cells["加算流量"].Value = string.Format("{0:f1}", additionalFlow);
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行加算流量时产生异常: " + ex.ToString(), i.ToString()));
            }        
        }

        /// <summary>
        /// 计算加算经验
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeAdditionalExp(int i)
        {
            float additionalExp = 0.00f;
            string vE = dataGridView1.Rows[i].Cells["单位时间经验(分)"].EditedFormattedValue.ToString().Trim(); // 实际经验
            string oT = dataGridView1.Rows[i].Cells["操作时间(秒)"].EditedFormattedValue.ToString().Trim(); // 操作时间
            string rN = dataGridView1.Rows[i].Cells["需求数量"].EditedFormattedValue.ToString().Trim(); // 需求数量
            string rT = dataGridView1.Rows[i].Cells["跑路时间(秒)"].EditedFormattedValue.ToString().Trim(); // 跑路时间
            string kP = dataGridView1.Rows[i].Cells["任务类型"].EditedFormattedValue.ToString().Trim(); // 任务类型
            string eP = dataGridView1.Rows[i].Cells["经验加算"].EditedFormattedValue.ToString().Trim(); // 经验加算

            if (vE == "" || vE == "——") // 未设置实际经验
            {
                MessageBox.Show(string.Format("未设置第{0}行实际经验!", i.ToString()));
                return;
            }
            if (oT == "" || oT == "——") // 未设置操作时间
            {
                MessageBox.Show(string.Format("未设置第{0}行操作时间!", i.ToString()));
                return;
            }
            if (rN == "" || rN == "——") // 未设置需求数量
            {
                MessageBox.Show(string.Format("未设置第{0}行需求数量!", i.ToString()));
                return;
            }
            if (rT == "" || rT == "——") // 未设置跑路时间
            {
                MessageBox.Show(string.Format("未设置第{0}行跑路时间!", i.ToString()));
                return;
            }
            if (eP == "" || eP == "——") // 未设置经验加算
            {
                MessageBox.Show(string.Format("未设置第{0}行经验加算!", i.ToString()));
                return;
            }
            
            string kind = kP;
            switch(kind)
            {
                case "前置任务":
                    {
                        kP = "1.00";
                        break;
                    }
                case "脚本任务":
                    {
                        kP = "1.15";
                        break;
                    }
                case "采集任务":
                    {
                        kP = "1.20";
                        break;
                    }
                case "收集任务":
                    {
                        kP = "1.15";
                        break;
                    }
                case "杀怪任务":
                    {
                        kP = "1.20";
                        break;
                    }
                case "送信任务":
                    {
                        kP = "1.05";
                        break;
                    }
                case "对话任务":
                    {
                        kP = "1.00";
                        break;
                    }
                default:
                    {
                        kP = "1.00";
                        break;
                    }
            }
   
            try
            {
                float virtualExp = float.Parse(vE); // 实际经验
                float operateTime = float.Parse(oT); // 操作时间
                float requireNumber = float.Parse(rN); // 需求数量
                float runTime = float.Parse(rT); // 跑路时间
                float kindPlus = float.Parse(kP); // 类型加算
                float expPlus = float.Parse(eP); // 经验加算
                // 任务经验 = (基础等级经验*(操作时间*需求数量+跑路时间)/60)*类型加算*经验加算
                additionalExp = (virtualExp * (operateTime * requireNumber + runTime) / 60) * kindPlus * expPlus;
                dataGridView1.Rows[i].Cells["单位时间金钱(分)"].Value = additionalExp.ToString();
            }
            catch (Exception ex)
            {
            	MessageBox.Show(string.Format("在计算第{0}行加算经验时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算压力比例
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputePressRate(int i)
        {
            float pressRate = 0.00f;
            string sT = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 单个任务时间
            string cT = dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期所有任务时间

            if (sT == "" || sT == "——") // 未设置单个任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行单个任务时间!", i.ToString()));
                return;
            }
            if (cT == "" || cT == "——") // 未设置同期所有任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期所有任务时间!", i.ToString()));
                return;
            }

            try
            {
                int singTastTime = int.Parse(sT); // 单个任务时间
                int coTastTime = int.Parse(cT); // 同期所有任务时间
                pressRate = (float) singTastTime * 100 / coTastTime;
                dataGridView1.Rows[i].Cells["压力比例"].Value = string.Format("{0:f1}%", pressRate);
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行压力比例时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        ///  计算降压比例
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeDepressRate(int i)
        {
            float depressRate = 0.00f;
            string oT = dataGridView1.Rows[i].Cells["同期其他任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期其他任务时间
            string cT = dataGridView1.Rows[i].Cells["同期所有任务时间(秒)"].EditedFormattedValue.ToString().Trim(); // 同期所有任务时间

            if (oT == "" || oT == "——") // 未设置同期其他任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期其他任务时间!", i.ToString()));
                return;
            }
            if (cT == "" || cT == "——") // 未设置同期所有任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行同期所有任务时间!", i.ToString()));
                return;
            }

            try
            {
                int otherCoTastTime = int.Parse(oT); // 同期其他任务时间
                int coTastTime = int.Parse(cT); // 同期所有任务时间
                depressRate = (float) otherCoTastTime * 100 / coTastTime;
                dataGridView1.Rows[i].Cells["降压比例"].Value = string.Format("{0:f1}%", depressRate);
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行降压比例时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算加算比例
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeAdditionalRate(int i)
        {
            float additionalRate = 0.00f;
            string dR = dataGridView1.Rows[i].Cells["降压比例"].EditedFormattedValue.ToString().TrimEnd('%'); // 降压比例

            if (dR == "" || dR == "——") // 未设置降压比例
            {
                MessageBox.Show(string.Format("未设置第{0}行降压比例!", i.ToString()));
                return;
            }
           
            try
            {
                float depressRate = float.Parse(dR); // 降压比例
                if(depressRate == 0.00) // 降压比例为零
                {
                    dataGridView1.Rows[i].Cells["加算比例"].Value = "∞";
                }
                else
                {
                    additionalRate = 100 / depressRate;
                    dataGridView1.Rows[i].Cells["加算比例"].Value = string.Format("{0:f1}%", additionalRate);
                }
                
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行加算比例时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算标准经验
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeNormalExp(int i)
        {
            int normalExp = 0;
            string singleTaskTime = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].Value.ToString(); // 单个任务时间
            string normalTime = dataGridView1.Rows[i].Cells["标准时间(秒)"].Value.ToString(); // 标准时间
            string singleTaskExp = dataGridView1.Rows[i].Cells["单位时间经验(分)"].Value.ToString(); // 单位时间经验

            if (singleTaskTime == "" || singleTaskTime == "——") // 未设置单个任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行单个任务时间!", i.ToString()));
                return;
            }

            if (normalTime == "" || normalTime == "——") // 未设置标准时间
            {
                MessageBox.Show(string.Format("未设置第{0}行标准时间!", i.ToString()));
                return;
            }

            if (singleTaskExp == "" || singleTaskExp == "——") // 未设置单位时间经验
            {
                MessageBox.Show(string.Format("未设置第{0}行单位时间经验!", i.ToString()));
                return;
            }

            try
            {
                int singleTT = int.Parse(singleTaskTime);
                int normalT = int.Parse(normalTime);
                int singleTE = int.Parse(singleTaskExp);

                // 标准经验=max(标准时间, 实际时间) * 该任务单位时间经验
                if (singleTT > normalT)
                {
                    normalExp = (int)(singleTT * singleTE / 60);
                }
                else
                {
                    normalExp = (int)(normalT * singleTE / 60);
                }

                totalNormalExp += normalExp; // 计算标准经验总和
                dataGridView1.Rows[i].Cells["标准经验"].Value = normalExp.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行标准经验时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算实际经验
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeActualExp(int i)
        {
            int actualExp = 0;
            string singleTaskTime = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].Value.ToString(); // 单个任务时间
            string singleTaskExp = dataGridView1.Rows[i].Cells["单位时间经验(分)"].Value.ToString(); // 单位时间经验

            if (singleTaskTime == "" || singleTaskTime == "——") // 未设置单个任务时间
            {
                MessageBox.Show(string.Format("未设置第{0}行单个任务时间!", i.ToString()));
                return;
            }

            if (singleTaskExp == "" || singleTaskExp == "——") // 未设置单位时间经验
            {
                MessageBox.Show(string.Format("未设置第{0}行单位时间经验!", i.ToString()));
                return;
            }

            try
            {
                int singleTT = int.Parse(singleTaskTime);
                int singleTE = int.Parse(singleTaskExp);

                // 实际经验 = 实际时间 * 单位时间经验
                actualExp = (int)(singleTT * singleTE / 60);
                totalActualExp += actualExp; // 计算实际经验总和
                dataGridView1.Rows[i].Cells["实际经验"].Value = actualExp.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行实际经验时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算经验总和
        /// </summary>
        private void ComputeTotalExp()
        {
            int totalNormalExp = 0; // 标准经验总和
            int totalActualExp = 0; // 实际经验总和

            try
            {
                foreach (DataGridViewRow r in dataGridView1.Rows)
                {

                    if ((bool)r.Cells["自动计算"].EditedFormattedValue == true)
                    {
                        // 计算标准经验和
                        string normalExp = r.Cells["标准经验"].EditedFormattedValue.ToString();
                        if (normalExp == "" || normalExp == "——")
                        {
                            continue;
                        }
                        int normalE = int.Parse(normalExp);
                        totalNormalExp += normalE;

                        // 计算实际经验和
                        string actualExp = r.Cells["实际经验"].EditedFormattedValue.ToString();
                        if (actualExp == "" || actualExp == "——")
                        {
                            continue;
                        }
                        int actualE = int.Parse(actualExp);
                        totalActualExp += actualE;
                    }
                }

                this.totalNormalExp = totalNormalExp;
                this.totalActualExp = totalActualExp;
            }
            catch (Exception ex)
            {
                MessageBox.Show("在计算经验总和时产生异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 计算重新分配经验
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeReallocExp(int i)
        {
            int reallocExp = 0; // 重新分配经验

            try
            {
                string actualExperiment = dataGridView1.Rows[i].Cells["实际经验"].EditedFormattedValue.ToString(); // 实际经验
                if (actualExperiment == "" || actualExperiment == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行实际经验!", i.ToString()));
                    return;
                }
                int actualEx = int.Parse(actualExperiment);

                // 重新分配经验 = (所有标准经验和 / 所有实际经验和) * 该任务实际经验
                reallocExp = (int)(((float)totalNormalExp / totalActualExp) * actualEx);
                dataGridView1.Rows[i].Cells["重新分配经验"].Value = reallocExp.ToString();
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行重新分配经验时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算重新分配金钱
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeReallocMoney(int i)
        {
            int reallocMoney = 0; // 重新分配金钱
            int totalNormalMoney = 0; // 标准金钱总和
            float totalTaskRate = 0; // 任务等级比例和

            try
            {
                string level = dataGridView1.Rows[i].Cells["等级"].EditedFormattedValue.ToString().Trim();

                foreach (DataGridViewRow r in dataGridView1.Rows)
                {
                    if ((bool)r.Cells["自动计算"].EditedFormattedValue == true)
                    {
                        if(r.Cells["等级"].EditedFormattedValue.ToString().Trim() == level) // 同等级的任务
                        {
                            // 计算任务等级比例和
                            string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型 
                            float rewardRate = 1f;
                            switch(kind)
                            {
                                case "前置任务":
                                    {
                                        break;
                                    }
                                case "脚本任务":
                                    {
                                        rewardRate = 1.15f;
                                        break;
                                    }
                                case "采集任务":
                                    {
                                        rewardRate = 1.15f;
                                        break;
                                    }
                                case "收集任务":
                                    {
                                        rewardRate = 1.2f;
                                        break;
                                    }
                                case "杀怪任务":
                                    {
                                        rewardRate = 1.2f;
                                        break;
                                    }
                                case "送信任务":
                                    {
                                        rewardRate = 1.05f;
                                        break;
                                    }
                                case "对话任务":
                                    {
                                        break;
                                    }
                                case "高级任务":
                                    {
                                        rewardRate = 1.3f;
                                        break;
                                    }
                                case "精英任务":
                                    {
                                        rewardRate = 1.5f;
                                        break;
                                    }
                            }

                            string singleTaskTime = r.Cells["单个任务时间(秒)"].EditedFormattedValue.ToString(); // 单个任务时间
                            if(singleTaskTime == "" || singleTaskTime == "——")
                            {
                                continue;
                            }
                            int singleTT = int.Parse(singleTaskTime);

                            // 计算任务等级比例
                            // 任务等级比例 = 任务类型加算 * 单个任务时间
                            float taskRate = rewardRate * singleTT;
                            totalTaskRate += taskRate;
                        }
                    }
                }

                string theKind = dataGridView1.Rows[i].Cells["任务类型"].Value.ToString(); // 任务类型 
                float theRewardRate = 1f;
                switch (theKind)
                {
                    case "前置任务":
                        {
                            break;
                        }
                    case "脚本任务":
                        {
                            theRewardRate = 1.15f;
                            break;
                        }
                    case "采集任务":
                        {
                            theRewardRate = 1.15f;
                            break;
                        }
                    case "收集任务":
                        {
                            theRewardRate = 1.2f;
                            break;
                        }
                    case "杀怪任务":
                        {
                            theRewardRate = 1.2f;
                            break;
                        }
                    case "送信任务":
                        {
                            theRewardRate = 1.05f;
                            break;
                        }
                    case "对话任务":
                        {
                            break;
                        }
                    case "高级任务":
                        {
                            theRewardRate = 1.3f;
                            break;
                        }
                    case "精英任务":
                        {
                            theRewardRate = 1.5f;
                            break;
                        }
                }

                string theSingleTaskTime = dataGridView1.Rows[i].Cells["单个任务时间(秒)"].EditedFormattedValue.ToString(); // 单个任务时间
                if (theSingleTaskTime == "" || theSingleTaskTime == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行单个任务时间!", i.ToString()));
                    return;
                }
                int theSTT = int.Parse(theSingleTaskTime);

                // 计算任务等级比例
                // 任务等级比例 = 任务类型加算 * 单个任务时间
                float theTaskRate = theRewardRate * theSTT;
                int l = int.Parse(level);
                if(l < taskData.TotalMoneyArray.Length)
                {
                    totalNormalMoney = taskData.TotalMoneyArray[l];
                }

                // 重新分配金钱 = (所有同等级任务标准金钱和 / 所有同等级任务比例和) * 该任务比例
                reallocMoney = (int)((totalNormalMoney / totalTaskRate) * theTaskRate);
                dataGridView1.Rows[i].Cells["重新分配金钱"].Value = reallocMoney.ToString();
                dataGridView1.Rows[i].Cells["重新分配金钱"].Tag = reallocMoney.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行重新分配金钱时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算经验终值
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeFinalExp(int i)
        {
            int finalExp = 0; // 经验终值

            try
            {
                string normalExp = dataGridView1.Rows[i].Cells["标准经验"].EditedFormattedValue.ToString(); // 标准经验
                string reallocExp = dataGridView1.Rows[i].Cells["重新分配经验"].EditedFormattedValue.ToString(); // 重新分配经验
                string flowPlus = dataGridView1.Rows[i].Cells["流量加算"].EditedFormattedValue.ToString(); // 流量加算
                string expPlus = dataGridView1.Rows[i].Cells["经验加算"].EditedFormattedValue.ToString(); // 经验加算
                string id = dataGridView1.Rows[i].Cells["任务名称"].Tag.ToString(); // 任务id

                if (normalExp == "" || normalExp == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行标准经验!", i.ToString()));
                    return;
                }

                if (reallocExp == "" || reallocExp == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行重新分配经验!", i.ToString()));
                    return;
                }

                if (flowPlus == "" || flowPlus == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行流量加算!", i.ToString()));
                    return;
                }

                if (expPlus == "" || expPlus == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行经验加算!", i.ToString()));
                    return;
                }

                int normalE = int.Parse(normalExp);
                int reallocE = int.Parse(reallocExp);
                float flowP = float.Parse(flowPlus);
                float expP = float.Parse(expPlus);

                // 经验终值 = max(任务经验, 重新分配经验) * 流量加算 * 经验加算
                if(normalE > reallocE)
                {
                    finalExp = (int)(normalE * flowP * expP);
                }
                else
                {
                    finalExp = (int)(reallocE * expP * expP);
                }
                finalExp = (int)((finalExp + 5) / 10) * 10; // 十位四舍五入

                if(finalExpTable[id] == null)
                {
                    finalExpTable[id] = finalExp.ToString();
                }
                else
                {
                    int totalExp = int.Parse(finalExpTable[id].ToString());
                    totalExp += finalExp;
                    finalExpTable[id] = totalExp.ToString();
                    dataGridView1.Rows[i].Cells["经验终值"].Style.ForeColor = Color.Gray;
                }
                dataGridView1.Rows[i].Cells["经验终值"].Value = finalExp.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行经验终值时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 计算金钱终值
        /// </summary>
        /// <param name="i">行序号</param>
        private void ComputeFinalMoney(int i)
        {
            int finalMoney = 0; // 金钱终值

            try
            {
                string reallocMoney = dataGridView1.Rows[i].Cells["重新分配金钱"].Tag.ToString(); // 重新分配金钱
                string flowPlus = dataGridView1.Rows[i].Cells["流量加算"].EditedFormattedValue.ToString(); // 流量加算
                string expPlus = dataGridView1.Rows[i].Cells["经验加算"].EditedFormattedValue.ToString(); // 经验加算
                string id = dataGridView1.Rows[i].Cells["任务名称"].Tag.ToString(); // 任务id

                if (reallocMoney == "")
                {
                    MessageBox.Show(string.Format("未设置第{0}行重新分配金钱!", i.ToString()));
                    return;
                }

                if (flowPlus == "" || flowPlus == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行流量加算!", i.ToString()));
                    return;
                }

                if (expPlus == "" || expPlus == "——")
                {
                    MessageBox.Show(string.Format("未设置第{0}行经验加算!", i.ToString()));
                    return;
                }

                int reallocM = int.Parse(reallocMoney);
                float flowP = float.Parse(flowPlus);
                float expP = float.Parse(expPlus);

                // 金钱终值 = 重新分配金钱 * 流量加算 * 经验加算            
                finalMoney = (int)(reallocM * expP * expP);               

                if (finalMoneyTable[id] == null)
                {
                    finalMoneyTable[id] = finalMoney.ToString();
                }
                else
                {
                    int totalMoney = int.Parse(finalMoneyTable[id].ToString());
                    totalMoney += finalMoney;
                    finalMoneyTable[id] = totalMoney.ToString();
                    dataGridView1.Rows[i].Cells["金钱终值"].Style.ForeColor = Color.Gray;
                }
                dataGridView1.Rows[i].Cells["金钱终值"].Value = finalMoney.ToString();
                dataGridView1.Rows[i].Cells["金钱终值"].Tag = finalMoney.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("在计算第{0}行金钱终值时产生异常: " + ex.ToString(), i.ToString()));
            }
        }

        /// <summary>
        /// 重新填写经验和金钱总数
        /// </summary>
        /// <param name="i"></param>
        private void CoordinateTotal(int i)
        {
            string id = dataGridView1.Rows[i].Cells["任务名称"].Tag.ToString(); // 任务id
            if(finalExpTable[id] == null || finalMoneyTable[id] == null) // 空值
            {
                dataGridView1.Rows[i].Cells["经验终值"].Value = "——";
                dataGridView1.Rows[i].Cells["金钱终值"].Value = "——";
                return;
            }
            string totalExp = finalExpTable[id].ToString();
            int totalMoney = int.Parse(finalMoneyTable[id].ToString());
            totalMoney = ((int)((totalMoney + 5) / 10)) * 10;

            dataGridView1.Rows[i].Cells["经验终值"].Value = totalExp.ToString();
            dataGridView1.Rows[i].Cells["金钱终值"].Value = totalMoney.ToString();
            dataGridView1.Rows[i].Cells["金钱终值"].Tag = totalMoney.ToString();
        }

        /// <summary>
        /// 同步所有同名任务的数值信息
        /// </summary>
        /// <param name="index">行序号</param>
        /// <param name="taskName">任务名称</param>
        /// <param name="value">数值信息</param>
        /// <param name="upward">是否向上检索</param>
        private void CoordinateData(int rowIndex, int columnIndex, string taskID, string value, bool upward)
        {
            if(rowIndex < 0 || rowIndex >= dataGridView1.Rows.Count) // 无效行
            {
                return;
            }

            if (dataGridView1.Rows[rowIndex].Cells["任务名称"].Tag.ToString() == taskID)
            {
                dataGridView1.Rows[rowIndex].Cells[columnIndex].Value = value;
                if(fontChangable) // 允许字体变化
                {
                    dataGridView1.Rows[rowIndex].Cells[columnIndex].Style.Font = boldFont;
                }
                if (upward) // 向上检索
                {             
                    CoordinateData(rowIndex - 1, columnIndex, taskID, value, upward);                    
                }
                else // 向下检索
                {                
                    CoordinateData(rowIndex + 1, columnIndex, taskID, value, upward);               
                }
            }
        }

        /// <summary>
        /// 同步所有同名资源的数量信息
        /// </summary>
        /// <param name="resourceName">资源名称</param>
        /// <param name="value">数量信息</param>
        private void CoordinateResourceNumber(string resourceName, string value)
        {
            foreach(DataGridViewRow r in dataGridView1.Rows)
            {
                if(r.Cells["需求资源"].Tag != null)
                {
                    if (r.Cells["需求资源"].Tag.ToString() == resourceName)
                    {
                        if (r.Cells["资源数量"].Value != null)
                        {
                            if (r.Cells["资源数量"].Value.ToString() != value)
                            {
                                r.Cells["资源数量"].Value = value;
                                if(fontChangable) // 允许字体变化
                                {
                                    r.Cells["资源数量"].Style.Font = boldFont;
                                }
                            }
                        }
                        else
                        {
                            r.Cells["资源数量"].Value = value;
                        }
                    }
                }          
            }
        }

        /// <summary>
        /// 同步所有同名资源刷新时间信息
        /// </summary>
        /// <param name="resourceName">资源名称</param>
        /// <param name="value">刷新时间</param>
        private void CoordinateRefreshTime(string resourceName, string value)
        {
            foreach (DataGridViewRow r in dataGridView1.Rows)
            {
                if (r.Cells["需求资源"].Tag != null)
                {
                    if (r.Cells["需求资源"].Tag.ToString() == resourceName)
                    {
                        if (r.Cells["刷新时间(秒)"].Value != null)
                        {
                            if (r.Cells["刷新时间(秒)"].Value.ToString() != value)
                            {
                                r.Cells["刷新时间(秒)"].Value = value;
                                if (fontChangable) // 允许字体变化
                                {
                                    r.Cells["刷新时间(秒)"].Style.Font = boldFont;
                                }
                            }
                        }
                        else
                        {
                            r.Cells["刷新时间(秒)"].Value = value;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 填充实际流量区间
        /// </summary>
        /// <param name="maxValue">实际流量上限</param>
        /// <param name="minValue">实际流量下限</param>
        private void FillVirtualFlowSection(float maxValue, float minValue)
        {
            foreach(float[] f in virtualFlowSectionList)
            {
                f[0] = maxValue;
                f[1] = minValue;
            }

            string value = string.Format("[{0}, {1}]", minValue, maxValue);
            foreach(DataGridViewRow r in dataGridView1.Rows)
            {
                string kind = r.Cells["任务类型"].Value.ToString();
                if (kind == "脚本任务" || kind == "收集任务" || kind == "采集任务" || kind == "杀怪任务") // 判断任务类型
                {
                    r.Cells["实际流量"].Value = value;
                }
                else
                {
                    r.Cells["实际流量"].Value = "——";
                }
            }
        }

        /// <summary>
        /// 填充加算流量区间
        /// </summary>
        /// <param name="maxValue">加算流量上限</param>
        /// <param name="minValue">加算流量下限</param>
        private void FillAdditionalFlowSection(float maxValue, float minValue)
        {
            foreach (float[] f in additionalFlowSectionList)
            {
                f[0] = maxValue;
                f[1] = minValue;
            }

            string value = string.Format("[{0}, {1}]", minValue, maxValue);
            foreach (DataGridViewRow r in dataGridView1.Rows)
            {
                string kind = r.Cells["任务类型"].Value.ToString();
                if (kind == "脚本任务" || kind == "收集任务" || kind == "采集任务" || kind == "杀怪任务") // 判断任务类型
                {
                    r.Cells["加算流量"].Value = value;
                }
                else
                {
                    r.Cells["加算流量"].Value = "——";
                }
            }
        }

        /// <summary>
        /// 转换金钱的表示形式
        /// </summary>
        /// <param name="n">铜的数量</param>
        /// <returns>金钱字符串</returns>
        private string ConvertMoney(int n)
        {
            string money = n.ToString();
            string result = "0金0银0铜";

            if(money.Length < 3)
            {
                result = string.Format("0金0银{0}铜", money);
            }
            else if(money.Length < 5)
            {
                result = string.Format("0金{0}银{1}铜", money.Substring(0, money.Length - 2),
                    money.Substring(money.Length - 2));
            }
            else
            {
                result = string.Format("{0}金{1}银{2}铜", money.Substring(0, money.Length - 4),
                    money.Substring(money.Length - 4, 2), money.Substring(money.Length - 2));
            }

            return result;
        }

        /// <summary>
        /// 转换金钱的表示形式
        /// </summary>
        /// <param name="n">金钱字符串</param>
        /// <returns>铜的数量</returns>
        private int ConvertMoney(string n)
        {
            string gold = "0";
            string silver = "0";
            string copper = "0";
            int goldNumber = 0;
            int silverNumber = 0;
            int copperNumber = 0;

            string regexPattern = @"(?<gold>\d*)金(?<silver>\d*)银(?<copper>\d*)铜";
            Regex r = new Regex(regexPattern);

            if (r.IsMatch(n))
            {
                gold = r.Match(n).Result("${gold}");
                goldNumber = int.Parse(gold);
                silver = r.Match(n).Result("${silver}");
                silverNumber = int.Parse(silver);
                copper = r.Match(n).Result("${copper}");
                copperNumber = int.Parse(copper);
            }

            return goldNumber * 10000 + silverNumber * 100 + copperNumber;
        }

        /// <summary>
        /// 全部选中全部计算框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem4_Click(object sender, EventArgs e)
        {
            startEdit = false;
            foreach(DataGridViewRow r in dataGridView1.Rows)
            {
                r.Cells["自动计算"].Value = selectAll;
                dataGridView1.CommitEdit(DataGridViewDataErrorContexts.Commit);
            }
            startEdit = true;
            if(selectAll) // 全部取消
            {
                buttonItem4.Text = "全部取消";
                selectAll = false;
            }
            else // 全部选中
            {
                buttonItem4.Text = "全部选中";
                selectAll = true;
            }
        }

        /// <summary>
        /// 显示右键菜单
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void dataGridView1_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if(dataGridView1.SelectedCells == null)
            {
                return;
            }

            DataGridViewCell currentCell = dataGridView1.SelectedCells[0];
            string kind = dataGridView1.Rows[currentCell.RowIndex].Cells["任务类型"].Value.ToString().Trim();

            if (e.Button == MouseButtons.Right) // 在有效行上点击右键
            {
                bool setDefault = defaultColumnList.Contains(currentCell.ColumnIndex);
                bool autoFill = fillableColumnList.Contains(currentCell.ColumnIndex);
                bool savable = savableColumnList.Contains(currentCell.ColumnIndex);

                setDefaultToolStripMenuItem.Enabled = setDefault;
                autoFillToolStripMenuItem.Enabled = autoFill;
                hideColumnToolStripMenuItem.Enabled = savable;
            
                computeCell = currentCell;
                currentCell.ContextMenuStrip = contextMenuStrip1;             
            }
        }

        /// <summary>
        /// 开始进行计算
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem5_Click(object sender, EventArgs e)
        {
            finalMoneyTable.Clear(); // 清空金钱终值表
            finalExpTable.Clear(); // 清空经验终值表
            
            // 初始化进度条
            pForm.TitleText = "数值计算中(第一阶段)";
            nodeTotal = dataGridView1.Rows.Count;
            nodeCount = 0;
            pForm.SetMaxValue(nodeTotal);
            pForm.Visible = true;
            startEdit = false;
            foreach(DataGridViewRow r in dataGridView1.Rows) // 第一次遍历
            {
                nodeCount++;
                pForm.SetValue(nodeCount);
                string taskType = r.Cells["任务类型"].Value.ToString();
                
                if((bool)r.Cells["自动计算"].EditedFormattedValue == true) // 是否已选中
                {
                    if (taskType == "前置任务") // 不用计算流量
                    {
                        // 添加实际流量
                        r.Cells["实际流量"].Value = "——";
                        // 添加加算流量
                        r.Cells["加算流量"].Value = "——";
                        // 添加单位时间经验
                        r.Cells["单位时间经验(分)"].Value = "——";
                        // 添加单位时间金钱
                        r.Cells["单位时间金钱(分)"].Value = "——";
                        // 添加同期其他任务时间
                        r.Cells["同期其他任务时间(秒)"].Value = "——";
                        // 添加同期所有任务时间
                        r.Cells["同期所有任务时间(秒)"].Value = "——"; 
                        // 添加压力比例
                        r.Cells["压力比例"].Value = "——";
                        // 添加降压比例
                        r.Cells["降压比例"].Value = "——";
                        // 添加加算比例
                        r.Cells["加算比例"].Value = "——";
                        // 添加公有资源加算
                        r.Cells["公有资源加算"].Value = "——";
                        // 添加同期任务加算
                        r.Cells["同期任务加算"].Value = "——";
                        // 添加单个任务时间
                        r.Cells["单个任务时间(秒)"].Value = "——";
                        // 添加标准经验
                        r.Cells["标准经验"].Value = "——";
                        // 添加实际经验
                        r.Cells["实际经验"].Value = "——";
                        // 添加重新分配经验
                        r.Cells["重新分配经验"].Value = "——";
                        // 添加经验终值
                        r.Cells["经验终值"].Value = "——";
                        // 添加重新分配金钱
                        r.Cells["重新分配金钱"].Value = "——";
                        // 添加金钱终值
                        r.Cells["金钱终值"].Value = "——";
                    }
                    else if (taskType == "送信任务" || taskType == "对话任务") // 需要计算经验及金钱
                    {
                        // 添加实际流量
                        r.Cells["实际流量"].Value = "——";
                        // 添加加算流量
                        r.Cells["加算流量"].Value = "——";
                        // 添加公有资源加算
                        r.Cells["公有资源加算"].Value = "——";

                        // 计算单个任务跑路时间
                        ComputeSingleTaskTime(r.Index);
                        // 计算标准经验
                        ComputeNormalExp(r.Index);
                        // 计算实际经验
                        ComputeActualExp(r.Index);
                    }
                    else // 需要计算流量
                    {
                        // 计算流量
                        ComputeVirtualFlow(r.Index);
                        // 计算单个任务跑路时间
                        ComputeSingleTaskTime(r.Index);
                        // 计算公有资源加算
                        ComputePublicResourcePlus(r.Index);
                        // 计算标准经验
                        ComputeNormalExp(r.Index);
                        // 计算实际经验
                        ComputeActualExp(r.Index);
                    }
                }
            }

            // ComputeTotalExp(); // 计算经验总和

            // 初始化进度条
            pForm.TitleText = "数值计算中(第二阶段)";
            nodeCount = 0;
            pForm.Visible = true;
            foreach(DataGridViewRow r in dataGridView1.Rows) // 第二次遍历
            {
                nodeCount++;
                pForm.SetValue(nodeCount);
                string taskType = r.Cells["任务类型"].Value.ToString();

                if ((bool)r.Cells["自动计算"].EditedFormattedValue == true) // 是否已选中
                {
                    if (taskType == "前置任务") // 不用计算流量
                    {

                    }
                    else if(taskType == "送信任务" || taskType == "对话任务") // 需要计算金钱及经验
                    {
                        // 计算所有同期任务时间
                        ComputeCoTaskTime(r.Index);
                        // 计算其他同期任务时间
                        ComputeOtherCoTaskTime(r.Index);
                        // 计算同期任务加算比例
                        ComputeCoTaskPlus(r.Index);
                        // 计算压力比例
                        ComputePressRate(r.Index);
                        // 计算降压比例
                        ComputeDepressRate(r.Index);
                        // 计算加算比例
                        ComputeAdditionalRate(r.Index);
                        // 计算重新分配经验
                        ComputeReallocExp(r.Index);
                        // 计算经验终值
                        ComputeFinalExp(r.Index);
                        // 计算重新分配金钱
                        ComputeReallocMoney(r.Index);
                        // 计算金钱终值
                        ComputeFinalMoney(r.Index);
                    }
                    else // 需要计算流量
                    {
                        // 计算所有同期任务时间
                        ComputeCoTaskTime(r.Index);
                        // 计算其他同期任务时间
                        ComputeOtherCoTaskTime(r.Index);
                        // 计算同期任务加算比例
                        ComputeCoTaskPlus(r.Index);
                        // 计算加算流量
                        ComputeAdditionFlow(r.Index);
                        // 计算压力比例
                        ComputePressRate(r.Index);
                        // 计算降压比例
                        ComputeDepressRate(r.Index);
                        // 计算加算比例
                        ComputeAdditionalRate(r.Index);
                        // 计算重新分配经验
                        ComputeReallocExp(r.Index);
                        // 计算经验终值
                        ComputeFinalExp(r.Index);
                        // 计算重新分配金钱
                        ComputeReallocMoney(r.Index);
                        // 计算金钱终值
                        ComputeFinalMoney(r.Index);
                    }
                }
            }

            foreach(DataGridViewRow r in dataGridView1.Rows) // 第三次遍历
            {
                if ((bool)r.Cells["自动计算"].EditedFormattedValue == true) // 是否已选中
                {
                    CoordinateTotal(r.Index);
                }
            }

            startEdit = true;
        }

        /// <summary>
        /// 处理单元格鼠标点击事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void dataGridView1_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if(dataGridView1.SelectedCells == null)
            {
                return;
            }
            DataGridViewCell currentCell = dataGridView1.SelectedCells[0];
        }

        /// <summary>
        /// 回复单元格默认值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void setDefaultToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(computeCell.Tag != null)
            {
                string defaultValue = computeCell.Tag.ToString(); // 默认值
                computeCell.Value = defaultValue;
            }
        }

        /// <summary>
        /// 检查单元格输入的有效性
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void dataGridView1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if(!startEdit) // 用户尚未开始编辑
            {
                return;
            }

            DataGridViewCell c = dataGridView1.Rows[e.RowIndex].Cells[e.ColumnIndex]; // 当前单元格
            bool fontBold = true; // 是否字体改为粗体

            switch(e.ColumnIndex)
            {
                case 0: // 同步同一任务的选择信息
                    {
                        string taskName = dataGridView1.Rows[e.RowIndex].Cells["任务名称"].Tag.ToString();
                        string value = c.Value.ToString();
                        startEdit = false;
                        CoordinateData(e.RowIndex - 1, 0, taskName, value, true);
                        CoordinateData(e.RowIndex + 1, 0, taskName, value, false);
                        startEdit = true;
                        break;
                    }
                case 7: // 同步所有的等级信息，并修改和等级有关的信息
                    {
                        string taskName = dataGridView1.Rows[e.RowIndex].Cells["任务名称"].Tag.ToString();
                        string value = c.Value.ToString();
                        startEdit = false;
                        CoordinateData(e.RowIndex - 1, 7, taskName, value, true);
                        CoordinateData(e.RowIndex + 1, 7, taskName, value, false);
                        startEdit = true;

                        int level = int.Parse(dataGridView1.Rows[e.RowIndex].Cells["等级"].Value.ToString().Trim()); // 等级
                        // 修改单位时间经验
                        string kind = dataGridView1.Rows[e.RowIndex].Cells["任务类型"].Value.ToString(); // 任务类型                                             
                        int exp = 0;

                        if(level < taskData.ExpArray.Length)
                        {
                            exp = taskData.ExpArray[level];                           
                            if (kind == "对话任务")
                            {
                            }
                            else if (kind == "送信任务")
                            {
                                exp = (int)(exp * 1.05);
                            }
                            else if (kind == "采集任务" || kind == "脚本任务")
                            {
                                exp = (int)(exp * 1.15);
                            }
                            else if (kind == "杀怪任务" || kind == "收集任务")
                            {
                                exp = (int)(exp * 1.2);
                            }        
                        }
                        dataGridView1.Rows[e.RowIndex].Cells["单位时间经验(分)"].Value = exp.ToString();
                        // 修改单位时间金钱
                        int money = 0;
                        if (level < taskData.MoneyArray.Length)
                        {
                            money = taskData.MoneyArray[level];
                            if (kind == "对话任务")
                            {
                            }
                            else if (kind == "送信任务")
                            {
                                money = (int)(money * 1.05);
                            }
                            else if (kind == "采集任务" || kind == "脚本任务")
                            {
                                money = (int)(money * 1.15);
                            }
                            else if (kind == "杀怪任务" || kind == "收集任务")
                            {
                                money = (int)(money * 1.2);
                            }    
                        }
                        dataGridView1.Rows[e.RowIndex].Cells["单位时间金钱(分)"].Value = money.ToString();
                        dataGridView1.Rows[e.RowIndex].Cells["单位时间金钱(分)"].Tag = money.ToString();
                        // 修改标准时间
                        int normalTime = 0;
                        if (level < taskData.NormalTimeArray5.Length)
                        {
                            normalTime = taskData.NormalTimeArray5[level];
                        }
                        dataGridView1.Rows[e.RowIndex].Cells["标准时间(秒)"].Value = normalTime.ToString();  
                        break;
                    }
                case 11: // 同步所有的资源数量信息
                    {
                        if(dataGridView1.Rows[e.RowIndex].Cells["需求资源"].Tag != null) // 有资源
                        {
                            string resourceName = dataGridView1.Rows[e.RowIndex].Cells["需求资源"].Tag.ToString();
                            if(publicResourceList.Contains(resourceName)) // 是公有资源
                            {
                                string value = dataGridView1.Rows[e.RowIndex].Cells["资源数量"].Value.ToString();
                                startEdit = false;
                                CoordinateResourceNumber(resourceName, value);
                                startEdit = true;
                            }        
                        }       
                        break;
                    }
                case 12: // 检测刷新时间的有效性
                    {
                        if(c.Tag != null) // 默认值非空
                        {
                            string defaultValue = c.Tag.ToString();
                            string value = c.Value.ToString();

                            if(defaultValue != "" && value != "") // 值不为空
                            {
                                try
                                {
                                    int dV = int.Parse(defaultValue); // 默认值
                                    int v = int.Parse(value); // 当前值
                                    if(v < dV)
                                    {
                                        if (dataGridView1.Rows[c.RowIndex].Cells["任务类型"].EditedFormattedValue.ToString() == "采集任务")
                                        {
                                            if(v < 90)
                                            {
                                                if(v != 1)
                                                {
                                                    DialogResult result = MessageBox.Show("是否进行修正", "输入值必须要么为1, 要么不能低于90!",
                                                        MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                                                    if(result == DialogResult.Yes)
                                                    {
                                                        startEdit = false;
                                                        c.Value = 1;
                                                        startEdit = true;
                                                        return;
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            DialogResult result = MessageBox.Show("是否进行修正", string.Format("输入值不能低于其下限值{0}!", defaultValue),
                                                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                                            if (result == DialogResult.Yes)
                                            {
                                                startEdit = false;
                                                c.Value = defaultValue;
                                                startEdit = true;
                                                return;
                                            }
                                        }
                                    }
                                }
                                catch(Exception ex)
                                {
                                    MessageBox.Show(string.Format("检测单元格[{0}, {1}]输入有效性时产生异常: " + ex.ToString(),
                                        e.RowIndex.ToString(), e.ColumnIndex.ToString()));
                                }

                                if(dataGridView1.Rows[e.RowIndex].Cells["需求资源"].Tag != null) // 有需求资源
                                {
                                    string requireResource = dataGridView1.Rows[e.RowIndex].Cells["需求资源"].Tag.ToString(); // 需求资源
                                    if(publicResourceList.Contains(requireResource)) // 是公共资源
                                    {
                                        startEdit = false;
                                        CoordinateRefreshTime(requireResource, value);
                                        startEdit = true;
                                    }
                                }
                            }                      
                        }
                        break;
                    }
                case 13: // 检测操作时间的有效性
                    {
                        if (c.Tag != null) // 默认值非空
                        {
                            string defaultValue = c.Tag.ToString();
                            string value = c.Value.ToString();

                            if (defaultValue != "" && value != "") // 值不为空
                            {
                                try
                                {
                                    int dV = int.Parse(defaultValue); // 默认值
                                    int v = int.Parse(value); // 当前值
                                    if (v < dV)
                                    {
                                        if (dataGridView1.Rows[c.RowIndex].Cells["任务类型"].EditedFormattedValue.ToString() == "收集任务" || 
                                            dataGridView1.Rows[c.RowIndex].Cells["任务类型"].EditedFormattedValue.ToString() == "杀怪任务" ||
                                            dataGridView1.Rows[c.RowIndex].Cells["任务类型"].EditedFormattedValue.ToString() == "采集任务")
                                        {
                                            DialogResult result = MessageBox.Show("是否进行修正", string.Format("输入值不能低于其下限值{0}!", defaultValue),
                                                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                                            if (result == DialogResult.Yes)
                                            {
                                                startEdit = false;
                                                c.Value = defaultValue;
                                                startEdit = true;
                                                return;
                                            }
                                        }
                                    }
                                }
                                catch (Exception ex)
                                {
                                    MessageBox.Show(string.Format("检测单元格[{0}, {1}]输入有效性时产生异常: " + ex.ToString(),
                                        e.RowIndex.ToString(), e.ColumnIndex.ToString()));
                                }
                            }
                        }
                        break;
                    }
                case 29: // 同步经验终值
                    {
                        string id = dataGridView1.Rows[e.RowIndex].Cells["任务名称"].Tag.ToString();
                        string value = c.Value.ToString();
                        startEdit = false;
                        CoordinateData(e.RowIndex - 1, 29, id, value, true);
                        CoordinateData(e.RowIndex + 1, 29, id, value, false);
                        startEdit = true;
                        break;
                    }
                case 31: // 同步金钱终值
                    {
                        string id = dataGridView1.Rows[e.RowIndex].Cells["任务名称"].Tag.ToString();
                        string value = c.Value.ToString();
                        startEdit = false;
                        CoordinateData(e.RowIndex - 1, 31, id, value, true);
                        CoordinateData(e.RowIndex + 1, 31, id, value, false);
                        startEdit = true;
                        break;
                    }
            }

            if(fontBold) // 修改值用粗体显示
            {
                if(fontChangable) // 允许字体变化
                {
                    c.Style.Font = boldFont;
                }
            }
        }

        /// <summary>
        /// 自动填充列
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void autoFillToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(computeCell == null) // 检测有效性
            {
                return;
            }
            if(computeCell.Value.ToString().Trim() == "" || computeCell.Value.ToString().Trim() == "——") // 值不能为空
            {
                MessageBox.Show("自动填充的数值不能为空!");
                return;
            }

            string data = computeCell.EditedFormattedValue.ToString();
            int rowIndex = computeCell.RowIndex; // 行序号
            int columnIndex = computeCell.ColumnIndex; // 列序号
        
            startEdit = false;
            switch(columnIndex)
            {
                case 11: // 填充资源数量
                    {
                        foreach(DataGridViewRow r in dataGridView1.Rows)
                        {
                            string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型
                            if(kind == "脚本任务" || kind == "采集任务" || kind == "收集任务" || kind == "杀怪任务") // 判断任务类型
                            {
                                r.Cells[columnIndex].Value = data;
                                if(fontChangable) // 允许字体变化
                                {
                                    r.Cells[columnIndex].Style.Font = boldFont; // 改变字体
                                }
                                
                            }
                            else
                            {
                                r.Cells[columnIndex].Style.BackColor = readOnlyColor;
                                r.Cells[columnIndex].Value = "——";
                            }
                        }
                        break;
                    }
                case 14: // 填充跑路时间
                    {
                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型
                            if(kind != "前置任务") // 判断任务类型
                            {
                                r.Cells[columnIndex].Value = data;
                                if (fontChangable) // 允许字体变化
                                {
                                    r.Cells[columnIndex].Style.Font = boldFont; // 改变字体
                                }
                            }
                            else
                            {
                                r.Cells[columnIndex].Style.BackColor = readOnlyColor;
                                r.Cells[columnIndex].Value = "——";
                            }
                        }
                        break;
                    }
                case 17: // 填充流量加算
                    {
                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型
                            if(kind != "前置任务") // 判断任务类型
                            {
                                r.Cells[columnIndex].Value = data;
                                if (fontChangable) // 允许字体变化
                                {
                                    r.Cells[columnIndex].Style.Font = boldFont; // 改变字体
                                }
                            }
                            else
                            {
                                r.Cells[columnIndex].Style.BackColor = readOnlyColor;
                                r.Cells[columnIndex].Value = "——";
                            }
                        }
                        break;
                    }
                case 18: // 填充经验加算
                    {
                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型
                            if(kind != "前置任务") // 判断任务类型
                            {
                                r.Cells[columnIndex].Value = data;
                                if (fontChangable) // 允许字体变化
                                {
                                    r.Cells[columnIndex].Style.Font = boldFont; // 改变字体
                                }
                            }
                            else
                            {
                                r.Cells[columnIndex].Style.BackColor = readOnlyColor;
                                r.Cells[columnIndex].Value = "——";
                            }
                        }
                        break;
                    }
            }
            startEdit = true;
        }

        /// <summary>
        /// 保存自定义数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem6_Click(object sender, EventArgs e)
        {
            string formerid = "";
            string formeroutput = "";
            bool exist = false;
            if(conn.State == ConnectionState.Closed) // 打开连接
            {
                conn.Open();
            }
            foreach(DataGridViewRow r in dataGridView1.Rows)
            {
                int index = r.Index;
                if(((bool) r.Cells["自动计算"].EditedFormattedValue == true)) // 该行被选中
                {
                    exist = true;
                    string id = r.Cells["任务名称"].Tag.ToString();
                    string output = virtualFlowSectionList[index][0] + "," + virtualFlowSectionList[index][1] +
                        "," + additionalFlowSectionList[index][0] + "," + additionalFlowSectionList[index][1] +
                        "," + r.Cells["资源数量"].EditedFormattedValue.ToString() + "," + r.Cells["跑路时间(秒)"].EditedFormattedValue.ToString() +
                        "," + r.Cells["流量加算"].EditedFormattedValue.ToString() + "," + r.Cells["经验加算"].EditedFormattedValue.ToString() +
                        ";";
                    if(id != formerid && formerid != "") // 新的id
                    {
                        SetQuestPlusInfo(formerid, formeroutput.TrimEnd(';')); // 更新附加信息
                        formerid = id;
                        formeroutput = output;
                    }
                    else
                    {
                        formerid = id;
                        formeroutput = formeroutput + output;
                    }

                    // 更新任务信息
                    string kind = r.Cells["任务类型"].Value.ToString(); // 任务类型
                    string attribute;
                    // 更新地图等级信息
                    string level = r.Cells["等级"].EditedFormattedValue.ToString();
                    if(level == "") // 等级为空
                    {
                        MessageBox.Show("第{0}号任务的等级未设置!", id);
                    }
                    else
                    {
                        UpdateMapLevel(id, level);
                    }
                    
                    // 更新物品的需求数量
                    if (kind == "脚本任务" || kind == "采集任务" || kind == "收集任务" || kind == "杀怪任务") // 检查任务类型
                    {    
                        string requireNumber = r.Cells["需求数量"].EditedFormattedValue.ToString();
                        attribute = r.Cells["需求数量"].Tag.ToString();
                        if (requireNumber == "") // 需求数量为空
                        {
                            MessageBox.Show("第{0}号任务的需求数量未设置!", id);
                        }
                        else
                        {
                            UpdateRequestNumber(id, attribute, requireNumber);
                        }
                    }
     
                    
                    // 更新物品的获得几率
                    if (kind == "采集任务" || kind == "收集任务") // 检查任务类型
                    {
                        attribute = r.Cells["获得几率"].Tag.ToString();
                        string rate = r.Cells["获得几率"].EditedFormattedValue.ToString().TrimEnd('%');
                        if (rate == "") // 获得几率为空
                        {
                            MessageBox.Show("第{0}号任务的获得几率未设置!", id);
                        }
                        else
                        {
                            int dropRate = (int)(float.Parse(rate) * 10485.76);
                            UpdateGetRate(id, attribute, dropRate.ToString());
                        }                  
                    }

                    // 更新NPC的刷新时间
                    if (kind == "收集任务" || kind == "杀怪任务") // 检查任务类型
                    {
                        attribute = npcIDTable[index.ToString()] as string; // NPC的ID
                        if(attribute != null)
                        {
                            string refreshTime = r.Cells["刷新时间(秒)"].EditedFormattedValue.ToString();
                            if (refreshTime == "") // 刷新时间为空
                            {
                                MessageBox.Show("第{0}号任务的刷新时间未设置!", id);
                            }
                            else
                            {
                                UpdateNPCRefreshTime(attribute, refreshTime);
                            }
                        }
                    }

                    // 更新Doodad的刷新时间
                    if (kind == "采集任务") // 检查任务类型
                    {
                        attribute = doodadIDTable[index.ToString()] as string; // Doodad的ID
                        if (attribute != null)
                        {
                            string refreshTime = r.Cells["刷新时间(秒)"].EditedFormattedValue.ToString();
                            if (refreshTime == "") // 刷新时间为空
                            {
                                MessageBox.Show("第{0}号任务的刷新时间未设置!", id);
                            }
                            else
                            {
                                UpdateDoodadRefreshTime(attribute, refreshTime);
                            }
                        }
                    }

                    // 更新任务的奖励经验
                    if(kind != "前置任务") // 检查任务类型
                    {
                        string rewardExp = r.Cells["经验终值"].EditedFormattedValue.ToString();
                        if (rewardExp == "") // 奖励经验为空
                        {
                            MessageBox.Show("第{0}号任务的奖励经验未设置!", id);
                        }
                        else
                        {
                            UpdatePresentExp(id, rewardExp);
                        } 
                    }

                    // 更新任务的奖励金钱
                    if(kind != "前置任务") // 检查任务类型
                    {
                        string rewardMoney = r.Cells["金钱终值"].EditedFormattedValue.ToString();
                        if (rewardMoney == "") // 奖励金钱为空
                        {
                            MessageBox.Show("第{0}号任务的奖励金钱未设置!", id);
                        }
                        else
                        {
                            UpdatePresentMoney(id, rewardMoney);
                        } 
                    }

                    // 改变单元格字体
                    if(fontChangable) // 允许字体变化
                    {
                        foreach (DataGridViewCell c in r.Cells)
                        {
                            if (c.Style.Font != regularFont)
                            {
                                c.Style.Font = regularFont;
                            }
                        }
                    }           
                }
            }
            if(exist)
            {
                SetQuestPlusInfo(formerid, formeroutput.TrimEnd(';')); // 更新附加信息
            }
            if (conn.State == ConnectionState.Open) // 关闭连接
            {
                conn.Close();
            }
            MessageBox.Show("自定义参数保存成功!");
        }

        /// <summary>
        /// 设置参数数值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem1_Click(object sender, EventArgs e)
        {
            DialogResult result = sa.ShowDialog();
            if(result != DialogResult.OK)
            {
                return;
            }
            startEdit = false;
            FillVirtualFlowSection(sa.MaxFlow, sa.MinFlow); // 填充实际流量区间
            FillAdditionalFlowSection(sa.MaxFlow, sa.MinFlow); // 填充加算流量区间
            startEdit = true;
            fontChangable = sa.FontChangable;
        }

        /// <summary>
        /// 用户选择保存数值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void hideColumnToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(computeCell != null)
            {
                string id = dataGridView1.Rows[computeCell.RowIndex].Cells["任务名称"].Tag.ToString(); // 任务id
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                switch(computeCell.ColumnIndex)
                {
                    case 7: // 任务等级
                        {
                            string level = computeCell.EditedFormattedValue.ToString();
                            UpdateMapLevel(id, level);
                            MessageBox.Show("数值保存成功!");
                            if (fontChangable) // 允许字体变化
                            {
                                computeCell.Style.Font = regularFont; // 改变字体
                            }
                            break;
                        }
                    case 9: // 需求数量
                        {
                            string requireNumber = computeCell.EditedFormattedValue.ToString();
                            string attribute = computeCell.Tag.ToString();
                            if (requireNumber == "" || requireNumber == "——") // 需求数量为空
                            {
                                MessageBox.Show("第{0}号任务的需求数量未设置!", id);
                            }
                            else
                            {
                                UpdateRequestNumber(id, attribute, requireNumber);
                                MessageBox.Show("数值保存成功!");
                                if (fontChangable) // 允许字体变化
                                {
                                    computeCell.Style.Font = regularFont; // 改变字体
                                }
                            }
                            break;
                        }
                    case 10: // 获得几率
                        {
                            string attribute = computeCell.Tag.ToString();
                            string rate = computeCell.EditedFormattedValue.ToString().TrimEnd('%');
                            if (rate == "" || rate == "——") // 获得几率为空
                            {
                                MessageBox.Show("第{0}号任务的获得几率未设置!", id);
                            }
                            else
                            {
                                int dropRate = (int)(float.Parse(rate) * 10485.76);
                                UpdateGetRate(id, attribute, dropRate.ToString());
                                MessageBox.Show("数值保存成功!");
                                if (fontChangable) // 允许字体变化
                                {
                                    computeCell.Style.Font = regularFont; // 改变字体
                                }
                            }    
                            break;
                        }
                    case 12: // 刷新时间
                        {
                            string refreshTime = computeCell.EditedFormattedValue.ToString();
                            string kind = computeCell.OwningRow.Cells["任务类型"].Value.ToString();

                            if(kind == "收集任务" || kind == "杀怪任务")
                            {
                                string attribute = npcIDTable[computeCell.RowIndex.ToString()] as string;
                                if (attribute != null)
                                {
                                    if (refreshTime == "") // 刷新时间为空
                                    {
                                        MessageBox.Show("第{0}号任务的刷新时间未设置!", id);
                                    }
                                    else
                                    {
                                        UpdateNPCRefreshTime(attribute, refreshTime);
                                        MessageBox.Show("数值保存成功!");
                                        if (fontChangable) // 允许字体变化
                                        {
                                            computeCell.Style.Font = regularFont; // 改变字体
                                        }
                                    }
                                }
                            }
                            else if (kind == "采集任务")
                            {
                                string attribute = doodadIDTable[computeCell.RowIndex.ToString()] as string;
                                if (attribute != null)
                                {
                                    if (refreshTime == "") // 刷新时间为空
                                    {
                                        MessageBox.Show("第{0}号任务的刷新时间未设置!", id);
                                    }
                                    else
                                    {
                                        UpdateDoodadRefreshTime(attribute, refreshTime);
                                        MessageBox.Show("数值保存成功!");
                                        if (fontChangable) // 允许字体变化
                                        {
                                            computeCell.Style.Font = regularFont; // 改变字体
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    case 29: // 经验终值
                        {
                            string rewardExp = computeCell.EditedFormattedValue.ToString();
                            if (rewardExp == "" || rewardExp == "——") // 奖励经验为空
                            {
                                MessageBox.Show("第{0}号任务的奖励经验未设置!", id);
                            }
                            else
                            {
                                UpdatePresentExp(id, rewardExp);
                                MessageBox.Show("数值保存成功!");
                                if (fontChangable) // 允许字体变化
                                {
                                    computeCell.Style.Font = regularFont; // 改变字体
                                }
                            } 
                            break;
                        }
                    case 31: // 金钱终值
                        {
                            string rewardMoney = computeCell.EditedFormattedValue.ToString();
                            if (rewardMoney == "" || rewardMoney == "——") // 奖励金钱为空
                            {
                                MessageBox.Show("第{0}号任务的奖励金钱未设置!", id);
                            }
                            else
                            {
                                UpdatePresentMoney(id, rewardMoney);
                                MessageBox.Show("数值保存成功!");
                                if (fontChangable) // 允许字体变化
                                {
                                    computeCell.Style.Font = regularFont; // 改变字体
                                }
                            } 
                            break;
                        }
                }
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 显示模式切换
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonItem2_Click(object sender, EventArgs e)
        {
            if(simpleMode) // 精简模式
            {
                foreach(int i in simpleModeList)
                {
                    dataGridView1.Columns[i].Visible = true;
                }
                buttonItem2.Text = "精简模式";
            }
            else // 完全模式
            {
                foreach(int i in simpleModeList)
                {
                    dataGridView1.Columns[i].Visible = false;
                }
                buttonItem2.Text = "完全模式";
            }
            simpleMode = !simpleMode;
        }
    }
}