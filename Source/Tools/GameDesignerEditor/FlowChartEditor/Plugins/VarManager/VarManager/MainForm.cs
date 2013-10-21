using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

namespace VarManager
{
    public partial class MainForm : Office2007Form
    {
        SqlConnection conn; // sql连接
        private DataTable table; // 数据表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public MainForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 读取数据表数据
            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT * FROM VarData";
                table = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取变量数据时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            // 构造数据面板
            DataGridViewCheckBoxColumn checkColumn = new DataGridViewCheckBoxColumn();
            checkColumn.Name = "checkColumn";
            checkColumn.HeaderText = "编辑";
            DataGridViewTextBoxColumn idColumn = new DataGridViewTextBoxColumn();
            idColumn.Name = "idColumn";
            idColumn.HeaderText = "变量ID";
            idColumn.ReadOnly = true;
            DataGridViewTextBoxColumn nameColumn = new DataGridViewTextBoxColumn();
            nameColumn.Name = "nameColumn";
            nameColumn.HeaderText = "变量名称";
            nameColumn.ReadOnly = true;
            DataGridViewTextBoxColumn bindTypeColumn = new DataGridViewTextBoxColumn();
            bindTypeColumn.Name = "bindTypeColumn";
            bindTypeColumn.HeaderText = "绑定类型";
            bindTypeColumn.ReadOnly = true;
            DataGridViewTextBoxColumn bindObjectColumn = new DataGridViewTextBoxColumn();
            bindObjectColumn.Name = "bindObjectColumn";
            bindObjectColumn.HeaderText = "绑定对象";
            bindObjectColumn.ReadOnly = true;
            DataGridViewTextBoxColumn typeColumn = new DataGridViewTextBoxColumn();
            typeColumn.Name = "typeColumn";
            typeColumn.HeaderText = "变量类型";
            typeColumn.ReadOnly = true;
            DataGridViewTextBoxColumn userColumn = new DataGridViewTextBoxColumn();
            userColumn.Name = "userColumn";
            userColumn.HeaderText = "创建者";
            userColumn.ReadOnly = true;
            DataGridViewTextBoxColumn descriptionColumn = new DataGridViewTextBoxColumn();
            descriptionColumn.Name = "descriptionColumn";
            descriptionColumn.HeaderText = "变量描述";
            descriptionColumn.ReadOnly = true;

            dataGridViewX1.Columns.Add(checkColumn);
            dataGridViewX1.Columns.Add(idColumn);
            dataGridViewX1.Columns.Add(nameColumn);
            dataGridViewX1.Columns.Add(bindTypeColumn);
            dataGridViewX1.Columns.Add(bindObjectColumn);
            dataGridViewX1.Columns.Add(typeColumn);
            dataGridViewX1.Columns.Add(userColumn);
            dataGridViewX1.Columns.Add(descriptionColumn);

            // 添加行数据
            int index = 0;
            foreach(DataRow row in table.Rows)
            {
                dataGridViewX1.Rows.Add(1);
                DataGridViewRow newRow = dataGridViewX1.Rows[index];
                newRow.Cells["idColumn"].Value = row["ID"];
                newRow.Cells["nameColumn"].Value = row["Name"];
                newRow.Cells["bindTypeColumn"].Value = row["BindType"];
                newRow.Cells["bindObjectColumn"].Value = row["BindObject"];
                newRow.Cells["typeColumn"].Value = row["Type"];
                newRow.Cells["userColumn"].Value = row["UserName"];
                newRow.Cells["descriptionColumn"].Value = row["Description"];
                index++;
            }

            // 清空当前用户上的锁
            ClearLock();
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        /// <summary>
        /// 获取可用变量的偏移
        /// </summary>
        /// <param name="varLength">变量的长度</param>
        /// <param name="bindType">绑定类型</param>
        /// <param name="bindID">绑定对象的ID</param>
        /// <returns>可用变量的偏移</returns>
        private int GetVarOffset(int varLength, string bindType, string bindID)
        {
            int varOffset = -1;
            int head = 0;
            int tail = head + varLength - 1;
            int maxLength = -1;

            switch(bindType)
            {
                case "场景":
                    {
                        maxLength = 1023;
                        break;
                    }
                case "玩家":
                    {
                        maxLength = 127;
                        break;
                    }
            }

            List<int[]> list = new List<int[]>();
            foreach(DataRow row in table.Rows)
            {
                if(row["BindType"].ToString() == bindType && row["BindID"].ToString() == bindID)
                {
                    int startIndex = (int)row["StartIndex"];
                    int length = (int)row["Length"];
                    int[] border = new int[] { startIndex, startIndex + length - 1 };
                    list.Add(border);
                }                
            }

            while(tail <= maxLength)
            {
                bool find = true;
                int newIndex = tail;

                foreach(int[] border in list)
                {
                    int startIndex = border[0];
                    int endIndex = border[1];

                    if((startIndex >= head && startIndex <= tail) ||
                        (endIndex >= head && endIndex <= tail)) // 检查两个变量的区间是否有交集
                    {
                        find = false;
                        newIndex = endIndex + 1;
                        break;
                    }
                }

                if(find)
                {
                    break;
                }
                else
                {
                    head = newIndex;
                    tail = head + varLength - 1;
                }
            }

            if(tail <= maxLength) // 找到可分配的空间
            {
                varOffset = head;
            }

            return varOffset;
        }

        /// <summary>
        /// 锁定要绑定变量的对象
        /// </summary>
        /// <param name="bindType">绑定类型</param>
        /// <param name="bindID">绑定对象的ID</param>
        /// <param name="userName">当前用户名</param>
        /// <returns>是否锁定成功</returns>
        private bool LockVarData(string bindType, string bindID, string userName)
        {
            bool success = false;

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT ID FROM VarLock WHERE BindType = '{0}' AND BindID = '{1}'",
                    bindType, bindID);
                DataTable lockTable = GetDataTable(sqlString);
                if(lockTable.Rows.Count == 0) // 没有人锁定当前对象，开始加锁
                {
                    sqlString = string.Format("INSERT INTO VarLock (BindType, BindID, LockUser) VALUES ('{0}', '{1}', '{2}')",
                        bindType, bindID, userName);
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                    success = true;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在锁定变量时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return success;
        }

        /// <summary>
        /// 解锁绑定的对象
        /// </summary>
        /// <param name="bindType">绑定的类型</param>
        /// <param name="bindID">绑定对象的ID</param>
        /// <param name="userName">当前用户名</param>
        /// <returns>是否解锁成功</returns>
        private bool UnlockVarData(string bindType, string bindID, string userName)
        {
            bool success = false;

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("DELETE FROM VarLock WHERE BindType = '{0}' AND BindID = {1} AND LockUser = '{2}'",
                    bindType, bindID, userName);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
                success = true;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在解锁变量时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return success;
        }

        /// <summary>
        /// 清空当前用户上的所有锁
        /// </summary>
        private void ClearLock()
        {
            string userName = System.Net.Dns.GetHostName();

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("DELETE FROM VarLock WHERE LockUser = '{0}'", userName);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
            	MessageBox.Show("在解除变量锁时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 刷新数据表格
        /// </summary>
        /// <param name="userName">当前用户名</param>
        /// <param name="varType">变量类型</param>
        /// <param name="bindType">绑定类型</param>
        /// <param name="bindObject">绑定对象</param>
        private void RefreshDataGridView(string userName, string varType, string bindType, string bindObject)
        {
            List<string> conditionList = new List<string>();

            if(userName != "所有用户")
            {
                conditionList.Add(string.Format("UserName = '{0}'", userName));                
            }

            if(varType != "所有变量类型")
            {
                conditionList.Add(string.Format("Type = '{0}'", varType));
            }

            if(bindType != "所有绑定类型")
            {
                conditionList.Add(string.Format("BindType = '{0}'", bindType));
            }

            if(bindObject != "所有绑定对象")
            {
                conditionList.Add(string.Format("BindObject = '{0}'", bindObject));
            }

            dataGridViewX1.Rows.Clear();
            int index = 0;
            if(conditionList.Count > 0)
            {
                string conditionString = "";
                foreach(string s in conditionList)
                {
                    conditionString += s + " AND ";
                }
                conditionString = conditionString.Substring(0, conditionString.Length - 5);   
                DataRow[] rows = table.Select(conditionString);
                foreach(DataRow row in rows)
                {
                    dataGridViewX1.Rows.Add(1);
                    DataGridViewRow newRow = dataGridViewX1.Rows[index];
                    newRow.Cells["idColumn"].Value = row["ID"];
                    newRow.Cells["nameColumn"].Value = row["Name"];
                    newRow.Cells["bindTypeColumn"].Value = row["BindType"];
                    newRow.Cells["bindObjectColumn"].Value = row["BindObject"];
                    newRow.Cells["typeColumn"].Value = row["Type"];
                    newRow.Cells["userColumn"].Value = row["UserName"];
                    newRow.Cells["descriptionColumn"].Value = row["Description"];
                    index++;
                }
            }
            else
            {
                foreach(DataRow row in table.Rows)
                {
                    dataGridViewX1.Rows.Add(1);
                    DataGridViewRow newRow = dataGridViewX1.Rows[index];
                    newRow.Cells["idColumn"].Value = row["ID"];
                    newRow.Cells["nameColumn"].Value = row["Name"];
                    newRow.Cells["bindTypeColumn"].Value = row["BindType"];
                    newRow.Cells["bindObjectColumn"].Value = row["BindObject"];
                    newRow.Cells["typeColumn"].Value = row["Type"];
                    newRow.Cells["userColumn"].Value = row["UserName"];
                    newRow.Cells["descriptionColumn"].Value = row["Description"];
                    index++;
                }
            }            
        }

        /// <summary>
        /// 查看变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bView_Click(object sender, EventArgs e)
        {
            ViewForm vForm = new ViewForm(table);
            DialogResult result = vForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                Hashtable viewData = vForm.ViewData;
                string userName = viewData["userName"] as string;
                string varType = viewData["type"] as string;
                string bindType = viewData["bindType"] as string;
                string bindObject = viewData["bindObject"] as string;

                RefreshDataGridView(userName, varType, bindType, bindObject);
            }
        }

        /// <summary>
        /// 添加变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAdd_Click(object sender, EventArgs e)
        {
            AddForm addForm = new AddForm(conn);
            DialogResult result = addForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                Hashtable newVarData = addForm.NewVarData;
                string name = newVarData["name"] as string;
                string userName = newVarData["userName"] as string;
                string varType = newVarData["type"] as string;
                string desctiption = newVarData["description"] as string;
                string bindType = newVarData["bindType"] as string;
                string bindObject = newVarData["bindObject"] as string;
                string bindID = newVarData["bindID"] as string;
                string stringLength = newVarData["stringLength"] as string;

                
                // 锁定绑定的对象
                bool lockSuccess = LockVarData(bindType, bindID, userName);
                if(lockSuccess) // 锁定成功
                {
                    string sqlString = "SELECT * FROM VarData ORDER BY StartIndex";
                    try
                    {
                        if (conn.State == ConnectionState.Closed)
                        {
                            conn.Open();
                        }

                        // 刷新数据表里的数据
                        table = GetDataTable(sqlString);
                    }
                    catch (SqlException ex)
                    {
                        MessageBox.Show("在读取变量信息时产生sql异常：" + ex.ToString());
                    }
                    finally
                    {
                        if (conn.State == ConnectionState.Open)
                        {
                            conn.Close();
                        }
                    }

                    // 计算出新加变量的偏移和长度
                    int varLength = 0;
                    switch (varType)
                    {
                        case "布尔":
                            {
                                varLength = 1;
                                break;
                            }
                        case "8位无符号整型":
                            {
                                varLength = 1;
                                break;
                            }
                        case "16位无符号整型":
                            {
                                varLength = 2;
                                break;
                            }
                        case "32位无符号整型":
                            {
                                varLength = 4;
                                break;
                            }
                        case "8位有符号整型":
                            {
                                varLength = 1;
                                break;
                            }
                        case "16位有符号整型":
                            {
                                varLength = 2;
                                break;
                            }
                        case "32位有符号整型":
                            {
                                varLength = 4;
                                break;
                            }
                        case "字符串":
                            {
                                varLength = int.Parse(stringLength) * 2;
                                break;
                            }
                    }

                    int varOffset = GetVarOffset(varLength, bindType, bindID);
                    if (varOffset >= 0) // 找到可分配的空间
                    {
                        // 更新数据库
                        try
                        {
                            if (conn.State == ConnectionState.Closed)
                            {
                                conn.Open();
                            }

                            sqlString = string.Format("INSERT INTO VarData (Name, UserName, Type, Description, StartIndex, Length, Version, BindID, BindType, BindObject) VALUES ('{0}', '{1}', '{2}', '{3}', '{4}', '{5}', '{6}', '{7}', '{8}', '{9}')",
                                name, userName, varType, desctiption, varOffset, varLength, 1, bindID, bindType, bindObject);
                            SqlCommand cmd = conn.CreateCommand();
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();
                            sqlString = "SELECT * FROM VarData";
                            table = GetDataTable(sqlString);
                        }
                        catch (SqlException ex)
                        {
                            MessageBox.Show("在更新变量信息时产生sql异常：" + ex.ToString());
                        }
                        finally
                        {
                            if (conn.State == ConnectionState.Open)
                            {
                                conn.Close();
                            }
                        }

                        // 解除绑定
                        UnlockVarData(bindType, bindID, userName);

                        // 刷新数据
                        RefreshDataGridView("所有用户", "所有变量类型", "所有绑定类型", "所有绑定对象");
                    }
                    else
                    {
                        MessageBox.Show("自定义数据块已满，无法分配空间给当前变量！", "变量管理",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else
                {
                    MessageBox.Show("当前绑定对象已经被锁定，请等待解锁后再申请新建变量！", "变量管理",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
        }

        /// <summary>
        /// 删除变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDelete_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("确定要删除选中的变量吗？", "变量管理",
                MessageBoxButtons.YesNo, MessageBoxIcon.Information);
            if(result == DialogResult.Yes)
            {
                List<string> idList = new List<string>();
                foreach(DataGridViewRow row in dataGridViewX1.Rows)
                {
                    if((bool)row.Cells["checkColumn"].EditedFormattedValue)
                    {
                        idList.Add(row.Cells["idColumn"].EditedFormattedValue as string);
                    }
                }

                try
                {
                    if(conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    string sqlString;
                    SqlCommand cmd = conn.CreateCommand();

                    foreach (string s in idList)
                    {
                        sqlString = string.Format("DELETE FROM VarData WHERE ID = '{0}'", s);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }

                    
                    sqlString = "SELECT * FROM VarData";
                    table = GetDataTable(sqlString);                    
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("在删除变量时产生sql异常：" + ex.ToString());
                }
                finally
                {
                    if(conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }

                // 刷新数据
                RefreshDataGridView("所有用户", "所有变量类型", "所有绑定类型", "所有绑定对象");
            }
        }

        /// <summary>
        /// 修改变量
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bChange_Click(object sender, EventArgs e)
        {
            DataGridViewRow editRow = null;

            foreach(DataGridViewRow row in dataGridViewX1.Rows)
            {
                if((bool)row.Cells["checkColumn"].EditedFormattedValue)
                {
                    editRow = row;
                    break;
                }
            }

            if(editRow != null)
            {
                DataRow[] rows = table.Select(string.Format("ID = '{0}'", 
                    editRow.Cells["idColumn"].EditedFormattedValue));
                EditForm eForm = new EditForm(rows[0]);
                DialogResult result = eForm.ShowDialog();
                if(result == DialogResult.OK)
                {
                    Hashtable editData = eForm.EditData;
                    string id = editData["id"] as string;
                    string name = editData["name"] as string;
                    string description = editData["description"] as string;
                    string startIndex = editData["startIndex"] as string;
                    string length = editData["length"] as string;

                    try
                    {
                        if(conn.State == ConnectionState.Closed)
                        {
                            conn.Open();
                        }

                        string sqlString = string.Format("UPDATE VarData SET Name = '{0}', Description = '{1}', StartIndex = '{2}', Length = '{3}' WHERE ID = '{4}'",
                            name, description, startIndex, length, id);
                        SqlCommand cmd = conn.CreateCommand();
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                        sqlString = "SELECT * FROM VarData";
                        table = GetDataTable(sqlString);
                    }
                    catch (SqlException ex)
                    {
                        MessageBox.Show("在更新变量信息时产生sql异常：" + ex.ToString());
                    }
                    finally
                    {
                        if(conn.State == ConnectionState.Open)
                        {
                            conn.Close();
                        }
                    }

                    // 刷新数据
                    RefreshDataGridView("所有用户", "所有变量类型", "所有绑定类型", "所有绑定对象");
                }
            }
            else
            {
                MessageBox.Show("请先选中要修改的数据行！", "变量管理",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 查看变量分配情况
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bViewSpace_Click(object sender, EventArgs e)
        {
            SpaceForm sForm = new SpaceForm(table);
            sForm.ShowDialog();
        }
    }
}