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

namespace Fireball.Printing
{
    public class PrintableDocument: IPrintElement , IHaveChild
    {
        protected PrintElementCollection childs = null;
        private string _IstanceID = Guid.NewGuid().ToString();
        private FlowRowCollection _Rows = null;

        private HorizontalAlignment _HorizontalAlignment = HorizontalAlignment.Left;

        public PrintableDocument()
        {
            this.childs = new PrintElementCollection(this);
            this._Rows = new FlowRowCollection();
        }


        #region IPrintElement

        public string IstanceID
        {
            get
            {
                return _IstanceID;
            }
        }
        public void Render(IPrintingContext context)
        {
            context.PrintPrintableDocument(this);
        }
        public PrintElementType ElementType
        {
            get {

                return new PrintableDocumentType();
            }
        }
        public IPrintElement Parent
        {
            get {
                return null;
            }
        }

        #endregion

        #region IHaveChild Membri di

        public PrintElementCollection Childs
        {
            get
            {
                return childs;
            }
        }
        PrintElementCollection IHaveChild.Childs
        {
            get {

                return childs;
            }
        }
        HorizontalAlignment IHaveChild.HorizontalContentAlignment
        {
            get
            {
                return _HorizontalAlignment;
            }
            set
            {
                _HorizontalAlignment = value;
            }
        }
        public FlowRowCollection Rows
        {
            get {
                return _Rows;
            }
        }

        public float AvailableWidth
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        #endregion
    }
}
