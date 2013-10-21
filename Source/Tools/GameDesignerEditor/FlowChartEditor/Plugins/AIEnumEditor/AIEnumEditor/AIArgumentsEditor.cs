using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace AIEnumEditor
{
    public partial class AIArgumentsEditor : Form
    {
        private SqlConnection conn; // sql连接
        private string diagramPath; // 流程图路径
        private string oldValue; // 旧值
        private string aiArguments; // AI参数
        private string filtField; // 过滤的字段

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="diagramPath">流程图路径</param>
        /// <param name="oldValue">旧值</param>
        /// <param name="filtField">过滤的字段</param>
        public AIArgumentsEditor(SqlConnection conn, string diagramPath, string oldValue, string filtField)
        {
            this.conn = conn;
            this.diagramPath = diagramPath;
            this.oldValue = oldValue;
            this.filtField = filtField;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// AI参数
        /// </summary>
        public string AIArguments
        {
            get
            {
                return aiArguments;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {           
            // 添加数据列
            DataGridViewTextBoxColumn sortIDColumn = new DataGridViewTextBoxColumn();
            sortIDColumn.Name = "SortID";
            sortIDColumn.HeaderText = "参数编号";
            sortIDColumn.ReadOnly = true;
            DataGridViewTextBoxColumn chineseNameColumn = new DataGridViewTextBoxColumn();
            chineseNameColumn.Name = "ChineseName";
            chineseNameColumn.HeaderText = "中文名";
            chineseNameColumn.ReadOnly = true;
            DataGridViewTextBoxColumn englishNameColumn = new DataGridViewTextBoxColumn();
            englishNameColumn.Name = "EnglishName";
            englishNameColumn.HeaderText = "英文名";
            englishNameColumn.ReadOnly = true;
            DataGridViewTextBoxColumn typeColumn = new DataGridViewTextBoxColumn();
            typeColumn.Name = "Type";
            typeColumn.HeaderText = "类型";
            typeColumn.ReadOnly = true;
            DataGridViewTextBoxColumn textValueColumn = new DataGridViewTextBoxColumn();
            textValueColumn.Name = "TextValue";
            textValueColumn.HeaderText = "参数值";
            DataGridViewComboBoxColumn enumValueColumn = new DataGridViewComboBoxColumn();
            enumValueColumn.Name = "EnumValue";
            enumValueColumn.HeaderText = "参数值";

            dataGridViewX1.Columns.Add(sortIDColumn);
            dataGridViewX1.Columns.Add(chineseNameColumn);
            dataGridViewX1.Columns.Add(englishNameColumn);
            dataGridViewX1.Columns.Add(typeColumn);
            dataGridViewX1.Columns.Add(textValueColumn);
            dataGridViewX1.Columns.Add(enumValueColumn);

            // 初始化过滤字段
            List<string> filtFieldList = new List<string>();
            string[] filtData = filtField.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            filtFieldList.AddRange(filtData);

            // 添加行数据
            DataTable table = null;
            DataTable enumTable = null;
            DataTable diagramTable = null;
            string sqlString = "";
            string relevatePath = diagramPath.Replace("scripts\\ai\\", "");
            relevatePath = relevatePath.Replace(".lua", "");            

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                sqlString = string.Format("SELECT ID, FullPath FROM changan.s3diagram_rtm.dbo.diagram WHERE Class = 'AI图' AND FullPath = '{0}'", relevatePath);
                diagramTable = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取AI图数据时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            if(diagramTable.Rows.Count > 0) // 找到对应的AI脚本
            {
                string diagramID = diagramTable.Rows[0]["ID"].ToString();

                try
                {
                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    sqlString = string.Format("SELECT * FROM AI_TemplateParamTable WHERE DiagramID = '{0}' ORDER BY SortID",
                        diagramID);
                    table = GetDataTable(sqlString);
                    sqlString = "SELECT * FROM AI_Enum_Define";
                    enumTable = GetDataTable(sqlString);
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("在读取AI参数数据时产生sql异常：" + ex.ToString());
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }

                List<string[]> valueList = GetValueList();
                int index = 0;
                bool argumentsChanged = CheckArgumentsChanged(valueList, table);

                foreach(DataRow row in table.Rows)
                {
                    dataGridViewX1.Rows.Add(1);
                    DataGridViewRow newRow = dataGridViewX1.Rows[index];

                    string id = row["KeyID"].ToString();
                    string sortID = row["SortID"].ToString();
                    string chineseName = row["name"].ToString();
                    string englishName = row["ename"].ToString();
                    string type = row["type"].ToString();         
                    string defaultValue = row["defaultvalue"].ToString();

                    newRow.Cells["SortID"].Value = sortID;
                    newRow.Cells["SortID"].Tag = id;
                    newRow.Cells["ChineseName"].Value = chineseName;
                    newRow.Cells["EnglishName"].Value = englishName;                

                    string typeName = "";

                    switch(type)
                    {
                        case "-2": // 字符串
                            {
                                typeName = "字符串";
                                break;
                            }
                        case "-1": // 数字
                            {
                                typeName = "数字";
                                break;
                            }
                        default:
                            {
                                typeName = GetTypeName(type, enumTable);
                                break;
                            }
                    }

                    newRow.Cells["Type"].Value = typeName;
                    newRow.Cells["Type"].Tag = type;                

                    switch(type)
                    {
                        case "-1": // 数字
                            {
                                newRow.Cells["EnumValue"].ReadOnly = true;
                                newRow.Cells["EnumValue"].Style.BackColor = Color.LightGray;

                                break;
                            }
                        case "-2": // 字符串
                            {
                                newRow.Cells["EnumValue"].ReadOnly = true;
                                newRow.Cells["EnumValue"].Style.BackColor = Color.LightGray;

                                break;
                            }
                        default: // 枚举
                            {
                                DataGridViewComboBoxCell comboCell = (DataGridViewComboBoxCell)newRow.Cells["EnumValue"];
                                List<string> enumValues = GetEnumValues(type, enumTable);
                                foreach (string s in enumValues)
                                {
                                    comboCell.Items.Add(s);
                                }
                                newRow.Cells["TextValue"].ReadOnly = true;
                                newRow.Cells["TextValue"].Style.BackColor = Color.LightGray;

                                break;
                            }
                    }

                    if(valueList.Count > 0) // 读到旧值
                    {
                        if(argumentsChanged) // 参数已经改变
                        {
                            foreach(string[] datas in valueList)
                            {
                                if(datas[0] == row["KeyID"].ToString()) // 找到对应的参数
                                {
                                    switch (type)
                                    {
                                        case "-2": // 字符串
                                            {
                                                newRow.Cells["TextValue"].Value = datas[1];

                                                break;
                                            }
                                        case "-1": // 数字
                                            {
                                                newRow.Cells["TextValue"].Value = datas[1];

                                                break;
                                            }
                                        default:
                                            {
                                                DataGridViewComboBoxCell comboCell = (DataGridViewComboBoxCell)newRow.Cells["EnumValue"];
                                                newRow.Cells["EnumValue"].Value = comboCell.Items[int.Parse(datas[1])].ToString();

                                                break;
                                            }
                                    }

                                    break;
                                }
                                else // 没找到对应的参数
                                {
                                    switch (type)
                                    {
                                        case "-2": // 字符串
                                            {
                                                newRow.Cells["TextValue"].Value = defaultValue;

                                                break;
                                            }
                                        case "-1": // 数字
                                            {
                                                newRow.Cells["TextValue"].Value = defaultValue;

                                                break;
                                            }
                                        default:
                                            {
                                                DataGridViewComboBoxCell comboCell = (DataGridViewComboBoxCell)newRow.Cells["EnumValue"];
                                                newRow.Cells["EnumValue"].Value = comboCell.Items[0].ToString();

                                                break;
                                            }
                                    }
                                }
                            }
                        }
                        else
                        {
                            string[] datas = valueList[index];

                            switch (type)
                            {
                                case "-2": // 字符串
                                    {
                                        newRow.Cells["TextValue"].Value = datas[1];

                                        break;
                                    }
                                case "-1": // 数字
                                    {
                                        newRow.Cells["TextValue"].Value = datas[1];

                                        break;
                                    }
                                default:
                                    {
                                        DataGridViewComboBoxCell comboCell = (DataGridViewComboBoxCell)newRow.Cells["EnumValue"];
                                        newRow.Cells["EnumValue"].Value = comboCell.Items[int.Parse(datas[1])].ToString();

                                        break;
                                    }
                            }
                        }                    
                    }
                    else // 旧值为空
                    {
                        switch(type)
                        {
                            case "-2": // 字符串
                                {
                                    newRow.Cells["TextValue"].Value = defaultValue;

                                    break;
                                }
                            case "-1": // 数字
                                {
                                    newRow.Cells["TextValue"].Value = defaultValue;

                                    break;
                                }
                            default:
                                {
                                    DataGridViewComboBoxCell comboCell = (DataGridViewComboBoxCell)newRow.Cells["EnumValue"];
                                    newRow.Cells["EnumValue"].Value = comboCell.Items[0].ToString();

                                    break;
                                }
                        }
                    }

                    if(filtFieldList.Contains(englishName))
                    {
                        newRow.Visible = false;
                    }
                    index++;
                }

                if(argumentsChanged) // 参数发生变化
                {
                    MessageBox.Show("AI模板的参数发生变化，需要重新设置！", "参数编辑", MessageBoxButtons.OK,
                        MessageBoxIcon.Information);
                }
            }                        

            
        }

        /// <summary>
        /// 检查参数是否发生变化
        /// </summary>
        /// <param name="valueList">旧值链表</param>
        /// <param name="table">参数数据表</param>
        /// <returns>是否发生变化</returns>
        private bool CheckArgumentsChanged(List<string[]> valueList, DataTable table)
        {
            bool changed = false;

            if(valueList.Count != table.Rows.Count) // 参数数量不等
            {
                return true;
            }

            for (int i = 0; i < table.Rows.Count; i++)
            {
                DataRow row = table.Rows[i];
                string[] value = valueList[i]; 
                
                if(row["KeyID"].ToString() != value[0])
                {
                    changed = true;
                    break;
                }                
            }

            return changed;
        }

        /// <summary>
        /// 获取枚举的名称
        /// </summary>
        /// <param name="type">枚举类型</param>
        /// <param name="enumTable">枚举数据表</param>
        /// <returns>枚举的名称</returns>
        private string GetTypeName(string type, DataTable enumTable)
        {
            string typeName = "";

            DataRow[] rows = enumTable.Select(string.Format("id = '{0}'", type));
            if(rows.Length > 0)
            {
                DataRow row = rows[0];
                string chineseName = row["name"].ToString();
                string englishName = row["ename"].ToString();
                typeName = string.Format("{0} ({1})", chineseName, englishName);
            }

            return typeName;
        }

        /// <summary>
        /// 获取参数值数据
        /// </summary>
        /// <returns>参数值数据</returns>
        private List<string[]> GetValueList()
        {
            List<string[]> valueList = new List<string[]>();

            string[] values = oldValue.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
            foreach(string s in values)
            {
                string[] datas = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                valueList.Add(datas);
            }

            return valueList;
        }

        /// <summary>
        /// 获取枚举值列表
        /// </summary>
        /// <param name="type">枚举类型</param>
        /// <param name="enumTable">枚举数据表</param>
        /// <returns>枚举值列表</returns>
        private List<string> GetEnumValues(string type, DataTable enumTable)
        {
            List<string> enumValues = new List<string>();

            DataRow[] rows = enumTable.Select(string.Format("id = '{0}'", type));
            if(rows.Length > 0)
            {
                DataRow row = rows[0];                
                string[] values = row["values"].ToString().Split(new char[]{'\n'});
                foreach(string s in values)
                {
                    string value = s.TrimEnd(new char[]{'\r'});
                    string[] datas = value.Split(new char[]{','});
                    enumValues.Add(string.Format("{0} ({1})", datas[0], datas[1]));
                }
            }

            return enumValues;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            DataTable tb = null;
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            tb = ds.Tables[0];

            return tb;
        }

        /// <summary>
        /// 获取枚举下拉框的编号
        /// </summary>
        /// <param name="comboCell">下拉框单元</param>
        /// <param name="enumValue">枚举值</param>
        /// <returns>枚举下拉框的编号</returns>
        private int GetEnumIndex(DataGridViewComboBoxCell comboCell, string enumValue)
        {
            int index = -1;

            for (int i = 0; i < comboCell.Items.Count; i++)
            {
                string value = comboCell.Items[i].ToString();
                if(value == enumValue)
                {
                    index = i;
                    break;
                }
            }

            return index;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string values = "";
            foreach(DataGridViewRow row in dataGridViewX1.Rows)
            {
                string id = row.Cells["SortID"].Tag.ToString();
                string type = row.Cells["Type"].Tag.ToString();
                string textValue = ""; 
                string enumValue = ""; 

                if(row.Cells["TextValue"].EditedFormattedValue != null)
                {
                    textValue = row.Cells["TextValue"].EditedFormattedValue.ToString();
                }

                if(row.Cells["EnumValue"].EditedFormattedValue != null)
                {
                    enumValue = row.Cells["EnumValue"].EditedFormattedValue.ToString();
                }

                switch(type)
                {
                    case "-2": // 字符串
                        {
                            values += string.Format("{0},{1};", id, textValue);
                            break;
                        }
                    case "-1": // 数字
                        {
                            values += string.Format("{0},{1};", id, textValue);
                            break;
                        }
                    default:
                        {
                            values += string.Format("{0},{1};", id, 
                                GetEnumIndex(row.Cells["EnumValue"] as DataGridViewComboBoxCell, enumValue));
                            break;
                        }
                }                
            }

            aiArguments = values.TrimEnd(new char[] { ';' });

            DialogResult = DialogResult.OK;
            this.Close();
        }
        
        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}