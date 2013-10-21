using System;
using System.Collections.Generic;
using System.Text;

namespace FireEdit.Docking
{
    public class FireEditDocumentWindow :FireEditDockableWindow
    {
        public FireEditDocumentWindow()
        {
            this.Padding = new System.Windows.Forms.Padding(1,2,1,1);
            this.DockableAreas = Fireball.Docking.DockAreas.Document;
        }
    }
}
