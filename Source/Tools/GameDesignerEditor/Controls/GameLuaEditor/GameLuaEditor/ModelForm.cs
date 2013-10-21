using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using VisualEditor;
using DataBaseIO;


namespace GameLuaEditor
{
    public partial class ModelForm : Form
    {
        private string[] data; // 显示数据数组
        private List<string> inputList = new List<string>(); // 用户输入链表
        Doit doit = new Doit(); // 文本编辑对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">显示数据数组</param>
        public ModelForm(string[] data)
        {
            this.data = data;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 用户输入链表属性
        /// </summary>
        public List<string> InputList
        {
            get
            {
                return inputList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            // 调整label的宽度
            int maxLabelLength = 0;
            int length = 0;
            foreach(string s in data)
            {
                string[] t = s.Split(new char[]{','});
                length = 0;
                foreach(char c in t[0].ToCharArray())
                {
                    if (c == 60 || c > 128) // 中文字符
                    {
                        length += 15;
                    }
                    else // 英文字符
                    {
                        length += 8;
                    }
                }

                if(length > maxLabelLength)
                {
                    maxLabelLength = length;
                }
            }

            for(int i = 0; i < data.Length; i++)
            {
                string[] values = data[i].Split(new char[] {','});

                ItemContainer container = new ItemContainer();
                LabelItem label = new LabelItem("labelItem" + i, values[0]);
                label.Width = maxLabelLength;
                TextBoxItem textBox = new TextBoxItem("textBoxItem" + i, values[2]);
                textBox.Tag = values[1];
                textBox.TextBoxWidth = 200;
               
                textBox.TextBox.BorderStyle = BorderStyle.Fixed3D;
                textBox.TextBox.Text = values[2];
                container.SubItems.Add(label);
                container.SubItems.Add(textBox);
                switch(values[1]) // 检查输入的类型
                {
                    case "text": // 文本类型
                        {
                            ButtonItem buttonItem = new ButtonItem("buttonItem" + i, "编辑");
                            buttonItem.Click += new EventHandler(editText);
                            buttonItem.Tag = textBox.TextBox; // 绑定文本框
                            container.SubItems.Add(buttonItem);
                            while (maxLabelLength + 270 + 30 > this.Width) // 自动调整宽度
                            {
                                this.Width += 30;
                            }
                            break;
                        }
                    case "int": // 整数类型
                        {
                            textBox.TextBox.TextChanged += new EventHandler(textBoxTextChanged);
                            while (maxLabelLength + 270 > this.Width) // 自动调整宽度
                            {
                                this.Width += 30;
                            }
                            break;
                        }
                    case "readonly": // 只读类型
                        {
                            textBox.Enabled = false;
                            while (maxLabelLength + 270 > this.Width) // 自动调整宽度
                            {
                                this.Width += 30;
                            }
                            break;
                        }
                    case "list": // 下拉菜单类型
                        {
                            ComboBoxItem comboBox = new ComboBoxItem("comboBox" + i, "");
                            string s = values[2];
                            string[] sections = s.Split(new char[]{'@'}, StringSplitOptions.RemoveEmptyEntries);
                            switch(sections.Length)
                            {
                                case 1: // 无内容有列表值
                                    {
                                        if(s.Contains("@")) // 带文本的下拉框
                                        {
                                            textBox.TextBox.Text = ""; // 更改文本内容
                                            string[] t = sections[0].Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                            foreach (string r in t)
                                            {
                                                comboBox.Items.Add(r);
                                            }
                                            comboBox.SelectedIndex = 0;
                                            container.SubItems.Add(comboBox);

                                            while (maxLabelLength + 270 + comboBox.ComboWidth> this.Width) // 自动调整宽度
                                            {
                                                this.Width += 30;
                                            }
                                        }
                                        else // 不带文本的下拉框
                                        {
                                            container.SubItems.Remove(textBox);
                                            string[] t = sections[0].Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                            foreach(string r in t)
                                            {
                                                comboBox.Items.Add(r);
                                            }
                                            comboBox.SelectedIndex = 0;
                                            comboBox.ComboWidth = 200;
                                            container.SubItems.Add(comboBox);

                                            while (maxLabelLength + 270 > this.Width) // 自动调整宽度
                                            {
                                                this.Width += 30;
                                            }
                                        }
                                      
                                        break;
                                    }
                                case 2: // 有内容和列表值
                                    {
                                        textBox.TextBox.Text = sections[0]; // 更改文本内容
                                        string[] t = sections[1].Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                        foreach(string r in t)
                                        {
                                            comboBox.Items.Add(r);
                                        }
                                        comboBox.SelectedIndex = 0;
                                        container.SubItems.Add(comboBox);

                                        while (maxLabelLength + 270 + comboBox.ComboWidth > this.Width) // 自动调整宽度
                                        {
                                            this.Width += 30;
                                        }
                                        break;
                                    }
                            }
                            break;
                        }
                }                
                
                itemPanel1.Items.Add(container);
            }
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        private void SaveData()
        {
            foreach(object o in itemPanel1.Items)
            {
                string text = string.Empty;
                ItemContainer container = o as ItemContainer;
                TextBoxItem textBox = container.SubItems[1] as TextBoxItem;
                if(textBox != null) // 存在文本框
                {
                    text += textBox.TextBox.Text;
                    if ((container.SubItems.Count == 3) && (container.SubItems[2] is ComboBoxItem)) // 添加下拉框的内容
                    {
                        text += (container.SubItems[2] as ComboBoxItem).SelectedItem.ToString();
                    }
                }
                else // 不存在文本框，检查下拉框
                {
                    if ((container.SubItems.Count == 2) && (container.SubItems[1] is ComboBoxItem)) // 添加下拉框的内容
                    {
                        text += (container.SubItems[1] as ComboBoxItem).SelectedItem.ToString();
                    }
                }
                
                inputList.Add(text);
            }
        }

        /// <summary>
        /// 编辑文本事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void editText(object sender, EventArgs e)
        {
            ButtonItem buttonItem = sender as ButtonItem;
            TextBox textBox = buttonItem.Tag as TextBox;
            System.Data.SqlClient.SqlConnection conn = DataBaseManager.GetDataBaseManager().Connection;
            string result = doit.show(textBox.Text, ref conn, Helper.ClientPath);
            textBox.Text = result;
        }

        /// <summary>
        /// 文本框内容改变事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxTextChanged(object sender, EventArgs e)
        {
            TextBox textBox = sender as TextBox;
            string text = textBox.Text;
            int tempInt;
            if (!int.TryParse(text, out tempInt))
            {
                MessageBox.Show("输入值必须为整数!");
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            SaveData();
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}