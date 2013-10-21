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
	public enum TextEffect
	{
		None = 0,
		Outline,
		ShadowRB,
		ShadowLB,
		ShadowRT,
		ShadowLT,
	}

	public class FormatLabelElement : IDisposable
	{
		protected string _Tag = "";
		protected string _TagName = "";
		public FormatLabelWord[] words = null;
		public string Text = "";
		public Font Font = null;
		public Color BackColor = Color.Black;
		public Color ForeColor = Color.Black;
		public Color EffectColor = Color.Black;
		public TextEffect Effect = 0;

		public bool NewLine = false;
		public FormatLabelElement Link = null;

		public FormatLabelElement()
		{
		}

		public string TagName
		{
			get { return _TagName; }
		}


		public string Tag
		{
			get { return _Tag; }
			set
			{
				_Tag = value.ToLower();
				_Tag = _Tag.Replace("\t", " ");
				if (_Tag.IndexOf(" ") >= 0)
				{
					_TagName = _Tag.Substring(0, _Tag.IndexOf(" "));
				}
				else
				{
					_TagName = _Tag;
				}
			}

		}

        #region IDisposable Members

        public void Dispose()
        {
            if (Font != null)
                Font.Dispose();
        }

        #endregion
    }
}