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
using Fireball.Collections.Generic;

namespace Fireball.Printing
{
    public sealed class PrintElementCollection : LightCollection<PrintElementBase>
    {
        private IPrintElement owner = null;

        public PrintElementCollection(IPrintElement owner)
        {
            this.owner = owner;
        }

        public override int Add(PrintElementBase item)
        {
            item.setParent(this.owner);
            return base.Add(item);
        }
        public override void AddRange(PrintElementBase[] items)
        {
            foreach (PrintElementBase current in items)
            {
                current.setParent(this.owner);
            }
            base.AddRange(items);
        }

        public override void Insert(int index, PrintElementBase item)
        {
            item.setParent(this.owner);
            base.Insert(index, item);            
        }

        public override void InsertRange(int index, PrintElementBase[] items)
        {
            foreach (PrintElementBase current in items)
            {
                current.setParent(this.owner);
            }

            base.InsertRange(index, items);
        }

    }
}
