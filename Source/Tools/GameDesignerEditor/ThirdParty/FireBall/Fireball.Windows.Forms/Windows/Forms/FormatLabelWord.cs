#region Fireball License
//    Copyright (C) 2005  Sebastian Faltoni sebastian{at}dotnetfireball{dot}net
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

using System.Drawing;
using System;

namespace Fireball.Windows.Forms
{
	public class FormatLabelWord : IDisposable
	{
        private Image _Image = null;
        private string _Text = "";
        private int _Width = 0;
        private int _Height = 0;
        private FormatLabelElement _Element = null;
        private Rectangle _ScreenArea = new Rectangle(0, 0, 0, 0);
		//	public bool Link=false;

        public Rectangle ScreenArea
        {
            get { return _ScreenArea; }
            set { _ScreenArea = value; }
        }

        public FormatLabelElement Element
        {
            get { return _Element; }
            set { _Element = value; }
        }

        public int Height
        {
            get { return _Height; }
            set { _Height = value; }
        }

        public int Width
        {
            get { return _Width; }
            set { _Width = value; }
        }

        public Image Image
        {
            get { return _Image; }
            set { _Image = value; }
        }

        public string Text
        {
            get { return _Text; }
            set { _Text = value; }
        }

		public FormatLabelWord()
		{
            
		}

        #region IDisposable Members

        public void Dispose()
        {
            if (Image != null)
                Image.Dispose();

            if (Element != null)
                Element.Dispose();
        }

        #endregion
    }
}