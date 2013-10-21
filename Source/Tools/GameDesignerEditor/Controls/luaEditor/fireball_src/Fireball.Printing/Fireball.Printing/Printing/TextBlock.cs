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
using Fireball.Printing.Types;

namespace Fireball.Printing
{

    /// <summary>
    /// TextBlock is a Block of text that can contains other's element type
    /// </summary>
    public class TextBlock : Block
    {
        public override PrintElementType ElementType
        {
            get
            {
                return TextBlockElementType.ElementType;
            }
        }

        private bool _FitToPage = false;

        public bool FitToPage
        {
            get { return _FitToPage; }
            set { _FitToPage = value; }
        }


        public override float AvailableWidth
        {
            get { return ((IHaveChild)this.Parent).AvailableWidth; }
        }
    }
}
