using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Drawing.Internal;

namespace Fireball.Drawing
{
    public class FreeImageColorConversion : FreeImagePixelTransformation
    {
        public enum ConvertTo
        {
            Bit16,
            Bit24,
            Bit32
        }
        private ConvertTo conversion = ConvertTo.Bit32;

        public ConvertTo Conversion
        {
            get { return conversion; }
        }

        public FreeImageColorConversion(ConvertTo conversion)
        {
            this.conversion = conversion;
        }

        internal override void Run(FreeImage image)
        {
            int dib = FreeImageApi.ConvertTo32Bits(image.GetFreeImageHwnd());

            image.DisposeAndSetHandle(dib);
        }
    }
}
