using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DevComponents.Editors;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using TableSelector;

namespace ArgumentsEditor
{
    public partial class FunctionEditForm : Form
    {
        private SqlConnection conn; // sql连接
        private string value; // 函数表达式值
        private DataTable functionTable; // 函数表
        private DataTable classTable; // 分类表
        private List<IntegerInput> inputControlList = new List<IntegerInput>(); // 数字输入控件链表
        private List<ButtonX> buttonList = new List<ButtonX>(); // 选择按钮链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public FunctionEditForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 函数表达式值
        /// </summary>
        public string Value
        {
            get
            {
                return value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化控件链表
            inputControlList.Add(integerInput1);
            inputControlList.Add(integerInput2);
            inputControlList.Add(integerInput3);

            buttonList.Add(buttonX3);
            buttonList.Add(buttonX4);
            buttonList.Add(buttonX5);

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM expression_functions");
                functionTable = GetDataTable(sqlString);
                sqlString = string.Format("SELECT Class FROM expression_functions GROUP BY Class");
                classTable = GetDataTable(sqlString);
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取表达式数据时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            // 增加分类
            foreach(DataRow row in classTable.Rows)
            {
                Node classNode = new Node();
                classNode.Text = row["Class"].ToString();
                advTree1.Nodes.Add(classNode);
            }
            
            // 增加子结点            
            foreach(DataRow row in functionTable.Rows)
            {
                Node classNode = null;
                string id = row["ID"].ToString();
                string functionClass = row["Class"].ToString();
                string englishName = row["EnglishName"].ToString();
                string arguments = row["Arguments"].ToString();

                if(id != "0")
                {
                    foreach (Node node in advTree1.Nodes)
                    {
                        if (node.Text == functionClass)
                        {
                            classNode = node;
                        }
                    }

                    if (classNode != null)
                    {
                        Node newNode = new Node();
                        newNode.Text = englishName;
                        newNode.Tag = arguments;
                        classNode.Nodes.Add(newNode);
                    }
                }                
            }

            foreach(Node node in advTree1.Nodes)
            {
                node.Expand();
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable table = ds.Tables[0];            

            return table;
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        private void RefreshData()
        {
            string arguments = advTree1.SelectedNode.Tag as string;            

            if(arguments != "")
            {
                string[] data = arguments.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                for(int i = 0; i < 3; i++)
                {
                    switch(data[i])
                    {
                        case "无":
                            {
                                inputControlList[i].Enabled = false;
                                buttonList[i].Enabled = false;
                                break;
                            }
                        case "数字":
                            {
                                buttonList[i].Enabled = false;
                                break;
                            }
                        default:
                            {
                                break;
                            }
                    }
                }
            }
            else
            {
                foreach(IntegerInput i in inputControlList)
                {
                    i.Enabled = false;
                }

                foreach(ButtonX b in buttonList)
                {
                    b.Enabled = false;
                }
            }
        }

        /// <summary>
        /// 通过关键字过滤函数
        /// </summary>
        /// <param name="content">关键字</param>
        private void FiltFunctions(string content)
        {
            advTree1.Nodes.Clear();
            content = content.ToLower();

            if(content == "") // 显示全部结点
            {
                // 增加分类
                foreach (DataRow row in classTable.Rows)
                {
                    Node classNode = new Node();
                    classNode.Text = row["Class"].ToString();
                    advTree1.Nodes.Add(classNode);
                }

                // 增加子结点            
                foreach (DataRow row in functionTable.Rows)
                {
                    Node classNode = null;
                    string id = row["ID"].ToString();
                    string functionClass = row["Class"].ToString();
                    string englishName = row["EnglishName"].ToString();
                    string arguments = row["Arguments"].ToString();

                    if(id != "0")
                    {
                        foreach (Node node in advTree1.Nodes)
                        {
                            if (node.Text == functionClass)
                            {
                                classNode = node;
                            }
                        }

                        if (classNode != null)
                        {
                            Node newNode = new Node();
                            newNode.Text = englishName;
                            newNode.Tag = arguments;
                            classNode.Nodes.Add(newNode);
                        }
                    }                    
                }
            }
            else
            {
                // 增加子结点            
                foreach (DataRow row in functionTable.Rows)
                {
                    Node classNode = null;
                    string id = row["ID"].ToString();
                    string functionClass = row["Class"].ToString();
                    string englishName = row["EnglishName"].ToString();
                    string arguments = row["Arguments"].ToString();                    

                    if(id != "0")
                    {
                        if (englishName.ToLower().Contains(content))
                        {
                            foreach (Node node in advTree1.Nodes)
                            {
                                if (node.Text == functionClass)
                                {
                                    classNode = node;
                                }
                            }

                            if (classNode != null)
                            {
                                Node newNode = new Node();
                                newNode.Text = englishName;
                                newNode.Tag = arguments;
                                classNode.Nodes.Add(newNode);
                            }
                            else
                            {
                                classNode = new Node();
                                classNode.Text = functionClass;
                                Node newNode = new Node();
                                newNode.Text = englishName;
                                newNode.Tag = arguments;
                                classNode.Nodes.Add(newNode);
                                advTree1.Nodes.Add(classNode);
                            }
                        }
                    }                                        
                }
            }

            foreach (Node node in advTree1.Nodes)
            {
                node.Expand();
            }
        }

        /// <summary>
        /// 选择数据表中的字段
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void SelectTableField(object sender, EventArgs e)
        {
            ButtonX button = sender as ButtonX;
            int index = int.Parse(button.Tag.ToString());

            string oldValue = inputControlList[index].Value.ToString();
            string arguments = advTree1.SelectedNode.Tag as string;
            string[] data = arguments.Split(new char[] { ';' });
            string functionInfo = data[index];
            string[] information = functionInfo.Split(new char[] { '[' });
            string tableName = information[0];
            string fieldInfo = information[1].TrimEnd(new char[] { ']' });
            string[] fields = fieldInfo.Split(new char[] { ',' });
            string valueField = fields[fields.Length - 1];
            string catString = "";
            for(int i = 0; i < fields.Length - 1; i++)
            {
                catString += string.Format("{0},", fields[i]);
            }
            catString = catString.TrimEnd(new char[] { ',' });

            TableSelector.Program tableSelector = new TableSelector.Program();
            bool editSuccess = tableSelector.Show(conn, "参数值选择", string.Format("SELECT * FROM {0}", tableName), catString, valueField, oldValue);
            if(editSuccess)
            {
                int intValue;
                if(int.TryParse(tableSelector.SelectedValue, out intValue))
                {
                    inputControlList[index].Value = intValue;
                }                
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            Node node = advTree1.SelectedNode;

            if(node != null && node.Level == 1)
            {
                string arguments = "";
                for (int i = 0; i < 3; i++)
                {
                    IntegerInput input = inputControlList[i];
                    if (input.Enabled)
                    {
                        arguments += string.Format("{0},", input.Value);
                    }
                }
                arguments = arguments.TrimEnd(new char[] { ',' });
                string functionName = node.Text;
                value = string.Format("{0}({1})", functionName, arguments);
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("请先选择要编辑的函数！", "函数参数编辑",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
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

        /// <summary>
        /// 选择树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_Click(object sender, EventArgs e)
        {
            Node node = advTree1.SelectedNode;
            if(node != null)
            {
                if(node.Level == 1)
                {
                    foreach (IntegerInput i in inputControlList)
                    {
                        i.Enabled = true;
                    }

                    foreach (ButtonX b in buttonList)
                    {
                        b.Enabled = true;
                    }

                    RefreshData();
                }
                else
                {
                    foreach(IntegerInput i in inputControlList)
                    {
                        i.Enabled = false;
                    }

                    foreach(ButtonX b in buttonList)
                    {
                        b.Enabled = false;
                    }
                }
            }
        }

        /// <summary>
        /// 关键字参数改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            FiltFunctions(textBoxX1.Text);
        }
    }
}