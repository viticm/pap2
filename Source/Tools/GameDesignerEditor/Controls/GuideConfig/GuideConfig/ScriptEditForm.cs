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
    public partial class ScriptEditForm : Office2007Form
    {
        private string scriptText; // 脚本内容

        /// <summary>
        /// 构造函数 
        /// </summary>
        /// <param name="scriptText">脚本内容</param>
        public ScriptEditForm(string scriptText)
        {
            this.scriptText = scriptText;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            luaEditorBox.Text = scriptText;
        }

        /// <summary>
        /// 脚本内容
        /// </summary>
        public string ScriptText
        {
            get
            {
                return scriptText;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            scriptText = luaEditorBox.Text;

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
    }
}