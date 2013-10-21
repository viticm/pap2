using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace StandardNpcCreate
{
    public partial class UpdateResultForm : Form
    {
        private List<string> updateSuccessNpcList; // 更新动作成功NPC链表
        private List<string> updateFailNpcList; // 更新动作失败NPC链表
        private string logText; // log文本

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="updateSuccessNpcList">更新动作成功NPC链表</param>
        /// <param name="updateFailNpcList">更新动作失败NPC链表</param>
        public UpdateResultForm(List<string> updateSuccessNpcList, List<string> updateFailNpcList)
        {
            this.updateSuccessNpcList = updateSuccessNpcList;
            this.updateFailNpcList = updateFailNpcList;
            InitializeComponent();
        }

        /// <summary>
        /// log文本
        /// </summary>
        public string LogText
        {
            set
            {
                logText = value;
            }
        }

        /// <summary>
        /// 数据初始化
        /// </summary>
        public void Init()
        {
            int successCount = updateSuccessNpcList.Count;
            int failCount = updateFailNpcList.Count;
            int totalCount = successCount + failCount;

            StringBuilder content = new StringBuilder();
            content.AppendLine(string.Format("共更新{0}个NPC...\r\n", totalCount.ToString()));
            content.AppendLine(string.Format("{0}个NPC更新成功：", successCount.ToString()));

            foreach(string s in updateSuccessNpcList)
            {
                content.AppendLine(s);
            }

            content.AppendLine();
            content.AppendLine(string.Format("{0}个NPC更新失败：", failCount.ToString()));

            foreach(string s in updateFailNpcList)
            {
                content.AppendLine(s);
            }

            textBoxX1.Text = string.Format("{0}\r\n日志：\r\n{1}", content.ToString(), logText.ToString());
        }
    }
}