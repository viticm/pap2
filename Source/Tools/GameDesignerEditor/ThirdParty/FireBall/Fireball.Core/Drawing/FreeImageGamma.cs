
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
using Fireball.Drawing.Internal;

namespace Fireball.Drawing
{
    public class FreeImageGamma : FreeImagePixelTransformation
    {
        private double _gamma;

        public double Gamma
        {
            get { return _gamma; }
            set { _gamma = value; }
        }

        private bool _completed;

        public bool Completed
        {
            get { return _completed; }
        }

        public FreeImageGamma(double gamma)
        {
            _gamma = gamma;
        }

        internal override void Run(FreeImage image)
        {
            _completed = FreeImageApi.AdjustGamma(image.GetFreeImageHwnd(), _gamma);
        }
    }
}
