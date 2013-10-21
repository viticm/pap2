using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FlowChartEditor
{
    public partial class TextForm : Form
    {
        string folder; // 保存文件的目录

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="folder">保存文件的目录</param>
        /// <param name="title">窗体标题</param>
        /// <param name="content">输出内容</param>
        public TextForm(string folder, string title, string content)
        {
            this.folder = folder;
            InitializeComponent();
            this.Text = title;
            textBoxX1.Text = content;
        }

        /// <summary>
        /// 关闭窗体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 保存文本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            string fileName = string.Format("{0}\\{1}\\{2}.txt", System.Environment.CurrentDirectory, folder,
                string.Format("{0}-{1}-{2} {3}-{4}-{5}", DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day,
                DateTime.Now.Hour, DateTime.Now.Minute, DateTime.Now.Second));
            bool success = Helper.GetHelper().SaveTextToFile(fileName, textBoxX1.Text);
            if(success)
            {
                MessageBox.Show(string.Format("文件{0}保存成功！", fileName), "文件保存", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
        }
    }
}