using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.Net;
using TableSelector;

namespace VarManager
{
    public partial class AddForm : Form
    {
        private SqlConnection conn; // sql连接
        private Hashtable newVarData = new Hashtable(); // 新增变量的数据 
         
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public AddForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 新增变量的数据
        /// </summary>
        public Hashtable NewVarData
        {
            get
            {
                return newVarData;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            
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
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string bindType = comboBoxEx2.Text;
            string bindObject = textBoxX3.Text;
            string varType = comboBoxEx1.Text.Replace(" ", "");
            string name = textBoxX1.Text;
            string description = textBoxX2.Text;
            string bindID = textBoxX3.Tag as string;
            string stringLength = integerInput1.Text; 

            // 自动填充bindID值
            switch(bindType)
            {
                case "玩家":
                    {
                        bindID = "0";
                        break;
                    }
            }

            if(bindType == "" || (bindType == "场景" && bindObject == "") ||
                varType == "" || name == "" || (varType == "字符串" && integerInput1.Text == ""))
            {
                MessageBox.Show("输入信息不完整！", "添加变量",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                newVarData["name"] = name;
                newVarData["userName"] = Dns.GetHostName();
                newVarData["type"] = varType;
                newVarData["description"] = description;
                newVarData["bindType"] = bindType;
                newVarData["bindObject"] = bindObject;
                newVarData["bindID"] = bindID;
                newVarData["stringLength"] = stringLength;

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
        /// 选择绑定对象
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">时间参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            string bindType = comboBoxEx2.Text;
            string id = null;
            string value = null;

            switch(bindType)
            {
                case "场景":
                    {
                        TableSelector.Program tableSelector = new TableSelector.Program();
                        bool selectFinished = tableSelector.Show(conn, "场景选择器", "Select Name, ID FROM MapList", "Name", "ID", textBoxX3.Text);
                        if(selectFinished)
                        {
                            id = tableSelector.SelectedValue;
                            value = string.Format("{0} ({1})", tableSelector.SelectedValues["Name"], tableSelector.SelectedValues["ID"]);
                        }

                        break;
                    }
                case "玩家":
                    {
                        MessageBox.Show("默认为当前玩家，不需要选择！", "添加变量",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        break;
                    }
                case "":
                    {
                        MessageBox.Show("请先选择绑定类型！", "添加变量",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        break;
                    }
            }

            textBoxX3.Text = value;
            textBoxX3.Tag = id;
        }

        /// <summary>
        /// 绑定类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx2_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch(comboBoxEx2.Text)
            {
                case "玩家":
                    {
                        textBoxX3.Text = "当前玩家";
                        break;
                    }
                default:
                    {
                        textBoxX3.Text = "";
                        break;
                    }
            }
        }

        /// <summary>
        /// 变量类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch(comboBoxEx1.Text)
            {
                case "字符串":
                    {
                        integerInput1.Enabled = true;
                        break;
                    }
                default:
                    {
                        integerInput1.Enabled = false;
                        break;
                    }
            }
        }
    }
}