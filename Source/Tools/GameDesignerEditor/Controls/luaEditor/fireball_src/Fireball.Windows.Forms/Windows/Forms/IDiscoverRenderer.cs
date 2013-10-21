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
using System.Drawing;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    public interface IDiscoverRenderer
    {
       
        void DrawBackground(Graphics graphics, Rectangle bounds, Color backColor);
        void DrawChevron(Graphics graphics, Rectangle bounds, Color color);
        void DrawDiscoverContentPane(Graphics graphics, Rectangle bounds, DrawItemState state, DiscoverPane pane, Font font, bool rtl);
        void DrawDiscoverContentPaneBackground(Graphics graphics, Rectangle bounds, DrawItemState state);
        
        void DrawSeparator(Graphics graphics, Rectangle bounds, string text, Font font, Color foreColor, bool rtl);
        void DrawDiscoverFooterPane(Graphics graphics, Rectangle bounds, DrawItemState state, DiscoverPane pane, Font font);
        void DrawDiscoverFooterPaneBackground(Graphics graphics, Rectangle bounds, DrawItemState state);
        void DrawGripper(Graphics graphics, Rectangle bounds);
        void DrawHeader(Graphics graphics, Rectangle bounds, string text, Font font, Image image, bool rtl);

        // Properties
        Color BorderColor { get; }
    }
}
