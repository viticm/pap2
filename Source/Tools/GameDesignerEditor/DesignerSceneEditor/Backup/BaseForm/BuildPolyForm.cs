using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class BuildPolyForm: Form
	{
        public int m_nVergeNum = 0;
        public float m_fRadius = 0.0f;
        public float m_fDegree = 0.0f;
        public float m_fHeight = 0.0f;


		public BuildPolyForm()
		{
			InitializeComponent();
            textboxVergeNum.Text = "4";
            textRadius.Text = "1000";
            textDegree.Text = "45";
            textHeight.Text = "100";
		}

        private void buttonX1_Click(object sender, EventArgs e) // OnOK
        {
            if (textboxVergeNum.Text.Length == 0 ||
                textRadius.Text.Length == 0 ||
                textDegree.Text.Length == 0 ||
                textHeight.Text.Length == 0)
            {
                MessageBox.Show("请输入完整信息。");
                return;
            }

            m_nVergeNum = Convert.ToInt32(textboxVergeNum.Text);
            m_fRadius = Convert.ToSingle(textRadius.Text);
            m_fDegree = Convert.ToSingle(textDegree.Text);
            m_fHeight = Convert.ToSingle(textHeight.Text);

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void buttonX2_Click(object sender, EventArgs e) // OnCancel
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
	}
}