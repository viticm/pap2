using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace ClientCompare
{
    public partial class ConfigForm : Office2007Form
    {
        public ConfigForm(string filePath)
        {
            InitializeComponent();
            textBoxX1.Text = filePath;
        }

        public string FilePath
        {
            get
            {
                return textBoxX1.Text;
            }
        }

        public bool CompareTabFile
        {
            get
            {
                return tabCheckBox.Checked;
            }
        }

        public bool CompareTxtFile
        {
            get
            {
                return txtCheckBox.Checked;
            }
        }

        public bool CompareIniFile
        {
            get
            {
                return iniCheckBox.Checked;
            }
        }

        public bool CompareLogicalFile
        {
            get
            {
                return logicalCheckBox.Checked;
            }
        }

        public bool CompareOtherFile
        {
            get
            {
                return otherCheckBox.Checked;
            }
        }

        public bool CompareLuaFile
        {
            get
            {
                return luaCheckBox.Checked;
            }
        }

        public bool CompareLSFile
        {
            get
            {
                return lsCheckBox.Checked;
            }
        }

        public bool CompareLHFile
        {
            get
            {
                return lhCheckBox.Checked;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {      
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

        /// <summary>
        /// 浏览文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "请选择比对程序文件";
            
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBoxX1.Text = openFileDialog.FileName;
            }
        }
    }
}