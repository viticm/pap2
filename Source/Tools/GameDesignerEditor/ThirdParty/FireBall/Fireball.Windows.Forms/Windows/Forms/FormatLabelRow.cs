//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE

using System.Collections;
using System.Collections.Generic;

namespace Fireball.Windows.Forms
{

	public class FormatLabelRow
	{
        public int Width
        {
            get { return _Width; }
            set { _Width = value; }
        }

        public int Height
        {
            get { return _Height; }
            set { _Height = value; }
        }

        public int BottomPadd
        {
            get { return _BottomPadd; }
            set { _BottomPadd = value; }
        }

        public List<FormatLabelWord> Words
        {
            get { return _Words; }
        }

        public bool RenderSeparator
        {
            get { return _RenderSeparator; }
            set { _RenderSeparator = value; }
        }

        public bool Visible
        {
            get { return _Visible; }
            set { _Visible = value; }
        }
        public int Top
        {
            get { return _Top; }
            set { _Top = value; }
        }

        private int _Width = 0;
        private int _Height = 0;
        private int _BottomPadd = 0;
        private List<FormatLabelWord> _Words = new List<FormatLabelWord>();
        private bool _RenderSeparator = false;
        private bool _Visible = false;
        private int _Top = 0;



		public FormatLabelRow()
		{
		}
	}
}