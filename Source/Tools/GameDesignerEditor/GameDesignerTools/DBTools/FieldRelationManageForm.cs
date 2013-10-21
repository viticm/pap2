using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

using DevComponents.AdvTree;

namespace GameDesingerTools
{
    public partial class FieldRelationForm : Form
    {
        private SqlConnection conn; // sql连接
        private DataTable metaDataTable;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public FieldRelationForm(SqlConnection conn)
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
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            //DataTable relationTable = Helper.GetDataTable("SELECT * FROM sys_field_relation ORDER BY TableName, KeyField", conn);

            DataTable relationTable = Helper.GetDataTableProxy("sys_field_relation", null, "TableName, KeyField", conn);
            metaDataTable = Helper.GetDataTableProxy("sys_meta_info", "parentfield IS NULL OR parentfield = 0", null, conn);
            
            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            // 初始化控件
            foreach (DataRow dataRow in relationTable.Rows)
            {
                string tableName = dataRow["TableName"].ToString();
                string keyField = dataRow["KeyField"].ToString();
                string relationData = dataRow["RelationData"].ToString();

                Node tableNode = null;

                foreach (Node node in keyFieldTree.Nodes)
                {
                    if (node.Text == tableName)
                    {
                        tableNode = node;
                        break;
                    }
                }

                if (tableNode == null)
                {
                    tableNode = new Node();
                    tableNode.Text = tableName;
                    tableNode.Tag = tableName;
                    keyFieldTree.Nodes.Add(tableNode);
                }

                Node fieldNode = new Node();
                fieldNode.Text = GetFieldDisplayName(tableName, keyField);
                fieldNode.Tag = relationData;
                tableNode.Nodes.Add(fieldNode);
            }

            // 展开所有结点
            foreach(Node tableNode in keyFieldTree.Nodes)
            {
                tableNode.Expand();                
            }

            WindowState = FormWindowState.Maximized;
        }

        /// <summary>
        /// 执行sql命令
        /// </summary>
        /// <param name="sqlString">sql命令</param>
        private void ExecuteSqlCommand(string sqlString, string tableName)
        {
            Helper.ExecuteCommand(tableName, sqlString, conn);
        }

        /// <summary>
        /// 获取字段显示名
        /// </summary>
        /// <param name="fieldName">字段名</param>
        /// <returns>显示名</returns>
        private string GetFieldDisplayName(string tableName, string fieldName)
        {
            string displayName = fieldName;

            DataRow[] rows = metaDataTable.Select(string.Format("tablename = '{0}' AND fieldname = '{1}'",
                                                                tableName, fieldName));
            if (rows.Length > 0)
            {
                string chineseName = rows[0]["fieldcnname"].ToString().Trim();

                if (!string.IsNullOrEmpty(chineseName))
                {
                    displayName = string.Format("{0} ({1})", chineseName, fieldName);
                }
            }

            return displayName;
        }

        /// <summary>
        /// 获取字段名
        /// </summary>
        /// <param name="displayText">显示名</param>
        /// <returns>字段名</returns>
        private string GetFieldName(string displayText)
        {            
            string fieldName = displayText;
            int index = displayText.LastIndexOf("(");

            if (index >= 0)
            {
                fieldName = displayText.Substring(index + 1, displayText.Length - index - 2);
            }             

            return fieldName;
        }

        /// <summary>
        /// 获取关联关系数据
        /// </summary>
        /// <returns>关联关系数据</returns>
        private string GetRelationData()
        {
            StringBuilder relationData = new StringBuilder();

            foreach (Node tableNode in relationTree.Nodes)
            {
                relationData.Append(string.Format("{0},", tableNode.Text));
                
                foreach (Node fieldNode in tableNode.Nodes)
                {
                    relationData.Append(string.Format("{0},", GetFieldName(fieldNode.Text)));
                }

                relationData.Remove(relationData.Length - 1, 1);
                relationData.Append(";");
            }

            if (relationData.Length > 0)
            {
                relationData.Remove(relationData.Length - 1, 1);
            }            

            return relationData.ToString();
        }

        /// <summary>
        /// 更新字段关联关系数据
        /// </summary>
        /// <param name="relationData">关联关系数据</param>
        private void UpdateFieldRelationData(string relationData)
        {
            Node fieldNode = keyFieldTree.SelectedNode;
            Node tableNode = fieldNode.Parent;

            string tableName = tableNode.Text;
            string fieldName = GetFieldName(fieldNode.Text);
            string sqlString = string.Format("UPDATE sys_field_relation SET RelationData = '{0}' WHERE TableName = '{1}' AND KeyField = '{2}'",
                                             relationData, tableName, fieldName);

            Helper.ExecuteCommand("sys_field_relation", sqlString, conn);
            //ExecuteSqlCommand(sqlString);

            fieldNode.Tag = relationData;
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void FieldRelationForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }

        /// <summary>
        /// 查找关联关系
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSearch_Click(object sender, EventArgs e)
        {
            string fieldValue = searchFieldBox.Text;

            if (fieldValue != "")
            {
                if (keyFieldTree.SelectedNode != null && keyFieldTree.SelectedNode.Level == 1)
                {
                    Hashtable infoTable = new Hashtable();

                    foreach (Node tableNode in relationTree.Nodes)
                    {
                        List<string> fieldList = new List<string>();

                        foreach (Node fieldNode in tableNode.Nodes)
                        {
                            fieldList.Add(GetFieldName(fieldNode.Text));
                        }

                        infoTable[tableNode.Text] = fieldList;
                    }

                    string displayInfo = string.Format("[{0}] {1}", keyFieldTree.SelectedNode.Parent.Text,
                                                       GetFieldName(keyFieldTree.SelectedNode.Text));

                    FieldRelationSearchForm sForm = new FieldRelationSearchForm(conn, displayInfo, fieldValue, infoTable);
                    sForm.ShowDialog();
                }
                else
                {
                    MessageBox.Show("请先选择要查找的字段！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
            else
            {
                MessageBox.Show("字段值不能为空！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 删除关联关系
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bRemoveRelation_Click(object sender, EventArgs e)
        {
            Node currentNode = relationTree.SelectedNode;

            if (currentNode != null)
            {
                if (MessageBox.Show("确定要删除选中的关联关系吗？", "删除关联关系", MessageBoxButtons.OKCancel,
                                    MessageBoxIcon.Question) == DialogResult.OK)
                {
                    currentNode.Remove();

                    string relationData = GetRelationData();
                    UpdateFieldRelationData(relationData);
                }
            }
            else
            {
                MessageBox.Show("请先选择要删除的关联关系！", "删除关联关系", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 添加关联关系
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddRelation_Click(object sender, EventArgs e)
        {
            Node keyFieldNode = keyFieldTree.SelectedNode;

            if (keyFieldNode != null && keyFieldNode.Level == 1)
            {
                string tableName = "";
                List<string> fieldNameList = new List<string>();
                Node tableNode = relationTree.SelectedNode;

                if (tableNode != null)
                {
                    if (tableNode.Level == 1)
                    {
                        tableNode = tableNode.Parent;
                    }

                    tableName = tableNode.Text;

                    foreach (Node fieldNode in tableNode.Nodes)
                    {
                        fieldNameList.Add(fieldNode.Text);
                    }
                }

                AddFieldRelationForm aForm = new AddFieldRelationForm(conn, tableName, fieldNameList, metaDataTable);

                if (aForm.ShowDialog() == DialogResult.OK)
                {
                    tableNode = null;

                    foreach (Node node in relationTree.Nodes)
                    {
                        if (node.Text == aForm.TableName)
                        {
                            tableNode = node;
                            break;
                        }
                    }

                    if (tableNode == null)
                    {
                        tableNode = new Node();
                        tableNode.Text = aForm.TableName;
                        tableNode.Tag = aForm.TableName;
                        relationTree.Nodes.Add(tableNode);
                    }

                    foreach (string s in aForm.FieldNameList)
                    {
                        bool exist = false;

                        foreach (Node node in tableNode.Nodes)
                        {
                            if (node.Text == s)
                            {
                                exist = true;
                                break;
                            }
                        }

                        if (!exist)
                        {
                            Node fieldNode = new Node();
                            fieldNode.Text = s;
                            fieldNode.Tag = s;
                            tableNode.Nodes.Add(fieldNode);
                        }
                    }

                    tableNode.Expand();

                    string relationData = GetRelationData();
                    UpdateFieldRelationData(relationData);
                }
            }
            else
            {
                MessageBox.Show("请先选择要添加关联关系的字段！", "添加关联关系", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void keyFieldTree_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            relationTree.Nodes.Clear();

            Node currentNode = keyFieldTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                string relationData = currentNode.Tag as string;
                string[] tableInfo = relationData.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                
                foreach (string s in tableInfo)
                {
                    string[] fieldInfo = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                    Node tableNode = new Node();
                    tableNode.Text = fieldInfo[0];
                    tableNode.Tag = fieldInfo[0];
                    relationTree.Nodes.Add(tableNode);

                    for (int i = 1; i < fieldInfo.Length; i++)
                    {
                        Node fieldNode = new Node();
                        fieldNode.Text = GetFieldDisplayName(fieldInfo[0], fieldInfo[i]);
                        fieldNode.Tag = fieldInfo[i];
                        tableNode.Nodes.Add(fieldNode);
                    }

                    tableNode.Expand();
                }
            }
        }

        /// <summary>
        /// 添加关联字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddField_Click(object sender, EventArgs e)
        {
            string tableName = "";
            List<string> fieldNameList = new List<string>();

            Node currentNode = keyFieldTree.SelectedNode;

            if (currentNode != null)
            {
                if (currentNode.Level == 1)
                {
                    tableName = currentNode.Parent.Text;
                }
                else
                {
                    tableName = currentNode.Text;
                }
            }

            AddFieldRelationForm aForm = new AddFieldRelationForm(conn, tableName, fieldNameList, metaDataTable);

            if (aForm.ShowDialog() == DialogResult.OK)
            {
                tableName = aForm.TableName;

                foreach (string fieldName in aForm.FieldNameList)
                {
                    Node tableNode = null;

                    foreach (Node node in keyFieldTree.Nodes)
                    {
                        if (node.Text == tableName)
                        {
                            tableNode = node;
                            break;
                        }
                    }

                    if (tableNode == null)
                    {
                        tableNode = new Node();
                        tableNode.Text = tableName;
                        tableNode.Tag = tableName;
                        keyFieldTree.Nodes.Add(tableNode);
                    }

                    Node fieldNode = null;

                    foreach (Node node in tableNode.Nodes)
                    {
                        if (node.Text == fieldName)
                        {
                            fieldNode = node;
                            break;
                        }
                    }

                    if (fieldNode == null)
                    {
                        string sqlString = string.Format("INSERT INTO sys_field_relation (TableName, KeyField) VALUES ('{0}', '{1}')",
                                                         tableName, GetFieldName(fieldName));
                    	Helper.ExecuteCommand("sys_field_relation", sqlString, conn);
                    	//ExecuteSqlCommand(sqlString);

                        fieldNode = new Node();
                        fieldNode.Text = fieldName;
                        fieldNode.Tag = "";
                        tableNode.Nodes.Add(fieldNode);
                        tableNode.Expand();
                    }
                    else
                    {
                        MessageBox.Show("当前关联字段已经存在！", "添加关联字段", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }                
            }
        }

        /// <summary>
        /// 删除关联字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemoveField_Click(object sender, EventArgs e)
        {
            Node currentNode = keyFieldTree.SelectedNode;

            if (currentNode != null)
            {
                if (MessageBox.Show("确定要删除选中的关联字段吗？", "删除关联字段", 
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                {
                    string sqlString = null;

                    if (currentNode.Level == 1)
                    {
                        string tableName = currentNode.Parent.Text;
                        string fieldName = GetFieldName(currentNode.Text);
                        sqlString = string.Format("DELETE FROM sys_field_relation WHERE TableName = '{0}' AND KeyField = '{1}'",
                                                  tableName, fieldName);
                    }
                    else
                    {
                        string tableName = currentNode.Text;
                        sqlString = string.Format("DELETE FROM sys_field_relation WHERE TableName = '{0}'", tableName);
                    }

                    Helper.ExecuteCommand("sys_field_relation", sqlString, conn);
                    currentNode.Remove();
                }                
            }
            else
            {
                MessageBox.Show("请先选择要删除的关联字段！", "删除关联字段", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 面板大小改变事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void relationContainer_SizeChanged(object sender, EventArgs e)
        {
            int spliterLocation = (int)((relationContainer.Size.Width - 4) / 2);
            relationContainer.SplitterDistance = spliterLocation;
        }
    }
}