using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HelperEditor
{
    class CustomListView : System.Windows.Forms.ListView
    {
        public CustomListView()
        {
            SetStyle(System.Windows.Forms.ControlStyles.DoubleBuffer |
                              System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer |
                              System.Windows.Forms.ControlStyles.AllPaintingInWmPaint,
                              true);
            UpdateStyles();
        }
    }
}
