using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Drawing.Design
{
    public class ColorChangedEventArgs
    {
        public ColorHLS OldColor;
        public ColorHLS NewColor;

        public ColorChangedEventArgs(ColorHLS oldColor, ColorHLS newColor)
        {
            OldColor = oldColor;
            NewColor = newColor;
        }
    }
}
