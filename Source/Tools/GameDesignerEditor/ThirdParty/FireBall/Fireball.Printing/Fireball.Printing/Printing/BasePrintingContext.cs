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
    public abstract class BasePrintingContext : IPrintingContext
    {
        #region IPrintingContext Membri di

        protected internal abstract void PrintPrintableDocument(PrintableDocument printableDocument);

        #endregion

        #region Fields
        protected int pages = 0;
        protected int currentPage = 0;
        protected float currentY = 0;
        protected float currentX = 0;
        #endregion

        #region IPrintingContext Membri di


        public int Pages
        {
            get {
                return pages;
            }
        }

        public int CurrentPage
        {
            get {

                return currentPage;
            }
        }

        public virtual float CurrentY
        {
            get {                
                return currentY;
            }
        }

        public virtual float CurrentX
        {
            get {
                return currentX;
            }
        }

        #endregion

        protected abstract Rect GetRect(PrintElementBase element);

        #region IPrintingContext Membri di

        void IPrintingContext.PrintPrintableDocument(PrintableDocument printableDocument)
        {
            this.PrintPrintableDocument(printableDocument);
        }

        #endregion

        protected abstract void DoRealignContent(PrintElementBase element);

        public abstract void Print();
    }
}
