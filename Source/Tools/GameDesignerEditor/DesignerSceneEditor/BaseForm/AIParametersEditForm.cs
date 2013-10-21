using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace DesignerSceneEditor
{
    public partial class AIParametersEditForm : Form
    {
        private Hashtable infoTable;
        public delegate void UpdateAIParameter(int npcIndex, string fieldName, int fieldValue); // 更新AI参数用的回调
        private UpdateAIParameter updateAIParameter;
        private int npcIndex;

        public AIParametersEditForm(int npcIndex, Hashtable infoTable, UpdateAIParameter updateAIParameter)
        {
            this.npcIndex = npcIndex;
            this.infoTable = infoTable;
            this.updateAIParameter = updateAIParameter;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化
            foreach (DataRow dataRow in BaseForm.patrolPathTable.Rows)
            {
                comboBoxEx3.Items.Add(string.Format("{0} ({1})", dataRow["Name"].ToString(), dataRow["ID"].ToString()));
            }

            // 显示数据
            if (infoTable["IsPositive"] as string == "1")
            {
                comboBoxEx1.SelectedIndex = 0;
            }
            else
            {
                comboBoxEx1.SelectedIndex = 1;
            }

            if (infoTable["IsAutoTurn"] as string == "1")
            {
                comboBoxEx2.SelectedIndex = 0;
            }
            else
            {
                comboBoxEx2.SelectedIndex = 1;
            }

            textBoxX1.Text = infoTable["NpcSceneType"] as string;
            textBoxX2.Text = infoTable["AlertRange"] as string;
            textBoxX3.Text = infoTable["AlertOverTime"] as string;
            textBoxX4.Text = infoTable["AttackRange"] as string;
            textBoxX5.Text = infoTable["EscapeLife"] as string;
            textBoxX6.Text = infoTable["EscapeRange"] as string;
            textBoxX7.Text = infoTable["EscapeTime"] as string;
            textBoxX8.Text = infoTable["EscapeInterval"] as string;
            textBoxX9.Text = infoTable["WanderRange"] as string;
            textBoxX10.Text = infoTable["WanderDistance"] as string;
            textBoxX11.Text = infoTable["WanderInterval"] as string;
            comboBoxEx3.SelectedIndex = int.Parse(infoTable["PatrolPathID"] as string) - 1;
            textBoxX12.Text = infoTable["PatrolOrderIndex"] as string;

            if (infoTable["ReviveTeammate"] as string == "1")
            {
                comboBoxEx4.SelectedIndex = 0;
            }
            else
            {
                comboBoxEx4.SelectedIndex = 1;
            }
        }

        /// <summary>
        /// 检查并更新对应字段上的数据
        /// </summary>
        /// <param name="fieldName">字段名</param>
        /// <param name="fieldValue">字段值</param>
        private void UpdateFieldValue(string fieldName, int fieldValue)
        {
            if (infoTable[fieldName] as string != fieldValue.ToString())
            {
                updateAIParameter(npcIndex, fieldName, fieldValue);
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            int fieldValue;

            if (comboBoxEx1.SelectedIndex == 0)
            {
                fieldValue = 1;
            }
            else
            {
                fieldValue = 0;
            }

            UpdateFieldValue("IsPositive", fieldValue);

            if (comboBoxEx2.SelectedIndex == 0)
            {
                fieldValue = 1;
            }
            else
            {
                fieldValue = 0;
            }

            UpdateFieldValue("IsAutoTurn", fieldValue);

            fieldValue = int.Parse(textBoxX1.Text);
            UpdateFieldValue("NpcSceneType", fieldValue);

            fieldValue = int.Parse(textBoxX2.Text);
            UpdateFieldValue("AlertRange", fieldValue);

            fieldValue = int.Parse(textBoxX3.Text);
            UpdateFieldValue("AlertOverTime", fieldValue);

            fieldValue = int.Parse(textBoxX4.Text);
            UpdateFieldValue("AttackRange", fieldValue);

            fieldValue = int.Parse(textBoxX5.Text);
            UpdateFieldValue("EscapeLife", fieldValue);

            fieldValue = int.Parse(textBoxX6.Text);
            UpdateFieldValue("EscapeRange", fieldValue);

            fieldValue = int.Parse(textBoxX7.Text);
            UpdateFieldValue("EscapeTime", fieldValue);

            fieldValue = int.Parse(textBoxX8.Text);
            UpdateFieldValue("EscapeInterval", fieldValue);

            fieldValue = int.Parse(textBoxX9.Text);
            UpdateFieldValue("WanderRange", fieldValue);

            fieldValue = int.Parse(textBoxX10.Text);
            UpdateFieldValue("WanderDistance", fieldValue);

            fieldValue = int.Parse(textBoxX11.Text);
            UpdateFieldValue("WanderInterval", fieldValue);

            fieldValue = comboBoxEx3.SelectedIndex + 1;
            UpdateFieldValue("PatrolPathID", fieldValue);

            fieldValue = int.Parse(textBoxX12.Text);
            UpdateFieldValue("PatrolOrderIndex", fieldValue);

            if (comboBoxEx4.SelectedIndex == 0)
            {
                fieldValue = 1;
            }
            else
            {
                fieldValue = 0;
            }

            UpdateFieldValue("ReviveTeammate", fieldValue);

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