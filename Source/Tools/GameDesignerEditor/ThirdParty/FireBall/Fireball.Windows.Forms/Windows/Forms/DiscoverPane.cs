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


using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace Fireball.Windows.Forms
{
    [Designer(typeof(DiscoverPaneDesigner)), ToolboxItem(false)]
    public class DiscoverPane : ContainerControl
    {
        private int buttonPadding = 0;
        private Image _SmallImage = null;

        protected override void OnTextChanged(EventArgs e)
        {
            base.OnTextChanged(e);

            if (this.Parent != null)
                this.Parent.Invalidate();
        }

        internal Rectangle ButtonRect
        {
            get
            {
                int index = -1;

                for (int i = this.Parent.Controls.Count-1; i >= 0 ; i--)
                {
                    DiscoverPane discover = (DiscoverPane)this.Parent.Controls[i];

                    if (!discover.ShowOnList)
                    {
                        index++;

                        if (discover == this)
                            break;
                    }
                }

                int x = 0;

                if (((DiscoverControl)this.Parent).ChewronRect == Rectangle.Empty)
                {
                    x = this.Parent.ClientSize.Width - 22;
                }
                else
                {
                    x = ((DiscoverControl)this.Parent).ChewronRect.Left - 15;
                }

                x -= (index * 20);

                x -= buttonPadding*index;

                Rectangle switchRect = ((DiscoverControl)this.Parent).SwitchBarRect;
              

                return new Rectangle( x
                     , switchRect.Top+1, 20, switchRect.Height-2);
            }
        }

        private Rectangle _ListButtonRect;

        internal Rectangle ListButtonRect
        {
            get
            {
                return _ListButtonRect;
            }
            set
            {
                _ListButtonRect = value;
            }
        }

        private bool _ShowOnList = false;

        public Image SmallImage
        {
            get
            {
                return _SmallImage;
            }
            set
            {
                if (value != null && value.Size != new Size(16, 16))
                {
                    throw new Exception("SmallImage must be 16x16.");
                }
                _SmallImage = value;

                if (this.Parent != null)
                    this.Parent.Invalidate();
            }
        }

        private Image _LargeImage = null;

        public Image LargeImage
        {
            get
            {
                return _LargeImage;
            }
            set
            {
                if (value != null && value.Size != new Size(24, 24))
                {
                    throw new Exception("SmallImage must be 16x16.");
                }
                _LargeImage = value;

                if (this.Parent != null)
                    this.Parent.Invalidate();
            }
        }

        private bool _Listed = true;

        public bool Listed
        {
            get
            {
                return _Listed;
            }
            set
            {
                _Listed = value;

                if (this.Parent != null)
                    this.Parent.Invalidate();
            }
        }

        internal bool ShowOnList
        {
            get
            {
                return _ShowOnList;
            }
            set
            {
                _ShowOnList = value;
            }
        }
    }
}
