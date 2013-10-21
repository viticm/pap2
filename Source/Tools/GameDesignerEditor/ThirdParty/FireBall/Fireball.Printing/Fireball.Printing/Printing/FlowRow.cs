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
using Fireball.Collections.Generic;

namespace Fireball.Printing
{
    public class FlowRow
    {
        private LightCollection<PrintElementBase> childs = null;
        private HorizontalAlignment hAlign;
        private PointF contentOffset;

        private bool _GoToNewPage = false;

        public bool GoToNewPage
        {
            get { return _GoToNewPage; }
            set { _GoToNewPage = value; }
        }

        public FlowRow()
        {
            childs = new LightCollection<PrintElementBase>();
            hAlign = HorizontalAlignment.Left;
            contentOffset = new PointF(0,0);
            childs.ItemAdd += new LightCollection<PrintElementBase>.LightCollectionAddHandler(childs_ItemAdd);

            childs.ItemRemove += new LightCollection<PrintElementBase>.LightCollectionRemoveHandler(childs_ItemRemove);
        }

        void childs_ItemRemove(object sender, LightCollection<PrintElementBase>.LightCollectionRemoveEventArgs e)
        {
            e.RemovedItem.SetOwnerRow(null);
        }

        void childs_ItemAdd(object sender, LightCollection<PrintElementBase>.LightCollectionAddEventArgs e)
        {
            e.Item.SetOwnerRow(this);
        }

        public LightCollection<PrintElementBase> Elements
        {
            get
            {
                return childs;
            }
        }

        public HorizontalAlignment Alignment
        {
            get
            {
                return hAlign;
            }
            set
            {
                hAlign = value;
            }
        }

        public PointF ContentOffset
        {
            get
            {
                return contentOffset;
            }
            set
            {
                contentOffset = value;
            }
        }

        public SizeF GetContentSize()
        {
            float w = 0;
            float h = 0;

            for (int i = 0; i < this.childs.Count; i++)
            {
                w += this.childs[i].RenderedBounds.Width;

                if (this.childs[i] is LineBreak)
                {

                }

                if (this.childs[i].RenderedBounds.Height > h)
                    h = this.childs[i].RenderedBounds.Height;
            }

            return new SizeF(w, h);
        }
    }
}
