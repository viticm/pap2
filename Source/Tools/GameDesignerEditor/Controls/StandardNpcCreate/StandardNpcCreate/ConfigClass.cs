using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace StandardNpcCreate
{
    public class ConfigClass
    {
        private ConfigForm cForm; // 选择窗体
        private StringBuilder logText = new StringBuilder(); // 日志文本

        /// <summary>
        /// 构造函数
        /// </summary>
        public ConfigClass()
        {

        }

        /// <summary>
        /// 是否更新插槽
        /// </summary>
        public bool UpdateSlot
        {
            get
            {
                return cForm.UpdateSlot;
            }
        }

        /// <summary>
        /// 是否更新动作
        /// </summary>
        public bool UpdateAnimation
        {
            get
            {
                return cForm.UpdateAnimation;
            }
        }

        /// <summary>
        /// 是否更新技能
        /// </summary>
        public bool UpdateSkill
        {
            get
            {
                return cForm.UpdateSkill;
            }
        }

        /// <summary>
        /// 显示编辑窗体
        /// </summary>
        /// <returns>是否编辑成功</returns>
        public bool ShowEditForm()
        {
            bool editSuccess = false;

            cForm = new ConfigForm();
            if (cForm.ShowDialog() == DialogResult.OK)
            {
                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 添加日志文本
        /// </summary>
        /// <param name="appendText">文本内容</param>
        public void AppendLogText(string appendText)
        {
            logText.AppendLine(appendText);
        }

        /// <summary>
        /// 显示日志文本
        /// </summary>
        public void ShowLogText()
        {
            MessageBox.Show(logText.ToString(), "刷新结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
            logText = new StringBuilder();
        }
    }
}
