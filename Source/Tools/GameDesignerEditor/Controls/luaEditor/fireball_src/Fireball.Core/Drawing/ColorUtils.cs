
//    Copyright (C) 2005  Riccardo Marzi  <riccardo@dotnetfireball.net>
//
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Fireball.Drawing
{
    public static class ColorUtils
    {
        public static Color BlendColors(Color backColor, Color foreColor, float alphaPercentage)
        {

            //TODO: Import code from PixelBuffer.BlendColorByAlpha......

            if (alphaPercentage > 100)
                throw new ArgumentOutOfRangeException("alphaPercentage must be under 100");

            if (alphaPercentage == 0)
                return foreColor;

            float redDiff = foreColor.R - backColor.R;
            float greenDiff = foreColor.G - backColor.G;
            float blueDiff = foreColor.B - backColor.B;

            alphaPercentage = alphaPercentage / 100f;

            redDiff = (redDiff * alphaPercentage) + backColor.R;
            greenDiff = (greenDiff * alphaPercentage) + backColor.G;
            blueDiff = (blueDiff * alphaPercentage) + backColor.B;

            return Color.FromArgb(255, (byte)redDiff, (byte)greenDiff, (byte)blueDiff);
        }

        public static float[] GetGradientColorStep(ColorHLS startColor, ColorHLS endColor, int stepCount)
        {
            if (stepCount == 0)
            {
                return new float[0];
            }
            if (stepCount == 1)
            {
                return new float[] { 0 };
            }

            float stepR = (endColor.Red - startColor.Red) / (float)stepCount;
            float stepG = (endColor.Green - startColor.Green) / (float)stepCount;
            float stepB = (endColor.Blue - startColor.Blue) / (float)stepCount;

            return new float[] { stepR, stepG, stepB };
        }

        public static float[,] GetGradientColorSteps(ColorHLS startColor, ColorHLS endColor, int stepCount)
        {
            if (stepCount == 0)
            {
                return new float[0,0];
            }

            float r = startColor.Red;
            float g = startColor.Green;
            float b = startColor.Blue;

            float[,] steps = new float[stepCount, 3];

            if (stepCount == 1)
            {
                for (int i = 0; i < stepCount; i++)
                {
                    steps[i, 0] = r;
                    steps[i, 1] = g;
                    steps[i, 2] = b;
                }
                return steps;
            }

            float stepR = (endColor.Red - startColor.Red) / (float)stepCount;
            float stepG = (endColor.Green - startColor.Green) / (float)stepCount;
            float stepB = (endColor.Blue - startColor.Blue) / (float)stepCount;

            for (int i = 0; i < stepCount; i++)
            {
                steps[i, 0] = r;
                steps[i, 1] = g;
                steps[i, 2] = b;

                r += stepR;
                g += stepG;
                b += stepB;
            }

            return steps;
        }

        public static ColorHLS[] CreateGradientColorArray(ColorHLS startColor, ColorHLS endColor, int stepCount)
        {
            if (stepCount == 0)
            {
                return new ColorHLS[0];
            }
            if (stepCount == 1)
            {
                return new ColorHLS[] { new ColorHLS(255, (byte)((startColor.Red + endColor.Red) / 2), (byte)((startColor.Green + endColor.Green) / 2), (byte)((startColor.Blue + endColor.Blue) / 2)) };
            }

            float stepR = (endColor.Red - startColor.Red) / (float)stepCount;
            float stepG = (endColor.Green - startColor.Green) / (float)stepCount;
            float stepB = (endColor.Blue - startColor.Blue) / (float)stepCount;

            float r = startColor.Red;
            float g = startColor.Green;
            float b = startColor.Blue;

            ColorHLS[] colors = new ColorHLS[stepCount];
            for (int i = 0; i < stepCount - 1; i++)
            {
                colors[i] = new ColorHLS(
                    255,
                    (byte)Math.Round(r, MidpointRounding.ToEven),
                    (byte)Math.Round(g, MidpointRounding.ToEven),
                    (byte)Math.Round(b, MidpointRounding.ToEven)
                    );

                r += stepR;
                g += stepG;
                b += stepB;
            }
            colors[colors.Length - 1] = endColor;

            return colors;
        }

        public static ColorHLS[] CreateGradientColorArray(ColorHLS[] colors, int stepCount)
        {
            if (stepCount == 0||colors.Length==0)
            {
                return new ColorHLS[0];
            }
            if (colors.Length == 1)
            {
                return colors;
            }
            if (stepCount == 1)
            {
                return new ColorHLS[] { new ColorHLS(ColorUtils.BlendColors(colors[0].Color,colors[colors.Length-1].Color,50) )};
            }

            ColorHLS[] retColors = new ColorHLS[stepCount];
            

            float step =  stepCount / (float)(colors.Length-1);
            int currentStep = 0;

            for (int i = 0; i < colors.Length-1; i++)
            {
                float r = colors[i].Red;
                float g = colors[i].Green;
                float b = colors[i].Blue;

                ColorHLS c1 = colors[i];
                ColorHLS c2 = colors[i + 1];
                float stepR = (c2.Red - c1.Red) / step;
                float stepG = (c2.Green - c1.Green) / step;
                float stepB = (c2.Blue- c1.Blue) / step;

                int count = (int)(step * (i+1));
                int k = (int)(step * i);
                while (k < count)
                {
                    retColors[currentStep] = new ColorHLS(255, (byte)r, (byte)g, (byte)b);
                    r += stepR;
                    g += stepG;
                    b += stepB;
                    currentStep++;
                    k++;
                }
            }
            retColors[stepCount - 1] = colors[colors.Length - 1].Clone();

            return retColors;
        }

        public static Color InterpolateColors(Color color1, Color color2, float percentage,bool useAlpha)
        {
            Color c = InterpolateColors(color1, color2, percentage);
            if (useAlpha)
            {
                int a1 = color1.A;
                int a2 = color2.A;
                byte alpha = Convert.ToByte((float)(a1 + ((a2 - a1) * percentage)));
                return Color.FromArgb(alpha, c);
            }
            return c;
        }

        public static Color InterpolateColors(Color color1, Color color2, float alpha)
        {
            if (alpha > 1) throw new ArgumentException("alpha must be lower or equal 1");

            int r1 = color1.R;
            int g1 = color1.G;
            int b1 = color1.B;
            int r2 = color2.R;
            int g2 = color2.G;
            int b2 = color2.B;
            byte r = Convert.ToByte((float)(r1 + ((r2 - r1) * alpha)));
            byte g = Convert.ToByte((float)(g1 + ((g2 - g1) * alpha)));
            byte b = Convert.ToByte((float)(b1 + ((b2 - b1) * alpha)));
            return Color.FromArgb(r, g, b);
        }
    }
}
