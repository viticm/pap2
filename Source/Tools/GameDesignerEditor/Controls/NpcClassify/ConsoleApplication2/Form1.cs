using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace NpcClassify
{
    public partial class Form1 : Form
    {
        private int mode = 0; // 当前插件模式
        private string result; // 用户选择的返回结果
        private SqlConnection Conn; // sql连接
        private string tblname; // 表的名称
        private string catfieldname1; // 第一个属性的参数
        private string catfieldname2; // 第二个属性的参数
        private string oldValue; // 旧值参数
        private string mapIndex; // 地图序号
        private string mapName; // 地图名称
        private string doodadKind; // 小物件的类型
        private string defaultValue; // 默认值
        private DataRow defaultRow; // 默认行
        private bool firstSelect = false; // 是否是第一次选择树结点
        private List<string[]> rows = new List<string[]>(); // 记录查询结果的链表
        private List<KeyValuePair<string, string>> list1 = new List<KeyValuePair<string, string>>(); // 帮派键值对链表
        private List<KeyValuePair<string, string>> list2 = new List<KeyValuePair<string, string>>(); // 小物件键值对链表
        private List<KeyValuePair<string, string>> list3 = new List<KeyValuePair<string, string>>(); // 小物件注释键值对链表
        private List<KeyValuePair<string, string>> list4 = new List<KeyValuePair<string, string>>(); // 物品键值对链表1
        private List<KeyValuePair<string, string>> list5 = new List<KeyValuePair<string, string>>(); // 物品键值对链表2

        /// <summary>
        /// 构造函数
        /// </summary>
        public Form1()
        {
            InitializeComponent(); // 初始化组件
        }

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="Conn">sql连接</param>
        /// <param name="catfieldname1">属性1</param>
        /// <param name="catfieldname2">属性2</param>
        /// <param name="mode">模式序号</param>
        /// <param name="mapIndex">地图序号</param>
        /// <param name="oldValue">旧值</param>
        public Form1(SqlConnection Conn, string catfieldname1, string catfieldname2, int mode, string mapIndex, string oldValue)
        {
            this.Conn = Conn; // 获取sql连接
            this.mode = mode; // 获取当前的模式
            this.catfieldname1 = catfieldname1; // 获取第一个属性的参数
            this.catfieldname2 = catfieldname2; // 获取第二个属性的参数
            this.mapIndex = mapIndex; // 获取地图序号
            this.oldValue = oldValue; // 获取旧值参数
            InitializeComponent(); // 初始化组件
            switch (mode) // 初始化默认行
            {
                case 1: // 读取npc表
                    {
                        string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "NpcTemplate");
                        DataTable table = GetDataTable(sqlString);
                        if (table.Rows.Count > 0)
                        {
                            defaultRow = table.Rows[0];
                        }

                        break;
                    }
                case 2: // 读取doodad表
                    {
                        string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "tbl_doodad");
                        DataTable table = GetDataTable(sqlString);
                        if (table.Rows.Count > 0)
                        {
                            defaultRow = table.Rows[0];
                        }

                        break;
                    }
            }
            ChangeTabText(catfieldname1, catfieldname2); // 修改Tab显示的文本值
            FillMap(); // 填充程序数据键值对
            FillTree(); // 填充树结点
            ShowResult(); // 显示对旧值参数的查找结果
        }

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="Conn">sql连接</param>
        /// <param name="tblname">表名</param>
        /// <param name="catfieldname1">属性1</param>
        /// <param name="catfieldname2">属性2</param>
        /// <param name="mode">模式序号</param>
        /// <param name="oldValue">旧值</param>
        public Form1(SqlConnection Conn, string tblname, string catfieldname1, string catfieldname2, int mode, string oldValue)
        {
            this.Conn = Conn; // 获取sql连接
            this.tblname = tblname; // 获取表的名称
            this.catfieldname1 = catfieldname1; // 获取第一个属性的参数
            this.catfieldname2 = catfieldname2; // 获取第二个属性的参数
            this.mode = mode; // 获取当前的模式
            this.oldValue = oldValue; // 获取旧值参数
            InitializeComponent(); // 初始化组件
            switch (mode) // 初始化默认行
            {
                case 1: // 读取npc表
                    {
                        string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "NpcTemplate");
                        DataTable table = GetDataTable(sqlString);
                        if (table.Rows.Count > 0)
                        {
                            defaultRow = table.Rows[0];
                        }

                        break;
                    }
                case 2: // 读取doodad表
                    {
                        string sqlString = string.Format("SELECT * FROM {0} WHERE ID = 0", "tbl_doodad");
                        DataTable table = GetDataTable(sqlString);
                        if (table.Rows.Count > 0)
                        {
                            defaultRow = table.Rows[0];
                        }

                        break;
                    }
            }
            ChangeTabText(catfieldname1, catfieldname2); // 修改Tab显示的文本值
            FillMap(); // 填充程序数据键值对
            FillTree(tblname, catfieldname1, catfieldname2, treeView1.Nodes); // 填充树结点
            FillTree(tblname, catfieldname2, catfieldname1, treeView2.Nodes);
            ShowResult(); // 显示对旧值参数的查找结果
        }

        /// <summary>
        /// 获取数据表默认值
        /// </summary>
        private void SetDefaultValue()
        {
            try
            {
                DataTable _tbl_ = GetDataTable("SELECT TOP 1 * FROM " + tblname);
                result = _tbl_.Rows[0][0].ToString().Trim();
                DialogResult = DialogResult.OK;
            }
            catch (Exception ex)
            {
                MessageBox.Show("读取数据表时产生异常: " + ex.ToString());
                DialogResult = DialogResult.No;
            }
            finally
            {
                this.Close(); // 关闭窗口
            }
        }

        /// <summary>
        /// 显示用户选择的字段的查询结果
        /// </summary>
        private void ShowResult()
        {
            switch (mode) // 选择不同的查询模式
            {
                case 1:
                    {
                        // 查询代码
                        string sql = string.Format("SELECT MapName, ForceID, Name FROM {0} WHERE ID = \'{1}\'", tblname, oldValue);
                        SqlCommand cmd = null;
                        SqlDataReader reader = null;

                        string text1 = ""; // MapName
                        string text2 = ""; // ForceID
                        string text3 = ""; // Name

                        try
                        {
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();
                          
                            if(!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            reader.Read(); // 读取查询结果
                            text1 = reader[0].ToString().Trim(); // MapName
                            text2 = reader[1].ToString().Trim(); // ForceID
                            text3 = reader[2].ToString().Trim(); // Name
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }

                        sql = string.Format("SELECT n.Name, n.ID, n.Title, m.middlemap FROM {0} n, middlemap_npc m WHERE n.id=m.id and n.MapName = \'{1}\' AND n.ForceID = \'{2}\' AND n.id=m.id AND n.MapName=m.map union all select n.name, n.id, n.title, null as middlemap from npctemplate n where n.MapName = \'{1}\' AND n.ForceID = \'{2}\' and n.id not in (select id from middlemap_npc)", tblname, text1, text2);
                        try
                        {
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();

                            if (!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            while (reader.Read()) // 遍历查询结果
                            {
                                rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), reader[2].ToString().Trim() });
                            }
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }
                            
                        foreach (KeyValuePair<string, string> k in list1) // 遍历帮派的键值对
                        {
                            if (text2 == k.Key)
                            {
                                text2 = k.Value;
                                break;
                            }
                        }

                        dataGridView1.ColumnCount = 4;
                        dataGridView1.Columns[0].Name = "Name";
                        dataGridView1.Columns[1].Name = "ID";
                        dataGridView1.Columns[2].Name = "Title";
                        dataGridView1.Columns[3].Name = "middlemap";
                        dataGridView1.Rows.Clear(); // 清除每行内容

                        foreach (string[] rowArray in rows) // 将查询到的数据添加入列表中
                        {
                            dataGridView1.Rows.Add(rowArray);
                        }

                        foreach (TreeNode node1 in treeView1.Nodes) // 遍历第一层树结点
                        {
                            if (text1 == node1.Text)
                            {
                                node1.Expand();
                                foreach (TreeNode node2 in node1.Nodes) // 遍历第二层数结点
                                {
                                    if (text2 == node2.Text)
                                    {
                                        treeView1.SelectedNode = node2;
                                        break;
                                    }
                                }
                                break;
                            }
                        }

                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            foreach (DataGridViewCell c in r.Cells)
                            {
                                if (c.Value != null && c.Value.ToString() == text3)
                                {
                                    dataGridView1.CurrentCell = c; // 选中查询结果
                                    break;
                                }
                            }
                        }

                        if (rows.Count == 0) // 检查查询结果是否为空
                        {
                            firstSelect = true;
                        }
                        textBoxX2.Text = string.Format("MapName = {0}, ForceID = {1}", text1, text2); // 显示注释           
                        break;
                    }
                case 2:
                    {
                        // 查询代码
                        string sql = string.Format("SELECT MapName, Kind, Name FROM {0} WHERE ID = \'{1}\'", tblname, oldValue);
                        SqlCommand cmd = null;
                        SqlDataReader reader = null;

                        string text1 = ""; // MapName
                        string text2 = ""; // Kind
                        string text3 = ""; // Name

                        try
                        {
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();                         

                            if (!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            reader.Read(); // 读取查询结果
                            text1 = reader[0].ToString().Trim(); // MapName
                            text2 = reader[1].ToString().Trim(); // Kind
                            text3 = reader[2].ToString().Trim(); // Name
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值信息时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }
                            
                        try
                        {
                            // 查询代码
                            sql = string.Format("SELECT Name, ID FROM {0} WHERE MapName = \'{1}\' AND Kind = \'{2}\'", tblname, text1, text2);
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();

                            if (!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            while (reader.Read()) // 遍历查询结果
                            {
                                rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim() });
                            }
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值信息时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }
                            
                        foreach (KeyValuePair<string, string> k in list2) // 遍历小物件的键值对
                        {
                            if (text2 == k.Key)
                            {
                                text2 = k.Value;
                                break;
                            }
                        }

                        dataGridView1.ColumnCount = 2;
                        dataGridView1.Columns[0].Name = "Name";
                        dataGridView1.Columns[1].Name = "ID";
                        dataGridView1.Rows.Clear(); // 清除每行内容

                        foreach (string[] rowArray in rows) // 将查询到的数据添加入列表中
                        {
                            dataGridView1.Rows.Add(rowArray);
                        }

                        foreach (TreeNode node1 in treeView1.Nodes) // 遍历第一层树结点
                        {
                            if (text1 == node1.Text)
                            {
                                node1.Expand();
                                foreach (TreeNode node2 in node1.Nodes) // 遍历第二层数结点
                                {
                                    if (text2 == node2.Text)
                                    {
                                        treeView1.SelectedNode = node2;
                                        break;
                                    }
                                }
                                break;
                            }
                        }

                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            foreach (DataGridViewCell c in r.Cells)
                            {
                                if (c.Value.ToString() == text3)
                                {
                                    dataGridView1.CurrentCell = c; // 选中查询结果
                                    break;
                                }
                            }
                        }

                        if (rows.Count == 0) // 检查查询结果是否为空
                        {
                            firstSelect = true;
                        }
                        textBoxX2.Text = string.Format("MapName = {0}, Kind = {1}", text1, text2);
                        break;
                    }
                case 3:
                    {
                        string text1 = ""; // Type
                        string text2 = ""; // Genre
                        string text3 = ""; // _CATEGORY
                        string text4 = ""; // Name

                        int index = int.Parse(mapIndex); // 地图类型序号
                        if ((index < 5) || (index > 8)) // 判断地图类型序号是否有效
                        {
                            mapIndex = "5"; // 自动设为5
                                // MessageBox.Show("地图类型参数无效！\n重设为Other.tab表");
                        }

                        foreach (KeyValuePair<string, string> k in list4) // 遍历物品的键值对1
                        {
                            if (mapIndex == k.Key)
                            {
                                text1 = k.Value;
                                mapName = k.Value;
                                tblname = k.Value;
                                break;
                            }
                        }

                        // 查询代码
                        string sql = string.Format("SELECT Genre, _CATEGORY, Name FROM {0} WHERE ID = \'{1}\'", text1, oldValue);
                        SqlCommand cmd = null;
                        SqlDataReader reader = null;

                        try
                        {
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();

                            if (!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            reader.Read(); // 读取查询结果
                            text2 = reader[0].ToString().Trim(); // Genre
                            text3 = reader[1].ToString().Trim(); // _CATEGORY
                            text4 = reader[2].ToString().Trim(); // Name
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }
                            
                        try
                        {
                            // 查询代码
                            sql = string.Format("SELECT Name, ID FROM {0} WHERE Genre = \'{1}\'", text1, text2);
                            cmd = new SqlCommand(sql, Conn);
                            reader = cmd.ExecuteReader();

                            if (!reader.HasRows) // 未读到数据
                            {
                                firstSelect = true;
                                return;
                            }

                            while (reader.Read()) // 遍历查询结果
                            {
                                rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), mapIndex });
                            }
                        }
                        catch(SqlException ex)
                        {
                            MessageBox.Show("在读取显示旧值时产生sql异常: " + ex.ToString());
                            firstSelect = true;
                        }
                        finally
                        {
                            if (reader != null) // 释放系统资源
                            {
                                reader.Close();
                            }
                        }      

                        foreach (KeyValuePair<string, string> k in list5) // 遍历物品的键值对2
                        {
                            if (text2 == k.Key)
                            {
                                text2 = k.Value;
                                break;
                            }
                        }

                        dataGridView1.ColumnCount = 3;
                        dataGridView1.Columns[0].Name = "Name";
                        dataGridView1.Columns[1].Name = "ID";
                        dataGridView1.Columns[2].Name = "Type";
                        dataGridView1.Rows.Clear(); // 清除每行内容

                        foreach (string[] rowArray in rows) // 将查询到的数据添加入列表中
                        {
                            dataGridView1.Rows.Add(rowArray);
                        }

                        foreach (TreeNode node1 in treeView1.Nodes) // 遍历第一层树结点
                        {
                            if (text1 == node1.Text)
                            {
                                node1.Expand();
                                foreach (TreeNode node2 in node1.Nodes) // 遍历第二层数结点
                                {
                                    if (text2 == node2.Text)
                                    {
                                        node2.Expand();
                                        foreach (TreeNode node3 in node2.Nodes) // 遍历第三层树结点
                                        {
                                            if (text3 == node3.Text)
                                            {
                                                treeView1.SelectedNode = node3;
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }

                        foreach (DataGridViewRow r in dataGridView1.Rows)
                        {
                            foreach (DataGridViewCell c in r.Cells)
                            {
                                if (c.Value.ToString() == text4)
                                {
                                    dataGridView1.CurrentCell = c; // 选中查询结果
                                    break;
                                }
                            }
                        }

                        if (rows.Count == 0) // 检查查询结果是否为空
                        {
                            firstSelect = true;
                        }
                        textBoxX2.Text = string.Format("Type = {0}, Genre = {1}, _CATEGORY = {2}", text1, text2, text3);
                        break;
                    }
                
            }
        }

        /// <summary>
        /// 修改Tab显示的文本值
        /// </summary>
        /// <param name="arg1">属性1</param>
        /// <param name="arg2">属性2</param>
        private void ChangeTabText(string arg1, string arg2)
        {
            tabItem1.Text = arg1;
            tabItem2.Text = arg2;
        }

        /// <summary>
        /// 填充程序数据键值对
        /// </summary>
        private void FillMap()
        {
            switch (mode)
            {
                case 1: // 填充RelationForce的键值对
                    {
                        FillRelationForceMap(); // 填充帮派ID与名称的键值对
                        break;
                    }
                case 2: // 填充DoodadTemplate的键值对
                    {
                        FillDoodadTemplateMap(); // 填充小物件Kind与名称的键值对
                        FillDoodadTemplateExplation(); // 填充小物件注释的键值对
                        break;
                    }
                case 3: // 填充Item的键值对
                    {
                        FillItemMap1(); // 填充物品键值对1
                        FillItemMap2(); // 填充物品键值对2
                        break;
                    }
            }
        }

        /// <summary>
        /// 填充帮派ID与名称的键值对
        /// </summary>                             
        private void FillRelationForceMap()
        {
            string sql = string.Format("SELECT Name, ID FROM {0}", "dic_npc_relation_force");
            SqlCommand cmd;
            SqlDataReader reader = null;

            try // 可能会抛出sql异常
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                while (reader.Read()) // 遍历查询结果，填充键值对
                {
                    string text1 = reader[0].ToString().Trim(); // Name
                    string text2 = reader[1].ToString().Trim(); // ID
                    if (!(text1 == "") && !(text2 == ""))
                    {
                        list1.Add(new KeyValuePair<string, string>(text2, text1));
                    }
                }
            }
            catch (SqlException ex) // 捕捉sql异常
            {
                MessageBox.Show("填充帮派键值对时发生sql异常: " + ex.ToString()); // 显示异常窗口
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }
        }

        /// <summary>
        /// 填充小物件Kind与名称的键值对
        /// </summary>
        private void FillDoodadTemplateMap()
        {
            list2.Add(new KeyValuePair<string, string>("0", "dkInvalid"));
            list2.Add(new KeyValuePair<string, string>("1", "dkNormal"));
            list2.Add(new KeyValuePair<string, string>("2", "dkCorpse"));
            list2.Add(new KeyValuePair<string, string>("3", "dkQuest"));
            list2.Add(new KeyValuePair<string, string>("4", "dkRead"));
            list2.Add(new KeyValuePair<string, string>("5", "dkDialog"));
            list2.Add(new KeyValuePair<string, string>("6", "dkAcceptQuest"));
            list2.Add(new KeyValuePair<string, string>("7", "dkTreasure"));
            list2.Add(new KeyValuePair<string, string>("8", "dkOrnament"));
            list2.Add(new KeyValuePair<string, string>("9", "dkCraftTarget"));
            list2.Add(new KeyValuePair<string, string>("10", "dkClientOnly"));
        }

        /// <summary>
        /// 填充小物件注释键值对链表
        /// </summary>
        private void FillDoodadTemplateExplation()
        {
            list3.Add(new KeyValuePair<string, string>("dkInvalid", "玩家初始"));
            list3.Add(new KeyValuePair<string, string>("dkNormal", "普通, 不可操作"));
            list3.Add(new KeyValuePair<string, string>("dkCorpse", "尸体, 特殊用"));
            list3.Add(new KeyValuePair<string, string>("dkQuest", "任务, 只有接了直接相关的任务后才可操作"));
            list3.Add(new KeyValuePair<string, string>("dkRead", "阅读, 可操作, 可带脚本, 鼠标显示为\"阅读\""));
            list3.Add(new KeyValuePair<string, string>("dkDialog", "对话, 可操作, 可带脚本, 鼠标显示为\"对话\""));
            list3.Add(new KeyValuePair<string, string>("dkAcceptQuest", "接交任务, 可操作, 可带脚本, 鼠标显示为\"任务\""));
            list3.Add(new KeyValuePair<string, string>("dkTreasure", "宝箱, 可操作, 可带脚本"));
            list3.Add(new KeyValuePair<string, string>("dkOrnament", "装饰, 不可操作"));
            list3.Add(new KeyValuePair<string, string>("dkCraftTarget", "采集, 生活技能采集物, 可操作"));
            list3.Add(new KeyValuePair<string, string>("dkClientOnly", "客户端, 客户端DOODAD, 不可操作"));
        }

        /// <summary>
        /// 填充物品键值对1
        /// </summary>
        private void FillItemMap1()
        {
            list4.Add(new KeyValuePair<string, string>("5", "Other"));
            list4.Add(new KeyValuePair<string, string>("6", "item_Custom_Weapon"));
            list4.Add(new KeyValuePair<string, string>("7", "item_Custom_Armor"));
            list4.Add(new KeyValuePair<string, string>("8", "Custom_Trinket"));
        }

        /// <summary>
        /// 填充物品键值对2
        /// </summary>
        private void FillItemMap2()
        {
            list5.Add(new KeyValuePair<string, string>("0", "未知"));
            list5.Add(new KeyValuePair<string, string>("1", "药品"));
            list5.Add(new KeyValuePair<string, string>("2", "任务"));
            list5.Add(new KeyValuePair<string, string>("3", "其他"));
        }

        /// <summary>
        /// 填充树节点
        /// </summary>
        private void FillTree()
        {            
            foreach (KeyValuePair<string, string> k1 in list4) // 遍历物品键值对1
            {
                // 构造第一层树结点
                TreeNode node1 = treeView1.Nodes.Add(k1.Value);
                TreeNode node2 = treeView2.Nodes.Add(k1.Value);

                // 调用填充两层树结点的方法
                FillTree(k1.Value, catfieldname1, catfieldname2, node1.Nodes);
                FillTree(k1.Value, catfieldname2, catfieldname1, node2.Nodes);
            }
        }

        /// <summary>
        /// 填充树结点
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="catfieldname1">属性1</param>
        /// <param name="catfieldname2">属性2</param>
        /// <param name="tree">树结点集合</param>                                  
        private void FillTree(string tblname, string catfieldname1, string catfieldname2, TreeNodeCollection tree)
        {
            string sql = string.Format("SELECT {0}, {1} FROM {2}", catfieldname1, catfieldname2, tblname);
            SqlCommand cmd;
            SqlDataReader reader = null;
            string text1; // 第一属性值
            string text2; // 第二属性值
            bool exist1 = false; // 第一层对应的树结点是否已存在
            bool exist2 = false; // 第二层对应的树结点是否已存在

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();

                while (reader.Read()) // 读取数据库查询结果集合
                {
                    text1 = reader[0].ToString().Trim();
                    text2 = reader[1].ToString().Trim();

                    switch (mode) // 选择遍历的键值对链表
                    {
                        case 1:
                            {
                                if (catfieldname2 == "ForceID")
                                {
                                    foreach (KeyValuePair<string, string> k in list1) // 遍历匹配键值对
                                    {
                                        if (text2 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text2 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    foreach (KeyValuePair<string, string> k in list1) // 遍历匹配键值对
                                    {
                                        if (text1 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text1 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        case 2:
                            {
                                if (catfieldname2 == "Kind")
                                {
                                    foreach (KeyValuePair<string, string> k in list2) // 遍历匹配键值对
                                    {
                                        if (text2 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text2 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    foreach (KeyValuePair<string, string> k in list2) // 遍历匹配键值对
                                    {
                                        if (text1 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text1 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        case 3:
                            {
                                if (catfieldname1 == "Genre")
                                {
                                    foreach (KeyValuePair<string, string> k in list5) // 遍历匹配键值对
                                    {
                                        if (text1 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text1 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    foreach (KeyValuePair<string, string> k in list5) // 遍历匹配键值对
                                    {
                                        if (text2 == k.Key) // 如果key值匹配，则用value值进行赋值
                                        {
                                            text2 = k.Value;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                    }

                    if (text1 != "") // 键值不为空
                    {
                        foreach (TreeNode node1 in tree) // 检查第一层对应的树结点是否已存在
                        {
                            if (node1.Text.Trim() == text1) // 第一层对应的树结点已经存在
                            {
                                exist1 = true;
                                foreach (TreeNode node2 in node1.Nodes) // 检查第二层对应的树结点是否已存在
                                {
                                    if (node2.Text.Trim() == text2) // 第二层对应的树结点已经存在
                                    {
                                        exist2 = true;
                                        break;
                                    }
                                }

                                if (!exist2 && (text2 != "")) // 第二层对应的树结点不存在，添加新结点
                                {
                                    TreeNode newNode = node1.Nodes.Add(text2);
                                }

                                exist2 = false; // 重设值
                                break;
                            }
                        }

                        if (!exist1) // 第一层对应的树结点不存在，添加新结点
                        {
                            TreeNode newNode1 = tree.Add(text1);

                            // 添加第二层的结点
                            if (text2 != "") // 不为空结点
                            {
                                TreeNode newNode2 = newNode1.Nodes.Add(text2);
                            }
                        }

                        exist1 = false; // 重设值
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("构造树结点时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }
        }

        /// <summary>
        /// 显示用户选择的树结点所包含的信息，两个列参数信息的版本
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="arg1">属性1</param>
        /// <param name="arg2">属性2</param>
        /// <param name="arg3">属性1的值</param>
        /// <param name="arg4">属性2的值</param>   
        private void ShowData(string tblname, string arg1, string arg2, string arg3, string arg4)
        {
            dataGridView1.ColumnCount = 2;
            dataGridView1.Columns[0].Name = "Name";
            dataGridView1.Columns[1].Name = "ID";
            if (mode == 1) // 处理npc分类
            {
                dataGridView1.ColumnCount = 4;
                dataGridView1.Columns[0].Name = "Name";
                dataGridView1.Columns[1].Name = "ID";
                dataGridView1.Columns[2].Name = "Title";
                dataGridView1.Columns[3].Name = "middlemap";
            }
            dataGridView1.Rows.Clear(); // 清除每行内容

            // 查询代码
            string sql = string.Format("SELECT Name, ID FROM {0} WHERE {1} = '{3}' AND {2} = '{4}' ORDER BY Name",
                tblname, arg3, arg4, arg1, arg2);

            if(defaultRow != null && defaultRow[arg3].ToString() == arg1) // 读取默认行内容
            {
                sql = string.Format("SELECT Name, ID FROM {0} WHERE ({1} = '{3}' OR {1} IS NULL) AND {2} = '{4}' ORDER BY Name",
                    tblname, arg3, arg4, arg1, arg2);
            }

            if(defaultRow != null && defaultRow[arg4].ToString() == arg2) // 读取默认行内容
            {
                sql = string.Format("SELECT Name, ID FROM {0} WHERE {1} = '{3}' AND ({2} = '{4}' OR {2} IS NULL) ORDER BY Name",
                    tblname, arg3, arg4, arg1, arg2);
            }

            if (mode == 1) // 处理npc分类
            {
                sql = string.Format("SELECT n.Name, n.ID, n.Title, m.middlemap FROM {0} n, middlemap_npc m WHERE n.id=m.id and n.{1} = '{3}' AND n.{2} = '{4}' union all select n.name, n.id, n.title, null as middlemap from npctemplate n where n.{1} = '{3}' AND n.{2} = '{4}' and n.id not in (select id from middlemap_npc) ORDER BY n.Name",
                    tblname, arg3, arg4, arg1, arg2);
            }
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                rows.Clear(); // 清空查询结果的数据
                while (reader.Read()) // 读取查询的数据
                {
                    if (mode == 1) // 处理npc分类
                    {
                        rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), reader[2].ToString().Trim(), reader[3].ToString().Trim() });
                    }
                    else // 处理小物件分类
                    {
                        rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim() });
                    }

                }
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询选择信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }

            foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
            {
                dataGridView1.Rows.Add(rowArray);
            }
        }

        /// <summary>
        /// 显示用户选择的树结点所包含的信息，一个列参数信息的版本
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="arg1">属性1的值</param>
        /// <param name="arg2">属性1</param>
        private void ShowData(string tblname, string arg1, string arg2)
        {
            dataGridView1.ColumnCount = 2;
            dataGridView1.Columns[0].Name = "Name";
            dataGridView1.Columns[1].Name = "ID";
            if (mode == 1) // 处理npc分类
            {
                dataGridView1.ColumnCount = 4;
                dataGridView1.Columns[0].Name = "Name";
                dataGridView1.Columns[1].Name = "ID";
                dataGridView1.Columns[2].Name = "Title";
                dataGridView1.Columns[3].Name = "middlemap";
            }
            dataGridView1.Rows.Clear(); // 清除每行内容

            // 查询代码
            string sql = string.Format("SELECT Name, ID FROM {0} WHERE {2} = \'{1}\' ORDER BY Name",
                tblname, arg1, arg2);
            if (mode == 1) // 处理npc分类
            {
                sql = string.Format("SELECT n.Name, n.ID, n.Title, m.middlemap FROM {0} n, middlemap_npc m WHERE n.id=m.id and {2} = \'{1}\' union all select n.name, n.id, n.title, null as middlemap from npctemplate n WHERE {2} = \'{1}\' and n.id not in (select id from middlemap_npc) ORDER BY n.Name",
                    tblname, arg1, arg2);
            }
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                rows.Clear(); // 清空查询结果的数据
                while (reader.Read()) // 读取查询的数据
                {
                    if (mode == 1) // 处理npc分类
                    {
                        rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), reader[2].ToString().Trim(), reader[3].ToString().Trim() });
                    }
                    else // 处理小物件分类
                    {
                        rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim() });
                    }
                }
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询选择信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }

            foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
            {
                dataGridView1.Rows.Add(rowArray);
            }
        }

        /// <summary>
        /// 显示用户选择的树结点所包含的信息，无列参数信息的版本
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="arg1">地图类型</param>
        private void ShowSpecialData(string tblname, string arg1)
        {
            dataGridView1.ColumnCount = 3;
            dataGridView1.Columns[0].Name = "Name";
            dataGridView1.Columns[1].Name = "ID";
            dataGridView1.Columns[2].Name = "Type";
            dataGridView1.Rows.Clear(); // 清除每行内容

            // 查询代码
            string sql = string.Format("SELECT TOP 10000 Name, ID FROM {0} ORDER BY Name", tblname);
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                rows.Clear(); // 清空查询结果的数据
                //dataGridView1.Rows.Clear();
                while (reader.Read()) // 读取查询的数据
                {
                    rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), arg1 });
                }
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询选择信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }

            bool bShow = true;
            if (rows.Count > 1000 && checkBox.Checked)
            { 
                bShow = false;
            }
            if (bShow)
            {
                foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
                {
                    dataGridView1.Rows.Add(rowArray);
                }
            }
        }

        /// <summary>
        /// 显示用户选择的树结点所包含的信息，一个参数信息的版本
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="arg1">属性1</param>
        /// <param name="arg2">属性1的值</param>
        /// <param name="arg3">地图类型</param>
        private void ShowSpecialData(string tblname, string arg1, string arg2, string arg3)
        {
            dataGridView1.ColumnCount = 3;
            dataGridView1.Columns[0].Name = "Name";
            dataGridView1.Columns[1].Name = "ID";
            dataGridView1.Columns[2].Name = "Type";
            dataGridView1.Rows.Clear(); // 清除每行内容

            string sql = string.Format("SELECT TOP 10000 Name, ID FROM {0} WHERE {1} = '{2}' ORDER BY Name",
                                tblname, arg1, arg2);
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                rows.Clear(); // 清空查询结果的数据
                while (reader.Read()) // 读取查询的数据
                {
                    rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), arg3 });
                }
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询选择信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if(reader != null) // 释放系统资源
                {
                    reader.Close(); 
                }
            }

            foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
            {
                dataGridView1.Rows.Add(rowArray);
            }
        }

        /// <summary>
        /// 显示用户选择的树结点所包含的信息，两个参数信息的版本
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="arg1">字段1</param>
        /// <param name="arg2">字段1的值</param>
        /// <param name="arg3">字段2</param>
        /// <param name="arg4">字段2的值</param>
        /// <param name="arg5">地图类型</param>
        private void ShowSpecialData(string tblname, string arg1, string arg2, string arg3, string arg4, string arg5)
        {
            dataGridView1.ColumnCount = 3;
            dataGridView1.Columns[0].Name = "Name";
            dataGridView1.Columns[1].Name = "ID";
            dataGridView1.Columns[2].Name = "Type";
            dataGridView1.Rows.Clear(); // 清除每行内容

            string sql = string.Format("SELECT TOP 10000 Name, ID FROM {0} WHERE {1} = '{2}' AND {3} = '{4}' ORDER BY Name",
                                       tblname, arg1, arg2, arg3, arg4);
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                rows.Clear(); // 清空查询结果的数据
                while (reader.Read()) // 读取查询的数据
                {
                    rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), arg5 });
                }
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询选择信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }

            foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
            {
                dataGridView1.Rows.Add(rowArray);
            }
        }

        /// <summary>
        /// 显示结点的文本注释
        /// </summary>
        /// <param name="text"></param>
        private void ShowExplaination(string text)
        {
            textBoxX2.Text = "无该类别的注释"; // 显示预设信息

            switch (mode)
            {
                case 1: // 显示NpcTemplate的注释
                    {
                        break;
                    }
                case 2: // 显示DoodadTemplate的注释
                    {
                        foreach (KeyValuePair<string, string> k in list3)
                        {
                            if (text == k.Key)
                            {
                                textBoxX2.Text = k.Value; // 显示注释值
                                break;
                            }
                        }
                        break;
                    }
                case 3: // 显示Item的注释
                    {
                        break;
                    }
            }
        }

        /// <summary>
        /// 根据id号获取额外的信息
        /// </summary>
        /// <param name="tblname">表名</param>
        /// <param name="catfieldname1">字段名</param>
        /// <param name="id">id号</param>
        /// <returns>额外信息</returns>
        private string GetExtraData(string tblname, string catfieldname1, string id)
        {
            string data = "0";
            string sql = string.Format("SELECT {0} FROM {1} WHERE ID = {2}",
                             catfieldname1, tblname, id);
            SqlCommand cmd;
            SqlDataReader reader = null;
            try
            {
                cmd = new SqlCommand(sql, Conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                data = reader[0].ToString().Trim();
            }
            catch (SqlException ex) // 捕获sql异常
            {
                MessageBox.Show("查询额外信息时发生发生sql异常: " + ex.ToString());
            }
            finally
            {
                if (reader != null) // 释放系统资源
                {
                    reader.Close();
                }
            }
            return data;
        }

        /// <summary>
        /// 用户选择值的属性
        /// </summary>
        public string Result
        {
            get
            {
                return result;
            }
        }

        /// <summary>
        /// 地图序号的属性
        /// </summary>
        public string MapIndex
        {
            get
            {
                return mapIndex;
            }
        }

        /// <summary>
        /// 地图名称的属性
        /// </summary>
        public string MapName
        {
            get
            {
                return mapName;
            }
        }

        /// <summary>
        /// 小物件类型的属性
        /// </summary>
        public string DoodadKind
        {
            get
            {
                return doodadKind;
            }
        }

        /// <summary>
        /// 用户选择treeView1上的结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            try
            {
                if (firstSelect) // 是否是第一次进行选择
                {
                    TreeNode currentNode = (sender as TreeView).SelectedNode; // 当前选中的结点

                    switch (currentNode.Level)
                    {
                        case 0: // 选中第一层树结点
                            {
                                string text1 = currentNode.Text.Trim(); // 第一属性信息

                                if (mode == 3) // 特殊的显示模式
                                {
                                    string text2 = "0"; // 地图对应的Type值

                                    foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                    {
                                        if (text1 == k.Value)
                                        {
                                            text2 = k.Key;
                                            break;
                                        }
                                    }
                                    ShowSpecialData(text1, text2);
                                }
                                else
                                {
                                    List<KeyValuePair<string, string>> l;

                                    if (mode == 1) // 处理RelationForce的树结点
                                    {
                                        l = list1;
                                    }
                                    else // 处理DoodadTemplate的树结点
                                    {
                                        l = list2;
                                    }

                                    ShowData(tblname, text1, catfieldname1); // 显示用户选择的树结点所包含的信息                               
                                }
                                break;
                            }
                        case 1: // 选中第二层树结点
                            {
                                string text1 = currentNode.Parent.Text.Trim(); // 第一层节点信息
                                string text2 = currentNode.Text.Trim(); // 第二层节点信息

                                if (mode == 3) // 特殊的显示模式
                                {
                                    string text3 = "0"; // 地图对应的Type值

                                    foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                    {
                                        if (text1 == k.Value)
                                        {
                                            text3 = k.Key;
                                            break;
                                        }
                                    }

                                    foreach (KeyValuePair<string, string> k in list5) // 遍历物品键值对2
                                    {
                                        if (text2 == k.Value)
                                        {
                                            text2 = k.Key;
                                        }
                                    }

                                    ShowSpecialData(text1, catfieldname1, text2, text3); // 显示用户选择的树结点所包含的信息                             
                                }
                                else
                                {
                                    List<KeyValuePair<string, string>> l;

                                    if (mode == 1) // 处理RelationForce的树结点
                                    {
                                        l = list1;
                                    }
                                    else // 处理DoodadTemplate的树结点
                                    {
                                        l = list2;
                                    }

                                    foreach (KeyValuePair<string, string> k in l) // 遍历匹配键值对
                                    {
                                        if (text2 == k.Value) // 如果value值匹配，则用key值进行赋值
                                        {
                                            text2 = k.Key;
                                            break;
                                        }
                                    }
                                    ShowData(tblname, text1, text2, catfieldname1, catfieldname2); // 显示用户选择的树结点所包含的信息
                                }
                                break;
                            }
                        case 2: // 选中第三层树结点
                            {
                                string text1 = currentNode.Parent.Parent.Text.Trim(); // 第一层节点信息
                                string text2 = currentNode.Parent.Text.Trim(); // 第二层节点信息
                                string text3 = currentNode.Text.Trim(); // 第三层结点信息

                                if (mode == 3) // 特殊的显示模式
                                {
                                    string text4 = "0"; // 地图对应的Type值

                                    foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                    {
                                        if (text1 == k.Value)
                                        {
                                            text4 = k.Key;
                                            break;
                                        }
                                    }

                                    foreach (KeyValuePair<string, string> k in list5) // 遍历物品键值对2
                                    {
                                        if (text2 == k.Value)
                                        {
                                            text2 = k.Key;
                                        }
                                    }

                                    ShowSpecialData(text1, catfieldname1, text2, catfieldname2, text3, text4); // 显示用户选择的树结点所包含的信息
                                }
                                else
                                {
                                    List<KeyValuePair<string, string>> l;

                                    if (mode == 1) // 处理RelationForce的树结点
                                    {
                                        l = list1;
                                    }
                                    else // 处理DoodadTemplate的树结点
                                    {
                                        l = list2;
                                    }

                                    foreach (KeyValuePair<string, string> k in l) // 遍历匹配键值对
                                    {
                                        if (text2 == k.Value) // 如果value值匹配，则用key值进行赋值
                                        {
                                            text2 = k.Key;
                                            break;
                                        }
                                    }
                                    ShowData(tblname, text1, text2, catfieldname1, catfieldname2); // 显示用户选择的树结点所包含的信息
                                }
                                break;
                            }
                    }

                    ShowExplaination(currentNode.Text); // 显示选中类别的注释
                    textBoxX1.Text = "关键字过滤..."; // 恢复关键字查询输入框的内容
                }
                else
                {
                    firstSelect = true;
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("读取显示树结点信息时产生异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            DataGridViewCell cell = null; // 用户选择的单元

            if (dataGridView1.SelectedCells == null) // 用户未选择单元
            {
                MessageBox.Show("无任何有效输入!\n程序将不保存任何修改信息!");
                DialogResult = DialogResult.No; // 返回取消信息
                this.Close(); // 关闭窗口
                return;
            }

            try // 检查用户是否进行选择
            {
                cell = dataGridView1.SelectedCells[0]; // 获取用户选择的单元
            }
            catch (Exception ex) // 捕获用户未作选择的异常
            {
                MessageBox.Show("无效的选择!\n程序将不保存任何修改信息!");
                DialogResult = DialogResult.No;
                this.Close();
                return;
            }

            DataGridViewCell resultCell = dataGridView1[1, cell.RowIndex]; // 贮存查询结果信息的单元
            result = resultCell.Value.ToString().Trim(); // 设置返回值

            switch (mode)
            {
                case 1:
                    {
                        break;
                    }
                case 2: // 设置小物件类型
                    {
                        doodadKind = GetExtraData(tblname, "Kind", result);
                        if((doodadKind == "") && (defaultRow != null) && !(defaultRow["Kind"] is DBNull))
                        {
                            doodadKind = defaultRow["Kind"].ToString();
                        }

                        break;
                    }
                case 3: // 设置地图类型
                    {
                        mapIndex = dataGridView1[2, cell.RowIndex].Value.ToString().Trim(); // 设置地图类型
                        foreach (KeyValuePair<string, string> k in list4)
                        {
                            if (mapIndex == k.Key)
                            {
                                mapName = k.Value; // 设置地图名称
                                break;
                            }
                        }
                        break;
                    }
            }
            if (mode == 3) // 是否需要设置地图类型
            {

            }

            DialogResult = DialogResult.OK; // 返回确认信息    
            this.Close(); // 关闭窗口
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No; // 返回取消信息
            this.Close();
        }

        /// <summary>
        /// 用户选中关键字查询框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_MouseClick(object sender, MouseEventArgs e)
        {
            if ((sender as TextBox).Text == "关键字过滤...")
            {
                textBoxX1.Text = ""; // 清空输入框
            }
        }

        /// <summary>
        /// 关键字查询框中的内容发生变化
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            string text = (sender as TextBox).Text; // 获取文本框中的内容
            if (text != "" && text != "关键字过滤...") // 关键字不为空
            {
                dataGridView1.Rows.Clear(); // 清除每行内容
                foreach (string[] s in rows) // 遍历查询结果
                {
                    if (s[0].Contains(text) || s[1].Contains(text))
                    {
                        dataGridView1.Rows.Add(s);
                    }
                }
            }
            else // 关键字为空，显示全部信息
            {
                dataGridView1.Rows.Clear(); // 清除每行内容

                bool bShow = true;
                if (rows.Count > 1000 && checkBox.Checked)
                {
                    bShow = false;
                }
                if (bShow)
                {
                    foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
                    {
                        dataGridView1.Rows.Add(rowArray);
                    }
                }

                //foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
                //{
                //    //dataGridView1.DataSource = DataTable
                //    dataGridView1.Rows.Add(rowArray);
                //}
            }
        }

        /// <summary>
        /// 用户选择恢复默认值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            SetDefaultValue(); // 获取数据表默认值
        }

        /// <summary>
        /// 用户选择treeView2上的结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView2_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode currentNode = (sender as TreeView).SelectedNode; // 当前选中的结点
            try
            {
                switch (currentNode.Level)
                {
                    case 0: // 选中第一层树结点
                        {
                            string text1 = currentNode.Text.Trim(); // 第一属性信息

                            if (mode == 3) // 特殊的显示模式
                            {
                                string text2 = "0"; // 地图对应的Type值

                                foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                {
                                    if (text1 == k.Value)
                                    {
                                        text2 = k.Key;
                                        break;
                                    }
                                }
                                ShowSpecialData(text1, text2);
                            }
                            else
                            {
                                List<KeyValuePair<string, string>> l;

                                if (mode == 1) // 处理RelationForce的树结点
                                {
                                    l = list1;
                                }
                                else // 处理DoodadTemplate的树结点
                                {
                                    l = list2;
                                }

                                text1 = currentNode.Text.Trim();
                                foreach (KeyValuePair<string, string> k in l) // 遍历匹配键值对
                                {
                                    if (text1 == k.Value) // 如果value值匹配，则用key值进行赋值
                                    {
                                        text1 = k.Key;
                                        break;
                                    }
                                }

                                ShowData(tblname, text1, catfieldname2); // 显示用户选择的树结点所包含的信息
                            }
                            break;
                        }
                    case 1: // 选中第二层树结点
                        {
                            string text1 = currentNode.Parent.Text.Trim(); // 第一层节点信息
                            string text2 = currentNode.Text.Trim(); // 第二层节点信息

                            if (mode == 3) // 特殊的显示模式
                            {
                                string text3 = "0"; // 地图对应的Type值

                                foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                {
                                    if (text1 == k.Value)
                                    {
                                        text3 = k.Key;
                                        break;
                                    }
                                }

                                ShowSpecialData(text1, catfieldname2, text2, text3); // 显示用户选择的树结点所包含的信息
                            }
                            else
                            {
                                List<KeyValuePair<string, string>> l;

                                if (mode == 1) // 处理RelationForce的树结点
                                {
                                    l = list1;
                                }
                                else // 处理DoodadTemplate的树结点
                                {
                                    l = list2;
                                }

                                text1 = currentNode.Text.Trim(); // 地图信息
                                text2 = currentNode.Parent.Text.Trim(); // 帮派信息

                                foreach (KeyValuePair<string, string> k in l) // 遍历匹配键值对
                                {
                                    if (text2 == k.Value) // 如果value值匹配，则用key值进行赋值
                                    {
                                        text2 = k.Key;
                                        break;
                                    }
                                }
                                ShowData(tblname, text1, text2, catfieldname1, catfieldname2); // 显示用户选择的树结点所包含的信息
                            }
                            break;
                        }
                    case 2: // 选中第三层树结点
                        {
                            string text1 = currentNode.Parent.Parent.Text.Trim(); // 第一层节点信息
                            string text2 = currentNode.Parent.Text.Trim(); // 第二层节点信息
                            string text3 = currentNode.Text.Trim(); // 第三层结点信息

                            if (mode == 3) // 特殊的显示模式
                            {
                                string text4 = "0"; // 地图对应的Type值

                                foreach (KeyValuePair<string, string> k in list4) // 遍历物品键值对1
                                {
                                    if (text1 == k.Value)
                                    {
                                        text4 = k.Key;
                                        break;
                                    }
                                }

                                text2 = currentNode.Text.Trim();
                                text3 = currentNode.Parent.Text.Trim();

                                foreach (KeyValuePair<string, string> k in list5) // 遍历物品键值对2
                                {
                                    if (text2 == k.Value)
                                    {
                                        text2 = k.Key;
                                    }
                                }

                                ShowSpecialData(text1, catfieldname1, text2, catfieldname2, text3, text4); // 显示用户选择的树结点所包含的信息
                            }
                            else
                            {
                                List<KeyValuePair<string, string>> l;

                                if (mode == 1) // 处理RelationForce的树结点
                                {
                                    l = list1;
                                }
                                else // 处理DoodadTemplate的树结点
                                {
                                    l = list2;
                                }

                                text1 = currentNode.Text.Trim(); // 地图信息
                                text2 = currentNode.Parent.Text.Trim(); // 帮派信息

                                foreach (KeyValuePair<string, string> k in l) // 遍历匹配键值对
                                {
                                    if (text2 == k.Value) // 如果value值匹配，则用key值进行赋值
                                    {
                                        text2 = k.Key;
                                        break;
                                    }
                                }

                                ShowData(tblname, text1, text2, catfieldname1, catfieldname2); // 显示用户选择的树结点所包含的信息
                            }
                            break;
                        }
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("读取显示树结点信息时产生异常: " + ex.ToString());
            }

            ShowExplaination(currentNode.Text); // 显示选中类别的注释
            textBoxX1.Text = "关键字过滤..."; // 恢复关键字查询输入框的内容
            textBoxX3.Text = "关键字过滤...";
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        /// <summary>
        /// 用户选中关键字查询框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX3_MouseClick(object sender, MouseEventArgs e)
        {
            if ((sender as TextBox).Text == "关键字过滤...")
            {
                textBoxX3.Text = ""; // 清空输入框
            }
        }

        /// <summary>
        /// 关键字查询框中的内容发生变化
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX3_TextChanged(object sender, EventArgs e)
        {
            string text = (sender as TextBox).Text; // 获取文本框中的内容
            if (text != "" && text != "关键字过滤...") // 关键字不为空
            {
                if (dataGridView1.ColumnCount == 0)
                {
                    dataGridView1.ColumnCount = 4;
                    dataGridView1.Columns[0].Name = "Name";
                    dataGridView1.Columns[1].Name = "ID";
                    dataGridView1.Columns[2].Name = "Title";
                    dataGridView1.Columns[3].Name = "middlemap";
                }
                dataGridView1.Rows.Clear(); // 清除每行内容

                string sql = string.Empty;
                string mapcomparestring = string.Empty;
                if (treeView1.SelectedNode != null)
                {
                    TreeNode snode = treeView1.SelectedNode.Parent != null ? treeView1.SelectedNode.Parent : treeView1.SelectedNode;
                    if (snode != null)
                        mapcomparestring = string.Format("and n.mapname = \'{0}\' ", snode.Text);
                }

                sql = string.Format("SELECT n.Name, n.ID, n.Title, m.middlemap FROM {0} n,middlemap_npc m WHERE n.id = m.id and n.Title like \'%{1}%\' {2} union all select n.name, n.id, n.title, null as middlemap from npctemplate n where n.Title like \'%{1}%\' {2} and n.id not in (select id from middlemap_npc)", tblname, text, mapcomparestring);

                SqlCommand cmd = null;
                SqlDataReader reader = null;
                try
                {
                    cmd = new SqlCommand(sql, Conn);
                    reader = cmd.ExecuteReader();

                    if (!reader.HasRows) // 未读到数据
                    {
                        firstSelect = true;
                        return;
                    }

                    while (reader.Read()) // 遍历查询结果
                    {
                        dataGridView1.Rows.Add(new string[] { reader[0].ToString().Trim(), reader[1].ToString().Trim(), reader[2].ToString().Trim(), reader[3].ToString().Trim() });
                    }
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("在读取显示旧值时产生sql异常: " + ex.ToString());
                    firstSelect = true;
                }
                finally
                {
                    if (reader != null) // 释放系统资源
                    {
                        reader.Close();
                    }
                }
            }
            else // 关键字为空，显示全部信息
            {
                dataGridView1.Rows.Clear(); // 清除每行内容
                foreach (string[] rowArray in rows) // 将查询到的数据添加如列表中
                {
                    dataGridView1.Rows.Add(rowArray);
                }
            }
        }
    }
}