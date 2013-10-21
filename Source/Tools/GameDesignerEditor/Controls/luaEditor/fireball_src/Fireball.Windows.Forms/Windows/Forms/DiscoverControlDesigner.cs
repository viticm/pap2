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
using System.Windows.Forms.Design;
using System.ComponentModel.Design;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    class DiscoverControlDesigner : ParentControlDesigner
    {
        public DiscoverControlDesigner()
        {
            this.Verbs.Add(new DesignerVerb("Add Discover Pane", 
                new EventHandler(add_nav_click)));
        }

        void add_nav_click(object sender, EventArgs e)
        {
            IDesignerHost designerHost = (IDesignerHost)this.GetService(typeof(IDesignerHost));
            IComponentChangeService componentService = (IComponentChangeService)this.GetService(typeof(IComponentChangeService));
            DesignerTransaction designerTransaction = designerHost.CreateTransaction("Add Discover Pane");
            componentService.OnComponentChanging(this.Control, TypeDescriptor.GetProperties(this.Control)["Controls"]);
            DiscoverPane discoverPane = (DiscoverPane)designerHost.CreateComponent(typeof(DiscoverPane));
            this.Control.Controls.Add(discoverPane);
            this.Discover.SelectedPane = discoverPane;
            this.Discover.ShowPanes++;
            componentService.OnComponentChanged(this.Control, TypeDescriptor.GetProperties(this.Control)["Controls"], null, null);
            designerTransaction.Commit();

        }

        private const int WM_LBUTTONDOWN = 0x201;
        private const int WM_LBUTTONUP = 0x202;


        protected override void WndProc(ref System.Windows.Forms.Message m)
        {
            if (m.Msg == WM_LBUTTONUP)
            {
                ISelectionService selectionService = (ISelectionService)this.GetService(typeof(ISelectionService));
                Point ptMouse = this.Discover.PointToClient(Cursor.Position);
                DiscoverPane discoverPane = this.Discover.GetPaneAt(ptMouse.X, ptMouse.Y);

                if (discoverPane != null)
                {
                    object[] objs = new object[] { discoverPane };

                    this.Discover.SelectedPane = discoverPane;

                    this.Discover.Invalidate();

                    selectionService.SetSelectedComponents(objs, SelectionTypes.Primary);
                }
                else
                {
                    base.WndProc(ref m);
                }
            }
            else
            {
                base.WndProc(ref m);
            }
        }


        private DiscoverControl Discover
        {
            get
            {
                return (DiscoverControl)this.Control;
            }
        }
    }
}
