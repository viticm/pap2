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
    public partial class EnumValueSelector : Form
    {
        private SqlConnection conn; // sql连接
        private string enumType; // 枚举类型
        private string oldValue; // 旧值
        private int enumValueIndex; // 枚举值的序号
        private string enumValueString; // 枚举值的字符串

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="aiChartID">AI图ID</param>
        /// <param name="oldValue">旧值</param>
        public EnumValueSelector(SqlConnection conn, string enumType, string oldValue)
        {
            this.conn = conn;
            this.enumType = enumType;
            this.oldValue = oldValue;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 枚举值的序号
        /// </summary>
        public int EnumValueIndex
        {
            get
            {
                return enumValueIndex;
            }
        }

        /// <summary>
        /// 枚举值的字符串
        /// </summary>
        public string EnumValueString
        {
            get
            {
                return enumValueString;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {            
            // 初始化行数据
            DataTable table = null;

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM AI_Enum_Define WHERE id = '{0}'", enumType);
                table = GetDataTable(sqlString);               
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取AI模板参数时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            if(table.Rows.Count > 0)
            {
                DataRow row = table.Rows[0];
                string chineseName = row["name"].ToString();
                string englishName = row["ename"].ToString();
                string enumValues = row["values"].ToString();

                string[] datas = enumValues.Split(new char[] { '\n' });
                
                for(int i = 0; i < datas.Length; i++)
                {
                    string s = datas[i];
                    string[] values = s.TrimEnd(new char[] { ',' }).Split(new char[] { ',' });
                    comboBoxEx1.Items.Add(string.Format("{0} ({1})", values[0], values[1]));
                }              

                if(oldValue != null && oldValue != "")
                {
                    comboBoxEx1.SelectedIndex = int.Parse(oldValue);
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
            enumValueString = comboBoxEx1.Text;
            enumValueIndex = comboBoxEx1.SelectedIndex;

            if(enumValueIndex == -1)
            {
                MessageBox.Show("参数不能为空！", "选择枚举值", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
            else
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
        }
    }
}