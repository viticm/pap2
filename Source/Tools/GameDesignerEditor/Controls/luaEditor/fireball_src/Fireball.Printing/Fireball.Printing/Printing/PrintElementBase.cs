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
    public abstract class PrintElementBase : IPrintElement
    {
        protected PrintElementType _elementType = null;
        protected IPrintElement _parent = null;
        protected Rect _bounds = Rect.Empty;

        private Nullable<float> _Width = null;
        private Nullable<float> _Height = null;
        private string _IstanceID = Guid.NewGuid().ToString();
        private PrintPen _BorderPen = null;

        private FlowRow _OwnerRow = null;

        public FlowRow OwnerRow
        {
            get { return _OwnerRow; }
        }

        public string IstanceID
        {
            get
            {
                return _IstanceID;
            }
        }
        public PrintPen BorderPen
        {
            get
            {
                return _BorderPen;
            }
            set
            {
                _BorderPen = value;
            }
        }
        public Nullable<float> Height
        {
            get { return _Height; }
            set { _Height = value; }
        }
        public Nullable<float> Width
        {
            get { return _Width; }
            set { _Width = value; }
        }
        public Rect RenderedBounds
        {
            get
            {
                return _bounds;
            }
        }

        public void SetRenderedBounds(Rect rect)
        {
            this._bounds = rect;
        }

        #region IPrintElement Membri di

        public virtual PrintElementType ElementType
        {
            get {
                return _elementType;
            }
        }
        public IPrintElement Parent
        {
            get {
                return _parent;
            }
        }

        #endregion

        internal void setParent(IPrintElement parent)
        {
            this._parent = parent;
        }

        internal void SetOwnerRow(FlowRow owner)
        {
            _OwnerRow = owner;
        }
    }
}
