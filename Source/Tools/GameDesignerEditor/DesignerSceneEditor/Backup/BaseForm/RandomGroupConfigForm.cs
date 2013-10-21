using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class RandomGroupConfigForm: Form
	{
        private List<string> npcInfoList;
        private List<string> npcTemplateIDList;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="npcInfoList">NPC信息链表</param>
		public RandomGroupConfigForm(List<string> npcInfoList)
		{
            this.npcInfoList = npcInfoList;
			InitializeComponent();
            Init();
		}

        /// <summary>
        /// NPC模板ID链表
        /// </summary>
        public List<string> NpcTemplateIDList
        {
            get
            {
                return npcTemplateIDList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            npcTemplateIDList = new List<string>();

            listBox1.Items.Clear();

            foreach (string s in npcInfoList)
            {
                listBox1.Items.Add(s);
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItems.Count == 0)
            {
                MessageBox.Show("随机组至少需要填入一个模板ID！", "配置随机组", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            foreach (string s in listBox1.SelectedItems)
            {
                int index = s.IndexOf(" ");
                string npcTemplateID = s.Substring(index + 2);
                npcTemplateID = npcTemplateID.Substring(0, npcTemplateID.Length - 1);

                npcTemplateIDList.Add(npcTemplateID);
            }

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