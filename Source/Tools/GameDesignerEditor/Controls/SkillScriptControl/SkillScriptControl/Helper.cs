using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace SkillScriptControl
{
    public static class Helper
    {
        #region InputBox
        static Form f;
        public static string InputBox(string strDefault, string strTitle)
        {
            f = new Form();
            TextBox text1 = new TextBox();
            Button buttonOK = new Button();
            Button buttonCancel = new Button();

            text1.Text = strDefault;
            text1.Left = 10;
            text1.Top = 10;
            text1.Size = new System.Drawing.Size(410, 12);

            buttonOK.Text = "确定";
            buttonOK.Top = 40;
            buttonOK.Left = 250;

            buttonCancel.Text = "取消";
            buttonCancel.Top = 40;
            buttonCancel.Left = 340;

            f.Controls.Add(text1);
            f.Controls.Add(buttonOK);
            f.Controls.Add(buttonCancel);
            f.AcceptButton = buttonOK;
            f.CancelButton = buttonCancel;
            f.MaximizeBox = false;
            f.MinimizeBox = false;
            f.ShowIcon = false;
            f.ShowInTaskbar = false;
            f.StartPosition = FormStartPosition.CenterScreen;
            buttonOK.Click += new EventHandler(buttonOK_Click);
            buttonCancel.Click += new EventHandler(buttonCancel_Click);
            f.Text = strTitle;
            f.Size = new System.Drawing.Size(430, 100);
            DialogResult dr = f.ShowDialog();
            if (dr == DialogResult.OK)
            {
                return text1.Text;
            }
            else
            {
                return null;
            }
        }

        static void buttonCancel_Click(object sender, EventArgs e)
        {
            f.DialogResult = DialogResult.Cancel;
            f.Visible = false;
        }

        static void buttonOK_Click(object sender, EventArgs e)
        {
            f.DialogResult = DialogResult.OK;
            f.Visible = false;
        }
        #endregion

    }

}
