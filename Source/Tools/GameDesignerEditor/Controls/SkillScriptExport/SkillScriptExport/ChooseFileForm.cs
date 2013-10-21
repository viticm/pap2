using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar.Controls;

namespace SkillScriptExport
{
    public partial class ChooseFileForm : Form
    {
        private bool selectAll = true; // 是否全部选择
        private List<string> fileNameList; // 当前可选文件名链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="fileNameList">文件名链表</param>
        public ChooseFileForm(List<string> fileNameList)
        {
            this.fileNameList = fileNameList;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 文件名链表
        /// </summary>
        public List<string> FileNameList
        {
            set
            {
                fileNameList = value;
            }

            get
            {
                return fileNameList;
            }
        }

        /// <summary>
        /// 初始化可选的文件名
        /// </summary>
        private void InitData()
        {
            foreach (string s in fileNameList)
            {
                checkedListBox1.Items.Add(s);
            }
        }

        /// <summary>
        /// 用户选择全部选中
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (checkedListBox1.CheckedItems.Count > 0)
            {
                fileNameList.Clear();
                for (int i = 0; i < checkedListBox1.Items.Count; i++)
                {
                    if (checkedListBox1.GetItemChecked(i))
                    {
                        fileNameList.Add(checkedListBox1.Items[i].ToString());
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