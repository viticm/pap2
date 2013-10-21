// Color.cs
//
// Authors:
//   Sebastian Faltoni
//
// (C) Ximian, Inc. 2003
// Copyright (C) 2004 Novell, Inc (http://www.novell.com)
// Copyright (C) 2006 DotNetFireball (http://www.dotnetfireball.net)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Cairo
{

    public struct Color
    {
        [Flags]
        enum ColorType
        {
            Empty = 0,
            ARGB = 1,
            Known = 2,
            Named = 4,
            System = 8
        }

        private ColorType colorType;
        private string myname;
        private KnownColor knownColor;

        internal static Color FromArgbNamed(int alpha, int red, int green, int blue, string name, KnownColor knownColor)
        {
            Color color = new Color(alpha, red, green, blue);
            color.colorType = ColorType.Known | ColorType.Named;
            //color.issystemcolor = false; //???
            color.myname = name;
            // FIXME: here happens SEGFAULT.
            //color.knownColor = (KnownColor) Enum.Parse (typeof (KnownColor), name, false);
            color.knownColor = knownColor;
            return color;
        }

        private static ArgumentException CreateColorArgumentException(int value, string color)
        {
            return new ArgumentException(string.Format("'{0}' is not a valid"
                + " value for '{1}'. '{1}' should be greater or equal to 0 and"
                + " less than or equal to 255.", value, color));
        }

        private static void CheckRGBValues(int red, int green, int blue)
        {
            if ((red > 255) || (red < 0))
                throw CreateColorArgumentException(red, "red");
            if ((green > 255) || (green < 0))
                throw CreateColorArgumentException(green, "green");
            if ((blue > 255) || (blue < 0))
                throw CreateColorArgumentException(blue, "blue");
        }


        private static void CheckARGBValues(int alpha, int red, int green, int blue)
        {
            if ((alpha > 255) || (alpha < 0))
                throw CreateColorArgumentException(alpha, "alpha");
            CheckRGBValues(red, green, blue);
        }

        public Color(int red, int green, int blue)
            : this(red, green, blue, 1)
        {
        }

        internal Color( int alpha,int red, int green, int blue)
        {
            CheckARGBValues(alpha, red, green, blue);

            this.red = red;
            this.green = green;
            this.blue = blue;
            this.alpha = alpha;

            knownColor = new KnownColor();
            myname = null;
            colorType = ColorType.ARGB;
        }

        double red, green, blue, alpha;

        public double R
        {
            get { return red; }
            set { red = value; }
        }

        public double G
        {
            get { return green; }
            set { green = value; }
        }

        public double B
        {
            get { return blue; }
            set { blue = value; }
        }

        public double A
        {
            get { return alpha; }
            set { alpha = value; }
        }


        public static Color FromKnownColor(KnownColor c)
        {
			if (c < KnownColor.ActiveBorder || c > KnownColor.MenuHighlight) {

                // This is what it returns!
                Color d = new Color(0, 0, 0, 0);

                d.myname = c.ToString();
                d.colorType |= ColorType.Named;
                d.knownColor = c;

                return d;
            }

           object color =  typeof(Colors).InvokeMember(c.ToString(), System.Reflection.BindingFlags.Static |
                 System.Reflection.BindingFlags.GetProperty| System.Reflection.BindingFlags.Public, null, null, null);

           return (Color)color;
        }

        public static Color FromArgb(int alpha, int r, int g, int b)
        {
            Color color = new Color(alpha, r, g, b);

            color.colorType = ColorType.ARGB;

            return color;
        }

        internal static Color FromArgbSystem(int alpha, int red, int green, int blue, string name, KnownColor knownColor)
        {
            Color color = FromArgbNamed(alpha, red, green, blue, name, knownColor);
            color.colorType |= ColorType.System;
            return color;
        }
    }
}
