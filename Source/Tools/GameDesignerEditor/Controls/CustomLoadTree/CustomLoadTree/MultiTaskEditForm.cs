using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace CustomLoadTree
{
    public partial class MultiTaskEditForm : Form
    {
        private SqlConnection conn; // sql连接
        private List<string> taskIDList; // 任务id链表
        private List<string> fieldList; // 字段链表
        private DataTable taskTable; // 任务数据表
        private bool selectAll = true; // 是否全部选中
        private bool beginEdit = false; // 是否已经开始编辑
        private Font boldFont; // 粗体
        private Font regularFont; // 常体

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="taskTable">任务数据表</param>
        /// <param name="taskIDList">任务id链表</param>
        /// <param name="fieldList">字段链表</param>
        public MultiTaskEditForm(SqlConnection conn, DataTable taskTable, List<string> taskIDList, List<string> fieldList)
        {
            this.conn = conn;
            this.taskTable = taskTable;
            this.taskIDList = taskIDList;
            this.fieldList = fieldList;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            // 添加列数据
            DataGridViewColumn selectAllColumn = new DataGridViewCheckBoxColumn();
            selectAllColumn.HeaderText = "是否保存";
            selectAllColumn.Name = "SelectAllColumn";
            dataGridView1.Columns.Add(selectAllColumn);

            DataGridViewColumn tempColumn;

            int index = 1;
            foreach(string s in fieldList)
            {
                dataGridView1.Columns.Add(s, s);
                tempColumn = dataGridView1.Columns[index];
                tempColumn.ValueType = taskTable.Columns[s].DataType;
                index++;
            }

            // 添加行数据
            DataRow row; // 当前数据行
            DataGridViewRow gridViewRow; // gridView数据行
            int rowIndex = 0; // 数据行序号
            foreach(string s in taskIDList)
            {
                row = taskTable.Rows.Find(s);
                if(row != null)
                {
                    dataGridView1.Rows.Add();
                    gridViewRow = dataGridView1.Rows[rowIndex];
                    foreach(string field in fieldList)
                    {
                        gridViewRow.Cells[field].Value = row[field];
                    }
                    gridViewRow.HeaderCell.Value = string.Format("{0} ({1})", row["QuestName"], row["QuestID"]);
                    gridViewRow.HeaderCell.Tag = row["QuestID"];
                    rowIndex++;
                }
            }

            // 初始化字体
            boldFont = new Font(dataGridView1.DefaultCellStyle.Font, FontStyle.Bold);
            regularFont = new Font(dataGridView1.DefaultCellStyle.Font, FontStyle.Regular);

            // 开始编辑
            beginEdit = true;
        }

        /// <summary>
        /// 获取字段对应的tab页id
        /// </summary>
        /// <param name="field">字段名</param>
        /// <returns>保存字段对应的信息的数据行数组</returns>
        private DataRow[] GetFieldInfo(string field)
        {
            DataRow[] rows = null;

            try
            {
                string sqlString = string.Format("SELECT fieldid, fieldcnname, modtabid FROM {0} WHERE fieldname = '{1}'",
                    "sys_meta_info", field);
                DataTable table = Helper.GetDataTable(sqlString, conn);
                if(table.Rows.Count > 0)
                {
                    rows = new DataRow[table.Rows.Count];
                    for (int i = 0; i < table.Rows.Count; i++)
                    {
                        rows[i] = table.Rows[i];
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取tab页的id值时产生sql异常: " + ex.ToString());
            }

            return rows;
        }

        /// <summary>
        /// 将数值变化写入数据库
        /// </summary>
        /// <param name="id">任务id</param>
        /// <param name="field">任务字段</param>
        /// <param name="value">任务新值</param>
        private void UpdateData(string id, string field, string newValue, string oldValue, string strRemarks)
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                // 修改字段信息
                string sqlString = string.Format("UPDATE {0} SET {1} = '{2}' WHERE QuestID = '{3}'", "tbl_quests", field, newValue, id);
                if(newValue == "") // 修正数字空值保存的问题
                {
                    sqlString = string.Format("UPDATE {0} SET {1} = NULL WHERE QuestID = '{2}'", "tbl_quests", field, id);
                }
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                // 记录字段修改历史
                DataRow[] fieldInfo = GetFieldInfo(field);
                if(fieldInfo != null) // 找到对应的字段信息数据行
                {
                    DataRow taskRow = taskTable.Rows.Find(id);
                    
                    int modelID = 63;                
                    string userName = Helper.GetHostName();
                    string key = id;
                    string nodeName = taskRow["QuestName"].ToString();
                    string catFields = taskRow["_Cat"].ToString();
                    DateTime time = DateTime.Now;
                     
                    foreach(DataRow r in fieldInfo) // 有些字段可能在多个tab页中出现，所以要多保存一份字段修改历史。
                    {
                        int tabID = (int)r["modtabid"];
                        int fieldID = (int)r["fieldid"];
                        string fieldName = r["fieldcnname"].ToString();
                        sqlString = string.Format("INSERT INTO {0} (hostname, edittime, keys, oldvalue, newvalue, modelid, tabid, nodename, " +
                                                "catfields, fielddisplayname, fieldid, remarks) VALUES ('{1}', '{2}', '{3}', '{4}', '{5}', '{6}', '{7}', '{8}', '{9}', '{10}', '{11}', '{12}')",
                                                "sys_dataRecord", userName, time, key, oldValue, newValue, modelID, tabID, nodeName, catFields, fieldName, fieldID, strRemarks);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show(string.Format("在更新id为{0}的任务的{1}字段数值时产生sql异常: " + ex.ToString(), id, field));
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
        /// 保存数据
        /// </summary>
        private void SaveData(string strRemarks)
        {
            string id; // 任务的id
            DataRow row; // 数据行
            string newValue; // 新值
            int editTaskCount = 0; // 更新的任务数
            bool valueChanged = false; // 数据行的值是否已改变

            foreach(DataGridViewRow r in dataGridView1.Rows)
            {
                if((bool)r.Cells["selectAllColumn"].EditedFormattedValue) // 该列被选中
                {
                    valueChanged = false;
                    id = r.HeaderCell.Tag.ToString();
                    row = taskTable.Rows.Find(id);
                    
                    if (row != null)
                    {
                        foreach (string s in fieldList)
                        {
                            newValue = r.Cells[s].EditedFormattedValue.ToString();
                            if (newValue != row[s].ToString())
                            {
                                UpdateData(id, s, newValue, row[s].ToString(), strRemarks);
                                r.Cells[s].Style.Font = regularFont; // 重设字体
                                // 更新数值
                                if (newValue == "") // 空值需要特殊处理
                                {
                                    row[s] = DBNull.Value;
                                } 
                                else
                                {
                                    row[s] = r.Cells[s].EditedFormattedValue;
                                }
                                 
                                valueChanged = true;
                            }
                        }

                        if(valueChanged) // 该数据行的值已经改变
                        {
                            editTaskCount++;
                        }
                    }
                }
                
            }

            MessageBox.Show(string.Format("共有{0}个任务的数据被更新!", editTaskCount.ToString()), "数据更新成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /// <summary>
        /// 全部选中或取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bSelectAll_Click(object sender, EventArgs e)
        {
            if(selectAll)
            {
                foreach(DataGridViewRow r in dataGridView1.Rows)
                {
                    r.Cells["SelectAllColumn"].Value = selectAll;
                }
                bSelectAll.Text = "全部取消";
                selectAll = false;
            }
            else
            {
                foreach (DataGridViewRow r in dataGridView1.Rows)
                {
                    r.Cells["SelectAllColumn"].Value = selectAll;
                }
                bSelectAll.Text = "全部选中";
                selectAll = true;
            }
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bSave_Click(object sender, EventArgs e)
        {
            if (HasChanged())
            {
                string strRemarks = "未添加备注";
                if (Class1.CommentOnSave)
                {
                    GameDesingerTools.InputBoxForm ibf = new GameDesingerTools.InputBoxForm();
                    if (ibf.ShowDialog() == DialogResult.OK)
                    {
                        strRemarks = ibf.Remarks;
                    }
                    else
                    {
                        return;
                    }                   
                }
                SaveData(strRemarks);
            }
            else
            {
                MessageBox.Show("选中的行没有内容被改变。");
            }
            
        }

        /// <summary>
        /// 单元格值改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void dataGridView1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (beginEdit)
            {
                dataGridView1[e.ColumnIndex, e.RowIndex].Style.Font = boldFont;
                if (dataGridView1.SelectedCells.Count > 1)
                {
                    foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
                    {
                        cell.Value = dataGridView1[e.ColumnIndex, e.RowIndex].Value;
                    }
                }
            }
            
        }

        private bool HasChanged()
        {
            string id; // 任务的id
            DataRow row; // 数据行
            string newValue; // 新值
            
            foreach (DataGridViewRow r in dataGridView1.Rows)
            {
                if ((bool)r.Cells["selectAllColumn"].EditedFormattedValue) // 该列被选中
                {   
                    id = r.HeaderCell.Tag.ToString();
                    row = taskTable.Rows.Find(id);

                    if (row != null)
                    {
                        foreach (string s in fieldList)
                        {
                            newValue = r.Cells[s].EditedFormattedValue.ToString();
                            if (newValue != row[s].ToString())
                            {
                                return true;                                
                            }
                        }
                    }
                }
            }

            return false;
        }

    }
}