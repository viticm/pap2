using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace GuideConfig
{
    public partial class SortForm : Office2007Form
    {
        private List<string> recordList; // 记录链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="recordList">记录链表</param>
        public SortForm(List<string> recordList)
        {
            this.recordList = recordList;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 记录链表
        /// </summary>
        public List<string> RecordList
        {
            get
            {
                return recordList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach (string recordName in recordList)
            {
                recordListBox.Items.Add(recordName);
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            recordList = new List<string>();

            foreach (string recordName in recordListBox.Items)
            {
                recordList.Add(recordName);
            }

            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 上移记录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMoveUp_Click(object sender, EventArgs e)
        {
            int index = recordListBox.SelectedIndex;

            if (index != -1)
            {
                if (index > 0)
                {
                    string itemInfo = recordListBox.Items[index] as string;
                    recordListBox.Items.RemoveAt(index);
                    recordListBox.Items.Insert(index - 1, itemInfo);
                    recordListBox.SelectedIndex = index - 1;
                }
            }
            else
            {
                MessageBox.Show("请先选择要上移的记录！", "上移记录",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 下移记录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMoveDown_Click(object sender, EventArgs e)
        {
            int index = recordListBox.SelectedIndex;

            if (index != -1)
            {
                if (index < recordListBox.Items.Count - 1)
                {
                    string itemInfo = recordListBox.Items[index] as string;
                    recordListBox.Items.RemoveAt(index);
                    recordListBox.Items.Insert(index + 1, itemInfo);
                    recordListBox.SelectedIndex = index + 1;
                }
            }
            else
            {
                MessageBox.Show("请先选择要下移的记录！", "下移记录",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}