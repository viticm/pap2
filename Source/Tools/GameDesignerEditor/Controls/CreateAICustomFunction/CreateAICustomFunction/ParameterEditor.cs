using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace CreateAICustomFunction
{
    public partial class ParameterEditor : Form
    {
        private SqlConnection conn; // sql连接
        private string oldValue; // 旧值
        private string newValue; // 新值
        private Hashtable typeTable = new Hashtable(); // 值类型哈希表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="oldValue">旧值</param>
        public ParameterEditor(SqlConnection conn, string oldValue)
        {
            this.conn = conn;
            this.oldValue = oldValue;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 旧值
        /// </summary>
        public string OldValue
        {
            set
            {
                oldValue = value;
            }
        }

        /// <summary>
        /// 新值
        /// </summary>
        public string NewValue
        {
            get
            {
                return newValue;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 添加数据列
            DataGridViewTextBoxColumn indexColumn = new DataGridViewTextBoxColumn();
            indexColumn.Name = "Index";
            indexColumn.HeaderText = "参数编号";
            indexColumn.ReadOnly = true;
            DataGridViewTextBoxColumn nameColumn = new DataGridViewTextBoxColumn();
            nameColumn.Name = "Name";
            nameColumn.HeaderText = "参数名";
            DataGridViewComboBoxColumn typeColumn = new DataGridViewComboBoxColumn();
            typeColumn.Name = "Type";
            typeColumn.HeaderText = "参数类型";
            DataGridViewTextBoxColumn defaultValueColumn = new DataGridViewTextBoxColumn();
            defaultValueColumn.Name = "DefaultValue";
            defaultValueColumn.HeaderText = "默认值";

            dataGridViewX1.Columns.Add(indexColumn);
            dataGridViewX1.Columns.Add(nameColumn);
            dataGridViewX1.Columns.Add(typeColumn);
            dataGridViewX1.Columns.Add(defaultValueColumn);

            // 初始化行数据
            DataTable enumTable = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM AI_Enum_Define");
                enumTable = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取AI枚举数据表时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            // 填充值类型
            typeColumn.Items.Add("无");
            typeColumn.Items.Add("数字");
            typeColumn.Items.Add("字符串");
            typeTable["数字"] = "-1";
            typeTable["字符串"] = "-2";

            foreach(DataRow row in enumTable.Rows)
            {
                string typeName = row["name"].ToString();
                string enumID = row["id"].ToString();
                typeColumn.Items.Add(typeName);
                typeTable[typeName] = enumID;
            }

            if (oldValue == null)
            {
                oldValue = "";
            }

            // 添加行数据
            string[] valueArray = oldValue.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);

            for (int i = 0; i < 5; i++)
            {
                dataGridViewX1.Rows.Add(1);
                DataGridViewRow newRow = dataGridViewX1.Rows[i];
                int valueIndex = i + 1;
                newRow.Cells["Index"].Value = valueIndex.ToString();

                if (i < valueArray.Length)
                {
                    string[] enumData = valueArray[i].TrimEnd(new char[] { '\r' }).Split(new char[] { ',' });                    
                    newRow.Cells["Name"].Value = enumData[0];
                    string enumType = enumData[1];
                    DataGridViewComboBoxCell typeCell = newRow.Cells["Type"] as DataGridViewComboBoxCell;

                    switch (enumType)
                    {
                        case "-1": // 数字
                            {
                                typeCell.Value = "数字";
                                break;
                            }
                        case "-2": // 字符串
                            {
                                typeCell.Value = "字符串";
                                break;
                            }
                        default:
                            {
                                typeCell.Value = enumType;
                                break;
                            }
                    }

                    if (enumData.Length > 2) // 有默认值
                    {
                        newRow.Cells["DefaultValue"].Value = enumData[2];
                    }
                    else
                    {
                        newRow.Cells["DefaultValue"].Value = "0";
                    }
                }
                else
                {
                    newRow.Cells["Type"].Value = "无";
                }
            }                         
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
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string enumValue = "";
            bool editSuccess = true;

            for (int i = 0; i < 5; i++)
            {
                DataGridViewRow row = dataGridViewX1.Rows[i];
                if(row.Cells["Type"].Value.ToString() != "无")
                {
                    string argumentName = ""; 
                    string argumentType = "";
                    string argumentDefaultValue = "";

                    if (row.Cells["Name"].EditedFormattedValue != null)
                    {
                        argumentName = row.Cells["Name"].Value.ToString();
                    }
                    
                    if (row.Cells["Type"].EditedFormattedValue != null)
                    {
                        argumentType = row.Cells["Type"].Value.ToString();
                    }

                    if (row.Cells["DefaultValue"].EditedFormattedValue != null)
                    {
                        argumentDefaultValue = row.Cells["DefaultValue"].Value.ToString();
                    }

                    if (argumentName == "" || argumentType == "" || argumentDefaultValue == "")
                    {
                        MessageBox.Show("参数没有输入完全！", "设置参数", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        editSuccess = false;
                        break;
                    }

                    enumValue += string.Format("{0},{1},{2}\r\n", argumentName, typeTable[argumentType] as string,
                        argumentDefaultValue);
                }
                else
                {
                    break;
                }
            }

            if (editSuccess)
            {
                newValue = enumValue;
                DialogResult = DialogResult.OK;
                this.Close();
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
    }
}