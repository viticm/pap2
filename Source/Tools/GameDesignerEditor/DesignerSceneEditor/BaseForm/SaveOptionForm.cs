using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class SaveOptionForm: Form
	{
        public bool m_SaveIniOnly = true;
        public bool m_SaveLogics = true;
        public bool m_ComputeCell = false;
        public bool m_ClearCell = false;
        public bool m_AppointCell = false;
        public bool m_UpdateAllRest = false;
        public bool m_AllRest = false;

		public SaveOptionForm()
		{
			InitializeComponent();
            radioIniOnly.Checked = m_SaveIniOnly;
            radioAll.Checked = !radioIniOnly.Checked;
            checkComputeCell.Checked = m_ComputeCell;
            checkClearCell.Checked = m_ClearCell;
            radioNotRest.Checked = true;
            checkSaveLogics.Checked = true;
		}

        private void bOK_Click(object sender, EventArgs e)
        {
            m_SaveIniOnly = radioIniOnly.Checked;
            m_SaveLogics = checkSaveLogics.Checked;
            m_ComputeCell = checkComputeCell.Checked;
            m_ClearCell = checkClearCell.Checked;
            m_AppointCell = checkAppointCell.Checked;
            m_UpdateAllRest = checkBoxRest.Checked;
            m_AllRest = radioRest.Checked;
            if (m_UpdateAllRest && !m_AllRest)
                MessageBox.Show("全地图安全信息的变更，会去除笔刷工具所绘制的安全信息。", "注意");
            this.DialogResult = DialogResult.OK;
            this.Dispose();
        }

        private void bCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Dispose();
        }

        private void cbCalcColi_CheckedChanged(object sender, EventArgs e)
        {
            if (this.checkComputeCell.Checked)
            {
                DialogResult dr = MessageBox.Show("勾选此项，将会导致除地面意外所有的 TRAP 全部丢失，要继续吗？", "确认计算障碍", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                this.checkComputeCell.Checked = this.checkClearCell.Enabled = this.checkAppointCell.Enabled = (dr == DialogResult.Yes);
            }
            else
            {
                this.checkClearCell.Enabled = this.checkAppointCell.Enabled = false;
            }
        }

        private void radioIniOnly_CheckedChanged(object sender, EventArgs e)
        {
            UpdateCheckBoxes();
        }

        private void radioAll_CheckedChanged(object sender, EventArgs e)
        {
            UpdateCheckBoxes();
        }

        private void UpdateCheckBoxes()
        {
            checkComputeCell.Enabled = checkAppointCell.Enabled = checkClearCell.Enabled = checkBoxRest.Enabled = radioRest.Enabled = radioNotRest.Enabled = checkSaveLogics.Enabled = radioAll.Checked;
            if (!checkComputeCell.Checked)
                checkAppointCell.Enabled = checkClearCell.Enabled = false;
            if (!checkBoxRest.Checked)
                radioRest.Enabled = radioNotRest.Enabled = false;
        }

        private void cbRest_CheckedChanged(object sender, EventArgs e)
        {
            radioRest.Enabled = radioNotRest.Enabled = checkBoxRest.Checked;
        }

	}
}