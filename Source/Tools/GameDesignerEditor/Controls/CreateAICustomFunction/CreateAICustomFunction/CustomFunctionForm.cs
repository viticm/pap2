using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace CreateAICustomFunction
{
    public partial class CustomFunctionForm : Form
    {
        SqlConnection conn; // sql连接
        private string functionName; // 动作名称
        private int functionID; // 动作id

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public CustomFunctionForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            comboBoxEx1.SelectedIndex = 0;
        }

        /// <summary>
        /// 动作名称
        /// </summary>
        public string FunctionName
        {
            get
            {
                return functionName;
            }
        }

        /// <summary>
        /// 动作ID
        /// </summary>
        public int FunctionID
        {
            get
            {
                return functionID;
            }
        }

        /// <summary>
        /// 更新脚本内容
        /// </summary>
        private void UpdateScript()
        {
            string englishName = textBoxX1.Text;
            string chineseName = textBoxX2.Text;
            string arguments = textBoxX5.Text;
            string returnValue = textBoxX6.Text;

            // 去掉返回值中的空白字符串
            if (returnValue.Contains(" "))
            {
                string[] returnData = returnValue.Split(new char[] { ',' });
                returnValue = "";
                
                foreach (string s in returnData)
                {
                    returnValue += string.Format("{0},", returnData[0].Trim());
                }
                returnValue = returnValue.TrimEnd(new char[] { ',' });
            } 

            StringBuilder scriptText = new StringBuilder();
            scriptText.Append(string.Format("-- {0}\r\n", chineseName));
            scriptText.Append(string.Format("-- 参数：{0}\r\n", GetArgumentString(arguments)));
            scriptText.Append(string.Format("-- 返回值：{0}\r\n", returnValue));

            scriptText.Append(string.Format("function _{0}(char, p1, p2, p3, p4, p5)\r\n\r\n", englishName));
            scriptText.Append("end\r\n");

            codeEditBox.Text = scriptText.ToString();
        }

        /// <summary>
        /// 检查方法名是否冲突
        /// </summary>
        /// <param name="functionName">方法名</param>
        /// <returns>是否冲突</returns>
        private bool CheckFunctionConflict(string functionName)
        {
            bool conflict = false;

            string sqlString = "SELECT ename FROM AI_Action_Define";
            DataTable functionTable = GetDataTable(sqlString, conn);

            DataRow[] rows = functionTable.Select(string.Format("ename = '{0}'", functionName));
            if (rows.Length > 0)
            {
                conflict = true;
            }

            return conflict;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        public DataTable GetDataTable(string sqlString, SqlConnection conn)
        {
            DataTable tb = null;
            ConnectionState state = conn.State;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                
                SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                tb = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                MessageBox.Show("读取数据表时产生sql异常：" + ex.ToString(), "读取数据表", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return tb;
        }

        /// <summary>
        /// 获取参数字符串
        /// </summary>
        /// <param name="arguments">参数值</param>
        /// <returns>参数字符串</returns>
        private string GetArgumentString(string arguments)
        {
            string argumentString = "";

            string[] lines = arguments.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            foreach(string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });
                string[] data = line.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                argumentString += string.Format("{0}, ", data[0]);
            }
            argumentString = argumentString.TrimEnd(new char[] { ',', ' ' });

            return argumentString;
        }


        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string englishName = textBoxX1.Text.Replace("'", "''");
            string chineseName = textBoxX2.Text.Replace("'", "''");
            string arguments = textBoxX5.Text.Replace("'", "''");
            string returnValue = textBoxX6.Text.Replace("'", "''");
            string description = textBoxX3.Text.Replace("'", "''");
            string actionType = textBoxX4.Text.Replace("'", "''");
            string scriptText = codeEditBox.Text.Replace("'", "''");
            string idString = null;
            int id = 1;
            int isSystem = comboBoxEx1.SelectedIndex;
            bool editSuccess = true;

            if (englishName == "")
            {
                MessageBox.Show("英文名不能为空！", "创建自定义动作", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (chineseName == "")
            {
                MessageBox.Show("中文名不能为空！", "创建自定义动作", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (description == "")
            {
                MessageBox.Show("描述不能为空！", "创建自定义动作", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            string[] returnData = returnValue.Split(new char[]{','}, StringSplitOptions.RemoveEmptyEntries);
            if(returnData.Length == 0 || returnData.Length > 3)
            {
                MessageBox.Show("动作返回值无效！", "参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            // 去掉返回值中的空白字符串
            if (returnValue.Contains(" "))
            {
                returnValue = "";
                foreach (string s in returnData)
                {
                    returnValue += string.Format("{0},", returnData[0].Trim());
                }
                returnValue = returnValue.TrimEnd(new char[] { ',' });
            } 

            if (actionType == "")
            {
                actionType = "未分类";
            }

            if (isSystem == 1)
            {
                scriptText = "";
            }

            functionName = chineseName;

            // 更新数据库            
            SqlTransaction transaction = null; // 事务

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                // 分配动作ID
                string sqlString = string.Format("SELECT MAX(id) FROM AI_Action_Define");
                cmd.CommandText = sqlString;
                idString = cmd.ExecuteScalar().ToString();
                id = int.Parse(idString);
                id++;
                functionID = id + 1000;

                // 插入记录
                sqlString = string.Format("INSERT INTO AI_Action_Define (id, actionid, issystem, name, ename, info, parms, rettype, script, class) VALUES ('{0}', '{1}', {2}, '{3}', '{4}', '{5}', '{6}', '{7}', '{8}', '{9}')",
                        id.ToString(), functionID.ToString(), isSystem.ToString(), chineseName, englishName, description, arguments, returnValue, scriptText, actionType);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();    
                            
                transaction.Commit(); // 提交事务 
                functionID = id;                               
            }
            catch (SqlException ex)
            {
                if (transaction != null) // 回滚事务
                {
                    transaction.Rollback();
                }
                MessageBox.Show("创建自定义动作产生Sql异常：" + ex.ToString(), "新建绘图", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
                editSuccess = false;
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            if (editSuccess)
            {                
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

        /// <summary>
        /// 英文名改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            if (textBoxX1.Text != "")
            {
                UpdateScript();
            }
        }

        /// <summary>
        /// 中文名改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void textBoxX2_TextChanged(object sender, EventArgs e)
        {
            if (textBoxX2.Text != "")
            {
                UpdateScript();
            }
        }

        /// <summary>
        /// 参数改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void textBoxX5_TextChanged(object sender, EventArgs e)
        {
            if (textBoxX5.Text != "")
            {
                UpdateScript();
            }
        }

        /// <summary>
        /// 返回值改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void textBoxX6_TextChanged(object sender, EventArgs e)
        {
            if (textBoxX6.Text != "")
            {
                UpdateScript();
            }
        }

        /// <summary>
        /// 动作类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxEx1.SelectedIndex == 0)
            {
                codeEditBox.Enabled = true;
            }
            else
            {
                codeEditBox.Enabled = false;
            }
        }

        /// <summary>
        /// 编辑动作参数
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            ParameterEditor pForm = new ParameterEditor(conn, textBoxX5.Text);
            if (pForm.ShowDialog() == DialogResult.OK)
            {
                textBoxX5.Text = pForm.NewValue;
            }
        }
    }
}