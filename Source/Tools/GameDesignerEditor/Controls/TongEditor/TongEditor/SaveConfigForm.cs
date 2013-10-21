using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;

namespace ToneEditor
{
    public partial class SaveConfigForm : Office2007Form
    {
        private string tongTemplateFolder; // TongTemplate.dat文件保存的目录
        private string tongOperationFolder; // TongOperation.tab文件保存的目录        

        /// <summary>
        /// 构造函数
        /// </summary>
        public SaveConfigForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// TongTemplate.dat文件保存的目录
        /// </summary>
        public string TongTemplateFolder
        {
            get
            {
                return tongTemplateFolder;
            }
        }

        /// <summary>
        /// TongOperation.tab文件保存的目录
        /// </summary>
        public string TongOperationFolder
        {
            get
            {
                return tongOperationFolder;
            }
        }

        /// <summary>
        /// 是否保存TongTemplate.dat文件
        /// </summary>
        public bool SaveTongTemplateFile
        {
            get
            {
                return checkBoxX1.Checked;
            }
        }

        /// <summary>
        /// 是否保存TongOperation.tab文件
        /// </summary>
        public bool SaveTongOperationFile
        {
            get
            {
                return checkBoxX2.Checked;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            tongTemplateFolder = Application.StartupPath;
            tongOperationFolder = Application.StartupPath;

            textBoxX1.Text = tongTemplateFolder;
            textBoxX2.Text = tongOperationFolder;
        }

        /// <summary>
        /// 勾选/取消导出TongTemplate.dat文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            textBoxX1.Enabled = checkBoxX1.Checked;
            buttonX3.Enabled = checkBoxX1.Checked;
        }

        /// <summary>
        /// 勾选/取消导出TongOperation.tab文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void checkBoxX2_CheckedChanged(object sender, EventArgs e)
        {
            textBoxX2.Enabled = checkBoxX2.Checked;
            buttonX4.Enabled = checkBoxX2.Checked;
        }

        /// <summary>
        /// 浏览文件夹
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBroserDialog = new FolderBrowserDialog();                        
            folderBroserDialog.SelectedPath = tongTemplateFolder;            
            folderBroserDialog.Description = "请选择文件保存的目录";

            if (folderBroserDialog.ShowDialog() == DialogResult.OK)
            {
                tongTemplateFolder = folderBroserDialog.SelectedPath;
                textBoxX1.Text = tongTemplateFolder;
            }
        }

        /// <summary>
        /// 浏览文件夹
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBroserDialog = new FolderBrowserDialog();
            folderBroserDialog.SelectedPath = tongOperationFolder;
            folderBroserDialog.Description = "请选择文件保存的目录";

            if (folderBroserDialog.ShowDialog() == DialogResult.OK)
            {
                tongOperationFolder = folderBroserDialog.SelectedPath;
                textBoxX1.Text = tongOperationFolder;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (checkBoxX1.Checked && textBoxX1.Text == "")
            {
                MessageBox.Show("TongTemplate.dat文件保存目录不能为空！", "文件保存配置", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
                return;
            }

            if (checkBoxX2.Checked && textBoxX2.Text == "")
            {
                MessageBox.Show("TongOperation.tab文件保存目录不能为空！", "文件保存配置", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
                return;
            }

            tongTemplateFolder = textBoxX1.Text;
            tongOperationFolder = textBoxX2.Text;
            DialogResult = DialogResult.OK;
            this.Close();
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