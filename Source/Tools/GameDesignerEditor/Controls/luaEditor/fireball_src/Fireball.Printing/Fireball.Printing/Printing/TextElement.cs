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
using System.Drawing;

namespace Fireball.Printing
{
    /// <summary>
    /// Reppresent an element with Text
    /// </summary>
    public abstract class TextElement : PrintElementBase
    {
        private string _fontName = null;
        private float _fontSize = -1;
        private FontStyle _fontStyle = FontStyle.Regular;
        private bool _wrapText = true;
        private PrintColor _printColor = PrintColor.Empty;
        private HorizontalAlignment _textAlignment;

        private object content = null;

        public virtual object Content
        {
            get { return content; }
            set { content = value; }
        }

        public virtual HorizontalAlignment TextAlignment
        {
            get
            {
                return _textAlignment;
            }
            set
            {
                _textAlignment = value;
            }
        }

        public virtual PrintColor Foreground
        {
            get
            {
                return _printColor;
            }
            set
            {
                _printColor = value;
            }
        }

        public virtual bool WrapText
        {
            get
            {
                return _wrapText;
            }
            set
            {
                _wrapText = value;
            }
        }

        public virtual FontStyle FontStyle
        {
            get
            {
                return _fontStyle;
            }
            set
            {
                _fontStyle = value;
            }
        }

        public virtual float FontSize
        {
            get
            {
                return _fontSize;
            }
            set
            {
                _fontSize = value;
            }
        }


        public virtual string FontName
        {
            get
            {
                return _fontName;
            }
            set
            {
                _fontName = value;
            }
        }
    }
}
