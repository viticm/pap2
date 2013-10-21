using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ClientDiff
{
    public partial class KeyConfigForm : Form
    {
        private List<string> keyList;
        private string keyString;
        private string displayField;
        private string rowCondition;

        public KeyConfigForm(List<string> keyList)
        {
            this.keyList = keyList;
            InitializeComponent();
            Init();
        }

        public string KeyString
        {
            get
            {
                return keyString;
            }
        }

        public string DisplayField
        {
            get
            {
                return displayField;
            }
        }

        public string RowCondition
        {
            get
            {
                return rowCondition;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach(string s in keyList)
            {
                comboBoxEx1.Items.Add(s);
                comboBoxEx2.Items.Add(s);
                comboBoxEx3.Items.Add(s);
                comboBoxEx4.Items.Add(s);
                comboBoxEx5.Items.Add(s);
                comboBoxEx6.Items.Add(s);
                comboBoxEx7.Items.Add(s);
                comboBoxEx8.Items.Add(s);
                comboBoxEx9.Items.Add(s);
                comboBoxEx10.Items.Add(s);
            }

            comboBoxEx1.SelectedIndex = 0;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e"事件参数></param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            keyString = "";

            if (comboBoxEx1.Text != "")
            {
                keyString += string.Format("{0},", comboBoxEx1.Text);
            }

            if (comboBoxEx2.Text != "")
            {
                keyString += string.Format("{0},", comboBoxEx2.Text);
            }

            if (comboBoxEx3.Text != "")
            {
                keyString += string.Format("{0},", comboBoxEx3.Text);
            }

            if (comboBoxEx4.Text != "")
            {
                keyString += string.Format("{0},", comboBoxEx4.Text);
            }

            if (comboBoxEx5.Text != "")
            {
                keyString += string.Format("{0},", comboBoxEx5.Text);
            }

            keyString = keyString.TrimEnd(new char[] { ',' });

            displayField = comboBoxEx6.Text;
            if (comboBoxEx7.Text.Length != 0)
                displayField += "," + comboBoxEx7.Text;
            if (comboBoxEx8.Text.Length != 0)
                displayField += "," + comboBoxEx8.Text;
            if (comboBoxEx9.Text.Length != 0)
                displayField += "," + comboBoxEx9.Text;
            if (comboBoxEx10.Text.Length != 0)
                displayField += "," + comboBoxEx10.Text;

            rowCondition = this.textboxRowCondition.Text;

            if (keyString != "" /*&& displayField != ""*/)
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("参数不能为空！", "字段配置", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
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