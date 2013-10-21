#region Copyright
//    Copyright (C) 2005  Sebastian Faltoni
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
#endregion

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Fireball.Printing.Types;

namespace Fireball.Printing
{
    public class ImageElement : PrintElementBase
    {
        private Stream _SourceStream = null;

        private int height = 0;
        private int width = 0;

        public int Width
        {
            get { return width; }
            set { width = value; }
        }

        public int Height
        {
            get { return height; }
            set { height = value; }
        }

        public Stream SourceStream
        {
            get { return _SourceStream; }
            set { _SourceStream = value; }
        }

        public override PrintElementType ElementType
        {
            get
            {
                return ImageElementType.ElementType;
            }
        }
    }
}
