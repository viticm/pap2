using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace CustomLoadTree
{
    public partial class MultiSelectForm : Form
    {
        private List<string> itemList; // 显示的内容链表
        private List<string> selectItemList; // 选中的内容链表
        private bool selectAll = true; // 是否全部选中

        /// <summary>
        /// 构造函数
        /// <param name="title">窗体的标题</param>
        /// </summary>
        public MultiSelectForm(string title)
        {
            InitializeComponent();
            this.Text = title;
        }

        /// <summary>
        /// 显示的内容链表属性
        /// </summary>
        public List<string> ItemList
        {
            set
            {
                itemList = value;
            }

            get
            {
                return itemList;
            }
        }

        /// <summary>
        /// 选中的内容链表属性
        /// </summary>
        public List<string> SelectItemList
        {
            set
            {
                selectItemList = value;
            }
        }

        /// <summary>
        /// 显示可供选择的内容
        /// </summary>
        public void ShowItems()
        {
            if((selectItemList != null) && (selectItemList.Count != 0)) // 自定义选中显示列的内容
            {
                int index = 0;
                foreach(string s in itemList)
                {
                    checkedListBox1.Items.Add(s);
                    if(selectItemList.Contains(s))
                    {
                        checkedListBox1.SetItemChecked(index, true);
                    }
                    index++;
                }
            }
            else
            {
                foreach (string s in itemList)
                {
                    checkedListBox1.Items.Add(s);
                }
            }
            
        }

        /// <summary>
        /// 确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (checkedListBox1.CheckedItems.Count > 0)
            {
                itemList.Clear();
                for (int i = 0; i < checkedListBox1.Items.Count; i++)
                {
                    if (checkedListBox1.GetItemChecked(i))
                    {
                        itemList.Add(checkedListBox1.Items[i].ToString());
                    }
                }

                DialogResult = DialogResult.OK;
            }
            else
            {
                DialogResult = DialogResult.No;
            }
            this.Close();
        }

        /// <summary>
        /// 取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 全部选中或取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            if (selectAll) // 全部选中
            {
                for (int i = 0; i < checkedListBox1.Items.Count; i++)
                {
                    checkedListBox1.SetItemChecked(i, true);
                }
                selectAll = false;
                buttonX3.Text = "全部取消";
            }
            else // 全部取消
            {
                for (int i = 0; i < checkedListBox1.Items.Count; i++)
                {
                    checkedListBox1.SetItemChecked(i, false);
                }

                selectAll = true;
                buttonX3.Text = "全部选中";
            }
        }
    }
}