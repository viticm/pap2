using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace ArgumentsEditor
{
    public partial class EditForm : Form
    {
        private SqlConnection conn; // 数据库连接
        private string oldValue; // 旧值
        private string argumentsValue; // 参数值字符串
        private bool beginEdit = false; // 是否已经开始编辑

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">数据库连接</param>
        /// <param name="oldValue">旧值</param>
        public EditForm(SqlConnection conn, string oldValue)
        {
            this.conn = conn;
            this.oldValue = oldValue;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 参数值字符串
        /// </summary>
        public string ArgumentsValue
        {
            get
            {
                return argumentsValue;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 生成选项            
            List<string> list = new List<string>();
            list.Add("无");
            list.Add("数字");
            list.Add("表字段");

            // 读取旧值
            string[] data = new string[]{"无", "无", "无"};
            if (oldValue != null)
            {
                string[] tempData = oldValue.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                for(int i = 0; i < tempData.Length && i < 3; i++)
                {
                    data[i] = tempData[i];
                }
            }

            comboBoxEx1.Text = data[0];
            comboBoxEx2.Text = data[1];
            comboBoxEx3.Text = data[2];
            beginEdit = true;
        }        

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(comboBoxEx1.Text == "" || comboBoxEx2.Text == "" || comboBoxEx3.Text == "")
            {
                MessageBox.Show("有参数没有正确的设置类型！", "参数设置",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                bool previousNone = false;
                bool valid = true;
                argumentsValue = "";
                List<string> list = new List<string>();
                list.Add(comboBoxEx1.Text);
                list.Add(comboBoxEx2.Text);
                list.Add(comboBoxEx3.Text);

                for (int i = 0; i < 3; i++)
                {
                    string value = list[i];
                    if (value == "无")
                    {
                        previousNone = true;
                    }
                    else
                    {
                        if (previousNone)
                        {
                            MessageBox.Show("有参数没有正确的设置类型！", "参数设置",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                            valid = false;
                        }
                        else
                        {
                            previousNone = false;
                        }
                    }
                    argumentsValue += string.Format("{0};", value);
                }
                argumentsValue = argumentsValue.TrimEnd(new char[] { ';' });

                if (valid)
                {
                    DialogResult = DialogResult.OK;
                    this.Close();
                }
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
        /// 参数类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (beginEdit)
            {
                if (comboBoxEx1.SelectedIndex == 2)
                {
                    FieldSelectForm sForm = new FieldSelectForm(conn);
                    DialogResult result = sForm.ShowDialog();
                    if (result == DialogResult.OK)
                    {
                        beginEdit = false;
                        comboBoxEx1.Items[2] = sForm.Value;
                        beginEdit = true;
                    }
                }
            }
        }

        /// <summary>
        /// 参数类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx2_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (beginEdit)
            {
                if (comboBoxEx2.SelectedIndex == 2)
                {
                    FieldSelectForm sForm = new FieldSelectForm(conn);
                    DialogResult result = sForm.ShowDialog();
                    if (result == DialogResult.OK)
                    {
                        beginEdit = false;
                        comboBoxEx2.Items[2] = sForm.Value;
                        beginEdit = true;
                    }
                }
            }
        }

        /// <summary>
        /// 参数类型改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx3_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (beginEdit)
            {
                if (comboBoxEx3.SelectedIndex == 2)
                {
                    FieldSelectForm sForm = new FieldSelectForm(conn);
                    DialogResult result = sForm.ShowDialog();
                    if (result == DialogResult.OK)
                    {
                        beginEdit = false;
                        comboBoxEx3.Items[2] = sForm.Value;
                        beginEdit = true;
                    }
                }
            }
        }     
    }
}