using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    public class NumericTextBox:TextBox
    {
        private const int WM_PASTE = 0x302;
        private NumericBox _owner;

        internal NumericTextBox(NumericBox owner)
        {
            _owner = owner;
        }

        protected override void WndProc(ref Message m)
        {

            if (m.Msg == WM_PASTE)
            {
                string clipBoard = (string)Clipboard.GetData(System.Windows.Forms.DataFormats.Text);
                clipBoard = _owner.ParseInputString(clipBoard);
                if (this.SelectedText.Length > 0)
                {
                    this.SelectedText = clipBoard;
                }
                else
                {
                    this.AppendText(clipBoard);
                }

                return;
            }

            base.WndProc(ref m);
        }

        protected override void OnTextChanged(EventArgs e)
        {
            base.OnTextChanged(e);
        }
    }
}
