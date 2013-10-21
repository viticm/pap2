using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using DevComponents.DotNetBar;

namespace FlowChartEditor
{
    public partial class SettingForm : Office2007Form
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        public SettingForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            Helper helper = Helper.GetHelper();
            textBoxX1.Text = helper.OutputDir;
        }

        /// <summary>
        /// 浏览文件夹
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog openFolderDialog = new FolderBrowserDialog();
            openFolderDialog.Description = "请选择客户端所在的目录";
            openFolderDialog.ShowNewFolderButton = true;
            openFolderDialog.RootFolder = Environment.SpecialFolder.MyComputer;

            if (openFolderDialog.ShowDialog() == DialogResult.OK)
            {
                textBoxX1.Text = openFolderDialog.SelectedPath;
            }
        }

        /// <summary>
        /// 确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            string outputDir = textBoxX1.Text;
            if (outputDir != "")
            {
                DirectoryInfo di = new DirectoryInfo(outputDir);

                if(di.Exists) // 客户端目录存在
                {
                    Helper helper = Helper.GetHelper();
                    helper.OutputDir = outputDir;

                    DialogResult = DialogResult.OK;
                    this.Close();
                }
                else
                {
                    MessageBox.Show("输出路径无效！", "参数设置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
            else
            {
                MessageBox.Show("输出路径不能为空！", "参数设置", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}