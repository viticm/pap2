using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class ThumbnailForm: Form
	{
		public ThumbnailForm()
		{
			InitializeComponent();
		}

        private void CancelBut_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void SaveThumbnailBut_Click(object sender, EventArgs e)
        {
            bool bRenderTree = checkBoxX1.Checked;
            float fLeafScale = Convert.ToSingle(textBoxX2.Text);
            float fCellScale = Convert.ToSingle(textBoxX1.Text);
            string filename = string.Empty;
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Í¼ÏñÎÄ¼þ(*.bmp)|*.bmp";
            if (dlg.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            filename = dlg.FileName;
            if (filename == string.Empty)
            {
                return;
            }

            BaseForm baseForm = MainForm.m_MainForm.ActiveFrm as BaseForm;
            if (baseForm != null)
            {
                baseForm.SceneSceneEditor.SetFocus();
                bool OldRenderPoly = MainForm.m_MainForm.m_bRenderPoly;
                baseForm.SceneSceneEditor.EnableRenderPoly(0);
                baseForm.SceneSceneEditor.SetSnapToolScale(fCellScale);
                if (bRenderTree)
                {
                    baseForm.SceneSceneEditor.SetSpeedTreeLeafScale(fLeafScale);
                }
                baseForm.SceneSceneEditor.RenderSceneSnap(filename);
                baseForm.SceneSceneEditor.EnableRenderPoly(OldRenderPoly ? 1 : 0);
            }
            this.Close();
        }

        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            BaseForm baseForm = MainForm.m_MainForm.ActiveFrm as BaseForm;
            if (baseForm != null)
            {
                bool bRenderTree = checkBoxX1.Checked;
                int nFlag = bRenderTree ? 1 : 0;
                if (bRenderTree)
                {
                    baseForm.SceneSceneEditor.SetRenderTreeFlag(nFlag);
                    labelX2.Enabled = true;
                    textBoxX2.Enabled = true;
                }
                else
                {
                    baseForm.SceneSceneEditor.SetRenderTreeFlag(nFlag);
                    labelX2.Enabled = false;
                    textBoxX2.Enabled = false;
                }
            }
        }
	}
}