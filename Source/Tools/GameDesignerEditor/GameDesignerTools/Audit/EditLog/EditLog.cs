using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;


namespace GameDesingerTools
{
    public partial class EditLog : Form
    {
        private SqlConnection conn; // sql连接
        private DataTable dataTable; // 数据表
        private DataTable modelTable; // 模块表
        private DataTable tabTable; // 属性页表
        private TreeNode node; // 当前选中的树结点
        private GridItem item; // 当前选中的GridItem
        private string fieldID; // 字段主键字符串
        private object[] primaryKeys; // 树结点主键数组
        private bool allUserName = true; // 是否选择全部用户条件
        private bool allNodeName = true; // 是否选择全部树结点条件
        private bool allCatName = true;  // 是否选择全部分类条件
        private bool allModelName = true; // 是否选择全部模块条件
        private bool allTabName = true; // 是否选择全部属性页条件
        private bool allFieldName = true; // 是否选择全部字段条件
        private bool beginSearch = false; // 是否已经开始查找
        private CatForm cForm = new CatForm(); // 分类选择窗体

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="fieldID">字段主键字符串</param>
        /// <param name="primaryKeys">树结点主键数组</param>
        /// <param name="searchLevel">搜索级别</param>
        public EditLog(SqlConnection conn, string fieldID, TreeNode node, GridItem item)
        {
            this.conn = conn;
            this.fieldID = fieldID;
            this.node = node;
            this.primaryKeys = node.Tag as object[];
            this.item = item;
            InitializeComponent();
            ResetTime();
            InitTable();
            InitData();
        }

        /// <summary>
        /// 初始化表格
        /// </summary>
        private void InitTable()
        {
            DataGridViewTextBoxColumn hostNameColumn = new DataGridViewTextBoxColumn();
            hostNameColumn.HeaderText = "用户名";
            hostNameColumn.Name = "用户名";

            DataGridViewTextBoxColumn editTimeColumn = new DataGridViewTextBoxColumn();
            editTimeColumn.HeaderText = "修改时间";
            editTimeColumn.Name = "修改时间";

            DataGridViewTextBoxColumn actionColumn = new DataGridViewTextBoxColumn();
            actionColumn.HeaderText = "操作";
            actionColumn.Name = "操作";

            DataGridViewTextBoxColumn modelNameColumn = new DataGridViewTextBoxColumn();
            modelNameColumn.HeaderText = "模块名";
            modelNameColumn.Name = "模块名";

            DataGridViewTextBoxColumn tabNameColumn = new DataGridViewTextBoxColumn();
            tabNameColumn.HeaderText = "属性页名";
            tabNameColumn.Name = "属性页名";

            DataGridViewTextBoxColumn catNameColumn = new DataGridViewTextBoxColumn();
            catNameColumn.HeaderText = "所属分类";
            catNameColumn.Name = "所属分类";

            DataGridViewTextBoxColumn recordNameColumn = new DataGridViewTextBoxColumn();
            recordNameColumn.HeaderText = "树结点名";
            recordNameColumn.Name = "树结点名";

            DataGridViewTextBoxColumn fieldDisplayNameColumn = new DataGridViewTextBoxColumn();
            fieldDisplayNameColumn.HeaderText = "字段名";
            fieldDisplayNameColumn.Name = "字段名";

            DataGridViewTextBoxColumn oldValueColumn = new DataGridViewTextBoxColumn();
            oldValueColumn.HeaderText = "旧值";
            oldValueColumn.Name = "旧值";

            DataGridViewTextBoxColumn newValueColumn = new DataGridViewTextBoxColumn();
            newValueColumn.HeaderText = "新值";
            newValueColumn.Name = "新值";

            DataGridViewTextBoxColumn remarksColumn = new DataGridViewTextBoxColumn();
            remarksColumn.HeaderText = "备注";
            remarksColumn.Name = "备注";

            dataGridView1.Columns.Add(hostNameColumn);
            dataGridView1.Columns.Add(editTimeColumn);

            dataGridView1.Columns.Add(actionColumn);

            dataGridView1.Columns.Add(modelNameColumn);
            dataGridView1.Columns.Add(tabNameColumn);
            dataGridView1.Columns.Add(catNameColumn);
            dataGridView1.Columns.Add(recordNameColumn);
            dataGridView1.Columns.Add(fieldDisplayNameColumn);
            dataGridView1.Columns.Add(oldValueColumn);
            dataGridView1.Columns.Add(newValueColumn);
            dataGridView1.Columns.Add(remarksColumn);
        }

        /// <summary>
        /// 修改初始的可选时间
        /// </summary>
        private void ResetTime()
        {
            DateTime dateTime = dateTimePicker1.Value;
            switch(dateTime.DayOfWeek) // 默认修改为本周一
            {
                case DayOfWeek.Monday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(0);
                        break;
                    }
                case DayOfWeek.Tuesday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-1);
                        break;
                    }
                case DayOfWeek.Wednesday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-2);
                        break;
                    }
                case DayOfWeek.Thursday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-3);
                        break;
                    }
                case DayOfWeek.Friday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-4);
                        break;
                    }
                case DayOfWeek.Saturday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-5);
                        break;
                    }
                case DayOfWeek.Sunday:
                    {
                        dateTimePicker1.Value = dateTime.AddDays(-6);
                        break;
                    }
            }
        }

        /// <summary>
        /// 获取当前字段的完整显示名称
        /// </summary>
        /// <returns>当前字段的完整显示名称</returns>
        private string GetFullDisplayName()
        {
            string fullDisplayName = item.Label; // 完整的显示名称
            GridItem parent = item.Parent; // 父GridItem

            while(parent != null) // 父GridItem不为空
            {
                DBCustomClass dbClass = parent.Value as DBCustomClass;
                if(dbClass != null && dbClass.ParentFieldID > 0) // 有效的字段并且不是分类
                {
                    fullDisplayName = string.Format("{0}.", parent.Label) + fullDisplayName;
                }
                parent = parent.Parent;
            }

            return fullDisplayName;
        }

        /// <summary>
        /// 重新填充查询范围的内容
        /// </summary>
        /// <param name="fieldid">字段id</param>
        private void FillSelectionNames(string fieldid)
        {
            //DataTable table = Helper.GetDataTable(string.Format("SELECT modtabid, fieldcnname FROM sys_meta_info WHERE fieldid = {0}", fieldid), conn);
            string filter = string.Format("fieldid = {0}", fieldid);
            DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

            if(table.Rows.Count > 0)
            {
                string tabID = table.Rows[0]["modtabid"].ToString();
                string fieldName = GetFullDisplayName();
                DataRow[] modelRows = tabTable.Select(string.Format("modtabid = {0}", tabID));
                if(modelRows.Length > 0)
                {
                    string modelID = modelRows[0]["modid"].ToString();
                    string nodeName = node.Text;
                    comboBoxEx5.Items[0] = string.Format("当前字段 [{0}]", fieldName);
                    comboBoxEx5.Items[1] = string.Format("当前记录 [{0}]", nodeName);
                    comboBoxEx5.Items[2] = string.Format("当前属性页 [{0}]", GetTabName(tabID));
                    comboBoxEx5.Items[3] = string.Format("当前模块 [{0}]", GetModelName(modelID));
                }
            }
        }

        /// <summary>
        /// 搜索当前字段的历史修改数据
        /// </summary>
        private void SearchFieldData()
        {
            beginSearch = false;
            DateTime startTime = dateTimePicker1.Value;
            DateTime endTime = dateTimePicker2.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            /* remoting
            string sql = string.Format("SELECT * FROM {0} WHERE fieldid = \'{1}\' AND keys = \'{2}\' AND edittime >= \'{3}\' AND edittime <= \'{4}\'",
                    "sys_dataRecord", fieldID, GetPrimaryKeyString(primaryKeys), newStartTime, newEndTime);
            dataTable = Helper.GetDataTable(sql, conn);
            */

            string filter = string.Format("fieldid = \'{0}\' AND keys = \'{1}\' AND edittime >= \'{2}\' AND edittime <= \'{3}\'",
                    fieldID, GetPrimaryKeyString(primaryKeys), newStartTime, newEndTime);
            dataTable = Helper.GetDataTableProxy("sys_dataRecord", filter, null, conn);


            dataGridView1.Rows.Clear(); // 清空原显示数据
            int index = 0;
            foreach (DataRow r in dataTable.Rows)
            {
                dataGridView1.Rows.Add();
                dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                if (r["action"].ToString() == "修改值")
                {
                    dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                }
                dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                index++;
            }
    
            if (dataGridView1.Rows.Count != 0) // 查询结果不为空
            {
                FillHostNames();
                FillModelNames();
                FillTabNames("所有模块");
                FillRecordNames();
                FillFieldNames();
                allUserName = true;
                checkBox1.Enabled = true;

                comboBoxEx2.Text = dataGridView1.Rows[0].Cells["模块名"].Value.ToString();
                comboBoxEx3.Text = dataGridView1.Rows[0].Cells["属性页名"].Value.ToString();
                comboBoxEx4.Text = dataGridView1.Rows[0].Cells["树结点名"].Value.ToString();
                comboBoxEx8.Text = dataGridView1.Rows[0].Cells["所属分类"].Value.ToString();
                comboBoxEx6.Text = dataGridView1.Rows[0].Cells["字段名"].Value.ToString();
                this.Text = string.Format("数据修改日志 [共找到{0}条相关信息]", index);
            }
            else
            {
                checkBox1.Enabled = false;
                this.Text = string.Format("数据修改日志 [没有找到相关信息]", index);
                MessageBox.Show("没有找到该字段的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            checkBox1.Checked = false;
            beginSearch = true;
        }

        /// <summary>
        /// 搜索当前记录的历史修改数据
        /// </summary>
        private void SearchRecordData()
        {
            beginSearch = false;
            DateTime startTime = dateTimePicker1.Value;
            DateTime endTime = dateTimePicker2.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            /* remoting
            string sql = string.Format("SELECT modtabid FROM {0} WHERE fieldid = \'{1}\'", 
                "sys_meta_info", fieldID);
            DataTable modTabIDTable = Helper.GetDataTable(sql, conn);
             */

            string filter = string.Format("fieldid = \'{0}\'",  fieldID);
            DataTable modTabIDTable = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

            if (modTabIDTable.Rows.Count != 0)
            {
                string modtabID = modTabIDTable.Rows[0]["modtabid"].ToString();
                DataRow[] tabRow = tabTable.Select(string.Format("modtabid = {0}", modtabID));
                //??
                string modelID = tabRow[0]["modid"].ToString();

                /* remoting
                sql = string.Format("SELECT * FROM {0} WHERE modelid = \'{1}\' AND keys = \'{2}\' AND edittime >= \'{3}\' AND edittime <= \'{4}\'",
                    "sys_dataRecord", modelID, GetPrimaryKeyString(primaryKeys), newStartTime, newEndTime);
                dataTable = Helper.GetDataTable(sql, conn);
                 */

                filter = string.Format("modelid = \'{0}\' AND keys = \'{1}\' AND edittime >= \'{2}\' AND edittime <= \'{3}\'",
                    modelID, GetPrimaryKeyString(primaryKeys), newStartTime, newEndTime);
                dataTable = Helper.GetDataTableProxy("sys_dataRecord", filter, null, conn); 

                if (dataTable.Rows.Count != 0)
                {
                    dataGridView1.Rows.Clear(); // 清空原显示数据
                    int index = 0;
                    foreach (DataRow r in dataTable.Rows)
                    {
                        dataGridView1.Rows.Add();
                        dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                        dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                        dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                        if (r["action"].ToString() == "修改值")
                        {
                            dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                        }
                        dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                        dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                        dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                        dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                        dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                        dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                        dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                        index++;
                    }

                    if(dataGridView1.Rows.Count != 0) // 查询结果不为空
                    {
                        FillHostNames();
                        FillModelNames();
                        FillTabNames("所有模块");
                        FillRecordNames();
                        FillFieldNames();
                        allUserName = true;
                        allFieldName = true;
                        checkBox1.Enabled = true;

                        comboBoxEx2.Text = dataGridView1.Rows[0].Cells["模块名"].Value.ToString();
                        comboBoxEx3.Text = dataGridView1.Rows[0].Cells["属性页名"].Value.ToString();
                        comboBoxEx4.Text = dataGridView1.Rows[0].Cells["树结点名"].Value.ToString();
                        comboBoxEx8.Text = dataGridView1.Rows[0].Cells["所属分类"].Value.ToString();
                        this.Text = string.Format("数据修改日志 [共找到{0}条相关信息]", index);
                    }                 
                }
                else
                {
                    checkBox1.Enabled = false;
                    this.Text = "数据修改日志 [没有找到相关信息]";
                    MessageBox.Show("没有找到该记录的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);                   
                }
            }
            else
            {
                checkBox1.Enabled = false;
                this.Text = "数据修改日志 [没有找到相关信息]";
                MessageBox.Show("没有找到该记录的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);              
            }
         
            checkBox1.Checked = false;           
            beginSearch = true;
        }

        /// <summary>
        /// 搜索当前属性页的历史修改数据
        /// </summary>
        private void SearchTabData()
        {
            beginSearch = false;
            DateTime startTime = dateTimePicker1.Value;
            DateTime endTime = dateTimePicker2.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            /* remoting
            string sql = string.Format(string.Format("SELECT modtabid FROM {0} WHERE fieldid = \'{1}\'", 
                "sys_meta_info", fieldID));
            DataTable modTabIDTable = Helper.GetDataTable(sql, conn);
            */

            string filter = string.Format("fieldid = \'{0}\'", fieldID);
            DataTable modTabIDTable = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

            if (modTabIDTable.Rows.Count != 0)
            {
                string modtabID = modTabIDTable.Rows[0]["modtabid"].ToString();

                /* remoting
                string sql = string.Format("SELECT * FROM {0} WHERE tabid = \'{1}\' AND edittime >= \'{2}\' AND edittime <= \'{3}\'", 
                    "sys_dataRecord", modtabID, newStartTime, newEndTime);
                dataTable = Helper.GetDataTable(sql, conn);
                */

                filter = string.Format("tabid = \'{0}\' AND edittime >= \'{1}\' AND edittime <= \'{2}\'",
                    modtabID, newStartTime, newEndTime);
                dataTable = Helper.GetDataTableProxy("sys_dataRecord", filter, null, conn);

                if (dataTable.Rows.Count != 0)
                {
                    dataGridView1.Rows.Clear(); // 清空原显示数据
                    int index = 0;
                    foreach (DataRow r in dataTable.Rows)
                    {
                        dataGridView1.Rows.Add();
                        dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                        dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                        dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                        if (r["action"].ToString() == "修改值")
                        {
                            dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                        }
                        dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                        dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                        dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                        dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                        dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                        dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                        dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                        index++;
                    }

                    if (dataGridView1.Rows.Count != 0) // 查询结果不为空
                    {
                        FillHostNames();
                        FillModelNames();
                        FillTabNames("所有模块");
                        FillRecordNames();
                        FillFieldNames();
                        comboBoxEx8.Text = "所有分类";
                        allUserName = true;
                        allNodeName = true;
                        allCatName = true;
                        allFieldName = true;
                        checkBox1.Enabled = true;

                        comboBoxEx2.Text = dataGridView1.Rows[0].Cells["模块名"].Value.ToString();
                        comboBoxEx3.Text = dataGridView1.Rows[0].Cells["属性页名"].Value.ToString();
                        this.Text = string.Format("数据修改日志 [共找到{0}条相关信息]", index);
                    }
                }
                else
                {
                    checkBox1.Enabled = false;
                    this.Text = "数据修改日志 [没有找到相关信息]";
                    MessageBox.Show("没有找到该属性页的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                checkBox1.Enabled = false;
                this.Text = "数据修改日志 [没有找到相关信息]";
                MessageBox.Show("没有找到该属性页的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            checkBox1.Checked = false;
            beginSearch = true;
        }

        /// <summary>
        /// 搜索当前模块的历史修改数据
        /// </summary>
        private void SearchModelData()
        {
            beginSearch = false;
            DateTime startTime = dateTimePicker1.Value;
            DateTime endTime = dateTimePicker2.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            /* remoting
            string sql = string.Format(string.Format("SELECT modtabid FROM {0} WHERE fieldid = \'{1}\'", 
                "sys_meta_info", fieldID));
            DataTable modTabIDTable = Helper.GetDataTable(sql, conn);
            */

            string filter = string.Format("fieldid = \'{0}\'", fieldID);
            DataTable modTabIDTable = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

            if (modTabIDTable.Rows.Count != 0)
            {
                string modtabID = modTabIDTable.Rows[0]["modtabid"].ToString();
                DataRow[] rows = tabTable.Select(string.Format("modtabid = {0}", modtabID));

                if(rows.Length != 0)
                {
                    string modelID = rows[0]["modid"].ToString();

                    /* remoting
                    sql = string.Format("SELECT * FROM {0} WHERE modelid = \'{1}\' AND edittime >= \'{2}\' AND edittime <= \'{3}\'", 
                        "sys_dataRecord", modelID, newStartTime, newEndTime);
                    dataTable = Helper.GetDataTable(sql, conn);
                    */

                    filter = string.Format("modelid = \'{0}\' AND edittime >= \'{1}\' AND edittime <= \'{2}\'",
                       modelID, newStartTime, newEndTime);
                     dataTable = Helper.GetDataTableProxy("sys_dataRecord",  filter, null, conn);
                    if (dataTable.Rows.Count != 0)
                    {
                        dataGridView1.Rows.Clear(); // 清空原显示数据
                        int index = 0;
                        foreach (DataRow r in dataTable.Rows)
                        {
                            dataGridView1.Rows.Add();
                            dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                            dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                            dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                            if (r["action"].ToString() == "修改值")
                            {
                                dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                            }
                            dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                            dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                            dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                            dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                            dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                            dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                            dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                            index++;
                        }

                        if (dataGridView1.Rows.Count != 0) // 查询结果不为空
                        {
                            FillHostNames();
                            FillModelNames();
                            FillTabNames("所有模块");
                            FillRecordNames();
                            FillFieldNames();
                            comboBoxEx8.Text = "所有分类";
                            allUserName = true;
                            allTabName = true;
                            allNodeName = true;
                            allCatName = true;
                            allFieldName = true;
                            checkBox1.Enabled = true;

                            comboBoxEx2.Text = dataGridView1.Rows[0].Cells["模块名"].Value.ToString();
                            this.Text = string.Format("数据修改日志 [共找到{0}条相关信息]", index);
                        }
                    }
                    else
                    {
                        checkBox1.Enabled = false;
                        this.Text = "数据修改日志 [没有找到相关信息]";
                        MessageBox.Show("没有找到该模块的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else
                {
                    checkBox1.Enabled = false;
                    this.Text = "数据修改日志 [没有找到相关信息]";
                    MessageBox.Show("没有找到该模块的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }          
            }
            else
            {
                checkBox1.Enabled = false;
                this.Text = "数据修改日志 [没有找到相关信息]";
                MessageBox.Show("没有找到该模块的历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            checkBox1.Checked = false;
            beginSearch = true;
        }

        /// <summary>
        /// 搜索全部历史记录信息
        /// </summary>
        private void SearchAllData()
        {
            beginSearch = false;
            DateTime startTime = dateTimePicker1.Value;
            DateTime endTime = dateTimePicker2.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            /* remoting
            string sql = string.Format("SELECT * FROM {0} WHERE edittime >= \'{1}\' AND edittime <= \'{2}\'", 
                "sys_dataRecord", newStartTime, newEndTime);
            dataTable = Helper.GetDataTable(sql, conn);
            */

            string filter = string.Format("edittime >= \'{0}\' AND edittime <= \'{1}\'", newStartTime, newEndTime);
            dataTable = Helper.GetDataTableProxy("sys_dataRecord", filter, null, conn);

            dataGridView1.Rows.Clear(); // 清空原显示数据
            int index = 0;
            foreach (DataRow r in dataTable.Rows)
            {
                dataGridView1.Rows.Add();
                dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                if (r["action"].ToString() == "修改值")
                {
                    dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                }
                dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                index++;
            }

            if (dataGridView1.Rows.Count > 0) // 查询结果不为空
            {
                FillHostNames();
                FillModelNames();
                FillTabNames("所有模块");
                FillRecordNames();
                FillFieldNames();
                comboBoxEx8.Text = "所有分类";
                allUserName = true;
                allModelName = true;
                allTabName = true;
                allNodeName = true;
                allCatName = true;
                allFieldName = true;
                checkBox1.Enabled = true;

                this.Text = string.Format("数据修改日志 [共找到{0}条相关信息]", index);
            }
            else
            {
                checkBox1.Enabled = false;
                this.Text = "数据修改日志 [没有找到相关信息]";
                MessageBox.Show("没有找到历史修改信息!", "数据查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            checkBox1.Checked = false;
            beginSearch = true;
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }           
                /* remoting
                modelTable = Helper.GetDataTable("SELECT id, modelname_cn FROM sys_modl_def", conn);
                tabTable = Helper.GetDataTable("SELECT modtabid, modid, name FROM sys_modl_tab_def", conn);
                 */

                modelTable = Helper.GetDataTableProxy("sys_modl_def", conn);
                tabTable = Helper.GetDataTableProxy("sys_modl_tab_def", conn);


                FillSelectionNames(fieldID);
                SearchFieldData(); // 搜索当前字段的历史修改数据
                comboBoxEx5.SelectedIndex = 0;
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取记录修改信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 获取树结点的主键字符串
        /// </summary>
        /// <param name="keys">树结点的主键数组</param>
        /// <returns>树结点的主键字符串</returns>
        private string GetPrimaryKeyString(object[] keys)
        {
            string keyString = "";
            foreach(object o in keys)
            {
                keyString += string.Format("{0},", o);
            }
            return keyString.TrimEnd(',');
        }

        /// <summary>
        /// 检查数据行是否满足查询条件
        /// </summary>
        /// <param name="r">数据行</param>
        /// <returns>是否满足查询条件</returns>
        private bool CheckAvail(DataRow r)
        {
            if(!allUserName) // 检查用户名条件
            {
                string userName = comboBoxEx1.Text;
                if(r["hostname"].ToString() != userName)
                {
                    return false;
                }
            }

            if(!allModelName) // 检查模块名条件
            {
                string modelName = comboBoxEx2.Text;
                if (r["modelid"].ToString() != GetModelID(modelName))
                {
                    return false;
                }
            }

            if(!allTabName) // 检查属性页名条件
            {
                string tabName = comboBoxEx3.Text;
                if(r["tabid"].ToString() != GetTabID(tabName))
                {
                    return false;
                }
            }

            if(!allCatName) // 检查分类条件
            {
                string catName = comboBoxEx8.Text;
                if(r["catfields"].ToString() != catName)
                {
                    return false;
                }
            }

            if(!allNodeName) // 检查树结点名条件
            {
                string nodeName = comboBoxEx4.Text;
                if(r["nodename"].ToString() != nodeName)
                {
                    return false;
                }
            }

            if(!allFieldName) // 检查字段名条件
            {
                string fieldName = comboBoxEx6.Text;
                if(r["fielddisplayname"].ToString() != fieldName)
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 检查数据行是否满足模糊查询条件
        /// </summary>
        /// <param name="r">数据行</param>
        /// <returns>是否满足查询条件</returns>
        private bool CheckRawAvail(DataRow r)
        {
            if (!allUserName) // 检查用户名条件
            {
                string userName = comboBoxEx1.Text;
                if (!r["hostname"].ToString().Contains(userName))
                {
                    return false;
                }
            }

            if (!allModelName) // 检查模块名条件
            {
                string modelName = comboBoxEx2.Text;
                if (!GetModelName(r["modelid"].ToString()).Contains(modelName))
                {
                    return false;
                }
            }

            if (!allTabName) // 检查属性页名条件
            {
                string tabName = comboBoxEx3.Text;
                if (!GetTabName(r["tabid"].ToString()).Contains(tabName))
                {
                    return false;
                }
            }

            if (!allCatName) // 检查分类条件
            {
                string catName = comboBoxEx8.Text;
                if (!r["catfields"].ToString().Contains(catName))
                {
                    return false;
                }
            }

            if (!allNodeName) // 检查树结点名条件
            {
                string nodeName = comboBoxEx4.Text;
                if (!r["nodename"].ToString().Contains(nodeName))
                {
                    return false;
                }
            }

            if (!allFieldName) // 检查字段名条件
            {
                string fieldName = comboBoxEx6.Text;
                if (!r["fielddisplayname"].ToString().Contains(fieldName))
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 重新加载显示的数据
        /// </summary>
        /// <param name="exactSearch">是否精确查找</param>
        private void ReLoadData(bool exactSearch)
        {
            if (!beginSearch) // 还没有开始正式查找
            {
                return;
            }

            dataGridView1.Rows.Clear(); // 清空行数据
            int index = 0;
            foreach(DataRow r in dataTable.Rows)
            {
                bool condition;
                if(exactSearch) // 精确查找
                {
                    condition = CheckAvail(r);
                }
                else // 模糊查找
                {
                    condition = CheckRawAvail(r);
                }
                if(condition)
                {
                    dataGridView1.Rows.Add();
                    dataGridView1.Rows[index].Cells["用户名"].Value = r["hostname"];
                    dataGridView1.Rows[index].Cells["修改时间"].Value = r["edittime"];
                    dataGridView1.Rows[index].Cells["模块名"].Value = GetModelName(r["modelid"]);
                    if (r["action"].ToString() == "修改值")
                    {
                        dataGridView1.Rows[index].Cells["属性页名"].Value = GetTabName(r["tabid"]);
                    }
                    dataGridView1.Rows[index].Cells["所属分类"].Value = r["catfields"];
                    dataGridView1.Rows[index].Cells["树结点名"].Value = r["nodename"];
                    dataGridView1.Rows[index].Cells["字段名"].Value = r["fielddisplayname"];
                    dataGridView1.Rows[index].Cells["旧值"].Value = r["oldvalue"];
                    dataGridView1.Rows[index].Cells["新值"].Value = r["newvalue"];
                    dataGridView1.Rows[index].Cells["备注"].Value = r["remarks"];
                    dataGridView1.Rows[index].Cells["操作"].Value = r["action"];
                    index++;
                }
            }
        }

        /// <summary>
        /// 获取模块名称
        /// </summary>
        /// <param name="modeid">模块id</param>
        /// <returns>模块名称</returns>
        private string GetModelName(object modeid)
        {
            DataRow r = modelTable.Rows.Find(modeid);
            if(r != null)
            {
                return r["modelname_cn"].ToString();
            }
            else
            {
                return "找不到该模块";
            }
        }

        /// <summary>
        /// 获取属性页名称
        /// </summary>
        /// <param name="tabid">属性页id</param>
        /// <returns>属性页名称</returns>
        private string GetTabName(object tabid)
        {
            DataRow r = tabTable.Rows.Find(tabid);
            if(r != null)
            {
                return r["name"].ToString();
            }
            else
            {
                return "找不到该属性页";
            }
        }

        /// <summary>
        /// 填充用户名
        /// </summary>
        private void FillHostNames()
        {
            comboBoxEx1.Items.Clear();
            comboBoxEx1.Items.Add("所有用户");
            foreach(DataRow r in dataTable.Rows)
            {
                object userName = r["hostname"];
                if(!comboBoxEx1.Items.Contains(userName))
                {
                    comboBoxEx1.Items.Add(userName);
                }
            }
            comboBoxEx1.SelectedIndex = 0;
        }

        /// <summary>
        /// 填充模块名
        /// </summary>
        private void FillModelNames()
        {
            comboBoxEx2.Items.Clear();
            comboBoxEx2.Items.Add("所有模块");
            foreach(DataRow r in dataTable.Rows)
            {
                object modelName = GetModelName(r["modelid"]);
                if(!comboBoxEx2.Items.Contains(modelName))
                {
                    comboBoxEx2.Items.Add(modelName);
                }
            }
            comboBoxEx2.SelectedIndex = 0;
        }

        /// <summary>
        /// 填充字段名
        /// </summary>
        private void FillFieldNames()
        {
            comboBoxEx6.Items.Clear();
            comboBoxEx6.Items.Add("所有字段");
            foreach (DataRow r in dataTable.Rows)
            {
                object fieldName = r["fielddisplayname"];
                if (!comboBoxEx6.Items.Contains(fieldName))
                {
                    comboBoxEx6.Items.Add(fieldName);
                }
            }
            comboBoxEx6.SelectedIndex = 0;
        }

        /// <summary>
        /// 获取模块的id
        /// </summary>
        /// <param name="modelName">模块的名称</param>
        /// <returns>模块的id</returns>
        private string GetModelID(string modelName)
        {
            DataRow[] modelRows = modelTable.Select(string.Format("modelname_cn = \'{0}\'", modelName));
            if (modelRows.Length > 0) // 找到模块的id
            {
                return modelRows[0]["id"].ToString();
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// 获取属性页的id
        /// </summary>
        /// <param name="tabName">属性页的名称</param>
        /// <returns>属性页的id</returns>
        private string GetTabID(string tabName)
        {
            DataRow[] tabRows = tabTable.Select(string.Format("name = \'{0}\'", tabName));
            if (tabRows.Length > 0) // 找到模块的id
            {
                return tabRows[0]["modtabid"].ToString();
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// 填充属性页名
        /// </summary>
        private void FillTabNames(string modelName)
        {
            comboBoxEx3.Items.Clear();
            comboBoxEx3.Items.Add("所有属性页");

            foreach (DataRow r in dataTable.Rows)
            {
                object tabName = GetTabName(r["tabid"]);
                if (!comboBoxEx3.Items.Contains(tabName))
                {
                    comboBoxEx3.Items.Add(tabName);
                }
            }
            comboBoxEx3.SelectedIndex = 0;
        }

        /// <summary>
        /// 填充树结点名
        /// </summary>
        private void FillRecordNames()
        {
            comboBoxEx4.Items.Clear();
            comboBoxEx4.Items.Add("所有树结点");
            foreach(DataRow r in dataTable.Rows)
            {
                object recordName = r["nodename"];
                if(!comboBoxEx4.Items.Contains(recordName))
                {
                    comboBoxEx4.Items.Add(recordName);
                }
            }
            comboBoxEx4.SelectedIndex = 0;
        }

        /// <summary>
        /// 重设分类名称
        /// </summary>
        public void ResetCatName(string catName)
        {
            comboBoxEx8.Text = catName;
            allCatName = (comboBoxEx8.Text == "所有分类");
            ReLoadData(true);
        }

        /// <summary>
        /// 用户选择分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx8_MouseClick(object sender, MouseEventArgs e)
        {
            Point location = this.Location + (Size)groupPanel1.Location + (new Size(0, 30)) + (Size)groupPanel2.Location + 
                (Size)comboBoxEx8.Location + comboBoxEx8.Size;
            if(location.Y + 220 > Screen.PrimaryScreen.Bounds.Height) // 超出屏幕大小
            {
                location.Y = location.Y - 200;
            }
            
            cForm.SetParameters(location, comboBoxEx8.Text, this);
            foreach(DataRow r in dataTable.Rows)
            {
                cForm.AddData(r["catfields"].ToString());
            }
            cForm.InitData();
            cForm.Visible = true;
        }

        /// <summary>
        /// 用户选择新的模块名
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx2_SelectedIndexChanged(object sender, EventArgs e)
        {
            allModelName = (comboBoxEx2.SelectedIndex == 0);
            ReLoadData(true);
            FillTabNames(comboBoxEx2.Text);
        }

        /// <summary>
        /// 用户选择查询
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                switch(comboBoxEx5.SelectedIndex)
                {
                    case 0: // 查询当前字段历史修改信息
                        {
                            SearchFieldData();
                            break;
                        }
                    case 1: // 查询当前记录历史修改信息
                        {
                            SearchRecordData();
                            break;
                        }
                    case 2: // 查询当前属性页历史修改信息
                        {
                            SearchTabData();
                            break;
                        }
                    case 3: // 查询当前模块历史修改信息
                        {
                            SearchModelData();
                            break;
                        }
                    case 4: // 查询所有历史修改信息
                        {
                            SearchAllData();
                            break;
                        }
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在查询历史记录时产生异常: " + ex.ToString());
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
        /// 用户选择新的用户名
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            allUserName = (comboBoxEx1.SelectedIndex == 0);
            ReLoadData(true);
        }

        /// <summary>
        /// 用户选择新的树结点名
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx4_SelectedIndexChanged(object sender, EventArgs e)
        {
            allNodeName = (comboBoxEx4.SelectedIndex == 0);
            ReLoadData(true);
        }

        /// <summary>
        /// 用户选择关闭
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// 用户选择新的属性页名
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx3_SelectedIndexChanged(object sender, EventArgs e)
        {
            allTabName = (comboBoxEx3.SelectedIndex == 0);
            ReLoadData(true);
        }

        /// <summary>
        /// 用户选择模糊过滤
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            ReLoadData(false);
        }

        /// <summary>
        /// 用户名条件的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_TextChanged(object sender, EventArgs e)
        {
            allUserName = (comboBoxEx1.Text == "所有用户");
        }

        /// <summary>
        /// 模块名条件的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx2_TextChanged(object sender, EventArgs e)
        {
            allModelName = (comboBoxEx2.Text == "所有模块");
        }

        /// <summary>
        /// 属性页名条件的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx3_TextChanged(object sender, EventArgs e)
        {
            allTabName = (comboBoxEx3.Text == "所有属性页");
        }

        /// <summary>
        /// 分类条件的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx8_TextChanged(object sender, EventArgs e)
        {
            allCatName = (comboBoxEx8.Text == "所有分类");
        }

        /// <summary>
        /// 树结点名条件的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx4_TextChanged(object sender, EventArgs e)
        {
            allNodeName = (comboBoxEx4.Text == "所有树结点");
        }

        /// <summary>
        /// 用户选择新的字段名
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx6_SelectedIndexChanged(object sender, EventArgs e)
        {
            allFieldName = (comboBoxEx6.SelectedIndex == 0);
            ReLoadData(true);
        }

        /// <summary>
        /// 字段名的文本改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx6_TextChanged(object sender, EventArgs e)
        {
            allFieldName = (comboBoxEx6.Text == "所有字段");
        }

        /// <summary>
        /// 用户选择过滤查询结果
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if(checkBox1.Checked == true)
            {
                groupPanel2.Enabled = true;
            }
            else
            {
                groupPanel2.Enabled = false;
            }
        }

        /// <summary>
        ///  用户选择复制旧值
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            try // 当用户未选中有效单元格时，SelectedCells也有可能不为空，容易引发异常
            {
                if (dataGridView1.SelectedCells != null)
                {
                    DataGridViewRow currentRow = dataGridView1.SelectedCells[0].OwningRow;
                    object data = currentRow.Cells["旧值"].Value;
                    object fieldDisplayName = currentRow.Cells["字段名"].Value;
                    Clipboard.SetDataObject(data, true);
                    MessageBox.Show(string.Format("{0} 的旧值 {1} 已经放入剪贴板!", fieldDisplayName, data), "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("未选择要复制到剪贴板的数据!", "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            catch (Exception ex)
            {
                ex.ToString();
                MessageBox.Show("选择的数据无效!", "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// 用户选择复制新值
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX5_Click(object sender, EventArgs e)
        {
            try // 当用户未选中有效单元格时，SelectedCells也有可能不为空，容易引发异常
            {
                if (dataGridView1.SelectedCells != null)
                {
                    DataGridViewRow currentRow = dataGridView1.SelectedCells[0].OwningRow;
                    object data = currentRow.Cells["新值"].Value;
                    object fieldDisplayName = currentRow.Cells["字段名"].Value;
                    Clipboard.SetDataObject(data, true);
                    MessageBox.Show(string.Format("{0} 的新值 {1} 已经放入剪贴板!", fieldDisplayName, data), "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("未选择要复制到剪贴板的数据!", "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            catch (Exception ex)
            {
                ex.ToString();
                MessageBox.Show("选择的数据无效!", "复制数据", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            } 
        }

        private void btnExport_Click(object sender, EventArgs e)
        {
            if (dataGridView1.Rows.Count == 0)
                return;
            SaveFileDialog dlgSaveFile = new SaveFileDialog();
            dlgSaveFile.Filter = "Tab Documents (*.tab) | *.tab | All Files | *.*";
            dlgSaveFile.DefaultExt = "tab";
            if (dlgSaveFile.ShowDialog() == DialogResult.OK)
            {
                string FileFullPath = dlgSaveFile.FileName;
                datagridviewExport(dataGridView1, FileFullPath);
            }
            
        }

        /// <summary>
        /// 把datagridview的内容导出
        /// 中间加的是\t每行后加\r\n
        /// </summary>
        /// <param name="dgv"></param>
        /// <param name="FileFullPath">文件全路径名</param>
        /// <returns></returns>
        private bool datagridviewExport(DataGridView dgv, string FileFullPath)
        {
            string strExamPath = FileFullPath.Substring(0, FileFullPath.LastIndexOf('\\'));
            if (!System.IO.Directory.Exists(strExamPath))
            {
                MessageBox.Show(FileFullPath, "目录错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            StringBuilder FileContent = new StringBuilder();

            foreach (DataGridViewColumn dgvc in dataGridView1.Columns)
            {
                FileContent.Append(dgvc.HeaderText);
                FileContent.Append('\t');
            }
            FileContent.Remove(FileContent.Length - 1, 1);
            FileContent.Append("\r\n");

            foreach (DataGridViewRow dgvr in dataGridView1.Rows)
            {
                foreach (DataGridViewCell dgvcell in dgvr.Cells)
                {                   
                    FileContent.Append(dgvcell.Value.ToString());
                    FileContent.Append('\t');
                }
                FileContent.Remove(FileContent.Length - 1, 1);
                FileContent.Append("\r\n");
            }
            try
            {
                FileFolderHelper.StringToFile(FileContent.ToString(), FileFullPath);
                return true;
            }
            catch (Exception ex)
            {
                string s = string.Format("导出 tab 文件:\r\n    {0}\r\n时出现错误。请检查文件名，磁盘空间等。\r\n\r\n错误讯息为:{1}", FileFullPath, ex.Message);
                System.Windows.Forms.MessageBox.Show(s, "Error");
                return false;
            } 
        }
    }
}