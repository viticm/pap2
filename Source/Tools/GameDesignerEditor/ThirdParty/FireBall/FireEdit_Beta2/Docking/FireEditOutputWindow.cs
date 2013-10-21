using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace FireEdit.Docking
{
    public class FireEditOutputWindow : FireEditDockableWindow
    {
        private RichTextBox _RichTextBox = null;

        public FireEditOutputWindow()
        {
            this.DockableAreas = Fireball.Docking.DockAreas.DockLeft
            | Fireball.Docking.DockAreas.DockRight
            | Fireball.Docking.DockAreas.DockTop
            | Fireball.Docking.DockAreas.Float
            | Fireball.Docking.DockAreas.DockBottom;

            _RichTextBox = new RichTextBox();

            this.Controls.Add(_RichTextBox);

            _RichTextBox.Dock = DockStyle.Fill;

            _RichTextBox.Font = new System.Drawing.Font("Lucida Console", 10);


            this.Text = "Output Window";
        }

        public void AppendOutput(string output)
        {
            _RichTextBox.AppendText(output);

            _RichTextBox.SelectionStart = _RichTextBox.TextLength;
        }

        public void Clear()
        {
            _RichTextBox.Text = string.Empty;
        }
    }
}
